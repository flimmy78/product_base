
/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_flow_control.c
*
* DESCRIPTION:
*       Wrapper functions for Flow Control APIs of FX950 device.
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
#include <FX950/include/fx950_api_flow_control.h>

/*****************************************************
*NAME
*  fx950_api_flow_control_get_scheduler_status
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  This procedure gets scheduler status.
*
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    scheduler_id -
*       Scheduler number (0..255).
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
*    status_ptr - 
*       Pointer to scheduler status.
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_get_scheduler_status
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_FC_SCHEDULER_STATUS status;
    unsigned long scheduler_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    scheduler_id = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_flow_control_get_scheduler_status(device_id, scheduler_id, &status);

    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", status);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_flow_control_set_scheduler_default_state
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  This procedure sets default flow control state for specific scheduler.
*  The default state is used only when scheduler is configured to use 
*  static flow control configuration by fx950_api_flow_control_set_scheduler_mode() API.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    scheduler_id -
*       Scheduler number (0..255).
*    FX950_FC_SCHEDULER_STATUS      default_state -
*       Default scheduler state: XON or XOFF.
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
*    None. 
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_set_scheduler_default_state
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_FC_SCHEDULER_STATUS default_state;
    unsigned long scheduler_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    scheduler_id = (unsigned long)inArgs[1];
    default_state = (FX950_FC_SCHEDULER_STATUS)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_flow_control_set_scheduler_default_state(device_id, 
                                                                scheduler_id, 
                                                                default_state);

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
*  fx950_api_flow_control_set_scheduler_default_state_bmp
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  This procedure sets default flow control state for all 256 schedulers.
*  The default state is used only when scheduler is configured to use 
*  static flow control configuration by fx950_api_flow_control_set_scheduler_mode() API.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_BMP_256      *default_state_bmp_ptr -
*      Default flow control state 
*           0 - XON
*           1 - XOFF
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
*       None.
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_set_scheduler_default_state_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long   result;
    unsigned int    device_id;
    FX950_BMP_256   default_state_bmp;
    unsigned long   i;
    unsigned short  rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    for (i = 0; i < (sizeof(default_state_bmp.bitmap)/sizeof(default_state_bmp.bitmap[0])); i++) 
    {
        default_state_bmp.bitmap[i] = (unsigned long)inFields[i];
    }


    /* call Ocelot API function */
    result = fx950_api_flow_control_set_scheduler_default_state_bmp(device_id, 
                                                                    &default_state_bmp);

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
*  fx950_api_flow_control_get_scheduler_default_state_bmp
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  This procedure gets default flow control state of all 256 schedulers.
*  The default state is used only when scheduler is configured to use 
*  static flow control configuration by fx950_api_flow_control_set_scheduler_mode() API.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
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
*    default_state_bmp_ptr -
*      Pointer to Default flow control state 
*           0 - XON
*           1 - XOFF
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_get_scheduler_default_state_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long   result;
    unsigned int    device_id;
    FX950_BMP_256   default_state_bmp;
    unsigned long   i;
    unsigned short  rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_flow_control_get_scheduler_default_state_bmp(device_id, 
                                                                    &default_state_bmp);
    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    for (i = 0; i < (sizeof(default_state_bmp.bitmap)/sizeof(default_state_bmp.bitmap[0])); i++)
    {
        inFields[i] = default_state_bmp.bitmap[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_flow_control_get_scheduler_default_state_bmp_end
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
*  fx950_api_flow_control_set_scheduler_mode
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  Sets scheduler mode to be according to the Flow control message or according 
*  to a static configuration.
*
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    scheduler_id -
*       Scheduler number (0..255).
*    FX950_FC_SCHEDULER_MODE     mode -
*       Scheduler mode.
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
*      None.
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_set_scheduler_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long   result;
    unsigned int    device_id;
    unsigned long   scheduler_id;
    FX950_FC_SCHEDULER_MODE   mode;
    unsigned short  rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    scheduler_id = (unsigned long)inArgs[1];
    mode = (FX950_FC_SCHEDULER_MODE)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_flow_control_set_scheduler_mode(device_id, 
                                                       scheduler_id,
                                                       mode);

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
*  fx950_api_flow_control_set_scheduler_mode_bmp
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  Sets scheduler mode to be according to the Flow control message or according 
*  to a static configuration for all schedulers.
*
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_BMP_256      *mode_bmp_ptr -
*      Pointer to bitmap Scheduler modes.
*       0 - The scheduler status is static, according to the Scheduler Default value.
*           The default value is configured by 
*           fx950_api_flow_control_set_scheduler_default_state() and
*           fx950_api_flow_control_set_scheduler_default_state_bmp APIs.
*       1 - The scheduler status is dynamically updated according to the
*           Flow Control messages.
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
*      None.
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_set_scheduler_mode_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long   result;
    unsigned int    device_id;
    FX950_BMP_256   mode_bmp;
    unsigned long   i;
    unsigned short  rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    for (i = 0; i < (sizeof(mode_bmp.bitmap)/sizeof(mode_bmp.bitmap[0])); i++)
    {
        mode_bmp.bitmap[i] = (unsigned long)inFields[i];
    }

    /* call Ocelot API function */
    result = fx950_api_flow_control_set_scheduler_mode_bmp(device_id, 
                                                           &mode_bmp);

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
*  fx950_api_flow_control_get_scheduler_mode_bmp
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  Gets scheduler mode.
*
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
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
*    *mode_bmp_ptr -
*      Pointer to bitmap Scheduler modes.
*       0 - The scheduler status is static, according to the Scheduler Default value.
*           The default value is configured by 
*           fx950_api_flow_control_set_scheduler_default_state() and
*           fx950_api_flow_control_set_scheduler_default_state_bmp APIs.
*       1 - The scheduler status is dynamically updated according to the
*           Flow Control messages.
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_get_scheduler_mode_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long   result;
    unsigned int    device_id;
    FX950_BMP_256   mode_bmp;
    unsigned long   i;
    unsigned short  rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_flow_control_get_scheduler_mode_bmp(device_id, 
                                                           &mode_bmp);
    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    for (i = 0; i < (sizeof(mode_bmp.bitmap)/sizeof(mode_bmp.bitmap[0])); i++) 
    {
        inFields[i] = mode_bmp.bitmap[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_flow_control_get_scheduler_mode_bmp_end
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
*  fx950_api_flow_control_set_cfg
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  Sets flow control configurations. 
*
*INPUT:
*  DIRECT:
*    unsigned int                           device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long                          port_queue_num -
*       Port queue, that controls the port-scheduler (0..7).
*    FX950_FC_STATUS_PAUSE_THRESHOLD_TYPE   status_pause_thr -
*       Indicates the threshold that pauses the port-scheduler.
*    FX950_FC_PARSER_MODE                   parser_mode - 
*       Indicates the parser mode: per port or per FC priority.
*    FX950_FC_SIGNAL_MODE                   signal_mode - 
*       The mode which is used to signal egress port queues status of 
*       Packet Processor.
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
*      None.
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_set_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long   result;
    unsigned int    device_id;
    unsigned long   port_queue_num;
    FX950_FC_STATUS_PAUSE_THRESHOLD_TYPE   status_pause_thr;
    FX950_FC_PARSER_MODE                   parser_mode;
    FX950_FC_SIGNAL_MODE                   signal_mode;
    unsigned short  rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_queue_num = (unsigned long)inArgs[1];
    status_pause_thr = (FX950_FC_STATUS_PAUSE_THRESHOLD_TYPE)inArgs[2];
    parser_mode = (FX950_FC_PARSER_MODE)inArgs[3];
    signal_mode = (FX950_FC_SIGNAL_MODE)inArgs[4];

    /* call Ocelot API function */
    result = fx950_api_flow_control_set_cfg(device_id, 
                                            port_queue_num,
                                            status_pause_thr,
                                            parser_mode,
                                            signal_mode);

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
*  fx950_api_flow_control_get_cfg
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  Gets flow control configurations. 
*
*INPUT:
*  DIRECT:
*    unsigned int                           device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
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
*    port_queue_num_ptr -
*       Pointer to Port queue, that controls the port-scheduler (0..7).
*    status_pause_thr_ptr -
*       Pointer to the threshold that pauses the port-scheduler.
*    parser_mode_ptr - 
*       Pointer to the parser mode: per port or per FC priority.
*    signal_mode_ptr - 
*       Pointer to the mode, which is used to signal egress port queues 
*       status of Packet Processor.
.
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_get_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long   result;
    unsigned int    device_id;
    unsigned long   port_queue_num;
    FX950_FC_STATUS_PAUSE_THRESHOLD_TYPE   status_pause_thr;
    FX950_FC_PARSER_MODE                   parser_mode;
    FX950_FC_SIGNAL_MODE                   signal_mode;
    unsigned short  rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_flow_control_get_cfg(device_id, 
                                            &port_queue_num,
                                            &status_pause_thr,
                                            &parser_mode,
                                            &signal_mode);

    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d%d%d", port_queue_num, status_pause_thr, parser_mode, signal_mode);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_flow_control_set_status_offset
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  Sets the offset of Inband port-level flow control (IB-PLFC) received from HyperG.Link1.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long                  offset -
*       Offset of Inband port-level flow control (0..31).
*       For example:
*       offset = 0, IB-PLFC from HyperG.Link1 is ignored. IB-PLFC from HyperG.Link0 
*                    controls HR schedulers [255:0].
*       offset = 1, HR schedulers [255:0] are controlled by {IB-PLFC HyperG.Link1[7:0], 
*                   IB-PLFC HyperG.Link0[247:0]}.
*       offset = 2, HR[255:0] are controlled by {IB-PLFC HyperG.Link1[15:0], 
*                   IB-PLFC HyperG.Link0[239:0]}.
*       offset = 31, HR[255:0] are controlled by {IB-PLFC HyperG.Link1[247:0], 
*                   IB-PLFC HyperG.Link0[7:0]}
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
*      None.
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_set_status_offset
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long   result;
    unsigned int    device_id;
    unsigned long   offset;
    unsigned short  rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    offset = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_flow_control_set_status_offset(device_id, 
                                                      offset);

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
*  fx950_api_flow_control_get_status_offset
*TYPE:
*  PROC
*DATE:
*  16/APR/2008
*FUNCTION:
*  Gets the offset of Inband port-level flow control (IB-PLFC) received from HyperG.Link1.
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
*    offset_ptr -
*       Pointer to offset of Inband port-level flow control (0..31).
*       For example:
*       offset = 0, IB-PLFC from HyperG. Link1 is ignored. IB-PLFC from HyperG.Link0  
*                   controls HR schedulers [255:0].
*       offset = 1, HR schedulers [255:0] are controlled by {IB-PLFC HyperG.Link1[7:0], 
*                   IB-PLFC HyperG.Link0[247:0]}.
*       offset = 2, HR[255:0] are controlled by {IB-PLFC HyperG.Link1[15:0], 
*                   IB-PLFC HyperG.Link0[239:0]}.
*       offset = 31, HR[255:0] are controlled by {IB-PLFC HyperG.Link1[247:0], 
*                   IB-PLFC HyperG.Link0[7:0]}
*REMARKS:
*  + Device must have been registered for this action to be
*    carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_flow_control_get_status_offset
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{

    unsigned long   result;
    unsigned int    device_id;
    unsigned long   offset;
    unsigned short  rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    offset = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_flow_control_get_status_offset(device_id, 
                                                      &offset);

    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", offset);

    return CMD_OK;
}

static CMD_COMMAND dbCommands[] =
{
        {"flow_control_get_scheduler_status",
         &wrFx950_api_flow_control_get_scheduler_status,
         2, 0},

        {"flow_control_set_scheduler_default_state",
         &wrFx950_api_flow_control_set_scheduler_default_state,
         3, 0},

        {"flow_control_set_scheduler_default_state_bmpSet",
         &wrFx950_api_flow_control_set_scheduler_default_state_bmp,
         1, 8},

        {"flow_control_set_scheduler_default_state_bmpGetFirst",
         &wrFx950_api_flow_control_get_scheduler_default_state_bmp,
         1, 0},

        {"flow_control_set_scheduler_default_state_bmpGetNext",
         &wrFx950_api_flow_control_get_scheduler_default_state_bmp_end,
         1, 0},

        {"flow_control_set_scheduler_mode",
         &wrFx950_api_flow_control_set_scheduler_mode,
         3, 0},

        {"flow_control_set_scheduler_mode_bmpSet",
         &wrFx950_api_flow_control_set_scheduler_mode_bmp,
         1, 8},

        {"flow_control_set_scheduler_mode_bmpGetFirst",
         &wrFx950_api_flow_control_get_scheduler_mode_bmp,
         1, 0},

        {"flow_control_set_scheduler_mode_bmpGetNext",
         &wrFx950_api_flow_control_get_scheduler_mode_bmp_end,
         1, 0},

        {"flow_control_set_cfg",
         &wrFx950_api_flow_control_set_cfg,
         5, 0},

        {"flow_control_get_cfg",
         &wrFx950_api_flow_control_get_cfg,
         1, 0},

        {"flow_control_set_status_offset",
         &wrFx950_api_flow_control_set_status_offset,
         2, 0},

        {"flow_control_get_status_offset",
         &wrFx950_api_flow_control_get_status_offset,
         1, 0}

};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_queue_selection
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
GT_STATUS cmdLibInitFx950_api_flow_control
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

