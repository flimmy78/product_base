/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_statistics.c
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
#include <FX950/include/fx950_api_statistics.h>


static GT_U32 index;
static FX950_STATISTICS_ALL_HGL_COUNTERS all_hgl_statistic_counters;
static FX950_STATISTICS_ALL_GLOBAL_COUNTERS all_global_statistic_counters;
static FX950_STATISTICS_ALL_HGS_COUNTERS all_hgs_statistic_counters;

/*****************************************************
*NAME
*  fx950_api_statistics_get_global_counter
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets global counter value according to counter type.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_STATISTICS_GLOBAL_COUNTER_TYPE   global_counter_type -
*      One of possible global counters 
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
static CMD_STATUS wrFx950_api_statistics_get_global_counter
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_STATISTICS_GLOBAL_COUNTER_TYPE global_counter_type;
    SAND_64CNT result_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    global_counter_type = (FX950_STATISTICS_GLOBAL_COUNTER_TYPE)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_statistics_get_global_counter(device_id, global_counter_type, &result_ptr);

    rc = sand_get_error_code_from_error_word(result);

    inFields[0] = result_ptr.u64.arr[0];
    inFields[1] = result_ptr.u64.arr[1];
    inFields[2] = result_ptr.overflowed;

    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_statistics_get_global_counter_end
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
*  fx950_api_statistics_get_hgl_counter
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets Cell Reassembly counter value according to counter type,
*  per link.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_STATISTICS_HGL_COUNTER_TYPE   hgl_counter_type -
*      One of possible Cell Reassembly counters
*    unsigned long    link_number - 
*       link number (0..1)
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
static CMD_STATUS wrFx950_api_statistics_get_hgl_counter
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_STATISTICS_HGL_COUNTER_TYPE hgl_counter_type;
    unsigned long link_number;
    SAND_64CNT result_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    hgl_counter_type = (FX950_STATISTICS_HGL_COUNTER_TYPE)inArgs[1];
    link_number = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_statistics_get_hgl_counter(device_id, hgl_counter_type, link_number, &result_ptr);

    rc = sand_get_error_code_from_error_word(result);

    inFields[0] = result_ptr.u64.arr[0];
    inFields[1] = result_ptr.u64.arr[1];
    inFields[2] = result_ptr.overflowed;

    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_statistics_get_hgl_counter_end
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
*  fx950_api_statistics_get_all_global_counters
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets all global counters.
*
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_STATISTICS_ALL_GLOBAL_COUNTERS*    
*                   all_global_statistic_counters_ptr -
*      Pointer to structure to store all global counters.
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
static CMD_STATUS wrFx950_api_statistics_get_all_global_counters
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
    result = fx950_api_statistics_get_all_global_counters(device_id, &all_global_statistic_counters);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = all_global_statistic_counters.counters[index].u64.arr[0];
    inFields[1] = all_global_statistic_counters.counters[index].u64.arr[1];
    inFields[2] = all_global_statistic_counters.counters[index].overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFx950_api_statistics_get_all_global_counters_end
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

    if (++index >= FX950_STATISTICS_GLOBAL_NOF_COUNTER_TYPES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = all_global_statistic_counters.counters[index].u64.arr[0];
    inFields[1] = all_global_statistic_counters.counters[index].u64.arr[1];
    inFields[2] = all_global_statistic_counters.counters[index].overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_statistics_get_all_hgl_counters
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets all Cell Reassembly counters, per link.
*
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    link_number - 
*       link number (0..1)
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
*    result_ptr
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*  + This procedure read all counters from device,
*    so the latest counter data is read.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_get_all_hgl_counters
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    unsigned long link_number;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_number = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_statistics_get_all_hgl_counters(device_id, link_number, &all_hgl_statistic_counters);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = all_hgl_statistic_counters.counters[index].u64.arr[0];
    inFields[1] = all_hgl_statistic_counters.counters[index].u64.arr[1];
    inFields[2] = all_hgl_statistic_counters.counters[index].overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFx950_api_statistics_get_all_hgl_counters_end
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

    if (++index >= FX950_STATISTICS_HGL_NOF_COUNTER_TYPES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = all_hgl_statistic_counters.counters[index].u64.arr[0];
    inFields[1] = all_hgl_statistic_counters.counters[index].u64.arr[1];
    inFields[2] = all_hgl_statistic_counters.counters[index].overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_statistics_get_hgs_port_mac_mib_counter
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets Hyper.G.Stack MIB counter value according to counter type.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_STATISTICS_HGS_COUNTER_TYPE   hgs_counter_type -
*      One of possible HyperG. Stack counters
*    unsigned long    port - 
*       Hyper.G.Stack port number (0..1)
*    unsigned int  read_from_capture_area -
*       When read_from_capture_area = TRUE, the counter is read from capture area
*       When read_from_capture_area = FALSE, the counter is read from it's regular
*       address.
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
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_get_hgs_port_mac_mib_counter
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_STATISTICS_HGS_COUNTER_TYPE hgs_counter_type;
    unsigned long port;
    unsigned int read_from_capture_area;
    SAND_64CNT result_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    hgs_counter_type = (FX950_STATISTICS_HGS_COUNTER_TYPE)inArgs[1];
    port = (unsigned long)inArgs[2];
    read_from_capture_area = (unsigned int)inArgs[3];

    /* call Ocelot API function */
    result = fx950_api_statistics_get_hgs_port_mac_mib_counter(device_id, hgs_counter_type, port,
                                                               read_from_capture_area, &result_ptr);

    rc = sand_get_error_code_from_error_word(result);

    inFields[0] = result_ptr.u64.arr[0];
    inFields[1] = result_ptr.u64.arr[1];
    inFields[2] = result_ptr.overflowed;

    /* pack output arguments to galtis string */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_statistics_get_hgs_port_mac_mib_counter_end
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
*  fx950_api_statistics_get_all_hgs_counters
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets all Hyper.G.Stack MIB counters, per port.
*
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port - 
*       link number (0..1)
*    unsigned int  read_from_capture_area -
*       When read_from_capture_area = TRUE, the counters are read from capture area
*       When read_from_capture_area = FALSE, the counters are read from it's regular
*       address.
*    FX950_STATISTICS_ALL_HGS_COUNTERS* all_hgs_statistic_counters_ptr -
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
*    all_hgs_statistic_counters_ptr - 
*       Pointer to Hyper.G Stack counters value.
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*  + This procedure read all counters from device,
*    so the latest counter data is read.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_get_all_hgs_counters
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long port;
    unsigned int read_from_capture_area;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];
    read_from_capture_area = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_statistics_get_all_hgs_counters(device_id, port, read_from_capture_area,
                                                       &all_hgs_statistic_counters);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = all_hgs_statistic_counters.counters[index].u64.arr[0];
    inFields[1] = all_hgs_statistic_counters.counters[index].u64.arr[1];
    inFields[2] = all_hgs_statistic_counters.counters[index].overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFx950_api_statistics_get_all_hgs_counters_end
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

    if (++index >= FX950_STATISTICS_HGS_NOF_COUNTER_TYPES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = all_hgs_statistic_counters.counters[index].u64.arr[0];
    inFields[1] = all_hgs_statistic_counters.counters[index].u64.arr[1];
    inFields[2] = all_hgs_statistic_counters.counters[index].overflowed;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_statistics_set_port_mac_mib_counter_cfg
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure sets network port MAC MIB Counters configuration.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Stack port number (0..1)
*    FX950_HGS_INTERFACE_PORT_MAC_MIB_COUNTER_CFG  *mac_mib_counter_cfg_ptr -
*       Pointer to Port MAC MIB Counters configuration.
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
static CMD_STATUS wrFx950_api_statistics_set_port_mac_mib_counter_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long port;
    FX950_STATISTICS_HGS_INTERFACE_PORT_MAC_MIB_COUNTER_CFG mac_mib_counter_cfg;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned int)inArgs[1];

    mac_mib_counter_cfg.mib_cnt_disable = (unsigned int)inFields[0];
    mac_mib_counter_cfg.rx_histogram_enable = (unsigned int)inFields[1];
    mac_mib_counter_cfg.tx_histogram_enable = (unsigned int)inFields[2];
    mac_mib_counter_cfg.mib_clear_on_read_enable = (unsigned int)inFields[3];
    
    /* call Ocelot API function */
    result = fx950_api_statistics_set_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_statistics_get_port_mac_mib_counter_cfg
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure gets network port MAC MIB Counters configuration.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Stack port number (0..1)
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
*    mac_mib_counter_cfg_ptr -
*       Pointer to Port MAC MIB Counters configuration.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_get_port_mac_mib_counter_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long port;
    FX950_STATISTICS_HGS_INTERFACE_PORT_MAC_MIB_COUNTER_CFG mac_mib_counter_cfg;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_statistics_get_port_mac_mib_counter_cfg(device_id, port, &mac_mib_counter_cfg);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = mac_mib_counter_cfg.mib_cnt_disable;
    inFields[1] = mac_mib_counter_cfg.rx_histogram_enable;
    inFields[2] = mac_mib_counter_cfg.tx_histogram_enable;
    inFields[3] = mac_mib_counter_cfg.mib_clear_on_read_enable;
    
    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);
    
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_statistics_get_port_mac_mib_counter_cfg_end
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
*  fx950_api_statistics_set_port_mac_mib_copy_trigger
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure triggers capturing of the MIB Counter.
*   The counters of the triggered captured port are copied to a designated memory 
*   space in a single atomic action. After this action, the counters can be
*   read  from a capture area.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Stack port number (0..1)
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
static CMD_STATUS wrFx950_api_statistics_set_port_mac_mib_copy_trigger
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long port;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_statistics_set_port_mac_mib_copy_trigger(device_id, port);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_statistics_get_port_mac_mib_copy_status
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure gets the copy (capture) status of the MIB Counters.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Stack port number (0..1)
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
*    copy_done_status_ptr - 
*       Copy status of the MIB Counters.
*       When copy_done_status_ptr = TRUE, counter capture action is done.
*       When copy_done_status_ptr = FALSE, counter capture action is not done.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_statistics_get_port_mac_mib_copy_status
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long port;
    unsigned int copy_done_status;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_statistics_get_port_mac_mib_copy_status(device_id, port, &copy_done_status);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", copy_done_status);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"statistics_global_counterGetFirst",
         &wrFx950_api_statistics_get_global_counter,
         2, 0},
        {"statistics_global_counterGetNext",
         &wrFx950_api_statistics_get_global_counter_end,
         2, 0},
        {"statistics_hgl_counterGetFirst",
         &wrFx950_api_statistics_get_hgl_counter,
         3, 0},
        {"statistics_hgl_counterGetNext",
         &wrFx950_api_statistics_get_hgl_counter_end,
         3, 0},
        {"statistics_api_all_global_countersGetFirst",
         &wrFx950_api_statistics_get_all_global_counters,
         1, 0},
        {"statistics_api_all_global_countersGetNext",
         &wrFx950_api_statistics_get_all_global_counters_end,
         1, 0},
        {"statistics_api_all_hgl_countersGetFirst",
         &wrFx950_api_statistics_get_all_hgl_counters,
         2, 0},
        {"statistics_api_all_hgl_countersGetNext",
         &wrFx950_api_statistics_get_all_hgl_counters_end,
         2, 0},
        {"statistics_hgs_port_mac_mib_counterGetFirst",
         &wrFx950_api_statistics_get_hgs_port_mac_mib_counter,
         4, 0},
        {"statistics_hgs_port_mac_mib_counterGetNext",
         &wrFx950_api_statistics_get_hgs_port_mac_mib_counter_end,
         4, 0},
        {"statistics_api_all_hgs_countersGetFirst",
         &wrFx950_api_statistics_get_all_hgs_counters,
         3, 0},
        {"statistics_api_all_hgs_countersGetNext",
         &wrFx950_api_statistics_get_all_hgs_counters_end,
         3, 0},
        {"statistics_port_mac_mib_counter_cfgSet",
         &wrFx950_api_statistics_set_port_mac_mib_counter_cfg,
         2, 0},
        {"statistics_port_mac_mib_counter_cfgGetFirst",
         &wrFx950_api_statistics_get_port_mac_mib_counter_cfg,
         2, 0},
        {"statistics_port_mac_mib_counter_cfgGetNext",
         &wrFx950_api_statistics_get_port_mac_mib_counter_cfg_end,
         2, 0},
        {"statistics_set_port_mac_mib_copy_trigger",
         &wrFx950_api_statistics_set_port_mac_mib_copy_trigger,
         2, 0},
        {"statistics_get_port_mac_mib_copy_status",
         &wrFx950_api_statistics_get_port_mac_mib_copy_status,
         2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_statistics
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
GT_STATUS cmdLibInitFx950_api_statistics
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

