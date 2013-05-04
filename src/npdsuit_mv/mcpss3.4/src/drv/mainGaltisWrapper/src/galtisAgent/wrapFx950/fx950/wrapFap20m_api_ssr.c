/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_ssr.c
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
#include <FX950/include/fap20m_api_ssr.h>
#include <FX950/include/fap20m_ssr.h>


/* Global variables */
static GT_U32 tbl_index;
static FAP20M_SSR_BUFF *ssr_buff;
static FAP20M_SSR_BUFF *ssr_buff_to_read;
static GT_U32 table_flag_mode;


/*****************************************************
*NAME
* fap20m_ssr_get_cfg_version
*TYPE:
*  PROC
*DATE: 
*  22/02/2006
*FUNCTION:
*  determine fap20 SSR version number based on sand version number
*INPUT:
*  DIRECT:
*    unsigned long sand_ver - sand version
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long - sand error word
*  INDIRECT:
*    FAP20M_SW_DB_CFG_VERSION_NUM - SSR version number.
*     if not valid SSR bersion - FAP20M_SW_DB_CFG_VERSION_INVALID is returned.
*REMARKS:
*    None
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_ssr_get_cfg_version
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned long sand_ver;
    FAP20M_SW_DB_CFG_VERSION_NUM sw_db_version;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    sand_ver = (unsigned long)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_ssr_get_cfg_version(sand_ver, &sw_db_version);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", sw_db_version);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_ssr_get_buff_size
*TYPE:
*  PROC
*DATE: 
*  21/02/2006
*FUNCTION:
*  get SSR buffer size
*INPUT:
*  DIRECT:
*    None.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long - sand error word
*  INDIRECT:
*    OUT unsigned long* byte_size_ptr - SSR buffer size
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_ssr_get_buff_size
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned long byte_size_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_ssr_get_buff_size(&byte_size_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", byte_size_ptr);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_max_sw_db_size
*TYPE:
*  PROC
*DATE: 
*  01/03/2006
*FUNCTION:
*  calculates the size of the largest software database of all SW_DB versions
*INPUT:
*  DIRECT:
*    unsigned int* size - pointer to the size (indirect return value)
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long - sand error word
*  INDIRECT:
*    unsigned int* size - the size of the largest software database of 
*                         all the SW_DB versions
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_max_sw_db_size
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int size;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_get_max_sw_db_size(&size);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", size);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_ssr_from_buff
*TYPE:
*  PROC
*DATE: 
*  21/02/2006
*FUNCTION:
*  load SSR data from buffer
*INPUT:
*  DIRECT:
*    SAND_OUT unsigned char* store_buff - SSR buffer
*    SAND_IN  unsigned long  buff_byte_size - SSR buffer size
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long - sand error word
*  INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_ssr_from_buff
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned long buff_byte_size;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    buff_byte_size = (unsigned long)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_ssr_from_buff((unsigned char*) ssr_buff, buff_byte_size);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_auto_aggregate_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_dev_index;
    unsigned int end_dev_index;
    unsigned int start_port_index;
    unsigned int end_port_index;
    unsigned int start_hierarcy_index;
    unsigned int end_hierarcy_index;
    unsigned int start_index;
    unsigned int end_index;
    unsigned int i, j, k, l;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    start_dev_index = (unsigned int)inArgs[0];
    end_dev_index = (unsigned int)inArgs[1];
    start_port_index = (unsigned int)inArgs[2];
    end_port_index = (unsigned int)inArgs[3];
    start_hierarcy_index = (unsigned int)inArgs[4];
    end_hierarcy_index = (unsigned int)inArgs[5];
    start_index = (unsigned int)inArgs[6];
    end_index = (unsigned int)inArgs[7];

    /* fill DB */
    for(i = start_dev_index; i <= end_dev_index; i++)
        for(j = start_port_index; j <= end_port_index; j++)
            for(k = start_hierarcy_index; k <= end_hierarcy_index; k++)
                for(l = start_index; l <= end_index; l++)
                {
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].sch.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].sch.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].sch.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];
                    
                    switch (ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].sch.type)
                    {
                        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
                            ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].sch.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
                            break;
                        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
                            ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].sch.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[4];
                            break;
                        default: break;
                    }

                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.id = (FAP20M_FLOW_ID)inFields[5];

                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].valid_mask = (unsigned long)inFields[6];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].id = (FAP20M_SUB_FLOW_ID)inFields[7];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[8];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].shaper.max_rate = (unsigned long)inFields[9];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].shaper.max_burst = (unsigned long)inFields[10];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE)inFields[11];

                    switch (ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].type)
                    {
                        case FAP20M_SUB_FLOW_TYPE_HR:
                            ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[12];
                            ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.hr.weight = (unsigned long)inFields[14];
                            break;
                        case FAP20M_SUB_FLOW_TYPE_CL:
                            ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[13];
                            ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[0].subflow_properties.cl.weight = (unsigned long)inFields[14];
                            break;
                        default: break;
                    }

                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].valid_mask = (unsigned long)inFields[15];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].id = (FAP20M_SUB_FLOW_ID)inFields[16];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[17];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].shaper.max_rate = (unsigned long)inFields[18];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].shaper.max_burst = (unsigned long)inFields[19];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE)inFields[20];

                    switch (ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].type)
                    {
                        case FAP20M_SUB_FLOW_TYPE_HR:
                            ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[21];
                            ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.hr.weight = (unsigned long)inFields[23];
                            break;
                        case FAP20M_SUB_FLOW_TYPE_CL:
                            ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[22];
                            ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.sub_flow[1].subflow_properties.cl.weight = (unsigned long)inFields[23];
                            break;
                        default: break;
                    }

                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.flow_type = (FAP20M_FLOW_TYPE)inFields[24];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].flow.is_slow_enabled = (unsigned int)inFields[25];
                    ssr_buff->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[i][j].item[k][l].valid = (unsigned int) inFields[26];
                }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_auto_flow_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int dev_start_index;
    unsigned int dev_end_index;
    unsigned int port_start_index;
    unsigned int port_end_index;
    unsigned int flow_start_index;
    unsigned int flow_end_index;
    unsigned int i, j, k;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    dev_start_index = (unsigned int)inArgs[0];
    dev_end_index = (unsigned int)inArgs[1];
    port_start_index = (unsigned int)inArgs[2];
    port_end_index = (unsigned int)inArgs[3];
    flow_start_index = (unsigned int)inArgs[4];
    flow_end_index = (unsigned int)inArgs[5];
    
    /* fill DB */
    for(i = dev_start_index; i <= dev_end_index; i++)
        for(j = port_start_index; j <= port_end_index; j++)
            for(k = flow_start_index; k <= flow_end_index; k++)
            {
                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].id = inFields[0];

                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].valid_mask = inFields[1];
                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].id = inFields[2];
                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].credit_source = inFields[3];
                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].shaper.max_rate = inFields[4];
                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].shaper.max_burst = inFields[5];

                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].type = inFields[6];

                switch (ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].type)
                {
                    case FAP20M_SUB_FLOW_TYPE_HR:
                        ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.hr.sp_class = inFields[7];
                        ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.hr.weight = inFields[9];
                        break;
                    case FAP20M_SUB_FLOW_TYPE_CL:
                        ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.cl.class_val = inFields[8];
                        ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[0].subflow_properties.cl.weight = inFields[9];
                        break;
                    default: break;
                }

                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].valid_mask = inFields[10];
                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].id = inFields[11];
                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].credit_source = inFields[12];
                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].shaper.max_rate = inFields[13];
                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].shaper.max_burst = inFields[14];

                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].type = inFields[15];

                switch (ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].type)
                {
                    case FAP20M_SUB_FLOW_TYPE_HR:
                        ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.hr.sp_class = inFields[16];
                        ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.hr.weight = inFields[18];
                        break;
                    case FAP20M_SUB_FLOW_TYPE_CL:
                        ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.cl.class_val = inFields[17];
                        ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].sub_flow[1].subflow_properties.cl.weight = inFields[18];
                        break;
                    default: break;
                }

                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].flow_type = inFields[19];
                ssr_buff->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[i][j].flows[k].is_slow_enabled = inFields[20];
            }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_auto_mngr_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_is_valid = (unsigned int)inFields[0];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.max_nof_faps_in_system = (unsigned int)inFields[1];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.max_nof_ports_per_fap = (unsigned int)inFields[2];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.is_fap10v = (unsigned int)inFields[3];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.max_nof_sub_flows_per_flow = (unsigned int)inFields[4];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.nof_fabric_classes = (FAP20M_FABRIC_CLASS)inFields[5];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.auto_first_queue_type = (FAP20M_QUEUE_TYPE)inFields[6];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.auto_last_queue_type = (FAP20M_QUEUE_TYPE)inFields[7];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.nof_faps_per_CPU = (unsigned int)inFields[8];
    
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[0].local_fap_id = (unsigned int)inFields[9];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[1].local_fap_id = (unsigned int)inFields[10];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[2].local_fap_id = (unsigned int)inFields[11];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[3].local_fap_id = (unsigned int)inFields[12];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[4].local_fap_id = (unsigned int)inFields[13];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[5].local_fap_id = (unsigned int)inFields[14];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[6].local_fap_id = (unsigned int)inFields[15];
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[7].local_fap_id = (unsigned int)inFields[16];
    
    ssr_buff->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.fabric_sch_rank = (FAP20M_SCH_RANK)inFields[17];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_auto_port_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_dev_index;
    unsigned int end_dev_index;
    unsigned int start_port_index;
    unsigned int end_port_index;
    unsigned int i, j;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    start_dev_index = (unsigned int)inArgs[0];
    end_dev_index = (unsigned int)inArgs[1];
    start_port_index = (unsigned int)inArgs[2];
    end_port_index = (unsigned int)inArgs[3];
    
    /* fill DB */
    for(i = start_dev_index; i <= end_dev_index; i++)
        for(j = start_port_index; j <= end_port_index; j++)
        {
            ssr_buff->data.fap20m_auto_port_sw_db_buff.fap20m_auto_port_info[i][j].eg_port.valid_mask = (unsigned long)inFields[1];
            ssr_buff->data.fap20m_auto_port_sw_db_buff.fap20m_auto_port_info[i][j].eg_port.nominal_bandwidth = (unsigned long)inFields[2];
            ssr_buff->data.fap20m_auto_port_sw_db_buff.fap20m_auto_port_info[i][j].eg_port.credit_bandwidth = (unsigned long)inFields[3];
            ssr_buff->data.fap20m_auto_port_sw_db_buff.fap20m_auto_port_info[i][j].hr_mode = (FAP20M_HR_SCHED_MODE)inFields[4];
        }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_auto_queue_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_fap_index;
    unsigned int end_fap_index;
    unsigned int start_port_index;
    unsigned int end_port_index;
    unsigned int start_queue_index;
    unsigned int end_queue_index;
    unsigned int i, j;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    start_fap_index = (unsigned int) inArgs[0];
    end_fap_index = (unsigned int) inArgs[1];
    start_port_index = (unsigned int) inArgs[2];
    end_port_index = (unsigned int) inArgs[3];
    start_queue_index = (unsigned int) inArgs[4];
    end_queue_index = (unsigned int) inArgs[5];

    /* fill queue DB */
    for(i = start_fap_index; i <= end_fap_index; i++)
        for(j = start_port_index; j <= end_port_index; j++)
        {
            ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[i][j].queue_type[0] = (FAP20M_QUEUE_TYPE)inFields[0];
            ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[i][j].queue_type[1] = (FAP20M_QUEUE_TYPE)inFields[1];
            ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[i][j].queue_type[2] = (FAP20M_QUEUE_TYPE)inFields[2];
            ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[i][j].queue_type[3] = (FAP20M_QUEUE_TYPE)inFields[3];
            ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[i][j].queue_type[4] = (FAP20M_QUEUE_TYPE)inFields[4];
            ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[i][j].queue_type[5] = (FAP20M_QUEUE_TYPE)inFields[5];
            ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[i][j].queue_type[6] = (FAP20M_QUEUE_TYPE)inFields[6];
            ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[i][j].queue_type[7] = (FAP20M_QUEUE_TYPE)inFields[7];
        }

    for(i = start_queue_index; i <= end_queue_index; i++)
    {
        ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.vacant[i] = (unsigned int)inFields[8];

        ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[i].RED_enable = (unsigned int)inFields[9];
        ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[i].RED_exp_weight = (unsigned int)inFields[10];
        ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[i].flow_slow_th_up = (unsigned long)inFields[11];
        ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[i].flow_stat_msg = (unsigned long)inFields[12];
        ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[i].max_q_size = (unsigned long)inFields[13];
        ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[i].flow_slow_hys_en = (unsigned int) inFields[14];
        ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[i].hysteresis_size = (unsigned long) inFields[15];
        ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[i].header_compensation = (int) inFields[16];
    }

    ssr_buff->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db_is_valid = (unsigned int) inFields[17];
        
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_cell_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    for(i = 0; i < SAND_MAX_DEVICE; i++)
    {
        ssr_buff->data.fap20m_cell_sw_db_buff.fap20m_last_cell_read_side[i] = (unsigned int)inFields[i];
    }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_egress_ports_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_dev_index;
    unsigned int end_dev_index;
    unsigned int start_port_index;
    unsigned int end_port_index;
    unsigned int i, j;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    start_dev_index = (unsigned int)inArgs[0];
    end_dev_index = (unsigned int)inArgs[1];
    start_port_index = (unsigned int)inArgs[2];
    end_port_index = (unsigned int)inArgs[3];

    for(i = start_dev_index; i <= end_dev_index; i++)
        ssr_buff->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table_valid[i][0] = (unsigned int)inFields[0];
        ssr_buff->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table_valid[i][1] = (unsigned int)inFields[1];

        for(j = start_port_index; j <= end_port_index; j++)
        {
            ssr_buff->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[i][0].ports[j].valid_mask = (unsigned long)inFields[2];
            ssr_buff->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[i][0].ports[j].nominal_bandwidth = (unsigned long)inFields[3];
            ssr_buff->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[i][0].ports[j].credit_bandwidth = (unsigned long)inFields[4];

            ssr_buff->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[i][1].ports[j].valid_mask = (unsigned long)inFields[5];
            ssr_buff->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[i][1].ports[j].nominal_bandwidth = (unsigned long)inFields[6];
            ssr_buff->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[i][1].ports[j].credit_bandwidth = (unsigned long)inFields[7];

            ssr_buff->data.fap20m_api_egress_ports_sw_db_buff.fap20m_port_nominal_rate[i][0][j] = (unsigned long)inFields[8];
            ssr_buff->data.fap20m_api_egress_ports_sw_db_buff.fap20m_port_nominal_rate[i][0][j] = (unsigned long)inFields[9];
        }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_egress_ports_fill_sw_db_v02
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_dev_index;
    unsigned int end_dev_index;
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    start_dev_index = (unsigned int)inArgs[0];
    end_dev_index = (unsigned int)inArgs[1];

    /* map input arguments to locals */
    for (i = start_dev_index; i <= end_dev_index; i++)
    {
        ssr_buff->data.fap20m_egress_ports_sw_db_buff.fap20m_link_a_total_ports_credit_rate[i][0] = (unsigned long)inFields[0];
        ssr_buff->data.fap20m_egress_ports_sw_db_buff.fap20m_link_a_total_ports_credit_rate[i][1] = (unsigned long)inFields[1];
        ssr_buff->data.fap20m_egress_ports_sw_db_buff.fap20m_link_b_total_ports_credit_rate[i][0] = (unsigned long)inFields[2];
        ssr_buff->data.fap20m_egress_ports_sw_db_buff.fap20m_link_b_total_ports_credit_rate[i][1] = (unsigned long)inFields[3];
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_interrupt_services_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_dev_index;
    unsigned int end_dev_index;
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    start_dev_index = (unsigned int)inArgs[0];
    end_dev_index = (unsigned int)inArgs[1];

    /* map input arguments to locals */
    for(i = start_dev_index; i <= end_dev_index; i++)
    {
        ssr_buff->data.fap20m_interrupt_services_sw_db_buff.fap20m_interrupt_mask_on[i] = (unsigned char)inFields[0];
        ssr_buff->data.fap20m_interrupt_services_sw_db_buff.fap20m_interrupt_mask_copy[i] = (unsigned long)inFields[1];
        ssr_buff->data.fap20m_interrupt_services_sw_db_buff.fap20m_interrupt_non_autoclear[i] = (unsigned long)inFields[2];
    }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_routing_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    ssr_buff->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[0] = (unsigned int)inFields[0];
    ssr_buff->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[1] = (unsigned int)inFields[1];
    ssr_buff->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[2] = (unsigned int)inFields[2];
    ssr_buff->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[3] = (unsigned int)inFields[3];
    ssr_buff->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[4] = (unsigned int)inFields[4];
    ssr_buff->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[5] = (unsigned int)inFields[5];
    ssr_buff->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[6] = (unsigned int)inFields[6];
    ssr_buff->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[7] = (unsigned int)inFields[7];

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_ssr_spatial_multicast_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_dev_index;
    unsigned int end_dev_index;
    unsigned int start_mc_index;
    unsigned int end_mc_index;
    unsigned int i, j;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff == NULL) 
    {
        /* allocate space */
        ssr_buff = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    start_dev_index = (unsigned int)inArgs[0];
    end_dev_index = (unsigned int)inArgs[1];
    start_mc_index = (unsigned int)inArgs[2];
    end_mc_index = (unsigned int)inArgs[3];

    for(i = start_dev_index; i <= end_dev_index; i++)
        for(j = start_mc_index; j <= end_mc_index; j++)
        {
            ssr_buff->data.fap20m_spatial_multicast_sw_db_buff.fap20m_spatial_multicast_ids[i].taken_spatial_multicast_id[j] = (unsigned long)inFields[0];
        }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************/
static void prvFap20mSsrGet_SPATIAL_MC_DB_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = ssr_buff_to_read->data.fap20m_spatial_multicast_sw_db_buff.fap20m_spatial_multicast_ids[0].taken_spatial_multicast_id[tbl_index];
    inFields[1] = ssr_buff_to_read->data.fap20m_spatial_multicast_sw_db_buff.fap20m_spatial_multicast_ids[1].taken_spatial_multicast_id[tbl_index];
    inFields[2] = ssr_buff_to_read->data.fap20m_spatial_multicast_sw_db_buff.fap20m_spatial_multicast_ids[2].taken_spatial_multicast_id[tbl_index];
    inFields[3] = ssr_buff_to_read->data.fap20m_spatial_multicast_sw_db_buff.fap20m_spatial_multicast_ids[3].taken_spatial_multicast_id[tbl_index];
    inFields[4] = ssr_buff_to_read->data.fap20m_spatial_multicast_sw_db_buff.fap20m_spatial_multicast_ids[4].taken_spatial_multicast_id[tbl_index];
    inFields[5] = ssr_buff_to_read->data.fap20m_spatial_multicast_sw_db_buff.fap20m_spatial_multicast_ids[5].taken_spatial_multicast_id[tbl_index];
    inFields[6] = ssr_buff_to_read->data.fap20m_spatial_multicast_sw_db_buff.fap20m_spatial_multicast_ids[6].taken_spatial_multicast_id[tbl_index];
    inFields[7] = ssr_buff_to_read->data.fap20m_spatial_multicast_sw_db_buff.fap20m_spatial_multicast_ids[7].taken_spatial_multicast_id[tbl_index];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 0);
}

