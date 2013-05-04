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
#include "cvmcs-test.h"
#include "cvm-cntq.h"
#include "cvm-ddoq.h"
#include "cvmcs-cntq-defs.h"
#include "cvmcs-cntq-test.h"
#include <errno.h>

#ifdef __linux__
void (*prev_sig_handler)(int);
#endif



extern uint32_t  core_id;
extern CVMX_SHARED    uint64_t  cpu_freq;
CVMX_SHARED uint32_t wqe_count[MAX_CORES]
  = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

CVMX_SHARED int ok_to_test_ddoq = 0;


/** Setup the FPA pools. The Octeon hardware, simple executive and
  * PCI core driver use  WQE and Packet pool. OQ pool is used to
  * allocate command buffers for Output queue by simple exec.
  * Test pool is used by this application. 
  */
int
cvmcs_cntq_setup_memory()
{

	cvmx_fpa_enable();

	if( cvmcs_app_mem_alloc("Packet Buffers", CVMX_FPA_PACKET_POOL,
	                         CVMX_FPA_PACKET_POOL_SIZE, FPA_PACKET_POOL_COUNT))
		return 1;

	if( cvmcs_app_mem_alloc("Work Queue Entries", CVMX_FPA_WQE_POOL,
	                         CVMX_FPA_WQE_POOL_SIZE, FPA_WQE_POOL_COUNT))
		return 1;

	if( cvmcs_app_mem_alloc("PKO Command Buffers",CVMX_FPA_OUTPUT_BUFFER_POOL,
	                     CVMX_FPA_OUTPUT_BUFFER_POOL_SIZE, FPA_OQ_POOL_COUNT))
		return 1;

	if( cvmcs_app_mem_alloc("Test Buffers", CVM_FPA_TEST_POOL,
	                        CVM_FPA_TEST_POOL_SIZE, FPA_TEST_POOL_COUNT))
		return 1;

	if(OCTEON_IS_MODEL(OCTEON_CN68XX) && (cvmx_helper_initialize_sso(FPA_WQE_POOL_COUNT)))
       return -1;

	return 0;
}






/** Global initialization. Performed by the boot core only. */
int
cvmcs_cntq_init_global()
{
	if(cvmcs_cntq_setup_memory())
		return 1;

	if(cvmcs_app_init_global())
		return 1;

	cvm_drv_setup_dma_queues();

	cvmx_ipd_enable();

	/* Enable pko here */
	cvmx_pko_enable();

	return 0;
}





/** Local initialization. Performed by all cores. */
int
cvmcs_cntq_init_local()
{
	cvmx_scratch_write64(CVMCS_TEST_BUF_PTR,  CVM_COMMON_ALLOC_INIT);
	CVMX_SYNCW;
	cvmx_fpa_async_alloc(CVMCS_TEST_BUF_PTR, CVM_FPA_TEST_POOL);

	if(cvmcs_app_init_local())
		return 1;

	CVMX_SYNCW;
	return 0;
}






/** All work received by the application is forwarded to this routine.
  * All RAW packets with opcode=0x1234 and param=0x10 are test instructions
  * and handle by the application. All other RAW packets with opcode in
  * the range 0x1000-0x1FFF is given to the core driver. All other packets
  * are dropped.
  */
static inline int
cvmcs_cntq_process_wqe(cvmx_wqe_t  *wqe)
{
	int ipprt = cvmx_wqe_get_port(wqe);

#ifdef CVM_PCI_TRACK_DMA
 	if(ipprt == CVM_PCI_DMA_TRACKER_PORT) {
		cvm_pci_dma_remove_from_tracker_list(wqe);
		return 0;
	}
#endif


	if(ipprt >= 16 && ipprt <= 19) {
		cvm_free_host_instr(wqe);
		return 0;
	} 

	if(wqe->word2.s.software)   {

		cvmx_raw_inst_front_t  *front=(cvmx_raw_inst_front_t *)wqe->packet_data;
		uint32_t                opcode;

		opcode = front->irh.s.opcode;

		if(opcode >= DRIVER_OP_START && opcode <= DRIVER_OP_END) {
			cvm_drv_process_instr(wqe);
		} else {
			if(cvmcs_test_process_wqe(wqe)) {
				cvm_free_host_instr(wqe);
			}
		}

	} else  {

		printf("Unsupported WQE format @ %p\n", wqe);
		cvm_drv_print_data(wqe, 64);
		cvm_free_host_instr(wqe);

	}
	return 0;
}








/** This loop is run by all cores running the application.
  * If any work is available it is processed. If there is no work
  * and 
  * If CVMCS_DUTY_CYCLE is enabled, it prints useful statistics
  * at intervals determined by DISPLAY_INTERVAL.
  * If CVMCS_TEST_PKO is enabled, then packets are sent to PKO 
  * port 32 at intervals determined by "cvmcs_run_freq".
  * If CVMCS_FIXED_SIZE_TEST is enabled, packets of fixed size
  * are sent. Else packet size can be of range 1-CVM_MAX_DATA
  * where CVM_MAX_DATA cannot be > 65520 bytes.
  */
