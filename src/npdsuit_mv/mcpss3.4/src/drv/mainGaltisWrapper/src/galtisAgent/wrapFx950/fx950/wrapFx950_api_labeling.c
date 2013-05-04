/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_labeling.c
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
#include <FX950/include/fx950_api_labeling.h>


static GT_U32 index;
static unsigned long tbl_index;
static unsigned long global_voq;
static FX950_LABELING_UC_LABEL_PARAM_ENTRY uc_label_params;

/*****************************************************
*NAME
*  fx950_api_labeling_set_add_mc_header_enable
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure enables/disables adding MC header to Multicast packets.
*  If MC header is not added, the device processes Multicast packets like Unicast.
*  That is, the packets must be queued to VoQs 3-0 and can't be replicated by the
*  device / 98FX9310. 
*  If MC header is added, the traffic is then enqueueud to VOQ queues 0:3, 
*  replicated and forwarded to Fabric-MC-ID.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int    add_mc_header_enable -
*       When add_mc_header_enable == TRUE, enables adding of MC header
*       When add_mc_header_enable == FALSE, disables adding of MC header 
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
static CMD_STATUS wrFx950_api_labeling_set_add_mc_header_enable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int add_mc_header_enable;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    add_mc_header_enable = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_labeling_set_add_mc_header_enable(device_id, add_mc_header_enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_get_add_mc_header_enable
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the status of adding MC header to Multicast 
*  packets (enabled/disabled) . 
*  If MC header is not added, the device processes Multicast packets like Unicast.
*  That is, the packets must be queued to VoQs 3-0 and can't be replicated by the
*  device / 98FX9310. 
*  If MC header is added, the traffic is then enqueueud to VOQ queues 0:3, 
*  replicated and forwarded to Fabric-MC-ID.
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
*    add_mc_header_enable_ptr -
*       pointer to MC header replication status.
*       When add_mc_header_enable == TRUE, enables adding of MC header
*       When add_mc_header_enable == FALSE, disables adding of MC header 
*       single-destination.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_add_mc_header_enable
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int add_mc_header_enable_ptr;
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
    result = fx950_api_labeling_get_add_mc_header_enable(device_id, &add_mc_header_enable_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", add_mc_header_enable_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_set_cpu_code_label_map
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure maps CPU code to label.
*  For TO_CPU traffic the queue selection is done in the following manner:
*  queue number = CPUCode2VoQ<CPUCode>.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    cpu_code - 
*       CPU code (0..255)
*    unsigned long   voq -
*       The voq number to be mapped to given cpu_code (0..12K).
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
static CMD_STATUS wrFx950_api_labeling_set_cpu_code_label_map
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
    unsigned long voq;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    cpu_code = (unsigned long)inFields[0];
    voq = (unsigned long)inFields[1];
    
    /* call Ocelot API function */
    result = fx950_api_labeling_set_cpu_code_label_map(device_id, cpu_code, voq);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_get_cpu_code_label_map
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of CPU code to label.
*  For TO_CPU traffic the queue selection is done in the following manner:
*  queue number = CPUCode2VoQ<CPUCode>.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    cpu_code - 
*       CPU code (0..255)
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
*    voq_ptr -
*       The voq number to be mapped to given cpu_code.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_cpu_code_label_map
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
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    cpu_code = index = 0;

    /* call Ocelot API function */
    result = fx950_api_labeling_get_cpu_code_label_map(device_id, cpu_code, &global_voq);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = index;
    inFields[1] = global_voq;

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_cpu_code_label_map_end
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
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (++index >= 256)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    cpu_code = index;

    /* call Ocelot API function */
    result = fx950_api_labeling_get_cpu_code_label_map(device_id, cpu_code, &global_voq);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = index;
    inFields[1] = global_voq;
    
    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);
    
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_set_priority_map
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure mutates new Unicast/Multicast priority for each one of the 
*  priorities. The mutated priority is then used for Unicast/Multicast queue 
*  selection.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_PACKETS_TYPE  packet_type - 
*       packets type: unicast or multicast
*    unsigned long priority_new -
*       array of new (mutated) Unicast/Multicast priorities. 
*       Priority range is (0..7).
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
static CMD_STATUS wrFx950_api_labeling_set_priority_map
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
    unsigned long priority_new[FX950_LABELING_NOF_PRIORITIES];
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id   = (unsigned int)inArgs[0];
    packet_type = (unsigned int)inArgs[1];

    priority_new[0] = (unsigned int)inFields[0];
    priority_new[1] = (unsigned int)inFields[1];
    priority_new[2] = (unsigned int)inFields[2];
    priority_new[3] = (unsigned int)inFields[3];
    priority_new[4] = (unsigned int)inFields[4];
    priority_new[5] = (unsigned int)inFields[5];
    priority_new[6] = (unsigned int)inFields[6];
    priority_new[7] = (unsigned int)inFields[7];

    /* call Ocelot API function */
    result = fx950_api_labeling_set_priority_map(device_id, packet_type, priority_new);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_get_priority_map
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the mutations for each one of Unicast/Multicast priorities.
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
*   priority_new - 
*   array of new (mutated) Unicast/Multicast priorities.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_priority_map
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
    unsigned long priority_new[FX950_LABELING_NOF_PRIORITIES];
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id   = (unsigned int)inArgs[0];
    packet_type = (unsigned int)inArgs[1];


    /* call Ocelot API function */
    result = fx950_api_labeling_get_priority_map(device_id, packet_type, priority_new);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = priority_new[0];
    inFields[1] = priority_new[1];
    inFields[2] = priority_new[2];
    inFields[3] = priority_new[3];
    inFields[4] = priority_new[4];
    inFields[5] = priority_new[5];
    inFields[6] = priority_new[6];
    inFields[7] = priority_new[7];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_labeling_get_priority_map_end
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
*  fx950_api_labeling_set_link_base_queue_number
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure sets for each source link labeling offset.
*  This offset determines the link base queue number of the packet.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   link_base_queue_number -
*       array of base labeling offsets per source link.
*       The range of each offset is (0..12K).
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
static CMD_STATUS wrFx950_api_labeling_set_link_base_queue_number
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link_base_queue_number[FX950_NOF_HGS_LINKS];
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id   = (unsigned int)inArgs[0];

    link_base_queue_number[0] = (unsigned long)inFields[0];
    link_base_queue_number[1] = (unsigned long)inFields[1];

    /* call Ocelot API function */
    result = fx950_api_labeling_set_link_base_queue_number(device_id, link_base_queue_number);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_get_link_base_queue_number
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets for each source link the labeling offset.
*  This offset determines the link base queue number of the packet.
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
*    link_base_queue_number -
*       array of base labeling offsets per source link.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_link_base_queue_number
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long link_base_queue_number[FX950_NOF_HGS_LINKS];
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_labeling_get_link_base_queue_number(device_id, link_base_queue_number);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = link_base_queue_number[0];
    inFields[1] = link_base_queue_number[1];

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_labeling_get_link_base_queue_number_end
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
*  fx950_api_labeling_set_uc_label_tbl_entry
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure configures Unicast Label Table entry. The table is indexed 
*  according to access mode (fx950_api_labeling_set_uc_label_tbl_access_mode) by  
*  port or by device.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   index -
*       Index in the Unicast Label Table (0..127). 
*       The index can be either target device or target port, according to 
*       Unicast label access mode. If the access mode is Port,  
*       then only indexes 0-63 are used.
*    FX950_LABELING_UC_LABEL_PARAM_ENTRY uc_label_params_ptr - 
*       Unicast label parameters.
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
static CMD_STATUS wrFx950_api_labeling_set_uc_label_tbl_entry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long index;
    FX950_LABELING_UC_LABEL_PARAM_ENTRY uc_label_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    index = (unsigned long)inArgs[1];

    uc_label_params_ptr.valid = (unsigned int)inFields[0];
    uc_label_params_ptr.use_flow_mode = (FX950_LABELING_UC_LABEL_MODE)inFields[1];
    uc_label_params_ptr.port_base_queue_number = (unsigned long)inFields[2];
    uc_label_params_ptr.flow_base_queue_number = (unsigned long)inFields[3];

    /* call Ocelot API function */
    result = fx950_api_labeling_set_uc_label_tbl_entry(device_id, index, &uc_label_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_get_uc_label_tbl_entry
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the configuration of UC Label Table entry. 
*  The table is indexed according to access mode 
*  (fx950_api_labeling_set_uc_label_tbl_access_mode) by port or by device.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   index -
*       Index in the Unicast Label Table (0..127). 
*       The index can be either target device or target port, according to 
*       Unicast label access mode. If the access mode is Port,  
*       then only indexes 0-63 are used.
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
*   uc_label_params_ptr - 
*   Unicast label Tabel entry parameters.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_uc_label_tbl_entry
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    tbl_index = 0;

    /* call Ocelot API function */
    result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, tbl_index, &uc_label_params);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = uc_label_params.valid;
    inFields[1] = uc_label_params.use_flow_mode;
    inFields[2] = uc_label_params.port_base_queue_number;
    inFields[3] = uc_label_params.flow_base_queue_number;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_uc_label_tbl_entry_end
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    if (++tbl_index >= FX950_NOF_DESTINATION_DEVICES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* call Ocelot API function */
    result = fx950_api_labeling_get_uc_label_tbl_entry(device_id, tbl_index, &uc_label_params);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = uc_label_params.valid;
    inFields[1] = uc_label_params.use_flow_mode;
    inFields[2] = uc_label_params.port_base_queue_number;
    inFields[3] = uc_label_params.flow_base_queue_number;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_set_ingress_labeling_parameters
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure configures Ingress labeling parameters, needed to 
*  define the queue selection process for both Unicast and Multicast packets.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_LABELING_INGRESS_PARAMS_STRUCT   label_params_ptr -
*       Ingress labeling parameters
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
static CMD_STATUS wrFx950_api_labeling_set_ingress_labeling_parameters
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_LABELING_INGRESS_PARAMS_STRUCT label_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    label_params_ptr.uc_label_mode = (FX950_LABELING_UC_LABEL_MODE)inFields[0];
    label_params_ptr.port_add_enable = (unsigned int)inFields[1];
    label_params_ptr.flow_add_enable = (unsigned int)inFields[2];
    label_params_ptr.uc_priority_num_bit = (unsigned long)inFields[3];
    label_params_ptr.uc_flow_id_num_bit = (unsigned long)inFields[4];
    label_params_ptr.uc_label_tbl_access_mode = (FX950_LABELING_UC_LABEL_TBL_ACCESS_MODE)inFields[5];
    label_params_ptr.mailbox_device = (unsigned long)inFields[6];
    label_params_ptr.mc_label_mode = (FX950_LABELING_MC_LABEL_MODE)inFields[7];
    label_params_ptr.mc_flow_vid_vidx_width = (unsigned long)inFields[8];
    label_params_ptr.mc_prio_width = (unsigned long)inFields[9];

    /* call Ocelot API function */
    result = fx950_api_labeling_set_ingress_labeling_parameters(device_id, &label_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_get_ingress_labeling_parameters
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the configuration of Ingress labeling parameters, 
*  needed to  define the queue selection process for both Unicast and Multicast 
*  packets.
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
*    label_params_ptr -
*       Ingress labeling parameters
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_ingress_labeling_parameters
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_LABELING_INGRESS_PARAMS_STRUCT label_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fx950_api_labeling_get_ingress_labeling_parameters(device_id, &label_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = label_params_ptr.uc_label_mode;
    inFields[1] = label_params_ptr.port_add_enable;
    inFields[2] = label_params_ptr.flow_add_enable;
    inFields[3] = label_params_ptr.uc_priority_num_bit;
    inFields[4] = label_params_ptr.uc_flow_id_num_bit;
    inFields[5] = label_params_ptr.uc_label_tbl_access_mode;
    inFields[6] = label_params_ptr.mailbox_device;
    inFields[7] = label_params_ptr.mc_label_mode;
    inFields[8] = label_params_ptr.mc_flow_vid_vidx_width;
    inFields[9] = label_params_ptr.mc_prio_width;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4],
                                        inFields[5], inFields[6], inFields[7], inFields[8], inFields[9]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_labeling_get_ingress_labeling_parameters_end
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
*  fx950_api_labeling_dx_set_analyzer_target_cfg
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure configures Ingress/Egress Analyzer parameters
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_LABELING_DIRECTION_TYPE   direction -
*       Type of direction: Ingress or Egress
*    FX950_LABELING_ANALYZER_PARAM_STRUCT   analyzer_params_ptr -
*       Analyzer parameters: device, port, tc and dp.
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
*  Those parameters are then used for TO_ANALYZER packets for following 
*  queue assignment.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_dx_set_analyzer_target_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_DIRECTION_TYPE direction;
    FX950_LABELING_ANALYZER_PARAM_STRUCT analyzer_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    direction = (FX950_DIRECTION_TYPE)inArgs[1];

    analyzer_params_ptr.device = (unsigned int)inFields[0];
    analyzer_params_ptr.port = (unsigned long)inFields[1];
    analyzer_params_ptr.tc = (unsigned long)inFields[2];
    analyzer_params_ptr.dp = (unsigned long)inFields[3];

    /* call Ocelot API function */
    result = fx950_api_labeling_dx_set_analyzer_target_cfg(device_id, direction, &analyzer_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_dx_get_analyzer_target_cfg
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the configuration of Ingress/Egress Analyzer.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_LABELING_DIRECTION_TYPE   direction -
*       Type of direction: Ingress or Egress
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
*    analyzer_params_ptr -
*       Analyzer parameters: device, port, tc and dp.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*  Those parameters are used for TO_ANALYZER packets queue assignment.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_dx_get_analyzer_target_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_DIRECTION_TYPE direction;
    FX950_LABELING_ANALYZER_PARAM_STRUCT analyzer_params_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    direction = (FX950_DIRECTION_TYPE)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_labeling_dx_get_analyzer_target_cfg(device_id, direction, &analyzer_params_ptr);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = analyzer_params_ptr.device;
    inFields[1] = analyzer_params_ptr.port;
    inFields[2] = analyzer_params_ptr.tc;
    inFields[3] = analyzer_params_ptr.dp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_labeling_dx_get_analyzer_target_cfg_end
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
*  fx950_api_labeling_set_qos_to_prio_map
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure maps Unicast/Multicast QoS profile to priority.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_PACKETS_TYPE packet_type - 
*       the type of the packets: unicast or multicast
*    unsigned long qos_profile - 
*       qos profile number (0..127)
*    unsigned long priority -
*       The priority to map to qos profile (0..7).
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
static CMD_STATUS wrFx950_api_labeling_set_qos_to_prio_map
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
    unsigned long qos_profile;
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
    packet_type = (FX950_PACKETS_TYPE)inArgs[1];
    qos_profile = (unsigned long)inArgs[2];
    priority = (unsigned long)inArgs[3];

    /* call Ocelot API function */
    result = fx950_api_labeling_set_qos_to_prio_map(device_id, packet_type, qos_profile, priority);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_get_qos_to_prio_map
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of Unicast/Multicast QoS profile to priority.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_PACKETS_TYPE packet_type - 
*       the type of the packets: unicast or multicast
*    unsigned long qos_profile - 
*       qos profile number (0..127)
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
*    priority_ptr -
*       The priority to map to qos profile.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_qos_to_prio_map
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
    unsigned long qos_profile;
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
    packet_type = (FX950_PACKETS_TYPE)inArgs[1];
    qos_profile = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_labeling_get_qos_to_prio_map(device_id, packet_type, qos_profile, &priority_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", priority_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_set_mc_base_queue_number
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure sets for multicast labeling offset.
*  This offset determines the mc base queue number of the packet.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   mc_base_queue_number -
*       multicast base queue number (0..12K).
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
static CMD_STATUS wrFx950_api_labeling_set_mc_base_queue_number
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long mc_base_queue_number;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    mc_base_queue_number = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_labeling_set_mc_base_queue_number(device_id, mc_base_queue_number);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_get_mc_base_queue_number
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets for multicast labeling offset.
*  This offset determines the mc base queue number of the packet.
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
*    mc_base_queue_number_ptr -
*       multicast base queue number.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_mc_base_queue_number
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long mc_base_queue_number_ptr;
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
    result = fx950_api_labeling_get_mc_base_queue_number(device_id, &mc_base_queue_number_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", mc_base_queue_number_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_dx_set_mc_id_assignment
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure sets MC ID assigment for DX mode.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_LABELING_MC_ID_ASSIGN_MODE mc_id_assign_mode -
*       MC-ID assignment mode.
*    unsigned long flood_all_mc_id - 
*       In case that assignment mode is flood all, this is the fabric Multicast
*       ID that is set to a packet (0..2047).
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
*  Only for DX mode.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_dx_set_mc_id_assignment
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_LABELING_MC_ID_ASSIGN_MODE mc_id_assign_mode;
    unsigned long flood_all_mc_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    mc_id_assign_mode = (FX950_LABELING_MC_ID_ASSIGN_MODE)inArgs[1];
    flood_all_mc_id = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_labeling_dx_set_mc_id_assignment(device_id, mc_id_assign_mode, flood_all_mc_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_dx_get_mc_id_assignment
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets MC ID assigment for DX mode.
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
*    mc_id_assign_mode_ptr -
*       MC-ID assignment mode.
*    flood_all_mc_id_ptr - 
*       In case that assignment mode is flood all, this is the fabric Multicast
*       ID that is set to a packet.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*  Only for DX mode
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_dx_get_mc_id_assignment
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FX950_LABELING_MC_ID_ASSIGN_MODE mc_id_assign_mode_ptr;
    unsigned long flood_all_mc_id_ptr;
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
    result = fx950_api_labeling_dx_get_mc_id_assignment(device_id, &mc_id_assign_mode_ptr, &flood_all_mc_id_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", mc_id_assign_mode_ptr, flood_all_mc_id_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_dx_set_control_packets_dp
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure maps DSA Tag to Drop Precedence.
*  This mapping is relevant for "TO_CPU" and "FROM_CPU" packets in the DX mode.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long   to_cpu_dp - 
*       Drop Precedence assigned to TO_CPU packets (0..3).
*    unsigned long   from_cpu_dp - 
*       Drop Precedence assigned to FROM_CPU packets (0..3).
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
*  Only for DX mode.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_dx_set_control_packets_dp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long to_cpu_dp;
    unsigned long from_cpu_dp;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    to_cpu_dp = (unsigned long)inArgs[1];
    from_cpu_dp = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_labeling_dx_set_control_packets_dp(device_id, to_cpu_dp, from_cpu_dp);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_dx_get_control_packets_dp
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of DSA Tag to Drop Precedence.
*  This mapping is relevant for "TO_CPU" and "FROM_CPU" packets in the DX mode.
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
*    to_cpu_dp_ptr - 
*       Drop Precedence assigned to TO_CPU packets.
*    from_cpu_dp_ptr - 
*       Drop Precedence assigned to FROM_CPU packets.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*  Only for DX mode.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_dx_get_control_packets_dp
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long to_cpu_dp_ptr;
    unsigned long from_cpu_dp_ptr;
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
    result = fx950_api_labeling_dx_get_control_packets_dp(device_id, &to_cpu_dp_ptr, &from_cpu_dp_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d%d", to_cpu_dp_ptr, from_cpu_dp_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_dx_set_qos_profile_to_dp_map
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure maps QoS Profile to Drop Precedence.
*  This mapping is relevant for "FORWARD" packets in the DX mode.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    qos_profile - 
*       QoS Profile (0..127)
*    unsigned long dp_num - 
*       Drop Precedence mapped to given QoS Profile (0..3).
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
*  Only for DX mode.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_dx_set_qos_profile_to_dp_map
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
    unsigned long dp_num;
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
    dp_num = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fx950_api_labeling_dx_set_qos_profile_to_dp_map(device_id, qos_profile, dp_num);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_dx_get_qos_profile_to_dp_map
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the mapping of QoS Profile to Drop Precedence.
*  This mapping is relevant for "FORWARD" packets in the DX mode.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    qos_profile - 
*       QoS Profile (0..127)
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
*    dp_num_ptr - 
*       Drop Precedence mapped to given QoS Profile (0..3).
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*  Only for DX mode.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_dx_get_qos_profile_to_dp_map
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
    unsigned long dp_num_ptr;
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

    /* call Ocelot API function */
    result = fx950_api_labeling_dx_get_qos_profile_to_dp_map(device_id, qos_profile, &dp_num_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", dp_num_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_set_coexist_mode
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure enables/disables 98FX950 and 98FX930 coexist mode.
*  If 98FX930 is in the system, coexist mode must be enabled in 98FX950.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int    coexist_enable -
*       When coexist_enable == TRUE  - coexist mode is enabled.
*       When coexist_enable == FALSE - coexist mode is disabled.
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
static CMD_STATUS wrFx950_api_labeling_set_coexist_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int coexist_enable;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    coexist_enable = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fx950_api_labeling_set_coexist_mode(device_id, coexist_enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_labeling_get_coexist_mode
*TYPE:
*  PROC
*DATE:
*  29/AUG/2007
*FUNCTION:
*  This procedure gets the status of 98FX950 and 98FX930 coexist mode 
*  (enabled/disabled).
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_LABELING_COEXIST_MODE coexist_mode -
*       co-exist mode.
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
*    coexist_enable_ptr -
*       When *coexist_enable_ptr == TRUE  - coexist mode is enabled.
*       When *coexist_enable_ptr == FALSE - coexist mode is disabled.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_labeling_get_coexist_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int coexist_enable_ptr;
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
    result = fx950_api_labeling_get_coexist_mode(device_id, &coexist_enable_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", coexist_enable_ptr);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"labeling_set_add_mc_header_enable",
         &wrFx950_api_labeling_set_add_mc_header_enable,
         2, 0},
        {"labeling_get_add_mc_header_enable",
         &wrFx950_api_labeling_get_add_mc_header_enable,
         1, 0},
        {"labeling_cpu_code_label_mapSet",
         &wrFx950_api_labeling_set_cpu_code_label_map,
         1, 2},
        {"labeling_cpu_code_label_mapGetFirst",
         &wrFx950_api_labeling_get_cpu_code_label_map,
         1, 0},
        {"labeling_cpu_code_label_mapGetNext",
         &wrFx950_api_labeling_get_cpu_code_label_map_end,
         1, 0},
        {"labeling_priority_mapSet",
         &wrFx950_api_labeling_set_priority_map,
         2, 8},
        {"labeling_priority_mapGetFirst",
         &wrFx950_api_labeling_get_priority_map,
         2, 0},
        {"labeling_priority_mapGetNext",
         &wrFx950_api_labeling_get_priority_map_end,
         2, 0},
        {"labeling_link_base_queue_numberSet",
         &wrFx950_api_labeling_set_link_base_queue_number,
         1, 2},
        {"labeling_link_base_queue_numberGetFirst",
         &wrFx950_api_labeling_get_link_base_queue_number,
         1, 0},
        {"labeling_link_base_queue_numberGetNext",
         &wrFx950_api_labeling_get_link_base_queue_number_end,
         1, 0},
        {"labeling_uc_label_tbl_entrySet",
         &wrFx950_api_labeling_set_uc_label_tbl_entry,
         2, 4},
        {"labeling_uc_label_tbl_entryGetFirst",
         &wrFx950_api_labeling_get_uc_label_tbl_entry,
         2, 0},
        {"labeling_uc_label_tbl_entryGetNext",
         &wrFx950_api_labeling_get_uc_label_tbl_entry_end,
         2, 0},
        {"labeling_ingress_labeling_parametersSet",
         &wrFx950_api_labeling_set_ingress_labeling_parameters,
         1, 10},
        {"labeling_ingress_labeling_parametersGetFirst",
         &wrFx950_api_labeling_get_ingress_labeling_parameters,
         1, 0},
        {"labeling_ingress_labeling_parametersGetNext",
         &wrFx950_api_labeling_get_ingress_labeling_parameters_end,
         1, 0},
        {"labeling_analyzer_target_cfgSet",
         &wrFx950_api_labeling_dx_set_analyzer_target_cfg,
         2, 4},
        {"labeling_analyzer_target_cfgGetFirst",
         &wrFx950_api_labeling_dx_get_analyzer_target_cfg,
         2, 0},
        {"labeling_analyzer_target_cfgGetNext",
         &wrFx950_api_labeling_dx_get_analyzer_target_cfg_end,
         2, 0},
        {"labeling_set_qos_to_prio_map",
         &wrFx950_api_labeling_set_qos_to_prio_map,
         4, 0},
        {"labeling_get_qos_to_prio_map",
         &wrFx950_api_labeling_get_qos_to_prio_map,
         3, 0},
        {"labeling_set_mc_base_queue_number",
         &wrFx950_api_labeling_set_mc_base_queue_number,
         2, 0},
        {"labeling_get_mc_base_queue_number",
         &wrFx950_api_labeling_get_mc_base_queue_number,
         1, 0},
        {"labeling_dx_set_mc_id_assignment",
         &wrFx950_api_labeling_dx_set_mc_id_assignment,
         3, 0},
        {"labeling_dx_get_mc_id_assignment",
         &wrFx950_api_labeling_dx_get_mc_id_assignment,
         1, 0},
        {"labeling_dx_set_control_packets_dp",
         &wrFx950_api_labeling_dx_set_control_packets_dp,
         3, 0},
        {"labeling_dx_get_control_packets_dp",
         &wrFx950_api_labeling_dx_get_control_packets_dp,
         1, 0},
        {"labeling_dx_set_qos_profile_to_dp_map",
         &wrFx950_api_labeling_dx_set_qos_profile_to_dp_map,
         3, 0},
        {"labeling_dx_get_qos_profile_to_dp_map",
         &wrFx950_api_labeling_dx_get_qos_profile_to_dp_map,
         2, 0},
        {"labeling_set_coexist_mode",
         &wrFx950_api_labeling_set_coexist_mode,
         2, 0},
        {"labeling_get_coexist_mode",
         &wrFx950_api_labeling_get_coexist_mode,
         1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_labeling
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
GT_STATUS cmdLibInitFx950_api_labeling
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

