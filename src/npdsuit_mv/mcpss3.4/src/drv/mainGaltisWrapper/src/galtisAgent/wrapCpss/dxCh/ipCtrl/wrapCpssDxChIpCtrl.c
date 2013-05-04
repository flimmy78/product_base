/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChIpCtrl.c
*
* DESCRIPTION:
*       Wrapper functions for IpCtrl cpss.dxCh functions
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
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpCtrl.h>
#include <cpss/dxCh/dxChxGen/ip/cpssDxChIpTypes.h>


/*******************************************************************************
* ipCtrlMultiPortGroupsBmpGet
*
* DESCRIPTION:
*       Get the portGroupsBmp for multi port groups device.
*       when 'enabled' --> wrappers will use the APIs
*       with portGroupsBmp parameter
*
* INPUTS:
*       devNum - device number
*
* OUTPUTS:
*       enablePtr - (pointer to)enable / disable the use of APIs with portGroupsBmp parameter.
*       portGroupsBmpPtr - (pointer to)port groups bmp , relevant only when enable = GT_TRUE
*
* RETURNS:
*       NONE
*
* COMMENTS:
*
*******************************************************************************/
static void ipCtrlMultiPortGroupsBmpGet
(
    IN   GT_U8               devNum,
    OUT  GT_BOOL             *enablePtr,
    OUT  GT_PORT_GROUPS_BMP  *portGroupsBmpPtr
)
{
    /* default */
    *enablePtr  = GT_FALSE;
    *portGroupsBmpPtr = CPSS_PORT_GROUP_UNAWARE_MODE_CNS;

    if (0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
    {
        return;
    }

    utilMultiPortGroupsBmpGet(devNum, enablePtr, portGroupsBmpPtr);
}


/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpPortGroupCntSet
(
    IN  GT_U8                        devNum,
    IN CPSS_IP_CNT_SET_ENT           cntSet,
    IN CPSS_DXCH_IP_COUNTER_SET_STC  *countersPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpCntSet(devNum, cntSet, countersPtr);
    }
    else
    {
        return cpssDxChIpPortGroupCntSet(devNum, pgBmp, cntSet, countersPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpPortGroupCntGet
(
    IN  GT_U8                         devNum,
    IN  CPSS_IP_CNT_SET_ENT           cntSet,
    OUT CPSS_DXCH_IP_COUNTER_SET_STC  *countersPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpCntGet(devNum, cntSet, countersPtr);
    }
    else
    {
        return cpssDxChIpPortGroupCntGet(devNum, pgBmp, cntSet, countersPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpDropCntGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *dropPktsPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpDropCntGet(devNum, dropPktsPtr);
    }
    else
    {
        return cpssDxChIpPortGroupDropCntGet(devNum, pgBmp, dropPktsPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpDropCntSet
(
    IN  GT_U8     devNum,
    IN GT_U32     dropPkts
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpDropCntSet(devNum, dropPkts);
    }
    else
    {
        return cpssDxChIpPortGroupDropCntSet(devNum, pgBmp, dropPkts);
    }
}


/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpRouterBridgedPacketsExceptionCntGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *routerBridgedExceptionPktsPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpRouterBridgedPacketsExceptionCntGet(devNum,
                                                             routerBridgedExceptionPktsPtr);
    }
    else
    {
        return cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntGet(devNum,
                                                                      pgBmp,
                                                                      routerBridgedExceptionPktsPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpRouterBridgedPacketsExceptionCntSet
(
    IN  GT_U8     devNum,
    IN GT_U32     routerBridgedExceptionPkts
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpRouterBridgedPacketsExceptionCntSet(devNum,
                                                             routerBridgedExceptionPkts);
    }
    else
    {
        return cpssDxChIpPortGroupRouterBridgedPacketsExceptionCntSet(devNum,
                                                                      pgBmp,
                                                                      routerBridgedExceptionPkts);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet
(
    IN  GT_U8     devNum,
    OUT GT_U32    *dropPktsPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMultiTargetQueueFullDropCntGet(devNum, dropPktsPtr);
    }
    else
    {
        return cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet(devNum, pgBmp,
                                                                 dropPktsPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpMultiTargetQueueFullDropCntSet
(
    IN  GT_U8    devNum,
    IN GT_U32    dropPkts
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMultiTargetQueueFullDropCntSet(devNum, dropPkts);
    }
    else
    {
        return cpssDxChIpPortGroupMultiTargetQueueFullDropCntSet(devNum, pgBmp,
                                                                 dropPkts);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpMllCntGet
(
    IN  GT_U8     devNum,
    IN GT_U32     mllCntSet,
    OUT GT_U32    *mllOutMCPktsPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMllCntGet(devNum, mllCntSet, mllOutMCPktsPtr);
    }
    else
    {
        return cpssDxChIpPortGroupMllCntGet(devNum, pgBmp, mllCntSet,
                                            mllOutMCPktsPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpPortGroupMllCntSet
(
    IN  GT_U8     devNum,
    IN GT_U32     mllCntSet,
    IN GT_U32     mllOutMCPkts
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMllCntSet(devNum, mllCntSet, mllOutMCPkts);
    }
    else
    {
        return cpssDxChIpPortGroupMllCntSet(devNum, pgBmp, mllCntSet,
                                            mllOutMCPkts);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChIpMultiTargetRateShaperSet
(
    IN   GT_U8      devNum,
    IN   GT_BOOL    multiTargetRateShaperEnable,
    IN   GT_U32     windowSize
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    ipCtrlMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChIpMultiTargetRateShaperSet(devNum,
                                                  multiTargetRateShaperEnable,
                                                  windowSize);
    }
    else
    {
        return cpssDxChIpPortGroupMultiTargetRateShaperSet(devNum, pgBmp,
                                                    multiTargetRateShaperEnable,
                                                    windowSize);
    }
}


/*******************************************************************************
* cpssDxChIpSpecialRouterTriggerEnable
*
* DESCRIPTION:
*       Sets the special router trigger enable modes for packets with bridge
*       command other then FORWARD or MIRROR.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum              - physical device number
*       bridgeExceptionCmd  - the bridge exception command the packet arrived to
*                             the router with.
*       enableRouterTrigger - enable /disable router trigger.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpSpecialRouterTriggerEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            devNum;
    CPSS_DXCH_IP_BRG_EXCP_CMD_ENT    bridgeExceptionCmd;
    GT_BOOL                          enableRouterTrigger;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    bridgeExceptionCmd = (CPSS_DXCH_IP_BRG_EXCP_CMD_ENT)inArgs[1];
    enableRouterTrigger = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpSpecialRouterTriggerEnable(devNum, bridgeExceptionCmd,
                                                         enableRouterTrigger);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpExceptionCommandSet
*
* DESCRIPTION:
*       set a specific excption commnad.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum        - physical device number
*       exceptionType  - the exception type
*       protocolStack - weather to set for ipv4/v6 or both
*       command       - the command, for availble commands see
*                       CPSS_DXCH_IP_EXCEPTION_TYPE_ENT.
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
* GalTis:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpExceptionCommandSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                        result;

    GT_U8                            devNum;
    CPSS_DXCH_IP_EXCEPTION_TYPE_ENT  exceptionType;
    CPSS_IP_PROTOCOL_STACK_ENT       protocolStack;
    CPSS_PACKET_CMD_ENT              command;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    exceptionType = (CPSS_DXCH_IP_EXCEPTION_TYPE_ENT)inArgs[1];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[2];
    command = (CPSS_PACKET_CMD_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpExceptionCommandSet(devNum, exceptionType,
                                          protocolStack, command);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpUcRouteAgingModeSet
*
* DESCRIPTION:
*      Sets the global route aging modes.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum           - physical device number
*       refreshEnable - Enables the global routing activity refresh mechanism
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRouteAgingModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS            result;

    GT_U8                devNum;
    GT_BOOL              refreshEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    refreshEnable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpUcRouteAgingModeSet(devNum, refreshEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterSourceIdSet
*
* DESCRIPTION:
*      set the router source id assignmnet.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum        - physical device number
*       ucMcSet       - weather to set it for unicast packets or multicast.
*       sourceId      - the assigned source id.
*
* OUTPUTS:
*      none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterSourceIdSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    CPSS_IP_UNICAST_MULTICAST_ENT   ucMcSet;
    GT_U32                          sourceId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ucMcSet = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[1];
    sourceId = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpRouterSourceIdSet(devNum, ucMcSet, sourceId);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet
*
* DESCRIPTION:
*      Sets the multi-target TC queue assigned to multi-target Control
*      packets and to RPF Fail packets where the RPF Fail Command is assigned
*      from the MLL entry.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum                    - physical device number
*       ctrlMultiTargetTCQueue    - The multi-target TC queue for control
*                                   traffic. There are 4 possible queues.
*       failRpfMultiTargetTCQueue - the multi-target TC queue for fail rpf
*                                   traffic. There are 4 possible queues.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       A packet is considered as a multi-target control if it is a FROM_CPU DSA
*       Tagged with DSA<Use_Vidx> = 1 or a multi-target packet that is also to
*       be mirrored to the CPU
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          ctrlMultiTargetTCQueue;
    GT_U32                          failRpfMultiTargetTCQueue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ctrlMultiTargetTCQueue = (GT_U32)inArgs[1];
    failRpfMultiTargetTCQueue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet(devNum,
                                               ctrlMultiTargetTCQueue,
                                            failRpfMultiTargetTCQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpQosProfileToMultiTargetTCQueueMapSet
*
* DESCRIPTION:
*     Sets the Qos Profile to multi-target TC queue mapping.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum             - physical device number
*       qosProfile         - QOS Profile index
*       multiTargeTCQueue  - the multi-target TC queue. There are 4 possible
*                            queues.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpQosProfileToMultiTargetTCQueueMapSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          qosProfile;
    GT_U32                          multiTargeTCQueue;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    qosProfile = (GT_U32)inArgs[1];
    multiTargeTCQueue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpQosProfileToMultiTargetTCQueueMapSet(devNum, qosProfile,
                                                             multiTargeTCQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetQueueFullDropCntGet
*
* DESCRIPTION:
*      Get the multi target queue full drop packet counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       dropPktsPtr - the number of counted dropped packets.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetQueueFullDropCntGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          dropPktsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupMultiTargetQueueFullDropCntGet(devNum,
                                                                  &dropPktsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropPktsPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetQueueFullDropCntSet
*
* DESCRIPTION:
*      set the multi target queue full drop packet counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum      - physical device number
*       dropPkts    - the counter value to set.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetQueueFullDropCntSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U32                          dropPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropPkts = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpMultiTargetQueueFullDropCntSet(devNum, dropPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetTCQueueSchedModeSet
*
* DESCRIPTION:
*      sets the multi-target TC queue scheduling mode.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum             - physical device number
*       multiTargetTcQueue - the multi-target TC queue. There are 4 possible
*                            queues.
*       schedulingMode     - the scheduling mode.
*       queueWeight        - the queue weight for SDWRR scheduler
*                           (relvant only if schedMode =
*                            CPSS_DXCH_IP_MT_TC_QUEUE_SDWRR_SCHED_MODE_E).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetTCQueueSchedModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      multiTargetTcQueue;
    CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT     schedulingMode;
    GT_U32                                      queueWeight;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    multiTargetTcQueue = (GT_U32)inArgs[1];
    schedulingMode = (CPSS_DXCH_IP_MT_TC_QUEUE_SCHED_MODE_ENT)inArgs[2];
    queueWeight = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpMultiTargetTCQueueSchedModeSet(devNum,
                                          multiTargetTcQueue,
                                              schedulingMode,
                                                 queueWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpBridgeServiceEnable
*
* DESCRIPTION:
*      enable/disable a router bridge service.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number
*       brgService      - the router bridge service
*       enableDisableMode - Enable/Disable mode of this function (weather
*                           to enable/disable for ipv4/ipv6/arp)
*       enableService   - weather to enable the service for the above more.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpBridgeServiceEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                               result;

    GT_U8                                                   devNum;
    CPSS_DXCH_IP_BRG_SERVICE_ENT                            bridgeService;
    CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT        enableDisableMode;
    GT_BOOL                                                 enableService;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    bridgeService = (CPSS_DXCH_IP_BRG_SERVICE_ENT)inArgs[1];
    enableDisableMode =
                    (CPSS_DXCH_IP_BRG_SERVICE_ENABLE_DISABLE_MODE_ENT)inArgs[2];
    enableService = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    result = cpssDxChIpBridgeServiceEnable(devNum, bridgeService,
                                enableDisableMode, enableService);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllBridgeEnable
*
* DESCRIPTION:
*      enable/disable MLL based bridging.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number
*       mllBridgeEnable - enable /disable MLL based bridging.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllBridgeEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                result;

    GT_U8                    devNum;
    GT_BOOL                  mllBridgeEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mllBridgeEnable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpMllBridgeEnable(devNum, mllBridgeEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetRateShaperSet
*
* DESCRIPTION:
*      set the Multi target Rate shaper params.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum     - physical device number
*       multiTargetRateShaperEnable - weather to enable (disable) the Multi
*                                     target Rate shaper
*       windowSize - if enabled then this is the shaper window size.
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetRateShaperSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_BOOL                                     multiTargetRateShaperEnable;
    GT_U32                                      windowSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    multiTargetRateShaperEnable = (GT_BOOL)inArgs[1];
    windowSize = (GT_U32)inArgs[2];

    /* call port group api function */
    result = pg_wrap_cpssDxChIpMultiTargetRateShaperSet(devNum,
                                                    multiTargetRateShaperEnable,
                                                    windowSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMultiTargetUcSchedModeSet
*
* DESCRIPTION:
*      set the Multi target/unicast sheduler mode.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum    - physical device number
*       ucSPEnable- weather the Unicast uses SP , if GT_FALSE then it means
*                   both the Unicast and multi target uses DSWRR scheduling
*       ucWeight  - if DSWRR scheduler is used (ucSPEnable == GT_FALSE) the this
*                   is the unicast weight.
*       mcWeight  - if DSWRR scheduler is used (ucSPEnable == GT_FALSE) the this
*                   is the multi target weight.
*       schedMtu  - The MTU used by the scheduler
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMultiTargetUcSchedModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_BOOL                                     ucSPEnable;
    GT_U32                                      ucWeight;
    GT_U32                                      mcWeight;
    CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT            schedMtu;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ucSPEnable = (GT_BOOL)inArgs[1];
    ucWeight = (GT_U32)inArgs[2];
    mcWeight = (GT_U32)inArgs[3];
    schedMtu = (CPSS_DXCH_IP_MT_UC_SCHED_MTU_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssDxChIpMultiTargetUcSchedModeSet(devNum, ucSPEnable, ucWeight,
                                                           mcWeight, schedMtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpArpBcModeSet
*
* DESCRIPTION:
*       set a arp broadcast mode.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum       - physical device number
*       arpBcMode    - the arp broadcast command. Possible Commands:
*                      CPSS_PACKET_CMD_NONE_E,CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                      CPSS_PACKET_CMD_MIRROR_TO_CPU_E
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       none.
* GalTis:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpArpBcModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_PACKET_CMD_ENT                         arpBcMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    arpBcMode = (CPSS_PACKET_CMD_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpArpBcModeSet(devNum, arpBcMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpPortRoutingEnable
*
* DESCRIPTION:
*       Enable multicast/unicast IPv4/v6 routing on a port.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum        - physical device number
*       portNum       - the port to enable on
*       ucMcEnable    - routing type to enable Unicast/Multicast
*       protocolStack - what type of traffic to enable ipv4 or ipv6 or both.
*       enableRouting - enable IP routing for this port
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortRoutingEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;

    GT_U8                                           devNum;
    GT_U8                                           portNum;
    CPSS_IP_UNICAST_MULTICAST_ENT                   ucMcEnable;
    CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack;
    GT_BOOL                                         enableRouting;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ucMcEnable = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[2];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[3];
    enableRouting = (GT_BOOL)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpPortRoutingEnable(devNum, portNum, ucMcEnable,
                                        protocolStack, enableRouting);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpPortRoutingEnableGet
*
* DESCRIPTION:
*       Get status of multicast/unicast IPv4/v6 routing on a port.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum        - the device number
*       portNum       - the port to enable on
*       ucMcEnable    - routing type to enable Unicast/Multicast
*       protocolStack - what type of traffic to enable ipv4 or ipv6 or both.
*
* OUTPUTS:
*       enableRoutingPtr - (pointer to)enable IP routing for this port
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - wrong protocolStack
*
* COMMENTS:
*       None
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortRoutingEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;

    GT_U8                                           devNum;
    GT_U8                                           portNum;
    CPSS_IP_UNICAST_MULTICAST_ENT                   ucMcEnable;
    CPSS_IP_PROTOCOL_STACK_ENT                      protocolStack;
    GT_BOOL                                         enableRouting;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ucMcEnable = (CPSS_IP_UNICAST_MULTICAST_ENT)inArgs[2];
    protocolStack = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpPortRoutingEnableGet(devNum, portNum, ucMcEnable,
                                        protocolStack, &enableRouting);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enableRouting);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpQosProfileToRouteEntryMapSet
*
* DESCRIPTION:
*       Sets the QoS profile to route entry offset mapping table.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum           - physical device number
*       qosProfile       - QOS Profile index
*       routeEntryOffset - The offset in the array of route entries to be
*                          selected for this QOS profile
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       none
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpQosProfileToRouteEntryMapSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      qosProfile;
    GT_U32                                      routeEntryOffset;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    qosProfile = (GT_U32)inArgs[1];
    routeEntryOffset = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpQosProfileToRouteEntryMapSet(devNum, qosProfile,
                                                      routeEntryOffset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRoutingEnable
*
* DESCRIPTION:
*       globally enable/disable routing.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum        - the device number
*       enableRouting - enable /disable global routing
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - wrong devNum
*
* COMMENTS:
*       the ASIC defualt is routing enabled.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRoutingEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    GT_BOOL                 enableRouting;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enableRouting = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRoutingEnable(devNum, enableRouting);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRoutingEnableGet
*
* DESCRIPTION:
*       Get global routing status
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum              - the device number
*
* OUTPUTS:
*       enableRoutingPtr    -  (pointer to)enable /disable global routing
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRoutingEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;

    GT_U8                   devNum;
    GT_BOOL                 enableRouting;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpRoutingEnableGet(devNum, &enableRouting);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enableRouting);
    return CMD_OK;
}


/*  table cpssDxChIpCnt global variable   */

static CPSS_IP_CNT_SET_ENT    cntSetCnt;

/*
* cpssDxChIpCntGet
*
* DESCRIPTION:
*       Return the IP counter set requested.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       cntSet - counters set to retrieve.
*
* OUTPUTS:
*       counters - struct contains the counter values.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_NOT_INITIALIZED - The library was not initialized.
*
* COMMENTS:
*       This function doesn't handle counters overflow.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DXCH_IP_COUNTER_SET_STC   counters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntSetCnt = 0;

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupCntGet(devNum, cntSetCnt, &counters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = cntSetCnt;
    inFields[1] = counters.inUcPkts;
    inFields[2] = counters.inMcPkts;
    inFields[3] = counters.inUcNonRoutedExcpPkts;
    inFields[4] = counters.inUcNonRoutedNonExcpPkts;
    inFields[5] = counters.inMcNonRoutedExcpPkts;
    inFields[6] = counters.inMcNonRoutedNonExcpPkts;
    inFields[7] = counters.inUcTrappedMirrorPkts;
    inFields[8] = counters.inMcTrappedMirrorPkts;
    inFields[9] = counters.mcRfpFailPkts;
    inFields[10] = counters.outUcRoutedPkts;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],
                  inFields[2], inFields[3], inFields[4],  inFields[5],
                  inFields[6], inFields[7], inFields[8],  inFields[9],
                  inFields[10]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    cntSetCnt++;

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                      result;

    GT_U8                          devNum;
    CPSS_DXCH_IP_COUNTER_SET_STC   counters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (cntSetCnt > 3)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupCntGet(devNum, cntSetCnt, &counters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = cntSetCnt;
    inFields[1] = counters.inUcPkts;
    inFields[2] = counters.inMcPkts;
    inFields[3] = counters.inUcNonRoutedExcpPkts;
    inFields[4] = counters.inUcNonRoutedNonExcpPkts;
    inFields[5] = counters.inMcNonRoutedExcpPkts;
    inFields[6] = counters.inMcNonRoutedNonExcpPkts;
    inFields[7] = counters.inUcTrappedMirrorPkts;
    inFields[8] = counters.inMcTrappedMirrorPkts;
    inFields[9] = counters.mcRfpFailPkts;
    inFields[10] = counters.outUcRoutedPkts;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],
                  inFields[2], inFields[3], inFields[4],  inFields[5],
                  inFields[6], inFields[7], inFields[8],  inFields[9],
                  inFields[10]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    cntSetCnt++;

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpCntSetModeSet
*
* DESCRIPTION:
*      Sets a counter set's bounded inteface and interface mode.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum        - the device number
*       cntSet        - the counter set
*       cntSetMode    - the counter set bind mode (interface or Next hop)
*       interfaceModeCfgPtr - if cntSetMode = CPSS_DXCH_IP_CNT_SET_INTERFACE_MODE_E
*                          this is the interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntSetModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                     result;

    GT_U8                                         devNum;
    CPSS_IP_CNT_SET_ENT                           cntSet;
    CPSS_DXCH_IP_CNT_SET_MODE_ENT                 cntSetMode;
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC    interfaceModeCfgPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    cntSet = (CPSS_INTERFACE_TYPE_ENT)inFields[0];
    cntSetMode = (CPSS_INTERFACE_TYPE_ENT)inFields[1];

    interfaceModeCfgPtr.portTrunkCntMode =
                             (CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT)inFields[2];
    interfaceModeCfgPtr.ipMode = (CPSS_IP_PROTOCOL_STACK_ENT)inFields[3];
    interfaceModeCfgPtr.vlanMode = (CPSS_DXCH_IP_VLAN_CNT_MODE_ENT)inFields[4];

    switch(inFields[2])
    {
    case 1:
        interfaceModeCfgPtr.devNum = (GT_U8)inFields[5];
        interfaceModeCfgPtr.portTrunk.port = (GT_U8)inFields[6];
        break;

    case 2:
        interfaceModeCfgPtr.portTrunk.trunk = (GT_TRUNK_ID)inFields[7];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceModeCfgPtr.portTrunk.trunk);
        break;

    default:
        break;
    }

    if(inFields[4])
        interfaceModeCfgPtr.vlanId = (GT_U16)inFields[8];

    /* call cpss api function */
    result = cpssDxChIpCntSetModeSet(devNum, cntSet, cntSetMode,
                                                      &interfaceModeCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpCntSet
*
* DESCRIPTION:
*       set the requested IP counter set.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum    - physical device number
*       cntSet    - counters set to clear.
*       counters  - the counter values to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpCntSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                           result;

    GT_U8                               devNum;
    CPSS_IP_CNT_SET_ENT                 cntSet;
    CPSS_DXCH_IP_COUNTER_SET_STC        counters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    cntSet = (CPSS_IP_CNT_SET_ENT)inFields[0];

    counters.inUcPkts = (GT_U32)inFields[1];
    counters.inMcPkts = (GT_U32)inFields[2];
    counters.inUcNonRoutedExcpPkts = (GT_U32)inFields[3];
    counters.inUcNonRoutedNonExcpPkts = (GT_U32)inFields[4];
    counters.inMcNonRoutedExcpPkts = (GT_U32)inFields[5];
    counters.inMcNonRoutedNonExcpPkts = (GT_U32)inFields[6];
    counters.inUcTrappedMirrorPkts = (GT_U32)inFields[7];
    counters.inMcTrappedMirrorPkts = (GT_U32)inFields[8];
    counters.mcRfpFailPkts = (GT_U32)inFields[9];
    counters.outUcRoutedPkts = (GT_U32)inFields[10];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupCntSet(devNum, cntSet, &counters);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpSetMllCntInterface
*
* DESCRIPTION:
*      Sets a mll counter set's bounded inteface.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mllCntSet       - the mll counter set out of the 2.
*       interfaceCfgPtr - the mll counter interface configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpSetMllCntInterface

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                     result;

    GT_U8                                         devNum;
    GT_U32                                        mllCntSet;
    CPSS_DXCH_IP_COUNTER_SET_INTERFACE_CFG_STC    interfaceCfgPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    mllCntSet = (GT_U32)inFields[0];

    interfaceCfgPtr.portTrunkCntMode =
                             (CPSS_DXCH_IP_PORT_TRUNK_CNT_MODE_ENT)inFields[1];
    interfaceCfgPtr.ipMode = (CPSS_IP_PROTOCOL_STACK_ENT)inFields[2];
    interfaceCfgPtr.vlanMode = (CPSS_DXCH_IP_VLAN_CNT_MODE_ENT)inFields[3];

    switch(inFields[1])
    {
    case 1:
        interfaceCfgPtr.devNum = (GT_U8)inFields[4];
        interfaceCfgPtr.portTrunk.port = (GT_U8)inFields[5];
        CONVERT_DEV_PORT_DATA_MAC(interfaceCfgPtr.devNum,interfaceCfgPtr.portTrunk.port);
        break;

    case 2:
        interfaceCfgPtr.portTrunk.trunk = (GT_TRUNK_ID)inFields[6];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(interfaceCfgPtr.portTrunk.trunk);
        break;

    default:
        break;
    }

    if(inFields[3])
        interfaceCfgPtr.vlanId = (GT_U16)inFields[7];

    /* call cpss api function */
    result = cpssDxChIpSetMllCntInterface(devNum, mllCntSet,
                                                  &interfaceCfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllCntGet
*
* DESCRIPTION:
*      Get the mll counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum     - physical device number.
*       mllCntSet  - the mll counter set out of the 2
*
* OUTPUTS:
*       mllOutMCPktsPtr - According to the configuration of this cnt set, The
*                      number of routed IP Multicast packets Duplicated by the
*                      MLL Engine and transmitted via this interface
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllCntGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      mllCntSet;
    GT_U32                                      mllOutMCPktsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mllCntSet = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpMllCntGet(devNum, mllCntSet, &mllOutMCPktsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mllOutMCPktsPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMllCntSet
*
* DESCRIPTION:
*      set an mll counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mllCntSet       - the mll counter set out of the 2
*       mllOutMCPkts    - the counter value to set
*
* OUTPUTS:
*      none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMllCntSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      mllCntSet;
    GT_U32                                      mllOutMCPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mllCntSet = (GT_U32)inArgs[1];
    mllOutMCPkts = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpPortGroupMllCntSet(devNum, mllCntSet,
                                                  mllOutMCPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpDropCntSet
*
* DESCRIPTION:
*      set the drop counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum      - physical device number.
*       dropPkts    - the counter value to set
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpDropCntSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      dropPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropPkts = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpDropCntSet(devNum, dropPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssDxChIpRouterBridgedPacketsExceptionCntGet
*
* DESCRIPTION:
*      Get exception counter for Special Services for Bridged Traffic.
*      it counts the number of Bridged packets failing any of the following checks:
*        - SIP Filter check for bridged IPv4/6 packets
*        - IP Header Check for bridged IPv4/6 packets
*        - Unicast RPF check for bridged IPv4/6 and ARP packets
*        - Unicast SIP/SA check for bridged IPv4/6 and ARP packets
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       routerBridgedExceptionPktsPtr - the number of counted router bridged
*                                       exception  packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on illegal pointer value
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterBridgedPacketsExceptionCntGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      routerBridgedExceptionPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpRouterBridgedPacketsExceptionCntGet(devNum,
                                                                   &routerBridgedExceptionPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", routerBridgedExceptionPkts);
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChIpRouterBridgedPacketsExceptionCntSet
*
* DESCRIPTION:
*      Set exception counter for Special Services for Bridged Traffic.
*      it counts the number of Bridged packets failing any of the following checks:
*        - SIP Filter check for bridged IPv4/6 packets
*        - IP Header Check for bridged IPv4/6 packets
*        - Unicast RPF check for bridged IPv4/6 and ARP packets
*        - Unicast SIP/SA check for bridged IPv4/6 and ARP packets
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum                        - the device number
*       routerBridgedExceptionPkts    - the counter value to set
*
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterBridgedPacketsExceptionCntSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      routerBridgedExceptionPkts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    routerBridgedExceptionPkts = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpRouterBridgedPacketsExceptionCntSet(devNum,
                                                                   routerBridgedExceptionPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpSetDropCntMode
*
* DESCRIPTION:
*      Sets the drop counter count mode.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum      - physical device number.
*       dropCntMode - the drop counter count mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpSetDropCntMode

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_DXCH_IP_DROP_CNT_MODE_ENT              dropCntMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropCntMode = (CPSS_DXCH_IP_DROP_CNT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpSetDropCntMode(devNum, dropCntMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpDropCntGet
*
* DESCRIPTION:
*      Get the drop counter.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum    - physical device number.
*
* OUTPUTS:
*       dropPktsPtr - the number of counted dropped packets according to the
*                     drop counter mode.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpDropCntGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      dropPktsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = pg_wrap_cpssDxChIpDropCntGet(devNum, &dropPktsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropPktsPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpMtuProfileSet
*
* DESCRIPTION:
*      Sets the global MTU profile limit values.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum          - physical device number.
*       mtuProfileIndex - the mtu profile index out of the possible 8 (0..7).
*       mtu             - the maximum transmission unit
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpMtuProfileSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      mtuProfileIndex;
    GT_U32                                      mtu;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mtuProfileIndex = (GT_U32)inArgs[1];
    mtu = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpMtuProfileSet(devNum, mtuProfileIndex, mtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv6AddrPrefixScopeSet
*
* DESCRIPTION:
*       Defines a prefix of a scope type.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum           - physical device number
*       prefix           - an IPv6 address prefix
*       prefixLen        - length of the prefix (0 - 16)
*       addressScope     - type of the address scope spanned by the prefix
*       prefixScopeIndex - index of the new prefix scope entry (0..4)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_BAD_PARAM       - case that the prefix length is out of range.
*       GT_NOT_SUPPORTED - if working with a PP that doesn't support IPv6
*
* COMMENTS:
*       Configures an entry in the prefix look up table
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6AddrPrefixScopeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_IPV6ADDR                                 prefix;
    GT_U32                                      prefixLen;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScope;
    GT_U32                                      prefixScopeIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisIpv6Addr(&prefix, (GT_U8*)inArgs[1]);
    prefixLen = (GT_U32)inArgs[2];
    addressScope = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[3];
    prefixScopeIndex = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChIpv6AddrPrefixScopeSet(devNum, prefix, prefixLen,
                                       addressScope, prefixScopeIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv6UcScopeCommandSet
*
* DESCRIPTION:
*       sets the ipv6 Unicast scope commands.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum           - physical device number
*       addressScopeSrc  - IPv6 address scope of source address
*       addressScopeDest - IPv6 address scope of destination address
*       borderCrossed    - GT_TRUE if source site ID is diffrent to destination
*                          site ID
*       scopeCommand     - action to be done on a packet that match the above
*                          scope configuration.
*                          possible commands:
*                           CPSS_PACKET_CMD_ROUTE_E,
*                           CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                           CPSS_PACKET_CMD_DROP_SOFT_E,
*                           CPSS_PACKET_CMD_DROP_HARD_E
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_FAIL          - on error.
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_NO_RESOURCE - if failed to allocate CPU memory
*       GT_NOT_SUPPORTED - if working with a PP that doesn't support IPv6
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6UcScopeCommandSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeDest;
    GT_BOOL                                     borderCrossed;
    CPSS_PACKET_CMD_ENT                         scopeCommand;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    borderCrossed = (GT_BOOL)inArgs[3];
    scopeCommand = (CPSS_PACKET_CMD_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssDxChIpv6UcScopeCommandSet(devNum, addressScopeSrc,
                     addressScopeDest, borderCrossed, scopeCommand);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpv6McScopeCommandSet
*
* DESCRIPTION:
*       sets the ipv6 Multicast scope commands.
*
* APPLICABLE DEVICES:
*       All DxCh2 devices.
*
* INPUTS:
*       devNum           - physical device number
*       addressScopeSrc  - IPv6 address scope of source address
*       addressScopeDest - IPv6 address scope of destination address
*       borderCrossed    - GT_TRUE if source site ID is diffrent to destination
*                          site ID
*       scopeCommand     - action to be done on a packet that match the above
*                          scope configuration.
*                          possible commands:
*                           CPSS_PACKET_CMD_ROUTE_E,
*                           CPSS_PACKET_CMD_ROUTE_AND_MIRROR_E,
*                           CPSS_PACKET_CMD_TRAP_TO_CPU_E,
*                           CPSS_PACKET_CMD_DROP_SOFT_E,
*                           CPSS_PACKET_CMD_DROP_HARD_E,
*                           CPSS_PACKET_CMD_BRIDGE_AND_MIRROR_E,
*                           CPSS_PACKET_CMD_BRIDGE_E
*       mllSelectionRule - rule for choosing MLL for IPv6 Multicast propogation
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success.
*       GT_FAIL          - on error.
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_NO_RESOURCE - if failed to allocate CPU memory
*       GT_NOT_SUPPORTED - if working with a PP that doesn't support IPv6
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpv6McScopeCommandSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeSrc;
    CPSS_IPV6_PREFIX_SCOPE_ENT                  addressScopeDest;
    GT_BOOL                                     borderCrossed;
    CPSS_PACKET_CMD_ENT                         scopeCommand;
    CPSS_IPV6_MLL_SELECTION_RULE_ENT            mllSelectionRule;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    addressScopeSrc = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[1];
    addressScopeDest = (CPSS_IPV6_PREFIX_SCOPE_ENT)inArgs[2];
    borderCrossed = (GT_BOOL)inArgs[3];
    scopeCommand = (CPSS_PACKET_CMD_ENT)inArgs[4];
    mllSelectionRule = (CPSS_IPV6_MLL_SELECTION_RULE_ENT)inArgs[5];

    /* call cpss api function */
    result = cpssDxChIpv6McScopeCommandSet(devNum, addressScopeSrc,
                                   addressScopeDest, borderCrossed,
                                    scopeCommand, mllSelectionRule);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpRouterMacSaBaseSet
*
* DESCRIPTION:
*       Sets 8 MSBs of Router MAC SA Base address on specified device.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev - device number
*       macPtr - (pointer to)The system Mac address to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaBaseSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8         devNum;
    GT_ETHERADDR  macPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisMacAddr(&macPtr, (GT_U8*)inArgs[1]);

    /* call cpss api function */
    result = cpssDxChIpRouterMacSaBaseSet(devNum, &macPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaBaseGet
*
* DESCRIPTION:
*       Get 8 MSBs of Router MAC SA Base address on specified device.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev - device number
*
* OUTPUTS:
*       macPtr - (pointer to)The system Mac address to set.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaBaseGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8         devNum;
    GT_ETHERADDR  macPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChIpRouterMacSaBaseGet(devNum, &macPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b", macPtr.arEther);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaLsbModeSet
*
* DESCRIPTION:
*       Sets the mode in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum          - The system unique device number
*       saLsbMode       - The MAC SA least-significant bit mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/saLsbMode.
*
* COMMENTS:
*       The device 5 most significant bytes are set by cpssDxChIpRouterMacSaBaseSet().
*
*       If saLsbMode is Port Mode, the Port library  action
*       cpssDxChPortMacSaLsbSet() sets the per port least significant byte.
*
*       CPSS_MAC_SA_LSB_MODE_ENT -
*       If saLsbMode is Vlan Mode, the Vlan library  action
*       cpssDxChBrgVlanMacPerVlanSet() sets the per vlan least significant byte.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaLsbModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    saLsbMode = (CPSS_MAC_SA_LSB_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRouterMacSaLsbModeSet(devNum, saLsbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaLsbModeGet
*
* DESCRIPTION:
*       Gets the mode in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum          - The system unique device number
*
* OUTPUTS:
*       saLsbMode       - (pointer to) The MAC SA least-significant bit mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/saLsbMode.
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       None;
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaLsbModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    CPSS_MAC_SA_LSB_MODE_ENT    saLsbMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    saLsbMode = (CPSS_MAC_SA_LSB_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRouterMacSaLsbModeGet(devNum, &saLsbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saLsbMode);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterPortVlanMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this VLAN / EGGRESS PORT.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*     devNum          - device number.
*     vlanPortId      - VLAN Id or Eggress Port Id
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to VLAN id / Eggress Port Id.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum or vlanId
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterPortVlanMacSaLsbSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_U16      vlanPortId;
    GT_U8       saMac;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanPortId = (GT_U16)inArgs[1];
    saMac = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpRouterPortVlanMacSaLsbSet(devNum, vlanPortId, saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterPortVlanMacSaLsbGet
*
* DESCRIPTION:
*       Gets the 8 LSB Router MAC SA for this VLAN / EGGRESS PORT.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*     devNum          - device number.
*     vlanPortId      - VLAN Id or Eggress Port Id
*
* OUTPUTS:
*     saMac           - (pointer to) The 8 bits SA mac value written to the SA
*                       bits of routed packet if SA alteration mode is configured
*                       to take LSB according to VLAN id / Eggress Port Id.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum or vlanId
*     GT_BAD_PTR       - one of the parameters is NULL pointer
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterPortVlanMacSaLsbGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       devNum;
    GT_U16      vlanPortId;
    GT_U8       saMac;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    vlanPortId = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRouterPortVlanMacSaLsbGet(devNum, vlanPortId, &saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saMac);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaModifyEnable
*
* DESCRIPTION:
*      Per Egress port bit Enable Routed packets MAC SA Modification
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum   - The system unique device number
*       portNum  - The port number to set
*       enable   - GT_FALSE: MAC SA Modification of routed packets is disabled
*                  GT_TRUE: MAC SA Modification of routed packets is enabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/portNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaModifyEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8      devNum;
    GT_U8      portNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpRouterMacSaModifyEnable(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterMacSaModifyEnableGet
*
* DESCRIPTION:
*      Per Egress port bit Get Routed packets MAC SA Modification State
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       devNum   - The system unique device number
*       portNum  - The port number to set (0-31)
*
* OUTPUTS:
*       enable   - GT_FALSE: MAC SA Modification of routed packets is disabled
*                  GT_TRUE: MAC SA Modification of routed packets is enabled
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/portNum.
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterMacSaModifyEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8      devNum;
    GT_U8      portNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

   /* call cpss api function */
    result = cpssDxChIpRouterMacSaModifyEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpPortRouterMacSaLsbModeSet
*
* DESCRIPTION:
*       Sets the mode, per port, in which the device sets the packet's MAC SA
*       least significant bytes.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - the port number
*       saLsbMode       - The MAC SA least-significant bit mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/saLsbMode.
*
* COMMENTS:
*       The device 5 most significant bytes are set by cpssDxChIpRouterMacSaBaseSet().
*
*       If saLsbMode is Port Mode, the Port library  action
*       cpssDxChPortMacSaLsbSet() sets the per port least significant byte.
*
*       CPSS_MAC_SA_LSB_MODE_ENT -
*       If saLsbMode is Vlan Mode, the Vlan library  action
*       cpssDxChBrgVlanMacPerVlanSet() sets the per vlan least significant byte.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortRouterMacSaLsbModeSet
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
    CPSS_MAC_SA_LSB_MODE_ENT saLsbMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    saLsbMode = (CPSS_MAC_SA_LSB_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpPortRouterMacSaLsbModeSet(devNum, portNum, saLsbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpPortRouterMacSaLsbModeGet
*
* DESCRIPTION:
*       Gets the mode, per port, in which the device sets the packet's MAC SA least
*       significant bytes.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*       devNum          - the device number
*       portNum         - the port number
*
* OUTPUTS:
*       saLsbModePtr    - (pointer to) The MAC SA least-significant bit mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_NOT_SUPPORTED - if the feature does not supported by specified device
*       GT_BAD_PARAM     - wrong devNum/saLsbMode.
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       None;
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortRouterMacSaLsbModeGet
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
    CPSS_MAC_SA_LSB_MODE_ENT saLsbMode;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpPortRouterMacSaLsbModeGet(devNum, portNum, &saLsbMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saLsbMode);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpRouterPortMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this EGGRESS PORT.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     portNum         - Eggress Port number
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to Eggress Port number.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterPortMacSaLsbSet
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
    GT_U8 saMac;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    saMac = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpRouterPortMacSaLsbSet(devNum, portNum, saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterPortMacSaLsbGet
*
* DESCRIPTION:
*       Gets the 8 LSB Router MAC SA for this EGGRESS PORT.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     portNum         - Eggress Port number
*
* OUTPUTS:
*     saMacPtr        - (pointer to) The 8 bits SA mac value written to the SA
*                       bits of routed packet if SA alteration mode is configured
*                       to take LSB according to Eggress Port number.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum
*     GT_BAD_PTR       - one of the parameters is NULL pointer
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterPortMacSaLsbGet
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
    GT_U8 saMac;


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChIpRouterPortMacSaLsbGet(devNum, portNum, &saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saMac);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpRouterVlanMacSaLsbSet
*
* DESCRIPTION:
*       Sets the 8 LSB Router MAC SA for this VLAN.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     vlan            - VLAN Id
*     saMac           - The 8 bits SA mac value to be written to the SA bits of
*                       routed packet if SA alteration mode is configured to
*                       take LSB according to VLAN.
*
* OUTPUTS:
*     None.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum or vlanId
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterVlanMacSaLsbSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vlan;
    GT_U8 saMac;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlan = (GT_U16)inArgs[1];
    saMac = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChIpRouterVlanMacSaLsbSet(devNum, vlan, saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpRouterVlanMacSaLsbGet
*
* DESCRIPTION:
*       Gets the 8 LSB Router MAC SA for this VLAN.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*     devNum          - the device number
*     vlan            - VLAN Id
*
* OUTPUTS:
*     saMacPtr        - (pointer to) The 8 bits SA mac value written to the SA
*                       bits of routed packet if SA alteration mode is configured
*                       to take LSB according to VLAN.
*
* RETURNS:
*     GT_OK            - on success
*     GT_NOT_SUPPORTED - if the feature does not supported by specified device
*     GT_FAIL          - on error.
*     GT_HW_ERROR      - on hardware error
*     GT_BAD_PARAM     - wrong devNum or vlanId
*     GT_BAD_PTR       - one of the parameters is NULL pointer
*
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpRouterVlanMacSaLsbGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U16 vlan;
    GT_U8 saMac;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    vlan = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssDxChIpRouterVlanMacSaLsbGet(devNum, vlan, &saMac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", saMac);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChIpEcmpUcRpfCheckEnableSet
*
* DESCRIPTION:
*       Globally enables/disables ECMP/QoS unicast RPF check.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*       devNum    - the device number
*       enable    - enable / disable ECMP/Qos unicast RPF check
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpUcRpfCheckEnableSet
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
    result = cpssDxChIpEcmpUcRpfCheckEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpEcmpUcRpfCheckEnableGet
*
* DESCRIPTION:
*       Gets globally enables/disables ECMP/QoS unicast RPF check state.
*
* APPLICABLE DEVICES:
*       DxCh3 and above.
*
* INPUTS:
*       devNum    - the device number
*       enablePtr - (points to) enable / disable ECMP/Qos unicast RPF check
*
* OUTPUTS:
*       none
*
* RETURNS:
*       GT_OK   - on success
*       GT_NOT_INITIALIZED - The library was not initialized.
*       GT_FAIL - on error
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpEcmpUcRpfCheckEnableGet
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
    result = cpssDxChIpEcmpUcRpfCheckEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpUcRpfVlanModeSet
*
* DESCRIPTION:
*       Defines the uRPF check mode for a given VID.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum    - the device number
*       vid       - Vlan Id
*       uRpfMode  - CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E = uRPF check is disabled
*                        for this VID.
*                   CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E= Vlan: If ECMP uRPF is
*                                        globally enabled, then uRPF check is done
*                                        by comparing the packet VID to the VID
*                                        in the additional route entry, otherwise
*                                        it is done using the SIP-Next Hop Entry VID.
*                   CPSS_DXCH_IP_VLAN_URPF_PORT_MODE_E = uRPF check is done by
*                        comparing the packet source {device,port}/Trunk to
*                              the SIP-Next Hop Entry {device,port}/Trunk.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device, vid or uRpfMode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       If ucRPFCheckEnable field configured in cpssDxChIpLttWrite is enabled
*       then VLAN-based uRPF check is performed, regardless of this configuration.
*       Otherwise, uRPF check is performed for this VID according to this
*       configuration.
*       Port-based uRPF mode is not supported if the address is associated
*       with an ECMP/QoS block of nexthop entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRpfVlanModeSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                           devNum;
    GT_U16                          vid;
    CPSS_DXCH_IP_VLAN_URPF_MODE_ENT uRpfMode;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    vid         = (GT_U16)inArgs[1];
    uRpfMode    = (CPSS_DXCH_IP_VLAN_URPF_MODE_ENT)inArgs[2];

    /* call cpss api function */
    status =  cpssDxChIpUcRpfVlanModeSet(devNum,vid, uRpfMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpUcRpfVlanModeGet
*
* DESCRIPTION:
*       Read uRPF check mode for a given VID.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum    - the device number
*       vid       - Vlan Id
*
* OUTPUTS:
*       uRpfModePtr  -  CPSS_DXCH_IP_VLAN_URPF_DISABLE_MODE_E = uRPF check is disabled
*                           for this VID.
*                       CPSS_DXCH_IP_VLAN_URPF_VLAN_MODE_E= Vlan: If ECMP uRPF is
*                                        globally enabled, then uRPF check is done
*                                        by comparing the packet VID to the VID
*                                        in the additional route entry, otherwise
*                                        it is done using the SIP-Next Hop Entry VID.
*                       CPSS_DXCH_IP_VLAN_URPF_PORT_MODE_E = uRPF check is done by
*                           comparing the packet source {device,port}/Trunk to
*                           the SIP-Next Hop Entry {device,port}/Trunk.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       If ucRPFCheckEnable field configured in cpssDxChIpLttWrite is enabled
*       then VLAN-based uRPF check is performed, regardless of this configuration.
*       Otherwise, uRPF check is performed for this VID according to this
*       configuration.
*       Port-based uRPF mode is not supported if the address is associated
*       with an ECMP/QoS block of nexthop entries.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpUcRpfVlanModeGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                           devNum;
    GT_U16                          vid;
    CPSS_DXCH_IP_VLAN_URPF_MODE_ENT uRpfMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    vid         = (GT_U16)inArgs[1];

    /* call cpss api function */
    status =  cpssDxChIpUcRpfVlanModeGet(devNum,vid, &uRpfMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%d", uRpfMode);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChIpPortSipSaEnableSet
*
* DESCRIPTION:
*       Enable SIP/SA check for packets received from the given port.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum    - the device number
*       portNum   - the port number (including CPU port)
*       enable    - GT_FALSE: disable SIP/SA check on the port
*                   GT_TRUE:  enable SIP/SA check on the port
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       SIP/SA check is triggered only if both this flag and
*       the sipSaCheckMismatchEnable field configured in cpssDxChIpLttWrite
*       are enabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortSipSaEnableSet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                        devNum;
    GT_U8                        portNum;
    GT_BOOL                      enable;

   /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_U8)inArgs[1];
    enable      = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    status =  cpssDxChIpPortSipSaEnableSet(devNum,portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChIpPortSipSaEnableGet
*
* DESCRIPTION:
*       Return the SIP/SA check status for packets received from the given port.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum    - the device number
*       portNum   - the port number (including CPU port)

* OUTPUTS:
*       enablePtr - GT_FALSE: SIP/SA check on the port is disabled
*                   GT_TRUE:  SIP/SA check on the port is enabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or port
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       SIP/SA check is triggered only if both this flag and
*       the sipSaCheckMismatchEnable field configured in cpssDxChIpLttWrite
*       are enabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChIpPortSipSaEnableGet
(
 IN  GT_32 inArgs[CMD_MAX_ARGS],
 IN  GT_32 inFields[CMD_MAX_FIELDS],
 IN  GT_32 numFields,
 OUT GT_8  outArgs[CMD_MAX_BUFFER]
 )
{

    GT_STATUS   status;

    GT_U8                        devNum;
    GT_U8                        portNum;
    GT_BOOL                      enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum      = (GT_U8)inArgs[0];
    portNum     = (GT_U8)inArgs[1];

    /* call cpss api function */
    status =  cpssDxChIpPortSipSaEnableGet(devNum,portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, status, "%d", enable);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChIpSpecialRouterTriggerEnable",
        &wrCpssDxChIpSpecialRouterTriggerEnable,
        3, 0},

    {"cpssDxChIpExceptionCommandSet",
        &wrCpssDxChIpExceptionCommandSet,
        4, 0},

    {"cpssDxChIpUcRouteAgingModeSet",
        &wrCpssDxChIpUcRouteAgingModeSet,
        2, 0},

    {"cpssDxChIpRouterSourceIdSet",
        &wrCpssDxChIpRouterSourceIdSet,
        3, 0},

    {"cpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet",
        &wrCpssDxChIpFailRpfCtrlTrafficMultiTargetTCQueueSet,
        3, 0},

    {"cpssDxChIpQosProfileToMultiTargetTCQueueMapSet",
        &wrCpssDxChIpQosProfileToMultiTargetTCQueueMapSet,
        3, 0},

    {"cpssDxChIpMultiTargetQueueFullDropCntGet",
        &wrCpssDxChIpMultiTargetQueueFullDropCntGet,
        1, 0},

    {"cpssDxChIpMultiTargetQueueFullDropCntSet",
        &wrCpssDxChIpMultiTargetQueueFullDropCntSet,
        2, 0},

    {"cpssDxChIpMultiTargetTCQueueSchedModeSet",
        &wrCpssDxChIpMultiTargetTCQueueSchedModeSet,
        4, 0},

    {"cpssDxChIpBridgeServiceEnable",
        &wrCpssDxChIpBridgeServiceEnable,
        4, 0},

    {"cpssDxChIpMllBridgeEnable",
        &wrCpssDxChIpMllBridgeEnable,
        2, 0},

    {"cpssDxChIpMultiTargetRateShaperSet",
        &wrCpssDxChIpMultiTargetRateShaperSet,
        3, 0},

    {"cpssDxChIpMultiTargetUcSchedModeSet",
        &wrCpssDxChIpMultiTargetUcSchedModeSet,
        5, 0},

    {"cpssDxChIpArpBcModeSet",
        &wrCpssDxChIpArpBcModeSet,
        2, 0},

    {"cpssDxChIpPortRoutingEnable",
        &wrCpssDxChIpPortRoutingEnable,
        5, 0},

    {"cpssDxChIpPortRoutingEnableGet",
        &wrCpssDxChIpPortRoutingEnableGet,
        4, 0},

    {"cpssDxChIpQosProfileToRouteEntryMapSet",
        &wrCpssDxChIpQosProfileToRouteEntryMapSet,
        3, 0},

    {"cpssDxChIpRoutingEnable",
        &wrCpssDxChIpRoutingEnable,
        2, 0},

    {"cpssDxChIpRoutingEnableGet",
        &wrCpssDxChIpRoutingEnableGet,
        1, 0},

    {"cpssDxChIpCntGetFirst",
        &wrCpssDxChIpCntGetFirst,
        1, 0},

    {"cpssDxChIpCntGetNext",
        &wrCpssDxChIpCntGetNext,
        1, 0},

    {"cpssDxChIpCntSetModeSet",
        &wrCpssDxChIpCntSetModeSet,
        1, 9},

    {"cpssDxChIpCntSet",
        &wrCpssDxChIpCntSet,
        1, 11},

    {"cpssDxChIpMllCntInterfaceSet",
        &wrCpssDxChIpSetMllCntInterface,
        1, 8},

    {"cpssDxChIpMllCntGet",
        &wrCpssDxChIpMllCntGet,
        2, 0},

    {"cpssDxChIpMllCntSet",
        &wrCpssDxChIpMllCntSet,
        3, 0},

    {"cpssDxChIpDropCntSet",
        &wrCpssDxChIpDropCntSet,
        2, 0},

    {"cpssDxChIpSetDropCntMode",
        &wrCpssDxChIpSetDropCntMode,
        2, 0},

    {"cpssDxChIpDropCntGet",
        &wrCpssDxChIpDropCntGet,
        1, 0},

    {"cpssDxChIpMtuProfileSet",
        &wrCpssDxChIpMtuProfileSet,
        3, 0},

    {"cpssDxChIpv6AddrPrefixScopeSet",
        &wrCpssDxChIpv6AddrPrefixScopeSet,
        5, 0},

    {"cpssDxChIpv6UcScopeCommandSet",
        &wrCpssDxChIpv6UcScopeCommandSet,
        5, 0},

    {"cpssDxChIpv6McScopeCommandSet",
        &wrCpssDxChIpv6McScopeCommandSet,
        6, 0},

    {"cpssDxChIpRouterMacSaBaseSet",
        &wrCpssDxChIpRouterMacSaBaseSet,
        2, 0},

    {"cpssDxChIpRouterMacSaBaseGet",
        &wrCpssDxChIpRouterMacSaBaseGet,
        1, 0},

    {"cpssDxChIpRouterMacSaLsbModeSet",
        &wrCpssDxChIpRouterMacSaLsbModeSet,
        2, 0},

    {"cpssDxChIpRouterMacSaLsbModeGet",
        &wrCpssDxChIpRouterMacSaLsbModeGet,
        1, 0},


    {"cpssDxChIpRouterPortVlanMacSaLsbSet",
        &wrCpssDxChIpRouterPortVlanMacSaLsbSet,
        3, 0},

    {"cpssDxChIpRouterPortVlanMacSaLsbGet",
        &wrCpssDxChIpRouterPortVlanMacSaLsbGet,
        2, 0},

    {"cpssDxChIpRouterMacSaModifyEnable",
        &wrCpssDxChIpRouterMacSaModifyEnable,
        3, 0},

    {"cpssDxChIpRouterMacSaModifyEnableGet",
        &wrCpssDxChIpRouterMacSaModifyEnableGet,
        2, 0},

   {"cpssDxChIpPortRouterMacSaLsbModeSet",
        &wrCpssDxChIpPortRouterMacSaLsbModeSet,
        3, 0},
    {"cpssDxChIpPortRouterMacSaLsbModeGet",
        &wrCpssDxChIpPortRouterMacSaLsbModeGet,
        2, 0},
    {"cpssDxChIpRouterPortMacSaLsbSet",
        &wrCpssDxChIpRouterPortMacSaLsbSet,
        3, 0},
    {"cpssDxChIpRouterPortMacSaLsbGet",
        &wrCpssDxChIpRouterPortMacSaLsbGet,
        2, 0},
    {"cpssDxChIpRouterVlanMacSaLsbSet",
         &wrCpssDxChIpRouterVlanMacSaLsbSet,
         3, 0},
    {"cpssDxChIpRouterVlanMacSaLsbGet",
         &wrCpssDxChIpRouterVlanMacSaLsbGet,
         2, 0},
    {"cpssDxChIpEcmpUcRpfCheckEnableSet",
         &wrCpssDxChIpEcmpUcRpfCheckEnableSet,
         2, 0},
    {"cpssDxChIpEcmpUcRpfCheckEnableGet",
         &wrCpssDxChIpEcmpUcRpfCheckEnableGet,
         1, 0},
    {"cpssDxChIpExceptionCommandSetExt",
        &wrCpssDxChIpExceptionCommandSet,
        4, 0},

    {"cpssDxChIpUcRpfVlanModeSet",
    &wrCpssDxChIpUcRpfVlanModeSet,
    3, 0},

    {"cpssDxChIpUcRpfVlanModeGet",
    &wrCpssDxChIpUcRpfVlanModeGet,
    2, 0},

    {"cpssDxChIpPortSipSaEnableSet",
    &wrCpssDxChIpPortSipSaEnableSet,
    3, 0},

    {"cpssDxChIpPortSipSaEnableGet",
    &wrCpssDxChIpPortSipSaEnableGet,
    2, 0},
    {"cpssDxChIpRouterBridgedPacketsExceptionCntSet",
        &wrCpssDxChIpRouterBridgedPacketsExceptionCntSet,
        2, 0},
    {"cpssDxChIpRouterBridgedPacketsExceptionCntGet",
        &wrCpssDxChIpRouterBridgedPacketsExceptionCntGet,
        1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChIpCtrl
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
GT_STATUS cmdLibInitCpssDxChIpCtrl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}



