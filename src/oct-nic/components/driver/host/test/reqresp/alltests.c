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


#include "octeon_req_perf.h"

extern struct __test_info  tinfo;

extern struct __seq_params  tseq;

extern struct __test_stats  tstats;


extern void  print_info_test_setup(void);

extern struct test_list * init_test_req_list(int *count, int max_reqs);

extern int  free_test_req_list(struct test_list  *tl, int max_reqs);



extern void print_test_results(void);




#ifdef  __KERNEL__
	OCTEON_RESPONSE_ORDER default_resp_order = OCTEON_RESP_ORDERED;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	extern cavium_threadid_t   reqresp_pid;
#else
	extern cavium_ostask_t    *reqresp_task;
#endif
	extern void oct_perf_kern_stop(void);
	extern int  perf_test_loop(void *arg);

#else
	OCTEON_RESPONSE_ORDER default_resp_order = OCTEON_RESP_UNORDERED;
	extern int  perf_test_loop(struct test_list *tl);
#endif


int
create_req_list_and_execute(void)
{
	int                count;
	int                test_pkt_count;
	struct test_list  *tl;

	print_info_test_setup();

	tl = init_test_req_list(&count, MAX_REQUESTS);
	if(tl == NULL) {
		PRINT_ERR("Request List allocation failed\n");
		return -ENOMEM;
	}

	if(count != MAX_REQUESTS) {
		free_test_req_list(tl, count);
		return -ENOMEM;
	}

	test_pkt_count = tinfo.pkt_cnt;

	memset(&tstats, 0, sizeof(tstats));
	tstats.u_nb.mindata = tstats.u_b.mindata = tstats.o.mindata =
	tstats.nr.mindata = (uint64_t)-1;

	tstats.u_nb.minbufs = tstats.u_b.minbufs = tstats.o.minbufs =
	tstats.nr.minbufs = (uint64_t)-1;


#ifdef  __KERNEL__

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	reqresp_pid = kernel_thread(perf_test_loop, tl,
                            CLONE_FS|CLONE_FILES|CLONE_SIGHAND);
	if (reqresp_pid < 0)
		goto init_thread_failed;
#else
	reqresp_task = kthread_run(perf_test_loop, tl,"Req/resp Test Thread");
	if (reqresp_task == NULL)
		goto alltest_thread_failed;

#endif
	while(tinfo.pkt_cnt)
		cavium_sleep_timeout(1);
	oct_perf_kern_stop();
	free_test_req_list(tl, MAX_REQUESTS);
#else
	perf_test_loop(tl);
	free_test_req_list(tl, MAX_REQUESTS);
	print_test_results();
#endif

	count = tstats.u_nb.send_passed + tstats.u_b.send_passed
	        + tstats.o.send_passed + tstats.nr.send_passed;
	if(count != test_pkt_count) {
		PRINT_ERR("%s There were %d send failures\n", __FUNCTION__,
		          (test_pkt_count - count));
		return 1;
	}

	count = tstats.u_nb.resp_timeout + tstats.u_b.resp_timeout
	        + tstats.o.resp_timeout;
	count += tstats.u_nb.resp_error + tstats.u_b.resp_error
	        + tstats.o.resp_error;
	if(count) {
		PRINT_ERR("%s There were %d response failures\n", __FUNCTION__, count);
		return 1;
	}

	return 0;

#ifdef  __KERNEL__
alltest_thread_failed:
	printk("Failed to create test thread\n");
	free_test_req_list(tl, MAX_REQUESTS);
	return -ENOMEM;
#endif
}




int
oct_perf_run_test_loop(uint32_t  max_data, uint32_t  pkt_cnt)
{
	do {
		tinfo.min_datasize = tinfo.max_datasize;
		tinfo.pkt_cnt      = pkt_cnt;
		tinfo.max_datasize = tinfo.min_datasize + MAX_REQUESTS - 1;
		if(tinfo.max_datasize > max_data)
			tinfo.max_datasize = max_data;
		tseq.bufcnt.curr   = tinfo.min_bufs;
		tseq.bufcnt.incr   = 1;
		tseq.datasize.curr = tinfo.min_datasize;
		tseq.datasize.incr = 1;

		if(create_req_list_and_execute()) {
			PRINT_ERR("%s:%d Test failed\n", __FUNCTION__, __LINE__);
			return 1;
		}
#ifdef __KERNEL__
		cavium_sleep_timeout(1);
#else
		usleep(1000);
#endif
	} while (tinfo.max_datasize < max_data);

	return 0;
}




int
oct_perf_run_all_tests(void)
{
	uint32_t    pkt_cnt = 1024;

	/* default_resp_order is UNORDERED for user and ORDERED for kernel */

	tinfo.resp_order = default_resp_order;
	tinfo.resp_mode  = OCTEON_RESP_NON_BLOCKING;
	tinfo.datagen    = SEQUENTIAL;

	tinfo.dma_mode   = OCTEON_DMA_DIRECT;
	tinfo.min_bufs   = 1;
	tinfo.max_bufs   = 1;
	tinfo.max_datasize = 1;

	if(oct_perf_run_test_loop(OCT_MAX_DIRECT_INPUT_DATA_SIZE, pkt_cnt))
		return 1;


	tinfo.dma_mode   = OCTEON_DMA_SCATTER_GATHER;
	tinfo.resp_order = default_resp_order;
	tinfo.min_bufs   = 1;
	tinfo.max_bufs   = 15;
	tinfo.max_datasize = 1;

	if(oct_perf_run_test_loop(OCT_MAX_GATHER_DATA_SIZE, pkt_cnt))
		return 1;

	return 0;
};


/* $Id: alltests.c 59434 2011-04-21 22:04:27Z panicker $ */