/*****************************************************/
static void prvFap20mSsrGet_EGRESS_PORTS_DB_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int port_index;


    /* map input arguments to locals */
    port_index = (unsigned int)inArgs[2];

    inFields[0] = ssr_buff_to_read->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table_valid[tbl_index][0];
    inFields[1] = ssr_buff_to_read->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table_valid[tbl_index][1];

    inFields[2] = ssr_buff_to_read->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[tbl_index][0].ports[port_index].valid_mask;
    inFields[3] = ssr_buff_to_read->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[tbl_index][0].ports[port_index].nominal_bandwidth;
    inFields[4] = ssr_buff_to_read->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[tbl_index][0].ports[port_index].credit_bandwidth;
    inFields[5] = ssr_buff_to_read->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[tbl_index][1].ports[port_index].valid_mask;
    inFields[6] = ssr_buff_to_read->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[tbl_index][1].ports[port_index].nominal_bandwidth;
    inFields[7] = ssr_buff_to_read->data.fap20m_api_egress_ports_sw_db_buff.fap20m_egress_port_qos_table[tbl_index][1].ports[port_index].credit_bandwidth;
    
    inFields[8] = ssr_buff_to_read->data.fap20m_api_egress_ports_sw_db_buff.fap20m_port_nominal_rate[tbl_index][0][port_index];
    inFields[9] = ssr_buff_to_read->data.fap20m_api_egress_ports_sw_db_buff.fap20m_port_nominal_rate[tbl_index][1][port_index];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3], inFields[4],
                                        inFields[5], inFields[6], inFields[7], inFields[8], inFields[9]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 1);
}

