/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_statistics.c
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

#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <SAND/Utils/include/sand_framework.h>
#include <FX950/include/fap20m_api_statistics.h>
#include <FX950/include/fap20m_statistics.h>


/* Global variables */
static GT_U32 tbl_index;
static FAP20M_ALL_STATISTIC_COUNTERS all_statistic_counters;

/*****************************************************
*NAME
*  fap20m_get_counter
*TYPE:
*  PROC
*DATE:
*  27/OCT/2002
*FUNCTION:
*  This procedure gets the count of the number of 
*  packets that were added to the ingress queues.
*  For immediate return, counter is cleared after
*  the reading.
*  For deferred action, counter is cleared before
*  long process of polling starts.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    FAP20M_COUNTER_TYPE  counter_type -
*      One of possible counters that the driver collects in the background
*    SAND_64CNT*          result_ptr -
*      Pointer to buffer to store counter.
*  INDIRECT:
*    Non.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    result_ptr
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_counter
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_COUNTER_TYPE  counter_type;
    SAND_64CNT result_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    counter_type = (FAP20M_COUNTER_TYPE)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_get_counter(device_id, counter_type, &result_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = result_ptr.u64.arr[0];
    inFields[1] = result_ptr.u64.arr[1];
    inFields[2] = result_ptr.overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_counter_end
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* avoid compiler warnings */
    inArgs = inArgs;
    inFields = inFields;
    numFields = numFields;

    galtisOutput(outArgs, GT_OK, "%d", -1);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_all_counters
*TYPE:
*  PROC
*DATE:
*  27/OCT/2002
*FUNCTION:
*  This procedure gets all 'FAP20M_COUNTER_TYPE' counters.
*  Counter is cleared after the reading.
*  
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    FAP20M_ALL_STATISTIC_COUNTERS* all_statistic_counters -
*      Pointer to structure to store all 'FAP20M_COUNTER_TYPE' counters.
*  INDIRECT:
*    Non.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    result_ptr
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*  + This procedure read all counters from device,
*    so the latest counter data is read.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_all_counters
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned short rc;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_get_all_counters(device_id, &all_statistic_counters);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = all_statistic_counters.counters[tbl_index].u64.arr[0];
    inFields[1] = all_statistic_counters.counters[tbl_index].u64.arr[1];
    inFields[2] = all_statistic_counters.counters[tbl_index].overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);
    
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_get_all_counters_end
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (++tbl_index >= FAP20M_NOF_COUNTER_TYPES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = all_statistic_counters.counters[tbl_index].u64.arr[0];
    inFields[1] = all_statistic_counters.counters[tbl_index].u64.arr[1];
    inFields[2] = all_statistic_counters.counters[tbl_index].overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);
    
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME: 
* fap20m_collect_pkt_counters
*DATE: 
* 25/DEC/2002
*FUNCTION: the actual method that collects the packet counters
* from the FAP20M. it's called by the general counter collection 
* callback that is registered at device registration time
*INPUT:
*  DIRECT:
*    unsigned long device_id - the device to approach
*  INDIRECT:
*OUTPUT:
*  DIRECT:
*  INDIRECT:
*   direct and deferred packet counters are updated 
*REMARKS:
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_collect_pkt_counters
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned long device_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_collect_pkt_counters(device_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*
 * Queue selection mask. A queue matches if the 
 * unmasked bits of PrgCntTargetQ match the queue 
 * number, i.e. count if
 * (queue|PrgCntTargetQMask) == 
 *   (PrgCntTargetQ| PrgCntTargetQMask) 
 * If only one queue should be tested, use 
 *  programmable_queue_mask = 0.
 * The results of the programmable counters will be displayed
 *  when the function 'fap20m_get_all_counters()' is called.
 */
static CMD_STATUS wrFap20m_set_programmable_queue
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int programmable_queue;
    unsigned int programmable_queue_mask;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    programmable_queue = (unsigned int)inArgs[1];
    programmable_queue_mask = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_set_programmable_queue(device_id, programmable_queue, programmable_queue_mask);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
 * See details in fap20m_statistics.c
 *****************************************************/
static CMD_STATUS wrFap20m_statistics_init
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int device_id;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    fap20m_statistics_init(device_id);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
 * See details in fap20m_statistics.c
 *****************************************************/
static CMD_STATUS wrFap20m_statistics_clear_deferred_counter
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int device_id;
    FAP20M_COUNTER_TYPE counter_type;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    counter_type = (FAP20M_COUNTER_TYPE)inArgs[1];

    /* call Ocelot API function */
    fap20m_statistics_clear_deferred_counter(device_id, counter_type);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}



/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"get_counterGetFirst",
         &wrFap20m_get_counter,
         2, 0},
        {"get_counterGetNext",
         &wrFap20m_get_counter_end,
         2, 0},

        {"get_all_countersGetFirst",
         &wrFap20m_get_all_counters,
         1, 0},
        {"get_all_countersGetNext",
         &wrFap20m_get_all_counters_end,
         1, 0},

        {"collect_pkt_counters",
         &wrFap20m_collect_pkt_counters,
         1, 0},
        {"set_programmable_queue",
         &wrFap20m_set_programmable_queue,
         3, 0},
        {"statistics_init",
         &wrFap20m_statistics_init,
         1, 0},
        {"statistics_clear_deferred_counter",
         &wrFap20m_statistics_clear_deferred_counter,
         2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_statistics
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
GT_STATUS cmdLibInitFap20m_api_statistics
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

