/*******************************************************************************
*              (c), Copyright 2001, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxNetIf.c
*
* DESCRIPTION:
*       Wrapper functions for NetworkIf cpss functions
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <galtisAgent/wrapCpss/exMx/NetworkIf/cmdCpssExMxNetTransmit.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>


/* Feature specific includes. */
#include <cpss/generic/cpssTypes.h>
#include <cpss/exMx/exMxGen/networkIf/cpssExMxGenNetIfTypes.h>
#include <cpss/exMx/exMxGen/networkIf/cpssExMxNetIf.h>
#include <cpss/exMx/exMxGen/trunk/cpssExMxTrunk.h>
#include <cpssCommon/private/prvCpssCrc.h>
#include <cpssCommon/private/prvCpssMemLib.h>

/* table TxNetIf/ExtTxNetIf global variables */
static GT_U32      netTxCurrIndex=0; /*Index of last descriptor got from table*/
extern PKT_DESC_STC* cpssTxPacketDescTbl;

GT_POOL_ID txBuffersPoolId;

static const char hexcode[] = "0123456789ABCDEF";



/*******************************************************************************
* setBufferArraysOfEqualSize
*
* DESCRIPTION:
*       build GT_BYTE_ARRY from string expanding or trunkating to size
*
* INPUTS:
*       sourceDataPtr       - byte array buffer (hexadecimal string)
*       totalSize           - exact total size of the buffer in bytes
*       bufferSize          - wanted size of each data buffer
*
* OUTPUTS:
*       pcktData    - array of byte array data
*       pcktDataLen - array of byte array size
*       numOfBuffs  - number of buffers the data was splited to.
*
* RETURNS:
*       None
*
* COMMENTS:
*
*******************************************************************************/
static GT_STATUS setBufferArraysOfEqualSize
(
    IN GT_U8   *sourceDataPtr,
    IN GT_U32   totalSize,
    IN GT_U32   bufferSize,
    OUT GT_U8   *pcktData[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN],
    OUT GT_U32  pcktDataLen[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN],
    OUT GT_U32  *numOfBuffs

)
{
    GT_U32 element;
    GT_U32 len = totalSize;
    GT_U32 numOfBuffers = len / bufferSize;
    GT_U32 sizeOfLastBuff = len % bufferSize;
    GT_U32 i;

    numOfBuffers = (sizeOfLastBuff > 0) ? numOfBuffers + 1 : numOfBuffers;

    if (numOfBuffers > MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN)
        return GT_NO_RESOURCE;

    if(cmdOsPoolGetBufFreeCnt(txBuffersPoolId) < numOfBuffers)
        return GT_NO_RESOURCE;

    /* if the length of the data is bigger than the length given by the user,
       the data is truncate otherwise it is expanded */

    for(i = 0; i < numOfBuffers ; i++)
    {
        if(len < bufferSize)
            bufferSize = len;
        else
            len = len - bufferSize;

        pcktData[i] = cmdOsPoolGetBuf(txBuffersPoolId);

        /*for every buffer, we will copy the data*/
        for (element = 0; element < bufferSize; element++)
        {
            pcktData[i][element] = (int)(cmdOsStrChr(hexcode, cmdOsToUpper(*sourceDataPtr++)) - (int)hexcode)<< 4;

            pcktData[i][element]+= (int)(cmdOsStrChr(hexcode, cmdOsToUpper(*sourceDataPtr++)) - (int)hexcode);
        }

        pcktDataLen[i] = bufferSize;

    }

    *numOfBuffs = numOfBuffers;

    return GT_OK;
}

/*******************************************************************************
* freeBufferArraysOfEqualSize
*
* DESCRIPTION:
*       Free GT_BYTE_ARRY from string
*
* INPUTS:
*       pcktData    - array of byte array data
*       pcktDataLen - array of byte array size
*
* OUTPUTS:
*       None
*
* RETURNS:
*       None
*
* COMMENTS:
*
* Toolkit:
*
*******************************************************************************/
static GT_VOID freeBufferArraysOfEqualSize
(
    IN GT_U8   *pcktData[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN],
    IN GT_U32  pcktDataLen[MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN]
)
{

    GT_U32 i;
    for (i=0; i<MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN; i++)
    {

        /*nothing to free */
        if((pcktData[i] == NULL) || (pcktDataLen[i] == 0))
            break;

         cmdOsPoolFreeBuf(txBuffersPoolId, pcktData[i]);
   }
}


/*============================
          CONTROL
=============================*/

/*******************************************************************************
* cpssExMxNetIfRxQueueEnable
*
* DESCRIPTION:
*       Enable/Disable the specified traffic class queue for RX
*       on all packet processors in the system.
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*        devNum    - device number
*        enable    - GT_TRUE, enable queue
*                    GT_FALSE, disable queue
*        queueIdx  - traffic class queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*
* GalTis:
*       Command - netQueueEnable
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfRxQueueEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_BOOL enable;
    GT_U8   queueIdx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    queueIdx = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxNetIfRxQueueEnable(devNum, enable, queueIdx);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxNetIfTxQueueEnable
*
* DESCRIPTION:
*       Enable/Disable the specified traffic class queue for TX
*       on all packet processors in the system.
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*        devNum    - device number
*        enable    - GT_TRUE, enable queue
*                    GT_FALSE, disable queue
*        queueIdx  - traffic class queue
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*
* GalTis:
*       Command - netQueueEnable
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfTxQueueEnable

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_BOOL enable;
    GT_U8   queueIdx;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    enable = (GT_BOOL)inArgs[1];
    queueIdx = (GT_U8)inArgs[2];

    /* call cpss api function */
    result = cpssExMxNetIfTxQueueEnable(devNum, enable, queueIdx);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*Table of counters for all queues*/
static GT_U8 queueIdx;

