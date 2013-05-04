/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssDxChNetif.c
*
* DESCRIPTION:
*       Wrapper functions for DxCh network interface API functions
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <galtisAgent/wrapUtil/cmdCpssNetReceive.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIf.h>
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpssCommon/private/prvCpssCrc.h>
#include <cpssCommon/private/prvCpssMemLib.h>
#include <galtisAgent/wrapCpss/dxCh/networkIf/cmdCpssDxChNetTransmit.h>

/* table TxNetIf/ExtTxNetIf global variables */
static GT_U32      netTxCurrIndex=0; /*Index of last descriptor got from table*/
extern DXCH_PKT_DESC_STC* cpssDxChTxPacketDescTbl;

GT_POOL_ID txBuffersPoolId;

static const char hexcode[] = "0123456789ABCDEF";
static GT_BOOL  poolCreated = GT_FALSE;

/*******************************************************************************
* netIfMultiPortGroupsBmpGet
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
static void netIfMultiPortGroupsBmpGet
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
static GT_STATUS pg_wrap_cpssDxChNetIfCpuCodeRateLimiterTableSet
(
    IN  GT_U8  devNum,
    IN  GT_U32 rateLimiterIndex,
    IN  GT_U32 windowSize,
    IN  GT_U32 pktLimit
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    netIfMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChNetIfCpuCodeRateLimiterTableSet(devNum, rateLimiterIndex,
                                                        windowSize, pktLimit);
    }
    else
    {
        return cpssDxChNetIfPortGroupCpuCodeRateLimiterTableSet(devNum, pgBmp,
                                                                rateLimiterIndex,
                                                                windowSize,
                                                                pktLimit);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChNetIfCpuCodeRateLimiterTableGet
(
    IN   GT_U8  devNum,
    IN   GT_U32 rateLimiterIndex,
    OUT  GT_U32 *windowSizePtr,
    OUT  GT_U32 *pktLimitPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    netIfMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChNetIfCpuCodeRateLimiterTableGet(devNum, rateLimiterIndex,
                                                        windowSizePtr,
                                                        pktLimitPtr);
    }
    else
    {
        return cpssDxChNetIfPortGroupCpuCodeRateLimiterTableGet(devNum, pgBmp,
                                                                rateLimiterIndex,
                                                                windowSizePtr,
                                                                pktLimitPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
(
    IN  GT_U8  devNum,
    IN  GT_U32 windowResolution
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    netIfMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(devNum,
                                                              windowResolution);
    }
    else
    {
        return cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionSet(devNum,
                                                              pgBmp,
                                                              windowResolution);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet
(
    IN   GT_U8  devNum,
    OUT  GT_U32 *windowResolutionPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    netIfMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet(devNum,
                                                           windowResolutionPtr);
    }
    else
    {
        return cpssDxChNetIfPortGroupCpuCodeRateLimiterWindowResolutionGet(devNum,
                                                           pgBmp,
                                                           windowResolutionPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet
(
    IN   GT_U8               devNum,
    IN   GT_U32              rateLimiterIndex,
    OUT  GT_U32              *packetCntrPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    netIfMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet(devNum,
                                                            rateLimiterIndex,
                                                            packetCntrPtr);
    }
    else
    {
        return cpssDxChNetIfPortGroupCpuCodeRateLimiterPacketCntrGet(devNum,
                                                               pgBmp,
                                                               rateLimiterIndex,
                                                               packetCntrPtr);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChNetIfCpuCodeRateLimiterDropCntrSet
(
    IN GT_U8                devNum,
    IN GT_U32               dropCntrVal
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    netIfMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChNetIfCpuCodeRateLimiterDropCntrSet(devNum,
                                                          dropCntrVal);
    }
    else
    {
        return cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrSet(devNum,
                                                                 pgBmp,
                                                                 dropCntrVal);
    }
}

/* Port Group and Regular version wrapper     */
/* description see in original function header */
static GT_STATUS pg_wrap_cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
(
    IN GT_U8                devNum,
    IN GT_U32               *dropCntrValPtr
)
{
    GT_BOOL             pgEnable; /* multi port group  enable */
    GT_PORT_GROUPS_BMP  pgBmp;    /* port group BMP           */

    netIfMultiPortGroupsBmpGet(devNum, &pgEnable, &pgBmp);

    if (pgEnable == GT_FALSE)
    {
        return cpssDxChNetIfCpuCodeRateLimiterDropCntrGet(devNum,
                                                          dropCntrValPtr);
    }
    else
    {
        return cpssDxChNetIfPortGroupCpuCodeRateLimiterDropCntrGet(devNum,
                                                                 pgBmp,
                                                                 dropCntrValPtr);
    }
}
/*******************************************************************************
* cpssDxChNetIfInit
*
* DESCRIPTION:
*       Initialize the network interface structures, Rx descriptors & buffers
*       and Tx descriptors -- SDMA or Ethernet port interface.
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*       devNum  - The device to initialize.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - one of the parameters value is wrong
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChNetIfInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfRemove
*
* DESCRIPTION:
*       This function is called upon Hot removal of a device, inorder to release
*       all Network Interface related structures.
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*       devNum  - The device that was removed.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - one of the parameters value is wrong
*
* COMMENTS:
*       None.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfRemove
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChNetIfRemove(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfPrePendTwoBytesHeaderSet
*
* DESCRIPTION:
*       Enables/Disable pre-pending a two-byte header to all packets forwarded
*       to the CPU (via the CPU port or the PCI interface).
*       This two-byte header is used to align the IPv4 header to 32 bits.
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*        devNum    - device number
*        enable    - GT_TRUE  - Two-byte header is pre-pended to packets
*                               forwarded to the CPU.
*                    GT_FALSE - Two-byte header is not pre-pended to packets
*                               forward to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfPrePendTwoBytesHeaderSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChNetIfPrePendTwoBytesHeaderSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfPrePendTwoBytesHeaderGet
*
* DESCRIPTION:
*       Get the Enable/Disable status of pre-pending a two-byte header to all
*       packets forwarded to the CPU (via the CPU port or the PCI interface).
*       This two-byte header is used to align the IPv4 header to 32 bits.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*        devNum    - device number
*
* OUTPUTS:
*        enablePtr -  (pointer to) GT_TRUE  - Two-byte header is pre-pended
*                                  to packets forwarded to the CPU.
*                    GT_FALSE - Two-byte header is not pre-pended to packets
*                               forward to the CPU.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on bad parameters
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfPrePendTwoBytesHeaderGet
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
    result = cpssDxChNetIfPrePendTwoBytesHeaderGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/****************Table cpssDxChNetIfSdmaRxCounters***************/
static GT_U8 gQueueIdx;

