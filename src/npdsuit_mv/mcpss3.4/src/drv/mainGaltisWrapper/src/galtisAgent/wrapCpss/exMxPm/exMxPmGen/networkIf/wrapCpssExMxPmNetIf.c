/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmNetIf.c
*
* DESCRIPTION:
*       TODO: Add proper description of this file here
*
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
#include <cpss/exMxPm/exMxPmGen/networkIf/cpssExMxPmNetIf.h>
#include <cpss/exMxPm/exMxPmGen/trunk/cpssExMxPmTrunk.h>

/*******************************************************************************
* cpssExMxPmNetIfInit
*
* DESCRIPTION:
*       Initialize the network interface structures, Rx descriptors & buffers
*       and Tx descriptors -- SDMA related.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*        devNum    - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success,
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - one of the parameters value is wrong
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmNetIfInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfRemove
*
* DESCRIPTION:
*       Release all Network Interface related structures.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*        devNum    - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success,
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - one of the parameters value is wrong
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       This function is called upon Hot removal of a device
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfRemove
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmNetIfRemove(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfPrePendTwoBytesHeaderSet
*
* DESCRIPTION:
*       Enables/Disable pre-appending a two-byte header to all packets forwarded
*       to the CPU.
*       This two-byte header is used to align the IPv4 header to 32 bits.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*        devNum    - device number
*        enable    - GT_TRUE  - Two-byte headers are pre-appended to packets
*                               forwarded to the CPU.
*                    GT_FALSE - Two-byte headers are not pre-appended to packets
*                               forwarded to the CPU.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on bad parameters
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfPrePendTwoBytesHeaderSet
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
    result = cpssExMxPmNetIfPrePendTwoBytesHeaderSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfPrePendTwoBytesHeaderGet
*
* DESCRIPTION:
*       Get state of pre-appending a two-byte header to all packets forwarded
*       to the CPU.
*       This two-byte header is used to align the IPv4 header to 32 bits.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*        devNum    - device number
*
* OUTPUTS:
*        enablePtr    - GT_TRUE  - Two-byte header are pre-appended to packets
*                               forwarded to the CPU.
*                       GT_FALSE - Two-byte header are not pre-appended to packets
*                               forwarded to the CPU.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on bad parameters
*       GT_BAD_PTR      - one of the parameters in NULL pointer.
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfPrePendTwoBytesHeaderGet
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
    result = cpssExMxPmNetIfPrePendTwoBytesHeaderGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfSdmaRxCountersGet
*
* DESCRIPTION:
*       Return the aggregate counter values for the given traffic class queue.
*       -- SDMA related
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
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
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on bad parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*       Counters are reset on every read.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfSdmaRxCountersGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 queueIdx;
    CPSS_EXMXPM_NET_SDMA_RX_COUNTERS_STC rxCounters;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    queueIdx=(GT_U8)inArgs[1];


    /* call cpss api function */
    result = cpssExMxPmNetIfSdmaRxCountersGet(devNum, queueIdx, &rxCounters);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",rxCounters.rxInOctets,rxCounters.rxInPkts);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfSdmaRxErrorCountGet
*
* DESCRIPTION:
*       Returns the total number of Rx resource errors that occurred on a given
*       Rx queue .-- SDMA related
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
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
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on bad parameters
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       Counters are reset on every read.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfSdmaRxErrorCountGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC rxErrCount;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];


    /* call cpss api function */
    result = cpssExMxPmNetIfSdmaRxErrorCountGet(devNum, &rxErrCount);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d",rxErrCount.counterArray[0],
                   rxErrCount.counterArray[0],rxErrCount.counterArray[1],rxErrCount.counterArray[2],
                   rxErrCount.counterArray[3],rxErrCount.counterArray[4],rxErrCount.counterArray[5],
                   rxErrCount.counterArray[6],rxErrCount.counterArray[7] );

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfSdmaRxQueueEnableSet
*
* DESCRIPTION:
*       Enable/Disable the specified traffic class queue for RX.
*       -- SDMA related.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
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
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on bad parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfSdmaRxQueueEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 queue;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    queue = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmNetIfSdmaRxQueueEnableSet(devNum, queue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfSdmaRxQueueEnableGet
*
* DESCRIPTION:
*      Get the specified traffic class queue state for RX
*       -- SDMA related.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*        devNum    - device number
*        queue     - traffic class queue
*
* OUTPUTS:
*        enablePtr    - GT_TRUE, queue is enabled
*                       GT_FALSE, queue is disabled
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on bad parameters
*       GT_BAD_PTR      - one of the parameters in NULL pointer.
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfSdmaRxQueueEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 queue;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    queue = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmNetIfSdmaRxQueueEnableGet(devNum, queue, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfSdmaTxQueueEnableSet
*
* DESCRIPTION:
*       Enable/Disable the specified traffic class queue for TX.
*       -- SDMA related.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
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
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on bad parameters
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfSdmaTxQueueEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 queue;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    queue = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmNetIfSdmaTxQueueEnableSet(devNum, queue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfSdmaTxQueueEnableGet
*
* DESCRIPTION:
*       Get the specified traffic class queue state for TX.
*       -- SDMA related.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*        devNum    - device number
*        queue     - traffic class queue
*
* OUTPUTS:
*        enablePtr    - GT_TRUE, queue is enabled
*                       GT_FALSE, queue is disabled
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - on bad parameters
*       GT_BAD_PTR      - one of the parameters in NULL pointer.
*       GT_FAIL         - otherwise
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfSdmaTxQueueEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 queue;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    queue = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmNetIfSdmaTxQueueEnableGet(devNum, queue, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfSdmaTxBufferQueueInfoGet
*
* DESCRIPTION:
*       This routine returns the information of transmitted packet.
*       The information retrieved by CPU as a result of txBufferQueue event.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*       hndl        - Handle got from gtEventBind.
*
* OUTPUTS:
*       devPtr      - The device from where the packet was transmitted.
*       cookiePtr   - The user cookie handed on the transmit request.
*       queuePtr    - The queue from which this packet was transmitted
*       statusPtr   - GT_OK if packet transmission was successful, GT_FAIL on
*                     packet reject.
*
* RETURNS:
*       GT_OK           - on success, or
*       GT_HW_ERROR     - on hardware error
*       GT_BAD_PARAM    - one of the parameters value is wrong
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfSdmaTxBufferQueueInfoGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 hndl=0;
    GT_U8 dev;
    GT_PTR cookie;
    GT_U8 queue;
    GT_STATUS status;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    /*TODO: add code here*/

    /* call cpss api function */
    result = cpssExMxPmNetIfSdmaTxBufferQueueInfoGet(hndl, &dev, &cookie, &queue, &status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfDsaTagBuild
*
* DESCRIPTION:
*       Build DSA tag bytes on the packet from the DSA tag parameters .
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*       devNum     - device number
*       dsaInfoPtr - (pointer to) the DSA parameters that need to be built into
*                     the packet
*
* OUTPUTS:
*       dsaBytesPtr - pointer to the start of DSA tag in the packet
*                     This pointer holds the space for the packet:
*                     4 bytes when using regular DSA tag.
*                     8 bytes when using extended DSA tag.
*                     This function only set values into the 4 or 8 bytes
*                     according to the parameters in parameter dsaInfoPtr.
*                     NOTE : this memory is the caller's responsibility (to
*                     allocate / free)
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR       - one of the parameters in NULL pointer.
*       GT_NOT_SUPPORTED - not supported DSA tag options.
*       GT_FAIL          - otherwise.
*
*
* COMMENTS:
*           support both :
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfDsaTagBuild
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum=0;
    CPSS_EXMXPM_NET_DSA_PARAMS_STC dsaInfo;
    GT_U8 dsaBytes[8];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(dsaBytes, 0, sizeof(dsaBytes));
    cmdOsMemSet(&dsaInfo, 0, sizeof(dsaInfo));
    /* map input arguments to locals */

    /*TODO: add code here*/

    /* call cpss api function */
    result = cpssExMxPmNetIfDsaTagBuild(devNum, &dsaInfo, dsaBytes);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmNetIfDsaTagParse
*
* DESCRIPTION:
*       parse the DSA tag parameters from the DSA tag on the packet.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices
*
* INPUTS:
*       devNum      - device number
*       dsaBytesPtr - pointer to the start of DSA tag in the packet
*
* OUTPUTS:
*       dsaInfoPtr  - (pointer to) the DSA parameters that were parsed from the
*                     packet
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on bad device number
*       GT_BAD_PTR       - one of the parameters in NULL pointer.
*       GT_NOT_SUPPORTED - not supported DSA tag options.
*       GT_FAIL          - otherwise.
*
*
* COMMENTS:
*           support both :
*           ===== regular DSA tag  ====
*           ===== extended DSA tag ====
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmNetIfDsaTagParse
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum=0;
    GT_U8 dsaBytes[8];
    CPSS_EXMXPM_NET_DSA_PARAMS_STC dsaInfo;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    cmdOsMemSet(dsaBytes, 0, sizeof(dsaBytes));
    cmdOsMemSet(&dsaInfo, 0, sizeof(dsaInfo));
    /* map input arguments to locals */

    /*TODO: add code here*/

    /* call cpss api function */
    result = cpssExMxPmNetIfDsaTagParse(devNum, dsaBytes, &dsaInfo);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/**** database initialization **************************************/

/* TODO: replace X_X with real number of arguments */
#define X_X  0

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmNetIfInit",
         &wrCpssExMxPmNetIfInit,
         1, 0},
        {"cpssExMxPmNetIfRemove",
         &wrCpssExMxPmNetIfRemove,
         1, 0},
        {"cpssExMxPmNetIfPrePendTwoBytesHeaderSet",
         &wrCpssExMxPmNetIfPrePendTwoBytesHeaderSet,
         2, 0},
        {"cpssExMxPmNetIfPrePendTwoBytesHeaderGet",
         &wrCpssExMxPmNetIfPrePendTwoBytesHeaderGet,
         1, 0},
        {"cpssExMxPmNetIfSdmaRxCountersGet",
         &wrCpssExMxPmNetIfSdmaRxCountersGet,
         2, 0},
        {"cpssExMxPmNetIfSdmaRxErrorCountGet",
         &wrCpssExMxPmNetIfSdmaRxErrorCountGet,
         1, 0},
        {"cpssExMxPmNetIfSdmaRxQueueEnableSet",
         &wrCpssExMxPmNetIfSdmaRxQueueEnableSet,
         3, 0},
        {"cpssExMxPmNetIfSdmaRxQueueEnableGet",
         &wrCpssExMxPmNetIfSdmaRxQueueEnableGet,
         2, 0},
        {"cpssExMxPmNetIfSdmaTxQueueEnableSet",
         &wrCpssExMxPmNetIfSdmaTxQueueEnableSet,
         3, 0},
        {"cpssExMxPmNetIfSdmaTxQueueEnableGet",
         &wrCpssExMxPmNetIfSdmaTxQueueEnableGet,
         2, 0},
        {"cpssExMxPmNetIfSdmaTxBufferQueueInfoGet",
         &wrCpssExMxPmNetIfSdmaTxBufferQueueInfoGet,
         X_X, X_X},
        {"cpssExMxPmNetIfDsaTagBuild",
         &wrCpssExMxPmNetIfDsaTagBuild,
         X_X, X_X},
        {"cpssExMxPmNetIfDsaTagParse",
         &wrCpssExMxPmNetIfDsaTagParse,
         X_X, X_X},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmNetIf
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
GT_STATUS cmdLibInitCpssExMxPmNetIf
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}


/*******************************************************************************/
/************************ GALTIS INTERNAL SERVICE ROUTINES *********************/
/*******************************************************************************/
#include <galtisAgent/wrapUtil/cmdCpssNetReceive.h>

static GT_U32 netRxExtLastIndex;

/*******************************************************************************
* wrCpssExMxPmNetIfRxPacketTableGet (table command)
*
* DESCRIPTION:
*     Get first/next entry from the EXMXPM rxNetIf table.
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
CMD_STATUS wrCpssExMxPmNetIfRxPacketTableGet
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
    GT_8   packetBuff[GALTIS_RX_BUFFER_MAX_SIZE_CNS];
    GT_U32 buffLen = GALTIS_RX_BUFFER_MAX_SIZE_CNS;
    GT_U8  devNum;
    GT_U8  queue;
    CPSS_EXMXPM_NET_RX_PARAMS_STC rxParam[2];
    GT_U8*      packetString;
    GT_U32  ii;
    CPSS_EXMXPM_NET_DSA_COMMON_STC  *commonParamsPtr;
    CPSS_EXMXPM_NET_DSA_TO_CPU_STC  *toCpuPtr;
    CPSS_EXMXPM_NET_DSA_FORWARD_STC *forwardPtr;
    GT_U8  __Dev,__Port; /* Dummy for converting. */

    GT_BOOL         memberOfTrunk;
    GT_TRUNK_ID     trunkId;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(getNext == GT_FALSE)
    {
        rc = cmdCpssRxPkGetFirst(&netRxExtLastIndex,
                                 packetBuff, &buffLen, &packetActualLength,
                                 &devNum,&queue,
                                 rxParam);
    }
    else
    {
        rc = cmdCpssRxPkGetNext(&netRxExtLastIndex,
                                 packetBuff, &buffLen, &packetActualLength,
                                 &devNum,&queue,
                                 rxParam);
    }

    if(rc != GT_OK)
    {
        if(rc == GT_NO_MORE)
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
        }
        else
        {
            galtisOutput(outArgs, rc, "");
        }

        return CMD_OK;
    }

    switch (rxParam[0].dsaParam.dsaCmd)
    {
        case CPSS_EXMXPM_NET_DSA_CMD_TO_CPU_E:
            toCpuPtr = &(rxParam[0].dsaParam.dsaInfo.toCpu);
            inFields[6] = toCpuPtr->isTagged;
            cpssExMxPmTrunkPortTrunkIdGet(devNum,toCpuPtr->portNum,&memberOfTrunk,&trunkId);
            if (memberOfTrunk == GT_TRUE)
            {
                inFields[7] = 1;          /*srcIsTrunk*/
                inFields[8] = trunkId;    /*trunk Id*/
            }
            else
            {
                inFields[7] = 0;                 /*srcIsPort*/
                inFields[8] = toCpuPtr->portNum; /*port Id*/

                __Dev  = (GT_U8)inFields[0];
                __Port = (GT_U8)inFields[8];
                CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port) ;
                inFields[0] = __Dev  ;
                inFields[8] = __Port ;
            }
            inFields[9] = toCpuPtr->cpuCode;
        break;

        case CPSS_EXMXPM_NET_DSA_CMD_FORWARD_E:
            forwardPtr = &(rxParam[0].dsaParam.dsaInfo.forward);
            inFields[6] = forwardPtr->srcIsTagged;

            inFields[0] = forwardPtr->srcDev ;
            inFields[7] = forwardPtr->srcIsTrunk;
            if (GT_TRUE == forwardPtr->srcIsTrunk)
            {
                inFields[8] = forwardPtr->source.trunkId;
            }
            else
            {
                inFields[8] = forwardPtr->source.portNum ;
            }
        break;

        default:
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
    }

    commonParamsPtr = &(rxParam[0].dsaParam.commonParams);

    byteArry.data    = packetBuff;
    byteArry.length  = buffLen;


    /* initialize the index of "inFields" parameters */
    ii = 0;

    inFields[0] = devNum;
    inFields[1] = queue;
    packetString = galtisBArrayOut(&byteArry);
    inFields[3]  = packetActualLength;
    inFields[4] = commonParamsPtr->vpt;
    inFields[5] = commonParamsPtr->vid;

    /* pack and output table fields */
    /*            0 1 2 3 4 5 6 7 8 9 */
    /*                                0 1 2 3 4 5*/
    fieldOutput("%d%d%s%d%d%d%d%d%d%d",
                inFields[0],
                inFields[1],
                packetString,
                inFields[3],
                inFields[4],
                inFields[5],
                inFields[6],
                inFields[7],
                inFields[8],
                inFields[9]
    );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

