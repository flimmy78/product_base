/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_scheduler.c
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
#include <FX950/include/fx950_api_scheduler.h>

/*****************************************************
*NAME
*  fx950_api_hgl_scheduler_set_mtu_size_mode
*TYPE:
*  PROC
*DATE:
*  05/AUG/2007
*FUNCTION:
*  This procedure sets MTU size for the WRR scheduling algorithm.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_HGL_SCHEDULER_MTU_SIZE_MODE mtu_size_mode - 
*       MTU size mode : 16k, 8K or 2K
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
static CMD_STATUS wrFx950_api_hgl_scheduler_set_mtu_size_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_HGL_SCHEDULER_MTU_SIZE_MODE mtu_size_mode;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    mtu_size_mode = (FX950_HGL_SCHEDULER_MTU_SIZE_MODE)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_hgl_scheduler_set_mtu_size_mode(device_id, mtu_size_mode);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_scheduler_get_mtu_size_mode
*TYPE:
*  PROC
*DATE:
*  05/AUG/2007
*FUNCTION:
*  This procedure gets MTU size for the WRR scheduling algorithm.
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
*    mtu_size_mode_ptr - 
*       MTU size mode: 16k, 8K or 2K
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_scheduler_get_mtu_size_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_HGL_SCHEDULER_MTU_SIZE_MODE mtu_size_mode_ptr;
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
    result = fx950_api_hgl_scheduler_get_mtu_size_mode(device_id, &mtu_size_mode_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", mtu_size_mode_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_scheduler_set_link_level_2_weight
*TYPE:
*  PROC
*DATE:
*  05/AUG/2007
*FUNCTION:
*  This procedure sets the weight assigned to link SP (Strict Priority) 
*  scheduler, when the 2nd level scheduler is SDWRR. 
* (Shaped Deficit Weighted Round Robin).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link  - 
*       Link number (0..1)
*    unsigned long sp_weight  - 
*       The weight assigned to link SP scheduler (0..254)
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
static CMD_STATUS wrFx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link;
    unsigned long sp_weight;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link = (unsigned long)inArgs[1];
    sp_weight = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight(device_id, link, sp_weight);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_scheduler_get_link_level_2_weight
*TYPE:
*  PROC
*DATE:
*  05/AUG/2007
*FUNCTION:
*  This procedure gets the weight assigned to link SP (Strict Priority) 
*  scheduler, when the 2nd level scheduler  is SDWRR 
* (Shaped Deficit Weighted Round Robin).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link  - 
*       Link number (0..1)
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
*    sp_weight_ptr  - 
*       The weight assigned to link SP scheduler
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_scheduler_get_link_level_2_weight
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link;
    unsigned long sp_weight_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_hgl_scheduler_get_link_level_2_weight(device_id, link, &sp_weight_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", sp_weight_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_scheduler_set_link_level_2_scheduler_type
*TYPE:
*  PROC
*DATE:
*  05/AUG/2007
*FUNCTION:
*  This procedure sets the scheduler type for level 2, per link.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link  - 
*       Link number (0..1)
*    FX950_HGL_SCHEDULER_TYPE    level_2_scheduler_type  - 
*       2nd level scheduler type: SP or SDWRR.
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
static CMD_STATUS wrFx950_api_hgl_scheduler_set_link_level_2_scheduler_type
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link;
    FX950_HGL_SCHEDULER_TYPE level_2_scheduler_type;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link = (unsigned long)inArgs[1];
    level_2_scheduler_type = (FX950_HGL_SCHEDULER_TYPE)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_hgl_scheduler_set_link_level_2_scheduler_type(device_id, link, level_2_scheduler_type);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_scheduler_get_link_level_2_scheduler_type
*TYPE:
*  PROC
*DATE:
*  05/AUG/2007
*FUNCTION:
*  This procedure gets the scheduler type for level 2, per link.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link  - 
*       Link number (0..1)
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
*    level_2_scheduler_type_ptr  - 
*       2nd level scheduler type: SP or SDWRR.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_scheduler_get_link_level_2_scheduler_type
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link;
    FX950_HGL_SCHEDULER_TYPE level_2_scheduler_type_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_hgl_scheduler_get_link_level_2_scheduler_type(device_id, link, &level_2_scheduler_type_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", level_2_scheduler_type_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_scheduler_set_link_queue_scheduling_type
*TYPE:
*  PROC
*DATE:
*  05/AUG/2007
*FUNCTION:
*  This procedure sets the scheduling type of the queue, per link and per 
*  traffic type (unicast/multicast).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link  - 
*       Link number (0..1)
*    FX950_HGL_SCHEDULER_TYPE    uc_queue_0_scheduler_type  - 
*       unicast queue 0 scheduler type: SP or SDWRR.
*    FX950_HGL_SCHEDULER_TYPE    uc_queue_1_scheduler_type  - 
*       unicast queue 1 scheduler type: SP or SDWRR.
*    FX950_HGL_SCHEDULER_TYPE    mc_queue_0_scheduler_type  - 
*       multicast queue 0 scheduler type: SP or SDWRR.
*    FX950_HGL_SCHEDULER_TYPE    mc_queue_1_scheduler_type  - 
*       multicast queue 1 scheduler type: SP or SDWRR.
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
static CMD_STATUS wrFx950_api_hgl_scheduler_set_link_queue_scheduling_type
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link;
    FX950_HGL_SCHEDULER_TYPE uc_queue_0_scheduler_type;
    FX950_HGL_SCHEDULER_TYPE uc_queue_1_scheduler_type;
    FX950_HGL_SCHEDULER_TYPE mc_queue_0_scheduler_type;
    FX950_HGL_SCHEDULER_TYPE mc_queue_1_scheduler_type;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link = (unsigned long)inArgs[1];
    uc_queue_0_scheduler_type = (FX950_HGL_SCHEDULER_TYPE)inArgs[2];
    uc_queue_1_scheduler_type = (FX950_HGL_SCHEDULER_TYPE)inArgs[3];
    mc_queue_0_scheduler_type = (FX950_HGL_SCHEDULER_TYPE)inArgs[4];
    mc_queue_1_scheduler_type = (FX950_HGL_SCHEDULER_TYPE)inArgs[5];

    /* call Ocelot API function */
    result = fx950_api_hgl_scheduler_set_link_queue_scheduling_type(device_id, link, 
                           uc_queue_0_scheduler_type, uc_queue_1_scheduler_type, 
                           mc_queue_0_scheduler_type, mc_queue_1_scheduler_type);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_scheduler_get_link_queue_scheduling_type