/*******************************************************************************
* cpssDxChNetIfSdmaRxCountersGet
*
* DESCRIPTION:
*       For each packet processor, get the Rx packet counters from its SDMA
*       packet interface.  Return the aggregate counter values for the given
*       traffic class queue. -- SDMA relate
*
* APPLICABLE DEVICES: ALL PCI/SDMA DXCH Devices
*
* INPUTS:
*     devNum    - device number
*     queueIdx  - traffic class queue
*
* OUTPUTS:
*     rxCountersPtr - (pointer to) rx counters on this queue
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*       Counters are reset on every read.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfSdmaRxCountersGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC  rxCounters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    gQueueIdx = 0;

    /* call cpss api function */
    result = cpssDxChNetIfSdmaRxCountersGet(devNum, gQueueIdx, &rxCounters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = rxCounters.rxInPkts;
    inFields[1] = rxCounters.rxInOctets;

    /* pack and output table fields */
    fieldOutput("%d%d%d", gQueueIdx, inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaRxCountersGet
*
* DESCRIPTION:
*       For each packet processor, get the Rx packet counters from its SDMA
*       packet interface.  Return the aggregate counter values for the given
*       traffic class queue. -- SDMA relate
*
* APPLICABLE DEVICES: ALL PCI/SDMA DXCH Devices
*
* INPUTS:
*     devNum    - device number
*     queueIdx  - traffic class queue
*
* OUTPUTS:
*     rxCountersPtr - (pointer to) rx counters on this queue
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*       Counters are reset on every read.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfSdmaRxCountersGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    CPSS_DXCH_NET_SDMA_RX_COUNTERS_STC  rxCounters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    gQueueIdx++;

    if (gQueueIdx >= CPSS_TC_RANGE_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChNetIfSdmaRxCountersGet(devNum, gQueueIdx, &rxCounters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = rxCounters.rxInPkts;
    inFields[1] = rxCounters.rxInOctets;

    /* pack and output table fields */
    fieldOutput("%d%d%d", gQueueIdx, inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/****************Table cpssDxChNetIfSdmaRxErrorCount***************/
static GT_U8                                  gQueueNum;
static CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC    gRxErrCount;
/*******************************************************************************
* cpssDxChNetIfSdmaRxErrorCountGet
*
* DESCRIPTION:
*       Returns the total number of Rx resource errors that occurred on a given
*       Rx queue . -- SDMA relate
*
* APPLICABLE DEVICES: ALL PCI/SDMA DXCH Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       rxErrCountPtr  - (pointer to) The total number of Rx resource errors on
*                        the device for all the queues.
*
* RETURNS:
*       GT_OK           - on success,
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on bad parameters
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       The counters returned by this function reflects the number of Rx errors
*       that occurred since the last call to this function.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfSdmaRxErrorCountGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                       devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    gQueueNum = 0;

    /* call cpss api function */
    result = cpssDxChNetIfSdmaRxErrorCountGet(devNum, &gRxErrCount);


    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d", gQueueNum, gRxErrCount.counterArray[gQueueNum]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaRxErrorCountGet
*
* DESCRIPTION:
*       Returns the total number of Rx resource errors that occurred on a given
*       Rx queue . -- SDMA relate
*
* APPLICABLE DEVICES: ALL PCI/SDMA DXCH Devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       rxErrCountPtr  - (pointer to) The total number of Rx resource errors on
*                        the device for all the queues.
*
* RETURNS:
*       GT_OK           - on success,
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on bad parameters
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       The counters returned by this function reflects the number of Rx errors
*       that occurred since the last call to this function.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfSdmaRxErrorCountGetNext
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

    gQueueNum++;

    if (gQueueNum >= CPSS_TC_RANGE_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d", gQueueNum, gRxErrCount.counterArray[gQueueNum]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaRxQueueEnable
*
* DESCRIPTION:
*       Enable/Disable the specified traffic class queue for RX
*       on all packet processors in the system. -- SDMA relate
*
* APPLICABLE DEVICES: ALL PCI/SDMA DXCH Devices
*
* INPUTS:
*        devNum    - device number
*        queue     - traffic class queue
*        enable    - GT_TRUE, enable queue
*                    GT_FALSE, disable queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfSdmaRxQueueEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_U8   queue;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queue = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChNetIfSdmaRxQueueEnable(devNum, queue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfSdmaTxQueueEnable
*
* DESCRIPTION:
*       Enable/Disable the specified traffic class queue for TX
*       on all packet processors in the system. -- SDMA relate
*
* APPLICABLE DEVICES: ALL PCI/SDMA DXCH Devices
*
* INPUTS:
*        devNum    - device number
*        enable    - GT_TRUE, enable queue
*                    GT_FALSE, disable queue
*        queueIdx  - traffic class queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfSdmaTxQueueEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_U8   queue;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queue = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChNetIfSdmaTxQueueEnable(devNum, queue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfDuplicateEnableSet
*
* DESCRIPTION:
*       Enable descriptor duplication (mirror, STC and ingress mirroring
*       to analyzer port when the analyzer port is the CPU), Or Disable any kind
*       of duplication.
*
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum - device number
*       enable -
*               GT_FALSE = Disable any kind of duplication
*
*               GT_TRUE = enable descriptor duplication (mirror, STC and ingress
*                         mirroring to analyzer port when the analyzer port is
*                         the CPU).
*
* OUTPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that were parsed from the
*                   packet
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfDuplicateEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChNetIfDuplicateEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfPortDuplicateToCpuSet
*
* DESCRIPTION:
*       set per ingress port if Packets received from the port that are
*       duplicated and their duplication target is the CPU, ( mirror, STC and
*       ingress mirroring to analyzer port when the analyzer port is the CPU)
*       are will be duplicated or just are only forwarded to their original
*       target.
*       NOTE : port may be "CPU port" (63)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum - device number
*       portNum - port number
*       enable -
*               GT_FALSE = Packets received from the port that are duplicated
*                          and their duplication target is the CPU, (mirror, STC
*                          and ingress mirroring to analyzer port when the
*                          analyzer port is the CPU) are not duplicated and are
*                          only forwarded to their original target.
*
*               GT_TRUE = Packets received from the port that are duplicated and
*                         their duplication target is the CPU, (mirror, STC and
*                         ingress mirroring to analyzer port when the analyzer
*                         port is the CPU) are duplicated and are only forwarded
*                         to their original target and to the CPU.
*
* OUTPUTS:
*       dsaInfoPtr - (pointer to) the DSA parameters that were parsed from the
*                   packet
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or port number
*
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfPortDuplicateToCpuSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_U8   portNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    CONVERT_DEV_PORT_MAC(devNum,portNum);

    /* call cpss api function */
    result = cpssDxChNetIfPortDuplicateToCpuSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpSynSet
*
* DESCRIPTION:
*       Enable/Disable Application Specific CPU Code for TCP SYN packets
*       forwarded to the CPU - TCP_SYN_TO_CPU.
*       The feature enabled for packets if one of conditions are true:
*       1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*       2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum - Device number.
*       enable - GT_TRUE  - enable application specific CPU Code for TCP SYN
*                           packets.
*               GT_FALSE - disable application specific CPU Code for TCP SYN
*                           packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfAppSpecificCpuCodeTcpSynSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChNetIfAppSpecificCpuCodeTcpSynSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpSynGet
*
* DESCRIPTION:
*       Get whether Application Specific CPU Code for TCP SYN packets forwarded
*       to the CPU - TCP_SYN_TO_CPU.
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum - Device number.
*
* OUTPUTS:
*       enablePtr - (pointer to) is the feature enabled
*               GT_TRUE  - enabled ,application specific CPU Code for TCP SYN
*                           packets.
*               GT_FALSE - disabled ,application specific CPU Code for TCP SYN
*                           packets.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR       - one of the parameters in NULL pointer.
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfAppSpecificCpuCodeTcpSynGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChNetIfAppSpecificCpuCodeTcpSynGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/***************Table cpssDxChNetIfAppSpecificCpuCodeIpProtocol************/
static GT_U8 gIpProtIndex;

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet
*
* DESCRIPTION:
*       Set IP Protocol in the IP Protocol CPU Code Table  with specific
*       CPU Code. There are 8 IP Protocols may be defined.
*       The feature enabled for packets if one of conditions are true:
*       1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*       2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (range 0..7)
*       protocol  - IP protocol (range 0..255)
*       cpuCode   - CPU Code for given IP protocol
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index or bad cpuCode
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfAppSpecificCpuCodeIpProtocolSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                      devNum;
    GT_U32                     index;
    GT_U8                      protocol;
    CPSS_NET_RX_CPU_CODE_ENT   cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    index = (GT_U32)inFields[0];
    protocol = (GT_U8)inFields[2];/* inFields[1] for validity field in get*/
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inFields[3];

    /* call cpss api function */
    result = cpssDxChNetIfAppSpecificCpuCodeIpProtocolSet(devNum, index,
                                                          protocol, cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate
*
* DESCRIPTION:
*       Invalidate entry in the IP Protocol CPU Code Table .
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (range 0..7)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                      devNum;
    GT_U32                     index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate(devNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet
*
* DESCRIPTION:
*       Get IP Protocol in the IP Protocol CPU Code Table  with specific
*       CPU Code.
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (range 0..7)
*
* OUTPUTS:
*       validPtr     - (pointer to)valid entry.
*                       GT_FALSE - the entry is not valid
*                       GT_TRUE - the entry is valid
*       protocolPtr  - (pointer to)IP protocol (range 0..255)
*       cpuCodePtr   - (pointer to)CPU Code for given IP protocol
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfAppSpecificCpuCodeIpProtocolGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    GT_BOOL                     valid;
    GT_U8                       protocol;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gIpProtIndex = 0;

    /* call cpss api function */
    result = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(devNum, gIpProtIndex,
                                                          &valid, &protocol,
                                                          &cpuCode);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gIpProtIndex;
    inFields[1] = valid;
    inFields[2] = protocol;
    inFields[3] = cpuCode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet
*
* DESCRIPTION:
*       Get IP Protocol in the IP Protocol CPU Code Table  with specific
*       CPU Code.
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum   - Device number.
*       index     - the index of the IP Protocol (range 0..7)
*
* OUTPUTS:
*       validPtr     - (pointer to)valid entry.
*                       GT_FALSE - the entry is not valid
*                       GT_TRUE - the entry is valid
*       protocolPtr  - (pointer to)IP protocol (range 0..255)
*       cpuCodePtr   - (pointer to)CPU Code for given IP protocol
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfAppSpecificCpuCodeIpProtocolGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                       devNum;
    GT_BOOL                     valid;
    GT_U8                       protocol;
    CPSS_NET_RX_CPU_CODE_ENT    cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gIpProtIndex++;

    if (gIpProtIndex > 7)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChNetIfAppSpecificCpuCodeIpProtocolGet(devNum, gIpProtIndex,
                                                          &valid, &protocol,
                                                          &cpuCode);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gIpProtIndex;
    inFields[1] = valid;
    inFields[2] = protocol;
    inFields[3] = cpuCode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/***********Table cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRange********/
static GT_U32   gRangeIndex;

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
*
* DESCRIPTION:
*       Set range to TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (range 0..15)
*       maxDstPort - the maximum destination port in this range.
*       minDstPort - the minimum destination port in this range
*       packetType - packet type (Unicast/Multicast)
*       protocol   - protocol type (UDP/TCP)
*       cpuCode    - CPU Code Index for this TCP/UDP range
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad rangeIndex or
*                          bad packetType or bad protocol or bad cpuCode
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                            devNum;
    GT_U32                           rangeIndex;
    GT_U16                           minDstPort;
    GT_U16                           maxDstPort;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT packetType;
    CPSS_NET_PROT_ENT                protocol;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    rangeIndex = (GT_U32)inFields[0];
    /* inFields[1] for validity field in get*/
    minDstPort = (GT_U16)inFields[2];
    maxDstPort = (GT_U16)inFields[3];
    packetType = (CPSS_NET_TCP_UDP_PACKET_TYPE_ENT)inFields[4];
    protocol = (CPSS_NET_PROT_ENT)inFields[5];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inFields[6];

    /* call cpss api function */
    result = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet(devNum,
                                                                   rangeIndex,
                                                                   minDstPort,
                                                                   maxDstPort,
                                                                   packetType,
                                                                   protocol,
                                                                   cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
*
* DESCRIPTION:
*       invalidate range to TCP/UPD Destination Port Range CPU Code Table with
*       specific CPU Code. There are 16 ranges may be defined.
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (range 0..15)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad rangeIndex
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                      devNum;
    GT_U32                     index;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate(devNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet
*
* DESCRIPTION:
*       Get range for TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (range 0..15)
*
* OUTPUTS:
*       validPtr  - (pointer to) is the entry valid
*                   GT_FALSE - range is't valid
*                   GT_TRUE  - range is valid
*       maxDstPortPtr - (pointer to)the maximum destination port in this range.
*       minDstPortPtr - (pointer to)the minimum destination port in this range
*       packetTypePtr - (pointer to)packet type (Unicast/Multicast)
*       protocolPtr- (pointer to)protocol type (UDP/TCP)
*                    NOTE : this field will hold valid value only when
*                           (*validPtr) = GT_TRUE
*                           because in HW the "valid" is one of the protocol
*                           options
*       cpuCodePtr - (pointer to)CPU Code Index for this TCP/UDP range
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad rangeIndex
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    GT_BOOL                             valid;
    GT_U16                              minDstPort;
    GT_U16                              maxDstPort;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT    packetType;
    CPSS_NET_PROT_ENT                   protocol;
    CPSS_NET_RX_CPU_CODE_ENT            cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gRangeIndex = 0;

    /* call cpss api function */
    result = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(devNum,
                                                                   gRangeIndex,
                                                                   &valid,
                                                                   &minDstPort,
                                                                   &maxDstPort,
                                                                   &packetType,
                                                                   &protocol,
                                                                   &cpuCode);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gRangeIndex;
    inFields[1] = valid;
    inFields[2] = minDstPort;
    inFields[3] = maxDstPort;
    inFields[4] = packetType;
    inFields[5] = protocol;
    inFields[6] = cpuCode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                  inFields[3], inFields[4], inFields[5],
                                  inFields[6]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet
*
* DESCRIPTION:
*       Get range for TCP/UPD Destination Port Range CPU Code Table with specific
*       CPU Code. There are 16 ranges may be defined.
*       The feature enabled for packets if one of conditions are true:
*        1. MAC DA is found in the FDB and Application Specific CPU Code field
*           of FDB entry is True (see appCpuCodeEn in GT_MAC_ENTRY)
*        2. Packet passed Routing and Application Specific CPU Code field is
*           True in the Route entry (Next Hop)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum     - Device number.
*       rangeIndex - the index of the range (range 0..15)
*
* OUTPUTS:
*       validPtr  - (pointer to) is the entry valid
*                   GT_FALSE - range is't valid
*                   GT_TRUE  - range is valid
*       maxDstPortPtr - (pointer to)the maximum destination port in this range.
*       minDstPortPtr - (pointer to)the minimum destination port in this range
*       packetTypePtr - (pointer to)packet type (Unicast/Multicast)
*       protocolPtr- (pointer to)protocol type (UDP/TCP)
*                    NOTE : this field will hold valid value only when
*                           (*validPtr) = GT_TRUE
*                           because in HW the "valid" is one of the protocol
*                           options
*       cpuCodePtr - (pointer to)CPU Code Index for this TCP/UDP range
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad rangeIndex
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    GT_BOOL                             valid;
    GT_U16                              minDstPort;
    GT_U16                              maxDstPort;
    CPSS_NET_TCP_UDP_PACKET_TYPE_ENT    packetType;
    CPSS_NET_PROT_ENT                   protocol;
    CPSS_NET_RX_CPU_CODE_ENT            cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gRangeIndex++;

    if (gRangeIndex > 15)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGet(devNum,
                                                                   gRangeIndex,
                                                                   &valid,
                                                                   &minDstPort,
                                                                   &maxDstPort,
                                                                   &packetType,
                                                                   &protocol,
                                                                   &cpuCode);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = gRangeIndex;
    inFields[1] = valid;
    inFields[2] = minDstPort;
    inFields[3] = maxDstPort;
    inFields[4] = packetType;
    inFields[5] = protocol;
    inFields[6] = cpuCode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                  inFields[3], inFields[4], inFields[5],
                                  inFields[6]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIpLinkLocalProtSet
*
* DESCRIPTION:
*       Configure CPU code for IPv4 and IPv6 Link Local multicast packets
*       with destination IP in range of 224.0.0.0 ... 224.0.0.255
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum   - Device number.
*       ipVer    - IP version: Ipv4 or Ipv6
*       protocol - Specifies the LSB of IP Link Local multicast protocol
*       cpuCode  - The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad ipVer or bad cpuCode
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeIpLinkLocalProtSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                            devNum;
    CPSS_IP_PROTOCOL_STACK_ENT       ipVer;
    GT_U8                            protocol;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ipVer = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    protocol = (GT_U8)inArgs[2];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeIpLinkLocalProtSet(devNum, ipVer, protocol,
                                                                   cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIpLinkLocalProtGet
*
* DESCRIPTION:
*       get the Configuration CPU code for IPv4 and IPv6 Link Local multicast
*       packets with destination IP in range of 224.0.0.0 ... 224.0.0.255
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum   - Device number.
*       ipVer    - IP version: Ipv4 or Ipv6
*       protocol - Specifies the LSB of IP Link Local multicast protocol
*
* OUTPUTS:
*       cpuCodePtr  - (pointer to) The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_1_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_2_E
*               CPSS_NET_IPV4_IPV6_LINK_LOCAL_MC_DIP_TRP_MRR_3_E
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad ipVer
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeIpLinkLocalProtGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                            devNum;
    CPSS_IP_PROTOCOL_STACK_ENT       ipVer;
    GT_U8                            protocol;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ipVer = (CPSS_IP_PROTOCOL_STACK_ENT)inArgs[1];
    protocol = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeIpLinkLocalProtGet(devNum, ipVer, protocol,
                                                                   &cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cpuCode);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
*
* DESCRIPTION:
*       Configure CPU code for particular IEEE reserved mcast protocol
*       (01-80-C2-00-00-00  ... 01-80-C2-00-00-FF)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum   - Device number.
*       protocol - Specifies the LSB of IEEE Multicast protocol
*       cpuCode  - The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad cpuCode
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeIeeeReservedMcastProtSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                            devNum;
    GT_U8                            protocol;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    protocol = (GT_U8)inArgs[1];
    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet(devNum, protocol,
                                                                  cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet
*
* DESCRIPTION:
*       get the Configuration CPU code for particular IEEE reserved mcast protocol
*       (01-80-C2-00-00-00  ... 01-80-C2-00-00-FF)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum   - Device number.
*       protocol - Specifies the LSB of IEEE Multicast protocol
*
* OUTPUTS:
*       cpuCodePtr  - (pointer to)The CPU Code
*           The 4 CPU acceptable CPU Codes are:
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_1_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_2_E
*               CPSS_NET_IEEE_RSRVD_MULTICAST_ADDR_3_E
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeIeeeReservedMcastProtGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                            devNum;
    GT_U8                            protocol;
    CPSS_NET_RX_CPU_CODE_ENT         cpuCode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    protocol = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet(devNum, protocol,
                                                                  &cpuCode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cpuCode);
    return CMD_OK;
}

/****************Table cpssDxChNetIfCpuCode***************/
static GT_U32 gCPUcodeIndex;

/*******************************************************************************
* cpssDxChNetIfCpuCodeTableSet
*
* DESCRIPTION:
*       Function to set the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*       entryInfoPtr - (pointer to) The entry information
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad cpuCode or bad one of
*                          bad one entryInfoPtr parameters
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeTableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                   devNum;
    CPSS_NET_RX_CPU_CODE_ENT                cpuCode;
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfo;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inFields[0];
    entryInfo.tc =  (GT_U8)inFields[1];
    entryInfo.dp =  (CPSS_DP_LEVEL_ENT)inFields[2];
    entryInfo.truncate =  (GT_BOOL)inFields[3];
    entryInfo.cpuRateLimitMode =
                            (CPSS_NET_CPU_CODE_RATE_LIMIT_MODE_ENT)inFields[4];
    entryInfo.cpuCodeRateLimiterIndex =  (GT_U32)inFields[5];
    entryInfo.cpuCodeStatRateLimitIndex =  (GT_U32)inFields[6];
    entryInfo.designatedDevNumIndex =  (GT_U32)inFields[7];

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeTableSet(devNum, cpuCode, &entryInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeTableGet
*
* DESCRIPTION:
*       Function to get the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*
* OUTPUTS:
*       entryInfoPtr - (pointer to) The entry information
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad cpuCode
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeTableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                   devNum;
    CPSS_NET_RX_CPU_CODE_ENT                cpuCode;
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfo;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    gCPUcodeIndex = 0;

    /*use prv func to go over all ch2 CPU codes only. DSA index = 0-255*/
    result = prvCpssDxChNetIfDsaToCpuCode(gCPUcodeIndex, &cpuCode);

    if(result !=GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeTableGet(devNum, cpuCode, &entryInfo);

    if(result !=GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = cpuCode;
    inFields[1] = entryInfo.tc;
    inFields[2] = entryInfo.dp;
    inFields[3] = entryInfo.truncate;
    inFields[4] = entryInfo.cpuRateLimitMode;
    inFields[5] = entryInfo.cpuCodeRateLimiterIndex;
    inFields[6] = entryInfo.cpuCodeStatRateLimitIndex;
    inFields[7] = entryInfo.designatedDevNumIndex;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                  inFields[3], inFields[4], inFields[5],
                                  inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChNetIfCpuCodeTableGet
*
* DESCRIPTION:
*       Function to get the "CPU code table" entry, for a specific CPU code.
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code (key parameter to access the "CPU code table").
*                 use CPSS_NET_ALL_CPU_OPCODES_E as a wildcard to be used when
*                 need to set all the table entries with the same info.
*
* OUTPUTS:
*       entryInfoPtr - (pointer to) The entry information
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad cpuCode
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeTableGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                   devNum;
    CPSS_NET_RX_CPU_CODE_ENT                cpuCode;
    CPSS_DXCH_NET_CPU_CODE_TABLE_ENTRY_STC  entryInfo;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    gCPUcodeIndex++;

    if(gCPUcodeIndex > 255)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /*use prv func to go over all ch2 CPU codes only. DSA index = 0-255*/
    result = prvCpssDxChNetIfDsaToCpuCode(gCPUcodeIndex, &cpuCode);

    if(result !=GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeTableGet(devNum, cpuCode, &entryInfo);

    if(result !=GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = cpuCode;
    inFields[1] = entryInfo.tc;
    inFields[2] = entryInfo.dp;
    inFields[3] = entryInfo.truncate;
    inFields[4] = entryInfo.cpuRateLimitMode;
    inFields[5] = entryInfo.cpuCodeRateLimiterIndex;
    inFields[6] = entryInfo.cpuCodeStatRateLimitIndex;
    inFields[7] = entryInfo.designatedDevNumIndex;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                  inFields[3], inFields[4], inFields[5],
                                  inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*Table: cpssDxChNetIfCpuCodeStatisticalRateLimits*/
static GT_U32 gStatIndex;
/*******************************************************************************
* cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet
*
* DESCRIPTION:
*       Function to set the "Statistical Rate Limits Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*       devNum  - device number
*       index - index into the Statistical Rate Limits Table (range 0..31)
*       statisticalRateLimit - The statistical rate limit compared to the
*                              32-bit pseudo-random generator (PRNG).
*       NOTE:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
*
*           This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 0 through 0xFFFFFFFF.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_U32  index;
    GT_U32  statisticalRateLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    index = (GT_U32)inFields[0];
    statisticalRateLimit =  (GT_U32)inFields[1];

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet(devNum, index,
                                                          statisticalRateLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet
*
* DESCRIPTION:
*       Function to get the "Statistical Rate Limits Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*       devNum  - device number
*       index - index into the Statistical Rate Limits Table (range 0..31)
*
* OUTPUTS:
*       statisticalRateLimitPtr - (pointer to)The statistical rate limit
*             compared to the 32-bit pseudo-random generator (PRNG).
*       NOTE:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
*
*           This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 0 through 0xFFFFFFFF.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeStatisticalRateLimitsTableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    GT_U32                              statisticalRateLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gStatIndex = 0;

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(devNum,
                                                               gStatIndex,
                                                        &statisticalRateLimit);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d", gStatIndex, statisticalRateLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet
*
* DESCRIPTION:
*       Function to get the "Statistical Rate Limits Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*       devNum  - device number
*       index - index into the Statistical Rate Limits Table (range 0..31)
*
* OUTPUTS:
*       statisticalRateLimitPtr - (pointer to)The statistical rate limit
*             compared to the 32-bit pseudo-random generator (PRNG).
*       NOTE:
*           For all packets to be forwarded to the CPU, set this field to
*           0xFFFFFFFF.
*           For all packets to be dropped, set this field to 0x0.
*
*           This mechanism is based on a hardware-based 32-bit pseudo-random
*           generator (PRNG). The PRNG generates evenly-distributed 32-bit
*           numbers ranging from 0 through 0xFFFFFFFF.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeStatisticalRateLimitsTableGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                               devNum;
    GT_U32                              statisticalRateLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gStatIndex++;

    if(gStatIndex > 31)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeStatisticalRateLimitsTableGet(devNum,
                                                               gStatIndex,
                                                        &statisticalRateLimit);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d", gStatIndex, statisticalRateLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}
/***********Table: cpssDxChNetIfCpuCodeDesignatedDevice*********************/
static GT_U32 gDesIndex;

/*******************************************************************************
* cpssDxChNetIfCpuCodeDesignatedDeviceTableSet
*
* DESCRIPTION:
*       Function to set the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (range 1..7)
*       designatedDevNum - The designated device number (range 0..31)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index or bad
*                          designatedDevNum
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeDesignatedDeviceTableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_U32  index;
    GT_U8   designatedDevNum;
    GT_U8  __Port; /* Dummy for converting. */


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    index = (GT_U32)inFields[0];
    designatedDevNum =  (GT_U8)inFields[1];

    CONVERT_DEV_PORT_DATA_MAC(designatedDevNum,__Port);

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeDesignatedDeviceTableSet(devNum, index,
                                                          designatedDevNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeDesignatedDeviceTableGet
*
* DESCRIPTION:
*       Function to get the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (range 1..7)
*
* OUTPUTS:
*       designatedDevNumPtr - (pointer to)The designated device number
*                              (range 0..31)
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeDesignatedDeviceTableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                              devNum;
    GT_U8                              designatedDevNum;
    GT_U8  __Port; /* Dummy for converting. */


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gDesIndex = 1;

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(devNum,
                                                          gDesIndex,
                                                          &designatedDevNum);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    CONVERT_BACK_DEV_PORT_DATA_MAC(designatedDevNum,__Port);

    /* pack and output table fields */
    fieldOutput("%d%d", gDesIndex, designatedDevNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChNetIfCpuCodeDesignatedDeviceTableGet
*
* DESCRIPTION:
*       Function to get the "designated device Table" entry.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES: ALL DXCH Devices
*
* INPUTS:
*       devNum  - device number
*       index - index into the designated device table (range 1..7)
*
* OUTPUTS:
*       designatedDevNumPtr - (pointer to)The designated device number
*                              (range 0..31)
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad index
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeDesignatedDeviceTableGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                              devNum;
    GT_U8                              designatedDevNum;
    GT_U8  __Port; /* Dummy for converting. */


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gDesIndex++;

    if(gDesIndex > 7)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChNetIfCpuCodeDesignatedDeviceTableGet(devNum,
                                                          gDesIndex,
                                                          &designatedDevNum);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    CONVERT_BACK_DEV_PORT_DATA_MAC(designatedDevNum,__Port);

    /* pack and output table fields */
    fieldOutput("%d%d", gDesIndex, designatedDevNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/************************Table: cpssDxChNetIfCpuCodeRateLimiter************/
static GT_U32   gRateLimiterIndex;

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterTableSet
*
* DESCRIPTION:
*       Configure rate limiter window size and packets limit forwarded to CPU
*       per rate limiter index.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..31.
*       windowSize        - window size for this Rate Limiter in steps of
*                           Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimit          - number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*                           range 0..0xFFFF (16 bits)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad rateLimiterIndex
*       GT_OUT_OF_RANGE - on pktLimit >= 0x10000 or windowSize >= 0x1000
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeRateLimiterTableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U32 rateLimiterIndex;
    GT_U32 windowSize;
    GT_U32 pktLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    rateLimiterIndex = (GT_U32)inFields[0];
    windowSize = (GT_U32)inFields[1];
    pktLimit = (GT_U32)inFields[2];

    /* call port group wrapper function */
    result = pg_wrap_cpssDxChNetIfCpuCodeRateLimiterTableSet(devNum,
                                                     rateLimiterIndex,
                                                     windowSize, pktLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterTableGet
*
* DESCRIPTION:
*       get the Configuration rate limiter window size and packets limit
*       forwarded to CPU per rate limiter index.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..31.
*
* OUTPUTS:
*       windowSizePtr - (pointer to) window size for this Rate Limiter in steps
*                       of Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimitPtr - (pointer to) number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*                           range 0..0xFFFF (16 bits)
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad rateLimiterIndex
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeRateLimiterTableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U32 windowSize;
    GT_U32 pktLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gRateLimiterIndex = 1;

    /* call port group api function */
    result = pg_wrap_cpssDxChNetIfCpuCodeRateLimiterTableGet(devNum,
                                                             gRateLimiterIndex,
                                                             &windowSize,
                                                             &pktLimit);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d", gRateLimiterIndex, windowSize, pktLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterTableGet
*
* DESCRIPTION:
*       get the Configuration rate limiter window size and packets limit
*       forwarded to CPU per rate limiter index.
*       The cpssDxChNetIfCpuCodeTableSet(...) points to this table (for a
*       specific CPU Code)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum            - Device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..31.
*
* OUTPUTS:
*       windowSizePtr - (pointer to) window size for this Rate Limiter in steps
*                       of Rate Limiter Window Resolution set by
*                       cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(...)
*                           range 0..0xFFF (12 bits)
*       pktLimitPtr - (pointer to) number of packets allowed to be sent to CPU
*                           during within the configured windowSize
*                           range 0..0xFFFF (16 bits)
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number or bad rateLimiterIndex
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeRateLimiterTableGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U32 windowSize;
    GT_U32 pktLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    gRateLimiterIndex++;

    if(gRateLimiterIndex > 31)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call port group api function */
    result = pg_wrap_cpssDxChNetIfCpuCodeRateLimiterTableGet(devNum,
                                                             gRateLimiterIndex,
                                                             &windowSize,
                                                             &pktLimit);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d", gRateLimiterIndex, windowSize, pktLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
*
* DESCRIPTION:
*       Set the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum            - Device number.
*       windowResolution - The TO CPU window size resolution -- this field is
*                          the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*                          So this function will round the value to the nearest
*                          PP's option.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*       GT_OUT_OF_RANGE - on windowResolution too large(depends on system clock)
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U32 windowResolution;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    windowResolution = (GT_U32)inArgs[1];

    /* call port group api function */
    result = pg_wrap_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet(devNum,
                                                             windowResolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet
*
* DESCRIPTION:
*       Get the TO CPU Rate Limiter Window Resolution,
*       which is used as steps in Rate Limiter Window size by
*       cpssDxChNetIfCpuCodeRateLimiterTableSet(...)
*
* APPLICABLE DEVICES: ALL DXCH2 Devices
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       windowResolutionPtr - (pointer to)The TO CPU window size resolution --
*                          this field is the number of NanoSeconds.
*                          NOTE:
*                          The PP's actual resolution is in 32 system clock
*                          cycles (for a system clock of 200 MHz = 5nano
*                          per tick --> from 160 nanoseconds up to 327
*                          milliseconds, steps of 160 nanoseconds)
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U32 windowResolution;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call port group api function */
    result = pg_wrap_cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet(devNum,
                                                             &windowResolution);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", windowResolution);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet
*
* DESCRIPTION:
*       Gets the cpu code rate limiter packet counter for specific
*       rate limiter index.
*
* APPLICABLE DEVICES: DxCh2 and above.
*
* INPUTS:
*       devNum      - physical device number.
*       rateLimiterIndex - CPU Code Rate Limiter Index
*                           range 1..31.
*
* OUTPUTS:
*       packetCntrPtr  - pointer to the number of packets forwarded to the CPU
*                       during the current window (set by
*                       cpssDxChNetIfCpuCodeRateLimiterTableSet)
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on HW error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_BAD_PTR               - on NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*   The counter is reset to 0 when the window ends.
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeRateLimiterPacketCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U32 rateLimiterIndex;
    GT_U32 packetCntr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rateLimiterIndex = (GT_U32)inArgs[1];

    /* call port group api function */
    result = pg_wrap_cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet (devNum,
                                                             rateLimiterIndex,
                                                             &packetCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", packetCntr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterDropCntrSet
*
* DESCRIPTION:
*   set the cpu code rate limiter drop counter to a specific value. This counter
*   counts the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES: DxCh2 and above
*
* INPUTS:
*       devNum            - Device number.
*       dropCntrVal       - the value to be configured.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeRateLimiterDropCntrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U32 dropCntrVal;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropCntrVal = (GT_U32)inArgs[1];

    /* call port group api function */
    result = pg_wrap_cpssDxChNetIfCpuCodeRateLimiterDropCntrSet(devNum,
                                                                dropCntrVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfCpuCodeRateLimiterDropCntrGet
*
* DESCRIPTION:
*   get the number of packets forwarded to the CPU and dropped due to any
*   of the CPU rate limiters.
*
* APPLICABLE DEVICES: DxCh2 and above
*
* INPUTS:
*       devNum            - Device number.
*
* OUTPUTS:
*       dropCntrPtr  - (pointer to) The total number of dropped packets
*                           due to any of the CPU rate limiters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - on bad parameters
*       GT_FAIL                  - otherwise
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfCpuCodeRateLimiterDropCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8  devNum;
    GT_U32 dropCntrVal;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call port group api function */
    result = pg_wrap_cpssDxChNetIfCpuCodeRateLimiterDropCntrGet(devNum,
                                                                &dropCntrVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropCntrVal);
    return CMD_OK;
}

/****************************
    TX
*****************************/

/*******************************************************************************
* setBufferArraysOfEqualSize
*
* DESCRIPTION:
*       build GT_BYTE_ARRY from string expanding or trunkating to size
*
* INPUTS:
*       sourceDataPtr       - byte array buffer (hexadecimal string)
*       totalSize           - exact total size of the buffer in bytes
*       bufferSize          - wanted size of each data buffer
*
* OUTPUTS:
*       pcktData    - array of byte array data
*       pcktDataLen - array of byte array size
*       numOfBuffs  - number of buffers the data was splited to.
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS setBufferArraysOfEqualSize
(
    IN GT_U8   *sourceDataPtr,
    IN GT_U32   totalSize,
    IN GT_U32   bufferSize,
    OUT GT_U8   *pcktData[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN],
    OUT GT_U32  pcktDataLen[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN],
    OUT GT_U32  *numOfBuffs

)
{
    GT_U32 element;
    GT_U32 len = totalSize;
    GT_U32 numOfBuffers = len / bufferSize;
    GT_U32 sizeOfLastBuff = len % bufferSize;
    GT_U32 i;

    numOfBuffers = (sizeOfLastBuff > 0) ? numOfBuffers + 1 : numOfBuffers;

    if (numOfBuffers > MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN)
        return GT_NO_RESOURCE;

    if(cmdOsPoolGetBufFreeCnt(txBuffersPoolId) < numOfBuffers)
        return GT_NO_RESOURCE;

    /* if the length of the data is bigger than the length given by the user,
       the data is truncate otherwise it is expanded */

    for(i = 0; i < numOfBuffers ; i++)
    {
        if(len < bufferSize)
            bufferSize = len;
        else
            len = len - bufferSize;

        pcktData[i] = cmdOsPoolGetBuf(txBuffersPoolId);

        /*for every buffer, we will copy the data*/
        for (element = 0; element < bufferSize; element++)
        {
            pcktData[i][element] = (int)(cmdOsStrChr(hexcode, cmdOsToUpper(*sourceDataPtr++)) - (int)hexcode)<< 4;

            pcktData[i][element]+= (int)(cmdOsStrChr(hexcode, cmdOsToUpper(*sourceDataPtr++)) - (int)hexcode);
        }

        pcktDataLen[i] = bufferSize;

    }

    *numOfBuffs = numOfBuffers;

    return GT_OK;
}

/*******************************************************************************
* freeBufferArraysOfEqualSize
*
* DESCRIPTION:
*       Free GT_BYTE_ARRY from string
*
* INPUTS:
*       pcktData    - array of byte array data
*       pcktDataLen - array of byte array size
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
* Toolkit:
*
*******************************************************************************/
static GT_VOID freeBufferArraysOfEqualSize
(
    IN GT_U8   *pcktData[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN],
    IN GT_U32  pcktDataLen[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN]
)
{

    GT_U32 i;
    for (i=0; i<MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN; i++)
    {

        /*nothing to free */
        if((pcktData[i] == NULL) || (pcktDataLen[i] == 0))
            break;

        cmdOsPoolFreeBuf(txBuffersPoolId, pcktData[i]);
   }
}

/*============================
          TX
=============================*/


/*******************************************************************************
* cpssDxChTxStart (general command)
*
* DESCRIPTION:
*     Starts transmition of packets
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfTxStart
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssDxChTxStart();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTxStop (general command)
*
* DESCRIPTION:
*     Stop transmition of packets.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfTxStop
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssDxChTxStop();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTxSetMode (general command)
*
* DESCRIPTION:
*     This command will set the the transmit parameters.
*
* INPUTS:
*     [0] GT_U32 lCyclesNum -
*          The maximum number of loop cycles (-1 = forever)
*
*     [1] GT_U32 lPcktsNum -
*          The maximum number packets all together (-1 = forever)
*
*     [2] GT_U32 lGap -
*          The time to wit between two cycles
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfTxSetMode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lCyclesNum;
    GT_U32 lGap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lCyclesNum = (GT_U32)inArgs[0];
    lGap = (GT_U32)inArgs[1];

    /* call tapi api function */
    result = cmdCpssDxChTxSetMode(lCyclesNum, lGap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTxGetMode (general command)
*
* DESCRIPTION:
*     This command will get the the transmit parameters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     [0] GT_U32 lCyclesNum -
*          The maximum number of loop cycles (-1 = forever)
*
*     [2] GT_U32 lGap -
*          The time to wit between two cycles.
*
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfTxGetMode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lCyclesNum;
    GT_U32 lGap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssDxChTxGetMode(&lCyclesNum, &lGap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", lCyclesNum,lGap);
    return CMD_OK;
}


/*
* Table: TxNetIf
*
* Description:
*     Transmited packet descriptor table.
*
* Comments:
*/
/*******************************************************************************
* cpssDxChNetIfTxFromCpuSet (table command)
*
* DESCRIPTION:
*     Creates new transmit parameters description entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfSetTxNetIfTblEntryFromCpu
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_32   fieldSize[] = {32, 27}; /*Number of fields in each instance*/
    GT_STATUS status;
    DXCH_PKT_DESC_STC * netTxPacketDesc;
    GT_U32   entryIndex=0;
    GT_U32   numOfBuffs;
    GT_U8  __Port; /* Dummy for converting. */
    GT_U8	 devNum;


    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    if(numFields < fieldSize[inArgs[1]])
        return CMD_FIELD_UNDERFLOW;
    if(numFields > fieldSize[inArgs[1]])
        return CMD_FIELD_OVERFLOW;
    
#ifndef __AX_PLATFORM__
	/*
	  * At first ,we pickout devNum
	  * we need devNum when creating DMA pool
	  * each device has different DMA memory pool
	  * by default we use devNum = 0!!!
	  */
	devNum = 0;
	
    switch(inArgs[1])
    {
        case DXCH_TX_BY_VIDX:
			devNum = (GT_U8)inFields[25];
			break;
		case DXCH_TX_BY_PORT:
			devNum = (GT_U8)inFields[23];
			break;
    }
#endif

    /* creat the pool only once */
    if(poolCreated == GT_FALSE)
    {
        /* create pool of buffers from Cache */
        status = cmdOsPoolCreateDmaPool(
                                  TX_BUFFER_SIZE + 64 /* ALIGN_ADDR_CHUNK */,
                                  GT_4_BYTE_ALIGNMENT,
                                  MAX_NUM_OF_BUFFERS_AT_POOL,
                                  GT_TRUE,
                                  &txBuffersPoolId);

        if (status != GT_OK)
        {
                cmdOsPrintf("ERROR-PoolCreateDmaPool\n\n");
                return status;
        }

        poolCreated = GT_TRUE;
    }

    netTxPacketDesc = cmdOsMalloc(sizeof(DXCH_PKT_DESC_STC));
    if (netTxPacketDesc==NULL)
    {
        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }
    cmdOsMemSet(netTxPacketDesc, 0, sizeof(DXCH_PKT_DESC_STC));


    /* Non zero data len */
    if(((GT_U32)inFields[7] == 0))
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }
    status = setBufferArraysOfEqualSize((GT_U8*)inFields[6],
                               (GT_U32)inFields[7],
                               TX_BUFFER_SIZE,
                               netTxPacketDesc->pcktData,
                               netTxPacketDesc->pcktDataLen,
                               &numOfBuffs);


    if (status != GT_OK)
    {
        cmdOsFree(netTxPacketDesc);
        freeBufferArraysOfEqualSize(netTxPacketDesc->pcktData,
                                    netTxPacketDesc->pcktDataLen);
        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }


    netTxPacketDesc->numOfBuffers = numOfBuffs;
    netTxPacketDesc->cmdType = (DXCH_TX_CMD_ENT)inArgs[1];

    netTxPacketDesc->entryId = (GT_U32)inFields[0];
    netTxPacketDesc->txSyncMode = (GT_BOOL)inFields[1];
    netTxPacketDesc->packetIsTagged = (GT_BOOL)inFields[2];

    netTxPacketDesc->pcktsNum = (GT_U32)inFields[3];
    netTxPacketDesc->gap = (GT_U32)inFields[4];
    netTxPacketDesc->waitTime = (GT_U32)inFields[5];
    netTxPacketDesc->numSentPackets = (GT_U32)inFields[8];

    netTxPacketDesc->sdmaInfo.recalcCrc = (GT_BOOL)inFields[9];;
    netTxPacketDesc->sdmaInfo.txQueue = (GT_U8)inFields[10];;
    netTxPacketDesc->sdmaInfo.invokeTxBufferQueueEvent = inFields[11];



    netTxPacketDesc->dsaParam.commonParams.dsaTagType = inFields[12];
    netTxPacketDesc->dsaParam.commonParams.vpt = (GT_U8)inFields[13];
    netTxPacketDesc->dsaParam.commonParams.cfiBit = (GT_8)inFields[14];
    netTxPacketDesc->dsaParam.commonParams.vid = (GT_U16)inFields[15];

    netTxPacketDesc->dsaParam.dsaType = DXCH_NET_DSA_CMD_FROM_CPU_E;

    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.tc = (GT_U8)inFields[16];
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dp = (CPSS_DP_LEVEL_ENT)inFields[17];
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.egrFilterEn = (GT_BOOL)inFields[18];
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.cascadeControl = (GT_BOOL)inFields[19];
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.egrFilterRegistered = (GT_BOOL)inFields[20];
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.srcId = (GT_U32)inFields[21];
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.srcDev = (GT_U8)inFields[22];
    CONVERT_DEV_PORT_DATA_MAC(netTxPacketDesc->dsaParam.dsaInfo.fromCpu.srcDev,__Port);

    switch(inArgs[1])
    {
        case DXCH_TX_BY_VIDX:

            /* user can select CPSS_INTERFACE_VIDX_E = 2 or CPSS_INTERFACE_VID_E = 3 */
            netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.type = inFields[23];
            if(inFields[23] == 2)/*CPSS_INTERFACE_VIDX_E*/
            {
                netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.vidx = (GT_U16)inFields[24];
            }
            else/*CPSS_INTERFACE_VID_E*/
            {
                netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = (GT_U16)inFields[24];
            }
            netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = (GT_U8)inFields[25];
            CONVERT_DEV_PORT_DATA_MAC(netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum,
                                      __Port);
            netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface = (GT_BOOL)inFields[26];
            /* type can be CPSS_INTERFACE_PORT_E = 0 or CPSS_INTERFACE_TRUNK_E = 1*/
            netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type = inFields[27];
            if(netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type == CPSS_INTERFACE_PORT_E)
            {
                netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.devNum = (GT_U8)inFields[28];
                netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.portNum = (GT_U8)inFields[29];

                /* Override Device and Port */
                CONVERT_DEV_PORT_DATA_MAC(netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.devNum,
                                     netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.portNum);

            }
            else/*CPSS_INTERFACE_TRUNK_E*/
            {
                netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.trunkId = (GT_U16)inFields[30];
                CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.trunkId);
            }
            netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs = (GT_BOOL)inFields[31];

            break;

        case DXCH_TX_BY_PORT:
            netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.type = CPSS_INTERFACE_PORT_E;
            netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum = (GT_U8)inFields[23];
            netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum = (GT_U8)inFields[24];

            /* Override Device and Port */
            CONVERT_DEV_PORT_DATA_MAC(netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum,
                                 netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum);

            netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged = (GT_BOOL)inFields[25];
            netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU = (GT_BOOL)inFields[26];

            break;

        default:
            break;
    }

    /* Insert into table */
    status = cmdCpssDxChTxSetPacketDesc(netTxPacketDesc->entryId,
                                netTxPacketDesc,
                                GT_FALSE,   /* Non extended */
                                &entryIndex);

    /* No need to flush packet buffers because the allocation was made from the cache */
    /*
    if(status == GT_OK)
    {
        for (k=0; k < MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN; k++)
        {
            if( cpssDxChTxPacketDescTbl[entryIndex].pcktDataLen[k] == 0 )
                break;

            extDrvMgmtCacheFlush(GT_MGMT_DATA_CACHE_E,
                                 cpssDxChTxPacketDescTbl[entryIndex].pcktData[k],
                                 cpssDxChTxPacketDescTbl[entryIndex].pcktDataLen[k]);
        }
    }*/

    cmdOsFree(netTxPacketDesc);
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*
* Table: TxNetIf
*
* Description:
*     Transmited packet descriptor table.
*
* Comments:
*/
/*******************************************************************************
* netSetTxNetIfTblEntry (table command)
*
* DESCRIPTION:
*     Creates new transmit parameters description entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfSetTxNetIfTblEntryForward
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_32   fieldSize[] = {27, 26}; /*Number of fields in each instance*/
    GT_STATUS status;
    DXCH_PKT_DESC_STC * netTxPacketDesc;
    GT_U32   entryIndex=0;
    GT_U32   numOfBuffs;
    GT_U8  __Port; /* Dummy for converting. */
    GT_U8	 devNum;


    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    if(numFields < fieldSize[inArgs[1]])
        return CMD_FIELD_UNDERFLOW;
    if(numFields > fieldSize[inArgs[1]])
        return CMD_FIELD_OVERFLOW;
    
#ifndef __AX_PLATFORM__
	/*
	  * At first ,we pickout devNum
	  * we need devNum when creating DMA pool
	  * each device has different DMA memory pool
	  * by default we use devNum = 0!!!
	  */
	devNum = 0;
	
    switch(inArgs[1])
    {
        case DXCH_TX_BY_VIDX:
			devNum = (GT_U8)inFields[26];
			break;
		case DXCH_TX_BY_PORT:
			devNum = (GT_U8)inFields[23];
			break;
    }
#endif

    /* creat the pool only once */
    if(poolCreated == GT_FALSE)
    {
        /* create pool of buffers from Cache */
        status = cmdOsPoolCreateDmaPool(
                                  TX_BUFFER_SIZE + 64 /* ALIGN_ADDR_CHUNK */,
                                  GT_4_BYTE_ALIGNMENT,
                                  MAX_NUM_OF_BUFFERS_AT_POOL,
                                  GT_TRUE,
                                  &txBuffersPoolId);
        if (status != GT_OK)
        {
                cmdOsPrintf("ERROR-PoolCreateDmaPool\n\n");
                return status;
        }

        poolCreated = GT_TRUE;
    }

    netTxPacketDesc = cmdOsMalloc(sizeof(DXCH_PKT_DESC_STC));
    if (netTxPacketDesc==NULL)
    {
        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }
    cmdOsMemSet(netTxPacketDesc, 0, sizeof(DXCH_PKT_DESC_STC));


    /* Non zero data len */
    if(((GT_U32)inFields[7] == 0))
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "");
        return CMD_OK;
    }
    status = setBufferArraysOfEqualSize((GT_U8*)inFields[6],
                               (GT_U32)inFields[7],
                               TX_BUFFER_SIZE,
                               netTxPacketDesc->pcktData,
                               netTxPacketDesc->pcktDataLen,
                               &numOfBuffs);

    if (status != GT_OK)
    {
        cmdOsFree(netTxPacketDesc);
        freeBufferArraysOfEqualSize(netTxPacketDesc->pcktData,
                                    netTxPacketDesc->pcktDataLen);
        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }


    netTxPacketDesc->numOfBuffers = numOfBuffs;
    netTxPacketDesc->cmdType = (DXCH_TX_CMD_ENT)inArgs[1];

    netTxPacketDesc->entryId = (GT_U32)inFields[0];
    netTxPacketDesc->txSyncMode = (GT_BOOL)inFields[1];
    netTxPacketDesc->packetIsTagged = (GT_BOOL)inFields[2];

    netTxPacketDesc->pcktsNum = (GT_U32)inFields[3];
    netTxPacketDesc->gap = (GT_U32)inFields[4];
    netTxPacketDesc->waitTime = (GT_U32)inFields[5];
    netTxPacketDesc->numSentPackets = (GT_U32)inFields[8];

    netTxPacketDesc->sdmaInfo.recalcCrc = (GT_BOOL)inFields[9];;
    netTxPacketDesc->sdmaInfo.txQueue = (GT_U8)inFields[10];;
    netTxPacketDesc->sdmaInfo.invokeTxBufferQueueEvent = inFields[11];



    netTxPacketDesc->dsaParam.commonParams.dsaTagType = inFields[12];
    netTxPacketDesc->dsaParam.commonParams.vpt = (GT_U8)inFields[13];
    netTxPacketDesc->dsaParam.commonParams.cfiBit = (GT_8)inFields[14];
    netTxPacketDesc->dsaParam.commonParams.vid = (GT_U16)inFields[15];

    netTxPacketDesc->dsaParam.dsaType = DXCH_NET_DSA_CMD_FORWARD_E;

    netTxPacketDesc->dsaParam.dsaInfo.forward.srcIsTagged = (GT_BOOL)inFields[16];
    netTxPacketDesc->dsaParam.dsaInfo.forward.srcDev = (GT_U8)inFields[17];
    netTxPacketDesc->dsaParam.dsaInfo.forward.srcIsTrunk = (GT_BOOL)inFields[18];
    if(netTxPacketDesc->dsaParam.dsaInfo.forward.srcIsTrunk == GT_TRUE)
    {
        netTxPacketDesc->dsaParam.dsaInfo.forward.source.trunkId = (GT_U16)inFields[19];
        CONVERT_TRUNK_ID_TEST_TO_CPSS_MAC(netTxPacketDesc->dsaParam.dsaInfo.forward.source.trunkId);

        CONVERT_DEV_PORT_DATA_MAC(netTxPacketDesc->dsaParam.dsaInfo.forward.srcDev,
                                  __Port);
    }
    else
    {
        netTxPacketDesc->dsaParam.dsaInfo.forward.source.portNum = (GT_U8)inFields[19];
        CONVERT_DEV_PORT_DATA_MAC(netTxPacketDesc->dsaParam.dsaInfo.forward.srcDev,
                                  netTxPacketDesc->dsaParam.dsaInfo.forward.source.portNum);
    }

    netTxPacketDesc->dsaParam.dsaInfo.forward.srcId = (GT_U32)inFields[20];
    netTxPacketDesc->dsaParam.dsaInfo.forward.egrFilterRegistered = (GT_BOOL)inFields[21];
    netTxPacketDesc->dsaParam.dsaInfo.forward.wasRouted = (GT_BOOL)inFields[22];
    netTxPacketDesc->dsaParam.dsaInfo.forward.qosProfileIndex = (GT_U32)inFields[23];

    switch(inArgs[1])
    {
        case DXCH_TX_BY_VIDX:
            /* user can select CPSS_INTERFACE_VIDX_E = 2 or CPSS_INTERFACE_VID_E = 3 */
            netTxPacketDesc->dsaParam.dsaInfo.forward.dstInterface.type = inFields[24];
            if(inFields[24] == 2)/*CPSS_INTERFACE_VIDX_E*/
            {
                netTxPacketDesc->dsaParam.dsaInfo.forward.dstInterface.vidx = (GT_U16)inFields[25];
            }
            else/*CPSS_INTERFACE_VID_E*/
            {
                netTxPacketDesc->dsaParam.dsaInfo.forward.dstInterface.vlanId = (GT_U16)inFields[25];
            }

            netTxPacketDesc->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum = (GT_U8)inFields[26];
            CONVERT_DEV_PORT_DATA_MAC(netTxPacketDesc->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum,
                                      __Port);


            break;

        case DXCH_TX_BY_PORT:
            netTxPacketDesc->dsaParam.dsaInfo.forward.dstInterface.type = CPSS_INTERFACE_PORT_E;
            netTxPacketDesc->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum = (GT_U8)inFields[24];
            netTxPacketDesc->dsaParam.dsaInfo.forward.dstInterface.devPort.portNum = (GT_U8)inFields[25];

            /* Override Device and Port */
            CONVERT_DEV_PORT_DATA_MAC(netTxPacketDesc->dsaParam.dsaInfo.forward.dstInterface.devPort.devNum,
                                 netTxPacketDesc->dsaParam.dsaInfo.forward.dstInterface.devPort.portNum);

            break;

        default:
            break;
    }

    /* Insert into table */
    status = cmdCpssDxChTxSetPacketDesc(netTxPacketDesc->entryId,
                                netTxPacketDesc,
                                GT_FALSE,   /* Non extended */
                                &entryIndex);

    /* No need to flush packet buffers because the allocation was made from the cache */
    /*
    if(status == GT_OK)
    {
        for (k=0; k < MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN; k++)
        {
            if( cpssDxChTxPacketDescTbl[entryIndex].pcktDataLen[k] == 0 )
                break;

            extDrvMgmtCacheFlush(GT_MGMT_DATA_CACHE_E,
                                 cpssDxChTxPacketDescTbl[entryIndex].pcktData[k],
                                 cpssDxChTxPacketDescTbl[entryIndex].pcktDataLen[k]);
        }
    }*/

    cmdOsFree(netTxPacketDesc);
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfTxFromCpuGetFirst (table command)
*
* DESCRIPTION:
*     Get first entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfGetTxNetIfTblFirstEntryFromCpu
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY lGtBuff;
    GT_U8  __Dev,__Port; /* Dummy for converting. */
    GT_TRUNK_ID __trunkId;/* Dummy for converting. */


    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    {
        GT_STATUS status;
        DXCH_PKT_DESC_STC SpecificPcktDesc;

        /* init to zeros */
        cmdOsMemSet(&SpecificPcktDesc, 0, sizeof(SpecificPcktDesc));

        netTxCurrIndex = 0;

        status = cmdCpssDxChTxBeginGetPacketDesc();
        if (status != GT_OK)
        {
            galtisOutput(outArgs, status, "");
            return CMD_OK;
        }

        status = cmdCpssDxChTxGetPacketDesc(&netTxCurrIndex, DXCH_NET_DSA_CMD_FROM_CPU_E, &SpecificPcktDesc);
        if (status != GT_OK)
        {
            /* indicates that table is empty or (GT_U32)inArgs[0] is invalid */
            if( (status == GT_EMPTY) ||
                (status == GT_NO_SUCH) ||
                (status == GT_NOT_FOUND) )
                galtisOutput(outArgs, GT_OK, "%d", -1);
            else
                galtisOutput(outArgs, status, "");

            cmdCpssDxChTxEndGetPacketDesc();
            return CMD_OK;
        }

        /* Mapping structure to fields */
         inFields[0] = SpecificPcktDesc.entryId;
         inFields[1] = SpecificPcktDesc.txSyncMode;
         inFields[2] = SpecificPcktDesc.packetIsTagged;
         inFields[3] = SpecificPcktDesc.pcktsNum;
         inFields[4] = SpecificPcktDesc.gap;
         inFields[5] = SpecificPcktDesc.waitTime;

         inFields[8] = SpecificPcktDesc.numSentPackets;
         inFields[9] = SpecificPcktDesc.sdmaInfo.recalcCrc;
         inFields[10] = SpecificPcktDesc.sdmaInfo.txQueue;
         inFields[11] = SpecificPcktDesc.sdmaInfo.invokeTxBufferQueueEvent;
         inFields[12] = SpecificPcktDesc.dsaParam.commonParams.dsaTagType;
         inFields[13] = SpecificPcktDesc.dsaParam.commonParams.vpt;
         inFields[14] = SpecificPcktDesc.dsaParam.commonParams.cfiBit;
         inFields[15] = SpecificPcktDesc.dsaParam.commonParams.vid;
         inFields[16] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.tc;
         inFields[17] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dp;
         inFields[18] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.egrFilterEn;
         inFields[19] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.cascadeControl;
         inFields[20] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.egrFilterRegistered;
         inFields[21] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.srcId;
         __Dev = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.srcDev;
         CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
         inFields[22] = __Dev;


         inArgs[1] = SpecificPcktDesc.cmdType;

         switch(SpecificPcktDesc.cmdType)
         {
             case DXCH_TX_BY_VIDX:

                 inFields[23] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.type;
                 if(inFields[23] == 2)/*CPSS_INTERFACE_VIDX_E*/
                 {
                     inFields[24] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.vidx;
                 }
                 else/*CPSS_INTERFACE_VID_E*/
                 {
                     inFields[24] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId;
                 }

                 __Dev = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum;
                 CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
                 inFields[25] = __Dev;
                 inFields[26] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface;
                 inFields[27] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type;
                 if(inFields[27] == 0) /*CPSS_INTERFACE_PORT_E*/
                 {
                     __Dev  = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.devNum;
                     __Port = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.portNum;

                     CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port) ;
                     inFields[28] = __Dev  ;
                     inFields[29] = __Port ;
                     inFields[30] = 0;
                 }
                 else/*CPSS_INTERFACE_TRUNK_E*/
                 {
                     inFields[28] = 0;
                     inFields[29] = 0;
                     __trunkId = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.trunkId;
                     CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(__trunkId);
                     inFields[30] = __trunkId;
                 }

                 inFields[31] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs;

                 break;

             case DXCH_TX_BY_PORT:
                 __Dev  = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum;
                 __Port = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum;

                 CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port);
                 inFields[23] = __Dev  ;
                 inFields[24] = __Port ;

                 inFields[25] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged;
                 inFields[26] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU;

                 break;

             default:
                 break;
         }


         /* Nathan - need to change this code */
         lGtBuff.data = cmdOsMalloc(SpecificPcktDesc.pcktDataLen[0]);
         lGtBuff.length = SpecificPcktDesc.pcktDataLen[0];
         if (!lGtBuff.data)
         {
             galtisOutput(outArgs, GT_NO_RESOURCE, "");
             return CMD_OK;
         }

         cmdOsMemCpy(lGtBuff.data, SpecificPcktDesc.pcktData[0],
             SpecificPcktDesc.pcktDataLen[0]);

         /* End --- need to be changed */


         switch(SpecificPcktDesc.cmdType)
         {
             case DXCH_TX_BY_VIDX:

                 /* pack and output table fields */
                 fieldOutput("%d%d%d%d%d%d%s%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                     inFields[0], inFields[1], inFields[2], inFields[3],
                     inFields[4], inFields[5], galtisBArrayOut(&lGtBuff),
                     inFields[7], inFields[8], inFields[9], inFields[10],
                     inFields[11], inFields[12], inFields[13], inFields[14],
                     inFields[15], inFields[16], inFields[17],inFields[18],
                     inFields[19], inFields[20], inFields[21],inFields[22],
                     inFields[23], inFields[24], inFields[25],inFields[26],
                     inFields[27], inFields[28], inFields[29],inFields[30], inFields[31]);
                 break;

             case DXCH_TX_BY_PORT:
                 fieldOutput("%d%d%d%d%d%d%s%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                     inFields[0], inFields[1], inFields[2], inFields[3],
                     inFields[4], inFields[5], galtisBArrayOut(&lGtBuff),
                     inFields[7], inFields[8], inFields[9], inFields[10],
                     inFields[11], inFields[12], inFields[13], inFields[14],
                     inFields[15], inFields[16], inFields[17],inFields[18],
                     inFields[19], inFields[20], inFields[21],inFields[22],
                     inFields[23], inFields[24], inFields[25],inFields[26]);
                 break;

             default:
                 break;
         }

        galtisOutput(outArgs, status, "%d%f", SpecificPcktDesc.cmdType);

        cmdOsFree(lGtBuff.data);
        cmdOsFree(SpecificPcktDesc.pcktData[0]);
    }
    return CMD_OK;
}

/*******************************************************************************
* netGetTxNetIfTblFirstEntry (table command)
*
* DESCRIPTION:
*     Get first entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfGetTxNetIfTblFirstEntryForward
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY lGtBuff;
    GT_U8  __Dev,__Port; /* Dummy for converting. */
    GT_TRUNK_ID __trunkId;/* Dummy for converting. */


    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    {
        GT_STATUS status;
        DXCH_PKT_DESC_STC SpecificPcktDesc;

        /* init to zeros */
        cmdOsMemSet(&SpecificPcktDesc, 0, sizeof(SpecificPcktDesc));

        netTxCurrIndex = 0;

        status = cmdCpssDxChTxBeginGetPacketDesc();
        if (status != GT_OK)
        {
            galtisOutput(outArgs, status, "");
            return CMD_OK;
        }

        status = cmdCpssDxChTxGetPacketDesc(&netTxCurrIndex, DXCH_NET_DSA_CMD_FORWARD_E, &SpecificPcktDesc);
        if (status != GT_OK)
        {
            /* indicates that table is empty or (GT_U32)inArgs[0] is invalid */
            if( (status == GT_EMPTY) ||
                (status == GT_NO_SUCH) ||
                (status == GT_NOT_FOUND) )
                galtisOutput(outArgs, GT_OK, "%d", -1);
            else
                galtisOutput(outArgs, status, "");

            cmdCpssDxChTxEndGetPacketDesc();
            return CMD_OK;
        }

        /* Mapping structure to fields */

        inArgs[1] = SpecificPcktDesc.cmdType;

        inFields[0] = SpecificPcktDesc.entryId;
        inFields[1] = SpecificPcktDesc.txSyncMode;
        inFields[2] = SpecificPcktDesc.packetIsTagged;
        inFields[3] = SpecificPcktDesc.pcktsNum;
        inFields[4] = SpecificPcktDesc.gap;
        inFields[5] = SpecificPcktDesc.waitTime;

        inFields[8] = SpecificPcktDesc.numSentPackets;
        inFields[9] = SpecificPcktDesc.sdmaInfo.recalcCrc;
        inFields[10] = SpecificPcktDesc.sdmaInfo.txQueue;
        inFields[11] = SpecificPcktDesc.sdmaInfo.invokeTxBufferQueueEvent;

        inFields[12] = SpecificPcktDesc.dsaParam.commonParams.dsaTagType;
        inFields[13] = SpecificPcktDesc.dsaParam.commonParams.vpt;
        inFields[14] = SpecificPcktDesc.dsaParam.commonParams.cfiBit;
        inFields[15] = SpecificPcktDesc.dsaParam.commonParams.vid;

        inFields[16] = SpecificPcktDesc.dsaParam.dsaInfo.forward.srcIsTagged;
        __Dev = SpecificPcktDesc.dsaParam.dsaInfo.forward.srcDev;
        CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
        inFields[17] = __Dev;
        inFields[18] = SpecificPcktDesc.dsaParam.dsaInfo.forward.srcIsTrunk;
        if(SpecificPcktDesc.dsaParam.dsaInfo.forward.srcIsTrunk == GT_TRUE)
        {
            __trunkId = SpecificPcktDesc.dsaParam.dsaInfo.forward.source.trunkId;
            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(__trunkId);
            inFields[19] = __trunkId;
        }
        else
        {
            __Dev   = SpecificPcktDesc.dsaParam.dsaInfo.forward.srcDev;
            __Port  = SpecificPcktDesc.dsaParam.dsaInfo.forward.source.portNum;

            CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port) ;
            inFields[19] = __Port;
        }

        inFields[20] = SpecificPcktDesc.dsaParam.dsaInfo.forward.srcId;
        inFields[21] = SpecificPcktDesc.dsaParam.dsaInfo.forward.egrFilterRegistered;
        inFields[22] = SpecificPcktDesc.dsaParam.dsaInfo.forward.wasRouted;
        inFields[23] = SpecificPcktDesc.dsaParam.dsaInfo.forward.qosProfileIndex;

        switch(SpecificPcktDesc.cmdType)
        {
            case DXCH_TX_BY_VIDX:
                /* user can select CPSS_INTERFACE_VIDX_E = 2 or CPSS_INTERFACE_VID_E = 3 */
                inFields[24] = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.type;
                if(inFields[24] == 2)/*CPSS_INTERFACE_VIDX_E*/
                {
                    inFields[25] = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.vidx;
                }
                else/*CPSS_INTERFACE_VID_E*/
                {
                    inFields[25] = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.vlanId;
                }

                __Dev = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum;
                CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
                inFields[26] = __Dev;

                break;

            case DXCH_TX_BY_PORT:
                __Dev  = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum;
                __Port = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum;

                CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port);
                inFields[24] = __Dev  ;
                inFields[25] = __Port ;

                break;

            default:
                break;
        }

        /* Nathan - need to change this code */
        lGtBuff.data = cmdOsMalloc(SpecificPcktDesc.pcktDataLen[0]);
        lGtBuff.length = SpecificPcktDesc.pcktDataLen[0];
        if (!lGtBuff.data)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }

        cmdOsMemCpy(lGtBuff.data, SpecificPcktDesc.pcktData[0],
            SpecificPcktDesc.pcktDataLen[0]);

        /* End --- need to be changed */

        switch(inArgs[1])
        {
            case DXCH_TX_BY_VIDX:
            fieldOutput("%d%d%d%d%d%d%s%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], galtisBArrayOut(&lGtBuff),
                inFields[7], inFields[8], inFields[9], inFields[10],
                inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17],inFields[18],
                inFields[19], inFields[20], inFields[21],inFields[22],
                inFields[23], inFields[24], inFields[25], inFields[26]);
            break;

            case DXCH_TX_BY_PORT:
            fieldOutput("%d%d%d%d%d%d%s%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], galtisBArrayOut(&lGtBuff),
                inFields[7], inFields[8], inFields[9], inFields[10],
                inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17],inFields[18],
                inFields[19], inFields[20], inFields[21],inFields[22],
                inFields[23], inFields[24], inFields[25]);
            break;

            default:
            break;
        }

        galtisOutput(outArgs, status, "%d%f", SpecificPcktDesc.cmdType);

        cmdOsFree(lGtBuff.data);
        cmdOsFree(SpecificPcktDesc.pcktData[0]);
    }
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfTxFromCpuGetNext (table command)
*
* DESCRIPTION:
*     Get next entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfGetTxNetIfTblNextEntryFromCpu
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY lGtBuff;
    GT_U8  __Dev,__Port; /* Dummy for converting. */
    GT_TRUNK_ID __trunkId;/* Dummy for converting. */


    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    {
        GT_STATUS status;
        DXCH_PKT_DESC_STC SpecificPcktDesc;

        /* init to zeros */
        cmdOsMemSet(&SpecificPcktDesc, 0, sizeof(SpecificPcktDesc));

        netTxCurrIndex++;

        status = cmdCpssDxChTxGetPacketDesc(&netTxCurrIndex, DXCH_NET_DSA_CMD_FROM_CPU_E, &SpecificPcktDesc);
        if (status != GT_OK)
        {
            /* indicates that table is empty or (GT_U32)inArgs[0] is invalid */
            if( (status == GT_EMPTY) ||
                (status == GT_NO_SUCH) ||
                (status == GT_NOT_FOUND) )
                galtisOutput(outArgs, GT_OK, "%d", -1);
            else
                galtisOutput(outArgs, status, "");

            cmdCpssDxChTxEndGetPacketDesc();
            return CMD_OK;
        }

        /* Mapping structure to fields */
         inFields[0] = SpecificPcktDesc.entryId;
         inFields[1] = SpecificPcktDesc.txSyncMode;
         inFields[2] = SpecificPcktDesc.packetIsTagged;
         inFields[3] = SpecificPcktDesc.pcktsNum;
         inFields[4] = SpecificPcktDesc.gap;
         inFields[5] = SpecificPcktDesc.waitTime;

         inFields[8] = SpecificPcktDesc.numSentPackets;
         inFields[9] = SpecificPcktDesc.sdmaInfo.recalcCrc;
         inFields[10] = SpecificPcktDesc.sdmaInfo.txQueue;
         inFields[11] = SpecificPcktDesc.sdmaInfo.invokeTxBufferQueueEvent;
         inFields[12] = SpecificPcktDesc.dsaParam.commonParams.dsaTagType;
         inFields[13] = SpecificPcktDesc.dsaParam.commonParams.vpt;
         inFields[14] = SpecificPcktDesc.dsaParam.commonParams.cfiBit;
         inFields[15] = SpecificPcktDesc.dsaParam.commonParams.vid;
         inFields[16] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.tc;
         inFields[17] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dp;
         inFields[18] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.egrFilterEn;
         inFields[19] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.cascadeControl;
         inFields[20] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.egrFilterRegistered;
         inFields[21] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.srcId;
         __Dev = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.srcDev;
         CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
         inFields[22] = __Dev;


         inArgs[1] = SpecificPcktDesc.cmdType;

         switch(SpecificPcktDesc.cmdType)
         {
             case DXCH_TX_BY_VIDX:

                 inFields[23] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.type;
                 if(inFields[23] == 2)/*CPSS_INTERFACE_VIDX_E*/
                 {
                     inFields[24] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.vidx;
                 }
                 else/*CPSS_INTERFACE_VID_E*/
                 {
                     inFields[24] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.vlanId;
                 }

                 __Dev = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum;
                 CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);

                 inFields[25] = __Dev;
                 inFields[26] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludeInterface;
                 inFields[27] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.type;
                 if(inFields[27] == 0) /*CPSS_INTERFACE_PORT_E*/
                 {
                     __Dev  = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.devNum;
                     __Port = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.devPort.portNum;

                     CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port);
                     inFields[28] = __Dev  ;
                     inFields[29] = __Port ;

                     inFields[30] = 0;
                 }
                 else/*CPSS_INTERFACE_TRUNK_E*/
                 {
                     inFields[28] = 0;
                     inFields[29] = 0;
                     __trunkId = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.excludedInterface.trunkId;
                     CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(__trunkId);
                     inFields[30] = __trunkId;
                 }

                 inFields[31] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.multiDest.mirrorToAllCPUs;

                 break;

             case DXCH_TX_BY_PORT:
                 __Dev  = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum;
                 __Port = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum;

                 CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port);
                 inFields[23] = __Dev  ;
                 inFields[24] = __Port ;

                 inFields[25] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged;
                 inFields[26] = SpecificPcktDesc.dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU;

                 break;

             default:
                 break;
         }


         /* Nathan - need to change this code */
         lGtBuff.data = cmdOsMalloc(SpecificPcktDesc.pcktDataLen[0]);
         lGtBuff.length = SpecificPcktDesc.pcktDataLen[0];
         if (!lGtBuff.data)
         {
             galtisOutput(outArgs, GT_NO_RESOURCE, "");
             return CMD_OK;
         }

         cmdOsMemCpy(lGtBuff.data, SpecificPcktDesc.pcktData[0],
             SpecificPcktDesc.pcktDataLen[0]);

         /* End --- need to be changed */


         switch(SpecificPcktDesc.cmdType)
         {
             case DXCH_TX_BY_VIDX:

                 /* pack and output table fields */
                 fieldOutput("%d%d%d%d%d%d%s%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                     inFields[0], inFields[1], inFields[2], inFields[3],
                     inFields[4], inFields[5], galtisBArrayOut(&lGtBuff),
                     inFields[7], inFields[8], inFields[9], inFields[10],
                     inFields[11], inFields[12], inFields[13], inFields[14],
                     inFields[15], inFields[16], inFields[17],inFields[18],
                     inFields[19], inFields[20], inFields[21],inFields[22],
                     inFields[23], inFields[24], inFields[25],inFields[26],
                     inFields[27], inFields[28], inFields[29],inFields[30], inFields[31]);
                 break;

             case DXCH_TX_BY_PORT:
                 fieldOutput("%d%d%d%d%d%d%s%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                     inFields[0], inFields[1], inFields[2], inFields[3],
                     inFields[4], inFields[5], galtisBArrayOut(&lGtBuff),
                     inFields[7], inFields[8], inFields[9], inFields[10],
                     inFields[11], inFields[12], inFields[13], inFields[14],
                     inFields[15], inFields[16], inFields[17],inFields[18],
                     inFields[19], inFields[20], inFields[21],inFields[22],
                     inFields[23], inFields[24], inFields[25],inFields[26]);
                 break;

             default:
                 break;
         }

        galtisOutput(outArgs, GT_OK, "%d%f", SpecificPcktDesc.cmdType);

        cmdOsFree(lGtBuff.data);
        cmdOsFree(SpecificPcktDesc.pcktData[0]);
    }
    return CMD_OK;
}

/*******************************************************************************
* netGetTxNetIfTblNextEntry (table command)
*
* DESCRIPTION:
*     Get next entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfGetTxNetIfTblNextEntryForward
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY lGtBuff;
    GT_U8  __Dev,__Port; /* Dummy for converting. */
    GT_TRUNK_ID __trunkId;/* Dummy for converting. */


    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    {
        GT_STATUS status;
        DXCH_PKT_DESC_STC SpecificPcktDesc;

        /* init to zeros */
        cmdOsMemSet(&SpecificPcktDesc, 0, sizeof(SpecificPcktDesc));

        netTxCurrIndex++;

        status = cmdCpssDxChTxGetPacketDesc(&netTxCurrIndex, DXCH_NET_DSA_CMD_FORWARD_E, &SpecificPcktDesc);
        if (status != GT_OK)
        {
            /* indicates that table is empty or (GT_U32)inArgs[0] is invalid */
            if( (status == GT_EMPTY) ||
                (status == GT_NO_SUCH) ||
                (status == GT_NOT_FOUND) )
                galtisOutput(outArgs, GT_OK, "%d", -1);
            else
                galtisOutput(outArgs, status, "");

            cmdCpssDxChTxEndGetPacketDesc();
            return CMD_OK;
        }

        /* Mapping structure to fields */

        inArgs[1] = SpecificPcktDesc.cmdType;

        inFields[0] = SpecificPcktDesc.entryId;
        inFields[1] = SpecificPcktDesc.txSyncMode;
        inFields[2] = SpecificPcktDesc.packetIsTagged;
        inFields[3] = SpecificPcktDesc.pcktsNum;
        inFields[4] = SpecificPcktDesc.gap;
        inFields[5] = SpecificPcktDesc.waitTime;

        inFields[8] = SpecificPcktDesc.numSentPackets;
        inFields[9] = SpecificPcktDesc.sdmaInfo.recalcCrc;
        inFields[10] = SpecificPcktDesc.sdmaInfo.txQueue;
        inFields[11] = SpecificPcktDesc.sdmaInfo.invokeTxBufferQueueEvent;

        inFields[12] = SpecificPcktDesc.dsaParam.commonParams.dsaTagType;
        inFields[13] = SpecificPcktDesc.dsaParam.commonParams.vpt;
        inFields[14] = SpecificPcktDesc.dsaParam.commonParams.cfiBit;
        inFields[15] = SpecificPcktDesc.dsaParam.commonParams.vid;

        inFields[16] = SpecificPcktDesc.dsaParam.dsaInfo.forward.srcIsTagged;
        __Dev = SpecificPcktDesc.dsaParam.dsaInfo.forward.srcDev;
        CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
        inFields[17] = __Dev;
        inFields[18] = SpecificPcktDesc.dsaParam.dsaInfo.forward.srcIsTrunk;
        if(SpecificPcktDesc.dsaParam.dsaInfo.forward.srcIsTrunk == GT_TRUE)
        {
            __trunkId = SpecificPcktDesc.dsaParam.dsaInfo.forward.source.trunkId;
            CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(__trunkId);
            inFields[19] = __trunkId;
        }
        else
        {
            __Dev   = SpecificPcktDesc.dsaParam.dsaInfo.forward.srcDev;
            __Port  = SpecificPcktDesc.dsaParam.dsaInfo.forward.source.portNum;

            CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port) ;
            inFields[19] = __Port;
        }

        inFields[20] = SpecificPcktDesc.dsaParam.dsaInfo.forward.srcId;
        inFields[21] = SpecificPcktDesc.dsaParam.dsaInfo.forward.egrFilterRegistered;
        inFields[22] = SpecificPcktDesc.dsaParam.dsaInfo.forward.wasRouted;
        inFields[23] = SpecificPcktDesc.dsaParam.dsaInfo.forward.qosProfileIndex;

        switch(SpecificPcktDesc.cmdType)
        {
            case DXCH_TX_BY_VIDX:
                /* user can select CPSS_INTERFACE_VIDX_E = 2 or CPSS_INTERFACE_VID_E = 3 */
                inFields[24] = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.type;
                if(inFields[24] == 2)/*CPSS_INTERFACE_VIDX_E*/
                {
                    inFields[25] = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.vidx;
                }
                else/*CPSS_INTERFACE_VID_E*/
                {
                    inFields[25] = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.vlanId;
                }

                __Dev = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum;
                CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
                inFields[26] = __Dev;

                break;

            case DXCH_TX_BY_PORT:
                __Dev = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.devPort.devNum;
                __Port = SpecificPcktDesc.dsaParam.dsaInfo.forward.dstInterface.devPort.portNum;

                CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port);
                inFields[24] = __Dev  ;
                inFields[25] = __Port ;

                break;

            default:
                break;
        }

        /* Nathan - need to change this code */
        lGtBuff.data = cmdOsMalloc(SpecificPcktDesc.pcktDataLen[0]);
        lGtBuff.length = SpecificPcktDesc.pcktDataLen[0];
        if (!lGtBuff.data)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }

        cmdOsMemCpy(lGtBuff.data, SpecificPcktDesc.pcktData[0],
            SpecificPcktDesc.pcktDataLen[0]);

        /* End --- need to be changed */

        switch(inArgs[1])
        {
            case DXCH_TX_BY_VIDX:
            fieldOutput("%d%d%d%d%d%d%s%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], galtisBArrayOut(&lGtBuff),
                inFields[7], inFields[8], inFields[9], inFields[10],
                inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17],inFields[18],
                inFields[19], inFields[20], inFields[21],inFields[22],
                inFields[23], inFields[24], inFields[25], inFields[26]);
            break;

            case DXCH_TX_BY_PORT:
            fieldOutput("%d%d%d%d%d%d%s%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[3],
                inFields[4], inFields[5], galtisBArrayOut(&lGtBuff),
                inFields[7], inFields[8], inFields[9], inFields[10],
                inFields[11], inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17],inFields[18],
                inFields[19], inFields[20], inFields[21],inFields[22],
                inFields[23], inFields[24], inFields[25]);
            break;

            default:
            break;
        }
        galtisOutput(outArgs, GT_OK, "%d%f", SpecificPcktDesc.cmdType);

        cmdOsFree(lGtBuff.data);
        cmdOsFree(SpecificPcktDesc.pcktData[0]);
    }
    return CMD_OK;
}


/*******************************************************************************
* cpssTransmitNetworkIfDelete (table command)
*
* DESCRIPTION:
*     Delete packet descriptor entry from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfDeleteTxNetIfTblEntryFromCpu
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;
    GT_32   fieldSizeFromCpu[] = {32, 27}; /*Number of fields in each instance*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    if(numFields < fieldSizeFromCpu[inArgs[1]])
        return CMD_FIELD_UNDERFLOW;
    if(numFields > fieldSizeFromCpu[inArgs[1]])
        return CMD_FIELD_OVERFLOW;

    status = cmdCpssDxChTxDelPacketDesc((GT_U32)inFields[0], DXCH_NET_DSA_CMD_FROM_CPU_E);
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssTransmitNetworkIfDelete (table command)
*
* DESCRIPTION:
*     Delete packet descriptor entry from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfDeleteTxNetIfTblEntryForward
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS status;
    GT_32   fieldSizeFromCpu[] = {27, 26}; /*Number of fields in each instance*/

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


    if(numFields < fieldSizeFromCpu[inArgs[1]])
        return CMD_FIELD_UNDERFLOW;
    if(numFields > fieldSizeFromCpu[inArgs[1]])
        return CMD_FIELD_OVERFLOW;

    status = cmdCpssDxChTxDelPacketDesc((GT_U32)inFields[0], DXCH_NET_DSA_CMD_FORWARD_E);
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfTxFromCpuClear (table command)
*
* DESCRIPTION:
*     Clear all packet descriptors entries from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfClearTxNetIfTableFromCpu
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

    status = cmdCpssDxChTxClearPacketDesc(DXCH_NET_DSA_CMD_FROM_CPU_E);
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChNetIfTxForwardClear (table command)
*
* DESCRIPTION:
*     Clear all packet descriptors entries from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] DXCH_TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfClearTxNetIfTableForward
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

    status = cmdCpssDxChTxClearPacketDesc(DXCH_NET_DSA_CMD_FORWARD_E);
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}


