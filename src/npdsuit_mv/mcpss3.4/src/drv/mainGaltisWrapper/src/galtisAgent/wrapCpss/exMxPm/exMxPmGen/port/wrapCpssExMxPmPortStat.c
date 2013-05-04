/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssExMxPmPortStat.c
*
* DESCRIPTION:
*       Wrapper functions for
*           cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortStat.h API's
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
#include <cpss/exMxPm/exMxPmGen/port/cpssExMxPmPortStat.h>

/*******************************************************************************
* cpssExMxPmPortStatInit
*
* DESCRIPTION:
*       Init port statistics counter set ExMxPm CPSS facility.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error
*
* COMMENTS:
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortStatInit
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];

    /* call cpss api function */
    result = cpssExMxPmPortStatInit(devNum);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortSpecificMacCounterGet
*
* DESCRIPTION:
*       Gets specific Ethernet MAC counter for a specified port on specified
*       device.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*       cntrName - specific counter name
*
* OUTPUTS:
*       cntrValuePtr - (pointer to) current counter value.
*
* RETURNS:
*       GT_OK            - on success
*       GT_BAD_PARAM     - on invalid input paramteres value
*       GT_NOT_SUPPORTED - not supported counter on the port
*       GT_BAD_PTR       - one of the parameters is NULL pointer
*       GT_HW_ERROR      - on hardware error
*       GT_FAIL          - on error
*
* COMMENTS:
*     The 10G MAC MIB counters are 64-bit wide.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortSpecificMacCounterGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    CPSS_PORT_MAC_COUNTERS_ENT cntrName;
    GT_U64 cntrValue;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    /*TODO: add code here. */
    devNum = (GT_U8)inArgs[0];
    portNum=(GT_U8)inArgs[1];
    cntrName=(CPSS_PORT_MAC_COUNTERS_ENT)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortSpecificMacCounterGet(devNum, portNum, cntrName, &cntrValue);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d%d",cntrValue.l[0],cntrValue.l[1]);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortAllMacCountersGet
*
* DESCRIPTION:
*       Gets all Ethernet MAC counters for a particular Port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on invalid input paramteres value
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*       GT_FAIL       - on error
*
* COMMENTS:
*
*******************************************************************************/
static GT_U8 devNum;
static GT_U8 portNum;

