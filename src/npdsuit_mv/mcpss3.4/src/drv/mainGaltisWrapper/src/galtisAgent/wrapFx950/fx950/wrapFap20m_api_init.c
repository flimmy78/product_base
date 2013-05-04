/*******************************************************************************
*              (c), Copyright 2006, Marvell International Ltd.                 *
* THIS CODE CONTAINS CONFIDENTIAL INFORMATION OF MARVELL SEMICONDUCTOR, INC.   *
* NO RIGHTS ARE GRANTED HEREIN UNDER ANY PATENT, MASK WORK RIGHT OR COPYRIGHT  *
* OF MARVELL OR ANY THIRD PARTY. MARVELL RESERVES THE RIGHT AT ITS SOLE        *
* DISCRETION TO REQUEST THAT THIS CODE BE IMMEDIATELY RETURNED TO MARVELL.     *
* THIS CODE IS PROVIDED "AS IS". MARVELL MAKES NO WARRANTIES, EXPRESSED,       *
* IMPLIED OR OTHERWISE, REGARDING ITS ACCURACY, COMPLETENESS OR PERFORMANCE.   *
********************************************************************************
* wrapFap20m_api_init.c
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
#include <cmdShell/cmdDb/cmdBase.h>
#include <cmdShell/common/cmdWrapUtils.h>
#include <string.h>

/* Feature specific includes. */
#include <FX950/include/fap20m_api_init.h>
#include <SAND/Utils/include/sand_framework.h>
#include <galtisAgent/wrapUtil/cmdCpssPresteraUtils.h>



static FAP20M_SINGLE_SERDES_STATUS  single_serdes_status[FAP20M_NUMBER_OF_LINKS];
static GT_U32                       serderIndex; 

/*****************************************************
*NAME
*  fap20m_hw_calibration
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Initialize HW aspect of the device. 
*  User is to give 'FAP20M_HW_ADJUSTMENTS' structure
*  that describes the device configuration.
*INPUT:
*  DIRECT:
*    unsigned int           device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . 
*    FAP20M_HW_ADJUSTMENTS* hw_ajust   -
*      HW adjustment parameters to be given from the user.
*      These parameters are board/system deepened. 
*    unsigned int           silent    -
*      Because this function might take few seconds, 
*      if silent-flag is FALSE: some informational will be printed
*      via the sand_os_printf() function.
*      Useful for debugging.
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
*  
*SEE ALSO:
*
*****************************************************/
static CMD_STATUS wrFap20m_hw_calibrationSetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    serderIndex = 0;

    memset(single_serdes_status, sizeof(char), FAP20M_NUMBER_OF_LINKS*sizeof(FAP20M_SINGLE_SERDES_STATUS));

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    single_serdes_status[serderIndex].valid   = inFields[0];
    single_serdes_status[serderIndex].amp     = inFields[1] ;
    single_serdes_status[serderIndex].pen     = inFields[2];
    single_serdes_status[serderIndex].vcms    = inFields[3];
    single_serdes_status[serderIndex].txreg_i = inFields[4];
    single_serdes_status[serderIndex].isel_eq  = inFields[5];
    single_serdes_status[serderIndex].sel_zero = inFields[6]; 
    single_serdes_status[serderIndex].int_7b   = inFields[7];
    single_serdes_status[serderIndex].stop_int = inFields[8]; 

    serderIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_hw_calibrationSetNext
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

    /* map input arguments to locals */
    single_serdes_status[serderIndex].valid   = inFields[0];
    single_serdes_status[serderIndex].amp     = inFields[1] ;
    single_serdes_status[serderIndex].pen     = inFields[2];
    single_serdes_status[serderIndex].vcms    = inFields[3];
    single_serdes_status[serderIndex].txreg_i = inFields[4];
    single_serdes_status[serderIndex].isel_eq  = inFields[5];
    single_serdes_status[serderIndex].sel_zero = inFields[6]; 
    single_serdes_status[serderIndex].int_7b   = inFields[7];
    single_serdes_status[serderIndex].stop_int = inFields[8]; 

    serderIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;

}

