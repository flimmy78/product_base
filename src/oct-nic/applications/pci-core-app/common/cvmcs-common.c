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



#include "cvmcs-common.h"
#include "cvmx-helper-cfg.h"


CVMX_SHARED    uint64_t  cpu_freq      = 0;
CVMX_SHARED    uint32_t  num_cores     = 0;
CVMX_SHARED    uint32_t  boot_core     = 0;
CVMX_SHARED    uint32_t  display_core  = 0;
CVMX_SHARED    uint32_t  coremask_all  = 0x0;
CVMX_SHARED    cvmx_sysinfo_t *appinfo = NULL;
CVMX_SHARED    uint32_t  core_count = 0;
CVMX_SHARED    uint32_t  core_active[32] =
	 {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

               uint32_t  core_id  = 0;

#define  MAX_CVMCS_TASKS   8

struct __cvmcs_tasks {
	uint64_t   time_to_call;
	uint64_t   interval;
	int        (* fn)(void *);
	void       *fn_arg;
};
cvmx_spinlock_t   cvmcs_task_lock;

CVMX_SHARED  struct __cvmcs_tasks  cvm_tasks[MAX_CVMCS_TASKS];



int
cvmcs_common_add_task(uint64_t  interval, int (* fn)(void *), void *fn_arg)
{
	int i;

	cvmx_spinlock_lock(&cvmcs_task_lock);
	for(i = 0; i < MAX_CVMCS_TASKS; i++) {
		if(cvm_tasks[i].fn == NULL) {
			cvm_tasks[i].fn           = fn;
			cvm_tasks[i].fn_arg       = fn_arg;
			cvm_tasks[i].interval     = interval;
			cvm_tasks[i].time_to_call = cvmx_get_cycle() + interval;
			break;
		}
	}
	cvmx_spinlock_unlock(&cvmcs_task_lock);

	return (i == MAX_CVMCS_TASKS);
}



int
cvmcs_common_remove_task(int (* fn)(void *), void *fn_arg)
{
	int i;

	cvmx_spinlock_lock(&cvmcs_task_lock);
	for(i = 0; i < MAX_CVMCS_TASKS; i++) {
		if( (cvm_tasks[i].fn == fn) && (cvm_tasks[i].fn_arg == fn_arg) ){
			cvm_tasks[i].fn           = NULL;
			cvm_tasks[i].fn_arg       = NULL;
			cvm_tasks[i].interval     = 0;
			cvm_tasks[i].time_to_call = 0;
			break;
		}
	}
	cvmx_spinlock_unlock(&cvmcs_task_lock);

	return (i == MAX_CVMCS_TASKS);
}




void
cvmcs_common_schedule_tasks(void)
{
	uint64_t  cycle = cvmx_get_cycle();
	int       i;

	cvmx_spinlock_lock(&cvmcs_task_lock);
	for(i = 0; i < MAX_CVMCS_TASKS; i++) {
		if(cvm_tasks[i].fn && (cycle >= cvm_tasks[i].time_to_call) ) {
			cvmx_spinlock_unlock(&cvmcs_task_lock);
			if(cvm_tasks[i].fn(cvm_tasks[i].fn_arg))
				cvmcs_common_remove_task(cvm_tasks[i].fn, cvm_tasks[i].fn_arg);
			cvmx_spinlock_lock(&cvmcs_task_lock);
			cvm_tasks[i].time_to_call = cvmx_get_cycle() + cvm_tasks[i].interval;
		}
	}
	cvmx_spinlock_unlock(&cvmcs_task_lock);
}







/** The core with the lowest core id becomes the boot core. */
void
cvmcs_get_boot_core(uint32_t  *boot_core)
{
	int i;
	for(i = 0; i < num_cores; i++)
		if(core_active[i])
			break;
	*boot_core = i;
	return;
}


/** The core with the highest core id becomes the display core. */
void
cvmcs_get_display_core(uint32_t  *display_core)
{
	int i;
	for(i = num_cores; i >= 0; i--)
		if(core_active[i])
			break;
	*display_core = i;
	return;
}





extern void cvmx_helper_cfg_show_cfg(void);



/** Global initialization. Performed by the boot core only. */
int
cvmcs_app_init_global()
{

	if(cvm_common_global_init())
		return 1;

	if (cvmx_helper_initialize_packet_io_global() == -1) {
		printf("# cvmcs: Failed to initialize input ports\n");
		return 1;
	}
	CVMX_SYNCW;

	/* Disable PKO while we configure PCI driver. */
	cvmx_pko_disable();

	memset(cvm_tasks, 0, sizeof(struct __cvmcs_tasks) * MAX_CVMCS_TASKS);
	cvmx_spinlock_init(&cvmcs_task_lock);

	cvmx_helper_ipd_and_packet_input_enable();


	/* Initialize PCI driver */
	if(cvm_drv_init())
		return 1;

	return 0;
}




/** Local initialization. Performed by all cores. */
int
cvmcs_app_init_local()
{
	cvm_common_local_init();

	/* Allocate a command buffer for PKO */
	cvmx_pko_initialize_local();

	/* Scratch pad initialization for core PCI driver */
	cvm_drv_local_init();

	/* Make sure we are not in NULL_NULL POW state
	   (if we are, we can't output a packet) */
	cvmx_pow_work_request_null_rd();

	CVMX_SYNCW;
	return 0;
}






/** Application shutdown. Disable and shutdown PKO and FPA pools. */
int
cvmcs_app_shutdown()
{
	int result = 0;
	int status;
	int pool;
	cvmx_fpa_ctl_status_t fpa_status;


	printf("# cvmcs: cvmcs_shutdown() called...\n");
	cvmx_pko_shutdown();

	for (pool=0; pool<CVMX_FPA_NUM_POOLS; pool++) {
		if (cvmx_fpa_get_block_size(pool) > 0) {
			status = cvmx_fpa_shutdown_pool(pool);
			/* Special check to allow PIP to lose packets due to hardware prefetch */
			if ((pool == CVMX_FPA_PACKET_POOL) && (status > 0) && (status < CVMX_PIP_NUM_INPUT_PORTS))
				status = 0;
			result |= status;
		}
	}

	fpa_status.u64 = 0;
	cvmx_write_csr(CVMX_FPA_CTL_STATUS, fpa_status.u64);
	cvmx_pko_disable();
	printf("# cvmcs: cvmcs_shutdown() completed.\n");
	return result;
}










void
cvmcs_app_duty_cycle_actions()
{
	cvmx_pko_port_status_t  stat;
	int i, first_port = 32, num_ports = 4;

	print_pool_count_stats();

	if(OCTEON_IS_MODEL(OCTEON_CN68XX)) {
		first_port = 0x100;
		num_ports = 32;
	}
	for(i = first_port; i < (first_port + num_ports); i++) {
		cvmx_pko_get_port_status(i, 0, &stat);
		if(stat.packets || stat.doorbell)
			printf("Port %d: db: %llu pkts: %u\n", i, cast64(stat.doorbell),
			       stat.packets);
	}


}





void
cvmcs_app_check_mode()
{
	int   ptrsize = sizeof(void *);
	char  mode[8];

	strcpy(mode, (ptrsize == 4)?"32-bit":(ptrsize == 8)?"64-bit":"unknown");
	printf("Application in %s mode (ptrsize= %d bytes)\n", mode, ptrsize);
}







/** MAIN */
int cvmcs_app_bringup()
{
	int  i;

	/* Call the simple exec application init routine. This will do the
	   required initialization for linux and plain simple exec apps. */
	cvmx_user_app_init();
	core_id = cvmx_get_core_num();
	core_active[core_id] = 1;
	
	/* compute coremask_all on all cores for the first barrier sync below */
	appinfo = cvmx_sysinfo_get();
	coremask_all = appinfo->core_mask;

	cvmx_coremask_barrier_sync(coremask_all);

	cvmcs_get_boot_core(&boot_core);

	/* Save the frequency of the cpu for later use */
	cpu_freq       = appinfo->cpu_clock_hz;

	if(core_id == boot_core) {
		num_cores = cvmx_pop(coremask_all);
		printf("Numcores: %d\n", num_cores);
		for(i = 0; i < num_cores; i++)
			core_count += (coremask_all & (1 << i))?1:0;

		cvmcs_get_display_core(&display_core);
		cvmcs_app_check_mode();
		printf("# cvmcs: Cores are running at %llu Hz\n", cast64(cpu_freq));
		printf("# cvmcs: BOOT CORE: Core %d; DISPLAY CORE: Core %d \n",
		       boot_core, display_core);	
	}
	cvmx_coremask_barrier_sync(coremask_all);
	
#if !defined(linux)
	cvm_common_set_cycle(0);
#endif

	cvmx_coremask_barrier_sync(coremask_all);
	return 0;
}



void
cvmcs_app_barrier()
{
	cvmx_coremask_barrier_sync(coremask_all);
}



int
is_boot_core(uint32_t   core_id)
{
	return (core_id == boot_core);
}


int
is_display_core(uint32_t   core_id)
{
	return (core_id == display_core);
}


uint8_t  *
cvmcs_app_get_macaddr_base()
{
	if(appinfo)
		return appinfo->mac_addr_base;
	return NULL;
}


/* $Id: cvmcs-common.c 63107 2011-08-03 18:24:19Z panicker $ */
