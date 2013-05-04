/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_hgs_interface.c
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
#include <FX950/include/fx950_api_hgs_interface.h>


/*****************************************************
*NAME
*  fx950_api_hgs_interface_set_port_parameters
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure sets network port parameters.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Stack port number (0..1)
*    FX950_HGS_INTERFACE_PORT_PARAM_STRUCT   port_params_ptr -
*       Pointer to Port Parameters.
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
static CMD_STATUS wrFx950_api_hgs_interface_set_port_parameters
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
    FX950_HGS_INTERFACE_PORT_PARAM_STRUCT port_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];

    port_params_ptr.port_enable = (unsigned int)inFields[0];
    port_params_ptr.force_link_pass_enable = (unsigned int)inFields[1];
    port_params_ptr.force_link_down_enable = (unsigned int)inFields[2];
    port_params_ptr.rx_crc_check_enable = (unsigned int)inFields[3];
    port_params_ptr.padding_disable = (unsigned int)inFields[4];
    port_params_ptr.preamble_length_rx_mode = (FX950_HGS_INTERFACE_PREAMBLE_MODE)inFields[5];
    port_params_ptr.preamble_length_tx_mode = (FX950_HGS_INTERFACE_PREAMBLE_MODE)inFields[6];
    port_params_ptr.mru_size = (unsigned long)inFields[7];
    port_params_ptr.tx_ipg_mode  = (FX950_HGS_INTERFACE_PORT_IPG_MODE)inFields[8];
    port_params_ptr.fixed_ipg_base_type = (FX950_HGS_INTERFACE_PORT_BASE_IPG_TYPE)inFields[9];
    port_params_ptr.rx_crc_mode = (FX950_HGS_INTERFACE_PORT_CRC_MODE)inFields[10];
    port_params_ptr.tx_crc_mode = (FX950_HGS_INTERFACE_PORT_CRC_MODE)inFields[11];
    
    /* call Ocelot API function */
    result = fx950_api_hgs_interface_set_port_parameters(device_id, port, &port_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgs_interface_get_port_parameters
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure gets network port parameters.
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
*    port_params_ptr -
*       Port Parameters.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgs_interface_get_port_parameters
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
    FX950_HGS_INTERFACE_PORT_PARAM_STRUCT port_params_ptr;
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
    result = fx950_api_hgs_interface_get_port_parameters(device_id, port, &port_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = port_params_ptr.port_enable;
    inFields[1] = port_params_ptr.force_link_pass_enable;
    inFields[2] = port_params_ptr.force_link_down_enable;
    inFields[3] = port_params_ptr.rx_crc_check_enable;
    inFields[4] = port_params_ptr.padding_disable;
    inFields[5] = port_params_ptr.preamble_length_rx_mode;
    inFields[6] = port_params_ptr.preamble_length_tx_mode;
    inFields[7] = port_params_ptr.mru_size;
    inFields[8] = port_params_ptr.tx_ipg_mode;
    inFields[9] = port_params_ptr.fixed_ipg_base_type;
    inFields[10] = port_params_ptr.rx_crc_mode;
    inFields[11] = port_params_ptr.tx_crc_mode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0], inFields[1], inFields[2],  inFields[3],
                inFields[4], inFields[5], inFields[6],  inFields[7],
                inFields[8], inFields[9], inFields[10], inFields[11]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_hgs_interface_get_port_parameters_end

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
*  fx950_api_hgs_interface_get_port_status
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure gets network port status parameters.
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
*    port_status_ptr -
*       Port Status parameters.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgs_interface_get_port_status
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
    FX950_HGS_INTERFACE_PORT_STATUS_STRUCT port_status_ptr;
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
    result = fx950_api_hgs_interface_get_port_status(device_id, port, &port_status_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = port_status_ptr.link_status;
    inFields[1] = port_status_ptr.remote_fault;
    inFields[2] = port_status_ptr.local_fault; 
    inFields[3] = port_status_ptr.port_rx_pause; 
    inFields[4] = port_status_ptr.port_tx_pause; 
    inFields[5] = port_status_ptr.port_buff_full; 

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_hgs_interface_get_port_status_end

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
*  fx950_api_hgs_interface_set_port_fc_parameters
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure sets network port Flow Control parameters.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Stack port number (0..1)
*    FX950_HGS_INTERFACE_PORT_FLOW_CONTROL_PARAMS_STRUCT   *port_fc_params_ptr -
*       Pointer to Port Flow Control Parameters.
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
static CMD_STATUS wrFx950_api_hgs_interface_set_port_fc_parameters
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
    FX950_HGS_INTERFACE_PORT_FLOW_CONTROL_PARAMS_STRUCT port_fc_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];

    port_fc_params_ptr.periodic_xoff_enable = (unsigned int)inFields[0];
    port_fc_params_ptr.periodic_xon_enable = (unsigned int)inFields[1];
    port_fc_params_ptr.rx_fc_enable = (unsigned int)inFields[2];
    port_fc_params_ptr.tx_fc_enable = (unsigned int)inFields[3];

    /* call Ocelot API function */
    result = fx950_api_hgs_interface_set_port_fc_parameters(device_id, port, 
                                                            &port_fc_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgs_interface_get_port_fc_parameters
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure gets network port Flow Control parameters.
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
*    port_fc_params_ptr -
*       Port Flow Control Parameters.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgs_interface_get_port_fc_parameters
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
    FX950_HGS_INTERFACE_PORT_FLOW_CONTROL_PARAMS_STRUCT port_fc_params_ptr;
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
    result = fx950_api_hgs_interface_get_port_fc_parameters(device_id, port, 
                                                            &port_fc_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = port_fc_params_ptr.periodic_xoff_enable;
    inFields[1] = port_fc_params_ptr.periodic_xon_enable;
    inFields[2] = port_fc_params_ptr.rx_fc_enable;
    inFields[3] = port_fc_params_ptr.tx_fc_enable;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_hgs_interface_get_port_fc_parameters_end
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
*  fx950_api_hgs_interface_set_sa_upper_40_bits
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure sets MAC Source Addresses that 
*   used for IEEE802.3x Flow Control.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    port -
*       Hyper.G.Stack port number (0..1)
*    unsigned char   sa[6] -
*      bytes of MAC address in the network order, 
*      sa[0] - bits 47:40, sa[5] - bits 7:0.
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
static CMD_STATUS wrFx950_api_hgs_interface_set_fc_packet_sa
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
    unsigned char sa[6];
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port = (unsigned long)inArgs[1];

    sa[0] = (unsigned char)inFields[0];
    sa[1] = (unsigned char)inFields[1];
    sa[2] = (unsigned char)inFields[2];
    sa[3] = (unsigned char)inFields[3];
    sa[4] = (unsigned char)inFields[4];
    sa[5] = (unsigned char)inFields[5];

    /* call Ocelot API function */
    result = fx950_api_hgs_interface_set_fc_packet_sa(device_id, port, sa);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgs_interface_get_sa_upper_40_bits
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*   This procedure gets upper 40 bits (bits 47:8) of MAC Source Addresses that 
*   used for IEEE802.3x Flow Control.
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
*    sa[5] -
*      5 upper bytes of MAC address in the network order, 
*      sa[0] - bits 47:40, sa[4] - bits 15:8.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgs_interface_get_fc_packet_sa
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
    unsigned char sa[6];
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
    result = fx950_api_hgs_interface_get_fc_packet_sa(device_id, port, sa);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = sa[0];
    inFields[1] = sa[1];
    inFields[2] = sa[2];
    inFields[3] = sa[3];
    inFields[4] = sa[4];
    inFields[5] = sa[5];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4], inFields[4]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_hgs_interface_get_fc_packet_sa_end
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
        {"hgs_interface_port_parametersSet",
         &wrFx950_api_hgs_interface_set_port_parameters,
         2, 12},
        {"hgs_interface_port_parametersGetFirst",
         &wrFx950_api_hgs_interface_get_port_parameters,
         2, 0},
        {"hgs_interface_port_parametersGetNext",
         &wrFx950_api_hgs_interface_get_port_parameters_end,
         2, 0},
        {"hgs_interface_port_statusGetFirst",
         &wrFx950_api_hgs_interface_get_port_status,
         2, 0},
        {"hgs_interface_port_statusGetNext",
         &wrFx950_api_hgs_interface_get_port_status_end,
         2, 0},
        {"hgs_interface_port_fc_parametersSet",
         &wrFx950_api_hgs_interface_set_port_fc_parameters,
         2, 4},
        {"hgs_interface_port_fc_parametersGetFirst",
         &wrFx950_api_hgs_interface_get_port_fc_parameters,
         2, 0},
        {"hgs_interface_port_fc_parametersGetNext",
         &wrFx950_api_hgs_interface_get_port_fc_parameters_end,
         2, 0},
        {"hgs_interface_fc_packet_saSet",
         &wrFx950_api_hgs_interface_set_fc_packet_sa,
         2, 6},
        {"hgs_interface_fc_packet_saGetFirst",
         &wrFx950_api_hgs_interface_get_fc_packet_sa,
         2, 0},
        {"hgs_interface_fc_packet_saGetNext",
         &wrFx950_api_hgs_interface_get_fc_packet_sa_end,
         2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_hgs_interface
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
CMD_STATUS cmdLibInitFx950_api_hgs_interface
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