static GT_STATUS portAllMacCountersGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_PORT_MAC_COUNTER_SET_STC portMacCounterSetArray;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call cpss api function */
    result = cpssExMxPmPortAllMacCountersGet(devNum, portNum, &portMacCounterSetArray);
    if (result != GT_OK)
    {
        return result;
    }

    /* the value of GOOD_PKTS is sum of three counters: Unicast+Broadcast+Multicast */
    portMacCounterSetArray.goodPktsRcv.l[0] =
        portMacCounterSetArray.ucPktsRcv.l[0] +
        portMacCounterSetArray.brdcPktsRcv.l[0] +
        portMacCounterSetArray.mcPktsRcv.l[0];

    portMacCounterSetArray.goodPktsSent.l[0] =
        portMacCounterSetArray.ucPktsSent.l[0] +
        portMacCounterSetArray.brdcPktsSent.l[0] +
        portMacCounterSetArray.mcPktsSent.l[0];

    inFields[0] = devNum;
    inFields[1] = portNum;
    inFields[2] = portMacCounterSetArray.goodOctetsRcv.l[0];
    inFields[3] = portMacCounterSetArray.goodOctetsRcv.l[1];
    inFields[4] = portMacCounterSetArray.badOctetsRcv.l[0];
    inFields[5] = portMacCounterSetArray.badOctetsRcv.l[1];
    inFields[6] = portMacCounterSetArray.macTransmitErr.l[0];
    inFields[7] = portMacCounterSetArray.macTransmitErr.l[1];
    inFields[8] = portMacCounterSetArray.goodPktsRcv.l[0];
    inFields[9] = portMacCounterSetArray.goodPktsRcv.l[1];
    inFields[10] = portMacCounterSetArray.badPktsRcv.l[0];
    inFields[11] = portMacCounterSetArray.badPktsRcv.l[1];
    inFields[12] = portMacCounterSetArray.brdcPktsRcv.l[0];
    inFields[13] = portMacCounterSetArray.brdcPktsRcv.l[1];
    inFields[14] = portMacCounterSetArray.mcPktsRcv.l[0];
    inFields[15] = portMacCounterSetArray.mcPktsRcv.l[1];
    inFields[16] = portMacCounterSetArray.pkts64Octets.l[0];
    inFields[17] = portMacCounterSetArray.pkts64Octets.l[1];
    inFields[18] = portMacCounterSetArray.pkts65to127Octets.l[0];
    inFields[19] = portMacCounterSetArray.pkts65to127Octets.l[1];
    inFields[20] = portMacCounterSetArray.pkts128to255Octets.l[0];
    inFields[21] = portMacCounterSetArray.pkts128to255Octets.l[1];
    inFields[22] = portMacCounterSetArray.pkts256to511Octets.l[0];
    inFields[23] = portMacCounterSetArray.pkts256to511Octets.l[1];
    inFields[24] = portMacCounterSetArray.pkts512to1023Octets.l[0];
    inFields[25] = portMacCounterSetArray.pkts512to1023Octets.l[1];
    inFields[26] = portMacCounterSetArray.pkts1024tomaxOoctets.l[0];
    inFields[27] = portMacCounterSetArray.pkts1024tomaxOoctets.l[1];
    inFields[28] = portMacCounterSetArray.goodOctetsSent.l[0];
    inFields[29] = portMacCounterSetArray.goodOctetsSent.l[1];
    inFields[30] = portMacCounterSetArray.goodPktsSent.l[0];
    inFields[31] = portMacCounterSetArray.goodPktsSent.l[1];
    inFields[32] = portMacCounterSetArray.excessiveCollisions.l[0];
    inFields[33] = portMacCounterSetArray.excessiveCollisions.l[1];
    inFields[34] = portMacCounterSetArray.mcPktsSent.l[0];
    inFields[35] = portMacCounterSetArray.mcPktsSent.l[1];
    inFields[36] = portMacCounterSetArray.brdcPktsSent.l[0];
    inFields[37] = portMacCounterSetArray.brdcPktsSent.l[1];
    inFields[38] = portMacCounterSetArray.unrecogMacCntrRcv.l[0];
    inFields[39] = portMacCounterSetArray.unrecogMacCntrRcv.l[1];
    inFields[40] = portMacCounterSetArray.fcSent.l[0];
    inFields[41] = portMacCounterSetArray.fcSent.l[1];
    inFields[42] = portMacCounterSetArray.goodFcRcv.l[0];
    inFields[43] = portMacCounterSetArray.goodFcRcv.l[1];
    inFields[44] = portMacCounterSetArray.dropEvents.l[0];
    inFields[45] = portMacCounterSetArray.dropEvents.l[1];
    inFields[46] = portMacCounterSetArray.undersizePkts.l[0];
    inFields[47] = portMacCounterSetArray.undersizePkts.l[1];
    inFields[48] = portMacCounterSetArray.fragmentsPkts.l[0];
    inFields[49] = portMacCounterSetArray.fragmentsPkts.l[1];
    inFields[50] = portMacCounterSetArray.oversizePkts.l[0];
    inFields[51] = portMacCounterSetArray.oversizePkts.l[1];
    inFields[52] = portMacCounterSetArray.jabberPkts.l[0];
    inFields[53] = portMacCounterSetArray.jabberPkts.l[1];
    inFields[54] = portMacCounterSetArray.macRcvError.l[0];
    inFields[55] = portMacCounterSetArray.macRcvError.l[1];
    inFields[56] = portMacCounterSetArray.badCrc.l[0];
    inFields[57] = portMacCounterSetArray.badCrc.l[1];
    inFields[58] = portMacCounterSetArray.collisions.l[0];
    inFields[59] = portMacCounterSetArray.collisions.l[1];
    inFields[60] = portMacCounterSetArray.lateCollisions.l[0];
    inFields[61] = portMacCounterSetArray.lateCollisions.l[1];
    inFields[62] = portMacCounterSetArray.badFcRcv.l[0];
    inFields[63] = portMacCounterSetArray.badFcRcv.l[1];


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%\
                d%d%d%d%d%d%d%d%d%d%d",       inFields[0],  inFields[1],
                inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],  inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21],
                inFields[22], inFields[23], inFields[24], inFields[25],
                inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32], inFields[33],
                inFields[34], inFields[35], inFields[36], inFields[37],
                inFields[38], inFields[39], inFields[40], inFields[41],
                inFields[42], inFields[43], inFields[44], inFields[45],
                inFields[46], inFields[47], inFields[48], inFields[49],
                inFields[50], inFields[51], inFields[52], inFields[53],
                inFields[54], inFields[55], inFields[56], inFields[57],
                inFields[58], inFields[59], inFields[60], inFields[61],
                inFields[62], inFields[63]);


    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortAllMacCountersGet
*
* DESCRIPTION:
*       Gets all Ethernet MAC counters for a particular Port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*                  CPU port counters are valid only when using "Ethernet CPU
*                  port" (not using the SDMA interface).
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on invalid input paramteres value
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*       GT_FAIL       - on error
*
* COMMENTS:
*
*******************************************************************************/

