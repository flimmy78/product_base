/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_links.c
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
#include <FX950/include/fap20m_api_links.h>


/* Global variables */
static GT_U32 tbl_index;
static FAP20M_ALL_LINK_STATUS global_all_link_status;
static FAP20M_ALL_LINK_CONNECTIVITY global_all_link_connectivity;
static FAP20M_ALL_SERDES_STATUS global_all_serdes_status;

/*****************************************************
*NAME
*  fap20m_get_links_status
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure gets device status, per link,
*  for all links.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    ALL_LINK_STATUS* all_link_status -
*      Pointer to buffer for this procedure to store
*      status and information on all links in.
*    unsigned long    list_of_links[1] -
*      Bitmap. Indication on which links to monitor and
*      get status of. 
*      Bit 0 (lsb) ==> First link.
*      Bit 1       ==> Second link.
*      ...
*      Bit 8       ==> Nine'th link.
*      If a bit is set then it requires monitoring.
*
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
*    all_link_status.
*REMARKS:
*  
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_links_status
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long list_of_links;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    list_of_links = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_get_links_status(device_id, &global_all_link_status, list_of_links);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = global_all_link_status.error_in_some;
    inFields[1] = global_all_link_status.single_link_status[tbl_index].valid;
    inFields[2] = global_all_link_status.single_link_status[tbl_index].leaky_bucket_counter;
    inFields[3] = global_all_link_status.single_link_status[tbl_index].crc_error;
    inFields[4] = global_all_link_status.single_link_status[tbl_index].misalignment_error;
    inFields[5] = global_all_link_status.single_link_status[tbl_index].code_group_error;
    inFields[6] = global_all_link_status.single_link_status[tbl_index].rx_tdm_hdr_crc_error;
    inFields[7] = global_all_link_status.single_link_status[tbl_index].rx_disparity_error;
    inFields[8] = global_all_link_status.single_link_status[tbl_index].accepting_cells;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                      inFields[3], inFields[4], inFields[5],
                                      inFields[6], inFields[7], inFields[8]);
    
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_get_links_status_end
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

    if (++tbl_index >= FAP20M_NUMBER_OF_LINKS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = global_all_link_status.error_in_some;
    inFields[1] = global_all_link_status.single_link_status[tbl_index].valid;
    inFields[2] = global_all_link_status.single_link_status[tbl_index].leaky_bucket_counter;
    inFields[3] = global_all_link_status.single_link_status[tbl_index].crc_error;
    inFields[4] = global_all_link_status.single_link_status[tbl_index].misalignment_error;
    inFields[5] = global_all_link_status.single_link_status[tbl_index].code_group_error;
    inFields[6] = global_all_link_status.single_link_status[tbl_index].rx_tdm_hdr_crc_error;
    inFields[7] = global_all_link_status.single_link_status[tbl_index].rx_disparity_error;
    inFields[8] = global_all_link_status.single_link_status[tbl_index].accepting_cells;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                      inFields[3], inFields[4], inFields[5],
                                      inFields[6], inFields[7], inFields[8]);
    
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_connectivity_map
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure gets connectivity map, per link,
*  for all links.
*INPUT:
*  DIRECT:
*    unsigned int                  device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_ALL_LINK_CONNECTIVITY* all_link_connectivity-
*      Pointer to buffer for this procedure to store
*      status and information on all links in.
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
*    all_link_connectivity
*REMARKS:
*  
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_connectivity_map
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
    result = fap20m_get_connectivity_map(device_id, &global_all_link_connectivity);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = global_all_link_connectivity.change_in_some;
    inFields[1] = global_all_link_connectivity.error_in_some;

    inFields[2] = global_all_link_connectivity.single_link_connectivity[tbl_index].accepting_cells;
    inFields[3] = global_all_link_connectivity.single_link_connectivity[tbl_index].other_chip_id;
    inFields[4] = global_all_link_connectivity.single_link_connectivity[tbl_index].other_entity_type;
    inFields[5] = global_all_link_connectivity.single_link_connectivity[tbl_index].other_entity_illegal;
    inFields[6] = global_all_link_connectivity.single_link_connectivity[tbl_index].other_link;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                  inFields[4], inFields[5], inFields[6]);
    
    galtisOutput(outArgs, rc, "%f");
    
    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_get_connectivity_map_end
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

    if (++tbl_index >= FAP20M_NUMBER_OF_LINKS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = global_all_link_connectivity.change_in_some;
    inFields[1] = global_all_link_connectivity.error_in_some;

    inFields[2] = global_all_link_connectivity.single_link_connectivity[tbl_index].accepting_cells;
    inFields[3] = global_all_link_connectivity.single_link_connectivity[tbl_index].other_chip_id;
    inFields[4] = global_all_link_connectivity.single_link_connectivity[tbl_index].other_entity_type;
    inFields[5] = global_all_link_connectivity.single_link_connectivity[tbl_index].other_entity_illegal;
    inFields[6] = global_all_link_connectivity.single_link_connectivity[tbl_index].other_link;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                  inFields[4], inFields[5], inFields[6]);
    
    galtisOutput(outArgs, GT_OK, "%f");
    
    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_serdes_params
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure gets status of specified serdes
*  elements, per link, for all specified links.
*INPUT:
*  DIRECT:
*    unsigned int               device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_ALL_SERDES_STATUS*  all_serdes_status -
*      On input:
*        Pointer to buffer for this procedure to
*        get indication on which serdes elements
*        to get status of:
*        If the 'valid' element is non zero then
*        information is required for this link.
*      On output:
*        Pointer to buffer for this procedure to store
*        status and information on all serdes elements.
*  INDIRECT:
*    result_ptr.
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
*    all_serdes_status
*REMARKS:
*  
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_serdes_params
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
    result = fap20m_get_serdes_params(device_id, &global_all_serdes_status);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = global_all_serdes_status.single_serdes_status[tbl_index].valid  ;
    inFields[2] = global_all_serdes_status.single_serdes_status[tbl_index].amp    ;
    inFields[3] = global_all_serdes_status.single_serdes_status[tbl_index].pen    ;
    inFields[4] = global_all_serdes_status.single_serdes_status[tbl_index].vcms   ;
    inFields[5] = global_all_serdes_status.single_serdes_status[tbl_index].txreg_i;
    inFields[6] = global_all_serdes_status.single_serdes_status[tbl_index].isel_eq ;
    inFields[7] = global_all_serdes_status.single_serdes_status[tbl_index].sel_zero;
    inFields[8] = global_all_serdes_status.single_serdes_status[tbl_index].int_7b  ;
    inFields[9] = global_all_serdes_status.single_serdes_status[tbl_index].stop_int;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);
    
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_get_serdes_params_end
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

    if (++tbl_index >= FAP20M_NUMBER_OF_LINKS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = global_all_serdes_status.single_serdes_status[tbl_index].valid  ;
    inFields[2] = global_all_serdes_status.single_serdes_status[tbl_index].amp    ;
    inFields[3] = global_all_serdes_status.single_serdes_status[tbl_index].pen    ;
    inFields[4] = global_all_serdes_status.single_serdes_status[tbl_index].vcms   ;
    inFields[5] = global_all_serdes_status.single_serdes_status[tbl_index].txreg_i;
    inFields[6] = global_all_serdes_status.single_serdes_status[tbl_index].isel_eq ;
    inFields[7] = global_all_serdes_status.single_serdes_status[tbl_index].sel_zero;
    inFields[8] = global_all_serdes_status.single_serdes_status[tbl_index].int_7b  ;
    inFields[9] = global_all_serdes_status.single_serdes_status[tbl_index].stop_int;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);
    
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_serdes_params
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure sets parameters of specified serdes
*  elements, per link, for all specified links.
*INPUT:
*  DIRECT:
*    unsigned int              device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_ALL_SERDES_STATUS* all_serdes_status -
*      Pointer to buffer for this procedure to
*      get new parameters for serdes elements:
*      If the 'valid' element is non zero then
*      serdes on this link is required to change
*      its parameters as specified.
*  INDIRECT:
*    all_serdes_status.
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
*    None
*REMARKS:
*  
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_first_serdes_params
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

    sand_os_memset((void*)&global_all_serdes_status, 0, sizeof(global_all_serdes_status));

    /* map input arguments to locals */
    index = (unsigned int) inFields[0];

    global_all_serdes_status.single_serdes_status[index].valid = (unsigned int)inFields[1];
    global_all_serdes_status.single_serdes_status[index].amp    = (unsigned int)inFields[2];
    global_all_serdes_status.single_serdes_status[index].pen    = (unsigned int)inFields[3];
    global_all_serdes_status.single_serdes_status[index].vcms   = (unsigned int)inFields[4];
    global_all_serdes_status.single_serdes_status[index].txreg_i= (unsigned int)inFields[5];
    global_all_serdes_status.single_serdes_status[index].isel_eq = (unsigned int)inFields[6];
    global_all_serdes_status.single_serdes_status[index].sel_zero= (unsigned int)inFields[7];
    global_all_serdes_status.single_serdes_status[index].int_7b  = (unsigned int)inFields[8];
    global_all_serdes_status.single_serdes_status[index].stop_int= (unsigned int)inFields[9];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_set_next_serdes_params
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
    index = (unsigned int) inFields[0];

    global_all_serdes_status.single_serdes_status[index].valid   = (unsigned int)inFields[1];
    global_all_serdes_status.single_serdes_status[index].amp     = (unsigned int)inFields[2];
    global_all_serdes_status.single_serdes_status[index].pen     = (unsigned int)inFields[3];
    global_all_serdes_status.single_serdes_status[index].vcms    = (unsigned int)inFields[4];
    global_all_serdes_status.single_serdes_status[index].txreg_i = (unsigned int)inFields[5];
    global_all_serdes_status.single_serdes_status[index].isel_eq = (unsigned int)inFields[6];
    global_all_serdes_status.single_serdes_status[index].sel_zero= (unsigned int)inFields[7];
    global_all_serdes_status.single_serdes_status[index].int_7b   = (unsigned int)inFields[8];
    global_all_serdes_status.single_serdes_status[index].stop_int = (unsigned int)inFields[9];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_end_set_serdes_params
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

    /* call Ocelot API function */
    result = fap20m_set_serdes_params(device_id, &global_all_serdes_status);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_serdes_power_down
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This procedure sets parameter of specified serdes elements,
*  Power down/up.
*  Note: The links are coupled, i.e., links 0-1, 2-3,…, 22-23 are
*  powered up-down together.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() 
*    unsigned int     link_id -
*       One of 24 links.
*    unsigned int     indication  -
*       1 - Power down
*       0 - Power up.
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
static CMD_STATUS wrFap20m_set_serdes_power_down
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int link_id;
    unsigned int indication;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_id = (unsigned int)inArgs[1];
    indication = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_set_serdes_power_down(device_id, link_id, indication);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_reset_serdes
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This procedure resets specified SerDes.
*  Note: The links are coupled, i.e., links 0-1, 2-3,…, 22-23 are
*  powered up-down together.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int     link_id -
*       One of 24 links.
*    unsigned int     indication  -
*       In reset is 1 - out of reset is 0.
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
static CMD_STATUS wrFap20m_reset_serdes
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int link_id;
    unsigned int indication;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_id = (unsigned int)inArgs[1];
    indication = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_reset_serdes(device_id, link_id, indication);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_reset_serdes_pll
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This procedure resets specified SerDes PLL.
*  Note: The links are coupled, i.e., links 0-1, 2-3,…, 22-23 are
*  powered up-down together.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int     link_id -
*       One of 24 links.
*    unsigned int     indication  -
*       In reset is 1 - out of reset is 0.
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
static CMD_STATUS wrFap20m_reset_serdes_pll
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int link_id;
    unsigned int indication;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_id = (unsigned int)inArgs[1];
    indication = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_reset_serdes_pll(device_id, link_id, indication);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_link_plane_ndx
