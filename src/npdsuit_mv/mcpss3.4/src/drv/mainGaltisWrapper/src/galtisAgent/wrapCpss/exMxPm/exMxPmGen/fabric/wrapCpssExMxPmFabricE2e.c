/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmFabricE2e.c
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
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricE2e.h>

/*******************************************************************************
* cpssExMxPmFabricE2eAdvertiseEnableSet
*
* DESCRIPTION:
*     This routine enables or disables advertising
*     of Packet Processor egress queues E2E status.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - the device number.
*       portNum         - Packet Processor port number
*                         use PSS_CPU_PORT_NUM_CNS for CPU port
*       tc              - Packet Processor Traffic Class [0..7]
*       enable          - GT_TRUE enable advertising,
*                         GT_FALSE - disable advertising
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, portNum or tc.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricE2eAdvertiseEnableSet
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
    enable = (GT_BOOL)inArgs[3];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmFabricE2eAdvertiseEnableSet(devNum, portNum, tc, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eAdvertiseEnableGet
*
* DESCRIPTION:
*     This routine gets enable/disable status of advertising
*     of Packet Processor egress queues E2E status.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       portNum        - Packet Processor port number
*                        use PSS_CPU_PORT_NUM_CNS for CPU port
*       tc             - Packet Processor Traffic Class [0..7]
*
* OUTPUTS:
*       enablePtr      - GT_TRUE enable advertising,
*                        GT_FALSE - disable advertising
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, portNum or tc.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricE2eAdvertiseEnableGet
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

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmFabricE2eAdvertiseEnableGet(devNum, portNum, tc, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eMsgConfigSet
*
* DESCRIPTION:
*     This routine Sets E2E message configuration
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - the device number.
*       msgConfigPtr        - E2E messages params
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on periodicCellFrequence or urgentCellFrequence out
*                           of range values.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricE2eMsgConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_E2E_MSG_CONFIG_STC msgConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    /*TODO: add code here. */

    devNum                          = (GT_U8)inArgs[0];
    msgConfig.e2eEnable             = (GT_BOOL)inArgs[1];
    msgConfig.urgentMode            =
        (CPSS_EXMXPM_FABRIC_E2E_URGENT_MODE_ENT)inArgs[2];
    msgConfig.urgentCellFrequence   = (GT_U32)inArgs[3];
    msgConfig.periodicCellFrequence = (GT_U32)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmFabricE2eMsgConfigSet(devNum, &msgConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eMsgConfigGet
*
* DESCRIPTION:
*     This routine Gets E2E message configuration
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - the device number.
*
* OUTPUTS:
*       msgConfigPtr        - E2E messages params
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
static CMD_STATUS wrCpssExMxPmFabricE2eMsgConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_E2E_MSG_CONFIG_STC msgConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    /*TODO: add code here. */

    devNum                          = (GT_U8)inArgs[0];
    /* zeros to trurn on fail */
    msgConfig.e2eEnable             = (GT_BOOL)0;
    msgConfig.urgentMode            =
        (CPSS_EXMXPM_FABRIC_E2E_URGENT_MODE_ENT)0;
    msgConfig.urgentCellFrequence   = (GT_U32)0;
    msgConfig.periodicCellFrequence = (GT_U32)0;

    /* call cpss api function */
    result = cpssExMxPmFabricE2eMsgConfigGet(devNum, &msgConfig);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d",
        msgConfig.e2eEnable, msgConfig.urgentMode,
        msgConfig.urgentCellFrequence,
        msgConfig.periodicCellFrequence);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eMsgPathSet
*
* DESCRIPTION:
*     This routine set path for E2E cells in the device
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - the device number.
*       msgPathPtr          - E2E messages path
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on xbar number, fport or fabric tc out of range values.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricE2eMsgPathSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_E2E_MSG_PATH_STC msgPath;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum              = (GT_U8) inArgs[0];
    msgPath.e2eVoq      = (GT_U32)inArgs[1];
    msgPath.e2eFabricTc = (GT_U32)inArgs[2];

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eMsgPathSet(devNum, &msgPath);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eMsgPathGet
*
* DESCRIPTION:
*     This routine get path for E2E cells in the device
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - the device number.
*
* OUTPUTS:
*       msgPathPtr        - E2E messages path
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
static CMD_STATUS wrCpssExMxPmFabricE2eMsgPathGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_E2E_MSG_PATH_STC msgPath;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum              = (GT_U8) inArgs[0];
    msgPath.e2eVoq      = (GT_U32)0;
    msgPath.e2eFabricTc = (GT_U32)0;

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eMsgPathGet(devNum, &msgPath);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d",
        msgPath.e2eVoq, msgPath.e2eFabricTc);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eMcStatusReportEnableSet
