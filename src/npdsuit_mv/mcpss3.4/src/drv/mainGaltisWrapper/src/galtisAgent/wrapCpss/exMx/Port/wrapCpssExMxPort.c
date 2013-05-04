/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapPortCpss.c
*
* DESCRIPTION:
*       Wrapper functions for Port cpss functions
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
#include <cpss/exMx/exMxGen/port/cpssExMxPortBackEnd.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortBufMg.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortCtrl.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortStat.h>
#include <cpss/exMx/exMxGen/port/cpssExMxPortTx.h>
/*#include <galtisAgent/wrapCpss/exMx/wrapPortCpss.h>*/

/*******************************************************************************
* cpssExMxPortBackEndModeEnable
*
* DESCRIPTION:
*       Enable/Disable Back-End mode on a given port
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev           - physical device number
*       port          - physical port number
*       backEndEnable - GT_TRUE, Back-end mode is enabled on the port
*                       GT_FALSE, Back-end mode is disabled on the port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       This function sets the backend port to use the BackEnd Ethertype
*       when transmitting packets.
*
*       was coreExMxPortBackEndModeEnable
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortBackEndModeEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U8  port;
    GT_BOOL backEndEnable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	backEndEnable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortBackEndModeEnable(dev, port, backEndEnable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortXonLimitSet
*
* DESCRIPTION:
*       Sets X-ON port limit. When Flow Control is enabled, X-ON packet sent
*       when the number of buffers for this port is less than xonLimit.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - physical device number
*       port     - physical port number
*       xonLimit - X-ON limit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortXonLimitSet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortXonLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U8  port;
    GT_U16 xonLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	xonLimit = (GT_U16)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortXonLimitSet(dev, port, xonLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPortXoffLimitSet
*
* DESCRIPTION:
*       Sets X-OFF port limit. When Flow Control is enabled, X-OFF packet sent
*       when the number of buffers for this port is less than xoffLimit.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       port      - physical port number
*       xoffLimit - X-OFF limit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortXoffLimitSet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortXoffLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U8  port;
    GT_U16 xoffLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	xoffLimit = (GT_U16)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortXoffLimitSet(dev, port, xoffLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortRxBufLimitSet
*
* DESCRIPTION:
*       Sets the maximum Rx buffer allocated for the specified port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - physical device number
*       port        - physical port number
*       rxBufLimit  - limit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortRxBufLimitSet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortRxBufLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U8  port;
    GT_U16 rxBufLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	rxBufLimit = (GT_U16)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortRxBufLimitSet(dev, port, rxBufLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
*  cpssExMxPortBufMngMaxBufSet
*
* DESCRIPTION:
*       Sets the total number of buffers used per device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev          - physical device number
*       maxBuffers   - maximum number of buffers
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong device number
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxBufMngMaxBufSet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortBufMngMaxBufSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U16 maxBuffers;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	maxBuffers = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPortBufMngMaxBufSet(dev, maxBuffers);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPortBufMngLinesLimitSet
*
* DESCRIPTION:
*       Sets the number of lines in the buffer management bitmap for long
*       buffers. Each line corresponds to 128 descriptors.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev         - physical device number
*       linesLimit  - lines limit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong device number
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxBufMngLinesLimitSet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortBufMngLinesLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U16 linesLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	linesLimit = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPortBufMngLinesLimitSet(dev, linesLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortCpuRxBufLimitSet
*
* DESCRIPTION:
*       Sets receive buffer limit threshold for the CPU.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev        - physical device number
*       rxBufLimit - limit
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong device number
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxCpuRxBufLimitSet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortCpuRxBufLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U16 rxBufLimit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	rxBufLimit = (GT_U16)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPortCpuRxBufLimitSet(dev, rxBufLimit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortBufModeGet
*
* DESCRIPTION:
*       Gets buffer mode.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum  - physical device number
*
* OUTPUTS:
*       bufModePtr - pointer to buffer mode.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong device number
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxBufModeGet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortBufModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    CPSS_BUF_MODE_ENT bufModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPortBufModeGet(dev, &bufModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", bufModePtr);
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPortCpuRxBufCountGet
*
* DESCRIPTION:
*       Gets receive buffer count for the CPU on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev  - The device to get the Cpu Rx buffer count for.
*
* OUTPUTS:
*       cpuRxBufCntPtr - pointer to number of buffers allocated for the CPU.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong device number
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxCpuRxBufCountGet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortCpuRxBufCountGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
    GT_U16 cpuRxBufCntPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPortCpuRxBufCountGet(dev, &cpuRxBufCntPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cpuRxBufCntPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortBufMgInit
*
* DESCRIPTION:
*       Init port buffer management facility.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev - physical device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK             - on success
*       GT_BAD_PARAM      - on wrong device number
*       GT_HW_ERROR       - on hardware error
*       GT_OUT_OF_CPU_MEM - on memory allocation failure
*       GT_FAIL           - otherwise
*
* COMMENTS:
*       was coreExMxPortBufMgInit.
*       This API must be called before any other call to port buffer management
*       related function.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortBufMgInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPortBufMgInit(dev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortMacSaLsbSet
*
* DESCRIPTION:
*       Set the least significant byte of the MAC SA of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - physical device number
*       port     - physical port number
*       macSaLsb - The ls byte of the MAC SA
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxSetPortMacSaLsb.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortMacSaLsbSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_U8   macSaLsb;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	macSaLsb = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortMacSaLsbSet(dev, port, macSaLsb);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortEnable
*
* DESCRIPTION:
*       Enable/disable a specified port on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number, CPU port number
*       state - GT_TRUE:  enable port,
*               GT_FALSE: disable port.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortEnable(dev, port, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortEnableGet
*
* DESCRIPTION:
*       Get enabling status of a specified port on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       statePtr -

* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetPortEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   stateptr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortEnableGet(dev, port, &stateptr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", stateptr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortDuplexModeSet
*
* DESCRIPTION:
*       Sets duplex mode for specified port on specified device.
*       Note that when duplex auto-negotiation is enabled the port's duplex mode
*       is resolved via auto-negotiation.
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       dMode - duplex mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported duplex mode
*
* COMMENTS:
*       was coreExMxSetPortDuplexMode.
*
*       For XGMII (10 Gbps) ports only Full duplex mode supported
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortDuplexModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_DUPLEX_ENT  dMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	dMode = (CPSS_PORT_DUPLEX_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortDuplexModeSet(dev, port, dMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPortDuplexModeGet
*
* DESCRIPTION:
*       Gets duplex mode for specified port on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       dModePtr - duplex mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetPortDuplexMode.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortDuplexModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_DUPLEX_ENT   dModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortDuplexModeGet(dev, port, &dModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dModePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortSpeedSet
*
* DESCRIPTION:
*       Sets speed for specified port on specified device.
*       Note that when auto-negotiation is enabled the speed is resolved by the
*       external 10/100/1000Base-T PHY and updated in the relevant register.
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       speed - port speed
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*       GT_NOT_SUPPORTED - on not supported speed
*
* COMMENTS:
*       was coreExMxSetPortSpeed.
*
*       For XGMII (10 Gbps) ports only CPSS_PORT_SPEED_10000_E supported
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortSpeedSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_SPEED_ENT	speed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	speed = (CPSS_PORT_SPEED_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortSpeedSet(dev, port, speed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortSpeedGet
*
* DESCRIPTION:
*       Gets speed for specified port on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       speedPtr - actual port speed
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetPortSpeed.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortSpeedGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_SPEED_ENT   speedPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortSpeedGet(dev, port, &speedPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", speedPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortAttributesOnPortGet
*
* DESCRIPTION:
*       Gets port attributes for particular port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - device number
*       port    - port number
*
* OUTPUTS:
*       portAttributesPtr     - attributes values.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetPortAttributesOnPort.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortAttributesOnPortGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_ATTRIBUTES_STC   portAttributesPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortAttributesOnPortGet(dev, port, &portAttributesPtr);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", portAttributesPtr.portLinkUp/*(GT_BOOL)*/,
		                                    portAttributesPtr.portSpeed/*(CPSS_PORT_SPEED_ENT)*/,
											portAttributesPtr.portDuplexity/*(CPSS_PORT_DUPLEX_ENT)*/);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortDuplexAutoNegEnable
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for duplex mode on specified port on
*       specified device.
*       When duplex Auto-Negotiation is enabled, the port's duplex mode is
*       resolved via Auto-Negotiation. The Auto-Negotiation advertised duplex
*       mode is full-duplex.
*       When duplex Auto-Negotiation is disabled, the port's duplex mode is set
*       via cpssExMxPortDuplexModeSet
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       state - GT_TRUE for enable Auto-Negotiation for duplex mode,
*               GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported state
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortDuplexAutoNegEnable.
*
*       For XGMII (10 Gbps) ports auto negotiation for duplex not supported
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortDuplexAutoNegEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortDuplexAutoNegEnable(dev, port, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortFlowCntrlAutoNegEnable
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation for Flow Control on specified port
*       on specified device.
*       When Flow Control is enabled and a Flow Control packet is received with
*       a non-zero timer, the MAC pauses the transmit packet stream.
*       In the opposite direction, the MAC sends Flow Control Pause packets when
*       the port buffer allocation count reaches the upper watermark threshold
*       (XOFF threshold). Flow Control pause frames are sent requesting the
*       remote node to stop sending frames (i.e., maximum pause time). To
*       prevent packet loss, set the XOFF threshold to ensure that enough
*       buffers are available to absorb the packets that are "in progress",
*       until the remote node ceases to transmit. When the buffer allocation
*       count goes back down to the lower watermark threshold (XON threshold),
*       a pause frame with a zero pause-time is sent that requests the remote
*       node to resume sending frames.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev            - physical device number
*       port           - physical port number
*       state          - GT_TRUE for enable Auto-Negotiation for Flow Control
*                        GT_FALSE otherwise
*       pauseAdvertise - Advertise symmetric flow control support in
*                        Auto-Negotiation.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported feature for a port
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortFlowCntrlAutoNegEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortFlowCntrlAutoNegEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   state;
	GT_BOOL   pauseAdvertise;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	state = (GT_BOOL)inArgs[2];
	pauseAdvertise = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortFlowCntrlAutoNegEnable(dev, port, state, pauseAdvertise);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortSpeedAutoNegEnable
*
* DESCRIPTION:
*       Enable/disable an Auto-Negotiation of interface speed on specified port
*       on specified device.
*       When speed Auto-Negotiation is enabled, the speed is resolved by the
*       external 10/100/1000BASE-T PHY and updated in the relevant register.
*       When speed Auto-Negotiation is disabled, the port's speed is configured
*       per port via cpssExMxPortSpeedSet.
*       The function does not configure the PHY connected to the port. It
*       configures only the MAC layer of the port.
*
* APPLICABLE DEVICES:  All ExMx devices except:
*       98EX130D, 98EX135D, 98MX635,
*       98MX638, 98EX136, 98EX136DI
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       state - GT_TRUE for enable Auto-Negotiation of interface speed,
*               GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported state
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortSpeedAutoNegEnable.
*
*       For XGMII (10 Gbps) ports auto negotiation for speed not supported
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortSpeedAutoNegEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortSpeedAutoNegEnable(dev, port, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortFlowControlEnable
*
* DESCRIPTION:
*       Enable/disable receiving and transmission of 802.3x Flow Control frames
*       in full duplex on specified port on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       state - GT_TRUE for enable Flow Control, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortFlowControlEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortFlowControlEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortFlowControlEnable(dev, port, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortFlowControlEnableGet
*
* DESCRIPTION:
*       Get status of 802.3x Flow Control on specific logical port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       statePtr - GT_TRUE for enabled Flow Control, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortGetFlowControlEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortFlowControlEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   statePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortFlowControlEnableGet(dev, port, &statePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", statePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortPeriodicFcEnable
*
* DESCRIPTION:
*       Enable/Disable transmit of periodic 802.3x flow control.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       enable  - If GT_TRUE, enable transmit.
*                 If GT_FALSE, disable transmit.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortPeriodicFcEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortPeriodicFcEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortPeriodicFcEnable(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortBackPressureEnable
*
* DESCRIPTION:
*       Enable/disable of Back Pressure in half-duplex on specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       state - GT_TRUE for enable Back Pressure, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_NOT_SUPPORTED - on not supported feature for a port
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortBackPressureEnable.
*
*       For XGMII (10 Gbps) ports feature not supported
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortBackPressureEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortBackPressureEnable(dev, port, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortLinkStatusGet
*
* DESCRIPTION:
*       Gets Link Status of specified port on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       isLinkUpPtr - GT_TRUE for link up, GT_FALSE otherwise
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetPortLinkStatus.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortLinkStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   isLinkUpPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortLinkStatusGet(dev, port, &isLinkUpPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isLinkUpPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortInterfaceModeGet
*
* DESCRIPTION:
*       Gets Interface mode on a specified port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       ifModePtr - interface mode
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetPortInterfaceMode.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortInterfaceModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_INTERFACE_MODE_ENT   ifModePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortInterfaceModeGet(dev, port, &ifModePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ifModePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortForceLinkDownEnable
*
* DESCRIPTION:
*       Enable/disable Force Link Down on specified port on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       state - GT_TRUE for force link down, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortForceLinkDownEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortForceLinkDownEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortForceLinkDownEnable(dev, port, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortForceLinkPassEnable
*
* DESCRIPTION:
*       Enable/disable Force Link Pass on specified port on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       state - GT_TRUE for force link pass, GT_FALSE otherwise
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxPortForceLinkPassEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortForceLinkPassEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   state;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	state = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortForceLinkPassEnable(dev, port, state);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortMruSet
*
* DESCRIPTION:
*       Sets the Maximal Receive Packet size for specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       port    - physical port number
*       mruSize - max receive packet size
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxSetPortMru.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortMruSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_MRU_SIZE_ENT   mruSize;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	mruSize = (CPSS_PORT_MRU_SIZE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortMruSet(dev, port, mruSize);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortCrcCheckEnableSet
*
* DESCRIPTION:
*       Enable/Disable 32-bit the CRC checking.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       port    - physical port number
*       enable  - If GT_TRUE, enable CRC checking
*                 If GT_FALSE, disable CRC checking
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxSetPortCrcCheckEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortCrcCheckEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortCrcCheckEnableSet(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPrtXGmiiModeSet
*
* DESCRIPTION:
*       Sets XGMII mode on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxXGmiiModeSet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortXGmiiModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_XGMII_MODE_ENT   mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	mode = (CPSS_PORT_XGMII_MODE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortXGmiiModeSet(dev, port, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortExtraIpgSet
*
* DESCRIPTION:
*       Sets the number of 32-bit words to add to the 12-byte IPG.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       number   -  number of words
*
* OUTPUTS:
*       Hence, 12+4*ExtraIPG is the basis for the entire IPG calculation.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxExtraIpgSet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortExtraIpgSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_U8   number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	number = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortExtraIpgSet(dev, port, number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortXgmiiLocalFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected local
*       fault messages.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       isLocalFault   -  pointer to buffer for output value
*
* OUTPUTS:
*       *isLocalFaultPtr  - GT_TRUE if bit is set or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxXgmiiLocalFaultGet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortXgmiiLocalFaultGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   isLocalFaultPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortXgmiiLocalFaultGet(dev, port, &isLocalFaultPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isLocalFaultPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortXgmiiRemoteFaultGet
*
* DESCRIPTION:
*       Reads bit then indicate if the XGMII RS has detected remote
*       fault messages.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev   - physical device number
*       port  - physical port number
*       isRemoteFault   - pointer to buffer for output value
*
* OUTPUTS:
*       *isRemoteFaultPtr  - GT_TRUE if bit is set or GT_FALSE otherwise
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxXgmiiRemoteFaultGet.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortXgmiiRemoteFaultGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   isRemoteFaultPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortXgmiiRemoteFaultGet(dev, port, &isRemoteFaultPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isRemoteFaultPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortMacStatusGet
*
* DESCRIPTION:
*       Reads bits that indicate different problems on specified port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       port    - physical port number
*
* OUTPUTS:
*       portMacStatusPtr  - info about port MAC
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetPortMacStatus.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortMacStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_MAC_STATUS_STC   portMacStatusPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortMacStatusGet(dev, port, &portMacStatusPtr);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d", portMacStatusPtr.isPortRxPause/*(GT_BOOL)*/,
		                                              portMacStatusPtr.isPortTxPause/*(GT_BOOL)*/,
									               	  portMacStatusPtr.isPortBackPres/*(GT_BOOL)*/,
										              portMacStatusPtr.isPortBufFull/*(GT_BOOL)*/,
										              portMacStatusPtr.isPortSyncFail/*(GT_BOOL)*/,
										              portMacStatusPtr.isPortHiErrorRate/*(GT_BOOL)*/,
										              portMacStatusPtr.isPortAnDone/*(GT_BOOL)*/,
										              portMacStatusPtr.isPortFatalError/*(GT_BOOL)*/);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortInternalLoopbackEnableSet
*
* DESCRIPTION:
*       Set the internal Loopback state in the packet processor MAC port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       port    - physical port number
*       enable  - If GT_TRUE, enable loopback
*                 If GT_FALSE, disable loopback
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxSetPortInternalLoopbackEnable.
*       LoopBack is enabled only on XG devices.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortInternalLoopbackEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortInternalLoopbackEnableSet(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortInternalLoopbackEnableGet
*
* DESCRIPTION:
*       Get Internal Loopback
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       port    - physical port number
*
* OUTPUTS:
*       enablePtr  - If GT_TRUE, loopback is enabled
*                    If GT_FALSE, loopback is disabled
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetPortInternalLoopbackEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortInternalLoopbackEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	GT_BOOL   enablePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortInternalLoopbackEnableGet(dev, port, &enablePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enablePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortMacCounterGet
*
* DESCRIPTION:
*       Gets MAC counter for a specified port on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - physical device number
*       port     - physical port number
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - current counter value.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - cntrValuePtr is a null pointer
*       GT_OUT_OF_RANGE  - on invalid cntrName
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetMacCounter.
*       The 10G MAC MIB counters are 64-bit wide. the 98EX13X provide two sets
*       of counters for each counter-name. each packet is counted by only one of
*       them. therefore on reading, must read from both and add the values.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortMacCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_MAC_COUNTERS_ENT  cntrName;
	GT_U64   cntrValuePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	cntrName = (CPSS_PORT_MAC_COUNTERS_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortMacCounterGet(dev, port, cntrName, &cntrValuePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%s", galtisU64COut(&cntrValuePtr));
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortMacCountersOnPortGet
*
* DESCRIPTION:
*       Gets values of all MAC counter for a particular Port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - device number
*       port    - port number
*
* OUTPUTS:
*       portMacCounterSetPtr - set of all the MAC counters for the port.
*                              this is pointer to structure allocated by caller.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetMacCountersOnPort.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortMacCountersOnPortGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   port;
	CPSS_PORT_MAC_COUNTER_SET_STC  portMacCounterSetPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortMacCountersOnPortGet(dev, port, &portMacCounterSetPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
		                                            portMacCounterSetPtr.goodOctetsRcv/*(GT_U64)*/,
		                                            portMacCounterSetPtr.badOctetsRcv/*(GT_U64)*/,
		                                            portMacCounterSetPtr.macTransmitErr/*(GT_U64)*/,
		                                            portMacCounterSetPtr.goodPktsRcv/*(GT_U64)*/,
		                                            portMacCounterSetPtr.badPktsRcv/*(GT_U64)*/,
		                                            portMacCounterSetPtr.brdcPktsRcv/*(GT_U64)*/,
		                                            portMacCounterSetPtr.mcPktsRcv/*(GT_U64)*/,
		                                            portMacCounterSetPtr.pkts64Octets/*(GT_U64)*/,
		                                            portMacCounterSetPtr.pkts65to127Octets/*(GT_U64)*/,
		                                            portMacCounterSetPtr.pkts128to255Octets/*(GT_U64)*/,
		                                            portMacCounterSetPtr.pkts256to511Octets/*(GT_U64)*/,
		                                            portMacCounterSetPtr.pkts512to1023Octets/*(GT_U64)*/,
		                                            portMacCounterSetPtr.pkts1024tomaxOoctets/*(GT_U64)*/,
		                                            portMacCounterSetPtr.goodOctetsSent/*(GT_U64)*/,
		                                            portMacCounterSetPtr.goodPktsSent/*(GT_U64)*/,
		                                            portMacCounterSetPtr.excessiveCollisions/*(GT_U64)*/,
		                                            portMacCounterSetPtr.mcPktsSent/*(GT_U64)*/,
		                                            portMacCounterSetPtr.brdcPktsSent/*(GT_U64)*/,
		                                            portMacCounterSetPtr.unrecogMacCntrRcv/*(GT_U64)*/,
		                                            portMacCounterSetPtr.fcSent/*(GT_U64)*/,
		                                            portMacCounterSetPtr.goodFcRcv/*(GT_U64)*/,
		                                            portMacCounterSetPtr.dropEvents/*(GT_U64)*/,
		                                            portMacCounterSetPtr.undersizePkts/*(GT_U64)*/,
		                                            portMacCounterSetPtr.fragmentsPkts/*(GT_U64)*/,
		                                            portMacCounterSetPtr.oversizePkts/*(GT_U64)*/,
		                                            portMacCounterSetPtr.jabberPkts/*(GT_U64)*/,
		                                            portMacCounterSetPtr.macRcvError/*(GT_U64)*/,
		                                            portMacCounterSetPtr.badCrc/*(GT_U64)*/,
		                                            portMacCounterSetPtr.collisions/*(GT_U64)*/,
		                                            portMacCounterSetPtr.lateCollisions/*(GT_U64)*/,
		                                            portMacCounterSetPtr.badFcRcv/*(GT_U64)*/);
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPortEgressCntrModeSet
*
* DESCRIPTION:
*       Configures a set of egress counters that work in specified bridge egress
*       counters mode on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev        - physical device number
*       cntrSetNum - counter set number.
*       setModeBmp - counter mode bitmap. For example:
*                    CPSS_EGRESS_CNT_PORT_E | CPSS_EGRESS_CNT_VLAN_E |
*                    CPSS_EGRESS_CNT_DP_E | CPSS_EGRESS_CNT_DP_E.
*       port      - physical port number, if CPSS_EGRESS_CNT_PORT_E bit in
*                   setModeBmp is 1.
*       vlan      - VLAN Id, if corresponding bit in CPSS_EGRESS_CNT_VLAN_E is
*                   1.
*       tc        - traffic class queue, if CPSS_EGRESS_CNT_DP_E bit in
*                   setModeBmp is 1.
*       dpLevel   - drop precedence level, if CPSS_EGRESS_CNT_DP_E bit in
*                   setModeBmp is 1.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_OUT_OF_RANGE  - on illegal cntrSetNum
*       GT_FAIL          - otherwise
*
* COMMENTS:
*        0 MibEgPrioSet0En 0x1     When set to 1, the set 0 Egress filter MIB
*                                  counter
*                                  count only packets with priority equal to
*                                  set0 priority
*        1 MibEgDpSet0En 0x1       When set to 1, the set 0 Egress filter MIB
*                                  counter
*                                  count only packets with Drop precedence equal
*                                  to set0 DP
*        2 MibEgPrioSet1En 0x1     When set to 1, the set 1 Egress filter MIB
*                                  counter count only packets with priority
*                                  equal to set1 priority
*        3 MibEgDpSet1En 0x1       When set to 1, the set 1 Egress filter MIB
*                                  counter count only packets with Drop
*                                  precedence equal to set1 DP
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortEgressCntrModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   cntrSetNum;
	CPSS_PORT_EGRESS_CNT_MODE_ENT   setModeBmp;
	GT_U8   port;
	GT_U16   vlan;
	GT_U8   tc;
	CPSS_DP_LEVEL_ENT   dpLevel;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	cntrSetNum = (GT_U8)inArgs[1];
	setModeBmp = (CPSS_PORT_EGRESS_CNT_MODE_ENT)inArgs[2];
	port = (GT_U8)inArgs[3];
	vlan = (GT_U16)inArgs[4];
	tc = (GT_U8)inArgs[5];
	dpLevel = (CPSS_DP_LEVEL_ENT)inArgs[6];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortEgressCntrModeSet(dev, cntrSetNum, setModeBmp, port, vlan, tc, dpLevel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPortEgressCntrsGet
*
* DESCRIPTION:
*       Gets a egress counters from specific counter-set.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev        - physical device number
*       cntrSetNum - counter set number.
*
* OUTPUTS:
*       egrCntrPtr - pointer to structure of egress counters values.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_OUT_OF_RANGE  - on illegal cntrSetNum
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxGetEgressCntrs.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortEgressCntrsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_U8   cntrSetNum;
	CPSS_PORT_EGRESS_CNTR_STC  egrCntrPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	cntrSetNum = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPortEgressCntrsGet(dev, cntrSetNum, &egrCntrPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d", egrCntrPtr.outUcFrames/*(GT_U32)*/,
		                                          egrCntrPtr.outMcFrames/*(GT_U32)*/,
		                                          egrCntrPtr.outBcFrames/*(GT_U32)*/,
		                                          egrCntrPtr.brgEgrFilterDisc/*(GT_U32)*/,
		                                          egrCntrPtr.txqFilterDisc/*(GT_U32)*/,
		                                          egrCntrPtr.outCtrlFrames/*(GT_U32)*/);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortStatInit
*
* DESCRIPTION:
*       Init of cpss port counters facility.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       None.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*
* COMMENTS:
*     was coreExMxPortStatInit.
*     This API must be called before any other call to port counters related
*     function.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortStatInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPortStatInit(dev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortTxQueueEnable
*
* DESCRIPTION:
*       Enable/Disable enqueuing on all Tx queues of specified port
*       on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       enable - GT_TRUE, Global enqueuing enabled
*                GT_FALSE, Global enqueuing disabled
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxQueueEnable.
*******************************************************************************/

static CMD_STATUS wrcpssExMxPortTxQueueEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   dev;
	GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPortTxQueueEnable(dev, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxQEnable
*
* DESCRIPTION:
*       Enable/Disable enqueuing to a Traffic Class queue
*       on the specified port of specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       queue  - traffic class queue on this device
*       enable - GT_TRUE, enable enqueuing to the queue
*                GT_FALSE, disable enqueuing to the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or queue
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortQEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxQEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_U8     queue;
	GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	queue = (GT_U8)inArgs[2];
	enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxQEnable(dev, port, queue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxQTxEnable
*
* DESCRIPTION:
*       Enable/Disable transmission from a Traffic Class queue
*       on the specified port of specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       queue  - traffic class queue on this Physical Port
*       enable - GT_TRUE, enable transmission from the queue
*                GT_FALSE, disable transmission from the queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or queue
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortQTxEnable.
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxQTxEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_U8     queue;
	GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	queue = (GT_U8)inArgs[2];
	enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxQTxEnable(dev, port, queue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxFlushQueues
*
* DESCRIPTION:
*       Flush all the traffic class queues on the specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortFlushQueues.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxFlushQueues
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxFlushQueues(dev, port);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPortTxShaperEnable
*
* DESCRIPTION:
*       Enable/Disable Token Bucket rate shaping on specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       enable - GT_TRUE, enable Shaping
*                GT_FALSE, disable Shaping
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortShaperEnable.
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxShaperEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_BOOL   enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxShaperEnable(dev, port, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxShaperProfileSet
*
* DESCRIPTION:
*       Set Token Bucket Shaper Profile on specified port of specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev        - physical device number
*       port       - physical port number
*       burstSize  - burst size in units of 4K bytes
*                   (max value is 4K which results in 16K burst size)
*       maxRatePtr - Requested Rate in Kbps
*
* OUTPUTS:
*       maxRatePtr - Actual Rate in Kbps
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - maxRatePtr is a null pointer
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortSetShaperProfile.
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxShaperProfileSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_U16    burstSize;
	GT_U32    maxRatePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	burstSize = (GT_U16)inArgs[2];
	maxRatePtr = (GT_U32)inArgs[3];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxShaperProfileSet(dev, port, burstSize, &maxRatePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maxRatePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxQShaperEnable
*
* DESCRIPTION:
*       Enable/Disable shaping of transmitted traffic from a specified Traffic
*       Class Queue and specified port of specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - physical port number
*       queue  - traffic class queue on this port
*       enable - GT_TRUE, enable shaping on this traffic queue
*                GT_FALSE, disable shaping on this traffic queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or queue
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortQShaperEnable.
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxQShaperEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_U8     queue;
	GT_BOOL   enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	queue = (GT_U8)inArgs[2];
	enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxQShaperEnable(dev, port, queue, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxQShaperProfileSet
*
* DESCRIPTION:
*       Set Shaper Profile for Traffic Class Queue of specified port on
*       specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev          - physical device number
*       port         - physical port number
*       queue        - traffic class queue on this Logical Port
*       burstSize    - burst size in units of 4K bytes
*                      (max value is 4K which results in 16K burst size)
*       userRatePtr  - Requested Rate in Kbps
*
* OUTPUTS:
*       userRatePtr  - Actual Rate in Kbps
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - userRatePtr is a null pointer
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortQSetShaperProfile.
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxQShaperProfileSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_U8     queue;
	GT_U16    burstSize;
	GT_U32    maxRatePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	queue = (GT_U8)inArgs[2];
	burstSize = (GT_U16)inArgs[3];
	maxRatePtr = (GT_U32)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxQShaperProfileSet(dev, port, queue, burstSize, &maxRatePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maxRatePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxQWrrProfileSet
*
* DESCRIPTION:
*       Set Weighted Round Robin profile on the specified port's
*       Traffic Class Queue.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       port      - physical port number
*       queue     - traffic class queue on this Port
*       wrrWeight - proportion of bandwidth assigned to this queue
*                   relative to the other queues in this
*                   Arbitration Group - resolution is 1/255.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or queue
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*           was coreExMxTxPortQSetWrrProfile.
*           - if weight will be less then port's MTU (maximum transmit unit) there
*             is possibility for empty WRR loops for given TC queue, but to not
*             tight user and for future ASIC's where this problem will be solved
*             check of wrrWeight * 256 > MTU not implemented
*
*              1)the port parameter will use to get:
*                   a. devNum - for specific device to configure
*                   b. portNum - for validity
*              2) the queue parameter will use to :
*                 validity (in range 0-3)
*              3) the wrrWeight parameter will use to :
*                  The value must be  = (2^(queue)) , meaning:
*                  For Queue number 0 --- WRR weight must be 1
*                  For Queue number 1 --- WRR weight must be 2
*                  For Queue number 2 --- WRR weight must be 4
*                  For Queue number 3 --- WRR weight must be 8
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxQWrrProfileSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_U8     queue;
	GT_U8   wrrWeight;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	queue = (GT_U8)inArgs[2];
	wrrWeight = (GT_U8)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxQWrrProfileSet(dev, port, queue, wrrWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxQArbGroupSet
*
* DESCRIPTION:
*       Set Traffic Class Queue scheduling arbitration group on
*       specified port of specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - physical device number
*       port     - physical port number
*       queue    - traffic class queue on this Logical Port
*       arbGroup - scheduling arbitration group:
*                     1) Strict Priority
*                     2) WRR Group 1
*                     3) WRR Group 0
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or queue
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortQSetArbGroup.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxQArbGroupSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_U8     queue;
	CPSS_PORT_TX_Q_ARB_GROUP_ENT   arbGroup;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	queue = (GT_U8)inArgs[2];
	arbGroup = (CPSS_PORT_TX_Q_ARB_GROUP_ENT)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxQArbGroupSet(dev, port, queue, arbGroup);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxDropMode
*
* DESCRIPTION:
*       Sets the Port Drop Mode for specified port on specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev      - physical device number
*       port     - physical port number
*       dropMode - Queue drop algorithm for all traffic class queues
*                  on this Port: 3-level GRED or 3-level Tail-Drop.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortDropMode.
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxDropMode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	CPSS_PORT_TX_Q_DROP_MODE_ENT     dropMode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	dropMode = (CPSS_PORT_TX_Q_DROP_MODE_ENT)inArgs[2];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxDropMode(dev, port, dropMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPortTxTcDpProfileSet
*
* DESCRIPTION:
*       There are 4 global Sets of Drop Profile Parameters, where each global
*       set consist of a set of Drop Profile Parameters per Traffic Class.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev               - physical device number
*       pfSet             - the Profile Set in which the Traffic
*                           Class Drop Parameters is associated
*       tc                - the Traffic Class associated with this set of
*                           Drop Parameters
*       dropProfileParams - the Drop Profile Parameters to associate
*                           with the Traffic Class in this Profile set.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or queue
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - dropProfileParams is a null pointer
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortTcDpProfileSet.
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxTcDpProfileSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                                   dev;
	CPSS_PORT_TX_DROP_PROFILE_SET_ENT       pfSet;
	GT_U8                                   tc;
	CPSS_PORT_TX_Q_DROP_PROFILE_PARAMS_STC  dropProfileParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	pfSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[1];
	tc = (GT_U8)inArgs[2];
	dropProfileParams.dp0MinThreshold = (GT_U16)inArgs[3];
    dropProfileParams.dp1MinThreshold = (GT_U16)inArgs[4];
    dropProfileParams.dp2MinThreshold = (GT_U16)inArgs[5];
    dropProfileParams.dp0MaxThreshold = (GT_U16)inArgs[6];
    dropProfileParams.dp1MaxThreshold = (GT_U16)inArgs[7];
    dropProfileParams.dp2MaxThreshold = (GT_U16)inArgs[8];
    dropProfileParams.dp0MaxProb = (GT_U8)inArgs[9];
    dropProfileParams.dp1MaxProb = (GT_U8)inArgs[10];
    dropProfileParams.dp2MaxProb = (GT_U8)inArgs[11];
    dropProfileParams.RedQWeightFactor = (GT_U8)inArgs[12];


    /* call cpss api function */
    result = cpssExMxPortTxTcDpProfileSet(dev, pfSet, tc, &dropProfileParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");                     return CMD_OK;
}


/*******************************************************************************
*  cpssExMxPortTxBindPortToDpSet
*
* DESCRIPTION:
*       Bind a Physical Port to a specific Drop Profile Set.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev           - physical device number
*       port          - physical port number
*       profileSet    - the Profile Set in which the Traffic
*                       Class Drop Parameters is associated
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortBindPortToDpSet.
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxBindPortToDpSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	CPSS_PORT_TX_DROP_PROFILE_SET_ENT     profileSet;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	profileSet = (CPSS_PORT_TX_DROP_PROFILE_SET_ENT)inArgs[2];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxBindPortToDpSet(dev, port, profileSet);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
*  cpssExMxPortTxMaxDescNumberSet
*
* DESCRIPTION:
*       Sets the maximal Tx queue descriptors that specified port can
*       allocate. The resolution is 16 descriptors.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       port    - physical port number
*       number  - number of descriptors
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortMaxDescNumber.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxMaxDescNumberSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_U16     number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	number = (GT_U16)inArgs[2];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxMaxDescNumberSet(dev, port, number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
*  cpssExMxPortTxDescNumberGet
*
* DESCRIPTION:
*       Gets the current number of descriptors allocated specified port.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       port    - physical port number
*
* OUTPUTS:
*       numberPtr  - pointer to number of descriptors
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - numberPtr is a null pointer
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortGetDescNumber.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxDescNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
	GT_U8       port;
	GT_U16      numberPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxDescNumberGet(dev, port, &numberPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", numberPtr);
    return CMD_OK;
}


/*******************************************************************************
*  cpssExMxPortTx4TcMaxDescNumberSet
*
* DESCRIPTION:
*       Sets the maximal Tx queue descriptors that specified port can
*       allocate for specified Traffic Class queues.
*       The resolution is 16 descriptors.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       port      - physical port number
*       trafClass - traffic class
*       number    - number of descriptors
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or queue
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPort4TcMaxDescNumber
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTx4TcMaxDescNumberSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_U8     trafClass;
	GT_U16    number;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	trafClass = (GT_U8)inArgs[2];
	number = (GT_U16)inArgs[3];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTx4TcMaxDescNumberSet(dev, port, trafClass, number);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*******************************************************************************
*  cpssExMxPortTx4TcDescNumberGet
*
* DESCRIPTION:
*       Gets the current number of descriptors allocated on specified port
*       for specified Traffic Class queues.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev       - physical device number
*       port      - physical port number
*       trafClass - traffic class
*
* OUTPUTS:
*       numberPtr - pointer to number of descriptors
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or traffic class
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - numberPtr is a null pointer
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPort4TcGetDescNumber
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTx4TcDescNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
	GT_U8       port;
	GT_U8       trafClass;
	GT_U16      numberPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	trafClass = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTx4TcDescNumberGet(dev, port, trafClass, &numberPtr);


    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", numberPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTx4TcMaxDescNumberGet
*
* DESCRIPTION:
*       Get Tx Port Traffic class max descriptors number
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       port    - port number
*       tc      - traffic class
*
* OUTPUTS:
*       maxDescNumPtr - pointer to ports' traffic class max descriptors number
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device or traffic class
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - maxDescNumPtr is a null pointer
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPort4TcGetMaxDescNumber.
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTx4TcMaxDescNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
	GT_U8       port;
	GT_U8       tc;
	GT_U16      maxDescNumPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	tc = (GT_U8)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTx4TcMaxDescNumberGet(dev, port, tc, &maxDescNumPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maxDescNumPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxMaxDescNumberGet
*
* DESCRIPTION:
*       Get Tx Port max descriptors number
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - port number
*
* OUTPUTS:
*       maxDescNumPtr - pointer to ports' max descriptors number
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - maxDescNumPtr is a null pointer
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreExMxTxPortGetMaxDescNumber.
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxMaxDescNumberGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
	GT_U8       port;
	GT_U16      maxDescNumPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxMaxDescNumberGet(dev, port, &maxDescNumPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", maxDescNumPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxWatchdogGet
*
* DESCRIPTION:
*       Get Tx Port Watchdog status and timeout
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev    - physical device number
*       port   - port number
*
* OUTPUTS:
*       enablePtr     - enable / disable
*       timeoutPtr    - the watchdog timeout
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_BAD_PTR       - enablePtr or timeoutPtr is a null pointer
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was gtCoreExMxTxPortGetWatchdog
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxWatchdogGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8       dev;
	GT_U8       port;
	GT_BOOL     enablePtr;
	GT_U16      timeoutPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxWatchdogGet(dev, port, &enablePtr, &timeoutPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enablePtr, timeoutPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPortTxWatchdogEnable
*
* DESCRIPTION:
*       Enable/Disable Watchdog on specified port of specified port of
*       specified device.
*
* APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       dev     - physical device number
*       port    - physical port number
*       enable  - GT_TRUE, enable Watchdog
*                 GT_FALSE, disable Watchdog
*       timeout - Watchdog timeout in milliseconds (1..4K ms)
*                 If disabling the Watchdog, this parameter is ignored
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on wrong port number or device
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - otherwise
*
* COMMENTS:
*       was coreGenTxPortWatchdogEnable.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxPortTxWatchdogEnable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     dev;
	GT_U8     port;
	GT_BOOL   enable;
	GT_U16    timeout;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev = (GT_U8)inArgs[0];
	port = (GT_U8)inArgs[1];
	enable = (GT_BOOL)inArgs[2];
	timeout = (GT_U16)inArgs[3];


    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(dev, port);

    /* call cpss api function */
    result = cpssExMxPortTxWatchdogEnable(dev, port, enable, timeout);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPortBackEndModeEnable",
        &wrcpssExMxPortBackEndModeEnable,
        3, 0},

    {"cpssExMxPortXonLimitSet",
        &wrcpssExMxPortXonLimitSet,
        3, 0},

     {"cpssExMxPortXoffLimitSet",
        &wrcpssExMxPortXoffLimitSet,
        3, 0},

    {"cpssExMxPortRxBufLimitSet",
        &wrcpssExMxPortRxBufLimitSet,
        3, 0},

    {"cpssExMxPortBufMngMaxBufSet",
        &wrcpssExMxPortBufMngMaxBufSet,
        2, 0},

    {"cpssExMxPortBufMngLinesLimitSet",
        &wrcpssExMxPortBufMngLinesLimitSet,
        2, 0},

        {"cpssExMxPortCpuRxBufLimitSet",
        &wrcpssExMxPortCpuRxBufLimitSet,
        2, 0},

    {"cpssExMxPortBufModeGet",
        &wrcpssExMxPortBufModeGet,
        1, 0},

     {"cpssExMxPortCpuRxBufCountGet",
        &wrcpssExMxPortCpuRxBufCountGet,
        1, 0},

    {"cpssExMxPortBufMgInit",
        &wrcpssExMxPortBufMgInit,
        1, 0},

    {"cpssExMxPortMacSaLsbSet",
        &wrcpssExMxPortMacSaLsbSet,
        3, 0},

    {"cpssExMxPortEnable",
        &wrcpssExMxPortEnable,
        3, 0},

    {"cpssExMxPortEnableGet",
        &wrcpssExMxPortEnableGet,
        2, 0},

    {"cpssExMxPortDuplexModeSet",
        &wrcpssExMxPortDuplexModeSet,
        3, 0},

     {"cpssExMxPortDuplexModeGet",
        &wrcpssExMxPortDuplexModeGet,
        2, 0},

    {"cpssExMxPortSpeedSet",
        &wrcpssExMxPortSpeedSet,
        3, 0},

    {"cpssExMxPortSpeedGet",
        &wrcpssExMxPortSpeedGet,
        2, 0},

    {"cpssExMxPortAttributesOnPortGet",
        &wrcpssExMxPortAttributesOnPortGet,
        2, 0},

        {"cpssExMxPortDuplexAutoNegEnable",
        &wrcpssExMxPortDuplexAutoNegEnable,
        3, 0},

    {"cpssExMxPortFlowCntrlAutoNegEnable",
        &wrcpssExMxPortFlowCntrlAutoNegEnable,
        4, 0},

     {"cpssExMxPortSpeedAutoNegEnable",
        &wrcpssExMxPortSpeedAutoNegEnable,
        3, 0},

    {"cpssExMxPortFlowControlEnable",
        &wrcpssExMxPortFlowControlEnable,
        3, 0},

    {"cpssExMxPortFlowControlEnableGet",
        &wrcpssExMxPortFlowControlEnableGet,
        2, 0},

    {"cpssExMxPortPeriodicFcEnable",
        &wrcpssExMxPortPeriodicFcEnable,
        3, 0},

    {"cpssExMxPortBackPressureEnable",
        &wrcpssExMxPortBackPressureEnable,
        3, 0},

    {"cpssExMxPortLinkStatusGet",
        &wrcpssExMxPortLinkStatusGet,
        2, 0},

     {"cpssExMxPortInterfaceModeGet",
        &wrcpssExMxPortInterfaceModeGet,
        2, 0},

    {"cpssExMxPortForceLinkDownEnable",
        &wrcpssExMxPortForceLinkDownEnable,
        3, 0},

    {"cpssExMxPortForceLinkPassEnable",
        &wrcpssExMxPortForceLinkPassEnable,
        3, 0},

    {"cpssExMxPortMruSet",
        &wrcpssExMxPortMruSet,
        3, 0},

        {"cpssExMxPortCrcCheckEnableSet",
        &wrcpssExMxPortCrcCheckEnableSet,
        3, 0},

    {"cpssExMxPortXGmiiModeSet",
        &wrcpssExMxPortXGmiiModeSet,
        3, 0},

     {"cpssExMxPortExtraIpgSet",
        &wrcpssExMxPortExtraIpgSet,
        3, 0},

    {"cpssExMxPortXgmiiLocalFaultGet",
        &wrcpssExMxPortXgmiiLocalFaultGet,
        2, 0},

    {"cpssExMxPortXgmiiRemoteFaultGet",
        &wrcpssExMxPortXgmiiRemoteFaultGet,
        2, 0},

    {"cpssExMxPortMacStatusGet",
        &wrcpssExMxPortMacStatusGet,
        2, 0},

    {"cpssExMxPortInternalLoopbackEnableSet",
        &wrcpssExMxPortInternalLoopbackEnableSet,
        3, 0},

    {"cpssExMxPortInternalLoopbackEnableGet",
        &wrcpssExMxPortInternalLoopbackEnableGet,
        2, 0},

     {"cpssExMxPortMacCounterGet",
        &wrcpssExMxPortMacCounterGet,
        3, 0},

    {"cpssExMxPortMacCountersOnPortGet",
        &wrcpssExMxPortMacCountersOnPortGet,
        2, 0},

    {"cpssExMxPortEgressCntrModeSet",
        &wrcpssExMxPortEgressCntrModeSet,
        7, 0},

    {"cpssExMxPortEgressCntrsGet",
        &wrcpssExMxPortEgressCntrsGet,
        2, 0},

        {"cpssExMxPortStatInit",
        &wrcpssExMxPortStatInit,
        1, 0},

    {"cpssExMxPortTxQueueEnable",
        &wrcpssExMxPortTxQueueEnable,
        2, 0},

     {"cpssExMxPortTxQEnable",
        &wrcpssExMxPortTxQEnable,
        4, 0},

    {"cpssExMxPortTxQTxEnable",
        &wrcpssExMxPortTxQTxEnable,
        4, 0},

    {"cpssExMxPortTxFlushQueues",
        &wrcpssExMxPortTxFlushQueues,
        2, 0},

    {"cpssExMxPortTxShaperEnable",
        &wrcpssExMxPortTxShaperEnable,
        3, 0},

     {"cpssExMxPortTxShaperProfileSet",
        &wrcpssExMxPortTxShaperProfileSet,
        4, 0},

    {"cpssExMxPortTxQShaperEnable",
        &wrcpssExMxPortTxQShaperEnable,
        4, 0},

    {"cpssExMxPortTxQShaperProfileSet",
        &wrcpssExMxPortTxQShaperProfileSet,
        5, 0},

    {"cpssExMxPortTxQWrrProfileSet",
        &wrcpssExMxPortTxQWrrProfileSet,
        4, 0},

        {"cpssExMxPortTxQArbGroupSet",
        &wrcpssExMxPortTxQArbGroupSet,
        4, 0},

    {"cpssExMxPortTxDropMode",
        &wrcpssExMxPortTxDropMode,
        3, 0},

     {"cpssExMxPortTxTcDpProfileSet",
        &wrcpssExMxPortTxTcDpProfileSet,
        13, 0},

    {"cpssExMxPortTxBindPortToDpSet",
        &wrcpssExMxPortTxBindPortToDpSet,
        3, 0},

    {"cpssExMxPortTxMaxDescNumberSet",
        &wrcpssExMxPortTxMaxDescNumberSet,
        3, 0},

    {"cpssExMxPortTxDescNumberGet",
        &wrcpssExMxPortTxDescNumberGet,
        2, 0},

    {"cpssExMxPortTx4TcMaxDescNumberSet",
        &wrcpssExMxPortTx4TcMaxDescNumberSet,
        4, 0},

    {"cpssExMxPortTx4TcDescNumberGet",
        &wrcpssExMxPortTx4TcDescNumberGet,
        3, 0},

     {"cpssExMxPortTx4TcMaxDescNumberGet",
        &wrcpssExMxPortTx4TcMaxDescNumberGet,
        3, 0},

    {"cpssExMxPortTxMaxDescNumberGet",
        &wrcpssExMxPortTxMaxDescNumberGet,
        2, 0},

    {"cpssExMxPortTxWatchdogGet",
        &wrcpssExMxPortTxWatchdogGet,
        2, 0},

    {"cpssExMxPortTxWatchdogEnable",
        &wrcpssExMxPortTxWatchdogEnable,
        4, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitCpssExMxPort
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
GT_STATUS cmdLibInitCpssExMxPort
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

