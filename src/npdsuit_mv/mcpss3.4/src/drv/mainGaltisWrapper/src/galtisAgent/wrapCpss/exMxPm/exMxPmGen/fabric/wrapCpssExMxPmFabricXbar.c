/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmFabricXbar.c
*
* DESCRIPTION:
*       The CPSS EXMXPM wrappers for Fabrix/Xbar configuration APIs
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
#include <cpss/exMxPm/exMxPmGen/fabric/cpssExMxPmFabricXbar.h>

/*******************************************************************************
* cpssExMxPmFabricXbarConfigSet
*
* DESCRIPTION:
*     This routine Sets XBAR configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       xbarCfgPtr     - XBAR configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE          - on activeHyperGLink out of range value.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarConfigSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_XBAR_CONFIG_STC xbarCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    xbarCfg.voQUnit0ActiveHyperGlink=(GT_U32)inArgs[1];
    xbarCfg.voQUnit1ActiveHyperGlink=(GT_U32)inArgs[2];
    xbarCfg.xbarTcModeEnable=(GT_BOOL)inArgs[3];
    xbarCfg.mode=(CPSS_EXMXPM_FABRIC_XBAR_MODE_ENT)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarConfigSet(devNum, &xbarCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarConfigGet
*
* DESCRIPTION:
*     This routine Gets XBAR configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*
* OUTPUTS:
*       xbarCfgPtr     - XBAR configuration
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
static CMD_STATUS wrCpssExMxPmFabricXbarConfigGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_XBAR_CONFIG_STC xbarCfg;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarConfigGet(devNum, &xbarCfg);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", xbarCfg.voQUnit0ActiveHyperGlink,
                 xbarCfg.voQUnit1ActiveHyperGlink,
                 xbarCfg.xbarTcModeEnable, xbarCfg.mode);

	return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportMulticastCellSwitchingSet
*
* DESCRIPTION:
*     This routine sets destination port bitmap that determines which ports
*     should receive the cells with the MC group ID (vidx).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fportGroup        - port group for multicast table configuration.
*       vidx              - multicast group entry index (0 - 4095).
*       destPortBmp       - destination port bit map.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fportGroup, vidx, destPortBmp.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportMulticastCellSwitchingSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8     devNum;
    CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP_ENT  fportGroup;
    GT_U16    vidx;
    GT_U32    destPortBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum=(GT_U8)inArgs[0];
    fportGroup = (CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP_ENT)inArgs[1];
    vidx = (GT_U16)inArgs[2];
    destPortBmp = inArgs[3];

    result = cpssExMxPmFabricXbarFportMulticastCellSwitchingSet(devNum, fportGroup, vidx, destPortBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportMulticastCellSwitchingGet
*
* DESCRIPTION:
*     This routine gets destination port bitmap that determines which ports
*     should receive the cells with the MC group ID (vidx).
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fportGroup        - port group for multicast table configuration.
*       vidx              - multicast group entry index (0 - 4095).
*
* OUTPUTS:
*       destPortBmpPtr       - pointer to destination port bit map
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fportGroup, vidx.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportMulticastCellSwitchingGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8                                  devNum;
    CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP_ENT  fportGroup;
    GT_U16                                 vidx;
    GT_U32                                 destPortBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum=(GT_U8)inArgs[0];
    fportGroup = (CPSS_EXMXPM_FABRIC_MC_FPORT_GROUP_ENT)inArgs[1];
    vidx = (GT_U16)inArgs[2];

    /* call cpss api function */    
    result = cpssExMxPmFabricXbarFportMulticastCellSwitchingGet(devNum, fportGroup, vidx, &destPortBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "0x%x", destPortBmp);

	return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportConfigSet
*
* DESCRIPTION:
*     This routine sets fabric port configuration.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       fport          - fabric port (0 - 5)
*       fportConfigPtr - fabric port configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportConfigSet
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
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_CONFIG_STC fportConfig;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  = (GT_U8)inArgs[0];
    fport   = (GT_FPORT)inArgs[1];
    fportConfig.portEnable = (GT_BOOL)inArgs[2];
    fportConfig.mcSrcDevFilterEnable = (GT_BOOL)inArgs[3];
    fportConfig.bcSrcDevFilterEnable = (GT_BOOL)inArgs[4];
    fportConfig.ucSrcDevFilterEnable = (GT_BOOL)inArgs[5];
    fportConfig.diagSrcDevFilterEnable = (GT_BOOL)inArgs[6];
    fportConfig.diagCellSuperHighPriorityAssign = (GT_BOOL)inArgs[7];
    fportConfig.bcCellSuperHighPriorityAssign = (GT_BOOL)inArgs[8];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportConfigSet(devNum, fport, &fportConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportConfigGet
*
* DESCRIPTION:
*     This routine Gets fabric port configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       fport          - fabric port (0 - 5)
*
* OUTPUTS:
*       fportConfigPtr    - fabric port configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportConfigGet
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
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_CONFIG_STC fportConfig;

	/* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportConfigGet(devNum, fport, &fportConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d%d%d",fportConfig.portEnable,
                 fportConfig.mcSrcDevFilterEnable,
                 fportConfig.bcSrcDevFilterEnable,
				 fportConfig.ucSrcDevFilterEnable,
				 fportConfig.diagSrcDevFilterEnable,
                 fportConfig.diagCellSuperHighPriorityAssign,
                 fportConfig.bcCellSuperHighPriorityAssign);

    return CMD_OK;
}
/*********Table:cpssExMxPmFabricXbarFportBlocking*********************************/
/*******************************************************************************
* cpssExMxPmFabricXbarFportBlockingSet
*
* DESCRIPTION:
*     This routine sets blocking crossbar port traffic configuration.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum              - the device number.
*       fport               - fabric port (0 - 5)
*       fportBlockingPtr    - fabric port blocking configuration
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportBlockingSet
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
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_BLOCKING_STC fportBlocking;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];

    fportBlocking.enableRxCells=(GT_BOOL)inFields[0];
    fportBlocking.enableTxCellsFromFPort5=(GT_BOOL)inFields[1];
    fportBlocking.enableTxCellsFromFPort4=(GT_BOOL)inFields[2];
    fportBlocking.enableTxCellsFromFPort3=(GT_BOOL)inFields[3];
    fportBlocking.enableTxCellsFromFPort2=(GT_BOOL)inFields[4];
    fportBlocking.enableTxCellsFromFPort1=(GT_BOOL)inFields[5];
    fportBlocking.enableTxCellsFromFPort0=(GT_BOOL)inFields[6];
    fportBlocking.enableTxCellsToFPort5=(GT_BOOL)inFields[7];
    fportBlocking.enableTxCellsToFPort4=(GT_BOOL)inFields[8];
    fportBlocking.enableTxCellsToFPort3=(GT_BOOL)inFields[9];
    fportBlocking.enableTxCellsToFPort2=(GT_BOOL)inFields[10];
    fportBlocking.enableTxCellsToFPort1=(GT_BOOL)inFields[11];
    fportBlocking.enableTxCellsToFPort0=(GT_BOOL)inFields[12];
    fportBlocking.enableHighPrioTxCells=(GT_BOOL)inFields[13];
    fportBlocking.enableFPrio3TxCells=(GT_BOOL)inFields[14];
    fportBlocking.enableFPrio2TxCells=(GT_BOOL)inFields[15];
    fportBlocking.enableFPrio1TxCells=(GT_BOOL)inFields[16];
    fportBlocking.enableFPrio0TxCells=(GT_BOOL)inFields[17];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportBlockingSet(devNum, fport, &fportBlocking);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportBlockingGet
*
* DESCRIPTION:
*     This routine Gets fabric port blocking configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       fport          - fabric port (0 - 5)
*
* OUTPUTS:
*       fportBlockingPtr    - fabric port blocking configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportBlockingGetFirst
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
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_BLOCKING_STC fportBlocking;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportBlockingGet(devNum, fport, &fportBlocking);

     if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
     }

      inFields[0] = fportBlocking.enableRxCells;
      inFields[1] = fportBlocking.enableTxCellsFromFPort5;
      inFields[2] = fportBlocking.enableTxCellsFromFPort4;
      inFields[3] = fportBlocking.enableTxCellsFromFPort3;
      inFields[4] = fportBlocking.enableTxCellsFromFPort2;
      inFields[5] = fportBlocking.enableTxCellsFromFPort1;
      inFields[6] = fportBlocking.enableTxCellsFromFPort0;
      inFields[7] = fportBlocking.enableTxCellsToFPort5;
      inFields[8] = fportBlocking.enableTxCellsToFPort4;
      inFields[9] = fportBlocking.enableTxCellsToFPort3;
      inFields[10] = fportBlocking.enableTxCellsToFPort2;
      inFields[11] = fportBlocking.enableTxCellsToFPort1;
      inFields[12] = fportBlocking.enableTxCellsToFPort0;
      inFields[13] = fportBlocking.enableHighPrioTxCells;
      inFields[14] = fportBlocking.enableFPrio3TxCells;
      inFields[15] = fportBlocking.enableFPrio2TxCells;
      inFields[16] = fportBlocking.enableFPrio1TxCells;
      inFields[17] = fportBlocking.enableFPrio0TxCells;

     fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],
                           inFields[1],  inFields[2],
                           inFields[3],  inFields[4],  inFields[5],
                           inFields[6],  inFields[7],  inFields[8],
                           inFields[9],  inFields[10], inFields[11],
                           inFields[12], inFields[13], inFields[14],
                           inFields[15], inFields[16], inFields[17]
                 );

    galtisOutput(outArgs, GT_OK, "%f");



    return CMD_OK;
}
/*******************************************************************************
* cpssExMxPmGetNext
*
* DESCRIPTION:
*     This galtis function for reafresh
*
* INPUTS:
*
*
*       none
*
* OUTPUTS:
*      none
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum,fport.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
     galtisOutput(outArgs, GT_OK, "%d", -1);
     return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportFcSet
*
* DESCRIPTION:
*     This routine sets fabric port flow control thresholds
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       fport          - fabric port (0 - 5)
*       fportFcPtr     - fabric port flow control configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE - on out of range fport fc configuration values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportFcSet
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
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_FC_STC fportFc;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    fportFc.regularPrioXoffTheshold=(GT_U32)inArgs[2];
    fportFc.regularPrioXonTheshold=(GT_U32)inArgs[3];
    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportFcSet(devNum, fport, &fportFc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportFcGet
*
* DESCRIPTION:
*     This routine gets fabric port flow control thresholds
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       fport          - fabric port  (0 - 5)
*
* OUTPUTS:
*       fportFcPtr    - fabric port flow control configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportFcGet
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
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_FC_STC fportFc;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportFcGet(devNum, fport, &fportFc);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",
                 fportFc.regularPrioXoffTheshold,fportFc.regularPrioXonTheshold);


    return CMD_OK;
}
/*********Table:cpssExMxPmFabricXbarFportArbiter*********************************/
/*******************************************************************************
* cpssExMxPmFabricXbarFportArbiterSet
*
* DESCRIPTION:
*     This routine Sets fabric port Arbiter configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       fport          - fabric port (0-5)
*       arbConfigPtr   - fabric port Arbiter configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE - on arbiter configuration out of range values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportArbiterSet
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
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_ARBITER_CONFIG_STC arbConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    arbConfig.srcFportRegularPrioGroup[0]=inFields[0];
    arbConfig.srcFportRegularPrioGroup[1]=inFields[1];
    arbConfig.srcFportRegularPrioGroup[2]=inFields[2];
    arbConfig.srcFportRegularPrioGroup[3]=inFields[3];
    arbConfig.srcFportRegularPrioGroup[4]=inFields[4];
    arbConfig.srcFportRegularPrioGroup[5]=inFields[5];
    arbConfig.srcFportSuperHighPrioGroup[0]=inFields[6];
    arbConfig.srcFportSuperHighPrioGroup[1]=inFields[7];
    arbConfig.srcFportSuperHighPrioGroup[2]=inFields[8];
    arbConfig.srcFportSuperHighPrioGroup[3]=inFields[9];
    arbConfig.srcFportSuperHighPrioGroup[4]=inFields[10];
    arbConfig.srcFportSuperHighPrioGroup[5]=inFields[11];
    arbConfig.groupRegularPrioWeight[0]=inFields[12];
    arbConfig.groupRegularPrioWeight[1]=inFields[13];
    arbConfig.groupSuperHighPrioWeight[0]=inFields[14];
    arbConfig.groupSuperHighPrioWeight[1]=inFields[15];


    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportArbiterSet(devNum, fport, &arbConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportArbiterGet
*
* DESCRIPTION:
*     This routine Gets fabric port Arbiter configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       fport          - fabric port (0 - 5)
*
* OUTPUTS:
*       arbConfigPtr   - fabric port Arbiter configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportArbiterGetFirst
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
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_ARBITER_CONFIG_STC arbConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];


    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportArbiterGet(devNum, fport, &arbConfig);

     if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
      }
     inFields[0] =arbConfig.srcFportRegularPrioGroup[0];
     inFields[1] =arbConfig.srcFportRegularPrioGroup[1];
     inFields[2] =arbConfig.srcFportRegularPrioGroup[2];
     inFields[3] =arbConfig.srcFportRegularPrioGroup[3];
     inFields[4] =arbConfig.srcFportRegularPrioGroup[4];
     inFields[5] =arbConfig.srcFportRegularPrioGroup[5];
     inFields[6] =arbConfig.srcFportSuperHighPrioGroup[0];
     inFields[7] =arbConfig.srcFportSuperHighPrioGroup[1];
     inFields[8] =arbConfig.srcFportSuperHighPrioGroup[2];
     inFields[9] =arbConfig.srcFportSuperHighPrioGroup[3];
     inFields[10]=arbConfig.srcFportSuperHighPrioGroup[4];
     inFields[11]=arbConfig.srcFportSuperHighPrioGroup[5];
     inFields[12]=arbConfig.groupRegularPrioWeight[0];
     inFields[13]=arbConfig.groupRegularPrioWeight[1];
     inFields[14]=arbConfig.groupSuperHighPrioWeight[0];
     inFields[15]=arbConfig.groupSuperHighPrioWeight[1];

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                           inFields[0],  inFields[1],  inFields[2],
                           inFields[3],  inFields[4],  inFields[5],
                           inFields[6],  inFields[7],  inFields[8],
                           inFields[9],  inFields[10], inFields[11],
                           inFields[12], inFields[13], inFields[14],
                           inFields[15]);


    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*************Table:cpssExMxPmFabricXbarClassArbiter*****************************/
/*******************************************************************************
* cpssExMxPmFabricXbarClassArbiterSet
*
* DESCRIPTION:
*     This routine Sets class Arbiter configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       fport          - fabric port (0 - 5)
*       arbConfigPtr   - class Arbiter configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE - on arbiter configuration out of range values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To disable the feature set rate to 0xFFFFFFFF.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarClassArbiterSet
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
    CPSS_EXMXPM_FABRIC_XBAR_CLASS_ARBITER_CONFIG_STC arbConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    arbConfig.tcArbType[0]=(CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT)inFields[0];
    arbConfig.tcArbType[1]=(CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT)inFields[1];
    arbConfig.tcArbType[2]=(CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT)inFields[2];
    arbConfig.tcArbType[3]=(CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT)inFields[3];
    arbConfig.tcWeight[0]=(GT_U32)inFields[4];
    arbConfig.tcWeight[1]=(GT_U32)inFields[5];
    arbConfig.tcWeight[2]=(GT_U32)inFields[6];
    arbConfig.tcWeight[3]=(GT_U32)inFields[7];
    arbConfig.superHighPrioArbType=(CPSS_EXMXPM_FABRIC_ARB_POLICY_ENT)inFields[8];
    arbConfig.superHighPrioWeight=(GT_U32)inFields[9];



    /* call cpss api function */
    result = cpssExMxPmFabricXbarClassArbiterSet(devNum, fport, &arbConfig);



    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarClassArbiterGet
*
* DESCRIPTION:
*     This routine Sets class Arbiter configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       fport          - fabric port (0 - 5)
*
* OUTPUTS:
*       arbConfigPtr   - class Arbiter configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarClassArbiterGet
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
    CPSS_EXMXPM_FABRIC_XBAR_CLASS_ARBITER_CONFIG_STC arbConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    /* call cpss api function */
    result = cpssExMxPmFabricXbarClassArbiterGet(devNum, fport, &arbConfig);

   if (result != GT_OK)
      {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
     }

     inFields[0]=arbConfig.tcArbType[0];
     inFields[1]=arbConfig.tcArbType[1];
     inFields[2]=arbConfig.tcArbType[2];
     inFields[3]=arbConfig.tcArbType[3];
     inFields[4]= arbConfig.tcWeight[0];
     inFields[5]=arbConfig.tcWeight[1];
     inFields[6]=arbConfig.tcWeight[2];
     inFields[7]=arbConfig.tcWeight[3];
     inFields[8]=arbConfig.superHighPrioArbType;
     inFields[9]=arbConfig.superHighPrioWeight;


     fieldOutput("%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                           inFields[3],  inFields[4],  inFields[5],
                           inFields[6],  inFields[7],  inFields[8],
                           inFields[9] );


    galtisOutput(outArgs, GT_OK, "%f");



    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarShaperSet
*
* DESCRIPTION:
*     This routine Sets fabric port shaper configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - the device number.
*       fport           - fabric port (0 - 5)
*       shaperConfigPtr - shaper configuration
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_OUT_OF_RANGE          - on shaper configuration out of range values.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       To disable the feature set rate to 0xFFFFFFFF.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarShaperSet
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
    CPSS_EXMXPM_FABRIC_XBAR_SHAPER_CONFIG_STC shaperConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    shaperConfig.tcCellsRate[0]=(GT_U32)inArgs[2];
    shaperConfig.tcCellsRate[1]=(GT_U32)inArgs[3];
    shaperConfig.tcCellsRate[2]=(GT_U32)inArgs[4];
    shaperConfig.tcCellsRate[3]=(GT_U32)inArgs[5];
    shaperConfig.superHighPrioCellsRate=(GT_U32)inArgs[6];


    /* call cpss api function */
    result = cpssExMxPmFabricXbarShaperSet(devNum, fport, &shaperConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarShaperGet
*
* DESCRIPTION:
*     This routine Gets fabric port shaper configuration
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum          - the device number.
*       fport           - fabric port (0 - 5)
*
* OUTPUTS:
*       shaperConfigPtr - shaper configuration
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarShaperGet
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
    CPSS_EXMXPM_FABRIC_XBAR_SHAPER_CONFIG_STC shaperConfig;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    /* call cpss api function */
    result = cpssExMxPmFabricXbarShaperGet(devNum, fport, &shaperConfig);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d",shaperConfig.tcCellsRate[0],
                 shaperConfig.tcCellsRate[1],shaperConfig.tcCellsRate[2],
                 shaperConfig.tcCellsRate[3],shaperConfig.superHighPrioCellsRate);

    return CMD_OK;
}
/********Table:cpssExMxPmFabricXbarFportFifoStatus********************************/
/*******************************************************************************
* cpssExMxPmFabricXbarFportFifoStatusGet
*
* DESCRIPTION:
*     This routine Gets fabric port FIFO status
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum         - the device number.
*       fport          - fabric port (0 - 5)
*
* OUTPUTS:
*       fportFifoStatusPtr    - fabric port FIFO status
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportFifoStatusGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 i, j, index;
    GT_FPORT fport;
    CPSS_EXMXPM_FABRIC_XBAR_FPORT_FIFO_STATUS_STC fportFifoStatus;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportFifoStatusGet(devNum, fport, &fportFifoStatus);
    if (result != GT_OK)
     {
          galtisOutput(outArgs, result, "%d", -1);
          return CMD_OK;
     }

    index = 0;
    for(i = 0; i < 4; i++)
    {
        for(j = 0; j < 6; j++)
        {
            inFields[index] = fportFifoStatus.srcFportPrio0TcXoff[i][j];
            index++;
        }

    }

    inFields[24]=fportFifoStatus.srcFportPrio1TxFifoEmpty[0];
    inFields[25]=fportFifoStatus.srcFportPrio1TxFifoEmpty[1];
    inFields[26]=fportFifoStatus.srcFportPrio1TxFifoEmpty[2];

    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                           inFields[0],  inFields[1],  inFields[2],
                           inFields[3],  inFields[4],  inFields[5],
                           inFields[6],  inFields[7],  inFields[8],
                           inFields[9],  inFields[10], inFields[11],
                           inFields[12], inFields[13], inFields[14],
                           inFields[16], inFields[17], inFields[18],
                           inFields[19], inFields[20], inFields[21],
                           inFields[22], inFields[23], inFields[24],
                           inFields[25], inFields[26]);


    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportCellCntrTypeSet
*
* DESCRIPTION:
*     This routine Sets bitmap of counted cells types
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5)
*       cellDirection     - Rx or Tx, there are Rx Counter and Tx Counter
*       cellTypeBitmap    - bitmap of CPSS_EXMXPM_FABRIC_CELL_TYPE_ENT
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportCellCntrTypeSet
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
    CPSS_EXMXPM_FABRIC_CELL_TYPE_ENT cellTypeBitmap;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    cellDirection=(CPSS_EXMXPM_FABRIC_DIR_ENT)inArgs[2];
    cellTypeBitmap=(CPSS_EXMXPM_FABRIC_CELL_TYPE_ENT)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportCellCntrTypeSet(devNum, fport, cellDirection, cellTypeBitmap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportCellCntrTypeGet
*
* DESCRIPTION:
*     This routine Gets bitmap of counted cells types
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5)
*       cellDirection     - Rx or Tx, there are Rx Counter and Tx Conter
*
* OUTPUTS:
*       cellTypeBitmapPtr - bitmap of CPSS_EXMXPM_FABRIC_CELL_TYPE_ENT
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportCellCntrTypeGet
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
    CPSS_EXMXPM_FABRIC_DIR_ENT cellDirection;
    GT_U32      cellTypeBitmap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    cellDirection=(CPSS_EXMXPM_FABRIC_DIR_ENT)inArgs[2];
    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportCellCntrTypeGet(devNum, fport, cellDirection, &cellTypeBitmap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",cellTypeBitmap);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportCellCntrGet
*
* DESCRIPTION:
*     This routine Gets Cell counter value
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5)
*       cellDirection     - Rx or Tx, there are Rx Counter and Tx Conter
*
* OUTPUTS:
*       cellCntrPtr      - cell Counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Counter is read only and cleared on read.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportCellCntrGet
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
    GT_U32 cellCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    cellDirection=(CPSS_EXMXPM_FABRIC_DIR_ENT)inArgs[2];
    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportCellCntrGet(devNum, fport, cellDirection, &cellCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d",cellCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet
*
* DESCRIPTION:
*     This routine Gets Tx FIFO Dropped Cell counter value
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5)
*       fifo              - fifo (0 - 5)
*
* OUTPUTS:
*       superHighPriCellsCntrPtr   - super high Priority Cells Counter value
*       regularCellCntrPtr         - regular Priority Cells Counter value
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport or fifo.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Counter is read only and cleared on read
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet
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
    GT_U32 fifo;
    GT_U32 superHighPriCellsCntr;
    GT_U32 regularCellCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];
    fifo=(GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet(devNum, fport, fifo, &superHighPriCellsCntr, &regularCellCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",superHighPriCellsCntr,regularCellCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet
*
* DESCRIPTION:
*     This routine Gets Rx Dropped Cells counters values
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5)
*
* OUTPUTS:
*       invalidTargetCntrPtr  - Count cells dropped
*                               due to invalid target device
*       rxFifoOverrunCntrPtr  - Count cells dropped
*                               due to receive FIFO overrun
*       ownDevFilterCntrPtr   - Count cells dropped
*                                due to Source filtering
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum,fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*       Counter is read only and cleared on read
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet
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
    GT_U32 invalidTargetCntr;
    GT_U32 rxFifoOverrunCntr;
    GT_U32 ownDevFilterCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    devNum=(GT_U8)inArgs[0];
    fport=(GT_FPORT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet(devNum, fport, &invalidTargetCntr, &rxFifoOverrunCntr, &ownDevFilterCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d",invalidTargetCntr,rxFifoOverrunCntr,ownDevFilterCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportUnicastCellSwitchingSet
*
* DESCRIPTION:
*     This routine sets destination port that should receive the cells according
*     to index that is consist of destination device number and LBH value.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5).
*       destDevNum        - destination device number (0 - 127).
*       lbh               - LBH (0 - 3).
*       lbhEnable         - An indication for the validity of the forwarding data.
*                           GT_TRUE  - destination port number configuration is
*                                      valid for the lbh.
*                           GT_FALSE - destination port number configuration
*                                      isn't valid for the lbh.
*       destPort          - destination port (0 - 5).
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport, destDevNum, destPort.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*        When crossbar is enabled, the user must configure Unicast table and
*        VOQ table Device Number to Fabric Port Mapping with identical values.
*        When the crossbar is in bypass mode, the crossbar's unicast table is
*        bypassed and fport selection per cell done only bythe VOQ table
*        deviceNumberToFabricPortMap (cpssExMxPmFabricVoqDevToFportMapSet).
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportUnicastCellSwitchingSet
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
    GT_U8 destDevNum;
    GT_U32 lbh;
    GT_BOOL lbhEnable;
    GT_U32 destPort;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  =   (GT_U8)inArgs[0];
    fport   =   (GT_FPORT)inArgs[1];
    destDevNum = (GT_U8)inArgs[2];
    lbh     =   (GT_U32)inArgs[3];
    lbhEnable = (GT_BOOL)inArgs[4];
    destPort =  (GT_U32)inArgs[5];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportUnicastCellSwitchingSet(devNum, fport, destDevNum, lbh, lbhEnable, destPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportUnicastCellSwitchingGet
*
* DESCRIPTION:
*     This routine gets destination port that should receive the cells according
*     to index that is consist of destination device number and LBH value.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5).
*       destDevNum        - destination device number (0 - 127).
*       lbh               - LBH (0 - 3).
*
* OUTPUTS:
*       lbhEnablePtr      - Pointer to indication for the validity of the
*                           forwarding data.
*                           GT_TRUE  - destination port number configuration is
*                                      valid for the lbh.
*                           GT_FALSE - destination port number configuration
*                                      isn't valid for the lbh.
*       destPortPtr       - pointer to destination port
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport, destDevNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportUnicastCellSwitchingGet
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
    GT_U8 destDevNum;
    GT_U32 lbh;
    GT_BOOL lbhEnable;
    GT_U32 destPort;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  =   (GT_U8)inArgs[0];
    fport   =   (GT_FPORT)inArgs[1];
    destDevNum = (GT_U8)inArgs[2];
    lbh     =   (GT_U32)inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportUnicastCellSwitchingGet(devNum, fport, destDevNum, lbh, &lbhEnable, &destPort);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", lbhEnable, destPort);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet
*
* DESCRIPTION:
*     This routine sets destination port bitmap for diagnostic cells.
*     A copy of this cell will be sent to Reassembly engine and will be
*     enqueued into CPU mail FIFO.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5).
*       destPortBmp       - destination port bit map.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport, destPortBmp.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet
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
    GT_U32 destPortBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  =   (GT_U8)inArgs[0];
    fport   =   (GT_FPORT)inArgs[1];
    destPortBmp = inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet(devNum, fport, destPortBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");  

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet
*
* DESCRIPTION:
*     This routine gets destination port bitmap for diagnostic cells.
*     A copy of this cell will be sent to Reassembly engine and will be
*     enqueued into CPU mail FIFO.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5).
*
* OUTPUTS:
*       destPortBmpPtr    - pointer to destination port bit map
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet
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
    GT_U32 destPortBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  =   (GT_U8)inArgs[0];
    fport   =   (GT_FPORT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet(devNum, fport, &destPortBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", destPortBmp);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet
*
* DESCRIPTION:
*     This routine sets destination port bitmap for broadcast cells.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5).
*       destPortBmp       - destination port bit map.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport, destPortBmp.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportBroadcastCellSwitchingSet
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
    GT_U32 destPortBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  =   (GT_U8)inArgs[0];
    fport   =   (GT_FPORT)inArgs[1];
    destPortBmp = inArgs[2];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet(devNum, fport, destPortBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet
*
* DESCRIPTION:
*     This routine gets destination port bitmap for broadcast cells.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum            - the device number.
*       fport             - fabric port (0 - 5).
*
* OUTPUTS:
*       destPortBmpPtr    - pointer to destination port bit map
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportBroadcastCellSwitchingGet
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
    GT_U32 destPortBmp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  =   (GT_U8)inArgs[0];
    fport   =   (GT_FPORT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet(devNum, fport, &destPortBmp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", destPortBmp);
    
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet
*
* DESCRIPTION:
*     This routine enables/disables filtering according to source device number.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - the device number.
*       fport      - fabric port (0 - 5).
*       srcDevNum  - source device number for filtering (0 - 127).
*       lbh        - LBH for filtering (0 - 3).
*       enable     - enable/disable source device filtering.
*                    GT_TRUE - if equivalent cell bit representing the same
*                              device ID as, srcDevNum and corresponds to the lbh,
*                              the crossbar will filter this cell by dropping it.
*                    GT_FALSE - filtering is disabled.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport, srcDevNum, lbh.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet
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
    GT_U8 srcDevNum;
    GT_U32 lbh;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  =   (GT_U8)inArgs[0];
    fport   =   (GT_FPORT)inArgs[1];
    srcDevNum = (GT_U8)inArgs[2];
    lbh     = inArgs[3];
    enable  = (GT_BOOL)inArgs[4];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet(devNum, fport, srcDevNum, lbh, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet
*
* DESCRIPTION:
*     This routine gets status of source device filtering.
*
* APPLICABLE DEVICES:  All ExMxPm devices
*
* INPUTS:
*       devNum     - the device number.
*       fport      - fabric port (0 - 5).
*       srcDevNum  - source device number for filtering (0 - 127).
*       lbh        - LBH for filtering (0 - 3).
*
* OUTPUTS:
*       enablePtr  - pointer to enable/disable source device filtering.
*                    GT_TRUE - if equivalent cell bit representing the same
*                              device ID as, srcDevNum and corresponds to the lbh,
*                              the crossbar will filter this cell by dropping it.
*                    GT_FALSE - filtering is disabled.
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fport, srcDevNum, lbh.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet
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
    GT_U8 srcDevNum;
    GT_U32 lbh;
    GT_BOOL enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum  =   (GT_U8)inArgs[0];
    fport   =   (GT_FPORT)inArgs[1];
    srcDevNum = (GT_U8)inArgs[2];
    lbh     = inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet(devNum, fport, srcDevNum, lbh, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricMulticastActiveActiveSelectSet
*
* DESCRIPTION:
*      Define the Active H.Glink ports that would be used for sending
*      MC cells in Active-Active XBAR mode.
*
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum         - the device number.
*       activePorts    - Active H.Glink ports that would be used for sending
*                        MC cells in Active-Active XBAR mode.
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                     - on success.
*       GT_HW_ERROR               - on HW error.
*       GT_BAD_PARAM              - on wrong devNum, activePorts.
*       GT_NOT_APPLICABLE_DEVICE  - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricMulticastActiveActiveSelectSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_ENT activePorts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    activePorts = (CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricMulticastActiveActiveSelectSet(devNum, activePorts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricMulticastActiveActiveSelectGet
*
* DESCRIPTION:
*      Gets the Active H.Glink ports that would be used for sending
*      MC cells in Active-Active XBAR mode.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum           - the device number.
*
* OUTPUTS:
*       activePortsPtr  -  Pointer to active H.Glink ports that would be used
*                          for sending MC cells in Active-Active XBAR mode.
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
static CMD_STATUS wrCpssExMxPmFabricMulticastActiveActiveSelectGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_FABRIC_MC_ACTIVE_ACTIVE_HGLINK_ENT activePorts;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmFabricMulticastActiveActiveSelectGet(devNum, &activePorts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", activePorts);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFcActiveActiveMappingSet
*
* DESCRIPTION:
*      Set mapping between external XBAR port in Link Layer Flow Control messages to VOQ. 
*       The mapping is relevant only for Active-Active XBAR mode.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum      - the device number.
*       voqNum      - Virtual Output Queue number: 0 - 23
*       sourcefPort - source port HyperG. Link port: 0 or 1 for voqUnit0, 
*                       2-3 for voqUnit1; because both units receive same 
*                       configuration 0 == 2 and 1 == 3
*       fcVectorBit - bit in flow control message mapped for given VOQ: 
*                       0-11 (12-15 are not valid)
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, fcVectorBit, voqNum
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFcActiveActiveMappingSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;    
    GT_U32 voqNum;
    CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_ENT sourcefPort;
    GT_U32 fcVectorBit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    voqNum = inArgs[1];
    sourcefPort = (CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_ENT)inArgs[2];
    fcVectorBit = inArgs[3];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFcActiveActiveMappingSet(devNum, voqNum, 
                                                          sourcefPort, fcVectorBit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmFabricXbarFcActiveActiveMappingGet
*
* DESCRIPTION:
*      Get what bit in flow control message represents given xbar port and
*		what port provides VOQ with flow control messages
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum      - the device number.
*       voqNum      - Virtual Output Queue number: 0 - 23
*
* OUTPUTS:
*       sourcefPort - source port HyperG. Link port: 0 or 1 for voqUnit0, 
*                       2-3 for voqUnit1; because both units receive same 
*                       configuration 0 == 2 and 1 == 3
*       fcVectorBit - bit in flow control message mapped for given VOQ: 
*                       0-11 (12-15 are not valid)
*
* RETURNS:
*       GT_OK                    - on success.
*       GT_HW_ERROR              - on HW error.
*       GT_BAD_PARAM             - on wrong devNum, voqNum.
*       GT_BAD_PTR               - on NULL pointer.
*       GT_NOT_APPLICABLE_DEVICE - on not applicable device
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmFabricXbarFcActiveActiveMappingGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS 	result;
    GT_U8 		devNum;
    GT_U32 		voqNum;
    CPSS_EXMXPM_FABRIC_VOQ_SRC_FC_PORT_ENT sourcefPort;
    GT_U32 		fcVectorBit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum=(GT_U8)inArgs[0];
    voqNum=inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmFabricXbarFcActiveActiveMappingGet(devNum, voqNum,
                                                           &sourcefPort, &fcVectorBit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", sourcefPort, fcVectorBit);

    return CMD_OK;
}

/**** database initialization **************************************/




