/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_scheduler.c
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
#include <FX950/include/fap20m_scheduler_device.h>
#include <FX950/include/fap20m_scheduler_elementary.h>
#include <FX950/include/fap20m_scheduler.h>
#include <FX950/include/fap20m_scheduler_flow.h>


/* Global variables */
static GT_U32 tbl_index;
static FAP20M_DEVICE_SCHEDULER dev_scheduler_to_fill;
static FAP20M_DEVICE_SCHEDULER dev_scheduler;
static FAP20M_SCT sct_to_fill;
static FAP20M_SCT sct;


/*****************************************************
*NAME
*  fap20m_set_primary_scheduler_role
*TYPE:
*  PROC
*DATE: 
*  21/05/2007
*FUNCTION:
*  The FAP20M device supports two schedulers.
*  A primary scheduler and a secondary scheduler.
*  The primary scheduler has more resources,
*  than the secondary.
*  One of the schedulers should schedule the fabric
*  traffic, and the other one should schedule the
*  egress ports.
*  This function sets the role of the primary scheduler.
*INPUT:
*  DIRECT:
*    IN   unsigned int      device_id -
*    IN   FAP20M_SCH_ROLE   scheduler_role -
*      The primary scheduler role:
*      Egress Traffic Manager Scheduler or Fabric Scheduler
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    error indication
*  INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_primary_scheduler_role
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SCH_ROLE scheduler_role;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    scheduler_role = (FAP20M_SCH_ROLE)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_set_primary_scheduler_role(device_id, scheduler_role);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_primary_scheduler_role
*TYPE:
*  PROC
*DATE: 
*  21/05/2007
*FUNCTION:
*  The FAP20M device supports two schedulers.
*  A primary scheduler and a secondary scheduler.
*  The primary scheduler has more resources,
*  than the secondary.
*  One of the schedulers should schedule the fabric
*  traffic, and the other one should schedule the
*  egress ports.
*  This function gets the role of the primary scheduler.
*INPUT:
*  DIRECT:
*    IN   unsigned int      device_id -
*    OUT  FAP20M_SCH_ROLE   *scheduler_role -
*      The primary scheduler role:
*      Egress Traffic Manager Scheduler or Fabric Scheduler
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    error indication
*  INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_primary_scheduler_role
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SCH_ROLE scheduler_role;
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
    result = fap20m_get_primary_scheduler_role(device_id, &scheduler_role);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", scheduler_role);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_device_rate_entry
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets an entry in the device rate table.
*   Each entry sets a credit generation rate, for a given pair of fabric congestion
*   (presented by rci_level) and the number of active fabric links.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_DEVICE_RATE_ENTRY       *dev_rate -
*       An entry in the device rate table
*   FAP20M_DEVICE_RATE_ENTRY       *exact_dev_rate-
*       An entry in the device rate table,
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    None.
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
*    exact_dev_rate - Configured device rate table entry
*REMARKS:
*  A standalone FAP application (no fabric) needs to fill the table
*  with a constant value.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_set_device_rate_entry
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
    FAP20M_DEVICE_RATE_ENTRY dev_rate;
    FAP20M_DEVICE_RATE_ENTRY exact_dev_rate;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    dev_rate.rci_level = (unsigned int)inFields[0];
    dev_rate.num_active_links = (unsigned int)inFields[1];
    dev_rate.rate = (unsigned long)inFields[2];

    /* call Ocelot API function */
    result = fap20m_set_device_rate_entry(device_id, sch_rank, &dev_rate, &exact_dev_rate);

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
*  fap20m_get_device_rate_entry
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets an entry in the device rate table.
*   Each entry sets a credit generation rate, for a given pair of fabric congestion
*   (presented by rci_level) and the number of active fabric links.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to get .Primary or Secondary?
*   FAP20M_DEVICE_RATE_ENTRY       *dev_rate-
*       An entry in the device rate table,
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    None.
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
*    dev_rate - Configured device rate table entry
*REMARKS:
*  A standalone FAP application (no fabric) needs to fill the table
*  with a constant value.
*SEE ALSO:
*  None.
*****************************************************/
static unsigned int    currRciLevel;
static unsigned int    currNumActiveLinks;

static CMD_STATUS wrFap20m_get_device_rate_entry
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
    FAP20M_DEVICE_RATE_ENTRY dev_rate;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    dev_rate.rci_level        = currRciLevel;
    dev_rate.num_active_links = currNumActiveLinks;

    /* call Ocelot API function */
    result = fap20m_get_device_rate_entry(device_id, sch_rank, &dev_rate);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = dev_rate.rci_level;
    inFields[1] = dev_rate.num_active_links;
    inFields[2] = dev_rate.rate;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}


/******************************************************************************/
static CMD_STATUS wrFap20m_get_device_rate_entry_first
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    currRciLevel       = 0;
    currNumActiveLinks = 0;

    return wrFap20m_get_device_rate_entry(inArgs,inFields,numFields,outArgs);
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_device_rate_entry_next
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    currNumActiveLinks ++;
    if( currNumActiveLinks == 25)
    {
        currRciLevel++ ;
        currNumActiveLinks = 0;
        if(currRciLevel == 8)
        {
            galtisOutput(outArgs, GT_OK, "%d", -1);
            return CMD_OK;
        }
    }

    return wrFap20m_get_device_rate_entry(inArgs,inFields,numFields,outArgs);
}

/*****************************************************
*NAME
*  fap20m_set_slow_max_rate
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets the slow rates.
*   Slow rate is rhe rate the shaper gives to flows at the slow state.
*   The FAP20M device supports two slow rates,
*   when each scheduler sub-flow might be configured to each one of
*   the slow rates.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     slow_max_rates[FAP20M_SCH_NOF_SLOW_RATES] -
*       The rate in Kbit/s. 
*  INDIRECT:
*    None.
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
*    class_type - Configured class type configuration
*REMARKS:
*  The implementation calls the SHD function, because,
*   the converting is similar to that table and to the
*   max slow rate.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_slow_max_rate
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
    unsigned int slow_max_rates[FAP20M_SCH_NOF_SLOW_RATES];
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    slow_max_rates[0] = (unsigned int)inFields[0];
    slow_max_rates[1] = (unsigned int)inFields[1];

    /* call Ocelot API function */
    result = fap20m_set_slow_max_rate(device_id, sch_rank, slow_max_rates);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_slow_max_rate
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets the slow rate.
*   A flow might be in slow state, and in that case lower
*   rate is needed.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     *slow_max_rate -
*       The rate Kbit/s. 
*  INDIRECT:
*    None.
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
*    class_type - Configured class type configuration
*REMARKS:
*  The implementation calls the SHD function, because,
*   the converting is similar to that table and to the
*   max slow rate.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_slow_max_rate
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
    unsigned int slow_max_rates[FAP20M_SCH_NOF_SLOW_RATES];
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
    result = fap20m_get_slow_max_rate(device_id, sch_rank, slow_max_rates);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = slow_max_rates[0];
    inFields[1] = slow_max_rates[1];

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_slow_max_rate_end
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
*  fap20m_set_device_interfaces_scheduler
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets the device interfaces scheduler.
*   For each interface (LINKA, LINKB & cpu) it
*   sets a maximum credit rate, and a weight for the
*   interfaces WFQ. The cpu interface does not have a weight,
*   as it has a strict priority over all others interfaces.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_DEVICE_IF_SCHEDULER    *dev_if_sch -
*       A structure that contains the device interfaces scheduler
*   FAP20M_DEVICE_IF_SCHEDULER    *exact_dev_if_sch -
*       A structure that contains the device interfaces scheduler,
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    None.
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
*    exact_dev_if_sceh - Configured device interfaces scheduler configuration
*REMARKS:
*   The LINK credit rates that are determined by the FAP20M_DEVICE_IF_SCHEDULER
*   param should be coordinated with the LINK maximum rates that are configured
*   in FAP20M_EGRESS_QUEUES_SHAPING_PARAMS, and with the ports credit and maximum rate 
*   that are configured in FAP20M_EGRESS_PORT_QOS.
*SEE ALSO:
*  fap20m_egress_queues_get_settings()
*  fap20m_get_egress_ports_qos
*****************************************************/
static CMD_STATUS wrFap20m_set_device_interfaces_scheduler
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
    FAP20M_DEVICE_IF_SCHEDULER dev_if_sch;
    FAP20M_DEVICE_IF_SCHEDULER exact_dev_if_sch;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    dev_if_sch.valid_mask = (unsigned long)inFields[0];
    dev_if_sch.link_a_max_rate = (unsigned long)inFields[1];
    dev_if_sch.link_a_weight = (unsigned long)inFields[2];
    dev_if_sch.link_b_max_rate = (unsigned long)inFields[3];
    dev_if_sch.link_b_weight = (unsigned long)inFields[4];

    /* call Ocelot API function */
    result = fap20m_set_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch, &exact_dev_if_sch);

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
*  fap20m_get_device_interfaces_scheduler
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets the device interfaces scheduler.
*   For each interface (LINKA, LINKB & cpu) it
*   gets a maximum credit rate, and a weight for the
*   interfaces WFQ. The cpu interface does not have a weight,
*   as it has a strict priority over all others interfaces.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_DEVICE_IF_SCHEDULER    *dev_if_sch -
*       A structure that contains the device interfaces scheduler,
*       that is loaded with the actual device parameters. 
*  INDIRECT:
*    None.
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
*    dev_if_sceh - Configured device interfaces scheduler configuration
*REMARKS:
*   The LINK credit rates that are determined by the FAP20M_DEVICE_IF_SCHEDULER
*   param should be coordinated with the LINK maximum rates that are configured
*   in FAP20M_EGRESS_QUEUES_SHAPING_PARAMS, and with the ports credit and maximum rate 
*   that are configured in FAP20M_EGRESS_PORT_QOS.
*SEE ALSO:
*  fap20m_egress_queues_get_settings()
*  fap20m_get_egress_ports_qos
*****************************************************/
static CMD_STATUS wrFap20m_get_device_interfaces_scheduler
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
    FAP20M_DEVICE_IF_SCHEDULER dev_if_sch;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    dev_if_sch.valid_mask = (unsigned long)inFields[0];
    dev_if_sch.link_a_max_rate = (unsigned long)inFields[1];
    dev_if_sch.link_a_weight = (unsigned long)inFields[2];
    dev_if_sch.link_b_max_rate = (unsigned long)inFields[3];
    dev_if_sch.link_b_weight = (unsigned long)inFields[4];

    /* call Ocelot API function */
    result = fap20m_get_device_interfaces_scheduler(device_id, sch_rank, &dev_if_sch);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = dev_if_sch.valid_mask;
    inFields[1] = dev_if_sch.link_a_max_rate;
    inFields[2] = dev_if_sch.link_a_weight;
    inFields[3] = dev_if_sch.link_b_max_rate;
    inFields[4] = dev_if_sch.link_b_weight;
    inFields[5] = 0;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_device_interfaces_scheduler_end
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

