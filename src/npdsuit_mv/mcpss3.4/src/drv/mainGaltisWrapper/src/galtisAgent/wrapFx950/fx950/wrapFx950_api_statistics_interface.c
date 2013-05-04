/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_statistics_interface.c
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
#include <FX950/include/fx950_api_statistics_interface.h>


static GT_U32 index;
static FX950_STATISTICS_INTERFACE_ALL_COUNTERS all_counters;

/*****************************************************
*NAME
*  fx950_api_statistics_interface_set_ingress_cfg
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure sets Ingress Statistics parameters. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_STATISTICS_INTERFACE_INGRESS_CFG_STRUCT  *ingr_stat_cfg_ptr -
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
*   None.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_set_ingress_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_STATISTICS_INTERFACE_INGRESS_CFG_STRUCT ingr_stat_cfg_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    ingr_stat_cfg_ptr.message_include_vid = (unsigned int)inFields[0];
    ingr_stat_cfg_ptr.statistics_enable = (unsigned int)inFields[1];
    ingr_stat_cfg_ptr.link_dev_num = (unsigned long)inFields[2];
    ingr_stat_cfg_ptr.eq_const_operation = (FX950_STATISTICS_INTERFACE_OPERATION_TYPE)inFields[3];
    ingr_stat_cfg_ptr.eq_const_bc = (unsigned long)inFields[4];
    ingr_stat_cfg_ptr.dq_const_operation = (FX950_STATISTICS_INTERFACE_OPERATION_TYPE)inFields[5];
    ingr_stat_cfg_ptr.dq_const_bc = (unsigned long)inFields[6];

    /* call Ocelot API function */
    result = fx950_api_statistics_interface_set_ingress_cfg(device_id, &ingr_stat_cfg_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_statistics_interface_get_ingress_cfg
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets Ingress Statistics parameters. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
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
*    ingr_stat_cfg_ptr -
*       Pointer to Ingress Statistics parameters
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_get_ingress_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_STATISTICS_INTERFACE_INGRESS_CFG_STRUCT ingr_stat_cfg_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_statistics_interface_get_ingress_cfg(device_id, &ingr_stat_cfg_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = ingr_stat_cfg_ptr.message_include_vid;
    inFields[1] = ingr_stat_cfg_ptr.statistics_enable;
    inFields[2] = ingr_stat_cfg_ptr.link_dev_num;
    inFields[3] = ingr_stat_cfg_ptr.eq_const_operation;
    inFields[4] = ingr_stat_cfg_ptr.eq_const_bc;
    inFields[5] = ingr_stat_cfg_ptr.dq_const_operation;
    inFields[6] = ingr_stat_cfg_ptr.dq_const_bc;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                  inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_get_ingress_cfg_end
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
*  fx950_api_statistics_interface_set_mac_params
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure sets Ingress/Egress Statistics MAC parameters. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_DIRECTION_TYPE   direction -
*       Type of direction: Ingress or Egress
*    FX950_STATISTICS_INTERFACE_MAC_PARAM_STRUCT *stat_mac_param_ptr -
*       Ingress/Egress Statistics MAC parameters
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
*   None.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_set_mac_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_DIRECTION_TYPE direction;
    FX950_STATISTICS_INTERFACE_MAC_PARAM_STRUCT stat_mac_param_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    direction = (FX950_DIRECTION_TYPE)inArgs[1];

    stat_mac_param_ptr.data_cycle_threshold = (unsigned long)inFields[0]; 
    stat_mac_param_ptr.idle_cycle_threshold = (unsigned long)inFields[1];

    /* call Ocelot API function */
    result = fx950_api_statistics_interface_set_mac_params(device_id, direction, &stat_mac_param_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_statistics_interface_get_mac_params
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets Ingress/Egress Statistics MAC parameters. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_DIRECTION_TYPE   direction -
*       Type of direction: Ingress or Egress
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
*    *stat_mac_param_ptr -
*       Pointer to Ingress/Egress Statistics MAC parameters
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_get_mac_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_DIRECTION_TYPE direction;
    FX950_STATISTICS_INTERFACE_MAC_PARAM_STRUCT stat_mac_param_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    direction = (FX950_DIRECTION_TYPE)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_statistics_interface_get_mac_params(device_id, direction, &stat_mac_param_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = stat_mac_param_ptr.data_cycle_threshold;
    inFields[1] = stat_mac_param_ptr.idle_cycle_threshold;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_get_mac_params_end
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
*  fx950_api_statistics_interface_get_counter
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets Statistics Interface Ingress/Egress counter value 
*  according to counter type.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_STATISTICS_INTERFACE_COUNTER_TYPE   counter_type -
*      One of possible Statistics Interface counters
*    FX950_DIRECTION_TYPE   direction -
*       Type of direction: Ingress or Egress
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
*    result_ptr - 
*       Pointer to Counter value
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*  + Parity Error counter is only for ingress traffic.
*  + The counters are cleared on read.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_get_counter
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_STATISTICS_INTERFACE_COUNTER_TYPE counter_type;
    FX950_DIRECTION_TYPE direction;
    SAND_64CNT result_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;
    
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    counter_type = (FX950_STATISTICS_INTERFACE_COUNTER_TYPE)inArgs[1];
    direction = (FX950_DIRECTION_TYPE)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_statistics_interface_get_counter(device_id, counter_type, 
                                                        direction, &result_ptr);

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
static CMD_STATUS wrFx950_api_statistics_interface_get_counter_end
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
*  fx950_api_statistics_interface_get_all_counters
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets all Statistics Interface Ingress/Egress counters.
*
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_DIRECTION_TYPE   direction -
*       Type of direction: Ingress or Egress
*    FX950_STATISTICS_ALL_HGL_COUNTERS* all_hgl_statistic_counters_ptr -
*      Pointer to structure to store all hgl counters.
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
*    all_counters_ptr - 
*       Pointer to Statistics Interface counters value
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*  + This procedure read all counters from device,
*    so the latest counter data is read.
*  + Parity Error counter is only for ingress traffic.
*  + The counters are cleared on read.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_get_all_counters
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_DIRECTION_TYPE direction;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    direction = (FX950_DIRECTION_TYPE)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_statistics_interface_get_all_counters(device_id, direction, &all_counters);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = all_counters.counters[index].u64.arr[0];
    inFields[1] = all_counters.counters[index].u64.arr[1];
    inFields[2] = all_counters.counters[index].overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);
    
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_get_all_counters_end
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

    if (++index >= FX950_STATISTICS_INTERFACE_NOF_COUNTER_TYPES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = all_counters.counters[index].u64.arr[0];
    inFields[1] = all_counters.counters[index].u64.arr[1];
    inFields[2] = all_counters.counters[index].overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);
    
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_statistics_interface_set_egress_cfg
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure sets Egress Statistics parameters. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_STATISTICS_INTERFACE_EGRESS_CFG_STRUCT  *egr_stat_cfg_ptr -
*       Egress Statistics parameters
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
*   None.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_set_egress_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_STATISTICS_INTERFACE_EGRESS_CFG_STRUCT egr_stat_cfg_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    egr_stat_cfg_ptr.message_include_vid = (unsigned long)inFields[0];
    egr_stat_cfg_ptr.const_operation = (FX950_STATISTICS_INTERFACE_OPERATION_TYPE)inFields[1];
    egr_stat_cfg_ptr.const_bc = (unsigned long)inFields[2];
    egr_stat_cfg_ptr.send_statistics_type = (FX950_STATISTICS_INTERFACE_SEND_STAT_TYPE)inFields[3];

    /* call Ocelot API function */
    result = fx950_api_statistics_interface_set_egress_cfg(device_id, &egr_stat_cfg_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_statistics_interface_get_egress_cfg
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets Egress Statistics parameters. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
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
*    *egr_stat_cfg_ptr - 
*       Egress Statistics parameters
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_get_egress_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_STATISTICS_INTERFACE_EGRESS_CFG_STRUCT egr_stat_cfg_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_statistics_interface_get_egress_cfg(device_id, &egr_stat_cfg_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = egr_stat_cfg_ptr.message_include_vid;
    inFields[1] = egr_stat_cfg_ptr.const_operation;
    inFields[2] = egr_stat_cfg_ptr.const_bc;
    inFields[3] = egr_stat_cfg_ptr.send_statistics_type;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_statistics_interface_get_egress_cfg_end
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


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"statistics_interface_ingress_cfgSet",
         &wrFx950_api_statistics_interface_set_ingress_cfg,
         1, 7},
        {"statistics_interface_ingress_cfgGetFirst",
         &wrFx950_api_statistics_interface_get_ingress_cfg,
         1, 0},
        {"statistics_interface_ingress_cfgGetNext",
         &wrFx950_api_statistics_interface_get_ingress_cfg_end,
         1, 0},
        {"statistics_interface_mac_paramsSet",
         &wrFx950_api_statistics_interface_set_mac_params,
         2, 2},
        {"statistics_interface_mac_paramsGetFirst",
         &wrFx950_api_statistics_interface_get_mac_params,
         2, 0},
        {"statistics_interface_mac_paramsGetNext",
         &wrFx950_api_statistics_interface_get_mac_params_end,
         2, 0},
        {"statistics_interface_counterGetFirst",
         &wrFx950_api_statistics_interface_get_counter,
         3, 0},
        {"statistics_interface_counterGetNext",
         &wrFx950_api_statistics_interface_get_counter_end,
         3, 0},
        {"statistics_interface_all_countersGetFirst",
         &wrFx950_api_statistics_interface_get_all_counters,
         2, 0},
        {"statistics_interface_all_countersGetNext",
         &wrFx950_api_statistics_interface_get_all_counters_end,
         2, 0},
        {"statistics_interface_egress_cfgSet",
         &wrFx950_api_statistics_interface_set_egress_cfg,
         1, 4},
        {"statistics_interface_egress_cfgGetFirst",
         &wrFx950_api_statistics_interface_get_egress_cfg,
         1, 0},
        {"statistics_interface_egress_cfgGetNext",
         &wrFx950_api_statistics_interface_get_egress_cfg_end,
         1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_statistics_interface
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
CMD_STATUS cmdLibInitFx950_api_statistics_interface
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

