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




/*!  \file cvm-drv-debug.h
     \brief Core driver:  debug and sanity check routines.
*/

#ifndef __CVM_DRV_DEBUG_H__
#define __CVM_DRV_DEBUG_H__

#include "cvmx.h"
#include "cvmx-fpa.h"
#include "cvmx-wqe.h"
#include "cvm-pci.h"


void    cvm_drv_print_data(void *udata, uint32_t size);
void    cvm_drv_print_wqe(cvmx_wqe_t  *wqe);
void    cvm_drv_print_pci_instr(cvmx_raw_inst_front_t *front);

#ifdef SANITY_CHECKS
int pci_output_sanity_and_buffer_count_update(cvmx_buf_ptr_t , uint32_t , uint32_t  , uint32_t , uint32_t );
#else
#define pci_output_sanity_and_buffer_count_update(arg1, arg2, arg3, arg4, arg5)   0
#endif

#define cvm_drv_init_debug_fn_list()   CAVIUM_INIT_LIST_HEAD(&cvm_drv_fn_list)

int cvm_drv_register_debug_fn(void (*fn)(void *), void *arg);

#ifdef CVM_DRV_SANITY_CHECKS
void  cvm_drv_add_dbg_lptr(uint64_t ptr);
void  cvm_drv_add_dbg_rptr(uint64_t ptr);
void  cvm_drv_print_dbg_lptr(void);
void  cvm_drv_print_dbg_rptr(void);
void  cvm_drv_print_dbg_ptrs(void);
void  cvm_drv_reset_dbg_ptr_cnt(void);
#else
#define cvm_drv_add_dbg_lptr(ptr)      do { }while(0);
#define cvm_drv_add_dbg_rptr(ptr)      do { }while(0);
#define cvm_drv_print_dbg_lptr()       do { }while(0);
#define cvm_drv_print_dbg_rptr()       do { }while(0);
#define cvm_drv_print_dbg_ptrs()       do { }while(0);
#define cvm_drv_reset_dbg_ptr_cnt()    do { }while(0);
#endif



void cvm_drv_debug_fn(void);

#endif

/* $Id: cvm-drv-debug.h 42868 2009-05-19 23:57:52Z panicker $ */

