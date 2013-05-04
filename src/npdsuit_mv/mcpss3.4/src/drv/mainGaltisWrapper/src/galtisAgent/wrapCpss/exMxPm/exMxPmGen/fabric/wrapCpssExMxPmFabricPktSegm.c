/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmFabricPktSegm.c
*
* DESCRIPTION:
*       CPSS ExMxPm Fabric Connectivity Packet Segmentation
*       and Cell Scheduler API wrappers.
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
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricPktSegm.h>

/*******************************************************************************
* cpssExMxPmFabricPktSegmArbiterEnableSet
*
* DESCRIPTION:
*     This routine enables/disables the transmission of cells from the
*     fabric adapter Cell Segmentation Cell FIFOs to
*     Fabric Crossbar interface.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum    - the device number.
*       enable    - GT_TRUE enables cell transmission from the segmentation
*                         cell FIFOs
*                   GT_FALSE disables cell transmission from the
*                         segmentation cell FIFOs

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmArbiterEnableSet
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
    result = cpssExMxPmFabricPktSegmArbiterEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmArbiterEnableGet
*
* DESCRIPTION:
*     This routine gets enable/disable status of the transmission of cells 
*     from the fabric adapter Cell Segmentation Cell FIFOs to
*     Fabric Crossbar interface.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum            - the device number.
*
* OUTPUTS:
*       enablePtr - GT_TRUE enables cell transmission from the segmentation
*                         cell FIFOs
*                   GT_FALSE disables cell transmission from the
*                         segmentation cell FIFOs
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmArbiterEnableGet
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
    result = cpssExMxPmFabricPktSegmArbiterEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmMiscConfigSet
*
* DESCRIPTION:
*     This routine Sets Miscelanous configuration 
*     of the segmentation engine.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       miscConfigPtr  - Miscelanous control data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmMiscConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_CONFIG_MISC_STC miscConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmMiscConfigGet(devNum, &miscConfig);

    miscConfig.mcVoqId = (CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_VOQ_ID_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmMiscConfigSet(devNum, &miscConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmMiscConfigGet
*
* DESCRIPTION:
*     This routine Gets Miscelanous configuration 
*     of the segmentation engine.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*
* OUTPUTS:
*       miscConfigPtr  - Miscelanous control data
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmMiscConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_CONFIG_MISC_STC miscConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmMiscConfigGet(devNum, &miscConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", miscConfig.mcVoqId);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmFabricPktSegmConfigFifoStatusModeSet
*
* DESCRIPTION:
*     This routine configures Segmentation Fifo Status Mode.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum  - the device number.
*       mode    - Segmentation Fifo Status Mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, mode.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmConfigFifoStatusModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmConfigFifoStatusModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmConfigFifoStatusModeGet
*
* DESCRIPTION:
*     This routine gets Segmentation Fifo Status Mode.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum     - the device number.
*
* OUTPUTS:
*       modePtr    - Segmentation Fifo Status Mode.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmConfigFifoStatusModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
                                GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_MODE_ENT mode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmConfigFifoStatusModeGet(devNum, &mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmFifoStatusGet
*
* DESCRIPTION:
*   This routine get Segmentation FIFO status as defined by 
*   cpssExMxPmFabricPktSegmFifoStatusModeSet
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       xbarNum        - the Xbar number 
*                        (ExMxPm device contains 2 Xbars 0 and 1)
*       fport          - fabric port to enable/disable dequeueing
*                        CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS for MC packets queue
*       fabricTc       - Fport/MC fabric priority queue (0-3)
* OUTPUTS:
*       fifoStatusPtr  -  FIFO status depend on status mode:
*                            - CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_FULL_E
*                                  GT_TRUE - FIFO full
*                                  GT_FALSE - FIFO is not full
*                            - CPSS_EXMXPM_FABRIC_PKT_SEGM_FIFO_STATUS_EMPTY_E
*                                  GT_TRUE - FIFO empty
*                                  GT_FALSE - FIFO is not empty
*
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, xbar number, fport or traffic class
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmFifoStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    IN  GT_U8    devNum;
    IN  GT_U32   voqUnitNum;
    IN  GT_FPORT fport;
    IN  GT_U32   fabricTc;
    OUT GT_BOOL  fifoStatus;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    fport = (GT_FPORT)inArgs[2];
    fabricTc = (GT_U32)inArgs[3];
    
    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmFifoStatusGet(devNum, voqUnitNum, fport, fabricTc, &fifoStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", fifoStatus);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmArbConfigSet
*
* DESCRIPTION:
*   This routine sets arbitration configuration per fabric traffic class. 
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - the device number.
*       arbConfigSetNum - WRR set number (0-3)
*       fabricTc        - fabric traffic class 0..3
*       arbPolicy       - Arbitration mode: Strict Priority or Round Robin
*       wrrWeight       - weight of fabric priority queue, in terms of Cells to be
*                   transmitted by the FA Tx Arbiter vs other 3 priority queues.
*                   If the weight value is 4095, then the
*                   corresponding traffic class has Strict Priority over the
*                   remaining traffic classes.  If more than one traffic class
*                   is assigned a weight of 4095, then a round-robin scheduling
*                   is performed between these Strict Priority classes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum,arbConfigSetNum,fabricTc or arbPolicy 
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on wrrWeight out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmArbConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 arbConfigSetNum;
    GT_U32 fabricTc;
    CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT arbPolicy;
    GT_U32 wrrWeight;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    arbConfigSetNum = (GT_U32)inArgs[1];
    fabricTc = (GT_U32)inArgs[2];
    arbPolicy = (CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT)inArgs[3];
    wrrWeight = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmArbConfigSet(devNum, arbConfigSetNum, fabricTc, arbPolicy, wrrWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmArbConfigGet
*
* DESCRIPTION:
*   This routine gets arbitration configuration per fabric traffic class.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - the device number.
*       arbConfigSetNum - WRR set number (0-3)
*       fabricTc        - fabric traffic class 0..3
*
* OUTPUTS:
*       arbPolicyPtr   - Arbitration mode: Strict Priority or Round Robin
*       wrrWeightPtr   - weight of fabric priority queue, in terms of Cells to be
*                   transmitted by the FA Tx Arbiter vs other 3 priority queues.
*                   If the weight value is 4095, then the
*                   corresponding traffic class has Strict Priority over the
*                   remaining traffic classes.  If more than one traffic class
*                   is assigned a weight of 4095, then a round-robin scheduling
*                   is performed between these Strict Priority classes.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum,arbConfigSetNum,fabricTc 
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmArbConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 arbConfigSetNum;
    GT_U32 fabricTc;
    CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT arbPolicy;
    GT_U32 wrrWeight;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    arbConfigSetNum = (GT_U32)inArgs[1];
    fabricTc = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmArbConfigGet(devNum, arbConfigSetNum, fabricTc, &arbPolicy, &wrrWeight);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", arbPolicy, wrrWeight);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmErrorCntrGet
*
* DESCRIPTION:
*     This routine gets number of parity errors 
*     detected in packets read from the Segmentation Fifo.
*     This counter counts number of errors generated in the
*     Data SRAM as well as in the Segmentaion FIFO
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
* OUTPUTS:
*       errorCntrPtr   - (pointer to) errors counter.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       The counters data is cleared after read.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmErrorCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 errorCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmErrorCntrGet(devNum, &errorCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", errorCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmFportToArbConfigMapSet
*
* DESCRIPTION:
*   This routine binds a target fabric port's or multicast traffic class FIFOs to
*   the specified arbitration configuration set.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - the device number.
*       xbarNum         - the Xbar number 
*                         (ExMxPm device contains 2 Xbars 0 and 1)
*       fport           - fabric port to enable/disable dequeueing
*                         CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS for MC packets queue
*       arbConfigSetNum - arbitration configuration set number (0-3)
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on out of range WRR set number.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmFportToArbConfigMapSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    IN GT_U8    devNum;
    IN GT_U32   voqUnitNum;
    IN GT_FPORT fport;
    IN GT_U32   arbConfigSetNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum          = (GT_U8)inArgs[0];
    voqUnitNum      = (GT_U32)inArgs[1];
    fport           = (GT_FPORT)inArgs[2];
    arbConfigSetNum = (GT_U32)inArgs[3];
    

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmFportToArbConfigMapSet(devNum, voqUnitNum, fport, arbConfigSetNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmFportToArbConfigMapGet
*
* DESCRIPTION:
*   This routine gets binds of target fabric port's or multicast traffic class FIFOs to
*   the specified arbitration configuration set.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - the device number.
*       xbarNum         - the Xbar number 
*                         (ExMxPm device contains 2 Xbars 0 and 1)
*       fport           - fabric port to enable/disable dequeueing
*                         CPSS_EXMXPM_FABRIC_MULTICAST_FPORT_CNS for MC packets queue
*
* OUTPUTS:
*       arbConfigSetNumPtr - (pointer to)arbitration configuration set number (0-3)
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmFportToArbConfigMapGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    IN  GT_U8    devNum;
    IN  GT_U32   voqUnitNum;
    IN  GT_FPORT fport;
    OUT GT_U32   arbConfigSetNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    voqUnitNum = (GT_U32)inArgs[1];
    fport = (GT_FPORT)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmFportToArbConfigMapGet(devNum, voqUnitNum, fport, &arbConfigSetNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", arbConfigSetNum);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmMcUcRatioSet
*
* DESCRIPTION:
*     This routine sets the transmission ratio 
*     of unicast cells vs. multicast cells from the 
*     segmentation FIFO to the fabric.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum      - the device number.
*       mcUcRatio   - MC and UC Cells Ratio
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmMcUcRatioSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_RATIO_ENT mcUcRatio;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    mcUcRatio = (CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_RATIO_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmMcUcRatioSet(devNum, mcUcRatio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmMcUcRatioGet
*
* DESCRIPTION:
*     This routine gets the transmission ratio 
*     of unicast cells vs. multicast cells from the 
*     segmentation FIFO to the fabric.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum            - the device number.
*
* OUTPUTS:
*       mcUcRatioPtr   - MC and UC Cells ratio
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmMcUcRatioGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_MC_UC_RATIO_ENT mcUcRatio;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmMcUcRatioGet(devNum, &mcUcRatio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", mcUcRatio);

    return CMD_OK;
}

/**********************/
/*** wrappers added ***/
/**********************/

