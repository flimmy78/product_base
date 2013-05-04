/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_spatial_multicast.c
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
#include <FX950/include/fap20m_api_spatial_multicast.h>
#include <FX950/include/fap20m_spatial_multicast.h>


/* Global variables */
static GT_U32 tbl_index;
static FAP20M_SPATIAL_MULTICAST_SW_DB sw_db_module_buff_to_fill;
static FAP20M_SPATIAL_MULTICAST_SW_DB sw_db_module_buff;


/*****************************************************
*NAME
*  fap20m_set_SMS_configuration
*TYPE:
*  PROC
*DATE: 
*  04-APR-05
*FUNCTION:
*   Set the SMS according to the sms_conf structure. The function 
*  output 'exact_sms_conf_given' that contains the actual parameters 
*  the SMS was configured (difference due to rounding).
*   If the rate or the busrt size are equal to ZERO, They would both
*  be set to ZERO.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_SMS_CONF* sms_conf -
*       Pointer to buffer of input parameters for this procedure.
*    FAP20M_SMS_CONF* exact_sms_conf_given -
*       Loaded with actual parameters loaded to the device.
*  INDIRECT:
*    SAND_NULL_POINTER_ERR
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
*  This configuration depends on the value of credit size!
*    If the credit size is changed, this configuration should
*    done again.
*  This is revB function. It should be used by owners of
*    fap20m_B0 chips. if you own fap20m_A chip, please
*    use the function fap20m_set_SMS_configuration.
*  The driver write to the device fields: SpatialQueue3CreditInterval,
*   SpatialQueue012CreditInterval, SpatialQueue3CreditMax,
*   and SpatailQueue012CreditMax.
*SEE ALSO:
* FAP20M_SMS_CONF
*****************************************************/
static CMD_STATUS wrFap20m_set_SMS_configuration
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SMS_CONF sms_conf;
    FAP20M_SMS_CONF exact_sms_conf_given;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    sms_conf.guaranteed_rate = (unsigned long)inFields[0];
    sms_conf.guaranteed_burst_size = (unsigned long)inFields[1];
    sms_conf.best_effort_rate = (unsigned long)inFields[2];
    sms_conf.best_effort_burst_size = (unsigned long)inFields[3];
    sms_conf.sync_with_unicast_priorities = (unsigned int)inFields[4];

    /* call Ocelot API function */
    result = fap20m_set_SMS_configuration(device_id, &sms_conf, &exact_sms_conf_given);

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
*  fap20m_get_SMS_configuration
*TYPE:
*  PROC
*DATE: 
*  02-Dec-04
*FUNCTION:
*  Get spatial multicast configuration.
*  The driver read from the device fields: SpatialQueue3CreditInterval,
*   SpatialQueue012CreditInterval, SpatialQueue3CreditMax,
*   and SpatailQueue012CreditMax.
*  When the burst size is ZERO, the rate become equal to ZERO too.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_SMS_CONF* sms_conf -
*       Pointer to buffer for this procedure 
*       to load configured SMS parameters 
*  INDIRECT:
*    SAND_NULL_POINTER_ERR
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
*  This configuration depends on the value of credit size!
*    If the credit size is changed, this configuration should
*    rechecked!
*  This is revB function. It should be used by owners of
*    fap20m_B0 chips. if you own fap20m_A chip, please
*    use the function fap20m_get_SMS_configuration.
*SEE ALSO:
* FAP20M_SMS_CONF
*****************************************************/
static CMD_STATUS wrFap20m_get_SMS_configuration
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SMS_CONF sms_conf;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_get_SMS_configuration(device_id, &sms_conf);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = sms_conf.guaranteed_rate;
    inFields[1] = sms_conf.guaranteed_burst_size;
    inFields[2] = sms_conf.best_effort_rate;
    inFields[3] = sms_conf.best_effort_burst_size;
    inFields[4] = sms_conf.sync_with_unicast_priorities;

    fieldOutput("%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_SMS_configuration_end
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
*  fap20m_open_spatial_multicast_flow
*TYPE:
*  PROC
*DATE: 
*  16-Dec-02
*FUNCTION:
*  Open spatial multicast flow.
*  The driver write to device tables:
*  Multicast Tree-Routing Table - MTR and 
*  Spatial Multicast Distribution Table - SMD.
*INPUT:
*  DIRECT:
*    unsigned int                   device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int                   multicast_id -
*       Multicast identifier to open as Spatial. Range from 0 to 8191.
*    FAP20M_SPATIAL_MULTICAST_FLOW* sp_flow -
*       Spatial flow parameters (queue and distribution bitmap). 
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
*
*  One should notice that the Spatial Multicast Distribution Table - SMD
*  is set with 0x1FF - distribute on all links.
*  The actual distribution is masked with the LnkActvMsk register.
*
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_open_spatial_multicast_flow
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int multicast_id;
    FAP20M_SPATIAL_MULTICAST_FLOW sp_flow;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    multicast_id = (unsigned int)inArgs[1];

    sp_flow.spatial_distribution_map = (unsigned long)inFields[0];
    sp_flow.spatial_distribution_map_op = (SAND_OP)inFields[1];
    sp_flow.links_active_mask = (unsigned long)inFields[2];

    /* call Ocelot API function */
    result = fap20m_open_spatial_multicast_flow(device_id, multicast_id, &sp_flow);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_update_spatial_multicast_flow
*TYPE:
*  PROC
*DATE: 
*  16-Dec-02
*FUNCTION:
*  Update spatial multicast flow.
*  The driver write to device tables:
*  Multicast Tree-Routing Table - MTR and 
*  Spatial Multicast Distribution Table - SMD.
*INPUT:
*  DIRECT:
*    unsigned int                   device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int                   multicast_id -
*       Spatial multicast identifier to update. Range from 0 to 8191.
*    FAP20M_SPATIAL_MULTICAST_FLOW* sp_flow -
*       Spatial flow parameters (queue and distribution bitmap). 
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
*
*  One should notice that the Spatial Multicast Distribution Table - SMD
*  is set with 0x1FF - distribute on all links.
*  The actual distribution is masked with the LnkActvMsk register.
*
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_update_spatial_multicast_flow
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int multicast_id;
    FAP20M_SPATIAL_MULTICAST_FLOW sp_flow;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    multicast_id = (unsigned int)inArgs[1];

    sp_flow.spatial_distribution_map = (unsigned long)inFields[0];
    sp_flow.spatial_distribution_map_op = (SAND_OP)inFields[1];
    sp_flow.links_active_mask = (unsigned long)inFields[2];

    /* call Ocelot API function */
    result = fap20m_update_spatial_multicast_flow(device_id, multicast_id, &sp_flow);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_spatial_multicast_flow_params
*TYPE:
*  PROC
*DATE: 
*  16-Dec-02
*FUNCTION:
*  Get spatial multicast flow parameters.
*  The driver read from the device tables:
*  Multicast Tree-Routing Table - MTR and
*  Spatial Multicast Distribution Table - SMD.
*INPUT:
*  DIRECT:
*    unsigned int                   device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int                   multicast_id -
*      Spatial multicast identifier to get. Range from 0 to 8191.
*    FAP20M_SPATIAL_MULTICAST_FLOW* sp_flow -
*      Spatial flow parameters. 
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          SAND_NULL_POINTER_ERR 
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    sp_flow
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_spatial_multicast_flow_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int multicast_id;
    FAP20M_SPATIAL_MULTICAST_FLOW sp_flow;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    multicast_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_get_spatial_multicast_flow_params(device_id, multicast_id, &sp_flow);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = sp_flow.spatial_distribution_map;
    inFields[1] = sp_flow.spatial_distribution_map_op;
    inFields[2] = sp_flow.links_active_mask;

    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_spatial_multicast_flow_params_end
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
*  fap20m_close_spatial_multicast_flow
*TYPE:
*  PROC
*DATE: 
*  16-Dec-02
*FUNCTION:
*  Close spatial multicast flow.
*  The driver write to the device tables:
*  Multicast Tree-Routing Table - MTR and
*  Spatial Multicast Distribution Table - SMD.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int     multicast_id -
*      Spatial multicast identifier to close. Range from 0 to 8191.
*      Note, the Spatial Distribution table has 2K entries.
*      I.e., the table is accessed with (multicast_id%2K).
*      E.g., multicast group 1 and 2049, if defined as spatial, 
*      will access, at runtime, the same distribution map.
*      This function will set the Spatial Distribution table to ZERO,
*      only if all the relevant entries are not spatial one.
*      In case there is a multicast group that use the Distribution map,
*      it will not be write.
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
static CMD_STATUS wrFap20m_close_spatial_multicast_flow
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int multicast_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    multicast_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_close_spatial_multicast_flow(device_id, multicast_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_spatial_multicast_active_links_get
*TYPE:
*  PROC
*DATE:
*  18/APR/2005
*FUNCTION:
*  This procedure gets the FAP links that are 
*  eligible for spatial multicast traffic
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long*   links_bitmap -
*      Loaded with a bitmap of the device links. if bit (0 - 23) is up, then its 
*      corresponding link (0 - 23) is eligible for spatial multicast distribution.
*  INDIRECT:
*    None.
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
*    links_bitmap
*REMARKS:
*  None.  
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_spatial_multicast_active_links_get
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long links_bitmap;
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
    result = fap20m_spatial_multicast_active_links_get(device_id, &links_bitmap);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", links_bitmap);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_spatial_multicast_active_links_set
*TYPE:
*  PROC
*DATE:
*  18/APR/2005
*FUNCTION:
*  This procedure sets the FAP links that are 
*  eligible for spatial multicast traffic
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    links_bitmap -
*      A bitmap of the device links. if bit (0 - 23) is up, then its 
*      corresponding link (0 - 23) is eligible for spatial multicast distribution.
*  INDIRECT:
*    None.
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
*  None.  
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_spatial_multicast_active_links_set
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long links_bitmap;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    links_bitmap = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_spatial_multicast_active_links_set(device_id, links_bitmap);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
 * See details in fap20m_api_spatial_multicast.c
 *****************************************************/
static CMD_STATUS wrFap20m_spatial_multicast_init
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    SAND_RET result;
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
    result = fap20m_spatial_multicast_init(device_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_spatial_multicast_set_conf_sync_with_unicast_priorities
*TYPE:
*  PROC
*DATE: 
*  18/07/2007
*FUNCTION:
*  improve prioritization between Q3,Unicast and Qs {0,1,2}
*INPUT:
*  DIRECT:
*    unsigned int   device_id
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    unsigned int   high_pri_queue_id
*      if sync_with_unicast_priorities_enabled==1, 
*      high_pri_queue_id will be treated as a high priority queue
*    unsigned int   best_effort0/1/2_queue_id
*      if sync_with_unicast_priorities_enabled==1, 
*      best_effort0/1/2_queue_id will be treated as a best effort queue
*    unsigned long   min_rate
*      rate in Mega-Bit-Sec 
*    unsigned int   sync_with_unicast_priorities_enabled
*      1 = Enable (high_pri_queue_id will be treated as a high priority queue and 
*          best_effort0/1/2_queue_id will be treated as a best effort priority queue)
*      2 = Disable
*OUTPUT:
*  DIRECT:
*    unsigned long - function's error word
*  INDIRECT:
*    None
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_spatial_multicast_set_conf_sync_with_unicast_priorities
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int high_pri_queue_id;
    unsigned int best_effort0_queue_id;
    unsigned int best_effort1_queue_id;
    unsigned int best_effort2_queue_id;
    unsigned long min_rate;
    unsigned int sync_with_unicast_priorities_enabled;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    high_pri_queue_id = (unsigned int)inArgs[1];
    best_effort0_queue_id = (unsigned int)inArgs[2];
    best_effort1_queue_id = (unsigned int)inArgs[3];
    best_effort2_queue_id = (unsigned int)inArgs[4];
    min_rate = (unsigned long)inArgs[5];
    sync_with_unicast_priorities_enabled = (unsigned int)inArgs[6];

    /* call Ocelot API function */
    result = fap20m_spatial_multicast_set_conf_sync_with_unicast_priorities(device_id,
                                      high_pri_queue_id, best_effort0_queue_id, 
                                      best_effort1_queue_id, best_effort2_queue_id, 
                                      min_rate, sync_with_unicast_priorities_enabled);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_spatial_multicast_get_conf_sync_with_unicast_priorities
*TYPE:
*  PROC
*DATE: 
*  18/07/2007
*FUNCTION:
*  improve prioritization between Q3,Unicast and Qs {0,1,2}
*INPUT:
*  DIRECT:
*    unsigned int   device_id
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    unsigned int   high_pri_queue_id
*      if sync_with_unicast_priorities_enabled==1, 
*      high_pri_queue_id will be treated as a high priority queue
*    unsigned int   best_effort0/1/2_queue_id
*      if sync_with_unicast_priorities_enabled==1, 
*      best_effort0/1/2_queue_id will be treated as a best effort queue
*    unsigned long   min_rate
*    unsigned int   sync_with_unicast_priorities_enabled
*      1 = Enable (high_pri_queue_id will be treated as a high priority queue and 
*          best_effort0/1/2_queue_id will be treated as a best effort priority queue)
*      2 = Disable
*OUTPUT:
*  DIRECT:
*    unsigned long - function's error word
*  INDIRECT:
*    None
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_spatial_multicast_get_conf_sync_with_unicast_priorities
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int high_pri_queue_id;
    unsigned int best_effort0_queue_id;
    unsigned int best_effort1_queue_id;
    unsigned int best_effort2_queue_id;
    unsigned long min_rate;
    unsigned int sync_with_unicast_priorities_enabled;
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
    result = fap20m_spatial_multicast_get_conf_sync_with_unicast_priorities(device_id, 
                            &high_pri_queue_id, &best_effort0_queue_id, 
                            &best_effort1_queue_id, &best_effort2_queue_id,
                            &min_rate, &sync_with_unicast_priorities_enabled);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d%d%d%d%d", 
                 high_pri_queue_id, best_effort0_queue_id, best_effort1_queue_id, 
                 best_effort2_queue_id, min_rate, sync_with_unicast_priorities_enabled);

    return CMD_OK;
}

/*****************************************************
*NAME
* fap20m_spatial_multicast_set_as_spatial
*TYPE:
*  PROC
*DATE: 
*  28/01/2004
*FUNCTION:
*  Set driver structure that specific multicast-id is spatial or not.
*INPUT:
*  DIRECT:
*    IN unsigned int device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    IN unsigned int multicast_id -
*      Multicast ID as given from the PP. Range 0:8191.
*    IN unsigned int spatial_indicator -
*      Indicator.
*      1 - Marks the entry as spatial.
*      0 - Marks the entry as NOT spatial (free).
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    None.
*  INDIRECT:
*    Change 'Fap20m_spatial_multicast_ids[device_id].taken_spatial_multicast_id'
*REMARKS:
*  The calling procedure should take semaphore of the device.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_spatial_multicast_set_as_spatial
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int device_id;
    unsigned int multicast_id;
    unsigned int spatial_indicator;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    multicast_id = (unsigned int)inArgs[1];
    spatial_indicator = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    fap20m_spatial_multicast_set_as_spatial(device_id, multicast_id, spatial_indicator);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* fap20m_spatial_multicast_get_spatial_share
*TYPE:
*  PROC
*DATE: 
*  28/01/2004
*FUNCTION:
*  Get driver structure for specific multicast-id,
*  whether spatial resource is free (all %2K share the same bitmap).
*INPUT:
*  DIRECT:
*    IN unsigned int device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    IN unsigned int multicast_id -
*      Multicast-id. Range 0:8191.
*  INDIRECT:
*    Read 'Fap20m_spatial_multicast_ids[device_id].taken_spatial_multicast_id'
*OUTPUT:
*  DIRECT:
*    unsigned int -
*      Indicator.
*      1 - bitmap resource is spatial.
*      0 - bitmap resource is NOT spatial.
*REMARKS:
*  The calling procedure should take semaphore of the device.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_spatial_multicast_get_spatial_share
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int is_spatial;
    unsigned int device_id;
    unsigned int multicast_id;
    

    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    multicast_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    is_spatial = fap20m_spatial_multicast_get_spatial_share(device_id, multicast_id);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d", is_spatial);

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_spatial_multicast_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_dev_index;
    unsigned int end_dev_index;
    unsigned int start_mc_index;
    unsigned int end_mc_index;
    unsigned int i, j;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    start_dev_index = (unsigned int)inArgs[0];
    end_dev_index = (unsigned int)inArgs[1];
    start_mc_index = (unsigned int)inArgs[2];
    end_mc_index = (unsigned int)inArgs[3];

    for(i = start_dev_index; i <= end_dev_index; i++)
        for(j = start_mc_index; j <= end_mc_index; j++)
        {
            sw_db_module_buff_to_fill.fap20m_spatial_multicast_ids[i].taken_spatial_multicast_id[j] = (unsigned long)inFields[0];
        }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_spatial_multicast_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_SPATIAL_MULTICAST_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_spatial_multicast_sw_db_load
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
    result = fap20m_spatial_multicast_sw_db_load(&sw_db_module_buff_to_fill);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_spatial_multicast_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_SPATIAL_MULTICAST_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_spatial_multicast_sw_db_save
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
    result = fap20m_spatial_multicast_sw_db_save(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = sw_db_module_buff.fap20m_spatial_multicast_ids[0].taken_spatial_multicast_id[tbl_index];
    inFields[1] = sw_db_module_buff.fap20m_spatial_multicast_ids[1].taken_spatial_multicast_id[tbl_index];
    inFields[2] = sw_db_module_buff.fap20m_spatial_multicast_ids[2].taken_spatial_multicast_id[tbl_index];
    inFields[3] = sw_db_module_buff.fap20m_spatial_multicast_ids[3].taken_spatial_multicast_id[tbl_index];
    inFields[4] = sw_db_module_buff.fap20m_spatial_multicast_ids[4].taken_spatial_multicast_id[tbl_index];
    inFields[5] = sw_db_module_buff.fap20m_spatial_multicast_ids[5].taken_spatial_multicast_id[tbl_index];
    inFields[6] = sw_db_module_buff.fap20m_spatial_multicast_ids[6].taken_spatial_multicast_id[tbl_index];
    inFields[7] = sw_db_module_buff.fap20m_spatial_multicast_ids[7].taken_spatial_multicast_id[tbl_index];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_spatial_multicast_sw_db_save_end
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

    if (++tbl_index >= FAP20M_MULTICAST_GROUPS_FROM_PP_LONGS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = sw_db_module_buff.fap20m_spatial_multicast_ids[0].taken_spatial_multicast_id[tbl_index];
    inFields[1] = sw_db_module_buff.fap20m_spatial_multicast_ids[1].taken_spatial_multicast_id[tbl_index];
    inFields[2] = sw_db_module_buff.fap20m_spatial_multicast_ids[2].taken_spatial_multicast_id[tbl_index];
    inFields[3] = sw_db_module_buff.fap20m_spatial_multicast_ids[3].taken_spatial_multicast_id[tbl_index];
    inFields[4] = sw_db_module_buff.fap20m_spatial_multicast_ids[4].taken_spatial_multicast_id[tbl_index];
    inFields[5] = sw_db_module_buff.fap20m_spatial_multicast_ids[5].taken_spatial_multicast_id[tbl_index];
    inFields[6] = sw_db_module_buff.fap20m_spatial_multicast_ids[6].taken_spatial_multicast_id[tbl_index];
    inFields[7] = sw_db_module_buff.fap20m_spatial_multicast_ids[7].taken_spatial_multicast_id[tbl_index];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"SMS_configurationSet",
         &wrFap20m_set_SMS_configuration,
         1, 5},
        
        {"SMS_configurationGetFirst",
         &wrFap20m_get_SMS_configuration,
         1, 0},
        {"SMS_configurationGetNext",
         &wrFap20m_get_SMS_configuration_end,
         1, 0},

        {"open_spatial_multicast_flowSet",
         &wrFap20m_open_spatial_multicast_flow,
         2, 3},
        {"spatial_multicast_flow_paramsSet",
         &wrFap20m_update_spatial_multicast_flow,
         2, 3},
        {"spatial_multicast_flow_paramsGetFirst",
         &wrFap20m_get_spatial_multicast_flow_params,
         2, 0},
        {"spatial_multicast_flow_paramsGetNext",
         &wrFap20m_get_spatial_multicast_flow_params_end,
         2, 0},

        {"spatial_multicast_flow_close",
         &wrFap20m_close_spatial_multicast_flow,
         2, 0},
        {"spatial_multicast_active_links_get",
         &wrFap20m_spatial_multicast_active_links_get,
         1, 0},
        {"spatial_multicast_active_links_set",
         &wrFap20m_spatial_multicast_active_links_set,
         2, 0},
        {"spatial_multicast_init",
         &wrFap20m_spatial_multicast_init,
         1, 0},
        {"spatial_multicast_set_as_spatial",
         &wrFap20m_spatial_multicast_set_as_spatial,
         3, 0},
        {"spatial_multicast_get_spatial_share",
         &wrFap20m_spatial_multicast_get_spatial_share,
         2, 0},
        {"spatial_multicast_set_conf_sync_with_uc_priorities",
         &wrFap20m_spatial_multicast_set_conf_sync_with_unicast_priorities,
         7, 0},
        {"spatial_multicast_get_conf_sync_with_uc_priorities",
         &wrFap20m_spatial_multicast_get_conf_sync_with_unicast_priorities,
         1, 0},

        {"spatial_multicast_fill_sw_dbSet",
         &wrFap20m_spatial_multicast_fill_sw_db,
         4, 1},
        {"spatial_multicast_sw_dbSet",
         &wrFap20m_spatial_multicast_sw_db_load,
         0, 0},
        {"spatial_multicast_sw_dbGetFirst",
         &wrFap20m_spatial_multicast_sw_db_save,
         0, 0},
        {"spatial_multicast_sw_dbGetNext",
         &wrFap20m_spatial_multicast_sw_db_save_end,
         0, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_spatial_multicast
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
GT_STATUS cmdLibInitFap20m_api_spatial_multicast
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

