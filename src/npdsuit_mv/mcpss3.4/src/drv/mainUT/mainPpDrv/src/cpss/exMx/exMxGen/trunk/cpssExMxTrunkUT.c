/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* cpssExMxTrunkUT.c
*
* DESCRIPTION:
*       Unit Tests for Trunk module.
*
*       Trunk module has two types of APIs: High-Level and Low-Level.
*       It's not recommended to mix both APIs so this test plan describes
*       two test suits. First suit is cpssExMxTrunkHighLevel which includes
*       all functions for High-Level mode and second suit is
*       cpssExMxTrunkLowLevel which includes all functions for Low-Level mode
*       and functions compliance with both modes.
*
*       For the cpssExMxTrunk HighLevel and LowLevel suits
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

#include <cpss/exMx/exMxGen/config/private/prvCpssExMxInfo.h>
#include <cpss/exMx/exMxGen/trunk/cpssExMxTrunk.h>

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
#define TRUNK_TESTED_VIRT_PORT_ID_CNS   0


/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssExMxTrunkMembersSet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN GT_U32                   numOfEnabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    enabledMembersArray[],
    IN GT_U32                   numOfDisabledMembers,
    IN CPSS_TRUNK_MEMBER_STC    disabledMembersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkMembersSet)
{
/*
ITERATE_DEVICES
1.1. Call with trunkId [2], numOfEnabledMembers [5], enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4], numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 5, 6, 7]. Expected: GT_OK.
1.2. Try to set already written trunk members to another trunk id. Call with trunk id [3], numOfEnabledMembers [2], enabledMembersArray [tested dev id, virtual port 7, 4], numOfDisabledMembers [2], disabledMembersArray [tested dev id, virtual port id 6, 2]. Expected: NON GT_OK.
1.3. Try to set invalid device id and legal port id as trunk entry member. Call with trunk id [2], numOfEnabledMembers [1], enabledMembersArray [out of range device id, virtual port 0], numOfDisabledMembers [0]. Expected: NON GT_OK.
1.4. Try to set legal device id and invalid port id as trunk entry member. Call with trunk id [2], numOfEnabledMembers [1], enabledMembersArray [tested device id, out of range virtual port], numOfDisabledMembers [0]. Expected: NON GT_OK.
1.5. Call cpssExMxTrunkDbEnabledMembersGet with trunkId [2], non-NULL numOfEnabledMembersPtr, and allocated enabledMembersArray for 8 members. Expected: GT_OK, numOfEnabledMembersPtr [5], and the same enabledMembersArray as written.
1.6. Call cpssExMxTrunkDbDisabledMembersGet with trunkId [2], non-NULL numOfDisabledMembersPtr, and allocated disabledMembersArray for 8 members. Expected: GT_OK, numOfDisabledMembersPtr [3], and the same disabledMembersArray as written.
1.7. Check out of range for sum [>CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS = 8] of enabled and disabled members. Call with trunkId [2], numOfEnabledMembers [6], enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4, 5], numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 6, 7, 8]. Expected: NON GT_OK.
1.8. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], numOfEnabledMembers [5], enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4], numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 5, 6, 7]. Expected: NON GT_OK.
1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], numOfEnabledMembers [5], enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4], numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 5, 6, 7]. Expected: NON GT_OK.
1.10. Call cpssExMxTrunkMembersSet with trunkId [2], numOfEnabledMembers [0], enabledMembersArray [NULL], numOfDisabledMembers [0], disabledMembersArray [NULL] to remove all members from the trunk. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_U8                   numOfVirtPorts = 0;
    GT_TRUNK_ID             trunkId;
    GT_U32                  numOfEnabledMembers;
    CPSS_TRUNK_MEMBER_STC   enabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32                  numOfDisabledMembers;
    CPSS_TRUNK_MEMBER_STC   disabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_32                   res;
        GT_U32                  numOfMembersWritten;
        CPSS_TRUNK_MEMBER_STC   membersArrayWritten[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];

        st = prvUtfNumVirtualPortsGet(dev, &numOfVirtPorts);
        if (8 > numOfVirtPorts)
        {
            continue;
        }

        /* 1.1. Call with trunkId [2], numOfEnabledMembers [5],                                         */
        /* enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4],                          */
        /* numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 5, 6, 7].     */
        /* Expected: GT_OK.                                                                             */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        numOfEnabledMembers = 5;
        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port = 0;
        enabledMembersArray[1].device = dev;
        enabledMembersArray[1].port = 1;
        enabledMembersArray[2].device = dev;
        enabledMembersArray[2].port = 2;
        enabledMembersArray[3].device = dev;
        enabledMembersArray[3].port = 3;
        enabledMembersArray[4].device = dev;
        enabledMembersArray[4].port = 4;

        numOfDisabledMembers = 3;
        disabledMembersArray[0].device = dev;
        disabledMembersArray[0].port = 5;
        disabledMembersArray[1].device = dev;
        disabledMembersArray[1].port = 6;
        disabledMembersArray[2].device = dev;
        disabledMembersArray[2].port = 7;

        st = cpssExMxTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.2. Try to set already written trunk members to another trunk id.                           */
        /* Call with trunk id [3], numOfEnabledMembers [2],                                             */
        /* enabledMembersArray [tested dev id, virtual port 7, 4],                                      */
        /* numOfDisabledMembers [2], disabledMembersArray [tested dev id, virtual port id 6, 2].        */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS + 1;

        numOfEnabledMembers = 2;
        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port = 7;
        enabledMembersArray[1].device = dev;
        enabledMembersArray[1].port = 4;

        numOfDisabledMembers = 2;
        disabledMembersArray[0].device = dev;
        disabledMembersArray[0].port = 6;
        disabledMembersArray[1].device = dev;
        disabledMembersArray[1].port = 2;

        st = cpssExMxTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.3. Try to set invalid device id and legal port id as trunk entry member.                   */
        /* Call with trunk id [2], numOfEnabledMembers [1],                                             */
        /* enabledMembersArray [out of range device id, virtual port 8], numOfDisabledMembers [0].      */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        numOfEnabledMembers = 1;
        enabledMembersArray[0].device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        enabledMembersArray[0].port = 0;

        numOfDisabledMembers = 0;

        st = cpssExMxTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.4. Try to set legal device id and invalid port id as trunk entry member.                   */
        /* Call with trunk id [2], numOfEnabledMembers [1],                                             */
        /* enabledMembersArray [tested device id, out of range virtual port],                           */
        /* numOfDisabledMembers [0].                                                                    */
        /* Expected: NON GT_OK.                                                                         */
        numOfEnabledMembers = 1;
        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        numOfDisabledMembers = 0;

        st = cpssExMxTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.5. Call cpssExMxTrunkDbEnabledMembersGet with trunkId [2],                                 */
        /* non-NULL numOfEnabledMembersPtr, and allocated enabledMembersArray for 8 members.            */
        /* Expected: GT_OK, numOfEnabledMembersPtr [5], and the same enabledMembersArray as written.    */
        numOfEnabledMembers = 5;
        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port = 0;
        enabledMembersArray[1].device = dev;
        enabledMembersArray[1].port = 1;
        enabledMembersArray[2].device = dev;
        enabledMembersArray[2].port = 2;
        enabledMembersArray[3].device = dev;
        enabledMembersArray[3].port = 3;
        enabledMembersArray[4].device = dev;
        enabledMembersArray[4].port = 4;

        st = cpssExMxTrunkDbEnabledMembersGet(dev, trunkId, &numOfMembersWritten, membersArrayWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTrunkDbEnabledMembersGet: %d, %d", dev, trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(numOfEnabledMembers, numOfMembersWritten, dev, trunkId);

        res = cpssOsMemCmp((const GT_VOID*)enabledMembersArray, (const GT_VOID*)membersArrayWritten,
                            numOfEnabledMembers * sizeof(enabledMembersArray[0]));
        UTF_VERIFY_EQUAL2_PARAM_MAC(0, res, dev, trunkId);

        /* 1.6. Call cpssExMxTrunkDbDisabledMembersGet with trunkId [2],                                */
        /* non-NULL numOfDisabledMembersPtr, and allocated disabledMembersArray for 8 members.          */
        /* Expected: GT_OK, numOfDisabledMembersPtr [3], and the same disabledMembersArray as written.  */
        numOfDisabledMembers = 3;
        disabledMembersArray[0].device = dev;
        disabledMembersArray[0].port = 5;
        disabledMembersArray[1].device = dev;
        disabledMembersArray[1].port = 6;
        disabledMembersArray[2].device = dev;
        disabledMembersArray[2].port = 7;

        st = cpssExMxTrunkDbDisabledMembersGet(dev, trunkId, &numOfMembersWritten, membersArrayWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTrunkDbDisabledMembersGet: %d, %d", dev, trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(numOfDisabledMembers, numOfMembersWritten, dev, trunkId);

        res = cpssOsMemCmp((const GT_VOID*)disabledMembersArray, (const GT_VOID*)membersArrayWritten,
                            numOfDisabledMembers * sizeof(disabledMembersArray[0]));
        UTF_VERIFY_EQUAL2_PARAM_MAC(0, res, dev, trunkId);

        /* 1.7. Check out of range for sum [>CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS = 8]                     */
        /* of enabled and disabled members. Call with trunkId [2], numOfEnabledMembers [6],             */
        /* enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4, 5],                       */
        /*  numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 6, 7, 8].    */
        /* Expected: NON GT_OK.                                                                         */
        numOfEnabledMembers = 6;
        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port = 0;
        enabledMembersArray[1].device = dev;
        enabledMembersArray[1].port = 1;
        enabledMembersArray[2].device = dev;
        enabledMembersArray[2].port = 2;
        enabledMembersArray[3].device = dev;
        enabledMembersArray[3].port = 3;
        enabledMembersArray[4].device = dev;
        enabledMembersArray[4].port = 4;
        enabledMembersArray[5].device = dev;
        enabledMembersArray[5].port = 5;

        numOfDisabledMembers = 3;
        disabledMembersArray[0].device = dev;
        disabledMembersArray[0].port = 6;
        disabledMembersArray[1].device = dev;
        disabledMembersArray[1].port = 7;
        disabledMembersArray[2].device = dev;
        disabledMembersArray[2].port = 8;

        st = cpssExMxTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.8. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], numOfEnabledMembers [5],               */
        /* enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4],                          */
        /* numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 5, 6, 7].     */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        numOfEnabledMembers = 5;
        enabledMembersArray[0].device = dev;
        enabledMembersArray[0].port = 0;
        enabledMembersArray[1].device = dev;
        enabledMembersArray[1].port = 1;
        enabledMembersArray[2].device = dev;
        enabledMembersArray[2].port = 2;
        enabledMembersArray[3].device = dev;
        enabledMembersArray[3].port = 3;
        enabledMembersArray[4].device = dev;
        enabledMembersArray[4].port = 4;

        numOfDisabledMembers = 2;
        disabledMembersArray[0].device = dev;
        disabledMembersArray[0].port = 5;
        disabledMembersArray[1].device = dev;
        disabledMembersArray[1].port = 6;
        disabledMembersArray[2].device = dev;
        disabledMembersArray[2].port = 7;

        st = cpssExMxTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.9. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], numOfEnabledMembers [5],        */
        /* enabledMembersArray [tested dev id, virtual port id 0, 1, 2, 3, 4],                          */
        /* numOfDisabledMembers [3], disabledMembersArray [tested dev id, virtual port id 5, 6, 7].     */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.10. Call cpssExMxTrunkMembersSet with trunkId [2], numOfEnabledMembers [0],                */
        /* enabledMembersArray [NULL], numOfDisabledMembers [0], disabledMembersArray [NULL]            */
        /* to remove all members from the trunk.                                                        */
        /* Expected: GT_OK.                                                                             */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        numOfEnabledMembers = 0;
        numOfDisabledMembers = 0;

        st = cpssExMxTrunkMembersSet(dev, trunkId, numOfEnabledMembers, NULL,
                                     numOfDisabledMembers, NULL);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

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

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                     numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkMembersSet(dev, trunkId, numOfEnabledMembers, enabledMembersArray,
                                 numOfDisabledMembers, disabledMembersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssExMxTrunkMemberAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkMemberAdd)
{
/*
ITERATE_DEVICES
1.1. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.2. Try to add the same trunk member again, accordingly to header the result must be OK. Call with trunk id [2], non-NULL memberPtr [tested dev, virtual port 8]. Expected: GT_OK.
1.3. Try to add already added trunk members to another trunk id. Call with trunk id [3], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: NON GT_OK.
1.4. Try to add invalid device id and legal port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8]. Expected: NON GT_OK.
1.5. Try to add legal device id and invalid port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port]. Expected: NON GT_OK.
1.6. Check for NULL handling. Call with trunk id [2], memberPtr [NULL]. Expected: GT_BAD_PTR.
1.7. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
1.8. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
1.9. Call cpssExMxTrunkMemberRemove with trunkId [2], memberPtr [tested dev id, virtual port 8] to remove added member from the trunk. Expected: GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC   member;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].             */
        /* Expected: GT_OK.                                                                             */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.2. Try to add the same trunk member again, accordingly to header the result must be OK.    */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev, virtual port 8].                     */
        /* Expected: GT_OK.                                                                             */
        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.3. Try to add already added trunk members to another trunk id.                             */
        /* Call with trunk id [3], non-NULL memberPtr [tested dev id, virtual port 8].                  */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS + 1;

        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.4. Try to add invalid device id and legal port id as trunk entry member.                   */
        /* Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8].                 */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.5. Try to add legal device id and invalid port id as trunk entry member.                   */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port].                    */
        /* Expected: NON GT_OK.                                                                         */
        member.device = dev;
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.6. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].                      */
        /* Expected: GT_BAD_PTR.                                                                        */
        st = cpssExMxTrunkMemberAdd(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trunkId);

        /* 1.7. Call with null trunk id [NULL_TRUNK_ID_CNS = 0],                                        */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                             */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.8. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                                 */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                             */
        /* Expected: NON GT_OK.                                                                         */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.9. Call cpssExMxTrunkMemberRemove with trunkId [2],                                        */
        /* memberPtr [tested dev id, virtual port 8] to remove added member from the trunk.             */
        /* Expected: GT_OK                                                                              */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssExMxTrunkMemberRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkMemberRemove)
{
/*
ITERATE_DEVICES
1.1. Add member to tested trunk id before call remove function. Call cpssExMxTrunkMemberAdd with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.2. Try to remove invalid device id and legal port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8]. Expected: NON GT_OK.
1.3. Try to remove legal device id and invalid port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port]. Expected: NON GT_OK.
1.4. Try to remove added member. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.5. Try to remove already removed trunk member again, accordingly to header the result must be OK. Call with trunk id [2], non-NULL memberPtr [tested dev, virtual port 8]. Expected: GT_OK.
1.6. Check for NULL handling. Call with trunk id [2], memberPtr [NULL]. Expected: GT_BAD_PTR.
1.7. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
1.8. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC   member;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Add member to tested trunk id before call remove function.                                      */
        /* Call cpssExMxTrunkMemberAdd with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].   */
        /* Expected: GT_OK.                                                                                     */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.2. Try to remove invalid device id and legal port id as trunk entry member.                        */
        /* Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8]                          */
        /* Expected: GT_OK.                                                                                     */
        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.3. Try to remove legal device id and invalid port id as trunk entry member.                        */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port].                            */
        /* Expected: NON GT_OK.                                                                                 */
        member.device = dev;
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.4. Try to remove added member.                                                                     */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                          */
        /* Expected: GT_OK.                                                                                     */
        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.5. Try to remove already removed trunk member again, accordingly to header the result must be OK.  */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev, virtual port 8].                             */
        /* Expected: GT_OK.                                                                                     */
        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.6. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].                              */
        /* Expected: GT_BAD_PTR.                                                                                */
        st = cpssExMxTrunkMemberRemove(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trunkId);

        /* 1.7. Call with null trunk id [NULL_TRUNK_ID_CNS = 0],                                                */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                                     */
        /* Expected: NON GT_OK.                                                                                 */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.8. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                                         */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                                     */
        /* Expected: NON GT_OK.                                                                                 */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssExMxTrunkMemberDisable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkMemberDisable)
{
/*
ITERATE_DEVICES
1.1. Add member to tested trunk before disable it. Call cpssExMxTrunkMemberAdd with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.2. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.3. Try to disable already disabled trunk member, accordingly to header the result must be OK. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.4. Try to disable invalid device id and legal port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8] Expected: NON GT_OK.
1.5. Try to disable legal device id and invalid port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port]. Expected: NON GT_OK.
1.6. Remove member from trunk and try to disable it then. Call cpssExMxTrunkMemberRemove with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.7. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: NON GT_OK.
1.8. Check for NULL handling. Call with trunk id [2], memberPtr [NULL]. Expected: GT_BAD_PTR.
1.9. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
1.10. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC   member;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Add member to tested trunk before disable it.                                                       */
        /* Call cpssExMxTrunkMemberAdd with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].       */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.2. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                         */
        /* Expected: GT_OK.                                                                                         */
        st = cpssExMxTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.3. Try to disable already disabled trunk member, accordingly to header the result must be OK.          */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                              */
        /* Expected: GT_OK.                                                                                         */
        st = cpssExMxTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.4. Try to disable invalid device id and legal port id as trunk entry member.                           */
        /* Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8]                              */
        /* Expected: NON GT_OK.                                                                                     */
        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.5. Try to disable legal device id and invalid port id as trunk entry member.                           */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port].                                */
        /* Expected: NON GT_OK.                                                                                     */
        member.device = dev;
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.6. Remove member from trunk and try to disable it then.                                                */
        /* Call cpssExMxTrunkMemberRemove with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].    */
        /* Expected: GT_OK.                                                                                         */
        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.7. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                         */
        /* Expected: NON GT_OK.                                                                                     */
        st = cpssExMxTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.8. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].                                  */
        /* Expected: GT_BAD_PTR.                                                                                    */
        st = cpssExMxTrunkMemberDisable(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trunkId);

        /* 1.9. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8].   */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.10. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                                            */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                                         */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkMemberDisable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkMemberDisable(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssExMxTrunkMemberEnable
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_TRUNK_MEMBER_STC    *memberPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkMemberEnable)
{
/*
ITERATE_DEVICES
1.1. Add member to tested trunk before enable it. Call cpssExMxTrunkMemberAdd with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.2. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.3. Try to enable already enabled trunk member, accordingly to header the result must be OK. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.4. Try to enable invalid device id and legal port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8] Expected: NON GT_OK.
1.5. Try to enable legal device id and invalid port id as trunk entry member. Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port]. Expected: NON GT_OK.
1.6. Remove member from trunk and try to enable it then. Call cpssExMxTrunkMemberRemove with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.7. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: NON GT_OK.
1.8. Check for NULL handling. Call with trunk id [2], memberPtr [NULL]. Expected: GT_BAD_PTR.
1.9. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
1.10. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL memberPtr [tested dev, virtual port 8]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC   member;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Add member to tested trunk before enable it.                                                        */
        /* Call cpssExMxTrunkMemberAdd with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].       */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.2. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                         */
        /* Expected: GT_OK.                                                                                         */
        st = cpssExMxTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.3. Try to enable already enabled trunk member, accordingly to header the result must be OK.            */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                              */
        /* Expected: GT_OK.                                                                                         */
        st = cpssExMxTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.4. Try to enable invalid device id and legal port id as trunk entry member.                            */
        /* Call with trunk id [2], non-NULL memberPtr [invalid dev id, virtual port 8]                              */
        /* Expected: NON GT_OK.                                                                                     */
        member.device = PRV_CPSS_MAX_PP_DEVICES_CNS;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.5. Try to enable legal device id and invalid port id as trunk entry member.                            */
        /* Call with trunk id [2], non-NULL memberPtr [tested dev id, invalid port].                                */
        /* Expected: NON GT_OK.                                                                                     */
        member.device = dev;
        member.port = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.6. Remove member from trunk and try to enable it then.                                                 */
        /* Call cpssExMxTrunkMemberRemove with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].    */
        /* Expected: GT_OK.                                                                                         */
        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.7. Call with trunk id [2], non-NULL memberPtr [tested dev id, virtual port 8].                         */
        /* Expected: NON GT_OK.                                                                                     */
        st = cpssExMxTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.8. Check for NULL handling. Call with trunk id [2], memberPtr [NULL].                                  */
        /* Expected: GT_BAD_PTR.                                                                                    */
        st = cpssExMxTrunkMemberEnable(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trunkId);

        /* 1.9. Call with null trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL memberPtr [tested dev, virtual port 8].   */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);

        /* 1.10. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                                            */
        /* non-NULL memberPtr [tested dev, virtual port 8].                                                         */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, member.device, member.port);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkMemberEnable(dev, trunkId, &member);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkMemberEnable(dev, trunkId, &member);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssExMxTrunkNonTrunkPortsAdd
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkNonTrunkPortsAdd)
{
/*
ITERATE_DEVICES
1.1. Call with trunk id [2], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00] to add first eight virtual ports to the trunk's non-trunk entry. Expected: GT_OK.
1.2. Check for NULL handling. Call with trunk id [2], nonTrunkPortsBmpPtr [NULL]. Expected: GT_BAD_PARAM.
1.3. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00]. Expected: GT_OK.
1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00]. Expected: NON GT_OK.
1.5. Call cpssExMxTrunkNonTrunkPortsRemove with trunkId [2], nonTrunkPortsBmp [0xFF, 0x00] to remove all non trunk members from the trunk  Expected: GT_OK.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPortsBmp;
    GT_U8               port;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

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

        st = cpssExMxTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);

        /* 1.2. Check for NULL handling. Call with trunk id [2], nonTrunkPortsBmpPtr [NULL].                        */
        /* Expected: GT_BAD_PARAM.                                                                                  */
        st = cpssExMxTrunkNonTrunkPortsAdd(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trunkId);

        /* 1.3. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0],                      */
        /* non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00].                                                               */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);

        /* 1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00].  */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);

        /* 1.5. Call cpssExMxTrunkNonTrunkPortsRemove with trunkId [2], nonTrunkPortsBmp [0xFF, 0x00]               */
        /* to remove all non trunk members from the trunk                                                           */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxTrunkNonTrunkPortsRemove: %d, %d, %d, %d",
                                     dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssExMxTrunkNonTrunkPortsRemove
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    IN CPSS_PORTS_BMP_STC       *nonTrunkPortsBmpPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkNonTrunkPortsRemove)
{
/*
ITERATE_DEVICES
1.1. Add first eight virtual ports to the trunk's non-trunk entry. Call cpssExMxTrunkNonTrunkPortsAdd with trunkId [2], nonTrunkPortsBmpPtr [0xFF, 0x00]. Expected: GT_OK.
1.2. Call with trunk id [2], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00] to remove first eight virtual ports from the trunk's non-trunk entry. Expected: GT_OK.
1.3. Check for NULL handling. Call with trunk id [2], nonTrunkPortsBmpPtr [NULL]. Expected: GT_BAD_PARAM.
1.4. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00]. Expected: GT_OK.
1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00]. Expected: NON GT_OK.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPortsBmp;
    GT_U8               port;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    cpssOsBzero((GT_VOID*)&nonTrunkPortsBmp, sizeof(nonTrunkPortsBmp));
    for (port = 0; port < CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS; port++)
    {
        CPSS_PORTS_BMP_PORT_SET_MAC(&nonTrunkPortsBmp, port);
    }

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Add first eight virtual ports to the trunk's non-trunk entry.                                       */
        /* Call cpssExMxTrunkNonTrunkPortsAdd with trunkId [2], nonTrunkPortsBmpPtr [0xFF, 0x00].                   */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsAdd(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxTrunkNonTrunkPortsAdd: %d, %d, %d, %d",
                                     dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);

        /* 1.2. Call with trunk id [2], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00] to remove first eight             */
        /* virtual ports to the trunk's non-trunk entry.                                                            */
        /* Expected: GT_OK.                                                                                         */
        st = cpssExMxTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);

        /* 1.3. Check for NULL handling. Call with trunk id [2], nonTrunkPortsBmpPtr [NULL].                        */
        /* Expected: GT_BAD_PARAM.                                                                                  */
        st = cpssExMxTrunkNonTrunkPortsRemove(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trunkId);

        /* 1.4. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0],                      */
        /* non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00].                                                               */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);

        /* 1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsBmpPtr [0xFF, 0x00].  */
        /* Expected: NON GT_OK.                                                                                     */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPortsBmp.ports[0], nonTrunkPortsBmp.ports[1]);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkNonTrunkPortsRemove(dev, trunkId, &nonTrunkPortsBmp);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low-Level Mode

