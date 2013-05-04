/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_chopping.c
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
#include <FX950/include/fx950_api_chopping.h>

/*****************************************************
*NAME
*  fx950_api_chopping_set_max_chunk_sizes
*TYPE:
*  PROC
*DATE:
*  13/AUG/2007
*FUNCTION:
*  This procedure sets maximal chunk and maximal last chunk sizes.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long  max_chunk_size - 
*       This size in unis of 128 bytes, is the size of all the chunks, except
*        for the last chunk (max_last_chunk_size).
*       The Size is the payload size only.
*       The Value 0 is not allowed  (1..31).
*       For example: max_chunk_size with value 5 is 5*128 bytes.
*       The maximum chunk payload size is 3968 bytes.
*    unsigned long   max_last_chunk_size -
*       This size in units of 64 bytes, is the limit for chopping a packet.
*       The Value 0 is not allowed (1..127).
*       For example: max_last_chunk_size with value 5 is 5*64 bytes.
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
*
*  If the packet byte_count is bigger than LastChunkMaxSize*64, then the 
*  packet is chopped to ChunkMaxSize*128 byte segments, 
*  until the residue is smaller than LastChunkMaxSize*64.
*  The last chunk size can be upto LastChunkMaxSize*64.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_chopping_set_max_chunk_sizes
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long max_chunk_size;
    unsigned long max_last_chunk_size;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    max_chunk_size = (unsigned long)inArgs[1];
    max_last_chunk_size = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_chopping_set_max_chunk_sizes(device_id, max_chunk_size, max_last_chunk_size);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_chopping_get_max_chunk_sizes
*TYPE:
*  PROC
*DATE:
*  13/AUG/2007
*FUNCTION:
*  This procedure gets maximal chunk and maximal last chunk sizes.
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
*    max_chunk_size_ptr - 
*       This size in units of 128 bytes, is the size of all the chunks except
*       for the last chunk (max_last_chunk_size).
*    max_last_chunk_size_ptr -
*       This size in units of 64 bytes, is the limit for chopping a packet.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*
*  If the packet byte_count is bigger than LastChunkMaxSize*64, then the 
*  packet is chopped to ChunkMaxSize*128 byte segments, 
*  until the residue is smaller than LastChunkMaxSize*64.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_chopping_get_max_chunk_sizes
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long max_chunk_size_ptr;
    unsigned long max_last_chunk_size_ptr;
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
    result = fx950_api_chopping_get_max_chunk_sizes(device_id, &max_chunk_size_ptr, &max_last_chunk_size_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", max_chunk_size_ptr, max_last_chunk_size_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_chopping_set_enable_params
*TYPE:
*  PROC
*DATE:
*  13/AUG/2007
*FUNCTION:
*  This procedure sets global chopping enable parameters.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_ENABLE_PARAMS_STRUCT chop_en_params_ptr - 
*       Chopping Enable parameters. 
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
static CMD_STATUS wrFx950_api_chopping_set_enable_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_CHOPPING_ENABLE_PARAMS_STRUCT chop_en_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    chop_en_params_ptr.ucChopEn = (unsigned int)inFields[0];
    chop_en_params_ptr.mcChopEn = (unsigned int)inFields[1];
    chop_en_params_ptr.maxQueueNum = (unsigned long)inFields[2];

    /* call Ocelot API function */
    result = fx950_api_chopping_set_enable_params(device_id, &chop_en_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_chopping_get_enable_params
*TYPE:
*  PROC
*DATE:
*  13/AUG/2007
*FUNCTION:
*  This procedure gets global chopping enable parameters.
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
*    chop_en_params_ptr - 
*       Chopping Enable parameters. 
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_chopping_get_enable_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_CHOPPING_ENABLE_PARAMS_STRUCT chop_en_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_chopping_get_enable_params(device_id, &chop_en_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = chop_en_params_ptr.ucChopEn;
    inFields[1] = chop_en_params_ptr.mcChopEn;
    inFields[2] = chop_en_params_ptr.maxQueueNum;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_chopping_get_enable_params_end
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
*  fx950_api_chopping_set_enable_target_dev_bmp
*TYPE:
*  PROC
*DATE:
*  13/AUG/2007
*FUNCTION:
*  This procedure enables/disables chopping for all target devices.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_BMP_128 chop_en_dev_bmp_ptr - 
*       bitmap of target devices.
*       0 - disable chopping
*       1 - enable chopping 
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
static CMD_STATUS wrFx950_api_chopping_set_enable_target_dev_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_BMP_128 chop_en_dev_bmp_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    chop_en_dev_bmp_ptr.bitmap[0] = (unsigned long)inFields[0];
    chop_en_dev_bmp_ptr.bitmap[1] = (unsigned long)inFields[1];
    chop_en_dev_bmp_ptr.bitmap[2] = (unsigned long)inFields[2];
    chop_en_dev_bmp_ptr.bitmap[3] = (unsigned long)inFields[3];

    /* call Ocelot API function */
    result = fx950_api_chopping_set_enable_target_dev_bmp(device_id, &chop_en_dev_bmp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_chopping_get_enable_target_dev_bmp
*TYPE:
*  PROC
*DATE:
*  13/AUG/2007
*FUNCTION:
*  This procedure gets the status of chopping of all target devices 
*  (enabled/disabled).
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
*    chop_en_dev_bmp_ptr - 
*       bitmap of target devices.
*       0 - chopping disabled 
*       1 - chopping enabled
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_chopping_get_enable_target_dev_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_BMP_128 chop_en_dev_bmp_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_chopping_get_enable_target_dev_bmp(device_id, &chop_en_dev_bmp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = chop_en_dev_bmp_ptr.bitmap[0];
    inFields[1] = chop_en_dev_bmp_ptr.bitmap[1];
    inFields[2] = chop_en_dev_bmp_ptr.bitmap[2];
    inFields[3] = chop_en_dev_bmp_ptr.bitmap[3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_chopping_get_enable_target_dev_bmp_end
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
*  fx950_api_chopping_set_enable_target_dev
*TYPE:
*  PROC
*DATE:
*  13/AUG/2007
*FUNCTION:
*  This procedure enables/disables chopping per target device.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    target_dev_num - 
*       Target device number (0..127).
*    unsigned int     chop_en - 
*       When chop_en == TRUE, enable chopping for target device.
*       When chop_en == FALSE, disable chopping for target device.
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
static CMD_STATUS wrFx950_api_chopping_set_enable_target_dev
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long target_dev_num;
    unsigned int chop_en;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    target_dev_num = (unsigned long)inArgs[1];
    chop_en = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_chopping_set_enable_target_dev(device_id, target_dev_num, chop_en);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_chopping_set_enable_tc
*TYPE:
*  PROC
*DATE:
*  13/AUG/2007
*FUNCTION:
*  This procedure enables chopping per Traffic Class for Unicast and Multicast
*  packets.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_PACKETS_TYPE  packet_type - 
*       packets type: unicast or multicast
*    unsigned int    tc_chop_bmp - 
*       bitmap per tc for unicast/multicast packets, only LS bits [0..7] are used.
*       0 - disable chopping
*       1 - enable chopping 
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
static CMD_STATUS wrFx950_api_chopping_set_enable_tc
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_PACKETS_TYPE packet_type;
    unsigned int tc_chop_bmp;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    packet_type = (FX950_PACKETS_TYPE)inArgs[1];
    tc_chop_bmp = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_chopping_set_enable_tc(device_id, packet_type, tc_chop_bmp);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_chopping_get_enable_tc
*TYPE:
*  PROC
*DATE:
*  13/AUG/2007
*FUNCTION:
*  This procedure gets the status of chopping per Traffic Class for Unicast and Multicast
*  packets (enabled/disabled).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_PACKETS_TYPE  packet_type - 
*       packets type: unicast or multicast
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
*    tc_chop_bmp_ptr - 
*       bitmap per tc for unicast/multicast packets, only LS bits [0..7] are used.
*       0 - chopping disabled
*       1 - chopping enabled
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_chopping_get_enable_tc
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_PACKETS_TYPE packet_type;
    unsigned int tc_chop_bmp_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    packet_type = (FX950_PACKETS_TYPE)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_chopping_get_enable_tc(device_id, packet_type, &tc_chop_bmp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", tc_chop_bmp_ptr);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"chopping_set_max_chunk_sizes",
         &wrFx950_api_chopping_set_max_chunk_sizes,
         3, 0},
        {"chopping_get_max_chunk_sizes",
         &wrFx950_api_chopping_get_max_chunk_sizes,
         1, 0},
        {"chopping_enable_paramsSet",
         &wrFx950_api_chopping_set_enable_params,
         1, 3},
        {"chopping_enable_paramsGetFirst",
         &wrFx950_api_chopping_get_enable_params,
         1, 0},
        {"chopping_enable_paramsGetNext",
         &wrFx950_api_chopping_get_enable_params_end,
         1, 0},
        {"chopping_enable_target_dev_bmpSet",
         &wrFx950_api_chopping_set_enable_target_dev_bmp,
         1, 4},
        {"chopping_enable_target_dev_bmpGetFirst",
         &wrFx950_api_chopping_get_enable_target_dev_bmp,
         1, 0},
        {"chopping_enable_target_dev_bmpGetNext",
         &wrFx950_api_chopping_get_enable_target_dev_bmp_end,
         1, 0},
        {"chopping_set_enable_target_dev",
         &wrFx950_api_chopping_set_enable_target_dev,
         3, 0},
        {"chopping_set_enable_tc",
         &wrFx950_api_chopping_set_enable_tc,
         3, 0},
        {"chopping_get_enable_tc",
         &wrFx950_api_chopping_get_enable_tc,
         2, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_chopping
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
GT_STATUS cmdLibInitFx950_api_chopping
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