static GT_STATUS portAllMacCountersGetExt
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_PORT_MAC_COUNTER_SET_STC portMacCounterSetArray;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call cpss api function */
    result = cpssExMxPmPortAllMacCountersGet(devNum, portNum, &portMacCounterSetArray);
    if (result != GT_OK)
    {
        return result;
    }

    inFields[0] = devNum;
    inFields[1] = portNum;
    inFields[2] = portMacCounterSetArray.goodOctetsRcv.l[0];
    inFields[3] = portMacCounterSetArray.goodOctetsRcv.l[1];
    inFields[4] = portMacCounterSetArray.badOctetsRcv.l[0];
    inFields[5] = portMacCounterSetArray.badOctetsRcv.l[1];
    inFields[6] = portMacCounterSetArray.macTransmitErr.l[0];
    inFields[7] = portMacCounterSetArray.macTransmitErr.l[1];
    inFields[8] = portMacCounterSetArray.badPktsRcv.l[0];
    inFields[9] = portMacCounterSetArray.badPktsRcv.l[1];
    inFields[10] = portMacCounterSetArray.brdcPktsRcv.l[0];
    inFields[11] = portMacCounterSetArray.brdcPktsRcv.l[1];
    inFields[12] = portMacCounterSetArray.mcPktsRcv.l[0];
    inFields[13] = portMacCounterSetArray.mcPktsRcv.l[1];
    inFields[14] = portMacCounterSetArray.pkts64Octets.l[0];
    inFields[15] = portMacCounterSetArray.pkts64Octets.l[1];
    inFields[16] = portMacCounterSetArray.pkts65to127Octets.l[0];
    inFields[17] = portMacCounterSetArray.pkts65to127Octets.l[1];
    inFields[18] = portMacCounterSetArray.pkts128to255Octets.l[0];
    inFields[10] = portMacCounterSetArray.pkts128to255Octets.l[1];
    inFields[20] = portMacCounterSetArray.pkts256to511Octets.l[0];
    inFields[21] = portMacCounterSetArray.pkts256to511Octets.l[1];
    inFields[22] = portMacCounterSetArray.pkts512to1023Octets.l[0];
    inFields[23] = portMacCounterSetArray.pkts512to1023Octets.l[1];
    inFields[24] = portMacCounterSetArray.pkts1024tomaxOoctets.l[0];
    inFields[25] = portMacCounterSetArray.pkts1024tomaxOoctets.l[1];
    inFields[26] = portMacCounterSetArray.goodOctetsSent.l[0];
    inFields[27] = portMacCounterSetArray.goodOctetsSent.l[1];
    inFields[28] = portMacCounterSetArray.excessiveCollisions.l[0];
    inFields[29] = portMacCounterSetArray.excessiveCollisions.l[1];
    inFields[30] = portMacCounterSetArray.mcPktsSent.l[0];
    inFields[31] = portMacCounterSetArray.mcPktsSent.l[1];
    inFields[32] = portMacCounterSetArray.brdcPktsSent.l[0];
    inFields[33] = portMacCounterSetArray.brdcPktsSent.l[1];
    inFields[34] = portMacCounterSetArray.unrecogMacCntrRcv.l[0];
    inFields[35] = portMacCounterSetArray.unrecogMacCntrRcv.l[1];
    inFields[36] = portMacCounterSetArray.fcSent.l[0];
    inFields[37] = portMacCounterSetArray.fcSent.l[1];
    inFields[38] = portMacCounterSetArray.goodFcRcv.l[0];
    inFields[39] = portMacCounterSetArray.goodFcRcv.l[1];
    inFields[40] = portMacCounterSetArray.dropEvents.l[0];
    inFields[41] = portMacCounterSetArray.dropEvents.l[1];
    inFields[42] = portMacCounterSetArray.undersizePkts.l[0];
    inFields[43] = portMacCounterSetArray.undersizePkts.l[1];
    inFields[44] = portMacCounterSetArray.fragmentsPkts.l[0];
    inFields[45] = portMacCounterSetArray.fragmentsPkts.l[1];
    inFields[46] = portMacCounterSetArray.oversizePkts.l[0];
    inFields[47] = portMacCounterSetArray.oversizePkts.l[1];
    inFields[48] = portMacCounterSetArray.jabberPkts.l[0];
    inFields[49] = portMacCounterSetArray.jabberPkts.l[1];
    inFields[50] = portMacCounterSetArray.macRcvError.l[0];
    inFields[51] = portMacCounterSetArray.macRcvError.l[1];
    inFields[52] = portMacCounterSetArray.badCrc.l[0];
    inFields[53] = portMacCounterSetArray.badCrc.l[1];
    inFields[54] = portMacCounterSetArray.collisions.l[0];
    inFields[55] = portMacCounterSetArray.collisions.l[1];
    inFields[56] = portMacCounterSetArray.lateCollisions.l[0];
    inFields[57] = portMacCounterSetArray.lateCollisions.l[1];
    inFields[58] = portMacCounterSetArray.ucPktsRcv.l[0];
    inFields[50] = portMacCounterSetArray.ucPktsRcv.l[1];
    inFields[60] = portMacCounterSetArray.ucPktsSent.l[0];
    inFields[61] = portMacCounterSetArray.ucPktsSent.l[1];
    inFields[62] = portMacCounterSetArray.multiplePktsSent.l[0];
    inFields[63] = portMacCounterSetArray.multiplePktsSent.l[1];
    inFields[64] = portMacCounterSetArray.deferredPktsSent.l[0];
    inFields[65] = portMacCounterSetArray.deferredPktsSent.l[1];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%\
                d%d%d%d%d%d%d%d%d%d%dd%d",
                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                inFields[8],  inFields[9],  inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32], inFields[33], inFields[34], inFields[35],
                inFields[36], inFields[37], inFields[38], inFields[39],
                inFields[40], inFields[41], inFields[42], inFields[43],
                inFields[44], inFields[45], inFields[46], inFields[47],
                inFields[48], inFields[49], inFields[50], inFields[51],
                inFields[52], inFields[53], inFields[54], inFields[55],
                inFields[56], inFields[57], inFields[58], inFields[59],
                inFields[60], inFields[61], inFields[62], inFields[63],
                inFields[64], inFields[65]);

    galtisOutput(outArgs, result, "%f");

    return CMD_OK;
}

/******************************************************************************/

static CMD_STATUS wrCpssExMxPmPortAllMacCountersGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    devNum = 0;
    portNum = 0;

    while(devNum < 128)
    {
        result = portAllMacCountersGet(inArgs,inFields,numFields,outArgs);
        if(result == GT_BAD_PARAM || result == GT_NOT_SUPPORTED)
        {
            portNum++;
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
        }
        else break;
    }
    if (devNum == 128)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}

/******************************************************************************/

static CMD_STATUS wrCpssExMxPmPortAllMacCountersGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    while(devNum < 128)
    {
        result = portAllMacCountersGet(inArgs,inFields,numFields,outArgs);
        if(result == GT_BAD_PARAM || result == GT_NOT_SUPPORTED)
        {
            portNum++;
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
        }
        else break;
    }
    if (devNum == 128)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}

/******************************************************************************/