/*****************************************************/
static void prvFap20mSsrGet_AUTO_AGGREGATE_DB_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int dev_index;
    unsigned int port_index;
    unsigned int i, j;


    /* map input arguments to locals */
    dev_index = (unsigned int)inArgs[1];
    port_index = (unsigned int)inArgs[2];

    /* calculate table's indexes */
    j = tbl_index % FAP20M_AUTO_NOF_INDEX;
    i = tbl_index / FAP20M_AUTO_NOF_INDEX;

    inFields[0] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].sch.id;
    inFields[1] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].sch.type;
    inFields[2] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].sch.state;

    switch (ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].sch.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].sch.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].sch.sched_properties.cl.class_type;
            break;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }
                    
    inFields[5] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.id;
    
    inFields[6] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].valid_mask;
    inFields[7] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].id;
    inFields[8] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].credit_source;
    inFields[9] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].shaper.max_rate;
    inFields[10] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].shaper.max_burst;

    inFields[11] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].type;

    switch (ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }

    inFields[15] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].valid_mask;
    inFields[16] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].id;
    inFields[17] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].credit_source;
    inFields[18] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].shaper.max_rate;
    inFields[19] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].shaper.max_burst;

    inFields[20] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].type;

    switch (ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.flow_type;
    inFields[25] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].flow.is_slow_enabled;
    inFields[26] = ssr_buff_to_read->data.fap20m_auto_aggregate_sw_db_buff.fap20m_auto_agg_info[dev_index][port_index].item[i][j].valid;
               
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d", 
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                                inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                                inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                                inFields[15], inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23], inFields[24],
                                inFields[25], inFields[26]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 2);
}

