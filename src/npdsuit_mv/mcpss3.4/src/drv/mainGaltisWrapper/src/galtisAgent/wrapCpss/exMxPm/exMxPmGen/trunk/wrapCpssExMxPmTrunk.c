/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmTrunk.c
*
* DESCRIPTION:
*       Wrapper functions for 802.3ad Link Aggregation (Trunk) facility
*       CPSS - ExMxPm
*
*       States and Modes :
*       A. Application can work in one of 2 modes:
*           1. "high level" trunk manipulations set of APIs - Mode "HL"
*           2. "low level" trunk HW tables/registers set of APIs - MODE "LL"
*
*       B. Using "high level" trunk manipulations set of APIs - "High level"
*           Mode.
*           In this mode the CPSS synchronizes all the relevant trunk
*           tables/registers, implements some workarounds for trunks Errata.
*           1. Create trunk
*           2. Destroy trunk
*           3. Add/remove member to/from trunk
*           4. Enable/disable in trunk
*           5. Add/Remove port to/from "non-trunk" entry
*        C. Using "low level" trunk HW tables/registers set of APIs- "Low level"
*           Mode.
*           In this mode the CPSS allows direct access to trunk tables/registers.
*           In this mode the Application required to implement the
*           synchronization between the trunk tables/registers and to implement
*           the workarounds to some of the trunk Errata.
*
*           1. Set per port the trunkId
*           2. Set per trunk the trunk members , number of members
*           3. Set per trunk the Non-trunk local ports bitmap
*           4. Set per Index the designated local ports bitmap
*
*         D. Application responsibility is not to use a mix of APIs from
*            those 2 modes
*            The only time that the Application can shift between the 2 modes,
*            is only when there are no trunks in the device.
*
********************************************************************************
*       The ExMxPm devices supports :
*       A. 255 trunks with up to 12 members (in each trunk).
*       B. 16 entries in designated trunk table.
*       C. 256 entries in the non-trunk table.
********************************************************************************
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMxPm/exMxPmGen/trunk/cpssExMxPmTrunk.h>

#define  WR_PRV_TRUNK_MAX_NUM 256
/*******************************************************************************
* cpssExMxPmTrunkInit
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS ExMxPm Trunk initialization of PP Tables/registers and
*       SW shadow data structures, all ports are set as non-trunk ports.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum             - device number
*       maxNumberOfTrunks  - maximum number of trunk groups.(0..255)
*                            when this number is 0 , there will be no shadow used
*                            Note:
*                            that means that API's that used shadow will FAIL.
*
* OUTPUTS:
*           none.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*       GT_BAD_PARAM     - wrong devNum
*       GT_OUT_OF_RANGE  - the numberOfTrunks > 255
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 maxNumberOfTrunks;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    maxNumberOfTrunks = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkInit(devNum, maxNumberOfTrunks);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/**************Table: cpssExMxPmTrunkMembersSet*****************/



/*******************************************************************************
* cpssExMxPmTrunkMembersSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function sets the trunk with the specified enabled and disabled
*       members.
*       This setting overrides the previous setting of the trunk members.
*
*       The user can "invalidate/unset" the trunk entry by setting :
*           numOfEnabledMembers = 0 and  numOfDisabledMembers = 0
*
*       This function supports next "set entry" options :
*       1. "Reset" the entry
*          The function will remove the previous settings
*       2. Set entry after the entry was empty
*          The function will set new settings
*       3. Set entry with the same members that it is already holding
*          The function will rewrite the HW entries as they were.
*       4. Set entry with different settings than the previous settings
*          a. function will remove the previous settings
*          b. function will set new settings
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id (1..255)
*       enabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfEnabledMembers = 0)
*       numOfEnabledMembers - number of enabled members in the array.
*       disabledMembersArray - (array of) members to set in this trunk as enabled
*                     members .
*                    (this parameter ignored if numOfDisabledMembers = 0)
*       numOfDisabledMembers - number of enabled members in the array.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on error.
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_OUT_OF_RANGE - when the number of enabled members + number of
*                         disabled members exceeds the maximum number
*                         of members in trunk (total of 0 - 12 members allowed)
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since the HW uses
*                                                                7 bits for the device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since the HW uses
*                                              6 bits for the port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST - one of the members already exists in another trunk
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkMembersSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_TRUNK_ID trunkId;
    GT_U32 numOfEnabledMembers;
    CPSS_TRUNK_MEMBER_STC membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32 numOfDisabledMembers;
    GT_U8                  i, j;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    trunkId = (GT_TRUNK_ID)inFields[0];
    numOfEnabledMembers = (GT_U32)inFields[1];
    numOfDisabledMembers = (GT_U32)inFields[2];

    if(numOfEnabledMembers + numOfDisabledMembers > CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
    {
        galtisOutput(outArgs, GT_BAD_VALUE, "");
        return CMD_OK;
    }

    j = 3;

    for(i=0; i < numOfEnabledMembers; i++)
    {
         membersArray[i].port = (GT_U8)inFields[j];
         membersArray[i].device = (GT_U8)inFields[j+1];
         CONVERT_DEV_PORT_DATA_MAC(membersArray[i].device, membersArray[i].port);

         j = j+2;
    }

    for(/* continue i , j*/ ; i < (numOfEnabledMembers + numOfDisabledMembers); i++)
    {
        membersArray[i].port = (GT_U8)inFields[j];
        membersArray[i].device = (GT_U8)inFields[j+1];
        CONVERT_DEV_PORT_DATA_MAC(membersArray[i].device, membersArray[i].port);

        j = j+2;
    }
    /* call cpss api function */
    result = cpssExMxPmTrunkMembersSet(devNum, trunkId, numOfEnabledMembers,
                                     &membersArray[0], numOfDisabledMembers,
                                     &membersArray[numOfEnabledMembers]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}





