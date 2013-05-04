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
#include "cvmcs-defs.h"
#include "cvmcs-test.h"


extern uint32_t  core_id;
extern CVMX_SHARED uint64_t  cpu_freq;
extern CVMX_SHARED uint32_t  core_count;
extern int       cvmcs_data_loop();
extern void      cvmcs_display_loop();

extern int  cvmcs_duty_cycle_actions(void  *arg);

#ifdef __linux__
void (*prev_sig_handler)(int);
#endif



void
cvmcs_print_compile_options()
{
	printf("Application compiled with: ");
	cvmcs_test_print_options();
}






/** Setup the FPA pools. The Octeon hardware, simple executive and
  * PCI core driver use  WQE and Packet pool. OQ pool is used to
  * allocate command buffers for Output queue by simple exec.
  * Test pool is used by this application. 
  */
int
cvmcs_setup_memory()
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

	if(OCTEON_IS_MODEL(OCTEON_CN68XX) && (cvmx_helper_initialize_sso(FPA_SSO_POOL_COUNT)))
       return -1;

	return 0;
}






/** Global initialization. Performed by the boot core only. */
int
cvmcs_init_global()
{
	if(cvmcs_setup_memory())
		return 1;

	if(cvmcs_app_init_global())
		return 1;

	cvmcs_test_global_init();

	cvmx_ipd_enable();

	/* Enable pko here */
	cvmx_pko_enable();

	return 0;
}







/** Local initialization. Performed by all cores. */
int
cvmcs_init_local()
{
	cvmx_scratch_write64(CVMCS_TEST_BUF_PTR,  CVM_COMMON_ALLOC_INIT);
	CVMX_SYNCW;
	cvmx_fpa_async_alloc(CVMCS_TEST_BUF_PTR, CVM_FPA_TEST_POOL);

	if(cvmcs_app_init_local())
		return 1;

	CVMX_SYNCW;
	return 0;
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






/** MAIN */
int main()
{

	if(cvmcs_app_bringup())
		return 1;

	if(is_boot_core(core_id)) {
		printf("SDK Build Number: %s\n", SDKVER);
		cvmcs_print_compile_options();
		printf("\n# cvmcs: Starting global initialization on core %d\n", core_id);
		if(cvmcs_init_global())
			return 1;
		printf("# cvmcs: Global initialization completed\n\n");
	}
	cvmcs_app_barrier();
	
	/* Initialization local to each core */
	cvmcs_init_local();

	printf("# cvmcs: Core [%d]: Local initialization completed\n",core_id);

	if(is_boot_core(core_id)) {
#ifdef __linux__
		prev_sig_handler = signal(SIGINT, signal_handler);
#endif
		cvm_drv_setup_app_mode(CVM_DRV_BASE_APP);
		printf("Boot Core: Calling cvm_drv_start()\n");
		cvm_drv_start();
		print_pool_count_stats();
	/*	cvmcs_print_reg_contents();*/
	}

	cvmcs_app_barrier();

	/* Start the data processing loop on each core. */
	cvmcs_data_loop();

	cvmcs_app_barrier();

	if(is_boot_core(core_id))
		cvmcs_app_shutdown();

	return 0;
}


/* $Id: cvmcs.c 63107 2011-08-03 18:24:19Z panicker $ */
