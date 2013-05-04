/***********************************************************************

  OCTEON TOOLKITS                                                         
  Copyright (c) 2007 Cavium Networks. All rights reserved.

  This file, which is part of the OCTEON TOOLKIT from Cavium Networks,
  contains proprietary and confidential information of Cavium Networks
  and in some cases its suppliers.

  Any licensed reproduction, distribution, modification, or other use of
  this file or confidential information embodied in this file is subject
  to your license agreement with Cavium Networks. The applicable license
  terms can be found by contacting Cavium Networks or the appropriate
  representative within your company.

  All other use and disclosure is prohibited.

  Contact Cavium Networks at info@caviumnetworks.com for more information.

 ************************************************************************/ 

#if defined(__KERNEL__) && defined(linux)
#include <linux/module.h>
#include <linux/kernel.h>
#include "cvmx.h"
#include "cvmx-wqe.h"
#include "cvmx-fau.h"

#define printf printk

#else
#include <stdio.h>
#include <string.h>
#include "global-config.h"
#include "cvmx.h"
#include "cvmx-wqe.h"
#include "cvmx-malloc.h"
#include "cvmx-spinlock.h"
#include "cvmx-scratch.h"

#endif

#include "cvmx-config.h"
#include "cvmx-fpa.h"
#include "cvm-common-wqe.h"
#include "cvm-common-defs.h"
#include "cvm-common-misc.h"
#include "cvm-common-fpa.h"


/* debug level */
CVMX_SHARED int cvm_common_debug_level = CVM_COMMON_DBG_LVL_CRITICAL;

CVMX_SHARED char pools_initialized = 0;

#if !defined(__KERNEL__)
/* shared arena for components */
CVMX_SHARED cvmx_arena_list_t  cvm_common_arenas;

/* fpa information */
CVMX_SHARED cvm_common_fpa_info_t cvm_common_fpa_info[CVMX_FPA_NUM_POOLS];

/* threadsafe functionality of cvmx_malloc */
CVMX_SHARED cvmx_spinlock_t cvm_common_malloc_lock;

/* per core shared information */
CVMX_SHARED cvm_common_history_t *cvm_common_history = NULL;


/* Pointer to queued outgoing data: packets/messages */
#ifndef OUT_SWP_IN_SCRATCH // {
cvm_common_wqe_t *out_swp = NULL;
cvm_common_wqe_t *out_swp_tail = NULL;
#endif // OUT_SWP_IN_SCRATCH }

/*
 * per core trace history related things
 */
char* cvm_common_trace_macro_names[] = 
{
  "CVM_COMMON_MARK_FPA_BUFFER_ALLOC",
  "CVM_COMMON_MARK_FPA_BUFFER_FREE"
};
#else
DEFINE_PER_CPU(cvm_common_wqe_t *, __out_swp)=NULL;
DEFINE_PER_CPU(cvm_common_wqe_t * , __out_swp_tail)=NULL;
#endif /* linux*/
