
/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapAppDemoFx950_api_interrupts.c
*
* DESCRIPTION:
*       Galtis wrapper for AppDemo FX950 and FAP20M interrupts.
*
*       DEPENDENCIES:
*       None.
*
* FILE REVISION NUMBER:
*       $Revision: 1.1.1.1 $
*
*******************************************************************************/

#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <galtisAgent/wrapUtil/wrapAppDemoFx950_api_interrupts.h> 
#include <FX950/include/fap20m_api_general.h>
#include <FX950/include/fx950_api_interrupts.h>
#include <FX950/include/fx950_api_framework.h>
#include <FX950/include/fx950_regs.h>

/* Number of Global events for FAP20M device */
#define WR_FAP20M_GLOBAL_EVENTS_NUM 2

/* Number of Global events for FX950 device wrapper */
#define WR_FX950_GLOBAL_EVENTS_NUM     (WR_FX950_PEX_INT_PEX_LINK_FAIL -            \
                                            WR_FAP20M_ACTIVE_LINK_CHANGED_EVENT)

/* Total number of Port events for FX950 device */
#define WR_FX950_PORT_EVENTS_NUM        (FX950_PORT_NOF_INT * FX950_NOF_HGS_LINKS)

/* Total number of Port Lane events for FX950 device */
#define WR_FX950_PORT_LANE_EVENTS_NUM        \
        (FX950_PORT_LANE_NOF_INT * FX950_NOF_HGS_LINKS * FX950_REGS_SERDES_LANES_NUM)

/* Total number of events for both FX950 and FAP20M devices */
#define WR_FX950_TOTAL_EVENTS_NUM (                             \
     WR_FAP20M_GLOBAL_EVENTS_NUM +                              \
     FAP20M_NUMBER_OF_LINKS + /* FAP20M FE link status */       \
     WR_FX950_GLOBAL_EVENTS_NUM +                               \
     WR_FX950_PORT_EVENTS_NUM +                                 \
     WR_FX950_PORT_LANE_EVENTS_NUM                              \
     )

static unsigned long    *fx950InterruptCounters[SAND_MAX_DEVICE]; 
static unsigned long    *fx950IntCounterBlockPtr;
static unsigned long    curInterruptIndex;
static unsigned long    endInterruptIndex;


/*******************************************************************************
* wrapFx950EventCounterIncrement (table command)
*
* DESCRIPTION:
* Routine to bind a CB function that recieves block of
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
GT_STATUS wrFx950EventCounterIncrement
(
    IN  unsigned int   device_id,
    IN  unsigned long   index
)
{
    /* Counting the event */
    if(fx950InterruptCounters[device_id] == NULL)
    {
        fx950InterruptCounters[device_id] =  (unsigned long *)cmdOsMalloc(WR_FX950_TOTAL_EVENTS_NUM * 4);

        if(fx950InterruptCounters[device_id] == NULL)
            return GT_OUT_OF_CPU_MEM;

        cmdOsMemSet(fx950InterruptCounters[device_id], 0, (WR_FX950_TOTAL_EVENTS_NUM * 4));
    }

    fx950InterruptCounters[device_id][index]++;

    return GT_OK;
}

GT_STATUS wrFx950EventCountersCbGet
(
        FX950_EVENT_COUNTER_INCREMENT_FUNC **fx950EventCounterBlockGetCB
)
{
    *fx950EventCounterBlockGetCB = wrFx950EventCounterIncrement;
    return GT_OK;
}


