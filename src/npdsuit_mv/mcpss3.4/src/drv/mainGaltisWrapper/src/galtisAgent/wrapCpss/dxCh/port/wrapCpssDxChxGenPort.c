/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapPhyCpssDxCh.c
*
* DESCRIPTION:
*       Wrapper functions for Phy cpss.dxCh functions
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
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortTx.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortStat.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCtrl.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortBufMg.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortSyncEther.h>
#include <cpss/generic/config/private/prvCpssConfigTypes.h>
#include <cpss/dxCh/dxChxGen/config/private/prvCpssDxChInfo.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortCn.h>
#include <cpss/dxCh/dxChxGen/port/cpssDxChPortPfc.h>
#include <cpss/generic/cos/cpssCosTypes.h>

/* port used in Mac Counters tables */
static GT_U8 portForMacCountersGet = 0xFF;

/*******************************************************************************
* cpssDxChPortTxInit
*
* DESCRIPTION:
*       Init port Tx configuration.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       In SALSA family the first profile (profile_0) is used for CPU port and
*       the second one (profile_1) for network ports.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxInit

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxQueueEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing on all Tx queues of specified port
*       on specified device.
*
* INPUTS:
*       devNum - physical device number
*       enable - GT_TRUE, Global enqueuing enabled
*                GT_FALSE, Global enqueuing disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQueueEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxQueueEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxQueueEnableGet
*
* DESCRIPTION:
*       Get status of enqueuing on all Tx queues on a specific device
*       (enabled/disabled).
*
* APPLICABLE DEVICES:  All DxCh Devices.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       enablePtr - GT_TRUE, Global enqueuing enabled
*                  GT_FALSE, Global enqueuing disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQueueEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxQueueEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxQueueingEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this device
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQueueingEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      portNum;
    GT_U8      tcQueue;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxQueueingEnableSet(devNum, portNum, tcQueue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxQueueingEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this device
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQueueingEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_U8      portNum;
    GT_U8      tcQueue;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxQueueingEnableGet(devNum, portNum, tcQueue, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}


static  GT_U8 tcQueueIndex;
static  GT_U8 maxTcQueueIndex = CPSS_TC_RANGE_CNS;

/*******************************************************************************
* cpssDxChPortTxQueueingEnableSet
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this device
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQueueingEnableEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      portNum;
    GT_U8      tcQueue;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    tcQueue = (GT_U8)inFields[0];
    enable = (GT_BOOL)inFields[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxQueueingEnableSet(devNum, portNum, tcQueue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxQueueingEnableGet
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this device
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQueueingEnableEntryGetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_U8      portNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (tcQueueIndex >= maxTcQueueIndex)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxQueueingEnableGet(devNum, portNum, tcQueueIndex, &enable);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]=tcQueueIndex;
    inFields[1]=enable;

    fieldOutput("%d%d", inFields[0],inFields[1]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/***************************************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQueueingEnableEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    tcQueueIndex = 0;

    return wrCpssDxChPortTxQueueingEnableEntryGetEntry(inArgs,
                                                       inFields,
                                                       numFields,
                                                       outArgs);
}


/***************************************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQueueingEnableEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    tcQueueIndex++;

    return wrCpssDxChPortTxQueueingEnableEntryGetEntry(inArgs,
                                                       inFields,
                                                       numFields,
                                                       outArgs);
}

/*******************************************************************************
* cpssDxChPortTxQueueTxEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmission from a Traffic Class queue
*       on the specified port of specified device.
*
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this Physical Port
*       enable  - GT_TRUE, enable transmission from the queue
*                GT_FALSE, disable transmission from the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQueueTxEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      portNum;
    GT_U8      tcQueue;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxQueueTxEnableSet(devNum, portNum, tcQueue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxQueueTxEnableGet
*
* DESCRIPTION:
*       Get the status of transmission from a Traffic Class queue
*       on the specified port of specified device (Enable/Disable).
*
*
* APPLICABLE DEVICES:  All DxCh Devices.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this Physical Port
*
* OUTPUTS:
*       enablePtr  - GT_TRUE, enable transmission from the queue
*                    GT_FALSE, disable transmission from the queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQueueTxEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_U8      portNum;
    GT_U8      tcQueue;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxQueueTxEnableGet(devNum, portNum, tcQueue, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxFlushQueuesSet
*
* DESCRIPTION:
*       Flush all the traffic class queues on the specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum - physical device number
*       portNum- physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxFlushQueuesSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxFlushQueuesSet(devNum, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable Token Bucket rate shaping on specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       enable  - GT_TRUE, enable Shaping
*                 GT_FALSE, disable Shaping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

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
    result = cpssDxChPortTxShaperEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable Token Bucket rate shaping status on specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       enablePtr - Pointer to Token Bucket rate shaping status.
*                   - GT_TRUE, enable Shaping
*                   - GT_FALSE, disable Shaping
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     portNum;
    GT_BOOL   enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxShaperEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperProfileSet
*
* DESCRIPTION:
*       Set Token Bucket Shaper Profile on specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       burstSize  - burst size in units of 4K bytes
*                    (max value is 4K which results in 16K burst size)
*       maxRatePtr - Requested Rate in Kbps
*
* OUTPUTS:
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperProfileSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      portNum;
    GT_U16     burstSize;
    GT_U32     maxRatePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    burstSize = (GT_U16)inArgs[2];
    maxRatePtr = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxShaperProfileSet(devNum, portNum, burstSize,
                                                           &maxRatePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maxRatePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperProfileGet
*
* DESCRIPTION:
*       Get Token Bucket Shaper Profile on specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       burstSizePtr - (pinter to) burst size in units of 4K bytes
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps.
*       enablePtr    - (pointer to) Tocken Bucket rate shaping status.
*                           - GT_TRUE - Tocken Bucket rate shaping is enabled.
*                           - GT_FALSE - Tocken Bucket rate shaping is disabled.
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperProfileGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     portNum;
    GT_U16    burstSize;
    GT_U32    maxRate;
    GT_BOOL   enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxShaperProfileGet(devNum, portNum,
                                             &burstSize, &maxRate, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", burstSize, maxRate, enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxQShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable shaping of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - physical device number
*       portNUm - physical port number
*       tcQueue - traffic class queue on this port
*       enable  - GT_TRUE, enable shaping on this traffic queue
*                 GT_FALSE, disable shaping on this traffic queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQShaperEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      portNum;
    GT_U8      tcQueue;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxQShaperEnableSet(devNum, portNum, tcQueue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxQShaperEnableGet
*
* DESCRIPTION:
*       Get Enable/Disable shaping status
*       of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this port
*
* OUTPUTS:
*       enablePtr  - Pointer to  shaping status
*                    - GT_TRUE, enable shaping on this traffic queue
*                    - GT_FALSE, disable shaping on this traffic queue
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQShaperEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     portNum;
    GT_U8     tcQueue;
    GT_BOOL   enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxQShaperEnableGet(devNum, portNum, tcQueue, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxQShaperProfileSet
*
* DESCRIPTION:
*       Set Shaper Profile for Traffic Class Queue of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum       - physical device number
*       portNum      - physical port number
*       tcQueue      - traffic class queue on this Logical Port
*       burstSize    - burst size in units of 4K bytes
*                      (max value is 4K which results in 16K burst size)
*       userRate Ptr - Requested Rate in Kbps
*
* OUTPUTS:
*       userRatePtr  - (pointer to) the actual Rate value in Kbps
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQShaperProfileSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      portNum;
    GT_U8      tcQueue;
    GT_U16     burstSize;
    GT_U32     userRatePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];
    burstSize = (GT_U16)inArgs[3];
    userRatePtr = (GT_U32)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxQShaperProfileSet(devNum, portNum, tcQueue,
                                              burstSize, &userRatePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", userRatePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxQShaperProfileGet
*
* DESCRIPTION:
*       Get Shaper Profile for Traffic Class Queue of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       tcQueue    - traffic class queue on the port
*
* OUTPUTS:
*       burstSizePtr - (pinter to) burst size in units of 4K bytes
*       maxRatePtr   - (pointer to) the actual Rate value in Kbps.
*       enablePtr    - (pointer to) Tocken Bucket rate shaping status.
*                           - GT_TRUE - Tocken Bucket rate shaping is enabled.
*                           - GT_FALSE - Tocken Bucket rate shaping is disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQShaperProfileGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     portNum;
    GT_U8     tcQueue;
    GT_U16    burstSize;
    GT_U32    maxRate;
    GT_BOOL   enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxQShaperProfileGet(devNum, portNum, tcQueue,
                                             &burstSize, &maxRate, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", burstSize, maxRate, enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxBindPortToSchedulerProfileSet
*
* DESCRIPTION:
*       Bind a port to scheduler profile set.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       profileSet - The Profile Set in which the scheduler's parameters are
*                    associated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxBindPortToSchedulerProfileSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U8                                       portNum;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT      profileSet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    profileSet = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxBindPortToSchedulerProfileSet(devNum, portNum,
                                                              profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTx4TcTailDropProfileSet
*
* DESCRIPTION:
*       Set tail drop profiles limits for particular TC.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devnum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters.
*       tailDropProfileParamsPtr -
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTx4TcTailDropProfileSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                 result;

    GT_U8                                     devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT         profileSet;
    GT_U8                                     trafficClass;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS   tailDropProfileParamsPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    trafficClass = (GT_U8)inArgs[2];

    /* call get cpss api function to set new added */
    /* tcMaxBuffNum & tcMaxDescrNum fields */
    result = cpssDxChPortTx4TcTailDropProfileGet(devNum, profileSet,
                              trafficClass, &tailDropProfileParamsPtr);

    if( GT_OK == result )
    {
        tailDropProfileParamsPtr.dp0MaxBuffNum = (GT_U16)inFields[0];
        tailDropProfileParamsPtr.dp1MaxBuffNum = (GT_U16)inFields[1];
        tailDropProfileParamsPtr.dp2MaxBuffNum = (GT_U16)inFields[2];
        tailDropProfileParamsPtr.dp0MaxDescrNum = (GT_U16)inFields[3];
        tailDropProfileParamsPtr.dp1MaxDescrNum = (GT_U16)inFields[4];
        tailDropProfileParamsPtr.dp2MaxDescrNum = (GT_U16)inFields[5];

        /* call cpss api function */
        result = cpssDxChPortTx4TcTailDropProfileSet(devNum, profileSet,
                              trafficClass, &tailDropProfileParamsPtr);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTx4TcTailDropProfileSet
*
* DESCRIPTION:
*       Set tail drop profiles limits for particular TC.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters.
*       tailDropProfileParamsPtr -
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTx4TcTailDropProfileSet1

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                 result;

    GT_U8                                     devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT         profileSet;
    GT_U8                                     trafficClass;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS   tailDropProfileParamsPtr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    trafficClass = (GT_U8)inArgs[2];

    tailDropProfileParamsPtr.dp0MaxBuffNum = (GT_U16)inFields[0];
    tailDropProfileParamsPtr.dp1MaxBuffNum = (GT_U16)inFields[1];
    tailDropProfileParamsPtr.dp2MaxBuffNum = (GT_U16)inFields[2];
    tailDropProfileParamsPtr.dp0MaxDescrNum = (GT_U16)inFields[3];
    tailDropProfileParamsPtr.dp1MaxDescrNum = (GT_U16)inFields[4];
    tailDropProfileParamsPtr.dp2MaxDescrNum = (GT_U16)inFields[5];
    tailDropProfileParamsPtr.tcMaxBuffNum = (GT_U16)inFields[6];
    tailDropProfileParamsPtr.tcMaxDescrNum = (GT_U16)inFields[7];

    /* call cpss api function */
    result = cpssDxChPortTx4TcTailDropProfileSet(devNum, profileSet,
                              trafficClass, &tailDropProfileParamsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTx4TcTailDropProfileGet
*
* DESCRIPTION:
*       Get tail drop profiles limits for particular TC.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters.
*
* OUTPUTS:
*       tailDropProfileParamsPtr -
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTx4TcTailDropProfileGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT profileSet;
    GT_U8 trafficClass;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS tailDropProfileParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    trafficClass = (GT_U8)inArgs[2];

    /* init with zeros */
    cmdOsMemSet(&tailDropProfileParams, 0, sizeof(tailDropProfileParams));

    /* call cpss api function */
    result = cpssDxChPortTx4TcTailDropProfileGet(devNum, profileSet, trafficClass, &tailDropProfileParams);

    inFields[0] = tailDropProfileParams.dp0MaxBuffNum;
    inFields[1] = tailDropProfileParams.dp1MaxBuffNum;
    inFields[2] = tailDropProfileParams.dp2MaxBuffNum;
    inFields[3] = tailDropProfileParams.dp0MaxDescrNum;
    inFields[4] = tailDropProfileParams.dp1MaxDescrNum;
    inFields[5] = tailDropProfileParams.dp2MaxDescrNum;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                              inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTx4TcTailDropProfileGet
*
* DESCRIPTION:
*       Get tail drop profiles limits for particular TC.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum             - physical device number
*       profileSet         - the Profile Set in which the Traffic
*                            Class Drop Parameters is associated.
*       trafficClass       - the Traffic Class associated with this set of
*                            Drop Parameters.
*
* OUTPUTS:
*       tailDropProfileParamsPtr - Pointer to
*                       the Drop Profile Parameters to associate
*                       with the Traffic Class in this Profile.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTx4TcTailDropProfileGet1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT profileSet;
    GT_U8 trafficClass;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS tailDropProfileParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    trafficClass = (GT_U8)inArgs[2];

    /* init with zeros */
    cmdOsMemSet(&tailDropProfileParams, 0, sizeof(tailDropProfileParams));

    /* call cpss api function */
    result = cpssDxChPortTx4TcTailDropProfileGet(devNum, profileSet, trafficClass, &tailDropProfileParams);

    inFields[0] = tailDropProfileParams.dp0MaxBuffNum;
    inFields[1] = tailDropProfileParams.dp1MaxBuffNum;
    inFields[2] = tailDropProfileParams.dp2MaxBuffNum;
    inFields[3] = tailDropProfileParams.dp0MaxDescrNum;
    inFields[4] = tailDropProfileParams.dp1MaxDescrNum;
    inFields[5] = tailDropProfileParams.dp2MaxDescrNum;
    inFields[6] = tailDropProfileParams.tcMaxBuffNum;
    inFields[7] = tailDropProfileParams.tcMaxDescrNum;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                    inFields[3], inFields[4], inFields[5],
                                    inFields[6], inFields[7]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTx4TcTailDropProfileGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPortTx4TcTailDropProfileGet(inArgs,inFields,numFields,outArgs);
}

static CMD_STATUS wrCpssDxChPortTx4TcTailDropProfileGetFirst1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    return wrCpssDxChPortTx4TcTailDropProfileGet1(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTx4TcTailDropProfileGetNext
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

/*******************************************************************************
* cpssDxChPortTxTailDropProfileSet
*
* DESCRIPTION:
*       Enables/Disables sharing of buffers and descriptors for all queues of an
*       egress port and set maximal port's limits of buffers and descriptors.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*   devNum        - physical device number
*   profileSet    - the Profile Set in which the Traffic
*                   Class Drop Parameters is associated
*   enableSharing - If GT_TRUE Sharing is enabled. If a packet is to be queued
*                  to an empty queue, it is queued even if the number of buffers
*                   or descriptors allocated to this port exceeds the
*                   < portMaxBuffLimit > and < portMaxDescrLimit >.
*                   If GT_FALSE Sharing is disabled. If the sum of buffers or
*                   descriptors allocated per queue is larger than the number of
*                   buffers or descriptors allocated to this port
*                   < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                   the queues may be starved.
*   portMaxBuffLimit - maximal number of buffers for a port
*   portMaxDescrLimit - maximal number of descriptors for a port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTailDropProfileSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT           profileSet;
    GT_BOOL                                     enableSharing;
    GT_U16                                      portMaxBuffLimit;
    GT_U16                                      portMaxDescrLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    enableSharing = (GT_BOOL)inArgs[2];
    portMaxBuffLimit = (GT_U16)inArgs[3];
    portMaxDescrLimit = (GT_U16)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPortTxTailDropProfileSet(devNum, profileSet,
                                              enableSharing, portMaxBuffLimit,
                                                            portMaxDescrLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxSniffedPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for mirrored packets.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring.
*                           For DxCh,DxCh2: range 0..0xFFF
*                           For DxCh3,DxChXcat: range 0..0x3FFF
*                           For Lion and above: range 0..0xFFFF
*       txSniffMaxDescNum - The number of descriptors allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring.
*                           For DxCh,DxCh2: range 0..0xFFF
*                           For DxCh3,DxChXcat: range 0..0x3FFF
*                           For Lion and above: range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSniffedPcktDescrLimitSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      rxSniffMaxDescNum;
    GT_U32                                      txSniffMaxDescNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rxSniffMaxDescNum = (GT_U32)inArgs[1];
    txSniffMaxDescNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxSniffedPcktDescrLimitSet(devNum, rxSniffMaxDescNum,
                                                            txSniffMaxDescNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxSniffedPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for mirrored packets.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum            - physical device number
*
* OUTPUTS:
*       rxSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the ingress analyzer
*                              port due to mirroring.
*       txSniffMaxDescNumPtr - (pointer to) The number of descriptors allocated
*                              for packets forwarded to the egress analyzer
*                              port due to mirroring.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSniffedPcktDescrLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rxSniffMaxDescNum;
    GT_U32 txSniffMaxDescNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxSniffedPcktDescrLimitGet(devNum, &rxSniffMaxDescNum, &txSniffMaxDescNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", rxSniffMaxDescNum, txSniffMaxDescNum);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxMcastPcktDescrLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for multicast packets.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum            - physical device number
*       mcastMaxDescNum   - The number of descriptors allocated for multicast
*                           packets.
*                           For all devices except Lion and above: in units of 128
*                           descriptors, the actual number descriptors
*                           allocated will be 128 * mcastMaxDescNum.
*                           For Lion and above: actual descriptors number (granularity
*                           of 1).
*                           For DxCh,DxCh2,DxCh3,DxChXcat: range 0..0xF80
*                           For Lion and above: range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxMcastPcktDescrLimitSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    GT_U32                                      mcastMaxDescNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mcastMaxDescNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxMcastPcktDescrLimitSet(devNum, mcastMaxDescNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxWrrGlobalParamSet
*
* DESCRIPTION:
*       Set global parameters for WRR scheduler.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - physical device number
*       wrrMode - Transmit Scheduler count mode.
*       wrrMtu  - MTU for DSWRR, resolution of WRR weights for byte based
*                 Transmit Scheduler count mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxWrrGlobalParamSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                   result;

    GT_U8                                       devNum;
    CPSS_PORT_TX_WRR_MODE_ENT                   wrrMode;
    CPSS_PORT_TX_WRR_MTU_ENT                    wrrMtu;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    wrrMode = (CPSS_PORT_TX_WRR_MODE_ENT)inArgs[1];
    wrrMtu = (CPSS_PORT_TX_WRR_MTU_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxWrrGlobalParamSet(devNum, wrrMode, wrrMtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxShaperGlobalParamsSet
*
* DESCRIPTION:
*       Set Global parameters for shaper.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*    devNum                - physical device number
*    xgPortsTokensRate    - tokens rate for XG ports' shapers
*    gigPortsTokensRate   - tokens rate for Tri Speed ports' shapers
*    gigPortsSlowRateRatio - slow rate ratio for Tri Speed ports. Tokens Update
*                           rate for ports with slow rate divided to the ratio.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       For 98DX250,98DX260, 98DX270 devices the following values are valid.
*       xgPortsTokensRate    - <1-15>
*       gigPortsTokensRate   - <1-15>
*       gigPortsSlowRateRatio - <1-16>
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperGlobalParamsSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                     devNum;
    GT_U32                    xgPortsTokensRate;
    GT_U32                    gigPortsTokensRate;
    GT_U32                    gigPortsSlowRateRatio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    xgPortsTokensRate = (GT_U32)inArgs[1];
    gigPortsTokensRate = (GT_U32)inArgs[2];
    gigPortsSlowRateRatio = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortTxShaperGlobalParamsSet(devNum, xgPortsTokensRate,
                                 gigPortsTokensRate, gigPortsSlowRateRatio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxQWrrProfileSet
*
* DESCRIPTION:
*       Set Weighted Round Robin profile on the specified port's
*       Traffic Class Queue.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Port
*       wrrWeight  - proportion of bandwidth assigned to this queue
*                    relative to the other queues in this
*                    Arbitration Group - resolution is 1/255.
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*         - if weight will be less then port's MTU (maximum transmit unit) there
*           is possibility for empty WRR loops for given TC queue, but to not
*           tight user and for future ASIC's where this problem will be solved
*           check of wrrWeight * 256 > MTU not implemented
*         - There is errata for Cheetah FEr#29. weigh cannot be = 255 for some
*           modes. The functions limits weight accordingly.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQWrrProfileSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;

    GT_U8                                           devNum;
    GT_U8                                           tcQueue;
    GT_U8                                           wrrWeight;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT          profileSet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U8)inArgs[1];
    wrrWeight = (GT_U8)inArgs[2];
    profileSet = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortTxQWrrProfileSet(devNum, tcQueue, wrrWeight,
                                                          profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxQWrrProfileGet
*
* DESCRIPTION:
*       Get Weighted Round Robin profile on the specified port's
*       Traffic Class Queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Port
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
* OUTPUTS:
*       wrrWeightPtr  - Pointer to proportion of bandwidth assigned to this queue
*                       relative to the other queues in this
*                       Arbitration Group - resolution is 1/255.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQWrrProfileGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    GT_U8           devNum;
    GT_U8           tcQueue;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT  profileSet;
    GT_U8                                   wrrWeight;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U8)inArgs[1];
    profileSet = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)inArgs[2];


    /* call cpss api function */
    result = cpssDxChPortTxQWrrProfileGet(devNum, tcQueue, profileSet, &wrrWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", wrrWeight);
    return CMD_OK;

}

static  GT_U8 tcQueueIndex;
static  CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT profileSetIndex;
/*******************************************************************************
* cpssDxChPortTxQWrrProfileSet
*
* DESCRIPTION:
*       Set Weighted Round Robin profile on the specified port's
*       Traffic Class Queue.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Port
*       wrrWeight  - proportion of bandwidth assigned to this queue
*                    relative to the other queues in this
*                    Arbitration Group - resolution is 1/255.
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, profile set or
*                                  traffic class
*       GT_HW_ERROR              - on hardware error
*       GT_TIMEOUT               - previous updated operation not yet completed
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*         - if weight will be less then port's MTU (maximum transmit unit) there
*           is possibility for empty WRR loops for given TC queue, but to not
*           tight user and for future ASIC's where this problem will be solved
*           check of wrrWeight * 256 > MTU not implemented
*         - There is errata for Cheetah FEr#29. weigh cannot be = 255 for some
*           modes. The functions limits weight accordingly.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQWrrProfileEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;

    GT_U8                                           devNum;
    GT_U8                                           tcQueue;
    GT_U8                                           wrrWeight;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT          profileSet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    tcQueue     = (GT_U8)inFields[0];
    profileSet  = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)inFields[1];
    wrrWeight   = (GT_U8)inFields[2];

    /* call cpss api function */
    result = cpssDxChPortTxQWrrProfileSet(devNum, tcQueue, wrrWeight,
                                                          profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPortTxQWrrProfileGet
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       tcQueue - traffic class queue on this device
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQWrrProfileEntryGetEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_U8           wrrWeight;

    if (tcQueueIndex >= maxTcQueueIndex)
    {
        galtisOutput(outArgs, CMD_OK, "%d", -1);
        return CMD_OK;
    }
    else
    {
        if (profileSetIndex > CPSS_PORT_TX_SCHEDULER_PROFILE_8_E)
        {
            /* reset profileSetIndex and increment tcQueueIndex */
            profileSetIndex = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;
            tcQueueIndex++;
        }
    }

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxQWrrProfileGet(devNum, tcQueueIndex, profileSetIndex, &wrrWeight);
    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]=tcQueueIndex;
    inFields[1]=profileSetIndex;
    inFields[2]=wrrWeight;

    fieldOutput("%d%d%d", inFields[0],inFields[1],inFields[2]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/***************************************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQWrrProfileEntryGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    tcQueueIndex = 0;
    profileSetIndex = CPSS_PORT_TX_SCHEDULER_PROFILE_1_E;

    return wrCpssDxChPortTxQWrrProfileEntryGetEntry(inArgs,
                                                    inFields,
                                                    numFields,
                                                    outArgs);
}

/***************************************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQWrrProfileEntryGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    profileSetIndex++;

    return wrCpssDxChPortTxQWrrProfileEntryGetEntry(inArgs,
                                                    inFields,
                                                    numFields,
                                                    outArgs);
}


/*******************************************************************************
* cpssDxChPortTxQArbGroupSet
*
* DESCRIPTION:
*       Set Traffic Class Queue scheduling arbitration group on
*       specificed port of specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Logical Port
*       arbGroup   - scheduling arbitration group:
*                     1) Strict Priority
*                     2) WRR Group 1
*                     3) WRR Group 0
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQArbGroupSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;

    GT_U8                                           devNum;
    GT_U8                                           tcQueue;
    CPSS_PORT_TX_Q_ARB_GROUP_ENT                    arbGroup;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT          profileSet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U8)inArgs[1];
    arbGroup = (CPSS_PORT_TX_Q_ARB_GROUP_ENT)inArgs[2];
    profileSet = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortTxQArbGroupSet(devNum, tcQueue, arbGroup, profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxQArbGroupGet
*
* DESCRIPTION:
*       Get Traffic Class Queue scheduling arbitration group on
*       specificed port of specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - physical device number
*       tcQueue    - traffic class queue on this Logical Port
*       profileSet - the Tx Queue scheduler Profile Set in which the wrrWeight
*                    Parameter is associated.
*
* OUTPUTS:
*       arbGroupPtr   - Pointer to scheduling arbitration group:
*                       1) Strict Priority
*                       2) WRR Group 1
*                       3) WRR Group 0
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number, device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters in NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxQArbGroupGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;

    GT_U8                                           devNum;
    GT_U8                                           tcQueue;
    CPSS_PORT_TX_Q_ARB_GROUP_ENT                    arbGroup;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT          profileSet;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U8)inArgs[1];
    profileSet = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)inArgs[2];


    /* call cpss api function */
    result = cpssDxChPortTxQArbGroupGet(devNum, tcQueue, profileSet, &arbGroup);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", arbGroup);
    return CMD_OK;

}

/*******************************************************************************
*  coreTxPortBindPortToDpSet
*
* DESCRIPTION:
*       Bind a Physical Port to a specific Drop Profile Set.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum        - physical device number
*       portNum       - physical port number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxBindPortToDpSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                  result;

    GT_U8                                      devNum;
    GT_U8                                      portNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT          profileSet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    profileSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxBindPortToDpSet(devNum, portNum, profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxDescNumberGet
*
* DESCRIPTION:
*       Gets the current number of descriptors allocated specified port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Actually gets maximal descriptors number for all ports
*       within this port profile.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxDescNumberGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    GT_U8           devNum;
    GT_U8           portNum;
    GT_U16          numberPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxDescNumberGet(devNum, portNum, &numberPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", numberPtr);
    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxBufNumberGet
*
* DESCRIPTION:
*       Gets the maximal Tx queue buffers that specified port can
*       allocate, for SALSA 2 only.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       numPtr   - number of buffers
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Actually gets maximal buffers number for all ports
*       within this port profile.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxBufNumberGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    GT_U8           devNum;
    GT_U8           portNum;
    GT_U16          numPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxBufNumberGet(devNum, portNum, &numPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", numPtr);
    return CMD_OK;
}
/*******************************************************************************
*  cpssDxChPortTx4TcDescNumberGet
*
* DESCRIPTION:
*       Gets the current number of descriptors allocated on specified port
*       for specified Traffic Class queues.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       trafClass - trafiic class
*
* OUTPUTS:
*       numberPtr  - (pointer to) the number of descriptors
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Note: this is two registers for descriptor number (dp0 and dp1)
*             but the output is just one of them!
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTx4TcDescNumberGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    GT_U8           devNum;
    GT_U8           portNum;
    GT_U8           trafClass;
    GT_U16          numberPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    trafClass = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTx4TcDescNumberGet(devNum, portNum,
                                      trafClass, &numberPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", numberPtr);
    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTx4TcBufNumberGet
*
* DESCRIPTION:
*       Gets the current number of buffers allocated on specified port
*       for specified Traffic Class queues, for SALSA 2 only
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       trafClass - trafiic class
*
* OUTPUTS:
*       numPtr    - (pointer to) the number of buffers
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Note: this is two registers for buffers number (dp0 and dp1)
*             but the output is just one of them!
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTx4TcBufNumberGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;

    GT_U8           devNum;
    GT_U8           portNum;
    GT_U8           trafClass;
    GT_U16          numPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    trafClass = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTx4TcBufNumberGet(devNum, portNum, trafClass, &numPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", numPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxToCpuShaperModeSet
*
* DESCRIPTION:
*       Set Shaper mode packet or byte based for CPU port shapers
*       Shapers are configured by gtTxPortSetShaperProfile or
*       gtTxPortQSetShaperProfile
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - physical device number
*       mode    - shaper mode: byte count or packet number based ackets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxToCpuShaperModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                              result;

    GT_U8                                  devNum;
    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT      mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_PORT_TX_DROP_SHAPER_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxToCpuShaperModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxSharingGlobalResourceEnableSet
*
* DESCRIPTION:
*       Enable/Disable sharing of resources for enqueuing of packets.
*       The shared resources configuration is set by
*       cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* APPLICABLE DEVICES:  DxCh2, DxCh3, DxChXcat
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE   - enable TX queue resourses sharing
*                 GT_FALSE - disable TX queue resources sharing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSharingGlobalResourceEnableSet

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
    result = cpssDxChPortTxSharingGlobalResourceEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxSharingGlobalResourceEnableGet
*
* DESCRIPTION:
*       Get enable/disable sharing of resources for enqueuing of packets.
*       The shared resources configuration is set by
*       cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* APPLICABLE DEVICES:  DxCh2, DxCh3, DxChXcat
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE   - enable TX queue resourses sharing
*                    GT_FALSE - disable TX queue resources sharing
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSharingGlobalResourceEnableGet
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
    result = cpssDxChPortTxSharingGlobalResourceEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxSharedGlobalResourceLimitsSet
*
* DESCRIPTION:
*       Configuration of shared resources for enqueuing of packets.
*       The using of shared resources is set
*       by cpssDxChPortTxSharingGlobalResourceEnableSet
*
* APPLICABLE DEVICES:  DxCh2, DxCh3, DxChXcat
*
* INPUTS:
*       devNum             - physical device number
*       sharedBufLimit     - The number of buffers in all Transmit queues that
*                            can be shared between eligible packets.
*                            For DxCh2: range 0..4095
*                            For DxCh3 and above: range 0..16380
*       sharedDescLimit    - The number of descriptors that can be shared
*                            between all eligible packets.
*                            For DxCh2: range 0..4095
*                            For DxCh3 and above: range 0..16380
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       In DxCh3 and above sharedBufLimit and sharedDescLimit are rounded up
*       to the nearest multiple of 4 since the corresponding fields in the
*       Transmit Queue Resource Sharing register are in 4 buffers\descriptors
*       resolution.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSharedGlobalResourceLimitsSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_U32       sharedBufLimit;
    GT_U32       sharedDescLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    sharedBufLimit = (GT_U32)inArgs[1];
    sharedDescLimit = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxSharedGlobalResourceLimitsSet(devNum,
                                sharedBufLimit, sharedDescLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxSharedGlobalResourceLimitsGet
*
* DESCRIPTION:
*       Get the configuration of shared resources for enqueuing of packets.
*       The using of shared resources is set
*       by cpssDxChPortTxSharingGlobalResourceEnableSet
*
* APPLICABLE DEVICES:  DxCh2, DxCh3, DxChXcat
*
* INPUTS:
*       devNum             - physical device number
*
* OUTPUTS:
*       sharedBufLimitPtr  - (pointer to) The number of buffers in all
*                            Transmit queues that
*                            can be shared between eligible packets.
*       sharedDescLimitPtr - (pointer to) The number of descriptors that
*                            can be shared between all eligible packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSharedGlobalResourceLimitsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 sharedBufLimit;
    GT_U32 sharedDescLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxSharedGlobalResourceLimitsGet(devNum, &sharedBufLimit, &sharedDescLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", sharedBufLimit, sharedDescLimit);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxSharedPolicySet
*
* DESCRIPTION:
*       Sets shared pool allocation policy for enqueuing of packets.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*       policy     - shared pool allocation policy.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or wrong policy
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSharedPolicySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT policy;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    policy = (CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxSharedPolicySet(devNum, policy);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxSharedPolicyGet
*
* DESCRIPTION:
*       Gets shared pool allocation policy for enqueuing of packets.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       policyPtr  - (pointer to) shared pool allocation policy.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSharedPolicyGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_TX_SHARED_POLICY_ENT policy;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxSharedPolicyGet(devNum, &policy);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", policy);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxWatchdogGet
*
* DESCRIPTION:
*       Get Tx Port Watchdog status and timeout
*
* APPLICABLE DEVICES:  DxCh1, DxCh2, DxCh3.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr     - (pointer to) the enable / disable state
*       timeoutPtr    - (pointer to) the watchdog timeout
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - enablePtr or timeoutPtr is a null pointer
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxWatchdogGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_U8        portNum;
    GT_BOOL      enablePtr;
    GT_U16       timeoutPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxWatchdogGet(devNum, portNum, &enablePtr,
                                                       &timeoutPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enablePtr, timeoutPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxWatchdogEnableSet
*
* DESCRIPTION:
*       Enable/Disable Watchdog on specified port of specified port of
*       specified device.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       enable  - GT_TRUE, enable Watchdog
*                 GT_FALSE, disable Watchdog
*       timeout - Watchdog timeout in milliseconds (1..4K ms)
*                 If disabling the Watchdog, this parameter is ignored
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxWatchdogEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;
    GT_U8        portNum;
    GT_BOOL      enable;
    GT_U16       timeout;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];
    timeout = (GT_U16)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxWatchdogEnableSet(devNum, portNum, enable, timeout);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortStatInit
*
* DESCRIPTION:
*       Init port egress counter set Core facility.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*     gtPosMacCounterOffset[] holds offsets for Packet Over Sonnet counters(POS).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortStatInit

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;

    GT_U8        devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortStatInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChMacCounterGet
*
* DESCRIPTION:
*       Gets MAC counter for a specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide. the 98EX13X, GT_98MX635 provide
*     two sets of counters for each counter-name. each packet is counted by only
*     one of them. therefore on reading, must read from both and add the values.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChMacCounterGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U8                           portNum;
    CPSS_PORT_MAC_COUNTERS_ENT      cntrName;
    GT_U64                          cntrValuePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    cntrName = (CPSS_PORT_MAC_COUNTERS_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChMacCounterGet(devNum, portNum, cntrName, &cntrValuePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", cntrValuePtr.l[0], cntrValuePtr.l[1]);
    return CMD_OK;
}

/*******************************************************************************
* util_wrCpssDxChPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets ethernet MAC counters and prints them.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum                  - physical device number
*       portNumPtr              - (pointer to) physical port number
*                                 if invokedForAllPorts specified
*                                 all ports up to 63 treated
*       insertPortNumBeforeTab  - insert Port Num Before Tab
*       overridePortNumCol1    - override by Port Num Column#1
*       invokedForAllPorts      - invoked For All Ports
*
* OUTPUTS:
*       inFields - memory of in fields used as temporary
*       outArgs  - output memory
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS  util_wrCpssDxChPortMacCountersOnPortGet
(
    IN     GT_U8   devNum,
    INOUT  GT_U8   *portNumPtr,
    IN     GT_BOOL insertPortNumBeforeTab,
    IN     GT_BOOL overridePortNumCol1,
    IN     GT_BOOL invokedForAllPorts,
    OUT    GT_32   inFields[CMD_MAX_FIELDS],
    OUT    GT_8    outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    CPSS_PORT_MAC_COUNTER_SET_STC   portMacCounterSetArray;
    GT_U8                           portNum;

    while (1)
    {
        portNum = *portNumPtr;

        if (portNum >= 64)
        {
            /* all ports alredy treated - end of table */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return GT_OK;
        }

        /* call cpss api function */
        result = cpssDxChPortMacCountersOnPortGet(
            devNum, portNum, &portMacCounterSetArray);
        if (result == GT_OK)
        {
            /* success, increment port number for the next calls */
            (*portNumPtr) ++;
            break;
        }

        if (invokedForAllPorts == GT_FALSE)
        {
            /* error when invoked for particular port */
            galtisOutput(outArgs, result, "%d", -1);
            return result;
        }

        /* this port failed, try the next port */
        (*portNumPtr) ++;
    }
    /* CPU port has it's own goodPktsSent and goodPktsRcv counters */
    if (portNum < CPSS_CPU_PORT_NUM_CNS)
    {
        /* the value of GOOD_PKTS is sum of three counters: Unicast+Broadcast+Multicast */
        portMacCounterSetArray.goodPktsRcv.l[0] =
            portMacCounterSetArray.ucPktsRcv.l[0] +
            portMacCounterSetArray.brdcPktsRcv.l[0] +
            portMacCounterSetArray.mcPktsRcv.l[0];

        portMacCounterSetArray.goodPktsSent.l[0] =
            portMacCounterSetArray.ucPktsSent.l[0] +
            portMacCounterSetArray.brdcPktsSent.l[0] +
            portMacCounterSetArray.mcPktsSent.l[0];
    }

    inFields[0] = portMacCounterSetArray.goodOctetsRcv.l[0];
    inFields[1] = portMacCounterSetArray.goodOctetsRcv.l[1];
    inFields[2] = portMacCounterSetArray.badOctetsRcv.l[0];
    inFields[3] = portMacCounterSetArray.badOctetsRcv.l[1];
    inFields[4] = portMacCounterSetArray.macTransmitErr.l[0];
    inFields[5] = portMacCounterSetArray.macTransmitErr.l[1];
    inFields[6] = portMacCounterSetArray.goodPktsRcv.l[0];
    inFields[7] = portMacCounterSetArray.goodPktsRcv.l[1];
    inFields[8] = portMacCounterSetArray.badPktsRcv.l[0];
    inFields[9] = portMacCounterSetArray.badPktsRcv.l[1];
    inFields[10] = portMacCounterSetArray.brdcPktsRcv.l[0];
    inFields[11] = portMacCounterSetArray.brdcPktsRcv.l[1];
    inFields[12] = portMacCounterSetArray.mcPktsRcv.l[0];
    inFields[13] = portMacCounterSetArray.mcPktsRcv.l[1];
    inFields[14] = portMacCounterSetArray.pkts64Octets.l[0];
    inFields[15] = portMacCounterSetArray.pkts64Octets.l[1];
    inFields[16] = portMacCounterSetArray.pkts65to127Octets.l[0];
    inFields[17] = portMacCounterSetArray.pkts65to127Octets.l[1];
    inFields[18] = portMacCounterSetArray.pkts128to255Octets.l[0];
    inFields[19] = portMacCounterSetArray.pkts128to255Octets.l[1];
    inFields[20] = portMacCounterSetArray.pkts256to511Octets.l[0];
    inFields[21] = portMacCounterSetArray.pkts256to511Octets.l[1];
    inFields[22] = portMacCounterSetArray.pkts512to1023Octets.l[0];
    inFields[23] = portMacCounterSetArray.pkts512to1023Octets.l[1];
    inFields[24] = portMacCounterSetArray.pkts1024tomaxOoctets.l[0];
    inFields[25] = portMacCounterSetArray.pkts1024tomaxOoctets.l[1];
    inFields[26] = portMacCounterSetArray.goodOctetsSent.l[0];
    inFields[27] = portMacCounterSetArray.goodOctetsSent.l[1];
    inFields[28] = portMacCounterSetArray.goodPktsSent.l[0];
    inFields[29] = portMacCounterSetArray.goodPktsSent.l[1];
    inFields[30] = portMacCounterSetArray.excessiveCollisions.l[0];
    inFields[31] = portMacCounterSetArray.excessiveCollisions.l[1];
    inFields[32] = portMacCounterSetArray.mcPktsSent.l[0];
    inFields[33] = portMacCounterSetArray.mcPktsSent.l[1];
    inFields[34] = portMacCounterSetArray.brdcPktsSent.l[0];
    inFields[35] = portMacCounterSetArray.brdcPktsSent.l[1];
    inFields[36] = portMacCounterSetArray.unrecogMacCntrRcv.l[0];
    inFields[37] = portMacCounterSetArray.unrecogMacCntrRcv.l[1];
    inFields[38] = portMacCounterSetArray.fcSent.l[0];
    inFields[39] = portMacCounterSetArray.fcSent.l[1];
    inFields[40] = portMacCounterSetArray.goodFcRcv.l[0];
    inFields[41] = portMacCounterSetArray.goodFcRcv.l[1];
    inFields[42] = portMacCounterSetArray.dropEvents.l[0];
    inFields[43] = portMacCounterSetArray.dropEvents.l[1];
    inFields[44] = portMacCounterSetArray.undersizePkts.l[0];
    inFields[45] = portMacCounterSetArray.undersizePkts.l[1];
    inFields[46] = portMacCounterSetArray.fragmentsPkts.l[0];
    inFields[47] = portMacCounterSetArray.fragmentsPkts.l[1];
    inFields[48] = portMacCounterSetArray.oversizePkts.l[0];
    inFields[49] = portMacCounterSetArray.oversizePkts.l[1];
    inFields[50] = portMacCounterSetArray.jabberPkts.l[0];
    inFields[51] = portMacCounterSetArray.jabberPkts.l[1];
    inFields[52] = portMacCounterSetArray.macRcvError.l[0];
    inFields[53] = portMacCounterSetArray.macRcvError.l[1];
    inFields[54] = portMacCounterSetArray.badCrc.l[0];
    inFields[55] = portMacCounterSetArray.badCrc.l[1];
    inFields[56] = portMacCounterSetArray.collisions.l[0];
    inFields[57] = portMacCounterSetArray.collisions.l[1];
    inFields[58] = portMacCounterSetArray.lateCollisions.l[0];
    inFields[59] = portMacCounterSetArray.lateCollisions.l[1];
    inFields[60] = portMacCounterSetArray.badFcRcv.l[0];
    inFields[61] = portMacCounterSetArray.badFcRcv.l[1];

    if (overridePortNumCol1 == GT_TRUE)
    {
        /* override the goodOctetsRcv.l[1] column by portNum */
        inFields[1] = portNum;
    }

    if (insertPortNumBeforeTab == GT_TRUE)
    {
        /* insert port number before all table columns */
        fieldOutput("%d", portNum);
        fieldOutputSetAppendMode();
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],
                inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],  inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21],
                inFields[22], inFields[23], inFields[24], inFields[25],
                inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32], inFields[33],
                inFields[34], inFields[35], inFields[36], inFields[37],
                inFields[38], inFields[39], inFields[40], inFields[41],
                inFields[42], inFields[43], inFields[44], inFields[45],
                inFields[46], inFields[47], inFields[48], inFields[49],
                inFields[50], inFields[51], inFields[52], inFields[53],
                inFields[54], inFields[55], inFields[56], inFields[57],
                inFields[58], inFields[59], inFields[60], inFields[61]);

    galtisOutput(outArgs, result, "%f");

    return GT_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS  wrCpssDxChPortMacCountersOnPortGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                           devNum;
    GT_U8                           portNum;
    GT_BOOL                         overridePortNumCol1;
    GT_BOOL                         invokedForAllPorts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    if (portNum >= 64)
    {
        /* print for all ports */
        portForMacCountersGet = 0;
        overridePortNumCol1   = GT_TRUE;
        invokedForAllPorts    = GT_TRUE;
    }
    else
    {
        /* print for specified port only */
        portForMacCountersGet = portNum;
        overridePortNumCol1   = GT_FALSE;
        invokedForAllPorts    = GT_FALSE;
    }

    util_wrCpssDxChPortMacCountersOnPortGet(
        devNum, &portForMacCountersGet,
        GT_FALSE, overridePortNumCol1, invokedForAllPorts,
        inFields, outArgs);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS  wrCpssDxChPortMacCountersOnPortGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                           devNum;
    GT_U8                           portNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    if (portNum < 64)
    {
        /* print for specified port only done at "GetFirst" call */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    util_wrCpssDxChPortMacCountersOnPortGet(
        devNum, &portForMacCountersGet,
        GT_FALSE, GT_TRUE, GT_TRUE,
        inFields, outArgs);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPortMacCountersTableGetFirst
*
* DESCRIPTION:
*       Gets ethernet MAC counters for all Ports.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS  wrCpssDxChPortMacCountersTableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                           devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portForMacCountersGet = 0;

    util_wrCpssDxChPortMacCountersOnPortGet(
        devNum, &portForMacCountersGet,
        GT_TRUE, GT_FALSE, GT_TRUE,
        inFields, outArgs);

    return CMD_OK;
}

