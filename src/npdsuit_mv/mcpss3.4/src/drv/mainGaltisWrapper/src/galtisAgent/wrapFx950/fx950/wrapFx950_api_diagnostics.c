/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFx950_api_diagnostics.c
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
#include <FX950/include/fx950_api_diagnostics.h>


static GT_U32 tbl_index;
static FX950_DIAGNOSTICS_PRBS_SETUP_STRUCT prbs_setup;
static FX950_DIAGNOSTICS_PRBS_COUNTERS_REPORT_STRUCT prbs_counters;

/*****************************************************
*NAME
*  fx950_api_diagnostics_set_prbs_mode
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure sets the mode of PRBS channels: Which links to activate and 
*  whether to activate generator or checker.
*  Checker/Generator are independent of each other.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_DIAGNOSTICS_PRBS_SETUP_STRUCT    *prbs_setup_ptr - 
*      Pointer to buffer of PRBS channels configuration.
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
static CMD_STATUS wrFx950_api_diagnostics_set_prbs_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int index;
    FX950_DIAGNOSTICS_PRBS_SETUP_STRUCT prbs_setup_ptr;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* initialize prbs_unit array */
    for (index = 0; index < FX950_DIAGNOSTICS_PRBS_NOF_UNITS; index++) 
    {
        prbs_setup_ptr.prbs_generator_checker_unit[index].valid = 0;
    }

    index = (unsigned int) inFields[0];
    prbs_setup_ptr.prbs_generator_checker_unit[index].prbs_test_mode_enable = (unsigned long)inFields[1];
    prbs_setup_ptr.prbs_generator_checker_unit[index].prbs_test_mode = (FX950_DIAGNOSTICS_PRBS_TEST_MODE)inFields[2];
    prbs_setup_ptr.prbs_generator_checker_unit[index].activate_prbs_generator = (unsigned long)inFields[3];
    prbs_setup_ptr.prbs_generator_checker_unit[index].activate_prbs_checker = (unsigned long)inFields[4];
    prbs_setup_ptr.prbs_generator_checker_unit[index].valid = (unsigned long)inFields[5];

    /* call Ocelot API function */
    result = fx950_api_diagnostics_set_prbs_mode(device_id, &prbs_setup_ptr);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_diagnostics_get_prbs_mode
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets information on mode of PRBS channels: Which links were 
*  activated and whether they were activated as generators or checkers.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FX950_DIAGNOSTICS_PRBS_SETUP_STRUCT    *prbs_setup_ptr - 
*      Pointer to buffer of PRBS channels configuration.
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
*    *prbs_setup_ptr - 
*      Pointer to buffer of PRBS channels configuration.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_diagnostics_get_prbs_mode
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
    unsigned int index;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];


    /* configure prbs_unit array to get data from all units */
    for (index = 0; index < FX950_DIAGNOSTICS_PRBS_NOF_UNITS; index++) 
    {
        prbs_setup.prbs_generator_checker_unit[index].valid = 1;
    }

    /* call Ocelot API function */
    result = fx950_api_diagnostics_get_prbs_mode(device_id, &prbs_setup);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;

    inFields[1] = prbs_setup.prbs_generator_checker_unit[tbl_index].prbs_test_mode_enable;
    inFields[2] = prbs_setup.prbs_generator_checker_unit[tbl_index].prbs_test_mode;
    inFields[3] = prbs_setup.prbs_generator_checker_unit[tbl_index].activate_prbs_generator;
    inFields[4] = prbs_setup.prbs_generator_checker_unit[tbl_index].activate_prbs_checker;
    inFields[5] = prbs_setup.prbs_generator_checker_unit[tbl_index].valid;
    
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);
    
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFx950_api_diagnostics_get_prbs_mode_end
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

    if (++tbl_index >= FX950_DIAGNOSTICS_PRBS_NOF_UNITS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = tbl_index;

    inFields[1] = prbs_setup.prbs_generator_checker_unit[tbl_index].prbs_test_mode_enable;
    inFields[2] = prbs_setup.prbs_generator_checker_unit[tbl_index].prbs_test_mode;
    inFields[3] = prbs_setup.prbs_generator_checker_unit[tbl_index].activate_prbs_generator;
    inFields[4] = prbs_setup.prbs_generator_checker_unit[tbl_index].activate_prbs_checker;
    inFields[5] = prbs_setup.prbs_generator_checker_unit[tbl_index].valid;
    
    /* pack and output table fields */
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);
    
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fx950_api_diagnostics_get_prbs_counters
*TYPE:
*  PROC
*DATE:
*  02/OCT/2007
*FUNCTION:
*  This procedure gets information of PRBS counters for each of the channels.
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
*    *prbs_counters_ptr - 
*      Pointer to buffer of PRBS counter's information.
*REMARKS:
*  Device must have been registered for this action to be
*  carried out properly.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFx950_api_diagnostics_get_prbs_counters
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

    /* call Ocelot API function */
    result = fx950_api_diagnostics_get_prbs_counters(device_id, &prbs_counters);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    tbl_index = 0;

    /* pack output arguments to galtis string */
    inFields[0] = prbs_counters.num_active_counters;

    inFields[1] = prbs_counters.prbs_counter[tbl_index].prbs_checker_link;
    inFields[2] = prbs_counters.prbs_counter[tbl_index].prbs_checker_locked;
    inFields[3] = prbs_counters.prbs_counter[tbl_index].prbs_error_counter;
    
    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);
    
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/*****************************************************/
static CMD_STATUS wrFx950_api_diagnostics_get_prbs_counters_end
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

    if (++tbl_index >= FX950_DIAGNOSTICS_PRBS_NOF_UNITS)
    {
        galtisOutput(outArgs, GT_OK, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = prbs_counters.num_active_counters;

    inFields[1] = prbs_counters.prbs_counter[tbl_index].prbs_checker_link;
    inFields[2] = prbs_counters.prbs_counter[tbl_index].prbs_checker_locked;
    inFields[3] = prbs_counters.prbs_counter[tbl_index].prbs_error_counter;
    
    /* pack and output table fields */
    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);
    
    galtisOutput(outArgs, GT_OK, "%f");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"diagnostics_prbs_modeSet",
         &wrFx950_api_diagnostics_set_prbs_mode,
         1, 5},
        {"diagnostics_prbs_modeGetFirst",
         &wrFx950_api_diagnostics_get_prbs_mode,
         1, 0},
        {"diagnostics_prbs_modeGetNext",
         &wrFx950_api_diagnostics_get_prbs_mode_end,
         1, 0},
        {"diagnostics_prbs_countersGetFirst",
         &wrFx950_api_diagnostics_get_prbs_counters,
         1, 0},
        {"diagnostics_prbs_countersGetNext",
         &wrFx950_api_diagnostics_get_prbs_counters_end,
         1, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFx950_api_diagnostics
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
CMD_STATUS cmdLibInitFx950_api_diagnostics
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