static GT_U32 netRxExtLastIndex;

/*******************************************************************************
* wrCpssDxChNetIfRxPacketTableGet (table command)
*
* DESCRIPTION:
*     Get first/next entry from the DXCH rxNetIf table. (always "TO_CPU" dsa tag !)
*
* INPUTS:
*     none
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*     getNext            - GT_TRUE - "get next"
*                          GT_FALSE - "get first"
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfRxPacketTableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    IN  GT_BOOL getNext
)
{
    GT_BYTE_ARRY byteArry;
    GT_STATUS rc;
    GT_U32 packetActualLength;
    GT_U8  packetBuff[GALTIS_RX_BUFFER_MAX_SIZE_CNS];
    GT_U32 buffLen = GALTIS_RX_BUFFER_MAX_SIZE_CNS;
    GT_U8  devNum;
    GT_U8  queue;
    CPSS_DXCH_NET_RX_PARAMS_STC rxParam;
    GT_U8*      packetString;
    GT_U32  ii;
    CPSS_DXCH_NET_DSA_COMMON_STC         *commonParamsPtr;
    CPSS_DXCH_NET_DSA_TO_CPU_STC         *toCpuPtr;
    CPSS_DXCH_NET_DSA_TO_ANALYZER_STC    *toAnalyzerPtr;
    CPSS_DXCH_NET_DSA_FORWARD_STC        *forwardPtr;
    GT_U8  __Dev,__Port; /* Dummy for converting. */
    GT_U32   portOrTrunkId;
    GT_TRUNK_ID __trunkId;/* Dummy for converting. */


    /* check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (getNext == GT_FALSE)
    {
        rc = cmdCpssRxPkGetFirst(&netRxExtLastIndex,
                                 packetBuff, &buffLen, &packetActualLength,
                                 &devNum,&queue,
                                 &rxParam);
    }
    else
    {
        rc = cmdCpssRxPkGetNext(&netRxExtLastIndex,
                                 packetBuff, &buffLen, &packetActualLength,
                                 &devNum,&queue,
                                 &rxParam);
    }

    if (rc != GT_OK)
    {
        if (rc == GT_NO_MORE)
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
        }
        else
        {
            galtisOutput(outArgs, rc, "");
        }

        return CMD_OK;
    }


    commonParamsPtr = &rxParam.dsaParam.commonParams;
    toCpuPtr        = &rxParam.dsaParam.dsaInfo.toCpu;
    forwardPtr      = &rxParam.dsaParam.dsaInfo.forward;
    toAnalyzerPtr   = &rxParam.dsaParam.dsaInfo.toAnalyzer;

    byteArry.data    = packetBuff;
    byteArry.length  = buffLen;

    /* initialize the index of "inFields" parameters */
    ii = 0;

    inFields[ii++] = devNum;                                      /*0*/
    inFields[ii++] = queue;                                       /*1*/

    ii++;/* one for the packetString */
    packetString = galtisBArrayOut(&byteArry);                    /*2*/

    inFields[ii++] = packetActualLength;                          /*3*/

    inFields[ii++] = commonParamsPtr->dsaTagType;                 /*4*/
    inFields[ii++] = commonParamsPtr->vpt;                        /*5*/
    inFields[ii++] = commonParamsPtr->cfiBit;                     /*6*/
    inFields[ii++] = commonParamsPtr->vid;                        /*7*/

    switch (rxParam.dsaParam.dsaType)
    {
        case CPSS_DXCH_NET_DSA_CMD_TO_CPU_E:
            __Dev  = toCpuPtr->devNum;
            if (toCpuPtr->srcIsTrunk == GT_FALSE)
            {
                __Port = toCpuPtr->interface.portNum;
                CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
                portOrTrunkId = __Port;
            }
            else
            {
                /* TRUNK case, convert device with dummy __Port */
                __Port = 0;
                CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
                __trunkId = toCpuPtr->interface.srcTrunkId;
                CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(__trunkId);
                portOrTrunkId = __trunkId;
            }
            inFields[ii++] = toCpuPtr->isEgressPipe;              /*8*/
            inFields[ii++] = toCpuPtr->isTagged;                  /*9*/
            inFields[ii++] = __Dev;                               /*10*/
            inFields[ii++] = toCpuPtr->srcIsTrunk;                /*11*/
            inFields[ii++] = portOrTrunkId;                       /*12*/
            inFields[ii++] = toCpuPtr->cpuCode;                   /*13*/
            inFields[ii++] = toCpuPtr->wasTruncated;              /*14*/
            inFields[ii++] = toCpuPtr->originByteCount;           /*15*/
            break;
        case CPSS_DXCH_NET_DSA_CMD_FORWARD_E:
            __Dev  = forwardPtr->srcDev;
            if (forwardPtr->srcIsTrunk == GT_FALSE)
            {
                __Port = forwardPtr->source.portNum;
                CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
                portOrTrunkId = __Port;
            }
            else
            {
                /* TRUNK case, convert device with dummy __Port */
                __Port = 0;
                CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
                __trunkId = forwardPtr->source.trunkId;
                CONVERT_TRUNK_ID_CPSS_TO_TEST_MAC(__trunkId);
                portOrTrunkId = __trunkId;
            }
            inFields[ii++] = 0;                                   /*8*/
            inFields[ii++] = forwardPtr->srcIsTagged;             /*9*/
            inFields[ii++] = __Dev;                               /*10*/
            inFields[ii++] = forwardPtr->srcIsTrunk;              /*11*/
            inFields[ii++] = portOrTrunkId;                       /*12*/
            inFields[ii++] = 0;                                   /*13*/
            inFields[ii++] = 0;                                   /*14*/
            inFields[ii++] = 0;                                   /*15*/
            break;
        case CPSS_DXCH_NET_DSA_CMD_TO_ANALYZER_E:
            __Dev  = toAnalyzerPtr->devPort.devNum;
            __Port = toAnalyzerPtr->devPort.portNum;
            CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev,__Port);
            inFields[ii++] = 0;                                   /*8*/
            inFields[ii++] = toAnalyzerPtr->isTagged;             /*9*/
            inFields[ii++] = __Dev;                               /*10*/
            inFields[ii++] = 0;                                   /*11*/
            inFields[ii++] = __Port;                              /*12*/
            inFields[ii++] = 0;                                   /*13*/
            inFields[ii++] = 0;                                   /*14*/
            inFields[ii++] = 0;                                   /*15*/
            break;
        default:
            inFields[ii++] = 0;                                   /*8*/
            inFields[ii++] = 0;                                   /*9*/
            inFields[ii++] = 0;                                   /*10*/
            inFields[ii++] = 0;                                   /*11*/
            inFields[ii++] = 0;                                   /*12*/
            inFields[ii++] = 0;                                   /*13*/
            inFields[ii++] = 0;                                   /*14*/
            inFields[ii++] = 0;                                   /*15*/
            break;
    }


    /* pack and output table fields */
    /*            0 1 2 3 4 5 6 7 8 9 */
    /*                                0 1 2 3 4 5*/
    fieldOutput("%d%d%s%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],
                inFields[1],
                packetString,
                inFields[3],
                inFields[4],
                inFields[5],
                inFields[6],
                inFields[7],
                inFields[8],
                inFields[9],
                inFields[10],
                inFields[11],
                inFields[12],
                inFields[13],
                inFields[14],
                inFields[15]
    );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* wrCmdDxChRxStartCapture (general command)