GT_STATUS cpssExMxTrunkPortTrunkIdSet
(
    IN GT_U8            devNum,
    IN GT_U8            portId,
    IN GT_BOOL          memberOfTrunk,
    IN GT_TRUNK_ID      trunkId
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkPortTrunkIdSet)
{
/*
ITERATE_DEVICES
1.1. Call with portId [8], memberOfTrunk [GT_TRUE], trunkId [2]. Expected: GT_OK.
1.2. Call cpssExMxTrunkPortTrunkIdGet with portId [8], non-NULL memberOfTrunkPtr, and non-NULL trunkIdPtr. Expected: GT_OK, memberOfTrunkPtr [GT_TRUE] and trunkIdPtr [2].
1.3. Call with portId [8], memberOfTrunk [GT_FALSE], trunkid [2]. Expected: GT_OK.
1.4. Call cpssExMxTrunkPortTrunkIdGet with portId [8], non-NULL memberOfTrunkPtr, and non-NULL trunkIdPtr. Expected: GT_OK, memberOfTrunkPtr [GT_FALSE] and trunkIdPtr [2].
1.5. Call with out of range virtual portId. Expected: GT_BAD_PARAM.
1.6. Call with null trunk id [NULL_TRUNK_ID_CNS = 0]. Expected: NON GT_OK.
1.7. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128]. Expected: NON GT_OK.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           portId;
    GT_BOOL         memberOfTrunk;
    GT_TRUNK_ID     trunkId;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_BOOL         memberOfTrunkWritten;
        GT_TRUNK_ID     trunkIdWritten;

        /* 1.1. Call with portId [8], memberOfTrunk [GT_TRUE], trunkId [2].                                             */
        /* Expected: GT_OK.                                                                                             */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        memberOfTrunk = GT_TRUE;
        portId = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkPortTrunkIdSet(dev, portId, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, portId, memberOfTrunk, trunkId);

        /* 1.2. Call cpssExMxTrunkPortTrunkIdGet with portId [8], non-NULL memberOfTrunkPtr, and non-NULL trunkIdPtr.   */
        /* Expected: GT_OK, memberOfTrunkPtr [GT_TRUE] and trunkIdPtr [2].                                              */
        st = cpssExMxTrunkPortTrunkIdGet(dev, portId, &memberOfTrunkWritten, &trunkIdWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTrunkPortTrunkIdGet: %d, %d", dev, portId);
        UTF_VERIFY_EQUAL4_PARAM_MAC(memberOfTrunk, memberOfTrunkWritten, dev, portId, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL4_PARAM_MAC(trunkId, trunkIdWritten, dev, portId, memberOfTrunk, trunkId);

        /* 1.3. Call with portId [8], memberOfTrunk [GT_FALSE], trunkid [2].  trunkid will be set to 0 .                */
        /* Expected: GT_OK.                                                                                             */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        memberOfTrunk = GT_FALSE;
        portId = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkPortTrunkIdSet(dev, portId, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, portId, memberOfTrunk, trunkId);

        trunkId = 0;
        /* 1.4. Call cpssExMxTrunkPortTrunkIdGet with portId [8], non-NULL memberOfTrunkPtr, and non-NULL trunkIdPtr.   */
        /* Expected: GT_OK, memberOfTrunkPtr [GT_FALSE] and trunkIdPtr [0].                                             */
        st = cpssExMxTrunkPortTrunkIdGet(dev, portId, &memberOfTrunkWritten, &trunkIdWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTrunkPortTrunkIdGet: %d, %d", dev, portId);
        UTF_VERIFY_EQUAL4_PARAM_MAC(memberOfTrunk, memberOfTrunkWritten, dev, portId, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL4_PARAM_MAC(trunkId, trunkIdWritten, dev, portId, memberOfTrunk, trunkId);

        /* 1.5. Call with out of range virtual portId.                                                                  */
        /* Expected: GT_BAD_PARAM.                                                                                      */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        memberOfTrunk = GT_TRUE;
        portId = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxTrunkPortTrunkIdSet(dev, portId, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_BAD_PARAM, st, dev, portId, memberOfTrunk, trunkId);

        /* 1.6. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].                                                        */
        /* Expected: NON GT_OK.                                                                                         */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        memberOfTrunk = GT_TRUE;
        portId = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkPortTrunkIdSet(dev, portId, memberOfTrunk, trunkId);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, portId, memberOfTrunk, trunkId);

        /* 1.7. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].                                                 */
        /* Expected: NON GT_OK.                                                                                         */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkPortTrunkIdSet(dev, portId, memberOfTrunk, trunkId);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, portId, memberOfTrunk, trunkId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    memberOfTrunk = GT_TRUE;
    portId = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkPortTrunkIdSet(dev, portId, memberOfTrunk, trunkId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkPortTrunkIdSet(dev, portId, memberOfTrunk, trunkId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssExMxTrunkPortTrunkIdGet
(
    IN GT_U8            devNum,
    IN GT_U8            portId,
    OUT GT_BOOL         *memberOfTrunkPtr,
    OUT GT_TRUNK_ID     *trunkIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkPortTrunkIdGet)
{
/*
ITERATE_DEVICES
1.1. Call with portId [8], non-NULL memberOfTrunkPtr, and non-NULL trunkIdPtr. Expected: GT_OK.
1.2. Call with portId [8], memberOfTrunkPtr [NULL], and non-NULL trunkIdPtr. Expected: GT_BAD_PTR.
1.3. Call with portId [8], non-NULL memberOfTrunkPtr, and trunkIdPtr [NULL]. Expected: GT_BAD_PTR.
1.4. Call with out of range virtual portid. Expected: GT_BAD_PARAM.
*/
    GT_STATUS       st = GT_OK;
    GT_U8           dev;
    GT_U8           portId;
    GT_BOOL         memberOfTrunk;
    GT_TRUNK_ID     trunkId;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with portId [8], non-NULL memberOfTrunkPtr, and non-NULL trunkIdPtr.   */
        /* Expected: GT_OK.                                                                 */
        portId = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkPortTrunkIdGet(dev, portId, &memberOfTrunk, &trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, portId);

        /* 1.2. Call with portId [8], memberOfTrunkPtr [NULL], and non-NULL trunkIdPtr.     */
        /* Expected: GT_BAD_PTR.                                                            */
        st = cpssExMxTrunkPortTrunkIdGet(dev, portId, NULL, &trunkId);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, portId);

        /* 1.3. Call with portId [8], non-NULL memberOfTrunkPtr, and trunkIdPtr [NULL].     */
        /* Expected: GT_BAD_PTR.                                                            */
        st = cpssExMxTrunkPortTrunkIdGet(dev, portId, &memberOfTrunk, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, portId);

        /* 1.4. Call with out of range virtual portid.                                      */
        /* Expected: GT_BAD_PARAM.                                                          */
        portId = PRV_CPSS_MAX_PP_PORTS_NUM_CNS;

        st = cpssExMxTrunkPortTrunkIdGet(dev, portId, &memberOfTrunk, &trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_BAD_PARAM, st, dev, portId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    portId = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkPortTrunkIdGet(dev, portId, &memberOfTrunk, &trunkId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkPortTrunkIdGet(dev, portId, &memberOfTrunk, &trunkId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low-Level Mode

GT_STATUS cpssExMxTrunkTableEntrySet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    IN  GT_U32                  numMembers,
    IN  CPSS_TRUNK_MEMBER_STC   membersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkTableEntrySet)
{
/*
ITERATE_DEVICES
1.1. Call with trunkId [2], numMembers [3], membersArray [tested dev id, virtual port 0, 1, 2]. Expected: GT_OK.
1.2. Call with cpssExMxTrunkTableEntryGet with trunkId [2], non-NULL numMembersPtr, membersArray. Expected: GT_OK, numMembersPtr [3], and the same membersArray as was set.
1.3. Call with out of range numMembers [9]. Expected: NON GT_OK.
1.4. Call with null trunk id [NULL_TRUNK_ID_CNS = 0]. Expected: NON GT_OK.
1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128]. Expected: NON GT_OK.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    GT_TRUNK_ID             trunkId;
    GT_U32                  numMembers;
    CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_32                   res;
        GT_U32                  numMembersWritten;
        CPSS_TRUNK_MEMBER_STC   membersArrayWritten[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];

        /* 1.1. Call with trunkId [2], numMembers [3], membersArray [tested dev id, virtual port 0, 1, 2].      */
        /* Expected: GT_OK.                                                                                     */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        numMembers = 3;
        membersArray[0].device = dev;
        membersArray[0].port = 0;
        membersArray[1].device = dev;
        membersArray[1].port = 1;
        membersArray[2].device = dev;
        membersArray[2].port = 2;

        st = cpssExMxTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, trunkId, numMembers);

        /* 1.2. Call with cpssExMxTrunkTableEntryGet with trunkId [2], non-NULL numMembersPtr, membersArray.    */
        /* Expected: GT_OK, numMembersPtr [3], and the same membersArray as was set.                            */
        st = cpssExMxTrunkTableEntryGet(dev, trunkId, &numMembersWritten, membersArrayWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTrunkTableEntryGet: %d, %d", dev, trunkId);
        UTF_VERIFY_EQUAL3_PARAM_MAC(numMembers, numMembersWritten, dev, trunkId, numMembers);

        res = cpssOsMemCmp((const GT_VOID*)membersArray, (const GT_VOID*)membersArrayWritten,
                            numMembers*sizeof(membersArray[0]));
        UTF_VERIFY_EQUAL3_PARAM_MAC(0, res, dev, trunkId, numMembers);

        /* 1.3. Call with out of range numMembers [9].                                                          */
        /* Expected: NON GT_OK.                                                                                 */
        numMembers = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS + 1;

        st = cpssExMxTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, trunkId, numMembers);

        /* 1.4. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].                                                */
        /* Expected: NON GT_OK.                                                                                 */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;
        numMembers = 3;

        st = cpssExMxTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, trunkId, numMembers);

        /* 1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].                                         */
        /* Expected: NON GT_OK.                                                                                 */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;
        numMembers = 3;

        st = cpssExMxTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL3_PARAM_MAC(GT_OK, st, dev, trunkId, numMembers);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    numMembers = 3;
    membersArray[0].device = dev;
    membersArray[0].port = 0;
    membersArray[1].device = dev;
    membersArray[1].port = 1;
    membersArray[2].device = dev;
    membersArray[2].port = 2;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkTableEntrySet(dev, trunkId, numMembers, membersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssExMxTrunkTableEntryGet
(
    IN  GT_U8                   devNum,
    IN  GT_TRUNK_ID             trunkId,
    OUT GT_U32                  *numMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS]
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkTableEntryGet)
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


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunkId [2], not-NULL numMembers, and allocated membersArray.         */
        /* Expected: GT_OK.                                                                     */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        st = cpssExMxTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.2. Call with numMembersPtr [NULL].                                                 */
        /* Expected: GT_BAD_PTR.                                                                */
        st = cpssExMxTrunkTableEntryGet(dev, trunkId, NULL, membersArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trunkId);

        /* 1.3. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].                                */
        /* Expected: NON GT_OK.                                                                 */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].                         */
        /* Expected: NON GT_OK.                                                                 */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkTableEntryGet(dev, trunkId, &numMembers, membersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low-Level Mode

GT_STATUS cpssExMxTrunkNonTrunkPortsEntrySet
(
    IN  GT_U8               devNum,
    IN  GT_TRUNK_ID         trunkId,
    IN  CPSS_PORTS_BMP_STC  *nonTrunkPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkNonTrunkPortsEntrySet)
{
/*
ITERATE_DEVICES
1.1. Call with trunkId [2], non-NULL nonTrunkPortsPtr [0xFF, 0x00]. Expected: GT_OK.
1.2. Call cpssExMxTrunkNonTrunkPortsEntryGet with trunkId [2], non-NULL nonTrunkPortsPtr. Expected: GT_OK and the same port bitmap as was written.
1.3. Check for NULL handling. Call with trunkId [2], nonTrunkPortsPtr [NULL]. Expected: GT_BAD_PTR.
1.4. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0], non-NULL nonTrunkPortsPtr [0xFF, 0x00]. Expected: GT_OK.
1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128], non-NULL nonTrunkPortsPtr [0xFF, 0x00]. Expected: NON GT_OK.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;
    GT_U8               port;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

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

        st = cpssExMxTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPorts.ports[0], nonTrunkPorts.ports[1]);

        /* 1.2. Call cpssExMxTrunkNonTrunkPortsEntryGet with trunkId [2], non-NULL nonTrunkPortsPtr.        */
        /* Expected: GT_OK and the same port bitmap as was written.                                         */
        st = cpssExMxTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPortsWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTrunkNonTrunkPortsEntryGet: %d, %d", dev, trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(nonTrunkPorts.ports[0], nonTrunkPortsWritten.ports[0], dev, trunkId);
        UTF_VERIFY_EQUAL2_PARAM_MAC(nonTrunkPorts.ports[1], nonTrunkPortsWritten.ports[1], dev, trunkId);

        /* 1.3. Check for NULL handling. Call with trunkId [2], nonTrunkPortsPtr [NULL].                    */
        /* Expected: GT_BAD_PTR.                                                                            */
        st = cpssExMxTrunkNonTrunkPortsEntrySet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trunkId);

        /* 1.4. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0],              */
        /* non-NULL nonTrunkPortsPtr [0xFF, 0x00].                                                          */
        /* Expected: GT_OK.                                                                                 */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPorts.ports[0], nonTrunkPorts.ports[1]);

        /* 1.5. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                                     */
        /* non-NULL nonTrunkPortsPtr [0xFF, 0x00].                                                          */
        /* Expected: NON GT_OK.                                                                             */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, trunkId, nonTrunkPorts.ports[0], nonTrunkPorts.ports[1]);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkNonTrunkPortsEntrySet(dev, trunkId, &nonTrunkPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssExMxTrunkNonTrunkPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_TRUNK_ID         trunkId,
    OUT  CPSS_PORTS_BMP_STC  *nonTrunkPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkNonTrunkPortsEntryGet)
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


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunkId [2], non-NULL nonTrunkPortsPtr.                               */
        /* Expected: GT_OK.                                                                     */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.2. Check for NULL handling. Call with trunk id [2], nonTrunkPortsPtr [NULL].       */
        /* Expected: GT_BAD_PTR.                                                                */
        st = cpssExMxTrunkNonTrunkPortsEntryGet(dev, trunkId, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", st, dev);

        /* 1.3. This API can accept trunk id zero. Call with trunk id [NULL_TRUNK_ID_CNS = 0],  */
        /* non-NULL nonTrunkPortsPtr [0xFF, 0x00].                                              */
        /* Expected: GT_OK.                                                                     */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128],                         */
        /* non-NULL nonTrunkPortsPtr [0xFF, 0x00].                                              */
        /* Expected: NON GT_OK.                                                                 */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkNonTrunkPortsEntryGet(dev, trunkId, &nonTrunkPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
Low-Level Mode

GT_STATUS cpssExMxTrunkDesignatedPortsEntrySet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    IN  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkDesignatedPortsEntrySet)
{
/*
ITERATE_DEVICES
1.1. Call with entryIndex [3], non-NULL designatedPortsPtr [0xFF, 0x00]. Expected: GT_OK.
1.2. Call cpssExMxTrunkDesignatedPortsEntryGet with entryIndex [3], non-NULL designatedPortsPtr. Expected: GT_OK and the same port bitmap as was written.
1.3. Call with out of range entryIndex [16], non-NULL designatedPortsPtr [0xFF, 0x00]. Expected: NON GT_OK.
1.4. Check for NULL handling. Call with entryIndex [3], designatedPortsPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_U32              entryIndex;
    CPSS_PORTS_BMP_STC  designatedPorts;
    GT_U8               port;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    cpssOsBzero((GT_VOID*)&designatedPorts, sizeof(designatedPorts));
    for (port = 0; port < CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS; port++)
    {
        CPSS_PORTS_BMP_PORT_SET_MAC(&designatedPorts, port);
    }

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        CPSS_PORTS_BMP_STC  designatedPortsWritten;

        /* 1.1. Call with entryIndex [3], non-NULL designatedPortsPtr [0xFF, 0x00].                             */
        /* Expected: GT_OK.                                                                                     */
        entryIndex = 3;

        st = cpssExMxTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryIndex, designatedPorts.ports[0], designatedPorts.ports[1]);

        /* 1.2. Call cpssExMxTrunkDesignatedPortsEntryGet with entryIndex [3], non-NULL designatedPortsPtr.     */
        /* Expected: GT_OK and the same port bitmap as was written.                                             */
        st = cpssExMxTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPortsWritten);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_OK, st, "cpssExMxTrunkDesignatedPortsEntryGet: %d, %d", dev, entryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(designatedPorts.ports[0], designatedPortsWritten.ports[0], dev, entryIndex);
        UTF_VERIFY_EQUAL2_PARAM_MAC(designatedPorts.ports[1], designatedPortsWritten.ports[1], dev, entryIndex);

        /* 1.3. Call with out of range entryIndex [16], non-NULL designatedPortsPtr [0xFF, 0x00].               */
        /* Expected: NON GT_OK.                                                                                 */
        entryIndex = 16;

        st = cpssExMxTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_NOT_EQUAL4_PARAM_MAC(GT_OK, st, dev, entryIndex, designatedPorts.ports[0], designatedPorts.ports[1]);

        /* 1.4. Check for NULL handling. Call with entryIndex [3], designatedPortsPtr [NULL].                   */
        /* Expected: GT_BAD_PTR.                                                                                */
        st = cpssExMxTrunkDesignatedPortsEntrySet(dev, entryIndex, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, entryIndex);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    entryIndex = 3;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkDesignatedPortsEntrySet(dev, entryIndex, &designatedPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssExMxTrunkDesignatedPortsEntryGet
(
    IN  GT_U8               devNum,
    IN  GT_U32              entryIndex,
    OUT  CPSS_PORTS_BMP_STC  *designatedPortsPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkDesignatedPortsEntryGet)
{
/*
ITERATE_DEVICES
1.1. Call with entryIndex [3], non-NULL designatedPortsPtr. Expected: GT_OK.
1.2. Call with out of range entryIndex [16], non-NULL designatedPortsPtr. Expected: NON GT_OK.
1.3. Check for NULL handling. Call with entryIndex [3], designatedPortsPtr [NULL]. Expected: GT_BAD_PTR.
*/
    GT_STATUS           st = GT_OK;
    GT_U8               dev;
    GT_U32              entryIndex;
    CPSS_PORTS_BMP_STC  designatedPorts;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with entryIndex [3], non-NULL designatedPortsPtr.                          */
        /* Expected: GT_OK.                                                                     */
        entryIndex = 3;

        st = cpssExMxTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* 1.2. Call with out of range entryIndex [16], non-NULL designatedPortsPtr.            */
        /* Expected: NON GT_OK.                                                                 */
        entryIndex = 16;

        st = cpssExMxTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, entryIndex);

        /* 1.3. Check for NULL handling. Call with entryIndex [3], designatedPortsPtr [NULL].   */
        /* Expected: GT_BAD_PTR.                                                                */
        st = cpssExMxTrunkDesignatedPortsEntryGet(dev, entryIndex, NULL);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, entryIndex);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    entryIndex = 3;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkDesignatedPortsEntryGet(dev, entryIndex, &designatedPorts);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssExMxTrunkDbEnabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfEnabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   enabledMembersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkDbEnabledMembersGet)
{
/*
ITERATE_DEVICES
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


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunkId [2], non-NULL numOfEnabledMembers,                            */
        /* and allocated enabledMembersArray for 8 members.                                     */
        /* Expected: GT_OK.                                                                     */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        st = cpssExMxTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnabledMembers, enabledMembersArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.2. Check for NULL handling. Call with trunkId [2], numOfEnabledMembers [NULL],     */
        /* and allocated enabledMembersArray.                                                   */
        /* Expected: GT_BAD_PTR.                                                                */
        st = cpssExMxTrunkDbEnabledMembersGet(dev, trunkId, NULL, enabledMembersArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trunkId);

        /* 1.3. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].                                */
        /* Expected: NON GT_OK.                                                                 */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnabledMembers, enabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].                         */
        /* Expected: NON GT_OK.                                                                 */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnabledMembers, enabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnabledMembers, enabledMembersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkDbEnabledMembersGet(dev, trunkId, &numOfEnabledMembers, enabledMembersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssExMxTrunkDbDisabledMembersGet
(
    IN GT_U8                    devNum,
    IN GT_TRUNK_ID              trunkId,
    INOUT GT_U32                *numOfDisabledMembersPtr,
    OUT CPSS_TRUNK_MEMBER_STC   disabledMembersArray[]
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkDbDisabledMembersGet)
{
/*
ITERATE_DEVICES
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


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with trunkId [2], non-NULL numOfDisabledMembersPtr,                            */
        /* and allocated disabledMembersArray for 8 members.                                        */
        /* Expected: GT_OK.                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        st = cpssExMxTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.2. Check for NULL handling. Call with trunkId [2], numOfDisabledMembersPtr [NULL],     */
        /* and allocated disabledMembersArray.                                                      */
        /* Expected: GT_BAD_PTR.                                                                    */
        st = cpssExMxTrunkDbDisabledMembersGet(dev, trunkId, NULL, disabledMembersArray);
        UTF_VERIFY_EQUAL2_STRING_MAC(GT_BAD_PTR, st, "%d, %d, NULL", dev, trunkId);

        /* 1.3. Call with null trunk id [NULL_TRUNK_ID_CNS = 0].                                    */
        /* Expected: NON GT_OK.                                                                     */
        trunkId = TRUNK_NULL_TRUNK_ID_CNS;

        st = cpssExMxTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);

        /* 1.4. Call with out of range trunkId [GT_TRUNK_MAX_ID = 128].                             */
        /* Expected: NON GT_OK.                                                                     */
        trunkId = TRUNK_MAX_TRUNK_ID_CNS;

        st = cpssExMxTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_NOT_EQUAL2_PARAM_MAC(GT_OK, st, dev, trunkId);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisabledMembers, disabledMembersArray);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkDbDisabledMembersGet(dev, trunkId, &numOfDisabledMembers, disabledMembersArray);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
High-Level Mode

GT_STATUS cpssExMxTrunkDbIsMemberOfTrunk
(
    IN  GT_U8                   devNum,
    IN  CPSS_TRUNK_MEMBER_STC   *memberPtr,
    OUT GT_TRUNK_ID             *trunkIdPtr
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkDbIsMemberOfTrunk)
{
/*
ITERATE_DEVICES
1.1. At first add some member to trunk. Call cpssExMxTrunkMemberAdd with trunkId [2], and non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.2. Call with non-NULL memberPtr [tested dev id, virtual port 8], non-NULL trunkIdPtr. Expected: GT_OK and trunkIdPtr [2].
1.3. Accordingly to header trunkIdPtr may be NULL. Call with non-NULL memberPtr [tested dev id, virtual port 8], trunkIdPtr [NULL]. Expected: GT_OK.
1.4. Remove member from trunk. Call cpssExMxTrunkMemberRemove with trunkId [2], non-NULL memberPtr [tested dev id, virtual port 8]. Expected: GT_OK.
1.5. Try to find already removed member. Call with non-NULL memberPtr [tested dev id, virtual port 8], non-NULL trunkIdPtr. Expected: GT_NOT_FOUND.
1.6. Check for NULL handling. Call with memberPtr [NULL], non-NULL trunkIdPtr. Expected: GT_BAD_PTR.
*/
    GT_STATUS               st = GT_OK;
    GT_U8                   dev;
    CPSS_TRUNK_MEMBER_STC   member;
    GT_TRUNK_ID             trunkId;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        GT_TRUNK_ID     trunkIdWritten;

        /* 1.1. At first add some member to trunk.                                                                  */
        /* Call cpssExMxTrunkMemberAdd with trunkId [2], and non-NULL memberPtr [tested dev id, virtual port 8].    */
        /* Expected: GT_OK.                                                                                         */
        trunkId = TRUNK_TESTED_TRUNK_ID_CNS;

        member.device = dev;
        member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

        st = cpssExMxTrunkMemberAdd(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxTrunkMemberAdd: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.2. Call with non-NULL memberPtr [tested dev id, virtual port 8], non-NULL trunkIdPtr.                  */
        /* Expected: GT_OK and trunkIdPtr [2].                                                                      */
        trunkIdWritten = 0;

        st = cpssExMxTrunkDbIsMemberOfTrunk(dev, &member, &trunkIdWritten);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, member.device, member.port);
        UTF_VERIFY_EQUAL3_PARAM_MAC(trunkId, trunkIdWritten, dev, member.device, member.port);

        /* 1.3. Accordingly to header trunkIdPtr may be NULL.                                                       */
        /* Call with non-NULL memberPtr [tested dev id, virtual port 8], trunkIdPtr [NULL].                         */
        /* Expected: GT_OK.                                                                                         */
        st = cpssExMxTrunkDbIsMemberOfTrunk(dev, &member, NULL);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_OK, st, dev, member.device, member.port);

        /* 1.4. Remove member from trunk.                                                                           */
        /* Call cpssExMxTrunkMemberRemove with trunkId [2], non-NULL memberPtr [tested dev id, virtual port 8].     */
        /* Expected: GT_OK.                                                                                         */
        st = cpssExMxTrunkMemberRemove(dev, trunkId, &member);
        UTF_VERIFY_EQUAL4_STRING_MAC(GT_OK, st, "cpssExMxTrunkMemberRemove: %d, %d, %d, %d",
                                     dev, trunkId, member.device, member.port);

        /* 1.5. Try to find already removed member.                                                                 */
        /* Call with non-NULL memberPtr [tested dev id, virtual port 8], non-NULL trunkIdPtr.                       */
        /* Expected: GT_NOT_FOUND.                                                                                  */
        trunkIdWritten = 0;

        st = cpssExMxTrunkDbIsMemberOfTrunk(dev, &member, &trunkIdWritten);
        UTF_VERIFY_EQUAL3_PARAM_MAC(GT_NOT_FOUND, st, dev, member.device, member.port);

        /* 1.6. Check for NULL handling. Call with memberPtr [NULL], non-NULL trunkIdPtr.                           */
        /* Expected: GT_BAD_PTR.                                                                                    */
        st = cpssExMxTrunkDbIsMemberOfTrunk(dev, NULL, &trunkIdWritten);
        UTF_VERIFY_EQUAL1_STRING_MAC(GT_BAD_PTR, st, "%d, NULL", dev);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    member.device = 0;
    member.port = TRUNK_TESTED_VIRT_PORT_ID_CNS;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkDbIsMemberOfTrunk(dev, &member, &trunkId);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkDbIsMemberOfTrunk(dev, &member, &trunkId);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssExMxTrunkHashIpModeSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enableIpHash
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkHashIpModeSet)
{
/*
ITERATE_DEVICES
1.1. Call with enableIpHash [GT_TRUE]. Expected: GT_OK.
1.2. Call with enableIpHash [GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enableIpHash;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with enableIpHash [GT_TRUE].   */
        /* Expected: GT_OK.                         */
        enableIpHash = GT_TRUE;

        st = cpssExMxTrunkHashIpModeSet(dev, enableIpHash);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enableIpHash);

        /* 1.2. Call with enableIpHash [GT_FALSE].  */
        /* Expected: GT_OK.                         */
        enableIpHash = GT_FALSE;

        st = cpssExMxTrunkHashIpModeSet(dev, enableIpHash);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enableIpHash);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enableIpHash = GT_TRUE;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkHashIpModeSet(dev, enableIpHash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkHashIpModeSet(dev, enableIpHash);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssExMxTrunkHashL4ModeSet
(
    IN GT_U8   devNum,
    IN GT_BOOL enableL4Hash
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkHashL4ModeSet)
{
/*
ITERATE_DEVICES
1.1. Call with enableL4Hash [GT_TRUE]. Expected: GT_OK.
1.2. Call with enableL4Hash [GT_FALSE]. Expected: GT_OK.
*/
    GT_STATUS   st = GT_OK;
    GT_U8       dev;
    GT_BOOL     enableL4Hash;


    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    /* 1. Go over all active devices. */
    while (GT_OK == prvUtfNextDeviceGet(&dev, GT_TRUE))
    {
        /* 1.1. Call with enableL4Hash [GT_TRUE].   */
        /* Expected: GT_OK.                         */
        enableL4Hash = GT_TRUE;

        st = cpssExMxTrunkHashL4ModeSet(dev, enableL4Hash);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enableL4Hash);

        /* 1.2. Call with enableL4Hash [GT_FALSE].  */
        /* Expected: GT_OK.                         */
        enableL4Hash = GT_FALSE;

        st = cpssExMxTrunkHashL4ModeSet(dev, enableL4Hash);
        UTF_VERIFY_EQUAL2_PARAM_MAC(GT_OK, st, dev, enableL4Hash);
    }

    st = prvUtfNextDeviceReset(&dev, UTF_EXMX_FAMILY_SET_CNS);
    UTF_VERIFY_EQUAL0_PARAM_MAC(GT_OK, st);

    enableL4Hash = GT_TRUE;

    /*2. Go over all non active devices. */
    while(GT_OK == prvUtfNextDeviceGet(&dev, GT_FALSE))
    {
        /* 2.1. <Call function for non active device and valid parameters>. */
        /* Expected: GT_BAD_PARAM.                                          */
        st = cpssExMxTrunkHashL4ModeSet(dev, enableL4Hash);
        UTF_VERIFY_EQUAL1_PARAM_MAC(GT_NOT_APPLICABLE_DEVICE, st, dev);
    }

    /* 3. Call function with out of range device id.    */
    /* Expected: GT_BAD_PARAM.                          */
    dev = PRV_CPSS_MAX_PP_DEVICES_CNS;

    st = cpssExMxTrunkHashL4ModeSet(dev, enableL4Hash);
    UTF_VERIFY_EQUAL1_PARAM_MAC(GT_BAD_PARAM, st, dev);
}

/*----------------------------------------------------------------------------*/
/*
All Modes

GT_STATUS cpssExMxTrunkDesignatedTableHashModeSet
(
    IN GT_U8   devNum,
    IN GT_BOOL useEntireTable
)
*/
UTF_TEST_CASE_MAC(cpssExMxTrunkDesignatedTableHashModeSet)
{
/*
ITERATE_DEVICES
1.1. Call with useEntireTable [GT_TRUE]. Expected: GT_OK.
1.2. Call with useEntireTable [GT_FALSE]. Expected: GT_OK.
*/
/* TODO: not implemented yet !!! */
}


/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Configuration of cpssExMxTrunk high-level suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxTrunkHighLevel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkMembersSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkMemberAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkMemberRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkMemberDisable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkMemberEnable)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkDbEnabledMembersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkDbDisabledMembersGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkDbIsMemberOfTrunk)
UTF_SUIT_END_TESTS_MAC(cpssExMxTrunkHighLevel)

/*
 * Configuration of cpssExMxTrunk low-level suit
 */
UTF_SUIT_BEGIN_TESTS_MAC(cpssExMxTrunkLowLevel)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkNonTrunkPortsAdd)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkNonTrunkPortsRemove)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkPortTrunkIdSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkPortTrunkIdGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkTableEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkTableEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkNonTrunkPortsEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkNonTrunkPortsEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkDesignatedPortsEntrySet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkDesignatedPortsEntryGet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkHashIpModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkHashL4ModeSet)
    UTF_SUIT_DECLARE_TEST_MAC(cpssExMxTrunkDesignatedTableHashModeSet)
UTF_SUIT_END_TESTS_MAC(cpssExMxTrunkLowLevel)
