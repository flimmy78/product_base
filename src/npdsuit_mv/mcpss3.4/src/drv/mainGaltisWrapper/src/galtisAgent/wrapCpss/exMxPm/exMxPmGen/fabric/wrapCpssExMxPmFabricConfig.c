/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmFabricConfig.c
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
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricConfig.h>

/*******************************************************************************
* cpssExMxPmFabricConfigVidxSet
*
* DESCRIPTION:
*       Set the Fabric VIDX configuration
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number
*       vidxConfigPtr  - (pointer to) Fabric VIDX configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or Fabric VIDX configuration.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on fabricVIDX0, fabricVIDX1 out of range values.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricConfigVidxSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_CONFIG_VIDX_STC vidxConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum                             = (GT_U8)inArgs[0];
    vidxConfig.fabricVidxBit0IsLbiBit0 = (GT_BOOL)inArgs[1];
    vidxConfig.fabricVIDX0             = (GT_U16)inArgs[2];
    vidxConfig.fabricVIDX1             = (GT_U16)inArgs[3];
    vidxConfig.fabricVIDXMode          = (CPSS_EXMXPM_FABRIC_CONFIG_VIDX_MODE_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmFabricConfigVidxSet(devNum, &vidxConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricConfigVidxGet
*
* DESCRIPTION:
*      Get the Fabric VIDX configuration
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       vidxConfigPtr  - (pointer to) Fabric VIDX configuration
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
static CMD_STATUS wrCpssExMxPmFabricConfigVidxGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_CONFIG_VIDX_STC vidxConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricConfigVidxGet(devNum, &vidxConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", vidxConfig.fabricVidxBit0IsLbiBit0, vidxConfig.fabricVIDX0,
                                                vidxConfig.fabricVIDX1, vidxConfig.fabricVIDXMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricConfigLbiModeSet
*
* DESCRIPTION:
*       Set the LBI assignment mode
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum   - the device number
*       lbiMode  - LBI assignment mode
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or LBI mode.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricConfigLbiModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_CONFIG_LBI_MODE_ENT lbiMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    lbiMode = (CPSS_EXMXPM_FABRIC_CONFIG_LBI_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricConfigLbiModeSet(devNum, lbiMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricConfigLbiModeGet
*
* DESCRIPTION:
*       Get the LBI assignment mode
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum      - the device number
*
* OUTPUTS:
*       lbiModePtr  - LBI assignment mode
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
static CMD_STATUS wrCpssExMxPmFabricConfigLbiModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_CONFIG_LBI_MODE_ENT lbiMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricConfigLbiModeGet(devNum, &lbiMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", lbiMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricConfigSrcPortToLbiMapSet
*
* DESCRIPTION:
*
*     Set LBI value per source port.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum   - the device number
*       port     - the port number
*       lbi      - the LBI value 0-3
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong port or devNum.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on LBI value is out-of-range.
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricConfigSrcPortToLbiMapSet
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
    GT_U32 lbi;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];
    lbi = (GT_U32)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmFabricConfigSrcPortToLbiMapSet(devNum, port, lbi);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricConfigSrcPortToLbiMapGet
*
* DESCRIPTION:
*
*     Get LBI value per source port.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum   - the device number
*       port     - the port number
*
* OUTPUTS:
*       lbiPtr   - the LBI value 0-3
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong port or devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricConfigSrcPortToLbiMapGet
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
    GT_U32 lbi;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    port = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmFabricConfigSrcPortToLbiMapGet(devNum, port, &lbi);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", lbi);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricConfigEccTypeSet
*
* DESCRIPTION:
*   This routine defines how to calculate
*   the error correction on the HyperG.Link cell
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       eccType        - the Error Correction Type
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
static CMD_STATUS wrCpssExMxPmFabricConfigEccTypeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_CONFIG_ECC_TYPE_ENT eccType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];
    eccType = (CPSS_EXMXPM_FABRIC_CONFIG_ECC_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricConfigEccTypeSet(devNum, eccType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricConfigEccTypeGet
*
* DESCRIPTION:
*   This routine gets error correction on the HyperG.Link cell
*   calculation way definitions.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*
* OUTPUTS:
*       eccTypePtr     - the Error Correction Type
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
static CMD_STATUS wrCpssExMxPmFabricConfigEccTypeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_CONFIG_ECC_TYPE_ENT eccType;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricConfigEccTypeGet(devNum, &eccType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", eccType);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmFabricConfigVidxSet",
         &wrCpssExMxPmFabricConfigVidxSet,
         5, 0},
        {"cpssExMxPmFabricConfigVidxGet",
         &wrCpssExMxPmFabricConfigVidxGet,
         1, 0},
        {"cpssExMxPmFabricConfigLbiModeSet",
         &wrCpssExMxPmFabricConfigLbiModeSet,
         2, 0},
        {"cpssExMxPmFabricConfigLbiModeGet",
         &wrCpssExMxPmFabricConfigLbiModeGet,
         1, 0},
        {"cpssExMxPmFabricConfigSrcPortToLbiMapSet",
         &wrCpssExMxPmFabricConfigSrcPortToLbiMapSet,
         3, 0},
        {"cpssExMxPmFabricConfigSrcPortToLbiMapGet",
         &wrCpssExMxPmFabricConfigSrcPortToLbiMapGet,
         2, 0},
        {"cpssExMxPmFabricConfigEccTypeSet",
         &wrCpssExMxPmFabricConfigEccTypeSet,
         2, 0},
        {"cpssExMxPmFabricConfigEccTypeGet",
         &wrCpssExMxPmFabricConfigEccTypeGet,
         1, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmFabricConfig
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
GT_STATUS cmdLibInitCpssExMxPmFabricConfig
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

