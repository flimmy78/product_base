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


#ifndef __KERNEL__
#include <sys/time.h>
#include <signal.h>
#endif

#include "octeon_req_perf.h"



struct __test_info  tinfo = 
	{	0,                      // DMA Type
#ifdef  __KERNEL__
		OCTEON_RESP_ORDERED,         // Kernel Mode Response Order
#else
		OCTEON_RESP_UNORDERED,       // User Mode Response Order
#endif
		OCTEON_RESP_NON_BLOCKING,    // Response Mode

		0,                           // Octeon Device Id
		0,                           // Octeon Input Queue Number
		1024,                        // Number of requests to send

		SEQUENTIAL,                  // Data Generation mode
		1,                           // Minimum Buffer count
		15,                           // Maximum Buffer count
		1,                           // Minimum Buffer size
		(2 * 1024),                        // Maximum Buffer size
	};



struct __seq_params  tseq;
struct __test_stats  tstats;
volatile int         non_stop=0;


extern int   process_and_verify_args(void);
extern int  oct_perf_run_all_tests(void);



int run_all_tests=0;




#ifdef  __KERNEL__   /* Kernel mode definitions */
module_param(run_all_tests, int, 0);

MODULE_LICENSE("Cavium Networks");

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
cavium_threadid_t   reqresp_pid=-1;
#else
cavium_ostask_t    *reqresp_task = NULL;
#endif

struct test_list  *kern_tl = NULL;
cavium_atomic_t    ord_reqs_pending;

#else              /* User mode definitions */

int                     signal_to_catch=SIGINT;
void (*prev_sig_handler)(int);
extern int   parse_args(int argc, char **argv);

#endif


void
print_soft_request(octeon_soft_request_t   *sr);

void
oct_perf_kern_stop(void);


struct test_pkt *
get_next_request(struct test_list  *tl)
{
	struct test_node    *node = NULL;
	struct test_pkt     *pkt  = NULL;

	node = get_next_node(tl);
	if(node) {
		pkt = (struct test_pkt *)node;
		pkt->pkt_state  = TREQ_PKT_USED;
	}
	return pkt;
}





int
put_free_request(struct test_list *tl, struct test_pkt *pkt)
{
	if(pkt->pkt_state != TREQ_PKT_USED) {
		PRINT_ERR(" %s Error: pkt in list has status %d\n",
		       __FUNCTION__, pkt->pkt_state);
		return 1;
	}
	pkt->pkt_state = TREQ_PKT_UNUSED;
	return  put_free_node(tl, (struct test_node *)pkt);
}




void
print_mismatch_buffers(uint8_t *a, uint8_t *b, int l)
{
	int i = 0, prints=0;

	PRINT_ERR("Printing upto 8 bytes of mismatch data\n");
	while( (prints < 8) && (i < l)) {
		if(a[i] != b[i]) {
			PRINT_ERR("Byte %d:  sent %d received: %d\n", i, a[i], b[i]);
			prints++;
		}
		i++;
	}

}



void
verify_data(octeon_soft_request_t  *r)
{
	int       incnt=0, outcnt=0;
	uint32_t  this_ibuf_size, this_obuf_size;
	uint8_t  *inptr, *outptr;

	outptr = SOFT_REQ_OUTBUF(r, 0);
	this_obuf_size = r->outbuf.size[0];

	inptr = SOFT_REQ_INBUF(r, 0);
	this_ibuf_size  = r->inbuf.size[0];

	while ( incnt < r->inbuf.cnt  &&  outcnt < r->outbuf.cnt) {
		uint32_t   check_size;

		#ifdef __KERNEL__
		if(outcnt == 0) {
			this_obuf_size -= 8;
			outptr     += 8;
		}
		if(outcnt == (r->outbuf.cnt - 1))
			this_obuf_size -= 8;
		#endif

		if(this_obuf_size > this_ibuf_size)
			check_size = this_ibuf_size;
		else
			check_size = this_obuf_size;

		if(memcmp(inptr, outptr, check_size)) {
			PRINT_ERR("Data mismatch inbuf[%d] @ %p, outbuf[%d] @ %p\n",
			         incnt, inptr, outcnt, outptr);
			print_mismatch_buffers(inptr, outptr, check_size);
			return;
		}

		this_ibuf_size -= check_size;
		this_obuf_size -= check_size;

		inptr  += check_size;
		outptr += check_size;

		if(this_ibuf_size == 0) {
			incnt++;
			if(incnt < r->inbuf.cnt) {
				inptr           = SOFT_REQ_OUTBUF(r, incnt);
				this_ibuf_size  = r->inbuf.size[incnt];
			}
		}

		if(this_obuf_size == 0) {
			outcnt++;
			if(outcnt < r->outbuf.cnt) {
				outptr          = SOFT_REQ_OUTBUF(r, outcnt);
				this_obuf_size  = r->outbuf.size[outcnt];
			}
		}

	}

}






