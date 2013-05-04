/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_auto_management.c
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

/* Common galtis includes */
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>

/* Feature specific includes. */
#include <SAND/Utils/include/sand_framework.h>
#include <SAND/Utils/include/sand_os_interface.h>
#include <FX950/include/fap20m_api_auto_management.h>
#include <FX950/include/fap20m_auto_aggregate.h>
#include <FX950/include/fap20m_auto_queue.h>
#include <FX950/include/fap20m_auto_port.h>
#include <FX950/include/fap20m_auto_aggregate.h>
#include <FX950/include/fap20m_auto_flow.h>


/* Global variables */
static GT_U32   tbl_index;
static FAP20M_AUTO_AGG_INFO        *p_aggregates_info;
static FAP20M_AUTO_AGG_INFO        *p_aggregates_info_to_fill;
static FAP20M_AUTO_AGG_INFO        *exact_aggregates_info;
static FAP20M_AUTO_FLOW_INFO       *p_flows_info_to_fill;
static FAP20M_AUTO_FLOW_INFO       *exact_flows_info;
static FAP20M_AUTO_FLOW_INFO       *p_flows_info;
static FAP20M_AUTO_PORT_INFO       *exact_port_info;
static FAP20M_AUTO_AGGREGATE_SW_DB *sw_db_module_buff_to_fill;
static FAP20M_AUTO_AGGREGATE_SW_DB *sw_db_module_buff;
static FAP20M_AUTO_FLOW_SW_DB      *sw_db_flow_module_buff_to_fill;
static FAP20M_AUTO_FLOW_SW_DB      *sw_db_flow_module_buff;
static FAP20M_AUTO_PORT_SW_DB      *sw_db_port_module_buff;
static FAP20M_AUTO_QUEUE_SW_DB     *sw_db_queue_module_buff_to_fill;
static FAP20M_AUTO_QUEUE_SW_DB     *sw_db_queue_module_buff;