static CMD_STATUS wrFap20m_device_scheduler_fill_device_rate_table
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_index;
    unsigned int end_index;
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    start_index = (unsigned int)inArgs[0];
    end_index = (unsigned int)inArgs[1];

    for (i = start_index; i <= end_index; i++)
    {
       dev_scheduler_to_fill.fcg.drt.rates[i].rci_level = (unsigned int)inFields[0];
       dev_scheduler_to_fill.fcg.drt.rates[i].num_active_links = (unsigned int)inFields[1];
       dev_scheduler_to_fill.fcg.drt.rates[i].rate = (unsigned long)inFields[2];
    }

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_device_scheduler
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets the device scheduler as a whole.
*   It sets the configuration of the device rate table and 
*   the device interfaces scheduler.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_DEVICE_SCHEDULER       *dev_scheduler -
*       A structure that contains the configuration of the device scheduler
*   FAP20M_DEVICE_SCHEDULER       *exact_dev_scheduler -
*       A structure that contains the configuration of the device scheduler,
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    None.
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
*    exact_dev_scheduler - Configured device scheduler configuration
*REMARKS:
*   The LINK schedulers, which are the lowest level of the device schedulers,
*   determine the credit rate per port. These rates must be coordinated with 
*   the ports maximum rate that are controlled by the egress traffic manager,
*   and therefore these dependant sets of port rates are configured 
*   simultaneously in FAP20M_EGRESS_PORT_QOS.
*SEE ALSO:
*  fap20m_set_device_rate_entry
*  fap20m_set_device_interfaces_scheduler
*  fap20m_egress_queues_set_settings()
*  fap20m_set_egress_ports_qos
*****************************************************/
static CMD_STATUS wrFap20m_set_first_device_scheduler
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
    
    sand_os_memset(&dev_scheduler_to_fill, 0, sizeof(dev_scheduler_to_fill));

    /* map input fields to the global table */
    tbl_index = (unsigned long)inFields[0];
    dev_scheduler_to_fill.fcg.drt.rates[tbl_index].rci_level = (unsigned int)inFields[1];
    dev_scheduler_to_fill.fcg.drt.rates[tbl_index].num_active_links = (unsigned int)inFields[2];
    dev_scheduler_to_fill.fcg.drt.rates[tbl_index].rate = (unsigned long)inFields[3];     
  
    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_set_next_device_scheduler
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
    
    /* map input fields to the global table */
    tbl_index = (unsigned long)inFields[0];
    dev_scheduler_to_fill.fcg.drt.rates[tbl_index].rci_level = (unsigned int)inFields[1];
    dev_scheduler_to_fill.fcg.drt.rates[tbl_index].num_active_links = (unsigned int)inFields[2];
    dev_scheduler_to_fill.fcg.drt.rates[tbl_index].rate = (unsigned long)inFields[3];  

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_end_set_device_scheduler
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
    FAP20M_DEVICE_SCHEDULER exact_dev_scheduler;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;
    
    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];
    dev_scheduler_to_fill.valid_mask = (unsigned long)inArgs[2];
    dev_scheduler_to_fill.dev_if_scheduler.valid_mask = (unsigned long)inArgs[3];
    dev_scheduler_to_fill.dev_if_scheduler.link_a_max_rate = (unsigned long)inArgs[4];
    dev_scheduler_to_fill.dev_if_scheduler.link_a_weight = (unsigned long)inArgs[5];
    dev_scheduler_to_fill.dev_if_scheduler.link_b_max_rate = (unsigned long)inArgs[6];
    dev_scheduler_to_fill.dev_if_scheduler.link_b_weight = (unsigned long)inArgs[7];

    /* call Ocelot API function */
    result = fap20m_set_device_scheduler(device_id, sch_rank, &dev_scheduler_to_fill, &exact_dev_scheduler);

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
*  fap20m_get_device_scheduler
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets the device scheduler as a whole.
*   It gets the configuration of the device rate table and 
*   the device interfaces scheduler.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_DEVICE_SCHEDULER       *dev_scheduler -
*       A structure that contains the configuation of the device scheduler,
*       that is loaded with the actual device parameters.
*  INDIRECT:
*    None.
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
*    dev_scheduler - Configured device scheduler configuration
*REMARKS:
*   The LINK schedulers, which are the lowest level of the device schedulers,
*   determine the credit rate per port. These rates must be coordinated with 
*   the ports maximum rate that are controlled by the egress traffic manager,
*   and therefore these dependant sets of port rates are configured 
*   simultaneously in FAP20M_EGRESS_PORT_QOS.
*SEE ALSO:
*  fap20m_get_device_rate_entry
*  fap20m_get_device_interfaces_scheduler
*  fap20m_egress_queues_get_settings()
*  fap20m_get_egress_ports_qos
*****************************************************/
static CMD_STATUS wrFap20m_get_device_scheduler
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
    result = fap20m_get_device_scheduler(device_id, sch_rank, &dev_scheduler);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = dev_scheduler.fcg.drt.rates[tbl_index].rci_level;
    inFields[2] = dev_scheduler.fcg.drt.rates[tbl_index].num_active_links;
    inFields[3] = dev_scheduler.fcg.drt.rates[tbl_index].rate;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d",
                inFields[0], inFields[1], inFields[2],inFields[3]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_get_device_scheduler_next
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

    if (++tbl_index >= FAP20M_DRT_SIZE)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;
    inFields[1] = dev_scheduler.fcg.drt.rates[tbl_index].rci_level;
    inFields[2] = dev_scheduler.fcg.drt.rates[tbl_index].num_active_links;
    inFields[3] = dev_scheduler.fcg.drt.rates[tbl_index].rate;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d",
                inFields[0], inFields[1], inFields[2], inFields[2]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_HR_scheduler_modes
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets the HR schedulers modes of operation.
*   It defines (on a device scope) which mode complements the SINGLE_WFQ 
*   mode: DUAL_WFQ or ENHANCED_PRIO_WFQ
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     use_enhanced_priority -
*       Indicator:
*         '1' - ENHANCED_PRIO_WFQ complements SINGLE_WFQ
*         '0' - DUAL_WFQ complements SINGLE_WFQ
*  INDIRECT:
*    None.
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
*    None.
*REMARKS:
*  None.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_set_HR_scheduler_modes
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
    unsigned int use_enhanced_priority;
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
    use_enhanced_priority = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_set_HR_scheduler_modes(device_id, sch_rank, use_enhanced_priority);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_HR_scheduler_modes
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets the HR schedulers modes of operation.
*   It reads (on a device scope) which mode complements the SINGLE_WFQ 
*   mode: DUAL_WFQ or ENHANCED_PRIO_WFQ
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int*                    use_enhanced_priority -
*       Loaded with indicator:
*         '1' - ENHANCED_PRIO_WFQ complements SINGLE_WFQ
*         '0' - DUAL_WFQ complements SINGLE_WFQ
*  INDIRECT:
*    None.
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
*    use_enhanced_priority.
*REMARKS:
*  None.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_get_HR_scheduler_modes
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
    unsigned int use_enhanced_priority;
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

    /* call Ocelot API function */
    result = fap20m_get_HR_scheduler_modes(device_id, sch_rank, &use_enhanced_priority);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", use_enhanced_priority);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_scheduler_class_type_params
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets a scheduler class type.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_CL_SCHED_CLASS_PARAMS   *class_type_params -
*       A structure that contains a class type configuration
*   FAP20M_CL_SCHED_CLASS_PARAMS   *exact_class_type_params -
*       A structure that contains a class type configuration,
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    None.
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
*    exact_class_type_params - Configured class type configuration
*REMARKS:
*  None.
*SEE ALSO:
*  fap20m_set_scheduler_class_type_params_table()
*****************************************************/
static CMD_STATUS wrFap20m_set_scheduler_class_type_params
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
    FAP20M_CL_SCHED_CLASS_PARAMS class_type_params;
    FAP20M_CL_SCHED_CLASS_PARAMS exact_class_type_params;
    unsigned short rc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* avoid compiler warnings */
    numFields = numFields;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    class_type_params.id = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[0];
    class_type_params.mode = (FAP20M_CL_SCHED_CLASS_MODE)inFields[1];

    class_type_params.weight[0] = (unsigned long )inFields[2];
    class_type_params.weight[1] = (unsigned long )inFields[3];
    class_type_params.weight[2] = (unsigned long )inFields[4];
    class_type_params.weight[3] = (unsigned long )inFields[5];

    class_type_params.weight_mode = (FAP20M_CL_SCHED_CLASS_WEIGHTS_MODE)inFields[6];

    /* call Ocelot API function */
    result = fap20m_set_scheduler_class_type_params(device_id, sch_rank, &class_type_params, &exact_class_type_params);

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
*  fap20m_get_scheduler_class_type_params
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets a scheduler class type.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_CL_SCHED_CLASS_PARAMS   *class_type_params -
*       A structure that contains a class type configuration,
*       that is loaded with the actual device parameters. 
*  INDIRECT:
*    None.
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
*    class_type_params - Configured class type configuration
*REMARKS:
*  None.
*SEE ALSO:
*  fap20m_get_scheduler_class_type_params_table()
*****************************************************/
static CMD_STATUS wrFap20m_get_scheduler_class_type_params
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
    FAP20M_CL_SCHED_CLASS_PARAMS class_type_params;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    class_type_params.id = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[0];
    class_type_params.mode = (FAP20M_CL_SCHED_CLASS_MODE)inFields[1];

    class_type_params.weight[0] = (unsigned long )inFields[2];
    class_type_params.weight[1] = (unsigned long )inFields[3];
    class_type_params.weight[2] = (unsigned long )inFields[4];
    class_type_params.weight[3] = (unsigned long )inFields[5];

    class_type_params.weight_mode = (FAP20M_CL_SCHED_CLASS_WEIGHTS_MODE)inFields[6];

    /* call Ocelot API function */
    result = fap20m_get_scheduler_class_type_params(device_id, sch_rank, &class_type_params);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = class_type_params.id;
    inFields[1] = class_type_params.mode;

    inFields[2] = class_type_params.weight[0];
    inFields[3] = class_type_params.weight[1];
    inFields[4] = class_type_params.weight[2];
    inFields[5] = class_type_params.weight[3];

    inFields[6] = class_type_params.weight_mode;

    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], 
                     inFields[3], inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_scheduler_class_type_params_end
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

/*****************************************************/
static CMD_STATUS wrFap20m_scheduler_class_type_params_fill_db
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int start_index;
    unsigned int end_index;
    unsigned int i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    start_index = (unsigned int)inArgs[0];
    end_index = (unsigned int)inArgs[1];
    
    for (i = start_index; i <= end_index; i++)
    {
       sct_to_fill.class_types[i].id = (FAP20M_CL_SCHED_CLASS_TYPE) inFields[0];
       sct_to_fill.class_types[i].mode = (FAP20M_CL_SCHED_CLASS_MODE) inFields[1];

       sct_to_fill.class_types[i].weight[0] = (unsigned long) inFields[2];
       sct_to_fill.class_types[i].weight[1] = (unsigned long) inFields[3];
       sct_to_fill.class_types[i].weight[2] = (unsigned long) inFields[4];
       sct_to_fill.class_types[i].weight[3] = (unsigned long) inFields[5];

       sct_to_fill.class_types[i].weight_mode = (FAP20M_CL_SCHED_CLASS_WEIGHTS_MODE) inFields[6];
    }

    sct_to_fill.nof_class_types = (unsigned int) inFields[7];

    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_scheduler_class_type_params_table
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets the scheduler class type table
*   as a whole.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_SCT                     *sct        -
*       A structure that contains a complete class type table
*   FAP20M_SCT                     *exact_sct -
*       A structure that contains a complete class type table,
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    None.
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
*    exact_sct - Configured class type table
*REMARKS:
*  None.
*SEE ALSO:
*  fap20m_set_scheduler_class_type_params()
*****************************************************/
static CMD_STATUS wrFap20m_set_scheduler_class_type_params_table
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
    FAP20M_SCT exact_sct;
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

    /* call Ocelot API function */
    result = fap20m_set_scheduler_class_type_params_table(device_id, sch_rank, &sct_to_fill, &exact_sct);

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
*  fap20m_get_scheduler_class_type_params_table
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets the scheduler class type table
*   as a whole.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_SCT                     *sct -
*       A structure that contains a complete class type table,
*       that is loaded with the actual device values.
*  INDIRECT:
*    None.
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
*    sct - Configured class type table
*REMARKS:
*  None.
*SEE ALSO:
*  fap20m_get_scheduler_class_type_params()
*****************************************************/
static CMD_STATUS wrFap20m_get_scheduler_class_type_params_table
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
    result = fap20m_get_scheduler_class_type_params_table(device_id, sch_rank, &sct);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = sct.class_types[tbl_index].id;
    inFields[1] = sct.class_types[tbl_index].mode;

    inFields[2] = sct.class_types[tbl_index].weight[0];
    inFields[3] = sct.class_types[tbl_index].weight[1];
    inFields[4] = sct.class_types[tbl_index].weight[2];
    inFields[5] = sct.class_types[tbl_index].weight[3];

    inFields[6] = sct.class_types[tbl_index].weight_mode;

    inFields[7] = sct.nof_class_types;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                  inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFap20m_get_scheduler_class_type_params_table_end
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

    if (++tbl_index >= FAP20M_SCH_NOF_CLASS_TYPES)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = sct.class_types[tbl_index].id;
    inFields[1] = sct.class_types[tbl_index].mode;

    inFields[2] = sct.class_types[tbl_index].weight[0];
    inFields[3] = sct.class_types[tbl_index].weight[1];
    inFields[4] = sct.class_types[tbl_index].weight[2];
    inFields[5] = sct.class_types[tbl_index].weight[3];

    inFields[6] = sct.class_types[tbl_index].weight_mode;

    inFields[7] = sct.nof_class_types;

    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                  inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_scheduler_port
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets a port scheduler.
*   The driver writes to the following tables:
*   Scheduler Enable Memory (SEM),
*   HR-Scheduler-Configuration (SHC),
*   Flow Group Memory (FGM)
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_ELEMENTARY_SCHEDULER    *scheduler        -
*       An elementary scheduler of type HR
*  INDIRECT:
*    None.
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
*    None.
*REMARKS:
*  Only HR 0 - 63 can be used as port schedulers, and they 
*  are mapped to Egress-Ports 0-63 respectively.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_set_scheduler_port
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
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    scheduler.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
    scheduler.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            scheduler.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            scheduler.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[3];
        default: break;
    }
    
    /* call Ocelot API function */
    result = fap20m_set_scheduler_port(device_id, sch_rank, &scheduler);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

