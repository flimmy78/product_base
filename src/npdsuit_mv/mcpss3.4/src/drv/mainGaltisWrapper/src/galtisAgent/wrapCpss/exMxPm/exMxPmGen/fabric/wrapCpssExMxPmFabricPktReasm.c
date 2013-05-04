/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmFabricPktReasm.c
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
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricPktReasm.h>

/*******************************************************************************
* cpssExMxPmFabricPktReasmControlConfigSet
*
* DESCRIPTION:
*     This routine Sets the control configuration.
*
* INPUTS:
*       devNum            - the device number.
*       controlConfigPtr  - (pointer to) the control configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on maxNumOfBuffers out of range value.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktReasmControlConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_CONTROL_CONFIG_STC controlConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    controlConfig.remoteCpuCellAction =
        (CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_ACTION_ENT)inArgs[1];
    controlConfig.bindContextMode =
        (CPSS_EXMXPM_FABRIC_BIND_CONTEXT_MODE_ENT)inArgs[2];
    controlConfig.buffersDropEnable = (GT_BOOL)inArgs[3];
    controlConfig.maxNumOfBuffers   = (GT_U32)inArgs[4];
    controlConfig.checkCrc          = (GT_BOOL)inArgs[5];

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmControlConfigSet(devNum, &controlConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktReasmControlConfigGet
*
* DESCRIPTION:
*     This routine Gets the control configuration.
*
* INPUTS:
*       devNum            - the device number.
*
* OUTPUTS:
*       controlConfigPtr  - (pointer to) the control configuration.
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
static CMD_STATUS wrCpssExMxPmFabricPktReasmControlConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_CONTROL_CONFIG_STC controlConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    controlConfig.remoteCpuCellAction =
        CPSS_EXMXPM_FABRIC_PKT_REASM_REMOTE_CPU_CELL_DROP_E;
    controlConfig.bindContextMode =
        CPSS_EXMXPM_FABRIC_BIND_CONTEXT_CLASS_BASED_LBI_E;
    controlConfig.buffersDropEnable = GT_FALSE;
    controlConfig.maxNumOfBuffers   = 0;
    controlConfig.checkCrc          = GT_FALSE;

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmControlConfigGet(devNum, &controlConfig);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d%d%d",
        controlConfig.remoteCpuCellAction,
        controlConfig.bindContextMode,
        controlConfig.buffersDropEnable,
        controlConfig.maxNumOfBuffers,
        controlConfig.checkCrc);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktReasmFifosConfigSet
*
* DESCRIPTION:
*     This routine Sets Fifos Sizes And Thresholds.
*
* INPUTS:
*       devNum         - the device number.
*       fifoSizeThrPtr - (pointer to) Fifos Sizes And Thresholds.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on Packet Reassembly fifos sizes thresholds out
*                         of range values.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktReasmFifosConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_FIFOS_CONFIG_STC fifoSizeThr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum                    = (GT_U8) inArgs[0];
    fifoSizeThr.e2eThrXOff    = (GT_U8) inArgs[1];
    fifoSizeThr.e2eThrXOn     = (GT_U8) inArgs[2];
    fifoSizeThr.rxDataThrXOff = (GT_U8) inArgs[3];
    fifoSizeThr.rxDataThrXOn  = (GT_U8) inArgs[4];

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmFifosConfigSet(devNum, &fifoSizeThr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktReasmFifosConfigGet
*
* DESCRIPTION:
*     This routine Gets Fifos Sizes And Thresholds.
*
* INPUTS:
*       devNum         - the device number.
*
* OUTPUTS:
*       fifoSizeThrPtr - (pointer to) Fifos Sizes And Thresholds.
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
static CMD_STATUS wrCpssExMxPmFabricPktReasmFifosConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_FIFOS_CONFIG_STC fifoSizeThr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                    = (GT_U8) inArgs[0];
    fifoSizeThr.e2eThrXOff    = 0;
    fifoSizeThr.e2eThrXOn     = 0;
    fifoSizeThr.rxDataThrXOff = 0;
    fifoSizeThr.rxDataThrXOn  = 0;


    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmFifosConfigGet(devNum, &fifoSizeThr);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d",
        fifoSizeThr.e2eThrXOff,
        fifoSizeThr.e2eThrXOn,
        fifoSizeThr.rxDataThrXOff,
        fifoSizeThr.rxDataThrXOn);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktReasmLastCellDropStatusGet
*
* DESCRIPTION:
*     This routine Gets Last Cell Drop Status.
*
* INPUTS:
*       devNum         - the device number.
*
* OUTPUTS:
*       dropStatusPtr     - (pointer to) Last Cell Drop Status.
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
static CMD_STATUS wrCpssExMxPmFabricPktReasmLastCellDropStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_LAST_CELL_DROP_STATUS_STC dropStatus;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    /*TODO: add code here. */
    devNum                                = (GT_U8)inArgs[0];
    dropStatus.unknownTypeDrop            = GT_FALSE;
    dropStatus.buffersLimitDropped        = GT_FALSE;
    dropStatus.badContextDrop             = GT_FALSE;
    dropStatus.bufferLimitPerPacketDrop   = GT_FALSE;
    dropStatus.wrongByteCountDrop         = GT_FALSE;
    dropStatus.badCellCRCDrop             = GT_FALSE;
    dropStatus.multicastCongestionDrop    = GT_FALSE;
    dropStatus.lastContextDropped         = 0;

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmLastCellDropStatusGet(devNum, &dropStatus);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d%d%d%d%d",
        dropStatus.unknownTypeDrop,
        dropStatus.buffersLimitDropped,
        dropStatus.badContextDrop,
        dropStatus.bufferLimitPerPacketDrop,
        dropStatus.wrongByteCountDrop,
        dropStatus.badCellCRCDrop,
        dropStatus.multicastCongestionDrop,
        dropStatus.lastContextDropped);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktReasmGlobalCellCounterGet
*
* DESCRIPTION:
*     This routine Gets a global cell counter value.
*
* INPUTS:
*       devNum           - the device number.
*       cellCounterType  - global cell counter type
*
* OUTPUTS:
*       cellCounterPtr - (pointer to) the cell counter value.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or cellCounterType.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktReasmGlobalCellCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_TYPE_ENT cellCounterType;
    GT_U32 cellCounter;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    cellCounterType = (CPSS_EXMXPM_FABRIC_PKT_REASM_GLOBAL_CNTR_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmGlobalCellCounterGet(devNum, cellCounterType, &cellCounter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cellCounter);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktReasmPerSetCellCounterGet
*
* DESCRIPTION:
*     This routine Gets a set's cell counter value.
*
* INPUTS:
*       devNum         - the device number.
*       setCounterType -  per set cell counter type.
*       setNum         -  set configuration number (0-3).
*
* OUTPUTS:
*       cntrPtr        - (pointer to) the cell counter value.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, setNum or setCounterType.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktReasmPerSetCellCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT setCounterType;
    GT_U32 setNum;
    GT_U32 cntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    setCounterType = (CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT)inArgs[1];
    setNum = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmPerSetCellCounterGet(devNum, setCounterType, setNum, &cntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", cntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet
*
* DESCRIPTION:
*     This routine Sets the cell conter configuration.
*
* INPUTS:
*       devNum            - the device number.
*       setCounterType    -  per set cell counter type.
*       setNum            -  set configuration number (0-3).
*       cellCntrConfigPtr - (pointer to) the cell counter configuration.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, setCounterType or setNum
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on cellCntrConfigPtr->fabricTc or srcDev out of range
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktReasmPerSetCellCntrConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT setCounterType;
    GT_U32 setNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_CNTR_CONFIG_STC cellCntrConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    setCounterType =
        (CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT)inArgs[1];
    setNum = (GT_U32)inArgs[2];

    cellCntrConfig.onlyEopCells = (GT_BOOL)inArgs[3];
    cellCntrConfig.cellType     = (CPSS_EXMXPM_FABRIC_CELL_TYPE_ENT)inArgs[4];
    cellCntrConfig.allFabricTc  = (GT_BOOL)inArgs[5];
    cellCntrConfig.fabricTc     = (GT_U32) inArgs[6];
    cellCntrConfig.allSrcDev    = (GT_BOOL)inArgs[7];
    cellCntrConfig.srcDev       = (GT_U8)  inArgs[8];


    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet(
        devNum, setCounterType, setNum, &cellCntrConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet
*
* DESCRIPTION:
*     This routine Gets the cell counter configuration.
*
* INPUTS:
*       devNum            - the device number.
*       setCounterType    -  per set cell counter type.
*       setNum            -  set configuration number (0-3).
*
* OUTPUTS:
*       cellCntrConfigPtr - (pointer to) the cell counter configuration.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, setCounterType or setNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktReasmPerSetCellCntrConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT setCounterType;
    GT_U32 setNum;
    CPSS_EXMXPM_FABRIC_PKT_REASM_CNTR_CONFIG_STC cellCntrConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    setCounterType =
        (CPSS_EXMXPM_FABRIC_PKT_REASM_PER_SET_CNTR_TYPE_ENT)inArgs[1];
    setNum = (GT_U32)inArgs[2];

    cellCntrConfig.onlyEopCells = GT_FALSE;
    cellCntrConfig.cellType     = CPSS_EXMXPM_FABRIC_CELL_TYPE_ANY_E;
    cellCntrConfig.allFabricTc  = GT_FALSE;
    cellCntrConfig.fabricTc     = 0;
    cellCntrConfig.allSrcDev    = GT_FALSE;
    cellCntrConfig.srcDev       = 0;

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet(devNum, setCounterType, setNum, &cellCntrConfig);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d%d%d",
        cellCntrConfig.onlyEopCells,
        cellCntrConfig.cellType,
        cellCntrConfig.allFabricTc,
        cellCntrConfig.fabricTc,
        cellCntrConfig.allSrcDev,
        cellCntrConfig.srcDev);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktReasmAgingTimerSet
*
* DESCRIPTION:
*     Configure Packet State Machine entries aging.
*
* INPUTS:
*       devNum          - the device number.
*       enable          - Perform/don't perform aging on the PSM entries.
*                         GT_TRUE - perform aging
*                         GT_FALSE - don't perform aging
*       timer           - Timer for the PSM aging mechanism.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_OUT_OF_RANGE - on aging timer out of range value.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricPktReasmPsmAgingSet
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
    GT_U32 timer;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    timer = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmPsmAgingSet(devNum, enable, timer);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricPktReasmPsmAgingGet
*
* DESCRIPTION:
*     Get configuration of Packet State Machine entries aging.
*
* INPUTS:
*       devNum          - the device number.
*
* OUTPUTS:
*       enablePtr          - (pointer to)  Perform/don't perform aging on
*                            the PSM entries.
*                            GT_TRUE - perform aging
*                            GT_FALSE - don't perform aging
*       timerPtr           - (pointer to) Timer for the PSM aging mechanism.
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
static CMD_STATUS wrCpssExMxPmFabricPktReasmPsmAgingGet
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
    GT_U32 timer;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    enable = GT_FALSE;
    timer = 0;

    /* call cpss api function */
    result = cpssExMxPmFabricPktReasmPsmAgingGet(devNum, &enable, &timer);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", enable, timer);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmFabricPktReasmControlConfigSet",
         &wrCpssExMxPmFabricPktReasmControlConfigSet,
         6, 0},
        {"cpssExMxPmFabricPktReasmControlConfigGet",
         &wrCpssExMxPmFabricPktReasmControlConfigGet,
         1, 0},
        {"cpssExMxPmFabricPktReasmFifosConfigSet",
         &wrCpssExMxPmFabricPktReasmFifosConfigSet,
         5, 0},
        {"cpssExMxPmFabricPktReasmFifosConfigGet",
         &wrCpssExMxPmFabricPktReasmFifosConfigGet,
         1, 0},
        {"cpssExMxPmFabricPktReasmLastCellDropStatusGet",
         &wrCpssExMxPmFabricPktReasmLastCellDropStatusGet,
         1, 0},
        {"cpssExMxPmFabricPktReasmGlobalCellCounterGet",
         &wrCpssExMxPmFabricPktReasmGlobalCellCounterGet,
         2, 0},
        {"cpssExMxPmFabricPktReasmPerSetCellCounterGet",
         &wrCpssExMxPmFabricPktReasmPerSetCellCounterGet,
         3, 0},
        {"cpssExMxPmFabricPktReasmPerSetCellCntrConfigSet",
         &wrCpssExMxPmFabricPktReasmPerSetCellCntrConfigSet,
         9, 0},
        {"cpssExMxPmFabricPktReasmPerSetCellCntrConfigGet",
         &wrCpssExMxPmFabricPktReasmPerSetCellCntrConfigGet,
         3, 0},
        {"cpssExMxPmFabricPktReasmPsmAgingSet",
         &wrCpssExMxPmFabricPktReasmPsmAgingSet,
         3, 0},
        {"cpssExMxPmFabricPktReasmPsmAgingGet",
         &wrCpssExMxPmFabricPktReasmPsmAgingGet,
         1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmFabricPktReasm
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
GT_STATUS cmdLibInitCpssExMxPmFabricPktReasm
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

