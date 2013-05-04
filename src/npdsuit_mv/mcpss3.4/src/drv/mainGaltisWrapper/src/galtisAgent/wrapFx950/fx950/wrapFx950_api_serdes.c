/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_serdes.c
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
#include <FX950/include/fx950_api_serdes.h>

/*****************************************************
*NAME
*  fx950_api_serdes_set_swap_select
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  Select which PCS Tx/Rx lane is connected to this SERDES lane.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperGlink or XGS port and 2 for Statistic Port.
*    unsigned long  lane - 
*      Lane number (0..5)
*    unsigned long   pcs_tx_lane - 
*       PCS Tx Lane number (0..5)
*    unsigned long   pcs_rx_lane - 
*       PCS Rx Lane number (0..5)
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
*  + Device must have been registered for this action to be
*    carried out properly.
*  + In order to configure loopback the function set PCS state to RESET, 
*    configure loopback and return state to NORMAL.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_set_swap_select
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
    unsigned long pcs_tx_lane;
    unsigned long pcs_rx_lane;
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
    pcs_tx_lane = (unsigned long)inArgs[3];
    pcs_rx_lane = (unsigned long)inArgs[4];

    /* call Ocelot API function */
    result = fx950_api_serdes_set_swap_select(device_id, port, lane, pcs_tx_lane, pcs_rx_lane);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_get_swap_select
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This procedure gets which of the PCS Tx/Rx lanes is connected to this 
*  SERDES lane.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperGlink or XGS port and 2 for Statistic Port.
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
*    *pcs_tx_lane_ptr - 
*       PCS Tx Lane number
*    *pcs_rx_lane_ptr - 
*       PCS Rx Lane number
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_get_swap_select
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
    unsigned long pcs_tx_lane;
    unsigned long pcs_rx_lane;
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
    result = fx950_api_serdes_get_swap_select(device_id, port, lane, &pcs_tx_lane, &pcs_rx_lane);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", pcs_tx_lane, pcs_rx_lane);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_set_reset_enable_bmp
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function enables/disables reset per lane.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperGlink or XGS port and 2 for Statistic Port.
*    unsigned long   reset_enable_bmp (only six lsb are used) - 
*      reset lane bitmap:
*      0 - lane is not reset
*      1 - lane is reset
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
static CMD_STATUS wrFx950_api_serdes_set_reset_enable_bmp
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
    unsigned long reset_enable_bmp;
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
    reset_enable_bmp = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_serdes_set_reset_enable_bmp(device_id, port, reset_enable_bmp);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_get_reset_enable_bmp
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function gets reset per lane status (enabled/disabled).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperGlink or XGS port and 2 for Statistic Port.
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
*    reset_bmp - 
*      reset lane bitmap (only six lsb are used):
*      0 - lane is not reset
*      1 - lane is reset
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_get_reset_enable_bmp
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
    unsigned long reset_enable_bmp;
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
    result = fx950_api_serdes_get_reset_enable_bmp(device_id, port, &reset_enable_bmp);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", reset_enable_bmp);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_set_loopback_enable
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function enables/disables Transmit to Receive loop on the MAC individual 
*   interface and Receive to Transmit loop on the SERDES individual interface.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperGlink or XGS port and 2 for Statistic Port.
*    unsigned long  lane - 
*      Lane number (0..5)
*    unsigned int   individual_loopback_enable - 
*      When individual_loopback_enable = TRUE, enable Tx to Rx loop.
*      When individual_loopback_enable = FALSE, disable Tx to Rx loop.
*    unsigned int   individual_line_loopback_enable - 
*      When individual_line_loopback_enable = TRUE, enable Rx to Tx loop.
*      When individual_line_loopback_enable = FALSE, disable Rx to Tx loop.
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
*  In order to configure loopback: set state to RESET, configure loopback and
*  return state to NORMAL.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_set_loopback_enable
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
    unsigned long individual_loopback_enable;
    unsigned long individual_line_loopback_enable;
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
    individual_loopback_enable = (unsigned int)inArgs[3];
    individual_line_loopback_enable = (unsigned int)inArgs[4];

    /* call Ocelot API function */
    result = fx950_api_serdes_set_loopback_enable(device_id, port, lane, 
                                                  individual_loopback_enable, 
                                                  individual_line_loopback_enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_get_loopback_enable
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function gets the status of Transmit to Receive loop on the MAC individual 
*  interface and Receive to Transmit loop on the SERDES individual interface
*  (enabled/disabled).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperGlink or XGS port and 2 for Statistic Port.
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
*    individual_loopback_enable_ptr - 
*      When individual_loopback_enable_ptr = TRUE, enable Tx to Rx loop.
*      When individual_loopback_enable_ptr = FALSE, disable Tx to Rx loop.
*    individual_line_loopback_enable_ptr - 
*      When individual_line_loopback_enable_ptr = TRUE, enable Rx to Tx loop.
*      When individual_line_loopback_enable_ptr = FALSE, disable Rx to Tx loop.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_get_loopback_enable
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
    unsigned int individual_loopback_enable;
    unsigned int individual_line_loopback_enable;
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
    result = fx950_api_serdes_get_loopback_enable(device_id, port, lane, 
                                                  &individual_loopback_enable,
                                                  &individual_line_loopback_enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", individual_loopback_enable, individual_line_loopback_enable);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_set_hgs_port_speed_mode
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function sets the speed mode for HyperG.Stack port.
*  individual interface.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Hyper.G.Stack port number (0..1)
*    FX950_SERDES_HGS_SPEED_MODE     speed_mode - 
*      Speed mode 10 or 12 Gbps.
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
static CMD_STATUS wrFx950_api_serdes_set_hgs_port_speed_mode
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
    FX950_SERDES_HGS_SPEED_MODE speed_mode;
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
    speed_mode = (FX950_SERDES_HGS_SPEED_MODE)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_serdes_set_hgs_port_speed_mode(device_id, port, speed_mode);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_get_hgs_port_speed_mode
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function gets the speed mode for HyperG.Stack port.
*  individual interface.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
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
*    speed_mode_ptr - 
*      Speed mode 10 or 12 Gbps.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_get_hgs_port_speed_mode
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
    FX950_SERDES_HGS_SPEED_MODE speed_mode;
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
    result = fx950_api_serdes_get_hgs_port_speed_mode(device_id, port, &speed_mode);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", speed_mode);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_set_lane_tx_params
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function sets the Output Current and Transmit Voltage Swing of the 
*  SERDES.
*INPUT:
*  DIRECT:
*    unsigned int       device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long      port - 
*       Port number, 0..1 for HyperG.link or HyperG.Stack port and 2 for 
*       Statistic Port.
*    unsigned long      lane - 
*       lane number (0..5)
*    unsigned long       out_current - 
*       (0..7). For further information, contact your Marvell representative.
*    unsigned long       pre_emphasis_current_control - 
*        (0..15). For further information, contact your Marvell representative.
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
*    None. 
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_set_lane_tx_params
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
    unsigned long out_current;
    unsigned long pre_emphasis_current_control;
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
    out_current = (unsigned long)inArgs[3];
    pre_emphasis_current_control = (unsigned long)inArgs[4];

    /* call Ocelot API function */
    result = fx950_api_serdes_set_lane_tx_params(device_id, port, lane, out_current, 
                                                 pre_emphasis_current_control);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_get_lane_tx_params
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function gets the Output Current and Transmit Voltage Swing of the 
*  SERDES.
*INPUT:
*  DIRECT:
*    unsigned int       device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long      port - 
*       Port number, 0..1 for HyperG.link or HyperG.Stack port and 2 for 
*       Statistic Port.
*    unsigned long      lane - 
*       lane number (0..5)
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
*    out_current_ptr - 
*       For further information, contact your Marvell representative.
*    pre_emphasis_current_control_ptr - 
*        For further information, contact your Marvell representative.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_get_lane_tx_params
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
    unsigned long out_current;
    unsigned long pre_emphasis_current_control;
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
    result = fx950_api_serdes_get_lane_tx_params(device_id, port, lane, &out_current, 
                                                 &pre_emphasis_current_control);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", out_current, pre_emphasis_current_control);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_get_lane_signal_status
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function Gets the Time Base Generator Locked and Signal-Detect of the SERDES.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperGlink or XGS port and 2 for Statistic Port.
*    unsigned long   lane - 
*       lane number (0..5)
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
*    signal_status_ptr - 
*      Lane status parameters.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_get_lane_signal_status
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
    FX950_SERDES_SIGNAL_STATUS_STRUCT signal_status_ptr;
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
    result = fx950_api_serdes_get_lane_signal_status(device_id, port, lane, &signal_status_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    inFields[0] = signal_status_ptr.pll_locked ;
    inFields[1] = signal_status_ptr.signal_detected ;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_serdes_get_lane_signal_status_end
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
*  fx950_api_serdes_set_power_up_status
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function power Up or Down  SERDES lanes for HyperG. Link, HyperG.Stack 
*  or Statistic ports' SERDESes.
*  SERDES.
*INPUT:
*  DIRECT:
*    unsigned int       device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperG Link or HyperG Stack port and 2 for Statistic Port.
*    unsigned long   rx_lane_power_up_bmp (max 6 lsb)
*                       all six lsb are used for HyperG Link port
*       rx power up/down bitmap:
*       - 1 - Power UP
*       - 0 - Power Down
*    unsigned long   tx_lane_power_up_bmp (max 6 lsb)
*                       all six lsb are used for HyperG Link port
*       tx power up/down bitmap:
*       - 1 - Power UP
*       - 0 - Power Down
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
*       None.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_set_power_up_status
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long       result;
    unsigned int        device_id;
    unsigned long       port;
    unsigned long       rx_lane_power_up_bmp;
    unsigned long       tx_lane_power_up_bmp;
    unsigned short      rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];
    rx_lane_power_up_bmp = (unsigned long)inArgs[2];
    tx_lane_power_up_bmp = (unsigned long)inArgs[3];

    /* call Ocelot API function */
    result = fx950_api_serdes_set_power_up_status(device_id, 
                                                  port,
                                                  rx_lane_power_up_bmp,
                                                  tx_lane_power_up_bmp);

    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, " ");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_serdes_get_power_up_status
*TYPE:
*  PROC
*DATE:
*  14/OCT/2007
*FUNCTION:
*  This function gets power status (Up or Down) of SERDES lanes for HyperG. Link, 
*  HyperG.Stack or Statistic ports' SERDESes
*  SERDES.
*INPUT:
*  DIRECT:
*    unsigned int       device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port - 
*       Port number, 0..1 for HyperG Link or HyperG Stack port and 2 for Statistic Port.
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
*    rx_lane_power_up_bmp_ptr (max 6 lsb)
*                       all six lsb are used for HyperG Link port
*       pointer to rx power up/down bitmap:
*       - 1 - Power UP
*       - 0 - Power Down
*    tx_lane_power_up_bmp_ptr (max 6 lsb)
*                       all six lsb are used for HyperG Link port
*       pointer to tx power up/down bitmap:
*       - 1 - Power UP
*       - 0 - Power Down
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_serdes_get_power_up_status
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long       result;
    unsigned int        device_id;
    unsigned long       port;
    unsigned long       rx_lane_power_up_bmp;
    unsigned long       tx_lane_power_up_bmp;
    unsigned short      rc;


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
    result = fx950_api_serdes_get_power_up_status(device_id, 
                                                  port,
                                                  &rx_lane_power_up_bmp,
                                                  &tx_lane_power_up_bmp);

    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", rx_lane_power_up_bmp, tx_lane_power_up_bmp);

    return CMD_OK;
}
/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"serdes_set_swap_select",
         &wrFx950_api_serdes_set_swap_select,
         5, 0},
        {"serdes_get_swap_select",
         &wrFx950_api_serdes_get_swap_select,
         3, 0},
        {"serdes_set_reset_enable_bmp",
         &wrFx950_api_serdes_set_reset_enable_bmp,
         3, 0},
        {"serdes_get_reset_enable_bmp",
         &wrFx950_api_serdes_get_reset_enable_bmp,
         2, 0},
        {"serdes_set_loopback_enable",
         &wrFx950_api_serdes_set_loopback_enable,
         5, 0},
        {"serdes_get_loopback_enable",
         &wrFx950_api_serdes_get_loopback_enable,
         3, 0},
        {"serdes_set_hgs_port_speed_mode",
         &wrFx950_api_serdes_set_hgs_port_speed_mode,
         3, 0},
        {"serdes_get_hgs_port_speed_mode",
         &wrFx950_api_serdes_get_hgs_port_speed_mode,
         2, 0},
        {"serdes_set_lane_tx_params",
         &wrFx950_api_serdes_set_lane_tx_params,
         5, 0},
        {"serdes_get_lane_tx_params",
         &wrFx950_api_serdes_get_lane_tx_params,
         3, 0},
        {"serdes_lane_signal_statusGetFirst",
         &wrFx950_api_serdes_get_lane_signal_status,
         3, 0},
        {"serdes_lane_signal_statusGetNext",
         &wrFx950_api_serdes_get_lane_signal_status_end,
         3, 0},
        {"serdes_set_power_up_status",
         &wrFx950_api_serdes_set_power_up_status,
         4, 0},
        {"serdes_get_power_up_status",
         &wrFx950_api_serdes_get_power_up_status,
         2, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_serdes
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
CMD_STATUS cmdLibInitFx950_api_serdes
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