/*******************************************************************************
* wrFx950_interrupt_get_info (table command)
*
* DESCRIPTION:
*   Gets interrupt info for current interrupt index: name, type, port number and
*   lane number.
*
* INPUTS:
*     none
*
* OUTPUTS:
*      Table fields as describe in the table description are written into
*      outArgs parameter.
*
* RETURNS:
*
* COMMENTS:
*
*******************************************************************************/
void wrFx950_interrupt_get_info
(
    INOUT GT_32 inFields[CMD_MAX_FIELDS]
)
{
    unsigned long   index = curInterruptIndex;

    if (index < WR_FAP20M_GLOBAL_EVENTS_NUM) 
    {
        /* FAP20M Global event */
        inFields[0] = index;                                /* interrupt name */
        inFields[1] = WR_FAP20M_EVENT_TYPE_GLOBAL;          /* interrupt type */
        inFields[2] = 0;                                    /* HGL or FE port number */
        inFields[3] = 0;                                    /* Lane number */
        return;
    }

    index -= WR_FAP20M_GLOBAL_EVENTS_NUM;
    if (index < FAP20M_NUMBER_OF_LINKS) 
    {
        /* FAP20M FE link event */
        inFields[0] = WR_FAP20M_ACTIVE_LINK_CHANGED_EVENT;  /* interrupt name */
        inFields[1] = WR_FAP20M_EVENT_TYPE_FE_PORT;         /* interrupt type */
        inFields[2] = index;                                /* HGL or FE port number */
        inFields[3] = 0;                                    /* HGL Port Lane number */
        return;
    }

    index -= FAP20M_NUMBER_OF_LINKS;
    if (curInterruptIndex < WR_FX950_GLOBAL_EVENTS_NUM) 
    {
        /* FX950 Global event */
        inFields[0] = WR_FX950_GLOBAL_INT_MISC_TWSI_TIME_OUT + 
                                        index;              /* interrupt name */
        inFields[1] = WR_FAP20M_EVENT_TYPE_GLOBAL;          /* interrupt type */
        inFields[2] = 0;                                    /* HGL or FE port number */
        inFields[3] = 0;                                    /* Lane number */
        return;
    }

    index -= WR_FX950_GLOBAL_EVENTS_NUM;
    if (index < WR_FX950_PORT_EVENTS_NUM) 
    {
        /* FX950 per HGL Port event */
        inFields[0] = WR_FX950_PORT_EVENTS_NUM + 
                      (index % FX950_PORT_NOF_INT);        /* interrupt name */
        inFields[1] = WR_FAP20M_EVENT_TYPE_HGL_PORT;       /* interrupt type */
        inFields[2] = (index / FX950_PORT_NOF_INT);        /* HGL or FE port number */
        inFields[3] = 0;                                   /* Lane number */
        return;
    }

    index -= WR_FX950_PORT_EVENTS_NUM;

    /* FX950 per HGL Port per Lane event */
    inFields[0] = WR_FX950_PORT_LANE_INT_PRBS_ERR + 
                  (index % FX950_PORT_LANE_NOF_INT);                        /* interrupt name */
    inFields[1] = WR_FAP20M_EVENT_TYPE_HGL_PORT_LANE;                       /* interrupt type */
    inFields[2] = (index / 
                   (FX950_PORT_LANE_NOF_INT * FX950_REGS_SERDES_LANES_NUM));/* HGL or FE port number */
    inFields[3] = ((index / FX950_PORT_LANE_NOF_INT) -
                   (inFields[2] * FX950_REGS_SERDES_LANES_NUM));            /* Lane number */
    return;
}

/*******************************************************************************
* wrFx950_api_interrupts_getFirst (table command)
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
static CMD_STATUS wrFx950_api_interrupts_getFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int device_id;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];



    if(fx950InterruptCounters[device_id] == NULL)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }
    fx950IntCounterBlockPtr = fx950InterruptCounters[device_id];

    curInterruptIndex = WR_FAP20M_RECV_SR_DATA_CELL_EVENT;
    endInterruptIndex = WR_FX950_TOTAL_EVENTS_NUM;


    while((curInterruptIndex < endInterruptIndex) &&
          (fx950IntCounterBlockPtr[curInterruptIndex] == 0))
    {
        curInterruptIndex++;
    }

    if(curInterruptIndex < endInterruptIndex)
    {
        /* fill interrupt info fields: name, event type, port number and lane number */
        wrFx950_interrupt_get_info(inFields);
        inFields[4] = fx950IntCounterBlockPtr[curInterruptIndex];   /* event counter */

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}


/*******************************************************************************
* wrFx950_api_interrupts_getNext (table command)
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
static CMD_STATUS wrFx950_api_interrupts_getNext
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

    curInterruptIndex++;

    while((curInterruptIndex < endInterruptIndex) &&
          (fx950IntCounterBlockPtr[curInterruptIndex] == 0))
    {
        curInterruptIndex++;
    }

    if(curInterruptIndex < endInterruptIndex)
    {
        /* fill interrupt info fields: name, event type, port number and lane number */
        wrFx950_interrupt_get_info(inFields);
        inFields[4] = fx950IntCounterBlockPtr[curInterruptIndex];   /* event counter */

        /* pack and output table fields */
        fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);
        galtisOutput(outArgs, GT_OK, "%f");
    }
    else
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
    }

    return CMD_OK;
}


/*******************************************************************************
* wrFx950_api_interrupts_clear (table command)
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
static CMD_STATUS wrFx950_api_interrupts_clear
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int       device_id;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    if(fx950InterruptCounters[device_id] == NULL)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    cmdOsMemSet(fx950InterruptCounters[device_id], 0, (WR_FX950_TOTAL_EVENTS_NUM * 4));
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}


/**** database initialization **************************************/


static CMD_COMMAND dbCommands[] =
{
    {"wrFx950_api_interruptsGetFirst",
        &wrFx950_api_interrupts_getFirst,
        1, 0},

    {"wrFx950_api_interruptsGetNext",
        &wrFx950_api_interrupts_getNext,
        1, 0},

    {"wrFx950_api_interruptsGetClear",
        &wrFx950_api_interrupts_clear,/* clear */
        1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))


/*******************************************************************************
* cmdLibInitAppDemoFx950_api_interrupts
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
GT_STATUS cmdLibInitAppDemoFx950_api_interrupts
(
    GT_VOID
)
{
        /* Reset Event Counter DB */
        cmdOsMemSet(fx950InterruptCounters, 0, sizeof(fx950InterruptCounters));

    return cmdInitLibrary(dbCommands, numCommands);
}