*TYPE:
*  PROC
*DATE:
*  05/AUG/2007
*FUNCTION:
*  This procedure gets the scheduling type of the queue, per link and per 
*  traffic type (unicast/multicast).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link  - 
*       Link number (0..1)
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
*    uc_queue_0_scheduler_type_ptr  - 
*       unicast queue 0 scheduler type: SP or SDWRR.
*    uc_queue_1_scheduler_type_ptr  - 
*       unicast queue 1 scheduler type: SP or SDWRR.
*    mc_queue_0_scheduler_type_ptr  - 
*       multicast queue 0 scheduler type: SP or SDWRR.
*    mc_queue_1_scheduler_type_ptr  - 
*       multicast queue 1 scheduler type: SP or SDWRR.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_scheduler_get_link_queue_scheduling_type
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link;
    FX950_HGL_SCHEDULER_TYPE uc_queue_0_scheduler_type_ptr;
    FX950_HGL_SCHEDULER_TYPE uc_queue_1_scheduler_type_ptr;
    FX950_HGL_SCHEDULER_TYPE mc_queue_0_scheduler_type_ptr;
    FX950_HGL_SCHEDULER_TYPE mc_queue_1_scheduler_type_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_hgl_scheduler_get_link_queue_scheduling_type(device_id, link, 
                     &uc_queue_0_scheduler_type_ptr, &uc_queue_1_scheduler_type_ptr, 
                     &mc_queue_0_scheduler_type_ptr, &mc_queue_1_scheduler_type_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d%d%d", uc_queue_0_scheduler_type_ptr, 
                                          uc_queue_1_scheduler_type_ptr,
                                          mc_queue_0_scheduler_type_ptr, 
                                          mc_queue_1_scheduler_type_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights
*TYPE:
*  PROC
*DATE:
*  05/AUG/2007
*FUNCTION:
*  This procedure sets the weight of queues per link and per traffic type
*  (unicast/multicast) , when they are part 
*  of the WRR arbitration.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link  - 
*       Link number (0..1)
*    unsigned long    uc_queue_0_weight  - 
*       Unicast queue 0 weight (0..254).
*    unsigned long    uc_queue_1_weight  - 
*       Unicast queue 1 weight (0..254).
*    unsigned long    mc_queue_2_weight  - 
*       Multicast queue 2 weight (0..254).
*    unsigned long    mc_queue_3_weight  - 
*       Multicast queue 3 weight (0..254).
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
static CMD_STATUS wrFx950_api_hgl_scheduler_set_link_queue_sdwrr_weights
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link;
    unsigned long uc_queue_0_weight;
    unsigned long uc_queue_1_weight;
    unsigned long mc_queue_0_weight;
    unsigned long mc_queue_1_weight;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link = (unsigned long)inArgs[1];
    uc_queue_0_weight = (unsigned long)inArgs[2];
    uc_queue_1_weight = (unsigned long)inArgs[3];
    mc_queue_0_weight = (unsigned long)inArgs[4];
    mc_queue_1_weight = (unsigned long)inArgs[5];

    /* call Ocelot API function */
    result = fx950_api_hgl_scheduler_set_link_queue_sdwrr_weights(device_id, link, 
                                            uc_queue_0_weight, uc_queue_1_weight, 
                                            mc_queue_0_weight, mc_queue_1_weight);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights
*TYPE:
*  PROC
*DATE:
*  05/AUG/2007
*FUNCTION:
*  This procedure gets the weight of queues per link and per traffic type
*  (unicast/multicast) .
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long link  - 
*       Link number (0..1)
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
*    uc_queue_0_weight_ptr  - 
*       Unicast queue 0 weight.
*    uc_queue_1_weight_ptr  - 
*       Unicast queue 1 weight.
*    mc_queue_2_weight_ptr  - 
*       Multicast queue 2 weight.
*    mc_queue_3_weight_ptr  - 
*       Multicast queue 3 weight.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_hgl_scheduler_get_link_queue_sdwrr_weights
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link;
    unsigned long uc_queue_0_weight_ptr;
    unsigned long uc_queue_1_weight_ptr;
    unsigned long mc_queue_2_weight_ptr;
    unsigned long mc_queue_3_weight_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    link = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_hgl_scheduler_get_link_queue_sdwrr_weights(device_id, link, 
                                   &uc_queue_0_weight_ptr, &uc_queue_1_weight_ptr, 
                                   &mc_queue_2_weight_ptr, &mc_queue_3_weight_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d%d%d", uc_queue_0_weight_ptr, uc_queue_1_weight_ptr, 
                                          mc_queue_2_weight_ptr, mc_queue_3_weight_ptr);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"hgl_scheduler_set_mtu_size_mode",
         &wrFx950_api_hgl_scheduler_set_mtu_size_mode,
         2, 0},
        {"hgl_scheduler_get_mtu_size_mode",
         &wrFx950_api_hgl_scheduler_get_mtu_size_mode,
         1, 0},
        {"hgl_scheduler_set_link_level_2_sdwrr_weight",
         &wrFx950_api_hgl_scheduler_set_link_level_2_sdwrr_weight,
         3, 0},
        {"hgl_scheduler_get_link_level_2_weight",
         &wrFx950_api_hgl_scheduler_get_link_level_2_weight,
         2, 0},
        {"hgl_scheduler_set_link_level_2_scheduler_type",
         &wrFx950_api_hgl_scheduler_set_link_level_2_scheduler_type,
         3, 0},
        {"hgl_scheduler_get_link_level_2_scheduler_type",
         &wrFx950_api_hgl_scheduler_get_link_level_2_scheduler_type,
         2, 0},
        {"hgl_scheduler_set_link_queue_scheduling_type",
         &wrFx950_api_hgl_scheduler_set_link_queue_scheduling_type,
         6, 0},
        {"hgl_scheduler_get_link_queue_scheduling_type",
         &wrFx950_api_hgl_scheduler_get_link_queue_scheduling_type,
         2, 0},
        {"hgl_scheduler_set_link_queue_sdwrr_weights",
         &wrFx950_api_hgl_scheduler_set_link_queue_sdwrr_weights,
         6, 0},
        {"hgl_scheduler_get_link_queue_sdwrr_weights",
         &wrFx950_api_hgl_scheduler_get_link_queue_sdwrr_weights,
         2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_scheduler
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
GT_STATUS cmdLibInitFx950_api_scheduler
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

