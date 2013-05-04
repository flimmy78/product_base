/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapHwInitCpss.c
*
* DESCRIPTION:
*       Wrapper functions for HwInit cpss functions.
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
#include <cpss/exMx/exMxGen/cpssHwInit/cpssExMxHwInit.h>
#include <cpss/exMx/exMxGen/cpssHwInit/cpssExMxHwInitLedCtrl.h>
/*#include <galtisAgent/wrapCpss/exMx/wrapCpssHwInitCpss.h>*/

/*******************************************************************************
* cpssExMxHwLedStreamParamsSet
*
* DESCRIPTION:
*       Sets the Led start and end bits to be driven from the Led bitMap.
*
* INPUTS:
*       devNum          - device number
*       startBit    - The first bit to be driver from the led bitmap.
*                     value that fits in 9 bits
*       endBit      - The last bit to be driver from the led bitmap.
*                     value that fits in 9 bits
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_OUT_OF_RANGE - startBit value or endBit value that not fits in 9 bits
*
* COMMENTS:
*       1.  See the Led bitmap in the device's data-sheet for full detail of
*           the leds bitmap.
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwLedStreamParamsSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_U16  startBit;
    GT_U16  endBit;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    startBit = (GT_U16)inArgs[1];
    endBit = (GT_U16)inArgs[2];

    /* call cpss api function */
    result = cpssExMxHwLedStreamParamsSet(devNum, startBit, endBit);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxHwLedOverrideDataSet
*
* DESCRIPTION:
*       Sets the user defined bits to be driven via the LED stream.
*
* INPUTS:
*       devNum          - device number
*       ledOvrData  - The user defined bits to be driven via the LED stream.
*                     value that fits in 30 bits
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - otherwise.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_OUT_OF_RANGE - ledOvrData value that not fits in 30 bits
*
* COMMENTS:
*       1.  See the Led bitmap in the device's data-sheet for full detail of
*           the leds bitmap.
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwLedOverrideDataSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_U32    ledOvrData;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    ledOvrData = (GT_U32)inArgs[1];

    /* call cpss api function */
    result = cpssExMxHwLedOverrideDataSet(devNum, ledOvrData);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxHwPpPhase1Init