/*****************************************************/
static void prvFap20mSsrGet_AUTO_FLOW_DB_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int dev_index;
    unsigned int port_index;


    /* map input arguments to locals */
    dev_index = (unsigned int)inArgs[1];
    port_index = (unsigned int)inArgs[2];

    inFields[0] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[0].id;
    inFields[1] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].valid_mask;
    inFields[2] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].id;
    inFields[3] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].credit_source;
    inFields[4] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].shaper.max_rate;
    inFields[5] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].shaper.max_burst;
    inFields[6] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].type;

    switch (ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[7] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].subflow_properties.hr.sp_class;
            inFields[9] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[8] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].subflow_properties.cl.class_val;
            inFields[9] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[7] = -1;
            inFields[8] = -1;
            inFields[9] = -1;
    }
                               
    inFields[10] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[1].valid_mask;
    inFields[11] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[1].id;
    inFields[12] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[1].credit_source;
    inFields[13] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[1].shaper.max_rate;
    inFields[14] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[1].shaper.max_burst;
    inFields[15] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[1].type;

    switch (ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[16] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[1].subflow_properties.hr.sp_class;
            inFields[18] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[17] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[1].subflow_properties.cl.class_val;
            inFields[18] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[16] = -1;
            inFields[17] = -1;
            inFields[18] = -1;
    }

    inFields[19] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].flow_type;
    inFields[20] = ssr_buff_to_read->data.fap20m_auto_flow_sw_db_buff.fap20m_auto_flow_info[dev_index][port_index].flows[tbl_index].is_slow_enabled;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],
                                inFields[4],  inFields[5],  inFields[6],  inFields[7],
                                inFields[8],  inFields[9],  inFields[10], inFields[11],
                                inFields[12], inFields[13], inFields[14], inFields[15],
                                inFields[16], inFields[17], inFields[18], inFields[19], inFields[20]);
    
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 3);
}