static void prvFap20mSchedulerPort_HR_E
(
    IN FAP20M_ELEMENTARY_SCHEDULER p_sch_port,
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = p_sch_port.id;
    inFields[1] = p_sch_port.type;
    inFields[2] = p_sch_port.state;
    inFields[3] = p_sch_port.sched_properties.hr.mode;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 0);
}

static void prvFap20mSchedulerPort_CL_E
(
    IN FAP20M_ELEMENTARY_SCHEDULER p_sch_port,
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = p_sch_port.id;
    inFields[1] = p_sch_port.type;
    inFields[2] = p_sch_port.state;
    inFields[3] = p_sch_port.sched_properties.cl.class_type;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 1);
}


/*****************************************************
*NAME
*  fap20m_get_scheduler_port
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets a port scheduler.
*   The driver reads from the following tables:
*   Scheduler Enable Memory (SEM),
*   HR-Scheduler-Configuration (SHC),
*   Flow Group Memory (FGM)
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_ELEMENTARY_SCHEDULER    *scheduler        -
*       An elementary scheduler of type HR, 
*       that is loaded with the actual device values.
*       'id' field should be a valid scheduler-port id 
*  INDIRECT:
*    None.
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
*    scheduler - Configured port scheduler
*REMARKS:
*  Only HR 0 - 63 can be used as port schedulers, and they 
*  are mapped to Egress-Ports 0-63 respectively.
*SEE ALSO:
*  None.
*****************************************************/
static FAP20M_ELEMENTARY_SCHEDULER_ID currentSchId, lastSchId;

static CMD_STATUS wrFap20m_get_scheduler_port
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
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    unsigned short rc = 0;

    if(currentSchId > lastSchId)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* avoid compiler warnings */
    numFields = numFields;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

/* 
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
    scheduler.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            scheduler.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            scheduler.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[3];
    }
*/

    /* call Ocelot API function */
    scheduler.id = currentSchId;
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inArgs[4];
    result = fap20m_get_scheduler_port(device_id, sch_rank, &scheduler);

    rc = sand_get_error_code_from_error_word(result);
    while(rc != SAND_OK)
    {
        currentSchId++;
        if(currentSchId > lastSchId)
            break;

        scheduler.id = currentSchId;
        result = fap20m_get_scheduler_port(device_id, sch_rank, &scheduler);

        rc = sand_get_error_code_from_error_word(result);
    }

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    if (FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR == scheduler.type)
    {
        prvFap20mSchedulerPort_HR_E(scheduler, inFields, outArgs);
    } 
    else
    {
        prvFap20mSchedulerPort_CL_E(scheduler, inFields, outArgs);
    }

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_scheduler_port_first
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    currentSchId = (FAP20M_ELEMENTARY_SCHEDULER_ID)inArgs[2];
    lastSchId    = (FAP20M_ELEMENTARY_SCHEDULER_ID)inArgs[3];

    return wrFap20m_get_scheduler_port(inArgs,inFields,numFields,outArgs);
}
/******************************************************************************/
static CMD_STATUS wrFap20m_get_scheduler_port_next
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    currentSchId++;
    return wrFap20m_get_scheduler_port(inArgs,inFields,numFields,outArgs);
}

/*****************************************************
*NAME
*  fap20m_is_scheduler_port_dynamically_used
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*  The function fap20m_is_scheduler_port_dynamically_used()
*  read the internal dynamic DB, and make sure that the 
*  port is not dynamically used,
*  and therefore, might be modified.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     port_id     -
*       The Port ID that the application plan on using. Range 0 - 64
*   unsigned int                     *is_dynamically_used
*       Is the resource currently free.
*  INDIRECT:
*    None.
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
*    scheduler - Configured port scheduler
*REMARKS:
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_is_scheduler_port_dynamically_used
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
    unsigned int is_dynamically_used;
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
    port_id = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_is_scheduler_port_dynamically_used(device_id, sch_rank, port_id, &is_dynamically_used);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", is_dynamically_used);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_scheduler_aggregate
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets an aggregate scheduler.
*   It configures an elementary scheduler, and defines
*   a credit flow to this scheduler from a 'father' scheduler.
*   The driver writes to the following tables:
*   Scheduler Enable Memory (SEM),
*   HR-Scheduler-Configuration (SHC),
*   CL-Schedulers Configuration (SCC),
*   Flow Group Memory (FGM)
*   Shaper Descriptor Memory (SHD)
*   Flow Sub-Flow (FSF)
*   Flow Descriptor Memory (FDM)
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_ELEMENTARY_SCHEDULER    *scheduler  -
*       An elementary scheduler of any type.    
*   FAP20M_FLOW                    *flow       -
*       A credit flow from a source scheduler (or 2 source schedulers)
*   FAP20M_FLOW                    *exact_flow_given  -
*       A credit flow from a source scheduler (or 2 source schedulers),
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.       
*  INDIRECT:
*    None.
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
*    exact_flow_given - Configured credit flow.
*REMARKS:
*  To close an aggregate, call this function with state
*   FAP20M_ELEMENTARY_SCHEDULER_STATE_DISABLE.
*   this will also close the aggregate's flow capabilities
*   so make sure that the flow->id is == scheduler->id
*  HR 0 - 63 can not be used as aggregate schedulers.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_set_scheduler_aggregate
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
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    FAP20M_FLOW flow;
    FAP20M_FLOW exact_flow_given;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    scheduler.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
    scheduler.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            scheduler.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            scheduler.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[4];
            break;
        default: break;

    }

    flow.id = (FAP20M_FLOW_ID)inFields[5];

    flow.sub_flow[0].valid_mask = (FAP20M_FLOW_ID)inFields[6];
    flow.sub_flow[0].id = (FAP20M_FLOW_ID)inFields[7];
    flow.sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[8];
    flow.sub_flow[0].shaper.max_rate = (unsigned long)inFields[9];
    flow.sub_flow[0].shaper.max_burst = (unsigned long)inFields[10];

    flow.sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE)inFields[11];
    switch (flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            flow.sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[12];
            flow.sub_flow[0].subflow_properties.hr.weight = (unsigned long)inFields[14];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            flow.sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[13];
            flow.sub_flow[0].subflow_properties.cl.weight = (unsigned long)inFields[14];
            break;
        default: break;
    }

    flow.sub_flow[1].valid_mask = (FAP20M_FLOW_ID)inFields[15];
    flow.sub_flow[1].id = (FAP20M_FLOW_ID)inFields[16];
    flow.sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[17];
    flow.sub_flow[1].shaper.max_rate = (unsigned long)inFields[18];
    flow.sub_flow[1].shaper.max_burst = (unsigned long)inFields[19];

    flow.sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE)inFields[20];
    switch (flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            flow.sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[21];
            flow.sub_flow[1].subflow_properties.hr.weight = (unsigned long)inFields[23];

            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            flow.sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[22];
            flow.sub_flow[1].subflow_properties.cl.weight = (unsigned long)inFields[23];
            break;
        default: break;
    }

    flow.flow_type = (FAP20M_FLOW_TYPE)inFields[24];
    flow.is_slow_enabled = (unsigned int)inFields[25];

    /* call Ocelot API function */
    result = fap20m_set_scheduler_aggregate(device_id, sch_rank, &scheduler, 
                                            &flow, &exact_flow_given);

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
*  fap20m_get_scheduler_aggregate
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets an aggregate scheduler.
*   It gets an elementary scheduler, and
*   a credit flow to this scheduler from a 'father' scheduler.
*   The driver reads from the following tables:
*   Scheduler Enable Memory (SEM),
*   HR-Scheduler-Configuration (SHC),
*   CL-Schedulers Configuration (SCC),
*   Flow Group Memory (FGM)
*   Shaper Descriptor Memory (SHD)
*   Flow Sub-Flow (FSF)
*   Flow Descriptor Memory (FDM)
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_ELEMENTARY_SCHEDULER    *scheduler  -
*       An elementary scheduler of any type.
*      'id' field should be a valid scheduler-aggregate id    
*   FAP20M_FLOW                    *flow       -
*       A credit flow from a source scheduler (or 2 source schedulers),
*       that is loaded with the actual device values. 
*  INDIRECT:
*    None.
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
*    scheduler - Configured aggregate scheduler
*    flow      - Configured credit flow.
*REMARKS:
*  HR 0 - 63 can not be used as aggregate schedulers.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_get_scheduler_aggregate
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
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    FAP20M_FLOW flow;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    scheduler.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
    scheduler.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            scheduler.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            scheduler.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[4];
            break;
        default: break;
    }

    /* call Ocelot API function */
    result = fap20m_get_scheduler_aggregate(device_id, sch_rank, &scheduler, &flow);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    /* SCHEDULER */
    inFields[0] = scheduler.id;
    inFields[1] = scheduler.type;
    inFields[2] = scheduler.state;

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = scheduler.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = scheduler.sched_properties.cl.class_type;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }

    /* FLOW */
    inFields[5] = flow.id;

    inFields[6] = flow.sub_flow[0].valid_mask;
    inFields[7] = flow.sub_flow[0].id;
    inFields[8] = flow.sub_flow[0].credit_source;
    inFields[9] = flow.sub_flow[0].shaper.max_rate;
    inFields[10] = flow.sub_flow[0].shaper.max_burst;

    inFields[11] = flow.sub_flow[0].type;
    switch (flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }


    inFields[15] = flow.sub_flow[1].valid_mask;
    inFields[16] = flow.sub_flow[1].id;
    inFields[17] = flow.sub_flow[1].credit_source;
    inFields[18] = flow.sub_flow[1].shaper.max_rate;
    inFields[19] = flow.sub_flow[1].shaper.max_burst;

    inFields[20] = flow.sub_flow[1].type;
    switch (flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = flow.flow_type;
    inFields[25] = flow.is_slow_enabled;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],  inFields[9],  inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_scheduler_aggregate_end
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


static FAP20M_FLOW flow;

