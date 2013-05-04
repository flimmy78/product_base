
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
*       Wrapper functions for MH Marking APIs of FX950 device.
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
#include <FX950/include/fx950_api_mh_marking.h>


/*****************************************************
*NAME
*  fx950_api_mh_marking_set_cfg
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure defines MH marking parameters.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_MH_MARKING_CFG_STRUCT     marking_cfg_ptr - 
*       Pointer to MH marking Configuration parameters.
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
static CMD_STATUS wrFx950_api_mh_marking_set_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long               result;
    unsigned int                device_id;
    FX950_MH_MARKING_CFG_STRUCT marking_cfg;
    unsigned short              rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    marking_cfg.dp_source_mode = (FX950_MH_MARKING_DP_SOURCE_MODE)inFields[0];
    marking_cfg.mc_dp = (unsigned long)inFields[1];
    marking_cfg.override_hgl_tc_en = (unsigned int)inFields[2];
    marking_cfg.override_mc_tc_dp_en = (unsigned int)inFields[3];
    marking_cfg.override_trap_dp_en = (unsigned int)inFields[4];
    marking_cfg.override_trap_tc_en = (unsigned int)inFields[5];
    marking_cfg.override_uc_tc_dp_en = (unsigned int)inFields[6];
    marking_cfg.uc_dp = (unsigned long)inFields[7];

    /* call Ocelot API function */
    result = fx950_api_mh_marking_set_cfg(device_id, &marking_cfg);

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
*  fx950_api_mh_marking_get_cfg
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets MH marking parameters.
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
*    marking_cfg_ptr - 
*       Pointer to MH marking Configuration parameters.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_mh_marking_get_cfg
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long               result;
    unsigned int                device_id;
    FX950_MH_MARKING_CFG_STRUCT marking_cfg;
    unsigned short              rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];


    /* call Ocelot API function */
    result = fx950_api_mh_marking_get_cfg(device_id, &marking_cfg);
    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = marking_cfg.dp_source_mode;
    inFields[1] = marking_cfg.mc_dp;
    inFields[2] = marking_cfg.override_hgl_tc_en;
    inFields[3] = marking_cfg.override_mc_tc_dp_en;
    inFields[4] = marking_cfg.override_trap_dp_en;
    inFields[5] = marking_cfg.override_trap_tc_en;
    inFields[6] = marking_cfg.override_uc_tc_dp_en;
    inFields[7] = marking_cfg.uc_dp;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");
    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_mh_marking_get_cfg_end
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
*  fx950_api_mh_marking_set_dp_to_dp_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure sets DP to DP mapping configuration.
*  If the DP overriding is enabled, the mapped DP overrides the MH<DP> or DSA<DP> 
*  fields in the outgoing packet to the downstream packet processor.
*  The DP used to index this mapping table is taken from static configuration or 
*  the MH/DSA-tag received from the fabric as defined by DP Source, TO_CPU_DP and 
*  Analyzer_DP.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_PACKETS_TYPE  packet_type - 
*       packets type: unicast or multicast
*    unsigned long       dp_new[FX950_MH_MARKING_NOF_DP_LEVELS] -
*       Array of mapped DP values, the index is DP to be remapped and 
*       the value is new DP value (0..3).
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
static CMD_STATUS wrFx950_api_mh_marking_set_dp_to_dp_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long       result;
    unsigned int        device_id;
    FX950_PACKETS_TYPE  packet_type;
    unsigned long       dp_new[FX950_MH_MARKING_NOF_DP_LEVELS];
    unsigned long       i;
    unsigned short      rc;

    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    packet_type = (FX950_PACKETS_TYPE)inArgs[1];

    for (i = 0; i < FX950_MH_MARKING_NOF_DP_LEVELS; i++) 
    {
        dp_new[i] = (unsigned long)inFields[i];
    }

    /* call Ocelot API function */
    result = fx950_api_mh_marking_set_dp_to_dp_map(device_id, packet_type, dp_new);

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
*  fx950_api_mh_marking_get_dp_to_dp_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets DP to DP mapping configuration.
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
*    dp_new[FX950_MH_MARKING_NOF_DP_LEVELS] -
*       Array of mapped DP values, the index is DP to be remapped and 
*       the value is new DP value.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_mh_marking_get_dp_to_dp_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long       result;
    unsigned int        device_id;
    FX950_PACKETS_TYPE  packet_type;
    unsigned long       dp_new[FX950_MH_MARKING_NOF_DP_LEVELS];
    unsigned long       i;
    unsigned short      rc;

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
    result = fx950_api_mh_marking_get_dp_to_dp_map(device_id, packet_type, dp_new);

    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    for (i = 0; i < FX950_MH_MARKING_NOF_DP_LEVELS; i++) 
    {
        inFields[i] = dp_new[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_mh_marking_get_dp_to_dp_map_end
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
*  fx950_api_mh_marking_set_tc_to_tc_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure sets TC to TC mapping configuration.
*  If the TC overriding is enabled, the mapped TC overrides the MH<TC> or DSA<TC> 
*  fields in the outgoing packet to the downstream packet processor.
*  The TC used to index this mapping table is taken from the MH/DSA-tag received 
*  from the fabric. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_PACKETS_TYPE  packet_type - 
*       packets type: unicast or multicast
*    unsigned long       tc_new[FX950_MH_MARKING_NOF_TC_LEVELS] -
*       Array of mapped TC values, the index is TC to be remapped and 
*       the value is new TC value (0..7).
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
static CMD_STATUS wrFx950_api_mh_marking_set_tc_to_tc_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long       result;
    unsigned int        device_id;
    FX950_PACKETS_TYPE  packet_type;
    unsigned long       tc_new[FX950_MH_MARKING_NOF_TC_LEVELS];
    unsigned long       i;
    unsigned short      rc;

    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    packet_type = (FX950_PACKETS_TYPE)inArgs[1];

    for (i = 0; i < FX950_MH_MARKING_NOF_TC_LEVELS; i++) 
    {
        tc_new[i] = (unsigned long)inFields[i];
    }

    /* call Ocelot API function */
    result = fx950_api_mh_marking_set_tc_to_tc_map(device_id, packet_type, tc_new);

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
*  fx950_api_mh_marking_get_tc_to_tc_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets TC to TC mapping configuration.
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
*    tc_new[FX950_MH_MARKING_NOF_TC_LEVELS] -
*       Array of mapped TC values, the index is TC to be remapped and 
*       the value is new TC value.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_mh_marking_get_tc_to_tc_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long       result;
    unsigned int        device_id;
    FX950_PACKETS_TYPE  packet_type;
    unsigned long       tc_new[FX950_MH_MARKING_NOF_TC_LEVELS];
    unsigned long       i;
    unsigned short      rc;

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
    result = fx950_api_mh_marking_get_tc_to_tc_map(device_id, packet_type, tc_new);

    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    for (i = 0; i < FX950_MH_MARKING_NOF_TC_LEVELS; i++) 
    {
        inFields[i] = tc_new[i];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_mh_marking_get_tc_to_tc_map_end
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
*  fx950_api_mh_marking_set_tc_to_hgl_tc_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure sets TC to HGL cell TC mapping configuration.
*  If the HGL TC overriding is enabled, the mapped TC overrides the MH<TC>  
*  field in the outgoing packet to the downstream packet processor.
*  The TC used to index this mapping table is taken from the MH-tag received 
*  from the fabric. 
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long       hgl_tc_new[FX950_MH_MARKING_NOF_TC_LEVELS] -
*       Array of mapped TC values, the index is TC to be remapped and 
*       the value is new HGL cell TC value (0..3).
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
static CMD_STATUS wrFx950_api_mh_marking_set_tc_to_hgl_tc_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long       result;
    unsigned int        device_id;
    unsigned long       hgl_tc_new[FX950_MH_MARKING_NOF_TC_LEVELS];
    unsigned long       i;
    unsigned short      rc;

    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    for (i = 0; i < FX950_MH_MARKING_NOF_TC_LEVELS; i++) 
    {
        hgl_tc_new[i] = (unsigned long)inFields[i];
    }

    /* call Ocelot API function */
    result = fx950_api_mh_marking_set_tc_to_hgl_tc_map(device_id, hgl_tc_new);

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
*  fx950_api_mh_marking_get_tc_to_hgl_tc_map
*TYPE:
*  PROC
*DATE:
*  30/AUG/2007
*FUNCTION:
*  This procedure gets TC to HGL cell TC mapping configuration.
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
*    hgl_tc_new[FX950_MH_MARKING_NOF_TC_LEVELS] -
*       Array of mapped TC values, the index is TC to be remapped and 
*       the value is new HGL cell TC value.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_mh_marking_get_tc_to_hgl_tc_map
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long       result;
    unsigned int        device_id;
    unsigned long       hgl_tc_new[FX950_MH_MARKING_NOF_TC_LEVELS];
    unsigned long       i;
    unsigned short      rc;

    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    for (i = 0; i < FX950_MH_MARKING_NOF_TC_LEVELS; i++) 
    {
        hgl_tc_new[i] = (unsigned long)inFields[i];
    }

    /* call Ocelot API function */
    result = fx950_api_mh_marking_get_tc_to_hgl_tc_map(device_id, hgl_tc_new);

    rc = sand_get_error_code_from_error_word(result);
    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFx950_api_mh_marking_get_tc_to_hgl_tc_map_end
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


static CMD_COMMAND dbCommands[] =
{
        {"mh_marking_set_cfgSet",
         &wrFx950_api_mh_marking_set_cfg,
         1, 8},

        {"mh_marking_get_cfgGetFirst",
         &wrFx950_api_mh_marking_get_cfg,
         1, 0},

        {"mh_marking_get_cfgGetNext",
         &wrFx950_api_mh_marking_get_cfg_end,
         1, 0},

        {"flow_control_set_dp_to_dp_mapSet",
         &wrFx950_api_mh_marking_set_dp_to_dp_map,
         2, 4},

        {"flow_control_set_dp_to_dp_mapGetFirst",
         &wrFx950_api_mh_marking_get_dp_to_dp_map,
         2, 0},

        {"flow_control_set_dp_to_dp_mapGetNext",
         &wrFx950_api_mh_marking_get_dp_to_dp_map_end,
         2, 0},

        {"flow_control_set_tc_to_tc_mapSet",
         &wrFx950_api_mh_marking_set_tc_to_tc_map,
         2, 8},

        {"flow_control_set_tc_to_tc_mapGetFirst",
         &wrFx950_api_mh_marking_get_tc_to_tc_map,
         2, 8},

        {"flow_control_set_tc_to_tc_mapGetNext",
        &wrFx950_api_mh_marking_get_tc_to_tc_map_end,
        2, 8},

        {"flow_control_set_tc_to_hgl_tc_mapSet",
         &wrFx950_api_mh_marking_set_tc_to_hgl_tc_map,
         2, 8},

        {"flow_control_set_tc_to_hgl_tc_mapGetFirst",
         &wrFx950_api_mh_marking_get_tc_to_hgl_tc_map,
         2, 8},

        {"flow_control_set_tc_to_hgl_tc_mapGetNext",
        &wrFx950_api_mh_marking_get_tc_to_hgl_tc_map_end,
        2, 8}

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
GT_STATUS cmdLibInitFx950_api_mh_marking
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