void
print_soft_request(octeon_soft_request_t   *sr)
{
	int j;

	PRINT_MSG("%s req @ %p inbufcnt: %d outbufcnt: %d\n", __FUNCTION__, sr,
	       sr->inbuf.cnt, sr->outbuf.cnt);
	for(j = 0; j < sr->inbuf.cnt; j++)
		PRINT_MSG("Inbuf[%d]: %dB @ %p\n", j, sr->inbuf.size[j],
		       SOFT_REQ_INBUF(sr, j));
	for(j = 0; j < sr->outbuf.cnt; j++)
		PRINT_MSG("Outbuf[%d]: %dB @ %p\n", j, sr->outbuf.size[j],
		       SOFT_REQ_OUTBUF(sr, j));

}




static inline void
update_test_stats(struct test_pkt       *pkt,
                  struct __test_stats   *teststats,
                  int                    update)
{
	struct __unit_test_stats  *stat;
	int                        dmatype = GET_SOFT_REQ_DMA_MODE(&pkt->r);
	int                        i, total_bytes=0;

	switch(GET_SOFT_REQ_RESP_ORDER(&pkt->r)) {
		case OCTEON_RESP_UNORDERED:
			if(GET_SOFT_REQ_RESP_MODE(&pkt->r) == OCTEON_RESP_BLOCKING)
				stat = &teststats->u_b;
			else
				stat = &teststats->u_nb;
			break;
		case OCTEON_RESP_ORDERED:
			stat = &teststats->o;
			break;
		case OCTEON_RESP_NORESPONSE:
			stat = &teststats->nr;
			break;
		default: PRINT_ERR("%s Unknown response order (%d) in request\n",
		                   __FUNCTION__, GET_SOFT_REQ_RESP_ORDER(&pkt->r));
			return;
	}

	if(pkt->r.inbuf.cnt < stat->minbufs)
		stat->minbufs = pkt->r.inbuf.cnt;

	if(pkt->r.inbuf.cnt > stat->maxbufs)
		stat->maxbufs = pkt->r.inbuf.cnt;

	for(i = 0; i < pkt->r.inbuf.cnt; i++)
		total_bytes += pkt->r.inbuf.size[i];
	if(total_bytes > stat->maxdata)
		stat->maxdata = total_bytes;
	if(total_bytes < stat->mindata)
		stat->mindata = total_bytes;

	if(update & UPDATE_SEND) {
		stat->dmacnt[dmatype]++;
		if(pkt->send_state == TREQ_SEND_SUCCESS) {
			stat->send_passed++;
		} else {
			PRINT_MSG("Send Request returned status: %d\n", pkt->send_state);
			stat->send_failed++;
		}

		if(stat->send_failed >= MAX_FAILURES) {
			PRINT_ERR("Too many send failures. Stopping test\n");
			tinfo.pkt_cnt = 0; non_stop = 0;
		}
	}

	if(update & UPDATE_RESP) {
		if(pkt->rinfo.status == 0) {
			stat->resp_passed++;
#ifdef  VERIFY_DATA
			verify_data(&pkt->r);
#endif
		} else {
			print_soft_request(&pkt->r);
			if(pkt->rinfo.status == 3) {
				stat->resp_timeout++;
				print_soft_request(&pkt->r);
			} else
				stat->resp_error++;
		}

		if( (stat->resp_timeout + stat->resp_error) >= MAX_FAILURES) {
			PRINT_ERR("Too many response failures. Stopping test\n");
			tinfo.pkt_cnt = 0; non_stop = 0;
		}
	}
}



#ifndef  __KERNEL__

void signal_handler(int x)
{
	pid_t   my_pid = getpid();
	PRINT_MSG("In signal handler for pid: %d\n", my_pid);

	non_stop = 0;
	tinfo.pkt_cnt = 0;
	signal(signal_to_catch, prev_sig_handler );

	PRINT_MSG(">>>>>> Exiting signal handler for pid: %d\n", my_pid);
}

