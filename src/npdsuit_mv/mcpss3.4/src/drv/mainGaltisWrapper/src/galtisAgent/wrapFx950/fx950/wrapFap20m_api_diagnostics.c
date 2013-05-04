/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_diagnostics.c
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
#include <SAND/Utils/include/sand_os_interface.h>
#include <FX950/include/fap20m_api_diagnostics.h>
#include <SAND/SAND_FM/include/sand_mem_access.h>


/* Global variables */
static GT_U32 tbl_index;
static FAP20M_PRBS_SETUP prbs_setup;
static FAP20M_BER_COUNTERS_REPORT ber_counters;


/*****************************************************
*NAME
*  fap20m_last_fabric_header_get
*TYPE:
*  PROC
*DATE: 
*  21/05/2007
*FUNCTION:
*  Read the last packet header from the device.
*INPUT:
*  DIRECT:
*    IN   unsigned int              device_id -
*    OUT  FAP20M_LAST_PACKET_HEADER *hrd
 -
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    error indication
*  INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_last_fabric_header_get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_LAST_PACKET_HEADER hrd;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot api function */
    result = fap20m_last_fabric_header_get(device_id, &hrd);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = hrd.first_chunk_flag;
    inFields[1] = hrd.queue_valid_bit;
    inFields[2] = hrd.queue_id;
    inFields[3] = hrd.drop_precedence;
    inFields[4] = hrd.chunk_size;
    inFields[5] = hrd.packet_size;

    inFields[6] = hrd.tag_info[0];
    inFields[7] = hrd.tag_info[1];
    inFields[8] = hrd.tag_info[2];
    inFields[9] = hrd.tag_info[3];
    inFields[10] = hrd.tag_info[4];

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                          inFields[4], inFields[5], inFields[6], inFields[7],
                                          inFields[8], inFields[9], inFields[10]);

    galtisOutput(outArgs, rc, "%f");
    
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_last_fabric_header_get_end
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
*  fap20m_set_prbs_mode
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure sets the mode of PRBS channels: Which
*  links to activate and whether to activate generator
*  or checker.
*  Checker/Generator are independent of each other.
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . 
*    FAP20M_PRBS_SETUP* prbs_setup -
*      Pointer to buffer for this procedure to read
*      PRBS channels configuration from.
*  INDIRECT:
*    result_ptr.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          FAP20M_SET_PRBS_MODE_001 -
*            Input element 'prbs_generator_link' in
*            FAP20M_PRBS_SETUP is out of range. See comment
*            preceding definition of FAP20M_PRBS_SETUP.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    Device goes into/out of special debug mode.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*  Invoking this procedure may clear BER counters (in
*  case links have changed. See fap20m_get_ber_counters().
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_prbs_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int index;
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

    sand_os_memset((void*)&prbs_setup, 0, sizeof(prbs_setup));

    /* map input arguments to locals */
    index = (unsigned int) inFields[0];
    prbs_setup.prbs_generator_checker_unit[index].activate_prbs_generator = (unsigned int) inFields[1];
    prbs_setup.prbs_generator_checker_unit[index].activate_prbs_checker = (unsigned int) inFields[2];
    prbs_setup.prbs_generator_checker_unit[index].local_loopback_on = (unsigned int) inFields[3];
    prbs_setup.prbs_generator_checker_unit[index].valid = (unsigned int) inFields[4];

    /* call Ocelot api function */
    result = fap20m_set_prbs_mode(device_id, &prbs_setup);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}