/*******************************************************************************
* cpssExMxPmTrunkPortTrunkIdSet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number
*       portNum  - the port number.
*       memberOfTrunk - is the port associated with the trunk
*                 GT_FALSE - the port is set to "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*
*       trunkId - the trunk to which the port associated with
*                 this field indicates the trunk group number (ID) to which the
*                 port is a member.
*                 1 through 255 = The port is a member of the trunk
*                 this value relevant only when memberOfTrunk = GT_TRUE
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad port number , or
*                      bad trunkId number
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkPortTrunkIdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL memberOfTrunk;
    GT_TRUNK_ID trunkId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    memberOfTrunk = (GT_BOOL)inArgs[2];
    trunkId = (GT_TRUNK_ID)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmTrunkPortTrunkIdSet(devNum, portNum, memberOfTrunk, trunkId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkPortTrunkIdGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number
*       portNum  - the port number.
*
* OUTPUTS:
*       memberOfTrunkPtr - (pointer to) the port associated with the trunk
*                 GT_FALSE - the port is set as a "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*       trunkIdPtr - (pointer to)the trunk to which the associated port with
*                 this field indicates the trunk group number (ID) to which the
*                 port is a member.
*                 1 through 255 = The port is a member of the trunk
*                 this value relevant only when (*memberOfTrunkPtr) = GT_TRUE
*
* RETURNS:
*       GT_OK   - successful completion
*       GT_FAIL - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad port number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_BAD_STATE - the trunkId value is not synchronized in the 2 registers
*                      that should hold the same value
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkPortTrunkIdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL memberOfTrunk;
    GT_TRUNK_ID trunkId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmTrunkPortTrunkIdGet(devNum, portNum, &memberOfTrunk, &trunkId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", memberOfTrunk, trunkId);

    return CMD_OK;
}
 /***************************Table: cpssExMxPmTrunk********************/
static GT_U32                gIndGet;

/*******************************************************************************
* cpssExMxPmTrunkTableEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk table entry , and set the number of members in it.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId     - trunk id (1..255)
*       numMembers - num of enabled members in the trunk
*                    Note : value 0 is not recommended.
*       membersArray - array of enabled members of the trunk
*
* OUTPUTS:
*       none.
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_OUT_OF_RANGE - numMembers exceed the number of maximum number
*                         of members in trunk (total of 0 - 12 members allowed)
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since the HW uses 7 bits
*                                              for the device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since the HW uses 6 bits
*                                              for the port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkTableEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_TRUNK_MEMBER_STC member;
    GT_U8 devNum;
    GT_BOOL enable;
    GT_TRUNK_ID trunkId,tempTrunkId;




    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];

    member.port = (GT_U8)inFields[0];
    member.device = (GT_U8)inFields[1];
    CONVERT_DEV_PORT_DATA_MAC(member.device, member.port);
    enable = (GT_BOOL)inFields[2];

    if(enable == GT_TRUE)
    {
        result = cpssExMxPmTrunkDbIsMemberOfTrunk(devNum, &member, &tempTrunkId);

        if(result == GT_NOT_FOUND)/*It is not a member --> so add it */
        {
           result = cpssExMxPmTrunkMemberAdd(devNum, trunkId, &member);
        }
        else if(result == GT_OK)
        {
            result = cpssExMxPmTrunkMemberEnable(devNum, trunkId, &member);
        }
    }
    else /*Disable*/
    {
        result = cpssExMxPmTrunkMemberDisable(devNum, trunkId, &member);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkTableEntryGetEntry
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk table entry , and get the number of members in it.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId     - trunk id (1..255)
*
* OUTPUTS:
*       numMembersPtr - (pointer to)num of members in the trunk
*       membersArray - array of enabled members of the trunk
*                      array is allocated by the caller , it is assumed that
*                      the array can hold CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS
*                      members
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkTableEntryGetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_TRUNK_ID trunkId;
    GT_U32                numEnabledMembers;
    GT_U32                numDisabledMembers;
    CPSS_TRUNK_MEMBER_STC membersArray[CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];

    numEnabledMembers = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;


    /* call cpss api function */
    result = cpssExMxPmTrunkDbEnabledMembersGet(devNum, trunkId, &numEnabledMembers, membersArray);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

     if(gIndGet < numEnabledMembers)
    {
         CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[gIndGet].device,
                              membersArray[gIndGet].port);
         inFields[0] = membersArray[gIndGet].port;
         inFields[1] = membersArray[gIndGet].device;
         inFields[2] = GT_TRUE;
         /* we need to retrieve another enabled trunk member */
         /* pack and output table fields */
         fieldOutput("%d%d%d", inFields[0],  inFields[1],  inFields[2]);

         /* pack output arguments to galtis string */
         galtisOutput(outArgs, GT_OK, "%f");
         return CMD_OK;
    }

    numDisabledMembers = CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS;
    result = cpssExMxPmTrunkDbDisabledMembersGet(devNum, trunkId, &numDisabledMembers,
                                                             membersArray);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    if(gIndGet < (numEnabledMembers+numDisabledMembers))
    {
        inFields[0] = membersArray[gIndGet-numEnabledMembers].port;
        inFields[1] = membersArray[gIndGet-numEnabledMembers].device;
        inFields[2] = GT_FALSE;
        /* we need to retrieve another disabled trunk member */
        /* pack and output table fields */
        fieldOutput("%d%d%d", inFields[0],  inFields[1],  inFields[2]);

        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%f");
        return CMD_OK;
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
}