/*****************************************************/
static void prvFap20mSsrGet_AUTO_MNGR_DB_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_is_valid;
    inFields[1] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.max_nof_faps_in_system;
    inFields[2] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.max_nof_ports_per_fap;
    inFields[3] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.is_fap10v;
    inFields[4] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.max_nof_sub_flows_per_flow;
    inFields[5] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.nof_fabric_classes;
    inFields[6] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.auto_first_queue_type;
    inFields[7] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.auto_last_queue_type;
    inFields[8] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.nof_faps_per_CPU;

    inFields[9] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[0].local_fap_id;
    inFields[10] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[1].local_fap_id;
    inFields[11] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[2].local_fap_id;
    inFields[12] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[3].local_fap_id;
    inFields[13] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[4].local_fap_id;
    inFields[14] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[5].local_fap_id;
    inFields[15] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[6].local_fap_id;
    inFields[16] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.device_info[7].local_fap_id;

    inFields[17] = ssr_buff_to_read->data.fap20m_auto_mngr_sw_db_buff.fap20m_auto_info.fabric_sch_rank;
    
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                            inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],  inFields[5],
                            inFields[6],  inFields[7],  inFields[8],  inFields[9],  inFields[10], inFields[11],
                            inFields[12], inFields[13], inFields[14], inFields[15], inFields[16], inFields[17]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 4);
}