/*******************************************************************************
* cpssExMxNetIfRxCountersGet
*
* DESCRIPTION:
*       For each packet processor, get the Rx packet counters from its SDMA
*       packet interface.  Return the aggregate counter values for the given
*       traffic class queue.
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*     devNum    - device number
*     queueIdx  - traffic class queue
*
* OUTPUTS:
*     rxCounters   - rx counters on this queue
*
* RETURNS:
*       GT_OK           - on success
*       GT_FAIL         - on error
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*       Counters are reset on every read.
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfRxCountersGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                          devNum;
    CPSS_EXMX_NET_RX_COUNTERS_STC  rxCounters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queueIdx = 0;

    /* call cpss api function */
    result = cpssExMxNetIfRxCountersGet(devNum, queueIdx, &rxCounters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = rxCounters.rxInPkts;
    inFields[1] = rxCounters.rxInOctets;

    /* pack and output table fields */
    fieldOutput("%d%d%d", queueIdx, inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


CMD_STATUS wrCpssExMxNetIfRxCountersGetNext

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                          devNum;
    CPSS_EXMX_NET_RX_COUNTERS_STC  rxCounters;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queueIdx++;

    if (queueIdx > 7)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    result = cpssExMxNetIfRxCountersGet(devNum, queueIdx, &rxCounters);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = rxCounters.rxInPkts;
    inFields[1] = rxCounters.rxInOctets;

    /* pack and output table fields */
    fieldOutput("%d%d%d", queueIdx, inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/* Table of totals for all queues */
static GT_U8 queueNum;

/* All values of 8 Rx Error counters */
CPSS_NET_SDMA_RX_ERROR_COUNTERS_STC     rxErrCount;

/*******************************************************************************
* wrCpssExMxNetIfRxErrorCountGetFirst
*
* DESCRIPTION:
*       Returns the total number of Rx resource errors that occurred on a given
*       Rx queue (for all devices in unit).
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*       devNum      - device number
*       queue       - The Rx queue to get the counters for.
*
* OUTPUTS:
*       rxErrCount  - The total number of Rx resource errors on the given queue.
*
* RETURNS:
*       GT_OK           - on success,
*       GT_NO_RESOURCE  - failed to allocate a t2c struct,
*       GT_BAD_PTR      - one of the parameters is NULL pointer
*       GT_BAD_PARAM    - on bad parameters
*       GT_FAIL         - otherwise.
*
* COMMENTS:
*       1.  queue parameter must be in the range of 0-7.
*       2.  The count returned by this function reflects the number of Rx errors
*           that occured since the last call to this function.
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfRxErrorCountGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8   devNum;
    GT_32   rxErrCountFirst;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queueNum = 0;

    /* call cpss api function */
    result = cpssExMxNetIfRxErrorCountGet(devNum, &rxErrCount);
    rxErrCountFirst = rxErrCount.counterArray[queueNum];

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }


    /* pack and output table fields */
    fieldOutput("%d%d", queueNum, rxErrCountFirst);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


CMD_STATUS wrCpssExMxNetIfRxErrorCountGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8   devNum;
    GT_U32  rxErrCountNext;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    queueNum++;

    if (queueNum > 7)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call cpss api function */
    rxErrCountNext = rxErrCount.counterArray[queueNum];

    /* pack and output table fields */
    fieldOutput("%d%d", queueNum, rxErrCountNext);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%f");
    return CMD_OK;
}


/**********Table of CPU Code Params******************************/
static CPSS_NET_RX_CPU_CODE_ENT cpuCodeArr[] =
{
    CPSS_NET_UNKNOWN_UC_E,
    CPSS_NET_UNREG_MC_E,
    CPSS_NET_IPV6_ICMP_PACKET_E,
    CPSS_NET_INGRESS_MIRRORED_TO_ANLYZER_E,
    CPSS_NET_EGRESS_MIRRORED_TO_ANLYZER_E,
    CPSS_NET_EGRESS_SAMPLED_E,
    CPSS_NET_INGRESS_SAMPLED_E,
    CPSS_NET_INVALID_PCL_KEY_TRAP_E
};

static GT_U32 cpuCodeIdx;

/*******************************************************************************
* cpssExMxNetIfCPUCodeParamsSet
*
* DESCRIPTION:
*               The Core function for 'netSetCPUCodeParams'.
*               Function to define designated device and trapping parameters
*               for frames with specific cpu opcode or any opcodes
*               The core function is called directly is stack applications
*               to configure for each device what is the device number the
*               CPU is connected to
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*       devNum  - device number
*       cpuCode - the CPU code mapped to designated CPU. An ALL_CPU_CODES
*                         wildcard will be used for devices that does not support
*                         multi CPU distinction (SOHO)
*       desigDevNum - Designated device local to the CPU that will receive the
*                       packets with the specified CPU code
*       trapParams - associated trap parameters such as TC,DP, enable mode.
*
* OUTPUTS:
*       None
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on other error.
*       GT_BAD_PARAM    - on bad parameters
*
* COMMENTS:
*       support only in tiger
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfCPUCodeParamsSet

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                         devNum;
    CPSS_NET_RX_CPU_CODE_ENT cpuCode;
    CPSS_EXMX_NET_TRAP_PARAMS_STC trapParams;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    cpuCode = (CPSS_NET_RX_CPU_CODE_ENT)inFields[0];
    trapParams.enable = (GT_BOOL)inFields[1];
    trapParams.tc = (GT_U8)inFields[2];
    trapParams.dp = (CPSS_DP_LEVEL_ENT)inFields[3];

    /* call cpss api function */
    result = cpssExMxNetIfCPUCodeParamsSet(devNum, cpuCode, trapParams);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cpssExMxNetIfCPUCodeParamsGet
*
* DESCRIPTION:
*       Get the packet TC according to the CPU code.
*
* APPLICABLE DEVICES: ALL ExMx Devices
*
* INPUTS:
*       devNum         - device number.
*       cpuCode        - CPU code.
*
* OUTPUTS:
*       trapParams - (pointer to) Parameters struct according to the CPU code.
*
* RETURNS:
*       GT_OK            - on success
*       GT_FAIL          - on error
*       GT_BAD_PARAM     - TC is not supported for this CPU code.
*       GT_NOT_SUPPORTED - PP does not support TC for CPU codes
*
* COMMENTS:
*       This function is only for debug.
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfCPUCodeParamsGetFirst

(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    CPSS_NET_RX_CPU_CODE_ENT        cpuCode;
    CPSS_EXMX_NET_TRAP_PARAMS_STC   trapParamsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    cpuCodeIdx = 0;
    cpuCode = cpuCodeArr[cpuCodeIdx];

    /* call cpss api function */
    result = cpssExMxNetIfCPUCodeParamsGet(devNum, cpuCode, &trapParamsPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = trapParamsPtr.enable;
    inFields[1] = trapParamsPtr.tc;
    inFields[2] = trapParamsPtr.dp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", cpuCode, inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}


CMD_STATUS wrCpssExMxNetIfCPUCodeParamsGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    GT_U8                           devNum;
    CPSS_NET_RX_CPU_CODE_ENT        cpuCode;
    CPSS_EXMX_NET_TRAP_PARAMS_STC   trapParamsPtr;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    cpuCodeIdx++;

    if (cpuCodeIdx > (sizeof(cpuCodeArr)/sizeof(cpuCode)))
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    cpuCode = cpuCodeArr[cpuCodeIdx];

    /* call cpss api function */
    result = cpssExMxNetIfCPUCodeParamsGet(devNum, cpuCode, &trapParamsPtr);

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "");
        return CMD_OK;
    }

    inFields[0] = trapParamsPtr.enable;
    inFields[1] = trapParamsPtr.tc;
    inFields[2] = trapParamsPtr.dp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", cpuCode, inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%f");
    return CMD_OK;
}