*TYPE:
*  PROC
*DATE: 
*  11-Jul-04
*FUNCTION:
*  Set the link on which the device promotes itself with (FAP-Id+1).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int     link_id   -
*       One of 24 links, range 0:23
*    unsigned int     link_plane_ndx   -
*       1 if the device should promotes FAP-Id+1 on this link.
*       0 if the device should promotes FAP-Id on this link.
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
static CMD_STATUS wrFap20m_set_link_plane_ndx
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int link_id;
    unsigned int link_plane_ndx;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_id = (unsigned int)inArgs[1];
    link_plane_ndx = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_set_link_plane_ndx(device_id, link_id, link_plane_ndx);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_link_plane_ndx
*TYPE:
*  PROC
*DATE: 
*  11-Jul-04
*FUNCTION:
*  Get the link on which the device promotes itself with (FAP-Id+1).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int     link_id   -
*       One of 24 links, range 0:23
*    unsigned int*    link_plane_ndx   -
*       Loaded with 0/1, which indicates:
*       1 if the device should promotes FAP-Id+1 on this link.
*       0 if the device should promotes FAP-Id on this link.
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
static CMD_STATUS wrFap20m_get_link_plane_ndx
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int link_id;
    unsigned int link_plane_ndx;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_get_link_plane_ndx(device_id, link_id, &link_plane_ndx);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "%d", link_plane_ndx);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_links_per_plane
