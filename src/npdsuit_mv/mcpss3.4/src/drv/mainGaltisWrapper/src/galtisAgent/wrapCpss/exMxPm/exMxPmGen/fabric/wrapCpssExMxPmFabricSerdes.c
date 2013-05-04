/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmFabricSerdes.c
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
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricSerdes.h>

/*******************************************************************************
* cpssExMxPmSerdesStatusGet
*
* DESCRIPTION:
*     This routine gets fabric port SERDES lanes status.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - the device number.
*       fport           - only fabric ports 0-3 supported.
*
* OUTPUTS:
*       serdesStatusPtr - (pointer to) fabric port SERDES status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesStatusGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_FPORT fport;
    CPSS_EXMXPM_FABRIC_SERDES_STATUS_STC serdesStatus;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    
    /* call cpss api function */
    result = cpssExMxPmSerdesStatusGet(devNum, fport, &serdesStatus);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d",
                 serdesStatus.pllLocked[0],
                 serdesStatus.pllLocked[1],
                 serdesStatus.pllLocked[2],
                 serdesStatus.pllLocked[3],
                 serdesStatus.pllLocked[4],
                 serdesStatus.pllLocked[5],
                 serdesStatus.cqOutConnected[0],
                 serdesStatus.cqOutConnected[1],
                 serdesStatus.cqOutConnected[2],
                 serdesStatus.cqOutConnected[3],
                 serdesStatus.cqOutConnected[4],
                 serdesStatus.cqOutConnected[5]
                 );

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesResetSet
*
* DESCRIPTION:
*     This routine sets fabric port SERDES lanes to be in
*     Reset or not
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                - the device number.
*       fport               - only fabric ports 0-3 supported.
*       resetSerdesLaneArr  - fabric port SERDES lanes Reset state:
*                             GT_TRUE - Reset lane
*                             GT_FALSE - Normal operation
*                             lane's number is index in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesResetSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_FPORT fport;
    GT_BOOL resetSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    resetSerdesLaneArr[0] = (GT_BOOL)inArgs[2];
    resetSerdesLaneArr[1] = (GT_BOOL)inArgs[3];
    resetSerdesLaneArr[2] = (GT_BOOL)inArgs[4];
    resetSerdesLaneArr[3] = (GT_BOOL)inArgs[5];
    resetSerdesLaneArr[4] = (GT_BOOL)inArgs[6];
    resetSerdesLaneArr[5] = (GT_BOOL)inArgs[7];

    /* call cpss api function */
    result = cpssExMxPmSerdesResetSet(devNum, fport, resetSerdesLaneArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesResetGet
*
* DESCRIPTION:
*     This routine gets fabric port SERDES lanes state to be in
*     Reset or not
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - the device number.
*       fport           - only fabric port 0-3 supported.
*
* OUTPUTS:
*       resetSerdesLaneArr    - fabric port SERDES lanes Reset state:
*                               GT_TRUE - Reset lane
*                               GT_FALSE - Normal operation
*                               lane's number is index in the array
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesResetGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_FPORT fport;
    GT_BOOL resetSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    
    /* call cpss api function */
    result = cpssExMxPmSerdesResetGet(devNum, fport, resetSerdesLaneArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d",
                 resetSerdesLaneArr[0],
                 resetSerdesLaneArr[1],
                 resetSerdesLaneArr[2],
                 resetSerdesLaneArr[3],
                 resetSerdesLaneArr[4],
                 resetSerdesLaneArr[5]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesPowerSet
*
* DESCRIPTION:
*     This routine sets fabric port SERDES lanes Power state (Up or Down).
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                - the device number.
*       fport                 - only fabric ports 0-3 supported.
*       powerSerdesLaneArr    - fabric port SERDES lanes Power state:
*                               GT_TRUE - Power Up
*                               GT_FALSE - Power Down
*                               lane's number is index in the array.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesPowerSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_FPORT fport;
    GT_BOOL powerSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];
    

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    powerSerdesLaneArr[0] = (GT_BOOL)inArgs[2];
    powerSerdesLaneArr[1] = (GT_BOOL)inArgs[3];
    powerSerdesLaneArr[2] = (GT_BOOL)inArgs[4];
    powerSerdesLaneArr[3] = (GT_BOOL)inArgs[5];
    powerSerdesLaneArr[4] = (GT_BOOL)inArgs[6];
    powerSerdesLaneArr[5] = (GT_BOOL)inArgs[7];

    /* call cpss api function */
    result = cpssExMxPmSerdesPowerSet(devNum, fport, powerSerdesLaneArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesPowerGet
*
* DESCRIPTION:
*     This routine gets state of fabric port SERDES lanes Power (Up or Down).
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - the device number.
*       fport           - only fabric port 0-3 supported.
*
* OUTPUTS:
*       powerSerdesLaneArr  - fabric port SERDES lanes Power state:
*                               GT_TRUE - Power Up
*                               GT_FALSE - Power Down
*                               lane's number is index in the array
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - if Tx power state not equal to Rx power state
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesPowerGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_FPORT fport;
    GT_BOOL powerSerdesLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSerdesPowerGet(devNum, fport, powerSerdesLaneArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d",
                 powerSerdesLaneArr[0],
                 powerSerdesLaneArr[1],
                 powerSerdesLaneArr[2],
                 powerSerdesLaneArr[3],
                 powerSerdesLaneArr[4],
                 powerSerdesLaneArr[5]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesLoopbackEnableSet
*
* DESCRIPTION:
*       This routine enables or disables loopback on fabric port SERDES lanes.     
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                - the device number.
*       fport                 - only fabric ports 0-3 supported.
*       loopbackEnableArr     - loopback status of fabric port SERDES Rx lanes:
*                               GT_TRUE - Loopback mode
*                               GT_FALSE - Normal mode
*                               
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesLoopbackEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_FPORT  fport;
    GT_BOOL   loopbackEnableArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    loopbackEnableArr[0] = (GT_BOOL)inArgs[2];
    loopbackEnableArr[1] = (GT_BOOL)inArgs[3];
    loopbackEnableArr[2] = (GT_BOOL)inArgs[4];
    loopbackEnableArr[3] = (GT_BOOL)inArgs[5];
    loopbackEnableArr[4] = (GT_BOOL)inArgs[6];
    loopbackEnableArr[5] = (GT_BOOL)inArgs[7];

    /* call cpss api function */
    result = cpssExMxPmSerdesLoopbackEnableSet(devNum, fport, loopbackEnableArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesLoopbackEnableGet
*
* DESCRIPTION:
*       This routine gets state of loopback mode on fabric port SERDES lanes.     
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                - the device number.
*       fport                 - only fabric ports 0-3 supported.
*
* OUTPUTS:
*       loopbackEnableArr     - loopback status of fabric port SERDES Rx lanes:
*                               GT_TRUE - Loopback mode
*                               GT_FALSE - Normal mode
*                               
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesLoopbackEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_FPORT  fport;
    GT_BOOL   loopbackEnableArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSerdesLoopbackEnableGet(devNum, fport, loopbackEnableArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d",
                 loopbackEnableArr[0],
                 loopbackEnableArr[1],
                 loopbackEnableArr[2],
                 loopbackEnableArr[3],
                 loopbackEnableArr[4],
                 loopbackEnableArr[5]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesTxVoltageSwingCfgSet
*
* DESCRIPTION:
*     This routine sets fabric port SERDES lanes Transmit Voltage Swing Configuration.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                - the device number.
*       fport                 - only fabric ports 0-3 supported.
*       laneNum               - SERDES lane number 0-5
*       txVltgSwingCfgPtr     - pointer to voltage swing parameters
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport or laneNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesTxVoltageSwingCfgSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_FPORT  fport;
    GT_U32    laneNum;
    CPSS_EXMXPM_FABRIC_SERDES_TX_VLTG_SWING_CFG_STC   txVltgSwingCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    laneNum = (GT_U32) inArgs[2];
    txVltgSwingCfg.txAmp                     = (GT_U32) inArgs[3];
    txVltgSwingCfg.emphasisType              = (CPSS_EXMXPM_FABRIC_SERDES_EMPHASIS_TYPE_ENT) inArgs[4];
    txVltgSwingCfg.txEmphasisEnable          = (GT_BOOL) inArgs[5];
    txVltgSwingCfg.emphasisLevelAdjustEnable = (GT_BOOL) inArgs[6];
    txVltgSwingCfg.txEmphasisAmp             = (GT_U32) inArgs[7];

    /* call cpss api function */
    result = cpssExMxPmSerdesTxVoltageSwingCfgSet(devNum, fport, laneNum, &txVltgSwingCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesTxVoltageSwingCfgGet
*
* DESCRIPTION:
*     This routine gets state of Transmit Voltage Swing Configuration.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                - the device number.
*       fport                 - only fabric ports 0-3 supported.
*       laneNum               - SERDES lane number 0-5
* OUTPUTS:
*       txVltgSwingCfgPtr     - pointer to voltage swing parameters
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport or laneNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesTxVoltageSwingCfgGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_FPORT    fport;
    GT_U32      laneNum;
    CPSS_EXMXPM_FABRIC_SERDES_TX_VLTG_SWING_CFG_STC   txVltgSwingCfg;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    laneNum = (GT_U32) inArgs[2];
    
    /* call cpss api function */
    result = cpssExMxPmSerdesTxVoltageSwingCfgGet(devNum, fport, laneNum, &txVltgSwingCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d",
                 txVltgSwingCfg.txAmp,
                 txVltgSwingCfg.emphasisType,
                 txVltgSwingCfg.txEmphasisEnable,
                 txVltgSwingCfg.emphasisLevelAdjustEnable,
                 txVltgSwingCfg.txEmphasisAmp);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesTxSwpSelSet
*
* DESCRIPTION:
*     This routine sets the connection between PCS Tx lanes and SERDES Tx lanes.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - the device number.
*       fport           - HyperG.Link port. (0 - 3)
*       pcsTxLaneArr    - array of PCS TX lane's numbers. 
*                         The index is SERDES lane and the value is PCS TX lane
*                         number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_OUT_OF_RANGE          - on wrong lane value from pcsTxLaneArr
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesTxSwpSelSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_FPORT    fport;
    GT_U32      pcsTxLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    pcsTxLaneArr[0] = (GT_U32) inArgs[2];
    pcsTxLaneArr[1] = (GT_U32) inArgs[3];
    pcsTxLaneArr[2] = (GT_U32) inArgs[4];
    pcsTxLaneArr[3] = (GT_U32) inArgs[5];
    pcsTxLaneArr[4] = (GT_U32) inArgs[6];
    pcsTxLaneArr[5] = (GT_U32) inArgs[7];

    /* call cpss api function */
    result = cpssExMxPmSerdesTxSwpSelSet(devNum, fport, pcsTxLaneArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesTxSwpSelGet
*
* DESCRIPTION:
*       This routine gets the connection between PCS Tx lanes and SERDES Tx 
*       lanes.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - the device number.
*       fport           - HyperG.Link port. (0 - 3)
*
* OUTPUTS:
*       pcsTxLaneArr    - array of PCS TX lane's numbers. 
*                         The index is SERDES lane and the value is PCS TX lane
*                         number.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesTxSwpSelGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_FPORT    fport;
    GT_U32      pcsTxLaneArr[CPSS_EXMXPM_FABRIC_SERDES_LANES_NUM_CNS];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmSerdesTxSwpSelGet(devNum, fport, pcsTxLaneArr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d",
                 pcsTxLaneArr[0],
                 pcsTxLaneArr[1],
                 pcsTxLaneArr[2],
                 pcsTxLaneArr[3],
                 pcsTxLaneArr[4],
                 pcsTxLaneArr[5]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesSpeedSet
*
* DESCRIPTION:
*     This routine configures fabric port SERDES speed
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - the device number.
*       fport               - HyperG.Link port. (0 - 3)
*       refClk              - reference clock frequency
*       speed               - port SERDES speed
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport, speed or refClk
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesSpeedSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_FPORT    fport;
    CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_TYPE_ENT refClk;
    CPSS_EXMXPM_FABRIC_SERDES_SPEED_TYPE_ENT speed;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    refClk = (CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_TYPE_ENT) inArgs[2];
    speed = (CPSS_EXMXPM_FABRIC_SERDES_SPEED_TYPE_ENT) inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmSerdesSpeedSet(devNum, fport, refClk, speed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmSerdesSpeedGet
*
* DESCRIPTION:
*     This routine gets fabric port SERDES speed.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - the device number.
*       fport           - HyperG.Link port. (0 - 3)
*       refClk          - reference clock frequency
*
* OUTPUTS:
*       speedPtr        - (pointer to) fabric port SERDES speed:
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport or refClk
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_BAD_STATE             - result speed differs from 3.125 Gbps or 3.333 Gbps
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmSerdesSpeedGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;
    GT_U8       devNum;
    GT_FPORT    fport;
    CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_TYPE_ENT refClk;
    CPSS_EXMXPM_FABRIC_SERDES_SPEED_TYPE_ENT speed;

        /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    refClk = (CPSS_EXMXPM_FABRIC_SERDES_REFERENCE_CLOCK_TYPE_ENT) inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmSerdesSpeedGet(devNum, fport, refClk, &speed);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",
                 speed);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmSerdesStatusGet",
         &wrCpssExMxPmSerdesStatusGet,
         2, 0},
        {"cpssExMxPmSerdesResetSet",
         &wrCpssExMxPmSerdesResetSet,
         8, 0},
        {"cpssExMxPmSerdesResetGet",
         &wrCpssExMxPmSerdesResetGet,
         2, 0},
        {"cpssExMxPmSerdesPowerSet",
         &wrCpssExMxPmSerdesPowerSet,
         8, 0},
        {"cpssExMxPmSerdesPowerGet",
         &wrCpssExMxPmSerdesPowerGet,
         2, 0},
        {"cpssExMxPmSerdesLoopbackEnableSet",
         &wrCpssExMxPmSerdesLoopbackEnableSet,
         8, 0},
        {"cpssExMxPmSerdesLoopbackEnableGet",
         &wrCpssExMxPmSerdesLoopbackEnableGet,
         2, 0},
        {"cpssExMxPmSerdesTxVoltageSwingCfgSet",
         &wrCpssExMxPmSerdesTxVoltageSwingCfgSet,
         8, 0},
        {"cpssExMxPmSerdesTxVoltageSwingCfgGet",
         &wrCpssExMxPmSerdesTxVoltageSwingCfgGet,
         3, 0},
        {"cpssExMxPmSerdesTxSwpSelSet",
         &wrCpssExMxPmSerdesTxSwpSelSet,
         8, 0},
        {"cpssExMxPmSerdesTxSwpSelGet",
         &wrCpssExMxPmSerdesTxSwpSelGet,
         2, 0},
        {"cpssExMxPmSerdesSpeedSet",
         &wrCpssExMxPmSerdesSpeedSet,
         4, 0},
        {"cpssExMxPmSerdesSpeedGet",
         &wrCpssExMxPmSerdesSpeedGet,
         3, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmFabricSerdes
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
GT_STATUS cmdLibInitCpssExMxPmFabricSerdes
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

