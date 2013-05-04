/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_interrupt_services.c
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
#include <FX950/include/fap20m_interrupt_services.h>

/*****************************************************
*NAME
* 
*  fap20m_interrupt_services_sw_db_load
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  loads this module's global variables with data passed by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_INTERRUPT_SERVICES_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_interrupt_services_sw_db_load
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    FAP20M_INTERRUPT_SERVICES_SW_DB sw_db_module_buff;
    unsigned short rc;
    unsigned int fieldNum, i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    fieldNum = 0;

    /* map input arguments to locals */
    for(i = 0; i < SAND_MAX_DEVICE; i++)
        sw_db_module_buff.fap20m_interrupt_mask_on[i] = (unsigned char)inFields[fieldNum++];

    for(i = 0; i < SAND_MAX_DEVICE; i++)
        sw_db_module_buff.fap20m_interrupt_mask_copy[i] = (unsigned long)inFields[fieldNum++];

    for(i = 0; i < SIZE_OF_BITSTRAEM_IN_LONGS; i++)
        sw_db_module_buff.fap20m_interrupt_non_autoclear[i] = (unsigned long)inFields[fieldNum++];

    /* call Ocelot API function */
    result = fap20m_interrupt_services_sw_db_load(&sw_db_module_buff);

    /* pack output arguments to galtis string */
    rc = sand_get_error_code_from_error_word(result);

    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
* 
*  fap20m_interrupt_services_sw_db_save
*TYPE:
*  PROC
*DATE: 
*  19/02/2006
*FUNCTION:
*  saves this module's global variables to the buffer supplied by the SSR mechanism
*INPUT:
*  DIRECT:
*    FAP20M_INTERRUPT_SERVICES_SW_DB* sw_db_module_buff -
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
static CMD_STATUS wrFap20m_interrupt_services_sw_db_save
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    FAP20M_INTERRUPT_SERVICES_SW_DB sw_db_module_buff;
    unsigned short rc;
    unsigned int fieldNum, i;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* call Ocelot API function */
    result = fap20m_interrupt_services_sw_db_save(&sw_db_module_buff);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    fieldNum = 0;

    /* pack and output table fields */
    for(i = 0; i < SAND_MAX_DEVICE; i++)
        inFields[fieldNum++] = sw_db_module_buff.fap20m_interrupt_mask_on[i];

    for(i = 0; i < SAND_MAX_DEVICE; i++)
        inFields[fieldNum++] = sw_db_module_buff.fap20m_interrupt_mask_copy[i];

    for(i = 0; i < SIZE_OF_BITSTRAEM_IN_LONGS; i++)
        inFields[fieldNum++] = sw_db_module_buff.fap20m_interrupt_non_autoclear[i];

    fieldOutput("%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
                        inFields[0],  inFields[1],  inFields[2],  inFields[3],  inFields[4],  inFields[5],
                        inFields[6],  inFields[7],  inFields[8],  inFields[9],  inFields[10], inFields[11],
                        inFields[12], inFields[13], inFields[14], inFields[15], inFields[16], inFields[17],
                        inFields[18], inFields[19], inFields[20], inFields[21], inFields[22], inFields[23]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_interrupt_services_sw_db_save_end
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
        {"interrupt_services_sw_dbSet",
         &wrFap20m_interrupt_services_sw_db_load,
         0, (SAND_MAX_DEVICE * 2 + SIZE_OF_BITSTRAEM_IN_LONGS)},
        {"interrupt_services_sw_dbGetFirst",
         &wrFap20m_interrupt_services_sw_db_save,
         0, 0},
        {"interrupt_services_sw_dbGetNext",
         &wrFap20m_interrupt_services_sw_db_save_end,
         0, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_interrupt_services
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
GT_STATUS cmdLibInitFap20m_interrupt_services
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

