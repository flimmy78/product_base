/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChIp.c
*
* DESCRIPTION:
*       Wrapper functions for Cpss DxChXcat Logical Target Mapping
*
* DEPENDENCIES:
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
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/logicalTarget/cpssDxChLogicalTargetMapping.h>

/*****************************Commands*******************************/

/*******************************************************************************
* cpssDxChLogicalTargetMappingEnableSet
*
* DESCRIPTION:
*       Enable/disable logical port mapping feature on the specified device.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
*
* INPUTS:
*       devNum   - physical device number
*       enable   - GT_TRUE:  enable
*                  GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_BOOL      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChLogicalTargetMappingEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status logical port mapping feature on the
*       specified  device.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       enablePtr - Pointer to the enable/disable state.
*                   GT_TRUE : enable,
*                   GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_BOOL      enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChLogicalTargetMappingDeviceEnableSet
*
* DESCRIPTION:
*       Enable/disable a target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       Range (24-31)
*       enable        - GT_TRUE:  enable
*                       GT_FALSE: disable .
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingDeviceEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_U8        logicalDevNum;
    GT_BOOL      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    logicalDevNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingDeviceEnableSet(devNum,
                                                         logicalDevNum,
                                                         enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssDxChLogicalTargetMappingDeviceEnableGet
*
* DESCRIPTION:
*       Get Enable/disable status of target device to be considered as a logical device
*       on the specified device.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
*
* INPUTS:
*       devNum        - physical device number
*       logicalDevNum - logical device number.
*                       Range (24-31)
*
* OUTPUTS:
*       enablePtr     - Pointer to the  Enable/disable state.
*                       GT_TRUE : enable,
*                       GT_FALSE: disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong devNum, logicalDevNum
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingDeviceEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_U8        logicalDevNum;
    GT_BOOL      enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    logicalDevNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChLogicalTargetMappingDeviceEnableGet(devNum,
                                                         logicalDevNum,
                                                         &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}


/*****************************Tables*******************************/