static CMD_STATUS wrCpssExMxPmPortAllMacCountersGetFirstExt
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    devNum = 0;
    portNum = 0;

    while(devNum < 128)
    {
        result = portAllMacCountersGetExt(inArgs,inFields,numFields,outArgs);
        if(result == GT_BAD_PARAM || result == GT_NOT_SUPPORTED)
        {
            portNum++;
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
        }
        else break;
    }
    if (devNum == 128)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}

/******************************************************************************/

static CMD_STATUS wrCpssExMxPmPortAllMacCountersGetNextExt
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;

    while(devNum < 128)
    {
        result = portAllMacCountersGetExt(inArgs,inFields,numFields,outArgs);
        if(result == GT_BAD_PARAM || result == GT_NOT_SUPPORTED)
        {
            portNum++;
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
        }
        else break;
    }
    if (devNum == 128)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}


/*******************************************************************************
* cpssExMxPmPortMacCountersEnableSet
*
* DESCRIPTION:
*       Enable or disable MAC Counters update for this port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev      - device number.
*       portNum  - physical port number (or CPU port)
*       enable   -  enable update of MAC counters
*                   GT_FALSE = MAC counters update for this port is disabled.
*                   GT_TRUE =  MAC counters update for this port is enabled.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacCountersEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacCountersEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacCountersEnableGet
*
* DESCRIPTION:
*       Get enable/disable MAC Counters update status for this port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev       - device number.
*       portNum   - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr - pointer to  MAC Counters update status:
*                   GT_FALSE = MAC counters update for this port is disabled.
*                   GT_TRUE = MAC counters update for this port is enabled.
*
* RETURNS:
*       GT_OK           - on success.
*       GT_BAD_PARAM    - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR     - on hardware error
*       GT_FAIL         - on error.
*
* COMMENTS:
*       None;
*
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacCountersEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacCountersEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacCountersClearOnReadEnableSet
*
* DESCRIPTION:
*       Enable or disable MAC Counters Clear on read per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev         - device number.
*       portNum     - physical port number (or CPU port)
*       enable      - enable clear on read for MAC counters
*                      GT_FALSE - Counters are not cleared.
*                      GT_TRUE  - Counters are cleared.

*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error.
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacCountersClearOnReadEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacCountersClearOnReadEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacCountersClearOnReadEnableGet
*
* DESCRIPTION:
*       Get enable/disable MAC Counters Clear on read status per group of
*       ports for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*       - Ports 0 through 5
*       - Ports 6 through 11
*       - Ports 12 through 17
*       - Ports 18 through 23
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev         - device number.
*       portNum     - physical port number (or CPU port)
*
* OUTPUTS:
*       enablePtr   - pointer to MAC Counters Clear on read status:
*                      GT_FALSE - Counters are not cleared.
*                      GT_TRUE - Counters are cleared.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error.
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacCountersClearOnReadEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacCountersClearOnReadEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacCountersRxHistogramEnableSet
*
* DESCRIPTION:
*       Enable/disable updating of the Rx histogram
*       counters for received packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev         - device number.
*       portNum     - physical port number
*       enable      - enable updating of the counters for received packets
*                     GT_FALSE - Counters are not updated.
*                     GT_TRUE - Counters are  updated.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error.
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacCountersRxHistogramEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacCountersRxHistogramEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacCountersRxHistogramEnableGet
*
* DESCRIPTION:
*       Get enable/disable updating status of the Rx histogram
*       counters for received packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev         - device number.
*       portNum     - physical port number
*
*
* OUTPUTS:
*       enablePtr   - pointer to updating status of the Rx
*                     histogram counters:
*                     GT_FALSE - Counters are not updated.
*                     GT_TRUE  - Counters are updated.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error.
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacCountersRxHistogramEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacCountersRxHistogramEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacCountersTxHistogramEnableSet
*
* DESCRIPTION:
*       Enable/disable updating of the Tx histogram
*       counters for transmitted packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev         - device number.
*       portNum     - physical port number
*       enable      - enable updating of the counters for transmitted packets
*                      GT_FALSE - Counters are not updated.
*                      GT_TRUE  - Counters are updated.
*
*
* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error.
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacCountersTxHistogramEnableSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    enable = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacCountersTxHistogramEnableSet(devNum, portNum, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacCountersTxHistogramEnableGet
*
* DESCRIPTION:
*       Get enable/disable updating status of the Tx histogram
*       counters for received packets per group of ports
*       for Tri-Speed Ports or per port for HyperG.Stack ports.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev         - device number.
*       portNum     - physical port number
*
*
* OUTPUTS:
*       enablePtr   - pointer to updating status of the Tx
*                     histogram counters:
*                     GT_FALSE - Counters are not updated.
*                     GT_TRUE  - Counters are updated.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error.
*
* COMMENTS:
*       None
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacCountersTxHistogramEnableGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacCountersTxHistogramEnableGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacCountersCaptureTriggerSet
*
* DESCRIPTION:
*       Set capturing for phisical port MAC MIB counters.
*       When this mechanism is triggered, the device reads all of a port’s MIB
*       counters into a capture area in a single  atomic action.
*       Once this read is done, the host CPU may read the counters from
*       the capture area.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev               - device number.
*       portNum           - physical port number
*       captureTrigger    - trigger capturing for phisical port's counters.
*                            GT_TRUE  - to trigger capturing for port.
*
                           GT_FALSE - otherwise

*

* OUTPUTS:
*       None.
*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error.
*
* COMMENTS:
*       <CaptureTrigger> is a self-clearing bit. It is set back to 0 once the
*       capture action is done. The host CPU must poll this bit until it is LOW.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacCountersCaptureTriggerSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL captureTrigger;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];
    captureTrigger = (GT_BOOL)inArgs[2];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacCountersCaptureTriggerSet(devNum, portNum, captureTrigger);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortMacCountersCaptureTriggerGet
*
* DESCRIPTION:
*       Get capturing status for phisical port MAC MIB counters.
*       When this mechanism is triggered, the device reads all of a port’s MIB
*       counters into a capture area in a single  atomic action.
*       Once this read is done, the host CPU may read the counters from
*       the capture area.
*       For Tri-Speed Ports groups are:
*           - Ports 0 through 5
*           - Ports 6 through 11
*           - Ports 12 through 17
*           - Ports 18 through 23
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       dev         - device number.
*       portNum     - physical port number
*
*
* OUTPUTS:
*       enablePtr   - pointer to enable/disable capturing status
*                     GT_TRUE  - Capturing is enabled.
*                     GT_FALSE - otherwise

*
* RETURNS:
*       GT_OK        - on success.
*       GT_BAD_PARAM - on invalid input paramteres value
*       GT_BAD_PTR   - one of the parameters is NULL pointer
*       GT_HW_ERROR  - on hardware error
*       GT_FAIL      - on error.
*
* COMMENTS:
*       <CaptureTrigger> is a self-clearing bit. It is set back to 0 once the
*       capture action is done. The host CPU must poll this bit until it is LOW.
*******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortMacCountersCaptureTriggerGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    GT_U8 devNum;
    GT_U8 portNum;
    GT_BOOL enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    portNum = (GT_U8)inArgs[1];

    /* Override Device and Port */
    CONVERT_DEV_PORT_MAC(devNum, portNum);

    /* call cpss api function */
    result = cpssExMxPmPortMacCountersCaptureTriggerGet(devNum, portNum, &enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "%d", enable);

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortCapturedMacCountersGet
*
* DESCRIPTION:
*       Gets captured counters for a particular Port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on invalid input paramteres value
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*       GT_FAIL       - on error
*
* COMMENTS:
*
*******************************************************************************/