static CMD_STATUS wrFap20m_hw_calibrationEndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long         result;
    unsigned int          device_id;
    FAP20M_HW_ADJUSTMENTS hwAdjustment;
    unsigned int          silent;
   

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    device_id = (unsigned int)inArgs[0];
    silent = (unsigned int)inArgs[1];  

    hwAdjustment.dram_conf.nof_columns = (unsigned long)inArgs[2];
    hwAdjustment.dram_conf.auto_precharge_pos = (unsigned long)inArgs[3];
    hwAdjustment.dram_conf.enable_8bank_mode = (unsigned long)inArgs[4];
    hwAdjustment.dram_conf.burst_size_mode = (unsigned long)inArgs[5];
    hwAdjustment.drc_conf.ddr_control_trig = (unsigned long)inArgs[6]; ;  
    hwAdjustment.drc_conf.ddr_mode1 = (unsigned long)inArgs[7];         
    hwAdjustment.drc_conf.ddr_mode2 = (unsigned long)inArgs[8];       
    hwAdjustment.drc_conf.ddr_ext_mode = (unsigned long)inArgs[9];     
    hwAdjustment.drc_conf.ac_op_cond1 = (unsigned long)inArgs[10];      
    hwAdjustment.drc_conf.ac_op_cond2 = (unsigned long)inArgs[11];    
    hwAdjustment.drc_conf.init_seq_reg = (unsigned long)inArgs[12];   
    hwAdjustment.drc_conf.ac_op_cond3 = (unsigned long)inArgs[13];      
    hwAdjustment.drc_conf.drc_general_config = (unsigned long)inArgs[14]; 
    hwAdjustment.drc_conf.write_read_rates = (unsigned long)inArgs[15];   
    hwAdjustment.drc_conf.gddr3_cfg = (unsigned long)inArgs[16];     
    hwAdjustment.drc_conf.ddr_extended_mode_reg2 = (unsigned long)inArgs[17];    
    hwAdjustment.drc_conf.ddr_extended_mode_reg3 = (unsigned long)inArgs[18];     
    hwAdjustment.drc_conf.ddr2_extended_mode_wr2_reg = (unsigned long)inArgs[19]; 
    hwAdjustment.drc_conf.ddr2_extended_mode_wr3_reg = (unsigned long)inArgs[20]; 
    hwAdjustment.drc_conf.glue_logic = (unsigned long)inArgs[21];
    hwAdjustment.qdr_conf.nof_buff_mem = (unsigned int)inArgs[22];
    hwAdjustment.qdr_conf.burst_length = (unsigned int)inArgs[23]; 
    hwAdjustment.qdr_conf.read_delay = (unsigned int)inArgs[24];
    hwAdjustment.qdr_conf.data_valid_delay = (unsigned int)inArgs[25];
    hwAdjustment.qdr_conf.start_burst_delay = (unsigned int)inArgs[26];
    hwAdjustment.gen_conf.dram_buff_size = (unsigned int)inArgs[27];
    hwAdjustment.load_serdes_conf = (unsigned int)inArgs[28];
    hwAdjustment.load_drc_conf = (unsigned int)inArgs[29];
    hwAdjustment.load_dram_conf = (unsigned int)inArgs[30];
    hwAdjustment.load_qdr_conf = (unsigned int)inArgs[31]; 
    hwAdjustment.load_gen_conf = (unsigned int)inArgs[32]; 
    hwAdjustment.scrambling_bits = (unsigned long)inArgs[33];

    /* call Ocelot API function */
    result = fap20m_hw_calibration(device_id, &hwAdjustment, silent);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}
   