/*****************************************************
*NAME
*  fap20m_set_scheduler_flow
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets a scheduler flow, 
*   from an elementary scheduler (or schedulers) to a queue.
*   The driver writes to the following tables:
*   Scheduler Enable Memory (SEM),
*   Flow to FIP and Queue Mapping (FFQ)
*   Shaper Descriptor Memory (SHD)
*   Flow Sub-Flow (FSF)
*   Flow Descriptor Memory (FDM)
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     source_fap_id -
*       Ingress device identifier.  Range from 0 to 2047 (2K).
*   unsigned int                     source_queue_id
*       Ingress queue identifier. Range from 0 to 8191 (8K).
*   FAP20M_FLOW                    *flow       -
*       A credit flow from a source scheduler (or 2 source schedulers)
*   FAP20M_FLOW                    *exact_flow_given  -
*       A credit flow from a source scheduler (or 2 source schedulers),
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.       
*  INDIRECT:
*    None.
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
*    exact_flow_given - Configured credit flow.
*REMARKS:
*  None.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_set_scheduler_flow
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
    unsigned int source_fap_id;
    unsigned int source_queue_id;
    FAP20M_FLOW exact_flow_given;
    unsigned short rc;
    

    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];
    source_fap_id = (unsigned int)inFields[0];
    source_queue_id = (unsigned int)inFields[1];

    flow.id = (FAP20M_FLOW_ID)inFields[2];

    flow.sub_flow[0].valid_mask = (FAP20M_FLOW_ID)inFields[3];
    flow.sub_flow[0].id = (FAP20M_FLOW_ID)inFields[4];
    flow.sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[5];
    flow.sub_flow[0].shaper.max_rate = (unsigned long)inFields[6];
    flow.sub_flow[0].shaper.max_burst = (unsigned long)inFields[7];

    flow.sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE)inFields[8];
    switch (flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            flow.sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[9];
            flow.sub_flow[0].subflow_properties.hr.weight = (unsigned long)inFields[11];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            flow.sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[10];
            flow.sub_flow[0].subflow_properties.cl.weight = (unsigned long)inFields[11];
            break;
        default: break;
    }

    flow.sub_flow[1].valid_mask = (FAP20M_FLOW_ID)inFields[12];
    flow.sub_flow[1].id = (FAP20M_FLOW_ID)inFields[13];
    flow.sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[14];
    flow.sub_flow[1].shaper.max_rate = (unsigned long)inFields[15];
    flow.sub_flow[1].shaper.max_burst = (unsigned long)inFields[16];

    flow.sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE)inFields[17];
    switch (flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            flow.sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[18];
            flow.sub_flow[1].subflow_properties.hr.weight = (unsigned long)inFields[20];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            flow.sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[19];
            flow.sub_flow[1].subflow_properties.cl.weight = (unsigned long)inFields[20];
            break;
        default: break;
    }

    flow.flow_type = (FAP20M_FLOW_TYPE)inFields[21];
    flow.is_slow_enabled = (unsigned int)inFields[22];

    /* call Ocelot API function */
    result = fap20m_set_scheduler_flow(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

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
*  fap20m_get_scheduler_flow
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets a scheduler flow, 
*   from an elementary scheduler (or schedulers) to a queue.
*   The driver reads from the following tables:
*   Scheduler Enable Memory (SEM),
*   Flow to FIP and Queue Mapping (FFQ)
*   Shaper Descriptor Memory (SHD)
*   Flow Sub-Flow (FSF)
*   Flow Descriptor Memory (FDM)
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     *source_fap_id -
*       Loaded with the ingress device identifier. Range from 0 to 2047 (2K).
*   unsigned int                     *source_queue_id -
*       Loaded with the ingress queue identifier. Range from 0 to 8191 (8K).
*   FAP20M_FLOW                    *flow    -
*       A credit flow from a source scheduler (or 2 source schedulers),
*       that is loaded with the actual device values.
*       'id' field should be a valid flow id
*  INDIRECT:
*    None.
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
*    source_fap_id   - Configured ingress device id.
*    source_queue_id - Configured ingress queue id.
*    flow            - Configured credit flow.
*REMARKS:
*  None.
*SEE ALSO:
*  None.
*****************************************************/
static FAP20M_FLOW_ID currentFlowId, lastFlowId;
static CMD_STATUS wrFap20m_get_scheduler_flow_entry
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
    unsigned int source_fap_id;
    unsigned int source_queue_id;
    FAP20M_FLOW exact_flow_given;
    unsigned short rc = 0;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    if(currentFlowId > lastFlowId)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* call Ocelot API function */
    exact_flow_given.id = currentFlowId;
    result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_id, &source_queue_id, &exact_flow_given);

    rc = sand_get_error_code_from_error_word(result);
    while(rc != SAND_OK)
    {
        if(currentFlowId > lastFlowId)
            break;

        currentFlowId++;
        exact_flow_given.id = currentFlowId;
        result = fap20m_get_scheduler_flow(device_id, sch_rank, &source_fap_id, &source_queue_id, &exact_flow_given);

        rc = sand_get_error_code_from_error_word(result);
    }

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = source_fap_id;
    inFields[1] = source_queue_id;
    inFields[2] = exact_flow_given.id;

    inFields[3] = exact_flow_given.sub_flow[0].valid_mask;
    inFields[4] = exact_flow_given.sub_flow[0].id;
    inFields[5] = exact_flow_given.sub_flow[0].credit_source;
    inFields[6] = exact_flow_given.sub_flow[0].shaper.max_rate;
    inFields[7] = exact_flow_given.sub_flow[0].shaper.max_burst;

    inFields[8] = exact_flow_given.sub_flow[0].type;
    switch (exact_flow_given.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[9] = exact_flow_given.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[11] = exact_flow_given.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[10] = exact_flow_given.sub_flow[0].subflow_properties.cl.class_val;
            inFields[11] = exact_flow_given.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[9]  = -1;
            inFields[10] = -1;
            inFields[11] = -1;
    }

    inFields[12] = exact_flow_given.sub_flow[1].valid_mask;
    inFields[13] = exact_flow_given.sub_flow[1].id;
    inFields[14] = exact_flow_given.sub_flow[1].credit_source;
    inFields[15] = exact_flow_given.sub_flow[1].shaper.max_rate;
    inFields[16] = exact_flow_given.sub_flow[1].shaper.max_burst;

    inFields[17] = exact_flow_given.sub_flow[1].type;
    switch (exact_flow_given.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[18] = exact_flow_given.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[20] = exact_flow_given.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[19] = exact_flow_given.sub_flow[1].subflow_properties.cl.class_val;
            inFields[20] = exact_flow_given.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[18] = -1;
            inFields[19] = -1;
            inFields[20] = -1;
    }

    inFields[21] = exact_flow_given.flow_type;
    inFields[22] = exact_flow_given.is_slow_enabled;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                            inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                            inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                            inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                            inFields[15], inFields[16], inFields[17], inFields[18], inFields[19], 
                            inFields[20], inFields[21], inFields[22]);

    galtisOutput(outArgs, rc, "%d%d%f", source_fap_id, source_queue_id);

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_scheduler_flow_first
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    currentFlowId = (FAP20M_FLOW_ID)inArgs[2];
    lastFlowId    = (FAP20M_FLOW_ID)inArgs[3];

    return wrFap20m_get_scheduler_flow_entry(inArgs,inFields,numFields,outArgs);
}
/******************************************************************************/
static CMD_STATUS wrFap20m_get_scheduler_flow_next
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    currentFlowId++;
    return wrFap20m_get_scheduler_flow_entry(inArgs,inFields,numFields,outArgs);
}

/*****************************************************
*NAME
*  fap20m_del_scheduler_flow
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function deletes a scheduler flow, 
*   from an elementary scheduler (or schedulers) to a queue.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   unsigned int                     scheduler_flow_id -
*       The flow to delete. Range from 0 to 16383 (16K).
*   unsigned int                     do_delay -
*     Indicator.
*     TRUE: Do some fixed delay via 'sand_os_task_delay_milisec'.
*     This delay enable to the device un-install the flow.
*     If this flow-id is not to be used (quickly) again 
*     one can ask not to do delay.
*     A higher application may, close one flow and open another to the 
*     needed queue, to reduce the amount of time to wait.
*  INDIRECT:
*    None.
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
*  None.
*REMARKS:
*  The flow will need some time to be completely un-installed 
*  from the inner scheduler structure.
*  So, the higher application that uses the close method needs
*  to give some time before using this flow-id again.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_del_scheduler_flow
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
    unsigned int scheduler_flow_id;
    unsigned int do_delay;
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
    scheduler_flow_id = (unsigned int)inArgs[2];
    do_delay = (unsigned int)inArgs[3];

    /* call Ocelot API function */
    result = fap20m_del_scheduler_flow(device_id, sch_rank, scheduler_flow_id, do_delay);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_update_scheduler_flow
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function update flow/aggregate atributes, that might be
*   updated when traffic is running.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     flow_id -
*        Flow identifier.  Range from 0 to 16385 (16K).
*         might also be done for aggregate.
*   unsigned int                     sub_flow_id
*        Sub Flow identifier.  Range from 0 to 16385 (16K).
*        The first sub flow will have the ID of the flow,
*         and the second one (if exist), will be flow ID + 1. 
*   FAP20M_FLOW_UPDATE_TYPE        update_type       -
*       The flow attribute that need to be changed.
*   unsigned long                      update_value    -
*       The new value that need to be written to the attribute.
*   unsigned long                      *exact_value
*       The new value that was actually written to the attribute.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*REMARKS:
*  Also valid for Aggregate!!.
*****************************************************/
static CMD_STATUS wrFap20m_update_scheduler_flow
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
    unsigned int flow_id;
    unsigned int sub_flow_id;
    FAP20M_FLOW_UPDATE_TYPE update_type;
    unsigned long update_value;
    unsigned long exact_value;
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
    flow_id = (unsigned int)inArgs[2];
    sub_flow_id = (unsigned int)inArgs[3];
    update_type = (FAP20M_FLOW_UPDATE_TYPE)inArgs[4];
    update_value = (unsigned long)inArgs[5];

    /* call Ocelot API function */
    result = fap20m_update_scheduler_flow(device_id, sch_rank, flow_id, sub_flow_id, update_type, update_value, &exact_value);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", exact_value);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_is_scheduler_flow_dynamically_used
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
* The function fap20m_is_scheduler_flow_dynamically_used()
* read the internal dynamic DB, and make sure that the 
* flow/aggregate is not dynamically used,
* and therefore, might be modified.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     flow_id -
*       The ID of the Flow. Range from 65 to 16K.
*   unsigned int                     nof_subflows -
*       The number of sub-flows that the application plan to
*         use with the above flow ID. Range from 0 or 1.
*   unsigned int                      *is_dynamically_used
*       Is the flow ID, or one of its subflows are used by
*        the internal device's DB, and therefore shouldn't
*        be used by the application
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*REMARKS:
*  None.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_is_scheduler_flow_dynamically_used
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
    unsigned int flow_id;
    unsigned int nof_subflows;
    unsigned int is_dynamically_used;
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
    flow_id = (unsigned int)inArgs[2];
    nof_subflows = (unsigned int)inArgs[3];

    /* call Ocelot API function */
    result = fap20m_is_scheduler_flow_dynamically_used(device_id, sch_rank, flow_id, nof_subflows, &is_dynamically_used);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", is_dynamically_used);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_sch_port_flow_control_set
*TYPE:
*  PROC
*DATE: 
*  12-JULY-07
*FUNCTION:
*   This function sets emulated flow control
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   FAP20M_SCH_RANK                  sch_rank -
*       Rank of scheduler (primary or secondary)
*   unsigned int                     port -
*     Which port to enable. If port 'port' is asserted 
*     then it emulates a situation that fc is received from the
*     egress for port 'port'.
*   unsigned int                     enable -
*     Enable or disable emulation of port 'port'
*  INDIRECT:
*    None.
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
*  None.
*REMARKS:
*  None
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_sch_port_flow_control_set
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
    unsigned int port;
    unsigned int enable;
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
    port = (unsigned int)inArgs[2];
    enable = (unsigned int)inArgs[3];

    /* call Ocelot API function */
    result = fap20m_sch_port_flow_control_set(device_id, sch_rank, port, enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_sch_port_flow_control_get
*TYPE:
*  PROC
*DATE: 
*  12-JULY-07
*FUNCTION:
*   This function gets whether emulated flow control is enabled
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   FAP20M_SCH_RANK                  sch_rank -
*       Rank of scheduler (primary or secondary)
*   unsigned int                     port -
*     Which port to enable. If port 'port' is asserted 
*     then it emulates a situation that fc is received from the
*     egress for port 'port'.
*   unsigned int                     *enable -
*     Enable or disable emulation of port 'port'
*  INDIRECT:
*    None.
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
*  None.
*REMARKS:
*  None
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_sch_port_flow_control_get
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
    unsigned int port;
    unsigned int enable;
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
    port = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_sch_port_flow_control_get(device_id, sch_rank, port, &enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", enable);

    return CMD_OK;
}


/*****************************************************
*NAME
*  fap20m_set_device_rate_entry_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets an entry in the device rate table.
*   Each entry sets a credit generation rate, for a given pair of fabric congestion
*   (presented by rci_level) and the number of active fabric links.
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_DEVICE_RATE_ENTRY       *dev_rate -
*       An entry in the device rate table
*   FAP20M_DEVICE_RATE_ENTRY       *exact_dev_rate-
*       An entry in the device rate table,
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    None.
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
*    exact_dev_rate - Configured device rate table entry
*REMARKS:
*  A standalone FAP application (no fabric) needs to fill the table
*  with a constant value.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_set_device_rate_entry_unsafe
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
    FAP20M_DEVICE_RATE_ENTRY dev_rate;
    FAP20M_DEVICE_RATE_ENTRY exact_dev_rate;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    dev_rate.rci_level = (unsigned int)inFields[0];
    dev_rate.num_active_links = (unsigned int)inFields[1];
    dev_rate.rate = (unsigned long)inFields[2];

    /* call Ocelot API function */
    result = fap20m_set_device_rate_entry_unsafe(device_id, sch_rank, &dev_rate, &exact_dev_rate);

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
*  fap20m_get_device_rate_entry_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets an entry in the device rate table.
*   Each entry sets a credit generation rate, for a given pair of fabric congestion
*   (presented by rci_level) and the number of active fabric links.
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_DEVICE_RATE_ENTRY       *dev_rate-
*       An entry in the device rate table,
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    None.
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
*    dev_rate - Configured device rate table entry
*REMARKS:
*  A standalone FAP application (no fabric) needs to fill the table
*  with a constant value.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_get_device_rate_entry_unsafe
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
    FAP20M_DEVICE_RATE_ENTRY dev_rate;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    dev_rate.rci_level = (unsigned int)inFields[0];
    dev_rate.num_active_links = (unsigned int)inFields[1];
    dev_rate.rate = (unsigned long)inFields[2];

    /* call Ocelot API function */
    result = fap20m_get_device_rate_entry_unsafe(device_id, sch_rank, &dev_rate);
    
    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = dev_rate.rci_level;
    inFields[1] = dev_rate.num_active_links;
    inFields[2] = dev_rate.rate;

    /* pack and output table fields */
    fieldOutput("%d%d%d", inFields[0], inFields[1], inFields[2]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_device_rate_entry_unsafe_end
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
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_set_elementary_scheduler_unsafe
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
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    scheduler.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
    scheduler.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            scheduler.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            scheduler.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[3];
            break;
        default: break;
    }
    
    /* call Ocelot API function */
    result = fap20m_set_elementary_scheduler_unsafe(device_id, sch_rank, &scheduler);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

static void prvFap20mSchedulerUnsafe_HR_E
(
    IN FAP20M_ELEMENTARY_SCHEDULER p_sch_port,
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = p_sch_port.id;
    inFields[1] = p_sch_port.type;
    inFields[2] = p_sch_port.state;
    inFields[3] = p_sch_port.sched_properties.hr.mode;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 0);
}

