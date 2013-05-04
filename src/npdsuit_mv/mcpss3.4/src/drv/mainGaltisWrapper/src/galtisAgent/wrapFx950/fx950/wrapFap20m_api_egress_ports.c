/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_egress_ports.c
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
#include <SAND/Utils/include/sand_os_interface.h>
#include <FX950/include/fap20m_api_scheduler.h>
#include <FX950/include/fap20m_api_egress_ports.h>
#include <FX950/include/fap20m_egress_ports.h>


/* Global variables */
static GT_U32 tbl_index;
static FAP20M_API_EGRESS_PORTS_SW_DB sw_db_module_buff_to_fill;
static FAP20M_API_EGRESS_PORTS_SW_DB sw_db_module_buff;
static FAP20M_EGRESS_PORT_QOS_TABLE ports_qos;


/*****************************************************/
static CMD_STATUS wrFap20m_egress_ports_fill_sw_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_dev_index;
    unsigned int end_dev_index;
    unsigned int start_rank_index;
    unsigned int end_rank_index;
    unsigned int start_port_index;
    unsigned int end_port_index;
    unsigned int i, j, k;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    start_dev_index = (unsigned int)inArgs[0];
    end_dev_index = (unsigned int)inArgs[1];
    start_rank_index = (unsigned int)inArgs[2];
    end_rank_index = (unsigned int)inArgs[3];
    start_port_index = (unsigned int)inArgs[4];
    end_port_index = (unsigned int)inArgs[5];

    for(i = start_dev_index; i <= end_dev_index; i++)
        for(j = start_rank_index; j <= end_rank_index; j++)
        {
            sw_db_module_buff_to_fill.fap20m_egress_port_qos_table_valid[i][j] = (unsigned int)inFields[0];

            for(k = start_port_index; k <= end_port_index; k++)
            {
                sw_db_module_buff_to_fill.fap20m_egress_port_qos_table[i][j].ports[k].valid_mask = (unsigned long)inFields[1];
                sw_db_module_buff_to_fill.fap20m_egress_port_qos_table[i][j].ports[k].nominal_bandwidth = (unsigned long)inFields[2];
                sw_db_module_buff_to_fill.fap20m_egress_port_qos_table[i][j].ports[k].credit_bandwidth = (unsigned long)inFields[3];

                sw_db_module_buff_to_fill.fap20m_port_nominal_rate[i][j][k] = (unsigned long)inFields[4];
            }
        }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_api_egress_ports_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_API_EGRESS_PORTS_SW_DB* sw_db_module_buff -
*    pointer to a structure containing mudule's Software Database (SW_DB)
*  INDIRECT:
*OUTPUT:
*  DIRECT:
*    unsigned long - function's error word
*  INDIRECT:
*    all modules global data structures
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_api_egress_ports_sw_db_load
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot api function */
    result = fap20m_api_egress_ports_sw_db_load(&sw_db_module_buff_to_fill);
    
    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_api_egress_ports_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_API_EGRESS_PORTS_SW_DB* sw_db_module_buff -
