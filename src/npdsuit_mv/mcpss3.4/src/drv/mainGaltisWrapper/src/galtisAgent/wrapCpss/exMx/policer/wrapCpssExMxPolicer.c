
/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxGenPolicer.c
*
* DESCRIPTION:
*       Wrapper functions for cpss Policer functions.
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
#include <cpss/exMx/exMxGen/policer/cpssExMxPolicer.h>

/*******************************************************************************
* cpssExMxPolicerInit
*
* DESCRIPTION:
*       Init Policer facility on specified PP device.
*
*       1. Calculate and store in the DB the possible ranges of rates that
*          can be achieved with specific Data Unit size and Tick size.
*       2. Update PP Policer DB with Policer Table base address and Policer
*          Entry size according to the availability of conformance counters.
*       3. Enable/Disable per Policer Entry Conformance Counters
*          (this issue has impact on the Policer Entries partition size).
*       4. Clear all Sets of Management Conformance Counters.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum  - PP's device number.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal devNum.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerInit

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8     devNum;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPolicerInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerCountingEnable
*
* DESCRIPTION:
*       This routine globally enables or disables the billing counting
*       of packets and octets.
*       Packet counters count all packets associated with the policer.
*       Octet Counters count the Count Data Unit (CDU) of the packets
*       associated with the policer. The granularity of the CDU is
*       configurable per Policer entry.
*       To enable or disable Conformance counters use
*       cpssExMxTgCfgPpLogicalInit for 98EX1x6 devices or
*       cpssExMxCfgPpLogicalInit for another EX and MX devices by setting
*       policerConformCountEn.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum - PP's device number.
*       enable - GT_FALSE Per policer counting is not enabled on the device
*                GT_TRUE  Policer counting is enabled on the device.
*                         Counting is triggered based on the Policy Action and
*                         the Ingress policer associated with the packet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       The function call may enable Policer Bypass that could disturb traffic
*       processing. Policer is bypassed to configure Metering and Counting
*       enable/disable bits under traffic and only when Policer conformance
*       countesr are disabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerCountingEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       devNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicerCountingEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerMeteringEnable
*
* DESCRIPTION:
*       This routine globally enables or disables metering on specified
*       PP device (metering/remarking/dropping).
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum         - Pp's device number.
*       enable  - GT_FALSE - Metering is not enabled on the device.
*                 GT_TRUE  - Metering is enabled and is triggered based on the
*                            Policy Action and the Ingress policer associated
*                            with the packet.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK                           - on success.
*       GT_FAIL                         - on error.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       The function call may enable Policer Bypass that could disturb traffic
*       processing. Policer is bypassed to configure Metering and Counting
*       enable/disable bits under traffic and only when Policer conformance
*       countesr are disabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerMeteringEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       devNum;
    GT_BOOL     enable;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicerMeteringEnable(devNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerPacketSizeModeSet
*
* DESCRIPTION:
*       Set Metered Packet Size Mode that the policing and counting is done
*       according to.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum             - Pp's device number.
*       lengthType         - Type of packet size:
*                             L3 datagram size only,
*                             L2 packet length including or
*                             L1 packet length including (preamble + IFG + FCS).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK                           - on success.
*       GT_FAIL                         - on error.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerPacketSizeModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    CPSS_POLICER_PACKET_SIZE_MODE_ENT       lengthType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    lengthType = (CPSS_POLICER_PACKET_SIZE_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicerPacketSizeModeSet(devNum, lengthType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerMeterDuRatioSet
*
* DESCRIPTION:
*       Set Data Unit (DU) mode ratio.
*       Metering operates on Data Units (DUs) of incoming packets. The meter
*       rounds the packet size to the nearest multiple of the DU, according to
*       the DU mode. Converting the packet size to DUs allows the user to have
*       finer granularity for low-rate meters, by using a ratio of
*       1:1 (1 byte = 1 DU), or enables higher rate meters by using a
*       1:16 or 1:64 ratio for the high-rate meters.
*
* APPLICABLE DEVICES: 98EX1x6 only.
*
* INPUTS:
*       devNum          - PP's device number.
*       duRatio     - the DU mode ratio value (actually 16 or 64 bytes).
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerMeterDuRatioSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    CPSS_EXMX_POLICER_METER_DU_RATIO_ENT    duRatio;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    duRatio = (CPSS_EXMX_POLICER_METER_DU_RATIO_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicerMeterDuRatioSet(devNum, duRatio);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerDefaultDscpSet
*
* DESCRIPTION:
*       Set packet's DSCP attribute for non-IP traffic.
*       The default DSCP for non-IP traffic is used for:
*         - CoS marking by Mapping the DSCP Attribute,
*           done by the Policy engine.
*         - CoS re-marking by Layer 3 CoS Re-marking Table,
*           done by the Policer engine.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum - Pp's device number.
*       dscp   - default DSCP.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerDefaultDscpSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    GT_U8                                   dscp;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    dscp = (GT_U8)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicerDefaultDscpSet(devNum, dscp);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerL2RemarkModeSet
*
* DESCRIPTION:
*       Set mode for User Priority (UP) and Traffic Class (TC) based
*       remarking model of the PP (maping UP or TC to CoS).
*
* APPLICABLE DEVICES: 98MX6x5, 98MX6x8, 98EX1x6
*
* INPUTS:
*       devNum       - Pp's device number.
*       mode         - defines CoS parameter, which used as index in the
*                      L2 remarking table (UP or TC).
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_HW_ERROR         - on Hardware error.
*       GT_NOT_SUPPORTED    - if the PP doesn't support the feature.
*
* COMMENTS:
*       Use cpssExMxCosByTcDpSet API to configure L2 remarking table.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerL2RemarkModeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    CPSS_EXMX_POLICER_L2_REMARK_MODE_ENT    mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_EXMX_POLICER_L2_REMARK_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicerL2RemarkModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerModifyUpPacketTypeSet
*
* DESCRIPTION:
*       Enable/disable User Priority modification for
*       IP and Non IP frames in the remarking stage
*
* APPLICABLE DEVICES: 98MX6x5, 98MX6x8, 98EX1x6
*
* INPUTS:
*       devNum      - Pp's device number.
*       packetType  - packet types for UP modification:
*                    1.Enable modification of CoS parameter in the
*                      remarking stage for IP packets only.
*                    2.Enable modification of CoS parameter in the
*                      remarking stage for non-IP packets only.
*                    3.Enable modification of CoS parameter in the
*                      remarking stage for all packets.
*                    4.Disable modification of CoS parameter in the
*                      remarking stage for all packets.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_HW_ERROR         - on Hardware error.
*       GT_NOT_SUPPORTED    - if the PP doesn't support the feature.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerModifyUpPacketTypeSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;

    GT_U8                                           devNum;
    CPSS_EXMX_POLICER_UP_REMARK_PACKET_TYPE_ENT     packetType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    packetType = (CPSS_EXMX_POLICER_UP_REMARK_PACKET_TYPE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicerModifyUpPacketTypeSet(devNum, packetType);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}


/*               table  cpssExMxPolicerEntry 
*
*
*
* cpssExMxPolicerEntrySet
*
* DESCRIPTION:
*       Set a new Policer Entry to perform traffic metering and/or counting of
*       flow aggregate. Function allows create Metering Only or Counting Only
*       Policer Entry.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       policerEntryIndex   - Policer Entry index. Range from 0 to
*                             maxNumOfPolicerEntries - 1, configured by the
*                             cpssExMxTgCfgPpLogicalInit for 98EX1x6 devices or
*                             cpssExMxCfgPpLogicalInit for another EX,MX PP's.
*
*       meterEntryCnfgPtr   - pointer to Metering Part of Policer Entry.
*
*                             Note:If meterEntryCnfgPtr is NULL,
*                                  Counting Only Entry will be created.
*
*       countEntryCnfgPtr   - pointer to Counting Part of Policer Entry.
*
*                             Note:If countEntryCnfgPtr is NULL,
*                                  Metering Only Entry will be created.
*
* OUTPUTS:
*       lbParamsPtr         - pointer to actual Leaky Bucket Parameters.
*                             Relevant when meterEntryCnfgPtr is not NULL
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PARAM        - on wrong Leaky Bucket parameters or when both
*                             meterEntryCnfgPtr and countEntryCnfgPtr are NULL.
*       GT_BAD_PTR          - on lbParamsPtr is NULL pointer
*
* COMMENTS:
*       The leaky bucket parameters are returned as output values.
*       This is due to the hardware rate resolution, the exact rate or burst
*       size requested may not be honored. The returned value gives the user
*       the actual parameters configured in the hardware.
*       Memory for SW Policer Entry should be allocated by calling SW.
*/