static void prvFap20mSchedulerUnsafe_CL_E
(
    IN FAP20M_ELEMENTARY_SCHEDULER p_sch_port,
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = p_sch_port.id;
    inFields[1] = p_sch_port.type;
    inFields[2] = p_sch_port.state;
    inFields[3] = p_sch_port.sched_properties.cl.class_type;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 1);
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_get_elementary_scheduler_unsafe
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
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    unsigned short rc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* avoid compiler warnings */
    numFields = numFields;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    scheduler.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
    scheduler.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            scheduler.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            scheduler.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[3];
            break;
        default: break;
    }

    /* call Ocelot API function */
    result = fap20m_get_elementary_scheduler_unsafe(device_id, sch_rank, &scheduler);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    if (FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL == scheduler.type)
    {
        prvFap20mSchedulerUnsafe_CL_E(scheduler, inFields, outArgs);
    } 
    else
    {
        prvFap20mSchedulerUnsafe_HR_E(scheduler, inFields, outArgs);
    }

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_elementary_scheduler_unsafe_end
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
*  fap20m_set_scheduler_aggregate_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets an aggregate scheduler.
*   It configures an elementary scheduler, and defines
*   a credit flow to this scheduler from a 'father' scheduler.
*   The driver writes to the following tables:
*   Scheduler Enable Memory (SEM),
*   HR-Scheduler-Configuration (SHC),
*   CL-Schedulers Configuration (SCC),
*   Flow Group Memory (FGM)
*   Shaper Descriptor Memory (SHD)
*   Flow Sub-Flow (FSF)
*   Flow Descriptor Memory (FDM)
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_ELEMENTARY_SCHEDULER    *scheduler  -
*       An elementary scheduler of any type.    
*   FAP20M_FLOW                    *flow       -
*       A credit flow from a source scheduler (or 2 source schedulers)
*   FAP20M_FLOW                    *exact_flow_given  -
*       A credit flow from a source scheduler (or 2 source schedulers),
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.       
*  INDIRECT:
*    None.
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
*    exact_flow_given - Configured credit flow.
*REMARKS:
*  HR 0 - 63 can not be used as aggregate schedulers.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_set_scheduler_aggregate_unsafe
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
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    FAP20M_FLOW flow;
    FAP20M_FLOW exact_flow_given;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    scheduler.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
    scheduler.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            scheduler.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            scheduler.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[4];
            break;
        default: break;
    }

    flow.id = (FAP20M_FLOW_ID)inFields[5];

    flow.sub_flow[0].valid_mask = (FAP20M_FLOW_ID)inFields[6];
    flow.sub_flow[0].id = (FAP20M_FLOW_ID)inFields[7];
    flow.sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[8];
    flow.sub_flow[0].shaper.max_rate = (unsigned long)inFields[9];
    flow.sub_flow[0].shaper.max_burst = (unsigned long)inFields[10];

    flow.sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE)inFields[11];
    switch (flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            flow.sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[12];
            flow.sub_flow[0].subflow_properties.hr.weight = (unsigned long)inFields[14];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            flow.sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[13];
            flow.sub_flow[0].subflow_properties.cl.weight = (unsigned long)inFields[14];
            break;
        default: break;
    }

    flow.sub_flow[1].valid_mask = (FAP20M_FLOW_ID)inFields[15];
    flow.sub_flow[1].id = (FAP20M_FLOW_ID)inFields[16];
    flow.sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[17];
    flow.sub_flow[1].shaper.max_rate = (unsigned long)inFields[18];
    flow.sub_flow[1].shaper.max_burst = (unsigned long)inFields[19];

    flow.sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE)inFields[20];
    switch (flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            flow.sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[21];
            flow.sub_flow[1].subflow_properties.hr.weight = (unsigned long)inFields[23];

            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            flow.sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[22];
            flow.sub_flow[1].subflow_properties.cl.weight = (unsigned long)inFields[23];
            break;
        default: break;
    }

    flow.flow_type = (FAP20M_FLOW_TYPE)inFields[24];
    flow.is_slow_enabled = (unsigned int)inFields[25];

    /* call Ocelot API function */
    result = fap20m_set_scheduler_aggregate_unsafe(device_id, sch_rank, &scheduler, 
                                                   &flow, &exact_flow_given);

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
*  fap20m_get_scheduler_aggregate_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets an aggregate scheduler.
*   It gets an elementary scheduler, and
*   a credit flow to this scheduler from a 'father' scheduler.
*   The driver reads from the following tables:
*   Scheduler Enable Memory (SEM),
*   HR-Scheduler-Configuration (SHC),
*   CL-Schedulers Configuration (SCC),
*   Flow Group Memory (FGM)
*   Shaper Descriptor Memory (SHD)
*   Flow Sub-Flow (FSF)
*   Flow Descriptor Memory (FDM)
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_ELEMENTARY_SCHEDULER    *scheduler  -
*       An elementary scheduler of any type.
*       'id' field should be a valid scheduler-aggregate id    
*   FAP20M_FLOW                    *flow       -
*       A credit flow from a source scheduler (or 2 source schedulers),
*       that is loaded with the actual device values. 
*  INDIRECT:
*    None.
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
*    scheduler - Configured aggregate scheduler
*    flow      - Configured credit flow.
*REMARKS:
*  HR 0 - 63 can not be used as aggregate schedulers.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_get_scheduler_aggregate_unsafe
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
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    FAP20M_FLOW flow;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    scheduler.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
    scheduler.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            scheduler.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            scheduler.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[4];
            break;
        default: break;
    }

    /* call Ocelot API function */
    result = fap20m_get_scheduler_aggregate_unsafe(device_id, sch_rank, &scheduler, &flow);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    /* SCHEDULER */
    inFields[0] = scheduler.id;
    inFields[1] = scheduler.type;
    inFields[2] = scheduler.state;

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            inFields[3] = scheduler.sched_properties.hr.mode;
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            inFields[4] = scheduler.sched_properties.cl.class_type;
        default:
            inFields[3] = -1;
            inFields[4] = -1;
    }

    /* FLOW */
    inFields[5] = flow.id;

    inFields[6] = flow.sub_flow[0].valid_mask;
    inFields[7] = flow.sub_flow[0].id;
    inFields[8] = flow.sub_flow[0].credit_source;
    inFields[9] = flow.sub_flow[0].shaper.max_rate;
    inFields[10] = flow.sub_flow[0].shaper.max_burst;

    inFields[11] = flow.sub_flow[0].type;
    switch (flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[12] = flow.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[14] = flow.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[13] = flow.sub_flow[0].subflow_properties.cl.class_val;
            inFields[14] = flow.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[12] = -1;
            inFields[13] = -1;
            inFields[14] = -1;
    }


    inFields[15] = flow.sub_flow[1].valid_mask;
    inFields[16] = flow.sub_flow[1].id;
    inFields[17] = flow.sub_flow[1].credit_source;
    inFields[18] = flow.sub_flow[1].shaper.max_rate;
    inFields[19] = flow.sub_flow[1].shaper.max_burst;

    inFields[20] = flow.sub_flow[1].type;
    switch (flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[21] = flow.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[23] = flow.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[22] = flow.sub_flow[1].subflow_properties.cl.class_val;
            inFields[23] = flow.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[21] = -1;
            inFields[22] = -1;
            inFields[23] = -1;
    }

    inFields[24] = flow.flow_type;
    inFields[25] = flow.is_slow_enabled;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],  inFields[5],
                inFields[6],  inFields[7],  inFields[8],  inFields[9],  inFields[10], inFields[11],
                inFields[12], inFields[13], inFields[14], inFields[15], inFields[16], inFields[17],
                inFields[18], inFields[19], inFields[20], inFields[21], inFields[22], inFields[23],
                inFields[24], inFields[25]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_scheduler_aggregate_unsafe_end
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
*  fap20m_set_scheduler_class_type_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets a scheduler class type.
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_CL_SCHED_CLASS_PARAMS          *class_type -
*       A structure that contains a class type configuration
*   FAP20M_CL_SCHED_CLASS_PARAMS          *exact_class_type -
*       A structure that contains a class type configuration,
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.
*  INDIRECT:
*    None.
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
*    exact_class_type - Configured class type configuration
*REMARKS:
*  None.
*SEE ALSO:
*  fap20m_set_scheduler_class_type()
*****************************************************/
static CMD_STATUS wrFap20m_set_scheduler_class_type_unsafe
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
    FAP20M_CL_SCHED_CLASS_PARAMS class_type_params;
    FAP20M_CL_SCHED_CLASS_PARAMS exact_class_type_params;
    unsigned short rc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* avoid compiler warnings */
    numFields = numFields;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    class_type_params.id = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[0];
    class_type_params.mode = (FAP20M_CL_SCHED_CLASS_MODE)inFields[1];

    class_type_params.weight[0] = (unsigned long )inFields[2];
    class_type_params.weight[1] = (unsigned long )inFields[3];
    class_type_params.weight[2] = (unsigned long )inFields[4];
    class_type_params.weight[3] = (unsigned long )inFields[5];

    class_type_params.weight_mode = (FAP20M_CL_SCHED_CLASS_WEIGHTS_MODE)inFields[6];

    /* call Ocelot API function */
    result = fap20m_set_scheduler_class_type_unsafe(device_id, sch_rank, &class_type_params, &exact_class_type_params);

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
*  fap20m_get_scheduler_class_type_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets a scheduler class type.
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_CL_SCHED_CLASS_PARAMS          *class_type -
*       A structure that contains a class type configuration,
*       that is loaded with the actual device parameters. 
*  INDIRECT:
*    None.
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
*    class_type - Configured class type configuration
*REMARKS:
*  None.
*SEE ALSO:
*  fap20m_get_scheduler_class_type_params()
*****************************************************/
static CMD_STATUS wrFap20m_get_scheduler_class_type_unsafe
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
    FAP20M_CL_SCHED_CLASS_PARAMS class_type_params;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    class_type_params.id = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[0];
    class_type_params.mode = (FAP20M_CL_SCHED_CLASS_MODE)inFields[1];

    class_type_params.weight[0] = (unsigned long )inFields[2];
    class_type_params.weight[1] = (unsigned long )inFields[3];
    class_type_params.weight[2] = (unsigned long )inFields[4];
    class_type_params.weight[3] = (unsigned long )inFields[5];

    class_type_params.weight_mode = (FAP20M_CL_SCHED_CLASS_WEIGHTS_MODE)inFields[6];

    /* call Ocelot API function */
    result = fap20m_get_scheduler_class_type_unsafe(device_id, sch_rank, &class_type_params);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = class_type_params.id;
    inFields[1] = class_type_params.mode;

    inFields[2] = class_type_params.weight[0];
    inFields[3] = class_type_params.weight[1];
    inFields[4] = class_type_params.weight[2];
    inFields[5] = class_type_params.weight[3];

    inFields[6] = class_type_params.weight_mode;

    fieldOutput("%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], 
                     inFields[3], inFields[4], inFields[5], inFields[6]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_scheduler_class_type_unsafe_end
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
*  fap20m_set_scheduler_flow_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets a scheduler flow, 
*   from an elementary scheduler (or schedulers) to a queue.
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     source_fap_id -
*       Ingress device identifier.  Range from 0 to 2047 (2K).
*   unsigned int                     source_queue_id
*       Ingress queue identifier. Range from 0 to 8191 (8K).
*   FAP20M_FLOW                    *flow       -
*       A credit flow from a source scheduler (or 2 source schedulers)
*   FAP20M_FLOW                    *exact_flow_given  -
*       A credit flow from a source scheduler (or 2 source schedulers),
*       that is loaded with the actual written values. These can differ from
*       the given values due to rounding.       
*  INDIRECT:
*    None.
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
*    exact_flow_given - Configured credit flow.
*REMARKS:
*  None.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_set_scheduler_flow_unsafe
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
    unsigned int source_fap_id;
    unsigned int source_queue_id;
    FAP20M_FLOW flow;
    FAP20M_FLOW exact_flow_given;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];
    source_fap_id = (unsigned int)inArgs[2];
    source_queue_id = (unsigned int)inArgs[3];

    flow.id = (FAP20M_FLOW_ID)inFields[0];

    flow.sub_flow[0].valid_mask = (FAP20M_FLOW_ID)inFields[1];
    flow.sub_flow[0].id = (FAP20M_FLOW_ID)inFields[2];
    flow.sub_flow[0].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[3];
    flow.sub_flow[0].shaper.max_rate = (unsigned long)inFields[4];
    flow.sub_flow[0].shaper.max_burst = (unsigned long)inFields[5];

    flow.sub_flow[0].type = (FAP20M_SUB_FLOW_TYPE)inFields[6];
    switch (flow.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            flow.sub_flow[0].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[7];
            flow.sub_flow[0].subflow_properties.hr.weight = (unsigned long)inFields[9];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            flow.sub_flow[0].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[8];
            flow.sub_flow[0].subflow_properties.cl.weight = (unsigned long)inFields[9];
            break;
        default: break;
    }

    flow.sub_flow[1].valid_mask = (FAP20M_FLOW_ID)inFields[10];
    flow.sub_flow[1].id = (FAP20M_FLOW_ID)inFields[11];
    flow.sub_flow[1].credit_source = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[12];
    flow.sub_flow[1].shaper.max_rate = (unsigned long)inFields[13];
    flow.sub_flow[1].shaper.max_burst = (unsigned long)inFields[14];

    flow.sub_flow[1].type = (FAP20M_SUB_FLOW_TYPE)inFields[15];
    switch (flow.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            flow.sub_flow[1].subflow_properties.hr.sp_class = (FAP20M_SUB_FLOW_HR_CLASS)inFields[16];
            flow.sub_flow[1].subflow_properties.hr.weight = (unsigned long)inFields[18];
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            flow.sub_flow[1].subflow_properties.cl.class_val = (FAP20M_SUB_FLOW_CL_CLASS)inFields[17];
            flow.sub_flow[1].subflow_properties.cl.weight = (unsigned long)inFields[18];
            break;
        default: break;
    }

    flow.flow_type = (FAP20M_FLOW_TYPE)inFields[19];
    flow.is_slow_enabled = (unsigned int)inFields[20];

    /* call Ocelot API function */
    result = fap20m_set_scheduler_flow_unsafe(device_id, sch_rank, source_fap_id, source_queue_id, &flow, &exact_flow_given);

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
*  fap20m_get_scheduler_flow_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets a scheduler flow, 
*   from an elementary scheduler (or schedulers) to a queue.
*   The driver reads from the following tables:
*   Scheduler Enable Memory (SEM),
*   Flow to FIP and Queue Mapping (FFQ)
*   Shaper Descriptor Memory (SHD)
*   Flow Sub-Flow (FSF)
*   Flow Descriptor Memory (FDM)
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     *source_fap_id -
*       Loaded with the ingress device identifier. Range from 0 to 2047 (2K).
*   unsigned int                     *source_queue_id -
*       Loaded with the ingress queue identifier. Range from 0 to 8191 (8K).
*   FAP20M_FLOW                    *flow    -
*       A credit flow from a source scheduler (or 2 source schedulers),
*       that is loaded with the actual device values.
*       'id' field should be a valid flow id
*  INDIRECT:
*    None.
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
*    source_fap_id   - Configured ingress device id.
*    source_queue_id - Configured ingress queue id.
*    flow            - Configured credit flow.
*REMARKS:
*  None.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_get_scheduler_flow_unsafe
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
    unsigned int source_fap_id;
    unsigned int source_queue_id;
    FAP20M_FLOW exact_flow_given;
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
    result = fap20m_get_scheduler_flow_unsafe(device_id, sch_rank, &source_fap_id, &source_queue_id, &exact_flow_given);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = exact_flow_given.id;

    inFields[1] = exact_flow_given.sub_flow[0].valid_mask;
    inFields[2] = exact_flow_given.sub_flow[0].id;
    inFields[3] = exact_flow_given.sub_flow[0].credit_source;
    inFields[4] = exact_flow_given.sub_flow[0].shaper.max_rate;
    inFields[5] = exact_flow_given.sub_flow[0].shaper.max_burst;

    inFields[6] = exact_flow_given.sub_flow[0].type;
    switch (exact_flow_given.sub_flow[0].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[7] = exact_flow_given.sub_flow[0].subflow_properties.hr.sp_class;
            inFields[9] = exact_flow_given.sub_flow[0].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[8] = exact_flow_given.sub_flow[0].subflow_properties.cl.class_val;
            inFields[9] = exact_flow_given.sub_flow[0].subflow_properties.cl.weight;
            break;
        default:
            inFields[7] = -1;
            inFields[8] = -1;
            inFields[9] = -1;
    }

    inFields[10] = exact_flow_given.sub_flow[1].valid_mask;
    inFields[11] = exact_flow_given.sub_flow[1].id;
    inFields[12] = exact_flow_given.sub_flow[1].credit_source;
    inFields[13] = exact_flow_given.sub_flow[1].shaper.max_rate;
    inFields[14] = exact_flow_given.sub_flow[1].shaper.max_burst;

    inFields[15] = exact_flow_given.sub_flow[1].type;
    switch (exact_flow_given.sub_flow[1].type)
    {
        case FAP20M_SUB_FLOW_TYPE_HR:
            inFields[16] = exact_flow_given.sub_flow[1].subflow_properties.hr.sp_class;
            inFields[18] = exact_flow_given.sub_flow[1].subflow_properties.hr.weight;
            break;
        case FAP20M_SUB_FLOW_TYPE_CL:
            inFields[17] = exact_flow_given.sub_flow[1].subflow_properties.cl.class_val;
            inFields[18] = exact_flow_given.sub_flow[1].subflow_properties.cl.weight;
            break;
        default:
            inFields[16] = -1;
            inFields[17] = -1;
            inFields[18] = -1;
    }

    inFields[19] = exact_flow_given.flow_type;
    inFields[20] = exact_flow_given.is_slow_enabled;

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                            inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],
                            inFields[5],  inFields[6],  inFields[7],  inFields[8],  inFields[9],
                            inFields[10], inFields[11], inFields[12], inFields[13], inFields[14],
                            inFields[15], inFields[16], inFields[17], inFields[18], inFields[19], inFields[20]);

    galtisOutput(outArgs, rc, "%d%d%f", source_fap_id, source_queue_id);

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_scheduler_flow_unsafe_end
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
*  fap20m_del_scheduler_flow_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function deletes a scheduler flow, 
*   from an elementary scheduler (or schedulers) to a queue.
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     scheduler_flow_id -
*       The flow to delete. Range from 0 to 16383 (16K).
*   unsigned int                     do_delay -
*     Indicator.
*     TRUE: Do some fixed delay via 'sand_os_task_delay_milisec'.
*     This delay enable to the device un-install the flow.
*     If this flow-id is not to be used (quickly) again 
*     one can ask not to do delay.
*     A higher application may, close one flow and open another to the 
*     needed queue, to reduce the amount of time to wait.
*  INDIRECT:
*    None.
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
*  None.
*REMARKS:
*  The flow will need some time to be completely un-installed 
*  from the inner scheduler structure.
*  So, the higher application that uses the close method needs
*  to give some time before using this flow-id again.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_del_scheduler_flow_unsafe
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
    unsigned int scheduler_flow_id;
    unsigned int do_delay;
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
    scheduler_flow_id = (unsigned int)inArgs[2];
    do_delay = (unsigned int)inArgs[3];

    /* call Ocelot API function */
    result = fap20m_del_scheduler_flow_unsafe(device_id, sch_rank, scheduler_flow_id, do_delay);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_scheduler_port_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets a port scheduler.