/*****************************************************
*NAME
*  fap20m_api_auto_system_info_get_defaults
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*   Each structure in the auto management system has a function
*   fap20m_api_auto_XXX_defaults().
*   This function will:
*   1. Clean the structure;
*   2. Set default values to parameters that have logical common values.
*   3. Set invalid values to parameters that have to be configured by
*      user.
*   
*   The function fap20m_api_auto_system_info_get_defaults() 
*    might be called before calling to fap20m_api_auto_init().
*   The function get the structure FAP20M_AUTO_SYSTEM_INFO
*    as parameter, and fill it with some default values.
*INPUT:
*  DIRECT:
*    None.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_SYSTEM_INFO *p_auto_info -
*      Returned with default parameter to be later used 
*       to call fap20m_api_auto_init()
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_init()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_system_info_get_defaults
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    FAP20M_AUTO_SYSTEM_INFO p_auto_info;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_api_auto_system_info_get_defaults(&p_auto_info);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = p_auto_info.max_nof_faps_in_system;
    inFields[1] = p_auto_info.max_nof_ports_per_fap;
    inFields[2] = p_auto_info.is_fap10v;
    inFields[3] = p_auto_info.max_nof_sub_flows_per_flow;
    inFields[4] = p_auto_info.nof_fabric_classes;
    inFields[5] = p_auto_info.auto_first_queue_type;
    inFields[6] = p_auto_info.auto_last_queue_type;
    inFields[7] = p_auto_info.nof_faps_per_CPU;
    inFields[8] = p_auto_info.device_info[0].local_fap_id;
    inFields[9] = p_auto_info.device_info[1].local_fap_id;
    inFields[10] = p_auto_info.device_info[2].local_fap_id;
    inFields[11] = p_auto_info.device_info[3].local_fap_id;
    inFields[12] = p_auto_info.device_info[4].local_fap_id;
    inFields[13] = p_auto_info.device_info[5].local_fap_id;
    inFields[14] = p_auto_info.device_info[6].local_fap_id;
    inFields[15] = p_auto_info.device_info[7].local_fap_id;

    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", inFields[0],  inFields[1],  inFields[2],  inFields[3],
                                                    inFields[4],  inFields[5],  inFields[6],  inFields[7],
                                                    inFields[8],  inFields[9],  inFields[10], inFields[11],
                                                    inFields[12], inFields[13], inFields[14], inFields[15]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_system_info_get_defaults_end
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
*  fap20m_api_auto_init 
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  The function fap20m_api_auto_init(), set all the 
*   information that the fap20m_api_auto module needs to  
*   know to enable mapping of logical IDs to physical IDs    
*   and the other way around.      
*INPUT:
*  DIRECT:
*    IN FAP20M_AUTO_SYSTEM_INFO *p_auto_info -
*      The structure would be set to the auto management module.
*      Contain system information.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*     None.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_system_info_get_defaults()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_init
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    FAP20M_AUTO_SYSTEM_INFO p_auto_info;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    p_auto_info.max_nof_faps_in_system  = (unsigned int) inFields[0];
    p_auto_info.max_nof_ports_per_fap   = (unsigned int) inFields[1];
    p_auto_info.is_fap10v               = (unsigned int) inFields[2];
    p_auto_info.max_nof_sub_flows_per_flow = (unsigned int) inFields[3];
    p_auto_info.nof_fabric_classes      = (FAP20M_FABRIC_CLASS) inFields[4];
    p_auto_info.auto_first_queue_type   = (FAP20M_QUEUE_TYPE) inFields[5];
    p_auto_info.auto_last_queue_type    = (FAP20M_QUEUE_TYPE) inFields[6];
    p_auto_info.nof_faps_per_CPU        = (unsigned int) inFields[7];

    p_auto_info.device_info[0].local_fap_id = (unsigned int) inFields[8];
    p_auto_info.device_info[1].local_fap_id = (unsigned int) inFields[9];
    p_auto_info.device_info[2].local_fap_id = (unsigned int) inFields[10];
    p_auto_info.device_info[3].local_fap_id = (unsigned int) inFields[11];
    p_auto_info.device_info[4].local_fap_id = (unsigned int) inFields[12];
    p_auto_info.device_info[5].local_fap_id = (unsigned int) inFields[13];
    p_auto_info.device_info[6].local_fap_id = (unsigned int) inFields[14];
    p_auto_info.device_info[7].local_fap_id = (unsigned int) inFields[15];

    p_auto_info.fabric_sch_rank = (FAP20M_SCH_RANK) inFields[16];

    /* call Ocelot API function */
    result = fap20m_api_auto_init(&p_auto_info);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_port_get_default
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*   Each structure in the auto management system has a function
*   fap20m_api_auto_XXX_defaults().
*   This function will:
*   1. Clean the structure;
*   2. Set default values to parameters that have logical common values.
*   3. Set invalid values to parameters that have to be configured by
*      user.
*   
*   The function get the structure FAP20M_AUTO_PORT_INFO
*    as parameter, and fill it with some default values.
*INPUT:
*  DIRECT:
*    IN  unsigned long           nominal_rate -
*      The function gets the nominal rate and and set 
*       default values to some other fields as well,
*       according to the nominal rate value.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_PORT_INFO *p_port_info -
*      Returned with default parameter to be later used 
*       to call the auto ports functions.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_set_info_and_open_all_ports()
*   fap20m_api_auto_update_port()
*   fap20m_api_auto_close_port()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_port_get_default
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    FAP20M_AUTO_PORT_INFO p_port_info;
    unsigned long nominal_rate;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    nominal_rate = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_api_auto_port_get_default(&p_port_info, nominal_rate);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = p_port_info.eg_port.valid_mask;
    inFields[1] = p_port_info.eg_port.nominal_bandwidth;
    inFields[2] = p_port_info.eg_port.credit_bandwidth;
    inFields[3] = p_port_info.hr_mode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_port_get_default_end
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
*  fap20m_api_auto_set_info_and_open_all_ports
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  The function should be called at the initialization phase
*  The function will set the FAP20M_AUTO_PORT_INFO as the
*   information of all the ports, and will also open all the 
*   ports in the system according to that 
*
*INPUT:
*  DIRECT:
*    IN FAP20M_AUTO_PORT_INFO *p_port_info -
*      The port information to be used for all the ports
*        on the card (CPU).
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*     None.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_port_get_default()
*   fap20m_api_auto_update_port()
*   fap20m_api_auto_close_port()
*   fap20m_api_auto_port_get_info()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_set_info_and_open_all_ports
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    FAP20M_AUTO_PORT_INFO p_port_info;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    p_port_info.eg_port.valid_mask        = (unsigned long)inFields[0];
    p_port_info.eg_port.nominal_bandwidth = (unsigned long)inFields[1];
    p_port_info.eg_port.credit_bandwidth  = (unsigned long)inFields[2];
    p_port_info.hr_mode                   = (FAP20M_HR_SCHED_MODE)inFields[3];

    /* call Ocelot API function */
    result = fap20m_api_auto_set_info_and_open_all_ports(&p_port_info);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_auto_port_set_info
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    FAP20M_AUTO_PORT_INFO p_port_info;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];

    p_port_info.eg_port.valid_mask        = (unsigned long)inFields[0];
    p_port_info.eg_port.nominal_bandwidth = (unsigned long)inFields[1];
    p_port_info.eg_port.credit_bandwidth  = (unsigned long)inFields[2];
    p_port_info.hr_mode                   = (FAP20M_HR_SCHED_MODE)inFields[3];

    /* call Ocelot api function */
    result = fap20m_auto_port_set_info(device_id, port_id, &p_port_info);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_port_get_info
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  The function should be after calling to:
*    fap20m_api_auto_set_info_and_open_all_ports(),
*   and before calling to:
*    fap20m_api_auto_update_port().
*
*  The function will get the FAP20M_AUTO_PORT_INFO 
*    of a physical port.
*  The common use of this information, is to be 
*    modified and than set to the auto management and 
*    to device with the function:
*     fap20m_api_auto_update_port().
*
*INPUT:
*  DIRECT:
*    IN  unsigned int            device_id - 
*       The device ID of the physical port to be read.
*    IN  unsigned int            port_id,
*       The physical port ID to be read.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_PORT_INFO *p_port_info -
*      Returned with the information currently set
*       to the physical port.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_update_port()
*   fap20m_api_auto_close_port()
*   fap20m_api_auto_set_info_and_open_all_ports()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_port_get_info
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    FAP20M_AUTO_PORT_INFO p_port_info;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id   = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_api_auto_port_get_info(device_id, port_id, &p_port_info);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = p_port_info.eg_port.valid_mask;
    inFields[1] = p_port_info.eg_port.nominal_bandwidth;
    inFields[2] = p_port_info.eg_port.credit_bandwidth;
    inFields[3] = p_port_info.hr_mode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_port_get_info_end
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
*  fap20m_api_auto_update_port
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  The function should be after calling to:
*    fap20m_api_auto_set_info_and_open_all_ports(),
*
*  The function will update the FAP20M_AUTO_PORT_INFO 
*    to the auto management module and to device.
*
*INPUT:
*  DIRECT:
*    IN  unsigned int            device_id - 
*       The device ID of the physical port to be updated.
*    IN  unsigned int            port_id -
*       The physical port ID to be updated.
*    IN  FAP20M_AUTO_PORT_INFO *port_info -
*       The information to be set to the physical port.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_PORT_INFO *exact_port_info -
*      Returned with the exact information set
*       to the physical port.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_update_port()
*   fap20m_api_auto_close_port()
*   fap20m_api_auto_port_get_info()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_update_port
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    FAP20M_AUTO_PORT_INFO p_port_info;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id   = (unsigned int)inArgs[1];

    p_port_info.eg_port.valid_mask        = (unsigned long)inFields[0];
    p_port_info.eg_port.nominal_bandwidth = (unsigned long)inFields[1];
    p_port_info.eg_port.credit_bandwidth  = (unsigned long)inFields[2];
    p_port_info.hr_mode                   = (FAP20M_HR_SCHED_MODE)inFields[3];

    /* call Ocelot API function */
    result = fap20m_api_auto_update_port(device_id, port_id, &p_port_info);

    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_api_auto_update_port_exact
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

    /* pack output arguments to galtis string */
    inFields[0] = exact_port_info->eg_port.valid_mask;
    inFields[1] = exact_port_info->eg_port.nominal_bandwidth;
    inFields[2] = exact_port_info->eg_port.credit_bandwidth;
    inFields[3] = exact_port_info->hr_mode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_update_port_exact_end
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
*  fap20m_api_auto_close_port
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  The function should be after calling to:
*    fap20m_api_auto_set_info_and_open_all_ports(),
*
*  The function close physical port.
*
*INPUT:
*  DIRECT:
*    IN  unsigned int            device_id - 
*       The device ID of the physical port to be closed.
*    IN  unsigned int            port_id -
*       The physical port ID to be closed.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*     None.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_update_port()
*   fap20m_api_auto_port_get_info()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_close_port
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id   = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_api_auto_close_port(device_id, port_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_aggregates_get_default
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*   Each structure in the auto management system has a function
*   fap20m_api_auto_XXX_defaults().
*   This function will:
*   1. Clean the structure;
*   2. Set default values to parameters that have logical common values.
*   3. Set invalid values to parameters that have to be configured by
*      user.
*   
*   The function get the structure FAP20M_AUTO_AGG_INFO
*    as parameter, and fill it with some default values.
*INPUT:
*  DIRECT:
*    None.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_AGG_INFO *p_port_info -
*      Returned with default parameter to be later used 
*       to call the auto aggs functions.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_set_info_and_open_all_aggregates()
*   fap20m_api_auto_update_port_aggregates()
*   fap20m_api_auto_close_port_aggregates()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_aggregates_get_default
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_api_auto_aggregates_get_default(p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = p_aggregates_info->item[0][0].sch.id;
    inFields[1] = p_aggregates_info->item[0][0].sch.type;
    inFields[2] = p_aggregates_info->item[0][0].sch.state;

    switch (p_aggregates_info->item[0][0].sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = p_aggregates_info->item[0][0].sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = p_aggregates_info->item[0][0].sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }

    inFields[5] = p_aggregates_info->item[0][0].flow.id;

    inFields[6] = p_aggregates_info->item[0][0].flow.sub_flow[0].valid_mask;
    inFields[7] = p_aggregates_info->item[0][0].flow.sub_flow[0].id;
    inFields[8] = p_aggregates_info->item[0][0].flow.sub_flow[0].credit_source;

    inFields[9] = p_aggregates_info->item[0][0].flow.sub_flow[0].shaper.max_rate;
    inFields[10] = p_aggregates_info->item[0][0].flow.sub_flow[0].shaper.max_burst;

    inFields[11] = p_aggregates_info->item[0][0].flow.sub_flow[0].type;

    switch (p_aggregates_info->item[0][0].flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = p_aggregates_info->item[0][0].flow.sub_flow[1].valid_mask;
    inFields[16] = p_aggregates_info->item[0][0].flow.sub_flow[1].id;
    inFields[17] = p_aggregates_info->item[0][0].flow.sub_flow[1].credit_source;

    inFields[18] = p_aggregates_info->item[0][0].flow.sub_flow[1].shaper.max_rate;
    inFields[19] = p_aggregates_info->item[0][0].flow.sub_flow[1].shaper.max_burst;

    inFields[20] = p_aggregates_info->item[0][0].flow.sub_flow[1].type;

    switch (p_aggregates_info->item[0][0].flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = p_aggregates_info->item[0][0].flow.flow_type;
    inFields[25] = p_aggregates_info->item[0][0].flow.is_slow_enabled;

    inFields[26] = p_aggregates_info->item[0][0].valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                                inFields[8],  inFields[9],  inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23],
                                inFields[24], inFields[25], inFields[26]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_aggregates_get_default_end
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

    if (++tbl_index >= FAP20M_AUTO_NOF_HIERARCY * FAP20M_AUTO_NOF_INDEX)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = (**(p_aggregates_info->item + tbl_index)).sch.id;
    inFields[1] = (**(p_aggregates_info->item + tbl_index)).sch.type;
    inFields[2] = (**(p_aggregates_info->item + tbl_index)).sch.state;

    switch ((**(p_aggregates_info->item + tbl_index)).sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = (**(p_aggregates_info->item + tbl_index)).sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = (**(p_aggregates_info->item + tbl_index)).sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }

    inFields[5] = (**(p_aggregates_info->item + tbl_index)).flow.id;

    inFields[6] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].valid_mask;
    inFields[7] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].id;
    inFields[8] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].credit_source;

    inFields[9] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].shaper.max_rate;
    inFields[10] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].shaper.max_burst;

    inFields[11] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].type;

    switch ((**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].valid_mask;
    inFields[16] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].id;
    inFields[17] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].credit_source;

    inFields[18] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].shaper.max_rate;
    inFields[19] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].shaper.max_burst;

    inFields[20] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].type;

    switch ((**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = (**(p_aggregates_info->item + tbl_index)).flow.flow_type;
    inFields[25] = (**(p_aggregates_info->item + tbl_index)).flow.is_slow_enabled;

    inFields[26] = (**(p_aggregates_info->item + tbl_index)).valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                                inFields[8],  inFields[9],  inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23],
                                inFields[24], inFields[25], inFields[26]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_set_info_and_open_all_aggregates
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  The function should be called at the initialization phase
*  The function will set the FAP20M_AUTO_AGG_INFO[class_i] as the
*   information of all the aggs of class_i, and will also open all the 
*   aggs in the system according to that 
*
*INPUT:
*  DIRECT:
*    IN FAP20M_AUTO_AGG_INFO *p_agg_info -
*      The port information to be used for all the ports
*        on the card (CPU).
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*     None.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_agg_get_default()
*   fap20m_api_auto_port_aggregates_get_info()
*   fap20m_api_auto_update_port_aggregates()
*   fap20m_api_auto_close_port_aggregates()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_set_info_and_open_all_aggregates
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    FAP20M_AUTO_AGG_INFO p_aggregates_info;
    unsigned short rc;
    unsigned int index;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    index = inFields[0];

    (**(p_aggregates_info.item + index)).sch.id    = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[1];
    (**(p_aggregates_info.item + index)).sch.type  = (FAP20M_ELEMENTARY_SCHEDULER_TYPE) inFields[2];
    (**(p_aggregates_info.item + index)).sch.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE) inFields[3];

    switch ((**(p_aggregates_info.item + index)).sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            (**(p_aggregates_info.item + index)).sch.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE) inFields[4];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            (**(p_aggregates_info.item + index)).sch.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE) inFields[5];
            break;
        default: break;
    }

    (**(p_aggregates_info.item + index)).flow.id = (FAP20M_FLOW_ID) inFields[6];

    (**(p_aggregates_info.item + index)).flow.sub_flow[0].valid_mask = (unsigned long) inFields[7];
    (**(p_aggregates_info.item + index)).flow.sub_flow[0].id = (FAP20M_SUB_FLOW_ID) inFields[8];
    (**(p_aggregates_info.item + index)).flow.sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[9];

    (**(p_aggregates_info.item + index)).flow.sub_flow[0].shaper.max_rate = (unsigned long) inFields[10];
    (**(p_aggregates_info.item + index)).flow.sub_flow[0].shaper.max_burst = (unsigned long) inFields[11];

    (**(p_aggregates_info.item + index)).flow.sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE) inFields[12];

    switch ((**(p_aggregates_info.item + index)).flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            (**(p_aggregates_info.item + index)).flow.sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[13];
            (**(p_aggregates_info.item + index)).flow.sub_flow[0].subflow_properties.hr.weight = (unsigned long) inFields[15];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            (**(p_aggregates_info.item + index)).flow.sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[15];
            (**(p_aggregates_info.item + index)).flow.sub_flow[0].subflow_properties.cl.weight = (unsigned long) inFields[15];
            break;
        default: break;
    }

    (**(p_aggregates_info.item + index)).flow.sub_flow[1].valid_mask = (unsigned long) inFields[16];
    (**(p_aggregates_info.item + index)).flow.sub_flow[1].id = (FAP20M_SUB_FLOW_ID) inFields[17];
    (**(p_aggregates_info.item + index)).flow.sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[18];

    (**(p_aggregates_info.item + index)).flow.sub_flow[1].shaper.max_rate = (unsigned long) inFields[19];
    (**(p_aggregates_info.item + index)).flow.sub_flow[1].shaper.max_burst = (unsigned long) inFields[20];
    
    (**(p_aggregates_info.item + index)).flow.sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE) inFields[21];

    switch ((**(p_aggregates_info.item + index)).flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            (**(p_aggregates_info.item + index)).flow.sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[22];
            (**(p_aggregates_info.item + index)).flow.sub_flow[1].subflow_properties.hr.weight = (unsigned long) inFields[24];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            (**(p_aggregates_info.item + index)).flow.sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[23];
            (**(p_aggregates_info.item + index)).flow.sub_flow[1].subflow_properties.cl.weight = (unsigned long) inFields[24];
            break;
        default: break;
    }

    (**(p_aggregates_info.item + index)).flow.flow_type = (FAP20M_FLOW_TYPE) inFields[25];
    (**(p_aggregates_info.item + index)).flow.is_slow_enabled = (unsigned int) inFields[26];

    (**(p_aggregates_info.item + index)).valid = (unsigned int) inFields[27];

    /* call Ocelot API function */
    result = fap20m_api_auto_set_info_and_open_all_aggregates(&p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_port_aggregates_get_info
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Get the aggs information from the software DB.
*  Might be called before
*  fap20m_api_auto_update_port_aggregates()
*
*INPUT:
*  DIRECT:
*    IN  unsigned int              device_id 
*    IN  unsigned int              port_id -
*      Get the information of all the aggs connected to that port ID.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_AGG_INFO   *p_aggregates_info
*      Returned with the information from SW DB 
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_agg_get_default()
*   fap20m_api_auto_port_aggregates_get_info()
*   fap20m_api_auto_update_port_aggregates()
*   fap20m_api_auto_close_port_aggregates()
*   fap20m_api_auto_set_info_and_open_all_aggregates()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_port_aggregates_get_info
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_api_auto_port_aggregates_get_info(device_id, port_id, p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    tbl_index = 0;

    inFields[0] = p_aggregates_info->item[0][0].sch.id;
    inFields[1] = p_aggregates_info->item[0][0].sch.type;
    inFields[2] = p_aggregates_info->item[0][0].sch.state;

    switch (p_aggregates_info->item[0][0].sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = p_aggregates_info->item[0][0].sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = p_aggregates_info->item[0][0].sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }

    inFields[5] = p_aggregates_info->item[0][0].flow.id;

    inFields[6] = p_aggregates_info->item[0][0].flow.sub_flow[0].valid_mask;
    inFields[7] = p_aggregates_info->item[0][0].flow.sub_flow[0].id;
    inFields[8] = p_aggregates_info->item[0][0].flow.sub_flow[0].credit_source;

    inFields[9] = p_aggregates_info->item[0][0].flow.sub_flow[0].shaper.max_rate;
    inFields[10] = p_aggregates_info->item[0][0].flow.sub_flow[0].shaper.max_burst;

    inFields[11] = p_aggregates_info->item[0][0].flow.sub_flow[0].type;

    switch (p_aggregates_info->item[0][0].flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = p_aggregates_info->item[0][0].flow.sub_flow[1].valid_mask;
    inFields[16] = p_aggregates_info->item[0][0].flow.sub_flow[1].id;
    inFields[17] = p_aggregates_info->item[0][0].flow.sub_flow[1].credit_source;

    inFields[18] = p_aggregates_info->item[0][0].flow.sub_flow[1].shaper.max_rate;
    inFields[19] = p_aggregates_info->item[0][0].flow.sub_flow[1].shaper.max_burst;

    inFields[20] = p_aggregates_info->item[0][0].flow.sub_flow[1].type;

    switch (p_aggregates_info->item[0][0].flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = p_aggregates_info->item[0][0].flow.flow_type;
    inFields[25] = p_aggregates_info->item[0][0].flow.is_slow_enabled;

    inFields[26] = p_aggregates_info->item[0][0].valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                                inFields[8],  inFields[9],  inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23],
                                inFields[24], inFields[25], inFields[26]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_port_aggregates_get_info_end
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

    if (++tbl_index >= FAP20M_AUTO_NOF_HIERARCY * FAP20M_AUTO_NOF_INDEX)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = (**(p_aggregates_info->item + tbl_index)).sch.id;
    inFields[1] = (**(p_aggregates_info->item + tbl_index)).sch.type;
    inFields[2] = (**(p_aggregates_info->item + tbl_index)).sch.state;

    switch ((**(p_aggregates_info->item + tbl_index)).sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = (**(p_aggregates_info->item + tbl_index)).sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = (**(p_aggregates_info->item + tbl_index)).sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }

    inFields[5] = (**(p_aggregates_info->item + tbl_index)).flow.id;

    inFields[6] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].valid_mask;
    inFields[7] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].id;
    inFields[8] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].credit_source;

    inFields[9] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].shaper.max_rate;
    inFields[10] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].shaper.max_burst;

    inFields[11] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].type;

    switch ((**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].valid_mask;
    inFields[16] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].id;
    inFields[17] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].credit_source;

    inFields[18] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].shaper.max_rate;
    inFields[19] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].shaper.max_burst;

    inFields[20] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].type;

    switch ((**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = (**(p_aggregates_info->item + tbl_index)).flow.flow_type;
    inFields[25] = (**(p_aggregates_info->item + tbl_index)).flow.is_slow_enabled;

    inFields[26] = (**(p_aggregates_info->item + tbl_index)).valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                                inFields[8],  inFields[9],  inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23],
                                inFields[24], inFields[25], inFields[26]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_port_aggregates_get_info
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Update aggregates capabilities, to both the software database
*   and to the device.
*  When updating the aggregate and the flow the sequence should
*   be: close flows, close aggregates, open aggregates,
*       open flows.
*  Therefore, updating the aggregate can only take place with 
*    updating the flows at the same time.
*
*INPUT:
*  DIRECT:
*    IN  unsigned int              device_id 
*    IN  unsigned int              port_id -
*      Update the aggs connected to that port ID.
*    IN  FAP20M_AUTO_AGG_INFO   *p_aggregates_info,
*      The new aggs capability.
*    IN  FAP20M_AUTO_FLOW_INFO  *flows_info - 
*      The new flows information.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_AGG_INFO   *exact_aggregates_info
*      Returned with the information from SW DB 
*    OUT FAP20M_AUTO_FLOW_INFO  *exact_flows_info
*      Returned with the information from SW DB 
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_agg_get_default()
*   fap20m_api_auto_port_aggregates_get_info()
*   fap20m_api_auto_update_port_aggregates()
*   fap20m_api_auto_close_port_aggregates()
*   fap20m_api_auto_set_info_and_open_all_aggregates()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_update_port_aggregates
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_api_auto_update_port_aggregates(device_id, port_id, p_aggregates_info_to_fill, exact_aggregates_info,
                                                    p_flows_info_to_fill, exact_flows_info);

    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_api_auto_aggregates_fill_info_table
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_hierarcy_index;
    unsigned int end_hierarcy_index;
    unsigned int start_index;
    unsigned int end_index;
    unsigned int i, j;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    start_hierarcy_index = (unsigned int)inArgs[0];
    end_hierarcy_index = (unsigned int)inArgs[1];
    start_index = (unsigned int)inArgs[2];
    end_index = (unsigned int)inArgs[3];

    for (i = start_hierarcy_index; i <= end_hierarcy_index; i++)
    {
        for (j = start_index; j <= end_index; j++)
        {
            p_aggregates_info_to_fill->item[i][j].sch.id    = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[0];
            p_aggregates_info_to_fill->item[i][j].sch.type  = (FAP20M_ELEMENTARY_SCHEDULER_TYPE) inFields[1];
            p_aggregates_info_to_fill->item[i][j].sch.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE) inFields[2];

            switch (p_aggregates_info_to_fill->item[i][j].sch.type)
            {
                case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
                    p_aggregates_info_to_fill->item[i][j].sch.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE) inFields[3];
                    break;
                case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
                    p_aggregates_info_to_fill->item[i][j].sch.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE) inFields[4];
                    break;
                default: break;
            }

            p_aggregates_info_to_fill->item[i][j].flow.id = (FAP20M_FLOW_ID) inFields[5];

            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].valid_mask = (unsigned long) inFields[6];
            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].id = (FAP20M_SUB_FLOW_ID) inFields[7];
            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[8];

            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].shaper.max_rate = (unsigned long) inFields[9];
            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].shaper.max_burst = (unsigned long) inFields[10];

            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE) inFields[11];

            switch (p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].type)
            {
                case FAP20M_SUB_FLOW_TYPE_HR:
                    p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[12];
                    p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].subflow_properties.hr.weight = (unsigned long) inFields[14];
                    break;
                case FAP20M_SUB_FLOW_TYPE_CL:
                    p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[13];
                    p_aggregates_info_to_fill->item[i][j].flow.sub_flow[0].subflow_properties.cl.weight = (unsigned long) inFields[14];

                    break;
                default: break;
            }

            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].valid_mask = (unsigned long) inFields[15];
            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].id = (FAP20M_SUB_FLOW_ID) inFields[16];
            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[17];

            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].shaper.max_rate = (unsigned long) inFields[18];
            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].shaper.max_burst = (unsigned long) inFields[19];

            p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE) inFields[20];

            switch (p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].type)
            {
                case FAP20M_SUB_FLOW_TYPE_HR:
                    p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[21];
                    p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].subflow_properties.hr.weight = (unsigned long) inFields[23];
                    break;
                case FAP20M_SUB_FLOW_TYPE_CL:
                    p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[22];
                    p_aggregates_info_to_fill->item[i][j].flow.sub_flow[1].subflow_properties.cl.weight = (unsigned long) inFields[23];
                    break;
                default: break;
            }

            p_aggregates_info_to_fill->item[i][j].flow.flow_type = (FAP20M_FLOW_TYPE) inFields[24];
            p_aggregates_info_to_fill->item[i][j].flow.is_slow_enabled = (unsigned int) inFields[25];

            p_aggregates_info_to_fill->item[i][j].valid = (unsigned int) inFields[26];
        }
    }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_api_auto_flow_info_fill_table
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_index;
    unsigned int end_index;
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    start_index = (unsigned int)inArgs[0];
    end_index = (unsigned int)inArgs[1];

    for(i = start_index; i <= end_index; i++)
    {
        p_flows_info_to_fill->flows[i].id = (FAP20M_FLOW_ID) inFields[0];

        p_flows_info_to_fill->flows[i].sub_flow[0].valid_mask = (unsigned long) inFields[1];
        p_flows_info_to_fill->flows[i].sub_flow[0].id = (FAP20M_SUB_FLOW_ID) inFields[2];
        p_flows_info_to_fill->flows[i].sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[3];

        p_flows_info_to_fill->flows[i].sub_flow[0].shaper.max_rate = (unsigned long) inFields[4];
        p_flows_info_to_fill->flows[i].sub_flow[0].shaper.max_burst = (unsigned long) inFields[5];

        p_flows_info_to_fill->flows[i].sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE) inFields[6];

        switch (p_flows_info_to_fill->flows[i].sub_flow[0].type)
        {
            case FAP20M_SUB_FLOW_TYPE_HR:
                p_flows_info_to_fill->flows[i].sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[7];
                p_flows_info_to_fill->flows[i].sub_flow[0].subflow_properties.hr.weight = (unsigned long) inFields[9];
                break;
            case FAP20M_SUB_FLOW_TYPE_CL:
                p_flows_info_to_fill->flows[i].sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[8];
                p_flows_info_to_fill->flows[i].sub_flow[0].subflow_properties.cl.weight = (unsigned long) inFields[9];
                break;
            default: break;
        }

        p_flows_info_to_fill->flows[i].sub_flow[1].valid_mask = (unsigned long) inFields[10];
        p_flows_info_to_fill->flows[i].sub_flow[1].id = (FAP20M_SUB_FLOW_ID) inFields[11];
        p_flows_info_to_fill->flows[i].sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[12];

        p_flows_info_to_fill->flows[i].sub_flow[1].shaper.max_rate = (unsigned long) inFields[13];
        p_flows_info_to_fill->flows[i].sub_flow[1].shaper.max_burst = (unsigned long) inFields[14];

        p_flows_info_to_fill->flows[i].sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE) inFields[15];

        switch (p_flows_info_to_fill->flows[i].sub_flow[1].type)
        {
            case FAP20M_SUB_FLOW_TYPE_HR:
                p_flows_info_to_fill->flows[i].sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[16];
                p_flows_info_to_fill->flows[i].sub_flow[1].subflow_properties.hr.weight = (unsigned long) inFields[18];
                break;
            case FAP20M_SUB_FLOW_TYPE_CL:
                p_flows_info_to_fill->flows[i].sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[17];
                p_flows_info_to_fill->flows[i].sub_flow[1].subflow_properties.cl.weight = (unsigned long) inFields[18];
                break;
            default: break;
        }

        p_flows_info_to_fill->flows[i].flow_type = (FAP20M_FLOW_TYPE) inFields[19];
        p_flows_info_to_fill->flows[i].is_slow_enabled = (unsigned int) inFields[20];
    }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_api_auto_update_port_aggregates_exact
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

    /* pack output arguments to galtis string */
    tbl_index = 0;

    /* AGGREGATES info */
    inFields[0] = exact_aggregates_info->item[0][0].sch.id;
    inFields[1] = exact_aggregates_info->item[0][0].sch.type;
    inFields[2] = exact_aggregates_info->item[0][0].sch.state;

    switch (exact_aggregates_info->item[0][0].sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = exact_aggregates_info->item[0][0].sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = exact_aggregates_info->item[0][0].sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }

    inFields[5] = exact_aggregates_info->item[0][0].flow.id;

    inFields[6] = exact_aggregates_info->item[0][0].flow.sub_flow[0].valid_mask;
    inFields[7] = exact_aggregates_info->item[0][0].flow.sub_flow[0].id;
    inFields[8] = exact_aggregates_info->item[0][0].flow.sub_flow[0].credit_source;

    inFields[9] = exact_aggregates_info->item[0][0].flow.sub_flow[0].shaper.max_rate;
    inFields[10] = exact_aggregates_info->item[0][0].flow.sub_flow[0].shaper.max_burst;

    inFields[11] = exact_aggregates_info->item[0][0].flow.sub_flow[0].type;

    switch (exact_aggregates_info->item[0][0].flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = exact_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = exact_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = exact_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = exact_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = exact_aggregates_info->item[0][0].flow.sub_flow[1].valid_mask;
    inFields[16] = exact_aggregates_info->item[0][0].flow.sub_flow[1].id;
    inFields[17] = exact_aggregates_info->item[0][0].flow.sub_flow[1].credit_source;

    inFields[18] = exact_aggregates_info->item[0][0].flow.sub_flow[1].shaper.max_rate;
    inFields[19] = exact_aggregates_info->item[0][0].flow.sub_flow[1].shaper.max_burst;

    inFields[20] = exact_aggregates_info->item[0][0].flow.sub_flow[1].type;

    switch (exact_aggregates_info->item[0][0].flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = exact_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = exact_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = exact_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = exact_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = exact_aggregates_info->item[0][0].flow.flow_type;
    inFields[25] = exact_aggregates_info->item[0][0].flow.is_slow_enabled;

    inFields[26] = exact_aggregates_info->item[0][0].valid;

    /* FLOW info */
/*    inFields[27] = exact_flows_info->flows[i].id; */

    inFields[27] = exact_flows_info->flows[0].sub_flow[0].valid_mask;
    inFields[28] = exact_flows_info->flows[0].sub_flow[0].id;
    inFields[29] = exact_flows_info->flows[0].sub_flow[0].credit_source;
    inFields[30] = exact_flows_info->flows[0].sub_flow[0].shaper.max_rate;
    inFields[31] = exact_flows_info->flows[0].sub_flow[0].shaper.max_burst;

    inFields[32] = exact_flows_info->flows[0].sub_flow[0].type;

    switch (exact_flows_info->flows[0].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[33] = exact_flows_info->flows[0].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[35] = exact_flows_info->flows[0].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[34] = exact_flows_info->flows[0].sub_flow[0].subflow_properties.cl.class_val;
            inFields[35] = exact_flows_info->flows[0].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[33] = -1;
            inFields[34] = -1;
            inFields[35] = -1;
    }

    inFields[36] = exact_flows_info->flows[0].sub_flow[1].valid_mask;
    inFields[37] = exact_flows_info->flows[0].sub_flow[1].id;
    inFields[38] = exact_flows_info->flows[0].sub_flow[1].credit_source;
    inFields[39] = exact_flows_info->flows[0].sub_flow[1].shaper.max_burst;
    inFields[40] = exact_flows_info->flows[0].sub_flow[1].shaper.max_rate;

    inFields[41] = exact_flows_info->flows[0].sub_flow[1].type;

    switch (exact_flows_info->flows[0].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[42] = exact_flows_info->flows[0].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[44] = exact_flows_info->flows[0].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[43] = exact_flows_info->flows[0].sub_flow[1].subflow_properties.cl.class_val;
            inFields[44] = exact_flows_info->flows[0].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[42] = -1;
            inFields[43] = -1;
            inFields[44] = -1;
    }

    inFields[45] = exact_flows_info->flows[0].flow_type;
    inFields[46] = exact_flows_info->flows[0].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                                inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                                inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                                inFields[15], inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23], inFields[24],
                                inFields[25], inFields[26], inFields[27], inFields[28], inFields[29],
                                inFields[30], inFields[31], inFields[32], inFields[33], inFields[34],
                                inFields[35], inFields[36], inFields[37], inFields[38], inFields[39],
                                inFields[40], inFields[41], inFields[42], inFields[43], inFields[44],
                                inFields[45], inFields[46]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_api_auto_update_port_aggregates_exact_end
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

    if (++tbl_index >= FAP20M_AUTO_NOF_HIERARCY * FAP20M_AUTO_NOF_INDEX)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */

    /* AGGREGATES info */
    inFields[0] = (**(exact_aggregates_info->item + tbl_index)).sch.id;
    inFields[1] = (**(exact_aggregates_info->item + tbl_index)).sch.type;
    inFields[2] = (**(exact_aggregates_info->item + tbl_index)).sch.state;

    switch ((**(exact_aggregates_info->item + tbl_index)).sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = (**(exact_aggregates_info->item + tbl_index)).sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = (**(exact_aggregates_info->item + tbl_index)).sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }

    inFields[5] = (**(exact_aggregates_info->item + tbl_index)).flow.id;

    inFields[6] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].valid_mask;
    inFields[7] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].id;
    inFields[8] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].credit_source;

    inFields[9] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].shaper.max_rate;
    inFields[10] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].shaper.max_burst;

    inFields[11] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].type;

    switch ((**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].valid_mask;
    inFields[16] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].id;
    inFields[17] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].credit_source;

    inFields[18] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].shaper.max_rate;
    inFields[19] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].shaper.max_burst;

    inFields[20] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].type;

    switch ((**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = (**(exact_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = (**(exact_aggregates_info->item + tbl_index)).flow.flow_type;
    inFields[25] = (**(exact_aggregates_info->item + tbl_index)).flow.is_slow_enabled;

    inFields[26] = (**(exact_aggregates_info->item + tbl_index)).valid;

    /* FLOW info */
    if (tbl_index < FAP20M_FABRIC_UNICAST_CLASSES)
    {
        /* inFields[27] = exact_flows_info->flows[i].id; */
        inFields[27] = exact_flows_info->flows[tbl_index].sub_flow[0].valid_mask;
        inFields[28] = exact_flows_info->flows[tbl_index].sub_flow[0].id;
        inFields[29] = exact_flows_info->flows[tbl_index].sub_flow[0].credit_source;
        inFields[30] = exact_flows_info->flows[tbl_index].sub_flow[0].shaper.max_rate;
        inFields[31] = exact_flows_info->flows[tbl_index].sub_flow[0].shaper.max_burst;
    
        inFields[32] = exact_flows_info->flows[tbl_index].sub_flow[0].type;
    
        switch (exact_flows_info->flows[tbl_index].sub_flow[0].type)
        {
            case FAP20M_SUB_FLOW_TYPE_HR:
                inFields[33] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.sp_class;
                inFields[35] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.weight;
                break;
            case FAP20M_SUB_FLOW_TYPE_CL:
                inFields[34] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.class_val;
                inFields[35] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.weight;
                break;
            default:
                inFields[33] = -1;
                inFields[34] = -1;
                inFields[35] = -1;
        }
    
        inFields[36] = exact_flows_info->flows[tbl_index].sub_flow[1].valid_mask;
        inFields[37] = exact_flows_info->flows[tbl_index].sub_flow[1].id;
        inFields[38] = exact_flows_info->flows[tbl_index].sub_flow[1].credit_source;
        inFields[39] = exact_flows_info->flows[tbl_index].sub_flow[1].shaper.max_burst;
        inFields[40] = exact_flows_info->flows[tbl_index].sub_flow[1].shaper.max_rate;
    
        inFields[41] = exact_flows_info->flows[tbl_index].sub_flow[1].type;
    
        switch (exact_flows_info->flows[tbl_index].sub_flow[1].type)
        {
            case FAP20M_SUB_FLOW_TYPE_HR:
                inFields[42] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.sp_class;
                inFields[44] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.weight;
                break;
            case FAP20M_SUB_FLOW_TYPE_CL:
                inFields[43] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.class_val;
                inFields[44] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.weight;
                break;
            default:
                inFields[42] = -1;
                inFields[43] = -1;
                inFields[44] = -1;
        }
    
        inFields[45] = exact_flows_info->flows[tbl_index].flow_type;
        inFields[46] = exact_flows_info->flows[tbl_index].is_slow_enabled;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                                inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                                inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                                inFields[15], inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23], inFields[24],
                                inFields[25], inFields[26], inFields[27], inFields[28], inFields[29],
                                inFields[30], inFields[31], inFields[32], inFields[33], inFields[34],
                                inFields[35], inFields[36], inFields[37], inFields[38], inFields[39],
                                inFields[40], inFields[41], inFields[42], inFields[43], inFields[44],
                                inFields[45], inFields[46]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_close_port_aggregates
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Close all the aggregate schedulers connected to a port.
*
*INPUT:
*  DIRECT:
*    IN  unsigned int              device_id 
*    IN  unsigned int              port_id -
*      The port ID to close.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    None.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_agg_get_default()
*   fap20m_api_auto_port_aggregates_get_info()
*   fap20m_api_auto_update_port_aggregates()
*   fap20m_api_auto_close_port_aggregates()
*   fap20m_api_auto_set_info_and_open_all_aggregates()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_close_port_aggregates
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_api_auto_close_port_aggregates(device_id, port_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_flows_get_default
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*   Each structure in the auto management system has a function
*   fap20m_api_auto_XXX_defaults().
*   This function will:
*   1. Clean the structure;
*   2. Set default values to parameters that have logical common values.
*   3. Set invalid values to parameters that have to be configured by
*      user.
*   
*   The function get the structure FAP20M_AUTO_FLOW_INFO
*    as parameter, and fill it with some default values.
*INPUT:
*  DIRECT:
*    None.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_FLOW_INFO *p_port_info -
*      Returned with default parameter to be later used 
*       to call the auto flows functions.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_set_info_and_open_all_flows()
*   fap20m_api_auto_update_port_flows()
*   fap20m_api_auto_close_port_flows()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_flows_get_default
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_api_auto_flows_get_default(p_flows_info);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* output for exact_flows_info */
    tbl_index = 0;

    inFields[0] = p_flows_info->flows[tbl_index].id;

    inFields[1] = p_flows_info->flows[tbl_index].sub_flow[0].valid_mask;
    inFields[2] = p_flows_info->flows[tbl_index].sub_flow[0].id;
    inFields[3] = p_flows_info->flows[tbl_index].sub_flow[0].credit_source;
    inFields[4] = p_flows_info->flows[tbl_index].sub_flow[0].shaper.max_rate;
    inFields[5] = p_flows_info->flows[tbl_index].sub_flow[0].shaper.max_burst;

    inFields[6] = p_flows_info->flows[tbl_index].sub_flow[0].type;

    switch (p_flows_info->flows[tbl_index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[7] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[9] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[8] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.class_val;
            inFields[9] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[7] = -1;
            inFields[8] = -1;
            inFields[9] = -1;
    }

    inFields[10]  = p_flows_info->flows[tbl_index].sub_flow[1].valid_mask;
    inFields[11] = p_flows_info->flows[tbl_index].sub_flow[1].id;
    inFields[12] = p_flows_info->flows[tbl_index].sub_flow[1].credit_source;
    inFields[13] = p_flows_info->flows[tbl_index].sub_flow[1].shaper.max_burst;
    inFields[14] = p_flows_info->flows[tbl_index].sub_flow[1].shaper.max_rate;
    
    inFields[15] = p_flows_info->flows[tbl_index].sub_flow[1].type;

    switch (p_flows_info->flows[tbl_index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[16] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[18] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[17] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.class_val;
            inFields[18] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[16] = -1;
            inFields[17] = -1;
            inFields[18] = -1;
    }

    inFields[19] = p_flows_info->flows[tbl_index].flow_type;
    inFields[20] = p_flows_info->flows[tbl_index].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                        inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                        inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                        inFields[15], inFields[16], inFields[17], inFields[18], inFields[19], inFields[20]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_flows_get_default_end
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

    if (++tbl_index >= FAP20M_FABRIC_UNICAST_CLASSES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = p_flows_info->flows[tbl_index].id;

    inFields[1] = p_flows_info->flows[tbl_index].sub_flow[0].valid_mask;
    inFields[2] = p_flows_info->flows[tbl_index].sub_flow[0].id;
    inFields[3] = p_flows_info->flows[tbl_index].sub_flow[0].credit_source;
    inFields[4] = p_flows_info->flows[tbl_index].sub_flow[0].shaper.max_rate;
    inFields[5] = p_flows_info->flows[tbl_index].sub_flow[0].shaper.max_burst;

    inFields[6] = p_flows_info->flows[tbl_index].sub_flow[0].type;

    switch (p_flows_info->flows[tbl_index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[7] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[9] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[8] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.class_val;
            inFields[9] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[7] = -1;
            inFields[8] = -1;
            inFields[9] = -1;
    }

    inFields[10]  = p_flows_info->flows[tbl_index].sub_flow[1].valid_mask;
    inFields[11] = p_flows_info->flows[tbl_index].sub_flow[1].id;
    inFields[12] = p_flows_info->flows[tbl_index].sub_flow[1].credit_source;
    inFields[13] = p_flows_info->flows[tbl_index].sub_flow[1].shaper.max_burst;
    inFields[14] = p_flows_info->flows[tbl_index].sub_flow[1].shaper.max_rate;
    
    inFields[15] = p_flows_info->flows[tbl_index].sub_flow[1].type;

    switch (p_flows_info->flows[tbl_index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[16] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[18] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[17] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.class_val;
            inFields[18] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[16] = -1;
            inFields[17] = -1;
            inFields[18] = -1;
    }

    inFields[19] = p_flows_info->flows[tbl_index].flow_type;
    inFields[20] = p_flows_info->flows[tbl_index].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                        inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                        inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                        inFields[15], inFields[16], inFields[17], inFields[18], inFields[19], inFields[20]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_set_info_and_open_all_flows
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  The function should be called at the initialization phase
*  The function will set the FAP20M_AUTO_FLOW_INFO[class_i] as the
*   information of all the flows of class_i, and will also open all the 
*   flows in the system according to that 
*
*INPUT:
*  DIRECT:
*    IN FAP20M_AUTO_FLOW_INFO *p_flow_info -
*      The port information to be used for all the ports
*        on the card (CPU).
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*     None.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_flow_get_default()
*   fap20m_api_auto_port_flows_get_info()
*   fap20m_api_auto_update_port_flows()
*   fap20m_api_auto_close_port_flows()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_set_first_info_and_open_all_flows
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

    sand_os_memset((void*) p_flows_info_to_fill, 0, sizeof(p_flows_info_to_fill));

    /* map input arguments to locals */
    index = (unsigned int) inFields[0];

    p_flows_info_to_fill->flows[index].id = (FAP20M_FLOW_ID) inFields[1];

    p_flows_info_to_fill->flows[index].sub_flow[0].valid_mask = (unsigned long) inFields[2];
    p_flows_info_to_fill->flows[index].sub_flow[0].id = (FAP20M_SUB_FLOW_ID) inFields[3];
    p_flows_info_to_fill->flows[index].sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[4];

    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_rate = (unsigned long) inFields[5];
    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_burst = (unsigned long) inFields[6];
    
    p_flows_info_to_fill->flows[index].sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE) inFields[7];

    switch (p_flows_info_to_fill->flows[index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[8];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.weight = (unsigned long) inFields[10];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.weight = (unsigned long) inFields[10];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].sub_flow[1].valid_mask = (unsigned long) inFields[11];
    p_flows_info_to_fill->flows[index].sub_flow[1].id = (FAP20M_SUB_FLOW_ID) inFields[12];
    p_flows_info_to_fill->flows[index].sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[13];

    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_rate = (unsigned long) inFields[14];
    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_burst = (unsigned long) inFields[15];

    p_flows_info_to_fill->flows[index].sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE) inFields[16];

    switch (p_flows_info_to_fill->flows[index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[17];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.weight = (unsigned long) inFields[19];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[18];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.weight = (unsigned long) inFields[19];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].flow_type = (FAP20M_FLOW_TYPE) inFields[20];
    p_flows_info_to_fill->flows[index].is_slow_enabled = (unsigned int) inFields[21];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_api_auto_set_next_info_and_open_all_flows
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

    p_flows_info_to_fill->flows[index].id = (FAP20M_FLOW_ID) inFields[1];

    p_flows_info_to_fill->flows[index].sub_flow[0].valid_mask = (unsigned long) inFields[2];
    p_flows_info_to_fill->flows[index].sub_flow[0].id = (FAP20M_SUB_FLOW_ID) inFields[3];
    p_flows_info_to_fill->flows[index].sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[4];

    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_rate = (unsigned long) inFields[5];
    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_burst = (unsigned long) inFields[6];
    
    p_flows_info_to_fill->flows[index].sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE) inFields[7];

    switch (p_flows_info_to_fill->flows[index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[8];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.weight = (unsigned long) inFields[10];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.weight = (unsigned long) inFields[10];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].sub_flow[1].valid_mask = (unsigned long) inFields[11];
    p_flows_info_to_fill->flows[index].sub_flow[1].id = (FAP20M_SUB_FLOW_ID) inFields[12];
    p_flows_info_to_fill->flows[index].sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[13];

    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_rate = (unsigned long) inFields[14];
    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_burst = (unsigned long) inFields[15];

    p_flows_info_to_fill->flows[index].sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE) inFields[16];

    switch (p_flows_info_to_fill->flows[index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[17];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.weight = (unsigned long) inFields[19];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[18];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.weight = (unsigned long) inFields[19];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].flow_type = (FAP20M_FLOW_TYPE) inFields[20];
    p_flows_info_to_fill->flows[index].is_slow_enabled = (unsigned int) inFields[21];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_api_auto_end_set_info_and_open_all_flows
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_api_auto_set_info_and_open_all_flows(p_flows_info_to_fill);

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
*  fap20m_api_auto_port_flows_get_info
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Get the flows information from the software DB.
*  Might be called before
*  fap20m_api_auto_update_port_flows()
*
*INPUT:
*  DIRECT:
*    IN  unsigned int              device_id 
*    IN  unsigned int              port_id -
*      Get the information of all the flows connected to that port ID.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_FLOW_INFO   *p_flows_info
*      Returned with the information from SW DB 
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_flow_get_default()
*   fap20m_api_auto_port_flows_get_info()
*   fap20m_api_auto_update_port_flows()
*   fap20m_api_auto_close_port_flows()
*   fap20m_api_auto_set_info_and_open_all_flows()
*****************************************************/
static CMD_STATUS wrFap20m_auto_flow_set_first_info
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

    sand_os_memset((void*)p_flows_info_to_fill, 0, sizeof(p_flows_info_to_fill));

    /* map input arguments to locals */
    index = (unsigned int) inFields[0];
    p_flows_info_to_fill->flows[index].id = (FAP20M_FLOW_ID) inFields[1];

    p_flows_info_to_fill->flows[index].sub_flow[0].valid_mask = (unsigned long) inFields[2];
    p_flows_info_to_fill->flows[index].sub_flow[0].id = (FAP20M_SUB_FLOW_ID) inFields[3];
    p_flows_info_to_fill->flows[index].sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[4];
    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_rate = (unsigned long) inFields[5];
    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_burst = (unsigned long) inFields[6];
    p_flows_info_to_fill->flows[index].sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE) inFields[7];

    switch (p_flows_info_to_fill->flows[index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[8];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.weight = (unsigned long) inFields[10];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.weight = (unsigned long) inFields[10];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].sub_flow[1].valid_mask = (unsigned long) inFields[11];
    p_flows_info_to_fill->flows[index].sub_flow[1].id = (FAP20M_SUB_FLOW_ID) inFields[12];
    p_flows_info_to_fill->flows[index].sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[13];
    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_rate = (unsigned long) inFields[14];
    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_burst = (unsigned long) inFields[15];
    p_flows_info_to_fill->flows[index].sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE) inFields[16];

    switch (p_flows_info->flows[index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[17];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.weight = (unsigned long) inFields[19];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[18];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.weight = (unsigned long) inFields[19];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].flow_type = (FAP20M_FLOW_TYPE) inFields[20];
    p_flows_info_to_fill->flows[index].is_slow_enabled = (unsigned int) inFields[21];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_auto_flow_set_next_info
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
    p_flows_info_to_fill->flows[index].id = (FAP20M_FLOW_ID) inFields[1];

    p_flows_info_to_fill->flows[index].sub_flow[0].valid_mask = (unsigned long) inFields[2];
    p_flows_info_to_fill->flows[index].sub_flow[0].id = (FAP20M_SUB_FLOW_ID) inFields[3];
    p_flows_info_to_fill->flows[index].sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[4];
    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_rate = (unsigned long) inFields[5];
    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_burst = (unsigned long) inFields[6];
    p_flows_info_to_fill->flows[index].sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE) inFields[7];

    switch (p_flows_info_to_fill->flows[index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[8];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.weight = (unsigned long) inFields[10];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.weight = (unsigned long) inFields[10];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].sub_flow[1].valid_mask = (unsigned long) inFields[11];
    p_flows_info_to_fill->flows[index].sub_flow[1].id = (FAP20M_SUB_FLOW_ID) inFields[12];
    p_flows_info_to_fill->flows[index].sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[13];
    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_rate = (unsigned long) inFields[14];
    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_burst = (unsigned long) inFields[15];
    p_flows_info_to_fill->flows[index].sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE) inFields[16];

    switch (p_flows_info_to_fill->flows[index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[17];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.weight = (unsigned long) inFields[19];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[18];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.weight = (unsigned long) inFields[19];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].flow_type = (FAP20M_FLOW_TYPE) inFields[20];
    p_flows_info_to_fill->flows[index].is_slow_enabled = (unsigned int) inFields[21];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_auto_flow_end_set_info
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];

    /* call Ocelot api function */
    result = fap20m_auto_flow_set_info(device_id, port_id, p_flows_info_to_fill);

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
*  fap20m_api_auto_port_flows_get_info
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Get the flows information from the software DB.
*  Might be called before
*  fap20m_api_auto_update_port_flows()
*
*INPUT:
*  DIRECT:
*    IN  unsigned int              device_id 
*    IN  unsigned int              port_id -
*      Get the information of all the flows connected to that port ID.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_FLOW_INFO   *p_flows_info
*      Returned with the information from SW DB 
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_flow_get_default()
*   fap20m_api_auto_port_flows_get_info()
*   fap20m_api_auto_update_port_flows()
*   fap20m_api_auto_close_port_flows()
*   fap20m_api_auto_set_info_and_open_all_flows()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_port_flows_get_info
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_api_auto_port_flows_get_info(device_id, port_id, p_flows_info);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    tbl_index = 0;

    inFields[0] = tbl_index;

    inFields[1] = p_flows_info->flows[tbl_index].id;

    inFields[2] = p_flows_info->flows[tbl_index].sub_flow[0].valid_mask;
    inFields[3] = p_flows_info->flows[tbl_index].sub_flow[0].id;
    inFields[4] = p_flows_info->flows[tbl_index].sub_flow[0].credit_source;
    inFields[5] = p_flows_info->flows[tbl_index].sub_flow[0].shaper.max_rate;
    inFields[6] = p_flows_info->flows[tbl_index].sub_flow[0].shaper.max_burst;

    inFields[7] = p_flows_info->flows[tbl_index].sub_flow[0].type;

    switch (p_flows_info->flows[tbl_index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[8] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[10] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[9] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.class_val;
            inFields[10] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[8] = -1;
            inFields[9] = -1;
            inFields[10] = -1;
    }

    inFields[11] = p_flows_info->flows[tbl_index].sub_flow[1].valid_mask;
    inFields[12] = p_flows_info->flows[tbl_index].sub_flow[1].id;
    inFields[13] = p_flows_info->flows[tbl_index].sub_flow[1].credit_source;
    inFields[14] = p_flows_info->flows[tbl_index].sub_flow[1].shaper.max_burst;
    inFields[15] = p_flows_info->flows[tbl_index].sub_flow[1].shaper.max_rate;

    inFields[16] = p_flows_info->flows[tbl_index].sub_flow[1].type;

    switch (p_flows_info->flows[tbl_index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[17] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[19] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[18] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.class_val;
            inFields[19] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[17] = -1;
            inFields[18] = -1;
            inFields[19] = -1;
    }

    inFields[20] = p_flows_info->flows[tbl_index].flow_type;
    inFields[21] = p_flows_info->flows[tbl_index].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],  inFields[3],
                        inFields[4],  inFields[5],  inFields[6],  inFields[7],
                        inFields[8],  inFields[9],  inFields[10], inFields[11],
                        inFields[12], inFields[13], inFields[14], inFields[15],
                        inFields[16], inFields[17], inFields[18], inFields[19],
                        inFields[20], inFields[21]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_port_flows_get_info_end
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

    if (++tbl_index >= FAP20M_FABRIC_UNICAST_CLASSES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;

    inFields[1] = p_flows_info->flows[tbl_index].id;

    inFields[2] = p_flows_info->flows[tbl_index].sub_flow[0].valid_mask;
    inFields[3] = p_flows_info->flows[tbl_index].sub_flow[0].id;
    inFields[4] = p_flows_info->flows[tbl_index].sub_flow[0].credit_source;
    inFields[5] = p_flows_info->flows[tbl_index].sub_flow[0].shaper.max_rate;
    inFields[6] = p_flows_info->flows[tbl_index].sub_flow[0].shaper.max_burst;

    inFields[7] = p_flows_info->flows[tbl_index].sub_flow[0].type;

    switch (p_flows_info->flows[tbl_index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[8] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[10] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[9] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.class_val;
            inFields[10] = p_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[8] = -1;
            inFields[9] = -1;
            inFields[10] = -1;
    }

    inFields[11] = p_flows_info->flows[tbl_index].sub_flow[1].valid_mask;
    inFields[12] = p_flows_info->flows[tbl_index].sub_flow[1].id;
    inFields[13] = p_flows_info->flows[tbl_index].sub_flow[1].credit_source;
    inFields[14] = p_flows_info->flows[tbl_index].sub_flow[1].shaper.max_burst;
    inFields[15] = p_flows_info->flows[tbl_index].sub_flow[1].shaper.max_rate;

    inFields[16] = p_flows_info->flows[tbl_index].sub_flow[1].type;

    switch (p_flows_info->flows[tbl_index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[17] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[19] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[18] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.class_val;
            inFields[19] = p_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[17] = -1;
            inFields[18] = -1;
            inFields[19] = -1;
    }

    inFields[20] = p_flows_info->flows[tbl_index].flow_type;
    inFields[21] = p_flows_info->flows[tbl_index].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],  inFields[3],
                        inFields[4],  inFields[5],  inFields[6],  inFields[7],
                        inFields[8],  inFields[9],  inFields[10], inFields[11],
                        inFields[12], inFields[13], inFields[14], inFields[15],
                        inFields[16], inFields[17], inFields[18], inFields[19],
                        inFields[20], inFields[21]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_port_flows_get_info
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Update flows capabilities, to both the software database
*   and to the device.
*
*INPUT:
*  DIRECT:
*    IN  unsigned int              device_id 
*    IN  unsigned int              port_id -
*      Update the flows connected to that port ID.
*    IN  FAP20M_AUTO_FLOW_INFO   *p_flows_info,
*      The new flows capability.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_FLOW_INFO   *exact_flows_info
*      Returned with the information from SW DB 
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_flow_get_default()
*   fap20m_api_auto_port_flows_get_info()
*   fap20m_api_auto_update_port_flows()
*   fap20m_api_auto_close_port_flows()
*   fap20m_api_auto_set_info_and_open_all_flows()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_update_port_flows_set_first
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

    sand_os_memset((void*)p_flows_info_to_fill, 0, sizeof(p_flows_info_to_fill));

    /* map input arguments to locals */
    index = (unsigned int) inFields[0];

    p_flows_info_to_fill->flows[index].id = (FAP20M_FLOW_ID) inFields[1];

    p_flows_info_to_fill->flows[index].sub_flow[0].valid_mask = (unsigned long) inFields[2];
    p_flows_info_to_fill->flows[index].sub_flow[0].id = (FAP20M_SUB_FLOW_ID) inFields[3];
    p_flows_info_to_fill->flows[index].sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[4];

    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_rate = (unsigned long) inFields[5];
    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_burst = (unsigned long) inFields[6];
    
    p_flows_info_to_fill->flows[index].sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE) inFields[7];

    switch (p_flows_info_to_fill->flows[index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[8];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.weight = (unsigned long) inFields[10];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.weight = (unsigned long) inFields[10];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].sub_flow[1].valid_mask = (unsigned long) inFields[11];
    p_flows_info_to_fill->flows[index].sub_flow[1].id = (FAP20M_SUB_FLOW_ID) inFields[12];
    p_flows_info_to_fill->flows[index].sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[13];

    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_rate = (unsigned long) inFields[14];
    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_burst = (unsigned long) inFields[15];

    p_flows_info_to_fill->flows[index].sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE) inFields[16];

    switch (p_flows_info_to_fill->flows[index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[17];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.weight = (unsigned long) inFields[19];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[18];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.weight = (unsigned long) inFields[19];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].flow_type = (FAP20M_FLOW_TYPE) inFields[20];
    p_flows_info_to_fill->flows[index].is_slow_enabled = (unsigned int) inFields[21];
    
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_api_auto_update_port_flows_set_next
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

    p_flows_info_to_fill->flows[index].id = (FAP20M_FLOW_ID) inFields[1];

    p_flows_info_to_fill->flows[index].sub_flow[0].valid_mask = (unsigned long) inFields[2];
    p_flows_info_to_fill->flows[index].sub_flow[0].id = (FAP20M_SUB_FLOW_ID) inFields[3];
    p_flows_info_to_fill->flows[index].sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[4];

    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_rate = (unsigned long) inFields[5];
    p_flows_info_to_fill->flows[index].sub_flow[0].shaper.max_burst = (unsigned long) inFields[6];
    
    p_flows_info_to_fill->flows[index].sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE) inFields[7];

    switch (p_flows_info_to_fill->flows[index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[8];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.hr.weight = (unsigned long) inFields[10];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[9];
            p_flows_info_to_fill->flows[index].sub_flow[0].subflow_properties.cl.weight = (unsigned long) inFields[10];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].sub_flow[1].valid_mask = (unsigned long) inFields[11];
    p_flows_info_to_fill->flows[index].sub_flow[1].id = (FAP20M_SUB_FLOW_ID) inFields[12];
    p_flows_info_to_fill->flows[index].sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID) inFields[13];

    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_rate = (unsigned long) inFields[14];
    p_flows_info_to_fill->flows[index].sub_flow[1].shaper.max_burst = (unsigned long) inFields[15];

    p_flows_info_to_fill->flows[index].sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE) inFields[16];

    switch (p_flows_info_to_fill->flows[index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS) inFields[17];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.hr.weight = (unsigned long) inFields[19];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS) inFields[18];
            p_flows_info_to_fill->flows[index].sub_flow[1].subflow_properties.cl.weight = (unsigned long) inFields[19];
            break;
        default: break;
    }

    p_flows_info_to_fill->flows[index].flow_type = (FAP20M_FLOW_TYPE) inFields[20];
    p_flows_info_to_fill->flows[index].is_slow_enabled = (unsigned int) inFields[21];
    
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_api_auto_update_port_flows_end_set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];
    
    /* call Ocelot API function */
    result = fap20m_api_auto_update_port_flows(device_id, port_id, p_flows_info_to_fill, exact_flows_info);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_api_auto_update_port_flows_exact
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

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;

    inFields[1] = exact_flows_info->flows[tbl_index].id;

    inFields[2] = exact_flows_info->flows[tbl_index].sub_flow[0].valid_mask;
    inFields[3] = exact_flows_info->flows[tbl_index].sub_flow[0].id;
    inFields[4] = exact_flows_info->flows[tbl_index].sub_flow[0].credit_source;
    inFields[5] = exact_flows_info->flows[tbl_index].sub_flow[0].shaper.max_rate;
    inFields[6] = exact_flows_info->flows[tbl_index].sub_flow[0].shaper.max_burst;

    inFields[7] = exact_flows_info->flows[tbl_index].sub_flow[0].type;

    switch (exact_flows_info->flows[tbl_index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[8] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[10] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[9] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.class_val;
            inFields[10] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[8] = -1;
            inFields[9] = -1;
            inFields[10] = -1;
    }

    inFields[11] = exact_flows_info->flows[tbl_index].sub_flow[1].valid_mask;
    inFields[12] = exact_flows_info->flows[tbl_index].sub_flow[1].id;
    inFields[13] = exact_flows_info->flows[tbl_index].sub_flow[1].credit_source;
    inFields[14] = exact_flows_info->flows[tbl_index].sub_flow[1].shaper.max_burst;
    inFields[15] = exact_flows_info->flows[tbl_index].sub_flow[1].shaper.max_rate;

    inFields[16] = exact_flows_info->flows[tbl_index].sub_flow[1].type;

    switch (exact_flows_info->flows[tbl_index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[17] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[19] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[18] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.class_val;
            inFields[19] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[17] = -1;
            inFields[18] = -1;
            inFields[19] = -1;
    }

    inFields[20] = exact_flows_info->flows[tbl_index].flow_type;
    inFields[21] = exact_flows_info->flows[tbl_index].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],  inFields[3],
                        inFields[4],  inFields[5],  inFields[6],  inFields[7],
                        inFields[8],  inFields[9],  inFields[10], inFields[11],
                        inFields[12], inFields[13], inFields[14], inFields[15],
                        inFields[16], inFields[17], inFields[18], inFields[19],
                        inFields[20], inFields[21]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_update_port_flows_exact_end
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

    if (++tbl_index >= FAP20M_FABRIC_UNICAST_CLASSES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;

    inFields[1] = exact_flows_info->flows[tbl_index].id;

    inFields[2] = exact_flows_info->flows[tbl_index].sub_flow[0].valid_mask;
    inFields[3] = exact_flows_info->flows[tbl_index].sub_flow[0].id;
    inFields[4] = exact_flows_info->flows[tbl_index].sub_flow[0].credit_source;
    inFields[5] = exact_flows_info->flows[tbl_index].sub_flow[0].shaper.max_rate;
    inFields[6] = exact_flows_info->flows[tbl_index].sub_flow[0].shaper.max_burst;

    inFields[7] = exact_flows_info->flows[tbl_index].sub_flow[0].type;

    switch (exact_flows_info->flows[tbl_index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[8] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[10] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[9] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.class_val;
            inFields[10] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[8] = -1;
            inFields[9] = -1;
            inFields[10] = -1;
    }

    inFields[11] = exact_flows_info->flows[tbl_index].sub_flow[1].valid_mask;
    inFields[12] = exact_flows_info->flows[tbl_index].sub_flow[1].id;
    inFields[13] = exact_flows_info->flows[tbl_index].sub_flow[1].credit_source;
    inFields[14] = exact_flows_info->flows[tbl_index].sub_flow[1].shaper.max_burst;
    inFields[15] = exact_flows_info->flows[tbl_index].sub_flow[1].shaper.max_rate;

    inFields[16] = exact_flows_info->flows[tbl_index].sub_flow[1].type;

    switch (exact_flows_info->flows[tbl_index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[17] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[19] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[18] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.class_val;
            inFields[19] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[17] = -1;
            inFields[18] = -1;
            inFields[19] = -1;
    }

    inFields[20] = exact_flows_info->flows[tbl_index].flow_type;
    inFields[21] = exact_flows_info->flows[tbl_index].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],  inFields[3],
                        inFields[4],  inFields[5],  inFields[6],  inFields[7],
                        inFields[8],  inFields[9],  inFields[10], inFields[11],
                        inFields[12], inFields[13], inFields[14], inFields[15],
                        inFields[16], inFields[17], inFields[18], inFields[19],
                        inFields[20], inFields[21]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_close_port_flows
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Close all the flows connected to a port.
*
*INPUT:
*  DIRECT:
*    IN  unsigned int              device_id 
*    IN  unsigned int              port_id -
*      The port ID to close.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    None.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_flow_get_default()
*   fap20m_api_auto_port_flows_get_info()
*   fap20m_api_auto_update_port_flows()
*   fap20m_api_auto_close_port_flows()
*   fap20m_api_auto_set_info_and_open_all_flows()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_close_port_flows
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_api_auto_close_port_flows(device_id, port_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_queue_type_params_get_default()
*  fap20m_api_auto_get_next_vacant_queue_type_id()
*  fap20m_api_auto_queue_set_queue_params()
*  fap20m_api_auto_queue_get_queue_params()
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
* The ingress queues configuration is done per queue type.
* Each queue has a queue type, and elsewhere, the queue 
*  type parameters table, hold the capabilities of each
*  queue type.
*  
* The auto management holds the queue type of each queue,
*  and also supply some helper functions to the queue
*  type parameters data base.
*
*
* The auto management user, may want to use different 
*  queue type parameters for different queue.
* When a queue with new queue type parameters should be opened,
*  the user need to call 
* fap20m_api_auto_get_next_vacant_queue_type_id(),
*  to get a vacant queue type ID, whose parameters
*  might be set.
*INPUT:
*  DIRECT:
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    None.
*REMARKS:
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_queue_type_params_get_default
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    FAP20M_QUEUE_TYPE_PARAM p_queue_type_params;
    unsigned int result;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_api_auto_queue_type_params_get_default(&p_queue_type_params);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = p_queue_type_params.RED_enable;
    inFields[1] = p_queue_type_params.RED_exp_weight;
    inFields[2] = p_queue_type_params.flow_slow_th_up;
    inFields[3] = p_queue_type_params.flow_stat_msg;
    inFields[4] = p_queue_type_params.max_q_size;
    inFields[5] = p_queue_type_params.flow_slow_hys_en;
    inFields[6] = p_queue_type_params.hysteresis_size;
    inFields[7] = p_queue_type_params.header_compensation;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_queue_type_params_get_default_end
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
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_get_next_vacant_queue_type_id
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    FAP20M_QUEUE_TYPE default_queue_type;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_api_auto_get_next_vacant_queue_type_id(&default_queue_type);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", default_queue_type);

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_queue_set_queue_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    FAP20M_QUEUE_TYPE queue_type;
    FAP20M_QUEUE_TYPE_PARAM queue_type_params;
    FAP20M_QUEUE_TYPE_PARAM exact_queue_type_params;
    unsigned short rc;

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    queue_type = (FAP20M_QUEUE_TYPE)inArgs[0];

    queue_type_params.RED_enable = (unsigned int) inFields[0];
    queue_type_params.RED_exp_weight = (unsigned int) inFields[1];
    queue_type_params.flow_slow_th_up = (unsigned long) inFields[2];
    queue_type_params.flow_stat_msg = (unsigned long) inFields[3];
    queue_type_params.max_q_size = (unsigned long) inFields[4];
    queue_type_params.flow_slow_hys_en = (unsigned int) inFields[5];
    queue_type_params.hysteresis_size = (unsigned long) inFields[6];
    queue_type_params.header_compensation = (int) inFields[7];

    /* call Ocelot API function */
    result = fap20m_api_auto_queue_set_queue_params(queue_type, &queue_type_params, &exact_queue_type_params);

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
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_queue_get_queue_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    FAP20M_QUEUE_TYPE queue_type;
    FAP20M_QUEUE_TYPE_PARAM queue_type_params;
    unsigned int is_vacant;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    queue_type = (FAP20M_QUEUE_TYPE)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_api_auto_queue_get_queue_params(queue_type, &queue_type_params, &is_vacant);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    galtisOutput(outArgs, rc, "%d", is_vacant);

    /* pack output arguments to galtis string */
    inFields[0] = queue_type_params.RED_enable;
    inFields[1] = queue_type_params.RED_exp_weight;
    inFields[2] = queue_type_params.flow_slow_th_up;
    inFields[3] = queue_type_params.flow_stat_msg;
    inFields[4] = queue_type_params.max_q_size;
    inFields[5] = queue_type_params.flow_slow_hys_en;
    inFields[6] = queue_type_params.hysteresis_size;
    inFields[7] = queue_type_params.header_compensation;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");
    
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_queue_get_queue_params_end
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
*  fap20m_api_auto_queue_info_get_default
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*   Each structure in the auto management system has a function
*   fap20m_api_auto_XXX_defaults().
*   This function will:
*   1. Clean the structure;
*   2. Set default values to parameters that have logical common values.
*   3. Set invalid values to parameters that have to be configured by
*      user.
*   
*   The function get the structure FAP20M_AUTO_QUEUE_INFO
*    as parameter, and fill it with some default values.
*INPUT:
*  DIRECT:
*    IN  FAP20M_QUEUE_TYPE        queue_type_id -
*      All the queue types would be set to this default.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_QUEUE_INFO *p_queue_info -
*      Returned with default parameter to be later used 
*       to call the auto flows functions.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_set_info_and_open_all_queues()
*   fap20m_api_auto_queues_get_info()
*   fap20m_api_auto_update_queues()
*   fap20m_api_auto_close_queues()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_queue_info_get_default
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    FAP20M_AUTO_QUEUE_INFO p_queue_info;
    FAP20M_QUEUE_TYPE queue_type_id;
    unsigned short rc;
    unsigned int index;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    queue_type_id = (FAP20M_QUEUE_TYPE)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_api_auto_queue_info_get_default(&p_queue_info, queue_type_id);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    for (index = 0; index < FAP20M_FABRIC_UNICAST_CLASSES; index++)
    {
        inFields[index] = p_queue_info.queue_type[index];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_queue_info_get_default_end
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
*  fap20m_api_auto_set_info_and_open_all_queues
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  The function should be called at the initialization phase
*  The function will set the FAP20M_AUTO_QUEUE_INFO[class_i] as the
*   information of all the queues of class_i, and will also open all the 
*   flows in the system according to that 
*
*INPUT:
*  DIRECT:
*    IN FAP20M_AUTO_QUEUE_INFO *p_queues_info -
*      The port information to be used for all the ports
*        on the card (CPU).
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*     None.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_set_info_and_open_all_queues()
*   fap20m_api_auto_queues_get_info()
*   fap20m_api_auto_update_queues()
*   fap20m_api_auto_close_queues()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_set_info_and_open_all_queues
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    FAP20M_AUTO_QUEUE_INFO p_queues_info;
    unsigned short rc;
    unsigned int index;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    for(index = 0; index < FAP20M_FABRIC_UNICAST_CLASSES; index++)
    {
        p_queues_info.queue_type[index] = (FAP20M_QUEUE_TYPE)inFields[index];
    }

    /* call Ocelot API function */
    result = fap20m_api_auto_set_info_and_open_all_queues(&p_queues_info);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_auto_queues_set_info
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int dest_fap_id;
    unsigned int dest_port_id;
    FAP20M_AUTO_QUEUE_INFO p_queues_info;
    unsigned short rc;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dest_fap_id = (unsigned int)inArgs[0];
    dest_port_id = (unsigned int)inArgs[1];

    p_queues_info.queue_type[0] = (FAP20M_QUEUE_TYPE)inFields[0];
    p_queues_info.queue_type[1] = (FAP20M_QUEUE_TYPE)inFields[1];
    p_queues_info.queue_type[2] = (FAP20M_QUEUE_TYPE)inFields[2];
    p_queues_info.queue_type[3] = (FAP20M_QUEUE_TYPE)inFields[3];
    p_queues_info.queue_type[4] = (FAP20M_QUEUE_TYPE)inFields[4];
    p_queues_info.queue_type[5] = (FAP20M_QUEUE_TYPE)inFields[5];
    p_queues_info.queue_type[6] = (FAP20M_QUEUE_TYPE)inFields[6];
    p_queues_info.queue_type[7] = (FAP20M_QUEUE_TYPE)inFields[7];

    /* call Ocelot API function */
    result = fap20m_auto_queues_set_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_queues_get_info
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Get the queues information from the software DB.
*  Might be called before
*  fap20m_api_auto_update_port_queues()
*
*INPUT:
*  DIRECT:
*    IN  unsigned int              dest_fap_id 
*    IN  unsigned int              dest_port_id -
*      Get the information of all the queues connected to that port ID.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    OUT FAP20M_AUTO_QUEUE_INFO   *p_queues_info
*      Returned with the information from SW DB 
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_set_info_and_open_all_queues()
*   fap20m_api_auto_queues_get_info()
*   fap20m_api_auto_update_queues()
*   fap20m_api_auto_close_queues()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_queues_get_info
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int dest_fap_id;
    unsigned int dest_port_id;
    FAP20M_AUTO_QUEUE_INFO p_queues_info;
    unsigned short rc;
    unsigned int index;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dest_fap_id = (unsigned int)inArgs[0];
    dest_port_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_api_auto_queues_get_info(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    for (index = 0; index < FAP20M_FABRIC_UNICAST_CLASSES; index++)
    {
        inFields[index] = p_queues_info.queue_type[index];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_api_auto_queues_get_info_end
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
*  fap20m_api_auto_update_queues
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Update queues capabilities, to both the software database
*   and to the device.
*
*INPUT:
*  DIRECT:
*    IN  unsigned int              dest_fap_id 
*    IN  unsigned int              dest_port_id -
*      Update the queues connected to that port ID.
*    IN  FAP20M_AUTO_QUEUE_INFO   *p_queues_info,
*      The new queues capability.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    None.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_set_info_and_open_all_queues()
*   fap20m_api_auto_queues_get_info()
*   fap20m_api_auto_update_queues()
*   fap20m_api_auto_close_queues()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_update_queues
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int dest_fap_id;
    unsigned int dest_port_id;
    FAP20M_AUTO_QUEUE_INFO p_queues_info;
    unsigned short rc;
    unsigned int index;


    /* avoid 7compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dest_fap_id = (unsigned int)inArgs[0];
    dest_port_id = (unsigned int)inArgs[1];

    for(index = 0; index < FAP20M_FABRIC_UNICAST_CLASSES; index++)
    {
        p_queues_info.queue_type[index] = (FAP20M_QUEUE_TYPE)inFields[index];
    }

    /* call Ocelot API function */
    result = fap20m_api_auto_update_queues(dest_fap_id, dest_port_id, &p_queues_info);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_close_queues
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Update queues capabilities, to both the software database
*   and to the device.
*
*INPUT:
*  DIRECT:
*    IN  unsigned int              dest_fap_id 
*    IN  unsigned int              dest_port_id -
*      Close the queues connected to that port ID.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    None.
*REMARKS:
*SEE ALSO:
*   fap20m_api_auto_set_info_and_open_all_queues()
*   fap20m_api_auto_queues_get_info()
*   fap20m_api_auto_update_queues()
*   fap20m_api_auto_close_queues()
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_close_queues
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int dest_fap_id;
    unsigned int dest_port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dest_fap_id = (unsigned int)inArgs[0];
    dest_port_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_api_auto_close_queues(dest_fap_id, dest_port_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_get_port_auto_relative_id
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
*  Get a port ID, and returns the auto relative port ID.
*INPUT:
*  DIRECT:
*    IN  unsigned int port_id - 
*      The actual port ID.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    FAP20M_AUTO_PORT_RELATIVE_ID:
*      The auto relative ID.
*  INDIRECT:
*    None.
*REMARKS:
*   Might also be useful for looping over all the ports.
*SEE ALSO:
*   fap20m_api_auto_set_info_and_open_all_ports()
*   FAP20M_AUTO_CREDIT_SOURCE_IS_LOCAL_PORT
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_get_port_auto_relative_id
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    FAP20M_AUTO_PORT_RELATIVE_ID result;
    unsigned int port_id;
    

    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    port_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_api_auto_get_port_auto_relative_id(port_id);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", result);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_api_auto_get_agg_auto_relative_id
*TYPE:
*  PROC
*DATE: 
*  27-Oct-05
*FUNCTION:
* The function gets the aggregate's level and index, and return
* an FAP20M_AUTO_AGG_RELATIVE_ID, that indicates the aggregate
* place to the FAP20M_AUTO module.
*
* For example: if there is a port scheduler, and three
*   levels of aggregate schedulers beneath it,
*   one should do the call fap20m_auto_agg_id(3,0),
*   to get the aggregate scheduler logical ID.
*INPUT:
*  DIRECT:
*    IN  unsigned int agg_level,
*      The logical ID is determined by the aggregate place in
*       the hierarchy, which is the aggregate level and index.
*       the agg_level, is the aggregate depth in the schedulers trees.
*       Should be a number between 0 - 5.
*    IN  unsigned int agg_index -
*       agg_index is the aggregate index IN THE LEVEL.
*       Should be a number between 0 - 9.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    FAP20M_AUTO_AGG_RELATIVE_ID:
*      The auto relative ID.
*  INDIRECT:
*    None.
*REMARKS:
*   Might also be useful for looping over all the ports.
*SEE ALSO:
*   fap20m_api_auto_set_info_and_open_all_ports()
*   FAP20M_AUTO_CREDIT_SOURCE_IS_LOCAL_PORT
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_get_agg_auto_relative_id
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    FAP20M_AUTO_AGG_RELATIVE_ID result;
    unsigned int agg_level;
    unsigned int agg_index;
    

    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    agg_level = (unsigned int)inArgs[0];
    agg_index = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_api_auto_get_agg_auto_relative_id(agg_level, agg_index);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", result);

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_auto_get_agg_actual_id
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    FAP20M_AUTO_AGG_RELATIVE_ID agg_auto_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];
    agg_auto_id = (FAP20M_AUTO_AGG_RELATIVE_ID)inArgs[2];

    /* call Ocelot api function */
    result = fap20m_auto_get_agg_actual_id(device_id, port_id, agg_auto_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_auto_open_dest_port_queues
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int destination_fap_id;
    unsigned int destination_port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    destination_fap_id = (unsigned int)inArgs[0];
    destination_port_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_auto_open_dest_port_queues(destination_fap_id, destination_port_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_auto_open_port
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned int update_eg_port;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];
    update_eg_port = (unsigned int)inArgs[2];

    /* call Ocelot api function */
    result = fap20m_auto_open_port(device_id, port_id, update_eg_port);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_print_port_scheme_aggregates
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];

    /* call Ocelot api function */
    result = fap20m_api_auto_print_port_scheme_aggregates(device_id, port_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_print_port_scheme_flows
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned int minimal_printing;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];
    minimal_printing = (unsigned int)inArgs[2];

    /* call Ocelot api function */
    result = fap20m_api_auto_print_port_scheme_flows(device_id, port_id, minimal_printing);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  
*  fap20m_auto_aggregate_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_AUTO_AGGREGATE_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_auto_aggregate_sw_db_load
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_auto_aggregate_sw_db_load(sw_db_module_buff_to_fill);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_auto_aggregate_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_dev_index;
    unsigned int end_dev_index;
    unsigned int start_port_index;
    unsigned int end_port_index;
    unsigned int start_hierarcy_index;
    unsigned int end_hierarcy_index;
    unsigned int start_index;
    unsigned int end_index;
    unsigned int i, j, k, l;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    start_dev_index = (unsigned int)inArgs[0];
    end_dev_index = (unsigned int)inArgs[1];
    start_port_index = (unsigned int)inArgs[2];
    end_port_index = (unsigned int)inArgs[3];
    start_hierarcy_index = (unsigned int)inArgs[4];
    end_hierarcy_index = (unsigned int)inArgs[5];
    start_index = (unsigned int)inArgs[6];
    end_index = (unsigned int)inArgs[7];

    /* fill DB */
    for(i = start_dev_index; i <= end_dev_index; i++)
        for(j = start_port_index; j <= end_port_index; j++)
            for(k = start_hierarcy_index; k <= end_hierarcy_index; k++)
                for(l = start_index; l <= end_index; l++)
                {
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].sch.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].sch.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].sch.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];
                    
                    switch (sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].sch.type)
                    {
                        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
                            sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].sch.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
                            break;
                        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
                            sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].sch.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[4];
                            break;
                        default: break;
                    }

                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.id = (FAP20M_FLOW_ID)inFields[5];

                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].valid_mask = (unsigned long)inFields[6];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].id = (FAP20M_SUB_FLOW_ID)inFields[7];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[8];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].shaper.max_rate = (unsigned long)inFields[9];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].shaper.max_burst = (unsigned long)inFields[10];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE)inFields[11];

                    switch (sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].type)
                    {
                        case FAP20M_SUB_FLOW_TYPE_HR:
                            sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[12];
                            sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.hr.weight = (unsigned long)inFields[14];
                            break;
                        case FAP20M_SUB_FLOW_TYPE_CL:
                            sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[13];
                            sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.cl.weight = (unsigned long)inFields[14];
                            break;
                        default: break;
                    }

                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].valid_mask = (unsigned long)inFields[15];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].id = (FAP20M_SUB_FLOW_ID)inFields[16];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[17];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].shaper.max_rate = (unsigned long)inFields[18];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].shaper.max_burst = (unsigned long)inFields[19];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE)inFields[20];

                    switch (sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].type)
                    {
                        case FAP20M_SUB_FLOW_TYPE_HR:
                            sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[21];
                            sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.hr.weight = (unsigned long)inFields[23];
                            break;
                        case FAP20M_SUB_FLOW_TYPE_CL:
                            sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[22];
                            sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.cl.weight = (unsigned long)inFields[23];
                            break;
                        default: break;
                    }

                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.flow_type = (FAP20M_FLOW_TYPE)inFields[24];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].flow.is_slow_enabled = (unsigned int)inFields[25];
                    sw_db_module_buff_to_fill->fap20m_auto_agg_info[i][j].item[k][l].valid = (unsigned int) inFields[26];
                }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_auto_aggregate_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_AUTO_AGGREGATE_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_auto_aggregate_sw_db_save
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned short rc;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_auto_aggregate_sw_db_save(sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].sch.id;
    inFields[1] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].sch.type;
    inFields[2] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].sch.state;
    
    switch (sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }
                    
    inFields[5] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.id;
    
    inFields[6] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].valid_mask;
    inFields[7] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].id;
    inFields[8] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].credit_source;
    inFields[9] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].shaper.max_rate;
    inFields[10] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].shaper.max_burst;

    inFields[11] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].type;

    switch (sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].valid_mask;
    inFields[16] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].id;
    inFields[17] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].credit_source;
    inFields[18] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].shaper.max_rate;
    inFields[19] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].shaper.max_burst;

    inFields[20] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].type;

    switch (sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.flow_type;
    inFields[25] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].flow.is_slow_enabled;
    inFields[26] = sw_db_module_buff->fap20m_auto_agg_info[0][0].item[0][0].valid;
               
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", 
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                                inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                                inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                                inFields[15], inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23], inFields[24],
                                inFields[25], inFields[26]);
                
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_auto_aggregate_sw_db_save_end
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long i, j, k, l;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (++tbl_index >= SAND_MAX_DEVICE * FAP20M_NOF_DATA_PORTS * FAP20M_AUTO_NOF_HIERARCY * FAP20M_AUTO_NOF_INDEX)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* calculate table's indexes */
    l = tbl_index % FAP20M_AUTO_NOF_INDEX;
    k = (tbl_index / FAP20M_AUTO_NOF_INDEX) % FAP20M_AUTO_NOF_HIERARCY;
    j = (tbl_index / (FAP20M_AUTO_NOF_INDEX * FAP20M_AUTO_NOF_HIERARCY)) % FAP20M_NOF_DATA_PORTS;
    i = (tbl_index / (FAP20M_AUTO_NOF_INDEX * FAP20M_AUTO_NOF_HIERARCY * FAP20M_NOF_DATA_PORTS)) % SAND_MAX_DEVICE;

    /* pack output arguments to galtis string */
    inFields[0] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].sch.id;
    inFields[1] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].sch.type;
    inFields[2] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].sch.state;
    
    switch (sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }
                    
    inFields[5] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.id;
    
    inFields[6] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].valid_mask;
    inFields[7] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].id;
    inFields[8] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].credit_source;
    inFields[9] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].shaper.max_rate;
    inFields[10] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].shaper.max_burst;

    inFields[11] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].type;

    switch (sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].valid_mask;
    inFields[16] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].id;
    inFields[17] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].credit_source;
    inFields[18] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].shaper.max_rate;
    inFields[19] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].shaper.max_burst;

    inFields[20] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].type;

    switch (sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.flow_type;
    inFields[25] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].flow.is_slow_enabled;
    inFields[26] = sw_db_module_buff->fap20m_auto_agg_info[i][j].item[k][l].valid;
               
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", 
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                                inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                                inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                                inFields[15], inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23], inFields[24],
                                inFields[25], inFields[26]);
                
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_auto_flow_sw_fill_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int dev_start_index;
    unsigned int dev_end_index;
    unsigned int port_start_index;
    unsigned int port_end_index;
    unsigned int flow_start_index;
    unsigned int flow_end_index;
    unsigned int i, j, k;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    dev_start_index = (unsigned int)inArgs[0];
    dev_end_index = (unsigned int)inArgs[1];
    port_start_index = (unsigned int)inArgs[2];
    port_end_index = (unsigned int)inArgs[3];
    flow_start_index = (unsigned int)inArgs[4];
    flow_end_index = (unsigned int)inArgs[5];
    
    /* fill DB */
    for(i = dev_start_index; i <= dev_end_index; i++)
        for(j = port_start_index; j <= port_end_index; j++)
            for(k = flow_start_index; k <= flow_end_index; k++)
            {
                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].id = inFields[0];

                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].valid_mask = inFields[1];
                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].id = inFields[2];
                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].credit_source = inFields[3];
                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].shaper.max_rate = inFields[4];
                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].shaper.max_burst = inFields[5];

                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].type = inFields[6];

                switch (sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].type)
                {
                    case FAP20M_SUB_FLOW_TYPE_HR:
                        sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.hr.sp_class = inFields[7];
                        sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.hr.weight = inFields[9];
                        break;
                    case FAP20M_SUB_FLOW_TYPE_CL:
                        sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.cl.class_val = inFields[8];
                        sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.cl.weight = inFields[9];
                        break;
                    default: break;
                }

                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].valid_mask = inFields[10];
                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].id = inFields[11];
                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].credit_source = inFields[12];
                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].shaper.max_rate = inFields[13];
                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].shaper.max_burst = inFields[14];

                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].type = inFields[15];

                switch (sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].type)
                {
                    case FAP20M_SUB_FLOW_TYPE_HR:
                        sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.hr.sp_class = inFields[16];
                        sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.hr.weight = inFields[18];
                        break;
                    case FAP20M_SUB_FLOW_TYPE_CL:
                        sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.cl.class_val = inFields[17];
                        sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.cl.weight = inFields[18];
                        break;
                    default: break;
                }

                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].flow_type = inFields[19];
                sw_db_flow_module_buff_to_fill->fap20m_auto_flow_info[i][j].flows[k].is_slow_enabled = inFields[20];
            }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_auto_flow_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_AUTO_FLOW_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_auto_flow_sw_db_load
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_auto_flow_sw_db_load(sw_db_flow_module_buff_to_fill);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_auto_flow_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_AUTO_FLOW_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_auto_flow_sw_db_save
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_auto_flow_sw_db_save(sw_db_flow_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].id;

    inFields[1] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].valid_mask;
    inFields[2] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].id;
    inFields[3] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].credit_source;
    inFields[4] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].shaper.max_rate;
    inFields[5] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].shaper.max_burst;
    inFields[6] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].type;

    switch (sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[7] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[9] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[8] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].subflow_properties.cl.class_val;
            inFields[9] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[7] = -1;
            inFields[8] = -1;
            inFields[9] = -1;
    }
                               
    inFields[10] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[1].valid_mask;
    inFields[11] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[1].id;
    inFields[12] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[1].credit_source;
    inFields[13] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[1].shaper.max_rate;
    inFields[14] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[1].shaper.max_burst;
    inFields[15] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[1].type;

    switch (sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[16] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[18] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[17] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[1].subflow_properties.cl.class_val;
            inFields[18] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[16] = -1;
            inFields[17] = -1;
            inFields[18] = -1;
    }

    inFields[19] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].flow_type;
    inFields[20] = sw_db_flow_module_buff->fap20m_auto_flow_info[0][0].flows[0].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                                inFields[8],  inFields[9],  inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19], inFields[20]);
            
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_auto_flow_sw_db_save_end
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long i, j, k;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (++tbl_index >= SAND_MAX_DEVICE * FAP20M_NOF_DATA_PORTS * FAP20M_FABRIC_UNICAST_CLASSES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* calculate table's indexes */
    k = tbl_index % FAP20M_FABRIC_UNICAST_CLASSES;
    j = (tbl_index / FAP20M_FABRIC_UNICAST_CLASSES) % FAP20M_NOF_DATA_PORTS;
    i = (tbl_index / (FAP20M_NOF_DATA_PORTS * FAP20M_FABRIC_UNICAST_CLASSES)) % SAND_MAX_DEVICE;
    
    /* pack output arguments to galtis string */
    inFields[0] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].id;

    inFields[1] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].valid_mask;
    inFields[2] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].id;
    inFields[3] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].credit_source;
    inFields[4] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].shaper.max_rate;
    inFields[5] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].shaper.max_burst;
    inFields[6] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].type;

    switch (sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[7] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[9] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[8] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.cl.class_val;
            inFields[9] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[7] = -1;
            inFields[8] = -1;
            inFields[9] = -1;
    }

    inFields[10] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].valid_mask;
    inFields[11] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].id;
    inFields[12] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].credit_source;
    inFields[13] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].shaper.max_rate;
    inFields[14] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].shaper.max_burst;
    inFields[15] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].type;

    switch (sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[16] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[18] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[17] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.cl.class_val;
            inFields[18] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[16] = -1;
            inFields[17] = -1;
            inFields[18] = -1;
    }

    inFields[19] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].flow_type;
    inFields[20] = sw_db_flow_module_buff->fap20m_auto_flow_info[i][j].flows[k].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                                inFields[8],  inFields[9],  inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19], inFields[20]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_auto_mngr_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_AUTO_MNGR_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_auto_mngr_sw_db_load
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    FAP20M_AUTO_MNGR_SW_DB sw_db_module_buff;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    sw_db_module_buff.fap20m_auto_is_valid = (unsigned int)inFields[0];
    sw_db_module_buff.fap20m_auto_info.max_nof_faps_in_system = (unsigned int)inFields[1];
    sw_db_module_buff.fap20m_auto_info.max_nof_ports_per_fap = (unsigned int)inFields[2];
    sw_db_module_buff.fap20m_auto_info.is_fap10v = (unsigned int)inFields[3];
    sw_db_module_buff.fap20m_auto_info.max_nof_sub_flows_per_flow = (unsigned int)inFields[4];
    sw_db_module_buff.fap20m_auto_info.nof_fabric_classes = (FAP20M_FABRIC_CLASS)inFields[5];
    sw_db_module_buff.fap20m_auto_info.auto_first_queue_type = (FAP20M_QUEUE_TYPE)inFields[6];
    sw_db_module_buff.fap20m_auto_info.auto_last_queue_type = (FAP20M_QUEUE_TYPE)inFields[7];
    sw_db_module_buff.fap20m_auto_info.nof_faps_per_CPU = (unsigned int)inFields[8];
    
    sw_db_module_buff.fap20m_auto_info.device_info[0].local_fap_id = (unsigned int)inFields[9];
    sw_db_module_buff.fap20m_auto_info.device_info[1].local_fap_id = (unsigned int)inFields[10];
    sw_db_module_buff.fap20m_auto_info.device_info[2].local_fap_id = (unsigned int)inFields[11];
    sw_db_module_buff.fap20m_auto_info.device_info[3].local_fap_id = (unsigned int)inFields[12];
    sw_db_module_buff.fap20m_auto_info.device_info[4].local_fap_id = (unsigned int)inFields[13];
    sw_db_module_buff.fap20m_auto_info.device_info[5].local_fap_id = (unsigned int)inFields[14];
    sw_db_module_buff.fap20m_auto_info.device_info[6].local_fap_id = (unsigned int)inFields[15];
    sw_db_module_buff.fap20m_auto_info.device_info[7].local_fap_id = (unsigned int)inFields[16];
    
    sw_db_module_buff.fap20m_auto_info.fabric_sch_rank = (FAP20M_SCH_RANK)inFields[17];

    /* call Ocelot API function */
    result = fap20m_auto_mngr_sw_db_load(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_auto_mngr_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_AUTO_MNGR_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_auto_mngr_sw_db_save
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    FAP20M_AUTO_MNGR_SW_DB sw_db_module_buff;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_auto_mngr_sw_db_save(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = sw_db_module_buff.fap20m_auto_is_valid;
    inFields[1] = sw_db_module_buff.fap20m_auto_info.max_nof_faps_in_system;
    inFields[2] = sw_db_module_buff.fap20m_auto_info.max_nof_ports_per_fap;
    inFields[3] = sw_db_module_buff.fap20m_auto_info.is_fap10v;
    inFields[4] = sw_db_module_buff.fap20m_auto_info.max_nof_sub_flows_per_flow;
    inFields[5] = sw_db_module_buff.fap20m_auto_info.nof_fabric_classes;
    inFields[6] = sw_db_module_buff.fap20m_auto_info.auto_first_queue_type;
    inFields[7] = sw_db_module_buff.fap20m_auto_info.auto_last_queue_type;
    inFields[8] = sw_db_module_buff.fap20m_auto_info.nof_faps_per_CPU;
    
    inFields[9] = sw_db_module_buff.fap20m_auto_info.device_info[0].local_fap_id;
    inFields[10] = sw_db_module_buff.fap20m_auto_info.device_info[1].local_fap_id;
    inFields[11] = sw_db_module_buff.fap20m_auto_info.device_info[2].local_fap_id;
    inFields[12] = sw_db_module_buff.fap20m_auto_info.device_info[3].local_fap_id;
    inFields[13] = sw_db_module_buff.fap20m_auto_info.device_info[4].local_fap_id;
    inFields[14] = sw_db_module_buff.fap20m_auto_info.device_info[5].local_fap_id;
    inFields[15] = sw_db_module_buff.fap20m_auto_info.device_info[6].local_fap_id;
    inFields[16] = sw_db_module_buff.fap20m_auto_info.device_info[7].local_fap_id;
    
    inFields[17] = sw_db_module_buff.fap20m_auto_info.fabric_sch_rank;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                    inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],  inFields[5],
                    inFields[6],  inFields[7],  inFields[8],  inFields[9],  inFields[10], inFields[11],
                    inFields[12], inFields[13], inFields[14], inFields[15], inFields[16], inFields[17]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_auto_mngr_sw_db_save_end
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
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_auto_open_port_aggs
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];

    /* call Ocelot api function */
    result = fap20m_auto_open_port_aggs(device_id, port_id, p_aggregates_info);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = p_aggregates_info->item[0][0].sch.id;
    inFields[1] = p_aggregates_info->item[0][0].sch.type;
    inFields[2] = p_aggregates_info->item[0][0].sch.state;

    switch (p_aggregates_info->item[0][0].sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = p_aggregates_info->item[0][0].sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = p_aggregates_info->item[0][0].sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }

    inFields[5] = p_aggregates_info->item[0][0].flow.id;

    inFields[6] = p_aggregates_info->item[0][0].flow.sub_flow[0].valid_mask;
    inFields[7] = p_aggregates_info->item[0][0].flow.sub_flow[0].id;
    inFields[8] = p_aggregates_info->item[0][0].flow.sub_flow[0].credit_source;

    inFields[9] = p_aggregates_info->item[0][0].flow.sub_flow[0].shaper.max_rate;
    inFields[10] = p_aggregates_info->item[0][0].flow.sub_flow[0].shaper.max_burst;

    inFields[11] = p_aggregates_info->item[0][0].flow.sub_flow[0].type;

    switch (p_aggregates_info->item[0][0].flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = p_aggregates_info->item[0][0].flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = p_aggregates_info->item[0][0].flow.sub_flow[1].valid_mask;
    inFields[16] = p_aggregates_info->item[0][0].flow.sub_flow[1].id;
    inFields[17] = p_aggregates_info->item[0][0].flow.sub_flow[1].credit_source;

    inFields[18] = p_aggregates_info->item[0][0].flow.sub_flow[1].shaper.max_rate;
    inFields[19] = p_aggregates_info->item[0][0].flow.sub_flow[1].shaper.max_burst;

    inFields[20] = p_aggregates_info->item[0][0].flow.sub_flow[1].type;

    switch (p_aggregates_info->item[0][0].flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = p_aggregates_info->item[0][0].flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = p_aggregates_info->item[0][0].flow.flow_type;
    inFields[25] = p_aggregates_info->item[0][0].flow.is_slow_enabled;

    inFields[26] = p_aggregates_info->item[0][0].valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                                inFields[8],  inFields[9],  inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23],
                                inFields[24], inFields[25], inFields[26]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_auto_open_port_aggs_end
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

    if (++tbl_index >= FAP20M_AUTO_NOF_HIERARCY * FAP20M_AUTO_NOF_INDEX)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = (**(p_aggregates_info->item + tbl_index)).sch.id;
    inFields[1] = (**(p_aggregates_info->item + tbl_index)).sch.type;
    inFields[2] = (**(p_aggregates_info->item + tbl_index)).sch.state;

    switch ((**(p_aggregates_info->item + tbl_index)).sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = (**(p_aggregates_info->item + tbl_index)).sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = (**(p_aggregates_info->item + tbl_index)).sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }

    inFields[5] = (**(p_aggregates_info->item + tbl_index)).flow.id;

    inFields[6] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].valid_mask;
    inFields[7] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].id;
    inFields[8] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].credit_source;

    inFields[9] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].shaper.max_rate;
    inFields[10] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].shaper.max_burst;

    inFields[11] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].type;

    switch ((**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].valid_mask;
    inFields[16] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].id;
    inFields[17] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].credit_source;

    inFields[18] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].shaper.max_rate;
    inFields[19] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].shaper.max_burst;

    inFields[20] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].type;

    switch ((**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = (**(p_aggregates_info->item + tbl_index)).flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = (**(p_aggregates_info->item + tbl_index)).flow.flow_type;
    inFields[25] = (**(p_aggregates_info->item + tbl_index)).flow.is_slow_enabled;

    inFields[26] = (**(p_aggregates_info->item + tbl_index)).valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                                inFields[8],  inFields[9],  inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23],
                                inFields[24], inFields[25], inFields[26]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_auto_open_port_flows
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_auto_open_port_flows(device_id, port_id, exact_flows_info);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    tbl_index = 0;

    inFields[0] = exact_flows_info->flows[tbl_index].id;

    inFields[1] = exact_flows_info->flows[tbl_index].sub_flow[0].valid_mask;
    inFields[2] = exact_flows_info->flows[tbl_index].sub_flow[0].id;
    inFields[3] = exact_flows_info->flows[tbl_index].sub_flow[0].credit_source;
    inFields[4] = exact_flows_info->flows[tbl_index].sub_flow[0].shaper.max_rate;
    inFields[5] = exact_flows_info->flows[tbl_index].sub_flow[0].shaper.max_burst;

    inFields[6] = exact_flows_info->flows[tbl_index].sub_flow[0].type;

    switch (exact_flows_info->flows[tbl_index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[7] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[9] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[8] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.class_val;
            inFields[9] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[7] = -1;
            inFields[8] = -1;
            inFields[9] = -1;
    }

    inFields[10] = exact_flows_info->flows[tbl_index].sub_flow[1].valid_mask;
    inFields[11] = exact_flows_info->flows[tbl_index].sub_flow[1].id;
    inFields[12] = exact_flows_info->flows[tbl_index].sub_flow[1].credit_source;
    inFields[13] = exact_flows_info->flows[tbl_index].sub_flow[1].shaper.max_burst;
    inFields[14] = exact_flows_info->flows[tbl_index].sub_flow[1].shaper.max_rate;
    
    inFields[15] = exact_flows_info->flows[tbl_index].sub_flow[1].type;

    switch (exact_flows_info->flows[tbl_index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[16] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[18] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[17] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.class_val;
            inFields[18] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[16] = -1;
            inFields[17] = -1;
            inFields[18] = -1;
    }

    inFields[19] = exact_flows_info->flows[tbl_index].flow_type;
    inFields[20] = exact_flows_info->flows[tbl_index].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                        inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                        inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                        inFields[15], inFields[16], inFields[17], inFields[18], inFields[19], inFields[20]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_auto_open_port_flows_end
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

    if (++tbl_index >= FAP20M_FABRIC_UNICAST_CLASSES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = exact_flows_info->flows[tbl_index].id;

    inFields[1] = exact_flows_info->flows[tbl_index].sub_flow[0].valid_mask;
    inFields[2] = exact_flows_info->flows[tbl_index].sub_flow[0].id;
    inFields[3] = exact_flows_info->flows[tbl_index].sub_flow[0].credit_source;
    inFields[4] = exact_flows_info->flows[tbl_index].sub_flow[0].shaper.max_rate;
    inFields[5] = exact_flows_info->flows[tbl_index].sub_flow[0].shaper.max_burst;

    inFields[6] = exact_flows_info->flows[tbl_index].sub_flow[0].type;

    switch (exact_flows_info->flows[tbl_index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[7] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[9] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[8] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.class_val;
            inFields[9] = exact_flows_info->flows[tbl_index].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[7] = -1;
            inFields[8] = -1;
            inFields[9] = -1;
    }

    inFields[10] = exact_flows_info->flows[tbl_index].sub_flow[1].valid_mask;
    inFields[11] = exact_flows_info->flows[tbl_index].sub_flow[1].id;
    inFields[12] = exact_flows_info->flows[tbl_index].sub_flow[1].credit_source;
    inFields[13] = exact_flows_info->flows[tbl_index].sub_flow[1].shaper.max_burst;
    inFields[14] = exact_flows_info->flows[tbl_index].sub_flow[1].shaper.max_rate;
    
    inFields[15] = exact_flows_info->flows[tbl_index].sub_flow[1].type;

    switch (exact_flows_info->flows[tbl_index].sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[16] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[18] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[17] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.class_val;
            inFields[18] = exact_flows_info->flows[tbl_index].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[16] = -1;
            inFields[17] = -1;
            inFields[18] = -1;
    }

    inFields[19] = exact_flows_info->flows[tbl_index].flow_type;
    inFields[20] = exact_flows_info->flows[tbl_index].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                        inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                        inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                        inFields[15], inFields[16], inFields[17], inFields[18], inFields[19], inFields[20]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

static void prvFap20mAutoPortGetDefaultSchPort_HR_E
(
    IN FAP20M_ELEMENTARY_SCHEDULER p_sch_port,
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = p_sch_port.id;
    inFields[1] = p_sch_port.type;
    inFields[2] = p_sch_port.state;
    inFields[3] = p_sch_port.sched_properties.hr.mode;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 0);
}

static void prvFap20mAutoPortGetDefaultSchPort_CL_E
(
    IN FAP20M_ELEMENTARY_SCHEDULER p_sch_port,
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = p_sch_port.id;
    inFields[1] = p_sch_port.type;
    inFields[2] = p_sch_port.state;
    inFields[3] = p_sch_port.sched_properties.cl.class_type;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 1);
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_auto_port_get_default_scheduler_port
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    FAP20M_ELEMENTARY_SCHEDULER p_sch_port;
    unsigned int result;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot api function */
    result = fap20m_auto_port_get_default_scheduler_port(&p_sch_port);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    if (FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR == p_sch_port.type)
    {
        prvFap20mAutoPortGetDefaultSchPort_HR_E(p_sch_port, inFields, outArgs);
    } 
    else
    {
        prvFap20mAutoPortGetDefaultSchPort_CL_E(p_sch_port, inFields, outArgs);
    }
    
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_auto_port_get_default_scheduler_port_end
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
* 
*  fap20m_auto_port_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_AUTO_PORT_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_auto_port_sw_db_load_first
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int index, i, j;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    sand_os_memset((void*)sw_db_port_module_buff, 0, sizeof(sw_db_port_module_buff));

    /* map input arguments to locals */
    index = (unsigned int)inFields[0];
    j = index % FAP20M_NOF_DATA_PORTS;
    i = index / FAP20M_NOF_DATA_PORTS;

    sw_db_port_module_buff->fap20m_auto_port_info[i][j].eg_port.valid_mask = (unsigned long)inFields[1];
    sw_db_port_module_buff->fap20m_auto_port_info[i][j].eg_port.nominal_bandwidth = (unsigned long)inFields[2];
    sw_db_port_module_buff->fap20m_auto_port_info[i][j].eg_port.credit_bandwidth = (unsigned long)inFields[3];
    sw_db_port_module_buff->fap20m_auto_port_info[i][j].hr_mode = (FAP20M_HR_SCHED_MODE)inFields[4];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_auto_port_sw_db_load_next
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int index, i, j;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    index = (unsigned int)inFields[0];
    j = index % FAP20M_NOF_DATA_PORTS;
    i = index / FAP20M_NOF_DATA_PORTS;

    sw_db_port_module_buff->fap20m_auto_port_info[i][j].eg_port.valid_mask = (unsigned long)inFields[1];
    sw_db_port_module_buff->fap20m_auto_port_info[i][j].eg_port.nominal_bandwidth = (unsigned long)inFields[2];
    sw_db_port_module_buff->fap20m_auto_port_info[i][j].eg_port.credit_bandwidth = (unsigned long)inFields[3];
    sw_db_port_module_buff->fap20m_auto_port_info[i][j].hr_mode = (FAP20M_HR_SCHED_MODE)inFields[4];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_auto_port_sw_db_load_end
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_auto_port_sw_db_load(sw_db_port_module_buff);

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
* 
*  fap20m_auto_port_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_AUTO_PORT_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_auto_port_sw_db_save
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned short rc;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_auto_port_sw_db_save(sw_db_port_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = sw_db_port_module_buff->fap20m_auto_port_info[0][0].eg_port.valid_mask;
    inFields[2] = sw_db_port_module_buff->fap20m_auto_port_info[0][0].eg_port.nominal_bandwidth;
    inFields[3] = sw_db_port_module_buff->fap20m_auto_port_info[0][0].eg_port.credit_bandwidth;
    inFields[4] = sw_db_port_module_buff->fap20m_auto_port_info[0][0].hr_mode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1],  inFields[2], inFields[3], inFields[4]);
            
    galtisOutput(outArgs, rc, "%f");
    
    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_auto_port_sw_db_save_end
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int i, j;

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (++tbl_index >= FAP20M_NOF_DATA_PORTS * SAND_MAX_DEVICE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* calculate table's indexes */
    j = tbl_index % FAP20M_NOF_DATA_PORTS;
    i = tbl_index / FAP20M_NOF_DATA_PORTS;

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = sw_db_port_module_buff->fap20m_auto_port_info[i][j].eg_port.valid_mask;
    inFields[2] = sw_db_port_module_buff->fap20m_auto_port_info[i][j].eg_port.nominal_bandwidth;
    inFields[3] = sw_db_port_module_buff->fap20m_auto_port_info[i][j].eg_port.credit_bandwidth;
    inFields[4] = sw_db_port_module_buff->fap20m_auto_port_info[i][j].hr_mode;
    
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1],  inFields[2], inFields[3], inFields[4]);
    
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_auto_queue_sw_fill_queue_info_table
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_fap_index;
    unsigned int end_fap_index;
    unsigned int start_port_index;
    unsigned int end_port_index;
    unsigned int i, j;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    start_fap_index = (unsigned int) inArgs[0];
    end_fap_index = (unsigned int) inArgs[1];
    start_port_index = (unsigned int) inArgs[2];
    end_port_index = (unsigned int) inArgs[3];

    /* fill queue DB */
    for(i = start_fap_index; i <= end_fap_index; i++)
        for(j = start_port_index; j <= end_port_index; j++)
        {
            sw_db_queue_module_buff_to_fill->fap20m_auto_queue_info[i][j].queue_type[0] = (FAP20M_QUEUE_TYPE)inFields[0];
            sw_db_queue_module_buff_to_fill->fap20m_auto_queue_info[i][j].queue_type[1] = (FAP20M_QUEUE_TYPE)inFields[1];
            sw_db_queue_module_buff_to_fill->fap20m_auto_queue_info[i][j].queue_type[2] = (FAP20M_QUEUE_TYPE)inFields[2];
            sw_db_queue_module_buff_to_fill->fap20m_auto_queue_info[i][j].queue_type[3] = (FAP20M_QUEUE_TYPE)inFields[3];
            sw_db_queue_module_buff_to_fill->fap20m_auto_queue_info[i][j].queue_type[4] = (FAP20M_QUEUE_TYPE)inFields[4];
            sw_db_queue_module_buff_to_fill->fap20m_auto_queue_info[i][j].queue_type[5] = (FAP20M_QUEUE_TYPE)inFields[5];
            sw_db_queue_module_buff_to_fill->fap20m_auto_queue_info[i][j].queue_type[6] = (FAP20M_QUEUE_TYPE)inFields[6];
            sw_db_queue_module_buff_to_fill->fap20m_auto_queue_info[i][j].queue_type[7] = (FAP20M_QUEUE_TYPE)inFields[7];
        }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_auto_queue_sw_fill_queue_param_table
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_vacant_index;
    unsigned int end_vacant_index;
    unsigned int start_param_index;
    unsigned int end_param_index;
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    start_vacant_index = (unsigned int) inArgs[0];
    end_vacant_index = (unsigned int) inArgs[1];
    start_param_index = (unsigned int) inArgs[2];
    end_param_index = (unsigned int) inArgs[3];
    sw_db_queue_module_buff_to_fill->fap20m_auto_queue_type_param_db_is_valid = (unsigned int) inArgs[4];

    /* fill queue DB */
    for (i = start_vacant_index; i <= end_vacant_index; i++)
    {
        sw_db_queue_module_buff_to_fill->fap20m_auto_queue_type_param_db.vacant[i] = (unsigned int) inFields[0];
    }

    for (i = start_param_index; i <= end_param_index; i++)
    {
        sw_db_queue_module_buff_to_fill->fap20m_auto_queue_type_param_db.queue_type_params[i].RED_enable = (unsigned int)inFields[1];
        sw_db_queue_module_buff_to_fill->fap20m_auto_queue_type_param_db.queue_type_params[i].RED_exp_weight = (unsigned int)inFields[2];
        sw_db_queue_module_buff_to_fill->fap20m_auto_queue_type_param_db.queue_type_params[i].flow_slow_th_up = (unsigned long)inFields[3];
        sw_db_queue_module_buff_to_fill->fap20m_auto_queue_type_param_db.queue_type_params[i].flow_stat_msg = (unsigned long)inFields[4];
        sw_db_queue_module_buff_to_fill->fap20m_auto_queue_type_param_db.queue_type_params[i].max_q_size = (unsigned long)inFields[5];
        sw_db_queue_module_buff_to_fill->fap20m_auto_queue_type_param_db.queue_type_params[i].flow_slow_hys_en = (unsigned int)inFields[6];
        sw_db_queue_module_buff_to_fill->fap20m_auto_queue_type_param_db.queue_type_params[i].hysteresis_size = (unsigned long)inFields[7];
        sw_db_queue_module_buff_to_fill->fap20m_auto_queue_type_param_db.queue_type_params[i].header_compensation = (int)inFields[8];
    }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_auto_queue_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_AUTO_QUEUE_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_auto_queue_sw_db_load
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_auto_queue_sw_db_load(sw_db_queue_module_buff_to_fill);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

static void prvFap20mAutoQueueSwDb_QUEUE_INFO_E
(
    IN  GT_U32 index,
    IN  GT_32  inFields[CMD_MAX_FIELDS],
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    GT_U32 i, j;


    /* calculate tables's indexes */
    j = index % FAP20M_NOF_DATA_PORTS;
    i = index / FAP20M_NOF_DATA_PORTS;

    /* pack output arguments to galtis string */
    inFields[0] = sw_db_queue_module_buff->fap20m_auto_queue_info[i][j].queue_type[0];
    inFields[1] = sw_db_queue_module_buff->fap20m_auto_queue_info[i][j].queue_type[1];
    inFields[2] = sw_db_queue_module_buff->fap20m_auto_queue_info[i][j].queue_type[2];
    inFields[3] = sw_db_queue_module_buff->fap20m_auto_queue_info[i][j].queue_type[3];
    inFields[4] = sw_db_queue_module_buff->fap20m_auto_queue_info[i][j].queue_type[4];
    inFields[5] = sw_db_queue_module_buff->fap20m_auto_queue_info[i][j].queue_type[5];
    inFields[6] = sw_db_queue_module_buff->fap20m_auto_queue_info[i][j].queue_type[6];
    inFields[7] = sw_db_queue_module_buff->fap20m_auto_queue_info[i][j].queue_type[7];

    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 0);
}

static void prvFap20mAutoQueueSwDb_QUEUE_TYPE_PARAM_E
(
    IN  GT_U32 index,
    IN  GT_32  inFields[CMD_MAX_FIELDS],
    OUT GT_8   outArgs[CMD_MAX_BUFFER]
)
{
    /* pack output arguments to galtis string */
    inFields[0] = sw_db_queue_module_buff->fap20m_auto_queue_type_param_db.vacant[index];
    inFields[1] = sw_db_queue_module_buff->fap20m_auto_queue_type_param_db.queue_type_params[index].RED_enable;
    inFields[2] = sw_db_queue_module_buff->fap20m_auto_queue_type_param_db.queue_type_params[index].RED_exp_weight;
    inFields[3] = sw_db_queue_module_buff->fap20m_auto_queue_type_param_db.queue_type_params[index].flow_slow_th_up;
    inFields[4] = sw_db_queue_module_buff->fap20m_auto_queue_type_param_db.queue_type_params[index].flow_stat_msg;
    inFields[5] = sw_db_queue_module_buff->fap20m_auto_queue_type_param_db.queue_type_params[index].max_q_size;
    inFields[6] = sw_db_queue_module_buff->fap20m_auto_queue_type_param_db.queue_type_params[index].flow_slow_hys_en;
    inFields[7] = sw_db_queue_module_buff->fap20m_auto_queue_type_param_db.queue_type_params[index].hysteresis_size;
    inFields[8] = sw_db_queue_module_buff->fap20m_auto_queue_type_param_db.queue_type_params[index].header_compensation;

    fieldOutput("%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], 
                                      inFields[3], inFields[4], inFields[5], 
                                      inFields[6], inFields[7], inFields[8]);

    galtisOutput(outArgs, GT_OK, "%d%f", 1);
}

/*****************************************************
*NAME
* 
*  fap20m_auto_queue_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_AUTO_QUEUE_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_auto_queue_sw_db_save
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned short rc;
    

    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_auto_queue_sw_db_save(sw_db_queue_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    prvFap20mAutoQueueSwDb_QUEUE_TYPE_PARAM_E(tbl_index, inFields, outArgs);

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_auto_queue_sw_db_save_end
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

    if (tbl_index > FAP20M_AUTO_MAX_NOF_FAPS * FAP20M_NOF_DATA_PORTS + FAP20M_NOF_QUEUE_TYPES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    if (FAP20M_NOF_QUEUE_TYPES - 1 > tbl_index)
    {
        /* queue type param output */
        prvFap20mAutoQueueSwDb_QUEUE_TYPE_PARAM_E(tbl_index + 1, inFields, outArgs);
    }
    else
    {
        /* queue info output */
        prvFap20mAutoQueueSwDb_QUEUE_INFO_E(tbl_index - FAP20M_NOF_QUEUE_TYPES, inFields, outArgs);
    }

    tbl_index++;
    
    return CMD_OK;
}

