/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_configuration.c
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
#include <FX950/include/fap20m_api_configuration.h>

/*****************************************************
*NAME
*  fap20m_set_dram_parameters
*TYPE:
*  PROC
*DATE: 
*  23-Jul-07
*FUNCTION:
*  This procedure sets the dram parameters -
*  writes to MMU and DRC registers.
*  DIRECT:
*    unsigned int             device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_DRC_CONFIGURATION drc_config -
*      User drc configuration to be set
*    unsigned int             init_drc_conf -
*      write configuration also to DRC registers 
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None
*      Otherwise, no error has been detected and device
*        has been read.
*  INDIRECT:
*    fap_id
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_dram_parameters
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_DRAM_CONFIGURATION dram_config;
    unsigned int init_drc_conf;
    unsigned int init_mmu_conf;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    init_drc_conf = (unsigned int)inArgs[1];
    init_mmu_conf = (unsigned int)inArgs[2];

    dram_config.nof_columns = (unsigned long)inFields[0];
    dram_config.auto_precharge_pos = (unsigned long)inFields[1];
    dram_config.enable_8bank_mode = (unsigned long)inFields[2];
    dram_config.burst_size_mode = (unsigned long)inFields[3];
    
    /* call Ocelot API function */
    result = fap20m_set_dram_parameters(device_id, dram_config, 
                                        init_drc_conf, init_mmu_conf);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_fap_id
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This procedure gets the device identifier
*  (as seen in the system).
*  The corresponding device register is: Identification.
*  DIRECT:
*    unsigned int  device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int* fap_id -
*      Loaded with the designated physical FAP id.
*      Range from 0 to 2047.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          None
*      Otherwise, no error has been detected and device
*        has been read.
*  INDIRECT:
*    fap_id
*REMARKS:
*
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_fap_id
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int fap_id;
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
    result = fap20m_get_fap_id(device_id, &fap_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", fap_id);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_fap_id
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This procedure sets the device with its identifier (note the
*  difference between the fap-id and the device-id.
*  The first is an id (number) within the fabric context.
*  The second is the driver software object).
*  The corresponding device registers are: Identification, SelfFAPID.
*INPUT:
*  DIRECT:
*    unsigned int device_id -
*      ISand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int fap_id -
*      The designated physical FAP id.
*      Range from 0 to 2047.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          FAP20M_FAP_OUT_OF_RANGE_ERR
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    NON
*REMARKS:
*  Note that, 'device_id' is parameter related to the 
*   inner device driver and have NO meaning in the chip.
*   'fap_id' on the other hand is the id of the device 
*   in the system, this is the fabric identifier.
*SEE ALSO:
* fap20m_get_id()
*****************************************************/
static CMD_STATUS wrFap20m_set_fap_id
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int fap_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    fap_id = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_set_fap_id(device_id, fap_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_profile_id
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure gets the identifier of current 'profile'
*  (the profile identifier loaded with 'fap20m_set_profile_id').
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    *result_ptr -
*      Loaded with profile Identifier.
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*            None
*      Otherwise, no error has been detected and device
*        has been read.
*  INDIRECT:
*    result_ptr
*REMARKS:
*  
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_profile_id
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long profile_id;
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
    result = fap20m_get_profile_id(device_id, &profile_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", profile_id);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_profile_id
*TYPE:
*  PROC
*DATE:
*  21/OCT/2002
*FUNCTION:
*  This procedure sets device with pre-defined profile.
*  A large set of registers is written to the device.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    profile_id -
*      The profile identifier, which the driver load the device.
*  INDIRECT:
*    None
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          FAP20M_SET_PROFILE_ID_001 -
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    Written object.
*REMARKS:
*  
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_profile_id
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long profile_id;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    profile_id = (unsigned long)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_set_profile_id(device_id, profile_id);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_profile_values
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This procedure read the profile values from the device.
*  A large set of register is read from the device.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*       Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_PROFILE*  profile -
*       Loaded with device values.
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
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_profile_values
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_PROFILE profile;
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
    result = fap20m_get_profile_values(device_id, &profile);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = profile.tbd;

    /* pack and output table fields */
    fieldOutput("%d", inFields[0]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_profile_values_end
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
*  fap20m_get_queue_type_params
*TYPE:
*  PROC
*DATE: 
*  29-MAR-05
*FUNCTION:
*  This procedure gets the queue parameters of 'queue_type'.
*  The driver write to table: Queue Type Parameter Table - QTP.
*INPUT:
*  DIRECT:
*    unsigned int                device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . 
*    FAP20M_QUEUE_TYPE           queue_type -
*       The queue type to get.
*    FAP20M_QUEUE_TYPE_PARAM*  queue_param -
*       Loaded with queue type parameters on the device.
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
*    queue_param
*REMARKS:
*  This is revB function. It should be used by owners of
*    fap20m_B0 chips. if you own fap20m_A chip, please
*    use the function fap20m_get_queue_type_params.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_queue_type_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_QUEUE_TYPE queue_type;
    FAP20M_QUEUE_TYPE_PARAM queue_param;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    queue_type = (FAP20M_QUEUE_TYPE)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_get_queue_type_params(device_id, queue_type, &queue_param);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = queue_param.RED_enable;
    inFields[1] = queue_param.RED_exp_weight;
    inFields[2] = queue_param.flow_slow_th_up;
    inFields[3] = queue_param.flow_stat_msg;
    inFields[4] = queue_param.max_q_size;
    inFields[5] = queue_param.flow_slow_hys_en;
    inFields[6] = queue_param.hysteresis_size;
    inFields[7] = queue_param.header_compensation;

    fieldOutput("%d%d%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3],
                                    inFields[4], inFields[5], inFields[6], inFields[7]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_queue_type_params_end
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
*  fap20m_set_queue_type_params
*TYPE:
*  PROC
*DATE: 
*  29-MAR-05
*FUNCTION:
*  This procedure sets the required queue type parameters.
*  The driver read from table: Queue Type Parameter Table - QTP.
*INPUT:
*  DIRECT:
*    unsigned int                device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_QUEUE_TYPE           queue_type -
*       The queue type to set.
*    FAP20M_QUEUE_TYPE_PARAM*  queue_params -
*       Pointer to parameters User requests to load into device.
*    FAP20M_QUEUE_TYPE_PARAM*  exact_queue_params_given -
*       Loaded with the exact parameters that were written to the
*       device (difference due to rounding). 
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP20M_NO_ERR then
*        specific error codes:
*          FAP20M_QUEUE_TYPE_OUT_OF_RANGE_ERR,
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    queue_param_out
*REMARKS:
*  This is revB function. It should be used by owners of
*    fap20m_B0 chips. if you own fap20m_A chip, please
*    use the function fap20m_set_queue_type_params.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_queue_type_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_QUEUE_TYPE queue_type;
    FAP20M_QUEUE_TYPE_PARAM  queue_params;
    FAP20M_QUEUE_TYPE_PARAM  exact_queue_params_given;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    queue_type = (FAP20M_QUEUE_TYPE)inArgs[1];

    queue_params.RED_enable = (unsigned int) inFields[0];
    queue_params.RED_exp_weight = (unsigned int) inFields[1];
    queue_params.flow_slow_th_up = (unsigned long) inFields[2];
    queue_params.flow_stat_msg = (unsigned long) inFields[3];
    queue_params.max_q_size = (unsigned long) inFields[4];
    queue_params.flow_slow_hys_en = (unsigned int) inFields[5];
    queue_params.hysteresis_size = (unsigned long) inFields[6];
    queue_params.header_compensation = (int) inFields[7];

    /* call Ocelot API function */
    result = fap20m_set_queue_type_params(device_id, queue_type, &queue_params, 
                                          &exact_queue_params_given);

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
*  fap20m_set_queues_priority
*TYPE:
*  PROC
*DATE: 
*  20-APR-05
*FUNCTION:
*  This procedure sets the priority (high/low) for
*  the 8K ingress queues. The ingress scheduler give strict 
*  priority to the high priority queues, that is, as long 
*  there are eligible high priority queues, they get access to the 
*  ingress memory. The 8K queues range is segmented to 128 segments 
*  of 64 contiguous queues, that is, queues 64N-64N+63 have the same priority.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    queus_segments_bitstream[FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE] -
*      A bitstream of queues segments. Bit N in queus_segments_bitstream maps 
*      to queue segment N, 0 < N < 127, which maps to the queue 
*      range 64*N to 64*N + 63.
*      If a bit is asserted, it indicates that the corresponding queue segment 
*      consists of high-priority queues.
*    unsigned int     flip_score_board -
*      Flip the score board - rows and coulombs are flipped - 
*      If flip_score_board==TRUE then instead of 64-64N+63 ranges, the same-priority
*      ranges are {N, 64+N, 128+N...} = {64k+N s.t. 0<= k <128}
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
*    Note that new mapping doesn't effect the MCQueues (0x2036-0x203a).
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_queues_priority
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long queus_segments_bitstream[FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE];
    unsigned int flip_score_board;
    unsigned short rc;
    unsigned short i;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    flip_score_board = (unsigned int)inArgs[1];

    /* FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE may be less then 6 */
    for(i = 0 ; (i < FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE) && (i < 6) ; i++ )
        queus_segments_bitstream[i] = (unsigned long)inFields[i];

    /* call Ocelot API function */
    result = fap20m_set_queues_priority(device_id, queus_segments_bitstream, 
                                        flip_score_board);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");
    
    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_queues_priority
*TYPE:
*  PROC
*DATE: 
*  20-APR-05
*FUNCTION:
*  This procedure gets the priority (high/low) for
*  the 8K ingress queues. The ingress scheduler give strict 
*  priority to the high priority queues, that is, as long 
*  there are eligible high priority queues, they get access to the 
*  ingress memory. The 8K queues range is segmented to 128 segments 
*  of 64 contiguous queues, that is, queues 64N-64N+63 have the same priority.
*INPUT:
*  DIRECT:
*    unsigned int     device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned long    queus_segments_bitstream[FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE] -
*      A bitstream of queues segments. Bit N in queus_segments_bitstream maps 
*      to queue segment N, 0 < N < 127, which maps to the queue 
*      range 64*N to 64*N + 63.
*      If a bit is asserted, it indicates that the corresponding queue segment 
*      consists of high-priority queues.
*    unsigned int     *flip_score_board -
*      Flip the score board - rows and coulombs are flipped - 
*      If flip_score_board==TRUE then instead of 64-64N+63 ranges, the same-priority
*      ranges are {N, 64+N, 128+N...} = {64k+N s.t. 0<= k <128}
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
*    queus_segments_bitmap
*REMARKS:
*    Note that new mapping doesn't effect the MCQueues (0x2036-0x203a).
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_queues_priority
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long queus_segments_bitstream[FAP20M_QUEUE_SEGMENTS_BITSTREAM_SIZE];
    unsigned int flip_score_board;
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
    result = fap20m_get_queues_priority(device_id, queus_segments_bitstream, &flip_score_board);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    inFields[0] = queus_segments_bitstream[0];
    inFields[1] = queus_segments_bitstream[1];
    inFields[2] = queus_segments_bitstream[2];
    inFields[3] = queus_segments_bitstream[3];
    inFields[4] = queus_segments_bitstream[4];
    inFields[5] = queus_segments_bitstream[5];
    

    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_queues_priority_end
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
*  fap20m_get_scrubber_params
*TYPE:
*  PROC
*DATE: 
*  03-APR-05
*FUNCTION:
*  This procedure gets the required scrubber parameters.
*INPUT:
*  DIRECT:
*    unsigned int                device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_SCRUBBER_PARAM*    scrubber_params -
*       Loaded with scrubber parameters on the device.
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
*    scrubber_params.
*REMARKS:
*  This is revB function. It should be used by owners of
*    fap20m_B0 chips.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_scrubber_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SCRUBBER_PARAM scrubber_params;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_get_scrubber_params(device_id, &scrubber_params);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = scrubber_params.enable;
    inFields[1] = scrubber_params.start_queue;
    inFields[2] = scrubber_params.end_queue;
    inFields[3] = scrubber_params.scan_rate;

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_scrubber_params_end
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
*  fap20m_set_scrubber_params
*TYPE:
*  PROC
*DATE: 
*  03-APR-05
*FUNCTION:
*  This procedure sets the required scrubber parameters.
*INPUT:
*  DIRECT:
*    unsigned int                device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_SCRUBBER_PARAM*    scrubber_params -
*       The scrubber parameters to set.
*    FAP20M_SCRUBBER_PARAM*    exact_scrubber_params -
*       Loaded with the exact parameters that were written to the
*       device (difference due to rounding). 
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
*    exact_scrubber_params.
*    Scrubber is configured.
*REMARKS:
*  This is revB function. It should be used by owners of
*    fap20m_B0 chips.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_scrubber_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_SCRUBBER_PARAM scrubber_params;
    FAP20M_SCRUBBER_PARAM exact_scrubber_params;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    scrubber_params.enable = (int)inFields[0];
    scrubber_params.start_queue = (unsigned long)inFields[1];
    scrubber_params.end_queue = (unsigned long)inFields[2];
    scrubber_params.scan_rate = (unsigned long)inFields[3];

    /* call Ocelot API function */
    result = fap20m_set_scrubber_params(device_id, &scrubber_params, 
                                        &exact_scrubber_params);

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
*  fap20m_get_backoff_params
*TYPE:
*  PROC
*DATE: 
*  03-APR-05
*FUNCTION:
*  This procedure gets the required backoff parameters.
*INPUT:
*  DIRECT:
*    unsigned int                device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_BACKOFF_PARAM*    backoff_params -
*       Loaded with backoff parameters on the device.
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
*    backoff_params.
*REMARKS:
*  This is revB function. It should be used by owners of
*    fap20m_B0 chips.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_backoff_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_BACKOFF_PARAM backoff_params;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_get_backoff_params(device_id, &backoff_params);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = backoff_params.upper_threshold;
    inFields[1] = backoff_params.lower_threshold;

    fieldOutput("%d%d", inFields[0], inFields[1]);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_backoff_params_end
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
*  fap20m_set_backoff_params
*TYPE:
*  PROC
*DATE: 
*  03-APR-05
*FUNCTION:
*  This procedure sets the required backoff parameters.
*INPUT:
*  DIRECT:
*    unsigned int                device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_BACKOFF_PARAM*    backoff_params -
*       The backoff parameters to set.
*    FAP20M_BACKOFF_PARAM*     exact_backoff_params
*       Loaded with the exact parameters that were written to the
*       device (difference due to rounding). 
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
*    Backoff is configured.
*REMARKS:
*  This is revB function. It should be used by owners of
*    fap20m_B0 chips.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_backoff_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_BACKOFF_PARAM backoff_params;
    FAP20M_BACKOFF_PARAM exact_backoff_params;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    backoff_params.upper_threshold = (unsigned long)inFields[0];
    backoff_params.lower_threshold = (unsigned long)inFields[1];

    /* call Ocelot API function */
    result = fap20m_set_backoff_params(device_id, &backoff_params, &exact_backoff_params);

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
*  fap20m_get_wred_params
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This procedure gets the WRED parameters of the specific
*  'queue_type' and 'drop_precedence'.
*  The driver write to table: Queue WRED Parameter Table - QWP.
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_QUEUE_TYPE   queue_type -
*      The queue type identifier of entry to get.
*    unsigned int        drop_precedence -
*      The drop precedence identifier of entry to get. Range: 0-3.
*    FAP20M_WRED_PARAM*  wred_param -
*      Loaded with the WRED parameters from the device.
*       Note: if 'min_avrg_th' & 'max_avrg_th' fields are the same,
*       no real WRED is activates. Since below all are admitted and 
*       above all are discarded.
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
*    wred_param
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_wred_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_QUEUE_TYPE queue_type;
    unsigned int drop_precedence;
    FAP20M_WRED_PARAM wred_param;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    queue_type = (FAP20M_QUEUE_TYPE)inArgs[1];
    drop_precedence = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_get_wred_params(device_id, queue_type, drop_precedence, &wred_param);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    inFields[0] = wred_param.min_avrg_th;
    inFields[1] = wred_param.max_avrg_th;
    inFields[2] = wred_param.max_packet_size.arr[0];
    inFields[3] = wred_param.max_packet_size.arr[1];
    inFields[4] = wred_param.max_probability;
    inFields[5] = wred_param.valid;
    
    fieldOutput("%d%d%d%d%d%d", inFields[0], inFields[1], inFields[2],
                                inFields[3], inFields[4], inFields[5]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_wred_params_end
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
*  fap20m_set_wred_params
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This procedure sets the WRED parameters, corresponding to
*  'queue_type' and 'drop_precedence'. After rounding them it
*  returns the actual values loaded to the device.
*  The driver read from table: Queue WRED Parameter Table - QWP.
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_QUEUE_TYPE   queue_type-
*       The queue type to set.
*    unsigned int        drop_precedence-
*       The drop precedence to set. Range: 0-3.
*    FAP20M_WRED_PARAM*  wred_param -
*       Requested WRED parameters. 
*    FAP20M_WRED_PARAM*  exact_wred_param_given -
*       Loaded with the actual WRED parameters.
*       (difference due to rounding).
*       Note: if 'min_avrg_th' & 'max_avrg_th' fields are the same,
*       no real WRED is activates. Since below all are admitted and 
*       above all are discarded.
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
*    exact_wred_param_given
*REMARKS:
*  wred_param_out.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_wred_params
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_QUEUE_TYPE queue_type;
    unsigned int drop_precedence;
    FAP20M_WRED_PARAM wred_param;
    FAP20M_WRED_PARAM exact_wred_param_given;
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    queue_type = (FAP20M_QUEUE_TYPE)inArgs[1];
    drop_precedence = (unsigned int)inArgs[2];

    wred_param.min_avrg_th = (unsigned long)inFields[0];
    wred_param.max_avrg_th = (unsigned long)inFields[1];
    wred_param.max_packet_size.arr[0] = (unsigned long)inFields[2];
    wred_param.max_packet_size.arr[1] = (unsigned long)inFields[3];
    wred_param.max_probability = (unsigned long)inFields[4];
    wred_param.valid = (unsigned int)inFields[5];

    /* call Ocelot API function */
    result = fap20m_set_wred_params(device_id, queue_type, drop_precedence, 
                                    &wred_param, &exact_wred_param_given);

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
*  fap20m_set_credit_size
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Set CrValue value to the device according to the credit byte worth.
*  **NOTE**
*  Setting of credit size should be done *before* setting of
*  SMS or PORT (fap20m_set_SMS_configuration(), fap20m_set_scheduler_port()).
*  Afterward, one should not change this number.
*INPUT:
*  DIRECT:
*    IN  unsigned int      device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    OUT SAND_CREDIT_SIZE  credit_byte_size -
*      Credit byte worth: 256, 512, 1024 or 2048.
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
*    NON
*REMARKS:
*  Should be set *before* setting of any SCH-port or SCH-flow.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_credit_size
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    SAND_CREDIT_SIZE credit_byte_size;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    credit_byte_size = (SAND_CREDIT_SIZE)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_set_credit_size(device_id, credit_byte_size);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_credit_size
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Read CrValue value from the device and calculate
*  and rerun credit byte worth.
*INPUT:
*  DIRECT:
*    IN  unsigned int      device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    OUT SAND_CREDIT_SIZE* credit_byte_size -
*      Loaded with device credit byte worth.
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
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_credit_size
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    SAND_CREDIT_SIZE credit_byte_size;
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
    result = fap20m_get_credit_size(device_id, &credit_byte_size);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", credit_byte_size);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_detect_if_single_fap_mode_required
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This function should be called before calling to to 
*   fap20m_set_single_fap_mode()
*  The value in 'single_fap_mode_required' indicates
*   on-line if the device is in single FAP mode.
*INPUT:
*  DIRECT:
*    unsigned int  device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int *single_fap_mode_required
*      Indicator.
*      1 - Device should be set to SINGLE-FAP mode.
*      0 - Device should NOT be set to SINGLE-FAP mode.
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
*    NON
*REMARKS:
*  fap20m_set_single_fap_mode() 
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_detect_if_single_fap_mode_required
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int single_fap_mode_required;
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
    result = fap20m_detect_if_single_fap_mode_required(device_id, &single_fap_mode_required);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", single_fap_mode_required);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_single_fap_mode
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Systems that consist only of single FAP
*  in a standalone mode, that switch data only among its ports,
*  require special configuration.
*  Use this function to set/unset this mode.
*  The fap20m_set_single_fap_mode() is to be used with no relation 
*  to the presence of FE in the system.
*  If the system is designed to be with ONE fap, set SINGLE-FAP mode.
*  If the system is designed to be with more than one FAP:
*  + If it start from ONE fap, set the FAP to SINGLE-FAP mode.
*    + When at least, one more FAP is in the system, set to NOT in SINGLE-FAP mode.
*  + If the system is with many FAPs, and all FAPs (but one) are removed, 
*    set the remaining FAP to be in SINGLE-FAP mode. 
*    (again, change it when another FAP is in)
*INPUT:
*  DIRECT:
*    unsigned int  device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int  single_fap_mode -
*      Indicator.
*      1 - Device is in SINGLE-FAP mode.
*      0 - Device is NOT in SINGLE-FAP mode.
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
*    NON
*REMARKS:
*  The call to fap20m_set_single_fap_mode() and 
*  fap20m_set_fabric_mode() are orthogonal.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_single_fap_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int single_fap_mode;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    single_fap_mode = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_set_single_fap_mode(device_id, single_fap_mode);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_single_fap_mode
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Systems that consist only of single FAP
*  in a standalone mode, that switch data only among its ports,
*  require special configuration.
*  Use this function to get device mode.
*INPUT:
*  DIRECT:
*    unsigned int  device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int* single_fap_mode -
*      Loaded with indicator:
*      1 - Device is in SINGLE-FAP mode.
*      0 - Device is NOT in SINGLE-FAP mode.
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
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_single_fap_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int single_fap_mode;
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
    result = fap20m_get_single_fap_mode(device_id, &single_fap_mode);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", single_fap_mode);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_fabric_mode
*TYPE:
*  PROC
*DATE: 
*  20-Sep-04
*FUNCTION:
*  Systems that consist only of 2,3,4,.. FAPs with no FE devices,
*  require special configuration.
*  Use this function to set/unset this mode.
*  Use fap20m_set_fabric_mode() in a system where there is no FE.
*  Setting of FAP20M_FABRIC_MODE_BACK2BACK/FAP20M_FABRIC_MODE_MESH mode,
*  should be done in system where no FE is present as a
*  design of the system and not as temporary state. 
*  User should set it once and no need to change it as FAPs 
*  are out and on the system.
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_FABRIC_MODE  fabric_mode -
*      One of the 'FAP20M_FABRIC_MODE' options.
*      + FAP with FE
*      + Back2Back
*      + Mesh
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
*    NON
*REMARKS:
*  The call to fap20m_set_single_fap_mode() and 
*  fap20m_set_fabric_mode() are orthogonal.
*  Setting of FAP20M_FABRIC_MODE_BACK2BACK/FAP20M_FABRIC_MODE_MESH
*  mode, should be done in system where no FE is present as a
*  design of the system and not as temporary state.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_fabric_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    FAP20M_FABRIC_MODE fabric_mode;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    fabric_mode = (FAP20M_FABRIC_MODE)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_set_fabric_mode(device_id, fabric_mode);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_fabric_mode
*TYPE:
*  PROC
*DATE: 
*  20-Sep-04
*FUNCTION:
*  Systems that consist only of 2,3,4,... FAPs with no FE deivces,
*  require special configuration.
*  Use this function to get device mode.
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_FABRIC_MODE* fabric_mode -
*      Loaded with one of the 'FAP20M_FABRIC_MODE' options.
*      + FAP with FE
*      + Back2Back
*      + Mesh
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
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_fabric_mode
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int  device_id;
    FAP20M_FABRIC_MODE fabric_mode;
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
    result = fap20m_get_fabric_mode(device_id, &fabric_mode);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", fabric_mode);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_mesh_links_topology
*TYPE:
*  PROC
*DATE: 
*  08/08/2007
*FUNCTION:
*  When working in mesh mode (more than two FAPs and no FEs)
*  this function sets the mesh topology.
*  I.E: The 4 entries in mesh_links_topology set 
*    the active links to each one of the other FAPs.
*    Note that the IDs of the FAPs in other size is irrelevant,
*    and that the entries indexes has no meaning.
*INPUT:
*  DIRECT:
*    IN   unsigned int  device_id -
*    IN   unsigned long mesh_links_topology[FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH] -
*                 Mesh topology
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    error indication
*  INDIRECT:
*    None.
*REMARKS:
*    Note that the IDs of the FAPs in other size is irrelevant,
*    and that the entries indexes has no meaning.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_mesh_links_topology
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long mesh_links_topology[FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH];
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    mesh_links_topology[0] = (unsigned long)inFields[0];
    mesh_links_topology[1] = (unsigned long)inFields[1];
    mesh_links_topology[2] = (unsigned long)inFields[2];
    mesh_links_topology[3] = (unsigned long)inFields[3];

    /* call Ocelot API function */
    result = fap20m_set_mesh_links_topology(device_id, mesh_links_topology);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_mesh_links_topology
*TYPE:
*  PROC
*DATE: 
*  08/08/2007
*FUNCTION:
*  When working in mesh mode (more than two FAPs and no FEs)
*  this function gets the mesh topology.
*  I.E: The 4 entries in mesh_links_topology is filled with
*    the active links to each one of the other FAPs.
*    Note that the IDs of the FAPs in other size is irrelevant,
*    and that the entries indexes has no meaning.
*INPUT:
*  DIRECT:
*    IN   unsigned int  device_id -
*    OUT  unsigned long mesh_links_topology[FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH] -
*                 Mesh topology
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    error indication
*  INDIRECT:
*    None.
*REMARKS:
*    Note that the IDs of the FAPs in other size is irrelevant,
*    and that the entries indexes has no meaning.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_mesh_links_topology
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned long mesh_links_topology[FAP20M_MAX_NOF_OTHER_FAPS_IN_MESH];
    unsigned short rc;


    /* avoid compiler warnings */
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_get_mesh_links_topology(device_id, mesh_links_topology);

    rc = sand_get_error_code_from_error_word(result);

    if (rc != SAND_OK)
    {
        galtisOutput(outArgs, rc, "%d", -1);
        return CMD_OK;
    }

    /* pack output arguments to galtis string */
    inFields[0] = mesh_links_topology[0];
    inFields[1] = mesh_links_topology[1];
    inFields[2] = mesh_links_topology[2];
    inFields[3] = mesh_links_topology[3];

    fieldOutput("%d%d%d%d", inFields[0], inFields[1], inFields[2], inFields[3]);

    galtisOutput(outArgs, rc, "%f");

    return CMD_OK;
}

/******************************************************************************/
static CMD_STATUS wrFap20m_get_mesh_links_topology_end
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
*  fap20m_set_dram_buff_size
*TYPE:
*  PROC
*DATE: 
*  12/07/2006
*FUNCTION:
*  Clear from the code.
*INPUT:
*  DIRECT:
*    unsigned int            device_id -
*    FAP20M_DRAM_BUFF_SIZE   buff_size -
*       DRAM buffer size
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP10M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_dram_buff_size
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    FAP20M_DRAM_BUFF_SIZE buff_size;
    unsigned long scrm_bit;
    unsigned short rc;


    /* avoid compiler warnings */
    inFields = inFields;
    numFields = numFields;

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    buff_size = (FAP20M_DRAM_BUFF_SIZE)inArgs[1];
    scrm_bit = (unsigned long)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_set_dram_buff_size(device_id, buff_size, scrm_bit);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*        
  fap20m_get_dram_buff_size
*TYPE:
*  PROC
*DATE: 
*  12/07/2006
*FUNCTION:
*  Clear from the code.
*INPUT:
*  DIRECT:
*    unsigned int            device_id -
*    FAP20M_DRAM_BUFF_SIZE   *buff_size  
*       DRAM buffer size
*  INDIRECT:
*    None.
*OUTPUT:
*  DIRECT:
*    unsigned long -
*      See formatting rules in ERROR RETURN VALUES above.
*      If error code is not FAP10M_NO_ERR then
*        specific error codes:
*          None.
*      Otherwise, no error has been detected and device
*        has been written.
*  INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_dram_buff_size
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned int result;
    unsigned int device_id;
    FAP20M_DRAM_BUFF_SIZE buff_size;  
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
    result = fap20m_get_dram_buff_size(device_id, &buff_size);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", buff_size);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_get_port_link_interface
*TYPE:
*  PROC
*DATE:
*  12-JUL-05
*FUNCTION:
*  Gets the link interface for specified Data-Port 
*  
*INPUT:
*  DIRECT:
*    unsigned int             device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    unsigned int             fap_data_port -
*      The DataPort in the device. This number is also used as the
*      scheduler port id. 
*    FAP20M_LINK_INTERFACE* link_interface -
*      Loaded interface A/B this DataPort belongs to.
*  INDIRECT:
*    Non.
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
*    link_interface
*REMARKS:
*  
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_get_port_link_interface
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
    unsigned int fap_data_port;
    FAP20M_LINK_INTERFACE link_interface;
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
    fap_data_port = (unsigned int)inArgs[2];

    /* call Ocelot api function */
    result = fap20m_get_port_link_interface(device_id, sch_rank, fap_data_port, &link_interface);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "%d", link_interface);

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_set_port_link_interface
*TYPE:
*  PROC
*DATE:
*  12-JUL-05
*FUNCTION:
*  Sets the link interface for specified Data-Port, 
*  
*INPUT:
*  DIRECT:
*    unsigned int             device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . This
*      identifier has been handed over to the caller
*      when 'fap20m_register_device' has been called.
*    unsigned int             fap_data_port -
*      The DataPort in the device. This number is also used as the
*      scheduler port id. 
*    FAP20M_LINK_INTERFACE  link_interface -
*      Value to load to SW DB mapping of Data-port to interface.
*  INDIRECT:
*    Non.
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
*    link_interface
*REMARKS:
*  
*SEE ALSO:
*****************************************************/
static CMD_STATUS wrFap20m_set_port_link_interface
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
    unsigned int fap_data_port;
    FAP20M_LINK_INTERFACE link_interface;
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
    fap_data_port = (unsigned int)inArgs[2];
    link_interface = (FAP20M_LINK_INTERFACE)inArgs[3];

    /* call Ocelot api function */
    result = fap20m_set_port_link_interface(device_id, sch_rank, fap_data_port, link_interface);

    rc = sand_get_error_code_from_error_word(result);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, rc, "");

    return CMD_OK;
}


/**** database initialization **************************************/

static CMD_COMMAND dbCommands[] =
{
        {"dram_parametersSet",
         &wrFap20m_set_dram_parameters,
         3, 4}
        ,
        {"get_fap_id",
         &wrFap20m_get_fap_id,
         1, 0},
        {"set_fap_id",
         &wrFap20m_set_fap_id,
         2, 0},

        {"get_profile_id",
         &wrFap20m_get_profile_id,
         1, 0},
        {"set_profile_id",
         &wrFap20m_set_profile_id,
         2, 0},

        {"profile_valuesGetFirst",
         &wrFap20m_get_profile_values,
         1, 0},
        {"profile_valuesGetNext",
         &wrFap20m_get_profile_values_end,
         1, 0},

        {"queue_type_paramsSet",
         &wrFap20m_set_queue_type_params,
         2, 8},
        {"queue_type_paramsGetFirst",
         &wrFap20m_get_queue_type_params,
         2, 0},
        {"queue_type_paramsGetNext",
         &wrFap20m_get_queue_type_params_end,
         2, 0},

        {"queues_prioritySet",
         &wrFap20m_set_queues_priority,
         2, 6},
        {"queues_priorityGetFirst",
         &wrFap20m_get_queues_priority,
         1, 0},
        {"queues_priorityGetNext",
         &wrFap20m_get_queues_priority_end,
         1, 0},

        {"scrubber_paramsSet",
         &wrFap20m_set_scrubber_params,
         1, 4},
        {"scrubber_paramsGetFirst",
         &wrFap20m_get_scrubber_params,
         1, 0},
        {"scrubber_paramsGetNext",
         &wrFap20m_get_scrubber_params_end,
         1, 0},
        

        {"backoff_paramsSet",
         &wrFap20m_set_backoff_params,
         1, 2},
        {"backoff_paramsGetFirst",
         &wrFap20m_get_backoff_params,
         1, 0},
        {"backoff_paramsGetNext",
         &wrFap20m_get_backoff_params_end,
         1, 0},

        {"wred_paramsSet",
         &wrFap20m_set_wred_params,
         3, 6},
        {"wred_paramsGetFirst",
         &wrFap20m_get_wred_params,
         3, 0},
        {"wred_paramsGetNext",
         &wrFap20m_get_wred_params_end,
         3, 0},

        {"set_credit_size",
         &wrFap20m_set_credit_size,
         2, 0},
        {"get_credit_size",
         &wrFap20m_get_credit_size,
         1, 0},
        {"detect_if_single_fap_mode_required",
         &wrFap20m_detect_if_single_fap_mode_required,
         1, 0},

        {"set_single_fap_mode",
         &wrFap20m_set_single_fap_mode,
         2, 0},
        {"get_single_fap_mode",
         &wrFap20m_get_single_fap_mode,
         1, 0},

        {"set_fabric_mode",
         &wrFap20m_set_fabric_mode,
         2, 0},
        {"get_fabric_mode",
         &wrFap20m_get_fabric_mode,
         1, 0},

        {"mesh_links_topologySet",
         &wrFap20m_set_mesh_links_topology,
         1, 4},
        {"mesh_links_topologyGetFirst",
         &wrFap20m_get_mesh_links_topology,
         1, 0},
        {"mesh_links_topologyGetNext",
         &wrFap20m_get_mesh_links_topology_end,
         1, 0},

        {"set_dram_buff_size",
         &wrFap20m_set_dram_buff_size,
         3, 0},
        {"get_dram_buff_size",
         &wrFap20m_get_dram_buff_size,
         1, 0},

        {"get_port_link_interface",
         &wrFap20m_get_port_link_interface,
         3, 0},
        {"set_port_link_interface",
         &wrFap20m_set_port_link_interface,
         4, 0}
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_configuration
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
GT_STATUS cmdLibInitFap20m_api_configuration
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