/*******************************************************************************
* wrCpssDxChPortMacCountersTableGetNext
*
* DESCRIPTION:
*       Gets ethernet MAC counters for all Ports.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS  wrCpssDxChPortMacCountersTableGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8                           devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    util_wrCpssDxChPortMacCountersOnPortGet(
        devNum, &portForMacCountersGet,
        GT_TRUE, GT_FALSE, GT_TRUE,
        inFields, outArgs);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortEgressCntrModeSet
*
* DESCRIPTION:
*       Create a set of egress counters that work in specified bridge egress
*       countres mode on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       cntrSetNum - counter set number.
*       setModeBmp - counter mode bitmap. For example:
*                   GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNum   - physical port number, if coresponding bit in cntrMode is 1.
*       vlanId    - VLAN Id, if coresponding bit in cntrMode is 1.
*       tc        - traffuc class queue, if coresponding bit in cntrMode is 1.
*       dpLevel   - drop precedence level, if coresponding bit in cntrMode is 1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*        0 MibEgPrioSet0En 0x1     When set to 1, the set 0 Egress filter MIB counter
*                                  count only packets with priority equal to set0 priority
*        1 MibEgDpSet0En 0x1       When set to 1, the set 0 Egress filter MIB counter
*                                  count only packets with Drop precedence equal to
*                                  set0 DP
*        2 MibEgPrioSet1En 0x1     When set to 1, the set 1 Egress filter MIB counter
*                                  count only packets with priority equal to set1 priority
*        3 MibEgDpSet1En 0x1       When set to 1, the set 1 Egress filter MIB counter
*                                  count only packets with Drop precedence equal to
*                                  set1 DP
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEgressCntrModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              cntrSetNum;
    GT_U8                              portNum;
    GT_U16                             vlanId;
    GT_U8                              tc;
    CPSS_DP_LEVEL_ENT                  dpLevel;
    GT_BOOL                            setPort;
    GT_BOOL                            setVlan;
    GT_BOOL                            setTc;
    GT_BOOL                            setDp;
    CPSS_PORT_EGRESS_CNT_MODE_ENT      setModeBmp = 0;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetNum = (GT_U8)inArgs[1];
    setPort = (GT_BOOL)inArgs[2];
    setVlan = (GT_BOOL)inArgs[3];
    setTc = (GT_BOOL)inArgs[4];
    setDp = (GT_BOOL)inArgs[5];
    portNum = (GT_U8)inArgs[6];
    vlanId = (GT_U16)inArgs[7];
    tc = (GT_U8)inArgs[8];
    dpLevel = (CPSS_DP_LEVEL_ENT)inArgs[9];

    if (setPort == GT_TRUE)
        setModeBmp |=  CPSS_EGRESS_CNT_PORT_E;

    if (setVlan == GT_TRUE)
        setModeBmp |=  CPSS_EGRESS_CNT_VLAN_E;

    if (setTc == GT_TRUE)
        setModeBmp |=  CPSS_EGRESS_CNT_TC_E;

    if (setDp == GT_TRUE)
        setModeBmp |=  CPSS_EGRESS_CNT_DP_E;

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortEgressCntrModeSet(devNum, cntrSetNum, setModeBmp,
                                             portNum, vlanId, tc, dpLevel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChPortEgressCntrsGet
*
* DESCRIPTION:
*       Gets egress countres from specific counter-set.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum        - physical device number
*       cntrSetNum - counter set number.
*
* OUTPUTS:
*       egrCntrPtr - (pointer to) structure of egress countres current values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEgressCntrsGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U8                           cntrSetNum;
    CPSS_PORT_EGRESS_CNTR_STC       egrCntrPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortEgressCntrsGet(devNum, cntrSetNum, &egrCntrPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = egrCntrPtr.outUcFrames;
    inFields[1] = egrCntrPtr.outMcFrames;
    inFields[2] = egrCntrPtr.outBcFrames;
    inFields[3] = egrCntrPtr.brgEgrFilterDisc;
    inFields[4] = egrCntrPtr.txqFilterDisc;
    inFields[5] = egrCntrPtr.outCtrlFrames;




    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d",  inFields[0],  inFields[1],  inFields[2],
                                 inFields[3],  inFields[4],  inFields[5]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortMacCountersEnable
*
* DESCRIPTION:
*       Enable or disable MAC Counters update for this port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev      - device number.
*       portNum  - physical port number (or CPU port)
*       enable   -  enable update of MAC counters
*                   GT_FALSE = MAC counters update for this port is disabled.
*                   GT_TRUE = MAC counters update for this port is enabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*
* COMMENTS:
*       None;
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacCountersEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           dev;
    GT_U8                           portNum;
    GT_BOOL                         enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    CONVERT_DEV_PORT_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacCountersEnable(dev, portNum, enable);

    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* wrCpssDxChGetNext
*
* DESCRIPTION:
*
*   This function is called for ending table printing.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChGetNext

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
/*******************************************************************************
* cpssDxChPortMacSaLsbSet
*
* DESCRIPTION:
*       Set the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*       macSaLsb    - The ls byte of the MAC SA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       see srcAddrMiddle and srcAddrHigh as related parameters..
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacSaLsbSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_U8                              macSaLsb;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    macSaLsb = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacSaLsbSet(devNum, portNum, macSaLsb);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacSaLsbGet
*
* DESCRIPTION:
*       Get the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number
*
* OUTPUTS:
*       macSaLsb    - (pointer to) The ls byte of the MAC SA
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       see srcAddrMiddle and srcAddrHigh as related parameters.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacSaLsbGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_U8                              macSaLsb;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacSaLsbGet(devNum, portNum, &macSaLsb);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", macSaLsb);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortEnableSet
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number, CPU port number
*       enable   - GT_TRUE:  enable port,
*                  GT_FALSE: disable port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            enable;

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
    result = cpssDxChPortEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortEnableGet
*
* DESCRIPTION:
*       Get the Enable/Disable status of a specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       statePtr - Pointer to the Get Enable/disable state of the port.
*                  GT_TRUE for enabled port, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            statePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortEnableGet(devNum, portNum, &statePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", statePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortDuplexModeSet
*
* DESCRIPTION:
*       Set the port to enable and the port mode to half- or full-duplex mode.
*       When port is disabled it does not transmit pending packets and
*       drops packets received from the line.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       dMode    - 0 = The port operates in half-duplex mode.
*                  1 = The port operates in full-duplex mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or dMode
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported duplex mode
*
* COMMENTS:
*       The <SetFullDx> bit is meaningless when <ANDuplex> is set.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortDuplexModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    CPSS_PORT_DUPLEX_ENT               dMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    dMode = (CPSS_PORT_DUPLEX_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortDuplexModeSet(devNum, portNum, dMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortDuplexModeGet
*
* DESCRIPTION:
*       Gets duplex mode for specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       dModePtr - duplex mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortDuplexModeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    CPSS_PORT_DUPLEX_ENT               dModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortDuplexModeGet(devNum, portNum, &dModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dModePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       speed    - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or speed
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported speed
*
* COMMENTS:
*       For XGMII (10 Gbps) ports only CPSS_PORT_SPEED_10000_E is supported.
*       If the port is enabled then the function disables the port before the
*       operation and re-enables it at the end.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSpeedSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    CPSS_PORT_SPEED_ENT                speed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    speed = (CPSS_PORT_SPEED_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSpeedSet(devNum, portNum, speed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       speedPtr - pointer to actual port speed
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSpeedGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    CPSS_PORT_SPEED_ENT                speedPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSpeedGet(devNum, portNum, &speedPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", speedPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortDuplexAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for duplex mode on specified port on
*       specified device.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported state
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortDuplexAutoNegEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortDuplexAutoNegEnableSet(devNum, portNum, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortDuplexAutoNegEnableGet
*
* DESCRIPTION:
*       Get enable/disable status of an Auto-Negotiation for MAC duplex mode
*       per port.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssDxChPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       state    - GT_TRUE for enable Auto-Negotiation for duplex mode,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the duplex auto negotiation
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortDuplexAutoNegEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortDuplexAutoNegEnableGet(devNum, portNum, &state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",state);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortFlowCntrlAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for Flow Control on
*       specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*       state          - GT_TRUE for enable Auto-Negotiation for Flow Control
*                        GT_FALSE otherwise
*       pauseAdvertise - Advertise symmetric flow control support in
*                        Auto-Negotiation. 0 = Disable, 1 = Enable.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported state
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortFlowCntrlAutoNegEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            state;
    GT_BOOL                            pauseAdvertise;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    state = (GT_BOOL)inArgs[2];
    pauseAdvertise = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortFlowCntrlAutoNegEnableSet(devNum, portNum, state,
                                                           pauseAdvertise);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortFlowCntrlAutoNegEnableGet
*
* DESCRIPTION:
*       Get Auto-Negotiation enable/disable state for Flow Control per port
*
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*
*
*
* OUTPUTS:
*
*       pauseAdvertise -   Advertise symmetric flow control support in
*                          Auto-Negotiation. 0 = Disable, 1 = Enable.
*       state          -   GT_TRUE for enable Auto-Negotiation for Flow Control
*                          GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the flow control auto negotiation
*
***********************************************************************************/
static CMD_STATUS wrCpssDxChPortFlowCntrlAutoNegEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            state;
    GT_BOOL                            pauseAdvertise;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortFlowCntrlAutoNegEnableGet(devNum, portNum, &state,
                                                           &pauseAdvertise);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",state,pauseAdvertise);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortSpeedAutoNegEnableSet
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation of interface speed on specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_SUPPORTED - on not supported state
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSpeedAutoNegEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSpeedAutoNegEnableSet(devNum, portNum, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortSpeedAutoNegEnableGet
*
* DESCRIPTION:
*
*   Get status  of  Auto-Negotiation of interface speed on specified port
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       state    - GT_TRUE for enabled Auto-Negotiation of interface speed,
*                  GT_FALSE otherwise
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported state
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CPU port not supports the speed auto negotiation
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSpeedAutoNegEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    OUT  GT_BOOL                       state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSpeedAutoNegEnableGet(devNum, portNum, &state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",state);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortFlowControlEnableSet
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex on specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Flow Control, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortFlowControlEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    state = (CPSS_PORT_FLOW_CONTROL_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortFlowControlEnableSet(devNum, portNum, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPeriodicFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmits of periodic 802.3x flow control.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       enable   - If GT_TRUE, enable transmit.
*                  If GT_FALSE, disable transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPeriodicFcEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            enable;

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
    result = cpssDxChPortPeriodicFcEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortBackPressureEnableSet
*
* DESCRIPTION:
*       Enable/disable of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for enable Back Pressure, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported feature for a port
*
* COMMENTS:
*       For XGMII (10 Gbps) ports feature is not supported.
*       If the port is enabled then the function disables the port before the
*       operation and re-enables it at the end.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortBackPressureEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortBackPressureEnableSet(devNum, portNum, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortLinkStatusGet
*
* DESCRIPTION:
*       Gets Link Status of specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       isLinkUpPtr - GT_TRUE for link up, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortLinkStatusGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            isLinkUpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortLinkStatusGet(devNum, portNum, &isLinkUpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isLinkUpPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortInterfaceModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortInterfaceModeGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    CPSS_PORT_INTERFACE_MODE_ENT       ifModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortInterfaceModeGet(devNum, portNum, &ifModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ifModePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortForceLinkPassEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Pass on specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for force link pass, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortForceLinkPassEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortForceLinkPassEnableSet(devNum, portNum, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortForceLinkDownEnableSet
*
* DESCRIPTION:
*       Enable/disable Force Link Down on specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       state    - GT_TRUE for force link down, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortForceLinkDownEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortForceLinkDownEnableSet(devNum, portNum, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortMruSet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       mruSize    - max receive packet size in bytes. (0..16382)
*                    value must be even
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or
*                          odd value of mruSize
*       GT_HW_ERROR      - on hardware error
*       GT_OUT_OF_RANGE  - mruSize > 16382
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMruSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_U32                             mruSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    mruSize = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMruSet(devNum, portNum, mruSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortMruGet
*
* DESCRIPTION:
*       gets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
*
* OUTPUTS:
*       mruSize    - max receive packet size in bytes. (0..16382)
*                    value must be even
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or
*                          odd value of mruSize
*       GT_HW_ERROR      - on hardware error
*       GT_OUT_OF_RANGE  - mruSize > 16382
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMruGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_U32                             mruSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMruGet(devNum, portNum, &mruSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",mruSize);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCrcCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable 32-bit the CRC checking.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       enable     - If GT_TRUE, enable CRC checking
*                    If GT_FALSE, disable CRC checking
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCrcCheckEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            enable;

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
    result = cpssDxChPortCrcCheckEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortXGmiiModeSet
*
* DESCRIPTION:
*       Sets XGMII mode on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXGmiiModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    CPSS_PORT_XGMII_MODE_ENT           mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    mode = (CPSS_PORT_XGMII_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortXGmiiModeSet(devNum, portNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortExtraIpgSet
*
* DESCRIPTION:
*       Sets the number of 32-bit words to add to the 12-byte IPG.
*
* APPLICABLE DEVICES:  All DxCh devices
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       number   -  number of words
*
* OUTPUTS:
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortExtraIpgSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_U8                              number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    number = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortExtraIpgSet(devNum, portNum, number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortXgmiiLocalFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected local
*       fault messages.
*
* APPLICABLE DEVICES:  All DxCh devices
* INPUTS:
*       devNum         - physical device number
*       portNum        - physical port number
*       isLocalFault   -  ptr to buffer for output value
*
* OUTPUTS:
*       *isLocalFault  - GT_TRUE, if bit set or
*                        GT_FALSE otherwise
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXgmiiLocalFaultGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            isLocalFaultPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortXgmiiLocalFaultGet(devNum, portNum, &isLocalFaultPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isLocalFaultPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortXgmiiRemoteFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected remote
*       fault messages.
*
* APPLICABLE DEVICES:  All DxCh devices
* INPUTS:
*       devNum          - physical device number
*       portNum         - physical port number
*       isRemoteFault   - ptr to buffer for output value
*
* OUTPUTS:
*       *isRemoteFaultPtr  - GT_TRUE, if bit set or
*                            GT_FALSE otherwise
*
* RETURNS:
*       GT_OK       - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXgmiiRemoteFaultGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            isRemoteFaultPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortXgmiiRemoteFaultGet(devNum, portNum,
                                           &isRemoteFaultPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isRemoteFaultPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortMacStatusGet
*
* DESCRIPTION:
*       Reads bits that indicate different problems on specified port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portMacStatusPtr  - info about port MAC
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U8 portNumber;

static CMD_STATUS wrCpssDxChPortMacStatusGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_PORT_MAC_STATUS_STC        portMacStatusPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNumber = 0;

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNumber);

    /* call cpss api function */
    result = cpssDxChPortMacStatusGet(devNum, portNumber, &portMacStatusPtr);

    while (result != GT_OK)
    {
        portNumber++;

        /* call cpss api function */
        result = cpssDxChPortMacStatusGet(devNum, portNumber, &portMacStatusPtr);

        if (portNumber > 63)
        {
            galtisOutput(outArgs, CMD_OK, "%d", -1);
            return CMD_OK;
        }
    }

    inFields[0] = portNumber;
    inFields[1] = portMacStatusPtr.isPortRxPause;
    inFields[2] = portMacStatusPtr.isPortTxPause;
    inFields[3] = portMacStatusPtr.isPortBackPres;
    inFields[4] = portMacStatusPtr.isPortBufFull;
    inFields[5] = portMacStatusPtr.isPortSyncFail;
    inFields[6] = portMacStatusPtr.isPortHiErrorRate;
    inFields[7] = portMacStatusPtr.isPortAnDone;
    inFields[8] = portMacStatusPtr.isPortFatalError;

    portNumber++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d",  inFields[0], inFields[1],
                      inFields[2], inFields[3], inFields[4],
                      inFields[5], inFields[6], inFields[7],
                      inFields[8]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacStatusGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;
    GT_U8                           devNum;
    CPSS_PORT_MAC_STATUS_STC        portMacStatusPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNumber);

    /* call cpss api function */
    result = cpssDxChPortMacStatusGet(devNum, portNumber, &portMacStatusPtr);

    while(result != GT_OK)
    {
        portNumber++;

        if (portNumber > 63)
        {
            galtisOutput(outArgs, CMD_OK, "%d", -1);
            return CMD_OK;
        }

        /* Override Device and Port */
        CONVERT_DEV_PORT_MAC(devNum, portNumber);

        /* call cpss api function */
        result = cpssDxChPortMacStatusGet(devNum, portNumber, &portMacStatusPtr);
    }

    inFields[0] = portNumber;
    inFields[1] = portMacStatusPtr.isPortRxPause;
    inFields[2] = portMacStatusPtr.isPortTxPause;
    inFields[3] = portMacStatusPtr.isPortBackPres;
    inFields[4] = portMacStatusPtr.isPortBufFull;
    inFields[5] = portMacStatusPtr.isPortSyncFail;
    inFields[6] = portMacStatusPtr.isPortHiErrorRate;
    inFields[7] = portMacStatusPtr.isPortAnDone;
    inFields[8] = portMacStatusPtr.isPortFatalError;

    portNumber++;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d",  inFields[0], inFields[1],
                      inFields[2], inFields[3], inFields[4],
                      inFields[5], inFields[6], inFields[7],
                      inFields[8]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortInternalLoopbackEnableSet
*
* DESCRIPTION:
*       Set the internal Loopback state in the packet processor MAC port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*       enable    - If GT_TRUE, enable loopback
*                   If GT_FALSE, disable loopback
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       LoopBack is enabled only on XG devices.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortInternalLoopbackEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            enable;

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
    result = cpssDxChPortInternalLoopbackEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortInternalLoopbackEnableGet
*
* DESCRIPTION:
*       Get Internal Loopback
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNUm    - physical port number
*
* OUTPUTS:
*       enablePtr  - Pointer to the Loopback state.
*                    If GT_TRUE, loopback is enabled
*                    If GT_FALSE, loopback is disabled
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortInternalLoopbackEnableGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortInternalLoopbackEnableGet(devNum, portNum, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortInbandAutoNegEnableSet
*
* DESCRIPTION:
*       Configure Auto-Negotiation mode of MAC for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:  All All DxCh devices
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*       enable  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                 GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK              - on success
*       GT_HW_ERROR        - on hardware error
*       GT_NOT_SUPPORTED   - The feature is not supported the port/device
*
* COMMENTS:
*       Supported only on DX and SOHO packet processors.
*       Not supported for CPU port of DX devices and not supported for XG ports.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*       The change of the Auto-Negotiation causes temporary change of the link
*       to down and up for ports with link up.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortInbandAutoNegEnableSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    GT_BOOL                            enable;

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
    result = cpssDxChPortInbandAutoNegEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortAttributesOnPortGet
*
* DESCRIPTION:
*       Gets port attributes for particular logical port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       portAttributSetArrayPtr - Pointer to attributes values array.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortAttributesOnPortGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                       result;

    GT_U8                           devNum;
    GT_U8                           portNum;
    CPSS_PORT_ATTRIBUTES_STC        portAttributSetArrayPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortAttributesOnPortGet(devNum, portNum,
                                    &portAttributSetArrayPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] =portAttributSetArrayPtr.portLinkUp;
    inFields[1] =portAttributSetArrayPtr.portSpeed;
    inFields[2] =portAttributSetArrayPtr.portDuplexity;




    /* pack and output table fields */
    fieldOutput("%d%d%d",  inFields[0],  inFields[1],  inFields[2]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortPreambleLengthSet
*
* DESCRIPTION:
*       Set the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev          - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction and "both directions"
*                      options (GE ports support only Tx direction)
*       length       - length of preamble in bytes
*                      support only values of 4,8
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPreambleLengthSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    CPSS_PORT_DIRECTION_ENT            direction;
    GT_U32                             length;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[2];
    length = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPreambleLengthSet(devNum, portNum, direction, length);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacSaBaseSet
*
* DESCRIPTION:
*       Sets the Source addresses of all of the device's ports.
*       This addresses is used as the MAC SA for Flow Control Packets
*       transmitted by the device.In addition this address is used for
*       reception of Flow Control packets.
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
static CMD_STATUS wrCpssDxChPortMacSaBaseSet
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
    result = cpssDxChPortMacSaBaseSet(devNum, &macPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortMacSaBaseGet
*
* DESCRIPTION:
*       Sets the Source addresses of all of the device's ports.
*       This addresses is used as the MAC SA for Flow Control Packets
*       transmitted by the device.In addition this address is used for
*       reception of Flow Control packets.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev - device number
*
* OUTPUTS:
*       macPtr - (pointer to)The system Mac address.
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
static CMD_STATUS wrCpssDxChPortMacSaBaseGet
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
    result = cpssDxChPortMacSaBaseGet(devNum, &macPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b", macPtr.arEther);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortRxFcProfileSet
*
* DESCRIPTION:
*       Bind a port to a flow control profile.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortRxFcProfileSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT    profileSet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortRxFcProfileSet(devNum, portNum, profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortRxFcProfileGet
*
* DESCRIPTION:
*       Get the port's flow control profile.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       profileSetPtr - (pointer to) the Profile Set in which the Flow Control
*                       Parameters are associated.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortRxFcProfileGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U8                              portNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT    profileSetPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortRxFcProfileGet(devNum, portNum, &profileSetPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileSetPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortXonLimitSet
*
* DESCRIPTION:
*       Sets X-ON port limit. When Flow Control is enabled, X-ON packet sent
*       when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       xonLimit   - X-ON limit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXonLimitSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT    profileSet;
    GT_U32                             xonLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];
    xonLimit = inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortXonLimitSet(devNum, profileSet, xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortXonLimitGet
*
* DESCRIPTION:
*       Gets the X-ON port limit. When Flow Control is enabled, X-ON packet
*       sent when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters of
*                    are associated.
*
* OUTPUTS:
*       xonLimitPtr  - (pointer to) the X-ON limit value
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXonLimitGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT    profileSet;
    GT_U32                             xonLimitPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortXonLimitGet(devNum, profileSet, &xonLimitPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", xonLimitPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortXoffLimitSet
*
* DESCRIPTION:
*       Sets X-OFF port limit. When Flow Control is enabled, X-OFF packet sent
*       when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       xoffLimit  - X-OFF limit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXoffLimitSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT    profileSet;
    GT_U32                             xoffLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];
    xoffLimit = inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortXoffLimitSet(devNum, profileSet, xoffLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortXoffLimitGet
*
* DESCRIPTION:
*       Gets the X-OFF port limit. When Flow Control is enabled, X-OFF packet
*       sent when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       xoffLimitPtr - (pointer to) the X-OFF limit value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXoffLimitGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT    profileSet;
    GT_U32                             xoffLimitPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortXoffLimitGet(devNum, profileSet, &xoffLimitPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", xoffLimitPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortRxBufLimitSet
*
* DESCRIPTION:
*       Sets receive buffer limit threshold for the specified port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*       rxBufLimit - limit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortRxBufLimitSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT    profileSet;
    GT_U32                             rxBufLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];
    rxBufLimit = inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortRxBufLimitSet(devNum, profileSet, rxBufLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortRxBufLimitGet
*
* DESCRIPTION:
*       Gets the receive buffer limit threshold for the specified port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Profile Set in which the Flow Control Parameters are
*                    associated.
*
* OUTPUTS:
*       rxBufLimitPtr - (pointer to) the rx buffer limit value.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortRxBufLimitGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    CPSS_PORT_RX_FC_PROFILE_SET_ENT    profileSet;
    GT_U32                             rxBufLimitPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_RX_FC_PROFILE_SET_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortRxBufLimitGet(devNum, profileSet, &rxBufLimitPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", rxBufLimitPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCpuRxBufCountGet
*
* DESCRIPTION:
*       Gets receive buffer count for the CPU on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - The device to get the Cpu Rx buffer count for.
*
* OUTPUTS:
*       cpuRxBufCntPtr - (Pointer to) number of buffers allocated for the CPU.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCpuRxBufCountGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    GT_U16                             cpuRxBufCntPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortCpuRxBufCountGet(devNum, &cpuRxBufCntPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cpuRxBufCntPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortFcHolSysModeSet
*
* DESCRIPTION:
*       Set Flow Control or HOL system mode on the specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum       - physical device number
*       modeFcHol - GT_FC_E  : set Flow Control mode
*                   GT_HOL_E : set HOL system mode
*                   GT_HOL_CELL_E : set cell based HOL for SALSA 2 only
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_NOT_SUPPORTED - on not supported HOF FC mode
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortFcHolSysModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                          result;

    GT_U8                              devNum;
    CPSS_DXCH_PORT_HOL_FC_ENT          modeFcHol;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    modeFcHol = (CPSS_DXCH_PORT_HOL_FC_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortFcHolSysModeSet(devNum, modeFcHol);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortBuffersModeSet
*
* DESCRIPTION:
*      The function sets buffers mode either shared or divided.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - device number.
*       bufferMode - buffers mode, divided or shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortBuffersModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_BUFFERS_MODE_ENT bufferMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    bufferMode = (CPSS_DXCH_PORT_BUFFERS_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortBuffersModeSet(devNum, bufferMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortBuffersModeGet
*
* DESCRIPTION:
*      The function gets buffers mode either shared or divided.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       bufferModePtr - pointer to the buffer mode.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortBuffersModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_BUFFERS_MODE_ENT bufferMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortBuffersModeGet(devNum, &bufferMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", bufferMode);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortGroupXonLimitSet
*
* DESCRIPTION:
*       The function sets the port-group X-ON threshold.
*       When Flow Control is enabled, X-ON packet sent
*       when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*       xonLimit  -  X-ON limit in buffers. Range: 0 - 0xFFF.
*                    Resolution of this field in hardware is two buffers.
*                    The value is rounded down before write to HW.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong port number or device
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - on out of range value
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortGroupXonLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_GROUP_ENT portGroup;
    GT_U32 xonLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portGroup = (CPSS_DXCH_PORT_GROUP_ENT)inArgs[1];
    xonLimit = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortGroupXonLimitSet(devNum, portGroup, xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortGroupXonLimitGet
*
* DESCRIPTION:
*       The function gets the port-group X-ON threshold.
*       When Flow Control is enabled, X-ON packet
*       sent when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       xonLimitPtr  - Pointer to the  X-ON limit in buffers.
*                      Resolution of this field in hardware is two buffers.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortGroupXonLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_GROUP_ENT portGroup;
    GT_U32 xonLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portGroup = (CPSS_DXCH_PORT_GROUP_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortGroupXonLimitGet(devNum, portGroup, &xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", xonLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortGroupXoffLimitSet
*
* DESCRIPTION:
*       The function sets the port-group X-OFF threshold.
*       When Flow Control is enabled, X-OFF packet sent
*       when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*       xoffLimit  - X-OFF limit in buffers. Range: 0 - 0xFFF.
*                    Resolution of this field in hardware is two buffers.
*                    The value is rounded down before write to HW.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong port number or device
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - on out of range value
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortGroupXoffLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_GROUP_ENT portGroup;
    GT_U32 xoffLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portGroup = (CPSS_DXCH_PORT_GROUP_ENT)inArgs[1];
    xoffLimit = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortGroupXoffLimitSet(devNum, portGroup, xoffLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortGroupXoffLimitGet
*
* DESCRIPTION:
*       The function gets the port-group X-OFF threshold.
*       When Flow Control is enabled, X-OFF packet
*       sent when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       xoffLimitPtr  - Pointer to the X-OFF limit in buffers.
*                       Resolution of this field in hardware is two buffers.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortGroupXoffLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_GROUP_ENT portGroup;
    GT_U32 xoffLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portGroup = (CPSS_DXCH_PORT_GROUP_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortGroupXoffLimitGet(devNum, portGroup, &xoffLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", xoffLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortGroupRxBufLimitSet
*
* DESCRIPTION:
*       Sets receive buffer limit threshold for the specified port-group.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*       rxBufLimit - Maximal number of buffers that may be allocated for the
*                    ports belong to a group. Range: 0-0xFFF.
*                    Resolution of this field in hardware is four buffers.
*                    The value is rounded down before write to HW.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - on wrong port number or device
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - on out of range value
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortGroupRxBufLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_GROUP_ENT portGroup;
    GT_U32 rxBufLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portGroup = (CPSS_DXCH_PORT_GROUP_ENT)inArgs[1];
    rxBufLimit = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortGroupRxBufLimitSet(devNum, portGroup, rxBufLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortGroupRxBufLimitGet
*
* DESCRIPTION:
*       Gets the receive buffer limit threshold for the specified port-group.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       rxBufLimitPtr - Pointer to value of maximal number of buffers that
*                       may be allocated for the ports belong to a group.
*                       Resolution of this field in hardware is four buffers.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortGroupRxBufLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_GROUP_ENT portGroup;
    GT_U32 rxBufLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portGroup = (CPSS_DXCH_PORT_GROUP_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortGroupRxBufLimitGet(devNum, portGroup, &rxBufLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", rxBufLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCrossChipFcPacketRelayEnableSet
*
* DESCRIPTION:
*     Enable/Disable SOHO Cross Chip Flow control packet relay.
*
* APPLICABLE DEVICES:  All DxCh devices.
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE:  Enable SOHO Cross Chip Flow control packet relay.
*                    GT_FALSE: Disable SOHO Cross Chip Flow control packet relay.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCrossChipFcPacketRelayEnableSet
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
    result = cpssDxChPortCrossChipFcPacketRelayEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCrossChipFcPacketRelayEnableGet
*
* DESCRIPTION:
*     Gets the current status of SOHO Cross Chip Flow control packet relay.
*
* APPLICABLE DEVICES:  All DxCh devices.
*
* INPUTS:
*       devNum    - device number.
*
* OUTPUTS:
*       enablePtr - pointer to SOHO Cross Chip Flow control packet relay staus:
*                   GT_TRUE  -  Enable SOHO Cross Chip Flow control packet relay.
*                   GT_FALSE - Disable SOHO Cross Chip Flow control packet relay.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong devNum
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCrossChipFcPacketRelayEnableGet
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
    result = cpssDxChPortCrossChipFcPacketRelayEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortClearByBufferEnableSet
*
* DESCRIPTION:
*       Set port to work in Clear By Buffer mode.
*
* APPLICABLE DEVICES:  DxCh1 and DxCh2 devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*       enable     - GT_TRUE: Clear By Buffer mode.
*                    GT_FALSE: Regular Clear mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortClearByBufferEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

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
    result = cpssDxChPortClearByBufferEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortClearByBufferEnableGet
*
* DESCRIPTION:
*       Get port Buffer Memory Clear type.
*
* APPLICABLE DEVICES:  DxCh1 and DxCh2 devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       enable     - (pointer to) GT_TRUE: Clear By Buffer mode.
*                                 GT_FALSE: Regular Clear mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortClearByBufferEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

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
    result = cpssDxChPortClearByBufferEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChGlobalXonLimitSet
*
* DESCRIPTION:
*       Sets the Global Xon limit value. Enables global control over the
*       number of buffers allocated for all ports by Global Xon limit setting.
*       When the total number of buffers allocated reaches this threshold
*       (xonLimit), all ports that support 802.3x Flow Control send an Xon
*       frame.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - PP's device number.
*       xonLimit   - Xon limit.
*                    Resolution of this field is two buffers of 256 Byte each.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or xonLimit.
*
* COMMENTS:
*       1. The Global Xon limit must be not be greater than Global Xoff Limit.
*       2. To disable Global Xon, the 2*xonLimit should be set greater-or-equal
*          of 8*<Max Buffer Limit> (maximal number of buffers allocated for
*          all ports).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChGlobalXonLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 xonLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    xonLimit = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChGlobalXonLimitSet(devNum, xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChGlobalXonLimitGet
*
* DESCRIPTION:
*       Gets the Global Xon limit value.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum          - PP's device number.
*
* OUTPUTS:
*       xonLimitPtr     - pointer to the Xon limit value (resolution of this
*                         value is two buffers of 256 Byte each).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChGlobalXonLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 xonLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChGlobalXonLimitGet(devNum, &xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", xonLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChGlobalXoffLimitSet
*
* DESCRIPTION:
*       Sets the Global Xoff limit value. Enables global control over the
*       number of buffers allocated for all ports by Global Xoff limit setting.
*       When the total number of buffers allocated reaches this threshold
*       (xoffLimit), all ports that support 802.3x Flow Control send a pause
*       frame.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum      - PP's device number.
*       xoffLimit   - Xoff limit.
*                     Resolution of this field is two buffers of 256 Byte each.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum or xoffLimit.
*
* COMMENTS:
*       To disable Global Xoff, the 2*xoffLimit should be set greater-or-equal
*       of 8*<Max Buffer Limit> (maximal number of buffers allocated for
*          all ports).
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChGlobalXoffLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 xoffLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    xoffLimit = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChGlobalXoffLimitSet(devNum, xoffLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChGlobalXoffLimitGet
*
* DESCRIPTION:
*       Gets the Global Xoff limit value.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum          - PP's device number.
*
* OUTPUTS:
*       xonLimitPtr     - pointer to the Xoff limit value (resolution of this
*                         value is two buffers of 256 Byte each).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_HW_ERROR     - on hardware error.
*       GT_BAD_PARAM    - on wrong devNum.
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChGlobalXoffLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 xoffLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChGlobalXoffLimitGet(devNum, &xoffLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", xoffLimit);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxBindPortToSchedulerProfileGet
*
* DESCRIPTION:
*       Get scheduler profile set that is binded to the port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       profileSetPtr - The Profile Set in which the scheduler's parameters are
*                        associated.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxBindPortToSchedulerProfileGet
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
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT profileSet;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxBindPortToSchedulerProfileGet(devNum, portNum, &profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileSet);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxTailDropProfileGet
*
* DESCRIPTION:
*       Get sharing status of buffers and descriptors for all queues of an
*       egress port and get maximal port's limits of buffers and descriptors.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*   devNum        - physical device number
*   profileSet    - the Profile Set in which the Traffic
*                   Class Drop Parameters is associated
*
* OUTPUTS:
*   enableSharingPtr - Pointer to sharing status.
*                  If GT_TRUE Sharing is enabled. If a packet is to be queued
*                  to an empty queue, it is queued even if the number of buffers
*                   or descriptors allocated to this port exceeds the
*                   < portMaxBuffLimit > and < portMaxDescrLimit >.
*                   If GT_FALSE Sharing is disabled. If the sum of buffers or
*                   descriptors allocated per queue is larger than the number of
*                   buffers or descriptors allocated to this port
*                   < portMaxBuffLimit > and < portMaxDescrLimit >, then some of
*                   the queues may be starved.
*   portMaxBuffLimitPtr - Pointer to maximal number of buffers for a port
*   portMaxDescrLimitPtr - Pointer to maximal number of descriptors for a port
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTailDropProfileGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT profileSet;
    GT_BOOL enableSharing;
    GT_U16 portMaxBuffLimit;
    GT_U16 portMaxDescrLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxTailDropProfileGet(devNum, profileSet, &enableSharing, &portMaxBuffLimit, &portMaxDescrLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", enableSharing, portMaxBuffLimit, portMaxDescrLimit);

    return CMD_OK;
}


/*******************************************************************************
*  cpssDxChPortTxBindPortToDpGet
*
* DESCRIPTION:
*       Get Drop Profile Set according to a Physical Port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum        - physical device number
*       portNum       - physical port number
*
* OUTPUTS:
*       profileSetPtr    - Pointer to the Profile Set in which the Traffic
*                          Class Drop Parameters is associated
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxBindPortToDpGet
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
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT profileSet;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxBindPortToDpGet(devNum, portNum, &profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileSet);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxTailDropUcEnableSet
*
* DESCRIPTION:
*       Enable/Disable Tail Drop for all Unicast packets.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   -  device number.
*       enable   -  GT_TRUE  - Tail Drop is enabled.
*                   GT_FALSE - The Tail Drop limits for all packets are
*                              ignored and Unicast packet is dropped only
*                              when the Tx Queue has reached its global
*                              descriptors limit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramters value
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTailDropUcEnableSet
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
    result = cpssDxChPortTxTailDropUcEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxTailDropUcEnableGet
*
* DESCRIPTION:
*       Get enable/disable Tail Drop status for all Unicast packets.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum      -  device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to tail drop status:
*                          GT_TRUE  - Tail Drop is enabled.
*                          GT_FALSE - The Tail Drop limits for all packets are
*                                     ignored and Unicast packet is dropped only
*                                     when the Tx Queue has reached its global
*                                     descriptors limit.
*
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramters value
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTailDropUcEnableGet
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
    result = cpssDxChPortTxTailDropUcEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxBufferTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Tail Drop according to the number of buffers in the
*       queues.
*
* APPLICABLE DEVICES:  DxCh, DxCh2, DxCh3, DxChXcat
*
* INPUTS:
*       devNum   - device number.
*       enable   -  GT_TRUE  -  Enables Tail Drop according to the number of
*                               buffers in the queues. Tail drop use both
*                               decsriptiors and buffers limits.
*                   GT_FALSE - Tail drop use only descriptor limits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxBufferTailDropEnableSet
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
    result = cpssDxChPortTxBufferTailDropEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxBufferTailDropEnableGet
*
* DESCRIPTION:
*       Get enable/disable Tail Drop status according to the number of buffers
*       in the queues.
*
* APPLICABLE DEVICES:  DxCh, DxCh2, DxCh3, DxChXcat
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to Tail Drop status:
*                       GT_TRUE  - Enables Tail Drop according to the number of
*                                  buffers in the queues. Tail drop use both
*                                  decsriptiors and buffers limits.
*                       GT_FALSE - Tail drop use only descriptor limits.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxBufferTailDropEnableGet
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
    result = cpssDxChPortTxBufferTailDropEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxBuffersSharingMaxLimitSet
*
* DESCRIPTION:
*       Sets the maximal number of shared buffers in a Tail Drop system.
*       When the total number of buffers exceeds this threshold, all of the
*       shared buffers are currently used and packets are queued according to
*       their target queue guaranteed limits.
*
* APPLICABLE DEVICES:  DxCh2, DxCh3, DxChXcat.
*
* INPUTS:
*       devNum  - physical device number
*       limit   -  maximal number of shared buffers in a Tail Drop system.
*                  - DxCh2 - Range: 0..0xFFF.
*                          To disable Buffers Sharing for Transmit queues
*                          set this field to 0xFFF.
*                  - DxCh3 and above - Range: 0..0x3FFF.
*                          To disable Buffers Sharing for Transmit queues
*                          set this field to 0.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxBuffersSharingMaxLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 limit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    limit = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxBuffersSharingMaxLimitSet(devNum, limit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxBuffersSharingMaxLimitGet
*
* DESCRIPTION:
*       Gets the maximal number of shared buffers in a Tail Drop system.
*       When the total number of buffers exceeds this threshold, all of the
*       shared buffers are currently used and packets are queued according to
*       their target queue guaranteed limits.
*
* APPLICABLE DEVICES:   DxCh2, DxCh3, DxChXcat.
*
* INPUTS:
*       devNum  - physical device number
*
*
* OUTPUTS:
*       limitPtr   -  pointer to maximal number of shared buffers
*                     in a Tail Drop system
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxBuffersSharingMaxLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 limit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxBuffersSharingMaxLimitGet(devNum, &limit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", limit);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortMacCountersClearOnReadSet
*
* DESCRIPTION:
*       Enable or disable MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - device number.
*       portNum     - physical port number (or CPU port)
*       enable      - enable clear on read for MAC counters
*                   GT_FALSE - Counters are not cleared.
*                   GT_TRUE - Counters are cleared.

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacCountersClearOnReadSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_U8      portNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacCountersClearOnReadSet(dev, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersClearOnReadGet
*
* DESCRIPTION:
*       Get "Clear on read" status of MAC Counters per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr   - (pointer to) "Clear on read" status for MAC counters
*                     GT_FALSE - Counters are not cleared.
*                     GT_TRUE - Counters are cleared.
*
* RETURNS:
*       GT_OK                       - on success.
*       GT_FAIL                     - on error.
*       GT_HW_ERROR                 - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*       GT_NOT_APPLICABLE_DEVICE    - on not applicable device
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacCountersClearOnReadGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_U8      portNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacCountersClearOnReadGet(dev, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersRxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for received packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - device number.
*       portNum     - physical port number
*       enable      - enable updating of the counters for received packets
*                   GT_FALSE - Counters are updated.
*                   GT_TRUE - Counters are not updated.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacCountersRxHistogramEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_U8      portNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacCountersRxHistogramEnable(dev, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersTxHistogramEnable
*
* DESCRIPTION:
*       Enable/disable updating of the RMON Etherstat histogram
*       counters for transmitted packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       dev         - device number.
*       portNum     - physical port number
*       enable      - enable updating of the counters for transmitted packets
*                   GT_FALSE - Counters are updated.
*                   GT_TRUE - Counters are not updated.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success.
*       GT_FAIL     - on error.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - on wrong device number or wrong physical port number
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacCountersTxHistogramEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      dev;
    GT_U8      portNum;
    GT_BOOL    enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacCountersTxHistogramEnable(dev, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxDp1SharedEnableSet
*
* DESCRIPTION:
*       Enable/Disable packets with DP1 (Red) to use the shared
*       descriptors / buffers pool.
*
* APPLICABLE DEVICES: DxCh3, DxChXcat.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE   - Allow DP1 (Red) in shared pool.
*                 GT_FALSE  - Disallow DP1 to be shared.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxDp1SharedEnableSet
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
    result = cpssDxChPortTxDp1SharedEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxDp1SharedEnableGet
*
* DESCRIPTION:
*       Gets current status of  shared  descriptors / buffer pool usage
*       for packets with DP1 (Yellow).
*
* APPLICABLE DEVICES: DxCh3, DxChXcat.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - pointer to current status of
*                    shared  descroptors / Buffers pool usage:
*                  - GT_TRUE   - Allow DP1 (Yellow) in shared pool.
*                  - GT_FALSE  - Allow DP0 (Green) only in shared pool.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxDp1SharedEnableGet
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
    result = cpssDxChPortTxDp1SharedEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxTcSharedProfileEnableSet
*
* DESCRIPTION:
*       Enable/Disable usage of the shared  descriptors / buffer pool for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* APPLICABLE DEVICES:  All DxCh3 devices
*
* INPUTS:
*       devNum   - physical device number
*       pfSet    - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc       - the Traffic Class, range 0..7.
*       enable   - GT_TRUE -  Enable usage of the shared descriptors/buffer pool
*                  GT_FALSE -  Disable usage of the shared descriptors/buffer pool
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - on wrong device number, pfSet
*       GT_OUT_OF_RANGE   - on out of range value
*       GT_HW_ERROR       - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTcSharedProfileEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet;
    GT_U8 tc;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    tc = (GT_U8)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* call cpss api function */
    if( GT_FALSE == enable )
    {
        result = cpssDxChPortTxTcSharedProfileEnableSet(devNum, pfSet, tc,
                                                        CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E);
    }
    else /* GT_TRUE == enable */
    {
        result = cpssDxChPortTxTcSharedProfileEnableSet(devNum, pfSet, tc,
                                                        CPSS_PORT_TX_SHARED_DP_MODE_ALL_E);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxTcSharedProfileEnableSet
*
* DESCRIPTION:
*       Enable/Disable usage of the shared  descriptors / buffer pool for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum   - physical device number
*       pfSet    - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc       - the Traffic Class, range 0..7.
*       enableMode  - Drop Precedence (DPs) enabled mode for sharing.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTcSharedProfileEnableSet1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet;
    GT_U8 tc;
    CPSS_PORT_TX_SHARED_DP_MODE_ENT enableMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    tc = (GT_U8)inArgs[2];
    enableMode = (CPSS_PORT_TX_SHARED_DP_MODE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortTxTcSharedProfileEnableSet(devNum, pfSet, tc, enableMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxTcSharedProfileEnableGet
*
* DESCRIPTION:
*       Gets usage of the shared  descriptors / buffer pool status for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* APPLICABLE DEVICES:  All DxCh3 devices
*
* INPUTS:
*       devNum   - physical device number
*       pfSet    - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc       - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       enablePtr   -  pointer to usage of
*                      the shared  descriptors / buffer pool status
*                  GT_TRUE  -  Enable usage of the shared descriptors/buffer pool
*                  GT_FALSE -  Disable usage of the shared descriptors/buffer pool
*
* RETURNS:
*       GT_OK             - on success
*       GT_FAIL           - on error
*       GT_BAD_PARAM      - on wrong device number, pfSet
*       GT_HW_ERROR       - on hardware error
*       GT_BAD_PTR        - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE   - on out of range value
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTcSharedProfileEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet;
    GT_U8 tc;
    GT_BOOL enable;
    CPSS_PORT_TX_SHARED_DP_MODE_ENT enableMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    tc = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxTcSharedProfileEnableGet(devNum, pfSet, tc, &enableMode);

    enable = ( CPSS_PORT_TX_SHARED_DP_MODE_DISABLE_E == enableMode ) ? GT_FALSE : GT_TRUE ;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxTcSharedProfileEnableGet
*
* DESCRIPTION:
*       Gets usage of the shared  descriptors / buffer pool status for
*       packets with the traffic class (tc) that are transmited via a
*       port that is associated with the Profile (pfSet).
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum   - physical device number
*       pfSet    - the Profile Set in which the Traffic
*                  Class Drop Parameters is associated
*       tc       - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       enableModePtr   - (pointer to) Drop Precedence (DPs) enabled mode for
*                         sharing.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTcSharedProfileEnableGet1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet;
    GT_U8 tc;
    CPSS_PORT_TX_SHARED_DP_MODE_ENT enableMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    tc = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxTcSharedProfileEnableGet(devNum, pfSet, tc, &enableMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enableMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperTokenBucketMtuSet
*
* DESCRIPTION:
*       Set Token bucket maximum transmission unit (MTU).
*       The token bucket MTU defines the minimum number of tokens required to
*       permit a packet to be transmitted (i.e., conforming).
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       mtu      - MTU for egress rate shaper
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, mtu
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortTxShaperTokenBucketMtuSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mtu = (CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxShaperTokenBucketMtuSet(devNum, mtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;


}

/*******************************************************************************
* cpssDxChPortTxShaperTokenBucketMtuGet
*
*       Get Token bucket maximum transmission unit (MTU).
*       The token bucket MTU defines the minimum number of tokens required to
*       permit a packet to be transmitted (i.e., conforming).
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*
* OUTPUTS:
*       mtuPtr   -  pointer to MTU for egress rate shaper
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_STATE             - on invalid hardware value read
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
CMD_STATUS wrCpssDxChPortTxShaperTokenBucketMtuGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxShaperTokenBucketMtuGet(devNum, &mtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mtu);

    return CMD_OK;

}



/*******************************************************************************
* cpssDxChPortMacCounterCaptureGet
*
* DESCRIPTION:
*       Gets the captured Ethernet MAC counter for a specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - not supported counter on the port
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*       Overiding of previous capture by another port in a group is occurs,
*       when the capture triggered port
*       (see cpssDxChPortMacCountersCaptureTriggerSet) and previous
*       capture triggered port are in the same group.
*       (This comment dosn't connected to HyperG.Stack ports.)
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacCounterCaptureGet
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
    CPSS_PORT_MAC_COUNTERS_ENT cntrName;
    GT_U64 cntrValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    cntrName=(CPSS_PORT_MAC_COUNTERS_ENT)inArgs[2];

    /* Convert counter to zero base value */
    cntrName = (cntrName) ? (cntrName - 1) : 0;

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacCounterCaptureGet(devNum, portNum, cntrName, &cntrValue);

     /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", cntrValue.l[0], cntrValue.l[1]);


    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersCaptureOnPortGet
*
* DESCRIPTION:
*       Gets captured Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Overiding of previous capture by another port in a group is occurs,
*       when the capture triggered port
*       (see cpssDxChPortMacCountersCaptureTriggerSet) and previous
*       capture triggered port are in the same group.
*       (This comment dosn't connected to HyperG.Stack ports.)
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacCountersCaptureOnPortGet
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
    CPSS_PORT_MAC_COUNTER_SET_STC portMacCounterSetArray;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacCountersCaptureOnPortGet(devNum, portNum, &portMacCounterSetArray);

     if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
     portMacCounterSetArray.goodPktsRcv.l[0] =
         portMacCounterSetArray.ucPktsRcv.l[0] +
         portMacCounterSetArray.brdcPktsRcv.l[0] +
         portMacCounterSetArray.mcPktsRcv.l[0];

     portMacCounterSetArray.goodPktsSent.l[0] =
         portMacCounterSetArray.ucPktsSent.l[0] +
         portMacCounterSetArray.brdcPktsSent.l[0] +
         portMacCounterSetArray.mcPktsSent.l[0];

     inFields[0] = portMacCounterSetArray.goodOctetsRcv.l[0];
     inFields[1] = portMacCounterSetArray.goodOctetsRcv.l[1];
     inFields[2] = portMacCounterSetArray.badOctetsRcv.l[0];
     inFields[3] = portMacCounterSetArray.badOctetsRcv.l[1];
     inFields[4] = portMacCounterSetArray.macTransmitErr.l[0];
     inFields[5] = portMacCounterSetArray.macTransmitErr.l[1];
     inFields[6] = portMacCounterSetArray.goodPktsRcv.l[0];
     inFields[7] = portMacCounterSetArray.goodPktsRcv.l[1];
     inFields[8] = portMacCounterSetArray.badPktsRcv.l[0];
     inFields[9] = portMacCounterSetArray.badPktsRcv.l[1];
     inFields[10] = portMacCounterSetArray.brdcPktsRcv.l[0];
     inFields[11] = portMacCounterSetArray.brdcPktsRcv.l[1];
     inFields[12] = portMacCounterSetArray.mcPktsRcv.l[0];
     inFields[13] = portMacCounterSetArray.mcPktsRcv.l[1];
     inFields[14] = portMacCounterSetArray.pkts64Octets.l[0];
     inFields[15] = portMacCounterSetArray.pkts64Octets.l[1];
     inFields[16] = portMacCounterSetArray.pkts65to127Octets.l[0];
     inFields[17] = portMacCounterSetArray.pkts65to127Octets.l[1];
     inFields[18] = portMacCounterSetArray.pkts128to255Octets.l[0];
     inFields[19] = portMacCounterSetArray.pkts128to255Octets.l[1];
     inFields[20] = portMacCounterSetArray.pkts256to511Octets.l[0];
     inFields[21] = portMacCounterSetArray.pkts256to511Octets.l[1];
     inFields[22] = portMacCounterSetArray.pkts512to1023Octets.l[0];
     inFields[23] = portMacCounterSetArray.pkts512to1023Octets.l[1];
     inFields[24] = portMacCounterSetArray.pkts1024tomaxOoctets.l[0];
     inFields[25] = portMacCounterSetArray.pkts1024tomaxOoctets.l[1];
     inFields[26] = portMacCounterSetArray.goodOctetsSent.l[0];
     inFields[27] = portMacCounterSetArray.goodOctetsSent.l[1];
     inFields[28] = portMacCounterSetArray.goodPktsSent.l[0];
     inFields[29] = portMacCounterSetArray.goodPktsSent.l[1];
     inFields[30] = portMacCounterSetArray.excessiveCollisions.l[0];
     inFields[31] = portMacCounterSetArray.excessiveCollisions.l[1];
     inFields[32] = portMacCounterSetArray.mcPktsSent.l[0];
     inFields[33] = portMacCounterSetArray.mcPktsSent.l[1];
     inFields[34] = portMacCounterSetArray.brdcPktsSent.l[0];
     inFields[35] = portMacCounterSetArray.brdcPktsSent.l[1];
     inFields[36] = portMacCounterSetArray.unrecogMacCntrRcv.l[0];
     inFields[37] = portMacCounterSetArray.unrecogMacCntrRcv.l[1];
     inFields[38] = portMacCounterSetArray.fcSent.l[0];
     inFields[39] = portMacCounterSetArray.fcSent.l[1];
     inFields[40] = portMacCounterSetArray.goodFcRcv.l[0];
     inFields[41] = portMacCounterSetArray.goodFcRcv.l[1];
     inFields[42] = portMacCounterSetArray.dropEvents.l[0];
     inFields[43] = portMacCounterSetArray.dropEvents.l[1];
     inFields[44] = portMacCounterSetArray.undersizePkts.l[0];
     inFields[45] = portMacCounterSetArray.undersizePkts.l[1];
     inFields[46] = portMacCounterSetArray.fragmentsPkts.l[0];
     inFields[47] = portMacCounterSetArray.fragmentsPkts.l[1];
     inFields[48] = portMacCounterSetArray.oversizePkts.l[0];
     inFields[49] = portMacCounterSetArray.oversizePkts.l[1];
     inFields[50] = portMacCounterSetArray.jabberPkts.l[0];
     inFields[51] = portMacCounterSetArray.jabberPkts.l[1];
     inFields[52] = portMacCounterSetArray.macRcvError.l[0];
     inFields[53] = portMacCounterSetArray.macRcvError.l[1];
     inFields[54] = portMacCounterSetArray.badCrc.l[0];
     inFields[55] = portMacCounterSetArray.badCrc.l[1];
     inFields[56] = portMacCounterSetArray.collisions.l[0];
     inFields[57] = portMacCounterSetArray.collisions.l[1];
     inFields[58] = portMacCounterSetArray.lateCollisions.l[0];
     inFields[59] = portMacCounterSetArray.lateCollisions.l[1];
     inFields[60] = portMacCounterSetArray.badFcRcv.l[0];
     inFields[61] = portMacCounterSetArray.badFcRcv.l[1];




     /* pack and output table fields */
     fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                 inFields[0],  inFields[1],
                 inFields[2],  inFields[3],  inFields[4],  inFields[5],
                 inFields[6],  inFields[7],  inFields[8],  inFields[9],
                 inFields[10], inFields[11], inFields[12], inFields[13],
                 inFields[14], inFields[15], inFields[16], inFields[17],
                 inFields[18], inFields[19], inFields[20], inFields[21],
                 inFields[22], inFields[23], inFields[24], inFields[25],
                 inFields[26], inFields[27], inFields[28], inFields[29],
                 inFields[30], inFields[31], inFields[32], inFields[33],
                 inFields[34], inFields[35], inFields[36], inFields[37],
                 inFields[38], inFields[39], inFields[40], inFields[41],
                 inFields[42], inFields[43], inFields[44], inFields[45],
                 inFields[46], inFields[47], inFields[48], inFields[49],
                 inFields[50], inFields[51], inFields[52], inFields[53],
                 inFields[54], inFields[55], inFields[56], inFields[57],
                 inFields[58], inFields[59], inFields[60], inFields[61]);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersCaptureOnPortGet
*
* DESCRIPTION:
*       Gets captured Ethernet MAC counter for a particular Port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*       Overiding of previous capture by another port in a group is occurs,
*       when the capture triggered port
*       (see cpssDxChPortMacCountersCaptureTriggerSet) and previous
*       capture triggered port are in the same group.
*       (This comment dosn't connected to HyperG.Stack ports.)
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacCountersCaptureOnPortGetExt
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
    CPSS_PORT_MAC_COUNTER_SET_STC portMacCounterSetArray;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacCountersCaptureOnPortGet(devNum, portNum, &portMacCounterSetArray);

     if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = portMacCounterSetArray.goodOctetsRcv.l[0];
    inFields[1] = portMacCounterSetArray.goodOctetsRcv.l[1];
    inFields[2] = portMacCounterSetArray.badOctetsRcv.l[0];
    inFields[3] = portMacCounterSetArray.badOctetsRcv.l[1];
    inFields[4] = portMacCounterSetArray.macTransmitErr.l[0];
    inFields[5] = portMacCounterSetArray.macTransmitErr.l[1];
    inFields[6] = portMacCounterSetArray.brdcPktsRcv.l[0];
    inFields[7] = portMacCounterSetArray.brdcPktsRcv.l[1];
    inFields[8] = portMacCounterSetArray.mcPktsRcv.l[0];
    inFields[9] = portMacCounterSetArray.mcPktsRcv.l[1];
    inFields[10] = portMacCounterSetArray.pkts64Octets.l[0];
    inFields[11] = portMacCounterSetArray.pkts64Octets.l[1];
    inFields[12] = portMacCounterSetArray.pkts65to127Octets.l[0];
    inFields[13] = portMacCounterSetArray.pkts65to127Octets.l[1];
    inFields[14] = portMacCounterSetArray.pkts128to255Octets.l[0];
    inFields[15] = portMacCounterSetArray.pkts128to255Octets.l[1];
    inFields[16] = portMacCounterSetArray.pkts256to511Octets.l[0];
    inFields[17] = portMacCounterSetArray.pkts256to511Octets.l[1];
    inFields[18] = portMacCounterSetArray.pkts512to1023Octets.l[0];
    inFields[19] = portMacCounterSetArray.pkts512to1023Octets.l[1];
    inFields[20] = portMacCounterSetArray.pkts1024tomaxOoctets.l[0];
    inFields[21] = portMacCounterSetArray.pkts1024tomaxOoctets.l[1];
    inFields[22] = portMacCounterSetArray.goodOctetsSent.l[0];
    inFields[23] = portMacCounterSetArray.goodOctetsSent.l[1];
    inFields[24] = portMacCounterSetArray.excessiveCollisions.l[0];
    inFields[25] = portMacCounterSetArray.excessiveCollisions.l[1];
    inFields[26] = portMacCounterSetArray.mcPktsSent.l[0];
    inFields[27] = portMacCounterSetArray.mcPktsSent.l[1];
    inFields[28] = portMacCounterSetArray.brdcPktsSent.l[0];
    inFields[29] = portMacCounterSetArray.brdcPktsSent.l[1];
    inFields[30] = portMacCounterSetArray.fcSent.l[0];
    inFields[31] = portMacCounterSetArray.fcSent.l[1];
    inFields[32] = portMacCounterSetArray.goodFcRcv.l[0];
    inFields[33] = portMacCounterSetArray.goodFcRcv.l[1];
    inFields[34] = portMacCounterSetArray.dropEvents.l[0];
    inFields[35] = portMacCounterSetArray.dropEvents.l[1];
    inFields[36] = portMacCounterSetArray.undersizePkts.l[0];
    inFields[37] = portMacCounterSetArray.undersizePkts.l[1];
    inFields[38] = portMacCounterSetArray.fragmentsPkts.l[0];
    inFields[39] = portMacCounterSetArray.fragmentsPkts.l[1];
    inFields[40] = portMacCounterSetArray.oversizePkts.l[0];
    inFields[41] = portMacCounterSetArray.oversizePkts.l[1];
    inFields[42] = portMacCounterSetArray.jabberPkts.l[0];
    inFields[43] = portMacCounterSetArray.jabberPkts.l[1];
    inFields[44] = portMacCounterSetArray.macRcvError.l[0];
    inFields[45] = portMacCounterSetArray.macRcvError.l[1];
    inFields[46] = portMacCounterSetArray.badCrc.l[0];
    inFields[47] = portMacCounterSetArray.badCrc.l[1];
    inFields[48] = portMacCounterSetArray.collisions.l[0];
    inFields[49] = portMacCounterSetArray.collisions.l[1];
    inFields[50] = portMacCounterSetArray.lateCollisions.l[0];
    inFields[51] = portMacCounterSetArray.lateCollisions.l[1];
    inFields[52] = portMacCounterSetArray.ucPktsRcv.l[0];
    inFields[53] = portMacCounterSetArray.ucPktsRcv.l[1];
    inFields[54] = portMacCounterSetArray.ucPktsSent.l[0];
    inFields[55] = portMacCounterSetArray.ucPktsSent.l[1];
    inFields[56] = portMacCounterSetArray.multiplePktsSent.l[0];
    inFields[57] = portMacCounterSetArray.multiplePktsSent.l[1];
    inFields[58] = portMacCounterSetArray.deferredPktsSent.l[0];
    inFields[59] = portMacCounterSetArray.deferredPktsSent.l[1];



    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],
                inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],  inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21],
                inFields[22], inFields[23], inFields[24], inFields[25],
                inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32], inFields[33],
                inFields[34], inFields[35], inFields[36], inFields[37],
                inFields[38], inFields[39], inFields[40], inFields[41],
                inFields[42], inFields[43], inFields[44], inFields[45],
                inFields[46], inFields[47], inFields[48], inFields[49],
                inFields[50], inFields[51], inFields[52], inFields[53],
                inFields[54], inFields[55], inFields[56], inFields[57],
                inFields[58], inFields[59]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersCaptureTriggerSet
*
* DESCRIPTION:
*     The function triggers a capture of MIB counters for specific port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number, whose counters are to be captured.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong port number or device
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_STATE - previous capture operation isn't finished.
*
*
* COMMENTS:
*        To get the counters, see cpssDxChPortMacCountersCaptureOnPortGet,
*                                 cpssDxChPortMacCounterCaptureGet.
*        For HyperG.Stack ports the feature operates per port, for other ports
*        it operates per group.
*        Ports 0-5 belong to group 0.
*        Ports 6-11 belong to group 1.
*        Ports 12-17 belong to group 2.
*        Ports 18-23 belong to group 3.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacCountersCaptureTriggerSet
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


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacCountersCaptureTriggerSet(devNum, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacCountersCaptureTriggerGet
*
* DESCRIPTION:
*    The function gets status of a capture of MIB counters for specific port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number.
*
* OUTPUTS:
*       captureIsDonePtr     - pointer to status of Capture counter Trigger
*                              - GT_TRUE  - capture is done.
*                              - GT_FALSE - capture action is in proccess.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong device number
*       GT_HW_ERROR  - on hardware error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*
* COMMENTS:
*        To get the counters, see cpssDxChPortMacCountersCaptureOnPortGet,
*                                 cpssDxChPortMacCounterCaptureGet.
*        For HyperG.Stack ports the feature operates per port, for other ports
*        it operates per group.
*        Ports 0-5 belong to group 0.
*        Ports 6-11 belong to group 1.
*        Ports 12-17 belong to group 2.
*        Ports 18-23 belong to group 3.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacCountersCaptureTriggerGet
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
    GT_BOOL captureIsDone;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacCountersCaptureTriggerGet(devNum, portNum, &captureIsDone);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", captureIsDone);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortInterfaceModeSet
*
* DESCRIPTION:
*       Sets Interface mode on a specified port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number (or CPU port)
*       ifMode    - Interface mode.For CPU port the interface may be one of
*                   GMII, RGMII, MII and MII PHY. For Tri-Speed ports
*                   the interface may be SGMII or 1000 Base X.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortInterfaceModeSet
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
    CPSS_PORT_INTERFACE_MODE_ENT ifMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ifMode = (CPSS_PORT_INTERFACE_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortInterfaceModeSet(devNum, portNum, ifMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortSerdesComboModeSet
*
* DESCRIPTION:
*       Set Serdes mode. If Serdes mode is CPSS_DXCH_PORT_SERDES_MODE_COMBO_E,
*       Combo Serdes are active for the port, else regular Serdes are active for
*       the port.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number, values 20..23.
*       mode     - serdes mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum, mode
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSerdesComboModeSet
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
    CPSS_DXCH_PORT_SERDES_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    mode = (CPSS_DXCH_PORT_SERDES_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesComboModeSet(devNum, portNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortSerdesComboModeGet
*
* DESCRIPTION:
*       Get Serdes mode. If Serdes mode is CPSS_DXCH_PORT_SERDES_MODE_COMBO_E,
*       Combo Serdes are active for the port, else regular Serdes are active for
*       the port.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number, values 20..23.
*
* OUTPUTS:
*       modePtr   -  pointer to Serdes mode.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSerdesComboModeGet
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
    CPSS_DXCH_PORT_SERDES_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesComboModeGet(devNum, portNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPaddingEnableSet
*
* DESCRIPTION:
*       Enable/Disable padding of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - Pad short packet in Tx.
*                - GT_FALSE - No padding in short packets.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPaddingEnableSet
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
    GT_BOOL enable;


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
    result = cpssDxChPortPaddingEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPaddingEnableGet
*
* DESCRIPTION:
*       Gets padding status of transmitted packets shorter than 64B.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr   - pointer to packet padding status.
*                     - GT_TRUE  - Pad short packet in Tx.
*                     - GT_FALSE - No padding in short packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPaddingEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPaddingEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortExcessiveCollisionDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number (or CPU port).
*       enable   - GT_TRUE  - if the number of collisions on the same packet
*                             is 16 the packet is dropped.
*                - GT_FALSE - A collided packet will be retransmitted by device
*                             until it is transmitted without collisions,
*                             regardless of the number of collisions on the packet.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       The setting is not relevant in full duplex mode
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortExcessiveCollisionDropEnableSet
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
    GT_BOOL enable;


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
    result = cpssDxChPortExcessiveCollisionDropEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortExcessiveCollisionDropEnableGet
*
* DESCRIPTION:
*       Gets status of excessive collision packets drop.
*       In half duplex mode if a collision occurs the device tries to transmit
*       the packet again. If the number of collisions on the same packet is 16
*       and  excessive collision packets drop is enabled,
*       the packet is dropped.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev       - device number
*       portNum   - physical port number (or CPU port).
*
* OUTPUTS:
*       enablePtr - pointer to status of excessive collision packets drop.
*                   - GT_TRUE  - if the number of collisions on the same packet
*                               is 16 the packet is dropped.
*                   - GT_FALSE - A collided packet will be retransmitted by
*                                device until it is transmitted
*                                without collisions, regardless of the number
*                                of collisions on the packet.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Not relevant in full duplex mode
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortExcessiveCollisionDropEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortExcessiveCollisionDropEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortXgLanesSwapEnableSet
*
* DESCRIPTION:
*       Enable/Disable swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*
*                - GT_FALSE - Normal operation (no swapping)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXgLanesSwapEnableSet
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
    GT_BOOL enable;


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
    result = cpssDxChPortXgLanesSwapEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortXgLanesSwapEnableGet
*
* DESCRIPTION:
*       Gets status of swapping XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number.
*
*
* OUTPUTS:
*       enablePtr - pointer to status of swapping XAUI PHY SERDES Lanes.
*                    - GT_TRUE  - Swap Lanes by follows:
*                             The lanes are swapped by follows:
*                             Lane 0 PSC Tx data is connected to SERDES Lane 3
*                             Lane 1 PSC Tx data is connected to SERDES Lane 2
*                             Lane 2 PSC Tx data is connected to SERDES Lane 1
*                             Lane 3 PSC Tx data is connected to SERDES Lane 0
*                    - GT_FALSE - Normal operation (no swapping)
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXgLanesSwapEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortXgLanesSwapEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortXgPscLanesSwapSet
*
* DESCRIPTION:
*       Set swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES: DxCh3 and above
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*       rxSerdesLaneArr - array for binding port Rx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*       txSerdesLaneArr - array for binding port Tx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_OUT_OF_RANGE - wrong SERDES lane
*       GT_BAD_VALUE    - multiple connection detected
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXgPscLanesSwapSet
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
    GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];
    GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    rxSerdesLaneArr[0] = (GT_U32)inArgs[2];
    rxSerdesLaneArr[1] = (GT_U32)inArgs[3];
    rxSerdesLaneArr[2] = (GT_U32)inArgs[4];
    rxSerdesLaneArr[3] = (GT_U32)inArgs[5];
    txSerdesLaneArr[0] = (GT_U32)inArgs[6];
    txSerdesLaneArr[1] = (GT_U32)inArgs[7];
    txSerdesLaneArr[2] = (GT_U32)inArgs[8];
    txSerdesLaneArr[3] = (GT_U32)inArgs[9];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortXgPscLanesSwapSet(devNum, portNum,
                                           rxSerdesLaneArr, txSerdesLaneArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortXgPscLanesSwapGet
*
* DESCRIPTION:
*       Get swapping configuration of XAUI or HyperG.Stack port SERDES Lanes.
*
* APPLICABLE DEVICES: DxCh3 and above
*
* INPUTS:
*       devNum   - device number.
*       portNum  - physical port number.
*
* OUTPUTS:
*       rxSerdesLaneArr - array for binding port Rx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*       txSerdesLaneArr - array for binding port Tx PSC and SERDES lanes:
*                         array index is PSC lane, array value is SERDES lane.
*
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_HW_ERROR     - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported for XAUI or HyperG.Stack ports only.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXgPscLanesSwapGet
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
    GT_U32 rxSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];
    GT_U32 txSerdesLaneArr[CPSS_DXCH_PORT_XG_PSC_LANES_NUM_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortXgPscLanesSwapGet(devNum, portNum,
                                           rxSerdesLaneArr, txSerdesLaneArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d",
                 rxSerdesLaneArr[0],
                 rxSerdesLaneArr[1],
                 rxSerdesLaneArr[2],
                 rxSerdesLaneArr[3],
                 txSerdesLaneArr[0],
                 txSerdesLaneArr[1],
                 txSerdesLaneArr[2],
                 txSerdesLaneArr[3]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortInBandAutoNegBypassEnableSet
*
* DESCRIPTION:
*       Enable/Disable Auto-Negotiation by pass.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number.
*       enable   - GT_TRUE  - Auto-Negotiation can't be bypassed.
*                - GT_FALSE - Auto-Negotiation is bypassed.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       Supported for ports in 1000Base-X mode only.
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssDxChPortInbandAutoNegEnableSet.)
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortInBandAutoNegBypassEnableSet
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
    GT_BOOL enable;


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
    result = cpssDxChPortInBandAutoNegBypassEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortInBandAutoNegBypassEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation by pass status.
*       If the link partner doesn't respond to Auto-Negotiation process,
*       the link is established by bypassing the Auto-Negotiation procedure.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev      - device number
*       portNum  - physical port number.
*
* OUTPUTS:
*       enablePtr - pointer to Auto-Negotiation by pass status.
*                   - GT_TRUE  - Auto-Negotiation can't be bypassed.
*                   - GT_FALSE - Auto-Negotiation is bypassed.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       Supported for ports in 1000Base-X mode only.
*       Relevant when Inband Auto-Negotiation is enabled.
*       (See cpssDxChPortInbandAutoNegEnableSet.)
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortInBandAutoNegBypassEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortInBandAutoNegBypassEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacErrorIndicationPortSet
*
* DESCRIPTION:
*       Sets port monitored for MAC errors.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev      - device number
*       portNum  - port monitored for MAC errors.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*
* COMMENTS:
*       MAC source addresses and the error type can be read
*       for the port. See cpssDxChPortMacErrorIndicationGet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacErrorIndicationPortSet
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


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacErrorIndicationPortSet(devNum, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacErrorIndicationPortGet
*
* DESCRIPTION:
*       Gets port monitored for MAC errors.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev      - device number
*
* OUTPUTS:
*       portNumPtr  - pointer to port monitored for MAC errors.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*       MAC source addresses and the error type can be read
*       for the port. See cpssDxChPortMacErrorIndicationGet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacErrorIndicationPortGet
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


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortMacErrorIndicationPortGet(devNum, &portNum);
    CONVERT_BACK_DEV_PORT_DATA_MAC(devNum,portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", portNum);

    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortMacErrorIndicationGet
*
* DESCRIPTION:
*       Gets MAC source addresses and the error type for the monitored port.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev      - device number
*
* OUTPUTS:
*       typePtr     - pointer to MAC error type.
*       macPtr      - pointer to MAC SA of the last packet received
*                     with  a MAC error on MAC error indication port.
*
* RETURNS:
*       GT_OK           - on success
*       GT_BAD_PARAM    - wrong devNum, portNum
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*
* COMMENTS:
*      Port monitored for MAC errors should be set.
*      See cpssDxChPortMacErrorIndicationPortSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacErrorIndicationGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_MAC_ERROR_TYPE_ENT type;
    GT_ETHERADDR mac;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];


    /* call cpss api function */
    result = cpssDxChPortMacErrorIndicationGet(devNum, &type, &mac);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result,"%d%6b", type,mac.arEther);

    return CMD_OK;
}
/*******************************************************************************
* wrCpssDxChPortAttributesOnPortGet
*
* DESCRIPTION:
*       wrapper for port statistics
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - port number
*
* OUTPUTS:
*       portAttributSetArrayPtr - Pointer to attributes values array.
*
* RETURNS:
*       GT_OK           - on success
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U8 devNum;
static GT_U8 portNum;

 static GT_STATUS portDxChAttributesGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result1, result2, result3,result4,result5,result6,result7;
    GT_BOOL flag1,flag2;

    OUT CPSS_PORT_ATTRIBUTES_STC        portAttributSetArray;
    OUT CPSS_PORT_INTERFACE_MODE_ENT    ifMode;
    OUT CPSS_PORT_FLOW_CONTROL_ENT      flowCtrlEnable;
    OUT GT_BOOL                         speedAutoNegEnable;
    OUT GT_BOOL                         duplexAutoNegEnable;

    OUT GT_BOOL                         flowCtrlAutoNegEnable;
    OUT GT_BOOL                         pauseAdvertise;
    OUT GT_U32                          mruSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return GT_INIT_ERROR;


    /* call cpss api function */
    result1 = cpssDxChPortAttributesOnPortGet(devNum, portNum,
                                    &portAttributSetArray);
    result2 = cpssDxChPortInterfaceModeGet(devNum,portNum,&ifMode);
    result3 = cpssDxChPortFlowControlEnableGet(devNum,portNum,&flowCtrlEnable);
    if (portNum != CPSS_CPU_PORT_NUM_CNS)
    {
        result4 = cpssDxChPortSpeedAutoNegEnableGet(devNum,portNum,&speedAutoNegEnable);

        result5 = cpssDxChPortDuplexAutoNegEnableGet(devNum,portNum,&duplexAutoNegEnable);

        result6 = cpssDxChPortFlowCntrlAutoNegEnableGet(devNum,portNum,
                                                            &flowCtrlAutoNegEnable,
                                                            &pauseAdvertise);
    }
    else
    {
        speedAutoNegEnable = GT_FALSE;
        result4 = GT_OK;

        duplexAutoNegEnable = GT_FALSE;
        result5 = GT_OK;

        flowCtrlAutoNegEnable = GT_FALSE;
        pauseAdvertise = GT_FALSE;
        result6 = GT_OK;
    }

    result7 = cpssDxChPortMruGet(devNum,portNum,&mruSize);

    if(result1 == GT_NOT_APPLICABLE_DEVICE)
    {
        galtisOutput(outArgs, result1, "%d", -1);
        return GT_NOT_APPLICABLE_DEVICE;
    }

    flag1=(result1 != GT_OK)||(result2 != GT_OK)||(result3 != GT_OK)||(result7 != GT_OK);
    flag2=(result4 != GT_OK)||(result5 != GT_OK)||(result6 != GT_OK);

    if (flag1 || (flag2 && portNum != CPSS_CPU_PORT_NUM_CNS))
    {
        galtisOutput(outArgs, result1, "%d", -1);
        return GT_OK;
    }

    inFields[0] = devNum;
    inFields[1] = portNum;
    inFields[2] = portAttributSetArray.portLinkUp;
    inFields[3] = portAttributSetArray.portSpeed;
    inFields[4] = portAttributSetArray.portDuplexity;
    inFields[5] = ifMode;
    inFields[6] = flowCtrlEnable;
    inFields[7] = speedAutoNegEnable;
    inFields[8] = duplexAutoNegEnable;
    inFields[9] = flowCtrlAutoNegEnable;
    inFields[10] = pauseAdvertise;
    inFields[11] = mruSize;



    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d",  inFields[0],  inFields[1],  inFields[2], inFields[3], inFields[4],
                               inFields[5],  inFields[6],inFields[7],  inFields[8],inFields[9],  inFields[10],
                               inFields[11]);


    galtisOutput(outArgs, result1, "%f");

    return GT_OK;
}
/*****************************************************************************************/
 static CMD_STATUS wrCpssDxChPortAttributesGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result=GT_OK;
    devNum = 0;
    portNum = 0;

    while(devNum < 64)
    {
        if(0 == PRV_CPSS_IS_DEV_EXISTS_MAC(devNum))
        {
            devNum++;
            continue;
        }

        result = portDxChAttributesGet(inArgs,inFields,numFields,outArgs);
        if(result == GT_NOT_APPLICABLE_DEVICE)
        {
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
            portNum++;
        }
        else break;
    }
    if (devNum == 64)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}
/*******************************************************************************/
 static CMD_STATUS wrCpssDxChPortAttributesGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result = GT_OK;

    while(1)
    {
        if(devNum == PRV_CPSS_MAX_PP_DEVICES_CNS)
        {
            /* no ports */
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }

        if((!PRV_CPSS_IS_DEV_EXISTS_MAC(devNum)) ||
           (portNum == CPSS_MAX_PORTS_NUM_CNS))
        {
            /* move to the next PP */
            devNum++;
            portNum = 0;
            continue;
        }
        else
        {
            if (PRV_CPSS_PHY_PORT_IS_EXIST_MAC(devNum, portNum) ||
                 (portNum == CPSS_CPU_PORT_NUM_CNS))
            {
                /* get port's attributes */
                result = portDxChAttributesGet(inArgs,inFields,numFields,outArgs);
                break;
            }
            else
            {
                portNum++;
            }
        }
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* move to the next portNum */
    portNum++;

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPortSerdesConfigSet
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
    CPSS_DXCH_PORT_SERDES_CONFIG_STC serdesConf;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    serdesConf.txAmp = (GT_U32)inArgs[2];
    serdesConf.txEmphEn = (GT_BOOL)inArgs[3];
    serdesConf.txEmphAmp = (GT_U32)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesConfigSet(devNum, portNum, &serdesConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

static CMD_STATUS wrCpssDxChPortSerdesConfigGet
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
    CPSS_DXCH_PORT_SERDES_CONFIG_STC serdesConf;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesConfigGet(devNum, portNum, &serdesConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d %d %d", serdesConf.txAmp,
                 serdesConf.txEmphAmp, serdesConf.txEmphEn);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxRandomTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Random Tail drop Threshold, to overcome synchronization.
*
* APPLICABLE DEVICES:  DxCh3, DxChXcat
*
* INPUTS:
*       devNum   - device number.
*       enable   -  GT_TRUE  -  Enable Random Tail drop Threshold.
*                   GT_FALSE -  Disable Random Tail drop Threshold.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxRandomTailDropEnableSet
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
    result = cpssDxChPortTxRandomTailDropEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxRandomTailDropEnableGet
*
* DESCRIPTION:
*       Get Random Tail drop Threshold status.
*
* APPLICABLE DEVICES:  DxCh3, DxChXcat
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to Random Tail drop Threshold status:
*                       GT_TRUE  - Random Tail drop Threshold enabled.
*                       GT_FALSE - Random Tail drop Threshold disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxRandomTailDropEnableGet
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
    result = cpssDxChPortTxRandomTailDropEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTxPortAllShapersDisable
*
* DESCRIPTION:
*       Disables all ports and queues shapers for specified device in minimum delay.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on wrong device
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChTxPortAllShapersDisable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChTxPortAllShapersDisable(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChTxPortShapersDisable
*
* DESCRIPTION:
*       Disable Shaping on Port and all it's queues.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum  - physical device number
*       port    - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_BAD_PARAM - on wrong device or port number
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssDxChTxPortShapersDisable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      port;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    CONVERT_DEV_PORT_MAC(devNum, port);
    /* call cpss api function */
    result = cpssDxChTxPortShapersDisable(devNum, port);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/****************************************************************************
* cpssDxChPortIpgSet
*
* DESCRIPTION:
*       Sets the Inter-Packet Gap (IPG) interval of a tri-speed physical port.
*       Using this API may be required to enable wire-speed in traffic paths
*       that include cascading ports, where it may not be feasible to reduce
*       the preamble length.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*       ipg         - IPG in bytes, acceptable range:
*                     [0..511] for DxCh3 and above devices.
*                     [0..15] for DxCh1 and DxCh2 devices.
*                     Default HW value is 12 bytes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on wrong port type
*       GT_OUT_OF_RANGE  - ipg value out of range
*       GT_BAD_PARAM     - on bad parameter
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortIpgSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    IN  GT_U8   devNum;
    IN  GT_U8   portNum;
    IN  GT_U32  ipg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ipg     = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortIpgSet(devNum, portNum, ipg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/****************************************************************************
* cpssDxChPortIpgGet
*
* DESCRIPTION:
*       Gets the Inter-Packet Gap (IPG) interval of a tri-speed physical port.
*
* APPLICABLE DEVICES:  All DxCh devices
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical or CPU port number
*
* OUTPUTS:
*       ipgPtr      - (pointer to) IPG value in bytes
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on wrong port type
*       GT_BAD_PARAM     - on bad parameter
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortIpgGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    IN  GT_U8   devNum;
    IN  GT_U8   portNum;
    OUT GT_U32  ipg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override device and port */
    CONVERT_DEV_PORT_MAC(devNum, portNum)

    /* call cpss api function */
    result = cpssDxChPortIpgGet(devNum, portNum, &ipg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ipg);

    return CMD_OK;
}



/******************************************************************************
* cpssDxChPortSerdesPowerStatusSet
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes according to port capabilities.
*       XG / XAUI ports: All 4 (TX or RX) lanes are set simultanuously.
*       HX / QX ports: lanes 0,1 or TX and RX may be set separately.
*
* APPLICABLE DEVICES: All DxCh devices
*
* INPUTS:
*       dev       - physical device number
*       port      - physical port number
*       direction - may be either CPSS_PORT_DIRECTION_RX_E,
*                                 CPSS_PORT_DIRECTION_TX_E, or
*                                 CPSS_PORT_DIRECTION_BOTH_E.
*       lanesBmp  - bitmap of SERDES lanes (bit 0-> lane 0, etc.)
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on bad parameters
*     GT_FAIL          - on error
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     DxCh3 and above devices supports only GT_PORT_DIRECTION_BOTH_RX_TX.
*
******************************************************************************/
static CMD_STATUS wrCpssDxChPortSerdesPowerStatusSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8                   devNum;
    GT_U8                   portNum;
    CPSS_PORT_DIRECTION_ENT direction;
    GT_U32                  lanesBmp;
    GT_BOOL                 powerUp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    direction   = (CPSS_PORT_DIRECTION_ENT)inArgs[2];
    lanesBmp = (GT_U32)inArgs[3];
    powerUp = (GT_U8)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesPowerStatusSet(devNum, portNum, direction,
                                              lanesBmp, powerUp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;


}



/*******************************************************************************
* cpssDxChPortSyncEtherSecondRecoveryClkEnableSet
*
* DESCRIPTION:
*       Function enables/disables second recovery clock output pin.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum  - device number.
*       enable  - enable/disable second clock output pin
*                   GT_TRUE  - enable
*                   GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Function changes two fields: <2nd_rcvr_clk_out_sel>
*          and <ref_clk_125_sel>.
*       2. Function performs all SERDES power down and restore
*          original SERDES power state.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherSecondRecoveryClkEnableSet
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
    devNum  = (GT_U8)inArgs[0];
    enable  = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortSyncEtherSecondRecoveryClkEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortSyncEtherSecondRecoveryClkEnableGet
*
* DESCRIPTION:
*       Function gets status (enabled/disabled) of
*       second recovery clock output pin.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       enablePtr - (pointer to) second clock output pin state
*                    GT_TRUE  - enabled
*                    GT_FALSE - disabled
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - on bad state
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherSecondRecoveryClkEnableGet
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
    devNum  = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortSyncEtherSecondRecoveryClkEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortSyncEtherPllRefClkSelectSet
*
* DESCRIPTION:
*       Function sets PLL reference clock select:
*       oscillator clock or recovered clock.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number. Valid ports for configuration:
*                       GiG Ports: 0,4,8,12,16,20
*                       XG  Ports: 24,25,26,27
*       pllClkSelect - clock source: recovered or oscillator
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum or pllClkSelect
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Function performs SERDES power down and restore
*       original SERDES power state.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherPllRefClkSelectSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT pllClkSelect;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    pllClkSelect = (CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSyncEtherPllRefClkSelectSet(devNum, portNum, pllClkSelect);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortSyncEtherPllRefClkSelectGet
*
* DESCRIPTION:
*       Function gets PLL reference clock select:
*       oscillator clock or recovered clock.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum    - device number.
*       portNum   - port number. Valid ports for configuration:
*                   GiG Ports: 0,4,8,12,16,20
*                   XG  Ports: 24,25,26,27
*
* OUTPUTS:
*       pllClkSelectPtr - (pointer to) clock source: recovered or oscillator
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherPllRefClkSelectGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    CPSS_DXCH_PORT_SYNC_ETHER_PLL_REF_CLK_TYPE_ENT pllClkSelect;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSyncEtherPllRefClkSelectGet(devNum, portNum, &pllClkSelect);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", pllClkSelect);

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortSyncEtherRefClkConfigSet
*
* DESCRIPTION:
*       Function configures the reference clock (0 or 1) enable/disable
*       state and sets its source portNum.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*       enable          - enable/disable recovered clock1/clock2
*                          GT_TRUE - enable
*                          GT_FALSE - disable
*       portNum         - port number. Valid ports for configuration:
*                          GiG Ports: 0,4,8,12,16,20
*                          XG  Ports: 24,25,26,27
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType or portNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherRefClkConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType;
    GT_BOOL     enable;
    GT_U8       portNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    recoveryClkType = (CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT)inArgs[1];
    enable  = (GT_BOOL)inArgs[2];
    portNum = (GT_U8)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSyncEtherRecoveryClkConfigSet(devNum, recoveryClkType, enable, portNum, 0);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortSyncEtherRefClkConfigGet
*
* DESCRIPTION:
*       Function gets the reference clock (0 or 1) enable/disable
*       state and sets its source portNum.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                           GT_TRUE - reference port
*                           GT_FALSE - regular port
*       portNumPtr      - (pointer to) port number. Valid ports for configuration:
*                           GiG Ports: 0,4,8,12,16,20
*                           XG  Ports: 24,25,26,27
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherRefClkConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType;
    GT_BOOL     enable;
    GT_U8       portNum;
    GT_U32      laneNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    recoveryClkType = (CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortSyncEtherRecoveryClkConfigGet(devNum, recoveryClkType, &enable, &portNum, &laneNum);
    CONVERT_BACK_DEV_PORT_DATA_MAC(devNum,portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, portNum);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkConfigSet
*
* DESCRIPTION:
*       Function configures the recovery clock enable/disable state and sets
*       its source portNum.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type.
*       enable          - enable/disable recovered clock1/clock2
*                         GT_TRUE - enable
*                         GT_FALSE - disable
*       portNum         - port number.
*                         For DxChXcat valid ports for configuration:
*                         Giga Ports: 0,4,8,12,16,20
*                         XG Ports: 24,25,26,27
*       laneNum         - selected SERDES lane numberwithin port.
*                         relevant only for Lion.
*                         Ranges according to port interface:
*                         RXAUI - 0..1
*                         XAUII - 0..3
*                         XLG   - 0..7
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType, portNum
*                                  or laneNum.
*       GT_BAD_STATE             - another SERDES already enabled
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherRefClkConfigSet1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType;
    GT_BOOL     enable;
    GT_U8       portNum;
    GT_U32      laneNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    recoveryClkType = (CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT)inArgs[1];
    enable  = (GT_BOOL)inArgs[2];
    portNum = (GT_U8)inArgs[3];
    laneNum = (GT_U32)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSyncEtherRecoveryClkConfigSet(devNum, recoveryClkType, enable, portNum, laneNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkConfigGet
*
* DESCRIPTION:
*       Function gets the recovery clock enable/disable state and its source
*       portNum.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum          - device number.
*       recoveryClkType - recovered clock output type
*
* OUTPUTS:
*       enablePtr       - (pointer to) port state as reference.
*                         GT_TRUE -  enabled
*                         GT_FALSE - disbled
*       portNumPtr      - (pointer to) port number.
*       laneNumPtr      - (pointer to) selected SERDES lane number within port.
*                         relevant only for Lion.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, recoveryClkType
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_STATE             - on bad port interface state
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherRefClkConfigGet1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT  recoveryClkType;
    GT_BOOL     enable;
    GT_U8       portNum;
    GT_U32      laneNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    recoveryClkType = (CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortSyncEtherRecoveryClkConfigGet(devNum, recoveryClkType, &enable, &portNum, &laneNum);
    CONVERT_BACK_DEV_PORT_DATA_MAC(devNum,portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", enable, portNum, laneNum);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
*
* DESCRIPTION:
*       Function sets recovery clock divider bypass enable.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum      - device number.
*       enable      - enable/disable recovery clock divider bypass enable
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum, enable
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet
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
    devNum  = (GT_U8)inArgs[0];
    enable  = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet
*
* DESCRIPTION:
*       Function gets recovery clock divider bypass status.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   - (pointer to) recovery clock divider bypass state
*                       GT_TRUE - enable
*                       GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet
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
    devNum  = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerValueSet
*
* DESCRIPTION:
*       Function sets recovery clock divider value.
*
* APPLICABLE DEVICES:  Lion
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       laneNum     - selected SERDES lane number within port.
*                     Ranges according to port interface:
*                     RXAUI - 0..1
*                     XAUII - 0..3
*                     XLG   - 0..7
*       value       - recovery clock divider value to set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherRecoveryClkDividerValueSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    GT_U32      laneNum;
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT  value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    laneNum = (GT_U32)inArgs[2];
    value = (CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSyncEtherRecoveryClkDividerValueSet(devNum, portNum, laneNum, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortSyncEtherRecoveryClkDividerValueGet
*
* DESCRIPTION:
*       Function gets recovery clock divider value.
*
* APPLICABLE DEVICES:  Lion
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       laneNum     - selected SERDES lane number within port.
*                     Ranges according to port interface:
*                     RXAUI - 0..1
*                     XAUII - 0..3
*                     XLG   - 0..7
*
* OUTPUTS:
*       valuePtr   - (pointer to) recovery clock divider value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherRecoveryClkDividerValueGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    GT_U32      laneNum;
    CPSS_DXCH_PORT_SYNC_ETHER_RECOVERY_CLK_DIVIDER_ENT  value;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    laneNum = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSyncEtherRecoveryClkDividerValueGet(devNum, portNum, laneNum, &value);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", value);

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet
*
* DESCRIPTION:
*       Function sets Recovered Clock Automatic Masking enabling.
*
* APPLICABLE DEVICES:  Lion
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       laneNum     - selected SERDES lane number within port.
*                     Ranges according to port interface:
*                     RXAUI - 0..1
*                     XAUII - 0..3
*                     XLG   - 0..7
*       enable      - enable/disable Recovered Clock Automatic Masking
*                     GT_TRUE - enable
*                     GT_FALSE - disable
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    GT_U32      laneNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    laneNum = (GT_U32)inArgs[2];
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet(devNum, portNum, laneNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet
*
* DESCRIPTION:
*       Function gets Recovered Clock Automatic Masking status.
*
* APPLICABLE DEVICES:  Lion
*
* INPUTS:
*       devNum      - device number.
*       portNum     - port number.
*       laneNum     - selected SERDES lane number within port.
*                     Ranges according to port interface:
*                     RXAUI - 0..1
*                     XAUII - 0..3
*                     XLG   - 0..7
*
* OUTPUTS:
*       enablePtr   - (pointer to) Recovered Clock Automatic Masking state
*                     GT_TRUE - enable
*                     GT_FALSE - disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong devNum or lane number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U8       portNum;
    GT_U32      laneNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    laneNum = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet(devNum, portNum, laneNum, &enable);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortCnModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable Congestion Notification (CN) mechanism.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable congestion notification mechanism.
*                    GT_FALSE: Disable congestion notification mechanism.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnModeEnableSet
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
    result = cpssDxChPortCnModeEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnModeEnableGet
*
* DESCRIPTION:
*       Gets the current status of Congestion Notification (CN) mode.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of Congestion Notification mode
*                     GT_TRUE:  Congestion Notification mode enabled.
*                     GT_FALSE: Congestion Notification mode disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnModeEnableGet
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
    result = cpssDxChPortCnModeEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileSet
*
* DESCRIPTION:
*       Binds a port to a Congestion Notification profile.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnProfileSet
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
    GT_U32 profileIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    profileIndex = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortCnProfileSet(devNum, portNum, profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileGet
*
* DESCRIPTION:
*       Gets the port's Congestion Notification profile.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnProfileGet
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
    GT_U32 profileIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortCnProfileGet(devNum, portNum, &profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileIndex);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueThresholdSet
*
* DESCRIPTION:
*       Sets the buffer threshold for triggering CN frame for a given tc queue.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*       threshold    - buffer limit threshold for triggering CN frame on a given
*                      tc queue (0..0x1FFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnProfileQueueThresholdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    GT_U8 tcQueue;
    GT_U32 threshold;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];
    threshold = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortCnProfileQueueThresholdSet(devNum, profileIndex, tcQueue, threshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueThresholdGet
*
* DESCRIPTION:
*       Gets the buffer threshold for triggering CN frame for a given tc queue.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       thresholdPtr - (pointer to) buffer limit threshold for
*                      triggering CN frame on a given tc queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnProfileQueueThresholdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    GT_U8 tcQueue;
    GT_U32 threshold;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortCnProfileQueueThresholdGet(devNum, profileIndex, tcQueue, &threshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", threshold);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileThresholdSet
*
* DESCRIPTION:
*       Sets the buffer threshold for triggering CN frame.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       threshold    - buffer limit threshold
*                      for triggering CN frame (0..0x1FFF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile index
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnProfileThresholdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    GT_U32 threshold;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];
    threshold = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortCnProfileThresholdSet(devNum, profileIndex, threshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileThresholdGet
*
* DESCRIPTION:
*       Gets the buffer threshold for triggering CN frame.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*
* OUTPUTS:
*       thresholdPtr - (pointer to) buffer limit threshold for
*                     triggering CN frame.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number or profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnProfileThresholdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    GT_U32 threshold;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortCnProfileThresholdGet(devNum, profileIndex, &threshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", threshold);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnQueueAwareEnableSet
*
* DESCRIPTION:
*       Enable/Disable Congestion Notification awareness on a given
*       tc queue.
*
* APPLICABLE DEVICES:  DxChXcat only
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
*       enable     - GT_TRUE: CN aware. Packets enqueued to tcQueue are enabled to
*                    trigger Congestion Notification frames.
*                    GT_FALSE: CN blind. Packets enqueued to tcQueue never trigger
*                    Congestion Notification frames.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnQueueAwareEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 tcQueue;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortCnQueueAwareEnableSet(devNum, tcQueue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnQueueAwareEnableGet
*
* DESCRIPTION:
*       Gets the status of Congestion Notification awareness on a given
*       tc queue.
*
* APPLICABLE DEVICES:  DxChXcat only
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue (0..7).
* OUTPUTS:
*       enablePtr  - (pointer to) status of Congestion Notification awareness
*                    GT_TRUE: CN aware. Packets enqueued to tcQueue are enabled to
*                    trigger Congestion Notification frames.
*                    GT_FALSE: CN blind. Packets enqueued to tcQueue never trigger
*                    Congestion Notification frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnQueueAwareEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 tcQueue;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortCnQueueAwareEnableGet(devNum, tcQueue, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnFrameQueueSet
*
* DESCRIPTION:
*       Set traffic class queue associated with CN frames generated by device.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*       tcQueue    - traffic class queue associated with CN frames (0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       CN frames must be enqueued to priority queues disabled to trigger CN.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnFrameQueueSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 tcQueue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortCnFrameQueueSet(devNum, tcQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnFrameQueueGet
*
* DESCRIPTION:
*       Get traffic class queue associated with CN frames generated by device.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       tcQueuePtr - (pointer to) traffic class queue associated with CN frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnFrameQueueGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 tcQueue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortCnFrameQueueGet(devNum, &tcQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tcQueue);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnEtherTypeSet
*
* DESCRIPTION:
*       Sets the EtherType to identify CN frames.
*       This EtherType also inserted in the CN header of generated CN frames.
*
* APPLICABLE DEVICES:  DxChXcat and above.
*
* INPUTS:
*       devNum      - device number.
*       etherType   - EtherType for CN frames (0..0xFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range etherType values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*       The CN EtherType must be unique in the system.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnEtherTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 etherType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    etherType = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortCnEtherTypeSet(devNum, etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnEtherTypeGet
*
* DESCRIPTION:
*       Gets the EtherType for CN frames. The EtherType inserted in the CN
*       header.
*
* APPLICABLE DEVICES:  DxChXcat and above.
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       etherTypePtr - (pointer to) EtherType for CN frames.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
*
* COMMENTS:
*         None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnEtherTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 etherType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortCnEtherTypeGet(devNum, &etherType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", etherType);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnSpeedIndexSet
*
* DESCRIPTION:
*       Sets port speed index of the egress port. The device inserts this index
*       in the generated CCFC frame.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       portSpeedIndex - port speed index (0..7).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range port speed index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Congested ports should use Port Speed Index 1 to 6 if
*          Panic Pause is used.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnSpeedIndexSet
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
    GT_U32 portSpeedIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    portSpeedIndex = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortCnSpeedIndexSet(devNum, portNum, portSpeedIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortSpeedIndexGet
*
* DESCRIPTION:
*       Gets port speed index of the egress port. The device inserts this index
*       in the generated CCFC frame.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum            - device number.
*       portNum    -      - port number.
*
* OUTPUTS:
*       portSpeedIndexPtr - (pointer to) port speed index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnSpeedIndexGet
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
    GT_U32 portSpeedIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortCnSpeedIndexGet(devNum, portNum, &portSpeedIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", portSpeedIndex);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnPauseTriggerEnableSet
*
* DESCRIPTION:
*       Enable the CN triggering engine to trigger a pause frame.
*
* APPLICABLE DEVICES:  DxChXcat
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE:  enable.
*                    GT_FALSE: disable.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        To enable proper operation of CCFC, this configuration must be
*        enabled for all network ports and disabled for all cascade ports.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnPauseTriggerEnableSet
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
    GT_BOOL enable;


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
    result = cpssDxChPortCnPauseTriggerEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnPauseTriggerEnableGet
*
* DESCRIPTION:
*       Gets the status triggering engine a pause frame on a given port.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE:  enable.
*                    GT_FALSE: disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnPauseTriggerEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortCnPauseTriggerEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnFcEnableSet
*
* DESCRIPTION:
*       Enable/Disable generation of flow control packets as result of CCFC
*       frame termination.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE:  enable. Flow control packets can be issued.
*                    GT_FALSE: disable. Flow control packets can't be issued.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnFcEnableSet
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
    GT_BOOL enable;


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
    result = cpssDxChPortCnFcEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnFcEnableGet
*
* DESCRIPTION:
*       Gets the status of generation of flow control packets as result of CCFC
*       frame termination on a given port.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE:  enable. Flow control packets is issued.
*                    GT_FALSE: disable. Flow control packets is not issued.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnFcEnableGet
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
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortCnFcEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnFcTimerSet
*
* DESCRIPTION:
*       Sets timer value for the IEEE 802.3x pause frame issued as result
*       of CCFC frame termination.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       portSpeedIndex - congested port speed index (0..7).
*       timer          - 802.3x pause time (0..0xFFFF).
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number, device or speed index
*       GT_OUT_OF_RANGE          - on out of range pause time value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Port timer configurations 0 and 7 must be configured
*          to 0x0 and 0xFFFF if Panic Pause is used.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnFcTimerSet
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
    GT_U32 portSpeedIndex;
    GT_U32 timer;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    portSpeedIndex = (GT_U32)inArgs[2];
    timer = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortCnFcTimerSet(devNum, portNum, portSpeedIndex, timer);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnFcTimerGet
*
* DESCRIPTION:
*       Gets timer value for the IEEE 802.3x pause frame issued as result
*       of CCFC frame termination.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum         - device number.
*       portNum        - port number.
*       portSpeedIndex - congested port speed index (0..7).
*
* OUTPUTS:
*       timerPtr       - (pointer to) 802.3x Pause time
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number, device or speed index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnFcTimerGet
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
    GT_U32 portSpeedIndex;
    GT_U32 timer;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    portSpeedIndex = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortCnFcTimerGet(devNum, portNum, portSpeedIndex, &timer);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", timer);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnPanicPauseThresholdsSet
*
* DESCRIPTION:
*       Sets the thresholds for triggering a Panic Pause: Global XOFF/XON frame
*       over all ports enabled to send a Panic Pause.
*       A global XOFF frame is triggered if the number of buffers occupied
*       by the CN aware frames crosses up xoffLimit. A global XON frame is
*       triggered if the number of buffers occupied by the CN aware frames
*       crosses down xonLimit.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum      - device number.
*       enable      - GT_TRUE: Enable Panic Pause.
*                     GT_FALSE: Disable Panic Pause.
*       xoffLimit   - X-OFF limit (0..1FFF).Relevant only if enable==GT_TRUE.
*       xonLimit    - X-ON limit (0..1FFF).Relevant only if enable==GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range xoffLimit or
*                                  xonLimit value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Panic Pause uses Port Speed Index 0 and 7 to indicate XON and XOFF.
*          Port timer configurations 0 and 7 must be configured
*           to 0x0 and 0xFFFF, see cpssDxChPortCnFcTimerSet.
*          Congested ports should use Port Speed Index 1 to 6,
*          see cpssDxChPortCnSpeedIndexSet.
*       2. Panic Pause cannot operate together with the IEEE 802.3x triggers.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnPanicPauseThresholdsSet
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
    GT_U32 xoffLimit;
    GT_U32 xonLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    xoffLimit = (GT_U32)inArgs[2];
    xonLimit = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortCnPanicPauseThresholdsSet(devNum, enable, xoffLimit, xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnPanicPauseThresholdsGet
*
* DESCRIPTION:
*       Gets the thresholds for triggering a Panic Pause: Global XOFF/XON frame
*       over all ports enabled to send a Panic Pause.
*       A global XOFF frame is triggered if the number of buffers occupied
*       by the CN aware frames crosses up xoffLimit. A global XON frame is
*       triggered if the number of buffers occupied by the CN aware frames
*       crosses down xonLimit.
*
* APPLICABLE DEVICES:  DxChXcat and above
*
* INPUTS:
*       devNum       - device number.
*
* OUTPUTS:
*       enablePtr    - (pointer to):
*                      GT_TRUE: Enable Panic Pause.
*                      GT_FALSE: Disable Panic Pause.
*       xoffLimitPtr - (pointer to) the X-OFF limit value.
*                      Relevant only if *enablePtr==GT_TRUE.
*       xonLimitPtr  - (pointer to) the X-ON limit value.
*                      Relevant only if *enablePtr==GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnPanicPauseThresholdsGet
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
    GT_U32 xoffLimit;
    GT_U32 xonLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortCnPanicPauseThresholdsGet(devNum, &enable, &xoffLimit, &xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",enable, xoffLimit, xonLimit);



    return CMD_OK;
}

/******************************************************************************
* cpssDxChPortGroupSerdesPowerStatusSet
*
* DESCRIPTION:
*       Sets power state of SERDES port lanes per group
*       according to port capabilities.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum     - physical device number
*       portSerdesGroup  - port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*
*       powerUp   - GT_TRUE  = power up, GT_FALSE = power down
*
* OUTPUTS:
*       None.
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong devNum, portSerdesGroup
*     GT_FAIL          - on error
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     Get SERDES port group according to port by calling to
*     cpssDxChPortSerdesGroupGet.
*
******************************************************************************/
static CMD_STATUS wrCpssDxChPortGroupSerdesPowerStatusSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;
    GT_U8      devNum;
    GT_U32     portSerdesGroup;
    GT_BOOL    powerUp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portSerdesGroup = (GT_U32)inArgs[1];
    powerUp = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortGroupSerdesPowerStatusSet(devNum, portSerdesGroup, powerUp);

  /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;

}

/******************************************************************************
* cpssDxChPortSerdesGroupGet
*
* DESCRIPTION:
*       Get SERDES port group that may be used in per SERDES group APIs.
*
* APPLICABLE DEVICES: DxCh3 and above.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number
*
* OUTPUTS:
*       portSerdesGroupPtr  - Pointer to port group number
*                 DxCh3 Giga/2.5 G, xCat GE devices:
*                        Ports       |    SERDES Group
*                        0..3        |      0
*                        4..7        |      1
*                        8..11       |      2
*                        12..15      |      3
*                        16..19      |      4
*                        20..23      |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*                DxCh3 XG devices:
*                        0           |      0
*                        4           |      1
*                        10          |      2
*                        12          |      3
*                        16          |      4
*                        22          |      5
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*               xCat FE devices
*                        24          |      6
*                        25          |      7
*                        26          |      8
*                        27          |      9
*
* RETURNS:
*     GT_OK            - on success.
*     GT_BAD_PARAM     - on wrong devNum, portNum
*     GT_FAIL          - on error
*     GT_BAD_PTR       - on NULL pointer
*     GT_NOT_SUPPORTED - HW does not support the requested operation
*     GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*     None.
*
*
******************************************************************************/
static CMD_STATUS wrCpssDxChPortSerdesGroupGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 portNum;
    GT_U32 portSerdesGroup;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesGroupGet(devNum, portNum, &portSerdesGroup);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",portSerdesGroup);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnTerminationEnableSet
*
* DESCRIPTION:
*       Enable/Disable termination of CNM (Congestion Notification Message) on given port.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       enable     - GT_TRUE: enable. CNM is terminated, i.e not transmited on this port.
*                    GT_FALSE: disable. CNM isn't terminated, i.e transmited on this port.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnTerminationEnableSet
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
    GT_BOOL enable;

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
    result = cpssDxChPortCnTerminationEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnTerminationEnableGet
*
* DESCRIPTION:
*       Gets the status of CNM termination on given port.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE: enable. CNM is terminated, i.e not transmited on this port.
*                    GT_FALSE: disable. CNM isn't terminated, i.e transmited on this port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnTerminationEnableGet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortCnTerminationEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets CN profile configurations for given tc queue.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum          - device number.
*       profileSet      - profile set.
*       tcQueue         - traffic class queue (0..7).
*       cnProfileCfgPtr - pointer to CN Profile configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to CN profile use cpssDxChPortTxBindPortToDpSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnProfileQueueConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT profileSet;
    GT_U8 tcQueue;
    CPSS_PORT_CN_PROFILE_CONFIG_STC cnProfileCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];

    /* init with zeros */
    cmdOsMemSet(&cnProfileCfg, 0, sizeof(cnProfileCfg));

    tcQueue = (GT_U8)inFields[0];
    cnProfileCfg.cnAware = (GT_U32)inFields[1];
    cnProfileCfg.threshold = (GT_U32)inFields[2];

    /* call cpss api function */
    result = cpssDxChPortCnProfileQueueConfigSet(devNum, profileSet, tcQueue, &cnProfileCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*cpssDxChPortCnProfileQueueConfig Table*/
/*index to iterate get func*/
static GT_U8  prvTcQueueIndex = 0;

/*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnProfileQueueConfigGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT profileSet;
    CPSS_PORT_CN_PROFILE_CONFIG_STC cnProfileCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum     = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];

    if(prvTcQueueIndex >= CPSS_TC_RANGE_CNS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssDxChPortCnProfileQueueConfigGet(devNum, profileSet, prvTcQueueIndex, &cnProfileCfg);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = prvTcQueueIndex;

    /* pack output arguments to galtis string */
    inFields[1] = cnProfileCfg.cnAware;
    inFields[2] = cnProfileCfg.threshold;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, result, "%f");

    prvTcQueueIndex++;
    return CMD_OK;
}
/*******************************************************************************
* cpssDxChPortCnProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets CN profile configurations for given tc queue.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       profileSet   - profile set.
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       cnProfileCfgPtr - pointer to CN Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnProfileQueueConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /*reset on first*/
    prvTcQueueIndex = 0;

    return wrCpssDxChPortCnProfileQueueConfigGetNext(inArgs,inFields,numFields,outArgs);
}

/*******************************************************************************
* cpssDxChPortCnQueueStatusModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable queue status mode. When enabled, CNM is sent to a
*       configurable target that examines the queue status.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: CNM is sent to a targetDev/targetPort.
*                    GT_FALSE: CNM is sent to a sampled packet's source.
*       targetDev  - Target device for the CNM (0..31).
*                    Relevant only if enable==GT_TRUE.
*       targetPort - Target port for the CNM (0..63).
*                    Relevant only if enable==GT_TRUE.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnQueueStatusModeEnableSet
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
    GT_U8 targetDev;
    GT_U8 targetPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    targetDev = (GT_U8)inArgs[2];
    targetPort = (GT_U8)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_DATA_MAC(targetDev,targetPort);

    /* call cpss api function */
    result = cpssDxChPortCnQueueStatusModeEnableSet(devNum, enable, targetDev, targetPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnQueueStatusModeEnableGet
*
* DESCRIPTION:
*        Gets the current status of queue status mode.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr     - (pointer to) status of queue status mode.
*                       GT_TRUE: CNM is sent to a targetDev/targetPort.
*                       GT_FALSE: CNM is sent to a sampled packet's source.
*       targetDevPtr  - (pointer to) Target device for the CNM.
*                                    Relevant only if enable==GT_TRUE.
*       targetPortPtr - (pointer to) Target port for the CNM.
*                                    Relevant only if enable==GT_TRUE.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnQueueStatusModeEnableGet
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
    GT_U8 targetDev;
    GT_U8 targetPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortCnQueueStatusModeEnableGet(devNum, &enable, &targetDev, &targetPort);

    CONVERT_BACK_DEV_PORT_DATA_MAC(targetDev,targetPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", enable, targetDev, targetPort);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnSampleEntrySet
*
* DESCRIPTION:
*       Sets CN sample interval entry.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum - device number.
*       index  - index: qFb 3 msb. (0..7)
*       entryPtr   - (pointer to) entry settings.
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, or entry index
*       GT_OUT_OF_RANGE          - on out of range interval
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1.To implement CCFC functionality set entryPtr fields to:
*          interval = (MTU+256b)/16
*          randBitmap = 0
*       2.To achieve uniform distribution of random values, clear the interval
*         to be randomized to 0. For example, if interval = 0xF0 and
*         randBitmap = 0x0F then the sampling interval is a random value
*         in [0xF0,0xFF].
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnSampleEntrySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 entryIndex;
    CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U8)inArgs[1];
    entry.interval = (GT_U32)inArgs[2];
    entry.randBitmap = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortCnSampleEntrySet(devNum, entryIndex, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnSampleEntryGet
*
* DESCRIPTION:
*       Gets CN sample interval entry.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum - device number.
*       index  - index: qFb 3 msb. (0..7)
* OUTPUTS:
*       entryPtr   - (pointer to) entry settings.
*
* RETURNS:
*       GT_OK                    - on success
*
*       GT_BAD_PARAM             - on wrong device number, or entry index
*       GT_OUT_OF_RANGE          - on out of range interval
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnSampleEntryGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 entryIndex;
    CPSS_PORT_CN_SAMPLE_INTERVAL_ENTRY_STC entry;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    entryIndex = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortCnSampleEntryGet(devNum, entryIndex, &entry);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", entry.interval, entry.randBitmap);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnFbCalcConfigSet
*
* DESCRIPTION:
*       Sets Fb (Feedback) calculation configuration.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum - device number.
*       fbCalcCfgPtr  - (pointer to) feedback Calculation configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To implement CCFC functionality set fbCalcCfgPtr fields to:
*          deltaEnable = 0
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnFbCalcConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC fbCalcCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&fbCalcCfg, 0, sizeof(fbCalcCfg));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    fbCalcCfg.wExp = (GT_32)inFields[0];
    fbCalcCfg.fbLsb = (GT_U32)inFields[1];
    fbCalcCfg.deltaEnable = (GT_BOOL)inFields[2];
    fbCalcCfg.fbMin = (GT_U32)inFields[3];
    fbCalcCfg.fbMax = (GT_U32)inFields[4];

    /* call cpss api function */
    result = cpssDxChPortCnFbCalcConfigSet(devNum, &fbCalcCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnFbCalcConfigGet
*
* DESCRIPTION:
*       Gets Fb (Feedback) calculation configuration.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       fbCalcCfgPtr  - (pointer to) feedback Calculation configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnFbCalcConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_CN_FB_CALCULATION_CONFIG_STC fbCalcCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortCnFbCalcConfigGet(devNum, &fbCalcCfg);

    /* pack output arguments to galtis string */
    inFields[0] = fbCalcCfg.wExp;
    inFields[1] = fbCalcCfg.fbLsb;
    inFields[2] = fbCalcCfg.deltaEnable;
    inFields[3] = fbCalcCfg.fbMin;
    inFields[4] = fbCalcCfg.fbMax;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                           inFields[3], inFields[4]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnFbCalcConfigGetNext
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

/*******************************************************************************
* cpssDxChPortCnPacketLengthSet
*
* DESCRIPTION:
*       Sets packet length used by the CN frames triggering logic.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       packetLength - packet length.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or packet length.
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Configuring a value other than CPSS_DXCH_PORT_CN_LENGTH_ORIG_PACKET_E
*       should be used when the packet length is unknown, e.g. Cut-through mode.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnPacketLengthSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT packetLength;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    packetLength = (CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortCnPacketLengthSet(devNum, packetLength);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnPacketLengthGet
*
* DESCRIPTION:
*       Gets packet length used by the CN frames triggering logic.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       packetLengthPtr - (pointer to) packet length.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnPacketLengthGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_CN_PACKET_LENGTH_ENT packetLength;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortCnPacketLengthGet(devNum, &packetLength);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", packetLength);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnMessageGenerationConfigSet
*
* DESCRIPTION:
*       Sets CNM generation configuration.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum - device number.
*       cnmGenerationCfgPtr - (pointer to) CNM generation configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnMessageGenerationConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_CNM_GENERATION_CONFIG_STC cnmGenerationCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&cnmGenerationCfg, 0, sizeof(cnmGenerationCfg));

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    cnmGenerationCfg.qosProfileId        = (GT_U32) inFields[0];
    cnmGenerationCfg.isRouted            = (GT_U8)  inFields[1];
    cnmGenerationCfg.overrideUp          = (GT_BOOL)inFields[2];
    cnmGenerationCfg.cnmUp               = (GT_U8)  inFields[3];
    cnmGenerationCfg.defaultVlanId       = (GT_U16) inFields[4];
    cnmGenerationCfg.scaleFactor         = (GT_U32) inFields[5];
    cnmGenerationCfg.version             = (GT_U32) inFields[6];
    cnmGenerationCfg.cpidMsb.l[0]        = (GT_U32) inFields[7];
    cnmGenerationCfg.cpidMsb.l[1]        = (GT_U32) inFields[8];
    cnmGenerationCfg.cnUntaggedEnable    = (GT_BOOL)inFields[9];
    cnmGenerationCfg.forceCnTag          = (GT_BOOL)inFields[10];
    cnmGenerationCfg.flowIdTag           = (GT_U32) inFields[11];
    cnmGenerationCfg.appendPacket        = (GT_BOOL)inFields[12];

    /* call cpss api function */
    result = cpssDxChPortCnMessageGenerationConfigSet(devNum, &cnmGenerationCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnMessageGenerationConfigGet
*
* DESCRIPTION:
*       Gets CNM generation configuration.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       cnmGenerationCfgPtr - (pointer to) CNM generation configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnMessageGenerationConfigGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_CNM_GENERATION_CONFIG_STC cnmGenerationCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortCnMessageGenerationConfigGet(devNum, &cnmGenerationCfg);

    /* pack and output table fields */
    inFields[0]  = cnmGenerationCfg.qosProfileId;
    inFields[1]  = cnmGenerationCfg.isRouted;
    inFields[2]  = cnmGenerationCfg.overrideUp;
    inFields[3]  = cnmGenerationCfg.cnmUp;
    inFields[4]  = cnmGenerationCfg.defaultVlanId;
    inFields[5]  = cnmGenerationCfg.scaleFactor;
    inFields[6]  = cnmGenerationCfg.version;
    inFields[7]  = cnmGenerationCfg.cpidMsb.l[0];
    inFields[8]  = cnmGenerationCfg.cpidMsb.l[1];
    inFields[9] = cnmGenerationCfg.cnUntaggedEnable;
    inFields[10] = cnmGenerationCfg.forceCnTag;
    inFields[11] = cnmGenerationCfg.flowIdTag;
    inFields[12] = cnmGenerationCfg.appendPacket;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5],
                                inFields[6], inFields[7], inFields[8],
                                inFields[9], inFields[10], inFields[11],
                                inFields[12]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnMessageGenerationConfigGetNext
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

/*******************************************************************************
* cpssDxChPortCnPrioritySpeedLocationSet
*
* DESCRIPTION:
*       Sets location of the priority/speed bits in the CNM header for parsing
*       and generation of CNM.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum        - device number.
*       indexLocation - index location.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnPrioritySpeedLocationSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT prioritySpeedLocation;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    prioritySpeedLocation = (CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortCnPrioritySpeedLocationSet(devNum, prioritySpeedLocation);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCnPrioritySpeedLocationGet
*
* DESCRIPTION:
*       Gets location of the index (priority/speed) bits in the CNM header
*       for parsing and generation of CNM.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum        - device number.
*
* OUTPUTS:
*       indexLocationPtr - (pointer to) index location.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCnPrioritySpeedLocationGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_CN_PRIORITY_SPEED_LOCATION_ENT prioritySpeedLocation;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortCnPrioritySpeedLocationGet(devNum, &prioritySpeedLocation);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", prioritySpeedLocation);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC (Priority Flow Control) triggering or/and response
*       functionality.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*       pfcEnable  - PFC enable option.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC enable option
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_PFC_ENABLE_ENT enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (CPSS_DXCH_PORT_PFC_ENABLE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortPfcEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcEnableGet
*
* DESCRIPTION:
*       Get the status of PFC (Priority Flow Control) triggering or/and response
*       functionality.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       pfcEnablePtr  - (pointer to) PFC enable option.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC enable option
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_PFC_ENABLE_ENT enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortPfcEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcProfileIndexSet
*
* DESCRIPTION:
*       Binds a source port to a PFC profile.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       profileIndex - profile index (0..7).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range profile index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcProfileIndexSet
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
    GT_U32 profileIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    profileIndex = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPfcProfileIndexSet(devNum, portNum, profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcProfileIndexGet
*
* DESCRIPTION:
*       Gets the port's PFC profile.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       profileIndexPtr - (pointer to) profile index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcProfileIndexGet
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
    GT_U32 profileIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPfcProfileIndexGet(devNum, portNum, &profileIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", profileIndex);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcProfileQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*       pfcProfileCfgPtr - pointer to PFC Profile configurations.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in bytes or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcProfileQueueConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       devNum;
    GT_U32      profileIndex;
    GT_U8       tcQueue;
    CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC pfcProfileCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U8)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];

    /* map input arguments to locals */
    pfcProfileCfg.xonThreshold = (GT_U32)inFields[0];
    pfcProfileCfg.xoffThreshold = (GT_U32)inFields[1];

    /* call cpss api function */
    result = cpssDxChPortPfcProfileQueueConfigSet(devNum, profileIndex,
                                                  tcQueue, &pfcProfileCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcProfileQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       profileIndex - profile index (0..7).
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       pfcProfileCfgPtr - pointer to PFC Profile configurations.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in bytes or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcProfileQueueConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 profileIndex;
    GT_U8 tcQueue;
    CPSS_DXCH_PORT_PFC_PROFILE_CONFIG_STC pfcProfileCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileIndex = (GT_U32)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortPfcProfileQueueConfigGet(devNum, profileIndex,
                                                    tcQueue, &pfcProfileCfg);

    /* pack output arguments to galtis string */
    inFields[0] = pfcProfileCfg.xonThreshold;
    inFields[1] = pfcProfileCfg.xoffThreshold;

    /* pack and output table fields */
    galtisOutput(outArgs, result, "%d%d", inFields[0], inFields[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcCountingModeSet
*
* DESCRIPTION:
*       Sets PFC counting mode.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       pfcCountMode - PFC counting mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcCountingModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT pfcCountMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    pfcCountMode = (CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortPfcCountingModeSet(devNum, pfcCountMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcCountingModeGet
*
* DESCRIPTION:
*       Gets PFC counting mode.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum          - device number.
*
* OUTPUTS:
*       pfcCountModePtr - (pointer to) PFC counting mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcCountingModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_PFC_COUNT_MODE_ENT pfcCountMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortPfcCountingModeGet(devNum, &pfcCountMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", pfcCountMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcGlobalDropEnableSet
*
* DESCRIPTION:
*       Enable/Disable PFC global drop.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*       enable     - GT_TRUE: Enable PFC global drop.
*                    GT_FALSE: Disable PFC global drop.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To configure drop threshold use cpssDxChPortPfcGlobalQueueConfigSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcGlobalDropEnableSet
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
    result = cpssDxChPortPfcGlobalDropEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcGlobalDropEnableGet
*
* DESCRIPTION:
*       Gets the current status of PFC global drop.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of PFC functionality
*                     GT_TRUE:  PFC global drop enabled.
*                     GT_FALSE: PFC global drop disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcGlobalDropEnableGet
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
    result = cpssDxChPortPfcGlobalDropEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcGlobalQueueConfigSet
*
* DESCRIPTION:
*       Sets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum        - device number.
*       tcQueue       - traffic class queue (0..7).
*       xoffThreshold - Xoff threshold (0..0x7FF).
*       dropThreshold - Drop threshold. When a global counter with given tcQueue
*               crosses up the dropThreshold the packets are dropped (0..0x7FF).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range threshold value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. dropThreshold is used when PFC global drop is enabled.
*       See cpssDxChPortPfcGlobalDropEnableSet.
*       2. All thresholds are set in bytes or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcGlobalQueueConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 tcQueue;
    GT_U32 xoffThreshold;
    GT_U32 dropThreshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U8)inArgs[1];
    xoffThreshold = (GT_U32)inArgs[2];
    dropThreshold = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortPfcGlobalQueueConfigSet(devNum, tcQueue,
                                        xoffThreshold, dropThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcGlobalQueueConfigGet
*
* DESCRIPTION:
*       Gets PFC profile configurations for given tc queue.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       xoffThresholdPtr - (pointer to) Xoff threshold.
*       dropThresholdPtr - (pointer to) Drop threshold. When a global counter with given tcQueue
*                       crosses up the dropThreshold the packets are dropped.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer.
*       GT_BAD_PARAM             - on wrong device number, profile index
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       All thresholds are set in bytes or packets.
*       See cpssDxChPortPfcCountingModeSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcGlobalQueueConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 tcQueue;
    GT_U32 xoffThreshold;
    GT_U32 dropThreshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tcQueue = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortPfcGlobalQueueConfigGet(devNum, tcQueue,
                                        &xoffThreshold, &dropThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", xoffThreshold, dropThreshold);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcTimerMapEnableSet
*
* DESCRIPTION:
*       Enables mapping of PFC timer to priority queue for given scheduler profile.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - physical device number.
*       profileSet - the Tx Queue scheduler profile.
*       enable     - Determines whether PFC timmer to Priority Queue map
*                    is used.
*                    GT_TRUE: PFC timer to Priority Queue map used.
*                    GT_FALSE: PFC timmer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcTimerMapEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT profileSet;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortPfcTimerMapEnableSet(devNum, profileSet, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcTimerMapEnableGet
*
* DESCRIPTION:
*       Get the status of PFS timer to priority queue mapping for given
*       scheduler profile.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum     - physical device number
*       profileSet - the Tx Queue scheduler profile Set
*
* OUTPUTS:
*       enablePtr   - (pointer to) status of PFC timmer to Priority Queue
*                      mapping.
*                    GT_TRUE: PFC timmer to Priority Queue map used.
*                    GT_FALSE: PFC timmer to Priority Queue map bypassed.
*                    1:1 mapping between a timer in PFC frame to an egress queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcTimerMapEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT profileSet;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortPfcTimerMapEnableGet(devNum, profileSet, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcTimerToQueueMapSet
*
* DESCRIPTION:
*       Sets PFC timer to priority queue map.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - physical device number
*       pfcTimer     - PFC timer (0..7)
*       tcQueue      - traffic class queue (0..7).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_OUT_OF_RANGE          - on out of traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcTimerToQueueMapSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 pfcTimer;
    GT_U32 tcQueue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    pfcTimer = (GT_U32)inArgs[1];
    tcQueue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortPfcTimerToQueueMapSet(devNum, pfcTimer, tcQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcTimerToQueueMapGet
*
* DESCRIPTION:
*       Gets PFC timer to priority queue map.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum          - physical device number.
*       pfcTimer        - PFC timer (0..7).
*
* OUTPUTS:
*       tcQueuePtr      - (pointer to) traffic class queue.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or PFC timer
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcTimerToQueueMapGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 pfcTimer;
    GT_U32 tcQueue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    pfcTimer = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortPfcTimerToQueueMapGet(devNum, pfcTimer, &tcQueue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", tcQueue);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcShaperToPortRateRatioSet
*
* DESCRIPTION:
*       Sets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum                - physical device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*       shaperToPortRateRatio - shaper rate to port speed ratio
*                               in percentage (0..100).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_OUT_OF_RANGE          - on out of range shaper rate to port speed ratio
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To bind port to scheduler profile use:
*          cpssDxChPortTxBindPortToSchedulerProfileSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcShaperToPortRateRatioSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT profileSet;
    GT_U8 tcQueue;
    GT_U32 shaperToPortRateRatio;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];
    shaperToPortRateRatio = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortPfcShaperToPortRateRatioSet(devNum, profileSet, tcQueue, shaperToPortRateRatio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcShaperToPortRateRatioGet
*
* DESCRIPTION:
*       Gets shaper rate to port speed ratio on given scheduler profile
*       and traffic class queue.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum                - physical device number.
*       profileSet            - the Tx Queue scheduler profile.
*       tcQueue               - traffic class queue (0..7).
*
* OUTPUTS:
*       shaperToPortRateRatioPtr - (pointer to)shaper rate to port speed ratio
*                               in percentage.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number ,profile set
*                                  or traffic class queue
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcShaperToPortRateRatioGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT profileSet;
    GT_U8 tcQueue;
    GT_U32 shaperToPortRateRatio;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    profileSet = (CPSS_PORT_TX_SCHEDULER_PROFILE_SET_ENT)inArgs[1];
    tcQueue = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortPfcShaperToPortRateRatioGet(devNum, profileSet, tcQueue, &shaperToPortRateRatio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", shaperToPortRateRatio);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcForwardEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of PFC frames to the ingress
*       pipeline of a specified port. Processing of unknown MAC control frames
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum  - device number.
*       portNum - physical port number (CPU port not supported)
*       enable  - GT_TRUE:  forward PFC frames to the ingress pipe,
*                 GT_FALSE: do not forward PFC frames to the ingress pipe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is not 01-80-C2-00-00-01 and not the port’s configured MAC Address
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcForwardEnableSet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortPfcForwardEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPfcForwardEnableGet
*
* DESCRIPTION:
*       Get status of PFC frames forwarding
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum      - device number.
*       portNum     - physical port number (CPU port not supported)
*       enablePtr   - current forward PFC frames status
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or profile set
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*   A packet is considered as a PFC frame if all of the following are true:
*   - Packet’s Length/EtherType field is 88-08
*   - Packet’s OpCode field is 01-01
*   - Packet’s MAC DA is not 01-80-C2-00-00-01 and not the port’s configured MAC Address
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPfcForwardEnableGet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortPfcForwardEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortForward802_3xEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of 802.3x Flow Control frames to the ingress
*       pipeline of a specified port. Processing of  802.3x Flow Control frames
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*       enable   - GT_TRUE:  forward 802.3x frames to the ingress pipe,
*                  GT_FALSE: do not forward 802.3x frames to the ingress pipe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       A packet is considered a valid Flow Control packet (i.e., it may be used
*       to halt the port’s packet transmission if it is an XOFF packet, or to
*       resume the port’s packets transmission, if it is an XON packet) if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is 00-01
*       - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortForward802_3xEnableSet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortForward802_3xEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortForward802_3xEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x frames forwarding on a specified port
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*       enablePtr - status of 802.3x frames forwarding
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       A packet is considered a valid Flow Control packet (i.e., it may be used
*       to halt the port’s packet transmission if it is an XOFF packet, or to
*       resume the port’s packets transmission, if it is an XON packet) if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is 00-01
*       - Packet’s MAC DA is 01-80-C2-00-00-01 or the port’s configured MAC Address
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortForward802_3xEnableGet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortForward802_3xEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortForwardUnknownMacControlFramesEnableSet
*
* DESCRIPTION:
*       Enable/disable forwarding of unknown MAC control frames to the ingress
*       pipeline of a specified port. Processing of unknown MAC control frames
*       is done like regular data frames if forwarding enabled.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (CPU port not supported)
*       enable   - GT_TRUE:  forward unknown MAC control frames to the ingress pipe,
*                  GT_FALSE: do not forward unknown MAC control frames to
*                               the ingress pipe.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       A packet is considered as an unknown MAC control frame if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is not 00-01 and not 01-01
*           OR
*         Packet’s MAC DA is not 01-80-C2-00-00-01 and not the port’s configured
*           MAC Address
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortForwardUnknownMacControlFramesEnableSet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortForwardUnknownMacControlFramesEnableSet(devNum,
                                                            portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortForwardUnknownMacControlFramesEnableGet
*
* DESCRIPTION:
*       Get current status of unknown MAC control frames
*           forwarding on a specified port
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
*
* INPUTS:
*       devNum      - physical device number
*       portNum     - physical port number (CPU port not supported)
*       enablePtr   - status of unknown MAC control frames forwarding
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - on NULL pointer
*
* COMMENTS:
*       A packet is considered as an unknown MAC control frame if all of
*       the following are true:
*       - Packet’s Length/EtherType field is 88-08
*       - Packet’s OpCode field is not 00-01 and not 01-01
*           OR
*         Packet’s MAC DA is not 01-80-C2-00-00-01 and not the port’s configured
*           MAC Address
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortForwardUnknownMacControlFramesEnableGet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortForwardUnknownMacControlFramesEnableGet(devNum,
                                                        portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortCrcCheckEnableGet
*
* DESCRIPTION:
*       Get CRC checking (Enable/Disable) state for received packets.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - Pointer to the CRS checking state :
*                       GT_TRUE  - CRC checking is enable,
*                       GT_FALSE - CRC checking is disable.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortCrcCheckEnableGet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortCrcCheckEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortExtraIpgGet
*
* DESCRIPTION:
*       Gets the number of 32-bit words to add to the 12-byte IPG.
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum    - physical device number
*       portNum   - physical port number
*
* OUTPUTS:
*       numberPtr -  pointer to number of words
*                   for DxCh,DxCh2  : number range 0..3
*                   DxCh3 and above : number range 0..127
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - the number is out of range
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortExtraIpgGet
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
    GT_U8 number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortExtraIpgGet(devNum, portNum, &number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", number);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortFlowControlModeSet
*
* DESCRIPTION:
*       Sets Flow Control mode on given port.
*
* APPLICABLE DEVICES: Lion and above
*
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*       fcMode     - flow control mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortFlowControlModeSet
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
    CPSS_DXCH_PORT_FC_MODE_ENT fcMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    fcMode = (CPSS_DXCH_PORT_FC_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortFlowControlModeSet(devNum, portNum, fcMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortFlowControlModeGet
*
* DESCRIPTION:
*       Gets Flow Control mode on given port.
*
* APPLICABLE DEVICES: Lion and above
*
*
* INPUTS:
*       devNum     - device number.
*       portNum    - port number.
*
* OUTPUTS:
*       fcModePtr  - flow control mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortFlowControlModeGet
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
    CPSS_DXCH_PORT_FC_MODE_ENT fcMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortFlowControlModeGet(devNum, portNum, &fcMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", fcMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortInbandAutoNegEnableGet
*
* DESCRIPTION:
*       Gets Auto-Negotiation mode of MAC for a port.
*       The Tri-Speed port MAC may operate in one of the following two modes:
*       - SGMII Mode - In this mode, Auto-Negotiation may be performed
*         out-of-band via the device's Master SMI interface or in-band.
*         The function sets the mode of Auto-Negotiation to in-band or
*         out-of-band.
*       - 1000BASE-X mode - In this mode, the port operates at 1000 Mbps,
*         full-duplex only and supports in-band Auto-Negotiation for link and
*         for Flow Control.
*         The function set in-band Auto-Negotiation mode only.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum  - device number
*       portNum - port number
*
* OUTPUTS:
*       enablePtr  - GT_TRUE  - Auto-Negotiation works in in-band mode.
*                    GT_FALSE - Auto-Negotiation works in out-of-band via
*                            the device's Master SMI interface mode.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - The feature is not supported the port/device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Not supported for CPU port of DX devices and not supported for XG ports.
*       SGMII port In-band Auto-Negotiation is performed by the PCS layer to
*       establish link, speed, and duplex mode.
*       1000BASE-X port In-band Auto-Negotiation is performed by the PCS layer
*       to establish link and flow control support.
*
*       Not relevant for the CPU port
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortInbandAutoNegEnableGet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortInbandAutoNegEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortIpgBaseSet
*
* DESCRIPTION:
*       Sets IPG base for fixed IPG mode on XG ports.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
*       ipgBase  - IPG base
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Relevant only when XG Port is in CPSS_PORT_XGMII_FIXED_E mode.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortIpgBaseSet
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
    CPSS_PORT_XG_FIXED_IPG_ENT ipgBase;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    ipgBase = (CPSS_PORT_XG_FIXED_IPG_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortIpgBaseSet(devNum, portNum, ipgBase);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortIpgBaseGet
*
* DESCRIPTION:
*       Gets IPG base for fixed IPG mode on XG ports.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
* OUTPUTS:
*       ipgBasePtr  - pointer to IPG base
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*
* COMMENTS:
*       Relevant only when XG Port is in CPSS_PORT_XGMII_FIXED_E mode.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortIpgBaseGet
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
    CPSS_PORT_XG_FIXED_IPG_ENT ipgBase;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortIpgBaseGet(devNum, portNum, &ipgBase);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ipgBase);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPeriodicFcEnableGet
*
* DESCRIPTION:
*       Get status of periodic 802.3x flow control transmition.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - Pointer to the periodic 802.3x flow control
*                    tramsition state.
*                    If GT_TRUE, enable transmit.
*                    If GT_FALSE, disable transmit.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPeriodicFcEnableGet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPeriodicFcEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortPreambleLengthGet
*
* DESCRIPTION:
*       Get the port with preamble length for Rx or Tx or both directions.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum       - device number.
*       portNum      - physical port number (or CPU port)
*       direction    - Rx or Tx or both directions
*                      only XG ports support Rx direction
*                      GE ports support only Tx direction.
*
*
* OUTPUTS:
*       lengthPtr    - pointer to preamble length in bytes :
*                      supported length values are : 4,8.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_FAIL                  - on error.
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM - on wrong device number or wrong physical port number or
*                      wrong direction or wrong length
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortPreambleLengthGet
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
    CPSS_PORT_DIRECTION_ENT direction;
    GT_U32 length;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    direction = (CPSS_PORT_DIRECTION_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortPreambleLengthGet(devNum, portNum, direction, &length);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", length);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortSerdesCalibrationStartSet
*
* DESCRIPTION:
*       Start calibration on all SERDES of given port.
*
* APPLICABLE DEVICES: DxChXcat and above
*
* INPUTS:
*       devNum          - device number
*       portNum         - physical port number.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - wrong devNum, portNum
*       GT_FAIL                  - on error
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSerdesCalibrationStartSet
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

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesCalibrationStartSet(devNum, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortXGmiiModeGet
*
* DESCRIPTION:
*       Gets XGMII mode on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number
* OUTPUTS:
*       modePtr  - Pointer to XGMII mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortXGmiiModeGet
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
    CPSS_PORT_XGMII_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortXGmiiModeGet(devNum, portNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortEgressCntrModeGet
*
* DESCRIPTION:
*       Get configuration of a bridge egress counters set.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum     - physical device number
*       cntrSetNum - counter set number.
*
* OUTPUTS:
*       setModeBmpPtr - Pointer to counter mode bitmap. For example:
*                       GT_EGRESS_CNT_PORT | GT_EGRESS_CNT_VLAN | GT_EGRESS_CNT_DP.
*       portNumPtr    - Pointer to the egress port of the packets counted by
*                       the set of counters
*       vlanIdPtr     - Pointer to the vlan id of the packets counted by the
*                       set of counters.
*       tcPtr         - Pointer to the tc of the packets counted by the set
*                       of counters.
*       dpLevelPtr    - Pointer to the dp of the packets counted by the set
*                       of counters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Supported DP levels:  CPSS_DP_GREEN_E, CPSS_DP_RED_E.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortEgressCntrModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 cntrSetNum;
    CPSS_PORT_EGRESS_CNT_MODE_ENT setModeBmp;
    GT_U8 portNum;
    GT_U16 vlanId;
    GT_U8 tc;
    CPSS_DP_LEVEL_ENT dpLevel;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortEgressCntrModeGet(devNum, cntrSetNum, &setModeBmp,
                                            &portNum, &vlanId, &tc, &dpLevel);
    CONVERT_BACK_DEV_PORT_DATA_MAC(devNum,portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d", setModeBmp, portNum, vlanId, tc, dpLevel);


    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxMcastPcktDescrLimitGet
*
* DESCRIPTION:
*       Get maximal descriptors limits for multicast packets.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       mcastMaxDescNumPtr  - (pointer to) the number of descriptors allocated
*                             for multicast packets.
*                             For all devices except Lion and above: in units of 128
*                             descriptors, the actual number descriptors
*                             allocated will be 128 * mcastMaxDescNum.
*                             For Lion and above: actual descriptors number (granularity
*                             of 1).
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxMcastPcktDescrLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mcastMaxDescNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxMcastPcktDescrLimitGet(devNum, &mcastMaxDescNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mcastMaxDescNum);


    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperGlobalParamsGet
*
* DESCRIPTION:
*       Get Global parameters for shaper.
*
* APPLICABLE DEVICES: All DxCh Devices.
*
* INPUTS:
*      devNum                   - device number
*
* OUTPUTS:
*       xgPortsTokensRatePtr     - (pointer to) tokens rate for XG ports' shapers
*       gigPortsTokensRatePtr    - (pointer to) tokens rate for Tri Speed ports' shapers
*       gigPortsSlowRateRatioPtr - (pointer to) slow rate ratio for Tri Speed ports.
*                                  Tokens Update rate for ports with slow rate divided
*                                  to the ratio.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For devices the following values are valid.
*       xgPortsTokensRate    - <1-15>
*       gigPortsTokensRate   - <1-15>
*       gigPortsSlowRateRatio - <1-16>
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperGlobalParamsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 xgPortsTokensRate;
    GT_U32 gigPortsTokensRate;
    GT_U32 gigPortsSlowRateRatio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxShaperGlobalParamsGet(devNum, &xgPortsTokensRate,
                                    &gigPortsTokensRate, &gigPortsSlowRateRatio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", xgPortsTokensRate,
                    gigPortsTokensRate, gigPortsSlowRateRatio);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortBackPressureEnableGet
*
* DESCRIPTION:
*       Gets the state of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr  - pointer to back pressure enable/disable state:
*                       - GT_TRUE to enable Back Pressure
*                       - GT_FALSE to disable Back Pressure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_SUPPORTED         - on not supported feature for a port
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       For XGMII (10 Gbps) ports feature is not supported.
*
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortBackPressureEnableGet
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
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortBackPressureEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacResetStateSet
*
* DESCRIPTION:
*       Set MAC Reset state on specified port on specified device.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state    - Reset state
*                  GT_TRUE   - Port MAC is under Reset
*                  GT_FALSE - Port MAC is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacResetStateSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     portNum;
    GT_BOOL   state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacResetStateSet(devNum, portNum, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortSerdesResetStateSet
*
* DESCRIPTION:
*       Set SERDES Reset state on specified port on specified device.
*
* APPLICABLE DEVICES: DxChXcat and above.
*
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (or CPU port)
*       state  - Reset state
*                GT_TRUE   - Port SERDES is under Reset
*                GT_FALSE - Port SERDES is Not under Reset, normal operation
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortSerdesResetStateSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8     portNum;
    GT_BOOL   state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortSerdesResetStateSet(devNum, portNum, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortFlowControlEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x Flow Control on specific logical port.
*
* APPLICABLE DEVICES:  All DxCh Devices
*
*
* INPUTS:
*       devNum   - physical device number
*       portNum  - physical port number (or CPU port)
*
* OUTPUTS:
*       statePtr - Pointer to Flow Control state: Both disabled,
*                  Both enabled, Only Rx or Only Tx enabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortFlowControlEnableGet
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
    CPSS_PORT_FLOW_CONTROL_ENT state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortFlowControlEnableGet(devNum, portNum, &state);

    if(result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", state);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxSchedulerDeficitModeEnableSet
*
* DESCRIPTION:
*       Enable/Disable scheduler deficit mode.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE: Enable Scheduler Deficit mode.
*                 GT_FALSE: Disable Scheduler Deficit mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Deficit mode should be enabled to support line-rate scheduling
*       in 40/100 Gbps ports.
*       2. Request Masks must be properly configured if deficit mode
*          is enabled, see:
*          cpssDxChPortTxRequestMaskSet
*          cpssDxChPortTxRequestMaskIndexSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSchedulerDeficitModeEnableSet
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
    result = cpssDxChPortTxSchedulerDeficitModeEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxSchedulerDeficitModeEnableGet
*
* DESCRIPTION:
*       Gets the current status of scheduler deficit mode.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) status of scheduler deficit mode
*                    GT_TRUE: Scheduler deficit mode enabled.
*                    GT_FALSE: Scheduler Deficit mode disabled.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSchedulerDeficitModeEnableGet
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
    result = cpssDxChPortTxSchedulerDeficitModeEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxRequestMaskSet
*
* DESCRIPTION:
*       Sets request mask value to given mask index.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       maskIndex    - request mask index (0..3).
*       maskValue    - request mask value defines the minimum number of core clock
*                      cycles between two consecutive service grants
*                      to the same port (3..255).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device number or mask index
*       GT_OUT_OF_RANGE          - on out of range mask value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. Request masks used only when deficit mode is enabled, see:
*          cpssDxChPortTxSchedulerDeficitModeEnableSet.
*       2. To bind a port to request mask index use:
*          cpssDxChPortTxRequestMaskIndexSet.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxRequestMaskSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 maskIndex;
    GT_U32 maskValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    maskIndex = (GT_U32)inArgs[1];
    maskValue = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxRequestMaskSet(devNum, maskIndex, maskValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxRequestMaskGet
*
* DESCRIPTION:
*       Gets request mask value to given mask index.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       maskIndex    - request mask index (0..3).
*
* OUTPUTS:
*       maskValuePtr - (pointer to) request mask value defines the minimum number
*                      of core clock cycles between two consecutive service
*                      grants to the same port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number or mask index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxRequestMaskGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 maskIndex;
    GT_U32 maskValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    maskIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxRequestMaskGet(devNum, maskIndex, &maskValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maskValue);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxRequestMaskIndexSet
*
* DESCRIPTION:
*       Binds a port to a request mask.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*       maskIndex    - mask index (0..3).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range mask index
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxRequestMaskIndexSet
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
    GT_U32 maskIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    maskIndex = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxRequestMaskIndexSet(devNum, portNum, maskIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxRequestMaskIndexGet
*
* DESCRIPTION:
*       Gets the port's request mask.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum       - device number.
*       portNum      - port number.
*
* OUTPUTS:
*       maskIndexPtr - (pointer to) mask index.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxRequestMaskIndexGet
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
    GT_U32 maskIndex;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxRequestMaskIndexGet(devNum, portNum, &maskIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maskIndex);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperModeSet
*
* DESCRIPTION:
*       Set Shaper mode, packet or byte based for given port.
*       Shapers are configured by cpssDxChPortTxShaperProfileSet or
*       cpssDxChPortTxQShaperProfileSet
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number (CPU port supported as well as regular ports).
*       mode    - shaper mode: byte count or packet number based.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperModeSet
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
    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    mode = (CPSS_PORT_TX_DROP_SHAPER_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxShaperModeSet(devNum, portNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperModeGet
*
* DESCRIPTION:
*       Get Shaper mode, packet or byte based for given port.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum  - device number.
*       portNum - port number (CPU port supported as well as regular ports).
*
* OUTPUTS:
*       modePtr - (pointer to) shaper mode: byte count or packet number based.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperModeGet
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
    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxShaperModeGet(devNum, portNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperBaselineSet
*
* DESCRIPTION:
*       Set Token Bucket Baseline.
*       The Token Bucket Baseline is the "zero" level of the token bucket.
*       When the token bucket fill level < Baseline, the respective queue/port
*       is not served.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum   - device number.
*       baseline - Token Bucket Baseline value in bytes(0..0xFFFFFF).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range baseline
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Token Bucket Baseline must be configured as follows:
*        1. At least MTU.
*        2. If PFC response is enabled, the Baseline must be at least 0x3FFFC0.
*        3. If Deficit scheduling is enabled, the Baseline must be
*           at least 4 packet length.
*        4. If both PFC and Deficit scheduling are enabled,
*           the Baseline must set to the sum of both #2 and #3.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperBaselineSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 baseline;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    baseline = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxShaperBaselineSet(devNum, baseline);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperBaselineGet
*
* DESCRIPTION:
*       Get Token Bucket Baseline.
*       The Token Bucket Baseline is the "zero" level of the token bucket.
*       When the token bucket fill level < Baseline, the respective queue/port
*       is not served.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       baselinePtr - (pointer to) Token Bucket Baseline value in bytes.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperBaselineGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 baseline;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxShaperBaselineGet(devNum, &baseline);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", baseline);



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperConfigurationSet
*
* DESCRIPTION:
*       Set global configuration for shaper.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*      devNum     - device number.
*      configsPtr - (pointer to) shaper configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperConfigurationSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC configs;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    configs.tokensRate = (GT_U32)inArgs[1];
    configs.slowRateRatio = (GT_U32)inArgs[2];
    configs.tokensRateGran = (CPSS_DXCH_PORT_TX_SHAPER_GRANULARITY_ENT)inArgs[3];
    configs.portsPacketLength = (GT_U32)inArgs[4];
    configs.cpuPacketLength = (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssDxChPortTxShaperConfigurationSet(devNum, &configs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperConfigurationGet
*
* DESCRIPTION:
*       Get global configuration for shaper.
*
* APPLICABLE DEVICES: Lion and above
*
* INPUTS:
*      devNum     - device number.
*
* OUTPUTS:
*      configsPtr - (pointer to) shaper configuration.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PTR               - on NULL pointer
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       none.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperConfigurationGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_TX_SHAPER_CONFIG_STC configs;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxShaperConfigurationGet(devNum, &configs);

    galtisOutput(outArgs, result, "%d%d%d%d%d",configs.tokensRate,configs.slowRateRatio,
                 configs.tokensRateGran,configs.portsPacketLength,configs.cpuPacketLength);



    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxTailDropBufferConsumptionModeSet
*
* DESCRIPTION:
*       Sets the packet buffer consumption mode and its parameter.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum  - device number.
*       mode    - packet buffer consumption mode.
*       length  - Defines the number of buffers consumed by a packet for
*                 Tail Drop and Scheduling. Relevant only for mode
*                 CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_PACKET_E.
*                 Range: 0..63
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or wrong mode
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on length out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTailDropBufferConsumptionModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT mode;
    GT_U32 length;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT)inArgs[1];
    length  = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxTailDropBufferConsumptionModeSet(devNum, mode, length);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxTailDropBufferConsumptionModeGet
*
* DESCRIPTION:
*       Gets the packet buffer consumption mode and its parameter.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum  - device number.
*
* OUTPUTS:
*       modePtr     - (pointer to) packet buffer consumption mode.
*       lengthPtr   - (pointer to ) the number of buffers consumed by a packet
*                     for Tail Drop and Scheduling. Relevant only for mode
*                 CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_PACKET_E.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong device or wrong mode
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTailDropBufferConsumptionModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_TX_TAIL_DROP_BUFFER_CONSUMPTION_MODE_ENT mode;
    GT_U32 length;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxTailDropBufferConsumptionModeGet(devNum, &mode, &length);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", mode, length);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxTcProfileSharedPoolSet
*
* DESCRIPTION:
*       Sets the shared pool associated for traffic class and Profile.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum  - physical device number
*       pfSet   - the Profile Set in which the Traffic
*                 Class Drop Parameters is associated
*       tc      - the Traffic Class, range 0..7.
*       poolNum - shared pool associated, range 0..7.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTcProfileSharedPoolSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet;
    GT_U8 tc;
    GT_U32 poolNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    tc = (GT_U8)inArgs[2];
    poolNum = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortTxTcProfileSharedPoolSet(devNum, pfSet, tc, poolNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxTcProfileSharedPoolGet
*
* DESCRIPTION:
*       Gets the shared pool associated for traffic class and Profile.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - physical device number
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated
*       tc          - the Traffic Class, range 0..7.
*
* OUTPUTS:
*       poolNumPtr  - (pointer to) shared pool associated.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, pfSet
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
GT_STATUS cpssDxChPortTxTcProfileSharedPoolGet
(
    IN  GT_U8                               devNum,
    IN  CPSS_PORT_TX_DROP_PROFILE_SET_ENT   pfSet,
    IN  GT_U8                               tc,
    OUT GT_U32                              *poolNumPtr
);
static CMD_STATUS wrCpssDxChPortTxTcProfileSharedPoolGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet;
    GT_U8 tc;
    GT_U32 poolNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    tc = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxTcProfileSharedPoolGet(devNum, pfSet, tc, &poolNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", poolNum);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxMcastBuffersLimitSet
*
* DESCRIPTION:
*       Set maximal buffers limits for multicast packets.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum            - physical device number
*       mcastMaxBufNum    - The number of buffers allocated for multicast
*                           packets. Range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range mcastMaxBufNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxMcastBuffersLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mcastMaxBufNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mcastMaxBufNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxMcastBuffersLimitSet(devNum, mcastMaxBufNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxMcastBuffersLimitGet
*
* DESCRIPTION:
*       Get maximal buffers limits for multicast packets.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       mcastMaxBufNumPtr   - (pointer to) the number of buffers allocated
*                             for multicast packets.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxMcastBuffersLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mcastMaxBufNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxMcastBuffersLimitGet(devNum, &mcastMaxBufNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mcastMaxBufNum);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxSniffedBuffersLimitSet
*
* DESCRIPTION:
*       Set maximal buffers limits for mirrored packets.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum            - physical device number
*       rxSniffMaxBufNum  - The number of buffers allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring. Range 0..0xFFFF
*       txSniffMaxBufNum  - The number of buffers allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring. Range 0..0xFFFF
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSniffedBuffersLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rxSniffMaxBufNum;
    GT_U32 txSniffMaxBufNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    rxSniffMaxBufNum = (GT_U32)inArgs[1];
    txSniffMaxBufNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxSniffedBuffersLimitSet(devNum, rxSniffMaxBufNum, txSniffMaxBufNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxSniffedBuffersLimitGet
*
* DESCRIPTION:
*       Get maximal buffers limits for mirrored packets.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum              - physical device number
*
* OUTPUTS:
*       rxSniffMaxBufNumPtr - (pointer to) The number of buffers allocated
*                              for packets forwarded to the ingress analyzer
*                              port due to mirroring.
*       txSniffMaxBufNumPtr - (pointer to) The number of buffers allocated
*                              for packets forwarded to the egress analyzer
*                              port due to mirroring.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSniffedBuffersLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rxSniffMaxBufNum;
    GT_U32 txSniffMaxBufNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxSniffedBuffersLimitGet(devNum, &rxSniffMaxBufNum, &txSniffMaxBufNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", rxSniffMaxBufNum, txSniffMaxBufNum);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxSharedPoolLimitsSet
*
* DESCRIPTION:
*       Set maximal descriptors and buffers limits for shared pool.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum     - physical device number
*       poolNum    - Shared pool number. Range 0..7
*       maxBufNum  - The number of buffers allocated for a shared pool.
*                    Range 0..0x3FFF.
*       maxDescNum - The number of descriptors allocated for a shared pool.
*                    Range 0..0x3FFF.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_OUT_OF_RANGE          - on out of range value
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSharedPoolLimitsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 poolNum;
    GT_U32 maxBufNum;
    GT_U32 maxDescNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    poolNum = (GT_U32)inArgs[1];
    maxBufNum = (GT_U32)inArgs[2];
    maxDescNum = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortTxSharedPoolLimitsSet(devNum, poolNum, maxBufNum, maxDescNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxSharedPoolLimitsGet
*
* DESCRIPTION:
*       Get maximal descriptors and buffers limits for shared pool.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum        - physical device number
*       poolNum       - Shared pool number. Range 0..7
*
* OUTPUTS:
*       maxBufNumPtr  - (pointer to) The number of buffers allocated for a
*                       shared pool.
*       maxDescNumPtr - (pointer to) The number of descriptors allocated
*                       for a shared pool.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSharedPoolLimitsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 poolNum;
    GT_U32 maxBufNum;
    GT_U32 maxDescNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    poolNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxSharedPoolLimitsGet(devNum, poolNum, &maxBufNum, &maxDescNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", maxBufNum, maxDescNum);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxProfileWeightedRandomTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Weighted Random Tail Drop Threshold to overcome
*       synchronization.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum      - device number.
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated.
*       dp          - Drop Precedence, range 0..2.
*       enablersPtr - (pointer to) structure defining WRTD enabling per
*                     {limit,DP} pair.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxProfileWeightedRandomTailDropEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet;
    GT_U32 dp;
    CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC enablers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    dp = (GT_U32)inArgs[2];

    enablers.tcDpLimit = (GT_BOOL)inArgs[3];
    enablers.portLimit = (GT_BOOL)inArgs[4];
    enablers.tcLimit = (GT_BOOL)inArgs[5];
    enablers.sharedPoolLimit = (GT_BOOL)inArgs[6];

    /* call cpss api function */
    result = cpssDxChPortTxProfileWeightedRandomTailDropEnableSet(devNum, pfSet, dp, 0, &enablers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxProfileWeightedRandomTailDropEnableGet
*
* DESCRIPTION:
*       Get Random Tail drop Threshold status.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum      - device number.
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated.
*                     Lion and above relevant only.
*       dp          - Drop Precedence, range 0..2.
*                     Lion and above relevant only.
*
* OUTPUTS:
*       enablersPtr - (pointer to) structure defining WRTD enabling per
*                     {limit,DP} pair.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxProfileWeightedRandomTailDropEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet;
    GT_U32 dp;
    CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC enablers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    dp = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxProfileWeightedRandomTailDropEnableGet(devNum, pfSet, dp, 0, &enablers);

    /* pack and output table fields */
    galtisOutput(outArgs, result, "%d%d%d%d",
                 enablers.tcDpLimit, enablers.portLimit, enablers.tcLimit, enablers.sharedPoolLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxProfileWeightedRandomTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Weighted Random Tail Drop Threshold to overcome
*       synchronization.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum      - device number.
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated.
*       dp          - Drop Precedence, range 0..2.
*       enablersPtr - (pointer to) structure defining WRTD enabling per
*                     {limit,DP} pair.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxProfileWeightedRandomTailDropEnableSet_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet;
    GT_U32 dp;
    GT_U8 tc;
    CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC enablers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    dp = (GT_U32)inArgs[2];
    tc = (GT_U8)inArgs[3];

    enablers.tcDpLimit = (GT_BOOL)inArgs[4];
    enablers.portLimit = (GT_BOOL)inArgs[5];
    enablers.tcLimit = (GT_BOOL)inArgs[6];
    enablers.sharedPoolLimit = (GT_BOOL)inArgs[7];

    /* call cpss api function */
    result = cpssDxChPortTxProfileWeightedRandomTailDropEnableSet(devNum, pfSet, dp, tc, &enablers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxProfileWeightedRandomTailDropEnableGet
*
* DESCRIPTION:
*       Get Random Tail drop Threshold status.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum      - device number.
*       pfSet       - the Profile Set in which the Traffic
*                     Class Drop Parameters is associated.
*                     Lion and above relevant only.
*       dp          - Drop Precedence, range 0..2.
*                     Lion and above relevant only.
*
* OUTPUTS:
*       enablersPtr - (pointer to) structure defining WRTD enabling per
*                     {limit,DP} pair.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on invalid input paramters value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_FAIL                  - on error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxProfileWeightedRandomTailDropEnableGet_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_PROFILE_SET_ENT pfSet;
    GT_U32 dp;
    GT_U8 tc;
    CPSS_PORT_TX_TAIL_DROP_WRTD_ENABLERS_STC enablers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
    dp = (GT_U32)inArgs[2];
    tc = (GT_U8)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortTxProfileWeightedRandomTailDropEnableGet(devNum, pfSet, dp, tc,&enablers);

    /* pack and output table fields */
    galtisOutput(outArgs, result, "%d%d%d%d",
                 enablers.tcDpLimit, enablers.portLimit, enablers.tcLimit, enablers.sharedPoolLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxTailDropWrtdMasksSet
*
* DESCRIPTION:
*       Sets Weighted Random Tail Drop (WRTD) masks.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum     - physical device number
*       maskLsbPtr - WRTD masked least significant bits.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTailDropWrtdMasksSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_WRTD_MASK_LSB_STC maskLsb;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    maskLsb.tcDp = (GT_U32)inArgs[1];
    maskLsb.port = (GT_U32)inArgs[2];
    maskLsb.tc   = (GT_U32)inArgs[3];
    maskLsb.pool = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssDxChPortTxTailDropWrtdMasksSet(devNum, &maskLsb);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxTailDropWrtdMasksGet
*
* DESCRIPTION:
*       Gets Weighted Random Tail Drop (WRTD) masks.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum     - physical device number
*
* OUTPUTS:
*       maskLsbPtr - (pointer to) WRTD masked least significant bits.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxTailDropWrtdMasksGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_WRTD_MASK_LSB_STC maskLsb;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxTailDropWrtdMasksGet(devNum, &maskLsb);

    /* pack and output table fields */
    galtisOutput(outArgs, result, "%d%d%d%d",
                 maskLsb.tcDp, maskLsb.port, maskLsb.tc, maskLsb.pool);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxResourceHistogramThresholdSet
*
* DESCRIPTION:
*       Sets threshold for Histogram counter increment.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum      - physical device number
*       cntrNum     - Histogram Counter number. Range 0..3.
*       threshold   - If the Global Descriptors Counter exceeds this Threshold,
*                     the Histogram Counter is incremented by 1.
*                     Range 0..0x3FFF.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxResourceHistogramThresholdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 cntrNum;
    GT_U32 threshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrNum = (GT_U32)inArgs[1];
    threshold = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssDxChPortTxResourceHistogramThresholdSet(devNum, cntrNum, threshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxResourceHistogramThresholdGet
*
* DESCRIPTION:
*       Gets threshold for Histogram counter increment.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum          - physical device number
*       cntrNum         - Histogram Counter number. Range 0..3.
*
* OUTPUTS:
*       thresholdPtr    - (pointer to) If the Global Descriptors Counter
*                         exceeds this Threshold, the Histogram Counter
*                         is incremented by 1.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxResourceHistogramThresholdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 cntrNum;
    GT_U32 threshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cntrNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxResourceHistogramThresholdGet(devNum, cntrNum, &threshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", threshold);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxResourceHistogramCounterGet
*
* DESCRIPTION:
*       Gets Histogram Counter.
*
* APPLICABLE DEVICES: Lion and above.
*
* INPUTS:
*       devNum  - physical device number
*       cntrNum - Histogram Counter number. Range 0..3.
*
* OUTPUTS:
*       cntrPtr - (pointer to) Histogram Counter value.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number, cntrNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Histogram Counter is cleared on read.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxResourceHistogramCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 cntrNum;
    GT_U32 cntr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cntrNum = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxResourceHistogramCounterGet(devNum, cntrNum, &cntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntr);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxGlobalDescNumberGet
*
* DESCRIPTION:
*       Gets total number of descriptors allocated.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxGlobalDescNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxGlobalDescNumberGet(devNum, &number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", number);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxGlobalBufNumberGet
*
* DESCRIPTION:
*       Gets total number of virtual buffers enqueued.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       numberPtr   - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The buffers associated with enqueued multi-target descriptors are
*       counted multiple times, once for each multi-target descriptor instance.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxGlobalBufNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxGlobalBufNumberGet(devNum, &number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", number);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxMcastDescNumberGet
*
* DESCRIPTION:
*       Gets total number of multi-target descriptors allocated.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Multi-target descriptors are descriptors that are duplicated to
*       multiple egress targets, e.g. Multicast, Broadcast, or unknown Unicast
*       flooded packets.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxMcastDescNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxMcastDescNumberGet(devNum, &number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", number);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxMcastBufNumberGet
*
* DESCRIPTION:
*       Gets total number of multi-target buffers allocated (virtual buffers).
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       numberPtr   - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The buffers associated with enqueued multi-target descriptors are
*       counted multiple times, once for each multi-target descriptor instance.
*       Multi-target descriptors are descriptors that are duplicated to
*       multiple egress targets, e.g. Multicast, Broadcast, or unknown Unicast
*       flooded packets.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxMcastBufNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxMcastBufNumberGet(devNum, &number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", number);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxSniffedDescNumberGet
*
* DESCRIPTION:
*       Gets total number of mirrored descriptors allocated.
*
* APPLICABLE DEVICES:  DxCh3 and above
*
* INPUTS:
*       devNum    - physical device number
*
* OUTPUTS:
*       rxNumberPtr - number of ingress mirrored descriptors.
*       txNumberPtr - number of egress mirrored descriptors.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSniffedDescNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rxNumber;
    GT_U32 txNumber;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxSniffedDescNumberGet(devNum, &rxNumber, &txNumber);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", rxNumber, txNumber);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxSniffedBufNumberGet
*
* DESCRIPTION:
*       Gets total number of mirrored buffers allocated.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum      - physical device number
*
* OUTPUTS:
*       rxNumberPtr - number of ingress mirrored buffers.
*       txNumberPtr - number of egress mirrored buffers.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSniffedBufNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 rxNumber;
    GT_U32 txNumber;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxSniffedBufNumberGet(devNum, &rxNumber, &txNumber);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", rxNumber, txNumber);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxSharedResourceDescNumberGet
*
* DESCRIPTION:
*       Gets total number of descriptors allocated from the shared descriptors
*       pool.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum    - physical device number
*       poolNum   - shared pool number, range 0..7
*
* OUTPUTS:
*       numberPtr - number of descriptors
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong poolNum or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSharedResourceDescNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 pool;
    GT_U32 number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    pool = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxSharedResourceDescNumberGet(devNum, pool, &number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", number);

    return CMD_OK;
}

/*******************************************************************************
*  cpssDxChPortTxSharedResourceBufNumberGet
*
* DESCRIPTION:
*       Gets the number of virtual buffers enqueued from the shared buffer pool.
*
* APPLICABLE DEVICES:  Lion and above
*
* INPUTS:
*       devNum    - physical device number
*       poolNum   - shared pool number, range 0..7
*
* OUTPUTS:
*       numberPtr - number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong poolNum or devNum
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxSharedResourceBufNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 pool;
    GT_U32 number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    pool = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxSharedResourceBufNumberGet(devNum, pool, &number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", number);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxShaperOnStackAsGigEnableSet
*
* DESCRIPTION:
*       Enables/Disables the shaper on stacking ports (24-27) to operate as
*       Tri Speed ports' shapers.
*
* APPLICABLE DEVICES: DxChXcat.
*
* INPUTS:
*       devNum  - physical device number
*       enable  - GT_TRUE, stacking ports shapers operates
*                          as Tri Speed ports' shapers.
*                 GT_FALSE, stacking ports shapers operates as XG ports.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       When "Stack As Gig" enabled, for a port number in the range 24..27,
*       whether Gig or XG type, the global shaper used parameters are those
*       referenced as gig ones -
*       "gigPortsTokensRate" and "gigPortsSlowRateRatio" inputs for
*       "cpssDxChPortTxShaperGlobalParamsSet" API.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperOnStackAsGigEnableSet
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
    result = cpssDxChPortTxShaperOnStackAsGigEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxShaperOnStackAsGigEnableGet
*
* DESCRIPTION:
*       Gets the current state (enable/disable) of the shaper's mode on
*       stacking ports (24-27).
*
* APPLICABLE DEVICES: DxChXcat.
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       enablePtr - (pointer to)
*                 GT_TRUE, stacking ports shapers operates
*                          as Tri Speed ports' shapers.
*                 GT_FALSE, stacking ports shapers operates as XG ports.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device number
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxShaperOnStackAsGigEnableGet
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
    result = cpssDxChPortTxShaperOnStackAsGigEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxByteCountChangeEnableSet
*
* DESCRIPTION:
*       Enables/Disables Byte Count Change of the packet length by per port
*       constant for shaping and/or scheduling rate calculation.
*
* APPLICABLE DEVICES: DxChXcat.
*
* INPUTS:
*       devNum - physical device number.
*       bcChangeEnable - Byte Count Change enable opition.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number or BC change option
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxByteCountChangeEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortTxByteCountChangeEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxByteCountChangeEnableGet
*
* DESCRIPTION:
*       Gets the Byte Count Change enable opition.
*
* APPLICABLE DEVICES: DxChXcat.
*
* INPUTS:
*       devNum - physical device number.
*
* OUTPUTS:
*       bcChangeEnablePtr - (pointer to) Byte Count Change enable opition.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - on wrong device number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxByteCountChangeEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_TX_BC_CHANGE_ENABLE_ENT enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortTxByteCountChangeEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortTxByteCountChangeValueSet
*
* DESCRIPTION:
*       Sets the number of bytes added/subtracted to the length of every frame
*       for shaping/scheduling rate calculation for given port when
*       Byte Count Change is enabled.
*
* APPLICABLE DEVICES: DxChXcat.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*       bcValue - number of bytes added to the length of every frame
*                 for shaping/scheduling rate calculation.(-63..63)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       1. To globally enable/disable Byte Count Change,
*          use cpssDxChPortTxByteCountChangeEnableSet
*       2. To update bcValue for given port Byte Count Change globally disabled.
*          This can cause side effects to shaper/scheduler behavior
*          on other ports if performed under traffic.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxByteCountChangeValueSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      portNum;
    GT_32      value;
    GT_BOOL    isValueMinus;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    isValueMinus = (GT_BOOL)inArgs[2];
    value  = (GT_32)inArgs[3];

    if (isValueMinus == GT_TRUE)
    {
        value *= -1;
    }

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxByteCountChangeValueSet(devNum, portNum, value);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortTxByteCountChangeValueGet
*
* DESCRIPTION:
*       Gets the number of bytes added/subtracted to the length of every frame
*       for shaping/scheduling rate calculation for given port when
*       Byte Count Change is enabled.
*
* APPLICABLE DEVICES: DxChXcat.
*
* INPUTS:
*       devNum  - physical device number
*       portNum - physical port number
*
* OUTPUTS:
*       bcValuePtr - (pointer to) number of bytes added to the length of
*                    every frame for shaping/scheduling rate calculation.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortTxByteCountChangeValueGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS  result;

    GT_U8      devNum;
    GT_U8      portNum;
    GT_32      value;
    GT_BOOL    isValueMinus = GT_FALSE;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortTxByteCountChangeValueGet(devNum, portNum, &value);

    if (value < 0)
    {
        isValueMinus = GT_TRUE;
        value *= -1;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",isValueMinus, value);
    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortModeSpeedAutoDetectAndConfig
*
* DESCRIPTION:
*       Autodetect and configure interface mode and speed of a given port
*       If the process succeeded the port gets configuration ready for link,
*       otherwise (none of provided by application options valid) the port
*       is left in reset mode.
*
* APPLICABLE DEVICES:  Lion.
*
* INPUTS:
*       devNum   - device number
*       portNum  - port number (CPU port not supported)
*       portModeSpeedOptionsArrayPtr – array of port mode and speed options
*                                      preferred for application,
*                                   NULL – if any of supported options acceptable
*       optionsArrayLen – length of options array (must be 0 if
*                                     portModeSpeedOptionsArrayPtr == NULL)
*
*
* OUTPUTS:
*       currentModePtr – if succeeded ifMode and speed that were configured
*                        on the port,
*                        otherwise previous ifMode and speed restored
*
* RETURNS:
*       GT_OK   - if process completed without errors (no matter if mode
*                                                       configured or not)
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_PARAM             - optionsArrayLen more then possible options
*       GT_NOT_SUPPORTED         – ifMode/speed pair not supported
*       GT_HW_ERROR              - HW error
*
* COMMENTS:
*       At this moment supported:
*           CPSS_PORT_INTERFACE_MODE_RXAUI_E         10G
*           CPSS_PORT_INTERFACE_MODE_XGMII_E (XAUI)  10G, 20G
*           CPSS_PORT_INTERFACE_MODE_1000BASE_X_E    1G
*           CPSS_PORT_INTERFACE_MODE_SGMII_E         1G
*
*       There is no way to distinguish between SGMII and 1000baseX
*       until the port is connected to an external device. Application
*       can remove not needed mode from portModeSpeedOptionsArrayPtr
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortModeSpeedAutoDetectAndConfig
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                result;

    GT_U8                    devNum;
    GT_U8                    portNum;
    CPSS_PORT_MODE_SPEED_STC portModeSpeedOptionsArray[4];
    CPSS_PORT_MODE_SPEED_STC *portModeSpeedOptionsArrayPtr = NULL;
    GT_U8                    optionsArrayLen;
    CPSS_PORT_MODE_SPEED_STC currentMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    optionsArrayLen = (GT_U8)inArgs[2];
    if(optionsArrayLen > 4)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_BAD_PARAM, "%s", "optionsArrayLen>4");
        return CMD_OK;
    }

    if(optionsArrayLen > 0)
    {
        portModeSpeedOptionsArray[0].ifMode = (CPSS_PORT_INTERFACE_MODE_ENT)inArgs[3];
        portModeSpeedOptionsArray[0].speed = (CPSS_PORT_SPEED_ENT)inArgs[4];
        portModeSpeedOptionsArrayPtr = portModeSpeedOptionsArray;
    }

    if(optionsArrayLen > 1)
    {
        portModeSpeedOptionsArray[1].ifMode = (CPSS_PORT_INTERFACE_MODE_ENT)inArgs[5];
        portModeSpeedOptionsArray[1].speed = (CPSS_PORT_SPEED_ENT)inArgs[6];
    }

    if(optionsArrayLen > 2)
    {
        portModeSpeedOptionsArray[2].ifMode = (CPSS_PORT_INTERFACE_MODE_ENT)inArgs[7];
        portModeSpeedOptionsArray[2].speed = (CPSS_PORT_SPEED_ENT)inArgs[8];
    }

    if(optionsArrayLen > 3)
    {
        portModeSpeedOptionsArray[3].ifMode = (CPSS_PORT_INTERFACE_MODE_ENT)inArgs[9];
        portModeSpeedOptionsArray[3].speed = (CPSS_PORT_SPEED_ENT)inArgs[10];
    }

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */

    result = cpssDxChPortModeSpeedAutoDetectAndConfig(devNum, portNum, portModeSpeedOptionsArrayPtr,
                                                        optionsArrayLen, &currentMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", currentMode.ifMode, currentMode.speed);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortGlobalRxBufNumberGet
*
* DESCRIPTION:
*       Gets total number of buffers currently allocated.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numOfBuffersPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortGlobalRxBufNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 numOfBuffers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortGlobalRxBufNumberGet(devNum, &numOfBuffers);

    /* pack and output table fields */
    galtisOutput(outArgs, result, "%d", numOfBuffers);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortGroupRxBufNumberGet
*
* DESCRIPTION:
*       Gets total number of buffers currently allocated for the specified
*       port group.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portGroup  - port group
*
* OUTPUTS:
*       numOfBuffersPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device or port group
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortGroupRxBufNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_GROUP_ENT  portGroup;
    GT_U32 numOfBuffers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portGroup = (CPSS_DXCH_PORT_GROUP_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssDxChPortGroupRxBufNumberGet(devNum,portGroup, &numOfBuffers);

    /* pack and output table fields */
    galtisOutput(outArgs, result, "%d", numOfBuffers);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortRxBufNumberGet
*
* DESCRIPTION:
*       Gets total number of buffers currently allocated for the specified port.
*
* APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        None.
*
* INPUTS:
*       devNum     - physical device number
*       portNum    - physical or CPU port number
*
* OUTPUTS:
*       numOfBuffersPtr - (pointer to) number of buffers
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortRxBufNumberGet
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
    GT_U32 numOfBuffers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortRxBufNumberGet(devNum,portNum, &numOfBuffers);

    /* pack and output table fields */
    galtisOutput(outArgs, result, "%d", numOfBuffers);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortGlobalPacketNumberGet
*
* DESCRIPTION:
*       Gets total number of unique packets currently in the system.
*
* APPLICABLE DEVICES:
*        DxCh3; xCat; Lion; xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       numOfPacketsPtr - (pointer to) number of packets
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortGlobalPacketNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 numOfPackets;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortGlobalPacketNumberGet(devNum, &numOfPackets);

    /* pack and output table fields */
    galtisOutput(outArgs, result, "%d", numOfPackets);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacOversizedPacketsCounterModeSet
*
* DESCRIPTION:
*       Sets MIB counters behaviour for oversized packets on given port.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number
*       counterMode - oversized packets counter mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number, device or counter mode
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The oversized packets counter mode determines the behavour of the
*       CPSS_PKTS_1024TOMAX_OCTETS_E and CPSS_DEFERRED_PKTS_SENT_E counters.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacOversizedPacketsCounterModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT  counterMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    counterMode = (CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT)inArgs[2];

    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacOversizedPacketsCounterModeSet(devNum, portNum, counterMode);

    /* pack and output table fields */
    galtisOutput(outArgs, result, "%d", counterMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortMacOversizedPacketsCounterModeGet
*
* DESCRIPTION:
*       Gets MIB counters behaviour for oversized packets on given port.
*
* APPLICABLE DEVICES:
*        xCat2.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; Lion.
*
* INPUTS:
*       devNum         - device number
*       portNum        - physical port number
*
* OUTPUTS:
*       counterModePtr - (pointer to) oversized packets counter mode
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong port number or device
*       GT_BAD_PTR               - on NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       The oversized packets counter mode determines the behavour of the
*       CPSS_PKTS_1024TOMAX_OCTETS_E and CPSS_DEFERRED_PKTS_SENT_E counters.
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortMacOversizedPacketsCounterModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_DXCH_PORT_MAC_OVERSIZED_PACKETS_COUNTER_MODE_ENT  counterMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssDxChPortMacOversizedPacketsCounterModeGet(devNum, portNum, &counterMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssDxChPortBufMemXgFifosThresholdSet
*
* DESCRIPTION:
*       Sets buffer memory XG FIFOs Threshold.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - physical device number
*       xgDescFifoThreshold - Number of descriptors per 20G/40G/HGL port mode.
*                             This is relevant to ports that work in shared
*                             mode (20G/40G/HGL).
*       xgTxFifoThreshold -  Number of lines per Network port in the Tx FIFO.
*       minXgPortXFifoReadThreshold - Minimal number of descriptors to read per
*                            XG port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortBufMemXgFifosThresholdSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      xgDescFifoThreshold;
    GT_U32      xgTxFifoThreshold;
    GT_U32      minXgPortXFifoReadThreshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    xgDescFifoThreshold = (GT_U32)inArgs[1];
    xgTxFifoThreshold = (GT_U32)inArgs[2];
    minXgPortXFifoReadThreshold = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssDxChPortBufMemXgFifosThresholdSet(devNum, xgDescFifoThreshold,
                                                   xgTxFifoThreshold,
                                                   minXgPortXFifoReadThreshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssDxChPortBufMemXgFifosThresholdGet
*
* DESCRIPTION:
*       Gets buffer memory XG FIFOs Threshold.
*
* APPLICABLE DEVICES:
*        Lion.
*
* NOT APPLICABLE DEVICES:
*        DxCh1; DxCh1_Diamond; DxCh2; DxCh3; xCat; xCat2.
*
* INPUTS:
*       devNum - physical device number
*
* OUTPUTS:
*       xgDescFifoThresholdPtr - (pointer to) Number of descriptors per
*                                20G/40G/HGL port mode. This is relevant to
*                                ports that work in shared mode (20G/40G/HGL).
*       xgTxFifoThresholdPtr -  (pointer to) Number of lines per Network
*                                port in the Tx FIFO.
*       minXgPortXFifoReadThresholdPtr - (pointer to) Minimal number of
*                               descriptors to read per XG port.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_FAIL                  - on error
*       GT_BAD_PARAM             - on wrong device
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None
*
*******************************************************************************/
static CMD_STATUS wrCpssDxChPortBufMemXgFifosThresholdGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_U32      xgDescFifoThreshold;
    GT_U32      xgTxFifoThreshold;
    GT_U32      minXgPortXFifoReadThreshold;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssDxChPortBufMemXgFifosThresholdGet(devNum, &xgDescFifoThreshold,
                                                   &xgTxFifoThreshold,
                                                   &minXgPortXFifoReadThreshold);

    /* pack output arguments to galtis string */
   galtisOutput(outArgs, result, "%d%d%d", xgDescFifoThreshold,
                xgTxFifoThreshold, minXgPortXFifoReadThreshold);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssDxChPortTxInit",
        &wrCpssDxChPortTxInit,
        1, 0},
    {"cpssDxChPortTxQueueEnableSet",
        &wrCpssDxChPortTxQueueEnableSet,
        2, 0},
    {"cpssDxChPortTxQueueEnableGet",
        &wrCpssDxChPortTxQueueEnableGet,
        1, 0},
    {"cpssDxChPortTxQueueingEnableSet",
        &wrCpssDxChPortTxQueueingEnableSet,
        4, 0},
    {"cpssDxChPortTxQueueingEnableGet",
        &wrCpssDxChPortTxQueueingEnableGet,
        3, 0},
    {"cpssDxChPortTxQueueingEnableEntrySet",
        &wrCpssDxChPortTxQueueingEnableEntrySet,
        2, 2},
    {"cpssDxChPortTxQueueingEnableEntryGetFirst",
        &wrCpssDxChPortTxQueueingEnableEntryGetFirst,
        2, 0},
    {"cpssDxChPortTxQueueingEnableEntryGetNext",
        &wrCpssDxChPortTxQueueingEnableEntryGetNext,
        2, 0},
    {"cpssDxChPortTxQueueTxEnableSet",
        &wrCpssDxChPortTxQueueTxEnableSet,
        4, 0},
    {"cpssDxChPortTxQueueTxEnableGet",
        &wrCpssDxChPortTxQueueTxEnableGet,
        3, 0},
    {"cpssDxChPortTxFlushQueuesSet",
        &wrCpssDxChPortTxFlushQueuesSet,
        2, 0},
    {"cpssDxChPortTxShaperEnableSet",
        &wrCpssDxChPortTxShaperEnableSet,
        3, 0},
    {"cpssDxChPortTxShaperEnableGet",
        &wrCpssDxChPortTxShaperEnableGet,
        2, 0},
    {"cpssDxChPortTxShaperProfileSet",
        &wrCpssDxChPortTxShaperProfileSet,
        4, 0},
    {"cpssDxChPortTxShaperProfileGet",
        &wrCpssDxChPortTxShaperProfileGet,
        2, 0},
    {"cpssDxChPortTxQShaperEnableSet",
        &wrCpssDxChPortTxQShaperEnableSet,
        4, 0},
    {"cpssDxChPortTxQShaperEnableGet",
        &wrCpssDxChPortTxQShaperEnableGet,
        3, 0},
    {"cpssDxChPortTxQShaperProfileSet",
        &wrCpssDxChPortTxQShaperProfileSet,
        5, 0},
    {"cpssDxChPortTxQShaperProfileGet",
        &wrCpssDxChPortTxQShaperProfileGet,
        3, 0},
    {"cpssDxChPortTxBindPortToSchedulerProfileSet",
        &wrCpssDxChPortTxBindPortToSchedulerProfileSet,
        3, 0},
    {"cpssDxChPortTx4TcTailDropProfile_1Set",
        &wrCpssDxChPortTx4TcTailDropProfileSet,
        3, 6},
    {"cpssDxChPortTx4TcTailDropProfile_2Set",
        &wrCpssDxChPortTx4TcTailDropProfileSet1,
        3, 8},
    {"cpssDxChPortTx4TcTailDropProfile_1GetFirst",
        &wrCpssDxChPortTx4TcTailDropProfileGetFirst,
        3, 0},
    {"cpssDxChPortTx4TcTailDropProfile_1GetNext",
        &wrCpssDxChPortTx4TcTailDropProfileGetNext,
        3, 0},
    {"cpssDxChPortTx4TcTailDropProfile_2GetFirst",
        &wrCpssDxChPortTx4TcTailDropProfileGetFirst1,
        3, 0},
    {"cpssDxChPortTx4TcTailDropProfile_2GetNext",
        &wrCpssDxChPortTx4TcTailDropProfileGetNext,
        3, 0},
    {"cpssDxChPortTxTailDropProfileSet",
        &wrCpssDxChPortTxTailDropProfileSet,
        5, 0},
    {"cpssDxChPortTxSniffedPcktDescrLimitSet",
        &wrCpssDxChPortTxSniffedPcktDescrLimitSet,
        3, 0},
    {"cpssDxChPortTxSniffedPcktDescrLimitGet",
        &wrCpssDxChPortTxSniffedPcktDescrLimitGet,
        1, 0},
    {"cpssDxChPortTxMcastPcktDescrLimitSet",
        &wrCpssDxChPortTxMcastPcktDescrLimitSet,
        2, 0},
    {"cpssDxChPortTxWrrGlobalParamSet",
        &wrCpssDxChPortTxWrrGlobalParamSet,
        3, 0},
    {"cpssDxChPortTxShaperGlobalParamsSet",
        &wrCpssDxChPortTxShaperGlobalParamsSet,
        4, 0},
    {"cpssDxChPortTxQWrrProfileSet",
        &wrCpssDxChPortTxQWrrProfileSet,
        4, 0},
    {"cpssDxChPortTxQWrrProfileGet",
        &wrCpssDxChPortTxQWrrProfileGet,
        3, 0},


    {"cpssDxChPortTxQWrrProfileEntrySet",
        &wrCpssDxChPortTxQWrrProfileEntrySet,
        1, 3},
    {"cpssDxChPortTxQWrrProfileEntryGetFirst",
        &wrCpssDxChPortTxQWrrProfileEntryGetFirst,
        1, 0},
    {"cpssDxChPortTxQWrrProfileEntryGetNext",
        &wrCpssDxChPortTxQWrrProfileEntryGetNext,
        1, 0},


    {"cpssDxChPortTxQArbGroupSet",
        &wrCpssDxChPortTxQArbGroupSet,
        4, 0},
    {"cpssDxChPortTxQArbGroupGet",
        &wrCpssDxChPortTxQArbGroupGet,
        3, 0},
    {"cpssDxChPortTxBindPortToDpSet",
        &wrCpssDxChPortTxBindPortToDpSet,
        3, 0},
    {"cpssDxChPortTxDescNumberGet",
        &wrCpssDxChPortTxDescNumberGet,
        2, 0},
    {"cpssDxChPortTxBufNumberGet",
        &wrCpssDxChPortTxBufNumberGet,
        2, 0},
    {"cpssDxChPortTx4TcDescNumberGet",
        &wrCpssDxChPortTx4TcDescNumberGet,
        3, 0},
    {"cpssDxChPortTx4TcBufNumberGet",
        &wrCpssDxChPortTx4TcBufNumberGet,
        3, 0},
    {"cpssDxChPortTxToCpuShaperModeSet",
        &wrCpssDxChPortTxToCpuShaperModeSet,
        2, 0},
    {"cpssDxChPortTxSharingGlobalResourceEnableSet",
        &wrCpssDxChPortTxSharingGlobalResourceEnableSet,
        2, 0},
    {"cpssDxChPortTxSharingGlobalResourceEnableGet",
        &wrCpssDxChPortTxSharingGlobalResourceEnableGet,
        1, 0},
    {"cpssDxChPortTxSharedGlobalResourceLimitsSet",
        &wrCpssDxChPortTxSharedGlobalResourceLimitsSet,
        3, 0},
    {"cpssDxChPortTxSharedGlobalResourceLimitsGet",
        &wrCpssDxChPortTxSharedGlobalResourceLimitsGet,
        1, 0},
    {"cpssDxChPortTxSharedPolicySet",
         &wrCpssDxChPortTxSharedPolicySet,
         2, 0},
    {"cpssDxChPortTxSharedPolicyGet",
         &wrCpssDxChPortTxSharedPolicyGet,
         1, 0},
    {"cpssDxChPortTxWatchdogGet",
        &wrCpssDxChPortTxWatchdogGet,
        2, 0},
    {"cpssDxChPortTxWatchdogEnableSet",
        &wrCpssDxChPortTxWatchdogEnableSet,
        4, 0},
    {"cpssDxChPortStatInit",
        &wrCpssDxChPortStatInit,
        1, 0},
    {"cpssDxChMacCounterGet",
        &wrCpssDxChMacCounterGet,
        3, 0},
    {"cpssDxChPortMacCountersEnable",
        &wrCpssDxChPortMacCountersEnable,
        3, 0},
    {"cpssDxChPortEgressCntrModeSet_1",
        &wrCpssDxChPortEgressCntrModeSet,
        10, 0},
    {"cpssDxChPortEgressCntrsGetFirst",
        &wrCpssDxChPortEgressCntrsGet,
        2, 0},
    {"cpssDxChPortEgressCntrsGetNext",
        &wrCpssDxChGetNext,
        2, 0},
    {"cpssDxChPortMacSaLsbSet",
        &wrCpssDxChPortMacSaLsbSet,
        3, 0},
    {"cpssDxChPortMacSaLsbGet",
        &wrCpssDxChPortMacSaLsbGet,
        2, 0},
    {"cpssDxChPortEnableSet",
        &wrCpssDxChPortEnableSet,
        3, 0},
    {"cpssDxChPortEnableGet",
        &wrCpssDxChPortEnableGet,
        2, 0},
    {"cpssDxChPortDuplexModeSet",
        &wrCpssDxChPortDuplexModeSet,
        3, 0},
    {"cpssDxChPortDuplexModeGet",
        &wrCpssDxChPortDuplexModeGet,
        2, 0},
    {"cpssDxChPortSpeedSet",
        &wrCpssDxChPortSpeedSet,
        3, 0},
    {"cpssDxChPortSpeedGet",
        &wrCpssDxChPortSpeedGet,
        2, 0},
    {"cpssDxChPortDuplexAutoNegEnableSet",
        &wrCpssDxChPortDuplexAutoNegEnableSet,
        3, 0},
    {"cpssDxChPortDuplexAutoNegEnableGet",
        &wrCpssDxChPortDuplexAutoNegEnableGet,
        2, 0},
    {"cpssDxChPortFlowCntrlAutoNegEnableSet",
        &wrCpssDxChPortFlowCntrlAutoNegEnableSet,
        4, 0},
    {"cpssDxChPortFlowCntrlAutoNegEnableGet",
        &wrCpssDxChPortFlowCntrlAutoNegEnableGet,
        2, 0},
    {"cpssDxChPortSpeedAutoNegEnableSet",
        &wrCpssDxChPortSpeedAutoNegEnableSet,
        3, 0},
    {"cpssDxChPortSpeedAutoNegEnableGet",
        &wrCpssDxChPortSpeedAutoNegEnableGet,
        2, 0},
    {"cpssDxChPortFlowControlEnableSet",
        &wrCpssDxChPortFlowControlEnableSet,
        3, 0},
    {"cpssDxChPortPeriodicFcEnableSet",
        &wrCpssDxChPortPeriodicFcEnableSet,
        3, 0},
    {"cpssDxChPortBackPressureEnableSet",
        &wrCpssDxChPortBackPressureEnableSet,
        3, 0},
    {"cpssDxChPortLinkStatusGet",
        &wrCpssDxChPortLinkStatusGet,
        2, 0},
    {"cpssDxChPortInterfaceModeGet",
        &wrCpssDxChPortInterfaceModeGet,
        2, 0},
    {"cpssDxChPortForceLinkPassEnableSet",
        &wrCpssDxChPortForceLinkPassEnableSet,
        3, 0},
    {"cpssDxChPortForceLinkDownEnableSet",
        &wrCpssDxChPortForceLinkDownEnableSet,
        3, 0},
    {"cpssDxChPortMruSet",
        &wrCpssDxChPortMruSet,
        3, 0},
    {"cpssDxChPortMruGet",
        &wrCpssDxChPortMruGet,
        2, 0},
    {"cpssDxChPortCrcCheckEnableSet",
        &wrCpssDxChPortCrcCheckEnableSet,
        3, 0},
    {"cpssDxChPortXGmiiModeSet",
        &wrCpssDxChPortXGmiiModeSet,
        3, 0},
    {"cpssDxChPortExtraIpgSet",
        &wrCpssDxChPortExtraIpgSet,
        3, 0},
    {"cpssDxChPortXgmiiLocalFaultGet",
        &wrCpssDxChPortXgmiiLocalFaultGet,
        2, 0},
    {"cpssDxChPortXgmiiRemoteFaultGet",
        &wrCpssDxChPortXgmiiRemoteFaultGet,
        2, 0},
    {"cpssDxChPortMacStatus_1GetFirst",
        &wrCpssDxChPortMacStatusGetFirst,
        1, 0},
    {"cpssDxChPortMacStatus_1GetNext",
        &wrCpssDxChPortMacStatusGetNext,
        1, 0},
    {"cpssDxChPortInternalLoopbackEnableSet",
        &wrCpssDxChPortInternalLoopbackEnableSet,
        3, 0},
    {"cpssDxChPortInternalLoopbackEnableGet",
        &wrCpssDxChPortInternalLoopbackEnableGet,
        2, 0},
    {"cpssDxChPortInbandAutoNegEnableSet",
        &wrCpssDxChPortInbandAutoNegEnableSet,
        3, 0},
    {"cpssDxChPortAttributesOnPortGetGetFirst",
        &wrCpssDxChPortAttributesOnPortGet,
        2, 0},
    {"cpssDxChPortAttributesOnPortGetGetNext",
        &wrCpssDxChGetNext,
        2, 0},
    {"cpssDxChPortPreambleLengthSet",
        &wrCpssDxChPortPreambleLengthSet,
        4, 0},
    {"cpssDxChPortMacSaBaseSet",
        &wrCpssDxChPortMacSaBaseSet,
        2, 0},
    {"cpssDxChPortMacSaBaseGet",
        &wrCpssDxChPortMacSaBaseGet,
        1, 0},
    {"cpssDxChPortRxFcProfileSet",
        &wrCpssDxChPortRxFcProfileSet,
        3, 0},
    {"cpssDxChPortRxFcProfileGet",
        &wrCpssDxChPortRxFcProfileGet,
        2, 0},
    {"cpssDxChPortXonLimitSet",
        &wrCpssDxChPortXonLimitSet,
        3, 0},
    {"cpssDxChPortXonLimitGet",
        &wrCpssDxChPortXonLimitGet,
        2, 0},
    {"cpssDxChPortXoffLimitSet",
        &wrCpssDxChPortXoffLimitSet,
        3, 0},
    {"cpssDxChPortXoffLimitGet",
        &wrCpssDxChPortXoffLimitGet,
        2, 0},
    {"cpssDxChPortRxBufLimitSet",
        &wrCpssDxChPortRxBufLimitSet,
        3, 0},
    {"cpssDxChPortRxBufLimitGet",
        &wrCpssDxChPortRxBufLimitGet,
        2, 0},
    {"cpssDxChPortCpuRxBufCountGet",
        &wrCpssDxChPortCpuRxBufCountGet,
        1, 0},
    {"cpssDxChPortFcHolSysModeSet",
        &wrCpssDxChPortFcHolSysModeSet,
        2, 0},
    {"cpssDxChPortBuffersModeSet",
         &wrCpssDxChPortBuffersModeSet,
         2, 0},
    {"cpssDxChPortBuffersModeGet",
         &wrCpssDxChPortBuffersModeGet,
         1, 0},
    {"cpssDxChPortGroupXonLimitSet",
         &wrCpssDxChPortGroupXonLimitSet,
         3, 0},
    {"cpssDxChPortGroupXonLimitGet",
         &wrCpssDxChPortGroupXonLimitGet,
         2, 0},
    {"cpssDxChPortGroupXoffLimitSet",
         &wrCpssDxChPortGroupXoffLimitSet,
         3, 0},
    {"cpssDxChPortGroupXoffLimitGet",
         &wrCpssDxChPortGroupXoffLimitGet,
         2, 0},
    {"cpssDxChPortGroupRxBufLimitSet",
         &wrCpssDxChPortGroupRxBufLimitSet,
         3, 0},
    {"cpssDxChPortGroupRxBufLimitGet",
         &wrCpssDxChPortGroupRxBufLimitGet,
         2, 0},
    {"cpssDxChPortCrossChipFcPacketRelayEnableSet",
         &wrCpssDxChPortCrossChipFcPacketRelayEnableSet,
         2, 0},
    {"cpssDxChPortCrossChipFcPacketRelayEnableGet",
         &wrCpssDxChPortCrossChipFcPacketRelayEnableGet,
         1, 0},
    {"cpssDxChPortClearByBufferEnableSet",
         &wrCpssDxChPortClearByBufferEnableSet,
         3, 0},
    {"cpssDxChPortClearByBufferEnableGet",
         &wrCpssDxChPortClearByBufferEnableGet,
         2, 0},
    {"cpssDxChGlobalXonLimitSet",
        &wrCpssDxChGlobalXonLimitSet,
        2, 0},
    {"cpssDxChGlobalXonLimitGet",
        &wrCpssDxChGlobalXonLimitGet,
        1, 0},
    {"cpssDxChGlobalXoffLimitSet",
        &wrCpssDxChGlobalXoffLimitSet,
     2, 0},
    {"cpssDxChGlobalXoffLimitGet",
        &wrCpssDxChGlobalXoffLimitGet,
        1, 0},
    {"cpssDxChPortTxBindPortToSchedulerProfileGet",
        &wrCpssDxChPortTxBindPortToSchedulerProfileGet,
        2, 0},
    {"cpssDxChPortTxTailDropProfileGet",
        &wrCpssDxChPortTxTailDropProfileGet,
        2, 0},
    {"cpssDxChPortTxBindPortToDpGet",
        &wrCpssDxChPortTxBindPortToDpGet,
        2, 0},
    {"cpssDxChPortTxTailDropUcEnableSet",
        &wrCpssDxChPortTxTailDropUcEnableSet,
        2, 0},
    {"cpssDxChPortTxTailDropUcEnableGet",
        &wrCpssDxChPortTxTailDropUcEnableGet,
        1, 0},
    {"cpssDxChPortTxBufferTailDropEnableSet",
        &wrCpssDxChPortTxBufferTailDropEnableSet,
        2, 0},
    {"cpssDxChPortTxBufferTailDropEnableGet",
        &wrCpssDxChPortTxBufferTailDropEnableGet,
        1, 0},
    {"cpssDxChPortTxBuffersSharingMaxLimitSet",
        &wrCpssDxChPortTxBuffersSharingMaxLimitSet,
        2, 0},
    {"cpssDxChPortTxBuffersSharingMaxLimitGet",
        &wrCpssDxChPortTxBuffersSharingMaxLimitGet,
        1, 0},
    {"cpssDxChPortMacCountersClearOnReadSet",
        &wrCpssDxChPortMacCountersClearOnReadSet,
        3, 0},
    {"cpssDxChPortMacCountersClearOnReadGet",
        &wrCpssDxChPortMacCountersClearOnReadGet,
        2, 0},
    {"cpssDxChPortMacCountersRxHistogramEnable",
        &wrCpssDxChPortMacCountersRxHistogramEnable,
        3, 0},
    {"cpssDxChPortMacCountersTxHistogramEnable",
        &wrCpssDxChPortMacCountersTxHistogramEnable,
        3, 0},
    {"cpssDxChPortTxDp1SharedEnableSet",
         &wrCpssDxChPortTxDp1SharedEnableSet,
         2, 0},
    {"cpssDxChPortTxDp1SharedEnableGet",
         &wrCpssDxChPortTxDp1SharedEnableGet,
         1, 0},
    {"cpssDxChPortTxTcSharedProfileEnableSet",
         &wrCpssDxChPortTxTcSharedProfileEnableSet,
         4, 0},
    {"cpssDxChPortTxTcSharedProfileEnableSet1",
         &wrCpssDxChPortTxTcSharedProfileEnableSet1,
         4, 0},
    {"cpssDxChPortTxTcSharedProfileEnableGet",
         &wrCpssDxChPortTxTcSharedProfileEnableGet,
         3, 0},
    {"cpssDxChPortTxTcSharedProfileEnableGet1",
         &wrCpssDxChPortTxTcSharedProfileEnableGet1,
         3, 0},
    {"cpssDxChPortTxShaperTokenBucketMtuSet",
         &wrCpssDxChPortTxShaperTokenBucketMtuSet,
         2, 0},
    {"cpssDxChPortTxShaperTokenBucketMtuGet",
         &wrCpssDxChPortTxShaperTokenBucketMtuGet,
         1, 0},
    {"cpssDxChPortMacCounterCaptureGet",
         &wrCpssDxChPortMacCounterCaptureGet,
         3, 0},
    {"cpssDxChPortMacCountersOnPortGetFirst",
        &wrCpssDxChPortMacCountersOnPortGetFirst,
        2, 0},
    {"cpssDxChPortMacCountersOnPortGetNext",
        &wrCpssDxChPortMacCountersOnPortGetNext,
        2, 0},
    {"cpssDxChPortMacCountersGetFirst",
        &wrCpssDxChPortMacCountersTableGetFirst,
        1, 0},
    {"cpssDxChPortMacCountersGetNext",
        &wrCpssDxChPortMacCountersTableGetNext,
        1, 0},
    {"cpssDxChPortMacCountersCaptureTriggerSet",
         &wrCpssDxChPortMacCountersCaptureTriggerSet,
         2, 0},
    {"cpssDxChPortMacCountersCaptureTriggerGet",
         &wrCpssDxChPortMacCountersCaptureTriggerGet,
         2, 0},
    {"cpssDxChPortInterfaceModeSet",
         &wrCpssDxChPortInterfaceModeSet,
         3, 0},
    {"cpssDxChPortSerdesComboModeSet",
         &wrCpssDxChPortSerdesComboModeSet,
         3, 0},
    {"cpssDxChPortSerdesComboModeGet",
         &wrCpssDxChPortSerdesComboModeGet,
         2, 0},
    {"cpssDxChPortPaddingEnableSet",
         &wrCpssDxChPortPaddingEnableSet,
         3, 0},
    {"cpssDxChPortPaddingEnableGet",
         &wrCpssDxChPortPaddingEnableGet,
         2, 0},
    {"cpssDxChPortExcessiveCollisionDropEnableSet",
         &wrCpssDxChPortExcessiveCollisionDropEnableSet,
         3, 0},
    {"cpssDxChPortExcessiveCollisionDropEnableGet",
         &wrCpssDxChPortExcessiveCollisionDropEnableGet,
         2, 0},
    {"cpssDxChPortXgLanesSwapEnableSet",
         &wrCpssDxChPortXgLanesSwapEnableSet,
         3, 0},
    {"cpssDxChPortXgLanesSwapEnableGet",
         &wrCpssDxChPortXgLanesSwapEnableGet,
         2, 0},
    {"cpssDxChPortXgPscLanesSwapSet",
         &wrCpssDxChPortXgPscLanesSwapSet,
         10, 0},
    {"cpssDxChPortXgPscLanesSwapGet",
         &wrCpssDxChPortXgPscLanesSwapGet,
         2, 0},
    {"cpssDxChPortInBandAutoNegBypassEnableSet",
         &wrCpssDxChPortInBandAutoNegBypassEnableSet,
         3, 0},
    {"cpssDxChPortInBandAutoNegBypassEnableGet",
         &wrCpssDxChPortInBandAutoNegBypassEnableGet,
         2, 0},
    {"cpssDxChPortMacErrorIndicationPortSet",
         &wrCpssDxChPortMacErrorIndicationPortSet,
         2, 0},
    {"cpssDxChPortMacErrorIndicationPortGet",
         &wrCpssDxChPortMacErrorIndicationPortGet,
         1, 0},
    {"cpssDxChPortMacErrorIndicationGet",
         &wrCpssDxChPortMacErrorIndicationGet,
         1, 0},
    {"cpssDxChPortMacCountersCaptureOnPortGetFirst",
         &wrCpssDxChPortMacCountersCaptureOnPortGet,
         2, 0},
    {"cpssDxChPortMacCountersCaptureOnPortGetNext",
        &wrCpssDxChGetNext,
        2, 0},
    {"cpssDxChPortMacCountersCaptureOnPortGetExtFirst",
         &wrCpssDxChPortMacCountersCaptureOnPortGetExt,
         2, 0},
    {"cpssDxChPortMacCountersCaptureOnPortGetExtNext",
        &wrCpssDxChGetNext,
        2, 0},
    {"cpssDxChPortAttributesGetFirst",
         &wrCpssDxChPortAttributesGetFirst,
         0, 0},
    {"cpssDxChPortAttributesGetNext",
         &wrCpssDxChPortAttributesGetNext,
         0, 0},
    {"cpssDxChPortSerdesConfigSet",
         &wrCpssDxChPortSerdesConfigSet,
         5, 0},
    {"cpssDxChPortSerdesConfigGet",
         &wrCpssDxChPortSerdesConfigGet,
         2, 0},
    {"cpssDxChPortTxRandomTailDropEnableSet",
        &wrCpssDxChPortTxRandomTailDropEnableSet,
        2, 0},
    {"cpssDxChPortTxRandomTailDropEnableGet",
        &wrCpssDxChPortTxRandomTailDropEnableGet,
        1, 0},
    {"cpssDxChPortIpgSet",
        &wrCpssDxChPortIpgSet,
        3, 0},
    {"cpssDxChPortIpgGet",
        &wrCpssDxChPortIpgGet,
        2, 0},
    {"cpssDxChTxPortAllShapersDisable",
        &wrCpssDxChTxPortAllShapersDisable,
        1, 0},
    {"cpssDxChTxPortShapersDisable",
        &wrCpssDxChTxPortShapersDisable,
        2, 0},
    {"cpssDxChPortSerdesPowerStatusSet",
        &wrCpssDxChPortSerdesPowerStatusSet,
        5, 0},
    {"cpssDxChPortSyncEtherSecondRecoveryClkEnableSet",
        &wrCpssDxChPortSyncEtherSecondRecoveryClkEnableSet,
        2, 0},
    {"cpssDxChPortSyncEtherSecondRecoveryClkEnableGet",
        &wrCpssDxChPortSyncEtherSecondRecoveryClkEnableGet,
        1, 0},
    {"cpssDxChPortSyncEtherPllRefClkSelectSet",
        &wrCpssDxChPortSyncEtherPllRefClkSelectSet,
        3, 0},
    {"cpssDxChPortSyncEtherPllRefClkSelectGet",
        &wrCpssDxChPortSyncEtherPllRefClkSelectGet,
        2, 0},
    {"cpssDxChPortSyncEtherRefClkConfigSet",
        &wrCpssDxChPortSyncEtherRefClkConfigSet,
        4, 0},
    {"cpssDxChPortSyncEtherRefClkConfigGet",
        &wrCpssDxChPortSyncEtherRefClkConfigGet,
        2, 0},
    {"cpssDxChPortSyncEtherRefClkConfigSet1",
        &wrCpssDxChPortSyncEtherRefClkConfigSet1,
        5, 0},
    {"cpssDxChPortSyncEtherRefClkConfigGet1",
        &wrCpssDxChPortSyncEtherRefClkConfigGet1,
        2, 0},
    {"cpssDxChPortSyncEtherRecoveryClkDivBypassEnableSet",
        &wrCpssDxChPortSyncEtherRecoveryClkDividerBypassEnableSet,
        2, 0},
    {"cpssDxChPortSyncEtherRecoveryClkDivBypassEnableGet",
        &wrCpssDxChPortSyncEtherRecoveryClkDividerBypassEnableGet,
        1, 0},
    {"cpssDxChPortSyncEtherRecoveryClkDividerValueSet",
        &wrCpssDxChPortSyncEtherRecoveryClkDividerValueSet,
        4, 0},
    {"cpssDxChPortSyncEtherRecoveryClkDividerValueGet",
        &wrCpssDxChPortSyncEtherRecoveryClkDividerValueGet,
        3, 0},
    {"cpssDxChPortSyncEtherRecoveredClkAutoMaskEnableSet",
        &wrCpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableSet,
        4, 0},
    {"cpssDxChPortSyncEtherRecoveredClkAutoMaskEnableGet",
        &wrCpssDxChPortSyncEtherRecoveredClkAutomaticMaskingEnableGet,
        3, 0},
    {"cpssDxChPortCnModeEnableSet",
         &wrCpssDxChPortCnModeEnableSet,
         2, 0},
    {"cpssDxChPortCnModeEnableGet",
         &wrCpssDxChPortCnModeEnableGet,
         1, 0},
    {"cpssDxChPortCnProfileSet",
         &wrCpssDxChPortCnProfileSet,
         3, 0},
    {"cpssDxChPortCnProfileGet",
         &wrCpssDxChPortCnProfileGet,
         2, 0},
    {"cpssDxChPortCnProfileQueueThresholdSet",
         &wrCpssDxChPortCnProfileQueueThresholdSet,
         4, 0},
    {"cpssDxChPortCnProfileQueueThresholdGet",
         &wrCpssDxChPortCnProfileQueueThresholdGet,
         3, 0},
    {"cpssDxChPortCnProfileThresholdSet",
         &wrCpssDxChPortCnProfileThresholdSet,
         3, 0},
    {"cpssDxChPortCnProfileThresholdGet",
         &wrCpssDxChPortCnProfileThresholdGet,
         2, 0},
    {"cpssDxChPortCnQueueAwareEnableSet",
         &wrCpssDxChPortCnQueueAwareEnableSet,
         3, 0},
    {"cpssDxChPortCnQueueAwareEnableGet",
         &wrCpssDxChPortCnQueueAwareEnableGet,
         2, 0},
    {"cpssDxChPortCnFrameQueueSet",
         &wrCpssDxChPortCnFrameQueueSet,
         2, 0},
    {"cpssDxChPortCnFrameQueueGet",
         &wrCpssDxChPortCnFrameQueueGet,
         1, 0},
    {"cpssDxChPortCnEtherTypeSet",
         &wrCpssDxChPortCnEtherTypeSet,
         2, 0},
    {"cpssDxChPortCnEtherTypeGet",
         &wrCpssDxChPortCnEtherTypeGet,
         1, 0},
    {"cpssDxChPortCnSpeedIndexSet",
         &wrCpssDxChPortCnSpeedIndexSet,
         3, 0},
    {"cpssDxChPortCnSpeedIndexGet",
         &wrCpssDxChPortCnSpeedIndexGet,
         2, 0},
    {"cpssDxChPortCnPauseTriggerEnableSet",
         &wrCpssDxChPortCnPauseTriggerEnableSet,
         3, 0},
    {"cpssDxChPortCnPauseTriggerEnableGet",
         &wrCpssDxChPortCnPauseTriggerEnableGet,
         2, 0},
    {"cpssDxChPortCnFcEnableSet",
         &wrCpssDxChPortCnFcEnableSet,
         3, 0},
    {"cpssDxChPortCnFcEnableGet",
         &wrCpssDxChPortCnFcEnableGet,
         2, 0},
    {"cpssDxChPortCnFcTimerSet",
         &wrCpssDxChPortCnFcTimerSet,
         4, 0},
    {"cpssDxChPortCnFcTimerGet",
         &wrCpssDxChPortCnFcTimerGet,
         3, 0},
    {"cpssDxChPortCnPanicPauseThresholdsSet",
         &wrCpssDxChPortCnPanicPauseThresholdsSet,
         4, 0},
    {"cpssDxChPortCnPanicPauseThresholdsGet",
         &wrCpssDxChPortCnPanicPauseThresholdsGet,
         1, 0},
    {"cpssDxChPortGroupSerdesPowerStatusSet",
         &wrCpssDxChPortGroupSerdesPowerStatusSet,
         3, 0},
    {"cpssDxChPortSerdesGroupGet",
         &wrCpssDxChPortSerdesGroupGet,
         2, 0},
    {"cpssDxChPortCnTerminationEnableSet",
         &wrCpssDxChPortCnTerminationEnableSet,
         3, 0},
    {"cpssDxChPortCnTerminationEnableGet",
         &wrCpssDxChPortCnTerminationEnableGet,
         2, 0},
    {"cpssDxChPortCnProfileQueueConfigSet",
         &wrCpssDxChPortCnProfileQueueConfigSet,
         2, 5},
    {"cpssDxChPortCnProfileQueueConfigGetFirst",
         &wrCpssDxChPortCnProfileQueueConfigGetFirst,
         2, 0},
    {"cpssDxChPortCnProfileQueueConfigGetNext",
         &wrCpssDxChPortCnProfileQueueConfigGetNext,
         2, 0},
    {"cpssDxChPortCnProfileQueueConfig_1Set",
         &wrCpssDxChPortCnProfileQueueConfigSet,
         2, 5},
    {"cpssDxChPortCnProfileQueueConfig_1GetFirst",
         &wrCpssDxChPortCnProfileQueueConfigGetFirst,
         2, 0},
    {"cpssDxChPortCnProfileQueueConfig_1GetNext",
         &wrCpssDxChPortCnProfileQueueConfigGetNext,
         2, 0},
    {"cpssDxChPortCnQueueStatusModeEnableSet",
         &wrCpssDxChPortCnQueueStatusModeEnableSet,
         4, 0},
    {"cpssDxChPortCnQueueStatusModeEnableGet",
         &wrCpssDxChPortCnQueueStatusModeEnableGet,
         1, 0},
    {"cpssDxChPortCnSampleEntrySet",
         &wrCpssDxChPortCnSampleEntrySet,
         4, 0},
    {"cpssDxChPortCnSampleEntryGet",
         &wrCpssDxChPortCnSampleEntryGet,
         2, 0},
    {"cpssDxChPortCnFbCalcConfigSet",
         &wrCpssDxChPortCnFbCalcConfigSet,
         1, 5},
    {"cpssDxChPortCnFbCalcConfigGetFirst",
         &wrCpssDxChPortCnFbCalcConfigGetFirst,
         1, 0},
    {"cpssDxChPortCnFbCalcConfigGetNext",
         &wrCpssDxChPortCnFbCalcConfigGetNext,
         1, 0},
    {"cpssDxChPortCnPacketLengthSet",
         &wrCpssDxChPortCnPacketLengthSet,
         2, 0},
    {"cpssDxChPortCnPacketLengthGet",
         &wrCpssDxChPortCnPacketLengthGet,
         1, 0},
    {"cpssDxChPortCnMessageGenerationConfigSet",
         &wrCpssDxChPortCnMessageGenerationConfigSet,
         1, 13},
    {"cpssDxChPortCnMessageGenerationConfigGetFirst",
         &wrCpssDxChPortCnMessageGenerationConfigGetFirst,
         1, 0},
    {"cpssDxChPortCnMessageGenerationConfigGetNext",
         &wrCpssDxChPortCnMessageGenerationConfigGetNext,
         1, 0},
    {"cpssDxChPortCnMessageGenerationConfig_1Set",
         &wrCpssDxChPortCnMessageGenerationConfigSet,
         1, 13},
    {"cpssDxChPortCnMessageGenerationConfig_1GetFirst",
         &wrCpssDxChPortCnMessageGenerationConfigGetFirst,
         1, 0},
    {"cpssDxChPortCnMessageGenerationConfig_1GetNext",
         &wrCpssDxChPortCnMessageGenerationConfigGetNext,
         1, 0},
    {"cpssDxChPortCnPrioritySpeedLocationSet",
         &wrCpssDxChPortCnPrioritySpeedLocationSet,
         2, 0},
    {"cpssDxChPortCnPrioritySpeedLocationGet",
         &wrCpssDxChPortCnPrioritySpeedLocationGet,
         1, 0},
    {"cpssDxChPortPfcEnableSet",
         &wrCpssDxChPortPfcEnableSet,
         2, 0},
    {"cpssDxChPortPfcEnableGet",
         &wrCpssDxChPortPfcEnableGet,
         1, 0},
    {"cpssDxChPortPfcProfileIndexSet",
         &wrCpssDxChPortPfcProfileIndexSet,
         3, 0},
    {"cpssDxChPortPfcProfileIndexGet",
         &wrCpssDxChPortPfcProfileIndexGet,
         2, 0},
    {"cpssDxChPortPfcProfileQueueConfigSet",
         &wrCpssDxChPortPfcProfileQueueConfigSet,
         3, 2},
    {"cpssDxChPortPfcProfileQueueConfigGet",
         &wrCpssDxChPortPfcProfileQueueConfigGet,
         3, 0},
    {"cpssDxChPortPfcCountingModeSet",
         &wrCpssDxChPortPfcCountingModeSet,
         2, 0},
    {"cpssDxChPortPfcCountingModeGet",
         &wrCpssDxChPortPfcCountingModeGet,
         1, 0},
    {"cpssDxChPortPfcGlobalDropEnableSet",
         &wrCpssDxChPortPfcGlobalDropEnableSet,
         2, 0},
    {"cpssDxChPortPfcGlobalDropEnableGet",
         &wrCpssDxChPortPfcGlobalDropEnableGet,
         1, 0},
    {"cpssDxChPortPfcGlobalQueueConfigSet",
         &wrCpssDxChPortPfcGlobalQueueConfigSet,
         4, 0},
    {"cpssDxChPortPfcGlobalQueueConfigGet",
         &wrCpssDxChPortPfcGlobalQueueConfigGet,
         2, 0},
    {"cpssDxChPortPfcTimerMapEnableSet",
         &wrCpssDxChPortPfcTimerMapEnableSet,
         3, 0},
    {"cpssDxChPortPfcTimerMapEnableGet",
         &wrCpssDxChPortPfcTimerMapEnableGet,
         2, 0},
    {"cpssDxChPortPfcTimerToQueueMapSet",
         &wrCpssDxChPortPfcTimerToQueueMapSet,
         3, 0},
    {"cpssDxChPortPfcTimerToQueueMapGet",
         &wrCpssDxChPortPfcTimerToQueueMapGet,
         2, 0},
    {"cpssDxChPortPfcShaperToPortRateRatioSet",
         &wrCpssDxChPortPfcShaperToPortRateRatioSet,
         4, 0},
    {"cpssDxChPortPfcShaperToPortRateRatioGet",
         &wrCpssDxChPortPfcShaperToPortRateRatioGet,
         3, 0},

    {"cpssDxChPortPfcForwardEnableSet",
         &wrCpssDxChPortPfcForwardEnableSet,
         3, 0},
    {"cpssDxChPortPfcForwardEnableGet",
         &wrCpssDxChPortPfcForwardEnableGet,
         2, 0},
    {"cpssDxChPortForward802_3xEnableSet",
         &wrCpssDxChPortForward802_3xEnableSet,
         3, 0},
    {"cpssDxChPortForward802_3xEnableGet",
         &wrCpssDxChPortForward802_3xEnableGet,
         2, 0},
    {"cpssDxChPortForwardUnknownMacControlFramesEnSet",
         &wrCpssDxChPortForwardUnknownMacControlFramesEnableSet,
         3, 0},
    {"cpssDxChPortForwardUnknownMacControlFramesEnGet",
         &wrCpssDxChPortForwardUnknownMacControlFramesEnableGet,
         2, 0},

    {"cpssDxChPortCrcCheckEnableGet",
         &wrCpssDxChPortCrcCheckEnableGet,
         2, 0},
    {"cpssDxChPortExtraIpgGet",
         &wrCpssDxChPortExtraIpgGet,
         2, 0},
    {"cpssDxChPortFlowControlModeSet",
         &wrCpssDxChPortFlowControlModeSet,
         3, 0},
    {"cpssDxChPortFlowControlModeGet",
         &wrCpssDxChPortFlowControlModeGet,
         2, 0},
    {"cpssDxChPortInbandAutoNegEnableGet",
         &wrCpssDxChPortInbandAutoNegEnableGet,
         2, 0},
    {"cpssDxChPortIpgBaseSet",
         &wrCpssDxChPortIpgBaseSet,
         3, 0},
    {"cpssDxChPortIpgBaseGet",
         &wrCpssDxChPortIpgBaseGet,
         2, 0},
    {"cpssDxChPortPeriodicFcEnableGet",
         &wrCpssDxChPortPeriodicFcEnableGet,
         2, 0},
    {"cpssDxChPortPreambleLengthGet",
         &wrCpssDxChPortPreambleLengthGet,
         3, 0},
    {"cpssDxChPortSerdesCalibrationStartSet",
         &wrCpssDxChPortSerdesCalibrationStartSet,
         2, 0},
    {"cpssDxChPortXGmiiModeGet",
         &wrCpssDxChPortXGmiiModeGet,
         2, 0},
    {"cpssDxChPortEgressCntrModeGet",
         &wrCpssDxChPortEgressCntrModeGet,
         2, 0},
    {"cpssDxChPortTxMcastPcktDescrLimitGet",
         &wrCpssDxChPortTxMcastPcktDescrLimitGet,
         1, 0},
    {"cpssDxChPortTxShaperGlobalParamsGet",
         &wrCpssDxChPortTxShaperGlobalParamsGet,
         1, 0},
    {"cpssDxChPortBackPressureEnableGet",
         &wrCpssDxChPortBackPressureEnableGet,
         2, 0},
    {"cpssDxChPortMacResetStateSet",
         &wrCpssDxChPortMacResetStateSet,
         3, 0},
    {"cpssDxChPortSerdesResetStateSet",
         &wrCpssDxChPortSerdesResetStateSet,
         3, 0},
    {"cpssDxChPortFlowControlEnableGet",
        &wrCpssDxChPortFlowControlEnableGet,
        2, 0},
    {"cpssDxChPortTxSchedulerDeficitModeEnableSet",
         &wrCpssDxChPortTxSchedulerDeficitModeEnableSet,
         2, 0},
    {"cpssDxChPortTxSchedulerDeficitModeEnableGet",
         &wrCpssDxChPortTxSchedulerDeficitModeEnableGet,
         1, 0},
    {"cpssDxChPortTxRequestMaskSet",
         &wrCpssDxChPortTxRequestMaskSet,
         3, 0},
    {"cpssDxChPortTxRequestMaskGet",
         &wrCpssDxChPortTxRequestMaskGet,
         2, 0},
    {"cpssDxChPortTxRequestMaskIndexSet",
         &wrCpssDxChPortTxRequestMaskIndexSet,
         3, 0},
    {"cpssDxChPortTxRequestMaskIndexGet",
         &wrCpssDxChPortTxRequestMaskIndexGet,
         2, 0},
    {"cpssDxChPortTxShaperModeSet",
         &wrCpssDxChPortTxShaperModeSet,
         3, 0},
    {"cpssDxChPortTxShaperModeGet",
         &wrCpssDxChPortTxShaperModeGet,
         2, 0},
    {"cpssDxChPortTxShaperBaselineSet",
         &wrCpssDxChPortTxShaperBaselineSet,
         2, 0},
    {"cpssDxChPortTxShaperBaselineGet",
         &wrCpssDxChPortTxShaperBaselineGet,
         1, 0},
    {"cpssDxChPortTxShaperConfigurationSet",
         &wrCpssDxChPortTxShaperConfigurationSet,
         6, 0},
    {"cpssDxChPortTxShaperConfigurationGet",
         &wrCpssDxChPortTxShaperConfigurationGet,
         1, 0},
    {"cpssDxChPortTxTailDropBufferConsumptionModeSet",
         &wrCpssDxChPortTxTailDropBufferConsumptionModeSet,
         3, 0},
    {"cpssDxChPortTxTailDropBufferConsumptionModeGet",
         &wrCpssDxChPortTxTailDropBufferConsumptionModeGet,
         1, 0},
    {"cpssDxChPortTxTcProfileSharedPoolSet",
         &wrCpssDxChPortTxTcProfileSharedPoolSet,
         4, 0},
    {"cpssDxChPortTxTcProfileSharedPoolGet",
         &wrCpssDxChPortTxTcProfileSharedPoolGet,
         3, 0},
    {"cpssDxChPortTxMcastBuffersLimitSet",
         &wrCpssDxChPortTxMcastBuffersLimitSet,
         2, 0},
    {"cpssDxChPortTxMcastBuffersLimitGet",
         &wrCpssDxChPortTxMcastBuffersLimitGet,
         1, 0},
    {"cpssDxChPortTxSniffedBuffersLimitSet",
         &wrCpssDxChPortTxSniffedBuffersLimitSet,
         3, 0},
    {"cpssDxChPortTxSniffedBuffersLimitGet",
         &wrCpssDxChPortTxSniffedBuffersLimitGet,
         1, 0},
    {"cpssDxChPortTxSharedPoolLimitsSet",
         &wrCpssDxChPortTxSharedPoolLimitsSet,
         4, 0},
    {"cpssDxChPortTxSharedPoolLimitsGet",
         &wrCpssDxChPortTxSharedPoolLimitsGet,
         2, 0},
    {"cpssDxChPortTxProfileWeightedRandTailDropEnableSet",
         &wrCpssDxChPortTxProfileWeightedRandomTailDropEnableSet,
         7, 0},
    {"cpssDxChPortTxProfileWeightedRandTailDropEnableGet",
         &wrCpssDxChPortTxProfileWeightedRandomTailDropEnableGet,
         3, 0},
    {"cpssDxChPortTxProfileWRTDEnableSet_1",
         &wrCpssDxChPortTxProfileWeightedRandomTailDropEnableSet_1,
         8, 0},
    {"cpssDxChPortTxProfileWRTDEnableGet_1",
         &wrCpssDxChPortTxProfileWeightedRandomTailDropEnableGet_1,
         4, 0},
    {"cpssDxChPortTxTailDropWrtdMasksSet",
         &wrCpssDxChPortTxTailDropWrtdMasksSet,
         5, 0},
    {"cpssDxChPortTxTailDropWrtdMasksGet",
         &wrCpssDxChPortTxTailDropWrtdMasksGet,
         1, 0},
    {"cpssDxChPortTxResourceHistogramThresholdSet",
         &wrCpssDxChPortTxResourceHistogramThresholdSet,
         3, 0},
    {"cpssDxChPortTxResourceHistogramThresholdGet",
         &wrCpssDxChPortTxResourceHistogramThresholdGet,
         2, 0},
    {"cpssDxChPortTxResourceHistogramCounterGet",
         &wrCpssDxChPortTxResourceHistogramCounterGet,
         2, 0},
    {"cpssDxChPortTxGlobalDescNumberGet",
         &wrCpssDxChPortTxGlobalDescNumberGet,
         1, 0},
    {"cpssDxChPortTxGlobalBufNumberGet",
         &wrCpssDxChPortTxGlobalBufNumberGet,
         1, 0},
    {"cpssDxChPortTxMcastDescNumberGet",
         &wrCpssDxChPortTxMcastDescNumberGet,
         1, 0},
    {"cpssDxChPortTxMcastBufNumberGet",
         &wrCpssDxChPortTxMcastBufNumberGet,
         1, 0},
    {"cpssDxChPortTxSniffedDescNumberGet",
         &wrCpssDxChPortTxSniffedDescNumberGet,
         1, 0},
    {"cpssDxChPortTxSniffedBufNumberGet",
         &wrCpssDxChPortTxSniffedBufNumberGet,
         1, 0},
    {"cpssDxChPortTxSharedResourceDescNumberGet",
         &wrCpssDxChPortTxSharedResourceDescNumberGet,
         2, 0},
    {"cpssDxChPortTxSharedResourceBufNumberGet",
         &wrCpssDxChPortTxSharedResourceBufNumberGet,
         2, 0},
    {"cpssDxChPortTxShaperOnStackAsGigEnableSet",
         &wrCpssDxChPortTxShaperOnStackAsGigEnableSet,
         2, 0},
    {"cpssDxChPortTxShaperOnStackAsGigEnableGet",
         &wrCpssDxChPortTxShaperOnStackAsGigEnableGet,
         1, 0},
    {"cpssDxChPortTxByteCountChangeEnableSet",
         &wrCpssDxChPortTxByteCountChangeEnableSet,
         2, 0},
    {"cpssDxChPortTxByteCountChangeEnableGet",
         &wrCpssDxChPortTxByteCountChangeEnableGet,
         1, 0},
    {"cpssDxChPortTxByteCountChangeValueSet",
         &wrCpssDxChPortTxByteCountChangeValueSet,
         4, 0},
    {"cpssDxChPortTxByteCountChangeValueGet",
         &wrCpssDxChPortTxByteCountChangeValueGet,
         2, 0},
    {"cpssDxChPortModeSpeedAutoDetectAndConfig",
         &wrCpssDxChPortModeSpeedAutoDetectAndConfig,
         11, 0},
    {"cpssDxChPortGlobalRxBufNumberGet",
         &wrCpssDxChPortGlobalRxBufNumberGet,
         1, 0},
    {"cpssDxChPortGroupRxBufNumberGet",
         &wrCpssDxChPortGroupRxBufNumberGet,
         2, 0},
    {"cpssDxChPortRxBufNumberGet",
         &wrCpssDxChPortRxBufNumberGet,
         2, 0},
    {"cpssDxChPortGlobalPacketNumberGet",
         &wrCpssDxChPortGlobalPacketNumberGet,
         1, 0},
    {"cpssDxChPortMacOversizedPacketsCounterModeSet",
         &wrCpssDxChPortMacOversizedPacketsCounterModeSet,
         3, 0},
    {"cpssDxChPortMacOversizedPacketsCounterModeGet",
         &wrCpssDxChPortMacOversizedPacketsCounterModeGet,
         2, 0},
    {"cpssDxChPortBufMemXgFifosThresholdSet",
         &wrCpssDxChPortBufMemXgFifosThresholdSet,
         4, 0},
    {"cpssDxChPortBufMemXgFifosThresholdGet",
         &wrCpssDxChPortBufMemXgFifosThresholdGet,
         1, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssDxChPortTx
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
GT_STATUS cmdLibInitCpssDxChPort
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