#ifdef FAP20M_DEBUG
/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_api_auto_print_port_scheme
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    unsigned int port_id;
    unsigned int minimal_printing;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_id = (unsigned int)inArgs[1];
    minimal_printing = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_api_auto_print_port_scheme(device_id, port_id, minimal_printing);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
#endif /* FAP20M_DEBUG */


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"auto_system_info_defaultsGetFirst",
         &wrFap20m_api_auto_system_info_get_defaults,
         0, 0},
        {"auto_system_info_defaultsGetNext",
         &wrFap20m_api_auto_system_info_get_defaults_end,
         0, 0},

        {"auto_initSet",
         &wrFap20m_api_auto_init,
         0, 17},

        {"auto_port_defaultGetFirst",
         &wrFap20m_api_auto_port_get_default,
         1, 0},
        {"auto_port_defaultGetNext",
         &wrFap20m_api_auto_port_get_default_end,
         1, 0},

        {"auto_info_and_open_all_portsSet",
         &wrFap20m_api_auto_set_info_and_open_all_ports,
         0, 4},

        {"auto_port_infoSet",
         &wrFap20m_auto_port_set_info,
         2, 4},   
        {"auto_port_infoGetFirst",
         &wrFap20m_api_auto_port_get_info,
         2, 0},
        {"auto_port_infoGetNext",
         &wrFap20m_api_auto_port_get_info_end,
         2, 0},

        {"auto_update_portSet",
         &wrFap20m_api_auto_update_port,
         2, 4},
        {"auto_update_portGetFirst",
         &wrFap20m_api_auto_update_port_exact,
         0, 0},
        {"auto_update_portGetNext",
         &wrFap20m_api_auto_update_port_exact_end,
         0, 0},

        {"auto_close_port",
         &wrFap20m_api_auto_close_port,
         2, 0},

        {"auto_aggregates_defaultGetFirst",
         &wrFap20m_api_auto_aggregates_get_default,
         0, 0},
        {"auto_aggregates_defaultGetNext",
         &wrFap20m_api_auto_aggregates_get_default_end,
         0, 0},

        {"auto_info_and_open_all_aggregatesSet",
         &wrFap20m_api_auto_set_info_and_open_all_aggregates,
         0, (FAP20M_AUTO_NOF_HIERARCY * FAP20M_AUTO_NOF_INDEX)},

        {"auto_port_aggregates_infoGetFirst",
         &wrFap20m_api_auto_port_aggregates_get_info,
         2, 0},
        {"auto_port_aggregates_infoGetNext",
         &wrFap20m_api_auto_port_aggregates_get_info_end,
         2, 0},

        {"auto_update_port_aggregatesSet",
         &wrFap20m_api_auto_update_port_aggregates,
         2, 0},
        {"auto_update_port_aggregatesGetFirst",
         &wrFap20m_api_auto_update_port_aggregates_exact,
         0, 0},
        {"auto_update_port_aggregatesGetNext",
         &wrFap20m_api_auto_update_port_aggregates_exact_end,
         0, 0},
        {"auto_aggregates_fill_info_tableSet",
         &wrFap20m_api_auto_aggregates_fill_info_table,
         4, 27},
        {"auto_flow_info_fill_tableSet",
         &wrFap20m_api_auto_flow_info_fill_table,
         2, 21},

        {"auto_close_port_aggregates",
         &wrFap20m_api_auto_close_port_aggregates,
         2, 0},

        {"auto_flows_defaultGetFirst",
         &wrFap20m_api_auto_flows_get_default,
         0, 0},
        {"auto_flows_defaultGetNext",
         &wrFap20m_api_auto_flows_get_default_end,
         0, 0},

        {"auto_info_and_open_all_flowsSetFirst",
         &wrFap20m_api_auto_set_first_info_and_open_all_flows,
         0, 22},
        {"auto_info_and_open_all_flowsSetNext",
         &wrFap20m_api_auto_set_next_info_and_open_all_flows,
         0, 22},
        {"auto_info_and_open_all_flowsEndSet",
         &wrFap20m_api_auto_end_set_info_and_open_all_flows,
         0, 0},

        {"auto_flow_infoSetFirst",
         &wrFap20m_auto_flow_set_first_info,
         0, 22},
        {"auto_flow_infoSetNext",
         &wrFap20m_auto_flow_set_next_info,
         0, 22},
        {"auto_flow_infoEndSet",
         &wrFap20m_auto_flow_end_set_info,
         2, 0},
        {"auto_flow_infoGetFirst",
         &wrFap20m_api_auto_port_flows_get_info,
         2, 0},
        {"auto_flow_infoGetNext",
         &wrFap20m_api_auto_port_flows_get_info_end,
         2, 0},

        {"auto_update_port_flowsSetFirst",
         &wrFap20m_api_auto_update_port_flows_set_first,
         0, 22},
        {"auto_update_port_flowsSetNext",
         &wrFap20m_api_auto_update_port_flows_set_next,
         0, 22},
        {"auto_update_port_flowsEndSet",
         &wrFap20m_api_auto_update_port_flows_end_set,
         2, 0},
        {"auto_update_port_flowsGetFirst",
         &wrFap20m_api_auto_update_port_flows_exact,
         0, 0},
        {"auto_update_port_flowsGetNext",
         &wrFap20m_api_auto_update_port_flows_exact_end,
         0, 0},

        {"auto_close_port_flows",
         &wrFap20m_api_auto_close_port_flows,
         2, 0},

        {"auto_queue_type_params_defaultGetFirst",
         &wrFap20m_api_auto_queue_type_params_get_default,
         0, 0},
        {"auto_queue_type_params_defaultGetNext",
         &wrFap20m_api_auto_queue_type_params_get_default_end,
         0, 0},

        {"auto_get_next_vacant_queue_type_id",
         &wrFap20m_api_auto_get_next_vacant_queue_type_id,
         0, 0},

        {"auto_queue_paramsSet",
         &wrFap20m_api_auto_queue_set_queue_params, /* UPDATE */
         1, 8},
        {"auto_queue_paramsGetFirst",
         &wrFap20m_api_auto_queue_get_queue_params,
         1, 0},
        {"auto_queue_paramsGetNext",
         &wrFap20m_api_auto_queue_get_queue_params_end,
         1, 0},

        {"auto_queue_info_defaultGetFirst",
         &wrFap20m_api_auto_queue_info_get_default,
         1, 0},
        {"auto_queue_info_defaultGetNext",
         &wrFap20m_api_auto_queue_info_get_default_end,
         1, 0},

        {"auto_info_and_open_all_queuesSet",
         &wrFap20m_api_auto_set_info_and_open_all_queues,
         0, FAP20M_FABRIC_UNICAST_CLASSES},

        {"auto_queues_infoSet",
         &wrFap20m_auto_queues_set_info,
         2, 8},
        {"auto_queues_infoGetFirst",
         &wrFap20m_api_auto_queues_get_info,
         2, 0},
        {"auto_queues_infoGetNext",
         &wrFap20m_api_auto_queues_get_info_end,
         2, 0},

        {"auto_update_queuesSet",
         &wrFap20m_api_auto_update_queues,
         2, FAP20M_FABRIC_UNICAST_CLASSES},

        {"auto_close_queues",
         &wrFap20m_api_auto_close_queues,
         2, 0},
        {"auto_get_port_auto_relative_id",
         &wrFap20m_api_auto_get_port_auto_relative_id,
         1, 0},
        {"auto_get_agg_auto_relative_id",
         &wrFap20m_api_auto_get_agg_auto_relative_id,
         2, 0},
        {"auto_get_agg_actual_id",
         &wrFap20m_auto_get_agg_actual_id,
         3, 0},
        {"auto_open_dest_port_queues",
         &wrFap20m_auto_open_dest_port_queues,
         2, 0},
        {"auto_open_port",
         &wrFap20m_auto_open_port,
         3, 0},
        {"auto_print_port_scheme_aggregates",
         &wrFap20m_api_auto_print_port_scheme_aggregates,
         2, 0},
        {"auto_print_port_scheme_flows",
         &wrFap20m_api_auto_print_port_scheme_flows,
         3, 0},

        {"auto_aggregate_fill_sw_dbSet",
         &wrFap20m_auto_aggregate_fill_sw_db,
         8, 27},
        {"auto_aggregate_sw_dbSet",
         &wrFap20m_auto_aggregate_sw_db_load,
         0, 0},
        {"auto_aggregate_sw_dbGetFirst",
         &wrFap20m_auto_aggregate_sw_db_save,
         0, 0},
        {"auto_aggregate_sw_dbGetNext",
         &wrFap20m_auto_aggregate_sw_db_save_end,
         0, 0},

        {"auto_flow_sw_fill_dbSet",
         &wrFap20m_auto_flow_sw_fill_db,
         6, 21},
        {"auto_flow_sw_dbSet",
         &wrFap20m_auto_flow_sw_db_load,
         0, 0},
        {"auto_flow_sw_dbGetFirst",
         &wrFap20m_auto_flow_sw_db_save,
         0, 0},
        {"auto_flow_sw_dbGetNext",
         &wrFap20m_auto_flow_sw_db_save_end,
         0, 0},

        {"auto_mngr_sw_dbSet",
         &wrFap20m_auto_mngr_sw_db_load,
         0, 18},
        {"auto_mngr_sw_dbGetFirst",
         &wrFap20m_auto_mngr_sw_db_save,
         0, 0},
        {"auto_mngr_sw_dbGetNext",
         &wrFap20m_auto_mngr_sw_db_save_end,
         0, 0},

        {"auto_open_port_aggsGetFirst",
         &wrFap20m_auto_open_port_aggs,
         2, 0},
        {"auto_open_port_aggsGetNext",
         &wrFap20m_auto_open_port_aggs_end,
         2, 0},

        {"auto_open_port_flowsGetFirst",
         &wrFap20m_auto_open_port_flows,
         2, 0},
        {"auto_open_port_flowsGetNext",
         &wrFap20m_auto_open_port_flows_end,
         2, 0},

        {"auto_port_default_scheduler_portGetFirst",
         &wrFap20m_auto_port_get_default_scheduler_port,
         0, 0},
        {"auto_port_default_scheduler_portGetNext",
         &wrFap20m_auto_port_get_default_scheduler_port_end,
         0, 0},

        {"auto_port_sw_dbSetFirst",
         &wrFap20m_auto_port_sw_db_load_first,
         0, 5},
        {"auto_port_sw_dbSetNext",
         &wrFap20m_auto_port_sw_db_load_next,
         0, 5},
        {"auto_port_sw_dbEndSet",
         &wrFap20m_auto_port_sw_db_load_end,
         0, 0},
        {"auto_port_sw_dbGetFirst",
         &wrFap20m_auto_port_sw_db_save,
         0, 0},
        {"auto_port_sw_dbGetNext",
         &wrFap20m_auto_port_sw_db_save_end,
         0, 0},

        {"auto_queue_sw_fill_queue_info_tableSet",
         &wrFap20m_auto_queue_sw_fill_queue_info_table,
         4, 8},
        {"auto_queue_sw_fill_queue_param_tableSet",
         &wrFap20m_auto_queue_sw_fill_queue_param_table,
         5, 9},
        {"auto_queue_sw_dbSet",
         &wrFap20m_auto_queue_sw_db_load,
         0, 0},
        {"auto_queue_sw_dbGetFirst",
         &wrFap20m_auto_queue_sw_db_save,
         0, 0},
        {"auto_queue_sw_dbGetNext",
         &wrFap20m_auto_queue_sw_db_save_end,
         0, 0}
