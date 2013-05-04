/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_mh_interoperability.c
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
#include <FX950/include/fx950_api_mh_interoperability.h>

/*****************************************************
*NAME
*  fx950_api_mh_interoperability_set_mc_dest_mode
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure sets the Type of the descriptor that should be sent on the 
*  fabric in case of Multicast packet.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_MH_FORMAT_TYPE    mh_format_type -
*       Marvell Header format type: Standard or Extended.
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
static CMD_STATUS wrFx950_api_mh_interoperability_set_mc_dest_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_MH_FORMAT_TYPE mh_format_type;
    unsigned short rc;


    /* warning fix */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    mh_format_type = (FX950_MH_FORMAT_TYPE)inArgs[1];
    
    /* call Ocelot API function */
    result = fx950_api_mh_interoperability_set_mc_dest_mode(device_id, mh_format_type);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_mh_interoperability_get_mc_dest_mode
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the Type of the descriptor that should be sent on the 
*  fabric in case of Multicast packet.
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
*    mh_format_type_ptr -
*       Marvell Header format type: Standard or Extended.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_mh_interoperability_get_mc_dest_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_MH_FORMAT_TYPE mh_format_type_ptr;
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
    result = fx950_api_mh_interoperability_get_mc_dest_mode(device_id, &mh_format_type_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", mh_format_type_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_mh_interoperability_set_uc_dest_mode
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure indicates per destination device the Marvell Header format 
*  used when forwarding unicast packets to that destination.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_BMP_128  mh_format_type_vector_ptr -
*       Vector, with one bit per destination device indicating the MH format:
*       0 - Standard (EX), 1 - Extended (MX)
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
static CMD_STATUS wrFx950_api_mh_interoperability_set_uc_dest_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_BMP_128 mh_format_type_vector_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    mh_format_type_vector_ptr.bitmap[0] = (unsigned long) inFields[0];
    mh_format_type_vector_ptr.bitmap[1] = (unsigned long) inFields[1];
    mh_format_type_vector_ptr.bitmap[2] = (unsigned long) inFields[2];
    mh_format_type_vector_ptr.bitmap[3] = (unsigned long) inFields[3];
    
    /* call Ocelot API function */
    result = fx950_api_mh_interoperability_set_uc_dest_mode(device_id, &mh_format_type_vector_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_mh_interoperability_get_uc_dest_mode
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets per destination device the Marvell Header format 
*  used when forwarding unicast packets to that destination.
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
*    mh_format_type_vector_ptr -
*       Vector, with one bit per destination device indicating the MH format:
*       0 - Standard (EX), 1 - Extended (MX)
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_mh_interoperability_get_uc_dest_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_BMP_128 mh_format_type_vector_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_mh_interoperability_get_uc_dest_mode(device_id, &mh_format_type_vector_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = mh_format_type_vector_ptr.bitmap[0];
    inFields[1] = mh_format_type_vector_ptr.bitmap[1];
    inFields[2] = mh_format_type_vector_ptr.bitmap[2];
    inFields[3] = mh_format_type_vector_ptr.bitmap[3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_mh_interoperability_get_uc_dest_mode_end
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
*  fx950_api_mh_interoperability_set_uc_dest_device_mode
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure sets Marvell Header format type for unicast packets forwarded 
*  to given destination device.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*   unsigned int     dest_device_id - 
*       destination device
*   FX950_MH_FORMAT_TYPE   mh_format_type - 
*       Marvell Header format type: Standard or Extended.
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
static CMD_STATUS wrFx950_api_mh_interoperability_set_uc_dest_device_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int dest_device_id;
    FX950_MH_FORMAT_TYPE mh_format_type;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    dest_device_id = (unsigned int)inArgs[1];
    mh_format_type = (FX950_MH_FORMAT_TYPE)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_mh_interoperability_set_uc_dest_device_mode(device_id, dest_device_id, mh_format_type);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"mh_interoperability_set_mc_dest_mode",
         &wrFx950_api_mh_interoperability_set_mc_dest_mode,
         2, 0},
        {"mh_interoperability_get_mc_dest_mode",
         &wrFx950_api_mh_interoperability_get_mc_dest_mode,
         1, 0},
        {"mh_interoperability_uc_dest_modeSet",
         &wrFx950_api_mh_interoperability_set_uc_dest_mode,
         1, 4},
        {"mh_interoperability_uc_dest_modeGetFirst",
         &wrFx950_api_mh_interoperability_get_uc_dest_mode,
         1, 0},
        {"mh_interoperability_uc_dest_modeGetNext",
         &wrFx950_api_mh_interoperability_get_uc_dest_mode_end,
         1, 0},
        {"mh_interoperability_set_uc_dest_device_mode",
         &wrFx950_api_mh_interoperability_set_uc_dest_device_mode,
         3, 0},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_mh_interoperability
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
GT_STATUS cmdLibInitFx950_api_mh_interoperability
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

