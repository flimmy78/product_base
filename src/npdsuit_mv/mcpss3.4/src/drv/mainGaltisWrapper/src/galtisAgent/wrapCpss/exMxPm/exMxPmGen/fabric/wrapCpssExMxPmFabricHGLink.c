/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmFabricHGLink.c
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
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricHGLink.h>

/*******************************************************************************
* cpssExMxPmFabricHGLinkPcsConfigSet
*
* DESCRIPTION:
*     This routine Sets fport HyperG.Link PCS configuration
*
* INPUTS:
*       devNum            - the device number.
*       fport             - only fabric port 0 supported
*       pcsConfigPtr      - HyperG.Link PCS configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or fport.
*       GT_OUT_OF_RANGE - on PCS Config parameters out of range.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkPcsConfigSet
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
    CPSS_EXMXPM_FABRIC_HGLINK_PCS_CONFIG_STC pcsConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    pcsConfig.loopbackEnable     = (GT_BOOL) inArgs[2];
    pcsConfig.repeaterModeEnable = (GT_BOOL) inArgs[3];
    pcsConfig.maxIdleCnt         = (GT_U32) inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkPcsConfigSet(devNum, fport, &pcsConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkPcsConfigGet
*
* DESCRIPTION:
*     This routine Gets fport HyperG.Link PCS configuration
*
* INPUTS:
*       devNum         - the device number.
*       fport          - only fabric port 0 supported.
*
* OUTPUTS:
*       pcsConfigPtr      - HyperG.Link PCS configuration
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or fport.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkPcsConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    GT_U8  fport;
    CPSS_EXMXPM_FABRIC_HGLINK_PCS_CONFIG_STC pcsConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport =  (GT_U8) inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkPcsConfigGet(devNum, fport, &pcsConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",
                 pcsConfig.loopbackEnable,
                 pcsConfig.repeaterModeEnable,
                 pcsConfig.maxIdleCnt);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkPcsRxStatusGet
*
* DESCRIPTION:
*     This routine Gets fport HyperG.Link PCS Rx Status
*
* INPUTS:
*       devNum         - the device number.
*       fport          - only fabric port 0 supported
*
* OUTPUTS:
*       statusPtr      - HyperG.Link PCS Rx Status
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or fport.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkPcsRxStatusGet
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
    CPSS_EXMXPM_FABRIC_HGLINK_PCS_RX_STATUS_STC status;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkPcsRxStatusGet(devNum, fport, &status);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d",
                 status.linkUp,
                 status.laneSyncOk[0],
                 status.laneSyncOk[1],
                 status.laneSyncOk[2],
                 status.laneSyncOk[3],
                 status.laneSyncOk[4],
                 status.laneSyncOk[5]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkMacConfigSet
*
* DESCRIPTION:
*     This routine Sets fport HyperG.Link MAC configuration
*
* INPUTS:
*       devNum            - the device number.
*       fport             - only fabric port 0 supported
*       macConfigPtr      - HyperG.Link MAC configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or fport.
*       GT_OUT_OF_RANGE - on MAC configuration parameters out of range
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkMacConfigSet
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
    CPSS_EXMXPM_FABRIC_HGLINK_MAC_CONFIG_STC macConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    macConfig.fcDeadLockLimit = (GT_U32) inArgs[2];
    macConfig.serdesMode = (CPSS_EXMXPM_FABRIC_HGLINK_SERDES_MODE_ENT) inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkMacConfigSet(devNum, fport, &macConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkMacConfigGet
*
* DESCRIPTION:
*     This routine Gets fport HyperG.Link MAC configuration
*
* INPUTS:
*       devNum         - the device number.
*       fport          - only fabric port 0 supported
*
* OUTPUTS:
*       macConfigPtr   - HyperG.Link MAC configuration
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or fport.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkMacConfigGet
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
    CPSS_EXMXPM_FABRIC_HGLINK_MAC_CONFIG_STC macConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];

    /*TODO: add code here. */

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkMacConfigGet(devNum, fport, &macConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",
                 macConfig.fcDeadLockLimit,
                 macConfig.serdesMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkFlowControlEnableSet
*
* DESCRIPTION:
*   Enable or disable Link Level Flow Control status of Fabric port in the XBAR
*   connected to this HyperG.Link.
*   The status of the fabric ports connected to
*   this HyperG.Link is extracted from a Link Level Flow Control cell sent
*   by the XBAR.
*
* INPUTS:
*       devNum          -  the device number.
*       fport           -  HyperG.Link port. (0 - 3)
*       enableBmpPtr    -  bit map:
*                            1 - FC enable, 0 - FC disable
*
* OUTPUTS:
*      None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkFlowControlEnableSet
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
    CPSS_PORTS_BMP_STC enableBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    enableBmp.ports[0] = inArgs[2];
    enableBmp.ports[1] = inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkFlowControlEnableSet(devNum, fport, &enableBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkFlowControlEnableGet
*
* DESCRIPTION:
*   Get enable or disable Link Level Flow Control status of Fabric port in the
*   XBAR connected to this HyperG.Link.
*   The status of the fabric ports in the corresponding XBAR connected to
*   this HyperG.Link is extracted from a Link Level Flow Control cell sent
*   by the XBAR.
*
* INPUTS:
*       devNum         - the device number.
*       fport           -  HyperG.Link port. (0 - 3)
*
* OUTPUTS:
*       enableBmpPtr    -    fport bit map:
*                            1 - FC enable, 0 - FC disable
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
static CMD_STATUS wrCpssExMxPmFabricHGLinkFlowControlEnableGet
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
    CPSS_PORTS_BMP_STC enableBmp;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkFlowControlEnableGet(devNum, fport, &enableBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",
                 enableBmp.ports[0],
                 enableBmp.ports[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkFlowControlStatusGet
*
* DESCRIPTION:
*   Get the Link Level Flow Control status of Fabric port<n>
*   XBAR connected to this HyperG.Link. The status of the fabric ports in the
*   connected to this HyperG.Link is extracted from a
*   Link Level Flow Control cell sent by the XBAR.
*
* INPUTS:
*       devNum          - the device number.
*       fport           -  HyperG.Link port. (0 - 3)
*       tc              - Traffic Class (0 - 3)
*
* OUTPUTS:
*       enableBmpPtr    -  port bit map:
*                            1 - FC enable, 0 - FC disable
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Read only dats
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkFlowControlStatusGet
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
    GT_U32  tc;
    CPSS_PORTS_BMP_STC enableBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    tc = (GT_U32) inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkFlowControlStatusGet(devNum, fport, tc, &enableBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",
                 enableBmp.ports[0],
                 enableBmp.ports[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkFlowControlTxConfigSet
*
* DESCRIPTION:
*     This routine Sets fport HyperG.Link flow contol TX configuration
*
* INPUTS:
*       devNum          - the device number.
*       fport           - only fabric port 0 supported
*       fcTxConfigPtr   - HyperG.Link port flow contol TX configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or fport.
*       GT_BAD_PTR      - on NULL pointer.
*       GT_OUT_OF_RANGE - on fcRefeshRate out of range value.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkFlowControlTxConfigSet
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
    CPSS_EXMXPM_FABRIC_HGLINK_FC_TX_CONFIG_STC fcTxConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    fcTxConfig.fcRefeshRate     = (GT_U32) inArgs[2];
    fcTxConfig.tcFcAwarePeer    = (GT_BOOL) inArgs[3];
    fcTxConfig.periodFcTxEnable = (GT_BOOL) inArgs[4];
    fcTxConfig.fcTxEnable       = (GT_BOOL) inArgs[5];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkFlowControlTxConfigSet(devNum, fport, &fcTxConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkFlowControlTxConfigGet
*
* DESCRIPTION:
*     This routine Gets fport HyperG.Link flow contol TX configuration
*
* INPUTS:
*       devNum          - the device number.
*       fport           - only fabric port 0 supported
*
* OUTPUTS:
*       fcTxConfigPtr   - HyperG.Link port flow contol TX configuration
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or fport.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkFlowControlTxConfigGet
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
    CPSS_EXMXPM_FABRIC_HGLINK_FC_TX_CONFIG_STC fcTxConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkFlowControlTxConfigGet(devNum, fport, &fcTxConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d",
                 fcTxConfig.fcRefeshRate,
                 fcTxConfig.tcFcAwarePeer,
                 fcTxConfig.periodFcTxEnable,
                 fcTxConfig.fcTxEnable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkFlowControlCellsCntrGet
*
* DESCRIPTION:
*     This routine Gets fport HyperG.Link flow contol cell counter
*
* INPUTS:
*       devNum          - the device number.
*       fport           - only fabric port 0 supported
*       cellDirection   - RX or TX cells counter
*
* OUTPUTS:
*       fcCellCntrPtr - HyperG.Link port flow contol cell counter
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum or fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Counter is read only and cleared on read
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkFlowControlCellsCntrGet
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
    CPSS_EXMXPM_FABRIC_DIR_ENT cellDirection;
    GT_U32 fcCellCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];
    cellDirection = (CPSS_EXMXPM_FABRIC_DIR_ENT) inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkFlowControlCellsCntrGet(devNum, fport, cellDirection, &fcCellCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", fcCellCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricHGLinkRxErrorCntrGet
*
* DESCRIPTION:
*     This routine Gets fport HyperG.Link Rx errors counter
*
* INPUTS:
*       devNum          - the device number.
*       fport           - only fabric port 0 supported
*
* OUTPUTS:
*       rxErrorsCntrsPtr - fport HyperG.Link Rx errors counter
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or fport.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       Counters are read only and cleared on read
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricHGLinkRxErrorCntrGet
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
    CPSS_EXMXPM_FABRIC_HGLINK_RX_ERROR_CNTRS_STC rxErrorsCntrs;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8) inArgs[0];
    fport = (GT_FPORT) inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricHGLinkRxErrorCntrGet(devNum, fport, &rxErrorsCntrs);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",
                 rxErrorsCntrs.badLengthCells,
                 rxErrorsCntrs.badHeaderCells,
                 rxErrorsCntrs.reformatErrorCells);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmFabricHGLinkPcsConfigSet",
         &wrCpssExMxPmFabricHGLinkPcsConfigSet,
         5, 0},
        {"cpssExMxPmFabricHGLinkPcsConfigGet",
         &wrCpssExMxPmFabricHGLinkPcsConfigGet,
         2, 0},
        {"cpssExMxPmFabricHGLinkPcsRxStatusGet",
         &wrCpssExMxPmFabricHGLinkPcsRxStatusGet,
         2, 0},
        {"cpssExMxPmFabricHGLinkMacConfigSet",
         &wrCpssExMxPmFabricHGLinkMacConfigSet,
         4, 0},
        {"cpssExMxPmFabricHGLinkMacConfigGet",
         &wrCpssExMxPmFabricHGLinkMacConfigGet,
         2, 0},
        {"cpssExMxPmFabricHGLinkFlowControlEnableSet",
         &wrCpssExMxPmFabricHGLinkFlowControlEnableSet,
         4, 0},
        {"cpssExMxPmFabricHGLinkFlowControlEnableGet",
         &wrCpssExMxPmFabricHGLinkFlowControlEnableGet,
         2, 0},
        {"cpssExMxPmFabricHGLinkFlowControlStatusGet",
         &wrCpssExMxPmFabricHGLinkFlowControlStatusGet,
         3, 0},
        {"cpssExMxPmFabricHGLinkFlowControlTxConfigSet",
         &wrCpssExMxPmFabricHGLinkFlowControlTxConfigSet,
         6, 0},
        {"cpssExMxPmFabricHGLinkFlowControlTxConfigGet",
         &wrCpssExMxPmFabricHGLinkFlowControlTxConfigGet,
         2, 0},
        {"cpssExMxPmFabricHGLinkFlowControlCellsCntrGet",
         &wrCpssExMxPmFabricHGLinkFlowControlCellsCntrGet,
         3, 0},
        {"cpssExMxPmFabricHGLinkRxErrorCntrGet",
         &wrCpssExMxPmFabricHGLinkRxErrorCntrGet,
         2, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmFabricHGLink
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
GT_STATUS cmdLibInitCpssExMxPmFabricHGLink
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