/*******************************************************************************
* cpssExMxPmTrunkTableEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk table entry , and get the number of members in it.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId     - trunk id (1..255)
*
* OUTPUTS:
*       numMembersPtr - (pointer to)num of members in the trunk
*       membersArray - array of enabled members of the trunk
*                      array is allocated by the caller , it is assumed that
*                      the array can hold CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS
*                      members
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkTableEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     gIndGet = 0; /*reset on first*/
     return wrCpssExMxPmTrunkTableEntryGetEntry(inArgs,inFields,numFields,outArgs);
}
/*******************************************************************************
* cpssExMxPmTrunkTableEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk table entry , and get the number of members in it.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId     - trunk id (1..255)
*
* OUTPUTS:
*       numMembersPtr - (pointer to)num of members in the trunk
*       membersArray - array of enabled members of the trunk
*                      array is allocated by the caller , it is assumed that
*                      the array can hold CPSS_EXMXPM_TRUNK_MAX_NUM_OF_MEMBERS_CNS
*                      members
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkTableEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gIndGet++;/* go to next index */
     return wrCpssExMxPmTrunkTableEntryGetEntry(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssExMxPmTrunkMemberRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function removes members from a trunk in the device.
*       If a member does not exist in this trunk, the function does nothing and
*       returns GT_OK.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum      - the device number on which to remove a member from the trunk
*       trunkId     - trunk id (1..255)
*       memberPtr   - (pointer to)the member to remove from the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since the HW uses 7 bits
*                                              for the device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since the HW uses 6 bits
*                                              for the port number
*       GT_BAD_PTR               - one of the parameters in the NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkTableEntryDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    GT_TRUNK_ID             trunkId;
    CPSS_TRUNK_MEMBER_STC   member;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];

    member.port = (GT_U8)inFields[0];
    member.device = (GT_U8)inFields[1];
    CONVERT_DEV_PORT_DATA_MAC(member.device, member.port);

    /* call cpss api function */
    result = cpssExMxPmTrunkMemberRemove(devNum, trunkId, &member);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*********************Table cpssExMxPmTrunkNonTrunkPorts***************/
static GT_TRUNK_ID  gTrunkId;