/*****************************************************
*NAME
*  fap20m_get_prbs_mode
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure gets information on mode of PRBS
*  channels: Which links were activated and whether
*  they were activated as generators or checkers.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_PRBS_SETUP* prbs_setup -
*      Pointer to buffer for this procedure to read
*      PRBS channels configuration from.
*  INDIRECT:
*    None
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
*    prbs_setup
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_prbs_mode
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

    /* call Ocelot api function */
    result = fap20m_get_prbs_mode(device_id, &prbs_setup);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = prbs_setup.prbs_generator_checker_unit[tbl_index].activate_prbs_generator;
    inFields[2] = prbs_setup.prbs_generator_checker_unit[tbl_index].activate_prbs_checker;
    inFields[3] = prbs_setup.prbs_generator_checker_unit[tbl_index].local_loopback_on;
    inFields[4] = prbs_setup.prbs_generator_checker_unit[tbl_index].valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_prbs_mode_end
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

    if (++tbl_index >= FAP20M_PRBS_NOF_UNITS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = prbs_setup.prbs_generator_checker_unit[tbl_index].activate_prbs_generator;
    inFields[2] = prbs_setup.prbs_generator_checker_unit[tbl_index].activate_prbs_checker;
    inFields[3] = prbs_setup.prbs_generator_checker_unit[tbl_index].local_loopback_on;
    inFields[4] = prbs_setup.prbs_generator_checker_unit[tbl_index].valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_start_prbs_test
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  For enabling PRBS test do the following:
*   1. Use 'fap20m_set_prbs_mode()' to set the links to 
*       generate PRBS stream or to check PRBS stream on.
*   2. Do above (1) on the connected device.
*   3. Call the 'fap20m_start_prbs_test()' procedure to 
*      clear the inner counters. This procedure activate
*      TCM task that counts device error into local structures.
*   4. Use 'fap20m_get_ber_counters()' to read the local 
*      counters - directly. Use 'fap20m_register_callback_function()'
*      read the local counters - periodically.
*   5. To end the test 
*       a. Call 'fap20m_stop_prbs_test()' to deactivate the 
*          related TCM task
*       b. Use 'fap20m_set_prbs_mode()' to set another links or
*          to completely stop the checker/generator.
*INPUT:
*  DIRECT:
*    unsigned int         device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
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
*  +) Use 'fap20m_stop_prbs_test()' to stop the test.
*  +) This procedure clear all 24 links BER counters.
*SEE ALSO:
* 
*****************************************************/
static CMD_STATUS wrFap20m_start_prbs_test
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
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot api function */
    result = fap20m_start_prbs_test(device_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_stop_prbs_test
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Call 'fap20m_stop_prbs_test()' to deactivate the 
*  related TCM task.
*INPUT:
*  DIRECT:
*    unsigned int         device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
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
*  fap20m_start_prbs_test()
*****************************************************/
static CMD_STATUS wrFap20m_stop_prbs_test
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
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot api function */
    result = fap20m_stop_prbs_test(device_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_ber_counters
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure gets the values of PRBS checker
*  error counters and clears them, as required.
*  For immediate return, counter is cleared after
*  the reading.
*  For deferred action, counter is cleared before
*  long process of polling starts.
*INPUT:
*  DIRECT:
*    unsigned int         device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_BER_COUNTERS_REPORT* result_ptr -
*      Pointer to buffer for this procedure to store
*      status and information on all counters in.
*  INDIRECT:
*    See fap20m_set_prbs_mode().
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
*    ber_counters
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*  Note that invoking 'fap20m_start_prbs_test()' may cause
*  clearing of counters.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_ber_counters
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    unsigned short rc;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot api function */
    result = fap20m_get_ber_counters(device_id, &ber_counters);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = ber_counters.single_ber_counter[tbl_index].prbs_checker_activated;
    inFields[1] = ber_counters.single_ber_counter[tbl_index].prbs_checker_link;
    inFields[2] = ber_counters.single_ber_counter[tbl_index].prbs_error_counter;
    inFields[3] = ber_counters.single_ber_counter[tbl_index].prbs_error_overflowed;
    inFields[4] = ber_counters.single_ber_counter[tbl_index].prbs_ber_valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);

    galtisOutput(outArgs, rc, "%d%f", ber_counters.num_active_counters);
    
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_ber_counters_end
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

    if (++tbl_index >= FAP20M_PRBS_NOF_UNITS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = ber_counters.single_ber_counter[tbl_index].prbs_checker_activated;
    inFields[1] = ber_counters.single_ber_counter[tbl_index].prbs_checker_link;
    inFields[2] = ber_counters.single_ber_counter[tbl_index].prbs_error_counter;
    inFields[3] = ber_counters.single_ber_counter[tbl_index].prbs_error_overflowed;
    inFields[4] = ber_counters.single_ber_counter[tbl_index].prbs_ber_valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);

    galtisOutput(outArgs, GT_OK, "%f");
    
    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_clear_prbs_test_info
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

    /* call Ocelot api function */
    fap20m_clear_prbs_test_info(device_id);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_iddr_read
*TYPE:
*  PROC
*DATE:
*  19/APR/2004
*FUNCTION:
*  Read Ingress DRAM word (256 bits).
*  In this memory the packet lay when traffic flows through 
*  the device. Write it only for diagnostics purposes.
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . 
*    unsigned long       dram_offset-
*      Offset in the DRAM.
*      0 - first 256 bits.
*      1 - second 256 bits.
*      2 - third 256 bits.
*      3 - ...
*    unsigned long       data[FAP20M_DRAM_WORD_NOF_LONGS] -
*      Poniter to buffer.
*      Loaded with DRAM data.
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
*    data.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_iddr_read
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    unsigned long dram_offset;
    unsigned long data[FAP20M_IDRAM_WORD_NOF_LONGS];
    unsigned short rc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* avoid compiler warnings */
    numFields = numFields;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    dram_offset = (unsigned long)inArgs[1];

    /* call Ocelot api function */
    result = fap20m_iddr_read(device_id, dram_offset, data);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = data[0];
    inFields[1] = data[1];
    inFields[2] = data[2];
    inFields[3] = data[3];
    inFields[4] = data[4];
    inFields[5] = data[5];
    inFields[6] = data[6];
    inFields[7] = data[7];

    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_iddr_read_end
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
*  fap20m_iddr_write
*TYPE:
*  PROC
*DATE:
*  19/APR/2004
*FUNCTION:
*  Write Ingress DRAM word (256 bits).
*  In this memory the packet lay when traffic flows through 
*  the device. Write it only for diagnostics purposes. 
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . 
*    unsigned long       dram_offset-
*      Offset in the DRAM.
*      0 - first 256 bits.
*      1 - second 256 bits.
*      2 - third 256 bits.
*      3 - ...
*    unsigned long       data[FAP20M_DRAM_WORD_NOF_LONGS] -
*      Poniter to buffer.
*      DRAM is loaded with this data.
*  INDIRECT:
*    data.
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
*    None.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_iddr_write
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    unsigned long dram_offset;
    unsigned long data[FAP20M_IDRAM_WORD_NOF_LONGS];
    unsigned short rc;
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    dram_offset = (unsigned long)inArgs[1];

    for(i = 0; i < FAP20M_IDRAM_WORD_NOF_LONGS; i++)
    {
        data[i] = (unsigned long) inFields[i];
    }

    /* call Ocelot api function */
    result = fap20m_iddr_write(device_id, dram_offset, data);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_qdr_read
*TYPE:
*  PROC
*DATE:
*  19/APR/2004
*FUNCTION:
*  Read QDR word (68 bits).
*  The QDR external memory stores the Link Descriptors.
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . 
*    unsigned long       qdr_offset-
*      Offset in the DRAM.
*      0 - first 68 bits.
*      1 - second 68 bits.
*      2 - third 68 bits.
*      3 - ...
*    unsigned long  data[FAP20M_IDRAM_WORD_NOF_LONGS]
*      QDR data word (68 bit long).
*  INDIRECT:
*    None.
*REMARKS:
*  The QDR access is for debug purpose only.
*  In normal operation, the module should only be 
*   accessed by the device.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_qdr_read
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    unsigned long qdr_offset;
    unsigned long data[FAP20M_QDR_WORD_NOF_LONGS];
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    qdr_offset = (unsigned long)inArgs[1];

    /* call Ocelot api function */
    result = fap20m_qdr_read(device_id, qdr_offset, data);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = data[0];
    inFields[1] = data[1];
    inFields[2] = data[2];

    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_qdr_read_end
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
*  fap20m_qdr_write
*TYPE:
*  PROC
*DATE:
*  19/APR/2004
*FUNCTION:
*  Write QDR word (68 bits).
*  The QDR external memory stores the Link Descriptors.
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . 
*    unsigned long       qdr_offset-
*      Offset in the DRAM.
*      0 - first 68 bits.
*      1 - second 68 bits.
*      2 - third 68 bits.
*      3 - ...
*    unsigned long  data[FAP20M_IDRAM_WORD_NOF_LONGS]
*      QDR data word (68 bit long).
*  INDIRECT:
*    None.
*REMARKS:
*  The QDR access is for debug purpose only.
*  In normal operation, the module should only be 
*   accessed by the device.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_qdr_write
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long qdr_offset;
    unsigned long data[FAP20M_QDR_WORD_NOF_LONGS];
    unsigned short rc;
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    qdr_offset = (unsigned long)inArgs[1];

    for(i = 0; i < FAP20M_QDR_WORD_NOF_LONGS; i++)
    {
        data[i] = (unsigned long) inFields[i];
    }

    /* call Ocelot api function */
    result = fap20m_qdr_write(device_id, qdr_offset, data);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_qdr_get_num_sft_err
*TYPE:
*  PROC
*DATE:
*  22/OCT/2004
*FUNCTION:
*  Get report on QDR parity errors collected so
*  far via monitoring of FAP20M registers.
*INPUT:
*  DIRECT:
*    unsigned int  device_id -
*      Sand-driver device-identifier (returned from
*      fap20m_register_device() . 
*    FAP20M_QDR_GET_NUM_SFT_ERR_REPORT
*                  *fap20m_qdr_get_num_sft_err_report -
*      Poniter to structure to contain results.
*      This procedure loads pointed memory with
*      data related to QDR errors.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    data.
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*  + Collection of QDR errors must be active for
*    this procedure to respond with meaningful
*    data. Such collection is activated by registering
*    QDR error callback (call
*    fap20m_register_callback_function() with
*    'function_id' parameter set to
*    FAP20M_QDR_GET_NUM_SFT_ERR.
*SEE ALSO:
*  FAP20M_QDR_GET_NUM_SFT_ERR_REPORT
*  DataSheet: QdrErrQDR
*  fap20m_register_callback_function()
*****************************************************/
static CMD_STATUS wrFap20m_qdr_get_num_sft_err
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    FAP20M_QDR_GET_NUM_SFT_ERR_REPORT fap20m_qdr_get_num_sft_err_report;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    
    /* call Ocelot api function */
    result = fap20m_qdr_get_num_sft_err(device_id, &fap20m_qdr_get_num_sft_err_report);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = fap20m_qdr_get_num_sft_err_report.accumulated_qdr_sft_err;

    fieldOutput("%d", inFields[0]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_qdr_get_num_sft_err_end
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
*  fap20m_QDP_init_sequence
*TYPE:
*  PROC
*DATE:
*  10/JUN/2004
*FUNCTION:
*  . 
*INPUT:
*  DIRECT:
*    unsigned int  device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . 
*    unsigned long wait_time_in_nano_sec-
*      Delay before setting the QDP-init.
*      Dune suggested time is 1000000, i.e, 1 mili-sec.
*    unsigned int* device_is_working -
*      Pointer to 'unsigned int'.
*      Indicator.
*      TRUE  - device is operational after QDP init.
*      FALSE - device *NOT* is operational after QDP init.
*  INDIRECT:
*    data.
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
*    None.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_QDP_init_sequence
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    unsigned long wait_time_in_nano_sec;
    unsigned int  device_is_working;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    wait_time_in_nano_sec = (unsigned long)inArgs[1];
    
    /* call Ocelot api function */
    result = fap20m_QDP_init_sequence(device_id, wait_time_in_nano_sec, &device_is_working);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", device_is_working);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_diagnostics_dump_regs
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure dumps all registers per unit type or all registers.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_DIAGNOSTICS_UNIT_TYPE unit_type - 
*      Unit type.
*  INDIRECT:
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
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_diagnostics_dump_regs
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
    device_id = (unsigned int) inArgs[0];
    
    /* call Ocelot API function */
    fap20m_diagnostics_dump_regs(device_id);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*
 * Per request, print ALL the indirect memory of the device.
 * Note, this call results in long printing to the screen,
 * using sand_os_printf().
 */
static CMD_STATUS wrFap20m_diagnostics_dump_indirect_tables
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int device_id;
    unsigned int print_zero;
    

    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int) inArgs[0];
    print_zero = (unsigned int) inArgs[2];
    
    /* call Ocelot API function */
    fap20m_diagnostics_dump_indirect_tables(device_id, print_zero);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}



typedef enum
{
    FAP20M_TABLES_MODULE_QDP_E ,
    FAP20M_TABLES_MODULE_RTP_E ,
    FAP20M_TABLES_MODULE_FDT_E ,
    FAP20M_TABLES_MODULE_SCH_PRIM_E ,
    FAP20M_TABLES_MODULE_SCH_SCND_E ,
    FAP20M_TABLES_MODULE_MMU_E, 
    FAP20M_TABLES_MODULE_END_E

}FAP20M_TABLES_MODULE_ENT;

static SAND_RET
  diagnostics_read_write_indirect_tables(
      unsigned int    device_id,
      FAP20M_TABLES_MODULE_ENT  module,
      unsigned int    offset,
      unsigned long   data[4],
      unsigned int    doWrite
  )
{
    unsigned long   realOffset, numOfWords;
    SAND_RET        ret = SAND_OK;


    switch(module)
    {
        case FAP20M_TABLES_MODULE_QDP_E:
            realOffset = (offset & 0x7FFFFFF) | FAP20M_QDP_INDIRECT_MASK;
			numOfWords = 1;
            break;

        case FAP20M_TABLES_MODULE_RTP_E:
            realOffset = (offset & 0x7FFFFFF) | FAP20M_RTP_INDIRECT_MASK;
			numOfWords = 1;
            break;

        case FAP20M_TABLES_MODULE_FDT_E:
            realOffset = (offset & 0x7FFFFFF) | FAP20M_FDT_INDIRECT_MASK;
			numOfWords = 1;
            break;

        case FAP20M_TABLES_MODULE_SCH_PRIM_E:
            realOffset = (offset & 0x7FFFFFF) | FAP20M_SCH_INDIRECT_MASK(FAP20M_SCH_RANK_PRIM);
			numOfWords = 1;
			if( ((realOffset & 0xFFFFF) >= 0xA0000) && ((realOffset & 0xFFFFF) < 0xA0020) )
				numOfWords = 3;
            break;

        case FAP20M_TABLES_MODULE_SCH_SCND_E:
            realOffset = (offset & 0x7FFFFFF) | FAP20M_SCH_INDIRECT_MASK(FAP20M_SCH_RANK_SCND);
			numOfWords = 1;
			if( ((realOffset & 0xFFFFF) >= 0xA0000) && ((realOffset & 0xFFFFF) < 0xA0020) )
				numOfWords = 3;
            break;

        case FAP20M_TABLES_MODULE_MMU_E:
            realOffset = (offset & 0x7FFFFFF) | FAP20M_MMU_INDIRECT_MASK;
			numOfWords = 1;
            break;

        default:
            return SAND_ERR; 
    }

    if(!doWrite)
        ret = sand_mem_read(device_id, data, realOffset, numOfWords*4, TRUE);
    else
        ret = sand_mem_write(device_id, data, realOffset, numOfWords*4, TRUE);

    return ret;
}

/*
 * Per request, read from indirect memory of the device.
 */

static  unsigned int    numOfWordsLeft;
static  unsigned int    fap20mAddress;

static CMD_STATUS wrFap20m_diagnostics_read_indirect_memory_first_line
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET     ret = SAND_OK;
    unsigned int    device_id;
    FAP20M_TABLES_MODULE_ENT  module;
    unsigned long   data[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    module = (SAND_IN FAP20M_TABLES_MODULE_ENT)inArgs[1];
    fap20mAddress = (unsigned int)inArgs[2];
    numOfWordsLeft = (unsigned int)inArgs[3];

    /* Address should be alligned to word  */
    if(numOfWordsLeft == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

	data[0] = data[1] = data[2] = data[3] = 0xBADADD00;
	ret = diagnostics_read_write_indirect_tables(device_id, module, fap20mAddress,
                                                 data, GT_FALSE);
	if(ret != SAND_OK)
	{
		galtisOutput(outArgs, ret, "%d", -1);
		return CMD_OK;
	}

    /* pack output arguments to galtis string */
    inFields[0] = fap20mAddress;
    inFields[1] = data[0];
    inFields[2] = data[1];
    inFields[3] = data[2];
    inFields[4] = data[3];

	fap20mAddress++;
	numOfWordsLeft--;

    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],inFields[4]);

    galtisOutput(outArgs, ret, "%f");
    
    return CMD_OK;
}

static CMD_STATUS wrFap20m_diagnostics_read_indirect_memory_next_line
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET     ret = SAND_OK;
    unsigned int    device_id;
    FAP20M_TABLES_MODULE_ENT  module;
    long   data[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    module = (SAND_IN FAP20M_TABLES_MODULE_ENT)inArgs[1];

    if(numOfWordsLeft == 0)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

	data[0] = data[1] = data[2] = data[3] = 0xBADADD00;
	ret = diagnostics_read_write_indirect_tables(device_id, module, fap20mAddress,
												 data, GT_FALSE);
	if(ret != SAND_OK)
	{
		galtisOutput(outArgs, ret, "%d", -1);
		return CMD_OK;
    }


    /* pack output arguments to galtis string */
    inFields[0] = fap20mAddress;
    inFields[1] = data[0];
    inFields[2] = data[1];
    inFields[3] = data[2];
    inFields[4] = data[3];

	fap20mAddress ++;
	numOfWordsLeft--;

    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],inFields[4]);

    galtisOutput(outArgs, ret, "%f");
    
    return CMD_OK;
}
#if 0
static CMD_STATUS wrFap20m_diagnostics_write_indirect_memory_line_first
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int    device_id;
    FAP20M_TABLES_MODULE_ENT  module;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    module = (SAND_IN FAP20M_TABLES_MODULE_ENT)inArgs[1];
    fap20mAddress = (unsigned int)inArgs[2];
    numOfWordsLeft = (unsigned int)inArgs[3];

    if(fap20mAddress & 0x3)
    {
        galtisOutput(outArgs, GT_BAD_PARAM, "%d%s", -1, "Offset should be word alligend\n");
        return CMD_OK;
    }

    galtisOutput(outArgs, GT_OK, "");
    return CMD_OK;
}
#endif