*   The driver writes to the following tables:
*   Scheduler Enable Memory (SEM),
*   HR-Scheduler-Configuration (SHC),
*   Flow Group Memory (FGM)
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_ELEMENTARY_SCHEDULER    *scheduler        -
*       An elementary scheduler of type HR
*  INDIRECT:
*    None.
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
*    None.
*REMARKS:
*  Only HR 0 - 63 can be used as port schedulers, and they 
*  are mapped to Egress-Ports 0-63 respectively.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_set_scheduler_port_unsafe
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
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    scheduler.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
    scheduler.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            scheduler.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            scheduler.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[3];
            break;
        default: break;
    }
    
    /* call Ocelot API function */
    result = fap20m_set_scheduler_port_unsafe(device_id, sch_rank, &scheduler);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

static void prvFap20mSchedulerPortUnsafe_HR_E
(
    IN FAP20M_ELEMENTARY_SCHEDULER p_sch_port,
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = p_sch_port.id;
    inFields[1] = p_sch_port.type;
    inFields[2] = p_sch_port.state;
    inFields[3] = p_sch_port.sched_properties.hr.mode;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 0);
}

static void prvFap20mSchedulerPortUnsafe_CL_E
(
    IN FAP20M_ELEMENTARY_SCHEDULER p_sch_port,
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    inFields[0] = p_sch_port.id;
    inFields[1] = p_sch_port.type;
    inFields[2] = p_sch_port.state;
    inFields[3] = p_sch_port.sched_properties.cl.class_type;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "%d%f", 1);
}

/*****************************************************
*NAME
*  fap20m_get_scheduler_port_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets a port scheduler.
*   The driver reads from the following tables:
*   Scheduler Enable Memory (SEM),
*   HR-Scheduler-Configuration (SHC),
*   Flow Group Memory (FGM)
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   FAP20M_ELEMENTARY_SCHEDULER    *scheduler        -
*       An elementary scheduler of type HR, 
*       that is loaded with the actual device values.
*       'id' field should be a valid scheduler-port id
*  INDIRECT:
*    None.
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
*    scheduler - Configured port scheduler
*REMARKS:
*  Only HR 0 - 63 can be used as port schedulers, and they 
*  are mapped to Egress-Ports 0-63 respectively.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_get_scheduler_port_unsafe
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
    FAP20M_ELEMENTARY_SCHEDULER scheduler;
    unsigned short rc;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* avoid compiler warnings */
    numFields = numFields;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    scheduler.id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inFields[0];
    scheduler.type = (FAP20M_ELEMENTARY_SCHEDULER_TYPE)inFields[1];
    scheduler.state = (FAP20M_ELEMENTARY_SCHEDULER_STATE)inFields[2];

    switch (scheduler.type)
    {
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR:
            scheduler.sched_properties.hr.mode = (FAP20M_HR_SCHED_MODE)inFields[3];
            break;
        case FAP20M_ELEMENTARY_SCHEDULER_TYPE_CL:
            scheduler.sched_properties.cl.class_type = (FAP20M_CL_SCHED_CLASS_TYPE)inFields[3];
            break;
        default: break;
    }

    /* call Ocelot API function */
    result = fap20m_get_scheduler_port_unsafe(device_id, sch_rank, &scheduler);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    if (FAP20M_ELEMENTARY_SCHEDULER_TYPE_HR == scheduler.type)
    {
        prvFap20mSchedulerPortUnsafe_HR_E(scheduler, inFields, outArgs);
    } 
    else
    {
        prvFap20mSchedulerPortUnsafe_CL_E(scheduler, inFields, outArgs);
    }

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_scheduler_port_unsafe_end
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
*  fap20m_set_slow_max_rate_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function sets the slow rates.
*   Slow rate is the rate the shaper gives to flows at the slow state.
*   The FAP20M device supports two slow rates,
*   when each scheduler sub-flow might be configured to each one of
*   the slow rates.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     slow_max_rates[FAP20M_SCH_NOF_SLOW_RATES] -
*       The rate in Kbit/s. 
*  INDIRECT:
*    None.
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
*    class_type - Configured class type configuration
*REMARKS:
*  The implementation calls the SHD function, because,
*   the converting is similar to that table and to the
*   max slow rate.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_slow_max_rate_unsafe
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
    unsigned int slow_max_rates[FAP20M_SCH_NOF_SLOW_RATES];
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    sch_rank = (FAP20M_SCH_RANK)inArgs[1];

    slow_max_rates[0] = (unsigned int)inFields[0];
    slow_max_rates[1] = (unsigned int)inFields[1];

    /* call Ocelot API function */
    result = fap20m_set_slow_max_rate_unsafe(device_id, sch_rank, slow_max_rates);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_slow_max_rate_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets the slow rate.
