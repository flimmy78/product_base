/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_cell.c
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
#include <FX950/include/fap20m_api_cell.h>
#include <FX950/include/fap20m_cell.h>

/*****************************************************
*NAME
*  fap20m_send_sr_data_cell
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure transmits one source routed data
*  cell.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    unsigned long    *result_ptr -
*      Pointer to buffer for this procedure to read
*      contents of source routed data cell from.
*      Buffer is of SAND_TX_SR_DATA_CELL type (i.e.
*      'result_ptr' is treated as pointer to
*      SAND_TX_SR_DATA_CELL).
*      Size of buffer must be at least
*      sizeof(SAND_TX_SR_DATA_CELL).
*      See details of data to fill  in 'Definitions
*      related to source routed data cells' above.
*  INDIRECT:
*    sr_data_cell, SAND_TX_CELL_TIMEOUT.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          FAP20M_SEND_SR_DATA_CELL_001 -
*            Input parameter 'fe3_link' is out of range.
*          FAP20M_SEND_SR_DATA_CELL_002 -
*            Input parameter 'fe2_link' is out of range.
*          FAP20M_SEND_SR_DATA_CELL_003 -
*            Input parameter 'destination_device_type' is
*            illegal.
*          FAP20M_SEND_SR_DATA_CELL_004 -
*            Input parameter 'source_device_type' is
*            illegal.
*          FAP20M_SEND_SR_DATA_CELL_005 -
*            Input parameter 'source_chip_id' is
*            out of range.
*          FAP20M_SEND_SR_DATA_CELL_006 -
*            Input parameter 'initial_output_link' is
*            out of range.
*          FAP20M_SEND_SR_DATA_CELL_007 -
*            Timeout on waiting for 'transmit data cell
*            trigger' to be deasserted.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    Data cell written into chip memory.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_send_sr_data_cell
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    SAND_TX_SR_DATA_CELL sr_data_cell;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    sr_data_cell.body_tx[0] = (unsigned long)inFields[0];
    sr_data_cell.body_tx[1] = (unsigned long)inFields[1];
    sr_data_cell.body_tx[2] = (unsigned long)inFields[2];
    sr_data_cell.body_tx[3] = (unsigned long)inFields[3];
    sr_data_cell.body_tx[4] = (unsigned long)inFields[4];
    sr_data_cell.body_tx[5] = (unsigned long)inFields[5];
    sr_data_cell.body_tx[6] = (unsigned long)inFields[6];
    sr_data_cell.body_tx[7] = (unsigned long)inFields[7];
    
    sr_data_cell.fe3_link = (unsigned int)inFields[8];
    sr_data_cell.fe2_link = (unsigned int)inFields[9];
    sr_data_cell.fe1_link = (unsigned int)inFields[10];

    sr_data_cell.destination_entity_type = (SAND_DEVICE_ENTITY)inFields[11];
    sr_data_cell.source_entity_type = (SAND_DEVICE_ENTITY)inFields[12];
    sr_data_cell.source_chip_id = (unsigned int)inFields[13];
    sr_data_cell.initial_output_link = (unsigned int)inFields[14];

    /* call Ocelot API function */
    result = fap20m_send_sr_data_cell(device_id, &sr_data_cell);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_recv_sr_data_cell
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure transmits one source routed data
*  cell.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    FAP20M_RX_SR_DATA_CELL* sr_data_cell -
*      Pointer to buffer for this procedure to store
*      received source routed data cell and related
*      information.
*      Size of buffer must be at least
*      sizeof(FAP20M_RX_SR_DATA_BUFFER).
*      See details of data to fill in 
*     'FAP20M_RX_SR_DATA_BUFFER' comments above.
*  INDIRECT:
*    None
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          FAP20M_RECV_SR_DATA_CELL_001 -
*            Illegal value for parameter 'links_to_check'.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    sr_data_cell
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_recv_sr_data_cell
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    FAP20M_RX_SR_DATA_BUFFER sr_data_cell_buff;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_recv_sr_data_cell(device_id, &sr_data_cell_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = sr_data_cell_buff.hand_over_reason;
    inFields[1] = sr_data_cell_buff.received_something;
    inFields[2] = sr_data_cell_buff.received_not_sr_data_cell;
    inFields[3] = sr_data_cell_buff.recv_overflow;
    
    inFields[4] = sr_data_cell_buff.rx_cell.body_tx[0];
    inFields[5] = sr_data_cell_buff.rx_cell.body_tx[1];
    inFields[6] = sr_data_cell_buff.rx_cell.body_tx[2];
    inFields[7] = sr_data_cell_buff.rx_cell.body_tx[3];
    inFields[8] = sr_data_cell_buff.rx_cell.body_tx[4];
    inFields[9] = sr_data_cell_buff.rx_cell.body_tx[5];
    inFields[10] = sr_data_cell_buff.rx_cell.body_tx[6];
    inFields[11] = sr_data_cell_buff.rx_cell.body_tx[7];
    
    inFields[12] = sr_data_cell_buff.rx_cell.source_entity_type;
    inFields[13] = sr_data_cell_buff.rx_cell.source_chip_id;

    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],
                                                inFields[3],  inFields[4],  inFields[5],
                                                inFields[6],  inFields[7],  inFields[8],
                                                inFields[9],  inFields[10], inFields[11],
                                                inFields[12], inFields[13]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_recv_sr_data_cell_end
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
*  fap20m_send_control_cell
*TYPE:
*  PROC
*DATE:
*  05/May/2005
*FUNCTION:
*  This procedure transmits one control cell.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    SAND_CONTROL_CELL* control_cell -
*      A structured control cell
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    Control cell written into chip memory.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_send_control_cell
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    SAND_CONTROL_CELL control_cell;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    control_cell.type = (SAND_CONTROL_CELL_TYPE)inFields[0];
    control_cell.dest_id = (unsigned int)inFields[1];
    control_cell.source_id = (unsigned int)inFields[2];
    control_cell.u.flow_status_info.sched_flow_id = (unsigned int)inFields[3];
    control_cell.u.flow_status_info.input_q_number = (unsigned int)inFields[4];
    control_cell.u.flow_status_info.flow_status = (SAND_SCHEDULER_FLOW_STATUS)inFields[5];

    /* call Ocelot API function */
    result = fap20m_send_control_cell(device_id, &control_cell);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}