#endif





void
print_info_test_setup(void)
{
	PRINT_MSG("\n   ----  Test Parameters  -----\n");
	if(tinfo.octeon_id == RANDOM)
		PRINT_MSG("Octeon Device: RANDOM ");
	else
		PRINT_MSG("Octeon Device: %d ", tinfo.octeon_id);

	if(tinfo.iq_no == RANDOM)
		PRINT_MSG("InputQueue: RANDOM ");
	else
		PRINT_MSG("InputQueue: %d ",tinfo.iq_no);

	if(tinfo.pkt_cnt)
		PRINT_MSG(" (Test %d requests)\n", tinfo.pkt_cnt);
	else
		PRINT_MSG(" (Continuous mode test)\n");

	PRINT_MSG("DMA: %s ", (tinfo.dma_mode == RANDOM)?"RANDOM":OCTEON_DMA_MODE_STRING(tinfo.dma_mode));
	PRINT_MSG("Resp_Order: %s ", (tinfo.resp_order == RANDOM)?"RANDOM":OCTEON_RESP_ORDER_STRING(tinfo.resp_order));
	PRINT_MSG("Resp_Mode: %s\n", (tinfo.resp_mode == RANDOM)?"RANDOM":OCTEON_RESP_MODE_STRING(tinfo.resp_mode));

	PRINT_MSG(" %s Data; ", (tinfo.datagen == RANDOM)?"Random":(tinfo.datagen == SEQUENTIAL)?"Sequential":"Fixed");
	PRINT_MSG("BufCount: Min: %d Max: %d Data Size: Min: %d Max: %d\n",
		      tinfo.min_bufs, tinfo.max_bufs, tinfo.min_datasize, tinfo.max_datasize);
}





void
print_info_unit_test(struct __unit_test_stats *s, char  *str)
{
	int i;

	if(s->send_passed) {
		PRINT_MSG("\n%s\n", str);
		PRINT_MSG("%llu requests (minbufs: %llu mindata: %llu) (maxbufs: %llu maxdata: %llu)\n",
		          CVM_CAST64(s->send_passed),
		          CVM_CAST64(s->minbufs), CVM_CAST64(s->mindata), 
		          CVM_CAST64(s->maxbufs), CVM_CAST64(s->maxdata));
		for(i = 0; i < 4; i++) {
			if(s->dmacnt[i]) {
				PRINT_MSG("%llu %s; ", CVM_CAST64(s->dmacnt[i]),
				          OCTEON_DMA_MODE_STRING(i));
			}
		}
		PRINT_MSG("\n");
	}
}



int
print_info_request_list(struct test_list  *tl)
{
	struct __test_stats      reqstats;
	struct test_pkt         *pkt;
	int                      i, count;

	memset(&reqstats, 0, sizeof(struct __test_stats));

	count = tl->avail_count;
	for(i = 0; i < count; i++) {
		pkt = get_next_request(tl);
		if(pkt == NULL) {
			PRINT_ERR("%s: No request in list\n", __FUNCTION__);
			return 1;
		}
		//print_soft_request(&pkt->r);
		pkt->send_state = TREQ_SEND_SUCCESS;
		update_test_stats(pkt, &reqstats, UPDATE_SEND);
		put_free_request(tl, pkt);
	}

	PRINT_MSG("%d requests in request list\n", count);
	print_info_unit_test(&reqstats.u_b, "UNORDERED BLOCKING");
	print_info_unit_test(&reqstats.u_nb, "UNORDERED NON-BLOCKING");
	print_info_unit_test(&reqstats.o, "ORDERED");
	print_info_unit_test(&reqstats.nr, "NORESPONSE");
	return 0;
}







void
print_unit_test_stats(struct __unit_test_stats *s, char  *str)
{
	int i;

	if(s->send_passed) {
		PRINT_MSG("\n%s\n", str);
		PRINT_MSG("\t%llu requests successful (minbufs: %llu maxbufs: %llu mindata: %llu maxdata: %llu)\n",
		          CVM_CAST64(s->send_passed),
		          CVM_CAST64(s->minbufs), CVM_CAST64(s->maxbufs),
		          CVM_CAST64(s->mindata), CVM_CAST64(s->maxdata));
		PRINT_MSG("\t");
		for(i = 0; i < 4; i++) {
			if(s->dmacnt[i]) {
				PRINT_MSG("%llu %s; ", CVM_CAST64(s->dmacnt[i]),
				          OCTEON_DMA_MODE_STRING(i));
			}
		}
		PRINT_MSG("\n");
		PRINT_MSG("\t%llu responses received ( %llu timeout %llu errors )\n",
		          CVM_CAST64(s->resp_passed), CVM_CAST64(s->resp_timeout),
		          CVM_CAST64(s->resp_error));

	} else {
		if(s->send_failed)
			PRINT_MSG("\n No %s requests sent (%llu failures)\n",
			           str, CVM_CAST64(s->send_failed));
	}
}