static  GT_U8 devNum;
static  GT_U8 portNum;

static GT_STATUS portCapturedMacCountersGet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_PORT_MAC_COUNTER_SET_STC portMacCounterSetArray;


     /* check for valid arguments */
    if(!inArgs || !outArgs)
        return GT_INIT_ERROR;


    /* call cpss api function */
    result = cpssExMxPmPortCapturedMacCountersGet(devNum, portNum, &portMacCounterSetArray);
    if (result != GT_OK)
    {
        return result;
    }

    /* the value of GOOD_PKTS is sum of three counters: Unicast+Broadcast+Multicast */
    portMacCounterSetArray.goodPktsRcv.l[0] =
        portMacCounterSetArray.ucPktsRcv.l[0] +
        portMacCounterSetArray.brdcPktsRcv.l[0] +
        portMacCounterSetArray.mcPktsRcv.l[0];

    portMacCounterSetArray.goodPktsSent.l[0] =
        portMacCounterSetArray.ucPktsSent.l[0] +
        portMacCounterSetArray.brdcPktsSent.l[0] +
        portMacCounterSetArray.mcPktsSent.l[0];


    inFields[0] = devNum;
    inFields[1] = portNum;
    inFields[2] = portMacCounterSetArray.goodOctetsRcv.l[0];
    inFields[3] = portMacCounterSetArray.goodOctetsRcv.l[1];
    inFields[4] = portMacCounterSetArray.badOctetsRcv.l[0];
    inFields[5] = portMacCounterSetArray.badOctetsRcv.l[1];
    inFields[6] = portMacCounterSetArray.macTransmitErr.l[0];
    inFields[7] = portMacCounterSetArray.macTransmitErr.l[1];
    inFields[8] = portMacCounterSetArray.goodPktsRcv.l[0];
    inFields[9] = portMacCounterSetArray.goodPktsRcv.l[1];
    inFields[10] = portMacCounterSetArray.badPktsRcv.l[0];
    inFields[11] = portMacCounterSetArray.badPktsRcv.l[1];
    inFields[12] = portMacCounterSetArray.brdcPktsRcv.l[0];
    inFields[13] = portMacCounterSetArray.brdcPktsRcv.l[1];
    inFields[14] = portMacCounterSetArray.mcPktsRcv.l[0];
    inFields[15] = portMacCounterSetArray.mcPktsRcv.l[1];
    inFields[16] = portMacCounterSetArray.pkts64Octets.l[0];
    inFields[17] = portMacCounterSetArray.pkts64Octets.l[1];
    inFields[18] = portMacCounterSetArray.pkts65to127Octets.l[0];
    inFields[19] = portMacCounterSetArray.pkts65to127Octets.l[1];
    inFields[20] = portMacCounterSetArray.pkts128to255Octets.l[0];
    inFields[21] = portMacCounterSetArray.pkts128to255Octets.l[1];
    inFields[22] = portMacCounterSetArray.pkts256to511Octets.l[0];
    inFields[23] = portMacCounterSetArray.pkts256to511Octets.l[1];
    inFields[24] = portMacCounterSetArray.pkts512to1023Octets.l[0];
    inFields[25] = portMacCounterSetArray.pkts512to1023Octets.l[1];
    inFields[26] = portMacCounterSetArray.pkts1024tomaxOoctets.l[0];
    inFields[27] = portMacCounterSetArray.pkts1024tomaxOoctets.l[1];
    inFields[28] = portMacCounterSetArray.goodOctetsSent.l[0];
    inFields[29] = portMacCounterSetArray.goodOctetsSent.l[1];
    inFields[30] = portMacCounterSetArray.goodPktsSent.l[0];
    inFields[31] = portMacCounterSetArray.goodPktsSent.l[1];
    inFields[32] = portMacCounterSetArray.excessiveCollisions.l[0];
    inFields[33] = portMacCounterSetArray.excessiveCollisions.l[1];
    inFields[34] = portMacCounterSetArray.mcPktsSent.l[0];
    inFields[35] = portMacCounterSetArray.mcPktsSent.l[1];
    inFields[36] = portMacCounterSetArray.brdcPktsSent.l[0];
    inFields[37] = portMacCounterSetArray.brdcPktsSent.l[1];
    inFields[38] = portMacCounterSetArray.unrecogMacCntrRcv.l[0];
    inFields[39] = portMacCounterSetArray.unrecogMacCntrRcv.l[1];
    inFields[40] = portMacCounterSetArray.fcSent.l[0];
    inFields[41] = portMacCounterSetArray.fcSent.l[1];
    inFields[42] = portMacCounterSetArray.goodFcRcv.l[0];
    inFields[43] = portMacCounterSetArray.goodFcRcv.l[1];
    inFields[44] = portMacCounterSetArray.dropEvents.l[0];
    inFields[45] = portMacCounterSetArray.dropEvents.l[1];
    inFields[46] = portMacCounterSetArray.undersizePkts.l[0];
    inFields[47] = portMacCounterSetArray.undersizePkts.l[1];
    inFields[48] = portMacCounterSetArray.fragmentsPkts.l[0];
    inFields[49] = portMacCounterSetArray.fragmentsPkts.l[1];
    inFields[50] = portMacCounterSetArray.oversizePkts.l[0];
    inFields[51] = portMacCounterSetArray.oversizePkts.l[1];
    inFields[52] = portMacCounterSetArray.jabberPkts.l[0];
    inFields[53] = portMacCounterSetArray.jabberPkts.l[1];
    inFields[54] = portMacCounterSetArray.macRcvError.l[0];
    inFields[55] = portMacCounterSetArray.macRcvError.l[1];
    inFields[56] = portMacCounterSetArray.badCrc.l[0];
    inFields[57] = portMacCounterSetArray.badCrc.l[1];
    inFields[58] = portMacCounterSetArray.collisions.l[0];
    inFields[59] = portMacCounterSetArray.collisions.l[1];
    inFields[60] = portMacCounterSetArray.lateCollisions.l[0];
    inFields[61] = portMacCounterSetArray.lateCollisions.l[1];
    inFields[62] = portMacCounterSetArray.badFcRcv.l[0];
    inFields[63] =portMacCounterSetArray.badFcRcv.l[1];


    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%\
                d%d%d%d%d%d%d%d%d%d%d",       inFields[0],  inFields[1],
                inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],  inFields[9],
                inFields[10], inFields[11], inFields[12], inFields[13],
                inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21],
                inFields[22], inFields[23], inFields[24], inFields[25],
                inFields[26], inFields[27], inFields[28], inFields[29],
                inFields[30], inFields[31], inFields[32], inFields[33],
                inFields[34], inFields[35], inFields[36], inFields[37],
                inFields[38], inFields[39], inFields[40], inFields[41],
                inFields[42], inFields[43], inFields[44], inFields[45],
                inFields[46], inFields[47], inFields[48], inFields[49],
                inFields[50], inFields[51], inFields[52], inFields[53],
                inFields[54], inFields[55], inFields[56], inFields[57],
                inFields[58], inFields[59], inFields[60], inFields[61],
                inFields[62], inFields[63]);



    galtisOutput(outArgs, result, "%f");

    return GT_OK;
}

