/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_ingress_queue.c
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
#include <FX950/include/fap20m_api_ingress_queue.h>

/*****************************************************
*NAME
*  fap20m_open_queue
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Open at the device an ingress FAP queue to 
*  a target FAP scheduler flow. 
*  Sets the queue parameters.
*  The driver write to the device tables: 
*  Queue Type Table - QUT and Queue-Flow Lookup Table - QFL.
*INPUT:
*  DIRECT:
*   unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   unsigned int     queue_id -
*       Queue identifier.
*       Range from 4 to 8191.
*       Queues 0 to 3 are dedicate to spatial multicast.
*   unsigned int     target_fap_id -
*       The target FAP for the queue. Range from 0 to 2047. 
*       Irrelevant for queues 0 to 3.
*   unsigned int     target_flow_id -
*       Flow, in egress scheduler, assigned for that queue.
*       Range from 0 to 16383 (16K).
*       Irrelevant for queues 0 to 3.
*       If the queue is low_delay, it should be configured
*       to use unused flow. 
*       Dune define port 16383 as the unused flow.
*   FAP20M_QUEUE_TYPE queue_type -
*       The queue type.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          FAP20M_FAP_ID_OUT_OF_RANGE_ERR
*          FAP20M_QUEUE_ID_OUT_OF_RANGE_ERR
*          FAP20M_SCH_FLOW_ID_OUT_OF_RANGE_ERR
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    NON
*REMARKS:
*  When opening the flow, use the real flow_id and queue_id,
*  and set TRUE in the low_delay parameter.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_open_queue
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int queue_id;
    unsigned int target_fap_id; 
    unsigned int target_flow_id;
    FAP20M_QUEUE_TYPE queue_type;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    queue_id = (unsigned int)inArgs[1];
    target_fap_id = (unsigned long)inArgs[2];
    target_flow_id = (unsigned long)inArgs[3];
    queue_type = (FAP20M_QUEUE_TYPE)inArgs[4];

    /* call Ocelot API function */
    result = fap20m_open_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_update_queue
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Update an ingress FAP queue parameters.
*  The driver write to the device tables: 
*  Queue Type Table - QUT and   Queue-Flow Lookup Table - QFL.
*INPUT:
*  DIRECT:
*   unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   unsigned int     queue_id -
*       Queue identifier.
*       Range from 4 to 8191.
*       Queues 0 to 3 are dedicate to spatial multicast.
*   unsigned int     target_fap_id -
*       The target FAP for the queue. Range from 0 to 2047. 
*       Irrelevant for queues 0 to 3.
*   unsigned int     target_flow_id -
*       Flow, in egress scheduler, assigned for that queue.
*       Range from 0 to 16383 (16K).
*       Irrelevant for queues 0 to 3.
*   FAP20M_QUEUE_TYPE queue_type -
*       The queue type.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          FAP20M_FAP_ID_OUT_OF_RANGE_ERR
*          FAP20M_QUEUE_ID_OUT_OF_RANGE_ERR
*          FAP20M_SCH_FLOW_ID_OUT_OF_RANGE_ERR
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_update_queue
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int queue_id;
    unsigned int target_fap_id; 
    unsigned int target_flow_id;
    FAP20M_QUEUE_TYPE queue_type;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    queue_id = (unsigned int)inArgs[1];
    target_fap_id = (unsigned long)inArgs[2];
    target_flow_id = (unsigned long)inArgs[3];
    queue_type = (FAP20M_QUEUE_TYPE)inArgs[4];

    /* call Ocelot API function */
    result = fap20m_update_queue(device_id, queue_id, target_fap_id, target_flow_id, queue_type);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_queue_params
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Get from the device the ingress FAP queue 
*  configured parameters.
*  The driver read from the device tables:
*  Queue Type Table - QUT and   Queue-Flow Lookup Table - QFL.
*INPUT:
*  DIRECT:
*   unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   unsigned int     queue_id -
*       Queue identifier. Range from 0 to 8191 (8K).
*   unsigned int*    target_fap_id -
*       Loaded with the target FAP for the queue. 
*       Irrelevant for queues 0 to 3.
*   unsigned int*     target_flow_id -
*       Loaded with flow id, in egress scheduler, 
*       assigned for the queue.
*       Irrelevant for queues 0 to 3.
*   FAP20M_QUEUE_TYPE* queue_type -
*       Loaded with the queue type.
*  INDIRECT:
*    None.
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
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_queue_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int queue_id;
    unsigned int target_fap_id;
    unsigned int target_flow_id;
    FAP20M_QUEUE_TYPE queue_type;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    queue_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_get_queue_params(device_id, queue_id, &target_fap_id, &target_flow_id, &queue_type);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d%d", target_fap_id, target_flow_id, queue_type);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_close_queue
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Close an ingress FAP queue.
*  The driver write to tables:
*  Queue Type Table - QUT and Queue-Flow Lookup Table - QFL.
*  Note the device is set with some default parameters. 
*  + These parameters do NOT cause the device not to send packets.
*  + Every 4 consecutive queues share the same destination FAP id.
*    Refer to DS section "Queue Binding (to flow, port, and FAP)".
*INPUT:
*  DIRECT:
*   unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   unsigned int     queue_id -
*       Queue identifier. Range from 0 to 8191 (8K).
*  INDIRECT:
*    None.
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
*    NON
*REMARKS:
*  Note!!! This function calls to 
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_close_queue
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int queue_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    queue_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_close_queue(device_id, queue_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_ingress_queue_get_size
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Get the current queue size.
*INPUT:
*  DIRECT:
*   unsigned int   device_id -
*     Sand-driver device-identifier (returned from fap20m_register_device() .
*   unsigned int   queue_id -
*     Queue identifier.
*     Range from 0 to 8191.
*   unsigned long* queue_byte_size -
*     Pointer to 'unsigned long'. Loaded with ingress queue size.
*     The size is in bytes. Note, every packet in the DRAM memory
*     is rounded to 16 byte. Example: if the queue has 2 packets of 60 bytes,
*     this will be loaded with 128 bytes.
*  INDIRECT:
*    None.
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
*    
*REMARKS:
*  None.
*SEE ALSO:
* fap20m_close_queue()
*****************************************************/
static CMD_STATUS wrFap20m_ingress_queue_get_size
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int queue_id;
    unsigned long queue_byte_size;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    queue_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_ingress_queue_get_size(device_id, queue_id, &queue_byte_size);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", queue_byte_size);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"open_queue",
         &wrFap20m_open_queue,
         5, 0},
        {"update_queue",
         &wrFap20m_update_queue,
         5, 0},
        {"get_queue_params",
         &wrFap20m_get_queue_params,
         2, 0},
        {"close_queue",
         &wrFap20m_close_queue,
         2, 0},
        {"ingress_queue_get_size",
         &wrFap20m_ingress_queue_get_size,
         2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_ingress_queue
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
GT_STATUS cmdLibInitFap20m_api_ingress_queue
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

