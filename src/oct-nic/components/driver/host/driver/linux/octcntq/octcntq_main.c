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



#include "cavium_sysdep.h"
#include "cavium_defs.h"
#include "cavium_release.h"
#include "octcntq_config.h"
#include "octeon_cntq.h"
#include "octeon_ddoq.h"
#include "octeon_macros.h"
#include "octcntq_proc.h"

void cleanup_module(void);



void
print_driver_compile_options(void)
{
  char  copts[160];

  cavium_memset(copts, 0, 160);
#ifdef CAVIUM_DEBUG
  strcat(copts," DEBUG ");
#endif
#ifdef USE_BUFFER_POOL
  strcat(copts," BUFPOOL ");
#endif

  if(strlen(copts))
      cavium_print_msg("OCTCNTQ: Driver compile options: %s\n", copts);
  else
      cavium_print_msg("OCTCNTQ: Driver compile options: NONE\n");
}



int
octcntq_init_queues(int  octeon_id, void  *octeon_dev)
{
	int i;
	octeon_cntq_config_t    cntq_config[MAX_OCTEON_DMA_QUEUES];
	octeon_dma_ops_t        dma_ops;
#ifdef USE_DDOQ_THREADS
	octeon_device_t         *oct_dev = (octeon_device_t *)octeon_dev;
#endif

	cavium_print_msg("OCTEON_CNTQ: Initializing DMA Queues for Octeon%d\n",
	                 octeon_id);
	for(i = 0; i < MAX_OCTEON_DMA_QUEUES; i++) {
		cntq_config[i].size                = OCTEON_CNTQ_SIZE;
		cntq_config[i].pkt_count_threshold = DMA_PKT_CNT_THRESHOLD;
		cntq_config[i].intr_time_threshold = DMA_TIME_THRESHOLD;
		cntq_config[i].pkts_per_interrupt  = CNTQ_PKTS_PER_INTERRUPT;
		cntq_config[i].credit_threshold    = OCTEON_CNTQ_CREDIT_THRESHOLD;
	}

	for (i = 0; i < MAX_OCTEON_DMA_QUEUES; i++)  {
		if(octeon_init_cntq(octeon_id, i, (void *)&(cntq_config[i])))  {
			cavium_error("OCTEON_CNTQ: CNTQ initialization failed for Octeon%d\n", octeon_id);
			goto OCTCNTQ_INIT_FAIL;
		}
	}

	if(octeon_init_ddoq_list(octeon_id))  {
		cavium_error("OCTEON_CNTQ: Initialization failed for DDOQ list on Octeon%d\n", octeon_id);
		goto OCTCNTQ_INIT_FAIL;
	}

#ifdef USE_DDOQ_THREADS
	cvm_ddoq_num_threads = cavium_get_cpu_count();

	for (i = 0; i < cvm_ddoq_num_threads; i++) {
		cvm_ddoq_thread_t *thr = &(oct_dev->ddoq_thread[i]);

		/* Initialize to zero, it set all ddoq_id to zero */
		cavium_memset (thr, 0x0, sizeof(cvm_ddoq_thread_t));

	        cavium_init_wait_channel(&thr->wc);
		cavium_atomic_set(&thr->ddoq_pkts_queued, 0);
		thr->oct_dev = oct_dev;
		cavium_spin_lock_init(&thr->th_lock);
		thr->th_read_idx = -1;
		thr->th_write_idx = 0;
        	cavium_kthread_setup(&thr->thread, oct_ddoq_thread, thr,
                	             "Oct DDOQ Thread", 0);
	        if(cavium_kthread_create(&thr->thread))
        	        goto OCTCNTQ_INIT_FAIL;
	        cavium_kthread_set_cpu_affinity(&thr->thread,
        	                                (i % cavium_get_cpu_count()));
	        cavium_kthread_run(&thr->thread);
	}
#endif
	cavium_memset(&dma_ops, 0, sizeof(octeon_dma_ops_t));
	dma_ops.read_stats  = octcntq_proc_read_stats;
	dma_ops.read_statsb = octcntq_proc_read_statsb;
	octeon_enable_cntq_ops(octeon_id, &dma_ops);
	octcntq_init_proc(octeon_id);

	cavium_print_msg("OCTEON_CNTQ: Octeon%d DMA Queues are now ready\n",
	                 octeon_id);

	return 0;

OCTCNTQ_INIT_FAIL:
	cleanup_module();
	return  -ENODEV;
}



