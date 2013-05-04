/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmPortTx.c
*
* DESCRIPTION:
*       Wrapper functions for
*           cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortTx.h API's
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
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortTx.h>

/*******************************************************************************
* cpssExMxPmPortTxInit
*
* DESCRIPTION:
*       Init port Tx configuration.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum - device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxInit
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
    result = cpssExMxPmPortTxInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueBlockEnableSet
*
* DESCRIPTION:
*       Globally enable/disable queuing on all Tx queues.
*       Affects all ports.
*
* INPUTS:
*       devNum - device number
*       enable - GT_TRUE, Global enqueuing enabled
*                GT_FALSE, Global enqueuing disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueBlockEnableSet
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
    result = cpssExMxPmPortTxQueueBlockEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueBlockEnableGet
*
* DESCRIPTION:
*       Gets enabled/disabled enqueuing status of all Tx queues.
*
* INPUTS:
*       devNum    - device number
*
* OUTPUTS:
*       enablePtr - (pointer to) GT_TRUE,  Global enqueuing enabled
*                                GT_FALSE, Global enqueuing disabled
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueBlockEnableGet
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
    result = cpssExMxPmPortTxQueueBlockEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueingEnableSet
*
* DESCRIPTION:
*       Enable/Disable queuing on Traffic-Class-queue per port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       tc      - traffic class queue
*       enable  - GT_TRUE, enable enqueuing to the queue
*                 GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueingEnableSet
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
    GT_U32 tc;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tc = (GT_U32)inFields[0];
    enable = (GT_BOOL)inFields[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueingEnableSet(devNum, portNum, tc, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueingEnableGet
*
* DESCRIPTION:
*       Gets enabled/disabled enqueuing status of a Traffic Class queue
*       per port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number
*       tc         - traffic class queue
* OUTPUTS:
*       enablePtr  - (pointer to)GT_TRUE,  enqueuing to the queue enabled
*                               GT_FALSE,  enqueuing to the queue disabled
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32 tc = 0;

static CMD_STATUS wrCpssExMxPmPortTxQueueingEnableGetFirst
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

    tc = 0;

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueingEnableGet(devNum, portNum, tc, &enable);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = tc;
    inFields[1] = (GT_BOOL)enable;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    tc++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");


    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueingEnableGetNext
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

    if(tc == 8)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueingEnableGet(devNum, portNum, tc, &enable);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = tc;
    inFields[1] = (GT_BOOL)enable;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    tc++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueTxEnableSet
*
* DESCRIPTION:
*       Enable/Disable transmission from a Traffic Class queue per port.
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       tc      - traffic class queue
*       enable  - GT_TRUE, enable transmission from the queue
*                 GT_FALSE, disable transmission from the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueTxEnableSet
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
    GT_U32 tc;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tc = (GT_U32)inFields[0];
    enable = (GT_BOOL)inFields[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueTxEnableSet(devNum, portNum, tc, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueTxEnableGet
*
* DESCRIPTION:
*       Gets enabled/disabled transmission status  from a Traffic Class queue
*       per port.
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       tc      - traffic class queue
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE, transmission from the queue enabled
*                                GT_FALSE, transmission from the queue disabled
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_U32 tc_queue_enable;

static CMD_STATUS wrCpssExMxPmPortTxQueueTxEnableGetFirst
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

    tc_queue_enable = 0;

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueTxEnableGet(devNum, portNum, tc_queue_enable, &enable);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    inFields[0] = tc_queue_enable;
    inFields[1] = (GT_BOOL)enable;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    tc_queue_enable++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueTxEnableGetNext
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

    if(tc_queue_enable == 8)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueTxEnableGet(devNum, portNum, tc_queue_enable, &enable);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    inFields[0] = tc_queue_enable;
    inFields[1] = (GT_BOOL)enable;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    tc_queue_enable++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;

}

/*******************************************************************************
* cpssExMxPmPortTxQueueTxEnableGet
*
* DESCRIPTION:
*       Gets enabled/disabled transmission status  from a Traffic Class queue
*       per port.
*
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       tc      - traffic class queue
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE, transmission from the queue enabled
*                                GT_FALSE, transmission from the queue disabled
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
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueTxEnableGet
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
    GT_U32 tc;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tc = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueTxEnableGet(devNum, portNum, tc, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueuesFlush
*
* DESCRIPTION:
*       Flush all the traffic class queues per port.
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueuesFlush
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
    result = cpssExMxPmPortTxQueuesFlush(devNum, portNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable Token Bucket rate shaping per port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
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
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxShaperEnableSet
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
    result = cpssExMxPmPortTxShaperEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxShaperEnableGet
*
* DESCRIPTION:
*       Gets enabled/disabled status of Token Bucket rate shaping per port.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE, Shaping enabled
*                                GT_FALSE, Shaping disabled
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxShaperEnableGet
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
    result = cpssExMxPmPortTxShaperEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxShaperProfileSet
*
* DESCRIPTION:
*       Set Token Bucket Shaper Profile per port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number
*       bucketSize - bucket size in units of 4K bytes
*                    (max value is 4K which results in 16M bucket size)
*       maxRatePtr - (pointer to) Requested Rate in Kbps
*
* OUTPUTS:
*       maxRatePtr - (pointer to) the actual Rate value in Kbps.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxShaperProfileSet
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
    GT_U32 bucketSize;
    GT_U32 maxRate;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    bucketSize = (GT_U32)inArgs[2];
    maxRate = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxShaperProfileSet(devNum, portNum, bucketSize, &maxRate);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maxRate);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxShaperProfileGet
*
*   DESCRIPTION:
*       Get Token Bucket Shaper Profile per port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number
*
* OUTPUTS:
*       bucketSizePtr- (pointer to) bucket size (in units of 4K bytes)
*       maxRatePtr   - (pointer to) Actual Rate in Kbps.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*      The actual maxRate may differ from the requested maxRate due to the
*      rate granularity of the shaper.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxShaperProfileGet
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
    GT_U32 bucketSize;
    GT_U32 maxRate;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxShaperProfileGet(devNum, portNum, &bucketSize, &maxRate);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", bucketSize, maxRate);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueShaperEnableSet
*
* DESCRIPTION:
*       Enable/Disable shaping of transmitted traffic from a specified Traffic
*       Class Queue and per port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       portNum - physical port number
*       tc      - traffic class queue
*       enable  - GT_TRUE, enable shaping on this traffic queue
*                 GT_FALSE, disable shaping on this traffic queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueShaperEnableSet
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
    GT_U32 tc;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tc = (GT_U32)inFields[0];
    enable = (GT_BOOL)inFields[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueShaperEnableSet(devNum, portNum, tc, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueShaperEnableGet
*
* DESCRIPTION:
*       Gets enabled/disabled shaping status of transmitted traffic from a
*       specified Traffic Class Queue and per port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number
*       tc          - traffic class queue
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE, shaping on this traffic queue enabled
*                                GT_FALSE, shaping on this traffic queue disabled
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static GT_U32 tc_shaper_enable;

static CMD_STATUS wrCpssExMxPmPortTxQueueShaperEnableGetFirst
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

    tc_shaper_enable = 0;

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueShaperEnableGet(devNum, portNum, tc_shaper_enable, &enable);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    inFields[0] = tc_shaper_enable;
    inFields[1] = enable;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    tc_shaper_enable++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueShaperEnableGetNext
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

    if(tc_shaper_enable == 8)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueShaperEnableGet(devNum, portNum, tc_shaper_enable, &enable);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    inFields[0] = tc_shaper_enable;
    inFields[1] = enable;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    tc_shaper_enable++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueShaperEnableGet
*
* DESCRIPTION:
*       Gets enabled/disabled shaping status of transmitted traffic from a
*       specified Traffic Class Queue and per port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       portNum     - physical port number
*       tc          - traffic class queue
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE, shaping on this traffic queue enabled
*                                GT_FALSE, shaping on this traffic queue disabled
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
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueShaperEnableGet
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
    GT_U32 tc;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tc = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueShaperEnableGet(devNum, portNum, tc, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueShaperProfileSet
*
* DESCRIPTION:
*       Set Token Bucket Shaper Profile for Traffic Class Queue per port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       portNum      - physical port number
*       tc           - traffic class queue
*       bucketSize   - bucket size in units of 4K bytes
*                      (max value is 4K which results in 16M bucket size)
*       maxRatePtr   - (pointer to) Requested Rate in Kbps
*
* OUTPUTS:
*       maxRatePtr   - (pointer to) The actual Rate value in Kbps
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueShaperProfileSet
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
    GT_U32 tc;
    GT_U32 bucketSize;
    GT_U32 maxRate;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tc = (GT_U32)inFields[0];
    bucketSize = (GT_U32)inFields[1];
    maxRate = (GT_U32)inFields[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueShaperProfileSet(devNum, portNum, tc, bucketSize, &maxRate);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueShaperProfileGet
*
* DESCRIPTION:
*       Get Token Bucket Shaper Profile for TC Queue per port.
*
*   APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       portNum      - physical port number
*       tc           - traffic class queue
*
* OUTPUTS:
*       bucketSizePtr- (pointer to) bucket size (in units of 4K bytes)
*       maxRatePtr   - (pointer to) Actual Rate in Kbps
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*      None.
*
*******************************************************************************/
static GT_U32 tc_shaper_profile;