#ifdef  __KERNEL__
void
oct_reqresp_test_callback(octeon_req_status_t  status, void  *arg)
{
	struct test_pkt   *pkt = (struct test_pkt *)arg;

	if(GET_SOFT_REQ_RESP_ORDER(&pkt->r) == OCTEON_RESP_ORDERED) {
		pkt->rinfo.status = status;
		PRINT_DBG("req callback with status: %d\n", pkt->rinfo.status);
		update_test_stats(pkt, &tstats, (UPDATE_RESP));
		cavium_atomic_dec(&ord_reqs_pending);
	}
	put_free_request(pkt->tl, pkt);
}
#endif




extern int
setup_request_params(struct test_pkt   *pkt);


struct test_list *
init_test_req_list(int *count, int max_reqs)
{
	int                i;
	struct test_list  *tl;
	struct test_pkt   *pkt;

	*count = 0;
	tl = initialize_test_list(max_reqs,
	                  (sizeof(struct test_pkt) - sizeof(struct test_node)));
	if(tl == NULL) {
		PRINT_ERR("%s test list alloc failed\n", __FUNCTION__);
		return NULL;
	}

	for(i = 0; i < max_reqs; i++) {
		pkt = get_next_request(tl);
		if( (pkt == NULL) || (setup_request_params(pkt)) ) {
			put_free_request(tl, pkt);
			cleanup_test_list(tl, max_reqs);
			return NULL;
		}
		PRINT_DBG("pkt[%d]:  %p\n", i, pkt);
		pkt->pkt_no = i;
		pkt->tl     = tl;
		put_free_request(tl, pkt);
	}

	*count = i;
	//print_test_list(tl);
	return tl;
}




int
free_test_req_list(struct test_list  *tl, int max_reqs)
{
	int                i, j;
	struct test_pkt   *pkt;

	for(i = 0; i < max_reqs; i++) {
		pkt = get_next_request(tl);
		if( (pkt == NULL) ) {
			PRINT_ERR("%s: Found only %d requests in list @ %p, expected %d\n",
			          __FUNCTION__, i, tl, max_reqs);
			break;
		}

		for(j = 0; j < pkt->r.inbuf.cnt; j++)
			oct_test_free(SOFT_REQ_INBUF(&pkt->r, j));
		for(j = 0; j < pkt->r.outbuf.cnt; j++)
			oct_test_free(SOFT_REQ_OUTBUF(&pkt->r, j));

		put_free_request(tl, pkt);
	}

	return cleanup_test_list(tl, max_reqs);
}





