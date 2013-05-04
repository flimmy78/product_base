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



#ifndef __CVMCS_APP_COMMON_H__
#define __CVMCS_APP_COMMON_H__

#include <stdio.h>
#include <string.h>
#ifdef __linux__
#include <signal.h>
#endif


#include "cvmx.h"
#include "cvmx-config.h"
#include "cvmx-fpa.h"
#include "cvmx-ipd.h"
#include "cvmx-pko.h"
#include "cvmx-pow.h"
#include "cvmx-sysinfo.h"
#include "cvmx-coremask.h"
#include "cvmx-malloc.h"
#include "cvmx-bootmem.h"
#include "cvmx-helper.h"
#ifdef CONFIG_EBT3000
#include "cvmx-ebt3000.h"
#endif

#include "octeon-opcodes.h"

#include "cvm-driver-defs.h"
#include "cvm-drv.h"
#include "cvm-drv-debug.h"



/* If OCTEON_DEBUG_LEVEL is defined all
   application debug messages are enabled. */
#ifdef OCTEON_DEBUG_LEVEL
#define DBG                      printf
#else
#define DBG(format, args...)     do{ }while(0)
#endif




static inline int
cvmcs_app_mem_alloc(char *str, int pool, int buf_size, int pool_count)
{
	void *memory, *base;

	memory = cvmx_bootmem_alloc((buf_size * pool_count), CVMX_CACHE_LINE_SIZE);
	if (memory == NULL) {
		printf("Out of memory initializing %s.\n", str);
		return 1;
	}

	printf("Allocating memory from %p to %p for Pool %d\n",
	       memory, (memory + (buf_size * pool_count)), pool);

#ifdef CN56XX_PEER_TO_PEER
	if( (pool <= 1) && ((unsigned long)(memory + (buf_size * pool_count)) > (64 * 1024 * 1024))) {
		printf("Pool %d exceeds 64M. Peer-to-Peer will not work\n", pool);
		return 1;
	}
#endif

	cvmx_fpa_setup_pool(pool, str, memory, buf_size, pool_count);

	base = CVM_COMMON_INIT_FPA_CHECKS(memory, pool_count, buf_size);
	cvm_common_fpa_add_pool_info(pool, memory, buf_size, pool_count,
CAST64(base));

	return 0;
}



/* Print the count of available buffers in each FPA pool. */
static inline void
print_pool_count_stats()
{
	int i;

	printf("Pool stats: ");
	for(i = 0; i < 8; i++) {
		uint64_t  cnt = cast64(cvmx_read_csr(CVMX_FPA_QUEX_AVAILABLE(i)));
		if(cnt)
			printf("[%d: %lu ] ", i, cnt);
	}
	printf("\n");
}



static inline cvmx_wqe_t  *
cvmcs_app_get_work_sync(int wait)
{
#ifdef SANITY_CHECKS
	return cvm_common_get_work_sync(wait);
#else
	return cvmx_pow_work_request_sync(wait);
#endif
}


int   cvmcs_common_add_task(uint64_t  interval, int (* fn)(void *), void *fn_arg);
int   cvmcs_common_remove_task(int (* fn)(void *), void *fn_arg);
void  cvmcs_common_schedule_tasks(void);




/** Global initialization. Performed by the boot core only. */
int   cvmcs_app_init_global();

/** Local initialization. Performed by all cores. */
int   cvmcs_app_init_local();

/** Application shutdown. Disable and shutdown PKO and FPA pools. */
int   cvmcs_app_shutdown();

/** Common duty cycle operations. */
void  cvmcs_app_duty_cycle_actions();

/** Check and print application mode (32/64 bit) */
void  cvmcs_app_check_mode();

/** Common application bringup routine. */
int   cvmcs_app_bringup();

/** Barrier call based on coremask in cvmcs-common. */
void  cvmcs_app_barrier();

/** Returns 1 if the core id is for the boot core, 0 otherwise. */
int   is_boot_core(uint32_t   core_id);

/** Returns 1 if the core id is for the display core, 0 otherwise. */
int   is_display_core(uint32_t   core_id);

/* Get the Base MAC address for Octeon device. */
uint8_t*  cvmcs_app_get_macaddr_base();


#endif

/* $Id: cvmcs-common.h 59474 2011-04-25 00:05:35Z panicker $ */
