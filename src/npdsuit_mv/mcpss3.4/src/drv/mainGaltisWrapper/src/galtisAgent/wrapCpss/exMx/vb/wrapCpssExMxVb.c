/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxVb.c
*
* DESCRIPTION:
*       Wrapper functions for vb cpss functions
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*******************************************************************************/


/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMx/exMxGen/vb/cpssExMxVb.h>

/*******************************************************************************
* cpssExMxVbInit
*
* DESCRIPTION:
*       Function Relevant mode : All modes
*
*       initialize the VB library for the device.
*       And set the VB mode used.
*
*       APPLICABLE DEVICES:  98EX1x6
*
* INPUTS:
*       devNum     - device number.
*       vbMode     - cascade group mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong vbMode
*       GT_NOT_SUPPORTED - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbInit

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    CPSS_EXMX_VB_MODE_ENT   vbMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vbMode = (CPSS_EXMX_VB_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxVbInit(devNum, vbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/******************************************************************************
* Table: cpssExMxVbConfig
*
* Description:
*     set the VB configuration for all virtual ports to the network ports in
*     the mux devices.
*
* Fields:
*    muxDevNum - SW mux device number
*    muxHwDevNum - HW mux device number -- used only when 98EX136 in 4XG mode
*    mirrorCscdConnectionIndex - index in the cscdConnectionsPtr that will be
*                                used for mirrored packets from the mux device
*                                to the ExMx device this value must be less
*                                then the value of numOfCscdConnections
*
*         NOTE :
*              1. this parameter relevant only for devices with more then 1
*                 port (i.e this parameter not relevant for 98Ex136)
*              2. the Application need to set the ExMx cascade port defined by
*                 cscdConnectionsPtr[mirrorCscdConnectionIndex].exMxCscdPort
*                 to receive the mirrored packets from the mux device connected
*                 to it.
*              3. this parameter used as default choice only if there is no
*                 info a bout mux port that apply to:
*        CPSS_EXMX_VB_VIRTUAL_PORT_TO_MUX_PORT_MAP_STC::networkMuxPortNum != 31
*        when working with less then 4 groups.
*                  or
*        CPSS_EXMX_VB_VIRTUAL_PORT_TO_MUX_PORT_MAP_STC::networkMuxPortNum != 15
*        when working with less then 8 groups.
*
*    numOfCscdConnections - number of cascade connections (ports)
*    exMxCscdPort      - physical ExMx cascade port
*    numOfVirtualPorts - number of virtual ports
*    networkMuxPortNum - physical port num in the mux device
*                      NOTE:
*                          if working in less then 4 groups value of
*                          networkMuxPortNum = 31 can be used to set the
*                          Analyzer port info.
*                          if working in less then 8 groups value of
*                          networkMuxPortNum = 15 can be used to set the
*                          Analyzer port info.
*                          if application not use those special ports , then
*                          the CPSS will set the relevant mirror virtual port
*                          according to description of field:
*                   CPSS_EXMX_VB_CSCD_GROUP_INFO_STC::mirrorCscdConnectionIndex
*
*
*    virtualPort - virtual port num in ExMx device
*
*
*
* Comments:
*
*       None.
*
*
*/

/* table cpssExMxVbConfig global variables */

static GT_U8                                devNum, i, j;
static GT_U32                               numOfGroups;
static GT_U32                               groupsCount;
static CPSS_EXMX_VB_CSCD_GROUP_INFO_STC     *cscdGroupsArray = NULL;


