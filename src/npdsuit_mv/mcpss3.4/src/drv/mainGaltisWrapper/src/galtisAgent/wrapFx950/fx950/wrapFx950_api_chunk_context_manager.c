/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_chunk_context_manager.c
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
#include <FX950/include/fx950_api_chunk_context_manager.h>

/*****************************************************
*NAME
*  fx950_api_chunk_context_manager_set_const_context_id
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure sets constant context ID for both links.
*  This value is used as a ContextID for unchopped packets sent to one of the links.
*  This context-ID must be used for the reassembly of chunks.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    context_id_0 - 
*       ContextID for unchopped packets sent to Link0 (0..0xFFF).
*    unsigned long    context_id_1 - 
*       ContextID for unchopped packets sent to Link1 (0..0xFFF).
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
static CMD_STATUS wrFx950_api_chunk_context_manager_set_const_context_id
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long context_id_0;
    unsigned long context_id_1;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    context_id_0 = (unsigned long)inArgs[1];
    context_id_1 = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_chunk_context_manager_set_const_context_id(device_id, context_id_0, context_id_1);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_chunk_context_manager_get_const_context_id
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets constant context ID of both links.
*  This value is used as a ContextID for unchopped packets sent to one of the links.
*  This context-ID must be used for the reassembly of chunks.
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
*    context_id_0_ptr - 
*       ContextID for unchopped packets sent to Link0.
*    context_id_1_ptr - 
*       ContextID for unchopped packets sent to Link1.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_chunk_context_manager_get_const_context_id
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long context_id_0_ptr;
    unsigned long context_id_1_ptr;
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
    result = fx950_api_chunk_context_manager_get_const_context_id(device_id, &context_id_0_ptr, &context_id_1_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", context_id_0_ptr, context_id_1_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure sets the Flow-ID to context-ID mapping.
*  The context-ID is used for chopped packets.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    flow_id - 
*       Flow-ID number (0..16K).
*    unsigned int valid - 
*       When valid = GT_TRUE, the entry is valid.
*       When valid = GT_FALSE, the entry is not valid.
*    unsigned long context_id - 
*       context ID to map to flow_id (0..0xFFF)
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
static CMD_STATUS wrFx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long flow_id;
    unsigned int valid;
    unsigned long context_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    flow_id = (unsigned long)inArgs[1];
    valid = (unsigned int)inArgs[2];
    context_id = (unsigned long)inArgs[3];

    /* call Ocelot API function */
    result = fx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping(device_id, flow_id, valid, context_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets the Flow-ID to context-ID mapping.
*  The context-ID is used for chopped packets.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    flow_id - 
*       Flow-ID number (0..16K).
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
*    valid_ptr - 
*       When valid = GT_TRUE, the entry is valid.
*       When valid = GT_FALSE, the entry is not valid.
*    context_id_ptr - 
*       context ID to map to flow_id (0..0xFFF)
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long flow_id;
    unsigned int valid_ptr;
    unsigned long context_id_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    flow_id = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping(device_id, flow_id, &valid_ptr, &context_id_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", valid_ptr, context_id_ptr);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"chunk_context_manager_set_const_context_id",
         &wrFx950_api_chunk_context_manager_set_const_context_id,
         3, 0},
        {"chunk_context_manager_get_const_context_id",
         &wrFx950_api_chunk_context_manager_get_const_context_id,
         1, 0},
        {"chunk_context_manager_set_flow_id_to_cnt_id_map",
         &wrFx950_api_chunk_context_manager_set_flow_id_to_context_id_mapping,
         4, 0},
        {"chunk_context_manager_get_flow_id_to_cnt_id_map",
         &wrFx950_api_chunk_context_manager_get_flow_id_to_context_id_mapping,
         2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_chunk_context_manager
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
GT_STATUS cmdLibInitFx950_api_chunk_context_manager
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