/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortCapturedMacCountersGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    devNum = 0;
    portNum = 0;

    while(devNum < 128)
    {
        result = portCapturedMacCountersGet(inArgs,inFields,numFields,outArgs);
        if(result == GT_BAD_PARAM || result == GT_NOT_SUPPORTED)
        {
            portNum++;
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
        }
        else break;
    }
    if (devNum == 128)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortCapturedMacCountersGetNext
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    while(devNum < 128)
    {
        result = portCapturedMacCountersGet(inArgs,inFields,numFields,outArgs);
        if(result == GT_BAD_PARAM || result == GT_NOT_SUPPORTED)
        {
            portNum++;
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
        }
        else break;
    }
    if (devNum == 128)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;

    return CMD_OK;
}

/*******************************************************************************
* cpssExMxPmPortCapturedMacCountersGet
*
* DESCRIPTION:
*       Gets captured counters for a particular Port.
*
* APPLICABLE DEVICES:  EXMXPM devices
*
* INPUTS:
*       devNum   - device number
*       portNum  - physical port number
*
*
* OUTPUTS:
*       portMacCounterSetArray - array of current counter values.
*
* RETURNS:
*       GT_OK         - on success
*       GT_BAD_PARAM  - on invalid input paramteres value
*       GT_BAD_PTR    - one of the parameters is NULL pointer
*       GT_HW_ERROR   - on hardware error
*       GT_FAIL       - on error
*
* COMMENTS:
*
*******************************************************************************/

static  GT_U8 devNum;
static  GT_U8 portNum;

