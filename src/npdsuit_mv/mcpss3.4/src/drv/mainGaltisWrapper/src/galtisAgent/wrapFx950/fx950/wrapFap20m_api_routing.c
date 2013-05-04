/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_routing.c
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
#include <FX950/include/fap20m_api_routing.h>
#include <FX950/include/fap20m_routing.h>
#include <FX950/include/fap20m_api_general.h>


/* Global variables */
static GT_U32 tbl_index;
static unsigned long result_ptr[FAP20M_SPATIAL_MULTICAST_NOF_GROUPS];
static unsigned long global_table[FAP20M_SPATIAL_MULTICAST_NOF_GROUPS];
static unsigned int end_table_entry;
static FAP20M_DIST_UNI_REPORT uni_report;


static void prvFap20mRouting_SHORT_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = uni_report.changes_only;
    inFields[1] = uni_report.offset;
    inFields[2] = uni_report.num_entries;
    inFields[3] = uni_report.link_status_changed;
    inFields[4] = uni_report.link_state_up;
    inFields[5] = uni_report.debug_flags_pending;
    inFields[6] = uni_report.entry[0].short_entry.distribution_entry;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                  inFields[4], inFields[5], inFields[6]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 0);
}

static void prvFap20mRouting_LONG_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = uni_report.changes_only;
    inFields[1] = uni_report.offset;
    inFields[2] = uni_report.num_entries;
    inFields[3] = uni_report.link_status_changed;
    inFields[4] = uni_report.link_state_up;
    inFields[5] = uni_report.debug_flags_pending;
    inFields[6] = uni_report.entry[0].long_entry.index_of_entry;
    inFields[7] = uni_report.entry[0].long_entry.distribution_entry;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 1);
}

