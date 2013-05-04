/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxStc.c
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
#include <cpss/exMx/exMxGen/mirror/cpssExMxStc.h>

/*******************************************************************************
* cpssExMxStcReloadModeSet
*
* DESCRIPTION:
*       Set the type of Sampling To CPU (STC) count reload mode. 
*
* APPLICABLE DEVICES:  Only ExMxTg devices and 98EX135
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       mode      - Sampling to CPU (STC) Reload mode
*                   CPSS_EXMX_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_EXMX_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
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
static CMD_STATUS wrCpssExMxStcReloadModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMX_STC_TYPE_ENT stcType;
    CPSS_EXMX_STC_COUNT_RELOAD_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_EXMX_STC_TYPE_ENT)inArgs[1];
    mode = (CPSS_EXMX_STC_COUNT_RELOAD_MODE_ENT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxStcReloadModeSet(devNum, stcType, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxStcReloadModeGet
*
* DESCRIPTION:
*       Get the type of Sampling To CPU (STC) count reload mode. 
*
* APPLICABLE DEVICES:  Only ExMxTg devices and 98EX135
*
* INPUTS:
*       devNum    - the device number
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       modePtr   - (pointer to) Sampling to CPU (STC) Reload mode
*                   CPSS_EXMX_STC_COUNT_RELOAD_CONTINUOUS_E - contiuous mode
*                   CPSS_EXMX_STC_COUNT_RELOAD_TRIGGERED_E - triggered mode
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
static CMD_STATUS wrCpssExMxStcReloadModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMX_STC_TYPE_ENT stcType;
    CPSS_EXMX_STC_COUNT_RELOAD_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_EXMX_STC_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxStcReloadModeGet(devNum, stcType, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxStcEnableSet
*
* DESCRIPTION:
*       Global Enable/Disable Sampling To CPU (STC). 
*
* APPLICABLE DEVICES:  Only ExMxTg devices and 98EX135
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
static CMD_STATUS wrCpssExMxStcEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMX_STC_TYPE_ENT stcType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_EXMX_STC_TYPE_ENT)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxStcEnableSet(devNum, stcType, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxStcEnableGet
*
* DESCRIPTION:
*       Get the global status of Sampling To CPU (STC) (Enabled/Disabled). 
*
* APPLICABLE DEVICES:  Only ExMxTg devices and 98EX135
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
static CMD_STATUS wrCpssExMxStcEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMX_STC_TYPE_ENT stcType;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    stcType = (CPSS_EXMX_STC_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxStcEnableGet(devNum, stcType, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxStcPortSampleValueSet
*
* DESCRIPTION:
*       Only for XG device.
*       Set Sampling to CPU (STC) packet skip value per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is 
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this 
*       decrement is sampled to the CPU.
*       For Ex136 and Ex135 devices, The written values are accumulate in a FIFO to be 
*       auto-loaded to packet count down counter. Non-zero value should be written 
*       only when Ingress STC(XG) Full is not set, the status can be read by
*       cpssExMxStcPortIngressFifoFullStatusGet API. 
*       For Ex136 and Ex135 devices, only ingress STC type is relevant. 
*
* APPLICABLE DEVICES:  Only ExMxTg devices and 98EX135
*
* INPUTS:
*       devNum    - the device number
*       port      - 0 (default). 
*       stcType   - CPSS_EXMX_STC_INGRESS_E
*       sampleVal - Count Down Limit (0 - 0xFFFFFFFF)
*                   value of 0 - means there is no sampling
*                   value of 1 - means every packet is sampled to CPU 
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
static CMD_STATUS wrCpssExMxStcPortSampleValueTableSet
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
    CPSS_EXMX_STC_TYPE_ENT stcType;
    GT_U32 sampleVal;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = 0;
    stcType = CPSS_EXMX_STC_INGRESS_E;
    sampleVal = (GT_U32)inFields[0];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxStcPortSampleValueSet(devNum, port, stcType, sampleVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxStcPortSampleValueSet
*
* DESCRIPTION:
*       Set Sampling to CPU (STC) packet skip value per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is 
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this 
*       decrement is sampled to the CPU.
*       For Ex136 and Ex135 devices, The written values are accumulate in a FIFO to be 
*       auto-loaded to packet count down counter. Non-zero value should be written 
*       only when Ingress STC(XG) Full is not set, the status can be read by
*       cpssExMxStcPortIngressFifoFullStatusGet API. 
*       For Ex136 and Ex135 devices, only ingress STC type is relevant. 
*
* APPLICABLE DEVICES:  Only ExMxTg devices and 98EX135
*
* INPUTS:
*       devNum    - the device number
*       port      - port number. 
*                   For STC of type CPSS_EXMX_STC_EGRESS_E, CPU port included.
*       stcType   - Sampling To CPU (STC) type: ingress or egress
*       sampleVal - Count Down Limit (0 - 0xFFFFFFFF)
*                   value of 0 - means there is no sampling
*                   value of 1 - means every packet is sampled to CPU 
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
static CMD_STATUS wrCpssExMxStcPortSampleValueSet
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
    CPSS_EXMX_STC_TYPE_ENT stcType;
    GT_U32 sampleVal;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stcType = (CPSS_EXMX_STC_TYPE_ENT)inArgs[2];
    sampleVal = (GT_U32)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxStcPortSampleValueSet(devNum, port, stcType, sampleVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxStcPortSampleValueGet
*
* DESCRIPTION:
*       Get Sampling to CPU (STC) packet skip value per port.
*       The limit to be loaded into the Count Down Counter.
*       This counter is decremented for each packet received on this port and is 
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this 
*       decrement is sampled to the CPU.
*       For Ex136 and Ex135 devices, this value is accumulated in a FIFO to be 
*       auto-loaded to packet count down counter.
*       For Ex136 and Ex135 devices, only ingress STC type is relevant. 
*
* APPLICABLE DEVICES:  Only ExMxTg devices and 98EX135
*
* INPUTS:
*       devNum          - the device number
*       port            - port number
*                         For STC of type CPSS_EXMX_STC_EGRESS_E, CPU port included.
*       stcType         - Sampling To CPU (STC) type: ingress or egress
*
* OUTPUTS:
*       sampleValPtr    - Count Down Limit (0 - 0xFFFFFFFF)
*                         value of 0 - means there is no sampling
*                         value of 1 - means every packet is sampled to CPU 
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
static CMD_STATUS wrCpssExMxStcPortSampleValueGet
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
    CPSS_EXMX_STC_TYPE_ENT stcType;
    GT_U32 sampleVal;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stcType = (CPSS_EXMX_STC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxStcPortSampleValueGet(devNum, port, stcType, &sampleVal);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", sampleVal);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxStcPortCountdownCntrGet
*
* DESCRIPTION:
*       Get STC Countdown Counter per port.
*       This counter represents the number of packets to skip till 
*       ingress/egress packet will be sampled.
*       This counter is decremented for each packet received on this port and is 
*       subject to sampling according to the setting of STC Count mode.
*       When this counter is decremented from 1 to 0, the packet causing this 
*       decrement is sampled to the CPU.
*       For Ex136 and Ex135 devices, only ingress STC type is relevant. 
*
* APPLICABLE DEVICES:  Only ExMxTg devices and 98EX135
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*                   For STC of type CPSS_EXMX_STC_EGRESS_E, CPU port included.
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
static CMD_STATUS wrCpssExMxStcPortCountdownCntrGet
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
    CPSS_EXMX_STC_TYPE_ENT stcType;
    GT_U32 cntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stcType = (CPSS_EXMX_STC_TYPE_ENT)inArgs[2];

     /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

   /* call cpss api function */
    result = cpssExMxStcPortCountdownCntrGet(devNum, port, stcType, &cntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxStcPortSampledPacketsCntrGet
*
* DESCRIPTION:
*       Get the number of packets Sampled to CPU taken on this port.
*       The counter is stuck at maximal value and doesn't overflow to 0.
*       The counter is cleared by read.
*       For Ex136 and Ex135 devices, only ingress STC type is relevant. 
*
* APPLICABLE DEVICES:  Only ExMxTg devices and 98EX135
*
* INPUTS:
*       devNum    - the device number
*       port      - port number
*                   For STC of type CPSS_EXMX_STC_EGRESS_E, CPU port included.
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
static CMD_STATUS wrCpssExMxStcPortSampledPacketsCntrGet
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
    CPSS_EXMX_STC_TYPE_ENT stcType;
    GT_U32 cntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    stcType = (CPSS_EXMX_STC_TYPE_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxStcPortSampledPacketsCntrGet(devNum, port, stcType, &cntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxStcIngressFifoFullLevelSet
*
* DESCRIPTION:
*       Set the treshold for STC FIFO stack.
*       When Ingress STC (XG) values FIFO reaches below this treshold, 
*       the Ingress STC Full interrupt is issued.
*
* APPLICABLE DEVICES:  Only Ex136 and Ex135 devices
*
* INPUTS:
*       devNum    - the device number
*       treshold  - STC FIFO treshold (0 - 31). 
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - wrong device number or STC type.
*       GT_OUT_OF_RANGE - when treshold is out of range
*       GT_HW_ERROR     - on writing to HW error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxStcIngressFifoFullLevelSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 treshold;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    treshold = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxStcIngressFifoFullLevelSet(devNum, treshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxStcIngressFifoFullLevelGet
*
* DESCRIPTION:
*       Get the treshold for Ingress STC FIFO stack.
*       When Ingress STC (XG) values FIFO reaches below this treshold, 
*       the Ingress STC Full interrupt is issued.
*
* APPLICABLE DEVICES:  Only Ex136 and Ex135 devices
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       tresholdPtr   - (pointer to) STC FIFO treshold. 
*
* RETURNS:
*       GT_OK         - on success.
*       GT_BAD_PARAM  - wrong device number or STC type.
*       GT_HW_ERROR   - on writing to HW error.
*
* COMMENTS:
*       The Application should monitor this interrupt, when the count mode is
*       Triggered. When such interrupt is issued, the Application should load 
*       new sample values to FIFO.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxStcIngressFifoFullLevelGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 treshold;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxStcIngressFifoFullLevelGet(devNum, &treshold);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", treshold);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxStcPortIngressFifoFullStatusGet
*
* DESCRIPTION:
*       Check whether FIFO level exceedes the FifoFullLevel value.  
*       per port.
*
* APPLICABLE DEVICES:  Only 98EX136 devices and 98EX135
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       emptyStatusPtr - (pointer to) empty FIFO level
*                    GT_TRUE = Empty, new value can be written.
*                    GT_FALSE = Full, write operation may override previous 
*                               value.
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
static CMD_STATUS wrCpssExMxStcPortIngressFifoFullStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_BOOL emptyStatus;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxStcPortIngressFifoFullStatusGet(devNum, &emptyStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", emptyStatus);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxStcFifoStatusGet
*
* DESCRIPTION:
*       Get Ingress FIFO Status.
*       Indicates the number of occupied entries in Ingress STC (XG) 
*       Value FIFO.
*
* APPLICABLE DEVICES:  Only Ex136 and Ex135 devices
*
* INPUTS:
*       devNum    - the device number
*
* OUTPUTS:
*       statusPtr   - (pointer to) Ingerss STC FIFO Value status. 
*                     0 - Empty FIFO, 31 - Full FIFO.
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
static CMD_STATUS wrCpssExMxStcIngressFifoStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 status;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxStcIngressFifoStatusGet(devNum, &status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", status);

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxStcReloadModeSet",
         &wrCpssExMxStcReloadModeSet,
         3, 0},
        {"cpssExMxStcReloadModeGet",
         &wrCpssExMxStcReloadModeGet,
         2, 0},
        {"cpssExMxStcEnableSet",
         &wrCpssExMxStcEnableSet,
         3, 0},
        {"cpssExMxStcEnableGet",
         &wrCpssExMxStcEnableGet,
         2, 0},
        {"cpssExMxStcPortSampleValueTableSet",
         &wrCpssExMxStcPortSampleValueTableSet,
         1, 1},
        {"cpssExMxStcPortSampleValueSet",
         &wrCpssExMxStcPortSampleValueSet,
         4, 0},
        {"cpssExMxStcPortSampleValueGet",
         &wrCpssExMxStcPortSampleValueGet,
         3, 0},
        {"cpssExMxStcPortCountdownCntrGet",
         &wrCpssExMxStcPortCountdownCntrGet,
         3, 0},
        {"cpssExMxStcPortSampledPacketsCntrGet",
         &wrCpssExMxStcPortSampledPacketsCntrGet,
         3, 0},
        {"cpssExMxStcIngressFifoFullLevelSet",
         &wrCpssExMxStcIngressFifoFullLevelSet,
         2, 0},
        {"cpssExMxStcIngressFifoFullLevelGet",
         &wrCpssExMxStcIngressFifoFullLevelGet,
         1, 0},
        {"cpssExMxStcPortIngressFifoFullStatusGet",
         &wrCpssExMxStcPortIngressFifoFullStatusGet,
         1, 0},
        {"cpssExMxStcIngressFifoStatusGet",
         &wrCpssExMxStcIngressFifoStatusGet,
         1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxStc
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
GT_STATUS cmdLibInitCpssExMxStc
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