static GT_STATUS portCapturedMacCountersGetExt
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result;
    CPSS_PORT_MAC_COUNTER_SET_STC portMacCounterSetArray;


     /* check for valid arguments */
    if(!inArgs || !outArgs)
        return GT_INIT_ERROR;


    /* call cpss api function */
    result = cpssExMxPmPortCapturedMacCountersGet(devNum, portNum, &portMacCounterSetArray);
    if (result != GT_OK)
    {
        return result;
    }

    inFields[0] = devNum;
    inFields[1] = portNum;
    inFields[2] = portMacCounterSetArray.goodOctetsRcv.l[0];
    inFields[3] = portMacCounterSetArray.goodOctetsRcv.l[1];
    inFields[4] = portMacCounterSetArray.badOctetsRcv.l[0];
    inFields[5] = portMacCounterSetArray.badOctetsRcv.l[1];
    inFields[6] = portMacCounterSetArray.macTransmitErr.l[0];
    inFields[7] = portMacCounterSetArray.macTransmitErr.l[1];
    inFields[8] = portMacCounterSetArray.badPktsRcv.l[0];
    inFields[9] = portMacCounterSetArray.badPktsRcv.l[1];
    inFields[10] = portMacCounterSetArray.brdcPktsRcv.l[0];
    inFields[11] = portMacCounterSetArray.brdcPktsRcv.l[1];
    inFields[12] = portMacCounterSetArray.mcPktsRcv.l[0];
    inFields[13] = portMacCounterSetArray.mcPktsRcv.l[1];
    inFields[14] = portMacCounterSetArray.pkts64Octets.l[0];
    inFields[15] = portMacCounterSetArray.pkts64Octets.l[1];
    inFields[16] = portMacCounterSetArray.pkts65to127Octets.l[0];
    inFields[17] = portMacCounterSetArray.pkts65to127Octets.l[1];
    inFields[18] = portMacCounterSetArray.pkts128to255Octets.l[0];
    inFields[10] = portMacCounterSetArray.pkts128to255Octets.l[1];
    inFields[20] = portMacCounterSetArray.pkts256to511Octets.l[0];
    inFields[21] = portMacCounterSetArray.pkts256to511Octets.l[1];
    inFields[22] = portMacCounterSetArray.pkts512to1023Octets.l[0];
    inFields[23] = portMacCounterSetArray.pkts512to1023Octets.l[1];
    inFields[24] = portMacCounterSetArray.pkts1024tomaxOoctets.l[0];
    inFields[25] = portMacCounterSetArray.pkts1024tomaxOoctets.l[1];
    inFields[26] = portMacCounterSetArray.goodOctetsSent.l[0];
    inFields[27] = portMacCounterSetArray.goodOctetsSent.l[1];
    inFields[28] = portMacCounterSetArray.excessiveCollisions.l[0];
    inFields[29] = portMacCounterSetArray.excessiveCollisions.l[1];
    inFields[30] = portMacCounterSetArray.mcPktsSent.l[0];
    inFields[31] = portMacCounterSetArray.mcPktsSent.l[1];
    inFields[32] = portMacCounterSetArray.brdcPktsSent.l[0];
    inFields[33] = portMacCounterSetArray.brdcPktsSent.l[1];
    inFields[34] = portMacCounterSetArray.unrecogMacCntrRcv.l[0];
    inFields[35] = portMacCounterSetArray.unrecogMacCntrRcv.l[1];
    inFields[36] = portMacCounterSetArray.fcSent.l[0];
    inFields[37] = portMacCounterSetArray.fcSent.l[1];
    inFields[38] = portMacCounterSetArray.goodFcRcv.l[0];
    inFields[39] = portMacCounterSetArray.goodFcRcv.l[1];
    inFields[40] = portMacCounterSetArray.dropEvents.l[0];
    inFields[41] = portMacCounterSetArray.dropEvents.l[1];
    inFields[42] = portMacCounterSetArray.undersizePkts.l[0];
    inFields[43] = portMacCounterSetArray.undersizePkts.l[1];
    inFields[44] = portMacCounterSetArray.fragmentsPkts.l[0];
    inFields[45] = portMacCounterSetArray.fragmentsPkts.l[1];
    inFields[46] = portMacCounterSetArray.oversizePkts.l[0];
    inFields[47] = portMacCounterSetArray.oversizePkts.l[1];
    inFields[48] = portMacCounterSetArray.jabberPkts.l[0];
    inFields[49] = portMacCounterSetArray.jabberPkts.l[1];
    inFields[50] = portMacCounterSetArray.macRcvError.l[0];
    inFields[51] = portMacCounterSetArray.macRcvError.l[1];
    inFields[52] = portMacCounterSetArray.badCrc.l[0];
    inFields[53] = portMacCounterSetArray.badCrc.l[1];
    inFields[54] = portMacCounterSetArray.collisions.l[0];
    inFields[55] = portMacCounterSetArray.collisions.l[1];
    inFields[56] = portMacCounterSetArray.lateCollisions.l[0];
    inFields[57] = portMacCounterSetArray.lateCollisions.l[1];
    inFields[58] = portMacCounterSetArray.ucPktsRcv.l[0];
    inFields[50] = portMacCounterSetArray.ucPktsRcv.l[1];
    inFields[60] = portMacCounterSetArray.ucPktsSent.l[0];
    inFields[61] = portMacCounterSetArray.ucPktsSent.l[1];
    inFields[62] = portMacCounterSetArray.multiplePktsSent.l[0];
    inFields[63] = portMacCounterSetArray.multiplePktsSent.l[1];
    inFields[64] = portMacCounterSetArray.deferredPktsSent.l[0];
    inFields[65] = portMacCounterSetArray.deferredPktsSent.l[1];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d\
                %d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%\
                d%d%d%d%d%d%d%d%d%d%dd%d",
                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                inFields[8],  inFields[9],  inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15],
                inFields[16], inFields[17], inFields[18], inFields[19],
                inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25], inFields[26], inFields[27],
                inFields[28], inFields[29], inFields[30], inFields[31],
                inFields[32], inFields[33], inFields[34], inFields[35],
                inFields[36], inFields[37], inFields[38], inFields[39],
                inFields[40], inFields[41], inFields[42], inFields[43],
                inFields[44], inFields[45], inFields[46], inFields[47],
                inFields[48], inFields[49], inFields[50], inFields[51],
                inFields[52], inFields[53], inFields[54], inFields[55],
                inFields[56], inFields[57], inFields[58], inFields[59],
                inFields[60], inFields[61], inFields[62], inFields[63],
                inFields[64], inFields[65]);

    galtisOutput(outArgs, result, "%f");

    return GT_OK;
}