int
cvmcs_cntq_data_loop()
{
	cvmx_wqe_t    *wqe;
	uint64_t       last_update=0, last_ddoq_run=0;
	uint64_t       idle_counter = 0, pko_test_counter=0;
#ifdef CVMCS_TEST_CNTQ
	uint64_t       last_cntq_run=0;
#endif
	int            ddret=0;

	printf("# cvmcs-cntq: Data loop started on core[%d]\n", core_id);
	do {

		wqe = (cvmx_wqe_t *)cvmcs_app_get_work_sync(0);
		if (wqe) {
			wqe_count[core_id]++;
			cvmcs_cntq_process_wqe(wqe);
			continue;
		}

		idle_counter++;
		pko_test_counter++;

	/* There is no definition of model PASS2_0 in SDK 1.8.0; check for 
	   model CN56XX_PASS2 returns true for CN56XX Pass2.0 parts only.
	   In SDK 1.8.1, check for model CN56XX_PASS2 returns true
	   for all CN56XX Pass 2.X parts, so use model CN56XX_PASS2_0 instead.
	*/
#if (OCTEON_SDK_VERSION_NUM < 108010000ULL)
		if(OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2))
#else
		if(OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2_0))
#endif
			cvm_56xx_pass2_update_pcie_req_num();

		if(is_display_core(core_id)) {
			
#ifdef  CVMCS_DUTY_CYCLE
			if( (cvmx_get_cycle() - last_update)
				 >= (cpu_freq * DISPLAY_INTERVAL)) {

				cvmcs_app_duty_cycle_actions();
#ifdef CVM_DDOQ_PROFILE
				cvm_drv_print_ddoq_profile(DISPLAY_INTERVAL);
#endif
				last_update = cvmx_get_cycle();
			}
#endif
		}

#ifdef CVMCS_TEST_CNTQ
		if( (cvmx_get_cycle() - last_cntq_run)
		     >= (cpu_freq >> CNTQ_TEST_SHIFT_FREQ))
		{
			cvmcs_do_cntq_test();
			last_cntq_run = cvmx_get_cycle();
		}
#endif

		/* ddret stores the return value of the last run. Continue test only
		  if there were no failures. */
		if(!ddret && ((cvmx_get_cycle() - last_ddoq_run)
		     >= (cpu_freq >> CNTQ_TEST_SHIFT_FREQ)) )
		{
			ddret = (cvmcs_do_ddoq_test() == -ENOMEM);
			last_ddoq_run = cvmx_get_cycle();
		}
	} while(1);
	printf("# cvmcs-cntq: Core %d Exited from data loop\n", core_id);
}







#ifdef __linux__
void signal_handler(int x)
{
        printf("# cvmcs: Received signal %d, quitting now!!\n", x);
        signal(SIGINT, prev_sig_handler );
        cvmcs_app_shutdown();
        exit(0);
}
#endif




int turntestoff(uint32_t tag, uint32_t ddoq_id, void *ptr)
{
	if(ddoq_id == 36) {
		ok_to_test_ddoq = 0;
		CVMX_SYNCWS;
	}
	return 0;
}


int turnteston(uint32_t tag, uint32_t ddoq_id, void *ptr)
{
	if(ddoq_id == 36) {
		ok_to_test_ddoq = 1;
		CVMX_SYNCWS;
	}
	return 0;
}



/** MAIN */
int main()
{
	if(cvmcs_app_bringup())
		return 1;

	if(is_boot_core(core_id)) {

		printf("\n# cvmcs-cntq: Starting global init on core %d\n", core_id);
		if(cvmcs_cntq_init_global()) 
			return 1;
		printf("# cvmcs-cntq: Global initialization completed\n\n");
	}

	cvmcs_app_barrier();
	
	/* Initialization local to each core */
	cvmcs_cntq_init_local();

	if(is_boot_core(core_id)) {

#ifdef __linux__
		prev_sig_handler = signal(SIGINT, signal_handler);
#endif

		cvmx_helper_setup_red(2000, 1000);

		cvm_drv_setup_app_mode(CVM_DRV_CNTQ_APP);
		cvm_drv_start();

		print_pool_count_stats();

	}

	cvmcs_app_barrier();
	
	cvmcs_init_ddoq_test();

	if(is_boot_core(core_id)) {
		cvm_ddoq_setup_appcallback(DDOQ_TYPE_TEST, DDOQ_ACTION_INIT, turnteston);
		cvm_ddoq_setup_appcallback(DDOQ_TYPE_TEST, DDOQ_ACTION_DELETE, turntestoff);
	}


	/* Start the data processing loop on each core. */
	cvmcs_cntq_data_loop();

	cvmcs_app_barrier();

	if(is_boot_core(core_id))
		cvmcs_app_shutdown();

	return 0;
}


/* $Id: cvmcs-cntq-main.c 63048 2011-08-01 22:41:47Z panicker $ */
