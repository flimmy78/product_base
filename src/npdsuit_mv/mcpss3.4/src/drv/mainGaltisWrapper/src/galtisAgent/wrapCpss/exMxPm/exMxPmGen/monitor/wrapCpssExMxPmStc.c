/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmStc.c
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
#include <cpss/exMxPm/exMxPmGen/monitor/cpssExMxPmStc.h>

/*******************************************************************************
* cpssExMxPmStcIngressCountModeSet
*
* DESCRIPTION:
*       Set the type of packets subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       mode      - CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E - All packets without 
*                                                         any MAC-level errors.
*                   CPSS_EXMXPM_STC_COUNT_NON_DROPPED_PACKETS_E - 
*                                                       only non-dropped packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcIngressCountModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_STC_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_STC_COUNT_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmStcIngressCountModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcIngressCountModeGet
*
* DESCRIPTION:
*       Get the type of packets that are subject to Ingress Sampling to CPU.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       modePtr   - CPSS_EXMXPM_STC_COUNT_ALL_PACKETS_E = All packets without 
*                                                           any MAC-level errors.
*                   CPSS_EXMXPM_STC_COUNT_NON_DROPPED_PACKETS_E - 
*                                                       only non-dropped packets.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcIngressCountModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_STC_COUNT_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmStcIngressCountModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcReloadModeSet
*
* DESCRIPTION:
*       Set the type of Sampling To CPU (STC) count reload mode. 
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       mode      - Sampling to CPU (STC) Reload mode
*                   CPSS_EXMXPM_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_EXMXPM_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,STC type or mode.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcReloadModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_STC_TYPE_ENT stcType;
    CPSS_EXMXPM_STC_COUNT_RELOAD_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_EXMXPM_STC_TYPE_ENT)inArgs[1];
    mode = (CPSS_EXMXPM_STC_COUNT_RELOAD_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmStcReloadModeSet(devNum, stcType, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcReloadModeGet
*
* DESCRIPTION:
*       Get the type of Sampling To CPU (STC) count reload mode. 
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       modePtr   - (pointer to) Sampling to CPU (STC) Reload mode
*                   CPSS_EXMXPM_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_EXMXPM_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcReloadModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_STC_TYPE_ENT stcType;
    CPSS_EXMXPM_STC_COUNT_RELOAD_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_EXMXPM_STC_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmStcReloadModeGet(devNum, stcType, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcEnableSet
*
* DESCRIPTION:
*       Enable/Disable Sampling To CPU (STC). 
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       enable    - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_STC_TYPE_ENT stcType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_EXMXPM_STC_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmStcEnableSet(devNum, stcType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcEnableGet
*
* DESCRIPTION:
*       Get the status of Sampling To CPU (STC) (Enabled/Disabled). 
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       enablePtr - GT_TRUE = enable Sampling To CPU (STC)
*                   GT_FALSE = disable Sampling To CPU (STC)
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_STC_TYPE_ENT stcType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_EXMXPM_STC_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmStcEnableGet(devNum, stcType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcPortLimitSet
*
* DESCRIPTION:
*       Set Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is 
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this 
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number. 
*                   For STC of type CPSS_EXMXPM_STC_EGRESS_E, CPU port included.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       limit     - Count Down Limit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_OUT_OF_RANGE - limit is out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcPortLimitSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_STC_TYPE_ENT stcType;
    GT_U32 limit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stcType = (CPSS_EXMXPM_STC_TYPE_ENT)inArgs[2];
    limit = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmStcPortLimitSet(devNum, port, stcType, limit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcPortLimitGet
*
* DESCRIPTION:
*       Get Sampling to CPU (STC) limit per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is 
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this 
*       decrement is sampled to the CPU.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*                   For STC of type CPSS_EXMXPM_STC_EGRESS_E, CPU port included.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       limit     - Count Down Limit
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcPortLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_STC_TYPE_ENT stcType;
    GT_U32 limit;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stcType = (CPSS_EXMXPM_STC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmStcPortLimitGet(devNum, port, stcType, &limit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", limit);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcPortReadyForNewLimitGet
*
* DESCRIPTION:
*       Check whether Sampling to CPU is ready to get new STC Limit Value per 
*       port.
*       The function cpssExMxPmStcPortLimitSet sets new limit value. 
*       But only after device finishes handling of new limit value the 
*       cpssExMxPmStcPortLimitSet may be called once more.
*       
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*                   For STC of type CPSS_EXMXPM_STC_EGRESS_E, CPU port included.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       isReadyPtr - (pointer to) Is Ready value
*                    GT_TRUE = Sampling to CPU ready to get new STC Limit Value
*                    GT_FALSE = Sampling to CPU handles STC Limit Value yet and 
*                               is not ready to get new value.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcPortReadyForNewLimitGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_STC_TYPE_ENT stcType;
    GT_BOOL isReady;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stcType = (CPSS_EXMXPM_STC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmStcPortReadyForNewLimitGet(devNum, port, stcType, &isReady);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", isReady);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcPortCntrGet
*
* DESCRIPTION:
*       Get Sampling to CPU (STC) Counter per port.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*                   For STC of type CPSS_EXMXPM_STC_EGRESS_E, CPU port included.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Count down counter. This is the number of
*                   packets left to send/receive in order that a packet will be
*                   sampled to CPU and a new value will be loaded.
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcPortCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_STC_TYPE_ENT stcType;
    GT_U32 cntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stcType = (CPSS_EXMXPM_STC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmStcPortCntrGet(devNum, port, stcType, &cntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcPortSampledPacketsCntrSet 
*
* DESCRIPTION:
*       Set the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*                   For STC of type CPSS_EXMXPM_STC_EGRESS_E, CPU port included.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       cntr      - STC Sampled to CPU packet's counter. 
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*       GT_OUT_OF_RANGE - on wrong cntr
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcPortSampledPacketsCntrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_STC_TYPE_ENT stcType;
    GT_U32 cntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stcType = (CPSS_EXMXPM_STC_TYPE_ENT)inArgs[2];
    cntr = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmStcPortSampledPacketsCntrSet(devNum, port, stcType, cntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmStcPortSampledPacketsCntrGet
*
* DESCRIPTION:
*       Get the number of packets Sampled to CPU per port.
*
* APPLICABLE DEVICES:  All EXMXPM devices
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*                   For STC of type CPSS_EXMXPM_STC_EGRESS_E, CPU port included.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       cntrPtr   - (pointer to) STC Sampled to CPU packet's counter. 
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number,port number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmStcPortSampledPacketsCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 port;
    CPSS_EXMXPM_STC_TYPE_ENT stcType;
    GT_U32 cntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stcType = (CPSS_EXMXPM_STC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmStcPortSampledPacketsCntrGet(devNum, port, stcType, &cntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntr);

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmStcIngressCountModeSet",
         &wrCpssExMxPmStcIngressCountModeSet,
         2, 0},
        {"cpssExMxPmStcIngressCountModeGet",
         &wrCpssExMxPmStcIngressCountModeGet,
         1, 0},
        {"cpssExMxPmStcReloadModeSet",
         &wrCpssExMxPmStcReloadModeSet,
         3, 0},
        {"cpssExMxPmStcReloadModeGet",
         &wrCpssExMxPmStcReloadModeGet,
         2, 0},
        {"cpssExMxPmStcEnableSet",
         &wrCpssExMxPmStcEnableSet,
         3, 0},
        {"cpssExMxPmStcEnableGet",
         &wrCpssExMxPmStcEnableGet,
         2, 0},
        {"cpssExMxPmStcPortLimitSet",
         &wrCpssExMxPmStcPortLimitSet,
         4, 0},
        {"cpssExMxPmStcPortLimitGet",
         &wrCpssExMxPmStcPortLimitGet,
         3, 0},
        {"cpssExMxPmStcPortReadyForNewLimitGet",
         &wrCpssExMxPmStcPortReadyForNewLimitGet,
         3, 0},
        {"cpssExMxPmStcPortCntrGet",
         &wrCpssExMxPmStcPortCntrGet,
         3, 0},
        {"cpssExMxPmStcPortSampledPacketsCntrSet",
         &wrCpssExMxPmStcPortSampledPacketsCntrSet,
         4, 0},
        {"cpssExMxPmStcPortSampledPacketsCntrGet",
         &wrCpssExMxPmStcPortSampledPacketsCntrGet,
         3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmStc
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
GT_STATUS cmdLibInitCpssExMxPmStc
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

