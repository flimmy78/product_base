/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_hgl_cell_reassembly.c
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
#include <FX950/include/fx950_api_hgl_cell_reassembly.h>

/*****************************************************
*NAME
*  fx950_api_hgl_cell_reassembly_set_link_config
*TYPE:
*  PROC
*DATE:
*  14/AUG/2007
*FUNCTION:
*  This procedure sets the configuration of cell reassembly engine per link.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    link_number - 
*       link number (0..1)
*    FX950_HGL_CELL_REASSEMBLY_LINK_CFG_STRUCT *cell_reassembly_link_cfg_ptr -
*       Pointer to Cell Reassembly link configuration
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
*  The MH type must not be modified under traffic.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_cell_reassembly_set_link_config
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link_number;
    FX950_HGL_CELL_REASSEMBLY_LINK_CFG_STRUCT cell_reassembly_link_cfg_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_number = (unsigned long)inArgs[1];

    cell_reassembly_link_cfg_ptr.link_enable = (unsigned int)inFields[0];
    cell_reassembly_link_cfg_ptr.interface_type = (FX950_HGL_CELL_REASSEMBLY_LINK_INTERFACE_TYPE)inFields[1];
    cell_reassembly_link_cfg_ptr.mh_desc_type = (FX950_MH_FORMAT_TYPE)inFields[2];

    /* call Ocelot API function */
    result = fx950_api_hgl_cell_reassembly_set_link_config(device_id, link_number, &cell_reassembly_link_cfg_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_cell_reassembly_get_link_config
*TYPE:
*  PROC
*DATE:
*  14/AUG/2007
*FUNCTION:
*  This procedure gets the configuration of cell reassembly engine per link.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    link_number - 
*       link number (0..1)
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
*   cell_reassembly_link_cfg_ptr
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*  The MH type must not be modified under traffic.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_cell_reassembly_get_link_config
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link_number;
    FX950_HGL_CELL_REASSEMBLY_LINK_CFG_STRUCT cell_reassembly_link_cfg_ptr;
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
    result = fx950_api_hgl_cell_reassembly_get_link_config(device_id, link_number, &cell_reassembly_link_cfg_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = cell_reassembly_link_cfg_ptr.link_enable;
    inFields[1] = cell_reassembly_link_cfg_ptr.interface_type;
    inFields[2] = cell_reassembly_link_cfg_ptr.mh_desc_type;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_hgl_cell_reassembly_get_link_config_end
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
*  fx950_api_hgl_cell_reassembly_set_engine_cfg
*TYPE:
*  PROC
*DATE:
*  14/AUG/2007
*FUNCTION:
*  This procedure sets the global configuration of cell reassembly engine.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_HGL_CELL_REASSEMBLY_ENGINE_CFG_STRUCT *cell_reassembly_cfg_ptr - 
*      Pointer to Cell Reassembly global configuration
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
*  Packet length should be configured to more then 128 bytes.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_cell_reassembly_set_engine_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_HGL_CELL_REASSEMBLY_ENGINE_CFG_STRUCT cell_reassembly_cfg_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    cell_reassembly_cfg_ptr.mc_id0_enable = (unsigned int)inFields[0];
    cell_reassembly_cfg_ptr.fc_enable = (unsigned int)inFields[1];
    cell_reassembly_cfg_ptr.max_bc = (unsigned long)inFields[2];

    /* call Ocelot API function */
    result = fx950_api_hgl_cell_reassembly_set_engine_cfg(device_id, &cell_reassembly_cfg_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_cell_reassembly_get_engine_cfg
*TYPE:
*  PROC
*DATE:
*  14/AUG/2007
*FUNCTION:
*  This procedure gets the global configuration of cell reassembly engine.
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
*   cell_reassembly_cfg_ptr
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*  Packet length should be configured to more then 128 bytes.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_cell_reassembly_get_engine_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_HGL_CELL_REASSEMBLY_ENGINE_CFG_STRUCT cell_reassembly_cfg_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_hgl_cell_reassembly_get_engine_cfg(device_id, &cell_reassembly_cfg_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = cell_reassembly_cfg_ptr.mc_id0_enable;
    inFields[1] = cell_reassembly_cfg_ptr.fc_enable;
    inFields[2] = cell_reassembly_cfg_ptr.max_bc;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_hgl_cell_reassembly_get_engine_cfg_end
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
*  fx950_api_hgl_cell_reassembly_set_buffer_thresholds
*TYPE:
*  PROC
*DATE:
*  14/AUG/2007
*FUNCTION:
*  This procedure sets the XON/XOFF and Overflow thresholds of the Unicast,
*  Multicast (data) or Marvell Header (descriptors) buffers.
*  per link.
*  The reassembly buffers are considered in an XOFF state if any of the per link
*  Unicast, Multicast or Marvell Header buffers exceed the LL-FC XOFF threshold. 
*  The reassembly buffers are considered in an XON state if all buffers do not  
*  the LL-FC XON exceed threshold.
*INPUT:
*  DIRECT:
*   unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*   unsigned long    link_number - 
*       link number(0..1)
*   unsigned FX950_HGL_CELL_REASSEMBLY_BUFFER_TYPE  buffer_type - 
*       the type of the buffer: Unicast, Multicast or Marvell Header.
*   FX950_HGL_CELL_REASSEMBLY_THRESHOLD_ENTRY data_threshold_cptr -
*       data threshold congiguration (XON/XOFF and Overflow thresholds of 
*       the MH FIFO)
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
static CMD_STATUS wrFx950_api_hgl_cell_reassembly_set_buffer_thresholds
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link_number;
    FX950_HGL_CELL_REASSEMBLY_BUFFER_TYPE buffer_type;
    FX950_HGL_CELL_REASSEMBLY_THRESHOLD_ENTRY buffer_threshold_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_number = (unsigned long)inArgs[1];
    buffer_type = (FX950_HGL_CELL_REASSEMBLY_BUFFER_TYPE)inArgs[2];

    buffer_threshold_ptr.xoff_threshold = (unsigned long)inFields[0];
    buffer_threshold_ptr.drop_threshold = (unsigned long)inFields[1];
    buffer_threshold_ptr.xon_threshold = (unsigned long)inFields[2];

    /* call Ocelot API function */
    result = fx950_api_hgl_cell_reassembly_set_buffer_thresholds(device_id, link_number, buffer_type, &buffer_threshold_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_cell_reassembly_get_buffer_thresholds
*TYPE:
*  PROC
*DATE:
*  14/AUG/2007
*FUNCTION:
*  This procedure gets the XON/XOFF and Overflow thresholds of the Unicast,
*  Multicast (data) or Marvell Header (descriptors) buffers.
*  per link.
*  The reassembly buffers are considered in an XOFF state if any of the per link
*  Unicast, Multicast or Marvell Header buffers exceed the LL-FC XOFF threshold. 
*  The reassembly buffers are considered in an XON state if all buffers do not  
*  the LL-FC XON exceed threshold.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   unsigned long    link_number - 
*       link number(0..1)
*   unsigned FX950_HGL_CELL_REASSEMBLY_BUFFER_TYPE  buffer_type - 
*       the type of the buffer: Unicast, Multicast or Marvell Header.
*  INDIRECT:
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting
 is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*   data_threshold_ptr -
*       data threshold congiguration (XON/XOFF and Overflow thresholds of 
*       the MH FIFO)
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_cell_reassembly_get_buffer_thresholds
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link_number;
    FX950_HGL_CELL_REASSEMBLY_BUFFER_TYPE buffer_type;
    FX950_HGL_CELL_REASSEMBLY_THRESHOLD_ENTRY buffer_threshold_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_number = (unsigned long)inArgs[1];
    buffer_type = (FX950_HGL_CELL_REASSEMBLY_BUFFER_TYPE)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_hgl_cell_reassembly_get_buffer_thresholds(device_id, link_number, buffer_type, &buffer_threshold_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = buffer_threshold_ptr.xoff_threshold;
    inFields[1] = buffer_threshold_ptr.drop_threshold;
    inFields[2] = buffer_threshold_ptr.xon_threshold;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_hgl_cell_reassembly_get_buffer_thresholds_end
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
        {"hgl_cell_reassembly_link_configSet",
         &wrFx950_api_hgl_cell_reassembly_set_link_config,
         2, 3},
        {"hgl_cell_reassembly_link_configGetFirst",
         &wrFx950_api_hgl_cell_reassembly_get_link_config,
         2, 0},
        {"hgl_cell_reassembly_link_configGetNext",
         &wrFx950_api_hgl_cell_reassembly_get_link_config_end,
         2, 0},
        {"hgl_cell_reassembly_engine_cfgSet",
         &wrFx950_api_hgl_cell_reassembly_set_engine_cfg,
         1, 3},
        {"hgl_cell_reassembly_engine_cfgGetFirst",
         &wrFx950_api_hgl_cell_reassembly_get_engine_cfg,
         1, 0},
        {"hgl_cell_reassembly_engine_cfgGetNext",
         &wrFx950_api_hgl_cell_reassembly_get_engine_cfg_end,
         1, 0},
        {"hgl_cell_reassembly_buffer_thresholdsSet",
         &wrFx950_api_hgl_cell_reassembly_set_buffer_thresholds,
         3, 3},
        {"hgl_cell_reassembly_buffer_thresholdsGetFirst",
         &wrFx950_api_hgl_cell_reassembly_get_buffer_thresholds,
         3, 0},
        {"hgl_cell_reassembly_buffer_thresholdsGetNext",
         &wrFx950_api_hgl_cell_reassembly_get_buffer_thresholds_end,
         3, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_hgl_cell_reassembly
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
GT_STATUS cmdLibInitFx950_api_hgl_cell_reassembly
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

