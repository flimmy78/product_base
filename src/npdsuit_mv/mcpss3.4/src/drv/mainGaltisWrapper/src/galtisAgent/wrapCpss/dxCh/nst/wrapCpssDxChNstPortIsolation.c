
/*******************************************************************************
*              Copyright 2001, GALILEO TECHNOLOGY, LTD.
*
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL. NO RIGHTS ARE GRANTED
* HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT OF MARVELL OR ANY THIRD
* PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE DISCRETION TO REQUEST THAT THIS
* CODE BE IMMEDIATELY RETURNED TO MARVELL. THIS CODE IS PROVIDED "AS IS".
* MARVELL MAKES NO WARRANTIES, EXPRESS, IMPLIED OR OTHERWISE, REGARDING ITS
* ACCURACY, COMPLETENESS OR PERFORMANCE. MARVELL COMPRISES MARVELL TECHNOLOGY
* GROUP LTD. (MTGL) AND ITS SUBSIDIARIES, MARVELL INTERNATIONAL LTD. (MIL),
* MARVELL TECHNOLOGY, INC. (MTI), MARVELL SEMICONDUCTOR, INC. (MSI), MARVELL
* ASIA PTE LTD. (MAPL), MARVELL JAPAN K.K. (MJKK), GALILEO TECHNOLOGY LTD. (GTL)
* AND GALILEO TECHNOLOGY, INC. (GTI).
********************************************************************************
* wrapCpssDxChNstPortIsolation.c
*
* DESCRIPTION:
*       Wrapper functions for NST Port Isolation feature functions
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
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/nst/cpssDxChNstPortIsolation.h>

/*******************************************************************************
* cpssDxChNstPortIsolationEnableSet
*
* DESCRIPTION:
*       Function enables/disables the port isolation feature.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*       enable    - port isolation feature enable/disable
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChNstPortIsolationEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationEnableGet
*
* DESCRIPTION:
*       Function gets enabled/disabled state of the port isolation feature.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       enablePtr - (pointer to) port isolation feature state
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChNstPortIsolationEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntrySet
*
* DESCRIPTION:
*       Function sets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking. If it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interface.
*                              Only portDev and Trunk are supported.
*       cpuPortMember        - port isolation for CPU Port
*                               GT_TRUE - member
*                               GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written to the
*                              L2/L3 PI table
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface or
*                                  localPortsMembersPtr
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationTableEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_BOOL                                        cpuPortMember;
    CPSS_PORTS_BMP_STC                             localPortsMembers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trafficType = (CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)inArgs[1];
    cmdOsMemSet(&srcInterface, 0, sizeof(srcInterface));
    srcInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (inArgs[2])
    {
    case 0:
        srcInterface.devPort.devNum = (GT_U8)inArgs[3];
        srcInterface.devPort.portNum = (GT_U8)inArgs[4];
        CONVERT_DEV_PORT_DATA_MAC(srcInterface.devPort.devNum ,
                             srcInterface.devPort.portNum);
        break;

    case 1:
        srcInterface.trunkId = (GT_TRUNK_ID)inArgs[5];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);
        break;

    default:
        break;
    }

    cpuPortMember = (GT_BOOL)inArgs[6];
    localPortsMembers.ports[0] = (GT_U32)inArgs[7];
    localPortsMembers.ports[1] = (GT_U32)inArgs[8];
    CONVERT_DEV_PORTS_BMP_MAC(devNum,localPortsMembers);

    /* call cpss api function */
    result = cpssDxChNstPortIsolationTableEntrySet(devNum,
                                                   trafficType,
                                                   srcInterface,
                                                   cpuPortMember,
                                                   &localPortsMembers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationTableEntryGet
*
* DESCRIPTION:
*       Function gets port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Each entry holds bitmap of all local device ports (and CPU port), where
*       for each local port there is a bit marking if it's a member of source
*       interface (if outgoing traffic from this source interface is allowed to
*       go out at this specific local port).
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum               - device number
*       trafficType          - packets traffic type - L2 or L3
*       srcInterface         - table index is calculated from source interfaces
*                              Only portDev and Trunk are supported.
*
* OUTPUTS:
*       cpuPortMemberPtr     - (pointer to) port isolation for CPU Port
*                                GT_TRUE - member
*                                GT_FALSE - not member
*       localPortsMembersPtr - (pointer to) port bitmap to be written
*                              to the L2/L3 PI table
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Traffic is separated for L2 and L3 (means that application may
*       block L2 traffic while allowing L3).
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationTableEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_BOOL                                        cpuPortMember;
    CPSS_PORTS_BMP_STC                             localPortsMembers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trafficType = (CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)inArgs[1];
    cmdOsMemSet(&srcInterface, 0, sizeof(srcInterface));
    srcInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (inArgs[2])
    {
    case 0:
        srcInterface.devPort.devNum = (GT_U8)inArgs[3];
        srcInterface.devPort.portNum = (GT_U8)inArgs[4];
        CONVERT_DEV_PORT_DATA_MAC(srcInterface.devPort.devNum ,
                             srcInterface.devPort.portNum);
        break;

    case 1:
        srcInterface.trunkId = (GT_TRUNK_ID)inArgs[5];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);
        break;

    default:
        break;
    }

    /* call cpss api function */
    result = cpssDxChNstPortIsolationTableEntryGet(devNum,
                                                   trafficType,
                                                   srcInterface,
                                                   &cpuPortMember,
                                                   &localPortsMembers);

    CONVERT_BACK_DEV_PORTS_BMP_MAC(devNum,localPortsMembers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", cpuPortMember,
                                            localPortsMembers.ports[0],
                                            localPortsMembers.ports[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationPortAdd
*
* DESCRIPTION:
*       Function adds single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Adding local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port isn't blocked.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be added to bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationPortAdd
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_U8                                          portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trafficType = (CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)inArgs[1];
    cmdOsMemSet(&srcInterface, 0, sizeof(srcInterface));
    srcInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (inArgs[2])
    {
    case 0:
        srcInterface.devPort.devNum = (GT_U8)inArgs[3];
        srcInterface.devPort.portNum = (GT_U8)inArgs[4];
        CONVERT_DEV_PORT_DATA_MAC(srcInterface.devPort.devNum ,
                             srcInterface.devPort.portNum);
        break;

    case 1:
        srcInterface.trunkId = (GT_TRUNK_ID)inArgs[5];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);
        break;

    default:
        break;
    }

    portNum = (GT_U8)inArgs[6];
    CONVERT_DEV_PORT_MAC(devNum,portNum);

    /* call cpss api function */
    result = cpssDxChNstPortIsolationPortAdd(devNum,
                                             trafficType,
                                             srcInterface,
                                             portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNstPortIsolationPortDelete
*
* DESCRIPTION:
*       Function deletes single local port to port isolation table entry.
*       Each entry represent single source port/device or trunk.
*       Deleting local port (may be also CPU port 63) to port isolation entry
*       means that traffic which came from srcInterface and wish to egress
*       at the specified local port is blocked.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum        - device number
*       trafficType   - packets traffic type - L2 or L3
*       srcInterface  - table index is calculated from source interface
*                       Only portDev and Trunk are supported.
*       portNum       - local port(include CPU port) to be deleted from bitmap
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - wrong devNum, srcInterface, portNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In srcInterface parameter only portDev and Trunk are supported.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNstPortIsolationPortDelete
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT  trafficType;
    CPSS_INTERFACE_INFO_STC                        srcInterface;
    GT_U8                                          portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    trafficType = (CPSS_DXCH_NST_PORT_ISOLATION_TRAFFIC_TYPE_ENT)inArgs[1];
    cmdOsMemSet(&srcInterface, 0, sizeof(srcInterface));
    srcInterface.type = (CPSS_INTERFACE_TYPE_ENT)inArgs[2];

    switch (inArgs[2])
    {
    case 0:
        srcInterface.devPort.devNum = (GT_U8)inArgs[3];
        srcInterface.devPort.portNum = (GT_U8)inArgs[4];
        CONVERT_DEV_PORT_DATA_MAC(srcInterface.devPort.devNum ,
                             srcInterface.devPort.portNum);
        break;

    case 1:
        srcInterface.trunkId = (GT_TRUNK_ID)inArgs[5];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(srcInterface.trunkId);
        break;

    default:
        break;
    }

    portNum = (GT_U8)inArgs[6];
    CONVERT_DEV_PORT_MAC(devNum,portNum);

    /* call cpss api function */
    result = cpssDxChNstPortIsolationPortDelete(devNum,
                                                trafficType,
                                                srcInterface,
                                                portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChNstPortIsolationEnableSet",
        &wrCpssDxChNstPortIsolationEnableSet,
        2, 0},

    {"cpssDxChNstPortIsolationEnableGet",
        &wrCpssDxChNstPortIsolationEnableGet,
        1, 0},

    {"cpssDxChNstPortIsolationTableEntrySet",
        &wrCpssDxChNstPortIsolationTableEntrySet,
        9, 0},

    {"cpssDxChNstPortIsolationTableEntryGet",
        &wrCpssDxChNstPortIsolationTableEntryGet,
        6, 0},

    {"cpssDxChNstPortIsolationPortAdd",
        &wrCpssDxChNstPortIsolationPortAdd,
        7, 0},

    {"cpssDxChNstPortIsolationPortDelete",
        &wrCpssDxChNstPortIsolationPortDelete,
        7, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChNstPortIsolation
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
GT_STATUS cmdLibInitCpssDxChNstPortIsolation
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