/*******************************************************************************
* cpssExMxVbConfigSetFirst
*
* DESCRIPTION:
*   The function set the VB configuration
*
*
* INPUTS:
*       devNum                - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number
*       GT_OUT_OF_RANGE - on cscdGroupNum < 1 or cscdGroupNum > 8
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_NOT_SUPPORTED - on not applicable device
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbConfigSetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    {
        if(numFields < 8)
            return CMD_FIELD_UNDERFLOW;
        if(numFields > 8)
            return CMD_FIELD_OVERFLOW;

        if (cscdGroupsArray != NULL)
        {
            cmdOsFree(cscdGroupsArray);
            cscdGroupsArray = NULL;
        }

        cscdGroupsArray = cmdOsMalloc(sizeof(CPSS_EXMX_VB_CSCD_GROUP_INFO_STC));

        if (cscdGroupsArray == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }

        groupsCount = 0;

        cscdGroupsArray[groupsCount].muxDevNum = (GT_U8)inFields[0];
        cscdGroupsArray[groupsCount].muxHwDevNum = (GT_U8)inFields[1];
        cscdGroupsArray[groupsCount].mirrorCscdConnectionIndex =
                                                            (GT_U32)inFields[2];
        cscdGroupsArray[groupsCount].numOfCscdConnections =
                                                            (GT_U32)inFields[3];

        cscdGroupsArray[groupsCount].cscdConnectionsPtr =
                             cmdOsMalloc(sizeof(CPSS_EXMX_VB_CSCD_CONNECTION_STC));

        if (cscdGroupsArray[groupsCount].cscdConnectionsPtr == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }

        for(i = 0;i < inFields[3];i++)
        {
            cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].exMxCscdPort =
                                                             (GT_U8)inFields[4];
            cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].numOfVirtualPorts
                                                          = (GT_U32)inFields[5];

            cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].virtPortsPtr =
                cmdOsMalloc(sizeof(CPSS_EXMX_VB_VIRTUAL_PORT_TO_MUX_PORT_MAP_STC));

            if (cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].
                                                           virtPortsPtr == NULL)
            {
                galtisOutput(outArgs, GT_NO_RESOURCE, "");
                return CMD_OK;
            }

            for(j = 0;j < inFields[5];j++)
            {
                cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].
                         virtPortsPtr[j].networkMuxPortNum = (GT_U8)inFields[6];
                cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].
                               virtPortsPtr[j].virtualPort = (GT_U8)inFields[7];
            }
        }

        groupsCount++ ;
        galtisOutput(outArgs, GT_OK, "");
    }
    return CMD_OK;
}
/*******************************************************************************
* cpssExMxVbConfigSetNext
*
* DESCRIPTION:
*   The function set the VB configuration
*
*
* INPUTS:
*       devNum                - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number
*       GT_OUT_OF_RANGE - on cscdGroupNum < 1 or cscdGroupNum > 8
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_NOT_SUPPORTED - on not applicable device
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbConfigSetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    {
        if(numFields < 8)
            return CMD_FIELD_UNDERFLOW;
        if(numFields > 8)
            return CMD_FIELD_OVERFLOW;


        if (groupsCount == 0)
        {
            galtisOutput(outArgs, GT_FAIL, "");
            return CMD_OK;
        }

        if (cscdGroupsArray == NULL)
        {
            galtisOutput(outArgs, GT_FAIL, "");
            return CMD_OK;
        }

        cscdGroupsArray = cmdOsRealloc(cscdGroupsArray, (groupsCount + 1) *
                          sizeof(CPSS_EXMX_VB_CSCD_GROUP_INFO_STC));

        if (cscdGroupsArray == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }

        cscdGroupsArray[groupsCount].muxDevNum = (GT_U8)inFields[0];
        cscdGroupsArray[groupsCount].muxHwDevNum = (GT_U8)inFields[1];
        cscdGroupsArray[groupsCount].mirrorCscdConnectionIndex =
                                                            (GT_U32)inFields[2];
        cscdGroupsArray[groupsCount].numOfCscdConnections = (GT_U32)inFields[3];

        cscdGroupsArray[groupsCount].cscdConnectionsPtr =
                             cmdOsMalloc(sizeof(CPSS_EXMX_VB_CSCD_CONNECTION_STC));

        if (cscdGroupsArray[groupsCount].cscdConnectionsPtr == NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }

        for(i = 0;i < inFields[3];i++)
        {
            cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].exMxCscdPort =
                                                             (GT_U8)inFields[4];
            cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].numOfVirtualPorts
                                                          = (GT_U32)inFields[5];

            cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].virtPortsPtr =
                cmdOsMalloc(sizeof(CPSS_EXMX_VB_VIRTUAL_PORT_TO_MUX_PORT_MAP_STC));

            if (cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].
                                                           virtPortsPtr == NULL)
            {
                galtisOutput(outArgs, GT_NO_RESOURCE, "");
                return CMD_OK;
            }

            for(j = 0;j < inFields[5];j++)
            {
                cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].
                         virtPortsPtr[j].networkMuxPortNum = (GT_U8)inFields[6];
                cscdGroupsArray[groupsCount].cscdConnectionsPtr[i].
                               virtPortsPtr[j].virtualPort = (GT_U8)inFields[7];
            }
        }

        groupsCount++ ;
        galtisOutput(outArgs, GT_OK, "");
    }
    return CMD_OK;
}
/*******************************************************************************
* cpssExMxVbConfigEndSet
*
* DESCRIPTION:
*   The function set the VB configuration
*
*
* INPUTS:
*       devNum                - device number
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number
*       GT_OUT_OF_RANGE - on cscdGroupNum < 1 or cscdGroupNum > 8
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_NOT_SUPPORTED - on not applicable device
*       CMD_FIELD_UNDERFLOW - not enough field arguments.
*       CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbConfigEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    numOfGroups = groupsCount;
    {
        if (groupsCount == 0)
        {
            galtisOutput(outArgs, GT_OK, "");
            return CMD_OK;
        }
        if (cscdGroupsArray == NULL)
        {
            galtisOutput(outArgs, GT_FAIL, "");
            return CMD_OK;
        }

        status = cpssExMxVbConfigSet(devNum, numOfGroups, cscdGroupsArray);
        if (status != GT_OK)
        {
            galtisOutput(outArgs, status, "");
            return CMD_OK;
        }

        cmdOsFree(cscdGroupsArray);
        cscdGroupsArray = NULL;
        numOfGroups = 0;

        galtisOutput(outArgs, status, "");
    }
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxVbConfigCancelSet (table command)
*
* DESCRIPTION:
*     Cancel operations on cpssExMxVbConfig table
*
* INPUTS:
*     none
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number
*       GT_OUT_OF_RANGE - on cscdGroupNum < 1 or cscdGroupNum > 8
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_NOT_SUPPORTED - on not applicable device
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbConfigCancelSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsFree(cscdGroupsArray);
    cscdGroupsArray = NULL;
    numOfGroups = 0;

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxVb8TcTo4TcMapSet
*
* DESCRIPTION:
*       Configures traffic class conversion. Range 0-7 to 0-3.
*       The conversion is use by the device when packet send from the device to
*       cascade ports with DSA tag (the DSA tag hold only 2 bit for TC).
*
*       APPLICABLE DEVICES:  98EX1x6
*
* INPUTS:
*       devNum              - device number.
*       value8Tc            - TC value in range 0..7.
*       value4Tc            - TC value in range 0..3.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - on wrong device number or value8Tc > 7 or
*                         value4Tc > 3
*       GT_NOT_SUPPORTED - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVb8TcTo4TcMapSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    GT_U8                   value8Tc;
    GT_U8                   value4Tc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    value8Tc = (GT_U8)inArgs[1];
    value4Tc = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxVb8TcTo4TcMapSet(devNum, value8Tc, value4Tc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxVbCscdDsaEnableGet
*
* DESCRIPTION:
*       get value is "VB mode" enabled/disabled in device
*
*       APPLICABLE DEVICES:  98EX1x6
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr   - (pointer to) "VB mode" enabled/disabled
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_NOT_SUPPORTED - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbCscdDsaEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    GT_BOOL                 enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxVbCscdDsaEnableGet(devNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxVbGroupModeGet
*
* DESCRIPTION:
*       get the mode of cascade groups (using 4 cascade groups or 8 cascade
*       groups)
*
*       APPLICABLE DEVICES:  98EX1x6
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       groupModePtr  - (pointer to) cascade group mode
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_NOT_SUPPORTED - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbGroupModeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_EXMX_VB_GROUPS_MODE_ENT    groupModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxVbGroupModeGet(devNum, &groupModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", groupModePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxVbEgressVirtualPortsGet
*
* DESCRIPTION:
*       get the configuration of virtual ports for the Egress Pipe in PP.
*
*       APPLICABLE DEVICES:  98EX1x6
*
* INPUTS:
*       devNum             - device number.
*       vbMode             - cascade group mode
*       virtualPortNum     - virtual port number (0..63)
*                            values 0..51 refer to the virtual ports (
*                            in 98EX136 values 0..31)
*                            values 52..63 needed to be set with value of 52..63
*                            in parameter physicalPortNum
*       muxHwDevNum        - HW device number of the Mux device (Dx/Sx)
*                            this value should be 0 unless VB is 4XG mode, which
*                            in this case the "middle" Dx device has HW
*                            devNum = 3 and the other 2 Dx devices should use
*                            HW devNum = 0 and 8
*
* OUTPUTS:
*       physicalPortNumPtr    - (pointer to)physical port number (0..63)
*       portNumOnMuxDevicePtr - (pointer to)(physical) port number in the Mux
*                            device (Dx/Sx) (0..63)
*                       NOTE: the value in this parameter is not relevant when
*                             virtualPortNum in range 52..63
*       addDsaPtr  - (pointer to)set the DSA sending mode for the virtualPortNum
*                            GT_TRUE - add DSA tag - should be set when
*                                      virtualPortNum is virtual port
*                                      (non turbo port , non cpu port , non
*                                       cascade port)
*                            GT_FALSE - don't add DSA tag - should be set for
*                                      virtualPortNum is Turbo port or cascade
*                                      port
*                       NOTE: the value in this parameter is not relevant when
*                             virtualPortNum in range 52..63
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number or muxHwDevNum != 0,3,8 or wrong
*                      vbMode
*       GT_OUT_OF_RANGE - on virtualPortNum > 63
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_BAD_STATE - the registers relate to the feature are not synchronized
*                      (NOTE : values returned in OUT parameters are valid)
*       GT_NOT_SUPPORTED - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbEgressVirtualPortsGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_EXMX_VB_MODE_ENT           vbMode;
    GT_U8                           virtualPortNum;
    GT_U8                           physicalCscdPortNumPtr;
    GT_U8                           muxHwDevNum;
    GT_U8                           portNumOnMuxDevicePtr;
    GT_BOOL                         addDsaPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vbMode = (CPSS_EXMX_VB_MODE_ENT)inArgs[1];
    virtualPortNum = (GT_U8)inArgs[2];
    muxHwDevNum = (GT_U8)inArgs[3];

    /* call cpss api function */
    result = cpssExMxVbEgressVirtualPortsGet(devNum, vbMode, virtualPortNum,
                                       &physicalCscdPortNumPtr, muxHwDevNum,
                                         &portNumOnMuxDevicePtr, &addDsaPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", physicalCscdPortNumPtr,
                                  portNumOnMuxDevicePtr, addDsaPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet
*
* DESCRIPTION:
*       Get the mapping between {cascade group and mux port} to ExMx virtual
*       port devices. (used to map the "DSA port number" to virtual port number
*       at the ingress pipe in ExMx PP)
*
*       NOTE about "group 0" (when cscdGroupNum = 0):
*         group 0 is used for ExMx "Turbo ports"(network Ex port) and ExMx
*         cascade ports.
*         portNumOnMuxDevice will have meaning of "exMx Physical port"
*
*       APPLICABLE DEVICES:  98EX1x6
*
* INPUTS:
*       devNum             - device number.
*       groupMode          - the group mode (4/8 groups)
*       cscdGroupNum       - cascade group num (0..3/7)
*       portNumOnMuxDevice - (physical) port number in mux device(0..15/31)
*                            see comment about meaning when cscdGroupNum = 0
*
* OUTPUTS:
*       virtualPortNumPtr  - (pointer to) virtual port number in the ExMx device
*                            (0..63)
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong groupMode
*       GT_OUT_OF_RANGE - on cscdGroupNum > 3/7 or portNumOnMuxDevice > 15/31
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_NOT_SUPPORTED - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_EXMX_VB_GROUPS_MODE_ENT    groupMode;
    GT_U32                          cscdGroupNum;
    GT_U8                           portNumOnMuxDevice;
    GT_U8                           virtualPortNumPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    groupMode = (CPSS_EXMX_VB_GROUPS_MODE_ENT)inArgs[1];
    cscdGroupNum = (GT_U32)inArgs[2];
    portNumOnMuxDevice = (GT_U8)inArgs[3];

    /* call cpss api function */
    result = cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet(devNum,
     groupMode, cscdGroupNum, portNumOnMuxDevice, &virtualPortNumPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", virtualPortNumPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxVbPortToCscdGroupMapGet
*
* DESCRIPTION:
*       Get the mapping between physical ports to cascade groups.
*       (cascade ports of Ex device to cascade groups of Ex device)
*
*       APPLICABLE DEVICES:  98EX1x6
*
* INPUTS:
*       devNum              - device number.
*       physicalPortNum     - physical port number (0..11)
*
* OUTPUTS:
*       cscdGroupNumPtr     - (pointer to)cascade group num (0..15)
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_NOT_SUPPORTED - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbPortToCscdGroupMapGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U8                           physicalPortNum;
    GT_U32                          cscdGroupNumPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    physicalPortNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxVbPortToCscdGroupMapGet(devNum, physicalPortNum,
                                                    &cscdGroupNumPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cscdGroupNumPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxVbModeGet
*
* DESCRIPTION:
*       Get the VB mode used.(regular/4XG mode)
*
*       APPLICABLE DEVICES:  98EX1x6
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       vbModePtr  - (pointer to)cascade group mode
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number
*       GT_BAD_PTR   - on NULL pointer parameter
*       GT_NOT_SUPPORTED - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxVbModeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_EXMX_VB_MODE_ENT           vbModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxVbModeGet(devNum, &vbModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", vbModePtr);
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxVbInit",
        &wrCpssExMxVbInit,
        2, 0},

    {"cpssExMxVb8TcTo4TcMapSet",
        &wrCpssExMxVb8TcTo4TcMapSet,
        3, 0},

    {"cpssExMxVbCscdDsaEnableGet",
        &wrCpssExMxVbCscdDsaEnableGet,
        1, 0},

    {"cpssExMxVbGroupModeGet",
        &wrCpssExMxVbGroupModeGet,
        1, 0},

    {"cpssExMxVbEgressVirtualPortsGet",
        &wrCpssExMxVbEgressVirtualPortsGet,
        4, 0},

    {"cpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet",
        &wrCpssExMxVbCscdGroupAndMuxPortToVirtualPortMapGet,
        4, 0},

    {"cpssExMxVbPortToCscdGroupMapGet",
        &wrCpssExMxVbPortToCscdGroupMapGet,
        2, 0},

    {"cpssExMxVbModeGet",
        &wrCpssExMxVbModeGet,
        1, 0},

    /*  call for table cpssExMxVbConfig  */

    {"cpssExMxVbConfigSetFirst",
        &wrCpssExMxVbConfigSetFirst,
        1, 8},

    {"cpssExMxVbConfigSetNext",
        &wrCpssExMxVbConfigSetNext,
        1, 8},

    {"cpssExMxVbConfigEndSet",
        &wrCpssExMxVbConfigEndSet,
        1, 0},

    {"cpssExMxVbConfigCancelSet",
        &wrCpssExMxVbConfigCancelSet,
        0, 0},

    /*  end call for table cpssExMxVbConfig  */


};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxVb
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
GT_STATUS cmdLibInitCpssExMxVb
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


