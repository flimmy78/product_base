/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* prvTgfTrunk.h
*
* DESCRIPTION:
*       trunk testing
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/
#ifndef __prvTgfTrunk
#define __prvTgfTrunk

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Typedef: enum PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT
 *
 * Description: enumerator for expected load balance modes , the mode that we expect the
 *  traffic that was sent to the trunk will egress from it's ports.
 *
 * Fields:
 *      PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E  - the traffic expected to
 *          egress only single port in the trunk (any port)
 *      PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EVEN_E  - the traffic expected to
 *          egress from all ports in trunk , with EVEN values (as much as possible)
 *          by '%' operator
 *      PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EMPTY_E - the traffic not
 *          expected to egress the trunk ports !
 *      PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E - the traffic
 *          expected to egress ONLY from specific ports in trunk , with EVEN values
 *          (as much as possible) by '%' operator.
 *          the specific members are given by prvTgfTrunkLoadBalanceSpecificMembersSet(...)
 *      PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E - the traffic
 *          expected to egress ONLY from limited number of ports in trunk , with EVEN values
 *          (as much as possible) by '%' operator.
 *          the specific number of limited members are given by prvTgfTrunkLoadBalanceLimitedNumSet(...)
 */
typedef enum
{
    PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E,
    PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EVEN_E,
    PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_EMPTY_E,
    PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E,
    PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E
} PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT;



/*******************************************************************************
* prvTgfTrunkForHashTestConfigure
*
* DESCRIPTION:
*       This function configures trunk of two ports
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkForHashTestConfigure
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTrunkForHashTestConfigurationReset
*
* DESCRIPTION:
*       This function resets configuration of trunk
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkForHashTestConfigurationReset
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTrunkHashTestTrafficMplsModeEnableSet
*
* DESCRIPTION:
*       Test for prvTgfTrunkHashMplsModeEnableSet;
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets from both trunk ports
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
GT_VOID prvTgfTrunkHashTestTrafficMplsModeEnableSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTrunkHashTestTrafficMaskSet
*
* DESCRIPTION:
*       Test for prvTgfTrunkHashMaskSet;
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets from both trunk ports
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
GT_VOID prvTgfTrunkHashTestTrafficMaskSet
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTrunkHashTestTrafficIpShiftSet
*
* DESCRIPTION:
*       Test for prvTgfTrunkHashIpShiftSet;
*       Generate traffic:
*       Send to device's port given packet:
*       Check amount of egressed packets from both trunk ports
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
GT_VOID prvTgfTrunkHashTestTrafficIpShiftSet
(
    GT_VOID
);


/*******************************************************************************
* prvTgfTrunkDesignatedPorts
*
* DESCRIPTION:
*       Test for designated member in trunk.
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
GT_VOID prvTgfTrunkDesignatedPorts
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTrunkAndVlanTestConfigure
*
* DESCRIPTION:
*       This function configures trunk and vlan of two ports
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkAndVlanTestConfigure
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTrunkToCpuTraffic
*
* DESCRIPTION:
*       Generate and test traffic
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
GT_VOID prvTgfTrunkToCpuTraffic
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTrunkAndVlanTestConfigurationReset
*
* DESCRIPTION:
*       This function resets configuration of trunk and vlan
*
* INPUTS:
*       None
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success
*       GT_FAIL            - on error
*       GT_NOT_INITIALIZED - trunk library was not initialized
*       GT_HW_ERROR        - on hardware error
*       GT_OUT_OF_RANGE    - some values are out of range
*       GT_BAD_PARAM       - on illegal parameters
*       GT_BAD_PTR         - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST   - one of the members already exists in another trunk
*
* COMMENTS:
*
*******************************************************************************/
GT_STATUS prvTgfTrunkAndVlanTestConfigurationReset
(
    GT_VOID
);

