/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_queue_selection.c
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
#include <FX950/include/fx950_api_queue_selection.h>

/*****************************************************
*NAME
*  fx950_api_queue_selection_set_mc_identification_mode
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure defines the indexing mode of Multicast ID to Link Mapping Table. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MODE id_mode -
*       The indexing mode of MC-ID to Link Mapping Table
*       It can be Fabric Multicast ID, Marvell Header VID or Marvell Header VIDX.
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
static CMD_STATUS wrFx950_api_queue_selection_set_mc_identification_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MODE id_mode;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    id_mode = (FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MODE)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_set_mc_identification_mode(device_id, id_mode);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_get_mc_identification_mode
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets the indexing mode of Multicast ID to Link Mapping Table. 
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
*    *id_mode_ptr -
*       The indexing mode of MC-ID to Link Mapping Table
*       It can be Fabric Multicast ID, Marvell Header VID or Marvell Header VIDX.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_queue_selection_get_mc_identification_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_QUEUE_SELECTION_MC_ID_IDENTIFICATION_MODE id_mode_ptr;
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
    result = fx950_api_queue_selection_get_mc_identification_mode(device_id, &id_mode_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", id_mode_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_set_mc_id_range_to_link_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure maps a range of Multicast IDs to link selection mode. 
*  Both links may be selected.
*  This mapping is done in order to select a link for Multicast traffic.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    start_mc_index - 
*       The start index of the MC-ID to Link Map Table (0..8K)
*       The index can be: Fabric Multicast ID, Marvell Header VID or 
*       Marvell Header VIDX. The index mode is defined by 
*       fx950_api_queue_selection_set_mc_identification_mode
*    unsigned long    num_of_mc_indexes - 
*       The number of indexes to map to given link mode.
*    FX950_QUEUE_SELECTION_MC_ID_LINK_MODE link_mode -
*       Pointer to link mode selection
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
static CMD_STATUS wrFx950_api_queue_selection_set_mc_id_range_to_link_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long start_mc_index;
    unsigned long num_of_mc_indexes;
    FX950_QUEUE_SELECTION_MC_ID_LINK_MODE link_mode;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    start_mc_index = (unsigned long)inArgs[1];
    num_of_mc_indexes = (unsigned long)inArgs[2];
    link_mode = (FX950_QUEUE_SELECTION_MC_ID_LINK_MODE)inArgs[3];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_set_mc_id_range_to_link_map(device_id, start_mc_index, num_of_mc_indexes, link_mode);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_get_mc_id_to_link_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of Multicast ID to link number.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    mc_id - 
*       Multicast ID (0..2K)
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
*    link_mode_ptr -
*       link mode selection
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_queue_selection_get_mc_id_to_link_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long mc_id;
    FX950_QUEUE_SELECTION_MC_ID_LINK_MODE link_mode_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    mc_id = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_get_mc_id_to_link_map(device_id, mc_id, &link_mode_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", link_mode_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_set_flow_id_to_link_prio_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure maps Flow ID to link number and priority queue. 
*  This mapping is done in order to select a link for Chopped Unicast traffic 
*  and to select priority queue for Chopped traffic.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    start_flow_id_index - 
*       Start index of Flow ID (0..16K)
*    unsigned long    num_of_flow_id_indexes - 
*       The number of Flow ID indexes (0..16K)
*    unsigned long    link - 
*       Mapped link number (0..1)
*    unsigned long    priority - 
*       Mapped priority number (0..1)
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
static CMD_STATUS wrFx950_api_queue_selection_set_flow_id_range_to_link_prio_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long start_flow_id_index;
    unsigned long num_of_flow_id_indexes;
    unsigned long link;
    unsigned long priority;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    start_flow_id_index = (unsigned long)inArgs[1];
    num_of_flow_id_indexes = (unsigned long)inArgs[2];
    link = (unsigned long)inArgs[3];
    priority = (unsigned long)inArgs[4];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_set_flow_id_range_to_link_prio_map(device_id, start_flow_id_index, num_of_flow_id_indexes, link, priority);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_get_flow_id_to_link_prio_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of Flow ID to link number and priority queue. 
*  This mapping is done in order to select a link for Chopped Unicast traffic 
*  and to select priority queue for Chopped traffic.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    flow_id - 
*       Multicast ID (0..16K)
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
*    link - 
*       Mapped link number
*    priority - 
*       Mapped priority number 
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_queue_selection_get_flow_id_to_link_prio_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long mc_id;
    unsigned long link_ptr;
    unsigned long priority_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    mc_id = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_get_flow_id_to_link_prio_map(device_id, mc_id, &link_ptr, &priority_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", link_ptr, priority_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure maps all CPU Codes to link number. 
*  This mapping is done in order to select a link for Unchopped Unicast 
*  packets with DSA Tag "TO_CPU".
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_BMP_256 *cpu_code_to_link_bmp_ptr - 
*       CPU Code to link bitmap 
*           0 - link0
*           1 - link1
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
static CMD_STATUS wrFx950_api_queue_selection_set_cpu_codes_to_link_map_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_BMP_256 cpu_code_to_link_bmp_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    cpu_code_to_link_bmp_ptr.bitmap[0] = (unsigned int)inFields[0];
    cpu_code_to_link_bmp_ptr.bitmap[1] = (unsigned int)inFields[1];
    cpu_code_to_link_bmp_ptr.bitmap[2] = (unsigned int)inFields[2];
    cpu_code_to_link_bmp_ptr.bitmap[3] = (unsigned int)inFields[3];
    cpu_code_to_link_bmp_ptr.bitmap[4] = (unsigned int)inFields[4];
    cpu_code_to_link_bmp_ptr.bitmap[5] = (unsigned int)inFields[5];
    cpu_code_to_link_bmp_ptr.bitmap[6] = (unsigned int)inFields[6];
    cpu_code_to_link_bmp_ptr.bitmap[7] = (unsigned int)inFields[7];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_set_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of all CPU Codes to link number. 
*  This mapping is done in order to select a link for Unchopped Unicast 
*  packets with DSA Tag "TO_CPU".
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
*    cpu_code_to_link_bmp_ptr - 
*       CPU Code to link bitmap 
*           0 - link0
*           1 - link1
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_queue_selection_get_cpu_codes_to_link_map_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_BMP_256 cpu_code_to_link_bmp_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_get_cpu_codes_to_link_map_bmp(device_id, &cpu_code_to_link_bmp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = cpu_code_to_link_bmp_ptr.bitmap[0];
    inFields[1] = cpu_code_to_link_bmp_ptr.bitmap[1];
    inFields[2] = cpu_code_to_link_bmp_ptr.bitmap[2];
    inFields[3] = cpu_code_to_link_bmp_ptr.bitmap[3];

    inFields[4] = cpu_code_to_link_bmp_ptr.bitmap[4];
    inFields[5] = cpu_code_to_link_bmp_ptr.bitmap[5];
    inFields[6] = cpu_code_to_link_bmp_ptr.bitmap[6];
    inFields[7] = cpu_code_to_link_bmp_ptr.bitmap[7];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_queue_selection_get_cpu_codes_to_link_map_bmp_end
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
*  fx950_api_queue_selection_set_cpu_code_to_link
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure maps CPU Code to link number. 
*  This mapping is done in order to select a link for Unchopped Unicast 
*  packets with DSA Tag "TO_CPU".
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long cpu_code  - 
*       CPU Code (0..255)
*    unsigned long link  - 
*       Link nuber assigned to CPU Code (0..1)
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
static CMD_STATUS wrFx950_api_queue_selection_set_cpu_code_to_link
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long cpu_code;
    unsigned long link;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    cpu_code = (unsigned long)inArgs[1];
    link = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_set_cpu_code_to_link(device_id, cpu_code, link);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_set_ports_to_link_map_bmp
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure maps all ports to link number. 
*  This mapping is done in order to select a link for Unchopped Unicast 
*  packets with the following DSA Tags: EX/MX TO_ANALYZER and FROM_CPU, FORWARD,
*  when there is one target device (PP Mode == "One PP").
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_BMP_64 *port_bmp_ptr  - 
*       ports mapping to link bitmap 
*           0 - link0
*           1 - link1
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
static CMD_STATUS wrFx950_api_queue_selection_set_ports_to_link_map_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_BMP_64 port_bmp_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    port_bmp_ptr.bitmap[0] = (unsigned long)inFields[0];
    port_bmp_ptr.bitmap[1] = (unsigned long)inFields[1];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_set_ports_to_link_map_bmp(device_id, &port_bmp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_get_ports_to_link_map_bmp
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of all ports to link number. 
*  This mapping is done in order to select a link for Unchopped Unicast 
*  packets with the following DSA Tags: EX/MX TO_ANALYZER and FROM_CPU, FORWARD,
*  when there is one target device (PP Mode == "One PP").
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
*    port_bmp_ptr  - 
*       ports mapping to link bitmap 
*           0 - link0
*           1 - link1
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_queue_selection_get_ports_to_link_map_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_BMP_64 port_bmp_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_get_ports_to_link_map_bmp(device_id, &port_bmp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = port_bmp_ptr.bitmap[0];
    inFields[1] = port_bmp_ptr.bitmap[1];

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_queue_selection_get_ports_to_link_map_bmp_end
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
*  fx950_api_queue_selection_set_port_to_link_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure maps port to link number. 
*  This mapping is done in order to select a link for Unchopped Unicast 
*  packets with the following DSA Tags: EX/MX TO_ANALYZER and FROM_CPU, FORWARD,
*  when there is one target device (PP Mode == "One PP").
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   port_num  - 
*       port number (0..63)
*    unsigned long   link  - 
*       Link number to assign to given device (0..1)
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
static CMD_STATUS wrFx950_api_queue_selection_set_port_to_link_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long port_num;
    unsigned long link;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    port_num = (unsigned long)inArgs[1];
    link = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_set_port_to_link_map(device_id, port_num, link);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_dx_set_analyzer_link_prio_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure maps Ingress/Egress analyzer to link number and priority. 
*  This mapping is done in order to select a link and queue for Unchopped Unicast 
*  packets for DX mode with TO_ANALYZER DSA Tag.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   ingress_analyzer_link  - 
*       Link number to assign to ingress analyzer (0..1)
*    unsigned long   Egress_analyzer_link  - 
*       Link number to assign to egress analyzer (0..1)
*    unsigned long   to_analyzer_prio  - 
*       Priority queue number to assign to Ingress/Egress TO_ANALYZER 
*       DSA Tag (0..1)
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
static CMD_STATUS wrFx950_api_queue_selection_dx_set_analyzer_link_prio_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long ingress_analyzer_link;
    unsigned long egress_analyzer_link;
    unsigned long to_analyzer_prio;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    ingress_analyzer_link = (unsigned long)inArgs[1];
    egress_analyzer_link = (unsigned long)inArgs[2];
    to_analyzer_prio = (unsigned long)inArgs[3];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_dx_set_analyzer_link_prio_map(device_id, ingress_analyzer_link, egress_analyzer_link, to_analyzer_prio);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_dx_get_analyzer_link_prio_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of Ingress/Egress analyzer to link 
*  number and priority. 
*  This mapping is done in order to select a link and queue for Unchopped Unicast 
*  packets for DX mode with TO_ANALYZER DSA Tag.
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
*    ingress_analyzer_link_ptr  - 
*       Link number to assign to ingress analyzer
*    egress_analyzer_link_ptr  - 
*       Link number to assign to egress analyzer
*    to_analyzer_prio_ptr  - 
*       Priority queue number to assign to Ingress/Egress TO_ANALYZER DSA Tag 
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_queue_selection_dx_get_analyzer_link_prio_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long ingress_analyzer_link_ptr;
    unsigned long egress_analyzer_link_ptr;
    unsigned long to_analyzer_prio_ptr;
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
    result = fx950_api_queue_selection_dx_get_analyzer_link_prio_map(device_id, &ingress_analyzer_link_ptr, &egress_analyzer_link_ptr, &to_analyzer_prio_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d%d", ingress_analyzer_link_ptr,
                                        egress_analyzer_link_ptr,
                                        to_analyzer_prio_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_set_prio_mapping
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure maps TO_CPU Unchopped packets, traffic classes and 
*  traffic types priorities to priority queue selection. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_QUEUE_SELECTION_PRIO_MAP_STRUCT  *prio_map_ptr  - 
*       Priority queue mappings.
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
static CMD_STATUS wrFx950_api_queue_selection_set_prio_mapping
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_QUEUE_SELECTION_PRIO_MAP_STRUCT prio_map_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    prio_map_ptr.to_cpu_priority_queue_num = (unsigned long)inFields[0];
    prio_map_ptr.tc_to_prio_bmp = (unsigned long)inFields[1];
    prio_map_ptr.uc_prio_0_queue = (unsigned long)inFields[2];
    prio_map_ptr.uc_prio_1_queue = (unsigned long)inFields[3];
    prio_map_ptr.mc_prio_0_queue = (unsigned long)inFields[4];
    prio_map_ptr.mc_prio_1_queue = (unsigned long)inFields[5];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_set_prio_mapping(device_id, &prio_map_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_set_prio_mapping
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of TO_CPU Unchopped packets, traffic classes 
*  and traffic types priorities to priority queue selection. 
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
*    *prio_map_ptr  - 
*       Priority queue mappings.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_queue_selection_get_prio_mapping
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_QUEUE_SELECTION_PRIO_MAP_STRUCT prio_map_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_get_prio_mapping(device_id, &prio_map_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = prio_map_ptr.to_cpu_priority_queue_num;
    inFields[1] = prio_map_ptr.tc_to_prio_bmp;
    inFields[2] = prio_map_ptr.uc_prio_0_queue;
    inFields[3] = prio_map_ptr.uc_prio_1_queue;
    inFields[4] = prio_map_ptr.mc_prio_0_queue;
    inFields[5] = prio_map_ptr.mc_prio_1_queue;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_queue_selection_get_prio_mapping_end
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
*  fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure maps all QoS Profiles to priority queue.
*  This mapping is done in order to select queue priority for Unchopped packets
*  with "FORWARD" command in DX mode.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_BMP_128  *qos_profile_bmp_ptr - 
*       QoS Profile bitmap, queue priority (bit) per QoS profile 
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
static CMD_STATUS wrFx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_BMP_128 qos_profile_bmp_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    qos_profile_bmp_ptr.bitmap[0] = (unsigned long)inFields[0];
    qos_profile_bmp_ptr.bitmap[1] = (unsigned long)inFields[1];
    qos_profile_bmp_ptr.bitmap[2] = (unsigned long)inFields[2];
    qos_profile_bmp_ptr.bitmap[3] = (unsigned long)inFields[3];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of all QoS Profiles to priority queue.
*  This mapping is done in order to select queue priority for Unchopped packets
*  with "FORWARD" command in DX mode.
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
*    *qos_profile_bmp_ptr - 
*       QoS Profile bitmap, queue priority (bit) per QoS profile 
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_BMP_128 qos_profile_bmp_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp(device_id, &qos_profile_bmp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = qos_profile_bmp_ptr.bitmap[0];
    inFields[1] = qos_profile_bmp_ptr.bitmap[1];
    inFields[2] = qos_profile_bmp_ptr.bitmap[2];
    inFields[3] = qos_profile_bmp_ptr.bitmap[3];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp_end
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
*  fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure maps QoS Profile to priority queue.
*  This mapping is done in order to select queue priority for Unchopped packets
*  with "FORWARD" command in DX mode.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long       qos_profile - 
*       QoS Profile (0..127)
*    unsigned long       priority_queue_num - 
*       Priority queue assigned to QoS Profile (0..1)
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
static CMD_STATUS wrFx950_api_queue_selection_dx_set_qos_profile_to_prio_queue
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long qos_profile;
    unsigned long priority_queue_num;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    qos_profile = (unsigned long)inArgs[1];
    priority_queue_num = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_queue_selection_dx_set_qos_profile_to_prio_queue(device_id, qos_profile, priority_queue_num);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"queue_selection_set_mc_identification_mode",
         &wrFx950_api_queue_selection_set_mc_identification_mode,
         2, 0},
        {"queue_selection_get_mc_identification_mode",
         &wrFx950_api_queue_selection_get_mc_identification_mode,
         1, 0},
        {"queue_selection_set_mc_id_range_to_link_map",
         &wrFx950_api_queue_selection_set_mc_id_range_to_link_map,
         4, 0},
        {"queue_selection_get_mc_id_to_link_map",
         &wrFx950_api_queue_selection_get_mc_id_to_link_map,
         2, 0},
        {"queue_selection_set_flow_id_range_to_link_prio_map",
         &wrFx950_api_queue_selection_set_flow_id_range_to_link_prio_map,
         5, 0},
        {"queue_selection_get_flow_id_to_link_prio_map",
         &wrFx950_api_queue_selection_get_flow_id_to_link_prio_map,
         2, 0},
        {"queue_selection_cpu_to_link_map_bmpSet",
         &wrFx950_api_queue_selection_set_cpu_codes_to_link_map_bmp,
         1, 8},
        {"queue_selection_cpu_to_link_map_bmpGetFirst",
         &wrFx950_api_queue_selection_get_cpu_codes_to_link_map_bmp,
         1, 0},
        {"queue_selection_cpu_to_link_map_bmpGetNext",
         &wrFx950_api_queue_selection_get_cpu_codes_to_link_map_bmp_end,
         1, 0},
        {"queue_selection_set_cpu_code_to_link",
         &wrFx950_api_queue_selection_set_cpu_code_to_link,
         3, 0},
        {"queue_selection_ports_to_link_map_bmpSet",
         &wrFx950_api_queue_selection_set_ports_to_link_map_bmp,
         1, 2},
        {"queue_selection_ports_to_link_map_bmpGetFirst",
         &wrFx950_api_queue_selection_get_ports_to_link_map_bmp,
         1, 0},
        {"queue_selection_ports_to_link_map_bmpGetNext",
         &wrFx950_api_queue_selection_get_ports_to_link_map_bmp_end,
         1, 0},
        {"queue_selection_set_port_to_link_map",
         &wrFx950_api_queue_selection_set_port_to_link_map,
         3, 0},
        {"queue_selection_dx_set_analyzer_link_prio_map",
         &wrFx950_api_queue_selection_dx_set_analyzer_link_prio_map,
         4, 0},
        {"queue_selection_dx_get_analyzer_link_prio_map",
         &wrFx950_api_queue_selection_dx_get_analyzer_link_prio_map,
         1, 0},
        {"queue_selection_prio_mappingSet",
         &wrFx950_api_queue_selection_set_prio_mapping,
         1, 6},
        {"queue_selection_prio_mappingGetFirst",
         &wrFx950_api_queue_selection_get_prio_mapping,
         1, 0},
        {"queue_selection_prio_mappingGetNext",
         &wrFx950_api_queue_selection_get_prio_mapping_end,
         1, 0},
        {"queue_selection_dx_qos_prof_to_prio_mapSet",
         &wrFx950_api_queue_selection_dx_set_qos_profile_to_prio_queue_map_bmp,
         1, 4},
        {"queue_selection_dx_qos_prof_to_prio_mapGetFirst",
         &wrFx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp,
         1, 0},
        {"queue_selection_dx_qos_prof_to_prio_mapGetNext",
         &wrFx950_api_queue_selection_dx_get_qos_profile_to_prio_queue_map_bmp_end,
         1, 0},
        {"queue_selection_dx_set_qos_profile_to_prio_queue",
         &wrFx950_api_queue_selection_dx_set_qos_profile_to_prio_queue,
         3, 0}
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
GT_STATUS cmdLibInitFx950_api_queue_selection
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