/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntrySet
*
* DESCRIPTION:
*       Set logical target mapping table entry.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      Range (24-31)
*       logicalPortNum               - logical device number.
*                                      Range (0-63)
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingTableEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                  result;
    GT_U8                                      devNum;
    GT_U8                                      logicalDevNum;
    GT_U8                                      logicalPortNum;
    CPSS_DXCH_OUTPUT_INTERFACE_STC             logicalPortMappingTable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    logicalDevNum = (GT_U8)inArgs[1];
    logicalPortNum = (GT_U8)inArgs[2];

    logicalPortMappingTable.isTunnelStart = (GT_BOOL)inFields[0];
    logicalPortMappingTable.tunnelStartInfo.passengerPacketType =
                                (CPSS_DXCH_TUNNEL_PASSANGER_TYPE_ENT)inFields[1];
    logicalPortMappingTable.tunnelStartInfo.ptr = (GT_U32)inFields[2];
    logicalPortMappingTable.physicalInterface.type =
                                            (CPSS_INTERFACE_TYPE_ENT)inFields[3];
    logicalPortMappingTable.physicalInterface.devPort.devNum = (GT_U8)inFields[4];
    logicalPortMappingTable.physicalInterface.devPort.portNum = (GT_U8)inFields[5];
    logicalPortMappingTable.physicalInterface.trunkId = (GT_TRUNK_ID)inFields[6];
    if (logicalPortMappingTable.physicalInterface.type == CPSS_INTERFACE_PORT_E )
    {
        CONVERT_DEV_PORT_DATA_MAC(logicalPortMappingTable.physicalInterface.devPort.devNum,
                             logicalPortMappingTable.physicalInterface.devPort.portNum);

    }
    if (logicalPortMappingTable.physicalInterface.type == CPSS_INTERFACE_TRUNK_E )
    {
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(logicalPortMappingTable.physicalInterface.trunkId);
    }
    logicalPortMappingTable.physicalInterface.vidx = (GT_U16)inFields[7];

   /* call cpss api function */
    result = cpssDxChLogicalTargetMappingTableEntrySet(devNum,
                                                       logicalDevNum,
                                                       logicalPortNum,
                                                       &logicalPortMappingTable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChLogicalTargetMappingTableEntryGet
*
* DESCRIPTION:
*       Get logical target mapping table entry.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
* INPUTS:
*       devNum                       - physical device number.
*       logicalDevNum                - logical device number.
*                                      Range (24-31)
*       logicalPortNum               - logical device number.
*                                      Range (0-63)
*
* OUTPUTS:
*       logicalPortMappingTablePtr   - points to logical Port Mapping  entry
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on wrong devNum, logicalDevNum, logicalPortNum
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - on the memebers of virtual port entry struct out of range.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChLogicalTargetMappingTableEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    GT_STATUS                                  result;
    GT_U8                                      devNum;
    GT_U8                                      logicalDevNum;
    GT_U8                                      logicalPortNum;
    CPSS_DXCH_OUTPUT_INTERFACE_STC             logicalPortMappingTable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    logicalDevNum = (GT_U8)inArgs[1];
    logicalPortNum = (GT_U8)inArgs[2];

    cmdOsMemSet(&logicalPortMappingTable, 0,sizeof(CPSS_DXCH_OUTPUT_INTERFACE_STC));

    result = cpssDxChLogicalTargetMappingTableEntryGet(devNum,
                                                       logicalDevNum,
                                                       logicalPortNum,
                                                       &logicalPortMappingTable);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = logicalPortMappingTable.isTunnelStart;
    inFields[1] = logicalPortMappingTable.tunnelStartInfo.passengerPacketType;
    inFields[2] = logicalPortMappingTable.tunnelStartInfo.ptr;
    inFields[3] = logicalPortMappingTable.physicalInterface.type;
    CONVERT_BACK_DEV_PORT_DATA_MAC(logicalPortMappingTable.physicalInterface.devPort.devNum,
                                   logicalPortMappingTable.physicalInterface.devPort.portNum);
    inFields[4] = logicalPortMappingTable.physicalInterface.devPort.devNum;
    inFields[5] = logicalPortMappingTable.physicalInterface.devPort.portNum;
    CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(logicalPortMappingTable.physicalInterface.trunkId);
    inFields[6] = logicalPortMappingTable.physicalInterface.trunkId;
    inFields[7] = logicalPortMappingTable.physicalInterface.vidx;
    inFields[8] = 0;
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                    inFields[3], inFields[4], inFields[5],
                                    inFields[6], inFields[7], inFields[8] );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChDummyGetNext
*
* DESCRIPTION:
*   The function used to return "No Next Table Entry"
*
* COMMENTS: Generic
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChDummyGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    galtisOutput(outArgs, GT_OK, "%d", -1);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssDxChLogicalTargetMappingEnableSet",
        &wrCpssDxChLogicalTargetMappingEnableSet,
        2, 0},
    {"cpssDxChLogicalTargetMappingEnableGet",
        &wrCpssDxChLogicalTargetMappingEnableGet,
        1, 0},
    {"cpssDxChLogicalTargetMappingDeviceEnableSet",
        &wrCpssDxChLogicalTargetMappingDeviceEnableSet,
        3, 0},
    {"cpssDxChLogicalTargetMappingDeviceEnableGet",
        &wrCpssDxChLogicalTargetMappingDeviceEnableGet,
        2, 0},
    {"cpssDxChLogicalTargetMappingTableSet",
        &wrCpssDxChLogicalTargetMappingTableEntrySet,
        3, 9},

    {"cpssDxChLogicalTargetMappingTableGetFirst",
        &wrCpssDxChLogicalTargetMappingTableEntryGetFirst,
        3, 0},

    {"cpssDxChLogicalTargetMappingTableGetNext",
        &wrCpssDxChDummyGetNext,
        1, 0}
};


#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))



/*******************************************************************************
* cmdLibInitCpssDxChLogicalTargetMapping
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
GT_STATUS cmdLibInitCpssDxChLogicalTargetMapping
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

