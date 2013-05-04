/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrCpssExMxMirror.c
*
* DESCRIPTION:
*       Wrapper functions for Mirror cpss functions.
*
* DEPENDENCIES:
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
#include <cpss/exMx/exMxGen/mirror/cpssExMxMirror.h>

/*******************************************************************************
* cpssExMxMirrorInit
*
* DESCRIPTION:
*       Mirror initialization of PP memory,
*       all ports mirror options are disabled.
*
* INPUTS:
*       devNum      - device number to init mirror
*
* OUTPUTS:
*       none.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*       GT_BAD_PARAMS    - on invalid device number
*
* COMMENTS: An application should disable unicast egress filtering to enable
*           mirroring over the vlans
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxMirrorInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxMirrorInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxMirrorRxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a specific port to be an analyzer port of Rx mirrored
*       ports.
*
* INPUTS:
*
*       devNum  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerDevNum - pp Device number of analyzer port
*       analyzerTc   - traffic class on analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*       GT_BAD_PARAMS    - on invalid device number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxMirrorRxAnalyzerPortSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    analyzerPort;
    GT_U8    analyzerDevNum;
    GT_U8    analyzerTc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    analyzerPort = (GT_U8)inArgs[1];
    analyzerDevNum = (GT_U8)inArgs[2];
    analyzerTc = (GT_U8)inArgs[3];

    /* call cpss api function */
    result = cpssExMxMirrorRxAnalyzerPortSet(devNum, analyzerPort, analyzerDevNum, analyzerTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxMirrorTxAnalyzerPortSet
*
* DESCRIPTION:
*       Sets a port to be an analyzer port of tx mirrored
*       ports on all system pp's.
*
* INPUTS:
*       devNum  - the device number
*       analyzerPort - port number of analyzer port
*       analyzerDevNum - pp Device number of analyzer port
*       analyzerTc   - traffic class on analyzer port
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAMS   - on invalid device number
*
* COMMENTS:
*       analyzerTc is supported only with 98MX6x5 and 98MX6x8 devices.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxMirrorTxAnalyzerPortSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    analyzerPort;
    GT_U8    analyzerDevNum;
    GT_U8    analyzerTc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    analyzerPort = (GT_U8)inArgs[1];
    analyzerDevNum = (GT_U8)inArgs[2];
    analyzerTc = (GT_U8)inArgs[3];

    /* call cpss api function */
    result = cpssExMxMirrorTxAnalyzerPortSet(devNum, analyzerPort, analyzerDevNum, analyzerTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxMirrorRxPortSet
*
* DESCRIPTION:
*       Sets a specific port in pp's device to be Rx mirrored port
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum       - the device number
*       mirrPort     - the Rx mirrored port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PTR      - on NULL pointer
*       GT_BAD_PARAMS   - on invalid device number
*
* COMMENTS:
*       CPU port can not be Rx mirrored port.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxMirrorRxPortSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32    mirrPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mirrPort = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxMirrorRxPortSet(devNum, mirrPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxMirrorTxPortSet
*
* DESCRIPTION:
*       Adds port to be Tx mirrored port .
*       Up to 8 ports in all system can be tx mirrored ports.
*       All ExMx devices in a system must be configured with
*       same set of Tx mirrored ports
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum   - the device number
*       mirrPort - the Tx mirrored port number.
*       mirrDev  - the Tx mirrored port's device number.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_FULL         - all tx mirror ports are used.
*       GT_BAD_PTR      - on NULL pointer
*       GT_BAD_PARAMS   - on invalid device number
*
* COMMENTS:
*       There can be up to 8 Tx mirrored ports per Prestera chipset.
*       CPU port can be Tx mirrored port.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxMirrorTxPortSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    mirrPort;
    GT_U8    mirrDev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mirrPort = (GT_U8)inArgs[1];
    mirrDev = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxMirrorTxPortSet(devNum, mirrPort, mirrDev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxMirrorRxPortDel
*
* DESCRIPTION:
*       Deletes a specific port from the set of Rx mirrored ports.
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum       - the device number
*       mirrPort     - the Rx mirrored port number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PTR      - on NULL pointer
*       GT_BAD_PARAMS   - on invalid device number
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxMirrorRxPortDel
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    mirrPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mirrPort = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxMirrorRxPortDel(devNum, mirrPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxMirrorTxPortDel
*
* DESCRIPTION:
*       Deletes a specific port from the set of Tx mirrored ports
*       All ExMx devices in a system must be configured with
*       same set of Tx mirrored ports
*
*       APPLICABLE DEVICES:  All ExMx devices
*
* INPUTS:
*       devNum  - the device number
*       mirrPort - the Tx mirrored port number.
*       mirrDev  - the Tx mirrored port's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_NO_SUCH      - the port isn't valid.
*       GT_BAD_PTR      - on NULL pointer
*       GT_BAD_PARAMS   - on invalid device number
*
* COMMENTS:
*       There can be up to 8 egress mirrored ports per Prestera chipset.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxMirrorTxPortDel
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U8    mirrPort;
    GT_U8    mirrDev;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mirrPort = (GT_U8)inArgs[1];
    mirrDev = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxMirrorTxPortDel(devNum, mirrPort, mirrDev);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxMirrorCpuRxMirrorParamsSet
*
* DESCRIPTION:
*       Sets Cpu Rx mirror parameters for flow mirrored packets.
*
* INPUTS:
*       devNum  - the device number
*       enable      - GT_TRUE enable Rx mirror to Cpu,
*                     GT_FALSE disable.
*       mirrorTc    - Traffic class of mirrored packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success,
*       GT_BAD_PARAM    - on bad paramters,
*       GT_FAIL         - otherwise.
*       GT_BAD_PARAMS   - on invalid device number
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxMirrorCpuRxMirrorParamsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL    enable;
    GT_U8    mirrorTc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    mirrorTc = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxMirrorCpuRxMirrorParamsSet(devNum, enable, mirrorTc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxMirrorInit",
        &wrcpssExMxMirrorInit,
        1, 0},

    {"cpssExMxMirrorRxAnalyzerPortSet",
        &wrcpssExMxMirrorRxAnalyzerPortSet,
        4, 0},

     {"cpssExMxMirrorTxAnalyzerPortSet",
        &wrcpssExMxMirrorTxAnalyzerPortSet,
       4, 0},

    {"cpssExMxMirrorRxPortSet",
        &wrcpssExMxMirrorRxPortSet,
        2, 0},

    {"cpssExMxMirrorTxPortSet",
        &wrcpssExMxMirrorTxPortSet,
        3, 0},

    {"cpssExMxMirrorRxPortDel",
        &wrcpssExMxMirrorRxPortDel,
        2, 0},

    {"cpssExMxMirrorTxPortDel",
        &wrcpssExMxMirrorTxPortDel,
        3, 0},

    {"cpssExMxMirrorCpuRxMirrorParamsSet",
        &wrcpssExMxMirrorCpuRxMirrorParamsSet,
        3, 0}


};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitMirrorCpss
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
GT_STATUS cmdLibInitCpssExMxMirror
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}



