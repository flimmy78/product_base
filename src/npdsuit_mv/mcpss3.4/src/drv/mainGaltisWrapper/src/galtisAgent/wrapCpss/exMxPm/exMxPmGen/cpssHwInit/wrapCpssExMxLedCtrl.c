/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmLedCtrl.c
*
* DESCRIPTION:
*       Wrapper functions for cpssExMxPmLedCtrl cpss.exMxPm functions
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
#include <cpss/exMxPm/exMxPmGen/cpssHwInit/cpssExMxPmLedCtrl.h>

/*******************************************************************************
* cpssExMxPmLedStreamConfigSet
*
* DESCRIPTION:
*       This routine configures the LED stream.
*
* APPLICABLE DEVICES:  All ExMxPm Devices
*
* INPUTS:
*       devNum          - physical device number
*       interfaceNum    - LED stream interface number, range 0-1.
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmLedStreamConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 interfaceNum;
    CPSS_LED_CONF_STC ledConf;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum =                    (GT_U8)inArgs[0];
    interfaceNum =              (GT_U32)inArgs[1];
    ledConf.ledStart =          (GT_U32)inArgs[2];
    ledConf.ledEnd =            (GT_U32)inArgs[3];
    ledConf.pulseStretch =      (CPSS_LED_PULSE_STRETCH_ENT)inArgs[4];
    ledConf.blink0Duration =    (CPSS_LED_BLINK_DURATION_ENT)inArgs[5];
    ledConf.blink0DutyCycle =   (CPSS_LED_BLINK_DUTY_CYCLE_ENT)inArgs[6];
    ledConf.blink1Duration =    (CPSS_LED_BLINK_DURATION_ENT)inArgs[7];
    ledConf.blink1DutyCycle =   (CPSS_LED_BLINK_DUTY_CYCLE_ENT)inArgs[8];
    ledConf.disableOnLinkDown = (GT_BOOL)inArgs[9];
    ledConf.ledOrganize =       (CPSS_LED_ORDER_MODE_ENT)inArgs[10];
    ledConf.clkInvert =         (GT_BOOL)inArgs[11];
    ledConf.class5select =      (CPSS_LED_CLASS_5_SELECT_ENT)inArgs[12];
    ledConf.class13select =     (CPSS_LED_CLASS_13_SELECT_ENT)inArgs[13];

    /* call cpss api function */
    result = cpssExMxPmLedStreamConfigSet(devNum, interfaceNum, &ledConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmLedStreamConfigGet
*
* DESCRIPTION:
*       This routine gets the LED stream configuration.
*
* APPLICABLE DEVICES:  All ExMxPm Devices
*
* INPUTS:
*       devNum          - physical device number
*       interfaceNum    - LED stream interface number, range 0-1.
*
* OUTPUTS:
*       ledConfPtr      - (pointer to) LED stream configuration parameters.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad hardware value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmLedStreamConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 interfaceNum;
    CPSS_LED_CONF_STC ledConf;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum =        (GT_U8)inArgs[0];
    interfaceNum =  (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmLedStreamConfigGet(devNum, interfaceNum, &ledConf);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d%d%d%d%d%d", ledConf.ledStart, ledConf.ledEnd, 
                 ledConf.pulseStretch, ledConf.blink0Duration ,ledConf.blink0DutyCycle ,
                 ledConf.blink1Duration ,ledConf.blink1DutyCycle ,ledConf.disableOnLinkDown ,
                 ledConf.ledOrganize ,ledConf.clkInvert ,ledConf.class5select ,ledConf.class13select);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmLedStreamClassManipulationSet
*
* DESCRIPTION:
*       This routine configures a LED stream class manipulation.
*
* APPLICABLE DEVICES:  All ExMxPm Devices
*
* INPUTS:
*       devNum          - physical device number
*       interfaceNum    - LED stream interface number, range 0-1.
*       classNum        - class number,
*                         For GE devices - range 0..6
*                         For XG devices - range 0..11
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - one of the parameters is out of range
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmLedStreamClassManipulationSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 interfaceNum;
    GT_U32 classNum;
    CPSS_LED_CLASS_MANIPULATION_STC classParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum                      = (GT_U8)inArgs[0];
    interfaceNum                = (GT_U32)inArgs[1];
    classNum                    = (GT_U32)inArgs[2];
    classParams.invertEnable    = (GT_BOOL)inArgs[3];
    classParams.blinkEnable     = (GT_BOOL)inArgs[4];
    classParams.blinkSelect     = (CPSS_LED_PULSE_STRETCH_ENT)inArgs[5];
    classParams.forceEnable     = (GT_BOOL)inArgs[6];
    classParams.forceData       = (GT_U32)inArgs[7];
    
    /* call cpss api function */
    result = cpssExMxPmLedStreamClassManipulationSet(devNum, interfaceNum, classNum, &classParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmLedStreamClassManipulationGet
*
* DESCRIPTION:
*       This routine gets a LED stream class manipulation configuration.
*
* APPLICABLE DEVICES:  All ExMxPm Devices
*
* INPUTS:
*       devNum          - physical device number
*       interfaceNum    - LED stream interface number, range 0-1.
*       classNum        - class number,
*                         For GE devices - range 0..6
*                         For XG devices - range 0..11
*
* OUTPUTS:
*       classParamsPtr  - (pointer to) class manipulation parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmLedStreamClassManipulationGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 interfaceNum;
    GT_U32 classNum;
    CPSS_LED_CLASS_MANIPULATION_STC classParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum          = (GT_U8)inArgs[0];
    interfaceNum    = (GT_U32)inArgs[1];
    classNum        = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmLedStreamClassManipulationGet(devNum, interfaceNum, classNum, &classParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d", classParams.invertEnable, classParams.blinkEnable,
                 classParams.blinkSelect, classParams.forceEnable ,classParams.forceData);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmLedStreamGroupConfigSet
*
* DESCRIPTION:
*       This routine configures a LED stream group.
*
* APPLICABLE DEVICES:  All ExMxPm Devices
*
* INPUTS:
*       devNum          - physical device number
*       interfaceNum    - LED stream interface number, range 0-1.
*       groupNum        - group number,
*                         For GE devices - range 0..3
*                         For XG devices - range 0..1
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_OUT_OF_RANGE          - on ou of range class number
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmLedStreamGroupConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 interfaceNum;
    GT_U32 groupNum;
    CPSS_LED_GROUP_CONF_STC groupParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum              = (GT_U8)inArgs[0];
    interfaceNum        = (GT_U32)inArgs[1];
    groupNum            = (GT_U32)inArgs[2];
    groupParams.classA  = (GT_BOOL)inArgs[3];
    groupParams.classB  = (GT_BOOL)inArgs[4];
    groupParams.classC  = (GT_BOOL)inArgs[5];
    groupParams.classD  = (GT_BOOL)inArgs[6];

    /* call cpss api function */
    result = cpssExMxPmLedStreamGroupConfigSet(devNum, interfaceNum, groupNum, &groupParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmLedStreamGroupConfigGet
*
* DESCRIPTION:
*       This routine gets a LED stream group configuration.
*
* APPLICABLE DEVICES:  All ExMxPm Devices
*
* INPUTS:
*       devNum          - physical device number
*       interfaceNum    - LED stream interface number, range 0-1.
*       groupNum        - group number,
*                         For GE devices - range 0..3
*                         For XG devices - range 0..1
*
* OUTPUTS:
*       groupParamsPtr  - (pointer to) group configuration parameters
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmLedStreamGroupConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 interfaceNum;
    GT_U32 groupNum;
    CPSS_LED_GROUP_CONF_STC groupParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum          = (GT_U8)inArgs[0];
    interfaceNum    = (GT_U32)inArgs[1];
    groupNum        = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmLedStreamGroupConfigGet(devNum, interfaceNum, groupNum, &groupParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", groupParams.classA, groupParams.classB,
                                              groupParams.classC, groupParams.classD);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmLedStreamXgLaneConfigSet
*
* DESCRIPTION:
*       This routine configures the indication used for XG-PCS lane LED.
*
* APPLICABLE DEVICES:  All ExMxPm Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - XG port number (0 XG port reflected on LED interface 0,
*                                 1 XG port reflected on LED interface 1).
*       laneNum - lane number, range 0..3
*       ledNum  - LED number for control (0 or 1).
*       ledData - Data reflected by the LED.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmLedStreamXgLaneConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 portNum;
    GT_U32 laneNum;
    GT_U32 ledNum;
    CPSS_LED_DATA_ENT ledData;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U32)inArgs[1];
    laneNum = (GT_U32)inArgs[2];
    ledNum  = (GT_U32)inArgs[3];
    ledData = (CPSS_LED_DATA_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmLedStreamXgLaneConfigSet(devNum, portNum, laneNum, ledNum, ledData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmLedStreamXgLaneConfigGet
*
* DESCRIPTION:
*       This routine gets the indication used for XG-PCS lane LED.
*
* APPLICABLE DEVICES:  All ExMxPm Devices
*
* INPUTS:
*       devNum  - physical device number
*       portNum - XG port number (0 XG port reflected on LED interface 0,
*                                 1 XG port reflected on LED interface 1).
*       laneNum - lane number, range 0..3
*       ledNum  - LED number for control (0 or 1).
*
* OUTPUTS:
*       ledDataPtr - (pointer to) Data reflected by the LED.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on bad hardware value
*       GT_BAD_PTR               - one of the parameters is NULL pointer
*       GT_BAD_PARAM             - one of the parameters value is wrong
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*       GT_FAIL                  - otherwise
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmLedStreamXgLaneConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 portNum;
    GT_U32 laneNum;
    GT_U32 ledNum;
    CPSS_LED_DATA_ENT ledData;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum  = (GT_U8)inArgs[0];
    portNum = (GT_U32)inArgs[1];
    laneNum = (GT_U32)inArgs[2];
    ledNum  = (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmLedStreamXgLaneConfigGet(devNum, portNum, laneNum, ledNum, &ledData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", ledData);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        
        {"cpssExMxPmLedStreamConfigSet",
         &wrCpssExMxPmLedStreamConfigSet,
         14, 0},
        {"cpssExMxPmLedStreamConfigGet",
         &wrCpssExMxPmLedStreamConfigGet,
         2, 0},
        {"cpssExMxPmLedStreamClassManipulationSet",
         &wrCpssExMxPmLedStreamClassManipulationSet,
         8, 0},
        {"cpssExMxPmLedStreamClassManipulationGet",
         &wrCpssExMxPmLedStreamClassManipulationGet,
         3, 0},
        {"cpssExMxPmLedStreamGroupConfigSet",
         &wrCpssExMxPmLedStreamGroupConfigSet,
         7, 0},
        {"cpssExMxPmLedStreamGroupConfigGet",
         &wrCpssExMxPmLedStreamGroupConfigGet,
         3, 0},
        {"cpssExMxPmLedStreamXgLaneConfigSet",
         &wrCpssExMxPmLedStreamXgLaneConfigSet,
         5, 0},
        {"cpssExMxPmLedStreamXgLaneConfigGet",
         &wrCpssExMxPmLedStreamXgLaneConfigGet,
         4, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmLedCtrl
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
GT_STATUS cmdLibInitCpssExMxPmLedCtrl
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

