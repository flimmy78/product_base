/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_tail_drop.c
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
#include <FX950/include/fx950_api_tail_drop.h>

/*****************************************************
*NAME
*  fx950_api_tail_drop_set_packet_thresholds
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure sets sharing packet threshold and the limit of maximum number
*  of packet instances.
*  To disable sharing, set packet_share_threshold and buffer_total_threshold 
*  to 0x0.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long packet_share_threshold - 
*       Sharing threshold of total number of packets in queues (0..1530)
*    unsigned long packet_total_max_limit -
*       Limits the maximum number of packet instances (1..1530).
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
static CMD_STATUS wrFx950_api_tail_drop_set_packet_thresholds
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long packet_share_threshold;
    unsigned long packet_total_max_limit;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    packet_share_threshold = (unsigned long)inArgs[1];
    packet_total_max_limit = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_tail_drop_set_packet_thresholds(device_id, packet_share_threshold, packet_total_max_limit);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_tail_drop_get_packet_thresholds
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets sharing packet threshold and the limit of maximum number
*  of packet instances.
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
*    packet_share_threshold_ptr - 
*       Sharing threshold of total number of packets in queues.
*    packet_total_max_limit_ptr -
*       Limits the maximum number of packet instances.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_tail_drop_get_packet_thresholds
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long packet_share_threshold_ptr;
    unsigned long packet_total_max_limit_ptr;
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
    result = fx950_api_tail_drop_get_packet_thresholds(device_id, &packet_share_threshold_ptr,
                                                       &packet_total_max_limit_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", packet_share_threshold_ptr, packet_total_max_limit_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_tail_drop_set_total_buffer_threshold
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure sets sharing threshold of total number of buffers in 
*  Buffer Manager.
*  To disable sharing, set packet_share_threshold and buffer_total_threshold 
*  to 0x0.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long buffer_total_threshold - 
*       buffer total threshold (1..1446)
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
static CMD_STATUS wrFx950_api_tail_drop_set_total_buffer_threshold
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long buffer_total_threshold;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    buffer_total_threshold = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_tail_drop_set_total_buffer_threshold(device_id, buffer_total_threshold);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_tail_drop_get_total_buffer_threshold
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets sharing threshold of total number of buffers in 
*  Buffer Manager.
*  To disable sharing, set packet_share_threshold and buffer_total_threshold 
*  to 0x0.
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
*    buffer_total_threshold_ptr - 
*       buffer total threshold.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_tail_drop_get_total_buffer_threshold
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long buffer_total_threshold_ptr;
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
    result = fx950_api_tail_drop_get_total_buffer_threshold(device_id, &buffer_total_threshold_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", buffer_total_threshold_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_tail_drop_set_queue_thresholds
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure sets queue thresholds per link and per queue type.
*  Those thresholds limit the number of virtual buffers per queue.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link_num - 
*       link number (0..1)
*    unsigned long uc_queue_0_threshold - 
*       unicast queue 0 threshold (0..1530).
*    unsigned long uc_queue_1_threshold - 
*       unicast queue 1 threshold (0..1530).
*    unsigned long uc_queue_0_threshold - 
*       multicast queue 0 threshold (0..1530).
*    unsigned long uc_queue_1_threshold - 
*       multicast queue 1 threshold (0..1530).
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
static CMD_STATUS wrFx950_api_tail_drop_set_queue_thresholds
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link_num;
    unsigned long uc_queue_0_threshold;
    unsigned long uc_queue_1_threshold;
    unsigned long mc_queue_0_threshold;
    unsigned long mc_queue_1_threshold;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_num = (unsigned long)inArgs[1];
    uc_queue_0_threshold = (unsigned long)inArgs[2];
    uc_queue_1_threshold = (unsigned long)inArgs[3];
    mc_queue_0_threshold = (unsigned long)inArgs[4];
    mc_queue_1_threshold = (unsigned long)inArgs[5];

    /* call Ocelot API function */
    result = fx950_api_tail_drop_set_queue_thresholds(device_id, link_num, uc_queue_0_threshold, uc_queue_1_threshold,
                                                      mc_queue_0_threshold, mc_queue_1_threshold);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_tail_drop_get_queue_thresholds
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets queue thresholds per link.
*  Those thresholds limit the number of virtual buffers per queue.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link_num - 
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
*    uc_queue_0_threshold_ptr - 
*       unicast queue 0 threshold.
*    uc_queue_1_threshold_ptr - 
*       unicast queue 1 threshold.
*    mc_queue_0_threshold_ptr - 
*       multicast queue 0 threshold.
*    mc_queue_1_threshold_ptr - 
*       multicast queue 1 threshold.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_tail_drop_get_queue_thresholds
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link_num;
    unsigned long uc_queue_0_threshold_ptr;
    unsigned long uc_queue_1_threshold_ptr;
    unsigned long mc_queue_0_threshold_ptr;
    unsigned long mc_queue_1_threshold_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_num = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_tail_drop_get_queue_thresholds(device_id, link_num, &uc_queue_0_threshold_ptr, &uc_queue_1_threshold_ptr,
                                                      &mc_queue_0_threshold_ptr, &mc_queue_1_threshold_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d%d%d", uc_queue_0_threshold_ptr, uc_queue_1_threshold_ptr,
                                          mc_queue_0_threshold_ptr, mc_queue_1_threshold_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_tail_drop_set_link_thresholds
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure sets link thresholds.
*  Those thresholds limit the number of buffers consumed by link.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link_0_threshold - 
*       link 0 threshold (0..1446).
*    unsigned long link_1_threshold - 
*       link 1 threshold (0..1446).
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
static CMD_STATUS wrFx950_api_tail_drop_set_link_thresholds
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link_0_threshold;
    unsigned long link_1_threshold;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link_0_threshold = (unsigned long)inArgs[1];
    link_1_threshold = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_tail_drop_set_link_thresholds(device_id, link_0_threshold, link_1_threshold);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_tail_drop_get_link_thresholds
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets link thresholds.
*  Those thresholds limit the number of buffers consumed by link.
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
*    link_0_threshold - 
*       link 0 threshold .
*    long link_1_threshold - 
*       link 1 threshold .
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_tail_drop_get_link_thresholds
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link_0_threshold_ptr;
    unsigned long link_1_threshold_ptr;
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
    result = fx950_api_tail_drop_get_link_thresholds(device_id, &link_0_threshold_ptr, &link_1_threshold_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", link_0_threshold_ptr, link_1_threshold_ptr);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"tail_drop_set_packet_thresholds",
         &wrFx950_api_tail_drop_set_packet_thresholds,
         3, 0},
        {"tail_drop_get_packet_thresholds",
         &wrFx950_api_tail_drop_get_packet_thresholds,
         1, 0},
        {"tail_drop_set_total_buffer_threshold",
         &wrFx950_api_tail_drop_set_total_buffer_threshold,
         2, 0},
        {"tail_drop_get_total_buffer_threshold",
         &wrFx950_api_tail_drop_get_total_buffer_threshold,
         1, 0},
        {"tail_drop_set_queue_thresholds",
         &wrFx950_api_tail_drop_set_queue_thresholds,
         6, 0},
        {"tail_drop_get_queue_thresholds",
         &wrFx950_api_tail_drop_get_queue_thresholds,
         2, 0},
        {"tail_drop_set_link_thresholds",
         &wrFx950_api_tail_drop_set_link_thresholds,
         3, 0},
        {"tail_drop_get_link_thresholds",
         &wrFx950_api_tail_drop_get_link_thresholds,
         1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_tail_drop
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
GT_STATUS cmdLibInitFx950_api_tail_drop
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

