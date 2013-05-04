/*
 *
 * OCTEON SDK
 *
 * Copyright (c) 2011 Cavium Networks. All rights reserved.
 *
 * This file, which is part of the OCTEON SDK which also includes the
 * OCTEON SDK Package from Cavium Networks, contains proprietary and
 * confidential information of Cavium Networks and in some cases its
 * suppliers. 
 *
 * Any licensed reproduction, distribution, modification, or other use of
 * this file or the confidential information or patented inventions
 * embodied in this file is subject to your license agreement with Cavium
 * Networks. Unless you and Cavium Networks have agreed otherwise in
 * writing, the applicable license terms "OCTEON SDK License Type 5" can be found 
 * under the directory: $OCTEON_ROOT/components/driver/licenses/
 *
 * All other use and disclosure is prohibited.
 *
 * Contact Cavium Networks at info@caviumnetworks.com for more information.
 *
 */



/*! \file cvm-drv.h
    \brief  Core Driver: Initialization and instruction processing. 
 */


#ifndef __CVM_DRV_H__
#define __CVM_DRV_H__

#include "cvm-driver-defs.h"

#define RED_LOW_WMARK       (2 * 1024)
#define RED_HIGH_WMARK      (3 * 1024)

#if !defined(DISABLE_PCIE14425_ERRATAFIX)
#define BP_CHECK_INTERVAL  1  /* Time in microseconds. */
#define BP_LOW_WMARK       RED_LOW_WMARK
#define BP_HIGH_WMARK      RED_HIGH_WMARK
#endif


/** Core: The application should call this routine when its doing its per-core
  * local initialization. This routine sets up the scratchpad registers
  * used by the driver.
  */
void
cvm_drv_local_init(void);




/** Core: The application should call this routine when it's doing global
  * initialization from only one of the cores its running on. This routines
  * sets up the PCI ports.
  *
  */
int
cvm_drv_init(void);





/** Core: This routine sends a notification to the host driver is ready to
  * accept instructions. The application should call this from only one of
  * its cores.
  * 
  */
int
cvm_drv_start(void);





/** Core: Common routine to parse all driver instructions. This routine looks
  * up the opcode to find a handler that's registered for this opcode.
  * If no handler is found, the wqe is freed. If a handler was found,
  * the handler is called with the WQE. The handler is expected to free
  * the WQE after its processing.
  *
  * @param  wqe - the work queue entry that contains the instruction.
  * @return  0 if a handler is found for the opcode; else 1.
  */
int
cvm_drv_process_instr(cvmx_wqe_t   *wqe);





/** Core: Register a handler for an opcode. When the driver sees the opcode, it
 *  will call the function registered for the opcode.
 *  @param opcode - register this opcode.
 *  @param handler - register this function. Call it with the WQE pointer
 *                   when a raw instruction with the opcode above is received.
 *  @return 0 if the handler was registered successfully, else 1.
 */
int
cvm_drv_register_op_handler(uint16_t   opcode, int (*handler)(cvmx_wqe_t *));




/** Core: Sets the application type. The application type registered by this call
 *  is passed to the host when the core driver sends the start notification
 *  in cvm_drv_start().  Application types are defined in
 *  driver/common/octeon-drv-opcodes.h
 *  @param app_mode: application type.
 */
void
cvm_drv_setup_app_mode(int app_mode);




void
cvm_56xx_pass2_update_pcie_req_num(void);

#if !defined(DISABLE_PCIE14425_ERRATAFIX)
void  cvmcs_sw_iq_bp(void);
#endif


#endif

/* $Id: cvm-drv.h 63425 2011-08-11 22:56:32Z panicker $ */