*TYPE:
*  PROC
*DATE: 
*  11-Jul-04
*FUNCTION:
*  Set the number of links per plane the destination FAP is using.
*  Which indicates the number of Fap-Ids to use when transmitting
*  toward that destination FAP.
*  Note: Do not change the links-per-plane for 'FAP-X' at 'FAP-X'.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int     spatial   -
*      Indicator.
*      TRUE - 'member_id' is Multicast Group Id.
*      FALSE- 'member_id' is FAP Id.
*    unsigned int     member_id    -
*      If Multicast Group Id, range 0:2047
*      If destination Fap-Id, range 0:2047
*    unsigned int     links_per_plane   -
*      Indicates the number of Fap-Ids to use when transmitting toward
*      that destination FAP.
*      1 - One link per plane.
*      2 - Two link per plane.
*      4 - Four link per plane.
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
static CMD_STATUS wrFap20m_set_links_per_plane
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int spatial;
    unsigned int member_id;
    unsigned int links_per_plane;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    spatial = (unsigned int)inArgs[1];
    member_id = (unsigned int)inArgs[2];
    links_per_plane = (unsigned int)inArgs[3];

    /* call Ocelot API function */
    result = fap20m_set_links_per_plane(device_id, spatial, member_id, links_per_plane);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_links_per_plane