/*******************************************************************************
* cpssExMxPmFabricPktSegmEnableSet
*
* DESCRIPTION:
*     This routine enables/disables segmentation activity.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum    - the device number.
*       enable    - GT_TRUE  - Segmentation engine is ready to get descriptors
*                              from the VoQ unit.
*                   GT_FALSE - Segmentation engine indicates that all channels
*                              are unavailable.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    IN GT_U8    devNum;
    IN GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}



/*******************************************************************************
* cpssExMxPmFabricPktSegmEnableGet
*
* DESCRIPTION:
*     This routine gets enables/disables status of segmentation activity.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum            - the device number.
*
* OUTPUTS:
*       enablePtr -  pointer to segmentation activity status
*                   GT_TRUE  - Segmentation engine is ready to get descriptors
*                              from the VoQ unit.
*                   GT_FALSE - Segmentation engine indicates that all channels
*                              are unavailable.
*
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    IN  GT_U8    devNum;
    OUT GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}




/*******************************************************************************
* cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet
*
* DESCRIPTION:
*     This routine enables/disables reducing the number of cells after
*     Flow Control to three.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum - the device number.
*       enable - GT_TRUE  - The channel that receives Xoff is masked also at
*                           the output of the FA arbiter. This reduce the
*                           number of cells send from the queue to three
*                           after channel recieves Xoff.
*                GT_FALSE - The channel that receives Xoff is masked only in
*                           the entrance to the FA arbiter. All cells of the
*                           queue are sent after channel receives Xoff.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet
(
        IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    IN GT_U8    devNum;
    IN GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet
*
* DESCRIPTION:
*     This routine gets enables/disables status for reducing the number of cells
*     after Flow Control to three.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum            - the device number.
*
* OUTPUTS:
*       enablePtr -  pointer to  reducing status of the number of cells after
*                    Flow Control to three.
*                    GT_TRUE  - The channel that receives Xoff is masked also at
*                               the output of the FA arbiter. This reduce the
*                               number of cells send from the queue to three
*                               after channel recieves Xoff.
*                    GT_FALSE - The channel that receives Xoff is masked only in
*                               the entrance to the FA arbiter. All cells of the
*                               queue are sent after channel receives Xoff.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS    result;
    IN  GT_U8    devNum;
    OUT GT_BOOL  enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    
    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmPeelCrcEnableSet
*
* DESCRIPTION:
*     This routine Sets the Peel CRC enable mode of Miscelanous  
*     configuration of the segmentation engine.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       miscConfigPtr  - Miscelanous control data
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmPeelCrcEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_CONFIG_MISC_STC miscConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmMiscConfigGet(devNum, &miscConfig);

    miscConfig.peelCrcEnable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmMiscConfigSet(devNum, &miscConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktSegmPeelCrcEnableSet
*
* DESCRIPTION:
*     This routine gets the Peel CRC enable mode of Miscelanous  
*     configuration of the segmentation engine.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*
* OUTPUTS:
*       miscConfigPtr  - Miscelanous control data
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktSegmPeelCrcEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_SEGM_CONFIG_MISC_STC miscConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricPktSegmMiscConfigGet(devNum, &miscConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", miscConfig.peelCrcEnable);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmFabricPktSegmArbiterEnableSet",
         &wrCpssExMxPmFabricPktSegmArbiterEnableSet,
         2, 0},
        {"cpssExMxPmFabricPktSegmArbiterEnableGet",
         &wrCpssExMxPmFabricPktSegmArbiterEnableGet,
         1, 0},
        {"cpssExMxPmFabricPktSegmMiscConfigSet",
         &wrCpssExMxPmFabricPktSegmMiscConfigSet,
         2, 0},
        {"cpssExMxPmFabricPktSegmMiscConfigGet",
         &wrCpssExMxPmFabricPktSegmMiscConfigGet,
         1, 0},
        {"cpssExMxPmFabricPktSegmConfigFifoStatusModeSet",
         &wrCpssExMxPmFabricPktSegmConfigFifoStatusModeSet,
         2, 0},
        {"cpssExMxPmFabricPktSegmConfigFifoStatusModeGet",
         &wrCpssExMxPmFabricPktSegmConfigFifoStatusModeGet,
         1, 0},
        {"cpssExMxPmFabricPktSegmFifoStatusGet",
         &wrCpssExMxPmFabricPktSegmFifoStatusGet,
         4, 0},
        {"cpssExMxPmFabricPktSegmArbConfigSet",
         &wrCpssExMxPmFabricPktSegmArbConfigSet,
         5, 0},
        {"cpssExMxPmFabricPktSegmArbConfigGet",
         &wrCpssExMxPmFabricPktSegmArbConfigGet,
         3, 0},
        {"cpssExMxPmFabricPktSegmErrorCntrGet",
         &wrCpssExMxPmFabricPktSegmErrorCntrGet,
         1, 0},
        {"cpssExMxPmFabricPktSegmFportToArbConfigMapSet",
         &wrCpssExMxPmFabricPktSegmFportToArbConfigMapSet,
         4, 0},
        {"cpssExMxPmFabricPktSegmFportToArbConfigMapGet",
         &wrCpssExMxPmFabricPktSegmFportToArbConfigMapGet,
         3, 0},
        {"cpssExMxPmFabricPktSegmMcUcRatioSet",
         &wrCpssExMxPmFabricPktSegmMcUcRatioSet,
         2, 0},
        {"cpssExMxPmFabricPktSegmMcUcRatioGet",
         &wrCpssExMxPmFabricPktSegmMcUcRatioGet,
         1, 0},
        {"cpssExMxPmFabricPktSegmTxDEnableSet",
            &wrCpssExMxPmFabricPktSegmEnableSet,
            2, 0},
        {"cpssExMxPmFabricPktSegmTxDEnableGet",
            &wrCpssExMxPmFabricPktSegmEnableGet,
            1, 0},
        {"cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet",
            &wrCpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableSet,
            2, 0},
        {"cpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet",
            &wrCpssExMxPmFabricPktSegmFcMaskAfterFaArbEnableGet,
            1, 0},
        {"cpssExMxPmFabricPktSegmPeelCrcEnableSet",
            &wrCpssExMxPmFabricPktSegmPeelCrcEnableSet,
            2, 0},
        {"cpssExMxPmFabricPktSegmPeelCrcEnableGet",
            &wrCpssExMxPmFabricPktSegmPeelCrcEnableGet,
            1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmFabricPktSegm
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
GT_STATUS cmdLibInitCpssExMxPmFabricPktSegm
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

