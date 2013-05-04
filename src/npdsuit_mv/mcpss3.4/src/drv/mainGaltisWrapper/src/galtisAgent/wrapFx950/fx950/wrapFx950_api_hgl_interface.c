/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_hgl_interface.c
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
#include <FX950/include/fx950_api_hgl_interface.h>

/*****************************************************
*NAME
*  fx950_api_hgl_interface_set_port_mac_params
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure sets Port's MAC parameters. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
*   FX950_HGL_INTERFACE_PORT_MAC_PARAMS_STRUCT  *port_mac_params_ptr - 
*       Pointer to port MAC parameters.
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
static CMD_STATUS wrFx950_api_hgl_interface_set_port_mac_params
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
    FX950_HGL_INTERFACE_PORT_MAC_PARAMS_STRUCT port_mac_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];

    port_mac_params_ptr.serdes_lanes_mode = (FX950_HGL_INTERFACE_SERDES_LANES_MODE)inFields[0];
    port_mac_params_ptr.ecc_checker_type = (FX950_HGL_INTERFACE_ECC_CHECKER_TYPE)inFields[1];

    /* call Ocelot API function */
    result = fx950_api_hgl_interface_set_port_mac_params(device_id, port, &port_mac_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_interface_get_port_mac_params
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets Port's MAC parameters. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
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
*   port_mac_params_ptr - 
*       Port MAC parameters.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_get_port_mac_params
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
    FX950_HGL_INTERFACE_PORT_MAC_PARAMS_STRUCT port_mac_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_hgl_interface_get_port_mac_params(device_id, port, &port_mac_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = port_mac_params_ptr.serdes_lanes_mode;
    inFields[1] = port_mac_params_ptr.ecc_checker_type;

    fieldOutput("%d%d", inFields[0], inFields[1]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_get_port_mac_params_end
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
*  fx950_api_hgl_interface_send_ping
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure sets Cell Data of the Tx Ping message and triggers the Tx Ping. 
*  The API checks status of previous operation and return <BUZY> error code 
*  when previous action is not finished yet.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
*    unsigned long       ping_cell_data - 
*       Ping Cell data, has 24 bits only (0..0xFFFFFF).
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
static CMD_STATUS wrFx950_api_hgl_interface_send_ping
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
    unsigned long ping_cell_data;
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
    ping_cell_data = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_hgl_interface_send_ping(device_id, port, ping_cell_data);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_interface_get_tx_ping_cell_data
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets Cell Data of the Tx Ping message and the status of the 
*  ping. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
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
*    *ping_cell_data_ptr - 
*       Ping Cell data.
*    *ping_sent_status_ptr -
*       When ping_sent_status_ptr = TRUE, ping cell was sent.
*       When ping_sent_status_ptr = FALSE, ping cell was'n sent yet.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_get_tx_ping_cell_data
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
    unsigned long ping_cell_data_ptr;
    unsigned int ping_sent_status_ptr;
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
    result = fx950_api_hgl_interface_get_tx_ping_cell_data(device_id, port, 
                                                           &ping_cell_data_ptr, 
                                                           &ping_sent_status_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", ping_cell_data_ptr, ping_sent_status_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_interface_get_rx_ping_cell_data
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets Cell Data of the Rx Ping message.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
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
*    *ping_cell_data_ptr - 
*       Ping Cell data.
*    *valid_ptr - 
*       When valid_ptr = TRUE, a Ping Cell Data is valid
*       When valid_ptr = FALSE,  a Ping Cell Data is not valid
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_get_rx_ping_cell_data
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
    unsigned long ping_cell_data_ptr;
    unsigned int valid_ptr;
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
    result = fx950_api_hgl_interface_get_rx_ping_cell_data(device_id, port, 
                                                           &ping_cell_data_ptr, 
                                                           &valid_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", ping_cell_data_ptr, valid_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_interface_set_fp_link_level_fc_status_enable
*
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*
*FUNCTION:
*  This procedure enables/disables Link Level Flow Control status of 
*  all Fabric ports in the corresponding Packet Processor connected to this HyperG.Link. 
*
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
*    unsigned int        enable  - 
*       When enable = TRUE, Enable Link Level Flow Control status
*       When enable = FALSE, Disable Link Level Flow Control status
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
*  The function enables or disables Link Level Flow Control status of all 64 fabric ports.
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_set_fp_link_level_fc_status_enable
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
    unsigned int enable;
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
    enable = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_hgl_interface_set_fp_link_level_fc_status_enable(device_id, port, enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_interface_get_fp_link_level_fc_status_enable_bmp
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets the status of Link Level Flow Control status of 
*  all Fabric ports in the corresponding 98FX915 connected to this HyperG.Link 
*  (enabled/disabled).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
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
*    *fc_status_en_fp_bmp_ptr - 
*       Bitmap of status enable/disable for Fabric Ports:
*       0 - Enable Link Level Flow Control status.
*       1 - Disable Link Level Flow Control status.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_get_fp_link_level_fc_status_enable
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
    unsigned int enable;
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
    result = fx950_api_hgl_interface_get_fp_link_level_fc_status_enable(device_id, port, &enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", enable);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets the Link Level Flow Control status of all fabric ports 
*  in the peer device connected to this HyperG.Link port. 
*  The status is extracted from a Link Level Flow Control cell sent by the 
*  peer device.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
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
*    *fc_status_fp_bmp_ptr - 
*       Bitmap of status Fabric Ports:
*       0 - Flow Control status is XON.
*       1 - Flow Control status is XOFF.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_get_fp_link_level_fc_status_bmp
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
    FX950_BMP_64 fc_status_fp_bmp_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_hgl_interface_get_fp_link_level_fc_status_bmp(device_id, port, 
                                                                     &fc_status_fp_bmp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = fc_status_fp_bmp_ptr.bitmap[0];
    inFields[1] = fc_status_fp_bmp_ptr.bitmap[1];

    fieldOutput("%d%d", inFields[0], inFields[1]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_get_fp_link_level_fc_status_bmp_end
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
*  fx950_api_hgl_interface_set_fc_tx_params
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure sets Flow Control parameters. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
*    FX950_API_HGL_INTERFACE_FC_TX_PARAMS_STRUCT *fc_tx_params_ptr - 
*       Flow Control parameters
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
static CMD_STATUS wrFx950_api_hgl_interface_set_fc_tx_params
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
    FX950_API_HGL_INTERFACE_FC_TX_PARAMS_STRUCT fc_tx_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];

    fc_tx_params_ptr.enable = (unsigned int)inFields[0];
    fc_tx_params_ptr.periodic_enable = (unsigned int)inFields[1];
    fc_tx_params_ptr.refresh_rate = (unsigned long)inFields[2];

    /* call Ocelot API function */
    result = fx950_api_hgl_interface_set_fc_tx_params(device_id, port, &fc_tx_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_interface_get_fc_tx_params
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets Flow Control parameters. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
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
*    *fc_tx_params_ptr - 
*       Flow Control parameters
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_get_fc_tx_params
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
    FX950_API_HGL_INTERFACE_FC_TX_PARAMS_STRUCT fc_tx_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_hgl_interface_get_fc_tx_params(device_id, port, &fc_tx_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = fc_tx_params_ptr.enable;
    inFields[1] = fc_tx_params_ptr.periodic_enable;
    inFields[2] = fc_tx_params_ptr.refresh_rate;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_get_fc_tx_params_end
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
*  fx950_api_hgl_interface_set_fc_deadlock_limit
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure sets Flow Control deadlock limit.
*  When XOFF status is held longer than specified deadlock limit an 
*  FCXOffDeadlock interrupt is asserted.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
*    unsigned long       deadlock_limit - 
*       DeadLock limit in 256 core clocks. E.g. deadlock_limit = 1 means 
*       after 256 core clocks the FCXOffDeadlock interrupt is asserted if port 
*       is still in XOFF state.  (0..0xFFFF).
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
static CMD_STATUS wrFx950_api_hgl_interface_set_fc_deadlock_limit
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
    unsigned long deadlock_limit;
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
    deadlock_limit = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_hgl_interface_set_fc_deadlock_limit(device_id, port, deadlock_limit);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_interface_get_fc_deadlock_limit
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets Flow Control deadlock limit.
*  This is the maximum number of core clock cycles, multiplied by 256, that
*  PCS Flow control to XBAR port.
can be at XOFF.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Link port number (0..1)
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
*    deadlock_limit_ptr - 
*       DeadLock limit (0..0xFFFF) 
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_interface_get_fc_deadlock_limit
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
    unsigned long deadlock_limit;
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
    result = fx950_api_hgl_interface_get_fc_deadlock_limit(device_id, port, &deadlock_limit);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", deadlock_limit);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"hgl_interface_port_mac_paramsSet",
         &wrFx950_api_hgl_interface_set_port_mac_params,
         2, 2},
        {"hgl_interface_port_mac_paramsGetFirst",
         &wrFx950_api_hgl_interface_get_port_mac_params,
         2, 0},
        {"hgl_interface_port_mac_paramsGetNext",
         &wrFx950_api_hgl_interface_get_port_mac_params_end,
         2, 0},
        {"hgl_interface_send_ping",
         &wrFx950_api_hgl_interface_send_ping,
         3, 0},
        {"hgl_interface_get_tx_ping_cell_data",
         &wrFx950_api_hgl_interface_get_tx_ping_cell_data,
         2, 0},
        {"hgl_interface_get_rx_ping_cell_data",
         &wrFx950_api_hgl_interface_get_rx_ping_cell_data,
         2, 0},
        {"hgl_interface_set_fp_link_level_fc_status_enable",
         &wrFx950_api_hgl_interface_set_fp_link_level_fc_status_enable,
         3, 0},
        {"hgl_interface_get_fp_link_level_fc_status_enable",
         &wrFx950_api_hgl_interface_get_fp_link_level_fc_status_enable,
         2, 0},
        {"hgl_interface_fp_level_fc_status_bmpGetFirst",
         &wrFx950_api_hgl_interface_get_fp_link_level_fc_status_bmp,
         2, 0},
        {"hgl_interface_fp_level_fc_status_bmpGetNext",
         &wrFx950_api_hgl_interface_get_fp_link_level_fc_status_bmp_end,
         2, 0},
        {"hgl_interface_fc_tx_paramsSet",
         &wrFx950_api_hgl_interface_set_fc_tx_params,
         2, 3},
        {"hgl_interface_fc_tx_paramsGetFirst",
         &wrFx950_api_hgl_interface_get_fc_tx_params,
         2, 0},
        {"hgl_interface_fc_tx_paramsGetNext",
         &wrFx950_api_hgl_interface_get_fc_tx_params_end,
         2, 0},
        {"hgl_interface_set_fc_deadlock_limit",
         &wrFx950_api_hgl_interface_set_fc_deadlock_limit,
         3, 0},
        {"hgl_interface_get_fc_deadlock_limit",
         &wrFx950_api_hgl_interface_get_fc_deadlock_limit,
         2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_hgl_interface
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
CMD_STATUS cmdLibInitFx950_api_hgl_interface
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