/*******************************************************************************
* cpssExMxPmTrunkNonTrunkPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk's non-trunk ports specific bitmap entry.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id(0..255) - in this API trunkId can be ZERO !
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkNonTrunkPortsEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_TRUNK_ID trunkId;
    CPSS_PORTS_BMP_STC nonTrunkPorts;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    trunkId = (GT_TRUNK_ID)inFields[0];
    nonTrunkPorts.ports[0] = (GT_U32)inFields[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkNonTrunkPortsEntrySet(devNum, trunkId, &nonTrunkPorts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkNonTrunkPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk's non-trunk ports bitmap specific entry.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id(0..255) - in this API trunkId can be ZERO !
*
* OUTPUTS:
*       nonTrunkPortsPtr - (pointer to) non trunk port bitmap of the trunk.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkNonTrunkPortsEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;

    GT_U32                  numOfEnabledMembers;
    CPSS_TRUNK_MEMBER_STC   enabledMembers[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gTrunkId = 0; /*Reset on first*/

    result = cpssExMxPmTrunkDbEnabledMembersGet(devNum, gTrunkId,
                                                      &numOfEnabledMembers,
                                                      enabledMembers);

    while(result != GT_OK && gTrunkId < WR_PRV_TRUNK_MAX_NUM)
    {
        gTrunkId++;
        result = cpssExMxPmTrunkDbEnabledMembersGet(devNum, gTrunkId,
                                                      &numOfEnabledMembers,
                                                      enabledMembers);
    }

    if(gTrunkId >= WR_PRV_TRUNK_MAX_NUM)/*No valid trunk*/
    {
        galtisOutput(outArgs, GT_EMPTY, "");
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPmTrunkNonTrunkPortsEntryGet(devNum, gTrunkId,
                                                &nonTrunkPorts);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = gTrunkId;
    inFields[1] = nonTrunkPorts.ports[0];

    gTrunkId++;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkNonTrunkPortsEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PORTS_BMP_STC  nonTrunkPorts;

    GT_U32                  numOfEnabledMembers;
    CPSS_TRUNK_MEMBER_STC   enabledMembers[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    if (gTrunkId >= WR_PRV_TRUNK_MAX_NUM)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    result = cpssExMxPmTrunkDbEnabledMembersGet(devNum, gTrunkId,
                                                      &numOfEnabledMembers,
                                                      enabledMembers);

    while(result != GT_OK && gTrunkId < WR_PRV_TRUNK_MAX_NUM)
    {
        gTrunkId++;
        result = cpssExMxPmTrunkDbEnabledMembersGet(devNum, gTrunkId,
                                                      &numOfEnabledMembers,
                                                      enabledMembers);
    }

    if(gTrunkId >= WR_PRV_TRUNK_MAX_NUM)/*No valid trunk*/
    {
        galtisOutput(outArgs, GT_EMPTY, "");
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPmTrunkNonTrunkPortsEntryGet(devNum, gTrunkId,
                                                &nonTrunkPorts);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = gTrunkId;
    inFields[1] = nonTrunkPorts.ports[0];

    gTrunkId++;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*************Table: cpssExMxPmTrunkDesignatedPorts**************/
static GT_U32  gEntryIndex;
/*******************************************************************************
* cpssExMxPmTrunkDesignatedPortsEntrySet [gt]
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table(0..15)
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 15)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkDesignatedPortsEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_U32              entryIndex;
    CPSS_PORTS_BMP_STC  designatedPorts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    entryIndex = (GT_U32)inFields[0];
    designatedPorts.ports[0] = (GT_U32)inFields[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkDesignatedPortsEntrySet(devNum, entryIndex,
                                                  &designatedPorts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}/*******************************************************************************
* cpssExMxPmTrunkDesignatedPortsEntryGet [gt]
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table(0..15)
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 15)
*
* COMMENTS:
*
*******************************************************************************/
 static CMD_STATUS wrCpssExMxPmTrunkDesignatedPortsEntryGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PORTS_BMP_STC  designatedPorts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gEntryIndex = 0;  /*reset on first*/

    /* call cpss api function */
    result = cpssExMxPmTrunkDesignatedPortsEntryGet(devNum, gEntryIndex,
                                                  &designatedPorts);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = gEntryIndex;
    inFields[1] = designatedPorts.ports[0];

    gEntryIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkDesignatedPortsEntryGet [gt]
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All DxCh devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
*       designatedPortsPtr - (pointer to) designated ports bitmap
* OUTPUTS:
*       designatedPortsPtr - (pointer to) designated ports bitmap
*
* RETURNS:
*       GT_OK       - successful completion
*       GT_FAIL     - an error occurred.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_OUT_OF_RANGE - entryIndex exceed the number of HW table.
*                         the index must be in range (0 - 7)
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkDesignatedPortsEntryGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    CPSS_PORTS_BMP_STC  designatedPorts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    if(gEntryIndex > 7)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPmTrunkDesignatedPortsEntryGet(devNum, gEntryIndex,
                                                  &designatedPorts);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = gEntryIndex;
    inFields[1] = designatedPorts.ports[0];

    gEntryIndex++;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}





/*******************************************************************************
* cpssExMxPmTrunkDbIsMemberOfTrunk
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Checks if a member {device,port} is a trunk member.
*       if it is a trunk member, the function retrieves the trunkId of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number.
*       memberPtr - (pointer to) the member to check if is trunk member
*
* OUTPUTS:
*       trunkIdPtr - (pointer to) trunk id of the port .
*                    this pointer is allocated by the caller.
*                    this can be a NULL pointer if the caller does not require the
*                    trunkId(only wants to know that the member belongs to a
*                    trunk)
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM - bad device number
*       GT_NOT_FOUND - the pair {devNum,port} not a trunk member
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkDbIsMemberOfTrunk
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_TRUNK_MEMBER_STC member;
    GT_TRUNK_ID trunkId;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    member.port = (GT_U8)inArgs[1];
    member.device = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmTrunkDbIsMemberOfTrunk(devNum, &member, &trunkId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", trunkId);
    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmTrunkHashDesignatedTableModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the hashing mode of designated trunk table hash generation.
*       NOTE : relevant to all multi-destination traffic.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number.
*       mode - the LBH mode to use
*                "no lbh": The designated hashing uses only the first entry
*                "vid,vidx,src interface" : LBH according to vid,vidx,srcDev,srcPort
*                "vid,vidx,src interface,flowId,ditPtr" :
*                               LBH according to vid,vidx,srcDev,srcPort,flowId,
*                               ditPtr
*                               ditPtr used for VPLS packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashDesignatedTableModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashDesignatedTableModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashDesignatedTableModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the hashing mode of designated trunk table hash generation.
*       NOTE : relevant to all multi-destination traffic.
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       modePtr - (pointer to)the LBH mode
*                "no lbh": The designated hashing uses only the first entry
*                "vid,vidx,src interface" : LBH according to vid,vidx,srcDev,srcPort
*                "vid,vidx,src interface,flowId,ditPtr" :
*                               LBH according to vid,vidx,srcDev,srcPort,flowId,
*                               ditPtr
*                               ditPtr used for VPLS packets.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashDesignatedTableModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TRUNK_LBH_DESIGNATED_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashDesignatedTableModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashGlobalModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the general hashing mode of trunk hash generation.
*
* APPLICABLE DEVICES:   All EXMXPM devices
*
* INPUTS:
*       devNum  - the device number.
*       hashMode   - hash mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or hash mode
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashGlobalModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TRUNK_LBH_GLOBAL_MODE_ENT hashMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    hashMode = (CPSS_EXMXPM_TRUNK_LBH_GLOBAL_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashGlobalModeSet(devNum, hashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashGlobalModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the general hashing mode of trunk hash generation.
*
* APPLICABLE DEVICES:   All EXMXPM devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       hashModePtr   - (pointer to)hash mode
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashGlobalModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TRUNK_LBH_GLOBAL_MODE_ENT hashMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashGlobalModeGet(devNum, &hashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hashMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashAddMacModeEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/disable the use of MAC address bits to trunk hash calculation
*       when packet is either IP and the "Ip trunk hash mode enabled",
*       or MPLS and the "MPLS hash mode enabled"
*
*       Note:
*       1. Relevant when Either IP and the "Ip trunk hash mode enabled" ,
*          Or MPLS and the "MPLS hash mode enabled"
*       2. Relevant when cpssExMxPmTrunkHashGlobalModeSet(devNum,
*                       CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E)
*       3. In any other case of packet based hash calculation, the MAC is added
*          even if this setting is disabled
*
* APPLICABLE DEVICES:   All EXMXPM devices
*
* INPUTS:
*       devNum  - the device number.
*       enable  - enable/disable feature
*                   GT_FALSE - If the packet is an IP/MPLS packet, MAC data is not
*                              added to the Trunk load balancing hash.
*                   GT_TRUE - The following function is added th the trunk load
*                             balancing hash:
*                             MACTrunkHash = MAC_SA[5:0] ^ MAC_DA[5:0]
*                             if the packet is IPv4/6-over-X tunnel-terminated,
*                             the mode is always GT_FALSE (since there is no
*                              passenger packet MAC header).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashAddMacModeEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashAddMacModeEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashAddMacModeEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the state of the use of MAC address bits to trunk hash calculations
*       when the packet is either IP and the "Ip trunk hash mode enabled" ,
*       or MPLS and the "MPLS hash mode enabled"
*
*       Note:
*       1. Relevant when Either IP and the "Ip trunk hash mode enabled" ,
*          Or MPLS and the "MPLS hash mode enabled"
*       2. Relevant when cpssExMxPmTrunkHashGlobalModeSet(devNum,
*                       CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E)
*       3. In any other case of packet based hash calculation, the MAC is added
*          even this setting is disabled
*
* APPLICABLE DEVICES:   All EXMXPM devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)enable/disable feature
*                   GT_FALSE - If the packet is an IP/MPLS packet MAC data is not
*                              added to the Trunk load balancing hash.
*                   GT_TRUE - The following function is added th the trunk load
*                             balancing hash:
*                             MACTrunkHash = MAC_SA[5:0] ^ MAC_DA[5:0]
*                             If the packet is IPv4/6-over-X tunnel-terminated,
*                             the mode is always GT_FALSE (since there is no
*                              passenger packet MAC header).
*
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashAddMacModeEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashAddMacModeEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashIpModeEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the IP SIP/DIP information,
*       when calculating the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant for NON IP packets.
*       2. Relevant when cpssExMxPmTrunkHashGlobalModeSet(devNum,
*                       CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E)
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number.
*       enable - GT_FALSE - IP data is not added to the trunk load balancing
*                           hash.
*               GT_TRUE - The following function is added to the trunk load
*                         balancing hash, if the packet is IPv6.
*                         IPTrunkHash = according to setting of API
*                                       cpssExMxPmTrunkHashIpv6ModeSet(...)
*                         else packet is IPv4:
*                         IPTrunkHash = SIP[5:0]^DIP[5:0]^SIP[21:16]^DIP[21:16].
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashIpModeEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashIpModeEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashIpModeEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the enable/disable of device from considering the IP SIP/DIP
*       information, when calculation the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant for non IP packets.
*       2. Relevant when cpssExMxPmTrunkHashGlobalModeSet(devNum,
*                       CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E)
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr -(pointer to)
*               GT_FALSE - IP data is not added to the trunk load balancing
*                           hash.
*               GT_TRUE - The following function is added to the trunk load
*                         balancing hash, if the packet is IPv6.
*                         IPTrunkHash = according to setting of API
*                                       cpssExMxPmTrunkHashIpv6ModeSet(...)
*                         else packet is IPv4:
*                         IPTrunkHash = SIP[5:0]^DIP[5:0]^SIP[21:16]^DIP[21:16].
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashIpModeEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashIpModeEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashL4ModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the L4 TCP/UDP
*       source/destination port information, when calculating the trunk hashing
*       index for a packet.
*
*       Note:
*       1. Not relevant to NON TCP/UDP packets.
*       2. The Ipv4 hash must also be enabled, otherwise the L4 hash mode
*          setting is not considered.
*       2. Relevant when cpssExMxPmTrunkHashGlobalModeSet(devNum,
*                       CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E)
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number.
*       hashMode - L4 hash mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashL4ModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TRUNK_LBH_L4_MODE_ENT hashMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    hashMode = (CPSS_EXMXPM_TRUNK_LBH_L4_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashL4ModeSet(devNum, hashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashL4ModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       get the Enable/Disable of device from considering the L4 TCP/UDP
*       source/destination port information, when calculation the trunk hashing
*       index for a packet.
*
*       Note:
*       1. Not relevant to NON TCP/UDP packets.
*       2. The Ipv4 hash must also be enabled , otherwise the L4 hash mode
*          setting not considered.
*       2. Relevant when cpssExMxPmTrunkHashGlobalModeSet(devNum,
*                       CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E)
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       hashModePtr - (pointer to)L4 hash mode.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashL4ModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TRUNK_LBH_L4_MODE_ENT hashMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashL4ModeGet(devNum, &hashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", hashMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashIpv6ModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the hash generation function for Ipv6 packets.
*
* APPLICABLE DEVICES:   All EXMXPM devices
*
* INPUTS:
*       devNum     - the device number.
*       hashMode   - the Ipv6 hash mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or hash mode
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashIpv6ModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TRUNK_LBH_IPV6_MODE_ENT hashMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    hashMode = (CPSS_EXMXPM_TRUNK_LBH_IPV6_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashIpv6ModeSet(devNum, hashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashIpv6ModeGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the hash generation function for Ipv6 packets.
*
* APPLICABLE DEVICES:   All EXMXPM devices
*
* INPUTS:
*       devNum     - the device number.
*
* OUTPUTS:
*       hashModePtr - (pointer to)the Ipv6 hash mode.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or hash mode
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashIpv6ModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TRUNK_LBH_IPV6_MODE_ENT hashMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    hashMode = (CPSS_EXMXPM_TRUNK_LBH_IPV6_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashIpv6ModeGet(devNum, &hashMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashMplsModeEnableSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the MPLS information,
*       when calculating the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant to NON-MPLS packets.
*       2. Relevant when cpssExMxPmTrunkHashGlobalModeSet(devNum,
*                       CPSS_EXMXPM_TRUNK_LBH_PACKETS_INFO_E)
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number.
*       enable - GT_FALSE - MPLS parameter are not used in trunk hash index
*                GT_TRUE  - The following function is added to the trunk load
*                   balancing hash:
*               MPLSTrunkHash = (mpls_label0[5:0] & mpls_trunk_lbl0_mask) ^
*                               (mpls_label1[5:0] & mpls_trunk_lbl1_mask) ^
*                               (mpls_label2[5:0] & mpls_trunk_lbl2_mask)
*               NOTE:
*                   If any of MPLS Labels 0:2 do not exist in the packet,
*                   the default value 0x0 is used for TrunkHash calculation
*                   instead.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashMplsModeEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashMplsModeEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashMplsModeEnableGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get trunk MPLS hash mode
*
* APPLICABLE DEVICES:   All ExMxPm devices
*
* INPUTS:
*       devNum  - the device number.
*
* OUTPUTS:
*       enablePtr - (pointer to)the MPLS hash mode.
*               GT_FALSE - MPLS parameter are not used in trunk hash index
*               GT_TRUE  - The following function is added to the trunk load
*                   balancing hash:
*               MPLSTrunkHash = (mpls_label0[5:0] & mpls_trunk_lbl0_mask) ^
*                               (mpls_label1[5:0] & mpls_trunk_lbl1_mask) ^
*                               (mpls_label2[5:0] & mpls_trunk_lbl2_mask)
*               NOTE:
*                   If any of MPLS Labels 0:2 do not exist in the packet,
*                   the default value 0x0 is used for TrunkHash calculation
*                   instead.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashMplsModeEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashMplsModeEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashIpShiftSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the shift being done to IP addresses prior to hash calculations.
*
* APPLICABLE DEVICES:   All EXMXPM devices
*
* INPUTS:
*       devNum  - the device number.
*       protocolStack - Set the shift to either IPv4 or IPv6 IP addresses.
*       isSrcIp       - GT_TRUE  = Set the shift to IPv4/6 source addresses.
*                       GT_FALSE = Set the shift to IPv4/6 destination addresses.
*       shiftValue    - The shift to be done.
*                       IPv4 valid shift: 0-3 bytes (Value = 0: no shift).
*                       IPv6 valid shift: 0-15 bytes (Value = 0: no shift).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or protocolStack
*       GT_OUT_OF_RANGE - shiftValue > 3 for IPv4 , shiftValue > 15 for IPv6
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashIpShiftSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    GT_BOOL isSrcIp;
    GT_U32 shiftValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    isSrcIp = (GT_BOOL)inArgs[2];
    shiftValue = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashIpShiftSet(devNum, protocolStack, isSrcIp, shiftValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashIpShiftGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the shift being done to IP addresses prior to hash calculations.
*
* APPLICABLE DEVICES:   All EXMXPM devices
*
* INPUTS:
*       devNum  - the device number.
*       protocolStack - Set the shift to either IPv4 or IPv6 IP addresses.
*       isSrcIp       - GT_TRUE  = Set the shift to IPv4/6 source addresses.
*                       GT_FALSE = Set the shift to IPv4/6 destination addresses.
*
* OUTPUTS:
*       shiftValuePtr    - (pointer to) The shift to be done.
*                       IPv4 valid shift: 0-3 bytes (Value = 0: no shift).
*                       IPv6 valid shift: 0-15 bytes (Value = 0: no shift).
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or protocolStack
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashIpShiftGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_IP_PROTOCOL_STACK_ENT protocolStack;
    GT_BOOL isSrcIp;
    GT_U32 shiftValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    isSrcIp = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashIpShiftGet(devNum, protocolStack, isSrcIp, &shiftValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", shiftValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashMaskSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Set the masks for the various packet fields being used at the Trunk
*       hash calculations
*
* APPLICABLE DEVICES:   All EXMXPM devices
*
* INPUTS:
*       devNum  - the device number.
*       maskedField - field to apply the mask on
*       maskValue - The mask value to be used (0..0x3F)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or maskedField
*       GT_OUT_OF_RANGE - maskValue > 0x3F
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashMaskSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TRUNK_LBH_MASK_ENT maskedField;
    GT_U8 maskValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    maskedField = (CPSS_EXMXPM_TRUNK_LBH_MASK_ENT)inArgs[1];
    maskValue = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashMaskSet(devNum, maskedField, maskValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmTrunkHashMaskGet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Get the masks for the various packet fields being used at the Trunk
*       hash calculations
*
* APPLICABLE DEVICES:   All EXMXPM devices
*
* INPUTS:
*       devNum  - the device number.
*       maskedField - field to apply the mask on
*
* OUTPUTS:
*       maskValuePtr - (pointer to)The mask value to be used (0..0x3F)
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or maskedField
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*       none
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkHashMaskGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_TRUNK_LBH_MASK_ENT maskedField;
    GT_U8 maskValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    maskedField = (CPSS_EXMXPM_TRUNK_LBH_MASK_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkHashMaskGet(devNum, maskedField, &maskValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maskValue);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmTrunkMcLocalSwitchingEnableSet
*
*       Function Relevant mode : High Level mode
*
* DESCRIPTION:
*       Enable/Disable sending multi-destination packets back to its source
*       trunk on the local device.
*
* APPLICABLE DEVICES:
*        ExMxPm1; ExMxPm1_Diamond; ExMxPm2; ExMxPm3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       enable      - Boolean value:
*                     GT_TRUE  - multi-destination packets may be sent back to
*                                their source trunk on the local device.
*                     GT_FALSE - multi-destination packets are not sent back to
*                                their source trunk on the local device.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - bad device number , or bad trunkId number
*       GT_BAD_STATE       - the trunk-type is not one of: CPSS_TRUNK_TYPE_FREE_E or CPSS_TRUNK_TYPE_REGULAR_E
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. the behavior of multi-destination traffic ingress from trunk is
*       not-affected by setting of cpssExMxPmBrgVlanLocalSwitchingEnableSet
*       and not-affected by setting of cpssExMxPmBrgPortEgressMcastLocalEnable
*       2. the functionality manipulates the 'non-trunk' table entry of the trunkId
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmTrunkMcLocalSwitchingEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS              result;

    GT_U8                  devNum;
    GT_TRUNK_ID            trunkId;
    GT_BOOL                 enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    trunkId = (GT_TRUNK_ID)inFields[0];
    enable = (GT_BOOL)inFields[1];

    /* call cpss api function */
    result = cpssExMxPmTrunkMcLocalSwitchingEnableSet(devNum, trunkId, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*
 the function keep on looking for next trunk that is set <multi-dest local switch EN> = GT_TRUE
 starting with trunkId = (gTrunkId+1).

 function return the trunk Id and if <multi-dest local switch EN> = GT_TRUE/GT_FALSE
*/
static CMD_STATUS wrCpssExMxPmTrunkMcLocalSwitchingEnableGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS              result;

    GT_U8                  devNum;
    GT_TRUNK_ID            tmpTrunkId;
    GT_BOOL                enable = GT_FALSE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tmpTrunkId = gTrunkId;

    gTrunkId++;
    /*check if trunk is valid by checking if GT_OK*/
    while(gTrunkId < WR_PRV_TRUNK_MAX_NUM)
    {
        tmpTrunkId = gTrunkId;

        result = cpssExMxPmTrunkDbMcLocalSwitchingEnableGet(devNum,tmpTrunkId,&enable);
        if(result != GT_OK)
        {
            galtisOutput(outArgs, result, "%d",-1);/* Error ???? */
            return CMD_OK;
        }

        if(enable == GT_TRUE)
        {
            break;
        }

        gTrunkId++;
    }

    if(gTrunkId >= WR_PRV_TRUNK_MAX_NUM)
    {
        /* we done with the last trunk , or last trunk is empty */
        galtisOutput(outArgs, GT_OK, "%d",-1);/* no more trunks */
        return CMD_OK;
    }

    inFields[0] = tmpTrunkId;
    inFields[1] = enable; /* the enable at this stage should be == GT_TRUE .
           because we wish to see only 'Special trunks' that 'flood back to src trunk' ! */

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

static CMD_STATUS wrCpssExMxPmTrunkMcLocalSwitchingEnableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    gTrunkId = 0;/*reset on first*/

    return wrCpssExMxPmTrunkMcLocalSwitchingEnableGetNext(inArgs,inFields,numFields,outArgs);
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmTrunkInit",
         &wrCpssExMxPmTrunkInit,
         2, 0},
        {"cpssExMxPmTrunkMembersSet",
         &wrCpssExMxPmTrunkMembersSet,
         1, 19},


        {"cpssExMxPmTrunkPortTrunkIdSet",
         &wrCpssExMxPmTrunkPortTrunkIdSet,
         4, 0},
        {"cpssExMxPmTrunkPortTrunkIdGet",
         &wrCpssExMxPmTrunkPortTrunkIdGet,
         2, 0},
        {"cpssExMxPmTrunkTableEntrySet",
         &wrCpssExMxPmTrunkTableEntrySet,
         2, 3},
        {"cpssExMxPmTrunkTableEntryGetFirst",
         &wrCpssExMxPmTrunkTableEntryGetFirst,
         2, 0},
        {"cpssExMxPmTrunkTableEntryGetNext",
         &wrCpssExMxPmTrunkTableEntryGetNext,
         2, 0},
        {"cpssExMxPmTrunkTableEntryDelete",
            &wrCpssExMxPmTrunkTableEntryDelete,
            2, 2},
        {"cpssExMxPmTrunkNonTrunkPortsEntrySet",
         &wrCpssExMxPmTrunkNonTrunkPortsEntrySet,
         1, 2},
        {"cpssExMxPmTrunkNonTrunkPortsEntryGetFirst",
         &wrCpssExMxPmTrunkNonTrunkPortsEntryGetFirst,
         1, 0},
        {"cpssExMxPmTrunkNonTrunkPortsEntryGetNext",
         &wrCpssExMxPmTrunkNonTrunkPortsEntryGetNext,
         1, 0},

        {"cpssExMxPmTrunkDesignatedPortsEntrySet",
         &wrCpssExMxPmTrunkDesignatedPortsEntrySet,
         1, 2},
        {"cpssExMxPmTrunkDesignatedPortsEntryGetFirst",
         &wrCpssExMxPmTrunkDesignatedPortsEntryGetFirst,
         1, 0},
        {"cpssExMxPmTrunkDesignatedPortsEntryGetNext",
         &wrCpssExMxPmTrunkDesignatedPortsEntryGetNext,
         1, 0},


        {"cpssExMxPmTrunkDbIsMemberOfTrunk",
         &wrCpssExMxPmTrunkDbIsMemberOfTrunk,
         3, 0},
        {"cpssExMxPmTrunkHashDesignatedTableModeSet",
         &wrCpssExMxPmTrunkHashDesignatedTableModeSet,
         2, 0},
        {"cpssExMxPmTrunkHashDesignatedTableModeGet",
         &wrCpssExMxPmTrunkHashDesignatedTableModeGet,
         1, 0},
        {"cpssExMxPmTrunkHashGlobalModeSet",
         &wrCpssExMxPmTrunkHashGlobalModeSet,
         2, 0},
        {"cpssExMxPmTrunkHashGlobalModeGet",
         &wrCpssExMxPmTrunkHashGlobalModeGet,
         1, 0},
        {"cpssExMxPmTrunkHashAddMacModeEnableSet",
         &wrCpssExMxPmTrunkHashAddMacModeEnableSet,
         2, 0},
        {"cpssExMxPmTrunkHashAddMacModeEnableGet",
         &wrCpssExMxPmTrunkHashAddMacModeEnableGet,
         1, 0},
        {"cpssExMxPmTrunkHashIpModeEnableSet",
         &wrCpssExMxPmTrunkHashIpModeEnableSet,
         2, 0},
        {"cpssExMxPmTrunkHashIpModeEnableGet",
         &wrCpssExMxPmTrunkHashIpModeEnableGet,
         1, 0},
        {"cpssExMxPmTrunkHashL4ModeSet",
         &wrCpssExMxPmTrunkHashL4ModeSet,
         2, 0},
        {"cpssExMxPmTrunkHashL4ModeGet",
         &wrCpssExMxPmTrunkHashL4ModeGet,
         1, 0},
        {"cpssExMxPmTrunkHashIpv6ModeSet",
         &wrCpssExMxPmTrunkHashIpv6ModeSet,
         2, 0},
        {"cpssExMxPmTrunkHashIpv6ModeGet",
         &wrCpssExMxPmTrunkHashIpv6ModeGet,
         2, 0},
        {"cpssExMxPmTrunkHashMplsModeEnableSet",
         &wrCpssExMxPmTrunkHashMplsModeEnableSet,
         2, 0},
        {"cpssExMxPmTrunkHashMplsModeEnableGet",
         &wrCpssExMxPmTrunkHashMplsModeEnableGet,
         1, 0},
        {"cpssExMxPmTrunkHashIpShiftSet",
         &wrCpssExMxPmTrunkHashIpShiftSet,
         4, 0},
        {"cpssExMxPmTrunkHashIpShiftGet",
         &wrCpssExMxPmTrunkHashIpShiftGet,
         3, 0},
        {"cpssExMxPmTrunkHashMaskSet",
         &wrCpssExMxPmTrunkHashMaskSet,
         3, 0},
        {"cpssExMxPmTrunkHashMaskGet",
         &wrCpssExMxPmTrunkHashMaskGet,
         2, 0},

    /* start table : cpssExMxPmTrunkMcLocalSwitchingEnable */
    {"cpssExMxPmTrunkMcLocalSwitchingEnableSet",
        &wrCpssExMxPmTrunkMcLocalSwitchingEnableSet,
        1, 2},

    {"cpssExMxPmTrunkMcLocalSwitchingEnableGetFirst",
        &wrCpssExMxPmTrunkMcLocalSwitchingEnableGetFirst,
        1, 0},

    {"cpssExMxPmTrunkMcLocalSwitchingEnableGetNext",
        &wrCpssExMxPmTrunkMcLocalSwitchingEnableGetNext,
        1, 0},
    /* end table   : cpssExMxPmTrunkMcLocalSwitchingEnable */

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmTrunk
*
* DESCRIPTION:
*     Library database initialization function.
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
*******************************************************************************/
GT_STATUS cmdLibInitCpssExMxPmTrunk
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