static CMD_STATUS wrFap20m_diagnostics_write_indirect_memory_line_next
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET     ret = SAND_OK;
    unsigned int    device_id;
    FAP20M_TABLES_MODULE_ENT  module;
    unsigned int    address;
    unsigned long   data[4];

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    module = (SAND_IN FAP20M_TABLES_MODULE_ENT)inArgs[1];

    address = (unsigned int)inFields[0];
    data[0] = (unsigned long)inFields[1];
    data[1] = (unsigned long)inFields[2];
    data[2] = (unsigned long)inFields[3];
    data[3] = (unsigned long)inFields[4];

	ret = diagnostics_read_write_indirect_tables(device_id, module, address,
												 data, GT_TRUE);
	if(ret != SAND_OK)
	{
		galtisOutput(outArgs, ret, "%d", -1);
		return CMD_OK;
	}

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_diagnostics_write_indirect_memory_line_end
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
    outArgs = outArgs;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, SAND_OK, "%d", -1);

    return CMD_OK;
}

/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"last_fabric_headerGetFirst",
         &wrFap20m_last_fabric_header_get,
         1, 0},
        {"last_fabric_headerGetNext",
         &wrFap20m_last_fabric_header_get_end,
         1, 0},

        {"prbs_modeSet",
         &wrFap20m_set_prbs_mode,
         0, 5},
        {"prbs_modeGetFirst",
         &wrFap20m_get_prbs_mode,
         1, 0},
        {"prbs_modeGetNext",
         &wrFap20m_get_prbs_mode_end,
         1, 0},

        {"start_prbs_test",
         &wrFap20m_start_prbs_test,
         1, 0},
        {"stop_prbs_test",
         &wrFap20m_stop_prbs_test,
         1, 0},
        {"clear_prbs_test_info",
         &wrFap20m_clear_prbs_test_info,
         1, 0},

        {"ber_countersGetFirst",
         &wrFap20m_get_ber_counters,
         1, 0},
        {"ber_countersGetNext",
         &wrFap20m_get_ber_counters_end,
         1, 0},

        {"iddr_read_writeSet",
         &wrFap20m_iddr_write,
         2, 8},
        {"iddr_read_writeGetFirst",
         &wrFap20m_iddr_read,
         2, 0},
        {"iddr_read_writeGetNext",
         &wrFap20m_iddr_read_end,
         2, 0},

        {"qdr_read_writeSet",
         &wrFap20m_qdr_write,
         2, 3},
        {"qdr_read_writeGetFirst",
         &wrFap20m_qdr_read,
         2, 0},
        {"qdr_read_writeGetNext",
         &wrFap20m_qdr_read_end,
         2, 0},
        
        {"qdr_num_sft_errGetFirst",
         &wrFap20m_qdr_get_num_sft_err,
         1, 0},
        {"qdr_num_sft_errGetNext",
         &wrFap20m_qdr_get_num_sft_err_end,
         1, 0},

        {"QDP_init_sequence",
         &wrFap20m_QDP_init_sequence,
         2, 0},

        {"dump_regs",
         &wrFap20m_diagnostics_dump_regs,
         1, 0},
        {"dump_indirect_tables",
         &wrFap20m_diagnostics_dump_indirect_tables,
         2, 0},
        {"module_read_write_memGetFirst",
         &wrFap20m_diagnostics_read_indirect_memory_first_line,
         4, 0},
        {"module_read_write_memGetNext",
         &wrFap20m_diagnostics_read_indirect_memory_next_line,
         4, 0},
        {"module_read_write_memSet",
         &wrFap20m_set_prbs_mode,
         4, 5},
        {"module_read_write_memSet",
         &wrFap20m_diagnostics_write_indirect_memory_line_next,
         4, 5},
        {"module_read_write_memEndSet",
         &wrFap20m_diagnostics_write_indirect_memory_line_end,
         4, 5},

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_diagnostics
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
GT_STATUS cmdLibInitFap20m_api_diagnostics
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