static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmFabricXbarConfigSet_1",
         &wrCpssExMxPmFabricXbarConfigSet,
         5, 0},
        {"cpssExMxPmFabricXbarConfigGet_1",
         &wrCpssExMxPmFabricXbarConfigGet,
         1, 0},
        {"cpssExMxPmFabricXbarFportMcastCellSwitchingSet",
         &wrCpssExMxPmFabricXbarFportMulticastCellSwitchingSet,
         4, 0},
        {"cpssExMxPmFabricXbarFportMcastCellSwitchingGet",
         &wrCpssExMxPmFabricXbarFportMulticastCellSwitchingGet,
         3, 0},
        {"cpssExMxPmFabricXbarFportConfigSet",
         &wrCpssExMxPmFabricXbarFportConfigSet,
         10, 0},
        {"cpssExMxPmFabricXbarFportConfigGet",
         &wrCpssExMxPmFabricXbarFportConfigGet,
         2, 0},
        {"cpssExMxPmFabricXbarFportBlockingSet",
         &wrCpssExMxPmFabricXbarFportBlockingSet,
         2, 18},
        {"cpssExMxPmFabricXbarFportBlockingGetFirst",
         &wrCpssExMxPmFabricXbarFportBlockingGetFirst,
         2, 0},
        {"cpssExMxPmFabricXbarFportBlockingGetNext",
         &wrCpssExMxPmGetNext,
         0, 0},
        {"cpssExMxPmFabricXbarFportFcSet",
         &wrCpssExMxPmFabricXbarFportFcSet,
         4, 0},
        {"cpssExMxPmFabricXbarFportFcGet",
         &wrCpssExMxPmFabricXbarFportFcGet,
         2, 0},
        {"cpssExMxPmFabricXbarFportArbiterSet",
         &wrCpssExMxPmFabricXbarFportArbiterSet,
         2, 16},
        {"cpssExMxPmFabricXbarFportArbiterGetFirst",
         &wrCpssExMxPmFabricXbarFportArbiterGetFirst,
         2, 0},
        {"cpssExMxPmFabricXbarFportArbiterGetNext",
         &wrCpssExMxPmGetNext,
         0, 0},
        {"cpssExMxPmFabricXbarClassArbiterSet",
         &wrCpssExMxPmFabricXbarClassArbiterSet,
         2, 10},
        {"cpssExMxPmFabricXbarClassArbiterGetFirst",
         &wrCpssExMxPmFabricXbarClassArbiterGet,
         2, 0},
        {"cpssExMxPmFabricXbarClassArbiterGetNext",
         &wrCpssExMxPmGetNext,
         0, 0},
        {"cpssExMxPmFabricXbarShaperSet",
         &wrCpssExMxPmFabricXbarShaperSet,
         7, 0},
        {"cpssExMxPmFabricXbarShaperGet",
         &wrCpssExMxPmFabricXbarShaperGet,
         2, 0},
        {"cpssExMxPmFabricXbarFportFifoStatusGetFirst",
         &wrCpssExMxPmFabricXbarFportFifoStatusGetFirst,
         2, 0},
        {"cpssExMxPmFabricXbarFportFifoStatusGetNext",
         &wrCpssExMxPmGetNext,
         0, 0},
        {"cpssExMxPmFabricXbarFportCellCntrTypeSet",
         &wrCpssExMxPmFabricXbarFportCellCntrTypeSet,
         4, 0},
        {"cpssExMxPmFabricXbarFportCellCntrTypeGet",
         &wrCpssExMxPmFabricXbarFportCellCntrTypeGet,
         3, 0},
        {"cpssExMxPmFabricXbarFportCellCntrGet",
         &wrCpssExMxPmFabricXbarFportCellCntrGet,
         3, 0},
        {"cpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet",
         &wrCpssExMxPmFabricXbarFportTxFifoDroppedCellsCntrGet,
         3, 0},
        {"cpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet",
         &wrCpssExMxPmFabricXbarFportRxDroppedCellsCntrsGet,
         2, 0},
        {"cpssExMxPmFabricXbarFportMulticastCellSwitchingSet",
         &wrCpssExMxPmFabricXbarFportMulticastCellSwitchingSet,
         4, 0},
        {"cpssExMxPmFabricXbarFportMulticastCellSwitchingGet",
         &wrCpssExMxPmFabricXbarFportMulticastCellSwitchingGet,
         3, 0},
        {"cpssExMxPmFabricXbarFportUnicastCellSwitchingSet",
         &wrCpssExMxPmFabricXbarFportUnicastCellSwitchingSet,
         6, 0},
        {"cpssExMxPmFabricXbarFportUnicastCellSwitchingGet",
         &wrCpssExMxPmFabricXbarFportUnicastCellSwitchingGet,
         4, 0},
        {"cpssExMxPmFabricXbarFportDiagCellSwitchSet",
         &wrCpssExMxPmFabricXbarFportDiagnosticCellSwitchingSet,
         3, 0},
        {"cpssExMxPmFabricXbarFportDiagCellSwitchGet",
         &wrCpssExMxPmFabricXbarFportDiagnosticCellSwitchingGet,
         2, 0},
        {"cpssExMxPmFabricXbarFportBroadcastCellSwitchingSet",
         &wrCpssExMxPmFabricXbarFportBroadcastCellSwitchingSet,
         3, 0},
        {"cpssExMxPmFabricXbarFportBroadcastCellSwitchingGet",
         &wrCpssExMxPmFabricXbarFportBroadcastCellSwitchingGet,
         2, 0},
        {"cpssExMxPmFabricXbarFportSrcDevLbhFilterEnSet",
         &wrCpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableSet,
         5, 0},
        {"cpssExMxPmFabricXbarFportSrcDevLbhFilterEnGet",
         &wrCpssExMxPmFabricXbarFportSrcDeviceLbhFilterEnableGet,
         4, 0},
        {"cpssExMxPmFabricMulticastActiveActiveSelectSet",
         &wrCpssExMxPmFabricMulticastActiveActiveSelectSet,
         2, 0},
        {"cpssExMxPmFabricMulticastActiveActiveSelectGet",
         &wrCpssExMxPmFabricMulticastActiveActiveSelectGet,
         1, 0},
        {"cpssExMxPmFabricXbarFcActiveActiveMappingSet",
         &wrCpssExMxPmFabricXbarFcActiveActiveMappingSet,
         4, 0},
        {"cpssExMxPmFabricXbarFcActiveActiveMappingGet",
         &wrCpssExMxPmFabricXbarFcActiveActiveMappingGet,
         2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmFabricXbar
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
GT_STATUS cmdLibInitCpssExMxPmFabricXbar
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