*
* DESCRIPTION:
*       This routine initialize the board specific parameters and registers
*       the packet processor for pre-discovery phase access and diagnostics.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       ppPhase1ParamsPtr  - Packet processor hardware specific parameters.
*
* OUTPUTS:
*       deviceTypePtr - The Pp's device Id.
*
* RETURNS:
*       GT_OK   - on success.
*       GT_FAIL - on failure.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - one of the parameters value is wrong
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_ALREADY_EXIST - the devNum already in use
*       GT_NOT_SUPPORTED - the device not supported by CPSS
*       GT_OUT_OF_CPU_MEM - failed to allocate CPU memory,
*       GT_BAD_VALUE - the driver found unknown device type
*       GT_NOT_IMPLEMENTED - the CPSS was not compiled properly
*
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwPpPhase1Init
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    CPSS_EXMX_PP_PHASE1_INIT_INFO_STC ppPhase1InfoPtr;
    CPSS_PP_DEVICE_TYPE          deviceIdPtr;

    GT_U32 i;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    ppPhase1InfoPtr.devNum = (GT_U8)inArgs[0];
    ppPhase1InfoPtr.busBaseAddr = (GT_U32)inArgs[1];
    ppPhase1InfoPtr.internalPciBase = (GT_U32)inArgs[2];
    ppPhase1InfoPtr.intVecNum = (GT_U32)inArgs[3];
    ppPhase1InfoPtr.intMask = (GT_U32)inArgs[4];
    ppPhase1InfoPtr.coreClock = (GT_U32)inArgs[5];
    ppPhase1InfoPtr.flowDramCfg.dramSize = (CPSS_DRAM_SIZE_ENT)inArgs[6];
    ppPhase1InfoPtr.flowDramCfg.dramFrequency = (CPSS_DRAM_FREQUENCY_ENT)inArgs[7];
    ppPhase1InfoPtr.flowDramCfg.overwriteDef = (GT_BOOL)inArgs[8];
    ppPhase1InfoPtr.flowDramCfg.mrs = (GT_U16)inArgs[9];
    ppPhase1InfoPtr.flowDramCfg.emrs = (GT_U16)inArgs[10];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.tdqss = (GT_U8)inArgs[11];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.trcd = (GT_U8)inArgs[12];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.trp = (GT_U8)inArgs[13];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.twr = (GT_U8)inArgs[14];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.twtr = (GT_U8)inArgs[15];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.tras = (GT_U8)inArgs[16];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.trrd = (GT_U8)inArgs[17];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.tmrd = (GT_U8)inArgs[18];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.trfc = (GT_U8)inArgs[19];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.rd2rdDelay = (GT_U8)inArgs[20];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.rd2wrDelay = (GT_U8)inArgs[21];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.writeCount = (GT_U8)inArgs[22];
    ppPhase1InfoPtr.flowDramCfg.dramTiming.readCount = (GT_U8)inArgs[23];
    ppPhase1InfoPtr.flowDramCfg.refreshCounter = (GT_U8)inArgs[24];
    ppPhase1InfoPtr.flowDramCfg.trCdWr = (GT_U8)inArgs[25];
    ppPhase1InfoPtr.flowDramCfg.operation = (GT_U8)inArgs[26];
    ppPhase1InfoPtr.flowDramCfg.dramPageSize = (CPSS_DRAM_PAGE_SIZE_ENT)inArgs[27];

    ppPhase1InfoPtr.bufferDramCfg.dramSize = (CPSS_DRAM_SIZE_ENT)inArgs[ 28];
    ppPhase1InfoPtr.bufferDramCfg.dramFrequency = (CPSS_DRAM_FREQUENCY_ENT)inArgs[29];
    ppPhase1InfoPtr.bufferDramCfg.overwriteDef = (GT_BOOL)inArgs[30];
    ppPhase1InfoPtr.bufferDramCfg.mrs = (GT_U16)inArgs[31];
    ppPhase1InfoPtr.bufferDramCfg.emrs = (GT_U16)inArgs[32];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.tdqss = (GT_U8)inArgs[33];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.trcd = (GT_U8)inArgs[34];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.trp = (GT_U8)inArgs[35];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.twr = (GT_U8)inArgs[36];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.twtr = (GT_U8)inArgs[37];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.tras = (GT_U8)inArgs[38];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.trrd = (GT_U8)inArgs[39];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.tmrd = (GT_U8)inArgs[40];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.trfc = (GT_U8)inArgs[41];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.rd2rdDelay = (GT_U8)inArgs[42];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.rd2wrDelay = (GT_U8)inArgs[43];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.writeCount = (GT_U8)inArgs[44];
    ppPhase1InfoPtr.bufferDramCfg.dramTiming.readCount = (GT_U8)inArgs[45];
    ppPhase1InfoPtr.bufferDramCfg.refreshCounter = (GT_U8)inArgs[46];
    ppPhase1InfoPtr.bufferDramCfg.trCdWr = (GT_U8)inArgs[47];
    ppPhase1InfoPtr.bufferDramCfg.operation = (GT_U8)inArgs[48];
    ppPhase1InfoPtr.bufferDramCfg.dramPageSize = (CPSS_DRAM_PAGE_SIZE_ENT)inArgs[49];
    ppPhase1InfoPtr.bufferMgmtCfg.overwriteDef = (GT_BOOL)inArgs[50];
    ppPhase1InfoPtr.bufferMgmtCfg.numPpBuffers = (GT_U16)inArgs[51];
    ppPhase1InfoPtr.bufferMgmtCfg.numUplinkBuffers = (GT_U16)inArgs[52];

    ppPhase1InfoPtr.uplinkCfg = (CPSS_PP_UPLINK_CONFIG_ENT)inArgs[53];
    ppPhase1InfoPtr.bufferDram = (CPSS_BUFFER_DRAM_PARAM_ENT)inArgs[54];
    ppPhase1InfoPtr.flowDram = (CPSS_FLOW_DRAM_PARAM_ENT)inArgs[55];
    ppPhase1InfoPtr.mngInterfaceType = (CPSS_PP_INTERFACE_CHANNEL_ENT)inArgs[56];
    ppPhase1InfoPtr.routingSramCfgType = (CPSS_NARROW_SRAM_CFG_ENT)inArgs[57];
    ppPhase1InfoPtr.numOfSramsCfg = (GT_U32)inArgs[58];

    for(i = 0; i < ppPhase1InfoPtr.numOfSramsCfg; i++)
    {
        ppPhase1InfoPtr.sramsCfg[i].sramSize = (CPSS_SRAM_SIZE_ENT)inArgs[59+i*5];
        ppPhase1InfoPtr.sramsCfg[i].sramFrequency = (CPSS_SRAM_FREQUENCY_ENT)inArgs[60+i*5];
        ppPhase1InfoPtr.sramsCfg[i].sramRate = (CPSS_SRAM_RATE_ENT)inArgs[61+i*5];
        ppPhase1InfoPtr.sramsCfg[i].externalMemory = (CPSS_EXTERNAL_MEMORY_ENT)inArgs[62+i*5];
        ppPhase1InfoPtr.sramsCfg[i].sramType = (CPSS_SRAM_TYPE_ENT)inArgs[63+i*5];
    }

    ppPhase1InfoPtr.maxBufSizeMode = (CPSS_BUF_MODE_ENT)inArgs[64+i*5];
    ppPhase1InfoPtr.ppHAState = (CPSS_SYS_HA_MODE_ENT)inArgs[65+i*5];



    /* call cpss api function */
    result = cpssExMxHwPpPhase1Init(&ppPhase1InfoPtr, &deviceIdPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", deviceIdPtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxHwPpStartInit
*
* DESCRIPTION:
*       This phase performs the start-init operation on a given device, and sets
*       it's registers according to the registers value list passed by user.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       devNum          - The device number to which the start-init operation
*                         should be done.
*       loadFromEeprom  - GT_TRUE, trigger the device to re-load the eeprom.
*       initDataList    - List of registers values to be set to the device.
*       initDataListLen - Number of elements in ppRegConfigList
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_TIMEOUT - on timed out retries
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       This function should be invoked after cpssExMxHwPpPhase1Init() and
*       before cpssExMxHwPpPhase2Init().
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwPpStartInit

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                   devNum;
    GT_BOOL                 loadFromEeprom;
    CPSS_REG_VALUE_INFO_STC initDataList;
    GT_U32                  initDataListLen;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    loadFromEeprom = (GT_BOOL)inArgs[1];
    initDataList.addrType = (GT_U32)inArgs[2];
    initDataList.regMask = (GT_U32)inArgs[3];
    initDataList.regVal = (GT_U32)inArgs[4];
    initDataList.repeatCount = (GT_U32)inArgs[5];
    initDataListLen = (GT_U32)inArgs[6];

    /* call cpss api function */
    result = cpssExMxHwPpStartInit(devNum, loadFromEeprom, &initDataList, initDataListLen);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

#if 0
/*******************************************************************************
* cpssExMxHwPpPhase2Init
*
* DESCRIPTION:
*       This function performs basic hardware configurations on the given PP.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       oldDevNum          - The PP's "old" device number.
*       ppPhase2ParamsPtr  - Phase2 initialization parameters..
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - one of the parameters value is wrong
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*       GT_ALREADY_EXIST - the devNum already in use
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwPpPhase2Init
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    oldDevNum;
    CPSS_EXMX_PP_PHASE2_INIT_INFO_STC    ppPhase2ParamsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    oldDevNum = (GT_U8)inArgs[0];
    ppPhase2ParamsPtr.newDevNum = (GT_U8)inArgs[1];
    ppPhase2ParamsPtr.netIfCfg.txDescBlock = (GT_U32)inArgs[2];
    ppPhase2ParamsPtr.netIfCfg.txDescBlockSize = (GT_U32)inArgs[3];
    ppPhase2ParamsPtr.netIfCfg.rxDescBlock = (GT_U32)inArgs[4];
    ppPhase2ParamsPtr.netIfCfg.rxDescBlockSize = (GT_U32)inArgs[5];
    ppPhase2ParamsPtr.netIfCfg.rxBufInfo.allocMethod = (CPSS_RX_BUFF_ALLOC_METHOD_ENT)inArgs[6];
    ppPhase2ParamsPtr.netIfCfg.rxBufInfo.allocMethod = (CPSS_RX_BUFF_ALLOC_METHOD_ENT)inArgs[7];
    ppPhase2ParamsPtr.netIfCfg.rxBufInfo.allocMethod = (CPSS_RX_BUFF_ALLOC_METHOD_ENT)inArgs[8];
    ppPhase2ParamsPtr.netIfCfg.rxBufInfo.allocMethod = (CPSS_RX_BUFF_ALLOC_METHOD_ENT)inArgs[9];
    ppPhase2ParamsPtr.netIfCfg.rxBufInfo.allocMethod = (CPSS_RX_BUFF_ALLOC_METHOD_ENT)inArgs[10];
    ppPhase2ParamsPtr.netIfCfg.rxBufInfo.allocMethod = (CPSS_RX_BUFF_ALLOC_METHOD_ENT)inArgs[11];

    /* call cpss api function */
    result = cpssExMxHwPpPhase2Init(oldDevNum, &ppPhase2ParamsPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}
#endif

/*******************************************************************************
* cpssExMxHwPpUplinkReset
*
* DESCRIPTION:
*       This function performs an Uplink reset operation for the given device.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       devNum  - The device to reset the uplink for.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*       This function returns only after the reset operation was fully
*       completed by the device.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwPpUplinkReset
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
    result = cpssExMxHwPpUplinkReset(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxHwPpUplinkTxQStatusMsgSet
*
* DESCRIPTION:
*       This function configures uplink Tx Queue status messages.
*       The messages are sent to uplink to cause flow control. They report
*       on PPs output packet queues status.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       devNum       - the device number.
*       enable       - GT_TRUE for enable Tx Queue status messages
*       msgFrequency - frequency of messages in 256 clock resolution
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS  wrcpssExMxHwPpUplinkTxQStatusMsgSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL enable;
    GT_U32  msgFrequency;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    msgFrequency = (GT_U32)inArgs[2];

    /* call cpss api function */
    result = cpssExMxHwPpUplinkTxQStatusMsgSet(devNum, enable, msgFrequency);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxHwAuDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Address Update
*       descriptor, for a given device.
*
* APPLICABLE DEVICES: a system with ExMx devices
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwAuDescSizeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    CPSS_PP_DEVICE_TYPE    devType;
    GT_U32             descSizePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devType = (CPSS_PP_DEVICE_TYPE)inArgs[0];

    /* call cpss api function */
    result = cpssExMxHwAuDescSizeGet(devType, &descSizePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "&d", descSizePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxHwRxDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Rx descriptor,
*       for a given device.
*
* APPLICABLE DEVICES: a system with ExMx devices
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK    - on success,
*       GT_FAIL  - on failure..
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwRxDescSizeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    CPSS_PP_DEVICE_TYPE    devType;
    GT_U32             descSizePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devType = (CPSS_PP_DEVICE_TYPE)inArgs[0];

    /* call cpss api function */
    result = cpssExMxHwRxDescSizeGet(devType, &descSizePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "&d", descSizePtr);
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxHwTxDescSizeGet
*
* DESCRIPTION:
*       This function returns the size in bytes of a single Tx descriptor,
*       for a given device.
*
* APPLICABLE DEVICES: a system with ExMx devices
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       descSizePtr- The descrptor's size (in bytes).
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure..
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwTxDescSizeGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    CPSS_PP_DEVICE_TYPE    devType;
    GT_U32             descSizePtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devType = (CPSS_PP_DEVICE_TYPE)inArgs[0];

    /* call cpss api function */
    result = cpssExMxHwTxDescSizeGet(devType, &descSizePtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "&d", descSizePtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxHwRxBufAlignmentGet
*
* DESCRIPTION:
*       This function returns the required alignment in bytes of a RX buffer,
*       for a given device.
*
*
* APPLICABLE DEVICES: a system with ExMx devices
*
* INPUTS:
*       devType - The PP's device type to return the descriptor's size for.
*
* OUTPUTS:
*       byteAlignmentPtr - The buffer alignment (in bytes).
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure..
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwRxBufAlignmentGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    CPSS_PP_DEVICE_TYPE    devType;
    GT_U32        byteAlignmentPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devType = (CPSS_PP_DEVICE_TYPE)inArgs[0];

    /* call cpss api function */
    result = cpssExMxHwRxBufAlignmentGet(devType, &byteAlignmentPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "&d", byteAlignmentPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxHwPpIsUplinkInitialized
*
* DESCRIPTION:
*       This function performs an Uplink reset operation for the given device.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       devNum - the device number.
*
* OUTPUTS:
*       isInitializedPtr - (pointer to) indicates whether the Uplink was
*                           initialized.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum
*       GT_BAD_PTR  - one of the parameters is NULL pointer
*
* COMMENTS:
*       This function returns only after the reset operation was fully
*       completed by the device.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwPpIsUplinkInitialized
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8    devNum;
    GT_BOOL  isInitializedPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxHwPpIsUplinkInitialized(devNum, &isInitializedPtr);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "&d", isInitializedPtr);
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxHwPpSoftResetTrigger
*
* DESCRIPTION:
*       This routine trigger soft reset for a specific PP.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       device - device number to reset.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK       - on success
*       GT_FAIL     - on error
*       GT_HW_ERROR - on hardware error
*
* COMMENTS:
*
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwPpSoftResetTrigger
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
    result = cpssExMxHwPpSoftResetTrigger(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxHwPpHaModeSet
*
* DESCRIPTION:
*       function to set CPU High Availability mode of operation.
*
*  APPLICABLE DEVICES:  all ExMx devices
*
* INPUTS:
*       devNum       - the device number.
*       mode - active or standby
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK   - on success,
*       GT_FAIL - on failure.
*       GT_HW_ERROR - on hardware error
*       GT_BAD_PARAM - wrong devNum or mode
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrcpssExMxHwPpHaModeSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                devNum;
    CPSS_SYS_HA_MODE_ENT mode;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    mode = (CPSS_SYS_HA_MODE_ENT)inArgs[1];

    /* call cpss api function */
    result = cpssExMxHwPpHaModeSet(devNum, mode);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxHwLedStreamParamsSet",
        &wrcpssExMxHwLedStreamParamsSet,
        3, 0},

    {"cpssExMxHwLedOverrideDataSet",
        &wrcpssExMxHwLedOverrideDataSet,
        2, 0},

     {"cpssExMxHwPpPhase1Init",
        &wrcpssExMxHwPpPhase1Init,
       2, 0},

    {"cpssExMxHwPpStartInit",
        &wrcpssExMxHwPpStartInit,
        7, 0},

/*    {"cpssExMxHwPpPhase2Init",
        &wrcpssExMxHwPpPhase2Init,
        12, 0}, */

    {"cpssExMxHwPpUplinkReset",
        &wrcpssExMxHwPpUplinkReset,
        1, 0},

        {"cpssExMxHwPpUplinkTxQStatusMsgSet",
        &wrcpssExMxHwPpUplinkTxQStatusMsgSet,
        3, 0},

    {"cpssExMxHwAuDescSizeGet",
        &wrcpssExMxHwAuDescSizeGet,
        1, 0},

     {"cpssExMxHwRxDescSizeGet",
        &wrcpssExMxHwRxDescSizeGet,
       1, 0},

    {"cpssExMxHwTxDescSizeGet",
        &wrcpssExMxHwTxDescSizeGet,
        1, 0},

    {"cpssExMxHwRxBufAlignmentGet",
        &wrcpssExMxHwRxBufAlignmentGet,
        1, 0},

    {"cpssExMxHwPpIsUplinkInitialized",
        &wrcpssExMxHwPpIsUplinkInitialized,
        1, 0},

        {"cpssExMxHwPpSoftResetTrigger",
        &wrcpssExMxHwPpSoftResetTrigger,
        1, 0},

    {"cpssExMxHwPpHaModeSet",
        &wrcpssExMxHwPpHaModeSet,
        2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitCpssExMxHwInit
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
GT_STATUS cmdLibInitCpssExMxHwInit
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

