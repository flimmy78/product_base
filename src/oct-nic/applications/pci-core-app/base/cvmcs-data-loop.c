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
extern CVMX_SHARED    uint64_t  cpu_freq;

CVMX_SHARED  int       cvmcs_test_ok = 1;

CVMX_SHARED uint32_t min_len = -1, max_len = 0;


unsigned long  last_wqe_cnt = 0, last_wqe_len = 0;

void
__cvmcs_print_wqe_count(void)
{
	uint64_t   twqe_cnt=0, twqe_len = 0;
	uint64_t   lwqe_cnt=0, lwqe_len = 0;

	twqe_cnt =  cvmx_read64_int64(__cvmx_fau_store_address(0, CVM_FAU_WQE_COUNT));
	twqe_len =  cvmx_read64_int64(__cvmx_fau_store_address(0, CVM_FAU_WQE_LEN));

	lwqe_cnt = (twqe_cnt - last_wqe_cnt);
	lwqe_len = (twqe_len - last_wqe_len);

	printf("WQE: %llu Len: %llu, (Last %llu wqe %llu bps (avg payload: %llu bytes))\n",
	      cast64(twqe_cnt), cast64(twqe_len),
	      cast64(lwqe_cnt), cast64((lwqe_len) * 8),
	      cast64((lwqe_cnt)?(lwqe_len/lwqe_cnt):0ULL));
	last_wqe_cnt = twqe_cnt;
	last_wqe_len = twqe_len;
}




int
cvmcs_duty_cycle_actions(void  *arg)
{
#ifdef  CN56XX_PEER_TO_PEER
	printf("\n\n Octeon device %d\n", octdev_get_device_id());
	cn56xx_print_ep_pkt_count();
	for(i = 0; i < octdev_get_max_peers(); i++)
		cn56xx_print_peer_iq_stats(i);
#endif

#ifdef DMA_PERF_TEST
	cvmcs_print_dma_perf_stats();
#endif

	cvmcs_app_duty_cycle_actions();
	__cvmcs_print_wqe_count();

	return 0;
}




#ifdef DMA_PERF_TEST 
extern CVMX_SHARED int dma_perf_run_ok;
#endif


/** All work received by the application is forwarded to this routine.
  * All RAW packets with opcode=0x1234 and param=0x10 are test instructions
  * and handle by the application. All other RAW packets with opcode in
  * the range 0x1000-0x1FFF is given to the core driver. All other packets
  * are dropped.
  */
static inline int
cvmcs_process_wqe(cvmx_wqe_t  *wqe)
{
	cvmx_raw_inst_front_t     *front;
	uint32_t                   opcode, len, port;

#ifdef CVM_PCI_TRACK_DMA
	if(wqe->ipprt == CVM_PCI_DMA_TRACKER_PORT) {
		cvm_pci_dma_remove_from_tracker_list(wqe);
		return 0;
	}
#endif

	len = cvmx_wqe_get_len(wqe);

	if(wqe->word2.s.software)
		len -= 24;


	cvmx_fau_atomic_add64(CVM_FAU_WQE_COUNT, 1);
	cvmx_fau_atomic_add64(CVM_FAU_WQE_LEN, len);

	if(len < min_len)
		min_len = len;
	if(len > max_len)
		max_len = len;
	
	//cvm_drv_print_wqe(wqe);

	port = cvmx_wqe_get_port(wqe);

//	printf("Received wqe @ %p ipprt: %d pkind: %d len: %d\n", wqe, port, wqe->word0.pip.cn68xx.pknd, len);

//	cvm_drv_print_data(wqe, 64);

	if(OCTEON_IS_MODEL(OCTEON_CN68XX)) {
		if (port < 0x100 || port > 0x11F) {
			cvm_free_host_instr(wqe);
			return 0;
		}
	} else {
		if(port < 32 || port > 35) {
			cvm_free_host_instr(wqe);
			return 0;
		}
	} 


	if(wqe->word2.s.software)   {
		/* Check if its a instruction for the core driver. */
		front = (cvmx_raw_inst_front_t *)wqe->packet_data;
		opcode = front->irh.s.opcode;


		if(opcode >= DRIVER_OP_START && opcode <= DRIVER_OP_END) {
			if(opcode == DEVICE_STOP_OP) {
#ifdef DMA_PERF_TEST 
				dma_perf_run_ok = 0;
#endif
				cvmcs_test_ok = 0;
				CVMX_SYNCWS;
			}
			cvm_drv_process_instr(wqe);
		} else {
			if(cvmcs_test_process_wqe(wqe))
				cvm_free_host_instr(wqe);
		}
	} else  {
		printf("Unsupported WQE format @ %p\n", wqe);
		cvm_drv_print_data(wqe, 64);
		cvm_free_host_instr(wqe);
	}
	return 0;
}