/*============================
          TX
=============================*/


/*******************************************************************************
* cmdCpssTxStart (general command)
*
* DESCRIPTION:
*     Starts transmition of packets
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfTxStart
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssTxStart();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cmdCpssTxStop (general command)
*
* DESCRIPTION:
*     Stop transmition of packets.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfTxStop
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssTxStop();

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cmdCpssTxSetMode (general command)
*
* DESCRIPTION:
*     This command will set the the transmit parameters.
*
* INPUTS:
*     [0] GT_U32 lCyclesNum -
*          The maximum number of loop cycles (-1 = forever)
*
*     [1] GT_U32 lPcktsNum -
*          The maximum number packets all together (-1 = forever)
*
*     [2] GT_U32 lGap -
*          The time to wit between two cycles
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfTxSetMode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lCyclesNum;
    GT_U32 lGap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    lCyclesNum = (GT_U32)inArgs[0];
    lGap = (GT_U32)inArgs[1];

    /* call tapi api function */
    result = cmdCpssTxSetMode(lCyclesNum, lGap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");
    return CMD_OK;
}

/*******************************************************************************
* cmdCpssTxGetMode (general command)
*
* DESCRIPTION:
*     This command will get the the transmit parameters.
*
* INPUTS:
*     none
*
* OUTPUTS:
*     [0] GT_U32 lCyclesNum -
*          The maximum number of loop cycles (-1 = forever)
*
*     [2] GT_U32 lGap -
*          The time to wit between two cycles.
*
*
* RETURNS:
*     CMD_OK          - on success.
*     CMD_AGENT_ERROR - on failure.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfTxGetMode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U32 lCyclesNum;
    GT_U32 lGap;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call tapi api function */
    result = cmdCpssTxGetMode(&lCyclesNum, &lGap);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d", lCyclesNum,lGap);
    return CMD_OK;
}