*TYPE:
*  PROC
*DATE: 
*  11-Jul-04
*FUNCTION:
*  Get the number of links per plane the destination FAP is using.
*  Which indicates the number of Fap-Ids to use when transmitting
*  toward that destination FAP.
*  Note: Do not change the links-per-plane for 'FAP-X' at 'FAP-X'.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int     spatial   -
*      Indicator.
*      TRUE - 'member_id' is Multicast Group Id.
*      FALSE- 'member_id' is FAP Id.
*    unsigned int     member_id    -
*      If Multicast Group Id, range 0:2047
*      If destination Fap-Id, range 0:2047
*    unsigned int*     links_per_plane   -
*       Loaded with the number of Fap-Ids to use when transmitting toward
*       that destination FAP.
*       1 - One link per plane.
*       2 - Two link per plane.
*       4 - Four link per plane.
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
static CMD_STATUS wrFap20m_get_links_per_plane
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int spatial;
    unsigned int member_id;
    unsigned int links_per_plane;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    spatial = (unsigned int)inArgs[1];
    member_id = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_get_links_per_plane(device_id, spatial, member_id, &links_per_plane);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "%d", links_per_plane);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_fabric_link_flow_control_set
*TYPE:
*  PROC
*DATE:
*  18-May-05
*FUNCTION:
*  This procedure sets fabric link flow-control
*INPUT:
*  DIRECT:
*    unsigned int     device_id  -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_FABRIC_LINK_FC* link_fc -
*      Pointer to 'FAP20M_FABRIC_LINK_FC' structure.
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
*    None
*REMARKS:
*  This function works only on REV-B or above.
*  Device must have been registered for this action to be
*   carried out properly.
*  System parameter: The FE that the link is connected to
*   must also be from revB or above, and support the 
*   mechanism.
*  Affecting the device registers: LnkLvlFcRxEn and LnkLvlFcTxEn.
*SEE ALSO:
*   FAP20M_FABRIC_LINK_FC
*****************************************************/
static CMD_STATUS wrFap20m_fabric_link_flow_control_set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_FABRIC_LINK_FC link_fc;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    link_fc.rx_enable = (unsigned int)inFields[0];
    link_fc.tx_enable = (unsigned int)inFields[1];

    /* call Ocelot API function */
    result = fap20m_fabric_link_flow_control_set(device_id, &link_fc);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_fabric_link_flow_control_get