*
* DESCRIPTION:
*     This routine enables or disables the Multicast Status Reporting
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum - the device number.
*       enable - Multicast Status Reporting Enable (GT_TRUE) Disable (GT_FALSE)
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
static CMD_STATUS wrCpssExMxPmFabricE2eMcStatusReportEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum      = (GT_U8) inArgs[0];
    enable      = (GT_BOOL)inArgs[1];

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eMcStatusReportEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eMcStatusReportEnableGet
*
* DESCRIPTION:
*     This routine gets the enable or disable state of
*     the Multicast Status Reporting
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum     - the device number.
*
* OUTPUTS:
*       enablePtr  - (pointer to)Multicast Status Reporting
*                    Enable (GT_TRUE) Disable (GT_FALSE)
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
static CMD_STATUS wrCpssExMxPmFabricE2eMcStatusReportEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_BOOL   enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum      = (GT_U8) inArgs[0];
    enable      = GT_FALSE;

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eMcStatusReportEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eVoqDropEnableSet
*
* DESCRIPTION:
*     This routine enables/disables Voq packet drop by E2E FC status.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum     - the device number to set.
*       voqUnitNum - the VOQ Unit number, 0..1
*       enable     - GT_TRUE - enable drop of packets,
*                    GT_FALSE - disable drop of packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricE2eVoqDropEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    vocUnutNum;
    GT_BOOL   enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum      = (GT_U8)  inArgs[0];
    vocUnutNum  = (GT_U32) inArgs[1];
    enable      = (GT_BOOL)inArgs[2];

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eVoqDropEnableSet(devNum, vocUnutNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eVoqDropEnableGet
*
* DESCRIPTION:
*     This routine gets enable/disable state of Voq packet drop by E2E FC status.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum     - the device number to set.
*       voqUnitNum - the VOQ Unit number, 0..1
*
* OUTPUTS:
*       enablePtr  - (pointer to)
*                    GT_TRUE  - enable drop of packets,
*                    GT_FALSE - disable drop of packets
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or voqUnitNum.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricE2eVoqDropEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U32    vocUnitNum;
    GT_BOOL   enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum      = (GT_U8)  inArgs[0];
    vocUnitNum  = (GT_U32) inArgs[1];
    enable      = GT_FALSE;

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eVoqDropEnableGet(devNum, vocUnitNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eDevConfigSet
*
* DESCRIPTION:
*     Sets the E2E status message base offset and number of
*     virtual ports per device.
*     This configurations controls the advertisement area of each
*     device in the FC status message.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - the device number.
*       voqUnitNum          - the VOQ Unit number, 0..1
*       targetDevNum        - the target HW Device number
*       targetDevCfgPtr     - (pointer to )E2E target device configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, voqUnitNum,
*                                  targetDevCfgPtr members.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE          - entry index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricE2eDevConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    GT_U32                                         vocUnutNum;
    GT_U8                                          targetDevNum;
    CPSS_EXMXPM_FABRIC_E2E_TARGET_DEV_CONFIG_STC   targetDevCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum       = (GT_U8)  inArgs[0];
    vocUnutNum   = (GT_U32) inArgs[1];
    targetDevNum = (GT_U8)  inArgs[2];
    targetDevCfg.devPortsOffset = (GT_U32)inArgs[3];
    targetDevCfg.devStatusMsgProcessedSize =
        (CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_ENT)inArgs[4];
    targetDevCfg.devMaxPortId =   (GT_U32)inArgs[5];

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eDevConfigSet(
        devNum, vocUnutNum, targetDevNum, &targetDevCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eDevConfigGet
*
* DESCRIPTION:
*     Gets the E2E status message base offset and number of
*     virtual ports per device.
*     This configurations controls the advertisement area of each
*     device in the FC status message.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - the device number.
*       voqUnitNum          - the VOQ Unit number, 0..1
*       targetDevNum        - the target HW Device number
*
* OUTPUTS:
*       targetDevCfgPtr     - (pointer to )E2E target device configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, voqUnitNum
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE          - entry index out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricE2eDevConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    GT_U32                                         vocUnutNum;
    GT_U8                                          targetDevNum;
    CPSS_EXMXPM_FABRIC_E2E_TARGET_DEV_CONFIG_STC   targetDevCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum       = (GT_U8)  inArgs[0];
    vocUnutNum   = (GT_U32) inArgs[1];
    targetDevNum = (GT_U8)  inArgs[2];
    targetDevCfg.devPortsOffset = 0;
    targetDevCfg.devStatusMsgProcessedSize =
        CPSS_EXMXPM_FABRIC_E2E_STATUS_PROCESSED_SIZE_96B_E;
    targetDevCfg.devMaxPortId =  0;

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eDevConfigGet(
        devNum, vocUnutNum, targetDevNum, &targetDevCfg);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d",
        targetDevCfg.devPortsOffset,
        targetDevCfg.devStatusMsgProcessedSize,
        targetDevCfg.devMaxPortId);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eDropProbabilitySet
*
* DESCRIPTION:
*     Sets the Drop Probability of packets in the ingress VOQ in case
*     that scoreboard status indicates that the egress queue is partially
*     congested (state 1 or 2).
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                 - the device number.
*       voqUnitNum             - the VOQ Unit number, 0..1
*       e2eDropProbabilityPtr  - (pointer to ) Drop Probability
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE          - on out of range values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricE2eDropProbabilitySet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    GT_U32                                         vocUnutNum;
    CPSS_EXMXPM_FABRIC_E2E_DROP_PROBABILITY_STC    dropProbCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum       = (GT_U8)  inArgs[0];
    vocUnutNum   = (GT_U32) inArgs[1];
    dropProbCfg.state1Dp0DropProbability = (GT_U32)inArgs[2];
    dropProbCfg.state1Dp1DropProbability = (GT_U32)inArgs[3];
    dropProbCfg.state1Dp2DropProbability = (GT_U32)inArgs[4];
    dropProbCfg.state2Dp0DropProbability = (GT_U32)inArgs[5];
    dropProbCfg.state2Dp1DropProbability = (GT_U32)inArgs[6];
    dropProbCfg.state2Dp2DropProbability = (GT_U32)inArgs[7];

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eDropProbabilitySet(
        devNum, vocUnutNum, &dropProbCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eDropProbabilityGet
*
* DESCRIPTION:
*     Gets the Drop Probability of packets in the ingress VOQ in case
*     that scoreboard status indicates that the egress queue is partially
*     congested (state 1 or 2).
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                 - the device number.
*       voqUnitNum             - the VOQ Unit number, 0..1
*
* OUTPUTS:
*       e2eDropProbabilityPtr  - (pointer to ) Drop Probability
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
static CMD_STATUS wrCpssExMxPmFabricE2eDropProbabilityGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                      result;
    GT_U8                                          devNum;
    GT_U32                                         vocUnutNum;
    CPSS_EXMXPM_FABRIC_E2E_DROP_PROBABILITY_STC    dropProbCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum       = (GT_U8)  inArgs[0];
    vocUnutNum   = (GT_U32) inArgs[1];
    dropProbCfg.state1Dp0DropProbability = 0;
    dropProbCfg.state1Dp1DropProbability = 0;
    dropProbCfg.state1Dp2DropProbability = 0;
    dropProbCfg.state2Dp0DropProbability = 0;
    dropProbCfg.state2Dp1DropProbability = 0;
    dropProbCfg.state2Dp2DropProbability = 0;

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eDropProbabilityGet(
        devNum, vocUnutNum, &dropProbCfg);

    /* pack output arguments to galtis string */
    galtisOutput(
        outArgs, result, "%d%d%d%d%d%d",
        dropProbCfg.state1Dp0DropProbability,
        dropProbCfg.state1Dp1DropProbability,
        dropProbCfg.state1Dp2DropProbability,
        dropProbCfg.state2Dp0DropProbability,
        dropProbCfg.state2Dp1DropProbability,
        dropProbCfg.state2Dp2DropProbability);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricE2eScoreBoardDumpGet
*
* DESCRIPTION:
*     Gets the specified segment of scoreboard.
*     The ScoreBoard is 64-entries table.
*     Each entry contains states of 16 TC queues.
*     The first entry for device contains the CPU port state only.
*     Other entries contain state of 8 TC queues for each of two ports.
*     The ExMxPm device configured to map the target device E2E status to
*     part of this table by cpssExMxPmFabricE2eDevConfigSet
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum            - the device number.
*       voqUnitNum        - the VOQ Unit number, 0..1
*       startEntryIndex   - the start index of entry array
*       entryArrSize      - size of entry array
*
* OUTPUTS:
*       entryArrPtr       - (pointer to )array of E2E Scoreboard
*                           entry structures
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or voqUnitNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE          - entry index out of range:
*                                  startEntryIndex, entryArrSize
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricE2eScoreBoardDumpGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS       result;
    GT_U8           devNum;
    GT_U32          vocUnutNum;
    GT_U32          startEntryIndex;
    GT_U32          entryArrSize;
    static CPSS_EXMXPM_FABRIC_E2E_PORT_TC_STATUS_ENTRY_STC e2eStatus[32];
    GT_U32          i;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum           = (GT_U8)  inArgs[0];
    vocUnutNum       = (GT_U32) inArgs[1];
    startEntryIndex  = (GT_U32) inArgs[2];
    entryArrSize     = (GT_U32) inArgs[3];

    if (entryArrSize > 32)
    {
        entryArrSize = 32;
    }

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricE2eScoreBoardDumpGet(
        devNum, vocUnutNum, startEntryIndex, entryArrSize, e2eStatus);


    for (i = 0; (i < entryArrSize); i++)
    {
        if (i != 0)
        {
            fieldOutputSetAppendMode();
        }

        fieldOutput(
            "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
            e2eStatus[i].status[0], e2eStatus[i].status[1],
            e2eStatus[i].status[2], e2eStatus[i].status[3],
            e2eStatus[i].status[4], e2eStatus[i].status[5],
            e2eStatus[i].status[6], e2eStatus[i].status[7],
            e2eStatus[i].status[8], e2eStatus[i].status[9],
            e2eStatus[i].status[10], e2eStatus[i].status[11],
            e2eStatus[i].status[12], e2eStatus[i].status[13],
            e2eStatus[i].status[14], e2eStatus[i].status[15]);
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/**** database initialization **************************************/


static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmFabricE2eAdvertiseEnableSet",
         &wrCpssExMxPmFabricE2eAdvertiseEnableSet,
         4, 0},
        {"cpssExMxPmFabricE2eAdvertiseEnableGet",
         &wrCpssExMxPmFabricE2eAdvertiseEnableGet,
         3, 0},
        {"cpssExMxPmFabricE2eMsgConfigSet",
         &wrCpssExMxPmFabricE2eMsgConfigSet,
         5, 0},
        {"cpssExMxPmFabricE2eMsgConfigGet",
         &wrCpssExMxPmFabricE2eMsgConfigGet,
         1, 0},
        {"cpssExMxPmFabricE2eMsgPathSet",
         &wrCpssExMxPmFabricE2eMsgPathSet,
         3, 0},
        {"cpssExMxPmFabricE2eMsgPathGet",
         &wrCpssExMxPmFabricE2eMsgPathGet,
         1, 0},
        {"cpssExMxPmFabricE2eMcStatusReportEnableSet",
         &wrCpssExMxPmFabricE2eMcStatusReportEnableSet,
         2, 0},
        {"cpssExMxPmFabricE2eMcStatusReportEnableGet",
         &wrCpssExMxPmFabricE2eMcStatusReportEnableGet,
         1, 0},
        {"cpssExMxPmFabricE2eVoqDropEnableSet",
         &wrCpssExMxPmFabricE2eVoqDropEnableSet,
         3, 0},
        {"cpssExMxPmFabricE2eVoqDropEnableGet",
         &wrCpssExMxPmFabricE2eVoqDropEnableGet,
         2, 0},
        {"cpssExMxPmFabricE2eDevConfigSet",
         &wrCpssExMxPmFabricE2eDevConfigSet,
         6, 0},
        {"cpssExMxPmFabricE2eDevConfigGet",
         &wrCpssExMxPmFabricE2eDevConfigGet,
         3, 0},
        {"cpssExMxPmFabricE2eDropProbabilitySet",
         &wrCpssExMxPmFabricE2eDropProbabilitySet,
         8, 0},
        {"cpssExMxPmFabricE2eDropProbabilityGet",
         &wrCpssExMxPmFabricE2eDropProbabilityGet,
         2, 0},
        {"cpssExMxPmFabricE2eScoreBoardDumpGet",
         &wrCpssExMxPmFabricE2eScoreBoardDumpGet,
         4, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmFabricE2e
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
GT_STATUS cmdLibInitCpssExMxPmFabricE2e
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