/*
* Table: TxNetIf
*
* Description:
*     Transmited packet descriptor table.
*
* Comments:
*/
/*******************************************************************************
* netSetTxNetIfTblEntry (table command)
*
* DESCRIPTION:
*     Creates new transmit parameters description entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] TX_CMD_ENT lCmdType -
*          Defines type of transmition (VIDX, PORT_TRUNK).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfSetTxNetIfTblEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_32   fieldSize[] = {19, 24}; /*Number of fields in each instance*/
    GT_STATUS status=GT_OK;
    PKT_DESC_STC * netTxPacketDesc;
    GT_U32   entryIndex=0;
    GT_U32   numOfBuffs=0;

    static GT_BOOL  poolCreated = GT_FALSE;

    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    if(numFields < fieldSize[inArgs[1]])
        return CMD_FIELD_UNDERFLOW;
    if(numFields > fieldSize[inArgs[1]])
        return CMD_FIELD_OVERFLOW;

    /* creat the pool only once */
    if(poolCreated == GT_FALSE)
    {
        /* create pool of buffers from Cache */
        status = cmdOsPoolCreateDmaPool(TX_BUFFER_SIZE + 64 /* ALIGN_ADDR_CHUNK */,
                                  GT_4_BYTE_ALIGNMENT,
                                  MAX_NUM_OF_BUFFERS_AT_POOL,
                                  GT_TRUE,
                                  &txBuffersPoolId);
        if (status != GT_OK)
        {
                cmdOsPrintf("ERROR-PoolCreateDmaPool\n\n");
                return status;
        }

        poolCreated = GT_TRUE;
    }

    netTxPacketDesc=cmdOsMalloc(sizeof(PKT_DESC_STC));
    if (netTxPacketDesc==NULL)
    {
        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }
    cmdOsMemSet(netTxPacketDesc, 0, sizeof(PKT_DESC_STC));

    switch(inArgs[1])
    {
        case TX_BY_VIDX:
            /*Non zero data len and aligmnent <= 8*/
            if( ((GT_U8)inFields[18] > 8) || ((GT_U32)inFields[17] == 0) )
            {
                galtisOutput(outArgs, GT_BAD_PARAM, "");
                return CMD_OK;
            }
            status = setBufferArraysOfEqualSize((GT_U8*)inFields[16],
                                       (GT_U32)inFields[17],
                                       TX_BUFFER_SIZE,
                                       netTxPacketDesc->pcktData,
                                       netTxPacketDesc->pcktDataLen,
                                       &numOfBuffs);


            break;

        case TX_BY_PORT_TRUNK:
            /*Non zero data len and aligmnent <= 8*/
            if( ((GT_U8)inFields[23] > 8) || ((GT_U32)inFields[22] == 0) )
            {
                galtisOutput(outArgs, GT_BAD_PARAM, "");
                return CMD_OK;
            }
            status = setBufferArraysOfEqualSize((GT_U8*)inFields[21],
                                       (GT_U32)inFields[22],
                                       TX_BUFFER_SIZE,
                                       netTxPacketDesc->pcktData,
                                       netTxPacketDesc->pcktDataLen,
                                       &numOfBuffs);
            break;
        default:
            status = GT_BAD_PARAM;
            break;
    }


    if (status != GT_OK)
    {
        cmdOsFree(netTxPacketDesc);
        freeBufferArraysOfEqualSize(netTxPacketDesc->pcktData,
                                    netTxPacketDesc->pcktDataLen);
        galtisOutput(outArgs, GT_NO_RESOURCE, "");
        return CMD_OK;
    }


    netTxPacketDesc->numOfBuffers = numOfBuffs;
    netTxPacketDesc->cmdType = (TX_CMD_ENT)inArgs[1];

    netTxPacketDesc->entryId = (GT_U32)inFields[0];
    netTxPacketDesc->txSyncMode = (GT_BOOL)inFields[1];
    netTxPacketDesc->userPrioTag = (GT_U8)inFields[2];
    netTxPacketDesc->tagged = (GT_BOOL)inFields[3];
    netTxPacketDesc->dropPrecedence = (CPSS_DP_LEVEL_ENT)inFields[4];
    netTxPacketDesc->txQueue = (GT_U8)inFields[5];
    netTxPacketDesc->encap = (CPSS_EXMX_NET_PCKT_ENCAP_ENT)inFields[6];
    netTxPacketDesc->appendCrc = (GT_BOOL)inFields[7];


    switch(inArgs[1])
    {
        case TX_BY_VIDX:

            netTxPacketDesc->macDaType = inFields[11];
            netTxPacketDesc->invokeTxBufferQueueEvent = inFields[12];
            netTxPacketDesc->pcktsNum = (GT_U32)inFields[13];
            netTxPacketDesc->gap = (GT_U32)inFields[14];
            netTxPacketDesc->waitTime = (GT_U32)inFields[15];
            netTxPacketDesc->numSentPackets = (GT_U32)inFields[18];

            netTxPacketDesc->cmdParams.byVlan.devNum = (GT_U8)inFields[8];
            netTxPacketDesc->cmdParams.byVlan.vid = (GT_U16)inFields[9];
            netTxPacketDesc->cmdParams.byVlan.vidx = (GT_U16)inFields[10];

            break;

        case TX_BY_PORT_TRUNK:

            netTxPacketDesc->macDaType = inFields[12];
            netTxPacketDesc->invokeTxBufferQueueEvent = inFields[17];
            netTxPacketDesc->pcktsNum = (GT_U32)inFields[18];
            netTxPacketDesc->gap = (GT_U32)inFields[19];
            netTxPacketDesc->waitTime = (GT_U32)inFields[20];
            netTxPacketDesc->numSentPackets = (GT_U32)inFields[23];

            netTxPacketDesc->cmdParams.byPort.vid = (GT_U16)inFields[8];

            if(inFields[9] == 0)
            {
                netTxPacketDesc->cmdParams.byPort.isTrunk = GT_FALSE;
            }
            else
            {
                netTxPacketDesc->cmdParams.byPort.isTrunk = GT_TRUE;
            }

            netTxPacketDesc->cmdParams.byPort.devNum = (GT_U8)inFields[10];
            netTxPacketDesc->cmdParams.byPort.portTrunk = (GT_U8)inFields[11];

            netTxPacketDesc->cmdParams.byPort.addDsa = (GT_BOOL)inFields[13];
            netTxPacketDesc->cmdParams.byPort.dsaInfo.physicalMuxDev = (GT_U8)inFields[14];
            netTxPacketDesc->cmdParams.byPort.dsaInfo.physicalMuxPort = (GT_U8)inFields[15];
            netTxPacketDesc->cmdParams.byPort.dsaInfo.tcOnMuxDevice = (GT_U8)inFields[16];

            break;

        default:
            break;
    }
    /* Insert into table */
    status = cmdCpssTxSetPacketDesc(netTxPacketDesc->entryId,
                                netTxPacketDesc,
                                GT_FALSE,   /* Non extended */
                                &entryIndex);

    /* No need to flush packet buffers because the allocation was made from the cache */
    /*
    if(status == GT_OK)
    {
        for (k=0; k < MAX_NUM_OF_SDMA_BUFFERS_PER_CHAIN; k++)
        {
            if( cpssTxPacketDescTbl[entryIndex].pcktDataLen[k] == 0 )
                break;

            extDrvMgmtCacheFlush(GT_MGMT_DATA_CACHE_E,
                                 cpssTxPacketDescTbl[entryIndex].pcktData[k],
                                 cpssTxPacketDescTbl[entryIndex].pcktDataLen[k]);
        }
    }*/

    cmdOsFree(netTxPacketDesc);
    galtisOutput(outArgs, status, "");

    return CMD_OK;
}