*    pointer to a buffer to which Software Database (SW_DB) will be saved
*OUTPUT:
*  DIRECT:
*    unsigned long - function's error word
*  INDIRECT:
*    pointer to the saved Software Database
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_api_egress_ports_sw_db_save
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned short rc;
    unsigned short i, fieldNum;
	

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot api function */
    result = fap20m_api_egress_ports_sw_db_save(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;
    fieldNum = 0;

    /* pack output arguments to galtis string */
    for (i = 0; i < SAND_MAX_DEVICE; i++)
    {
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table_valid[i][0];
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table_valid[i][1];
    }

    for (i = 0; i < SAND_MAX_DEVICE; i++)
    {
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][0].ports[tbl_index].valid_mask;
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][0].ports[tbl_index].nominal_bandwidth;
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][0].ports[tbl_index].credit_bandwidth;
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][1].ports[tbl_index].valid_mask;
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][1].ports[tbl_index].nominal_bandwidth;
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][1].ports[tbl_index].credit_bandwidth;
    }

    for (i = 0; i < SAND_MAX_DEVICE; i++)
    {
        inFields[fieldNum++] = sw_db_module_buff.fap20m_port_nominal_rate[i][0][tbl_index];
        inFields[fieldNum++] = sw_db_module_buff.fap20m_port_nominal_rate[i][1][tbl_index];
    }
    
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                                inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                                inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                                inFields[15], inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23], inFields[24],
                                inFields[25], inFields[26], inFields[27], inFields[28], inFields[29],
                                inFields[30], inFields[31], inFields[32], inFields[33], inFields[34],
                                inFields[35], inFields[36], inFields[37], inFields[38], inFields[39],
                                inFields[40], inFields[41], inFields[42], inFields[43], inFields[44],
                                inFields[45], inFields[46], inFields[47], inFields[48], inFields[49],
                                inFields[50], inFields[51], inFields[52], inFields[53], inFields[54],
                                inFields[55], inFields[56], inFields[57], inFields[58], inFields[59],
                                inFields[60], inFields[61], inFields[62], inFields[63], inFields[64],
                                inFields[65], inFields[66], inFields[67], inFields[68], inFields[69],
                                inFields[70], inFields[71], inFields[72], inFields[73], inFields[74],
                                inFields[75], inFields[76], inFields[77], inFields[78], inFields[79]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_api_egress_ports_sw_db_save_end
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned short i, fieldNum;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    if (++tbl_index >= FAP20M_NOF_DATA_PORTS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    fieldNum = 0;

    /* pack output arguments to galtis string */
    for (i = 0; i < SAND_MAX_DEVICE; i++)
    {
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table_valid[i][0];
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table_valid[i][1];
    }

    for (i = 0; i < SAND_MAX_DEVICE; i++)
    {
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][0].ports[tbl_index].valid_mask;
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][0].ports[tbl_index].nominal_bandwidth;
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][0].ports[tbl_index].credit_bandwidth;
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][1].ports[tbl_index].valid_mask;
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][1].ports[tbl_index].nominal_bandwidth;
        inFields[fieldNum++] = sw_db_module_buff.fap20m_egress_port_qos_table[i][1].ports[tbl_index].credit_bandwidth;
    }

    for (i = 0; i < SAND_MAX_DEVICE; i++)
    {
        inFields[fieldNum++] = sw_db_module_buff.fap20m_port_nominal_rate[i][0][tbl_index];
        inFields[fieldNum++] = sw_db_module_buff.fap20m_port_nominal_rate[i][1][tbl_index];
    }

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d"
                "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                                inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                                inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                                inFields[15], inFields[16], inFields[17], inFields[18], inFields[19],
                                inFields[20], inFields[21], inFields[22], inFields[23], inFields[24],
                                inFields[25], inFields[26], inFields[27], inFields[28], inFields[29],
                                inFields[30], inFields[31], inFields[32], inFields[33], inFields[34],
                                inFields[35], inFields[36], inFields[37], inFields[38], inFields[39],
                                inFields[40], inFields[41], inFields[42], inFields[43], inFields[44],
                                inFields[45], inFields[46], inFields[47], inFields[48], inFields[49],
                                inFields[50], inFields[51], inFields[52], inFields[53], inFields[54],
                                inFields[55], inFields[56], inFields[57], inFields[58], inFields[59],
                                inFields[60], inFields[61], inFields[62], inFields[63], inFields[64],
                                inFields[65], inFields[66], inFields[67], inFields[68], inFields[69],
                                inFields[70], inFields[71], inFields[72], inFields[73], inFields[74],
                                inFields[75], inFields[76], inFields[77], inFields[78], inFields[79]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_port_desc_init
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Initialize the port descriptor data base.
*INPUT:
*  DIRECT:
*    unsigned int device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned short -
*      SAND_OK - else error occurred.
*  INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_port_desc_init
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
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot api function */
    result = fap20m_port_desc_init(device_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_port_desc_get_nominal_rate
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Get the port nominal rate.
*INPUT:
*  DIRECT:
*    IN   unsigned int      device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    IN   unsigned int      scheduler_port_id -
*      Port identifier to get.
*    OUT  unsigned long*    nominal_rate -
*      Loaded with the nominal rate
*      (this is the rate the packet processor 
*      physical interface rate)
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned short -
*      FAP20M_NO_ERR else error occurred.
*  INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_port_desc_get_nominal_rate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned short result;
    unsigned int device_id;
    FAP20M_SCH_RANK sch_rank;
    unsigned int scheduler_port_id;
    unsigned long nominal_rate;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];
    scheduler_port_id = (unsigned int)inArgs[2];

    /* call Ocelot api function */
    result = fap20m_port_desc_get_nominal_rate(device_id, sch_rank, scheduler_port_id, &nominal_rate);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", nominal_rate);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_port_desc_set_nominal_rate
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Set the port nominal rate.
*INPUT:
*  DIRECT:
*    IN   unsigned int      device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    IN   unsigned int      scheduler_port_id -
*      Port identifier to set.
*    IN   unsigned int      nominal_rate -
*      The nominal rate
*      (this is the rate the packet processor 
*      physical interface rate)
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned short -
*      FAP20M_NO_ERR else error occurred.
*  INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_port_desc_set_nominal_rate
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned short result;
    unsigned int device_id;
    FAP20M_SCH_RANK sch_rank;
    unsigned int scheduler_port_id;
    unsigned int nominal_rate;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];
    scheduler_port_id = (unsigned int)inArgs[2];
    nominal_rate = (unsigned int)inArgs[3];

    /* call Ocelot api function */
    result = fap20m_port_desc_set_nominal_rate(device_id, sch_rank, scheduler_port_id, nominal_rate);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