static CMD_STATUS wrFap20m_set_unicast_distribution_table
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

    /* map input arguments to locals */
    uni_report.changes_only = (unsigned int)inFields[0];
    uni_report.offset = (unsigned int)inFields[1];
    uni_report.num_entries = (unsigned int)inFields[2];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_unicast_distribution_table
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure gets unicast distribution table. 
*  User may request the full table or just changes
*  to the table (since last report).
*  The driver reads device table: Unicast Distribution Table - UDS.
*  This table is automatically updated by the hardware.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_DIST_UNI_REPORT* uni_report
*      Pointer to buffer for this procedure to store
*      status and information on all links in.
*      Note, however, that this structure only contains the
*      first entry of the table. See explanation
*      in definition of FAP20M_DIST_UNI_REPORT.
*      To be on the safe size, size of buffer should be
*      at least sizeof(FAP20M_DIST_UNI_REPORT)
*      plus [2047 * sizeof(DIST_ROUTE_ENTRY)]. However,
*      a smaller buffer may be used if a smaller
*      'num_entries' is specified. Read closely
*      description of FAP20M_DIST_UNI_REPORT
*      above.
*      To avoid errors, caller is required, below,
*      to specify buffer size.
*      See details of data to fill in 'Definitions
*      related to distribution table' above.
*      Note: 'FAP20M_NOF_FAPS_IN_SYSTEM' is defined to 2048.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          FAP20M_GET_UNICAST_TABLE_001 -
*            Size of buffer pointed by 'result_ptr' is
*            insufficient for containing required table.
*          FAP20M_GET_UNICAST_TABLE_002 -
*            'offset' element in input
*            FAP20M_DIST_UNI_REPORT (pointed by
*            'result_ptr') is out of range.
*          FAP20M_GET_UNICAST_TABLE_003 -
*            'num_entries' element in input
*            FAP20M_DIST_UNI_REPORT (pointed by
*            'result_ptr') is out of range.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    uni_report
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*  It IS possible to issue an 'immediate return' request
*  ('activate_polling' = 0) while a polling transaction
*  is active! It is also possible to activate a few
*  polling transactions at the same time.
*  Note that once a change has been reported, on any
*  polling transaction or 'immediate return' request,
*  then it is marked as such and is not reported as
*  a 'change' any more.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_unicast_distribution_table
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

    uni_report.changes_only = (unsigned int)inFields[0];

    /* call Ocelot API function */
    result = fap20m_get_unicast_distribution_table(device_id, &uni_report);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    if (0 == uni_report.changes_only)
    {
        prvFap20mRouting_SHORT_E(inFields, outArgs);
    } 
    else
    {
        prvFap20mRouting_LONG_E(inFields, outArgs);
    }
    
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_unicast_distribution_table_end
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
*  fap20m_get_spatial_multicast_distribution_table
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure retrieves the multicast distribution table.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long*   result_ptr -
*      Pointer to array of 'unsigned long' with 
*      'nof_table_entries' entries. 
*      (Hence total size of array nof_table_entries*4.)
*    unsigned int     start_table_entry -
*       The first entry on the multicast distribution table 
*       to start copy user's array. 
*       Range from 0 to 2047.
*    unsigned int     nof_table_entries -
*       Number of user requested entries to copy.
*       start_table_entry + nof_table_entries should be 
*       smaller equal than 2048 (total number of entries).
*  INDIRECT:
*    result_ptr.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          SAND_NULL_POINTER_ERR, FAP20M_MULTICAST_SPATIAL_ID_OUT_OF_RANGE_ERR.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    result_ptr
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_spatial_multicast_distribution_table
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int start_table_entry;
    unsigned int nof_table_entries;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    start_table_entry = (unsigned int)inArgs[1];
    nof_table_entries = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_get_spatial_multicast_distribution_table(device_id, result_ptr, start_table_entry, nof_table_entries);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = start_table_entry;
    end_table_entry = start_table_entry + nof_table_entries;

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = result_ptr[tbl_index];

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_spatial_multicast_distribution_table_end
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

    if (++tbl_index >= end_table_entry)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = result_ptr[tbl_index];

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_spatial_multicast_distribution_table
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure sets the multicast distribution table.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long*   table -
*      Pointer to array of 'unsigned long' with 
*      'nof_table_entries' entries. 
*      (Hence total size of array nof_table_entries*4.)
*    unsigned int     start_table_entry -
*       The first entry on the multicast distribution table 
*       to write to.
*       Range from 0 to 2047.
*    unsigned int     nof_table_entries -
*       Number of user requested entries to copy.
*       start_table_entry + nof_table_entries should be 
*       smaller equal than 2048 (total number of entries).
*  INDIRECT:
*    table.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          SAND_NULL_POINTER_ERR, FAP20M_MULTICAST_SPATIAL_ID_OUT_OF_RANGE_ERR.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    None.
*REMARKS:
*  This function only updates the "Multicast Distribution Table".
*  If one wishes to open spatial multicast flow please refer to:
*   fap20m_api_spatial_multicast.h
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_first_spatial_multicast_distribution_table
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int index;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    sand_os_memset(global_table, 0, sizeof(global_table));

    /* map input arguments to locals */
    index = (unsigned int)inFields[0];
    global_table[index] = (unsigned long)inFields[1];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_set_next_spatial_multicast_distribution_table
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int index;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    index = (unsigned int)inFields[0];
    global_table[index] = (unsigned long)inFields[1];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_end_set_spatial_multicast_distribution_table
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int start_table_entry;
    unsigned int nof_table_entries;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    start_table_entry = (unsigned int)inArgs[1];
    nof_table_entries = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_set_spatial_multicast_distribution_table(device_id, global_table, start_table_entry, nof_table_entries);

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
* 
*  fap20m_routing_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_ROUTING_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_routing_sw_db_load
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    FAP20M_ROUTING_SW_DB sw_db_module_buff;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[0] = (unsigned int)inFields[0];
    sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[1] = (unsigned int)inFields[1];
    sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[2] = (unsigned int)inFields[2];
    sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[3] = (unsigned int)inFields[3];
    sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[4] = (unsigned int)inFields[4];
    sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[5] = (unsigned int)inFields[5];
    sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[6] = (unsigned int)inFields[6];
    sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[7] = (unsigned int)inFields[7];

    /* call Ocelot API function */
    result = fap20m_routing_sw_db_load(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_routing_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_ROUTING_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_routing_sw_db_save
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    FAP20M_ROUTING_SW_DB sw_db_module_buff;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_routing_sw_db_save(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[0];
    inFields[1] = sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[1];
    inFields[2] = sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[2];
    inFields[3] = sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[3];
    inFields[4] = sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[4];
    inFields[5] = sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[5];
    inFields[6] = sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[6];
    inFields[7] = sw_db_module_buff.fap20m_rtp_link_activ_mask_changed[7];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", 
                            inFields[0], inFields[1], inFields[2], inFields[3],
                            inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_routing_sw_db_save_end
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
*  fap20m_get_rtp_link_activ_mask
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Read the device 'rtp_link_activ_mask'.
*  24 bits, one per link.
*  1 - indicates link up and running.
*  0 - indicates link down.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() . 
*    unsigned long*   rtp_link_activ_mask -
*       Loaded with result.
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
*    rtp_mask
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_rtp_link_active_mask
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long rtp_mask;
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
    result = fap20m_get_rtp_link_active_mask(device_id, &rtp_mask);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", rtp_mask);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_rtp_link_active_mask_changed
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Read the device 'rtp_link_activ_mask_changed'.
*  1 bit.
*  1 - indicates some of the links where changed from last time reported.
*  0 - indicates no chnage from last report.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() . 
*    unsigned int*  link_status_changed -
*       Loaded with result.
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
*    link_status_changed
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_rtp_link_active_mask_changed
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int link_status_changed;
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
    result = fap20m_get_rtp_link_active_mask_changed(device_id, &link_status_changed);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", link_status_changed);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"unicast_distribution_tableSet",
         &wrFap20m_set_unicast_distribution_table,
         0, 3},
        {"unicast_distribution_tableGetFirst",
         &wrFap20m_get_unicast_distribution_table,
         1, 0},
        {"unicast_distribution_tableGetNext",
         &wrFap20m_get_unicast_distribution_table_end,
         1, 0},

        {"spatial_multicast_distribution_tableSetFirst",
         &wrFap20m_set_first_spatial_multicast_distribution_table,
         0, 2},
        {"spatial_multicast_distribution_tableSetNext",
         &wrFap20m_set_next_spatial_multicast_distribution_table,
         0, 2},
        {"spatial_multicast_distribution_tableEndSet",
         &wrFap20m_end_set_spatial_multicast_distribution_table,
         3, 0},
        {"spatial_multicast_distribution_tableGetFirst",
         &wrFap20m_get_spatial_multicast_distribution_table,
         3, 0},
        {"spatial_multicast_distribution_tableGetNext",
         &wrFap20m_get_spatial_multicast_distribution_table_end,
         3, 0},

        {"routing_sw_dbSet",
         &wrFap20m_routing_sw_db_load,
         0, 8},
        {"routing_sw_dbGetFirst",
         &wrFap20m_routing_sw_db_save,
         0, 0},
        {"routing_sw_dbGetNext",
         &wrFap20m_routing_sw_db_save_end,
         0, 0},

        {"get_rtp_link_active_mask",
         &wrFap20m_get_rtp_link_active_mask,
         1, 0},
        {"get_rtp_link_active_mask_changed",
         &wrFap20m_get_rtp_link_active_mask_changed,
         1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_routing
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
GT_STATUS cmdLibInitFap20m_api_routing
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

