/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapCpssGenEvents.c
*
* DESCRIPTION:
*
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
#include <galtisAgent/wrapCpss/Gen/Events/wrapCpssGenEvents.h>
#include <cpss/generic/events/cpssGenEventUnifyTypes.h>

#define MAX_NUM_DEVICES 128

/* Event Counter DB for PP, FA and XBAR */
/* The size of counter DB per device is sum of PP, FA and XBAR unified  */
/* events, for convenience. */
static GT_U32   *uniEvCounterBlockPtr, uniEvIndex, endUniIndex;
static GT_U32   *uniEventCounters[MAX_NUM_DEVICES];

#define END_OF_TABLE_MAC    0x7fffffff
static struct{
    GT_32 oldIndex;/* integer on purpose */
    GT_32 newIndex;/* integer on purpose */
} convertGaltisIndexToUniEvIndexArr[] =
{
    {189 , CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E},/* CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E  */
    {228 , CPSS_PP_PCL_TCC_ECC_ERR_E},/*CPSS_PP_PCL_TCC_ECC_ERR_E */
    {235 , CPSS_PP_CNC_WRAPAROUND_BLOCK_E},/*CPSS_PP_CNC_WRAPAROUND_BLOCK_E */
    {333 , CPSS_XBAR_HG_LANE_SYNCH_CHANGE_E},/*CPSS_XBAR_HG_LANE_SYNCH_CHANGE_E*/

    {END_OF_TABLE_MAC,END_OF_TABLE_MAC}/* must be last */
};

/*******************************************************************************
* convertCpssUniEvIndexToGaltisIndex
*
* DESCRIPTION:
*    when the events added to galtis the event of:
*    CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E had value of 189
*
*    but due to changes on the events that relate to 'per port PCS (XPCS)' and 'per port - per lane'
*    the 13 existing events reduce to 12 events  , so the event of
*    CPSS_PP_BM_EGRESS_EMPTY_CLEAR_E has now value of 188 !!!
*
*    we need a function that will manage those 'WA' to support the Galtis.
*
* INPUTS:
*       cpssUniEvIndex - the cpss Unified index to convert
* OUTPUTS:
*       none
*
* RETURNS:
*       the modified Galtis index to use
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static GT_U32 convertCpssUniEvIndexToGaltisIndex
(
    IN GT_U32   cpssUniEvIndex
)
{
    GT_U32  ii;
    GT_32   offset = 0;/* integer on purpose */
    GT_32   origIndex = (GT_32)cpssUniEvIndex;/* integer on purpose */
    GT_32   finalIndex;/* integer on purpose */

    ii = 0;
    while(convertGaltisIndexToUniEvIndexArr[ii].oldIndex != END_OF_TABLE_MAC)
    {
        if(origIndex >= convertGaltisIndexToUniEvIndexArr[ii].newIndex)
        {
            offset += (convertGaltisIndexToUniEvIndexArr[ii].oldIndex -
                       convertGaltisIndexToUniEvIndexArr[ii].newIndex);
        }

        ii++;
    }
                             /* offset may be negative */
    finalIndex = origIndex + offset;

    if(finalIndex < 0)
    {
        /* error */
        finalIndex = 0;
    }

    if(offset != 0)
    {
        cmdOsPrintf("cpssUniEvIndex = 0x%x , %d\n",cpssUniEvIndex,cpssUniEvIndex);
    }

    return (GT_U32)finalIndex;
}

/*******************************************************************************
* wrCpssGenEventCountersCbGet (table command)
*
* DESCRIPTION:
* Routine to bind a CB function that receives block of
*  event counters for given device.
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
*
* COMMENTS:
*     Called from cpssEnabler.
*
*******************************************************************************/
static GT_STATUS wrCpssGenEventCounterIncrement
(
    IN  GT_U8   devNum,
    IN GT_U32   uniEvCounter
)
{
    /* Counting the event */
    if(uniEventCounters[devNum] == NULL)
    {
        uniEventCounters[devNum] =  (GT_U32 *)cmdOsMalloc((CPSS_FA_UNI_EV_MAX_E - CPSS_PP_UNI_EV_MIN_E) * 4);

        if(uniEventCounters[devNum] == NULL)
            return GT_OUT_OF_CPU_MEM;

        cmdOsMemSet(uniEventCounters[devNum], 0, ( (CPSS_FA_UNI_EV_MAX_E - CPSS_PP_UNI_EV_MIN_E) * 4));
    }

    uniEventCounters[devNum][uniEvCounter]++;

    return GT_OK;
}

GT_STATUS wrCpssGenEventCountersCbGet
(
        EVENT_COUNTER_INCREMENT_FUNC **eventCounterBlockGetCB
)
{
    *eventCounterBlockGetCB = wrCpssGenEventCounterIncrement;
    return GT_OK;
}