GT_STATUS wrAppDemoFx950DumpCounters
(
    IN unsigned int device_id
)
{
    unsigned long index;
    unsigned int ii;
    unsigned long events_num_printed;
    unsigned long events_num_to_print;
    unsigned long portNum;
    unsigned long laneNum;

    if(fx950InterruptCounters[device_id] == NULL)
    {
        return GT_FAIL;
        /*fx950InterruptCounters[device_id] =  (unsigned long *)cmdOsMalloc(WR_FX950_TOTAL_EVENTS_NUM * 4);

        if(fx950InterruptCounters[device_id] == NULL)
            return GT_OUT_OF_CPU_MEM;

        cmdOsMemSet(fx950InterruptCounters[device_id], 0, (WR_FX950_TOTAL_EVENTS_NUM * 4));
        */
    }

    fx950IntCounterBlockPtr = fx950InterruptCounters[device_id];

    cmdOsPrintf("\n\r");
    cmdOsPrintf("                               FAP20M/FX950 Events        \n\r");
    cmdOsPrintf("|____________________________________________________________________|\n\r");
    cmdOsPrintf("|   Event Name   |");
    cmdOsPrintf("   Event Type   |");
    cmdOsPrintf("   Port Number   |");
    cmdOsPrintf("   Lane Number  |");
    cmdOsPrintf("  Event Counter  |\n\r");
    cmdOsPrintf("|_____________|_____________|_____________|_____________|_____________|\n\r");


    /* dump FAP20M global events */
    events_num_printed = 0;
    events_num_to_print = WR_FAP20M_GLOBAL_EVENTS_NUM;
     
    for (index = events_num_printed; index < events_num_to_print; index++) 
    {
        cmdOsPrintf("|______%d______|______%d______|______%d______|______%d______|______%d______|\n\r", 
                    index, 
                    WR_FAP20M_EVENT_TYPE_GLOBAL, 
                    0, 
                    0, 
                    fx950IntCounterBlockPtr[index]);
    }

    /* dump FAP20M FE port events */
    events_num_printed = WR_FAP20M_GLOBAL_EVENTS_NUM;
    events_num_to_print += FAP20M_NUMBER_OF_LINKS;
    ii = 0;

    for (index = events_num_printed; index < events_num_to_print; index++) 
    {
        cmdOsPrintf("|______%d______|______%d______|______%d______|______%d______|______%d______|\n\r", 
                    WR_FAP20M_ACTIVE_LINK_CHANGED_EVENT, 
                    WR_FAP20M_EVENT_TYPE_FE_PORT, 
                    ii, 
                    0, 
                    fx950IntCounterBlockPtr[index]);
        ii++;
    }

    /* dump FX950 Global events */
    events_num_printed += FAP20M_NUMBER_OF_LINKS;
    events_num_to_print += WR_FX950_GLOBAL_EVENTS_NUM;
    ii = 0;

    for (index = events_num_printed; index < events_num_to_print; index++) 
    {
        cmdOsPrintf("|______%d______|______%d______|______%d______|______%d______|______%d______|\n\r", 
                    WR_FX950_GLOBAL_INT_MISC_TWSI_TIME_OUT + ii, 
                    WR_FAP20M_EVENT_TYPE_GLOBAL, 
                    0, 
                    0, 
                    fx950IntCounterBlockPtr[index]);
        ii++;
    }

    /* dump FX950 HGL Port events */
    events_num_printed += WR_FX950_GLOBAL_EVENTS_NUM;
    events_num_to_print += WR_FX950_PORT_EVENTS_NUM;
    ii = 0;

    for (index = events_num_printed; index < events_num_to_print; index++) 
    {
        cmdOsPrintf("|______%d______|______%d______|______%d______|______%d______|______%d______|\n\r", 
                    WR_FX950_PORT_INT_CONVERTOR_MIB_COUNT_WA + (ii % FX950_PORT_NOF_INT), 
                    WR_FAP20M_EVENT_TYPE_HGL_PORT, 
                    (ii / FX950_PORT_NOF_INT), 
                    0, 
                    fx950IntCounterBlockPtr[index]);
        ii++;
    }

    /* dump FX950 HGL Port Lane events */
    events_num_printed += WR_FX950_PORT_EVENTS_NUM;
    events_num_to_print += WR_FX950_PORT_LANE_EVENTS_NUM;
    ii = 0;

    for (index = events_num_printed; index < events_num_to_print; index++) 
    {
        portNum = (ii / (FX950_PORT_LANE_NOF_INT * FX950_REGS_SERDES_LANES_NUM));
        laneNum = ((ii / FX950_PORT_LANE_NOF_INT) - (portNum * FX950_REGS_SERDES_LANES_NUM));

        cmdOsPrintf("|______%d______|______%d______|______%d______|______%d______|______%d______|\n\r", 
                    WR_FX950_PORT_LANE_INT_PRBS_ERR + (ii % FX950_PORT_LANE_NOF_INT), 
                    WR_FAP20M_EVENT_TYPE_HGL_PORT_LANE, 
                    portNum, 
                    laneNum, 
                    fx950IntCounterBlockPtr[index]);
        ii++;
    }

    return GT_OK;
}