/*******************************************************************************
* netGetTxNetIfTbl (table command)
*
* DESCRIPTION:
*     Retruns specific packet descriptor entry from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] TX_CMD_ENT lCmdType -
*          Defines type of transmition (VLAN, If, LPort).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfGetTxNetIfTbl
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY lGtBuff;

    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    {
        GT_STATUS status;
        PKT_DESC_STC SpecificPcktDesc;

        /* init with zeros */
        cmdOsMemSet(&SpecificPcktDesc, 0, sizeof(SpecificPcktDesc));

        status = cmdCpssTxGetPacketDesc((GT_U32*)&inArgs[0], &SpecificPcktDesc);
        if (status != GT_OK)
        {
            /* indicates that table is empty or (GT_U32)inArgs[0] is invalid */
            if( (status == GT_EMPTY) ||
                (status == GT_NO_SUCH) ||
                (status == GT_NOT_FOUND) )
                galtisOutput(outArgs, GT_OK, "%d", -1);
            else
                galtisOutput(outArgs, status, "");
            return CMD_OK;
        }

        /* Mapping structure to fields */

        inFields[0] = SpecificPcktDesc.entryId;
        inFields[1] = SpecificPcktDesc.txSyncMode;
        inFields[2] = SpecificPcktDesc.userPrioTag;
        inFields[3] = SpecificPcktDesc.tagged;
        inFields[4] = SpecificPcktDesc.dropPrecedence;
        inFields[5] = SpecificPcktDesc.txQueue;
        inFields[6] = SpecificPcktDesc.encap;
        inFields[7] = SpecificPcktDesc.appendCrc;

        /* Nathan - need to change this code */
        lGtBuff.data = cmdOsMalloc(SpecificPcktDesc.pcktDataLen[0]);
        lGtBuff.length = SpecificPcktDesc.pcktDataLen[0];
        if (!lGtBuff.data)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }

        cmdOsMemCpy(lGtBuff.data, SpecificPcktDesc.pcktData[0],
            SpecificPcktDesc.pcktDataLen[0]);

        /* End --- need to be changed */

        inArgs[1] = SpecificPcktDesc.cmdType;

        switch(SpecificPcktDesc.cmdType)
        {
            case TX_BY_VIDX:

                inFields[8] = SpecificPcktDesc.cmdParams.byVlan.devNum;
                inFields[9] = SpecificPcktDesc.cmdParams.byVlan.vid;
                inFields[10] = SpecificPcktDesc.cmdParams.byVlan.vidx;
                inFields[11] = SpecificPcktDesc.macDaType;
                inFields[12] = SpecificPcktDesc.invokeTxBufferQueueEvent;
                inFields[13] = SpecificPcktDesc.pcktsNum;
                inFields[14] = SpecificPcktDesc.gap;
                inFields[15] = SpecificPcktDesc.waitTime;
                inFields[17] = SpecificPcktDesc.pcktDataLen[0];
                inFields[18] = SpecificPcktDesc.numSentPackets;


                /* pack and output table fields */
                fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%s%d%d",
                    inFields[0], inFields[1], inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6], inFields[7], inFields[8],
                    inFields[9], inFields[10], inFields[11], inFields[12],
                    inFields[13],inFields[14], inFields[15], galtisBArrayOut(&lGtBuff),
                    inFields[17],inFields[18]);
                break;

            case TX_BY_PORT_TRUNK:

                inFields[8] = SpecificPcktDesc.cmdParams.byPort.vid;
                inFields[9] = SpecificPcktDesc.cmdParams.byPort.isTrunk;
                inFields[10] = SpecificPcktDesc.cmdParams.byPort.devNum;
                inFields[11] = SpecificPcktDesc.cmdParams.byPort.portTrunk;
                inFields[12] = SpecificPcktDesc.macDaType;
                inFields[13] = SpecificPcktDesc.cmdParams.byPort.addDsa;
                inFields[14] = SpecificPcktDesc.cmdParams.byPort.dsaInfo.physicalMuxDev;
                inFields[15] = SpecificPcktDesc.cmdParams.byPort.dsaInfo.physicalMuxPort;
                inFields[16] = SpecificPcktDesc.cmdParams.byPort.dsaInfo.tcOnMuxDevice;
                inFields[17] = SpecificPcktDesc.invokeTxBufferQueueEvent;
                inFields[18] = SpecificPcktDesc.pcktsNum;
                inFields[19] = SpecificPcktDesc.gap;
                inFields[20] = SpecificPcktDesc.waitTime;
                inFields[22] = SpecificPcktDesc.pcktDataLen[0];
                inFields[23] = SpecificPcktDesc.numSentPackets;


                fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%s%d%d",
                    inFields[0],inFields[1], inFields[2], inFields[3], inFields[4],
                    inFields[5], inFields[6], inFields[7], inFields[8],inFields[9],
                    inFields[10], inFields[11],inFields[12],inFields[13],
                    inFields[14],inFields[15],inFields[16], inFields[17],
                    inFields[18],inFields[19],inFields[20],
                    galtisBArrayOut(&lGtBuff),inFields[22],inFields[23]);
                break;

            default:
                break;
        }

        galtisOutput(outArgs, status, "%d%f", SpecificPcktDesc.cmdType);

        cmdOsFree(lGtBuff.data);
        cmdOsFree(SpecificPcktDesc.pcktData[0]);
    }
    return CMD_OK;
}