/*****************************************************/
static void prvFap20mSsrGet_AUTO_PORT_DB_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int dev_index;
    unsigned int port_index;


    /* map input arguments to locals */
    dev_index = (unsigned int)inArgs[1];
    port_index = (unsigned int)inArgs[2];

    inFields[0] = ssr_buff_to_read->data.fap20m_auto_port_sw_db_buff.fap20m_auto_port_info[0][0].eg_port.valid_mask;
    inFields[1] = ssr_buff_to_read->data.fap20m_auto_port_sw_db_buff.fap20m_auto_port_info[dev_index][port_index].eg_port.nominal_bandwidth;
    inFields[2] = ssr_buff_to_read->data.fap20m_auto_port_sw_db_buff.fap20m_auto_port_info[dev_index][port_index].eg_port.credit_bandwidth;
    inFields[3] = ssr_buff_to_read->data.fap20m_auto_port_sw_db_buff.fap20m_auto_port_info[dev_index][port_index].hr_mode;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1],  inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 5);
}

/*****************************************************/
static void prvFap20mSsrGet_AUTO_QUEUE_DB_E
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int fap_index;
    unsigned int port_index;


    /* map input arguments to locals */
    fap_index = (unsigned int)inArgs[1];
    port_index = (unsigned int)inArgs[2];

    inFields[0] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[fap_index][port_index].queue_type[0];
    inFields[1] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[fap_index][port_index].queue_type[1];
    inFields[2] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[fap_index][port_index].queue_type[2];
    inFields[3] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[fap_index][port_index].queue_type[3];
    inFields[4] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[fap_index][port_index].queue_type[4];
    inFields[5] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[fap_index][port_index].queue_type[5];
    inFields[6] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[fap_index][port_index].queue_type[6];
    inFields[7] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_info[fap_index][port_index].queue_type[7];

    inFields[8] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.vacant[tbl_index];
    inFields[9] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[tbl_index].RED_enable;
    inFields[10] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[tbl_index].RED_exp_weight;
    inFields[11] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[tbl_index].flow_slow_th_up;
    inFields[12] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[tbl_index].flow_stat_msg;
    inFields[13] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[tbl_index].max_q_size;
    inFields[14] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[tbl_index].flow_slow_hys_en;
    inFields[15] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[tbl_index].hysteresis_size;
    inFields[16] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db.queue_type_params[tbl_index].header_compensation;

    inFields[17] = ssr_buff_to_read->data.fap20m_auto_queue_sw_db_buff.fap20m_auto_queue_type_param_db_is_valid;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 6);
}

/*****************************************************/
static void prvFap20mSsrGet_EGRESS_PORTS_DB2_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = ssr_buff_to_read->data.fap20m_egress_ports_sw_db_buff.fap20m_link_a_total_ports_credit_rate[tbl_index][0];
    inFields[1] = ssr_buff_to_read->data.fap20m_egress_ports_sw_db_buff.fap20m_link_a_total_ports_credit_rate[tbl_index][1];
    inFields[2] = ssr_buff_to_read->data.fap20m_egress_ports_sw_db_buff.fap20m_link_b_total_ports_credit_rate[tbl_index][0];
    inFields[3] = ssr_buff_to_read->data.fap20m_egress_ports_sw_db_buff.fap20m_link_b_total_ports_credit_rate[tbl_index][1];
    
    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 7);
}

