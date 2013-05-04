/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmBrgCount.c
*
* DESCRIPTION:
*       Wrapper functions for
*           cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgCount.h API's
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
#include <cpss/exMxPm/exMxPmGen/bridge/cpssExMxPmBrgCount.h>

/*******************************************************************************
* cpssExMxPmBrgCntLearnedEntryDiscardGet
*
* DESCRIPTION:
*       Gets the total number of source addresses that were  
*       not learned due to bridge internal congestion.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - physical device number.
*
* OUTPUTS:
*       countValuePtr       - The value of the counter.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on HW error.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_NOT_SUPPORTED    - if API is not supported.
*
* COMMENTS:
*       Learned Entry Discards Counter is clear-on-read.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntLearnedEntryDiscardGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 countValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgCntLearnedEntryDiscardGet(devNum, &countValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", countValue);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntDropCntrModeSet
*
* DESCRIPTION:
*       Sets Drop Counter Mode (configures a Bridge Drop Counter "reason").
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - physical device number.
*       dropMode            - Drop Counter mode.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on HW error.
*       GT_BAD_PARAM        - on wrong devNum or dropMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntDropCntrModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT dropMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropMode = (CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgCntDropCntrModeSet(devNum, dropMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntDropCntrModeGet
*
* DESCRIPTION:
*       Gets the Drop Counter Mode (Bridge Drop Counter "reason").
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - physical device number.
*
* OUTPUTS:
*       dropModePtr         - pointer to the Drop Counter mode.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on HW error.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_BAD_PTR          - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntDropCntrModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_DROP_CNTR_MODE_ENT dropMode;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgCntDropCntrModeGet(devNum, &dropMode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropMode);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntDropCntrSet
*
* DESCRIPTION:
*       Sets the Bridge Ingress Drop Counter of specified device.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       dropCntr        - number of packets that were dropped   
*                         due to drop reason configured by the 
*                         cpssExMxPmBrgCntDropCntrModeSet().
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
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntDropCntrSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 dropCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dropCntr = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgCntDropCntrSet(devNum, dropCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntDropCntrGet
*
* DESCRIPTION:
*       Gets the Bridge Ingress Drop Counter of specified device.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       dropCntrPtr     - pointer to the number of packets that were   
*                         dropped due to drop reason configured by the
*                         cpssExMxPmBrgCntDropCntrModeSet().
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntDropCntrGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 dropCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgCntDropCntrGet(devNum, &dropCntr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", dropCntr);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntMacDaSaSet
*
* DESCRIPTION:
*       Sets a specific MAC DA and SA to be monitored by Host  
*       and Matrix counter groups on specified device.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       macSaPtr        - pointer to the source MAC address.
*       macDaPtr        - pointer to the destination MAC address.
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
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntMacDaSaSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_ETHERADDR macSa;
    GT_ETHERADDR macDa;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    galtisMacAddr(&macSa, (GT_U8*)inArgs[1]);
    galtisMacAddr(&macDa, (GT_U8*)inArgs[2]);

    /* call cpss api function */
    result = cpssExMxPmBrgCntMacDaSaSet(devNum, &macSa, &macDa);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntMacDaSaGet
*
* DESCRIPTION:
*       Gets a MAC DA and SA are monitored by Host  
*       and Matrix counter groups on specified device.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*
* OUTPUTS:
*       macSaPtr        - pointer to the source MAC address.
*       macDaPtr        - pointer to the destination MAC address.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntMacDaSaGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_ETHERADDR macSa;
    GT_ETHERADDR macDa;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
	devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgCntMacDaSaGet(devNum, &macSa, &macDa);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%6b%6b", macSa.arEther, macDa.arEther);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntHostGroupCntrsGet
*
* DESCRIPTION:
*       Gets Bridge Host group counters value of specified device.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum              - physical device number.
*
* OUTPUTS:
*       hostGroupCntrPtr    - pointer to structure with current counters value.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_HW_ERROR         - on HW error.
*       GT_BAD_PARAM        - on wrong devNum.
*       GT_BAD_PTR          - on NULL pointer.
*
* COMMENTS:
*       The Bridge Host counters are clear-on-read.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntHostGroupCntrsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_HOST_CNTR_STC hostGroupCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgCntHostGroupCntrsGet(devNum, &hostGroupCntr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = hostGroupCntr.gtHostInPkts;
    inFields[1] = hostGroupCntr.gtHostOutPkts;
    inFields[2] = hostGroupCntr.gtHostOutBroadcastPkts;
    inFields[3] = hostGroupCntr.gtHostOutMulticastPkts;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1],
                            inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntHostGroupCntrsEndGet

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
* cpssExMxPmBrgCntMatrixGroupCntrsGet
*
* DESCRIPTION:
*       Gets the Bridge Matrix counter value of specified device.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum                  - physical device number.
*
* OUTPUTS:
*       matrixCntrSaDaPktsPtr    - pointer to the number of packets with  
*                                 a MAC SA/DA matching of the CPU-configured 
*                                 MAC SA/DA.
*
* RETURNS:
*       GT_OK                   - on success.
*       GT_HW_ERROR             - on HW error.
*       GT_BAD_PARAM            - on wrong devNum.
*       GT_BAD_PTR              - on NULL pointer.
*
* COMMENTS:
*       The Bridge Matrix counter is clear-on-read.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntMatrixGroupCntrsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U32 matrixCntrSaDaPkts;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmBrgCntMatrixGroupCntrsGet(devNum, &matrixCntrSaDaPkts);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", matrixCntrSaDaPkts);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntBridgeIngressCntrModeSet 
*                                               
* DESCRIPTION:
*       Configures a specified Set of Bridge Ingress  
*       counters to work in requested mode.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSetId       - Counter Set ID [0:1] (there are 2 Bridge Ingress 
*                         Counter Sets).
*       setMode         - count mode of specified Set of Bridge
*                         Ingress Counters.
*       port            - port number monitored by Set of Counters. 
*                         This parameter is applied upon 
*                         CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_MODE_E and
*                         CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E 
*                         counter modes.
*       vlan            - VLAN Id monitored by Counters Set.
*                         This parameter is applied upon 
*                         CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E and
*                         CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E 
*                         counter modes.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, cntrSetId, setMode, port or vlan.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntBridgeIngressCntrModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT cntrSetId;
    CPSS_EXMXPM_BRG_INGRESS_CNTR_MODE_ENT setMode;
    GT_U8 port;
    GT_U16 vlan;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetId = (CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT)inArgs[1];
    setMode = (CPSS_EXMXPM_BRG_INGRESS_CNTR_MODE_ENT)inArgs[2];
    port = (GT_U8)inArgs[3];
    vlan = (GT_U16)inArgs[4];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmBrgCntBridgeIngressCntrModeSet(devNum, cntrSetId, setMode, port, vlan);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntBridgeIngressCntrModeGet
*
* DESCRIPTION:
*       Gets the mode and relevant mode parameter (port number or/and VID) 
*       of specified Bridge Ingress counters Set.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSetId       - Counter Set ID [0:1] (there are 2 Bridge Ingress 
*                         Counter Sets).
*
* OUTPUTS:
*       setModePtr      - pointer to the count mode of specified Set of  
*                         Bridge Ingress Counters.
*       portPtr         - pointer to the port number monitored by Counter Set. 
*                         This parameter is applied upon 
*                         CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_MODE_E and
*                         CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E 
*                         counter modes (can be NULL).
*       vlanPtr         - pointer to the VLAN ID monitored by Counters Set.
*                         This parameter is applied upon 
*                         CPSS_EXMXPM_BRG_INGRESS_CNTR_VID_MODE_E and
*                         CPSS_EXMXPM_BRG_INGRESS_CNTR_PORT_VID_MODE_E 
*                         counter modes (can be NULL).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on error.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or cntrSetId.
*       GT_BAD_PTR      - on NULL pointer setMode.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntBridgeIngressCntrModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT cntrSetId;
    CPSS_EXMXPM_BRG_INGRESS_CNTR_MODE_ENT setMode;
    GT_U8 port;
    GT_U16 vlan;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetId = (CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmBrgCntBridgeIngressCntrModeGet(devNum, cntrSetId, &setMode, &port, &vlan);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", setMode, port, vlan);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntBridgeIngressCntrsGet
*
* DESCRIPTION:
*       Gets a Bridge ingress Port/VLAN/Device counters from
*       specified counter set.
*
* APPLICABLE DEVICES: ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSetId       - counter set number [0:1] (there are 2 Bridge Ingress 
*                         Counter Sets).
*
* OUTPUTS:
*       ingressCntrPtr  - pointer to the structure of Bridge Ingress Counters
*                         current values.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or cntrSetId.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       The Bridge Ingress counters are clear-on-read.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntBridgeIngressCntrsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT cntrSetId;
    CPSS_BRIDGE_INGRESS_CNTR_STC ingressCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetId = (CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgCntBridgeIngressCntrsGet(devNum, cntrSetId, &ingressCntr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = ingressCntr.gtBrgInFrames;
    inFields[1] = ingressCntr.gtBrgVlanIngFilterDisc;
    inFields[2] = ingressCntr.gtBrgSecFilterDisc;
    inFields[3] = ingressCntr.gtBrgLocalPropDisc;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1],
                            inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntBridgeIngressCntrsEndGet

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
* cpssExMxPmBrgCntBridgeEgressCntrModeSet
*
* DESCRIPTION:
*       Configures a specified Set of Bridge Egress counters
*       to work in requested mode.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSetId       - Counter Set ID [0:1] (there are 2 Bridge Egress 
*                         Counter Sets).
*       cntrModeBmp     - counter mode bitmap. 
*       port            - physical port number, if <cntrModeBmp> contains  
*                         assigned corresponding bit(CPSS_EGRESS_CNT_PORT_E).
*       vlan            - VLAN Id, if <cntrModeBmp> contains assigned  
*                         corresponding bit(CPSS_EGRESS_CNT_VLAN_E).
*       tc              - traffic class queue, if <cntrModeBmp> contains  
*                         assigned corresponding bit(CPSS_EGRESS_CNT_TC_E).
*       dpLevel         - drop precedence level (Green, Yellow, Red), if 
*                         <cntrModeBmp> contains assigned corresponding bit
*                         (CPSS_EGRESS_CNT_DP_E).
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, cntrSetId or corresponding to bitmap 
*                         of counter mode parameters: port, vlan, tc, dpLevel.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntBridgeEgressCntrModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT cntrSetId;
    CPSS_PORT_EGRESS_CNT_MODE_ENT cntrModeBmp;
    GT_U8 port;
    GT_U16 vlan;
    GT_U8 tc;
    CPSS_DP_LEVEL_ENT dpLevel;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetId = (CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT)inArgs[1];
    cntrModeBmp = (CPSS_PORT_EGRESS_CNT_MODE_ENT)inArgs[2];
    port = (GT_U8)inArgs[3];
    vlan = (GT_U16)inArgs[4];
    tc = (GT_U8)inArgs[5];
    dpLevel = (CPSS_DP_LEVEL_ENT)inArgs[6];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmBrgCntBridgeEgressCntrModeSet(devNum, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntBridgeEgressCntrModeGet
*
* DESCRIPTION:
*       Gets the mode and relevant mode parameter (port number or/and VID
*       or/and tc or/and dpLevel) of specified Bridge Egress counters Set.    
*       
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSetId       - Counter Set ID [0:1] (there are 2 Bridge Egress 
*                         Counter Sets).
*
* OUTPUTS:
*       cntrModeBmpPtr  - pointer to the counter mode bitmap. 
*       portPtr         - pointer to the physical port number value, if   
*                         <cntrModeBmpPtr> contains assigned corresponding bit
*                         (CPSS_EGRESS_CNT_PORT_E) (can be NULL).
*       vlanPtr         - pointer to the VLAN Id value, if <cntrModeBmpPtr>   
*                         contains assigned corresponding bit
*                         (CPSS_EGRESS_CNT_VLAN_E) (can be NULL).
*       tcPtr           - pointer to the traffic class queue value, if  
*                         <cntrModeBmpPtr> contains assigned corresponding bit
*                         (CPSS_EGRESS_CNT_TC_E) (can be NULL).
*       dpLevelPtr      - pointer to the drop precedence level value 
*                         (Green, Yellow, Red), if  <cntrModeBmpPtr> contains  
*                         assigned corresponding bit (CPSS_EGRESS_CNT_DP_E) 
*                         (can be NULL).
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PTR      - on NULL pointer cntrModeBmp.
*       GT_BAD_PARAM    - on wrong devNum or cntrSetId.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntBridgeEgressCntrModeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT cntrSetId;
    CPSS_PORT_EGRESS_CNT_MODE_ENT cntrModeBmp;
    GT_U8 port;
    GT_U16 vlan;
    GT_U8 tc;
    CPSS_DP_LEVEL_ENT dpLevel;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetId = (CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmBrgCntBridgeEgressCntrModeGet(devNum, cntrSetId, &cntrModeBmp, &port, &vlan, &tc, &dpLevel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d%d", cntrModeBmp, port, vlan, tc, dpLevel);

    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmBrgCntBridgeEgressCntrModeSet
*
* DESCRIPTION:
*       Configures a specified Set of Bridge Egress counters
*       to work in requested mode.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSetId       - Counter Set ID [0:1] (there are 2 Bridge Egress 
*                         Counter Sets).
*       cntrModeBmp     - counter mode bitmap. 
*       port            - physical port number, if <cntrModeBmp> contains  
*                         assigned corresponding bit(CPSS_EGRESS_CNT_PORT_E).
*       vlan            - VLAN Id, if <cntrModeBmp> contains assigned  
*                         corresponding bit(CPSS_EGRESS_CNT_VLAN_E).
*       tc              - traffic class queue, if <cntrModeBmp> contains  
*                         assigned corresponding bit(CPSS_EGRESS_CNT_TC_E).
*       dpLevel         - drop precedence level (Green, Yellow, Red), if 
*                         <cntrModeBmp> contains assigned corresponding bit
*                         (CPSS_EGRESS_CNT_DP_E).
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum, cntrSetId or corresponding to bitmap 
*                         of counter mode parameters: port, vlan, tc, dpLevel.
*
* COMMENTS:
*       None.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntBridgeEgressCntrModeSet_1
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT cntrSetId;
    CPSS_PORT_EGRESS_CNT_MODE_ENT cntrModeBmp =0;
    GT_U8 port;
    GT_U16 vlan;
    GT_U8 tc;
    CPSS_DP_LEVEL_ENT dpLevel;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetId = (CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT)inArgs[1];
    cntrModeBmp |= (GT_BOOL)inArgs[2] == GT_TRUE ? CPSS_EGRESS_CNT_PORT_E : 0;
    cntrModeBmp |= (GT_BOOL)inArgs[3] == GT_TRUE ? CPSS_EGRESS_CNT_VLAN_E : 0;
    cntrModeBmp |= (GT_BOOL)inArgs[4] == GT_TRUE ? CPSS_EGRESS_CNT_TC_E : 0;
    cntrModeBmp |= (GT_BOOL)inArgs[5] == GT_TRUE ? CPSS_EGRESS_CNT_DP_E : 0;
    port = (GT_U8)inArgs[6];
    vlan = (GT_U16)inArgs[7];
    tc = (GT_U8)inArgs[8];
    dpLevel = (CPSS_DP_LEVEL_ENT)inArgs[9];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, port);

    /* call cpss api function */
    result = cpssExMxPmBrgCntBridgeEgressCntrModeSet(devNum, cntrSetId, cntrModeBmp, port, vlan, tc, dpLevel);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmBrgCntBridgeEgressCntrsGet
*
* DESCRIPTION:
*       Gets a Bridge egress counters from specific counter set.
*
* APPLICABLE DEVICES:  ALL EXMXPM Devices.
*
* INPUTS:
*       devNum          - physical device number.
*       cntrSetId       - Counter Set ID [0:1] (there are 2 Bridge Egress 
*                         Counter Sets).
*
* OUTPUTS:
*       egressCntrPtr   - pointer to the structure of Bridge Egress Counters
*                         current values.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_HW_ERROR     - on HW error.
*       GT_BAD_PARAM    - on wrong devNum or cntrSetId.
*       GT_BAD_PTR      - on NULL pointer.
*
* COMMENTS:
*       The Bridge Egress counters are clear-on-read.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntBridgeEgressCntrsGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT cntrSetId;
    CPSS_EXMXPM_BRG_EGRESS_CNTR_STC egressCntr;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    cntrSetId = (CPSS_EXMXPM_BRG_CNTR_SET_ID_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPmBrgCntBridgeEgressCntrsGet(devNum, cntrSetId, &egressCntr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = egressCntr.outUcPkts;
    inFields[1] = egressCntr.outMcPkts;
    inFields[2] = egressCntr.outBcPkts;
    inFields[3] = egressCntr.brgEgrFilterPkts;
    inFields[4] = egressCntr.tailDropPkts;
    inFields[5] = egressCntr.ctrlPkts;
    inFields[6] = egressCntr.frwdRestrictPkts;
    inFields[7] = egressCntr.mcTailDropPkts;


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1],
                                    inFields[2], inFields[3],
                                    inFields[4], inFields[5],
                                    inFields[6], inFields[7]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmBrgCntBridgeEgressCntrsEndGet
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

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{

    {"cpssExMxPmBrgCntLearnedEntryDiscardGet",
        &wrCpssExMxPmBrgCntLearnedEntryDiscardGet,
        1, 0},

    {"cpssExMxPmBrgCntDropCntrModeSet",
        &wrCpssExMxPmBrgCntDropCntrModeSet,
        2, 0},

    {"cpssExMxPmBrgCntDropCntrModeGet",
        &wrCpssExMxPmBrgCntDropCntrModeGet,
        1, 0},

    {"cpssExMxPmBrgCntDropCntrSet",
        &wrCpssExMxPmBrgCntDropCntrSet,
        2, 0},

    {"cpssExMxPmBrgCntDropCntrGet",
        &wrCpssExMxPmBrgCntDropCntrGet,
        1, 0},

    {"cpssExMxPmBrgCntMacDaSaSet",
        &wrCpssExMxPmBrgCntMacDaSaSet,
        3, 0},

    {"cpssExMxPmBrgCntMacDaSaGet",
        &wrCpssExMxPmBrgCntMacDaSaGet,
        1, 0},

    {"cpssExMxPmBrgCntHostGroupCntrsGetFirst",
        &wrCpssExMxPmBrgCntHostGroupCntrsGet,
        1, 0},

    {"cpssExMxPmBrgCntHostGroupCntrsGetNext",
        &wrCpssExMxPmBrgCntHostGroupCntrsEndGet,
        1, 0},

    {"cpssExMxPmBrgCntMatrixGroupCntrsGet",
        &wrCpssExMxPmBrgCntMatrixGroupCntrsGet,
        1, 0},

    {"cpssExMxPmBrgCntBridgeIngressCntrModeSet",
        &wrCpssExMxPmBrgCntBridgeIngressCntrModeSet,
        5, 0},

    {"cpssExMxPmBrgCntBridgeIngressCntrModeSetExt",
        &wrCpssExMxPmBrgCntBridgeIngressCntrModeSet,
        5, 0},

    {"cpssExMxPmBrgCntBridgeIngressCntrModeGet",
        &wrCpssExMxPmBrgCntBridgeIngressCntrModeGet,
        2, 0},

    
    {"cpssExMxPmBrgCntBridgeIngressCntrsGetFirst",
        &wrCpssExMxPmBrgCntBridgeIngressCntrsGet,
        2, 0},

    {"cpssExMxPmBrgCntBridgeIngressCntrsGetNext",
        &wrCpssExMxPmBrgCntBridgeIngressCntrsEndGet,
        2, 0},

    {"cpssExMxPmBrgCntBridgeEgressCntrModeSet",
        &wrCpssExMxPmBrgCntBridgeEgressCntrModeSet,
        7, 0},

    {"cpssExMxPmBrgCntBridgeEgressCntrModeGet",
        &wrCpssExMxPmBrgCntBridgeEgressCntrModeGet,
        2, 0},

    {"cpssExMxPmBrgCntBridgeEgressCntrModeSet_1",
        &wrCpssExMxPmBrgCntBridgeEgressCntrModeSet_1,
        10, 0},
    
    {"cpssExMxPmBrgCntBridgeEgressCntrsGetFirst",
        &wrCpssExMxPmBrgCntBridgeEgressCntrsGet,
        2, 0},

    {"cpssExMxPmBrgCntBridgeEgressCntrsGetNext",
        &wrCpssExMxPmBrgCntBridgeEgressCntrsEndGet,
        2, 0},

    /* CMD_END */

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmBrgCount
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
GT_STATUS cmdLibInitCpssExMxPmBrgCount
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