/*******************************************************************************
* netGetTxNetIfTblFirstEntry (table command)
*
* DESCRIPTION:
*     Get first entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] TX_CMD_ENT lCmdType -
*          Defines type of transmition (VLAN, If, LPort).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfGetTxNetIfTblFirstEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY lGtBuff;

    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    {
        GT_STATUS status;
        PKT_DESC_STC SpecificPcktDesc;

        /* init with zeros */
        cmdOsMemSet(&SpecificPcktDesc, 0, sizeof(SpecificPcktDesc));

        netTxCurrIndex = 0;

        status = cmdCpssTxBeginGetPacketDesc();
        if (status != GT_OK)
        {
            galtisOutput(outArgs, status, "");
            return CMD_OK;
        }

        status = cmdCpssTxGetPacketDesc(&netTxCurrIndex, &SpecificPcktDesc);
        if (status != GT_OK)
        {
            /* indicates that table is empty or (GT_U32)inArgs[0] is invalid */
            if( (status == GT_EMPTY) ||
                (status == GT_NO_SUCH) ||
                (status == GT_NOT_FOUND) )
                galtisOutput(outArgs, GT_OK, "%d", -1);
            else
                galtisOutput(outArgs, status, "");

            cmdCpssTxEndGetPacketDesc();
            return CMD_OK;
        }

        /* Mapping structure to fields */
        inFields[0] = SpecificPcktDesc.entryId;
        inFields[1] = SpecificPcktDesc.txSyncMode;
        inFields[2] = SpecificPcktDesc.userPrioTag;
        inFields[3] = SpecificPcktDesc.tagged;
        inFields[4] = SpecificPcktDesc.dropPrecedence;
        inFields[5] = SpecificPcktDesc.txQueue;
        inFields[6] = SpecificPcktDesc.encap;
        inFields[7] = SpecificPcktDesc.appendCrc;

        /* Nathan - need to change this code */
        lGtBuff.data = cmdOsMalloc(SpecificPcktDesc.pcktDataLen[0]);
        lGtBuff.length = SpecificPcktDesc.pcktDataLen[0];
        if (!lGtBuff.data)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }

        cmdOsMemCpy(lGtBuff.data, SpecificPcktDesc.pcktData[0],
            SpecificPcktDesc.pcktDataLen[0]);

        /* End --- need to be changed */

        inArgs[1] = SpecificPcktDesc.cmdType;

        switch(SpecificPcktDesc.cmdType)
        {

            case TX_BY_VIDX:

                inFields[8] = SpecificPcktDesc.cmdParams.byVlan.devNum;
                inFields[9] = SpecificPcktDesc.cmdParams.byVlan.vid;
                inFields[10] = SpecificPcktDesc.cmdParams.byVlan.vidx;
                inFields[11] = SpecificPcktDesc.macDaType;
                inFields[12] = SpecificPcktDesc.invokeTxBufferQueueEvent;
                inFields[13] = SpecificPcktDesc.pcktsNum;
                inFields[14] = SpecificPcktDesc.gap;
                inFields[15] = SpecificPcktDesc.waitTime;
                inFields[17] = SpecificPcktDesc.pcktDataLen[0];
                inFields[18] = SpecificPcktDesc.numSentPackets;


                /* pack and output table fields */
                fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%s%d%d",
                    inFields[0], inFields[1], inFields[2], inFields[3],
                    inFields[4], inFields[5], inFields[6], inFields[7], inFields[8],
                    inFields[9], inFields[10], inFields[11], inFields[12],
                    inFields[13],inFields[14], inFields[15], galtisBArrayOut(&lGtBuff),
                    inFields[17],inFields[18]);
                break;

            case TX_BY_PORT_TRUNK:

                inFields[8] = SpecificPcktDesc.cmdParams.byPort.vid;
                inFields[9] = SpecificPcktDesc.cmdParams.byPort.isTrunk;
                inFields[10] = SpecificPcktDesc.cmdParams.byPort.devNum;
                inFields[11] = SpecificPcktDesc.cmdParams.byPort.portTrunk;
                inFields[12] = SpecificPcktDesc.macDaType;
                inFields[13] = SpecificPcktDesc.cmdParams.byPort.addDsa;
                inFields[14] = SpecificPcktDesc.cmdParams.byPort.dsaInfo.physicalMuxDev;
                inFields[15] = SpecificPcktDesc.cmdParams.byPort.dsaInfo.physicalMuxPort;
                inFields[16] = SpecificPcktDesc.cmdParams.byPort.dsaInfo.tcOnMuxDevice;
                inFields[17] = SpecificPcktDesc.invokeTxBufferQueueEvent;
                inFields[18] = SpecificPcktDesc.pcktsNum;
                inFields[19] = SpecificPcktDesc.gap;
                inFields[20] = SpecificPcktDesc.waitTime;
                inFields[22] = SpecificPcktDesc.pcktDataLen[0];
                inFields[23] = SpecificPcktDesc.numSentPackets;


                fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%s%d%d",
                    inFields[0],inFields[1], inFields[2], inFields[3], inFields[4],
                    inFields[5], inFields[6], inFields[7], inFields[8],inFields[9],
                    inFields[10], inFields[11],inFields[12],inFields[13],
                    inFields[14],inFields[15],inFields[16], inFields[17],
                    inFields[18],inFields[19],inFields[20],
                    galtisBArrayOut(&lGtBuff),inFields[22],inFields[23]);
                break;

            default:
                break;
        }

        galtisOutput(outArgs, status, "%d%f", SpecificPcktDesc.cmdType);

        cmdOsFree(lGtBuff.data);
        cmdOsFree(SpecificPcktDesc.pcktData[0]);
    }
    return CMD_OK;
}