#ifdef TEST_PEER_TO_PEER
CVMX_SHARED   uint64_t   last_ep_pkt_sent=0;


void 
cvmcs_send_ep_test_pkt(void)
{
	int i;


	if((cvmx_get_cycle() - last_ep_pkt_sent) >= (cpu_freq >> TEST_SHIFT_FREQ)) {
		last_ep_pkt_sent = cvmx_get_cycle();
		for(i = 0; i < oct->max_peers; i++)
			cn56xx_send_ep_test_pkt_to_peer(i);
	}

}
#endif






void
cvmcs_print_test_settings(void)
{
#if defined(MAX_PACKET_RATE_TEST)
	/* Max rate test over-rides PKO test, so check for max rate first. */
	printf(">>> Max Packet rate PKO test enabled\n");
#elif defined(CVMCS_TEST_PKO)
	printf(">>> PKO test enabled (Data size Min: %d Max: %d)\n",
	       CVM_MIN_DATA, CVM_MAX_DATA);
#endif

#if defined(CVMCS_DMA_DEMO)
	printf(">>> DMA Demo test application enabled\n");
#endif

#ifdef DMA_PERF_TEST 
	printf(">>> DMA performance test enabled\n");
	if(DISPLAY_INTERVAL > 1) {
		printf("Display Interval is %d currently. Set this to 1.\n",
		       DISPLAY_INTERVAL);
	}
	printf("DMA Cycle counts are useful only if 1 pending command on 1 core is used\n");
#endif

#if defined(TEST_PEER_TO_PEER)
	printf("Endpoint to Endpoint Forwarding test enabled\n");
#endif
}







void
cvmcs_display_loop(void)
{
	printf("# cvmcs: Display loop started on core[%d]\n", core_id);
	printf("\n\n >>>>>> CORE %d WILL NOT PROCESS WQE  <<<<<<\n\n", core_id);

	cvmcs_print_test_settings();

	while(1) {
#if !defined(DISABLE_PCIE14425_ERRATAFIX)
		cvmcs_sw_iq_bp();
#endif
		cvmcs_common_schedule_tasks();
	}
}







int
cvmcs_data_loop()
{

	printf("# cvmcs: Data loop started on core[%d]\n", core_id);

	cvmcs_app_barrier();

	if(is_display_core(core_id)) {
		cvmcs_common_add_task(cpu_freq * DISPLAY_INTERVAL, cvmcs_duty_cycle_actions, NULL);
		printf("\n# cvmcs: Statistics monitored by core %d\n\n", core_id);
		cvmcs_print_test_settings();
	}

	do {
		cvmx_wqe_t    *wqe = cvmcs_app_get_work_sync(0);
		if (wqe == NULL)
			goto nowqe_actions;

		//	printf("Received wqe @ %p ipprt: %d\n", wqe, wqe->ipprt);

		cvmcs_process_wqe(wqe);

nowqe_actions:
		#if defined(CVMCS_TEST_PKO) || defined(MAX_PACKET_RATE_TEST)
		cvmcs_do_pko_test();
		#endif

		if(OCTEON_IS_MODEL(OCTEON_CN56XX_PASS2_0)) {
			cvm_56xx_pass2_update_pcie_req_num();
		}

		#ifdef TEST_PEER_TO_PEER
		if(core_id == 0)
			cvmcs_send_ep_test_pkt();
		#endif

		if(is_display_core(core_id)) {
			#if !defined(DISABLE_PCIE14425_ERRATAFIX)
			cvmcs_sw_iq_bp();
			#endif
			cvmcs_common_schedule_tasks();
		}

	} while(cvmcs_test_ok);


	printf("# cvmcs: Core %d Exited from data loop\n", core_id);
	return 0;
}




/* $Id: cvmcs-data-loop.c 63107 2011-08-03 18:24:19Z panicker $ */