/*****************************************************
*NAME
* 
*  fap20m_cell_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_CELL_SW_DB* sw_db_module_buff -
*    pointer to a structure containing mudule's Software Database (SW_DB)
*  INDIRECT:
*OUTPUT:
*  DIRECT:
*    unsigned long - function's error word
*  INDIRECT:
*    all modules global data structures
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_cell_sw_db_load
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    FAP20M_CELL_SW_DB sw_db_module_buff;
    unsigned short rc;
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    for(i = 0; i < SAND_MAX_DEVICE; i++)
    {
        sw_db_module_buff.fap20m_last_cell_read_side[i] = (unsigned int)inFields[i];
    }

    /* call Ocelot API function */
    result = fap20m_cell_sw_db_load(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_cell_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_CELL_SW_DB* sw_db_module_buff -
*    pointer to a buffer to which Software Database (SW_DB) will be saved
*OUTPUT:
*  DIRECT:
*    unsigned long - function's error word
*  INDIRECT:
*    pointer to the saved Software Database
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_cell_sw_db_save
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    FAP20M_CELL_SW_DB sw_db_module_buff;
    unsigned short rc;
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_cell_sw_db_save(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    for(i = 0; i < SAND_MAX_DEVICE; i++)
    {
        inFields[i] = sw_db_module_buff.fap20m_last_cell_read_side[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_cell_sw_db_save_end
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
*  fap20m_decide_which_side_to_read_cell_from
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Decide on the side to take cells from.
*  If case have 2 side - do round robin.
*INPUT:
*  DIRECT:
*    SAND_IN unsigned int device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() . 
*    SAND_IN unsigned int data_cell_fifo_not_empty_a_b -
*    bit 0 is set: side A have cell.
*    bit 1 is set: side B have cell.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      Indicator.
*      1 - indicate side to take is A.
*      0 - indicate side to take is B.
*  INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_decide_which_side_to_read_cell_from
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int data_cell_fifo_not_empty_a_b;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
    
    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    data_cell_fifo_not_empty_a_b = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_decide_which_side_to_read_cell_from(device_id, data_cell_fifo_not_empty_a_b);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"send_sr_data_cellSet",
         &wrFap20m_send_sr_data_cell,
         1, 15},

        {"recv_sr_data_cellGetFirst",
         &wrFap20m_recv_sr_data_cell,
         1, 0},
        {"recv_sr_data_cellGetNext",
         &wrFap20m_recv_sr_data_cell_end,
         1, 0},

        {"send_recv_control_cellSet",
         &wrFap20m_send_control_cell,
         1, 6},

        {"cell_sw_dbSet",
         &wrFap20m_cell_sw_db_load,
         0, 8},
        {"cell_sw_dbGetFirst",
         &wrFap20m_cell_sw_db_save,
         0, 0},
        {"cell_sw_dbGetNext",
         &wrFap20m_cell_sw_db_save_end,
         0, 0},

        {"decide_which_side_to_read_cell_from",
         &wrFap20m_decide_which_side_to_read_cell_from,
         2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_cell
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
GT_STATUS cmdLibInitFap20m_api_cell
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