static CMD_STATUS wrCpssExMxPmPortTxQueueShaperProfileGetFirst
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
    GT_U32 bucketSize;
    GT_U32 maxRate;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    tc_shaper_profile = 0;

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueShaperProfileGet(devNum, portNum, tc_shaper_profile, &bucketSize, &maxRate);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = tc_shaper_profile;
    inFields[1] = bucketSize;
    inFields[2] = maxRate;

    tc_shaper_profile++;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1],inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueShaperProfileGetNext
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
    GT_U32 bucketSize;
    GT_U32 maxRate;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    if(tc_shaper_profile == 8)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueShaperProfileGet(devNum, portNum, tc_shaper_profile, &bucketSize, &maxRate);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    inFields[0] = tc_shaper_profile;
    inFields[1] = bucketSize;
    inFields[2] = maxRate;

    tc_shaper_profile++;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1],inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxQueueShaperProfileGet
*
* DESCRIPTION:
*       Get Token Bucket Shaper Profile for TC Queue per port.
*
*   APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*       portNum      - physical port number
*       tc           - traffic class queue
*
* OUTPUTS:
*       bucketSizePtr- (pointer to) bucket size (in units of 4K bytes)
*       maxRatePtr   - (pointer to) Actual Rate in Kbps
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
*      None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxQueueShaperProfileGet
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
    GT_U32 tc;
    GT_U32 bucketSize;
    GT_U32 maxRate;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    tc = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPortTxQueueShaperProfileGet(devNum, portNum, tc, &bucketSize, &maxRate);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", bucketSize, maxRate);



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxShaperParamsSet
*
* DESCRIPTION:
*       Set Global parameters for shaper.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum                - device number
*    xgPortsTokensRate    - tokens rate for XG ports' shapers
*    gigPortsTokensRate   - tokens rate for Tri Speed ports' shapers
*    gigPortsSlowRateRatio - slow rate ratio for Tri Speed ports. Tokens Update
*                            rate for ports with slow rate divided to the ratio.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       For devices the following values are valid.
*       xgPortsTokensRate    - <1-15>
*       gigPortsTokensRate   - <1-15>
*       gigPortsSlowRateRatio - <1-16>
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxShaperParamsSet
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
    xgPortsTokensRate = (GT_U32)inArgs[1];
    gigPortsTokensRate = (GT_U32)inArgs[2];
    gigPortsSlowRateRatio = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmPortTxShaperParamsSet(devNum, xgPortsTokensRate, gigPortsTokensRate, gigPortsSlowRateRatio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxShaperParamsGet
*
* DESCRIPTION:
*       Get Global parameters for shaper.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*    devNum                   - device number
*
* OUTPUTS:
*    xgPortsTokensRatePtr    - (pointer to) tokens rate for XG ports' shapers
*    gigPortsTokensRatePtr   - (pointer to) tokens rate for Tri Speed ports' shapers
*    gigPortsSlowRateRatioPtr - (pointer to) slow rate ratio for Tri Speed ports.
*                               Tokens Update rate for ports with slow rate divided
*                               to the ratio.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       For devices the following values are valid.
*       xgPortsTokensRate    - <1-15>
*       gigPortsTokensRate   - <1-15>
*       gigPortsSlowRateRatio - <1-16>
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxShaperParamsGet
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
    result = cpssExMxPmPortTxShaperParamsGet(devNum, &xgPortsTokensRate, &gigPortsTokensRate, &gigPortsSlowRateRatio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", xgPortsTokensRate, gigPortsTokensRate, gigPortsSlowRateRatio);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxToCpuShaperModeSet
*
* DESCRIPTION:
*       Set Shaper mode packet or byte based for CPU port shapers
*       Shapers are configured by cpssExMxPmPortTxShaperProfileSet or
*       cpssExMxPmPortTxQueueShaperProfileSet.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       mode    - shaper mode: byte count or packet number based ackets.
*                       CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E
*                       CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxToCpuShaperModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_PORT_TX_DROP_SHAPER_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortTxToCpuShaperModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxToCpuShaperModeGet
*
* DESCRIPTION:
*       Gets Shaper mode packet or byte based for CPU port shapers
*       Shapers are configured by cpssExMxPmPortTxShaperProfileSet or
*       cpssExMxPmPortTxQueueShaperProfileSet.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*
* OUTPUTS:
*       modePtr - (pointer to) shaper mode.
*                 Valid values:
*                       CPSS_PORT_TX_DROP_SHAPER_BYTE_MODE_E
*                       CPSS_PORT_TX_DROP_SHAPER_PACKET_MODE_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxToCpuShaperModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_DROP_SHAPER_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPortTxToCpuShaperModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmPortTxShaperTokenBucketMtuSet
*
* DESCRIPTION:
*       Set Token bucket maximum transmission unit (MTU).
*       The token bucket MTU defines the minimum number of tokens required to
*       permit a packet to be transmitted (i.e., conforming).
*
* APPLICABLE DEVICES:  All ExMxPm Devices.
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
static CMD_STATUS wrCpssExMxPmPortTxShaperTokenBucketMtuSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mtu = (CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortTxShaperTokenBucketMtuSet(devNum, mtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxShaperTokenBucketMtuGet
*
*       Get Token bucket maximum transmission unit (MTU).
*       The token bucket MTU defines the minimum number of tokens required to
*       permit a packet to be transmitted (i.e., conforming).
*
* APPLICABLE DEVICES:  All ExMxPm Devices.
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
static CMD_STATUS wrCpssExMxPmPortTxShaperTokenBucketMtuGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_PORT_TX_TOKEN_BUCKET_MTU_ENT mtu;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPortTxShaperTokenBucketMtuGet(devNum, &mtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mtu);

    

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxTailDropUcEnableSet
*
* DESCRIPTION:
*       Enable/Disable Tail Drop for all Unicast packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices
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
static CMD_STATUS wrCpssExMxPmPortTxTailDropUcEnableSet
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
    result = cpssExMxPmPortTxTailDropUcEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxTailDropUcEnableGet
*
* DESCRIPTION:
*       Get enable/disable Tail Drop status for all Unicast packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices
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
static CMD_STATUS wrCpssExMxPmPortTxTailDropUcEnableGet
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
    result = cpssExMxPmPortTxTailDropUcEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxBufferTailDropEnableSet
*
* DESCRIPTION:
*       Enable/disable Tail Drop according to the number of buffers in the
*       queues.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number.
*       enable   -  GT_TRUE - Enables Tail Drop according to the number of
*                             buffers in the queues.
*                   GT_FALSE - otherwise.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            on success.
*       GT_BAD_PARAM    - on invalid input paramters value
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxBufferTailDropEnableSet
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
    result = cpssExMxPmPortTxBufferTailDropEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxBufferTailDropEnableGet
*
* DESCRIPTION:
*       Get enable/disable Tail Drop status according to the number of buffers
*       in the queues.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number.
*
* OUTPUTS:
*       enablePtr   -  pointer to Tail Drop status:
*                         GT_TRUE  - Enables Tail Drop according to the number of
*                                    buffers in the queues.
*                         GT_FALSE - otherwise.
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
static CMD_STATUS wrCpssExMxPmPortTxBufferTailDropEnableGet
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
    result = cpssExMxPmPortTxBufferTailDropEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxTailDropProfileSet
*
* DESCRIPTION:
*       Sets tail drop profiles limits per TC.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devnum             - device number
*       index              - The tail drop profile index. range 0..7.
*       tc                 - the Traffic Class associated with this set of
*                            Drop Parameters.
*       tailDropProfileParamsPtr -
*                            (pointer to) the Drop Profile Parameters to
*                            associate with the Traffic Class in this Profile.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropProfileSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 tc;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS tailDropProfileParams;




    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;



    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tc = (GT_U8)inArgs[1];


    index = (GT_U32)inFields[0];
    tailDropProfileParams.dp0MaxBuffNum = (GT_U16)inFields[1];
    tailDropProfileParams.dp1MaxBuffNum = (GT_U16)inFields[2];
    tailDropProfileParams.dp2MaxBuffNum = (GT_U16)inFields[3];
    tailDropProfileParams.dp0MaxDescrNum = (GT_U16)inFields[4];
    tailDropProfileParams.dp1MaxDescrNum = (GT_U16)inFields[5];
    tailDropProfileParams.dp2MaxDescrNum = (GT_U16)inFields[6];




    /* call cpss api function */
    result = cpssExMxPmPortTxTailDropProfileSet(devNum, index, tc, &tailDropProfileParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
static GT_U8 indexTailDrop;
/*******************************************************************************
* cpssExMxPmPortTxTailDropProfileGet
*
* DESCRIPTION:
*       Gets tail drop profiles limits per TC.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devnum             - device number
*       index              - The tail drop profile index. range 0..7.
*       tc                 - the Traffic Class associated with this set of
*                            Drop Parameters.
*
* OUTPUTS:
*       tailDropProfileParamsPtr -
*                            (pointer to) the Drop Profile Parameters to
*                            associate with the Traffic Class in this Profile set.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropProfileGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
 /*   GT_U32 index;*/
    GT_U32 tc;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS tailDropProfileParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    tc = (GT_U8)inArgs[1];


    indexTailDrop=0;
    /* call cpss api function */
    result = cpssExMxPmPortTxTailDropProfileGet(devNum, indexTailDrop, tc, &tailDropProfileParams);

     if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
   inFields[0]=indexTailDrop;
   inFields[1]=tailDropProfileParams.dp0MaxBuffNum;
   inFields[2]=tailDropProfileParams.dp1MaxBuffNum;
   inFields[3]=tailDropProfileParams.dp2MaxBuffNum;
   inFields[4]=tailDropProfileParams.dp0MaxDescrNum;
   inFields[5]=tailDropProfileParams.dp1MaxDescrNum;
   inFields[6]=tailDropProfileParams.dp2MaxDescrNum;

   /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],inFields[3], inFields[4],inFields[5],
                                     inFields[6]);

   /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropProfileGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
 /*   GT_U32 index;*/
    GT_U32 tc;
    CPSS_PORT_TX_Q_TAIL_DROP_PROF_TC_PARAMS tailDropProfileParams;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

     /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    tc = (GT_U8)inArgs[1];


    indexTailDrop++;
     if(indexTailDrop >= 8)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    /* call cpss api function */
    result = cpssExMxPmPortTxTailDropProfileGet(devNum, indexTailDrop, tc, &tailDropProfileParams);

   if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

   inFields[0]=indexTailDrop;
   inFields[1]=tailDropProfileParams.dp0MaxBuffNum;
   inFields[2]=tailDropProfileParams.dp1MaxBuffNum;
   inFields[3]=tailDropProfileParams.dp2MaxBuffNum;
   inFields[4]=tailDropProfileParams.dp0MaxDescrNum;
   inFields[5]=tailDropProfileParams.dp1MaxDescrNum;
   inFields[6]=tailDropProfileParams.dp2MaxDescrNum;

   /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],inFields[3], inFields[4],inFields[5],
                                     inFields[6]);
   /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmPortTxProfileTailDropLimitsSet
*
* DESCRIPTION:
*       Set tail drop profiles limits per profile.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*   devNum           - device number
*   index            - Tail Drop Profile index. range 0..7.
*   portMaxBuffLimit - The maximum number of buffers that can be allocated to all
*                      ports' TC Tx queues. range 0..8191.
*   portMaxDescrLimit- The maximum number of descriptors that can be allocated to
*                      all ports' TC Tx queues. range 0..8191.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_OUT_OF_RANGE - on out of range value
*       GT_BAD_PARAM    - on illegal parameter
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*     The configuration affects per port tail drop limit.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxProfileTailDropLimitsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 portMaxBuffLimit;
    GT_U32 portMaxDescrLimit;
    CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ENT sharedPoolUsers;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    portMaxBuffLimit = (GT_U32)inArgs[2];
    portMaxDescrLimit = (GT_U32)inArgs[3];
    sharedPoolUsers=(CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmPortTxProfileTailDropLimitsSet(devNum, index, portMaxBuffLimit, portMaxDescrLimit,sharedPoolUsers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxProfileTailDropLimitsGet
*
* DESCRIPTION:
*       Gets tail drop profiles limits per profile.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*   devNum               - device number
*   index                - Tail Drop Profile index. range 0..7.
*
* OUTPUTS:
*   portMaxBuffLimitPtr  - (pointer to) The maximum number of buffers that can
*                          be allocated to all ports' TC Tx queues. range 0..8191.
*   portMaxDescrLimitPtr - (pointer to) The maximum number of descriptors that
*                          can be allocated to all ports' TC Tx queues. range 0..8191.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_OUT_OF_RANGE - on out of range value
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*     The configuration affects per port tail drop limit.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxProfileTailDropLimitsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 portMaxBuffLimit;
    GT_U32 portMaxDescrLimit;
    CPSS_EXMXPM_PORT_TX_SHARED_POOL_USERS_ENT sharedPoolUsers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortTxProfileTailDropLimitsGet(devNum, index, &portMaxBuffLimit, &portMaxDescrLimit,&sharedPoolUsers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", portMaxBuffLimit, portMaxDescrLimit,sharedPoolUsers);

    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPmPortTxTailDropE2eLimitsSet
*
* DESCRIPTION:
*       Sets End to End Flow Control configuration.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum              - device number
*       profileIndex        - Tail drop profile index. range 0..7.
*       xoffThresholdCfgPtr - E2E limits values.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropE2eLimitsV1Set
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
    CPSS_EXMXPM_PORT_TX_E2E_LIMIT_CONFIG_STC xoffThresholdCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


   /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    profileIndex=(GT_U32)inArgs[1];
    xoffThresholdCfg.dp0XoffThreshold=(GT_U32)inFields[0];
    xoffThresholdCfg.dp1XoffThreshold=(GT_U32)inFields[1];
    xoffThresholdCfg.dp2XoffThreshold=(GT_U32)inFields[2];
    /* call cpss api function */
    result = cpssExMxPmPortTxTailDropE2eLimitsSet(devNum, profileIndex, &xoffThresholdCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPmPortTxTailDropE2eLimitsGet
*
* DESCRIPTION:
*       Gets End to End Flow Control configuration.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum              - device number
*       profileIndex        - Tail drop profile index. range 0..7.
*
* OUTPUTS:
*       xoffThresholdCfgPtr - E2E limits values.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropE2eLimitsV1GetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 indexE2eLimitsV1;

 /*   GT_U32 profileIndex;*/
    CPSS_EXMXPM_PORT_TX_E2E_LIMIT_CONFIG_STC xoffThresholdCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    indexE2eLimitsV1 = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortTxTailDropE2eLimitsGet(
        devNum, indexE2eLimitsV1, &xoffThresholdCfg);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]=xoffThresholdCfg.dp0XoffThreshold;
    inFields[1]=xoffThresholdCfg.dp1XoffThreshold;
    inFields[2]=xoffThresholdCfg.dp2XoffThreshold;

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropE2eLimitsV1GetNext
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
*  cpssExMxPmPortTxTailDropE2eLimitsSet
*
* DESCRIPTION:
*       Sets End to End Flow Control configuration.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum              - device number
*       profileIndex        - Tail drop profile index. range 0..7.
*       xoffThresholdCfgPtr - E2E limits values.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropE2eLimitsSet
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
    CPSS_EXMXPM_PORT_TX_E2E_LIMIT_CONFIG_STC xoffThresholdCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;


   /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    profileIndex=(GT_U32)inFields[0];
    xoffThresholdCfg.dp0XoffThreshold=(GT_U32)inFields[1];
    xoffThresholdCfg.dp1XoffThreshold=(GT_U32)inFields[2];
    /* workaround on error in GUI - the same value */
    xoffThresholdCfg.dp2XoffThreshold=(GT_U32)inFields[2];
    /* call cpss api function */
    result = cpssExMxPmPortTxTailDropE2eLimitsSet(devNum, profileIndex, &xoffThresholdCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
static GT_U8 indexE2eLimits;
/*******************************************************************************
*  cpssExMxPmPortTxTailDropE2eLimitsGet
*
* DESCRIPTION:
*       Gets End to End Flow Control configuration.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum              - device number
*       profileIndex        - Tail drop profile index. range 0..7.
*
* OUTPUTS:
*       xoffThresholdCfgPtr - E2E limits values.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropE2eLimitsGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
 /*   GT_U32 profileIndex;*/
    CPSS_EXMXPM_PORT_TX_E2E_LIMIT_CONFIG_STC xoffThresholdCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    indexE2eLimits=0;
    /* call cpss api function */
    result = cpssExMxPmPortTxTailDropE2eLimitsGet(devNum, indexE2eLimits, &xoffThresholdCfg);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]=indexE2eLimits;
    inFields[1]=xoffThresholdCfg.dp0XoffThreshold;
    inFields[2]=xoffThresholdCfg.dp1XoffThreshold;
    inFields[3]=xoffThresholdCfg.dp2XoffThreshold;

    /* pack and output table fields */
    fieldOutput(
        "%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}
/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropE2eLimitsGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
 /*   GT_U32 profileIndex;*/
    CPSS_EXMXPM_PORT_TX_E2E_LIMIT_CONFIG_STC xoffThresholdCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    indexE2eLimits++;

    if(indexE2eLimits >= 8)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPmPortTxTailDropE2eLimitsGet(devNum, indexE2eLimits, &xoffThresholdCfg);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0]=indexE2eLimits;
    inFields[1]=xoffThresholdCfg.dp0XoffThreshold;
    inFields[2]=xoffThresholdCfg.dp1XoffThreshold;
    inFields[3]=xoffThresholdCfg.dp2XoffThreshold;

     /* pack and output table fields */
    fieldOutput(
        "%d%d%d", inFields[0], inFields[1], inFields[2]);
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPmPortTxTailDropProfileIndexSet
*
* DESCRIPTION:
*       Bind a Physical Port to a specific Drop profile entry.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*       index         - the Drop Parameters profile index. range 0..7.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropProfileIndexSet
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
    GT_U32 index;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    index = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxTailDropProfileIndexSet(devNum, portNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPmPortTxTailDropProfileIndexGet
*
* DESCRIPTION:
*       Gets DP profile index of a  Physical Port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*
* OUTPUTS:
*       indexPtr      - (pointer to) the Drop Parameters profile index. range 0..7.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxTailDropProfileIndexGet
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
    GT_U32 index;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxTailDropProfileIndexGet(devNum, portNum, &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", index);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet
*
* DESCRIPTION:
*       Set maximal descriptors limits for mirrored packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - device number
*       ingressMirroredLimitNum
*                         - The number of descriptors allocated for packets
*                           forwarded to the ingress analyzer port due to
*                           mirroring. range 0..8191.
*       egressMirroredLimitNum
*                         - The number of descriptors allocated for packets
*                           forwarded to the egress analyzer port due to
*                           mirroring. range 0..8191.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSniffedPacketDescriptorsLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ingressMirroredLimitNum;
    GT_U32 egressMirroredLimitNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ingressMirroredLimitNum = (GT_U32)inArgs[1];
    egressMirroredLimitNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet(devNum, ingressMirroredLimitNum, egressMirroredLimitNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet
*
* DESCRIPTION:
*       Gets maximal descriptors limits for mirrored packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum               - device number
*
* OUTPUTS:
*       ingressMirroredLimitPtr
*                           - (pointer to) The number of descriptors allocated
*                             for packets forwarded to the ingress analyzer port
*                             due to mirroring. range 0..8191.
*       engressMirroredLimitPtr
*                           - (pointer to) The number of descriptors allocated
*                             for packets forwarded to the egress analyzer port
*                             due to mirroring. range 0..8191.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSniffedPacketDescriptorsLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 ingressMirroredLimit;
    GT_U32 egressMirroredLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet(devNum, &ingressMirroredLimit, &egressMirroredLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", ingressMirroredLimit, egressMirroredLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxMcPacketDescriptorsLimitSet
*
* DESCRIPTION:
*       Set maximum descriptors limits for multicast packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum    - device number
*       mcLimit   - The number of descriptors allocated for multicast packets.
*                   range 0..32767.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_OUT_OF_RANGE - on out of range value
*       GT_BAD_PARAM    - on illegal parameter
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxMcPacketDescriptorsLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mcLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mcLimit = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortTxMcPacketDescriptorsLimitSet(devNum, mcLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxMcPacketDescriptorsLimitGet
*
* DESCRIPTION:
*       Gets maximum descriptors limits for multicast packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum       - device number
*
* OUTPUTS:
*       mcLimitPtr   - (pointer to) The number of descriptors allocated
*                      for multicast packets. range 0..32767.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxMcPacketDescriptorsLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 mcLimit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPortTxMcPacketDescriptorsLimitGet(devNum, &mcLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mcLimit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSchedulerConfigSet
*
* DESCRIPTION:
*       Set global parameters for WRR scheduler.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum  - device number
*       wrrMode - Transmit Scheduler count mode.
*       wrrMtu  - MTU for SDWRR, resolution of WRR weights for byte based
*                 Transmit Scheduler count mode.
*                 Valid values:
*                       CPSS_PORT_TX_WRR_MTU_2K_E
*                       CPSS_PORT_TX_WRR_MTU_8K_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_WRR_MODE_ENT wrrMode;
    CPSS_PORT_TX_WRR_MTU_ENT wrrMtu;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    wrrMode = (CPSS_PORT_TX_WRR_MODE_ENT)inArgs[1];
    wrrMtu = (CPSS_PORT_TX_WRR_MTU_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerConfigSet(devNum, wrrMode, wrrMtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSchedulerConfigGet
*
* DESCRIPTION:
*       Gets global parameters for WRR scheduler.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       wrrModePtr - (pointer to) Transmit Scheduler count mode.
*       wrrMtuPtr  - (pointer to) MTU for SDWRR, resolution of WRR weights for
*                    byte based Transmit Scheduler count mode.
*                    Valid values:
*                           CPSS_PORT_TX_WRR_MTU_2K_E
*                           CPSS_PORT_TX_WRR_MTU_8K_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_PORT_TX_WRR_MODE_ENT wrrMode;
    CPSS_PORT_TX_WRR_MTU_ENT wrrMtu;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerConfigGet(devNum, &wrrMode, &wrrMtu);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", wrrMode, wrrMtu);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSchedulerProfileIndexSet
*
* DESCRIPTION:
*       Bind a port to scheduler profile set.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       portNum    - physical port number
*       index      - The scheduler profile index. range 0..7.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerProfileIndexSet
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
    GT_U32 index;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    index = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerProfileIndexSet(devNum, portNum, index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSchedulerProfileIndexGet
*
* DESCRIPTION:
*       Get scheduler profile set that is binded to the port.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum        - device number
*       portNum       - physical port number
*
* OUTPUTS:
*       indexPtr      - (pointer to) The scheduler profile index. range 0..7.
*
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerProfileIndexGet
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
    GT_U32 index;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerProfileIndexGet(devNum, portNum, &index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", index);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmPortTxSchedulerWrrProfileSet
*
* DESCRIPTION:
*       Sets Weighted Round Robin (WRR) weight per profile.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       index      - the Tx Queue scheduler Profile Set index to which the
*                    wrrWeight Parameter is associated.
*       tc         - traffic class queue
*       wrrWeight  - proportion of bandwidth assigned to this queue
*                    relative to the other queues in this
*                    Arbitration Group - range 0..255 .
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*         None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerWrrProfileSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 tc;
    GT_U32 wrrWeight;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    tc = (GT_U32)inArgs[2];
    wrrWeight = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerWrrProfileSet(devNum, index, tc, wrrWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSchedulerWrrProfileGet
*
* DESCRIPTION:
*       Gets Weighted Round Robin (WRR) weight per profile.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       index      - the Tx Queue scheduler Profile Set index to which the
*                    wrrWeight Parameter is associated.
*       tc         - traffic class queue
* OUTPUTS:
*       wrrWeightPtr
*                  - (pointer to) proportion of bandwidth assigned to this queue
*                    relative to the other queues in this
*                    Arbitration Group - range 0..255 .
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*         None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerWrrProfileGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 tc;
    GT_U32 wrrWeight;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    tc = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerWrrProfileGet(devNum, index, tc, &wrrWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", wrrWeight);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSchedulerWrrProfileSet
*
* DESCRIPTION:
*       Sets Weighted Round Robin (WRR) weight per profile.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       index      - the Tx Queue scheduler Profile Set index to which the
*                    wrrWeight Parameter is associated.
*       tc         - traffic class queue
*       wrrWeight  - proportion of bandwidth assigned to this queue
*                    relative to the other queues in this
*                    Arbitration Group - range 0..255 .
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*         None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerWrrProfileSetTable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 tc;
    GT_U32 wrrWeight;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    tc = (GT_U32)inFields[0];
    wrrWeight = (GT_U32)inFields[1];

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerWrrProfileSet(devNum, index, tc, wrrWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSchedulerWrrProfileGet
*
* DESCRIPTION:
*       Gets Weighted Round Robin (WRR) weight per profile.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       index      - the Tx Queue scheduler Profile Set index to which the
*                    wrrWeight Parameter is associated.
*       tc         - traffic class queue
* OUTPUTS:
*       wrrWeightPtr
*                  - (pointer to) proportion of bandwidth assigned to this queue
*                    relative to the other queues in this
*                    Arbitration Group - range 0..255 .
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*         None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerWrrProfileGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 wrrWeight;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    tc = 0;

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerWrrProfileGet(devNum, index, tc, &wrrWeight);

    inFields[0] = tc;
    inFields[1] = wrrWeight;

    tc++;

    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerWrrProfileGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 wrrWeight;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (tc > 7)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);

        return CMD_OK;
    }

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerWrrProfileGet(devNum, index, tc, &wrrWeight);

    inFields[0] = tc;
    inFields[1] = wrrWeight;

    tc++;

    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSchedulerArbitrationGroupSet
*
* DESCRIPTION:
*       Set Traffic Class Queue scheduling arbitration group profile.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       index      - The scheduler profile index. range 0..7.
*       tc         - traffic class queue
*       arbGroup   - scheduling arbitration group:
*                     1) WRR Group 0        - CPSS_PORT_TX_WRR_ARB_GROUP_0_E
*                     2) WRR Group 1        - CPSS_PORT_TX_WRR_ARB_GROUP_1_E
*                     3) Strict Priority    - CPSS_PORT_TX_SP_ARB_GROUP_E
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerArbitrationGroupSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    GT_U32 tc;
    CPSS_PORT_TX_Q_ARB_GROUP_ENT arbGroup;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];
    tc = (GT_U32)inFields[0];
    arbGroup = (CPSS_PORT_TX_Q_ARB_GROUP_ENT)inFields[1];

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerArbitrationGroupSet(devNum, index, tc, arbGroup);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortTxSchedulerArbitrationGroupGet
*
* DESCRIPTION:
*       Gets Traffic Class Queue scheduling arbitration group per profile.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - device number
*       index      - The scheduler profile index. range 0..7.
*       tc         - traffic class queue
*
* OUTPUTS:
*       arbGroupPtr
*                  - (pointer to) Scheduling arbitration group:
*                     1) WRR Group 0        - CPSS_PORT_TX_WRR_ARB_GROUP_0_E
*                     2) WRR Group 1        - CPSS_PORT_TX_WRR_ARB_GROUP_1_E
*                     3) Strict Priority    - CPSS_PORT_TX_SP_ARB_GROUP_E
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static GT_U32 tc_sched;

static CMD_STATUS wrCpssExMxPmPortTxSchedulerArbitrationGroupGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    CPSS_PORT_TX_Q_ARB_GROUP_ENT arbGroup;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    tc_sched = 0;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerArbitrationGroupGet(devNum, index, tc_sched, &arbGroup);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    inFields[0] = tc_sched;
    inFields[1] = (CPSS_PORT_TX_Q_ARB_GROUP_ENT)arbGroup;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    tc_sched++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSchedulerArbitrationGroupGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 index;
    CPSS_PORT_TX_Q_ARB_GROUP_ENT arbGroup;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    index = (GT_U32)inArgs[1];

    if(tc_sched == 8)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;

    }
    /* call cpss api function */
    result = cpssExMxPmPortTxSchedulerArbitrationGroupGet(devNum, index, tc_sched, &arbGroup);

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    inFields[0] = tc_sched;
    inFields[1] = (CPSS_PORT_TX_Q_ARB_GROUP_ENT)arbGroup;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    tc_sched++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPmPortTxSharingResourcesEnableSet
*
* DESCRIPTION:
*       Globally enables/disables sharing of resources for enqueuing of packets.
*       The shared resources configuration is set by
*       cpssExMxPmPortTxSharedResourcesLimitsSet
*
* APPLICABLE DEVICES:  All ExMxPM devices
*
* INPUTS:
*       devNum  - device number
*       enable  - GT_TRUE   - enable TX queue resourses sharing
*                 GT_FALSE  - disable TX queue resources sharing
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSharingResourcesEnableSet
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
    result = cpssExMxPmPortTxSharingResourcesEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPmPortTxSharingResourcesEnableGet
*
* DESCRIPTION:
*       Gets enabled/disabled sharing status of resources for enqueuing of packets.
*
* APPLICABLE DEVICES:  All ExMxPM devices
*
* INPUTS:
*       devNum     - device number
*
* OUTPUTS:
*       enablePtr  - (pointer to) GT_TRUE  - TX queue resourses sharing enabled
*                                 GT_FALSE - TX queue resources sharing disabled
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSharingResourcesEnableGet
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
    result = cpssExMxPmPortTxSharingResourcesEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPmPortTxSharedResourcesLimitsSet
*
* DESCRIPTION:
*       Global configuration of shared resources for enqueuing of packets.
*       The using of shared resources is set by
*       cpssExMxPmPortTxSharingResourcesEnableSet
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum             - device number
*       sharedBufLimit     - The number of buffers in all Transmit queues that
*                            can be shared between eligible packets (value 0..8191).
*       sharedDescLimit    - The number of descriptors that can be shared
*                            between all eligible packets (value 0..8191).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK        - on success
*       GT_FAIL      - on error
*       GT_BAD_PARAM - on illegal parameter
*       GT_HW_ERROR  - on hardware error
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSharedResourcesLimitsSet
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
    sharedBufLimit = (GT_U32)inArgs[1];
    sharedDescLimit = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmPortTxSharedResourcesLimitsSet(devNum, sharedBufLimit, sharedDescLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPmPortTxSharedResourcesLimitsGet
*
* DESCRIPTION:
*       Gets configuration of shared resources for enqueuing of packets.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       sharedBufLimitPtr   - (pointer to) The number of buffers in all Transmit
*                             queues that can be shared between eligible packets.
*       sharedDescLimitPtr  - (pointer to) The number of descriptors that can be
*                             shared between all eligible packets.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on illegal parameter
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxSharedResourcesLimitsGet
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
    result = cpssExMxPmPortTxSharedResourcesLimitsGet(devNum, &sharedBufLimit, &sharedDescLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", sharedBufLimit, sharedDescLimit);

    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPmPortTxRandomTailDropMasksSet
*
* DESCRIPTION:
*       Setting the masks used for calculating the limits when random
*       tail drop algorithm is activated.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum                  - device number
*       randomTailDropMasksPtr  - (pointer to) tail drop threshold masks structure.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - NULL pointer
*       GT_OUT_OF_RANGE          - on out of range value
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxRandomTailDropMasksSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_PORT_TX_RANDOM_TAIL_DROP_MASKS_STC randomTailDropMasks;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    randomTailDropMasks.globalRandomTailDropMask = (GT_U32)inArgs[1];
    randomTailDropMasks.portRandomTailDropMask = (GT_U32)inArgs[2];
    randomTailDropMasks.priorityRandomTailDropMask = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmPortTxRandomTailDropMasksSet(devNum, &randomTailDropMasks);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");



    return CMD_OK;
}

/*******************************************************************************
*  cpssExMxPmPortTxRandomTailDropMasksGet
*
* DESCRIPTION:
*       Getting the masks used for calculating the limits when random
*       tail drop algorithm is activated.
*
* APPLICABLE DEVICES:  All ExMxPm devices.
*
* INPUTS:
*       devNum              - device number
*
* OUTPUTS:
*       randomTailDropMasksPtr - (pointer to) tail drop threshold masks structure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on illegal parameter
*       GT_BAD_PTR               - NULL pointer
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortTxRandomTailDropMasksGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_PORT_TX_RANDOM_TAIL_DROP_MASKS_STC randomTailDropMasks;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPortTxRandomTailDropMasksGet(devNum, &randomTailDropMasks);

    /* pack and output table fields */
    galtisOutput(outArgs, result, "%d%d%d",
                 randomTailDropMasks.globalRandomTailDropMask,
                 randomTailDropMasks.portRandomTailDropMask,
                 randomTailDropMasks.priorityRandomTailDropMask);

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmPortTxInit",
         &wrCpssExMxPmPortTxInit,
         1, 0},
        {"cpssExMxPmPortTxQueueBlockEnableSet",
         &wrCpssExMxPmPortTxQueueBlockEnableSet,
         2, 0},
        {"cpssExMxPmPortTxQueueBlockEnableGet",
         &wrCpssExMxPmPortTxQueueBlockEnableGet,
         1, 0},
        {"cpssExMxPmPortTxQueueingEnableSet",
         &wrCpssExMxPmPortTxQueueingEnableSet,
         2, 2},
        {"cpssExMxPmPortTxQueueingEnableGetFirst",
         &wrCpssExMxPmPortTxQueueingEnableGetFirst,
         2, 0},
        {"cpssExMxPmPortTxQueueingEnableGetNext",
         &wrCpssExMxPmPortTxQueueingEnableGetNext,
         2, 0},
        {"cpssExMxPmPortTxQueueTxEnableSet",
         &wrCpssExMxPmPortTxQueueTxEnableSet,
         2, 2},
        {"cpssExMxPmPortTxQueueTxEnableGetFirst",
         &wrCpssExMxPmPortTxQueueTxEnableGetFirst,
         2, 0},
        {"cpssExMxPmPortTxQueueTxEnableGetNext",
         &wrCpssExMxPmPortTxQueueTxEnableGetNext,
         2, 0},
        {"cpssExMxPmPortTxQueueTxEnableGet",
         &wrCpssExMxPmPortTxQueueTxEnableGet,
         3, 0},
        {"cpssExMxPmPortTxQueuesFlush",
         &wrCpssExMxPmPortTxQueuesFlush,
         2, 0},
        {"cpssExMxPmPortTxShaperEnableSet",
         &wrCpssExMxPmPortTxShaperEnableSet,
         3, 0},
        {"cpssExMxPmPortTxShaperEnableGet",
         &wrCpssExMxPmPortTxShaperEnableGet,
         2, 0},
        {"cpssExMxPmPortTxShaperProfileSet",
         &wrCpssExMxPmPortTxShaperProfileSet,
         4, 0},
        {"cpssExMxPmPortTxShaperProfileGet",
         &wrCpssExMxPmPortTxShaperProfileGet,
         2, 0},
        {"cpssExMxPmPortTxQueueShaperEnableSet",
         &wrCpssExMxPmPortTxQueueShaperEnableSet,
         2, 2},
        {"cpssExMxPmPortTxQueueShaperEnableGetFirst",
         &wrCpssExMxPmPortTxQueueShaperEnableGetFirst,
         2, 0},
        {"cpssExMxPmPortTxQueueShaperEnableGetNext",
         &wrCpssExMxPmPortTxQueueShaperEnableGetNext,
         2, 0},
        {"cpssExMxPmPortTxQueueShaperEnableGet",
         &wrCpssExMxPmPortTxQueueShaperEnableGet,
         3, 0},
        {"cpssExMxPmPortTxQueueShaperProfileSet",
         &wrCpssExMxPmPortTxQueueShaperProfileSet,
         2, 3},
        {"cpssExMxPmPortTxQueueShaperProfileGetFirst",
         &wrCpssExMxPmPortTxQueueShaperProfileGetFirst,
         2, 0},
        {"cpssExMxPmPortTxQueueShaperProfileGetNext",
         &wrCpssExMxPmPortTxQueueShaperProfileGetNext,
         2, 0},
        {"cpssExMxPmPortTxQueueShaperProfileGet",
         &wrCpssExMxPmPortTxQueueShaperProfileGet,
         3, 0},
        {"cpssExMxPmPortTxShaperParamsSet",
         &wrCpssExMxPmPortTxShaperParamsSet,
         4, 0},
        {"cpssExMxPmPortTxShaperParamsGet",
         &wrCpssExMxPmPortTxShaperParamsGet,
         1, 0},
        {"cpssExMxPmPortTxToCpuShaperModeSet",
         &wrCpssExMxPmPortTxToCpuShaperModeSet,
         2, 0},
        {"cpssExMxPmPortTxToCpuShaperModeGet",
         &wrCpssExMxPmPortTxToCpuShaperModeGet,
         1, 0},
        {"cpssExMxPmPortTxShaperTokenBucketMtuSet",
         &wrCpssExMxPmPortTxShaperTokenBucketMtuSet,
         2, 0},
        {"cpssExMxPmPortTxShaperTokenBucketMtuGet",
         &wrCpssExMxPmPortTxShaperTokenBucketMtuGet,
         1, 0},
        {"cpssExMxPmPortTxTailDropUcEnableSet",
         &wrCpssExMxPmPortTxTailDropUcEnableSet,
         2, 0},
        {"cpssExMxPmPortTxTailDropUcEnableGet",
         &wrCpssExMxPmPortTxTailDropUcEnableGet,
         1, 0},
        {"cpssExMxPmPortTxBufferTailDropEnableSet",
         &wrCpssExMxPmPortTxBufferTailDropEnableSet,
         2, 0},
        {"cpssExMxPmPortTxBufferTailDropEnableGet",
         &wrCpssExMxPmPortTxBufferTailDropEnableGet,
         1, 0},
        {"cpssExMxPmPortTxTailDropProfileSet",
         &wrCpssExMxPmPortTxTailDropProfileSet,
         2, 7},
        {"cpssExMxPmPortTxTailDropProfileGetFirst",
         &wrCpssExMxPmPortTxTailDropProfileGetFirst,
         2, 0},
        {"cpssExMxPmPortTxTailDropProfileGetNext",
         &wrCpssExMxPmPortTxTailDropProfileGetNext,
         2, 0},
        {"cpssExMxPm2PortTxProfileTailDropLimitsSet",
         &wrCpssExMxPmPortTxProfileTailDropLimitsSet,
         5, 0},
        {"cpssExMxPm2PortTxProfileTailDropLimitsGet",
         &wrCpssExMxPmPortTxProfileTailDropLimitsGet,
         2, 0},
        {"cpssExMxPmPortTxTailDropE2eLimitsSet",
         &wrCpssExMxPmPortTxTailDropE2eLimitsSet,
         1, 3},
        {"cpssExMxPmPortTxTailDropE2eLimitsGetFirst",
         &wrCpssExMxPmPortTxTailDropE2eLimitsGetFirst,
         1, 0},
         {"cpssExMxPmPortTxTailDropE2eLimitsGetNext",
         &wrCpssExMxPmPortTxTailDropE2eLimitsGetNext,
         1, 0},

         {"cpssExMxPmPortTxTailDropE2eLimitsSet_V1Set",
          &wrCpssExMxPmPortTxTailDropE2eLimitsV1Set,
          2, 3},
         {"cpssExMxPmPortTxTailDropE2eLimitsSet_V1GetFirst",
          &wrCpssExMxPmPortTxTailDropE2eLimitsV1GetFirst,
          2, 0},
          {"cpssExMxPmPortTxTailDropE2eLimitsSet_V1GetNext",
          &wrCpssExMxPmPortTxTailDropE2eLimitsV1GetNext,
          2, 0},

        {"cpssExMxPmPortTxTailDropProfileIndexSet",
         &wrCpssExMxPmPortTxTailDropProfileIndexSet,
         3, 0},
        {"cpssExMxPmPortTxTailDropProfileIndexGet",
         &wrCpssExMxPmPortTxTailDropProfileIndexGet,
         2, 0},
        {"cpssExMxPmPortTxSniffedPacketDescriptorsLimitSet",
         &wrCpssExMxPmPortTxSniffedPacketDescriptorsLimitSet,
         3, 0},
        {"cpssExMxPmPortTxSniffedPacketDescriptorsLimitGet",
         &wrCpssExMxPmPortTxSniffedPacketDescriptorsLimitGet,
         1, 0},
        {"cpssExMxPmPortTxMcPacketDescriptorsLimitSet",
         &wrCpssExMxPmPortTxMcPacketDescriptorsLimitSet,
         2, 0},
        {"cpssExMxPmPortTxMcPacketDescriptorsLimitGet",
         &wrCpssExMxPmPortTxMcPacketDescriptorsLimitGet,
         1, 0},
        {"cpssExMxPmPortTxSchedulerConfigSet",
         &wrCpssExMxPmPortTxSchedulerConfigSet,
         3, 0},
        {"cpssExMxPmPortTxSchedulerConfigGet",
         &wrCpssExMxPmPortTxSchedulerConfigGet,
         1, 0},
        {"cpssExMxPmPortTxSchedulerProfileIndexSet",
         &wrCpssExMxPmPortTxSchedulerProfileIndexSet,
         3, 0},
        {"cpssExMxPmPortTxSchedulerProfileIndexGet",
         &wrCpssExMxPmPortTxSchedulerProfileIndexGet,
         2, 0},

        /* TxSchedulerWrrProfile commands */
        {"cpssExMxPmPortTxSchedulerWrrProfileSet",
         &wrCpssExMxPmPortTxSchedulerWrrProfileSet,
         4, 0},
        {"cpssExMxPmPortTxSchedulerWrrProfileGet",
         &wrCpssExMxPmPortTxSchedulerWrrProfileGet,
         3, 0},
        /* TxSchedulerWrrProfile table */
        {"cpssExMxPmPortTxSchedulerWrrProfile_1Set",
         &wrCpssExMxPmPortTxSchedulerWrrProfileSetTable,
         2, 2},
        {"cpssExMxPmPortTxSchedulerWrrProfile_1GetFirst",
         &wrCpssExMxPmPortTxSchedulerWrrProfileGetFirst,
         2, 0},
        {"cpssExMxPmPortTxSchedulerWrrProfile_1GetNext",
         &wrCpssExMxPmPortTxSchedulerWrrProfileGetNext,
         2, 0},


        {"cpssExMxPmPortTxSchedArbitrationGroupSet",
         &wrCpssExMxPmPortTxSchedulerArbitrationGroupSet,
         2, 2},
        {"cpssExMxPmPortTxSchedArbitrationGroupGetFirst",
         &wrCpssExMxPmPortTxSchedulerArbitrationGroupGetFirst,
         2, 0},
        {"cpssExMxPmPortTxSchedArbitrationGroupGetNext",
         &wrCpssExMxPmPortTxSchedulerArbitrationGroupGetNext,
         2, 0},
        {"cpssExMxPmPortTxSharingResourcesEnableSet",
         &wrCpssExMxPmPortTxSharingResourcesEnableSet,
         2, 0},
        {"cpssExMxPmPortTxSharingResourcesEnableGet",
         &wrCpssExMxPmPortTxSharingResourcesEnableGet,
         1, 0},
        {"cpssExMxPmPortTxSharedResourcesLimitsSet",
         &wrCpssExMxPmPortTxSharedResourcesLimitsSet,
         3, 0},
        {"cpssExMxPmPortTxSharedResourcesLimitsGet",
         &wrCpssExMxPmPortTxSharedResourcesLimitsGet,
         1, 0},
        {"cpssExMxPmPortTxRandomTailDropMasksSet",
         &wrCpssExMxPmPortTxRandomTailDropMasksSet,
         4, 0},
        {"cpssExMxPmPortTxRandomTailDropMasksGet",
         &wrCpssExMxPmPortTxRandomTailDropMasksGet,
         1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmPortTx
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
GT_STATUS cmdLibInitCpssExMxPmPortTx
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