static FAP20M_EGRESS_PORT_QOS_TABLE ports_qos;
static GT_U32                       index_ports_qos;  

/*****************************************************
*NAME
*  fap20m_set_egress_ports_qos
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets the QoS parameters of all egress ports.
*   It sets the nominal rate, the maximum rate, the credit rate 
*   and the priority.
*   The nominal rate is the actual rate of the NP port. it
*   should be set to zero for a non-existing port.
*   The maximum rate is the limitation that can be configured
*   at the egress traffic manager port shaper.
*   The credit rate is different from the nominal rate as it
*   should take into account more factors (i.e. unscheduled 
*   traffic that also uses this port bandwidth, a credit 
*   speedup, and others).
*   The priority is the property that the port uses when it 
*   competes for bandwidth in the egress traffic manager SP
*   scheduler.
*   The scheduled and unscheduled weights define how the port resources
*   would be divided among the scheduled traffic (unicast, and optionally 
*   recycled multicast), and the unscheduled traffic (spatial multicast, and
*   optionally recycled multicast). Either one can be set to have strict 
*   priority on top of the other, by giving it a zero weight.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   FAP20M_EGRESS_PORT_QOS_TABLE   *ports_qos
*       A table of QoS descriptors, one per port.
*   FAP20M_EGRESS_PORT_QOS_TABLE   *exact_ports_qos
*       A table of QoS descriptors, one per port, that is loaded 
*       with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    The LINK interfaces rates should already be set at this point.
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
*    exact_ports_qos - allocated QoS params.
*REMARKS:
*  A. The port rates are implemented as weights on a work-
*  conserving scheduler, and this have the following 
*  consequences:
*  - If the configured port rates oversubscribe the
*    physical rate of their NP interface (LINK4), then
*    the bandwidth would be divided between the ports
*    according to their proportional rates.
*  - If the NP interface rate is changed (for example,
*    in the event of a module HotSwap to a slower NP),
*    then the port actual rates would be changed 
*    accordingly.
*  - If a port does not consume credits (due to flow
*    control from the traffic manager or to lack 
*    of traffic), its allocated bandwidth would be
*    divided between the other ports.
*  B. All ports have a shared maximum burst size, that
*  can be configured by the egress traffic manager API
*SEE ALSO:
*  fap20m_set_device_interfaces_scheduler()
*  fap20m_egress_queues_set_settings()
*****************************************************/
static CMD_STATUS wrFap20m_set_first_egress_ports_qos
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

    sand_os_memset(&ports_qos, 0, sizeof(ports_qos));
       
    /* map fileds arguments to the gloabal table */
    index_ports_qos = (unsigned int) inFields[0];
    ports_qos.ports[index_ports_qos].valid_mask = (unsigned long)inFields[1];
    ports_qos.ports[index_ports_qos].nominal_bandwidth = (unsigned long)inFields[2];
    ports_qos.ports[index_ports_qos].credit_bandwidth  = (unsigned long)inFields[3];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_set_next_egress_ports_qos
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

       
    /* map fileds arguments to the gloabal table */
    index_ports_qos = (unsigned int) inFields[0];
    ports_qos.ports[index_ports_qos].valid_mask = (unsigned long)inFields[1];
    ports_qos.ports[index_ports_qos].nominal_bandwidth = (unsigned long)inFields[2];
    ports_qos.ports[index_ports_qos].credit_bandwidth  = (unsigned long)inFields[3];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_end_set_egress_ports_qos
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SCH_RANK sch_rank;
    FAP20M_EGRESS_PORT_QOS_TABLE exact_ports_qos;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
      
    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    /* call Ocelot api function */
    result = fap20m_set_egress_ports_qos(device_id, sch_rank, &ports_qos, &exact_ports_qos);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    } 

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_one_egress_port_qos
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets the QoS parameters of one egress ports.
*   It sets the nominal rate, the maximum rate, the credit rate 
*   and the priority.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   unsigned int                     port_id   -
*       The egress port to be modified.
*   FAP20M_EGRESS_PORT_QOS_TABLE   *port_qos
*       A table of QoS descriptors, one per port.
*   FAP20M_EGRESS_PORT_QOS_TABLE   *exact_port_qos
*       A table of QoS descriptors, one per port, that is loaded 
*       with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    The LINK interfaces rates should already be set at this point.
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
*    exact_ports_qos - allocated QoS params.
*REMARKS:
*  This function uses the function 
*   fap20m_set_egress_ports_qos(), since some of the 
*   parameters are implemented with a calendar,
*   and therefore, all the ports should be modified toghether.
*SEE ALSO:
*  fap20m_set_egress_ports_qos().
*****************************************************/
static CMD_STATUS wrFap20m_set_one_egress_port_qos
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SCH_RANK sch_rank;
    unsigned int port_id;
    FAP20M_EGRESS_PORT_QOS port_qos;
    FAP20M_EGRESS_PORT_QOS exact_port_qos;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];
    port_id = (unsigned int)inArgs[2];

    port_qos.valid_mask = (unsigned long)inFields[0];
    port_qos.nominal_bandwidth = (unsigned long)inFields[1];
    port_qos.credit_bandwidth  = (unsigned long)inFields[2];

    /* call Ocelot api function */
    result = fap20m_set_one_egress_port_qos(device_id, sch_rank, port_id, 
                                            &port_qos, &exact_port_qos);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_egress_ports_qos
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets the QoS parameters of all egress ports.
*   It gets the nominal rate, the maximum rate, the credit rate 
*   and the priority.
*   The nominal rate is the actual rate of the NP port. it
*   should be set to zero for a non-existing port.
*   The maximum rate is the limitation that can be configured
*   at the egress traffic manager port shaper.
*   The credit rate is different from the nominal rate as it
*   should take into account more factors (i.e. unscheduled 
*   traffic that also uses this port bandwidth, a credit 
*   speedup, and others).
*   The priority is the property that the port uses when it 
*   competes for bandwidth in the egress traffic manager SP
*   scheduler.
*   The scheduled and unscheduled weights define how the port resources
*   would be divided among the scheduled traffic (unicast, and optionally 
*   recycled multicast), and the unscheduled traffic (spatial multicast, and
*   optionally recycled multicast). Either one can be set to have strict 
*   priority on top of the other, by giving it a zero weight.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   FAP20M_EGRESS_PORT_QOS_TABLE   *ports_qos
*       A table of QoS descriptors, one per port, that is loaded 
*       with the actual device parameters.
*  INDIRECT:
*    The LINK interfaces rates should already be set at this point.
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
*    ports_qos - allocated QoS params.
*REMARKS:
*  The port rates are implemented as weights on a work-
*  conserving scheduler, and this have the following 
*  consequences:
*  - If the configured port rates oversubscribe the
*    physical rate of their NP interface (LINK4), then
*    the bandwidth would be divided between the ports
*    according to their proportional rates.
*  - If the NP interface rate is changed (for example,
*    in the event of a module HotSwap to a slower NP),
*    then the port actual rates would be changed 
*    accordingly.
*  - If a port does not consume credits (due to flow
*    control from the traffic manager or to lack 
*    of traffic), its allocated bandwidth would be
*    divided between the other ports.
*  B. All ports have a shared maximum busrt size, that
*  can be configured by the egress traffic manager API
*SEE ALSO:
*  fap20m_get_link_rate()
*  fap20m_egress_queues_set_settings()
*****************************************************/
static CMD_STATUS wrFap20m_get_egress_ports_qos
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SCH_RANK sch_rank;
    unsigned short rc;
	

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    /* call Ocelot api function */
    result = fap20m_get_egress_ports_qos(device_id, sch_rank, &ports_qos);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = ports_qos.ports[tbl_index].valid_mask;
    inFields[2] = ports_qos.ports[tbl_index].nominal_bandwidth;
    inFields[3] = ports_qos.ports[tbl_index].credit_bandwidth;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");
    
    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_get_egress_ports_qos_end
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

    if (++tbl_index >= FAP20M_NOF_DATA_PORTS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = ports_qos.ports[tbl_index].valid_mask;
    inFields[2] = ports_qos.ports[tbl_index].nominal_bandwidth;
    inFields[3] = ports_qos.ports[tbl_index].credit_bandwidth;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");
    
    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_egress_ports_qos_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets the QoS parameters of all egress ports.
*   It gets the nominal rate, the maximum rate, the credit rate 
*   and the priority.
*   The nominal rate is the actual rate of the NP port. it
*   should be set to zero for a non-existing port.
*   The maximum rate is the limitation that can be configured
*   at the egress traffic manager port shaper.
*   The credit rate is different from the nominal rate as it
*   should take into account more factors (i.e. unscheduled 
*   traffic that also uses this port bandwidth, a credit 
*   speedup, and others).
*   The priority is the property that the port uses when it 
*   competes for bandwidth in the egress traffic manager SP
*   scheduler.
*   The scheduled and unscheduled weights define how the port resources
*   would be divided among the scheduled traffic (unicast),
*    and the unscheduled traffic (multicast)
*   Either one can be set to have strict 
*   priority on top of the other, by giving it a zero weight.
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   FAP20M_EGRESS_PORT_QOS_TABLE   *ports_qos
*       A table of QoS descriptors, one per port, that is loaded 
*       with the actual device parameters.
*  INDIRECT:
*    The LINK interfaces rates should already be set at this point.
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
*    ports_qos - allocated QoS params.
*REMARKS:
*  The port rates are implemented as weights on a work-
*  conserving scheduler, and this have the following 
*  consequences:
*  - If the configured port rates oversubscribe the
*    physical rate of their NP interface (LINK4), then
*    the bandwidth would be divided between the ports
*    according to their proportional rates.
*  - If the NP interface rate is changed (for example,
*    in the event of a module HotSwap to a slower NP),
*    then the port actual rates would be changed 
*    accordingly.
*  - If a port does not consume credits (due to flow
*    control from the traffic manager or to lack 
*    of traffic), its allocated bandwidth would be
*    divided between the other ports.
*  B. All ports have a shared maximum busrt size, that
*  can be configured by the egress traffic manager API
*SEE ALSO:
*  fap20m_get_link_rate()
*  fap20m_egress_queues_set_settings()
*****************************************************/
static CMD_STATUS wrFap20m_get_egress_ports_qos_unsafe
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SCH_RANK sch_rank;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_get_egress_ports_qos_unsafe(device_id, sch_rank, &ports_qos);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = ports_qos.ports[tbl_index].valid_mask;
    inFields[2] = ports_qos.ports[tbl_index].nominal_bandwidth;
    inFields[3] = ports_qos.ports[tbl_index].credit_bandwidth;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");
    
    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_get_egress_ports_qos_unsafe_end
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

    if (++tbl_index >= FAP20M_NOF_DATA_PORTS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = ports_qos.ports[tbl_index].valid_mask;
    inFields[2] = ports_qos.ports[tbl_index].nominal_bandwidth;
    inFields[3] = ports_qos.ports[tbl_index].credit_bandwidth;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, GT_OK, "%f");
    
    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_egress_ports_qos_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets the QoS parameters of all egress ports.
*   It sets the nominal rate, the maximum rate, the credit rate 
*   and the priority.
*   The nominal rate is the actual rate of the NP port. it
*   should be set to zero for a non-existing port.
*   The maximum rate is the limitation that can be configured
*   at the egress traffic manager port shaper.
*   The credit rate is different from the nominal rate as it
*   should take into account more factors (i.e. unscheduled 
*   traffic that also uses this port bandwidth, a credit 
*   speedup, and others).
*   The priority is the property that the port uses when it 
*   competes for bandwidth in the egress traffic manager SP
*   scheduler.
*   The scheduled and unscheduled weights define how the port resources
*   would be divided among the scheduled traffic (unicast)
*   and the unscheduled traffic (multicast)
*   Either one can be set to have strict 
*   priority on top of the other, by giving it a zero weight.
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   FAP20M_EGRESS_PORT_QOS_TABLE   *ports_qos
*       A table of QoS descriptors, one per port.
*   FAP20M_EGRESS_PORT_QOS_TABLE   *exact_ports_qos
*       A table of QoS descriptors, one per port, that is loaded 
*       with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    The LINK interfaces rates should already be set at this point.
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
*    exact_ports_qos - allocated QoS params.
*REMARKS:
*  A. The port rates are implemented as weights on a work-
*  conserving scheduler, and this have the following 
*  consequences:
*  - If the configured port rates oversubscribe the
*    physical rate of their NP interface (LINK4), then
*    the bandwidth would be divided between the ports
*    according to their proportional rates.
*  - If the NP interface rate is changed (for example,
*    in the event of a module HotSwap to a slower NP),
*    then the port actual rates would be changed 
*    accordingly.
*  - If a port does not consume credits (due to flow
*    control from the traffic manager or to lack 
*    of traffic), its allocated bandwidth would be
*    divided between the other ports.
*  B. All ports have a shared maximum burst size, that
*  can be configured by the egress traffic manager API
*SEE ALSO:
*  fap20m_set_device_interfaces_scheduler()
*  fap20m_egress_queues_set_settings()
*****************************************************/
static CMD_STATUS wrFap20m_set_egress_ports_qos_unsafe
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SCH_RANK sch_rank;
    FAP20M_EGRESS_PORT_QOS_TABLE exact_ports_qos;
    unsigned int index;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    index = (unsigned int) inFields[0];
    ports_qos.ports[index].valid_mask = (unsigned long)inFields[1];
    ports_qos.ports[index].nominal_bandwidth = (unsigned long)inFields[2];
    ports_qos.ports[index].credit_bandwidth = (unsigned long)inFields[3];

    /* call Ocelot API function */
    result = fap20m_set_egress_ports_qos_unsafe(device_id, sch_rank, &ports_qos, &exact_ports_qos);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_egress_ports_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_EGRESS_PORTS_SW_DB* sw_db_module_buff -
*    pointer to a structure containing mudule's Software Database (SW_DB)
*  INDIRECT:
*OUTPUT:
*  DIRECT:
*    unsigned long - function's error word
*  INDIRECT:
*    all modules global data structures
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_egress_ports_sw_db_load
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    FAP20M_EGRESS_PORTS_SW_DB sw_db_module_buff;
    unsigned short i, fieldNum;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    fieldNum = 0;

    /* map input arguments to locals */
    for (i = 0; i < SAND_MAX_DEVICE; i++)
    {
        sw_db_module_buff.fap20m_link_a_total_ports_credit_rate[i][0] = (unsigned long)inFields[fieldNum++];
        sw_db_module_buff.fap20m_link_a_total_ports_credit_rate[i][1] = (unsigned long)inFields[fieldNum++];
    }

    for (i = 0; i < SAND_MAX_DEVICE; i++)
    {
        sw_db_module_buff.fap20m_link_b_total_ports_credit_rate[i][0] = (unsigned long)inFields[fieldNum++];
        sw_db_module_buff.fap20m_link_b_total_ports_credit_rate[i][1] = (unsigned long)inFields[fieldNum++];
    }

    /* call Ocelot API function */
    result = fap20m_egress_ports_sw_db_load(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_egress_ports_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_EGRESS_PORTS_SW_DB* sw_db_module_buff -
*    pointer to a buffer to which Software Database (SW_DB) will be saved
*OUTPUT:
*  DIRECT:
*    unsigned long - function's error word
*  INDIRECT:
*    pointer to the saved Software Database
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_egress_ports_sw_db_save
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    FAP20M_EGRESS_PORTS_SW_DB sw_db_module_buff;
    unsigned short i, fieldNum;
    unsigned short rc;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_egress_ports_sw_db_save(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    fieldNum = 0;

    /* pack output arguments to galtis string */
    for(i = 0; i < SAND_MAX_DEVICE; i++)
    {
        inFields[fieldNum++] = sw_db_module_buff.fap20m_link_a_total_ports_credit_rate[i][0];
        inFields[fieldNum++] = sw_db_module_buff.fap20m_link_a_total_ports_credit_rate[i][1];
    }

    for(i = 0; i < SAND_MAX_DEVICE; i++)
    {
        inFields[fieldNum++] = sw_db_module_buff.fap20m_link_b_total_ports_credit_rate[i][0];
        inFields[fieldNum++] = sw_db_module_buff.fap20m_link_b_total_ports_credit_rate[i][1];
    }

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                                    inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4], 
                                    inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                                    inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                                    inFields[15], inFields[16], inFields[17], inFields[18], inFields[19],
                                    inFields[20], inFields[21], inFields[22], inFields[23], inFields[24], 
                                    inFields[25], inFields[26], inFields[27], inFields[28], inFields[29],
                                    inFields[30],  inFields[31]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_egress_ports_sw_db_save_end
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


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"egress_ports_fill_sw_dbSet",
         &wrFap20m_egress_ports_fill_sw_db,
         6, 5},
        {"api_egress_ports_sw_dbSet",
         &wrFap20m_api_egress_ports_sw_db_load,
         0, 0},
        {"api_egress_ports_sw_dbGetFirst",
         &wrFap20m_api_egress_ports_sw_db_save,
         0, 0},
        {"api_egress_ports_sw_dbGetNext",
         &wrFap20m_api_egress_ports_sw_db_save_end,
         0, 0},

        {"port_desc_init",
         &wrFap20m_port_desc_init,
         1, 0},
        {"port_desc_get_nominal_rate",
         &wrFap20m_port_desc_get_nominal_rate,
         3, 0},
        {"port_desc_set_nominal_rate",
         &wrFap20m_port_desc_set_nominal_rate,
         4, 0},

        {"egress_ports_qosSetFirst",
         &wrFap20m_set_first_egress_ports_qos,
         2, 4},
        {"egress_ports_qosSetNext",
         &wrFap20m_set_next_egress_ports_qos,
         2, 4},
        {"egress_ports_qosEndSet",
         &wrFap20m_end_set_egress_ports_qos,
         2, 0},

        {"egress_ports_qosGetFirst",
         &wrFap20m_get_egress_ports_qos,
         2, 0},
        {"egress_ports_qosGetNext",
         &wrFap20m_get_egress_ports_qos_end,
         2, 0},

        {"one_egress_port_qosSet",
         &wrFap20m_set_one_egress_port_qos,
         3, 3},

        {"egress_ports_qos_unsafeSet",
         &wrFap20m_set_egress_ports_qos_unsafe,
         2, 3},

        {"egress_ports_qos_unsafeGetFirst",
         &wrFap20m_get_egress_ports_qos_unsafe,
         2, 0},
        {"egress_ports_qos_unsafeGetNext",
         &wrFap20m_get_egress_ports_qos_unsafe_end,
         2, 0},

        {"egress_ports_sw_dbSet",
         &wrFap20m_egress_ports_sw_db_load,
         0, 32},
        {"egress_ports_sw_dbGetFirst",
         &wrFap20m_egress_ports_sw_db_save,
         0, 0},
        {"egress_ports_sw_dbGetNext",
         &wrFap20m_egress_ports_sw_db_save_end,
         0, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_egress_ports
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
GT_STATUS cmdLibInitFap20m_api_egress_ports
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