#ifdef FAP20M_DEBUG
        ,{"auto_print_port_scheme",
         &wrFap20m_api_auto_print_port_scheme,
         3, 0}
#endif /* FAP20M_DEBUG */
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_auto_management
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
GT_STATUS cmdLibInitFap20m_api_auto_management
(
    GT_VOID
)
{
    /* allocate data base */
    p_aggregates_info = 
        (FAP20M_AUTO_AGG_INFO *)cmdOsMalloc(sizeof(FAP20M_AUTO_AGG_INFO));
    if (p_aggregates_info==NULL)
    {
        return GT_NO_RESOURCE;
    }

    p_aggregates_info_to_fill = 
        (FAP20M_AUTO_AGG_INFO *)cmdOsMalloc(sizeof(FAP20M_AUTO_AGG_INFO));
    if (p_aggregates_info_to_fill==NULL)
    {
        return GT_NO_RESOURCE;
    }

    exact_aggregates_info = 
        (FAP20M_AUTO_AGG_INFO *)cmdOsMalloc(sizeof(FAP20M_AUTO_AGG_INFO));
    if (exact_aggregates_info==NULL)
    {
        return GT_NO_RESOURCE;
    }

    p_flows_info_to_fill = 
        (FAP20M_AUTO_FLOW_INFO *)cmdOsMalloc(sizeof(FAP20M_AUTO_FLOW_INFO));
    if (p_flows_info_to_fill==NULL)
    {
        return GT_NO_RESOURCE;
    }

    exact_flows_info = 
        (FAP20M_AUTO_FLOW_INFO *)cmdOsMalloc(sizeof(FAP20M_AUTO_FLOW_INFO));
    if (exact_flows_info==NULL)
    {
        return GT_NO_RESOURCE;
    }

    p_flows_info = 
        (FAP20M_AUTO_FLOW_INFO *)cmdOsMalloc(sizeof(FAP20M_AUTO_FLOW_INFO));
    if (p_flows_info==NULL)
    {
        return GT_NO_RESOURCE;
    }

    exact_port_info = 
        (FAP20M_AUTO_PORT_INFO *)cmdOsMalloc(sizeof(FAP20M_AUTO_PORT_INFO));
    if (exact_port_info==NULL)
    {
        return GT_NO_RESOURCE;
    }

    sw_db_module_buff_to_fill = 
        (FAP20M_AUTO_AGGREGATE_SW_DB *)cmdOsMalloc(sizeof(FAP20M_AUTO_AGGREGATE_SW_DB));
    if (sw_db_module_buff_to_fill==NULL)
    {
        return GT_NO_RESOURCE;
    }

    sw_db_module_buff = 
        (FAP20M_AUTO_AGGREGATE_SW_DB *)cmdOsMalloc(sizeof(FAP20M_AUTO_AGGREGATE_SW_DB));
    if (sw_db_module_buff==NULL)
    {
        return GT_NO_RESOURCE;
    }

    sw_db_flow_module_buff_to_fill = 
        (FAP20M_AUTO_FLOW_SW_DB *)cmdOsMalloc(sizeof(FAP20M_AUTO_FLOW_SW_DB));
    if (sw_db_flow_module_buff_to_fill==NULL)
    {
        return GT_NO_RESOURCE;
    }

    sw_db_flow_module_buff = 
        (FAP20M_AUTO_FLOW_SW_DB *)cmdOsMalloc(sizeof(FAP20M_AUTO_FLOW_SW_DB));
    if (sw_db_flow_module_buff==NULL)
    {
        return GT_NO_RESOURCE;
    }

    sw_db_port_module_buff = 
        (FAP20M_AUTO_PORT_SW_DB *)cmdOsMalloc(sizeof(FAP20M_AUTO_PORT_SW_DB));
    if (sw_db_port_module_buff==NULL)
    {
        return GT_NO_RESOURCE;
    }

    sw_db_queue_module_buff_to_fill = 
        (FAP20M_AUTO_QUEUE_SW_DB *)cmdOsMalloc(sizeof(FAP20M_AUTO_QUEUE_SW_DB));
    if (sw_db_queue_module_buff_to_fill==NULL)
    {
        return GT_NO_RESOURCE;
    }

    sw_db_queue_module_buff = 
        (FAP20M_AUTO_QUEUE_SW_DB *)cmdOsMalloc(sizeof(FAP20M_AUTO_QUEUE_SW_DB));
    if (sw_db_queue_module_buff==NULL)
    {
        return GT_NO_RESOURCE;
    }

    return cmdInitLibrary(dbCommands, numCommands);
}