*
* DESCRIPTION:
*     Start collecting the received packets
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCmdDxChRxStartCapture
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssRxStartCapture();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCmdDxChRxStopCapture (general command)
*
* DESCRIPTION:
*     Stop collecting the received packets
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCmdDxChRxStopCapture
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssRxStopCapture();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCmdDxChRxSetMode (general command)
*
* DESCRIPTION:
*     set received packets collection mode and parameters
*
* INPUTS:
*     [0] GALTIS_RX_MODE_ENT lRxMode -
*          collection mode
*
*     [1] GT_U32 lBuferSize -
*          buffer size
*
*     [2] GT_U32 lNumOfEnteries -
*          number of entries
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCmdDxChRxSetMode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GALTIS_RX_MODE_ENT lRxMode;
    GT_U32 lBuferSize;
    GT_U32 lNumOfEnteries;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lRxMode = (GALTIS_RX_MODE_ENT)inArgs[0];
    lBuferSize = (GT_U32)inArgs[1];
    lNumOfEnteries = (GT_U32)inArgs[2];

    /* call tapi api function */
    result = cmdCpssRxSetMode(lRxMode, lBuferSize, lNumOfEnteries);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCmdDxChRxGetMode (general command)
*
* DESCRIPTION:
*     Get received packets collection mode and parameters
*
* INPUTS:
*     none
*
* OUTPUTS:
*     [0] GALTIS_RX_MODE_ENT lMode -
*          GT_RX_CYCLIC or GT_RX_ONCE
*
*     [1] GT_U32 lBuffSize -
*          packet buffer size
*
*     [2] GT_U32 lNumEntries -
*          number of entries
*
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCmdDxChRxGetMode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GALTIS_RX_MODE_ENT lMode;
    GT_U32 lBuffSize;
    GT_U32 lNumEntries;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssRxGetMode(&lMode, &lBuffSize, &lNumEntries);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", lMode, lBuffSize,
        lNumEntries);
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChNetIfRxPacketTableGetFirst (table command)
*
* DESCRIPTION:
*     Get first entry from the DXCH rxNetIf table. (always "TO_CPU" dsa tag !)
*
* INPUTS:
*     none
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfRxPacketTableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChNetIfRxPacketTableGet(inArgs,inFields,numFields,outArgs,
                    GT_FALSE);/* get first */
}