/***       table  cpssExMxPolicerEntry  global parameter       ***/

static  GT_U32        policerEntryCnt;

/******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntrySet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    
    GT_U8                                   devNum;
    GT_U32                                  policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfgPtr;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfgPtr;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParamsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];


    policerEntryIndex = (GT_U32)inFields[0];

    meterEntryCnfgPtr.doMetering = (GT_BOOL)inFields[1];

    if(inFields[1])
    {
        meterEntryCnfgPtr.remarkBasedOnL3Cos = (GT_BOOL)inFields[2];
        meterEntryCnfgPtr.remarkDp = (GT_BOOL)inFields[3];
        meterEntryCnfgPtr.modifyTcEnable = (GT_BOOL)inFields[4];
        meterEntryCnfgPtr.remarkBasedOnL2Cos = (GT_BOOL)inFields[5];
        meterEntryCnfgPtr.dropRed = (GT_BOOL)inFields[6];
        meterEntryCnfgPtr.meterColorMode = 
                                       (CPSS_POLICER_COLOR_MODE_ENT)inFields[7];
        meterEntryCnfgPtr.meterMode = 
                                  (CPSS_EXMX_POLICER_METER_MODE_ENT)inFields[8];
        meterEntryCnfgPtr.modifyExternalCosEnable = (GT_BOOL)inFields[9];
        meterEntryCnfgPtr.mngCounterSet = 
                          (CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT)inFields[10];

        switch(inFields[8])
        {
        case CPSS_EXMX_POLICER_METER_SR_TCM_E:
            meterEntryCnfgPtr.lbParams.srTcmParams.cir = (GT_U32)inFields[11];
            meterEntryCnfgPtr.lbParams.srTcmParams.cbs = (GT_U32)inFields[12];
            meterEntryCnfgPtr.lbParams.srTcmParams.ebs = (GT_U32)inFields[13];
            break;
        
        case CPSS_EXMX_POLICER_METER_TR_TCM_E:
            meterEntryCnfgPtr.lbParams.trTcmParams.cir = (GT_U32)inFields[11];
            meterEntryCnfgPtr.lbParams.trTcmParams.cbs = (GT_U32)inFields[12];
            meterEntryCnfgPtr.lbParams.trTcmParams.pbs = (GT_U32)inFields[13];
            meterEntryCnfgPtr.lbParams.trTcmParams.pir = (GT_U32)inFields[14];
            break;
        
        default:
            break;
        }
    }

    countEntryCnfgPtr.doCounting = (GT_BOOL)inFields[15];

    if(inFields[15])
    {
        countEntryCnfgPtr.counterDuMode = 
                              (CPSS_EXMX_POLICER_COUNT_DU_MODE_ENT)inFields[16];
        countEntryCnfgPtr.counterMode = 
                                 (CPSS_EXMX_POLICER_COUNT_MODE_ENT)inFields[17];
    }
    
    /* call cpss api function */
    result = cpssExMxPolicerEntrySet(devNum, policerEntryIndex, 
                          &meterEntryCnfgPtr, &countEntryCnfgPtr, &lbParamsPtr);

    /* pack output arguments to galtis string */
    if(inFields[1])
    {
        switch(inFields[8])
        {
        case CPSS_EXMX_POLICER_METER_SR_TCM_E:
            galtisOutput(outArgs, result, "%d%d%d%s", 
                         lbParamsPtr.srTcmParams.cir,
                         lbParamsPtr.srTcmParams.cbs, 
                         lbParamsPtr.srTcmParams.ebs, "NO_SUCH");
            return CMD_OK;

        case CPSS_EXMX_POLICER_METER_TR_TCM_E:
            galtisOutput(outArgs, result, "%d%d%d%d", 
                         lbParamsPtr.trTcmParams.cir, 
                         lbParamsPtr.trTcmParams.cbs, 
                         lbParamsPtr.trTcmParams.pbs, 
                         lbParamsPtr.trTcmParams.pir);
            return CMD_OK;
            
        default:
            galtisOutput(outArgs, result, "");
            return CMD_OK;
        }
    }
        
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryGet
*
* DESCRIPTION:
*       Get the Policer Entry.
*       The Function returns Metering and Countering parts separately.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum                   - PP's device number.
*       policerEntryIndex        - The Policer Entry index.
*
* OUTPUTS:
*       meterEntryCnfgPtr   - pointer to the Metering Policer Entry parameters.
*                             Note:If meterEntryCnfgPtr is NULL, only Counting
*                                  part of the Policer Entry will be return.
*
*       countEntryCnfgPtr   - pointer to the Counting Policer Entry parameters.
*                             Note:If countEntryCnfgPtr is NULL, only Metering
*                                  part of the Policer Entry will be return.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PARAM        - on NULL pointers (when meterEntryCnfgPtr and
*                             countEntryCnfgPtr are NULL) or illegal devNum.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfgPtr;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfgPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    policerEntryCnt = inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicerEntryGet(devNum, policerEntryCnt, 
                                     &meterEntryCnfgPtr, &countEntryCnfgPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = policerEntryCnt;

    if(meterEntryCnfgPtr.doMetering)
    {
        inFields[1] = meterEntryCnfgPtr.doMetering;
        inFields[2] = meterEntryCnfgPtr.remarkBasedOnL3Cos;
        inFields[3] = meterEntryCnfgPtr.remarkDp;
        inFields[4] = meterEntryCnfgPtr.modifyTcEnable;
        inFields[5] = meterEntryCnfgPtr.remarkBasedOnL2Cos;
        inFields[6] = meterEntryCnfgPtr.dropRed;
        inFields[7] = meterEntryCnfgPtr.meterColorMode;
        inFields[8] = meterEntryCnfgPtr.meterMode;
        inFields[9] = meterEntryCnfgPtr.modifyExternalCosEnable;
        inFields[10] = meterEntryCnfgPtr.mngCounterSet;
     
        switch(meterEntryCnfgPtr.meterMode)
        {
        case CPSS_EXMX_POLICER_METER_SR_TCM_E:
            inFields[11] = meterEntryCnfgPtr.lbParams.srTcmParams.cir;
            inFields[12] = meterEntryCnfgPtr.lbParams.srTcmParams.cbs;
            inFields[13] = meterEntryCnfgPtr.lbParams.srTcmParams.ebs;
            break;
                        
        case CPSS_EXMX_POLICER_METER_TR_TCM_E:
            inFields[11] = meterEntryCnfgPtr.lbParams.trTcmParams.cir;
            inFields[12] = meterEntryCnfgPtr.lbParams.trTcmParams.cbs;
            inFields[13] = meterEntryCnfgPtr.lbParams.trTcmParams.pbs;
            inFields[14] = meterEntryCnfgPtr.lbParams.trTcmParams.pir;
            break;
        
        default:
            break;
        }
    }

    if(countEntryCnfgPtr.doCounting)
    {   
        inFields[15] = countEntryCnfgPtr.doCounting;
        inFields[16] = countEntryCnfgPtr.counterDuMode;
        inFields[17] = countEntryCnfgPtr.counterMode;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", 
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],
                inFields[9],  inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                 result;

    GT_U8                                   devNum, maxNumOfPolicerEntries;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfgPtr;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfgPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    maxNumOfPolicerEntries = (GT_U8)inArgs[2];
    policerEntryCnt++;

    if(policerEntryCnt > maxNumOfPolicerEntries)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPolicerEntryGet(devNum, policerEntryCnt, 
                                     &meterEntryCnfgPtr, &countEntryCnfgPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = policerEntryCnt;

    if(meterEntryCnfgPtr.doMetering)
    {
        inFields[1] = meterEntryCnfgPtr.doMetering;
        inFields[2] = meterEntryCnfgPtr.remarkBasedOnL3Cos;
        inFields[3] = meterEntryCnfgPtr.remarkDp;
        inFields[4] = meterEntryCnfgPtr.modifyTcEnable;
        inFields[5] = meterEntryCnfgPtr.remarkBasedOnL2Cos;
        inFields[6] = meterEntryCnfgPtr.dropRed;
        inFields[7] = meterEntryCnfgPtr.meterColorMode;
        inFields[8] = meterEntryCnfgPtr.meterMode;
        inFields[9] = meterEntryCnfgPtr.modifyExternalCosEnable;
        inFields[10] = meterEntryCnfgPtr.mngCounterSet;
     
        switch(meterEntryCnfgPtr.meterMode)
        {
        case CPSS_EXMX_POLICER_METER_SR_TCM_E:
            inFields[11] = meterEntryCnfgPtr.lbParams.srTcmParams.cir;
            inFields[12] = meterEntryCnfgPtr.lbParams.srTcmParams.cbs;
            inFields[13] = meterEntryCnfgPtr.lbParams.srTcmParams.ebs;
            break;
                        
        case CPSS_EXMX_POLICER_METER_TR_TCM_E:
            inFields[11] = meterEntryCnfgPtr.lbParams.trTcmParams.cir;
            inFields[12] = meterEntryCnfgPtr.lbParams.trTcmParams.cbs;
            inFields[13] = meterEntryCnfgPtr.lbParams.trTcmParams.pbs;
            inFields[14] = meterEntryCnfgPtr.lbParams.trTcmParams.pir;
            break;
        
        default:
            break;
        }
    }

    if(countEntryCnfgPtr.doCounting)
    {   
        inFields[15] = countEntryCnfgPtr.doCounting;
        inFields[16] = countEntryCnfgPtr.counterDuMode;
        inFields[17] = countEntryCnfgPtr.counterMode;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", 
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],
                inFields[9],  inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14],
                inFields[15], inFields[16], inFields[17]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryInvalidate
*
* DESCRIPTION:
*       Invalidate the specified Policer Entry.
*       The physical memory, where the current Policer Entry
*       is allocated, sets to zero (first 128 bit).
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum                   - PP's device number.
*       policerEntryIndex        - index of Policer Entry to be invalidated.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryInvalidate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    
    GT_U8                                   devNum;
    GT_U32                                  policerEntryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    policerEntryIndex = (GT_U32)inFields[0];
    
    /* call cpss api function */
    result = cpssExMxPolicerEntryInvalidate(devNum, policerEntryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryCountingUpdate
*
* DESCRIPTION:
*       Update Counting Part of Policer Entry to flow aggregate counting.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       policerEntryIndex   - Policer Entry index.
*       countEntryCnfgPtr   - pointer to updating parameters of Counting Entry.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PTR          - on countEntryCnfgPtr is NULL pointer
*
* COMMENTS:
*       The Metering Part of Policer Entry remains as it was before updating.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryCountingUpdate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    
    GT_U8                                   devNum;
    GT_U32                                  policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_COUNT_CFG_STC   countEntryCnfgPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    policerEntryIndex = (GT_U32)inFields[0];

    countEntryCnfgPtr.doCounting = (GT_BOOL)inFields[1];
    countEntryCnfgPtr.counterDuMode = 
                               (CPSS_EXMX_POLICER_COUNT_DU_MODE_ENT)inFields[2];
    countEntryCnfgPtr.counterMode = 
                                  (CPSS_EXMX_POLICER_COUNT_MODE_ENT)inFields[3];
    
    /* call cpss api function */
    result = cpssExMxPolicerEntryCountingUpdate(devNum, policerEntryIndex, 
                                                       &countEntryCnfgPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryMeteringUpdate
*
* DESCRIPTION:
*       Update Metering Part of Policer Entry to flow aggregate metering.
*
*       For the 98MX6x8, 98EX6x5, 98EX1x6 PP devices, in case of the Policer
*       Entry Updating from trTCM to srTCM, to prevent a long period of meter
*       inaccuracy, this function should be used only within required
*       Application SW workaround, which unbinds/binds all rules associated
*       with the meter before/after updating.
*
*       If Application SW doesn't support method for bind/unbind rules
*       associated with the meter, upon updating the Entry from trTCM to srTCM,
*       the functioncpssExMxPolicerEntryMeteringUpdateWithBypassWa()
*       shall be used.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       policerEntryIndex   - Policer Entry index.
*       meterEntryCnfgPtr   - pointer to updating parameters of Metering Entry.
*
* OUTPUTS:
*       lbParamsPtr         - pointer to actual Leaky Bucket Parameters.
*
* RETURNS:
*       GT_OK             - on success.
*       GT_FAIL           - on error.
*       GT_BAD_PARAM      - on illegal parameter.
*       GT_HW_ERROR       - on Hardware error.
*       GT_BAD_PTR        - on meterEntryCnfgPtr or lbParamsPtr is NULL pointer
*
* COMMENTS:
*       The Counting Part of Policer Entry remains as it was before updating.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryMeteringUpdate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    
    GT_U8                                   devNum;
    GT_U32                                  policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfgPtr;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParamsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    
    policerEntryIndex = (GT_U32)inFields[0];

    meterEntryCnfgPtr.doMetering = (GT_BOOL)inFields[1];
    meterEntryCnfgPtr.remarkBasedOnL3Cos = (GT_BOOL)inFields[2];
    meterEntryCnfgPtr.remarkDp = (GT_BOOL)inFields[3];
    meterEntryCnfgPtr.modifyTcEnable = (GT_BOOL)inFields[4];
    meterEntryCnfgPtr.remarkBasedOnL2Cos = (GT_BOOL)inFields[5];
    meterEntryCnfgPtr.dropRed = (GT_BOOL)inFields[6];
    meterEntryCnfgPtr.meterColorMode = (CPSS_POLICER_COLOR_MODE_ENT)inFields[7];
    meterEntryCnfgPtr.meterMode = (CPSS_EXMX_POLICER_METER_MODE_ENT)inFields[8];
    meterEntryCnfgPtr.modifyExternalCosEnable = (GT_BOOL)inFields[9];
    meterEntryCnfgPtr.mngCounterSet = 
                          (CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT)inFields[10];

    switch(inFields[8])
    {
    case CPSS_EXMX_POLICER_METER_SR_TCM_E:
        meterEntryCnfgPtr.lbParams.srTcmParams.cir = (GT_U32)inFields[11];
        meterEntryCnfgPtr.lbParams.srTcmParams.cbs = (GT_U32)inFields[12];
        meterEntryCnfgPtr.lbParams.srTcmParams.ebs = (GT_U32)inFields[13];
        break;
    
    case CPSS_EXMX_POLICER_METER_TR_TCM_E:
        meterEntryCnfgPtr.lbParams.trTcmParams.cir = (GT_U32)inFields[11];
        meterEntryCnfgPtr.lbParams.trTcmParams.cbs = (GT_U32)inFields[12];
        meterEntryCnfgPtr.lbParams.trTcmParams.pbs = (GT_U32)inFields[13];
        meterEntryCnfgPtr.lbParams.trTcmParams.pir = (GT_U32)inFields[14];
        break;
    
    default:
        break;
    }

    /* call cpss api function */
    result = cpssExMxPolicerEntryMeteringUpdate(devNum, policerEntryIndex, 
                                         &meterEntryCnfgPtr, &lbParamsPtr);

    /* pack output arguments to galtis string */
    switch(inFields[8])
    {
    case CPSS_EXMX_POLICER_METER_SR_TCM_E:
        galtisOutput(outArgs, result, "%d%d%d%s", 
                     lbParamsPtr.srTcmParams.cir,
                     lbParamsPtr.srTcmParams.cbs, 
                     lbParamsPtr.srTcmParams.ebs, "NO_SUCH");
        return CMD_OK;

    case CPSS_EXMX_POLICER_METER_TR_TCM_E:
        galtisOutput(outArgs, result, "%d%d%d%d", 
                     lbParamsPtr.trTcmParams.cir, 
                     lbParamsPtr.trTcmParams.cbs, 
                     lbParamsPtr.trTcmParams.pbs, 
                     lbParamsPtr.trTcmParams.pir);
        return CMD_OK;
        
    default:
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
        
}

/*******************************************************************************
* cpssExMxPolicerEntryMeteringUpdateWithBypassWa
*
* DESCRIPTION:
*       Update Metering Part of Policer Entry.
*
*       If Application SW doesn't provide method for bind/unbind rules
*       associated with the meter, upon updating the Metering part of
*       Policer Entry from trTCM to srTCM, this function shall be used
*       to prevent a long period of meter inaccuracy in the 98MX6x8, 98EX6x5,
*       98EX1x6 PP devices.
*       This method has the following side effects:
*           - Traffic policing is disabled when the counter is updated. During
*             this time period, non-conforming traffic passes through the
*             device without remarking.
*           - When policing counters are not activated in the device, there is
*             a short instance when the Policer is not activated (Policer
*             is Bypassed). This may cause packets, that should have been
*             filtered due to policy, to pass through the device.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       policerEntryIndex   - Policer Entry index.
*       meterEntryCnfgPtr   - pointer to updating parameters of Metering Entry.
*
* OUTPUTS:
*       lbParamsPtr         - pointer to actual Leaky Bucket Parameters.
*
* RETURNS:
*       GT_OK              - on success.
*       GT_FAIL            - on error.
*       GT_BAD_PARAM       - on illegal parameter.
*       GT_HW_ERROR        - on Hardware error.
*       GT_BAD_PTR         - on meterEntryCnfgPtr or lbParamsPtr is NULL pointer
*
* COMMENTS:
*       The Counting Part of Policer Entry remains as it was before updating.
*       Warning: The function may enables and disables the Policer Bypass.
*       Upon enabled Policer Bypass, the traffic processing is disturbed.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryMeteringUpdateWithBypassWa

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;
    
    GT_U8                                   devNum;
    GT_U32                                  policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_METER_CFG_STC   meterEntryCnfgPtr;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbParamsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    
    policerEntryIndex = (GT_U32)inFields[0];

    meterEntryCnfgPtr.doMetering = (GT_BOOL)inFields[1];
    meterEntryCnfgPtr.remarkBasedOnL3Cos = (GT_BOOL)inFields[2];
    meterEntryCnfgPtr.remarkDp = (GT_BOOL)inFields[3];
    meterEntryCnfgPtr.modifyTcEnable = (GT_BOOL)inFields[4];
    meterEntryCnfgPtr.remarkBasedOnL2Cos = (GT_BOOL)inFields[5];
    meterEntryCnfgPtr.dropRed = (GT_BOOL)inFields[6];
    meterEntryCnfgPtr.meterColorMode = (CPSS_POLICER_COLOR_MODE_ENT)inFields[7];
    meterEntryCnfgPtr.meterMode = (CPSS_EXMX_POLICER_METER_MODE_ENT)inFields[8];
    meterEntryCnfgPtr.modifyExternalCosEnable = (GT_BOOL)inFields[9];
    meterEntryCnfgPtr.mngCounterSet = 
                          (CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT)inFields[10];

    switch(inFields[8])
    {
    case CPSS_EXMX_POLICER_METER_SR_TCM_E:
        meterEntryCnfgPtr.lbParams.srTcmParams.cir = (GT_U32)inFields[11];
        meterEntryCnfgPtr.lbParams.srTcmParams.cbs = (GT_U32)inFields[12];
        meterEntryCnfgPtr.lbParams.srTcmParams.ebs = (GT_U32)inFields[13];
        break;
    
    case CPSS_EXMX_POLICER_METER_TR_TCM_E:
        meterEntryCnfgPtr.lbParams.trTcmParams.cir = (GT_U32)inFields[11];
        meterEntryCnfgPtr.lbParams.trTcmParams.cbs = (GT_U32)inFields[12];
        meterEntryCnfgPtr.lbParams.trTcmParams.pbs = (GT_U32)inFields[13];
        meterEntryCnfgPtr.lbParams.trTcmParams.pir = (GT_U32)inFields[14];
        break;
    
    default:
        break;
    }

    /* call cpss api function */
    result = cpssExMxPolicerEntryMeteringUpdateWithBypassWa(devNum, 
               policerEntryIndex, &meterEntryCnfgPtr, &lbParamsPtr);

    /* pack output arguments to galtis string */
    switch(inFields[8])
    {
    case CPSS_EXMX_POLICER_METER_SR_TCM_E:
        galtisOutput(outArgs, result, "%d%d%d%s", 
                     lbParamsPtr.srTcmParams.cir,
                     lbParamsPtr.srTcmParams.cbs, 
                     lbParamsPtr.srTcmParams.ebs, "NO_SUCH");
        return CMD_OK;

    case CPSS_EXMX_POLICER_METER_TR_TCM_E:
        galtisOutput(outArgs, result, "%d%d%d%d", 
                     lbParamsPtr.trTcmParams.cir, 
                     lbParamsPtr.trTcmParams.cbs, 
                     lbParamsPtr.trTcmParams.pbs, 
                     lbParamsPtr.trTcmParams.pir);
        return CMD_OK;
        
    default:
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
        
}

/*******************************************************************************
* cpssExMxPolicerEntryCountingEnable
*
* DESCRIPTION:
*       Enables / Disables Counting of Policer Entry.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       policerEntryIndex   - The Policer Entry index to update the
*                             Counting fields for.
*       enableCounting      - GT_FALSE -  Disable Counting on rules pointing
*                                         to this Policer Entry.
*                              GT_TRUE  - Enable Counting on rules pointing
*                                         to this Policer Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal devNum.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       This function updates only the <Counting Enable/Disable> field
*       of Policer Entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryCountingEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       devNum;
    GT_U32      policerEntryIndex;
    GT_BOOL     enableCounting;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    policerEntryIndex = (GT_U32)inArgs[1];
    enableCounting = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPolicerEntryCountingEnable(devNum, policerEntryIndex, 
                                                           enableCounting);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryMeteringEnable
*
* DESCRIPTION:
*       Enables / Disables Metering of Policer Entry.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       policerEntryIndex   - The Policer Entry index to update the
*                             Metering fields for.
*       enableMetering      - GT_FALSE -  Disable Metring on rules pointing
*                                         to this Policer Entry.
*                             GT_TRUE  -  Enable Metering on rules pointing
*                                         to this Policer Entry.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal devNum.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       This function updates only the <Metering Enable/Disable> field
*       of Policer Entry and may be applied only for already configured
*       metering Policer Entry.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryMeteringEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       devNum;
    GT_U32      policerEntryIndex;
    GT_BOOL     enableMetering;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    policerEntryIndex = (GT_U32)inArgs[1];
    enableMetering = (GT_BOOL)inArgs[2];

    /* call cpss api function */
    result = cpssExMxPolicerEntryMeteringEnable(devNum, policerEntryIndex, 
                                                           enableMetering);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryMeterParamsCalculate
*
* DESCRIPTION:
*       Calculate Leaky Bucket parameters in the Application format without
*       HW update.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       lbInParamsPtr       - pointer to the desired Leaky Bucket Parameters.
*       meterMode           - meter color marking mode: srTCM or trTCM.
*
* OUTPUTS:
*       lbOutParamsPtr      - pointer to the Leaky Bucket Parameters, could be
*                             supported by the Policer (calculation based on
*                             the data of Leaky Bucket parameters provided by
*                             Application SW - lbInParamsPtr).
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal parameter
*       GT_BAD_PTR          - on lbInParamsPtr or lbOutParamsPtr is NULL pointer
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryMeterParamsCalculate

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    CPSS_EXMX_POLICER_METER_MODE_ENT        meterMode;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbInParamsPtr;
    CPSS_EXMX_POLICER_METER_LB_PARAMS_UNT   lbOutParamsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    meterMode = (CPSS_EXMX_POLICER_METER_MODE_ENT)inArgs[1];

    switch(inArgs[1])
    {
    case CPSS_EXMX_POLICER_METER_SR_TCM_E:
        lbInParamsPtr.srTcmParams.cir = (GT_U32)inArgs[2];
        lbInParamsPtr.srTcmParams.cbs = (GT_U32)inArgs[3]; 
        lbInParamsPtr.srTcmParams.ebs = (GT_U32)inArgs[4];
        break;

    case CPSS_EXMX_POLICER_METER_TR_TCM_E:
        lbInParamsPtr.trTcmParams.cir = (GT_U32)inArgs[2]; 
        lbInParamsPtr.trTcmParams.cbs = (GT_U32)inArgs[3]; 
        lbInParamsPtr.trTcmParams.pbs = (GT_U32)inArgs[4]; 
        lbInParamsPtr.trTcmParams.pir = (GT_U32)inArgs[5];
        break;

    default:
        break;
    }
    /* call cpss api function */
    result = cpssExMxPolicerEntryMeterParamsCalculate(devNum, meterMode, 
                                        &lbInParamsPtr, &lbOutParamsPtr);

    /* pack output arguments to galtis string */
    switch(inArgs[1])
    {
    case CPSS_EXMX_POLICER_METER_SR_TCM_E:
        galtisOutput(outArgs, result, "%d%d%d%s", 
                     lbOutParamsPtr.srTcmParams.cir,
                     lbOutParamsPtr.srTcmParams.cbs, 
                     lbOutParamsPtr.srTcmParams.ebs, "NO_SUCH");
        return CMD_OK;

    case CPSS_EXMX_POLICER_METER_TR_TCM_E:
        galtisOutput(outArgs, result, "%d%d%d%d", 
                     lbOutParamsPtr.trTcmParams.cir, 
                     lbOutParamsPtr.trTcmParams.cbs, 
                     lbOutParamsPtr.trTcmParams.pbs, 
                     lbOutParamsPtr.trTcmParams.pir);
        return CMD_OK;

    default:
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }
}

/*******************************************************************************
* cpssExMxPolicerEntryMeterRefresh
*
* DESCRIPTION:
*       Refresh the Policer Entry meter, needed in order to avoid HW inaccuracy.
*
* APPLICABLE DEVICES: 98EX1x6 only.
*
* INPUTS:
*       devNum                   - PP's device number.
*       policerEntryIndex        - index of Policer Entry, it's meter should be
*                                   refreshed.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on error.
*       GT_TIMEOUT      - on time out.
*       GT_BAD_PARAM    - on illegal parameter.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       To prevent inaccuracies in meters are remained idle for a long period
*       (more than 100 minutes), Application SW shall provide SW Timer that
*       will be handled according to the following rules:
*           1.Start the timer immediately, after meters idle state has been
*             recognized.
*           2.Stop the timer, if the meter idle state has been interrupted
*             earlier, than 100 minutes from the Timer starting.
*           3.The Timer has Expired if it was started 100 minutes ago.
*       In case of Timer Expiration, the Application SW shall call in the loop
*       this function for each Policer Entry.
*
*       The Application SW may pereodicaly refresh all Policer Entries
*       to work without "idle timers".
*
*       Due to the fact that the Policer Entry Counters Reset and
*       the Policer Entry Meter Refresh are performed via
*       <Ingress Policing Reset Counter/Refresh Meter Register> intended for
*       both operation, the Application SW method for meters refreshing shall
*       be protected by a binary semaphore.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryMeterRefresh

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       devNum;
    GT_U32      policerEntryIndex;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    policerEntryIndex = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicerEntryMeterRefresh(devNum, policerEntryIndex);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerGlobalCountersGet
*
* DESCRIPTION:
*       Get Policer Global Counters.
*       The Global Counters are cleared on the read.
*       These counters reflect all traffic directed to the Ingress Policing
*       Engine, independent of the specific Policing Entry.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum  - PP's device number.
*
* OUTPUTS:
*       countersPtr - pointer to Global Counters Values.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerGlobalCountersGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                               result;

    GT_U8                                   devNum;
    CPSS_EXMX_POLICER_GLOBAL_CNTR_STC       countersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPolicerGlobalCountersGet(devNum, &countersPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%s%d%s", countersPtr.rxPckts, 
                                galtisU64COut(&(countersPtr.rxBytes)), 
                                            countersPtr.dropPckts, 
                              galtisU64COut(&(countersPtr.dropBytes)));
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerSetOfManagementCountersSet
*
* DESCRIPTION:
*       Set specified set of Management Conformance Counters:
*           - GREEN, YELLOW and RED Packet Conformance Counters;
*           - GREEN, YELLOW and RED Octet Conformance Counters;
*       to the passed values.
*       This function can be used for Policer Management Counters clearing,
*       for this purpose, the counters values, passede through the function
*       should be 0.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum      - PP's device number.
*       confCntrSet - Management Conformance Counters Set: 1,2,3.
*       countersPtr - pointer to the Management Conformance Counters associated
*                     with the specified set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       Each Policing Entry is associated with one of the 3 conformance counter
*       set, or not at all.
*       The function clear the Policer Shadow Register.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerSetOfManagementCountersSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                result;
    
    GT_U8                                    devNum;
    CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT  confCntrSet;
    CPSS_EXMX_POLICER_MNG_CONF_CNTR_STC      countersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    confCntrSet = (CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT)inFields[0];
    
    galtisU64(&(countersPtr.greenBytes), inFields[1], inFields[2]);
    countersPtr.greenPkts = (GT_U32)inFields[3];
    galtisU64(&(countersPtr.yellowBytes), inFields[4], inFields[5]);
    countersPtr.yellowPkts = (GT_U32)inFields[6];
    galtisU64(&(countersPtr.redBytes), inFields[7], inFields[8]);
    countersPtr.redPkts = (GT_U32)inFields[9];
    galtisU64(&(countersPtr.dropBytes), inFields[10], inFields[11]);
    countersPtr.dropPkts = (GT_U32)inFields[12];

    
    /* call cpss api function */
    result = cpssExMxPolicerSetOfManagementCountersSet(devNum, confCntrSet, 
                                                              &countersPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerSetOfManagementCountersGet
*
* DESCRIPTION:
*       Get Management Conformance Counters value of specified set:
*           - GREEN, YELLOW and RED Packet Conformance Counters;
*           - GREEN, YELLOW and RED Octet Conformance Counters.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum      - PP's device number.
*       confCntrSet - Management Conformance Counters Set: 1,2,3.
*
* OUTPUTS:
*       countersPtr - pointer to the Management Conformance Counters associated
*                     with the specified set.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PARAM        - on NULL pointer.
*
* COMMENTS:
*       Notes: each Policing Entry may be associated with
*       one of 3 conformance counter set, or not at all.
*
*
******** table cpssExMxPolicerSetOfManagementCounters global parameter ********/ 

static  CPSS_EXMX_POLICER_MNG_CONF_CNTR_SET_ENT     confCntrGet;

/******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerSetOfManagementCountersGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    CPSS_EXMX_POLICER_MNG_CONF_CNTR_STC  countersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    confCntrGet = CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET1_E;

    /* call cpss api function */
    result = cpssExMxPolicerSetOfManagementCountersGet(devNum, confCntrGet, 
                                                              &countersPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = confCntrGet;

    inFields[1] = countersPtr.greenBytes.l[0];
    inFields[2] = countersPtr.greenBytes.l[1];
    inFields[3] = countersPtr.greenPkts;
    inFields[4] = countersPtr.yellowBytes.l[0];
    inFields[5] = countersPtr.yellowBytes.l[1];
    inFields[6] = countersPtr.yellowPkts;
    inFields[7] = countersPtr.redBytes.l[0];
    inFields[8] = countersPtr.redBytes.l[1];
    inFields[9] = countersPtr.redPkts;
    inFields[10] = countersPtr.dropBytes.l[0];
    inFields[11] = countersPtr.dropBytes.l[1];
    inFields[12] = countersPtr.dropPkts;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d",inFields[0], 
                 inFields[1],  inFields[2],  inFields[3],  
                 inFields[4],  inFields[5],  inFields[6],  
                 inFields[7],  inFields[8],  inFields[9],  
                 inFields[10], inFields[11], inFields[12]);

    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerSetOfManagementCountersGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                            result;

    GT_U8                                devNum;
    CPSS_EXMX_POLICER_MNG_CONF_CNTR_STC  countersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    confCntrGet++;

    if(confCntrGet > CPSS_EXMX_POLICER_MNG_CONF_COUNTER_SET3_E)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPolicerSetOfManagementCountersGet(devNum, confCntrGet, 
                                                              &countersPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = confCntrGet;

    inFields[1] = countersPtr.greenBytes.l[0];
    inFields[2] = countersPtr.greenBytes.l[1];
    inFields[3] = countersPtr.greenPkts;
    inFields[4] = countersPtr.yellowBytes.l[0];
    inFields[5] = countersPtr.yellowBytes.l[1];
    inFields[6] = countersPtr.yellowPkts;
    inFields[7] = countersPtr.redBytes.l[0];
    inFields[8] = countersPtr.redBytes.l[1];
    inFields[9] = countersPtr.redPkts;
    inFields[10] = countersPtr.dropBytes.l[0];
    inFields[11] = countersPtr.dropBytes.l[1];
    inFields[12] = countersPtr.dropPkts;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d",inFields[0], 
                 inFields[1],  inFields[2],  inFields[3],  
                 inFields[4],  inFields[5],  inFields[6],  
                 inFields[7],  inFields[8],  inFields[9],  
                 inFields[10], inFields[11], inFields[12]);

    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryCountersSet
*
* DESCRIPTION:
*       Set Policer Entry Counters (Packet/Octet and Conformance)
*       to specified values.
*
*       This function is used for the Policer Entry Counters reset for PP's,
*       those don't support "Read-and-Reset" HW atomic operation.
*       To prevent HW inaccuracy, function should be used only within required
*       Application SW workaround, which unbinds/binds all the rules associated
*       with the Policer Entry before/after counter's reset.
*
*
*       If Application SW doesn't provide the method for bind/unbind rules
*       associated with the current Policer Entry, for counters handling, the
*       cpssExMxPolicerEntryCountersSetWithBypassWa() function shall be used.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       policerEntryIndex   - Policer Entry Index.
*       pcktDuCountersPtr   - pointer to Policer Entry Packet/Octet counters
*                             should be set. If this pointer is NULL, the
*                             Packet/Octet counters aren't set.
*       confCountersPtr     - pointer to Policer Entry Conformance counters
*                             should be set.If this pointer is NULL, the
*                             Conformance counters aren't set.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PARAM        - on NULL or invalid pointers.
*       GT_HW_ERROR         - on Hardware error.
*
* COMMENTS:
*       In the 98EX1x6, 98MX6x5 and 98MX6x8 the sizes of the Packet and CDU
*       counters determine according to the current Counter Mode:
*           Hybrid mode        - 26 bit CDU and 22 bit Packet Counters size;
*           Packet Only mode   - 48 bit Packet Counter size;
*           CDU Only mode      - 48 bit CDU Counter size.
*

****       table  cpssExMxPolicerEntry  global parameter       ***/

static  GT_U32        policerEntryCountersCnt;

/******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryCountersSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    
    GT_U8                                           devNum;
    GT_U32                                          policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  pcktDuCountersPtr;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC       confCountersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    policerEntryIndex = (GT_U32)inFields[0];
    
    galtisU64(&(pcktDuCountersPtr.cduCounter), inFields[1], inFields[2]);
    galtisU64(&(pcktDuCountersPtr.pktCounter), inFields[3], inFields[4]);
    galtisU64(&(confCountersPtr.greenDuCounter), inFields[5], inFields[6]);
    galtisU64(&(confCountersPtr.yellowDuCounter), inFields[7], inFields[8]);
    galtisU64(&(confCountersPtr.redDuCounter), inFields[9], inFields[10]);


    /* call cpss api function */
    result = cpssExMxPolicerEntryCountersSet(devNum, policerEntryIndex, 
                                  &pcktDuCountersPtr, &confCountersPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryCountersGet
*
* DESCRIPTION:
*       Get Policer Entry Counters value.
*       Each Ingress Policer supports up to five counters for counting total
*       packets, total CDU and a per-color counter (i.e., Conformance counter):
*       Green, Yellow, and Red.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       policerEntryIndex   - Policer Entry Index.
*
* OUTPUTS:
*       pcktDuCountersPtr - pointer to the values of Policer Entry Packet/Octet
*                           counters.
*       confCountersPtr   - pointer to the values of Policer Entry Conformance
*                           counters.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_FAIL         - on error.
*       GT_BAD_PARAM    - on 2 NULL pointers.
*       GT_HW_ERROR     - on Hardware error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryCountersGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                        result;

    GT_U8                                            devNum;
    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC   pcktDuCountersPtr;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC        confCountersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    policerEntryCountersCnt = inArgs[1];

    /* call cpss api function */
    result = cpssExMxPolicerEntryCountersGet(devNum, policerEntryCountersCnt, 
                                        &pcktDuCountersPtr, &confCountersPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = policerEntryCountersCnt;

    inFields[1] = pcktDuCountersPtr.cduCounter.l[0];
    inFields[2] = pcktDuCountersPtr.cduCounter.l[1];
    inFields[3] = pcktDuCountersPtr.pktCounter.l[0];
    inFields[4] = pcktDuCountersPtr.pktCounter.l[1];
    inFields[5] = confCountersPtr.greenDuCounter.l[0];
    inFields[6] = confCountersPtr.greenDuCounter.l[1];
    inFields[7] = confCountersPtr.yellowDuCounter.l[0];
    inFields[8] = confCountersPtr.yellowDuCounter.l[1];
    inFields[9] = confCountersPtr.redDuCounter.l[0];
    inFields[10] = confCountersPtr.redDuCounter.l[1];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], 
                inFields[2], inFields[3], inFields[4], inFields[5], 
                inFields[6], inFields[7], inFields[8], inFields[9], 
                                                       inFields[10]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryCountersGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                        result;

    GT_U8                                            devNum, 
                                                     maxNumOfPolicerEntryCounters;
    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC   pcktDuCountersPtr;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC        confCountersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    maxNumOfPolicerEntryCounters = (GT_U8)inArgs[2];
    policerEntryCountersCnt++;

    if(policerEntryCountersCnt > maxNumOfPolicerEntryCounters)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPolicerEntryCountersGet(devNum, policerEntryCountersCnt, 
                                        &pcktDuCountersPtr, &confCountersPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = policerEntryCountersCnt;

    inFields[1] = pcktDuCountersPtr.cduCounter.l[0];
    inFields[2] = pcktDuCountersPtr.cduCounter.l[1];
    inFields[3] = pcktDuCountersPtr.pktCounter.l[0];
    inFields[4] = pcktDuCountersPtr.pktCounter.l[1];
    inFields[5] = confCountersPtr.greenDuCounter.l[0];
    inFields[6] = confCountersPtr.greenDuCounter.l[1];
    inFields[7] = confCountersPtr.yellowDuCounter.l[0];
    inFields[8] = confCountersPtr.yellowDuCounter.l[1];
    inFields[9] = confCountersPtr.redDuCounter.l[0];
    inFields[10] = confCountersPtr.redDuCounter.l[1];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], 
                inFields[2], inFields[3], inFields[4], inFields[5], 
                inFields[6], inFields[7], inFields[8], inFields[9], 
                                                       inFields[10]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryCountersGetAndReset
*
* DESCRIPTION:
*       Get and Reset Policer Entry Counters according to the specified
*       reset mode.
*
* APPLICABLE DEVICES: 98MX6x8, 98EX6x5, 98EX1x6.
*
* INPUTS:
*       devNum              - PP's device number.
*       policerEntryIndex   - Policer Entry index.
*       resetMode              - CPSS_EXMX_POLICER_COUNT_RESET_PCKT_DU_E: 
*                                 reset only Policer Entry Packet and Octet
*                                 Billing Counters. Conformance counters are
*                                 not reset and not returned in this mode.
*                              - CPSS_EXMX_POLICER_COUNT_RESET_ALL_E: 
*                                 reset both counter's groups of Policer Entry
*                                 Billing Counters: Packet/Octet and Conformance
*                                 Counters.
*
* OUTPUTS:
*       pcktDuCountersPtr - pointer to the values of Policer Entry Packet/Octet
*                           counters before resetting. If this pointer is NULL,
*                           only reset is performed.
*       confCountersPtr   - pointer to the values of Policer Entry Conformance
*                           counters before resetting. If this pointer is NULL,
*                           only reset is performed.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_HW_ERROR         - on Hardware error.
*       GT_NOT_SUPPORTED    - if PP device does't support this operation.
*       GT_TIMEOUT          - on time out.
*
* COMMENTS:
*       If passed pointer parameters are NULL, the function performs reset of
*       the Policer Entry Counters according to the specified reset mode
*       without "Get" operation.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryCountersGetFirstAndReset

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                        result;

    GT_U8                                            devNum;
    CPSS_EXMX_POLICER_COUNT_RESET_MODE_ENT           resetMode;
    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC   pcktDuCountersPtr;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC        confCountersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    resetMode = (CPSS_EXMX_POLICER_COUNT_RESET_MODE_ENT)inArgs[1];
    policerEntryCountersCnt = inArgs[2];

    /* call cpss api function */
    result = cpssExMxPolicerEntryCountersGetAndReset(devNum, resetMode, 
                                                     policerEntryCountersCnt, 
                                        &pcktDuCountersPtr, &confCountersPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = policerEntryCountersCnt;

    inFields[1] = pcktDuCountersPtr.cduCounter.l[0];
    inFields[2] = pcktDuCountersPtr.cduCounter.l[1];
    inFields[3] = pcktDuCountersPtr.pktCounter.l[0];
    inFields[4] = pcktDuCountersPtr.pktCounter.l[1];
    inFields[5] = confCountersPtr.greenDuCounter.l[0];
    inFields[6] = confCountersPtr.greenDuCounter.l[1];
    inFields[7] = confCountersPtr.yellowDuCounter.l[0];
    inFields[8] = confCountersPtr.yellowDuCounter.l[1];
    inFields[9] = confCountersPtr.redDuCounter.l[0];
    inFields[10] = confCountersPtr.redDuCounter.l[1];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", 
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryCountersGetNextAndReset

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                        result;

    GT_U8                                            devNum, 
                                                     maxNumOfPolicerEntryCounters;
    CPSS_EXMX_POLICER_COUNT_RESET_MODE_ENT           resetMode;
    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC   pcktDuCountersPtr;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC        confCountersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    resetMode = (CPSS_EXMX_POLICER_COUNT_RESET_MODE_ENT)inArgs[1];
    maxNumOfPolicerEntryCounters = (GT_U8)inArgs[3];
    policerEntryCountersCnt++;

    if(policerEntryCountersCnt > maxNumOfPolicerEntryCounters)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxPolicerEntryCountersGetAndReset(devNum, resetMode, 
                                                     policerEntryCountersCnt, 
                                        &pcktDuCountersPtr, &confCountersPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = policerEntryCountersCnt;

    inFields[1] = pcktDuCountersPtr.cduCounter.l[0];
    inFields[2] = pcktDuCountersPtr.cduCounter.l[1];
    inFields[3] = pcktDuCountersPtr.pktCounter.l[0];
    inFields[4] = pcktDuCountersPtr.pktCounter.l[1];
    inFields[5] = confCountersPtr.greenDuCounter.l[0];
    inFields[6] = confCountersPtr.greenDuCounter.l[1];
    inFields[7] = confCountersPtr.yellowDuCounter.l[0];
    inFields[8] = confCountersPtr.yellowDuCounter.l[1];
    inFields[9] = confCountersPtr.redDuCounter.l[0];
    inFields[10] = confCountersPtr.redDuCounter.l[1];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", 
                inFields[0],  inFields[1],  inFields[2],
                inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],
                              inFields[9],  inFields[10]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryCountersSetWithBypassWa
*
* DESCRIPTION:
*       Set Policer Entry Counters (Packet/Octet and Conformance)
*       to specified values.
*
*       This function is used for the Policer Entry Counters reset for PP's,
*       those don't support "Read-and-Reset" HW atomic operation.
*
*       If Application SW doesn't provide method for bind/unbind rules
*       associated with the Policer entry, this function shall be used
*       to prevent HW inaccuracy.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum              - PP's device number.
*       policerEntryIndex   - Policer Entry Index.
*       pcktDuCountersPtr   - pointer to Policer Entry Packet/Octet counters
*                             should be set.If this pointer is NULL, the
*                             Packet/Octet counters aren't set.
*       confCountersPtr     - pointer to Policer Entry Conformance counters
*                             should be set. If this pointer is NULL, the
*                             Packet/Octet counters aren't set.
*
* OUTPUTS:
*
* RETURNS:
*       GT_OK          - on success.
*       GT_FAIL        - on error.
*       GT_HW_ERROR    - on Hardware error.
*       GT_BAD_PARAM   - when the both pointers are NULL.
*
* COMMENTS:
*       The function call may enable Policer Bypass that could disturb traffic
*       processing. Policer is bypassed to configure Metering and Counting
*       enable/disable bits under traffic and only when Policer conformance
*       countesr are disabled.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryCountersSetWithBypassWa

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    
    GT_U8                                           devNum;
    GT_U32                                          policerEntryIndex;
    CPSS_EXMX_POLICER_ENTRY_PACKET_DU_COUNTERS_STC  pcktDuCountersPtr;
    CPSS_EXMX_POLICER_ENTRY_CONF_COUNTERS_STC       confCountersPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    policerEntryIndex = (GT_U32)inFields[0];
    
    galtisU64(&(pcktDuCountersPtr.cduCounter), inFields[1], inFields[2]);
    galtisU64(&(pcktDuCountersPtr.pktCounter), inFields[3], inFields[4]);
    galtisU64(&(confCountersPtr.greenDuCounter), inFields[5], inFields[6]);
    galtisU64(&(confCountersPtr.yellowDuCounter), inFields[7], inFields[8]);
    galtisU64(&(confCountersPtr.redDuCounter), inFields[9], inFields[10]);

    
    /* call cpss api function */
    result = cpssExMxPolicerEntryCountersSetWithBypassWa(devNum, 
        policerEntryIndex, &pcktDuCountersPtr, &confCountersPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerCountersAlarmLimitSet
*
* DESCRIPTION:
*       Set the alarm-limit for Policer Entry counters.
*       These are global limits applied to all Policer Entries.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum           - PP's device number.
*       alarmLimitPtr    - pointer to Alarm Limits structure for
*                          Conformance, Packet and CDU (in the Hybrid mode)
*                          Policer Entry Counters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_HW_ERROR         - on Hardware error.
*       GT_BAD_PARAM        - on wrong alarm limit or devNum.
*
* COMMENTS:
*       For the 98EX1x6, 98MX6x5 and 98MX6x8 PP's this function is useful upon
*       Hybrid Counter Mode only ( 26 bit CDU and 22 bit Packet Counters size ).
*       For Packet/CDU Only Counter Mode (48 bit Packet/CDU Counter size),
*       the function cpssExMxPolicerLargeCountersAlarmLimitSet() shall be used.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerCountersAlarmLimitSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    
    GT_U8                                           devNum;
    CPSS_EXMX_POLICER_ALARM_LIMIT_STC               alarmLimitPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    alarmLimitPtr.packetCountLimit = (GT_U32)inFields[0];
    alarmLimitPtr.cduCountLimit = (GT_U32)inFields[1];
    
    galtisU64(&(alarmLimitPtr.conformanceCountLimit), inFields[2], inFields[3]);

    
    /* call cpss api function */
    result = cpssExMxPolicerCountersAlarmLimitSet(devNum, &alarmLimitPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerLargeCountersAlarmLimitSet
*
* DESCRIPTION:
*       Set the alarm-limit for Policer Entry large Packet/CDU Counters
*       (Upon Packet/CDU Only Counter Mode, Packet/CDU counters size is 48 Bit).
*       These are global limits applied to all Policer Entries.
*
* APPLICABLE DEVICES: 98MX6x8, 98EX6x5, 98EX1x6.
*
* INPUTS:
*       devNum             - PP's device number.
*       largeAlarmLimitPtr - pointer to Alarm Limits structure for
*                                 Large Packet and CDU Policer Entry Counters.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_BAD_PARAM        - on illegal parameter.
*       GT_HW_ERROR         - on Hardware error.
*       GT_NOT_SUPPORTED    - on not supported function for device
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerLargeCountersAlarmLimitSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS                                       result;
    
    GT_U8                                           devNum;
    CPSS_EXMX_POLICER_ALARM_LIMIT_LARGE_STC         largeAlarmLimitPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    galtisU64(&(largeAlarmLimitPtr.packetCountLimit), inFields[0], inFields[1]);
    galtisU64(&(largeAlarmLimitPtr.cduCountLimit), inFields[2], inFields[3]);

    
    /* call cpss api function */
    result = cpssExMxPolicerLargeCountersAlarmLimitSet(devNum, 
                                          &largeAlarmLimitPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet
*
* DESCRIPTION:
*       Return the counter type (CDU or Packet counter) and Policer Entry Index
*       offset that cause the alarm.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum                  - PP's device number.
*
* OUTPUTS:
*       cduCountExceedPtr       - (pointer to Boolean) if it is GT_TRUE, CDU
*                                 counter was exceeded and caused alarm.
*       pcktCountExceedPtr      - (pointer to Boolean) if it is GT_TRUE, Packet
*                                 counter was exceeded and caused alarm.
*       policerEntryIndexPtr    - pointer to the Policer Entry which the
*                                 exceeded counter belongs to.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_HW_ERROR         - on Hardware error.
*       GT_ERROR            - if the alarm information is not valid.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       devNum;
    GT_BOOL     cduCountExceedPtr;
    GT_BOOL     pcktCountExceedPtr;
    GT_U32      policerEntryIndexPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet(devNum, 
                                                  &cduCountExceedPtr, 
                                                 &pcktCountExceedPtr, 
                                               &policerEntryIndexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d", cduCountExceedPtr, 
                     pcktCountExceedPtr, policerEntryIndexPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPolicerEntryConfCountAlarmInfoGet
*
* DESCRIPTION:
*       Return the Conformance Counter Type (GREEN, YELLOW or RED) and Policer
*       Entry Index offset that cause the alarm.
*
* APPLICABLE DEVICES: ALL ExMx Devices.
*
* INPUTS:
*       devNum                 - PP's device number.
*
* OUTPUTS:
*       greenExceedPtr          - (pointer to Boolean) when GT_TRUE, Green
*                                 counter was exceeded and caused alarm.
*       yellowExceedPtr         - (pointer to Boolean) when GT_TRUE, Yellow
*                                 counter was exceeded and caused alarm.
*       redExceedPtr            - (pointer to Boolean) when GT_TRUE, Red
*                                 counter was exceeded and caused alarm.
*       policerEntryIndexPtr    - pointer to the Policer Entry which the
*                                 exceeded counter belongs to.
*
* RETURNS:
*       GT_OK               - on success.
*       GT_FAIL             - on error.
*       GT_BAD_PTR          - on NULL pointer.
*       GT_HW_ERROR         - on Hardware error
*       GT_ERROR            - if the alarm information is not valid
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPolicerEntryConfCountAlarmInfoGet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS   result;

    GT_U8       devNum;
    GT_BOOL     greenExceedPtr;
    GT_BOOL     yellowExceedPtr;
    GT_BOOL     redExceedPtr;
    GT_U32      policerEntryIndexPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPolicerEntryConfCountAlarmInfoGet(devNum, &greenExceedPtr, 
                                               &yellowExceedPtr, &redExceedPtr, 
                                                         &policerEntryIndexPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d%d%d", greenExceedPtr, yellowExceedPtr, 
                                           redExceedPtr, policerEntryIndexPtr);
    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxPolicerInit",
        &wrCpssExMxPolicerInit,
        1, 0},

    {"cpssExMxPolicerCountingEnable",
        &wrCpssExMxPolicerCountingEnable,
        2, 0},

    {"cpssExMxPolicerMeteringEnable",
        &wrCpssExMxPolicerMeteringEnable,
        2, 0},

    {"cpssExMxPolicerPacketSizeModeSet",
        &wrCpssExMxPolicerPacketSizeModeSet,
        2, 0},

    {"cpssExMxPolicerMeterDuRatioSet",
        &wrCpssExMxPolicerMeterDuRatioSet,
        2, 0},

    {"cpssExMxPolicerDefaultDscpSet",
        &wrCpssExMxPolicerDefaultDscpSet,
        2, 0},

    {"cpssExMxPolicerL2RemarkModeSet",
        &wrCpssExMxPolicerL2RemarkModeSet,
        2, 0},

    {"cpssExMxPolicerModifyUpPacketTypeSet",
        &wrCpssExMxPolicerModifyUpPacketTypeSet,
        2, 0},

    {"cpssExMxPolicerEntrySet",
        &wrCpssExMxPolicerEntrySet,
        3, 18},

    {"cpssExMxPolicerEntryGetFirst",
        &wrCpssExMxPolicerEntryGetFirst,
        3, 0},

    {"cpssExMxPolicerEntryGetNext",
        &wrCpssExMxPolicerEntryGetNext,
        3, 0},

    {"cpssExMxPolicerEntryDelete",
        &wrCpssExMxPolicerEntryInvalidate,
        3, 18},

    {"cpssExMxPolicerEntryCountingUpdateSet",
        &wrCpssExMxPolicerEntryCountingUpdate,
        1, 4},

    {"cpssExMxPolicerEntryMeteringUpdateSet",
        &wrCpssExMxPolicerEntryMeteringUpdate,
        1, 15},

    {"policerEntryMeteringUpdateWithBypassWaSet",
        &wrCpssExMxPolicerEntryMeteringUpdateWithBypassWa,
        1, 15},

    {"cpssExMxPolicerEntryCountingEnable",
        &wrCpssExMxPolicerEntryCountingEnable,
        3, 0},

    {"cpssExMxPolicerEntryMeteringEnable",
        &wrCpssExMxPolicerEntryMeteringEnable,
        3, 0},

    {"cpssExMxPolicerEntryMeterParamsCalculate",
        &wrCpssExMxPolicerEntryMeterParamsCalculate,
        6, 0},

    {"cpssExMxPolicerEntryMeterRefresh",
        &wrCpssExMxPolicerEntryMeterRefresh,
        2, 0},

    {"cpssExMxPolicerGlobalCountersGet",
        &wrCpssExMxPolicerGlobalCountersGet,
        1, 0},

    {"cpssExMxPolicerSetOfManagementCountersSet",
        &wrCpssExMxPolicerSetOfManagementCountersSet,
        1, 13},

    {"cpssExMxPolicerSetOfManagementCountersGetFirst",
        &wrCpssExMxPolicerSetOfManagementCountersGetFirst,
        1, 0},

    {"cpssExMxPolicerSetOfManagementCountersGetNext",
        &wrCpssExMxPolicerSetOfManagementCountersGetNext,
        1, 0},

    {"cpssExMxPolicerEntryCountersSet",
        &wrCpssExMxPolicerEntryCountersSet,
        3, 11},

    {"cpssExMxPolicerEntryCountersGetFirst",
        &wrCpssExMxPolicerEntryCountersGetFirst,
        3, 0},

    {"cpssExMxPolicerEntryCountersGetNext",
        &wrCpssExMxPolicerEntryCountersGetNext,
        3, 0},

    {"policerEntryCountersGetAndResetGetFirst",
        &wrCpssExMxPolicerEntryCountersGetFirstAndReset,
        4, 0},

    {"policerEntryCountersGetAndResetGetNext",
        &wrCpssExMxPolicerEntryCountersGetNextAndReset,
        4, 0},

    {"PolicerEntryCountersSetWithBypassWaSet",
        &wrCpssExMxPolicerEntryCountersSetWithBypassWa,
        1, 11},

    {"cpssExMxPolicerCountersAlarmLimitSet",
        &wrCpssExMxPolicerCountersAlarmLimitSet,
        1, 4},

    {"cpssExMxPolicerLrgCountersAlarmLimitSet",
        &wrCpssExMxPolicerLargeCountersAlarmLimitSet,
        1, 4},

    {"cpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet",
        &wrCpssExMxPolicerEntryPacketAndDuCountAlarmInfoGet,
        1, 0},

    {"cpssExMxPolicerEntryConfCountAlarmInfoGet",
        &wrCpssExMxPolicerEntryConfCountAlarmInfoGet,
        1, 0},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))



/*******************************************************************************
* cmdLibInitCpssExMxPolicer
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
GT_STATUS cmdLibInitCpssExMxPolicer
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}