int
check_req_status(octeon_soft_request_t    *sr)
{
	octeon_query_request_t  query;

	query.octeon_id  = SOFT_REQ_INFO(sr)->octeon_id;
	query.request_id = SOFT_REQ_INFO(sr)->request_id;
	query.status = 0xff;

#ifdef __KERNEL__
	if(octeon_query_request_status(query.octeon_id,&query)) {
#else
	if(octeon_query_request(query.octeon_id, &query)) {
#endif
		PRINT_ERR("%s query for request_id %d failed\n", __FUNCTION__,
		          query.request_id);
		return -1;
	}
	return query.status;
}




int
check_unordered_req_list(struct test_list *reqtl, struct test_list *utl)
{
	struct test_node    *node = NULL;
	struct test_pkt     *pkt  = NULL;
	int                  u_count=0;

	node = get_next_node(utl);
	PRINT_DBG("%s node @ %p\n", __FUNCTION__, node);
	while(node) {
		int status;

		pkt = (struct test_pkt *)node;
		status = check_req_status(&pkt->r);
		if(status != OCTEON_REQUEST_PENDING) {
			pkt->rinfo.status = status;
			update_test_stats(pkt, &tstats, (UPDATE_RESP));
			put_free_request(reqtl, pkt);
			u_count++;
		} else {
			put_free_node(utl, node);
		}
		node = get_next_node(utl);
	}

	return u_count;
}








#ifdef  __KERNEL__
int
perf_test_loop(void* arg)
{
	struct test_list *tl = (struct test_list *)arg;
	int                    pkt_cnt=0, u_nb_count=0;
	struct test_pkt       *pkt;
	struct test_list      *u_nb_l;
	int                    wait_time=MAX_LOOP_COUNT, pkt_none_cnt=0;
	octeon_instr_status_t  retval;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
   char name[] = "Octeon REQUEST/RESPONSE TEST Thread\n";
   daemonize((char *)name);
   allow_signal(SIGTERM);
#else
   daemonize();
   reparent_to_init();
#endif
	cavium_atomic_set(&ord_reqs_pending, 0);

	u_nb_l = initialize_test_list(0, 0);
	if(u_nb_l == NULL) {
		PRINT_ERR("%s: unordered non-blocking pending list alloc failed\n",
		          __FUNCTION__);
		return -ENOMEM;
	}


	if(tinfo.pkt_cnt)
		non_stop = 0;

	PRINT_MSG(" --- Starting perf_test_loop ----\n");

	while(tinfo.pkt_cnt || non_stop) {

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
		if(signal_pending(current))
			break;
#else
		if(kthread_should_stop())
			break;
#endif


		pkt = get_next_request(tl);
		if(pkt == NULL) {
			pkt_none_cnt++;
			if(pkt_none_cnt > MAX_LOOP_COUNT) {
				PRINT_ERR("%s: No more requests in test list\n", __FUNCTION__);
				break;
			}
			cavium_sleep_timeout(HZ/100);
			if(u_nb_count) {
				u_nb_count -= check_unordered_req_list(tl, u_nb_l);
			}
			continue;
		}

		PRINT_DBG("%s: pkt[%d]: %p\n", __FUNCTION__, pkt_cnt, pkt);
		pkt_cnt++;
		pkt->send_state = TREQ_SEND_PENDING;

		pkt_none_cnt = 0;
		retval = octeon_process_request(pkt->rinfo.octeon_id, &pkt->r);
		if(!retval.s.error) {
			pkt->send_state = TREQ_SEND_SUCCESS;
		} else {
			PRINT_ERR("%s: Request[%d] Failed Req status: %x\n", 
				  __FUNCTION__, pkt_cnt, retval.s.status);
			pkt->send_state = TREQ_SEND_FAILED;
			update_test_stats(pkt, &tstats, UPDATE_SEND);
			put_free_request(tl, pkt);
			cavium_sleep_timeout(HZ);
			goto send_next_kernel_pkt;
		}


		/* Do not put the test pkt back into the test request list. Always
		   put into test list in the callback (for noresp and for ordered)
		*/
		if( GET_SOFT_REQ_RESP_ORDER(&pkt->r) == OCTEON_RESP_ORDERED) {
			cavium_atomic_inc(&ord_reqs_pending);
			update_test_stats(pkt, &tstats, UPDATE_SEND);
		} else {
			if( GET_SOFT_REQ_RESP_ORDER(&pkt->r) == OCTEON_RESP_UNORDERED) {
				if (GET_SOFT_REQ_RESP_MODE(&pkt->r) == OCTEON_RESP_NON_BLOCKING) {
					u_nb_count++;
					update_test_stats(pkt, &tstats, (UPDATE_SEND));
					put_free_node(u_nb_l, (struct test_node *)pkt);
				} else {
					PRINT_ERR("UNORDERED BLOCKING MODE NOT SUPPORTED\n");
				}
			} else {
				if( GET_SOFT_REQ_RESP_ORDER(&pkt->r) == OCTEON_RESP_NORESPONSE) {
					update_test_stats(pkt, &tstats, UPDATE_SEND);
				}
			}		
		}


		if( (tstats.o.send_passed && !(tstats.o.send_passed & 0x3ff))
		    || (tstats.nr.send_passed && !(tstats.nr.send_passed & 0x3ff)) ) {
			cavium_sleep_timeout(1);
		}

send_next_kernel_pkt:
		if(tinfo.pkt_cnt)
			tinfo.pkt_cnt--;

		if(u_nb_count >= MAX_PENDING_REQUESTS) {
			u_nb_count -= check_unordered_req_list(tl, u_nb_l);
		}
	}


	wait_time = MAX_LOOP_COUNT;
	while(cavium_atomic_read(&ord_reqs_pending) && wait_time--) {
		if(signal_pending(current))
			break;
		cavium_sleep_timeout(1);
	}
	if(cavium_atomic_read(&ord_reqs_pending)) {
		PRINT_ERR("%s: There are %d requests pending\n", __FUNCTION__,
		           cavium_atomic_read(&ord_reqs_pending));
	}
	while(u_nb_count) {
		u_nb_count -= check_unordered_req_list(tl, u_nb_l);
	}
	cleanup_test_list(u_nb_l, 0);

	PRINT_MSG(" --- Quitting perf_test_loop ----\n");

	while(1) {

#if  LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
		if(signal_pending(current))
			break;
#else
		if(kthread_should_stop())
			break;
#endif

		cavium_sleep_timeout(1);
	}

	return 0;
}

#else

int
perf_test_loop(struct test_list *tl)
{
	int                    pkt_cnt=0, u_nb_count=0, nr_count=0;
	struct test_pkt       *pkt;
	struct test_list      *u_nb_l;
	int                    retval=1, pkt_none_cnt;


	if(octeon_initialize()) {
		PRINT_ERR("%s: Failed to initialize octeon device\n", __FUNCTION__);
		return -ENODEV;
	}

	u_nb_l = initialize_test_list(0, 0);
	if(u_nb_l == NULL) {
		PRINT_ERR("%s: unordered non-blocking pending list alloc failed\n",
		          __FUNCTION__);
		return -ENOMEM;
	}

	if(tinfo.pkt_cnt)
		non_stop = 0;

	//PRINT_MSG(" --- Starting perf_test_loop ----\n");

	while(tinfo.pkt_cnt || non_stop) {
		pkt = get_next_request(tl);
		if(pkt == NULL) {
			PRINT_DBG("No more pkts in req list\n");
			pkt_none_cnt++;
			if(pkt_none_cnt > MAX_LOOP_COUNT) {
				PRINT_ERR("%s: No more requests in test list\n", __FUNCTION__);
				break;
			}
			sleep(1);
			if(u_nb_count) {
				u_nb_count -= check_unordered_req_list(tl, u_nb_l);
			}
			continue;
		}

		PRINT_DBG("%s: pkt[%d]: %p\n", __FUNCTION__, pkt_cnt, pkt);
		pkt_cnt++;
		pkt->send_state = TREQ_SEND_PENDING;

		retval = octeon_send_request(pkt->rinfo.octeon_id, &pkt->r);
		if(!retval){
			pkt->send_state = TREQ_SEND_SUCCESS;
		} else {
			PRINT_ERR("%s: Request[%d] Failed\n", __FUNCTION__, pkt_cnt);
			print_soft_request(&pkt->r);
			pkt->send_state = TREQ_SEND_FAILED;
			update_test_stats(pkt, &tstats, UPDATE_SEND);
			put_free_request(tl, pkt);
			sleep(1);
			goto send_next_user_pkt;
		}

		if( GET_SOFT_REQ_RESP_ORDER(&pkt->r) == OCTEON_RESP_UNORDERED) {
			if (GET_SOFT_REQ_RESP_MODE(&pkt->r) == OCTEON_RESP_NON_BLOCKING) {
				update_test_stats(pkt, &tstats, (UPDATE_SEND));
				put_free_node(u_nb_l, (struct test_node *)pkt);
				u_nb_count++;
				if(!(u_nb_count & 0xff))
					usleep(10 * u_nb_count);
			} else {
				update_test_stats(pkt, &tstats, (UPDATE_SEND | UPDATE_RESP));
				put_free_request(tl, pkt);
			}
		} else {
			if( GET_SOFT_REQ_RESP_ORDER(&pkt->r) == OCTEON_RESP_NORESPONSE) {
				update_test_stats(pkt, &tstats, UPDATE_SEND);
				nr_count++;
				if(!(nr_count & 0xff)) {
					//printf("sleep for %d us\n", (pkt->reqdatasize >> 2));
					usleep(pkt->reqdatasize >> 1);
				}
				put_free_request(tl, pkt);
			} else {
				if( GET_SOFT_REQ_RESP_ORDER(&pkt->r) == OCTEON_RESP_ORDERED) {
					update_test_stats(pkt, &tstats, UPDATE_SEND);
				}
			}
		}

send_next_user_pkt:
		if(tinfo.pkt_cnt)
			tinfo.pkt_cnt--;

		u_nb_count -= check_unordered_req_list(tl, u_nb_l);
	}


	while(u_nb_count) {
		u_nb_count -= check_unordered_req_list(tl, u_nb_l);
	}

	cleanup_test_list(u_nb_l, 0);

	octeon_shutdown();
	//PRINT_MSG(" --- Quitting perf_test_loop ----\n");
	return 0;
}


#endif




void
print_test_results(void)
{
	PRINT_MSG("\n --- Test Results ---\n");
	print_unit_test_stats(&tstats.u_b, "UNORDERED BLOCKING");
	print_unit_test_stats(&tstats.u_nb, "UNORDERED NON-BLOCKING");
	print_unit_test_stats(&tstats.o, "ORDERED");
	print_unit_test_stats(&tstats.nr, "NORESPONSE");
}







#ifndef __KERNEL__
int   main(int argc, char **argv)
#else
int   oct_perf_kern_main(void)
#endif
{
	int               count;
	struct test_list *tl;
	
	/* Seed the pseudo-random number generator */
#ifndef __KERNEL__
	srandom((int)time(NULL));
	if(parse_args(argc, argv))
		return -EINVAL;
#endif

	memset(&tstats, 0, sizeof(tstats));
	tstats.u_nb.mindata = tstats.u_b.mindata = tstats.o.mindata =
	tstats.nr.mindata = (uint64_t)-1;

	tstats.u_nb.minbufs = tstats.u_b.minbufs = tstats.o.minbufs =
	tstats.nr.minbufs = (uint64_t)-1;

	if(run_all_tests) {
#ifndef __KERNEL__
		prev_sig_handler = signal(signal_to_catch, signal_handler);
#endif
		return oct_perf_run_all_tests();
	}


	if(process_and_verify_args())
		return -EINVAL;

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

#ifdef __KERNEL__
	kern_tl = tl;
#endif


	PRINT_MSG("\n --- Request List created ---\n");
	print_info_request_list(tl);

	
	PRINT_MSG("\n --- Starting Request/Response test ---\n");

#ifdef __KERNEL__

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	reqresp_pid = kernel_thread(perf_test_loop, tl,
                            CLONE_FS|CLONE_FILES|CLONE_SIGHAND);
	if (reqresp_pid < 0)
		goto init_thread_failed;
#else
	reqresp_task = kthread_run(perf_test_loop, tl,"Req/resp Test Thread");
	if (reqresp_task == NULL)
		goto init_thread_failed;

#endif

#else

	prev_sig_handler = signal(signal_to_catch, signal_handler);
	perf_test_loop(tl);
	free_test_req_list(tl, MAX_REQUESTS);
	print_test_results();

#endif


	return 0;

#ifdef __KERNEL__
init_thread_failed:
	cavium_error("Failed to create test thread\n");
	free_test_req_list(tl, MAX_REQUESTS);
	return -ENOMEM;
#endif
}




#ifdef __KERNEL__
void oct_perf_kern_stop(void)
{
	int  wait_time=100;

	PRINT_MSG("%s Stopping requests\n", __FUNCTION__);
	non_stop = 0; tinfo.pkt_cnt = 0;

	PRINT_MSG("%s Waiting for responses\n", __FUNCTION__);
	while(cavium_atomic_read(&ord_reqs_pending) && wait_time--) {
		cavium_sleep_timeout(1);
	}

	if(cavium_atomic_read(&ord_reqs_pending)) {
		PRINT_ERR("%s: There are %d requests pending\n", __FUNCTION__,
		           cavium_atomic_read(&ord_reqs_pending));
	}

	PRINT_MSG("%s Killing perf_test_loop\n", __FUNCTION__);

#if  LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
	if(reqresp_pid != -1)
		kill_proc(reqresp_pid, SIGTERM, 1);
	reqresp_pid = -1;
#else
	if(reqresp_task != NULL)
		kthread_stop(reqresp_task);
	reqresp_task = NULL;
#endif
	
	if(kern_tl) {
		PRINT_MSG("%s Freeing requests\n", __FUNCTION__);
		free_test_req_list(kern_tl, MAX_REQUESTS);
	}

	print_test_results();
}


module_init(oct_perf_kern_main);
module_exit(oct_perf_kern_stop);
#endif


/* $Id: octeon_req_perf.c 59434 2011-04-21 22:04:27Z panicker $ */