/*******************************************************************************
* wrCpssDxChNetIfRxPacketTableGetNext (table command)
*
* DESCRIPTION:
*     Get next entry from the DXCH rxNetIf table. (always "TO_CPU" dsa tag !)
*
* INPUTS:
*     none
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfRxPacketTableGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChNetIfRxPacketTableGet(inArgs,inFields,numFields,outArgs,
                    GT_TRUE);/* get next */
}

/*******************************************************************************
* wrCpssDxChNetIfRxPacketTableClear (table command)
*
* DESCRIPTION:
*     clear all entries from the DXCH rxNetIf table.
*
* INPUTS:
*     none
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssDxChNetIfRxPacketTableClear
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

    cmdCpssRxPktClearTbl();
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChNetIfFromCpuDpSet
*
* DESCRIPTION:
*       Set DP for from CPU DSA tag packets.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       inArgs[0]            - device number
*       inArgs[1]            - drop precedence level
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       CMD_OK                    - on success
*       CMD_AGENT_ERROR           - on failure
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNetIfFromCpuDpSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS               result;
    IN GT_U8                devNum;
    IN CPSS_DP_LEVEL_ENT    dpLevel;

    /* Check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dpLevel = (CPSS_DP_LEVEL_ENT)inArgs[1];

    /* call cpss API function */
    result = cpssDxChNetIfFromCpuDpSet(devNum, dpLevel);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChNetIfFromCpuDpGet
*
* DESCRIPTION:
*       Get DP for from CPU DSA tag packets.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       inArgs[0]         - device number
*
* OUTPUTS:
*       outArgs[1]        - pointer to drop precedence level
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - invalid hardware value for drop precedence level
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNetIfFromCpuDpGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS          result;

    GT_U8              devNum;
    CPSS_DP_LEVEL_ENT  dpLevelPtr;

    /* Check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum = (GT_U8)inArgs[0];

    /* map input arguments to locals */
    result = cpssDxChNetIfFromCpuDpGet(devNum, &dpLevelPtr);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "%d", dpLevelPtr);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChNetIfSdmaRxResourceErrorModeSet
*
* DESCRIPTION:
*       Set a bit per TC queue which defines the behavior in case of
*       RX resource error
*
* APPLICABLE DEVICES: All DxCh devices.
*
* INPUTS:
*       devNum     - device number.
*       queue      - traffic class queue
*       mode       - current packet mode: retry to send or abort
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on bad device number, queue, mode
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_NOT_SUPPORTED         - the request is not supported
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChNetIfSdmaRxResourceErrorModeSet
(

    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;
    IN  GT_U8                                   devNum;
    IN  GT_U8                                   queue;
    IN  CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT   mode;

    /* Check for valid arguments */
    if (!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queue  = (GT_U8)inArgs[1];
    mode   = (CPSS_DXCH_NET_RESOURCE_ERROR_MODE_ENT)inArgs[2];

    /* call cpss API function */
    result = cpssDxChNetIfSdmaRxResourceErrorModeSet(devNum, queue, mode);

    /* pack output arguments to GalTis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChNetIfInit",
        &wrCpssDxChNetIfInit,
        1, 0},

    {"cpssDxChNetIfRemove",
        &wrCpssDxChNetIfRemove,
        1, 0},

    {"cpssDxChNetIfPrePendTwoBytesHeaderSet",
        &wrCpssDxChNetIfPrePendTwoBytesHeaderSet,
        2, 0},

    {"cpssDxChNetIfPrePendTwoBytesHeaderGet",
        &wrCpssDxChNetIfPrePendTwoBytesHeaderGet,
        1, 0},

    {"cpssDxChNetIfSdmaRxCountersGetFirst",
        &wrCpssDxChNetIfSdmaRxCountersGetFirst,
        1, 0},

    {"cpssDxChNetIfSdmaRxCountersGetNext",
        &wrCpssDxChNetIfSdmaRxCountersGetNext,
        1, 0},

    {"cpssDxChNetIfSdmaRxErrorCountGetFirst",
        &wrCpssDxChNetIfSdmaRxErrorCountGetFirst,
        1, 0},

    {"cpssDxChNetIfSdmaRxErrorCountGetNext",
        &wrCpssDxChNetIfSdmaRxErrorCountGetNext,
        1, 0},

    {"cpssDxChNetIfSdmaRxQueueEnable",
        &wrCpssDxChNetIfSdmaRxQueueEnable,
        3, 0},

    {"cpssDxChNetIfSdmaTxQueueEnable",
        &wrCpssDxChNetIfSdmaTxQueueEnable,
        3, 0},

    {"cpssDxChNetIfDuplicateEnableSet",
        &wrCpssDxChNetIfDuplicateEnableSet,
        2, 0},

    {"cpssDxChNetIfPortDuplicateToCpuSet",
        &wrCpssDxChNetIfPortDuplicateToCpuSet,
        3, 0},

    {"cpssDxChNetIfAppSpecificCpuCodeTcpSynSet",
        &wrCpssDxChNetIfAppSpecificCpuCodeTcpSynSet,
        2, 0},

    {"cpssDxChNetIfAppSpecificCpuCodeTcpSynGet",
        &wrCpssDxChNetIfAppSpecificCpuCodeTcpSynGet,
        1, 0},

    {"cpssDxChNetIfAppSpcfcCpuCodeIpPrtclSet",
        &wrCpssDxChNetIfAppSpecificCpuCodeIpProtocolSet,
        1, 4},

    {"cpssDxChNetIfAppSpcfcCpuCodeIpPrtclInvalidate",
        &wrCpssDxChNetIfAppSpecificCpuCodeIpProtocolInvalidate,
        2, 0},

    {"cpssDxChNetIfAppSpcfcCpuCodeIpPrtclGetFirst",
        &wrCpssDxChNetIfAppSpecificCpuCodeIpProtocolGetFirst,
        1, 0},

    {"cpssDxChNetIfAppSpcfcCpuCodeIpPrtclGetNext",
        &wrCpssDxChNetIfAppSpecificCpuCodeIpProtocolGetNext,
        1, 0},

    {"cpssDxChNetIfAppSpcfcCpuCodeDstPrtRngSet",
        &wrCpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeSet,
        1, 7},

    {"cpssDxChNetIfAppSpcfcCpuCodeDstPrtRngInvlidt",
        &wrCpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeInvalidate,
        2, 0},

    {"cpssDxChNetIfAppSpcfcCpuCodeDstPrtRngGetFirst",
        &wrCpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGetFirst,
        1, 0},

    {"cpssDxChNetIfAppSpcfcCpuCodeDstPrtRngGetNext",
        &wrCpssDxChNetIfAppSpecificCpuCodeTcpUdpDestPortRangeGetNext,
        1, 0},

    {"cpssDxChNetIfCpuCodeIpLinkLocalProtSet",
        &wrCpssDxChNetIfCpuCodeIpLinkLocalProtSet,
        4, 0},

    {"cpssDxChNetIfCpuCodeIpLinkLocalProtGet",
        &wrCpssDxChNetIfCpuCodeIpLinkLocalProtGet,
        3, 0},

    {"cpssDxChNetIfCpuCodeIeeeReservedMcastProtSet",
        &wrCpssDxChNetIfCpuCodeIeeeReservedMcastProtSet,
        3, 0},

    {"cpssDxChNetIfCpuCodeIeeeReservedMcastProtGet",
        &wrCpssDxChNetIfCpuCodeIeeeReservedMcastProtGet,
        2, 0},

    {"cpssDxChNetIfCpuCodeSet",
        &wrCpssDxChNetIfCpuCodeTableSet,
        1, 8},

    {"cpssDxChNetIfCpuCodeGetFirst",
        &wrCpssDxChNetIfCpuCodeTableGetFirst,
        1, 0},

    {"cpssDxChNetIfCpuCodeGetNext",
        &wrCpssDxChNetIfCpuCodeTableGetNext,
        1, 0},

    {"cpssDxChNetIfCpuCodeSttstclRateLimitsSet",
        &wrCpssDxChNetIfCpuCodeStatisticalRateLimitsTableSet,
        1, 2},

    {"cpssDxChNetIfCpuCodeSttstclRateLimitsGetFirst",
        &wrCpssDxChNetIfCpuCodeStatisticalRateLimitsTableGetFirst,
        1, 0},

    {"cpssDxChNetIfCpuCodeSttstclRateLimitsGetNext",
        &wrCpssDxChNetIfCpuCodeStatisticalRateLimitsTableGetNext,
        1, 0},

    {"cpssDxChNetIfCpuCodeDesignatedDeviceSet",
        &wrCpssDxChNetIfCpuCodeDesignatedDeviceTableSet,
        1, 2},

    {"cpssDxChNetIfCpuCodeDesignatedDeviceGetFirst",
        &wrCpssDxChNetIfCpuCodeDesignatedDeviceTableGetFirst,
        1, 0},

    {"cpssDxChNetIfCpuCodeDesignatedDeviceGetNext",
        &wrCpssDxChNetIfCpuCodeDesignatedDeviceTableGetNext,
        1, 0},

    {"cpssDxChNetIfCpuCodeRateLimiterSet",
        &wrCpssDxChNetIfCpuCodeRateLimiterTableSet,
        1, 3},

    {"cpssDxChNetIfCpuCodeRateLimiterGetFirst",
        &wrCpssDxChNetIfCpuCodeRateLimiterTableGetFirst,
        1, 0},

    {"cpssDxChNetIfCpuCodeRateLimiterGetNext",
        &wrCpssDxChNetIfCpuCodeRateLimiterTableGetNext,
        1, 0},

    {"cpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet",
        &wrCpssDxChNetIfCpuCodeRateLimiterWindowResolutionSet,
        2, 0},

    {"cpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet",
        &wrCpssDxChNetIfCpuCodeRateLimiterWindowResolutionGet,
        1, 0},

    {"cpssDxChNetIfTxFromCpuSet",
        &wrCpssDxChNetIfSetTxNetIfTblEntryFromCpu,
        2, 32},

    {"cpssDxChNetIfTxFromCpuGetFirst",
        &wrCpssDxChNetIfGetTxNetIfTblFirstEntryFromCpu,
        2, 0},

    {"cpssDxChNetIfTxFromCpuGetNext",
        &wrCpssDxChNetIfGetTxNetIfTblNextEntryFromCpu,
        2, 0},

    {"cpssDxChNetIfTxForwardSet",
        &wrCpssDxChNetIfSetTxNetIfTblEntryForward,
        2, 27},

    {"cpssDxChNetIfTxForwardGetFirst",
        &wrCpssDxChNetIfGetTxNetIfTblFirstEntryForward,
        2, 0},

    {"cpssDxChNetIfTxForwardGetNext",
        &wrCpssDxChNetIfGetTxNetIfTblNextEntryForward,
        2, 0},

    {"cpssDxChNetIfTxFromCpuDelete",
        &wrCpssDxChNetIfDeleteTxNetIfTblEntryFromCpu,
        2, 32},

    {"cpssDxChNetIfTxForwardDelete",
        &wrCpssDxChNetIfDeleteTxNetIfTblEntryForward,
        2, 27},

    {"cpssDxChNetIfTxFromCpuClear",
        &wrCpssDxChNetIfClearTxNetIfTableFromCpu,
        2, 0},

    {"cpssDxChNetIfTxForwardClear",
        &wrCpssDxChNetIfClearTxNetIfTableForward,
        2, 0},

    {"cpssDxChTxSetMode",
        &wrCpssDxChNetIfTxSetMode,
        2, 0},

    {"cpssDxChTxGetMode",
        &wrCpssDxChNetIfTxGetMode,
        0, 0},

    {"cpssDxChTxStart",
     &wrCpssDxChNetIfTxStart,
     0, 0},

    {"cpssDxChTxStop",
     &wrCpssDxChNetIfTxStop,
     0, 0},

    /* Rx packets table -- start */
    {"cmdDxChRxStartCapture",
        &wrCmdDxChRxStartCapture,
        0, 0},

    {"cmdDxChRxStopCapture",
        &wrCmdDxChRxStopCapture,
        0, 0},

    {"cmdDxChRxSetMode",
        &wrCmdDxChRxSetMode,
        3, 0},

    {"cmdDxChRxGetMode",
        &wrCmdDxChRxGetMode,
        0, 0},

    {"cpssDxChNetIfRxPacketTableGetFirst",/* get first */
        &wrCpssDxChNetIfRxPacketTableGetFirst,
        0, 0},

    {"cpssDxChNetIfRxPacketTableGetNext",
        &wrCpssDxChNetIfRxPacketTableGetNext,/* get next */
        0, 0},

    {"cpssDxChNetIfRxPacketTableClear",
        &wrCpssDxChNetIfRxPacketTableClear,/* clear */
        0, 0},
    /* Rx packets table -- End */

    {"cpssDxChNetIfFromCpuDpSet",
        &wrCpssDxChNetIfFromCpuDpSet,
        2, 0},

    {"cpssDxChNetIfFromCpuDpGet",
        &wrCpssDxChNetIfFromCpuDpGet,
        1, 0},

    {"cpssDxChNetIfRxPacketTable_1GetFirst",/* get first */
        &wrCpssDxChNetIfRxPacketTableGetFirst,
        0, 0},

    {"cpssDxChNetIfRxPacketTable_1GetNext",
        &wrCpssDxChNetIfRxPacketTableGetNext,/* get next */
        0, 0},

    {"cpssDxChNetIfRxPacketTable_1Clear",
        &wrCpssDxChNetIfRxPacketTableClear,/* clear */
        0, 0},
    /* Rx packets table -- End */

    {"cpssDxChNetIfSdmaRxResourceErrorModeSet",
        &wrCpssDxChNetIfSdmaRxResourceErrorModeSet,
        3, 0},

    {"cpssDxChNetIfCpuCodeRateLimiterPacketCntrGet",
        &wrCpssDxChNetIfCpuCodeRateLimiterPacketCntrGet,
        2, 0},

    {"cpssDxChNetIfCpuCodeRateLimiterDropCntrSet",
        &wrCpssDxChNetIfCpuCodeRateLimiterDropCntrSet,
        2, 0},

    {"cpssDxChNetIfCpuCodeRateLimiterDropCntrGet",
        &wrCpssDxChNetIfCpuCodeRateLimiterDropCntrGet,
        1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChNetIf
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
GT_STATUS cmdLibInitCpssDxChNetIf
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


#ifndef __AX_PLATFORM__
/* ARP packet buffer */
unsigned char galtisBufferARP[] =  \
	"ffffffffffff" /* DMAC */				/* 6   */
	"00023F034100" /* SMAC */				/* 6   */
	"8100000a"	   /* VLAN TAGGED */		/* 4   */
	"080600010800"							/* 6   */
	"06040001"								/* 4   */
	"00023F034100"							/* 6   */
	"c0a80089"								/* 4   */
	"000000000000"							/* 6   */
	"c0a80001"								/* 4   */
	"a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5"/* PADDING */ /* 12 */
	"a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5" ;	/* 12  */	
unsigned int arpBufLen = sizeof(galtisBufferARP)/2;

/* ICMP packet buffer */
unsigned char galtisBufferICMP[] = \
	"001b1188022a" /* DMAC */				/* 6   */
	"0019d2d6ac5f" /* SMAC */				/* 6   */
	"8100000A"	   /* VLAN TAGGED */		/* 4   */
	"08004500"								/* 4   */
	"003c51780000"							/* 6   */
	"8001676ec0a8"							/* 6   */
	"0089c0a80001"							/* 6   */
	"0800495c0200"							/* 6   */
	"020061626364"							/* 6   */
	"65666768696a"							/* 6   */
	"6b6c6d6e6f70"							/* 6   */
	"717273747576"							/* 6   */
	"776162636465"							/* 6   */
	"66676869";								/* 4   */
unsigned int icmpBufLen = sizeof(galtisBufferICMP)/2;

/* packet desc info */
DXCH_PKT_DESC_STC galtisPacketDesc[] = {
/* ARP packet descriptor */
{
	.cmdType 			= DXCH_TX_BY_PORT,
	.entryId			= 0,
	.txSyncMode 		= GT_TRUE,
	.packetIsTagged 	= GT_TRUE,
	.pcktsNum			= 0xA,
	.gap				= 0x5,
	.numSentPackets		= 0x10,
	.sdmaInfo 			= {GT_TRUE,0x0,GT_TRUE},
	.dsaParam			= 
	{
		.commonParams	= 
		{
			DXCH_NET_DSA_TYPE_EXTENDED_E,/* dsaTagType */
			0x0,						 /* vpt */							
			0x0,						 /* cfiBit */
			0x1,						 /* vid */
		}/* commonParam */,
		.dsaType		= DXCH_NET_DSA_CMD_FROM_CPU_E,
		.dsaInfo.fromCpu 	=
		{
			.dstInterface 	= 
			{
				CPSS_INTERFACE_PORT_E, 	/* type */
				{0x1,0x1A},				/* {devNum,portNum} */
				0x0,					/* trunkId */		
				0xFFF,					/* vidx */
				0x1						/* vlanId */
			}/* dstInterface */,
			.tc				= 0,
			.dp				= 0,
			.egrFilterEn	= GT_FALSE,
			.cascadeControl = GT_FALSE,
			.egrFilterRegistered = GT_FALSE,
			.srcId			= 0,
			.srcDev			= 0,
			.extDestInfo.devPort = {GT_FALSE,GT_FALSE}
		}/* dsaInfo.fromCpu */
	}/* dsaParam */
}/* ARP packet descriptor */,
/* ICMP packet descriptor */
{
	.cmdType			= DXCH_TX_BY_PORT,
	.entryId			= 1,
	.txSyncMode 		= GT_TRUE,
	.packetIsTagged 	= GT_TRUE,
	.pcktsNum			= 0x1,
	.gap				= 0x5,
	.numSentPackets 	= 0x10,
	.sdmaInfo			= {GT_TRUE,0x0,GT_TRUE},
	.dsaParam			= 
	{
		.commonParams	= 
		{
			DXCH_NET_DSA_TYPE_EXTENDED_E,/* dsaTagType */
			0x0,						 /* vpt */							
			0x0,						 /* cfiBit */
			0x1,						 /* vid */
		}/* commonParam */,
		.dsaType		= DXCH_NET_DSA_CMD_FROM_CPU_E,
		.dsaInfo.fromCpu	=
		{
			.dstInterface	= 
			{
				CPSS_INTERFACE_PORT_E,	/* type */
				{0x1,0x1A},				/* {devNum,portNum} */
				0x0,					/* trunkId */		
				0x0,					/* vidx */
				0x1 					/* vlanId */
			}/* dstInterface */,
			.tc 			= 0,
			.dp 			= 0,
			.egrFilterEn	= GT_FALSE,
			.cascadeControl = GT_FALSE,
			.egrFilterRegistered = GT_FALSE,
			.srcId			= 0,
			.srcDev 		= 0,
			.extDestInfo.devPort = {GT_FALSE,GT_FALSE}
		}/* dsaInfo.fromCpu */
	}/* dsaParam */
}/* ICMP packet descriptor */
};


CMD_STATUS cpssDxChNetIfTblEntry0Set
(
	unsigned int packetType
)
{
    GT_STATUS status;
    DXCH_PKT_DESC_STC * netTxPacketDesc;
    GT_U32   entryIndex=0;
    GT_U32   numOfBuffs;
	GT_U32 	 devNum;
	unsigned int i = 0,packetLen = arpBufLen;
	unsigned char * packetPtr = galtisBufferARP;

	osPrintf("cpssDxChNetIfTblEntry0Set::enter type %d\r\n",packetType);
	if( 0 == packetType )
	{/* ARP packet */
		i = 0;
		packetPtr = galtisBufferARP;
		packetLen = arpBufLen;
	}
	else if( 1 == packetType )
	{
		i = 1;
		packetPtr = galtisBufferICMP;
		packetLen = icmpBufLen;
	}
	else
	{
		return GT_BAD_PARAM;
	}

	devNum = galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum;
	
    /* creat the pool only once */
    if(poolCreated == GT_FALSE)
    {
#ifdef APP_DEMO_CPU_ETH_PORT_MANAGE
        /* create pool of buffers from Cache */
        if(appDemoSysConfig.cpuEtherPortUsed == GT_TRUE)
        {
            status = gtPoolCreatePool(devNum,
                                      TX_BUFFER_SIZE + 64 /* ALIGN_ADDR_CHUNK */,
                                      GT_4_BYTE_ALIGNMENT,
                                      MAX_NUM_OF_BUFFERS_AT_POOL,
                                      GT_TRUE,
                                      &txBuffersPoolId);
        }
        else
#endif/*APP_DEMO_CPU_ETH_PORT_MANAGE*/
        {
            /* create pool of buffers from Cache */
            status = gtPoolCreateDmaPool(devNum,
                                      TX_BUFFER_SIZE + 64 /* ALIGN_ADDR_CHUNK */,
                                      GT_4_BYTE_ALIGNMENT,
                                      MAX_NUM_OF_BUFFERS_AT_POOL,
                                      GT_TRUE,
                                      &txBuffersPoolId);
        }

        if (status != GT_OK)
        {
                osPrintf("ERROR-PoolCreateDmaPool\n\n");
                return status;
        }
		osPrintf("wrCpssDxChNetIfSetTxNetIfTblEntryFromCpu::pool created\r\n");
        poolCreated = GT_TRUE;
    }

    netTxPacketDesc=osMalloc(sizeof(DXCH_PKT_DESC_STC));
    if (netTxPacketDesc==NULL)
    {
        osPrintf("cpssDxChNetIfTblEntry0Set::no resource desc\r\n");
        return CMD_OK;
    }
    osMemSet(netTxPacketDesc, 0, sizeof(DXCH_PKT_DESC_STC));

    status = setBufferArraysOfEqualSize(packetPtr,
                               packetLen,
                               TX_BUFFER_SIZE,
                               netTxPacketDesc->pcktData,
                               netTxPacketDesc->pcktDataLen,
                               &numOfBuffs);
	osPrintf("wrCpssDxChNetIfSetTxNetIfTblEntryFromCpu::setBufferArraysOfEqualSize len %d need %d buffers %d\r\n", \
				packetLen,numOfBuffs,status);

    if (status != GT_OK)
    {
        osFree(netTxPacketDesc);
        freeBufferArraysOfEqualSize(netTxPacketDesc->pcktData,
                                    netTxPacketDesc->pcktDataLen);
		osPrintf("cpssDxChNetIfTblEntry0Set::no resource data buffer\r\n");
        return CMD_OK;
    }


    netTxPacketDesc->numOfBuffers 	= numOfBuffs;
    netTxPacketDesc->cmdType 		= galtisPacketDesc[i].cmdType;

    netTxPacketDesc->entryId 		= galtisPacketDesc[i].entryId;
    netTxPacketDesc->txSyncMode 	= galtisPacketDesc[i].txSyncMode;
    netTxPacketDesc->packetIsTagged	= galtisPacketDesc[i].packetIsTagged;

    netTxPacketDesc->pcktsNum 		= galtisPacketDesc[i].pcktsNum;
    netTxPacketDesc->gap 			= galtisPacketDesc[i].gap;
    netTxPacketDesc->waitTime 		= galtisPacketDesc[i].waitTime;
    netTxPacketDesc->numSentPackets = galtisPacketDesc[i].numSentPackets;

    netTxPacketDesc->sdmaInfo.recalcCrc 				= galtisPacketDesc[i].sdmaInfo.recalcCrc;
    netTxPacketDesc->sdmaInfo.txQueue 					= galtisPacketDesc[i].sdmaInfo.txQueue;
    netTxPacketDesc->sdmaInfo.invokeTxBufferQueueEvent 	= galtisPacketDesc[i].sdmaInfo.invokeTxBufferQueueEvent;

    netTxPacketDesc->dsaParam.commonParams.dsaTagType 	= galtisPacketDesc[i].dsaParam.commonParams.dsaTagType;
    netTxPacketDesc->dsaParam.commonParams.vpt 			= galtisPacketDesc[i].dsaParam.commonParams.vpt;
    netTxPacketDesc->dsaParam.commonParams.cfiBit 		= galtisPacketDesc[i].dsaParam.commonParams.cfiBit;
    netTxPacketDesc->dsaParam.commonParams.vid 			= galtisPacketDesc[i].dsaParam.commonParams.vid;

    netTxPacketDesc->dsaParam.dsaType = DXCH_NET_DSA_CMD_FROM_CPU_E;

    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.tc 	= galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.tc;
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dp 	= galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dp;
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.egrFilterEn 		  = galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.egrFilterEn;
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.cascadeControl 	  = galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.cascadeControl;
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.egrFilterRegistered = galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.egrFilterRegistered;
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.srcId	= galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.srcId;
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.srcDev= galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.srcDev;
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.type = galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.type;
	if(CPSS_INTERFACE_PORT_E == netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.type)
	{
    	netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum =  \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum;
    	netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum =  \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum;
	}
	else if(CPSS_INTERFACE_VID_E == netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.type)
	{
		netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.vlanId;
		netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.vidx = \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.vidx;
	}
	else
	{
    	netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum =  \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum;
    	netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum =  \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum;
		netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.vlanId = \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.vlanId;
		netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.vidx = \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.vidx;
		netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.trunkId = \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.dstInterface.trunkId;
	}
	
    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.devNum,
                         netTxPacketDesc->dsaParam.dsaInfo.fromCpu.dstInterface.devPort.portNum);

    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged =  \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.dstIsTagged;
    netTxPacketDesc->dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU =  \
					galtisPacketDesc[i].dsaParam.dsaInfo.fromCpu.extDestInfo.devPort.mailBoxToNeighborCPU;


	osPrintf("cpssDxChNetIfTblEntry0Set::::Insert into table\r\n");
    /* Insert into table */
    status = cmdCpssDxChTxSetPacketDesc(netTxPacketDesc->entryId,
                                netTxPacketDesc,
                                GT_FALSE,   /* Non extended */
                                &entryIndex);

    osFree(netTxPacketDesc);
    osPrintf("cpssDxChNetIfTblEntry0Set::entryIndex %d result %d\r\n",entryIndex,status);

    return CMD_OK;
}

#endif