/*****************************************************/
static void prvFap20mSsrGet_CELL_DB_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = ssr_buff_to_read->data.fap20m_cell_sw_db_buff.fap20m_last_cell_read_side[0];
    inFields[1] = ssr_buff_to_read->data.fap20m_cell_sw_db_buff.fap20m_last_cell_read_side[1];
    inFields[2] = ssr_buff_to_read->data.fap20m_cell_sw_db_buff.fap20m_last_cell_read_side[2];
    inFields[3] = ssr_buff_to_read->data.fap20m_cell_sw_db_buff.fap20m_last_cell_read_side[3];
    inFields[4] = ssr_buff_to_read->data.fap20m_cell_sw_db_buff.fap20m_last_cell_read_side[4];
    inFields[5] = ssr_buff_to_read->data.fap20m_cell_sw_db_buff.fap20m_last_cell_read_side[5];
    inFields[6] = ssr_buff_to_read->data.fap20m_cell_sw_db_buff.fap20m_last_cell_read_side[6];
    inFields[7] = ssr_buff_to_read->data.fap20m_cell_sw_db_buff.fap20m_last_cell_read_side[7];
    
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 8);
}

/*****************************************************/
static void prvFap20mSsrGet_INTERRUPT_DB_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = ssr_buff_to_read->data.fap20m_interrupt_services_sw_db_buff.fap20m_interrupt_mask_on[tbl_index];
    inFields[1] = ssr_buff_to_read->data.fap20m_interrupt_services_sw_db_buff.fap20m_interrupt_mask_copy[tbl_index];
    inFields[2] = ssr_buff_to_read->data.fap20m_interrupt_services_sw_db_buff.fap20m_interrupt_non_autoclear[tbl_index];

    
    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 9);
}

/*****************************************************/
static void prvFap20mSsrGet_ROUTING_DB_E
(
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = ssr_buff_to_read->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[0];
    inFields[1] = ssr_buff_to_read->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[1];
    inFields[2] = ssr_buff_to_read->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[2];
    inFields[3] = ssr_buff_to_read->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[3];
    inFields[4] = ssr_buff_to_read->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[4];
    inFields[5] = ssr_buff_to_read->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[5];
    inFields[6] = ssr_buff_to_read->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[6];
    inFields[7] = ssr_buff_to_read->data.fap20m_routing_sw_db_buff.fap20m_rtp_link_activ_mask_changed[7];

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 10);
}