/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortCapturedMacCountersGetFirstExt
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    devNum = 0;
    portNum = 0;

    while(devNum < 128)
    {
        result = portCapturedMacCountersGetExt(inArgs,inFields,numFields,outArgs);
        if(result == GT_BAD_PARAM || result == GT_NOT_SUPPORTED)
        {
            portNum++;
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
        }
        else break;
    }
    if (devNum == 128)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;
    return CMD_OK;
}
/******************************************************************************/
static CMD_STATUS wrCpssExMxPmPortCapturedMacCountersGetNextExt
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_STATUS result = GT_OK;
    while(devNum < 128)
    {
        result = portCapturedMacCountersGetExt(inArgs,inFields,numFields,outArgs);
        if(result == GT_BAD_PARAM || result == GT_NOT_SUPPORTED)
        {
            portNum++;
            if(portNum == 64)
            {
                portNum = 0;
                devNum++;
            }
        }
        else break;
    }
    if (devNum == 128)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (result != GT_OK)
    {
        galtisOutput(outArgs, result, "%d", -1);
        return CMD_OK;
    }
    portNum++;

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"cpssExMxPmPortStatInit",
         &wrCpssExMxPmPortStatInit,
         1, 0},
        {"cpssExMxPmPortSpecificMacCounterGet",
         &wrCpssExMxPmPortSpecificMacCounterGet,
         3, 0},
        {"cpssExMxPmPortAllMacCountersGetFirst",
         &wrCpssExMxPmPortAllMacCountersGetFirst,
         0, 0},
        {"cpssExMxPmPortAllMacCountersGetNext",
         &wrCpssExMxPmPortAllMacCountersGetNext,
         0, 0},
        {"cpssExMxPmPortAllMacCounters_1GetFirst",
         &wrCpssExMxPmPortAllMacCountersGetFirst,
         0, 0},
        {"cpssExMxPmPortAllMacCounters_1GetNext",
         &wrCpssExMxPmPortAllMacCountersGetNext,
         0, 0},
        {"cpssExMxPmPortAllMacCountersGetExtFirst",
         &wrCpssExMxPmPortAllMacCountersGetFirstExt,
         0, 0},
        {"cpssExMxPmPortAllMacCountersGetExtNext",
         &wrCpssExMxPmPortAllMacCountersGetNextExt,
         0, 0},
        {"cpssExMxPmPortMacCountersEnableSet",
         &wrCpssExMxPmPortMacCountersEnableSet,
         3, 0},
        {"cpssExMxPmPortMacCountersEnableGet",
         &wrCpssExMxPmPortMacCountersEnableGet,
         2, 0},
        {"cpssExMxPmPortMacCountersClearOnReadEnableSet",
         &wrCpssExMxPmPortMacCountersClearOnReadEnableSet,
         3, 0},
        {"cpssExMxPmPortMacCountersClearOnReadEnableGet",
         &wrCpssExMxPmPortMacCountersClearOnReadEnableGet,
         2, 0},
        {"cpssExMxPmPortMacCountersRxHistogramEnableSet",
         &wrCpssExMxPmPortMacCountersRxHistogramEnableSet,
         3, 0},
        {"cpssExMxPmPortMacCountersRxHistogramEnableGet",
         &wrCpssExMxPmPortMacCountersRxHistogramEnableGet,
         2, 0},
        {"cpssExMxPmPortMacCountersTxHistogramEnableSet",
         &wrCpssExMxPmPortMacCountersTxHistogramEnableSet,
         3, 0},
        {"cpssExMxPmPortMacCountersTxHistogramEnableGet",
         &wrCpssExMxPmPortMacCountersTxHistogramEnableGet,
         2, 0},
        {"cpssExMxPmPortMacCountersCaptureTriggerSet",
         &wrCpssExMxPmPortMacCountersCaptureTriggerSet,
         3, 0},
        {"cpssExMxPmPortMacCountersCaptureTriggerGet",
         &wrCpssExMxPmPortMacCountersCaptureTriggerGet,
         2, 0},
        {"cpssExMxPmPortCapturedMacCountersGetFirst",
         &wrCpssExMxPmPortCapturedMacCountersGetFirst,
         0, 0},
        {"cpssExMxPmPortCapturedMacCountersGetNext",
         &wrCpssExMxPmPortCapturedMacCountersGetNext,
         0, 0},
         {"cpssExMxPmPortCapturedMacCountersGetExtFirst",
          &wrCpssExMxPmPortCapturedMacCountersGetFirstExt,
          0, 0},
         {"cpssExMxPmPortCapturedMacCountersGetExtNext",
          &wrCpssExMxPmPortCapturedMacCountersGetNextExt,
          0, 0}};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitCpssExMxPmPortStat
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
GT_STATUS cmdLibInitCpssExMxPmPortStat
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