/*******************************************************************************
* netGetTxNetIfTblNextEntry (table command)
*
* DESCRIPTION:
*     Get next entry in the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] TX_CMD_ENT lCmdType -
*          Defines type of transmition (VLAN, If, LPort).
*
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfGetTxNetIfTblNextEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_BYTE_ARRY lGtBuff;

    /* check for valid arguments */
    if(inArgs == NULL || outArgs == NULL)
        return CMD_AGENT_ERROR;

    {
        GT_STATUS status;
        PKT_DESC_STC SpecificPcktDesc;

        /* init with zeros */
        cmdOsMemSet(&SpecificPcktDesc, 0, sizeof(SpecificPcktDesc));

        netTxCurrIndex++;

        status = cmdCpssTxGetPacketDesc(&netTxCurrIndex, &SpecificPcktDesc);
        if (status != GT_OK)
        {
            /* indicates that table is empty or (GT_U32)inArgs[0] is invalid */
            if( (status == GT_EMPTY) ||
                (status == GT_NO_SUCH) ||
                (status == GT_NOT_FOUND) )
                galtisOutput(outArgs, GT_OK, "%d", -1);
            else
                galtisOutput(outArgs, status, "");

            cmdCpssTxEndGetPacketDesc();
            return CMD_OK;
        }

        /* Mapping structure to fields */

        inFields[0] = SpecificPcktDesc.entryId;
        inFields[1] = SpecificPcktDesc.txSyncMode;
        inFields[2] = SpecificPcktDesc.userPrioTag;
        inFields[3] = SpecificPcktDesc.tagged;
        inFields[4] = SpecificPcktDesc.dropPrecedence;
        inFields[5] = SpecificPcktDesc.txQueue;
        inFields[6] = SpecificPcktDesc.encap;
        inFields[7] = SpecificPcktDesc.appendCrc;

        /* Nathan - need to change this code */
        lGtBuff.data = cmdOsMalloc(SpecificPcktDesc.pcktDataLen[0]);
        lGtBuff.length = SpecificPcktDesc.pcktDataLen[0];
        if (!lGtBuff.data)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }

        cmdOsMemCpy(lGtBuff.data, SpecificPcktDesc.pcktData[0],
            SpecificPcktDesc.pcktDataLen[0]);

        /* End --- need to be changed */

        inArgs[1] = SpecificPcktDesc.cmdType;

         switch(SpecificPcktDesc.cmdType)
        {
            case TX_BY_VIDX:

                inFields[8] = SpecificPcktDesc.cmdParams.byVlan.devNum;
                inFields[9] = SpecificPcktDesc.cmdParams.byVlan.vid;
                inFields[10] = SpecificPcktDesc.cmdParams.byVlan.vidx;
                inFields[11] = SpecificPcktDesc.macDaType;
                inFields[12] = SpecificPcktDesc.invokeTxBufferQueueEvent;
                inFields[13] = SpecificPcktDesc.pcktsNum;
                inFields[14] = SpecificPcktDesc.gap;
                inFields[15] = SpecificPcktDesc.waitTime;
                inFields[17] = SpecificPcktDesc.pcktDataLen[0];
                inFields[18] = SpecificPcktDesc.numSentPackets;


                /* pack and output table fields */
                fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%s%d%d",
                   inFields[0], inFields[1], inFields[2], inFields[3],
                   inFields[4], inFields[5], inFields[6], inFields[7], inFields[8],
                   inFields[9], inFields[10], inFields[11], inFields[12],
                   inFields[13],inFields[14], inFields[15], galtisBArrayOut(&lGtBuff),
                   inFields[17],inFields[18]);
                break;

            case TX_BY_PORT_TRUNK:

                inFields[8] = SpecificPcktDesc.cmdParams.byPort.vid;
                inFields[9] = SpecificPcktDesc.cmdParams.byPort.isTrunk;
                inFields[10] = SpecificPcktDesc.cmdParams.byPort.devNum;
                inFields[11] = SpecificPcktDesc.cmdParams.byPort.portTrunk;
                inFields[12] = SpecificPcktDesc.macDaType;
                inFields[13] = SpecificPcktDesc.cmdParams.byPort.addDsa;
                inFields[14] = SpecificPcktDesc.cmdParams.byPort.dsaInfo.physicalMuxDev;
                inFields[15] = SpecificPcktDesc.cmdParams.byPort.dsaInfo.physicalMuxPort;
                inFields[16] = SpecificPcktDesc.cmdParams.byPort.dsaInfo.tcOnMuxDevice;
                inFields[17] = SpecificPcktDesc.invokeTxBufferQueueEvent;
                inFields[18] = SpecificPcktDesc.pcktsNum;
                inFields[19] = SpecificPcktDesc.gap;
                inFields[20] = SpecificPcktDesc.waitTime;
                inFields[22] = SpecificPcktDesc.pcktDataLen[0];
                inFields[23] = SpecificPcktDesc.numSentPackets;


                fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%s%d%d",
                    inFields[0],inFields[1], inFields[2], inFields[3], inFields[4],
                    inFields[5], inFields[6], inFields[7], inFields[8],inFields[9],
                    inFields[10], inFields[11],inFields[12],inFields[13],
                    inFields[14],inFields[15],inFields[16], inFields[17],
                    inFields[18],inFields[19],inFields[20],
                    galtisBArrayOut(&lGtBuff),inFields[22],inFields[23]);
                break;

            default:
                break;
        }

        galtisOutput(outArgs, GT_OK, "%d%f", SpecificPcktDesc.cmdType);

        cmdOsFree(lGtBuff.data);
        cmdOsFree(SpecificPcktDesc.pcktData[0]);
    }
    return CMD_OK;
}

/*******************************************************************************
* netDeleteTxNetIfTblEntry (table command)
*
* DESCRIPTION:
*     Delete packet descriptor entry from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] TX_CMD_ENT lCmdType -
*          Defines type of transmition (VLAN, If, LPort).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfDeleteTxNetIfTblEntry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_32   fieldSize[] = {19, 24}; /*Number of fields in each instance*/
    GT_BYTE_ARRY lGtBuff;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* init with zeros */
    cmdOsMemSet(&lGtBuff, 0, sizeof(lGtBuff));

    if(numFields < fieldSize[inArgs[1]])
        return CMD_FIELD_UNDERFLOW;
    if(numFields > fieldSize[inArgs[1]])
        return CMD_FIELD_OVERFLOW;

    switch (inArgs[1]) {
    case 0x0000:
        galtisBArray(&lGtBuff,(GT_U8*)inFields[16]);
        break;
    case 0x0002:
        galtisBArray(&lGtBuff,(GT_U8*)inFields[21]);
        break;
    }
    {
        GT_STATUS status;
        status = cmdCpssTxDelPacketDesc((GT_U32)inFields[0]);
        galtisOutput(outArgs, status, "");
    }

    galtisBArrayFree(&lGtBuff);
    return CMD_OK;
}

/*******************************************************************************
* netClearTxNetIfTable (table command)
*
* DESCRIPTION:
*     Clear all packet descriptors entries from the table.
*
* INPUTS:
*     [0] GT_U32 lIndex -
*          Entry ID of packet descriptor.
*
*     [1] TX_CMD_ENT lCmdType -
*          Defines type of transmition (VLAN, If, LPort).
*
*
* OUTPUTS:
*     none
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfClearTxNetIfTable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    {
        GT_STATUS status;
        status = cmdCpssTxClearPacketDesc();
        galtisOutput(outArgs, status, "");
    }
    return CMD_OK;
}

/************************ GALTIS INTERNAL SERVICE ROUTINES *********************/
#include <galtisAgent/wrapUtil/cmdCpssNetReceive.h>
static GT_U32 netRxExtIndex;