/*****************************************************
*NAME
*   fap20m_ssr_to_buff
*TYPE:
*  PROC
*DATE: 
*  21/02/2006
*FUNCTION:
*  load SSR data to buffer
*INPUT:
*  DIRECT:
*    OUT unsigned char* store_buff - SSR buffer
*    IN  unsigned long  buff_byte_size - SSR buffer size
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long - sand error word
*  INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_ssr_to_buff
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned long buff_byte_size;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff_to_read == NULL) 
    {
        /* allocate space */
        ssr_buff_to_read = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff_to_read==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    /* map input arguments to locals */
    buff_byte_size = (unsigned long)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_ssr_to_buff((unsigned char*) ssr_buff_to_read, buff_byte_size);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;
    table_flag_mode = 0;

    /* pack output arguments to galtis string */
    prvFap20mSsrGet_SPATIAL_MC_DB_E(inFields, outArgs);

    tbl_index = 1;

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_ssr_to_buff_end
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (ssr_buff_to_read == NULL) 
    {
        /* allocate space */
        ssr_buff_to_read = (FAP20M_SSR_BUFF *)cmdOsMalloc(sizeof(FAP20M_SSR_BUFF));
        if (ssr_buff_to_read==NULL)
        {
            galtisOutput(outArgs, GT_NO_RESOURCE, "");
            return CMD_OK;
        }
    }

    switch (table_flag_mode)
    {
        /* case SPATIAL_MULTICAST_TBL: */
        case 0:
            /* pack output arguments to galtis string */
            prvFap20mSsrGet_SPATIAL_MC_DB_E(inFields, outArgs);

            if (++tbl_index >= FAP20M_MULTICAST_GROUPS_FROM_PP_LONGS)
            {
                tbl_index = 0;
                table_flag_mode++;
            }
            break;

        /* case EGRESS_PORTS_TBL: */
        case 1:
            /* pack output arguments to galtis string */     
            prvFap20mSsrGet_EGRESS_PORTS_DB_E(inArgs, inFields, outArgs);

            if (++tbl_index >= SAND_MAX_DEVICE)
            {
                tbl_index = 0;
                table_flag_mode++;
            }
            break;

        /* case AUTO_AGGREGATE_TBL: */
        case 2:
            /* pack output arguments to galtis string */     
            prvFap20mSsrGet_AUTO_AGGREGATE_DB_E(inArgs, inFields, outArgs);

            if (++tbl_index >= FAP20M_AUTO_NOF_HIERARCY * FAP20M_AUTO_NOF_INDEX)
            {
                tbl_index = 0;
                table_flag_mode++;
            }
            break;

        /* case AUTO_FLOW_TBL: */
        case 3:
            /* pack output arguments to galtis string */     
            prvFap20mSsrGet_AUTO_FLOW_DB_E(inArgs, inFields, outArgs);

            if (++tbl_index >= FAP20M_FABRIC_UNICAST_CLASSES)
            {
                tbl_index = 0;
                table_flag_mode++;
            }
            break;

        /* case AUTO_MNGR_TBL:*/
        case 4:
            /* pack output arguments to galtis string */     
            prvFap20mSsrGet_AUTO_MNGR_DB_E(inFields, outArgs);

            tbl_index = 0;
            table_flag_mode++;

            break;

        /* case AUTO_PORT_TBL: */
        case 5:
            /* pack output arguments to galtis string */     
            prvFap20mSsrGet_AUTO_PORT_DB_E(inArgs, inFields, outArgs);

            tbl_index = 0;
            table_flag_mode++;

            break;

        /* case AUTO_QUEUE_TBL:*/
        case 6:
            /* pack output arguments to galtis string */     
            prvFap20mSsrGet_AUTO_QUEUE_DB_E(inArgs, inFields, outArgs);

            if (++tbl_index >= FAP20M_NOF_QUEUE_TYPES)
            {
                tbl_index = 0;
                table_flag_mode++;
            }
            break;

        /* case EGRESS_PORTS2_TBL:*/
        case 7:
            /* pack output arguments to galtis string */     
            prvFap20mSsrGet_EGRESS_PORTS_DB2_E(inFields, outArgs);

            if (++tbl_index >= SAND_MAX_DEVICE)
            {
                tbl_index = 0;
                table_flag_mode++;
            }
            break;

        /* case CELL_TBL:*/
        case 8:
            /* pack output arguments to galtis string */     
            prvFap20mSsrGet_CELL_DB_E(inFields, outArgs);

            tbl_index = 0;
            table_flag_mode++;

            break;
        
        /* case INTERRUPT_TBL:*/
        case 9:
            /* pack output arguments to galtis string */     
            prvFap20mSsrGet_INTERRUPT_DB_E(inFields, outArgs);

            if (++tbl_index >= SAND_MAX_DEVICE)
            {
                tbl_index = 0;
                table_flag_mode++;
            }
            break;

        /* case ROUTING_TBL:*/
        case 10:
            /* pack output arguments to galtis string */     
            prvFap20mSsrGet_ROUTING_DB_E(inFields, outArgs);

            tbl_index = 0;
            table_flag_mode++;

            break;
        
        default:
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
    }

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"ssr_get_cfg_version",
         &wrFap20m_ssr_get_cfg_version,
         1, 0},
        {"ssr_get_buff_size",
         &wrFap20m_ssr_get_buff_size,
         0, 0},
        {"get_max_sw_db_size",
         &wrFap20m_get_max_sw_db_size,
         0, 0},

        {"ssr_auto_aggregate_fill_sw_dbSet",
         &wrFap20m_ssr_auto_aggregate_fill_sw_db,
         8, 27},
        {"auto_flow_fill_sw_dbSet",
         &wrFap20m_ssr_auto_flow_fill_sw_db,
         6, 21},
        {"ssr_auto_mngr_fill_sw_dbSet",
         &wrFap20m_ssr_auto_mngr_fill_sw_db,
         0, 18},
        {"ssr_auto_port_fill_sw_dbSet",
         &wrFap20m_ssr_auto_port_fill_sw_db,
         4, 4},
        {"ssr_auto_queue_fill_sw_dbSet",
         &wrFap20m_ssr_auto_queue_fill_sw_db,
         6, 18},
        {"ssr_cell_fill_sw_dbSet",
         &wrFap20m_ssr_cell_fill_sw_db,
         0, 8},
        {"egress_ports_fill_sw_dbSet",
         &wrFap20m_ssr_egress_ports_fill_sw_db,
         4, 10},
        {"ssr_egress_ports_fill_sw_db_v02Set",
         &wrFap20m_ssr_egress_ports_fill_sw_db_v02,
         2, 4},
        {"ssr_interrupt_services_fill_sw_dbSet",
         &wrFap20m_ssr_interrupt_services_fill_sw_db,
         2, 3},
        {"ssr_routing_fill_sw_dbSet",
         &wrFap20m_ssr_routing_fill_sw_db,
         0, 8},
        {"ssr_spatial_multicast_fill_sw_dbSet",
         &wrFap20m_ssr_spatial_multicast_fill_sw_db,
         4, 1},
        {"ssr_buffSet",
         &wrFap20m_ssr_from_buff,
         1, 0},
        {"ssr_buffGetFirst",
         &wrFap20m_ssr_to_buff,
         3, 0},
        {"ssr_buffGetNext",
         &wrFap20m_ssr_to_buff_end,
         3, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_ssr
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
GT_STATUS cmdLibInitFap20m_api_ssr
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