*   A flow might be in slow state, and in that case lower
*   rate is needed.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     *slow_max_rate -
*       The rate Kbit/s. 
*  INDIRECT:
*    None.
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
*    class_type - Configured class type configuration
*REMARKS:
*  The implementation calls the SHD function, because,
*   the converting is similar to that table and to the
*   max slow rate.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_slow_max_rate_unsafe
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
    unsigned int slow_max_rates[FAP20M_SCH_NOF_SLOW_RATES];
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
    result = fap20m_get_slow_max_rate_unsafe(device_id, sch_rank, slow_max_rates);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = slow_max_rates[0];
    inFields[1] = slow_max_rates[1];

    /* pack and output table fields */
    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_slow_max_rate_unsafe_end
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
*  fap20m_set_device_interface_rate
*TYPE:
*  PROC
*DATE: 
*  01-JUN-05
*FUNCTION:
*   This function sets a device interface maximum credit rate.
*   Is is called with a single interface (one of LINKA, LINKB
*   and CPU) register offset, shift and mask. It then calculates the interval
*   in clocks between 2 consecutive credits distributed to this interface
*   (using the device clock frequency and credit size) and writes it to the
*   device.
*INPUT:
*  DIRECT:
*    IN  unsigned int  device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*    IN  unsigned long offset -
*       Offset in the device memory (direct region)
*    IN  unsigned long shift -
*       start bit of the field in the register
*    IN  unsigned long mask -
*       the field mask
*    IN  unsigned long* rate -
*       The required interface rate in Mega-bits per second
*    OUT unsigned long* exact_rate -
*       loaded with the actual interface rate in Mega-bits per second. 
*       These can differ from the given values due to rounding. 
*  INDIRECT:
*    The device credit size
*    The device clock frequency
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
*    exact_rate
*REMARKS:
*  NONE.
*SEE ALSO:
*  fap20m_set_device_interface_rate_unsafe()
*****************************************************/
static CMD_STATUS wrFap20m_set_device_interface_rate
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
    unsigned long offset;
    unsigned long shift;
    unsigned long mask;
    unsigned long rate;
    unsigned long exact_rate;
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
    offset = (unsigned long)inArgs[2];
    shift = (unsigned long)inArgs[3];
    mask = (unsigned long)inArgs[4];
    rate = (unsigned long)inArgs[5];

    /* call Ocelot API function */
    result = fap20m_set_device_interface_rate(device_id, sch_rank, offset, shift, 
                                              mask, rate, &exact_rate);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", exact_rate);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_device_interface_rate_unsafe
*TYPE:
*  PROC
*DATE: 
*  01-JUN-05
*FUNCTION:
*   This function sets a device interface maximum credit rate.
*   Is is called with a single interface (one of LINKA, LINKB
*   and CPU) register offset, shift and mask. It then calculates the interval
*   in clocks between 2 consecutive credits distributed to this interface
*   (using the device clock frequency and credit size) and writes it to the
*   device.
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*    IN  unsigned int  device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*    IN  unsigned long offset -
*       Offset in the device memory (direct region)
*    IN  unsigned long shift -
*       start bit of the field in the register
*    IN  unsigned long mask -
*       the field mask
*    IN  unsigned long* rate -
*       The required interface rate in Mega-bits per second
*    OUT unsigned long* exact_rate -
*       loaded with the actual interface rate in Mega-bits per second. 
*       These can differ from the given values due to rounding. 
*  INDIRECT:
*    The device credit size
*    The device clock frequency
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
*    exact_rate
*REMARKS:
*  NONE.
*SEE ALSO:
*  fap20m_set_device_interface_rate()
*****************************************************/
static CMD_STATUS wrFap20m_set_device_interface_rate_unsafe
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
    unsigned long offset;
    unsigned long shift;
    unsigned long mask;
    unsigned long rate;
    unsigned long exact_rate;
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
    offset = (unsigned long)inArgs[2];
    shift = (unsigned long)inArgs[3];
    mask = (unsigned long)inArgs[4];
    rate = (unsigned long)inArgs[5];

    /* call Ocelot API function */
    result = fap20m_set_device_interface_rate_unsafe(device_id, sch_rank, offset,
                                                     shift, mask, rate, &exact_rate);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", exact_rate);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_device_interface_rate
*TYPE:
*  PROC
*DATE: 
*  01-JUN-05
*FUNCTION:
*   This function gets a device interface maximum credit rate.
*   Is is called with a single interface (one of LINKA, LINKB
*   and CPU) register offset, shift and mask. It then reads the interval
*   in clocks between 2 consecutive credits distributed to this interface
*   by the scheduler, and translates it (using the device clock frequency 
*   and credit size) to credit rate in Mega-bits per second.
*INPUT:
*  DIRECT:
*    IN  unsigned int  device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*    IN  unsigned long offset -
*       Offset in the device memory (direct region)
*    IN  unsigned long shift -
*       start bit of the field in the register
*    IN  unsigned long mask -
*       the field mask
*    OUT unsigned long* rate -
*       loaded with the interface rate in Mega-bits per second
*  INDIRECT:
*    The device credit size
*    The device clock frequency
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
*    rate
*REMARKS:
*  NONE.
*SEE ALSO:
*  fap20m_get_device_interface_rate_unsafe()
*****************************************************/
static CMD_STATUS wrFap20m_get_device_interface_rate
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
    unsigned long offset;
    unsigned long shift;
    unsigned long mask;
    unsigned long rate;
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
    offset = (unsigned long)inArgs[2];
    shift = (unsigned long)inArgs[3];
    mask = (unsigned long)inArgs[4];

    /* call Ocelot API function */
    result = fap20m_get_device_interface_rate(device_id, sch_rank, offset, shift, mask, &rate);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", rate);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_device_interface_rate_unsafe
*TYPE:
*  PROC
*DATE: 
*  01-JUN-05
*FUNCTION:
*   This function gets a device interface maximum credit rate.
*   Is is called with a single interface (one of LINKA, LINKB 
*   and CPU) register offset, shift and mask. It then reads the interval
*   in clocks between 2 consecutive credits distributed to this interface
*   by the scheduler, and translates it (using the device clock frequency 
*   and credit size) to credit rate in Mega-bits per second.
*   This is the UNSAFE version of the function, meaning that it must be 
*   called from within a SAFE context (Driver and Device were checked, 
*   and the device semaphore was acquired successfully).
*INPUT:
*  DIRECT:
*    IN  unsigned int  device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*    IN  unsigned long offset -
*       Offset in the device memory (direct region)
*    IN  unsigned long shift -
*       start bit of the field in the register
*    IN  unsigned long mask -
*       the field mask
*    OUT unsigned long* rate -
*       loaded with the interface rate in Mega-bits per second
*  INDIRECT:
*    The device credit size
*    The device clock frequency
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
*    rate
*REMARKS:
*  NONE.
*SEE ALSO:
*  fap20m_get_device_interface_rate()
*****************************************************/
static CMD_STATUS wrFap20m_get_device_interface_rate_unsafe
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
    unsigned long offset;
    unsigned long shift;
    unsigned long mask;
    unsigned long rate;
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
    offset = (unsigned long)inArgs[2];
    shift = (unsigned long)inArgs[3];
    mask = (unsigned long)inArgs[4];

    /* call Ocelot API function */
    result = fap20m_get_device_interface_rate_unsafe(device_id, sch_rank, offset, shift, mask, &rate);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", rate);

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_get_elementary_scheduler_flow_group
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
    FAP20M_ELEMENTARY_SCHEDULER_ID sched_id;
    FAP20M_FLOW_GROUP group;
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
    sched_id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_get_elementary_scheduler_flow_group(device_id, sch_rank, sched_id, &group);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", group);

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_get_elementary_scheduler_state
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
    FAP20M_ELEMENTARY_SCHEDULER_ID sched_id;
    unsigned int is_sched_enabled;
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
    sched_id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_get_elementary_scheduler_state(device_id, sch_rank, sched_id, &is_sched_enabled);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", is_sched_enabled);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_scheduler_flow_nof_subflows
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function gets the number of sub flows from the FSF table. 
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     scheduler_flow_id -
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*       The flow ID. Range from 0 to 16383 (16K).
*  INDIRECT:
*    None.
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
*    OUT    unsigned long                *nof_subflows -
*       return with the number of currently active sub flows.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_get_scheduler_flow_nof_subflows
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
    FAP20M_FLOW_ID base_flow_id;
    unsigned long nof_subflows;
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
    base_flow_id = (FAP20M_FLOW_ID)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_get_scheduler_flow_nof_subflows(device_id, sch_rank, 
                                                    base_flow_id, &nof_subflows);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", nof_subflows);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_is_sch_flow_dynamic_used
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
* The function fap20m_is_scheduler_flow_dynamically_used()
* read the internal dynamic DB, and make sure that the 
* flow/aggregate is not dynamically used,
* and therefore, might be modified.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     flow_id -
*       The ID of the Flow. Range from 65 to 16K.
*   unsigned int                     nof_subflows -
*       The number of sub-flows that the application plan to
*         use with the above flow ID. Range from 0 or 1.
*   unsigned int                      *is_dynamically_used
*       Is the flow ID, or one of its subflows are used by
*        the internal device's DB, and therefore shouldn't
*        be used by the application
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*REMARKS:
*  None.
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_is_sch_flow_dynamic_used
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
    unsigned int flow_id;
    unsigned int nof_subflows;
    unsigned int is_dynamically_used;
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
    flow_id = (unsigned int)inArgs[2];
    nof_subflows = (unsigned int)inArgs[3];

    /* call Ocelot API function */
    result = fap20m_is_sch_flow_dynamic_used(device_id, sch_rank, flow_id, 
                                             nof_subflows, &is_dynamically_used);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", is_dynamically_used);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_is_sch_port_dynamic_used
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*  The function fap20m_is_sch_port_dynamic_used()
*  read the internal dynamic DB, and make sure that the 
*  port is not dynamically used,
*  and therefore, might be modified.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     port_id     -
*       The Port ID that the application plan on using. Range 0 - 64
*   unsigned int                     *is_dynamically_used
*       Is the resource currently free.
*  INDIRECT:
*    None.
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
*    scheduler - Configured port scheduler
*REMARKS:
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_is_sch_port_dynamic_used
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
    unsigned int is_dynamically_used;
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
    port_id = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_is_sch_port_dynamic_used(device_id, sch_rank, port_id, &is_dynamically_used);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", is_dynamically_used);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_sch_port_flow_control_get_unsafe
*TYPE:
*  PROC
*DATE: 
*  12-JULY-07
*FUNCTION:
*   This function gets if emulated flow control is enabled
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   FAP20M_SCH_RANK                  sch_rank -
*       Rank of scheduler (primary or secondary)
*   unsigned int                     port -
*     Which port to enable. If port 'port' is asserted 
*     then it emulates a situation that fc is received from the
*     egress for port 'port'.
*   unsigned int                     *enable -
*     Enable or disable emulation of port 'port'
*  INDIRECT:
*    None.
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
*  None.
*REMARKS:
*  None
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_sch_port_flow_control_get_unsafe
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
    unsigned int port;
    unsigned int enable;
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
    port = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_sch_port_flow_control_get_unsafe(device_id, sch_rank, port, &enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", enable);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_sch_get_sch_rank_of_egress
*TYPE:
*  PROC
*DATE: 
*  05-JUN-07
*FUNCTION:
*   There are two schedulers in the Fabric Output Processor a 
*   Primary Scheduler and a Secondary Scheduler. 
*   Each of the two schedulers Primary and Secondary can serve for 
*   each of the two - Fabric and Egress traffic Manager. 
*   This function check which of the schedulers is used by the egress.
*
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*    FAP20M_SCH_RANK  sch_rank    -
*       Which of the scheduler (Primary or Secondary) is used by the egress.
*  INDIRECT:
*   None
*REMARKS:
*   None
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_sch_get_sch_rank_of_egress
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
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_sch_get_sch_rank_of_egress(device_id, &sch_rank);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "%d", sch_rank);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_sch_get_sch_rank_of_fabric
*TYPE:
*  PROC
*DATE: 
*  05-JUN-07
*FUNCTION:
*   There are two schedulers in the Fabric Output Processor a 
*   Primary Scheduler and a Secondary Scheduler. 
*   Each of the two schedulers Primary and Secondary can serve for 
*   each of the two - Fabric and Egress traffic Manager. 
*   This function check which of the schedulers is used by the fabric.
*
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*    FAP20M_SCH_RANK  sch_rank    -
*       Which of the scheduler (Primary or Secondary) is used by the fabric.
*  INDIRECT:
*   None
*REMARKS:
*   None
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_sch_get_sch_rank_of_fabric
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
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_sch_get_sch_rank_of_fabric(device_id, &sch_rank);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", sch_rank);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_sch_port_flow_control_set_unsafe
*TYPE:
*  PROC
*DATE: 
*  12-JULY-07
*FUNCTION:
*   This function sets emulated flow control
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   FAP20M_SCH_RANK                  sch_rank -
*       Rank of scheduler (primary or secondary)
*   unsigned int                     port -
*     Which port to enable. If port 'port' is asserted 
*     then it emulates a situation that fc is received from the
*     egress for port 'port'.
*   unsigned int                     enable -
*     Enable or disable emulation of port 'port'
*  INDIRECT:
*    None.
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
*  None.
*REMARKS:
*  None
*SEE ALSO:
*  None.
*****************************************************/
static CMD_STATUS wrFap20m_sch_port_flow_control_set_unsafe
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
    unsigned int port;
    unsigned int enable;
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
    port = (unsigned int)inArgs[2];
    enable = (unsigned int)inArgs[3];

    /* call Ocelot API function */
    result = fap20m_sch_port_flow_control_set_unsafe(device_id, sch_rank, port, enable);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_set_elementary_scheduler_flow_group
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
    FAP20M_ELEMENTARY_SCHEDULER_ID sched_id;
    FAP20M_FLOW_GROUP group;
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
    sched_id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inArgs[2];
    group = (FAP20M_FLOW_GROUP)inArgs[3];

    /* call Ocelot API function */
    result = fap20m_set_elementary_scheduler_flow_group(device_id, sch_rank, sched_id, group);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*   There is no function's description
