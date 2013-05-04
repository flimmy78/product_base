/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapTrunkCpss.c
*
* DESCRIPTION:
*       Wrapper functions for Trunk cpss functions
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
#include <cpss/generic/trunk/cpssGenTrunkTypes.h>
#include <cpss/exMx/exMxGen/trunk/cpssExMxTrunk.h>


/*******************************************************************************
* cpssExMxTrunkInit
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       CPSS ExMx Trunk initialization of PP Tables/registers and
*       SW shadow data structures, all ports are set as non-trunk ports.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum             - device number
*       maxNumberOfTrunks  - maximum number of trunk groups.
*                            when this number is 0 , there will be no shadow
*                            used.
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
*       GT_OUT_OF_RANGE  - the numberOfTrunks is more then what HW support
*                          the Ex1x5, Ex1x6 devices support 31 trunks.
*                          the Ex6x5, Ex6x8 , Ex6x0 devices support 7 trunks.
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkInit

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U8  maxNumberOfTrunks;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    maxNumberOfTrunks = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTrunkInit(devNum, maxNumberOfTrunks);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTrunkMembersSet
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function set the trunk with the specified enabled and disabled
*       members.
*       this setting override the previous setting of the trunk members.
*
*       the user can "invalidate/unset" trunk entry by setting :
*           numOfEnabledMembers = 0 and  numOfDisabledMembers = 0
*
*       This function support next "set entry" options :
*       1. "reset" the entry
*          function will remove the previous settings
*       2. set entry after the entry was empty
*          function will set new settings
*       3. set entry with the same members that it is already hold
*          function will rewrite the HW entries as it was
*       4. set entry with different setting then previous setting
*          a. function will remove the previous settings
*          b. function will set new settings
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*
*       devNum      - device number
*       trunkId     - trunk id
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
*       GT_OUT_OF_RANGE - when the sum of number of enabled members + number of
*                         disabled members exceed the number of maximum number
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST - one of the members already exists in another trunk
*
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkMembersSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS              result;

    GT_U8                  devNum;
    GT_U32                 numOfEnabledMembers = 0;
    CPSS_TRUNK_MEMBER_STC  enabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_U32                 numOfDisabledMembers = 0;
    CPSS_TRUNK_MEMBER_STC  disabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];
    GT_TRUNK_ID            trunkId;
    GT_U8                  i, j;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    trunkId = (GT_TRUNK_ID)inFields[0];

    j = 1;

    for(i=0; i < CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS; i++)
    {
       if(inFields[j])
       {
             enabledMembersArray[i].port = (GT_U8)inFields[j+1];
             enabledMembersArray[i].device = (GT_U8)inFields[j+2];
             CONVERT_DEV_PORT_DATA_MAC(enabledMembersArray[i].device, enabledMembersArray[i].port);
             numOfEnabledMembers++;
       }

       else
       {
             disabledMembersArray[i].port = (GT_U8)inFields[j+1];
             disabledMembersArray[i].device = (GT_U8)inFields[j+2];
             CONVERT_DEV_PORT_DATA_MAC(enabledMembersArray[i].device, enabledMembersArray[i].port);
             numOfDisabledMembers++;
       }

       j = j+3;

    }

    /* call tapi api function */
    result = cpssExMxTrunkMembersSet(devNum, trunkId, numOfEnabledMembers,
                                     enabledMembersArray, numOfDisabledMembers,
                                     disabledMembersArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTrunkMemberAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function add member to the trunk in the device.
*       If member is already in this trunk , function do nothing and
*       return GT_OK.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum      - the device number on which to add member to the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to add to the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_ALREADY_EXIST - this member already exists in another trunk.
*       GT_FULL - trunk already contains maximum supported members
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkMemberAdd

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                  devNum;
    GT_TRUNK_ID            trunkId;
    CPSS_TRUNK_MEMBER_STC  memberPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    memberPtr.port = (GT_U8)inArgs[2];
    memberPtr.device = (GT_U8)inArgs[3];
    CONVERT_DEV_PORT_DATA_MAC(memberPtr.device, memberPtr.port);


    /* call cpss api function */
    result = cpssExMxTrunkMemberAdd(devNum, trunkId, &memberPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTrunkMemberRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function remove member from a trunk in the device.
*       If member not exists in this trunk , function do nothing and
*       return GT_OK.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum      - the device number on which to remove member from the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to remove from the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkMemberRemove

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                  devNum;
    GT_TRUNK_ID            trunkId;
    CPSS_TRUNK_MEMBER_STC  memberPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    memberPtr.port = (GT_U8)inArgs[2];
    memberPtr.device = (GT_U8)inArgs[3];
    CONVERT_DEV_PORT_DATA_MAC(memberPtr.device, memberPtr.port);

    /* call cpss api function */
    result = cpssExMxTrunkMemberRemove(devNum, trunkId, &memberPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTrunkMemberDisable
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function disable (enabled)existing member of trunk in the device.
*       If member is already disabled in this trunk , function do nothing and
*       return GT_OK.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum      - the device number on which to disable member in the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to disable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkMemberDisable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                  devNum;
    GT_TRUNK_ID            trunkId;
    CPSS_TRUNK_MEMBER_STC  memberPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    memberPtr.port = (GT_U8)inArgs[2];
    memberPtr.device = (GT_U8)inArgs[3];
    CONVERT_DEV_PORT_DATA_MAC(memberPtr.device, memberPtr.port);

    /* call cpss api function */
    result = cpssExMxTrunkMemberDisable(devNum, trunkId, &memberPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTrunkMemberEnable
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       This function enable (disabled)existing member of trunk in the device.
*       If member is already enabled in this trunk , function do nothing and
*       return GT_OK.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum      - the device number on which to enable member in the trunk
*       trunkId     - the trunk id.
*       memberPtr   - (pointer to)the member to enable in the trunk.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad member parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*       GT_NOT_FOUND - this member not found (member not exist) in the trunk
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkMemberEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                  devNum;
    GT_TRUNK_ID            trunkId;
    CPSS_TRUNK_MEMBER_STC  memberPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    memberPtr.port = (GT_U8)inArgs[2];
    memberPtr.device = (GT_U8)inArgs[3];
    CONVERT_DEV_PORT_DATA_MAC(memberPtr.device, memberPtr.port);

    /* call cpss api function */
    result = cpssExMxTrunkMemberEnable(devNum, trunkId, &memberPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTrunkNonTrunkPortsAdd
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*       add the ports to the trunk's non-trunk entry .
*       NOTE : the ports are add to the "non trunk" table only and not effect
*              other trunk relate tables/registers.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsBmpPtr - (pointer to)bitmap of ports to add to
*                             "non-trunk members"
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkNonTrunkPortsAdd

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPortsBmpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    nonTrunkPortsBmpPtr.ports[0] = (GT_U32)inArgs[2];
    nonTrunkPortsBmpPtr.ports[1] = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxTrunkNonTrunkPortsAdd(devNum, trunkId,
                                      &nonTrunkPortsBmpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTrunkNonTrunkPortsRemove
*
* DESCRIPTION:
*       Function Relevant mode : High Level mode
*
*      Removes the ports from the trunk's non-trunk entry .
*      NOTE : the ports are removed from the "non trunk" table only and not
*              effect other trunk relate tables/registers.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id - in this API trunkId can be ZERO !
*       nonTrunkPortsBmpPtr - (pointer to)bitmap of ports to remove from
*                             "non-trunk members"
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkNonTrunkPortsRemove

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPortsBmpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];
    nonTrunkPortsBmpPtr.ports[0] = (GT_U32)inArgs[2];
    nonTrunkPortsBmpPtr.ports[1] = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxTrunkNonTrunkPortsRemove(devNum, trunkId,
                                         &nonTrunkPortsBmpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTrunkPortTrunkIdSet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number
*       portId  - the port number.
*       memberOfTrunk - is the port associated with the trunk
*                 GT_FALSE - the port is set as "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*
*       trunkId - the trunk to which the port associate with
*                 This field indicates the trunk group number (ID) to which the
*                 port is a member.
*                 1 through 31 = The port is a member of the trunk
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
CMD_STATUS wrCpssExMxTrunkPortTrunkIdSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portId;
    GT_BOOL  memberOfTrunk;
    GT_TRUNK_ID  trunkId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portId = (GT_U8)inArgs[1];
    memberOfTrunk = (GT_BOOL)inArgs[2];
    trunkId = (GT_TRUNK_ID)inArgs[3];

    /* call cpss api function */
    result = cpssExMxTrunkPortTrunkIdSet(devNum, portId, memberOfTrunk, trunkId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTrunkPortTrunkIdGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunkId field in the port's control register in the device
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number
*       portId  - the port number.
*
* OUTPUTS:
*       memberOfTrunkPtr - (pointer to) is the port associated with the trunk
*                 GT_FALSE - the port is set as "not member" in the trunk
*                 GT_TRUE  - the port is set with the trunkId
*       trunkIdPtr - (pointer to)the trunk to which the port associate with
*                 This field indicates the trunk group number (ID) to which the
*                 port is a member.
*                 1 through 31 = The port is a member of the trunk
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
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkPortTrunkIdGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    portId;
    GT_BOOL  memberOfTrunkPtr;
    GT_TRUNK_ID  trunkIdPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portId = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTrunkPortTrunkIdGet(devNum, portId, &memberOfTrunkPtr,
                                                               &trunkIdPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", memberOfTrunkPtr, trunkIdPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTrunkTableEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk table entry , and set the number of members in it.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
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
*                         of members in trunk (total of 0 - 8 members allowed)
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number , or
*                      bad members parameters :
*                          (device & 0x80) != 0  means that the HW can't support
*                                              this value , since HW has 7 bit
*                                              for device number
*                          (port & 0xC0) != 0  means that the HW can't support
*                                              this value , since HW has 6 bit
*                                              for port number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkTableEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                 devNum, i, j;
    GT_TRUNK_ID           trunkId;
    GT_U32                numMembers;
    CPSS_TRUNK_MEMBER_STC membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];

    numMembers = (GT_U32)inFields[0];

    if(numMembers > CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
        return CMD_FIELD_OVERFLOW;

    j = 1;

    for(i=0; i < numMembers; i++)
    {
        membersArray[i].port = (GT_U8)inFields[j];
        membersArray[i].device = (GT_U8)inFields[j+1];
        CONVERT_DEV_PORT_DATA_MAC(membersArray[i].device, membersArray[i].port);

        j = j+2;
    }

    /* call cpss api function */
    result = cpssExMxTrunkTableEntrySet(devNum, trunkId, numMembers,
                                                       membersArray);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTrunkTableEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk table entry , and get the number of members in it.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id
*
* OUTPUTS:
*       numMembersPtr - (pointer to)num of members in the trunk
*       membersArray - array of enabled members of the trunk
*                      array is allocated by the caller , it is assumed that
*                      the array can hold CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS
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
CMD_STATUS wrCpssExMxTrunkTableEntryGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                 devNum;
    GT_TRUNK_ID           trunkId;
    GT_U32                numMembersPtr;
    CPSS_TRUNK_MEMBER_STC membersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];

    cmdOsMemSet(membersArray, 0xFF, sizeof(membersArray));

    /* call cpss api function */
    result = cpssExMxTrunkTableEntryGet(devNum, trunkId, &numMembersPtr,
                                                           membersArray);

    CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[0].device ,membersArray[0].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[1].device ,membersArray[1].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[2].device ,membersArray[2].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[3].device ,membersArray[3].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[4].device ,membersArray[4].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[5].device ,membersArray[5].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[6].device ,membersArray[6].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(membersArray[7].device ,membersArray[7].port);


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                membersArray[0].port, membersArray[0].device,
                membersArray[1].port, membersArray[1].device,
                membersArray[2].port, membersArray[2].device,
                membersArray[3].port, membersArray[3].device,
                membersArray[4].port, membersArray[4].device,
                membersArray[5].port, membersArray[5].device,
                membersArray[6].port, membersArray[6].device,
                membersArray[7].port, membersArray[7].device);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************
* Function: cpssExMxTrunkNonTrunkPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the trunk's non-trunk ports specific bitmap entry.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
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
CMD_STATUS wrCpssExMxTrunkNonTrunkPortsEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPortsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];

    nonTrunkPortsPtr.ports[0] = (GT_U32)inFields[0];
    nonTrunkPortsPtr.ports[1] = (GT_U32)inFields[1];


    /* call cpss api function */
    result = cpssExMxTrunkNonTrunkPortsEntrySet(devNum, trunkId,
                                                &nonTrunkPortsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* Function: cpssExMxTrunkNonTrunkPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the trunk's non-tr
unk ports bitmap specific entry.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number
*       trunkId - trunk id - in this API trunkId can be ZERO !
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
CMD_STATUS wrCpssExMxTrunkNonTrunkPortsEntryGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8               devNum;
    GT_TRUNK_ID         trunkId;
    CPSS_PORTS_BMP_STC  nonTrunkPortsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTrunkNonTrunkPortsEntryGet(devNum, trunkId,
                                                &nonTrunkPortsPtr);

    inFields[0] = nonTrunkPortsPtr.ports[0];
    inFields[1] = nonTrunkPortsPtr.ports[1];

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTrunkDesignatedPortsEntrySet
*
* DESCRIPTION:
*       Function Relevant mode : Low level mode
*
*       Set the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum          - the device number
*       entryIndex      - the index in the designated ports bitmap table
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
CMD_STATUS wrCpssExMxTrunkDesignatedPortsEntrySet

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
    CPSS_PORTS_BMP_STC  designatedPortsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U32)inArgs[1];

    designatedPortsPtr.ports[0] = (GT_U32)inFields[0];
    designatedPortsPtr.ports[1] = (GT_U32)inFields[1];

    /* call cpss api function */
    result = cpssExMxTrunkDesignatedPortsEntrySet(devNum, entryIndex,
                                                  &designatedPortsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTrunkDesignatedPortsEntryGet
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       Get the designated trunk table specific entry.
*
* APPLICABLE DEVICES:   All ExMx devices
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
*                         the index must be in range (0 - 15)
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkDesignatedPortsEntryGet

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
    CPSS_PORTS_BMP_STC  designatedPortsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_TRUNK_ID)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTrunkDesignatedPortsEntryGet(devNum, entryIndex,
                                                  &designatedPortsPtr);

    inFields[0] = designatedPortsPtr.ports[0];
    inFields[1] = designatedPortsPtr.ports[1];

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxTrunkDbEnabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the enabled members of the trunk
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       numOfEnabledMembersPtr - (pointer to) max num of enabled members to
*                                retrieve - this value refer to the number of
*                                members that the array of enabledMembersArray[]
*                                can retrieve.
*
* OUTPUTS:
*       numOfEnabledMembersPtr - (pointer to) the actual num of enabled members
*                      in the trunk (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
*       enabledMembersArray - (array of) enabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkDbEnabledMembersGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                 devNum;
    GT_TRUNK_ID           trunkId;
    GT_U32                numOfEnabledMembersPtr;
    CPSS_TRUNK_MEMBER_STC enabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];

    cmdOsMemSet(enabledMembersArray, 0xFF, sizeof(enabledMembersArray));

    /* call cpss api function */
    result = cpssExMxTrunkDbEnabledMembersGet(devNum, trunkId,
                                              &numOfEnabledMembersPtr,
                                                  enabledMembersArray);

    CONVERT_BACK_DEV_PORT_DATA_MAC(enabledMembersArray[0].device ,enabledMembersArray[0].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(enabledMembersArray[1].device ,enabledMembersArray[1].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(enabledMembersArray[2].device ,enabledMembersArray[2].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(enabledMembersArray[3].device ,enabledMembersArray[3].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(enabledMembersArray[4].device ,enabledMembersArray[4].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(enabledMembersArray[5].device ,enabledMembersArray[5].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(enabledMembersArray[6].device ,enabledMembersArray[6].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(enabledMembersArray[7].device ,enabledMembersArray[7].port);


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                enabledMembersArray[0].port, enabledMembersArray[0].device,
                enabledMembersArray[1].port, enabledMembersArray[1].device,
                enabledMembersArray[2].port, enabledMembersArray[2].device,
                enabledMembersArray[3].port, enabledMembersArray[3].device,
                enabledMembersArray[4].port, enabledMembersArray[4].device,
                enabledMembersArray[5].port, enabledMembersArray[5].device,
                enabledMembersArray[6].port, enabledMembersArray[6].device,
                enabledMembersArray[7].port, enabledMembersArray[7].device);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTrunkDbDisabledMembersGet
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       return the disabled members of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum      - the device number .
*       trunkId     - the trunk id.
*       numOfDisabledMembersPtr - (pointer to) max num of disabled members to
*                                retrieve - this value refer to the number of
*                                members that the array of enabledMembersArray[]
*                                can retrieve.
*
* OUTPUTS:
*       numOfDisabledMembersPtr -(pointer to) the actual num of disabled members
*                      in the trunk (up to CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS)
*       disabledMembersArray - (array of) disabled members of the trunk
*                             array was allocated by the caller
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM - bad device number , or
*                      bad trunkId number
*       GT_BAD_PTR - one of the parameters in NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkDbDisabledMembersGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                 devNum;
    GT_TRUNK_ID           trunkId;
    GT_U32                numOfDisabledMembersPtr;
    CPSS_TRUNK_MEMBER_STC disabledMembersArray[CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trunkId = (GT_TRUNK_ID)inArgs[1];

    cmdOsMemSet(disabledMembersArray, 0xFF, sizeof(disabledMembersArray));

    /* call cpss api function */
    result = cpssExMxTrunkDbDisabledMembersGet(devNum, trunkId,
                                              &numOfDisabledMembersPtr,
                                                  disabledMembersArray);

    CONVERT_BACK_DEV_PORT_DATA_MAC(disabledMembersArray[0].device ,disabledMembersArray[0].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(disabledMembersArray[1].device ,disabledMembersArray[1].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(disabledMembersArray[2].device ,disabledMembersArray[2].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(disabledMembersArray[3].device ,disabledMembersArray[3].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(disabledMembersArray[4].device ,disabledMembersArray[4].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(disabledMembersArray[5].device ,disabledMembersArray[5].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(disabledMembersArray[6].device ,disabledMembersArray[6].port);
    CONVERT_BACK_DEV_PORT_DATA_MAC(disabledMembersArray[7].device ,disabledMembersArray[7].port);

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                disabledMembersArray[0].port, disabledMembersArray[0].device,
                disabledMembersArray[1].port, disabledMembersArray[1].device,
                disabledMembersArray[2].port, disabledMembersArray[2].device,
                disabledMembersArray[3].port, disabledMembersArray[3].device,
                disabledMembersArray[4].port, disabledMembersArray[4].device,
                disabledMembersArray[5].port, disabledMembersArray[5].device,
                disabledMembersArray[6].port, disabledMembersArray[6].device,
                disabledMembersArray[7].port, disabledMembersArray[7].device);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxTrunkDbIsMemberOfTrunk
*
* DESCRIPTION:
*       Function Relevant mode : High level mode
*
*       Checks if a member {device,port} is a trunk member.
*       if it is trunk member the function retrieve the trunkId of the trunk.
*
*       function uses the DB (no HW operations)
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number.
*       memberPtr - (pointer to) the member to check if is trunk member
*
* OUTPUTS:
*       trunkIdPtr - (pointer to) trunk id of the port .
*                    this pointer allocated by the caller.
*                    this can be NULL pointer if the caller not require the
*                    trunkId(only wanted to know that the member belongs to a
*                    trunk)
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED -the trunk library was not initialized for the device
*       GT_BAD_PARAM - bad device number
*       GT_NOT_FOUND - the pair {devNum,port} not a trunk member
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssExMxTrunkDbIsMemberOfTrunk

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                  devNum;
    CPSS_TRUNK_MEMBER_STC  memberPtr;
    GT_TRUNK_ID            trunkIdPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    memberPtr.port = (GT_U8)inArgs[1];
    memberPtr.device = (GT_U8)inArgs[2];
    CONVERT_DEV_PORT_DATA_MAC(memberPtr.device ,memberPtr.port);


    /* call cpss api function */
    result = cpssExMxTrunkDbIsMemberOfTrunk(devNum, &memberPtr, &trunkIdPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", trunkIdPtr);
    return CMD_OK;
}

/*******************************************************************************
* Function: cpssExMxTrunkHashIpModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the IP SIP/DIP information,
*       when calculation the trunk hashing index for a packet.
*
*       Note:
*       1. Not relevant to NON Ip packets.
*       2. Not relevant to multi-destination packets (include routed IPM).
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number.
*       ipHashEnable - GT_TRUE: if the packet is IP, the bridging engine
*                      uses the IP source and destination address in the
*                      trunking hash function.  If the packet is not IP,
*                      the bridging engine uses the MAC SA and DA in the
*                      trunking hash function
*                      GT_FALSE: the bridging engine always uses the
*                      MAC SA and DA in the trunking hash function.
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
CMD_STATUS wrCpssExMxTrunkHashIpModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enableIpHash;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enableIpHash = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTrunkHashIpModeSet(devNum, enableIpHash);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* Function: cpssExMxTrunkHashL4ModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device from considering the L4 TCP/UDP
*       source/destination port information, when calculation the trunk hashing
*       index for a packet.
*
*       Note:
*       1. Not relevant to NON TCP/UDP packets.
*       2. The Ipv4 hash must also be enabled , otherwise the L4 hash mode
*          setting not considered.
*       3. Not relevant for routed packets (only to bridged) - For Ex1x6 this is
*          relevant to routed packets also.
*       4. Not relevant to multi-destination packets (include routed IPM).
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number.
*    L4HashEnable -
*            GT_TRUE:
*               L4 hashing enable -
*               Enable the device to calculate trunk hash index considering
*               TCP/UDP source/destination port info.
*
*            GT_FALSE:
*               L4 hashing disable - the packet hashing not consider any L4
*               parameters.
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
CMD_STATUS wrCpssExMxTrunkHashL4ModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  enableL4Hash;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enableL4Hash = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTrunkHashL4ModeSet(devNum, enableL4Hash);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* Function: cpssExMxTrunkDesignatedTableHashModeSet
*
* DESCRIPTION:
*       Function Relevant mode : ALL modes
*
*       Enable/Disable the device using the entire designated port trunk table
*       or to always use only the first entry.
*
* APPLICABLE DEVICES:   All ExMx devices
*
* INPUTS:
*       devNum  - the device number.
*       useEntireTable -
*                       GT_TRUE:
*                           The designated hashing uses the entire table.
*                           (one of 16 entries)
*                       GT_FALSE:
*                           The designated hashing use only the first entry
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
CMD_STATUS wrCpssExMxTrunkDesignatedTableHashModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  useEntireTable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    useEntireTable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxTrunkDesignatedTableHashModeSet(devNum, useEntireTable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}




/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxTrunkInit",
        &wrCpssExMxTrunkInit,
        2, 0},

    {"cpssExMxTrunkMembersSet",
        &wrCpssExMxTrunkMembersSet,
        1, CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS * 3},

    {"cpssExMxTrunkMemberAdd",
        &wrCpssExMxTrunkMemberAdd,
        4, 0},

    {"cpssExMxTrunkMemberRemove",
        &wrCpssExMxTrunkMemberRemove,
        4, 0},

    {"cpssExMxTrunkMemberDisable",
        &wrCpssExMxTrunkMemberDisable,
        4, 0},

    {"cpssExMxTrunkMemberEnable",
        &wrCpssExMxTrunkMemberEnable,
        4, 0},

    {"cpssExMxTrunkNonTrunkPortsAdd",
        &wrCpssExMxTrunkNonTrunkPortsAdd,
        4, 0},

    {"cpssExMxTrunkNonTrunkPortsRemove",
        &wrCpssExMxTrunkNonTrunkPortsRemove,
        4, 0},

    {"cpssExMxTrunkPortTrunkIdSet",
        &wrCpssExMxTrunkPortTrunkIdSet,
        4, 0},

    {"cpssExMxTrunkPortTrunkIdGet",
        &wrCpssExMxTrunkPortTrunkIdGet,
        2, 0},

    {"cpssExMxTrunkTableSet",
        &wrCpssExMxTrunkTableEntrySet,
        2, CPSS_TRUNK_MAX_NUM_OF_MEMBERS_CNS * 2},

    {"cpssExMxTrunkTableGetFirst",
        &wrCpssExMxTrunkTableEntryGet,
        2, 0},

    {"cpssExMxTrunkNonTrunkPortsSet",
        &wrCpssExMxTrunkNonTrunkPortsEntrySet,
        2, 2},

    {"cpssExMxTrunkNonTrunkPortsGetFirst",
        &wrCpssExMxTrunkNonTrunkPortsEntryGet,
        2, 0},

    {"cpssExMxTrunkDesignatedPortsSet",
        &wrCpssExMxTrunkDesignatedPortsEntrySet,
        2, 2},

    {"cpssExMxTrunkDesignatedPortsGetFirst",
        &wrCpssExMxTrunkDesignatedPortsEntryGet,
        2, 0},

    {"cpssExMxTrunkDbEnabledMembersGetFirst",
        &wrCpssExMxTrunkDbEnabledMembersGet,
        2, 0},

    {"cpssExMxTrunkDbDisabledMembersGetFirst",
        &wrCpssExMxTrunkDbDisabledMembersGet,
        2, 0},

    {"cpssExMxTrunkDbIsMemberOfTrunk",
        &wrCpssExMxTrunkDbIsMemberOfTrunk,
        3, 0},

    {"cpssExMxTrunkHashIpModeSet",
        &wrCpssExMxTrunkHashIpModeSet,
        2, 0},

    {"cpssExMxTrunkHashL4ModeSet",
        &wrCpssExMxTrunkHashL4ModeSet,
        2, 0},

    {"cpssExMxTrunkDesignatedTableHashModeSet",
        &wrCpssExMxTrunkDesignatedTableHashModeSet,
        2, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxTrunk
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
GT_STATUS cmdLibInitCpssExMxTrunk
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