/*******************************************************************************
* prvTgfTrunkLoadBalanceCheck
*
* DESCRIPTION:
*   check that total summary of counters of ports that belong to trunk got value
*   of numPacketSent.
*
* INPUTS:
*       trunkId - trunk Id that his ports should received the traffic and on
*                 them we expect total of numPacketSent packets
*       mode    - the mode of expected load balance .
*                   all traffic expected to be received from single port or with
*                   even distribution on the trunk ports
*       numPacketSent - number of packet that we expect the trunk to receive
*       tolerance - relevant to PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_SINGLE_PORT_E
*                   state the number of packet that may get to other port in the trunk.
*                   meaning that (numPacketSent - tolerance) should get to port A
*                   and tolerance should get to port B , but other ports (in trunk)
*                   should not get it.
*       trunkMemberSenderPtr - when not NULL , indicates that the traffic to check
*                   LBH that egress the trunk was originally INGRESSED from the trunk.
*                   but since in the 'enhanced UT' the CPU send traffic to a port
*                   due to loopback it returns to it, we need to ensure that the
*                   member mentioned here should get the traffic since it is the
*                   'original sender'
* OUTPUTS:
*       stormingDetectedPtr - when not NULL , used to check if storming detected.
*                   relevant only when trunkMemberSenderPtr != NULL
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
void prvTgfTrunkLoadBalanceCheck
(
    IN GT_TRUNK_ID  trunkId,
    IN PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ENT  mode,
    IN GT_U32       numPacketSent,
    IN GT_U32       tolerance,
    IN CPSS_TRUNK_MEMBER_STC *trunkMemberSenderPtr,
    OUT GT_BOOL     *stormingDetectedPtr
);

/*******************************************************************************
* prvTgfTrunkLoadBalanceSpecificMembersSet
*
* DESCRIPTION:
*   set the specific members of a trunk that will get traffic when the LBH
*   expected mode is : PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_ONLY_SPECIFIC_MEMBERS_E.
*
* INPUTS:
*       membersIndexesBmp - bmp of indexes in the arrays of :
*       prvTgfDevsArray[] , prvTgfPortsArray[]
*           the traffic should do LBH in those ports and not in other ports of
*           the trunk.
*
*       WARNING : this is NOT a good practice to set the system to get this behavior
*
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
void prvTgfTrunkLoadBalanceSpecificMembersSet
(
    IN GT_U32   membersIndexesBmp
);

/*******************************************************************************
* prvTgfTrunkLoadBalanceLimitedNumSet
*
* DESCRIPTION:
*   set the limited number of members of a trunk that will get traffic when the LBH
*   expected mode is : PRV_TGF_TRUNK_EXPECTED_LOAD_BALANCE_MODE_LIMITED_MEMBERS_E.
*
* INPUTS:
*       numOfLimitedMemebers - limited number of members of a trunk
*
*       WARNING : this is NOT a good practice to set the system to get this behavior
*
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
void prvTgfTrunkLoadBalanceLimitedNumSet
(
    IN GT_U32   numOfLimitedMemebers
);




/*******************************************************************************
* prvTgfTrunkCrcHashTest
*
* DESCRIPTION:
*   check CRC hash feature.
*
*       applicable devices: Lion and above
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
void prvTgfTrunkCrcHashTest
(
    void
);

/*******************************************************************************
* prvTgfTrunkCrcHashTestInit
*
* DESCRIPTION:
*   init for CRC hash feature test
*
*       applicable devices: Lion and above
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
void prvTgfTrunkCrcHashTestInit
(
    void
);

/*******************************************************************************
* prvTgfTrunkCrcHashTestRestore
*
* DESCRIPTION:
*   restore for CRC hash feature test
*
*       applicable devices: Lion and above
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
void prvTgfTrunkCrcHashTestRestore
(
    void
);


/*******************************************************************************
* prvTgfTrunkCrcHashMaskPriorityTest
*
* DESCRIPTION:
*   check CRC hash mask priority:
*       default of accessing the CRC mask hash table is :
*           'Traffic type' --> index 16..27
*       higher priority for the 'per port' --> index 0..15
*       highest priority to the TTI action --> index 1..15 (without index 0)
*
*       applicable devices: Lion and above
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
void prvTgfTrunkCrcHashMaskPriorityTest
(
    void
);

/*******************************************************************************
* tgfTrunkCascadeTrunkTestInit
*
* DESCRIPTION:
*   init for cascade trunks test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkCascadeTrunkTestInit
(
    void
);


/*******************************************************************************
* tgfTrunkCascadeTrunkTestRestore
*
* DESCRIPTION:
*   restore values after cascade trunks test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkCascadeTrunkTestRestore
(
    void
);


/*******************************************************************************
* tgfTrunkCascadeTrunkTest
*
* DESCRIPTION:
*   The cascade trunks test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkCascadeTrunkTest
(
    void
);

/*******************************************************************************
* tgfTrunkDesignatedTableModesTestInit
*
* DESCRIPTION:
*   init for designated table modes test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkDesignatedTableModesTestInit
(
    void
);


/*******************************************************************************
* tgfTrunkDesignatedTableModesTestRestore
*
* DESCRIPTION:
*   restore values after designated table modes test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkDesignatedTableModesTestRestore
(
    void
);


/*******************************************************************************
* tgfTrunkDesignatedTableModesTest
*
* DESCRIPTION:
*   The designated table modes test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkDesignatedTableModesTest
(
    void
);


/*******************************************************************************
* tgfTrunkMcLocalSwitchingTestInit
*
* DESCRIPTION:
*   init for trunk multi-destination local switching test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkMcLocalSwitchingTestInit
(
    void
);


/*******************************************************************************
* tgfTrunkMcLocalSwitchingTestRestore
*
* DESCRIPTION:
*   restore values after trunk multi-destination local switching test
*
*       applicable devices: ALL DxCh devices
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
void tgfTrunkMcLocalSwitchingTestRestore
(
    void
);


/*******************************************************************************
* tgfTrunkMcLocalSwitchingTest
*
* DESCRIPTION:
*   The trunk multi-destination local switching test
*
*       applicable devices: ALL DxCh devices
*
*       The test:
*    a. set trunk with 2 ports
*    b. send flooding from it   --> check no flood back
*    c. 'disable filter' on the trunk
*    d. send flooding from it   --> check flood back to only one of the 2 ports of the trunk
*    e. add port to the trunk
*    f. send flooding from it   --> check flood back to only one of the 3 ports of the trunk
*    g. 'enable filter' on the trunk
*    h. send flooding from it   --> check no flood back
*    i. 'disable filter' on the trunk
*    j. send flooding from it   --> check flood back to only one of the 3 ports of the trunk
*    k. remove port from the trunk
*    l. send flooding from it   --> check flood back to only one of the 2 ports of the trunk, check that the removed port get flood.
*    m. 'enable filter' on the trunk
*
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
void tgfTrunkMcLocalSwitchingTest
(
    void
);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __prvTgfTrunk */