/*******************************************************************************
* wrCpssGenEventTableGetFirst (table command)
*
* DESCRIPTION:
*     get first event counter entry.
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
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenEventTableGetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;
    GT_U32      devType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    devType = inArgs[1];

    if(devType > 2)
        return CMD_AGENT_ERROR;


    if(uniEventCounters[devNum] == NULL)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    uniEvCounterBlockPtr = uniEventCounters[devNum];

    if(devType == 0) /* In case of PP,  the first and last index*/
    {
        uniEvIndex  = CPSS_PP_UNI_EV_MIN_E;
        endUniIndex = CPSS_PP_UNI_EV_MAX_E + 1; /* we should increment the
                            endUniIndex in order not to loose the last event */
    }
    else if(devType == 1) /* In case of XBAR,  the first and last index*/
    {
        uniEvIndex  = CPSS_XBAR_UNI_EV_MIN_E;
        endUniIndex = CPSS_XBAR_UNI_EV_MAX_E + 1; /* we should increment the
                            endUniIndex in order not to loose the last event */
    }
    else /* In case of FA,  the first and last index*/
    {
        uniEvIndex  = CPSS_FA_UNI_EV_MIN_E;
        endUniIndex = CPSS_FA_UNI_EV_MAX_E + 1; /* we should increment the
                            endUniIndex in order not to loose the last event */
    }

    while((uniEvIndex < endUniIndex) &&
          (uniEvCounterBlockPtr[uniEvIndex] == 0))
    {
        uniEvIndex++;
    }

    if(uniEvIndex < endUniIndex)
    {
        /* update the index sent to galtis with the WA of modifications in the cpssEvents enum
           Vs. the static Galtis enum */
        inFields[0] = convertCpssUniEvIndexToGaltisIndex(uniEvIndex);
        inFields[1] = uniEvCounterBlockPtr[uniEvIndex];
        /* pack and output table fields */
        fieldOutput("%d%d", inFields[0], inFields[1]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}


/*******************************************************************************
* wrCpssGenEventTableGetNext (table command)
*
* DESCRIPTION:
*     get next event counter entry.
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
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenEventTableGetNext
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

    uniEvIndex++;

    while((uniEvIndex < endUniIndex) &&
          (uniEvCounterBlockPtr[uniEvIndex] == 0))
    {
        uniEvIndex++;
    }

    if(uniEvIndex < endUniIndex)
    {
        /* update the index sent to galtis with the WA of modifications in the cpssEvents enum
           Vs. the static Galtis enum */
        inFields[0] = convertCpssUniEvIndexToGaltisIndex(uniEvIndex);
        inFields[1] = uniEvCounterBlockPtr[uniEvIndex];
        /* pack and output table fields */
        fieldOutput("%d%d", inFields[0], inFields[1]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}


/*******************************************************************************
* wrCpssExMxPmNetIfRxPacketTableClear (table command)
*
* DESCRIPTION:
*     clear all entries in event counter entry block.
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
*
* COMMENTS:
*     none
*
*******************************************************************************/
static CMD_STATUS wrCpssGenEventTableClear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    GT_U8       devNum;
    GT_U32      devType;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    devNum = (GT_U8)inArgs[0];
    devType = inArgs[1];

    if(devType > 2)
        return CMD_AGENT_ERROR;


    if(uniEventCounters[devNum] == NULL)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    uniEvCounterBlockPtr = uniEventCounters[devNum];

    if(devType == 0) /* In case of PP,  the first and last index*/
    {
        uniEvIndex  = CPSS_PP_UNI_EV_MIN_E;
        endUniIndex = CPSS_PP_UNI_EV_MAX_E;
    }
    else if(devType == 1) /* In case of XBAR,  the first and last index*/
    {
        uniEvIndex  = CPSS_XBAR_UNI_EV_MIN_E;
        endUniIndex = CPSS_XBAR_UNI_EV_MAX_E;
    }
    else /* In case of FA,  the first and last index*/
    {
        uniEvIndex  = CPSS_FA_UNI_EV_MIN_E;
        endUniIndex = CPSS_FA_UNI_EV_MAX_E;
    }

    while(uniEvIndex < endUniIndex)
    {
        uniEvCounterBlockPtr[uniEvIndex] = 0;
        uniEvIndex++;
    }


    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/**** database initialization **************************************/


static CMD_COMMAND dbCommands[] =
{
    {"cpssGenEventTableGetFirst",
        &wrCpssGenEventTableGetFirst,
        2, 0},

    {"cpssGenEventTableGetNext",
        &wrCpssGenEventTableGetNext,
        2, 0},

    {"cpssGenEventTableClear",
        &wrCpssGenEventTableClear,/* clear */
        2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitCpssGenEvents
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
GT_STATUS cmdLibInitCpssGenEvents
(
    GT_VOID
)
{
    /* Reset Event Counter DB */
    cmdOsMemSet(uniEventCounters, 0, sizeof(uniEventCounters));

    return cmdInitLibrary(dbCommands, numCommands);
}