/*****************************************************
*NAME
*  fap20m_enable_revision
*TYPE:
*  PROC
*DATE: 
*  03/12/2006
*FUNCTION:
*  This function enable/disable bug fixes and new feature,
*   added in new revisions.
*  This function must be called before calling fap20m_init_sequence_phase1()
*  Calling fap20m_enable_revision() after fap20m_init_sequence_phase1()
*   results in error.
*INPUT:
*  DIRECT:
*    unsigned int              device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_REVISION_CONTROL   revision_control -
*      Contains the revision control information.
*    unsigned int              silent            -
*      When silent == TRUE, the function will not print to the screen.
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
static CMD_STATUS wrFap20m_enable_revision
(
    IN  long inArgs[CMD_MAX_ARGS],
    IN  long inFields[CMD_MAX_FIELDS],
    IN  long numFields,
    OUT char outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_REVISION_CONTROL revision_control;
    unsigned int silent;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    revision_control.user_application_rev = (FAP20M_LAST_KNOWN_REVISION)inArgs[1];
    silent = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_enable_revision(device_id, &revision_control, silent);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");           
    

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_init_sequence_phase1
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This function may be called prior to setting the device FAP-ID.
*   - Device Hardware Adjustments
*      o Serdes
*   - SCH/QDP Dynamic Memories Init.
*   - Zero Some Static Tables: FSF, (RTP multicast?)
*   - FCT, RTP, FDR - Prevent all control cells.
*INPUT:
*  DIRECT:
*    unsigned int     device_id  -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_HW_ADJUSTMENTS* hw_ajust   -
*      HW adjustment parameters to be given from the user.
*      These parameters are board/system deepened. 
*    unsigned int     silent    -
*      Because this function might take few seconds, 
*      if silent-flag is FALSE: some informational will be printed
*      via the sand_os_printf() function.
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
static CMD_STATUS wrFap20m_init_sequence_phase1SetFirst
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    serderIndex = 0;

    memset(single_serdes_status, sizeof(char), FAP20M_NUMBER_OF_LINKS*sizeof(FAP20M_SINGLE_SERDES_STATUS));

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    single_serdes_status[serderIndex].valid   = inFields[0];
    single_serdes_status[serderIndex].amp     = inFields[1] ;
    single_serdes_status[serderIndex].pen     = inFields[2];
    single_serdes_status[serderIndex].vcms    = inFields[3];
    single_serdes_status[serderIndex].txreg_i = inFields[4];
    single_serdes_status[serderIndex].isel_eq  = inFields[5];
    single_serdes_status[serderIndex].sel_zero = inFields[6]; 
    single_serdes_status[serderIndex].int_7b   = inFields[7];
    single_serdes_status[serderIndex].stop_int = inFields[8]; 

    serderIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;
}

static CMD_STATUS wrFap20m_init_sequence_phase1SetNext
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

    /* map input arguments to locals */
    single_serdes_status[serderIndex].valid   = inFields[0];
    single_serdes_status[serderIndex].amp     = inFields[1] ;
    single_serdes_status[serderIndex].pen     = inFields[2];
    single_serdes_status[serderIndex].vcms    = inFields[3];
    single_serdes_status[serderIndex].txreg_i = inFields[4];
    single_serdes_status[serderIndex].isel_eq  = inFields[5];
    single_serdes_status[serderIndex].sel_zero = inFields[6]; 
    single_serdes_status[serderIndex].int_7b   = inFields[7];
    single_serdes_status[serderIndex].stop_int = inFields[8]; 

    serderIndex++;

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, GT_OK, "");

    return CMD_OK;

}