*TYPE:
*  PROC
*DATE:
*  18-May-05
*FUNCTION:
*  This procedure gets fabric link flow-control
*INPUT:
*  DIRECT:
*    unsigned int     device_id  -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_FABRIC_LINK_FC* link_fc -
*      Pointer to 'FAP20M_FABRIC_LINK_FC' structure.
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
*    None
*REMARKS:
*  This function works only on REV-B or above.
*  Device must have been registered for this action to be
*   carried out properly.
*  System parameter: The FE that the link is connected to
*   must also be from revB or above, and support the 
*   mechanism.
*  Affecting the device registers: LnkLvlFcRxEn and LnkLvlFcTxEn.
*SEE ALSO:
*   FAP20M_FABRIC_LINK_FC
*****************************************************/
static CMD_STATUS wrFap20m_fabric_link_flow_control_get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_FABRIC_LINK_FC link_fc;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_fabric_link_flow_control_get(device_id, &link_fc);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = link_fc.rx_enable;
    inFields[1] = link_fc.tx_enable;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_fabric_link_flow_control_get_end
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

#if FAP20M_DEBUG
/*****************************************************
*NAME
*  fap20m_links_TEST
*TYPE:
*  PROC
*DATE: 
*  7-Jul-04
*FUNCTION:
*  Testing utility.
*INPUT:
*  DIRECT:
*    None
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    TRUE iff test pass.
*  INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_links_TEST
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int silent;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    silent = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_links_TEST(device_id, silent);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
#endif /* FAP20M_DEBUG */


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"links_statusGetFirst",
         &wrFap20m_get_links_status,
         2, 0},
        {"links_statusGetNext",
         &wrFap20m_get_links_status_end,
         2, 0},

        {"connectivity_mapGetFirst",
         &wrFap20m_get_connectivity_map,
         1, 0},
        {"connectivity_mapGetNext",
         &wrFap20m_get_connectivity_map_end,
         1, 0},

        {"serdes_paramsSetFirst",
         &wrFap20m_set_first_serdes_params,
         0, 8},
        {"serdes_paramsSetNext",
         &wrFap20m_set_next_serdes_params,
         0, 8},
        {"serdes_paramsEndSet",
         &wrFap20m_end_set_serdes_params,
         1, 0},
        {"serdes_paramsGetFirst",
         &wrFap20m_get_serdes_params,
         1, 0},
        {"serdes_paramsGetNext",
         &wrFap20m_get_serdes_params_end,
         1, 0},
        
        {"set_serdes_power_down",
         &wrFap20m_set_serdes_power_down,
         3, 0},
        {"reset_serdes",
         &wrFap20m_reset_serdes,
         3, 0},
        {"reset_serdes_pll",
         &wrFap20m_reset_serdes_pll,
         3, 0},
        {"set_link_plane_ndx",
         &wrFap20m_set_link_plane_ndx,
         3, 0},
        {"get_link_plane_ndx",
         &wrFap20m_get_link_plane_ndx,
         2, 0},
        {"set_links_per_plane",
         &wrFap20m_set_links_per_plane,
         4, 0},
        {"get_links_per_plane",
         &wrFap20m_get_links_per_plane,
         3, 0},

        {"fabric_link_flow_controlSet",
         &wrFap20m_fabric_link_flow_control_set,
         1, 2},
        {"fabric_link_flow_controlGetFirst",
         &wrFap20m_fabric_link_flow_control_get,
         1, 0},
        {"fabric_link_flow_controlGetNext",
         &wrFap20m_fabric_link_flow_control_get_end,
         1, 0}
#if FAP20M_DEBUG
        ,{"links_TEST",
         &wrFap20m_links_TEST,
         2, 0}
#endif /* FAP20M_DEBUG */
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_links
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
GT_STATUS cmdLibInitFap20m_api_links
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

