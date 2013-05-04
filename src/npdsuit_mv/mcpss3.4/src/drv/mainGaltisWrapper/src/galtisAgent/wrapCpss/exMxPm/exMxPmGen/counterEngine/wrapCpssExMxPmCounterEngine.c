/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmCounterEngine.c
*
* DESCRIPTION:
*       wrappers for cpssExMxPmCounterEngine.c
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
#include <cpss/extServices/private/prvCpssBindFunc.h>
#include <cpss/exMxPm/exMxPmGen/counterEngine/cpssExMxPmCounterEngine.h>

/*************************************************************************
* cpssExMxPmCounterEngineEnableSet
*
* DESCRIPTION:
*   The function enables\disables counter engine.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       enable      - GT_TRUE for enable, GT_FALSE for disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCounterEngineEnableSet
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

    devNum=(GT_U8)inArgs[0];
    enable=(GT_BOOL)inArgs[1];
    
    /* call cpss api function */
    result = cpssExMxPmCounterEngineEnableSet(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmCounterEngineEnableGet
*
* DESCRIPTION:
*   The function gets the status of the counter engine.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE for enable, GT_FALSE for disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCounterEngineEnableGet
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

    devNum=(GT_U8)inArgs[0];
    
    /* call cpss api function */
    result = cpssExMxPmCounterEngineEnableGet(devNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmCounterEngineClientEnableSet
*
* DESCRIPTION:
*   The function enables\disables counter engine client.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       client   - counter engine client
*       enable   - GT_TRUE for enable, GT_FALSE for disable
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCounterEngineClientEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_ENT   client;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    client=(CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_ENT)inArgs[1];
    enable=(GT_BOOL)inArgs[2];
    
    /* call cpss api function */
    result = cpssExMxPmCounterEngineClientEnableSet(devNum, client, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmCounterEngineClientEnableGet
*
* DESCRIPTION:
*   The function gets the counter engine client enabling state.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum   - device number
*       client   - counter engine client
*
* OUTPUTS:
*       enablePtr   - (pointer to) GT_TRUE for enable, GT_FALSE for disable
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCounterEngineClientEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_ENT   client;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    client=(CPSS_EXMXPM_COUNTER_ENGINE_CLIENT_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmCounterEngineClientEnableGet(devNum, client, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmCounterEngineCounterGet
*
* DESCRIPTION:
*   The function gets counter engine counter.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - device number
*       counterIndex    - counter index. range 0..4K-1.
*
* OUTPUTS:
*       counterPtr      - (pointer to) counter structure containing 
*                         packets and bytes counter values.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_OUT_OF_RANGE          - counter index value out of range
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Packets counter and bytes counter are 36 and 42 bits 
*       length respectively.
*       TxQ counter index value is built from:
*       bit[0]     - 0 for transmitted packets, 1 for dropped ones.
*       bits[2:1]  - DP
*       bits[5:3]  - TC
*       bits[11:6] - Virtual Port Number (62 for Null value).
*      
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCounterEngineCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 counterIndex;
    CPSS_EXMXPM_COUNTER_ENGINE_COUNTER_STC counter;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    counterIndex=(GT_U32)inArgs[1];
    
    /* call cpss api function */
    result = cpssExMxPmCounterEngineCounterGet(devNum, counterIndex, &counter);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", counter.bytesCounter.l[0],
                                              counter.bytesCounter.l[1],
                                              counter.packetsCounter.l[0],
                                              counter.packetsCounter.l[1]);

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmCounterEngineEccErrorInfoGet
*
* DESCRIPTION:
*   The function gets counter engine counter ECC error information.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       errorPtr    - (pointer to) ECC error information structure.
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_STATE             - on wrong HW value
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*      
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCounterEngineEccErrorInfoGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_COUNTER_ENGINE_ECC_INFO_STC error;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    
    /* call cpss api function */
    result = cpssExMxPmCounterEngineEccErrorInfoGet(devNum, &error);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", error.line,error.type);

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmCounterEngineEccConfigSet
*
* DESCRIPTION:
*   The function sets counter engine ECC behavior.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*       behavior    - disregard or skip
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmCounterEngineEccConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_COUNTER_ENGINE_ECC_BEHAVIOR_ENT     behavior;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    behavior=(CPSS_EXMXPM_COUNTER_ENGINE_ECC_BEHAVIOR_ENT)inArgs[1];
    
    /* call cpss api function */
    result = cpssExMxPmCounterEngineEccConfigSet(devNum, behavior);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*************************************************************************
* cpssExMxPmCounterEngineEccConfigGet
*
* DESCRIPTION:
*   The function gets counter engine ECC behavior.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum      - device number
*
* OUTPUTS:
*       behaviorPtr - (pointer to) disregard or skip
*
* RETURNS:
*       GT_OK                    - on success
*       GT_BAD_PARAM             - on wrong parameters
*       GT_HW_ERROR              - on hardware error
*       GT_BAD_PTR               - on null pointer
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       
*******************************************************************************/
GT_STATUS cpssExMxPmCounterEngineEccConfigGet
(
    IN  GT_U8                                           devNum,
    OUT CPSS_EXMXPM_COUNTER_ENGINE_ECC_BEHAVIOR_ENT     *behaviorPtr
);
static CMD_STATUS wrCpssExMxPmCounterEngineEccConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_COUNTER_ENGINE_ECC_BEHAVIOR_ENT behavior;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    
    /* call cpss api function */
    result = cpssExMxPmCounterEngineEccConfigGet(devNum, &behavior);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", behavior);

    return CMD_OK;
}

/**** database initialization **************************************/
static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPmCounterEngineEnableSet",
     &wrCpssExMxPmCounterEngineEnableSet,
     2, 0},

    {"cpssExMxPmCounterEngineEnableGet",
     &wrCpssExMxPmCounterEngineEnableGet,
     1, 0}, 

    {"cpssExMxPmCounterEngineClientEnableSet",
     &wrCpssExMxPmCounterEngineClientEnableSet,
     3, 0},

    {"cpssExMxPmCounterEngineClientEnableGet",
     &wrCpssExMxPmCounterEngineClientEnableGet,
     2, 0},

    {"cpssExMxPmCounterEngineCounterGet",
     &wrCpssExMxPmCounterEngineCounterGet,
     2, 0},

    {"cpssExMxPmCounterEngineEccErrorInfoGet",
     &wrCpssExMxPmCounterEngineEccErrorInfoGet,
     1, 0},

    {"cpssExMxPmCounterEngineEccConfigSet",
     &wrCpssExMxPmCounterEngineEccConfigSet,
     2, 0},

    {"cpssExMxPmCounterEngineEccConfigGet",
     &wrCpssExMxPmCounterEngineEccConfigGet,
     1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmCounterEngine
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
GT_STATUS cmdLibInitCpssExMxPmCounterEngine
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