static CMD_STATUS wrFap20m_init_sequence_phase1EndSet
(
    IN  GT_32 inArgs[CMD_MAX_ARGS],
    IN  GT_32 inFields[CMD_MAX_FIELDS],
    IN  GT_32 numFields,
    OUT GT_8  outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long         result;
    unsigned int          device_id;
    FAP20M_HW_ADJUSTMENTS hwAdjustment;
    unsigned int          silent;
   

    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */

    device_id = (unsigned int)inArgs[0];
    silent = (unsigned int)inArgs[1];  

    hwAdjustment.dram_conf.nof_columns = (unsigned long)inArgs[2];
    hwAdjustment.dram_conf.auto_precharge_pos = (unsigned long)inArgs[3];
    hwAdjustment.dram_conf.enable_8bank_mode = (unsigned long)inArgs[4];
    hwAdjustment.dram_conf.burst_size_mode = (unsigned long)inArgs[5];
    hwAdjustment.drc_conf.ddr_control_trig = (unsigned long)inArgs[6]; ;  
    hwAdjustment.drc_conf.ddr_mode1 = (unsigned long)inArgs[7];         
    hwAdjustment.drc_conf.ddr_mode2 = (unsigned long)inArgs[8];       
    hwAdjustment.drc_conf.ddr_ext_mode = (unsigned long)inArgs[9];     
    hwAdjustment.drc_conf.ac_op_cond1 = (unsigned long)inArgs[10];      
    hwAdjustment.drc_conf.ac_op_cond2 = (unsigned long)inArgs[11];    
    hwAdjustment.drc_conf.init_seq_reg = (unsigned long)inArgs[12];   
    hwAdjustment.drc_conf.ac_op_cond3 = (unsigned long)inArgs[13];      
    hwAdjustment.drc_conf.drc_general_config = (unsigned long)inArgs[14]; 
    hwAdjustment.drc_conf.write_read_rates = (unsigned long)inArgs[15];   
    hwAdjustment.drc_conf.gddr3_cfg = (unsigned long)inArgs[16];     
    hwAdjustment.drc_conf.ddr_extended_mode_reg2 = (unsigned long)inArgs[17];    
    hwAdjustment.drc_conf.ddr_extended_mode_reg3 = (unsigned long)inArgs[18];     
    hwAdjustment.drc_conf.ddr2_extended_mode_wr2_reg = (unsigned long)inArgs[19]; 
    hwAdjustment.drc_conf.ddr2_extended_mode_wr3_reg = (unsigned long)inArgs[20]; 
    hwAdjustment.drc_conf.glue_logic = (unsigned long)inArgs[21];
    hwAdjustment.qdr_conf.nof_buff_mem = (unsigned int)inArgs[22];
    hwAdjustment.qdr_conf.burst_length = (unsigned int)inArgs[23]; 
    hwAdjustment.qdr_conf.read_delay = (unsigned int)inArgs[24];
    hwAdjustment.qdr_conf.data_valid_delay = (unsigned int)inArgs[25];
    hwAdjustment.qdr_conf.start_burst_delay = (unsigned int)inArgs[26];
    hwAdjustment.gen_conf.dram_buff_size = (unsigned int)inArgs[27];
    hwAdjustment.load_serdes_conf = (unsigned int)inArgs[28];
    hwAdjustment.load_drc_conf = (unsigned int)inArgs[29];
    hwAdjustment.load_dram_conf = (unsigned int)inArgs[30];
    hwAdjustment.load_qdr_conf = (unsigned int)inArgs[31]; 
    hwAdjustment.load_gen_conf = (unsigned int)inArgs[32]; 
    hwAdjustment.scrambling_bits = (unsigned long)inArgs[33];

    /* call Ocelot API function */
    result = fap20m_init_sequence_phase1(device_id, &hwAdjustment, silent);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_init_sequence_phase2
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  This function MUST be called AFTER setting the 
*    device UNIQUE FAP-ID.
*   - FCT, RTP, FDR Enable All
*INPUT:
*  DIRECT:
*    unsigned int device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
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
static CMD_STATUS wrFap20m_init_sequence_phase2
(
    IN  long inArgs[CMD_MAX_ARGS],
    IN  long inFields[CMD_MAX_FIELDS],
    IN  long numFields,
    OUT char outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];

    /* call Ocelot API function */
    result = fap20m_init_sequence_phase2(device_id);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");   

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_enable_fabric_traffic
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  Enables / disables traffic from going to/from the fabric.
*  Disable/enable:
*  1. Packet from the PP to the fabric.
*  2. Cells from FEs to the FAP (FDR).
*  If enable also: set the MAC to count the right cells,
*  and clears the RX counters.
*INPUT:
*  DIRECT:
*    unsigned int device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() .
*    unsigned int enable    -
*      TRUE  - Enables traffic to go to/from the fabric.
*      FALSE - Disables traffic to go to/from the fabric.
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
static CMD_STATUS wrFap20m_enable_fabric_traffic
(
    IN  long inArgs[CMD_MAX_ARGS],
    IN  long inFields[CMD_MAX_FIELDS],
    IN  long numFields,
    OUT char outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0]; 
    enable = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_enable_fabric_traffic(device_id, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");                     

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_all_ctrl_cells_enable
*TYPE:
*  PROC
*DATE: 
*  11-Jan-04
*FUNCTION:
*  All control cell from device are dropped.
*INPUT:
*  DIRECT:
*    unsigned int device_id -
*      Sand-driver device-identifier (returned from fap20m_register_device() . 
*    unsigned int enable -
*      TRUE/FALSE - enable/disable device to generate all control cells.
*        One need to disable the generation before the device have a valid 
*        FAP-ID.
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
static CMD_STATUS wrFap20m_all_ctrl_cells_enable
(
    IN  long inArgs[CMD_MAX_ARGS],
    IN  long inFields[CMD_MAX_FIELDS],
    IN  long numFields,
    OUT char outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    unsigned int enable;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0]; 
    enable = (unsigned int)inArgs[1];

    /* call Ocelot API function */
    result = fap20m_all_ctrl_cells_enable(device_id, enable);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");       

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_init_drc_and_dram
*TYPE:
*  PROC
*DATE: 
*  11/12/2006
*FUNCTION:
*  Init DRC configuration and DRAM configuration
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*                         Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_DRC_CONFIGURATION *drc_conf - 
*                         DRC configuration.
*    FAP20M_DRAM_CONFIGURATION *dram_conf - 
*                         DRAM configuration.
*    unsigned int        load_drc_conf - 
*                         If TRUE , DRC configuration will be loaded
*    unsigned int        load_dram_conf - 
*                         If TRUE , DRAM configuration will be loaded
*    unsigned int        take_out_of_reset - 
*                         If TRUE, DRC will be taken out of reset after configuration.
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
static CMD_STATUS wrFap20m_init_drc_and_dram
(
    IN  long inArgs[CMD_MAX_ARGS],
    IN  long inFields[CMD_MAX_FIELDS],
    IN  long numFields,
    OUT char outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_DRC_CONFIGURATION drc_conf;
    FAP20M_DRAM_CONFIGURATION dram_conf;
    unsigned int load_drc_conf;
    unsigned int load_dram_conf;
    unsigned int take_out_of_reset;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    drc_conf.ddr_control_trig = (unsigned long)inFields[0];
    drc_conf.ddr_mode1 = (unsigned long)inFields[1];
    drc_conf.ddr_mode2 = (unsigned long)inFields[2];
    drc_conf.ddr_ext_mode = (unsigned long)inFields[3];
    drc_conf.ac_op_cond1 = (unsigned long)inFields[4];
    drc_conf.ac_op_cond2 = (unsigned long)inFields[5];
    drc_conf.init_seq_reg = (unsigned long)inFields[6];
    drc_conf.ac_op_cond3 = (unsigned long)inFields[7];
    drc_conf.drc_general_config = (unsigned long)inFields[8];
    drc_conf.write_read_rates = (unsigned long)inFields[9];
    drc_conf.gddr3_cfg = (unsigned long)inFields[10];
    drc_conf.ddr_extended_mode_reg2 = (unsigned long)inFields[11];
    drc_conf.ddr_extended_mode_reg3 = (unsigned long)inFields[12];
    drc_conf.ddr2_extended_mode_wr2_reg = (unsigned long)inFields[13];
    drc_conf.ddr2_extended_mode_wr3_reg = (unsigned long)inFields[14];
    drc_conf.glue_logic = (unsigned long)inFields[15];
    dram_conf.nof_columns = (unsigned long)inFields[16];
    dram_conf.auto_precharge_pos = (unsigned long)inFields[17];
    dram_conf.enable_8bank_mode = (unsigned long)inFields[18];
    dram_conf.burst_size_mode = (unsigned long)inFields[19];
    load_drc_conf = (unsigned int)inArgs[1];
    load_dram_conf = (unsigned int)inArgs[2];
    take_out_of_reset = (unsigned int)inArgs[3];

    /* call Ocelot API function */
    result = fap20m_init_drc_and_dram(device_id, &drc_conf, &dram_conf, load_drc_conf, load_dram_conf, take_out_of_reset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");    

    return CMD_OK;
}

/*****************************************************
*NAME
*  fap20m_init_drc
*TYPE:
*  PROC
*DATE: 
*  11/12/2006
*FUNCTION:
*  Initializes DRC registers with the specified values.
*INPUT:
*  DIRECT:
*    unsigned int        device_id -
*                         Sand-driver device-identifier (returned from fap20m_register_device() .
*    FAP20M_DRC_CONFIGURATION*     drc_conf - 
*                        DRC configuration.
*    unsigned int        load_drc_conf - 
*                         If TRUE , DRC configuration will be loaded
*    unsigned int        take_out_of_reset - 
*                         If TRUE, DRC will be taken out of reset after configuration.
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
static CMD_STATUS wrFap20m_init_drc
(
    IN  long inArgs[CMD_MAX_ARGS],
    IN  long inFields[CMD_MAX_FIELDS],
    IN  long numFields,
    OUT char outArgs[CMD_MAX_BUFFER]
)
{
    unsigned long result;
    unsigned int device_id;
    FAP20M_DRC_CONFIGURATION drc_conf;
    unsigned int load_drc_conf;
    unsigned int take_out_of_reset;


    /* check for valid arguments */
    if(!inArgs || !outArgs)
        return CMD_AGENT_ERROR;

    /* map input arguments to locals */
    device_id = (unsigned int)inArgs[0];
    drc_conf.ddr_control_trig = (unsigned long)inFields[0];
    drc_conf.ddr_mode1 = (unsigned long)inFields[1];
    drc_conf.ddr_mode2 = (unsigned long)inFields[2];
    drc_conf.ddr_ext_mode = (unsigned long)inFields[3];
    drc_conf.ac_op_cond1 = (unsigned long)inFields[4];
    drc_conf.ac_op_cond2 = (unsigned long)inFields[5];
    drc_conf.init_seq_reg = (unsigned long)inFields[6];
    drc_conf.ac_op_cond3 = (unsigned long)inFields[7];
    drc_conf.drc_general_config = (unsigned long)inFields[8];
    drc_conf.write_read_rates = (unsigned long)inFields[9];
    drc_conf.gddr3_cfg = (unsigned long)inFields[10];
    drc_conf.ddr_extended_mode_reg2 = (unsigned long)inFields[11];
    drc_conf.ddr_extended_mode_reg3 = (unsigned long)inFields[12];
    drc_conf.ddr2_extended_mode_wr2_reg = (unsigned long)inFields[13];
    drc_conf.ddr2_extended_mode_wr3_reg = (unsigned long)inFields[14];
    drc_conf.glue_logic = (unsigned long)inFields[15];
    load_drc_conf = (unsigned int)inArgs[1];
    take_out_of_reset = (unsigned int)inArgs[2];

    /* call Ocelot API function */
    result = fap20m_init_drc(device_id, &drc_conf, load_drc_conf, take_out_of_reset);

    /* pack output arguments to galtis string */
    galtisOutput(outArgs, result, "");         

    return CMD_OK;
}

/**** database initialization **************************************/



static CMD_COMMAND dbCommands[] =
{
        {"fap20m_hw_calibrationSetFirst",
         &wrFap20m_hw_calibrationSetFirst,
         0, 7},
        {"fap20m_hw_calibrationSetNext",
         &wrFap20m_hw_calibrationSetNext,
         0, 7},
        {"fap20m_hw_calibrationEndSet",
         &wrFap20m_hw_calibrationEndSet,
         34, 0},    
        {"fap20m_init_sequence_phase1SetFirst",
         &wrFap20m_init_sequence_phase1SetFirst,
         0, 7},
        {"fap20m_init_sequence_phase1SetNext",
         &wrFap20m_init_sequence_phase1SetNext,
         0, 7},
        {"fap20m_init_sequence_phase1EndSet",
         &wrFap20m_init_sequence_phase1EndSet,
         34, 0},
        {"fap20m_init_sequence_phase2",
         &wrFap20m_init_sequence_phase2,
         1, 0},
        {"fap20m_enable_fabric_traffic",
         &wrFap20m_enable_fabric_traffic,
         2, 0},
        {"fap20m_all_ctrl_cells_enable",
         &wrFap20m_all_ctrl_cells_enable,
         2, 0},
        {"fap20m_enable_revision",
         &wrFap20m_enable_revision,
         3, 0},
       {"fap20m_init_drc_and_dramSet",
         &wrFap20m_init_drc_and_dram,
         4, 19},
        {"fap20m_init_drcSet",
         &wrFap20m_init_drc,
         3, 16},
};

#define numCommands (sizeof(dbCommands) / sizeof(CMD_COMMAND))

/*******************************************************************************
* cmdLibInitFap20m_api_init
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
GT_STATUS cmdLibInitFap20m_api_init
(
    GT_VOID
)
{
    return cmdInitLibrary(dbCommands, numCommands);
}

