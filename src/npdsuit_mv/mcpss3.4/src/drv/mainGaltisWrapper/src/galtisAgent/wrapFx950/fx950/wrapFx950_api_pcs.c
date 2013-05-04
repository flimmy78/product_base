/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_pcs.c
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
#include <FX950/include/fx950_api_pcs.h>
#include <FX950/include/fx950_pcs.h>

/*****************************************************
*NAME
*  fx950_api_pcs_set_loopback_enable
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure Enables/Disables Tx to Rx Loop on the PCS TBI interface to 
*  the SERDES.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for "HyperG. Link or HyperG. Stack port and 2 for 
*       Statistic Port.
*    unsigned int    loopback_enable - 
*       When loopback_enable = TRUE, enale loopback.
*       When loopback_enable = FALSE, disable loopback.
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
*  In order to configure loopback the function set PCS state to RESET,
*  configure loopback and return state to NORMAL. 
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_pcs_set_loopback_enable
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
    unsigned int loopback_enable;
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
    loopback_enable = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_pcs_set_loopback_enable(device_id, port, loopback_enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_pcs_get_loopback_enable
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets the status of Tx to Rx Loop on the PCS TBI interface to 
*  the SERDES (Enabled/Disabled).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperG. Link or HyperG. Stack port and 2 for 
*       Statistic Port.
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
*    loopback_enable_ptr - 
*       When loopback_enable_ptr = TRUE, enale loopback.
*       When loopback_enable_ptr = FALSE, disable loopback.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_pcs_get_loopback_enable
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
    unsigned int loopback_enable;
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
    result = fx950_api_pcs_get_loopback_enable(device_id, port, &loopback_enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", loopback_enable);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_pcs_get_lane_status
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets the lane status.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperG. Link or HyperG. Stack port and 2 for Statistic Port.
*    unsigned long  lane - 
*      Lane number (0..5)
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
*    lane_status_ptr - 
*       Lane pcs status.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_pcs_get_lane_status
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
    unsigned long lane;
    FX950_PCS_LANE_STATUS_STRUCT lane_status_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];
    lane = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_pcs_get_lane_status(device_id, port, lane, &lane_status_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = lane_status_ptr.sync_ok;

    /* pack and output table fields */
    fieldOutput("%d", inFields[0]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_pcs_get_lane_status_end
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
*  fx950_api_pcs_get_status
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets the status of port's PCS.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperG. Link or HyperG. Stack port and 2 for 
*       Statistic Port.
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
*    link_up_ptr - 
*       Port's link is up.
*       When link_up = TRUE, Link is Up.
*       When link_up = FALSE, Link is Down.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_pcs_get_status
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
    unsigned int link_up_ptr;
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
    result = fx950_api_pcs_get_status(device_id, port, &link_up_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", link_up_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_pcs_set_reset_state
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure sets RESET state of the PCS for a port
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperG. Link or HyperG. Stack port and 2 for 
*       Statistic Port.
*    unsigned long   not_in_reset - 
*       PCS RESET state to set.
*        0 - PSC in normal mode, 1 - PSC in RESET state
*  INDIRECT:
*OUTPUT:
*  DIRECT:
*    SAND_RET -
*      See formatting rules in ERROR RETURN VALUES.
*  INDIRECT:
*     unsigned long   *old_not_in_reset_ptr
*      old state of PSC RESET
*        0 - PSC in normal mode, 1 - PSC in RESET state
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_pcs_set_reset_state
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
    unsigned long not_in_reset;
    unsigned long old_not_in_reset;
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
    not_in_reset = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_pcs_set_reset_state(device_id, port, not_in_reset, &old_not_in_reset);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", old_not_in_reset);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_pcs_get_port_type
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure gets PCS type for a port
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperG. Link or HyperG. Stack port and 2 for 
*       Statistic Port.
*  INDIRECT:
*OUTPUT:
*  DIRECT:
*    SAND_RET -
*      See formatting rules in ERROR RETURN VALUES.
*  INDIRECT:
*   FX950_PSC_PORT_MODE   *port_mode_ptr
*     port mode of PSC
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_pcs_get_port_type
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
    FX950_PSC_PORT_MODE port_mode;
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
    result = fx950_pcs_get_port_type(device_id, port, &port_mode);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", port_mode);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_pcs_check_lane_number
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure check lane number according to PCS mode
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperG. Link or HyperG. Stack port and 2 for 
*       Statistic Port.
*    unsigned long  lane - 
*      Lane number (0..5)
*       for HyperG Link port all 6 lanes may be used, 
*       for HyperG Stack port only 4 lanes may be used and 
*       for Statistic port only 2 lanes - 
*       0 for Ingress statistic, 1 for Egress statistic
*  INDIRECT:
*OUTPUT:
*  DIRECT:
*    SAND_RET -
*      See formatting rules in ERROR RETURN VALUES.
*  INDIRECT:
*   unsigned long   *is_ok_ptr
*     is lane in the acceptable range
*     1 - OK, 0 - bad lane number
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_pcs_check_lane_number
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
    unsigned long lane;
    unsigned long is_ok;
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
    lane = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_pcs_check_lane_number(device_id, port, lane, &is_ok);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", is_ok);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"pcs_set_loopback_enable",
         &wrFx950_api_pcs_set_loopback_enable,
         3, 0},
        {"pcs_get_loopback_enable",
         &wrFx950_api_pcs_get_loopback_enable,
         2, 0},
        {"pcs_lane_statusGetFirst",
         &wrFx950_api_pcs_get_lane_status,
         3, 0},
        {"pcs_lane_statusGetNext",
         &wrFx950_api_pcs_get_lane_status_end,
         3, 0},
        {"pcs_get_status",
         &wrFx950_api_pcs_get_status,
         2, 0},
        {"pcs_set_reset_state",
         &wrFx950_pcs_set_reset_state,
         3, 0},
        {"pcs_get_port_type",
         &wrFx950_pcs_get_port_type,
         2, 0},
        {"pcs_check_lane_number",
         &wrFx950_pcs_check_lane_number,
         3, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_pcs
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
CMD_STATUS cmdLibInitFx950_api_pcs
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