*****************************************************/
static CMD_STATUS wrFap20m_set_elementary_scheduler_state
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
    FAP20M_ELEMENTARY_SCHEDULER_ID sched_id;
    unsigned int is_sched_enabled;
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
    sched_id = (FAP20M_ELEMENTARY_SCHEDULER_ID)inArgs[2];
    is_sched_enabled = (unsigned int)inArgs[3];

    /* call Ocelot API function */
    result = fap20m_set_elementary_scheduler_state(device_id, sch_rank, sched_id, is_sched_enabled);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_update_scheduler_flow_unsafe
*TYPE:
*  PROC
*DATE: 
*  08-APR-05
*FUNCTION:
*   This function update flow/aggregate atributes, that might be
*   updated when traffic is running.
*INPUT:
*  DIRECT:
*   unsigned int                     device_id   -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*   IN  FAP20M_SCH_RANK         sch_rank    -
*       Which scheduler to set .Primary or Secondary?
*   unsigned int                     flow_id -
*        Flow identifier.  Range from 0 to 16385 (16K).
*         might also be done for aggregate.
*   unsigned int                     sub_flow_id
*        Sub Flow identifier.  Range from 0 to 16385 (16K).
*        The first sub flow will have the ID of the flow,
*         and the second one (if exist), will be flow ID + 1. 
*   FAP20M_FLOW_UPDATE_TYPE        update_type       -
*       The flow attribute that need to be changed.
*   unsigned long                      update_value    -
*       The new value that need to be written to the attribute.
*   unsigned long                      *exact_value
*       The new value that was actually written to the attribute.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*REMARKS:
*  Also valid for Aggregate!!.
*****************************************************/
static CMD_STATUS wrFap20m_update_scheduler_flow_unsafe
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
    unsigned int flow_id;
    unsigned int sub_flow_id;
    FAP20M_FLOW_UPDATE_TYPE update_type;
    unsigned long update_value;
    unsigned long exact_value;
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
    flow_id = (unsigned int)inArgs[2];
    sub_flow_id = (unsigned int)inArgs[3];
    update_type = (FAP20M_FLOW_UPDATE_TYPE)inArgs[4];
    update_value = (unsigned long)inArgs[5];

    /* call Ocelot API function */
    result = fap20m_update_scheduler_flow_unsafe(device_id, sch_rank, flow_id, 
                                                 sub_flow_id, update_type, 
                                                 update_value, &exact_value);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", exact_value);

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"set_primary_scheduler_role",
         &wrFap20m_set_primary_scheduler_role,
         2, 0},
        {"get_primary_scheduler_role",
         &wrFap20m_get_primary_scheduler_role,
         1, 0},

        {"device_rate_entrySet",
         &wrFap20m_set_device_rate_entry,
         2, 3},
        {"device_rate_entryGetFirst",
         &wrFap20m_get_device_rate_entry_first,
         2, 0},
        {"device_rate_entryGetNext",
         &wrFap20m_get_device_rate_entry_next,
         2, 0},

        {"slow_max_rateSet",
         &wrFap20m_set_slow_max_rate,
         2, 2},
        {"slow_max_rateGetFirst",
         &wrFap20m_get_slow_max_rate,
         2, 0},
        {"slow_max_rateGetNext",
         &wrFap20m_get_slow_max_rate_end,
         2, 0},
        {"device_interfaces_schedulerSet",
         &wrFap20m_set_device_interfaces_scheduler,
         2, 6},
        {"device_interfaces_schedulerGetFirst",
         &wrFap20m_get_device_interfaces_scheduler,
         2, 0},
        {"device_interfaces_schedulerGetNext",
         &wrFap20m_get_device_interfaces_scheduler_end,
         2, 0},

        {"device_scheduler_fill_device_rate_tableSet",
         &wrFap20m_device_scheduler_fill_device_rate_table,
         2, 3},

        {"device_schedulerSetFirst",
         &wrFap20m_set_first_device_scheduler,
         9, 3},
        {"device_schedulerSetNext",
         &wrFap20m_set_next_device_scheduler,
         9, 3},
        {"device_schedulerEndSet",
         &wrFap20m_end_set_device_scheduler,
         2, 0}, 
        {"device_schedulerGetFirst",
         &wrFap20m_get_device_scheduler,
         9, 0},
        {"device_schedulerGetNext",
         &wrFap20m_get_device_scheduler_next,
         9, 0},

        {"set_HR_scheduler_modes",
         &wrFap20m_set_HR_scheduler_modes,
         3, 0},
        {"get_HR_scheduler_modes",
         &wrFap20m_get_HR_scheduler_modes,
         2, 0},

        {"scheduler_class_type_paramsSet",
         &wrFap20m_set_scheduler_class_type_params,
         2, 7},
        {"scheduler_class_type_paramsGetFirst",
         &wrFap20m_get_scheduler_class_type_params,
         2, 0},
        {"scheduler_class_type_paramsGetNext",
         &wrFap20m_get_scheduler_class_type_params_end,
         2, 0},

        {"scheduler_class_type_params_fill_dbSet",
         &wrFap20m_scheduler_class_type_params_fill_db,
         2, 0},
        {"scheduler_class_type_params_tableSet",
         &wrFap20m_set_scheduler_class_type_params_table,
         2, 0},
        {"scheduler_class_type_params_tableGetFirst",
         &wrFap20m_get_scheduler_class_type_params_table,
         2, 0},
        {"scheduler_class_type_params_tableGetNext",
         &wrFap20m_get_scheduler_class_type_params_table_end,
         2, 0},

        {"scheduler_portSet",
         &wrFap20m_set_scheduler_port,
         2, 4},
        {"scheduler_portGetFirst",
         &wrFap20m_get_scheduler_port_first,
         5, 0},
        {"scheduler_portGetNext",
         &wrFap20m_get_scheduler_port_next,
         5, 0},

        {"is_scheduler_port_dynamically_used",
         &wrFap20m_is_scheduler_port_dynamically_used,
         3, 0},

        {"scheduler_aggregateSet",
         &wrFap20m_set_scheduler_aggregate,
         2, 26},
        {"scheduler_aggregateGetFirst",
         &wrFap20m_get_scheduler_aggregate,
         2, 0},
        {"scheduler_aggregateGetNext",
         &wrFap20m_get_scheduler_aggregate_end,
         2, 0},

        {"scheduler_flowSet",
         &wrFap20m_set_scheduler_flow,
         5, 23},
        {"scheduler_flowGetFirst",
         &wrFap20m_get_scheduler_flow_first,
         5, 0},
        {"scheduler_flowGetNext",
         &wrFap20m_get_scheduler_flow_next,
         5, 0},
        {"scheduler_flowDelete",
         &wrFap20m_del_scheduler_flow,
         5, 0},

        {"update_scheduler_flow",
         &wrFap20m_update_scheduler_flow,
         6, 0},

        {"is_scheduler_flow_dynamically_used",
         &wrFap20m_is_scheduler_flow_dynamically_used,
         4, 0},
        
        {"sch_port_flow_control_set",
         &wrFap20m_sch_port_flow_control_set,
         4, 0},
        {"sch_port_flow_control_get",
         &wrFap20m_sch_port_flow_control_get,
         3, 0},

        {"device_rate_entry_unsafeSet",
         &wrFap20m_set_device_rate_entry_unsafe,
         2, 3},
        {"device_rate_entry_unsafeGetFirst",
         &wrFap20m_get_device_rate_entry_unsafe,
         2, 0},
        {"device_rate_entry_unsafeGetNext",
         &wrFap20m_get_device_rate_entry_unsafe_end,
         2, 0},

        {"elementary_scheduler_unsafeSet",
         &wrFap20m_set_elementary_scheduler_unsafe,
         3, 4},
        {"elementary_scheduler_unsafeGetFirst",
         &wrFap20m_get_elementary_scheduler_unsafe,
         2, 0},
        {"elementary_scheduler_unsafeGetNext",
         &wrFap20m_get_elementary_scheduler_unsafe_end,
         2, 0},

        {"scheduler_aggregate_unsafeSet",
         &wrFap20m_set_scheduler_aggregate_unsafe,
         2, 26},
        {"scheduler_aggregate_unsafeGetFirst",
         &wrFap20m_get_scheduler_aggregate_unsafe,
         2, 0},
        {"scheduler_aggregate_unsafeGetNext",
         &wrFap20m_get_scheduler_aggregate_unsafe_end,
         2, 0},
        
        {"scheduler_class_type_unsafeSet",
         &wrFap20m_set_scheduler_class_type_unsafe,
         2, 7},
        {"scheduler_class_type_unsafeGetFirst",
         &wrFap20m_get_scheduler_class_type_unsafe,
         2, 0},
        {"scheduler_class_type_unsafeGetNext",
         &wrFap20m_get_scheduler_class_type_unsafe_end,
         2, 0},

        {"scheduler_flow_unsafeSet",
         &wrFap20m_set_scheduler_flow_unsafe,
         4, 21},
        {"scheduler_flow_unsafeGetFirst",
         &wrFap20m_get_scheduler_flow_unsafe,
         2, 0},
        {"scheduler_flow_unsafeGetNext",
         &wrFap20m_get_scheduler_flow_unsafe_end,
         2, 0},
        {"scheduler_flow_unsafeDelete",
         &wrFap20m_del_scheduler_flow_unsafe,
         4, 0},

        {"scheduler_port_unsafeSet",
         &wrFap20m_set_scheduler_port_unsafe,
         2, 4},
        {"scheduler_port_unsafeGetFirst",
         &wrFap20m_get_scheduler_port_unsafe,
         2, 0},
        {"scheduler_port_unsafeGetNext",
         &wrFap20m_get_scheduler_port_unsafe_end,
         2, 0},

        {"slow_max_rate_unsafeSet",
         &wrFap20m_set_slow_max_rate_unsafe,
         2, 2},
        {"slow_max_rate_unsafeGetFirst",
         &wrFap20m_get_slow_max_rate_unsafe,
         2, 0},
        {"slow_max_rate_unsafeGetNext",
         &wrFap20m_get_slow_max_rate_unsafe_end,
         2, 0},

        {"set_device_interface_rate",
         &wrFap20m_set_device_interface_rate,
         6, 0},
        {"get_device_interface_rate",
         &wrFap20m_get_device_interface_rate,
         5, 0},
        {"set_device_interface_rate_unsafe",
         &wrFap20m_set_device_interface_rate_unsafe,
         6, 0},
        {"get_device_interface_rate_unsafe",
         &wrFap20m_get_device_interface_rate_unsafe,
         5, 0},

        {"get_elementary_scheduler_flow_group",
         &wrFap20m_get_elementary_scheduler_flow_group,
         3, 0},
        {"set_elementary_scheduler_flow_group",
         &wrFap20m_set_elementary_scheduler_flow_group,
         4, 0},
        {"get_elementary_scheduler_state",
         &wrFap20m_get_elementary_scheduler_state,
         3, 0},
        {"set_elementary_scheduler_state",
         &wrFap20m_set_elementary_scheduler_state,
         4, 0},
        {"get_scheduler_flow_nof_subflows",
         &wrFap20m_get_scheduler_flow_nof_subflows,
         3, 0},
        {"is_sch_flow_dynamic_used",
         &wrFap20m_is_sch_flow_dynamic_used,
         4, 0},
        {"is_sch_port_dynamic_used",
         &wrFap20m_is_sch_port_dynamic_used,
         3, 0},
        {"port_flow_control_get_unsafe",
         &wrFap20m_sch_port_flow_control_get_unsafe,
         3, 0},
        {"sch_port_flow_control_set_unsafe",
         &wrFap20m_sch_port_flow_control_set_unsafe,
         4, 0},
        {"sch_get_sch_rank_of_egress",
         &wrFap20m_sch_get_sch_rank_of_egress,
         1, 0},
        {"sch_get_sch_rank_of_fabric",
         &wrFap20m_sch_get_sch_rank_of_fabric,
         1, 0},
        {"update_scheduler_flow_unsafe",
         &wrFap20m_update_scheduler_flow_unsafe,
         6, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_scheduler
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
GT_STATUS cmdLibInitFap20m_api_scheduler
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