static int
octcntq_delete_queues(int  octeon_id, void *octeon_dev)
{
	int i;

	/* Remove the proc entry for CNTQ and DDOQ. */
	octcntq_delete_proc(octeon_id);

	/* Cleanup existing DDOQ's. */
	octeon_cleanup_active_ddoqs(octeon_id);

	/* Delete the ddoq list structure for this octeon device. */
	octeon_delete_ddoq_list(octeon_id);

	/* Disable CNTQ operations */
	octeon_disable_cntq_ops(octeon_id);

	/* Finally delete the CNTQ on host and core. */
	for (i = 0; i < MAX_OCTEON_DMA_QUEUES; i++)  {
		octeon_delete_cntq(octeon_id, i);
	}

#ifdef USE_DDOQ_THREADS
	octeon_ddoq_stop_threads(octeon_dev);
#endif

	return 0;
}





/* 
   Called by the module handler in base driver when the octeon device
   is being reset. 
*/
int
octcntq_reset_queues(int   octeon_id, void  *octeon_dev)
{
	uint64_t   active_ddoqs;

	cavium_print_msg("OCTEON_CNTQ: Reset DMA Queues for Octeon%d\n",
	                 octeon_id);

	if(CVM_MOD_IN_USE)
		return 1;

	octeon_change_cntq_status(octeon_id, CNTQ_DISABLE);

	active_ddoqs = octeon_get_active_ddoq_count(octeon_id);
	if((active_ddoqs != CVM_CAST64(~0)) && active_ddoqs) {
		cavium_error("OCTEON_CNTQ: There are %llu active DDOQ's. Cannot reset CNTQ module\n", CVM_CAST64(active_ddoqs));
		octeon_change_cntq_status(octeon_id, CNTQ_ENABLE);
		return 1;
	} 
	return octcntq_delete_queues(octeon_id, octeon_dev);
}





/* 
   Called by the module handler in base driver when the CNTQ module is
   unloaded.
*/
int
octcntq_stop_queues(int   octeon_id, void  *octeon_dev)
{
	uint64_t   active_ddoqs;

	cavium_print_msg("OCTEON_CNTQ: Stopping DMA Queues for Octeon%d\n",
	                 octeon_id);

	octeon_change_cntq_status(octeon_id, CNTQ_DISABLE);

	active_ddoqs = octeon_get_active_ddoq_count(octeon_id);
	if((active_ddoqs != CVM_CAST64(~0)) && active_ddoqs) {
		cavium_error("OCTEON_CNTQ: Active DDOQ's: %llu\n",
		             CVM_CAST64(active_ddoqs));
	}

	octcntq_delete_queues(octeon_id, octeon_dev);

	cavium_print_msg("OCTEON_CNTQ: Stopped DMA Queues for Octeon%d\n",
	                 octeon_id);
	return 0;
}





int
init_module(void)
{
	int retval = 0;
	const char *cntq_cvs_tag = "$Name: PCI_CNTQ_RELEASE_2_2_0_build_84 ";
	char        cntq_version[80];
	octeon_module_handler_t   dmaq;

	cavium_print_msg("-- OCTEON_CNTQ: Starting CNTQ module for Octeon\n");
	cavium_parse_cvs_string(cntq_cvs_tag, cntq_version, 80);
	cavium_print_msg("Version: %s\n", cntq_version);
	print_driver_compile_options();

	dmaq.startptr = octcntq_init_queues;
	dmaq.resetptr = octcntq_reset_queues;
	dmaq.stopptr  = octcntq_stop_queues;
	dmaq.app_type = CVM_DRV_CNTQ_APP;
	retval = octeon_register_module_handler(&dmaq);
	cavium_print_msg("-- OCTEON_CNTQ: CNTQ module loaded for Octeon\n");
	return retval;
}



void
cleanup_module()
{
	cavium_print_msg("-- OCTEON_CNTQ: Preparing to unload CNTQ module for Octeon\n");
	octeon_unregister_module_handler(CVM_DRV_CNTQ_APP);
	cavium_print_msg("-- OCTEON_CNTQ: Stopped CNTQ module for Octeon\n");
}




EXPORT_SYMBOL(octeon_cntq_get_stats);

EXPORT_SYMBOL(octeon_ddoq_create);
EXPORT_SYMBOL(octeon_ddoq_delete);
EXPORT_SYMBOL(octeon_ddoq_list_get_stats);
EXPORT_SYMBOL(octeon_ddoq_get_stats);
EXPORT_SYMBOL(octeon_ddoq_change_tag);
EXPORT_SYMBOL(octeon_ddoq_restart_rx);

/* $Id: octcntq_main.c 67088 2011-11-15 19:39:17Z mchalla $ */