/*******************************************************************************
* wrCpssExMxNetIfRxPacketTableGet (table command)
*
* DESCRIPTION:
*     Get first/next entry from the EXMX rxNetIf table. (always "TO_CPU" dsa tag !)
*
* INPUTS:
*     none
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*     CMD_OK            - on success.
*     CMD_AGENT_ERROR   - on failure.
*     CMD_FIELD_UNDERFLOW - not enough field arguments.
*     CMD_FIELD_OVERFLOW  - too many field arguments.
*     getNext            - GT_TRUE - "get next"
*                          GT_FALSE - "get first"
*
* COMMENTS:
*     none
*
*******************************************************************************/
CMD_STATUS wrCpssExMxNetIfRxPacketTableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER],
    GT_BOOL   getNext
)
{
    GT_STATUS result;
    GT_8   packetBuff[GALTIS_RX_BUFFER_MAX_SIZE_CNS];
    GT_U32 buffLen = GALTIS_RX_BUFFER_MAX_SIZE_CNS;
    GT_U32 packetActualLength;
    GT_U8  devNum;
    GT_U8  queue;
    CPSS_EXMX_NET_RX_PARAMS_STC rxParam[2];

    GT_BYTE_ARRY byteArry;
    GT_U8*      packetString;

    GT_BOOL         memberOfTrunk;
    GT_TRUNK_ID     trunkId;
    GT_U8  __Dev,__Port; /* Dummy for converting. */

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if(getNext == GT_FALSE)
    {
        result = cmdCpssRxPkGetFirst(&netRxExtIndex,
                                 packetBuff, &buffLen, &packetActualLength,
                                 &devNum,&queue,
                                 rxParam);
    }
    else
    {
        result = cmdCpssRxPkGetNext(&netRxExtIndex,
                                 packetBuff, &buffLen, &packetActualLength,
                                 &devNum,&queue,
                                 rxParam);
    }

    if(result == GT_NO_MORE)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "%d", -1);

        return CMD_OK;
    }

    if(result != GT_OK)
    {
        /* pack output arguments to galtis string */
        galtisOutput(outArgs, result, "");

        return CMD_OK;
    }

    cpssExMxTrunkPortTrunkIdGet(devNum,rxParam[0].srcPortNum,&memberOfTrunk,&trunkId);

    byteArry.data    = packetBuff;
    byteArry.length  = buffLen;

    inFields[0] = devNum;
    inFields[1] = queue;
    packetString = galtisBArrayOut(&byteArry);
    inFields[3]  = packetActualLength;
    inFields[4] = 0xFF;
    inFields[5] = rxParam[0].vlanId;
    inFields[6] = 1;
    if (memberOfTrunk == GT_TRUE)
    {
        inFields[7] = 1;          /*srcIsTrunk*/
        inFields[8] = trunkId; /*trunk Id*/
    }
    else
    {
        inFields[7] = 0;                     /*srcIsPort*/
        inFields[8] = rxParam[0].srcPortNum; /*port Id*/

        __Dev  = (GT_U8)inFields[0];
        __Port = (GT_U8)inFields[8];
        CONVERT_BACK_DEV_PORT_DATA_MAC(__Dev, __Port) ;
        inFields[0] = __Dev  ;
        inFields[8] = __Port ;
    }
    inFields[9] = rxParam[0].cpuCode;

    /* pack and output table fields */
    /*            0 1 2 3 4 5 6 7 8 9 */
    /*                                0 1 2 3 4 5*/
    fieldOutput("%d%d%s%d%d%d%d%d%d%d",
                inFields[0],
                inFields[1],
                packetString,
                inFields[3],
                inFields[4],
                inFields[5],
                inFields[6],
                inFields[7],
                inFields[8],
                inFields[9]
    );

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
    {"cpssExMxNetIfRxQueueEnable",
        &wrCpssExMxNetIfRxQueueEnable,
        3, 0},

    {"cpssExMxNetIfTxQueueEnable",
        &wrCpssExMxNetIfTxQueueEnable,
        3, 0},

    {"cpssExMxNetIfRxCountersGetFirst",
        &wrCpssExMxNetIfRxCountersGetFirst,
        1, 0},

    {"cpssExMxNetIfRxCountersGetNext",
        &wrCpssExMxNetIfRxCountersGetNext,
        1, 0},

    {"cpssExMxNetIfRxErrorCountGetFirst",
        &wrCpssExMxNetIfRxErrorCountGetFirst,
        1, 0},

    {"cpssExMxNetIfRxErrorCountGetNext",
        &wrCpssExMxNetIfRxErrorCountGetNext,
        1, 0},

    {"cpssExMxNetIfCPUCodeParamsSet",
        &wrCpssExMxNetIfCPUCodeParamsSet,
        1, 4},

    {"cpssExMxNetIfCPUCodeParamsGetFirst",
        &wrCpssExMxNetIfCPUCodeParamsGetFirst,
        1, 0},

    {"cpssExMxNetIfCPUCodeParamsGetNext",
        &wrCpssExMxNetIfCPUCodeParamsGetNext,
        1, 0},

    /*netSetTxNetIfTblEntry, */
    {"cpssTransmitNetworkIfSet",
        &wrCpssExMxNetIfSetTxNetIfTblEntry,
        2, 24},

    {"netGetTxNetIfTbl",
        &wrCpssExMxNetIfGetTxNetIfTbl,
        2, 0},

    /*netGetTxNetIfTblFirstEntry ,cpssTransmitNetworkIfGetFirst*/
    {"cpssTransmitNetworkIfGetFirst",
        &wrCpssExMxNetIfGetTxNetIfTblFirstEntry,
        2, 0},

    /*netGetTxNetIfTblNextEntry ,cpssTransmitNetworkIfGetFirst*/
    {"cpssTransmitNetworkIfGetNext",
        &wrCpssExMxNetIfGetTxNetIfTblNextEntry,
        2, 0},

    /*netDeleteTxNetIfTblEntry, cpssTransmitNetworkIfDelete*/
    {"cpssTransmitNetworkIfDelete",
        &wrCpssExMxNetIfDeleteTxNetIfTblEntry,
        2, 24},

    /*netClearTxNetIfTable, cpssTransmitNetworkIfClear*/
    {"cpssTransmitNetworkIfClear",
        &wrCpssExMxNetIfClearTxNetIfTable,
        2, 0},
    /*cmdCpssTxSetMode , cpssExMxTxSetMode */
    {"cpssExMxTxSetMode",
        &wrCpssExMxNetIfTxSetMode,
        2, 0},

    /*cmdCpssTxGetMode , cpssExMxTxGetMode */
    {"cpssExMxTxGetMode",
        &wrCpssExMxNetIfTxGetMode,
        0, 0},

    /*cmdCpssTxStart , cpssExMxTxStart */
    {"cpssExMxTxStart",
     &wrCpssExMxNetIfTxStart,
     0, 0},

    /*cmdCpssTxStop , cpssExMxTxStop */
    {"cpssExMxTxStop",
     &wrCpssExMxNetIfTxStop,
     0, 0}



};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxNetworkIf

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
GT_STATUS cmdLibInitCpssExMxNetworkIf
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

