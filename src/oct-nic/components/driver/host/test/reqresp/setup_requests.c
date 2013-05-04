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


#ifdef  __KERNEL__   /* Kernel mode definitions */

static inline  int
get_random(void)
{
	int  r;
	get_random_bytes(&r, sizeof(int));
	return r;
}

extern void
oct_reqresp_test_callback(octeon_req_status_t  status, void  *arg);

#else

#define  get_random()           (random())

#endif


extern struct __seq_params  tseq;
extern struct __test_info  tinfo;

static inline int
get_test_bufcnt(struct __test_info  *t)
{
	int bufcnt;

	if(t->datagen == RANDOM) {
		bufcnt = (get_random()%t->max_bufs);
	} else {
		if(t->datagen == SEQUENTIAL) {
			bufcnt = (tseq.bufcnt.curr % t->max_bufs);
			tseq.bufcnt.curr +=  tseq.bufcnt.incr;
		} else {
			bufcnt = t->max_bufs;
		}
	}

#ifdef ENFORCE_CONSTRAINTS
	if(!bufcnt || bufcnt > tinfo.max_bufs)
		bufcnt = tinfo.max_bufs;
	if(bufcnt < tinfo.min_bufs)
		bufcnt = tinfo.min_bufs;
#endif

	return bufcnt;
}



static inline int
get_test_datasize(struct __test_info  *t)
{
	int datasize;

	if(t->datagen == RANDOM) {
		datasize = (get_random()%t->max_datasize);
	} else {
		if(t->datagen == SEQUENTIAL) {
			datasize = (tseq.datasize.curr % t->max_datasize);
			tseq.datasize.curr +=  tseq.datasize.incr;
		} else {
			datasize = t->max_datasize;
		}
	}

#ifdef ENFORCE_CONSTRAINTS
	if( datasize > tinfo.max_datasize)
		datasize = tinfo.max_datasize;

	if( (t->resp_order != OCTEON_RESP_NORESPONSE) && (!datasize) )
		datasize = tinfo.max_datasize;

	if(datasize < tinfo.min_datasize)
		datasize = tinfo.min_datasize;
#endif

	return datasize;
}













int
setup_output_buffers(octeon_soft_request_t  *r, int bufcnt, int reqdatasize)
{
	int i, bufsize, total_bytes= 0;

	memset(&r->outbuf, 0, sizeof(octeon_buffer_t));
	if(bufcnt == 0)
		return 0;

#ifdef ENFORCE_CONSTRAINTS
	if(reqdatasize < bufcnt)
		return 1;
#endif

	bufsize = (reqdatasize/bufcnt);

	for(i = 0; i < bufcnt; i++)  {
		int outbufsize;

		if( (i == (bufcnt-1)) && ((total_bytes + bufsize) < reqdatasize) ) {
			bufsize = (reqdatasize - total_bytes);
		}
		outbufsize = bufsize;

		#ifdef __KERNEL__
		/* In kernel mode, the outbuf should have space for response header
		   and completion bytes. */
		if(!i)
			outbufsize += 8;
		if(i == (bufcnt - 1))
			outbufsize += 8;
		#endif
		SOFT_REQ_OUTBUF(r, i) = oct_test_alloc(outbufsize);
		if(SOFT_REQ_OUTBUF(r, i) == NULL)
			goto free_outbufs;
		r->outbuf.size[i] = outbufsize;
		r->outbuf.cnt++;

		total_bytes += bufsize;
#ifdef VERIFY_DATA
		memset(SOFT_REQ_OUTBUF(r, i), i+1, outbufsize);
#endif
		PRINT_DBG("outbuf.ptr[%d] = %p size: %d\n", i,
		          SOFT_REQ_OUTBUF(r, i), r->outbuf.size[i]);
	}

	if(total_bytes != reqdatasize) {
		PRINT_ERR("%s: Data size mismatch\n", __FUNCTION__);
		goto free_outbufs;
	}
	return 0;


free_outbufs:
	for(i = 0; i < bufcnt; i++)  {
		if(SOFT_REQ_OUTBUF(r, i))
			oct_test_free(SOFT_REQ_OUTBUF(r, i));
	}

	return 1;
}




int
setup_input_buffers(octeon_soft_request_t  *r, int bufcnt, int reqdatasize)
{
	int i, bufsize, total_bytes= 0;

	memset(&r->inbuf, 0, sizeof(octeon_buffer_t));

	if(bufcnt == 0)
		return 0;

#ifdef ENFORCE_CONSTRAINTS
	if(reqdatasize < bufcnt)
		return 1;
#endif

	bufsize = (reqdatasize/bufcnt);

	for(i = 0; i < bufcnt; i++)  {

		if( (i == (bufcnt-1)) && ((total_bytes + bufsize) < reqdatasize) ) {
			bufsize = (reqdatasize - total_bytes);
		}

		SOFT_REQ_INBUF(r, i) = oct_test_alloc(bufsize);
		if(SOFT_REQ_INBUF(r, i) == NULL)
			goto free_inbufs;
		r->inbuf.size[i] = bufsize;
		r->inbuf.cnt++;

		total_bytes += bufsize;

#ifdef VERIFY_DATA
		{
		int j;
		uint8_t  *ptr = SOFT_REQ_INBUF(r, i);
		for(j = 0; j < bufsize; j++) {
			ptr[j] = ((j&0xff)==0)?1:j;
		}
		}
#else
		memset(SOFT_REQ_INBUF(r, i), 1, bufsize);
#endif
		PRINT_DBG("inbuf.ptr[%d]  = %p size: %d\n", i, SOFT_REQ_INBUF(r, i),
		    r->inbuf.size[i]);
	}

	if(total_bytes != reqdatasize) {
		PRINT_ERR("%s: Data size mismatch\n", __FUNCTION__);
		goto free_inbufs;
	}
	return 0;

free_inbufs:
	for(i = 0; i < bufcnt; i++)  {
		if(SOFT_REQ_INBUF(r, i))
			oct_test_free(SOFT_REQ_INBUF(r, i));
	}

	return 1;
}





int
check_buffer_constraints(OCTEON_DMA_MODE         dma_mode,
                         OCTEON_RESPONSE_ORDER   resp_order,
                         int                     reqdatasize)
{
	switch(dma_mode) {
		case OCTEON_DMA_DIRECT:
			if(reqdatasize > OCT_MAX_DIRECT_INPUT_DATA_SIZE) {
				PRINT_ERR("%s:%d reqdatasize (%d) exceeds max (%d)\n",
				          __FUNCTION__, __LINE__, reqdatasize,
				          OCT_MAX_DIRECT_INPUT_DATA_SIZE);
				return 1;
			}
			break;
		case OCTEON_DMA_GATHER:
			if(resp_order == OCTEON_RESP_NORESPONSE) {
				if(reqdatasize > OCT_MAX_GATHER_DATA_SIZE) {
					PRINT_ERR("%s:%d reqdatasize (%d) exceeds max (%d)\n",
					    __FUNCTION__, __LINE__, reqdatasize,
					    OCT_MAX_GATHER_DATA_SIZE);
					return 1;
				}
			} else {
				if(reqdatasize > OCT_MAX_DIRECT_INPUT_DATA_SIZE) {
					PRINT_ERR("%s:%d reqdatasize (%d) exceeds max (%d)\n",
					    __FUNCTION__, __LINE__, reqdatasize,
				        OCT_MAX_DIRECT_INPUT_DATA_SIZE);
					return 1;
				}
			}
			break;
		case OCTEON_DMA_SCATTER:
			if(reqdatasize > OCT_MAX_DIRECT_INPUT_DATA_SIZE)  {
				PRINT_ERR("%s:%d reqdatasize (%d) exceeds max (%d)\n",
				          __FUNCTION__, __LINE__, reqdatasize,
				          OCT_MAX_DIRECT_INPUT_DATA_SIZE);
				return 1;
			}
			break;
		case OCTEON_DMA_SCATTER_GATHER:
			if(reqdatasize > OCT_MAX_GATHER_DATA_SIZE) {
				PRINT_ERR("%s:%d reqdatasize (%d) exceeds max (%d)\n",
				          __FUNCTION__, __LINE__, reqdatasize,
				          OCT_MAX_GATHER_DATA_SIZE);
				return 1;
			}
			break;
	}
	return 0;
}






int
setup_buffers(OCTEON_DMA_MODE         dma_mode,
              OCTEON_RESPONSE_ORDER   resp_order,
              struct test_pkt        *pkt)
{
	int  inbufcnt=0, outbufcnt=0, reqdatasize;
	octeon_soft_request_t  *r  = &pkt->r;

	reqdatasize = get_test_datasize(&tinfo);

	switch(dma_mode) {
		case OCTEON_DMA_DIRECT:
			inbufcnt = outbufcnt = 1;
			break;
		case OCTEON_DMA_GATHER:
			inbufcnt = get_test_bufcnt(&tinfo); outbufcnt = 1;
			break;
		case OCTEON_DMA_SCATTER:
			outbufcnt = get_test_bufcnt(&tinfo); inbufcnt = 1;
			break;
		case OCTEON_DMA_SCATTER_GATHER:
			inbufcnt = outbufcnt = get_test_bufcnt(&tinfo);
			break;
	}

#ifdef ENFORCE_CONSTRAINTS
	if(reqdatasize < inbufcnt)
		inbufcnt = reqdatasize;
	if(reqdatasize < outbufcnt)
		outbufcnt = reqdatasize;

	if(resp_order == OCTEON_RESP_NORESPONSE)
		outbufcnt = 0;

	if(check_buffer_constraints(dma_mode, resp_order, reqdatasize))
		return 1;
#endif

	PRINT_DBG("%s req @ %p datasize: %d inbufcnt: %d outbufcnt: %d\n",
	          __FUNCTION__, r, reqdatasize, inbufcnt, outbufcnt);


	if(setup_input_buffers(r, inbufcnt, reqdatasize))
		return 1;
	if(setup_output_buffers(r, outbufcnt, reqdatasize))
		return 1;

	pkt->reqdatasize = reqdatasize;

	return 0;
}





int
setup_request_params(struct test_pkt   *pkt)
{
	octeon_soft_request_t  *r     = &pkt->r;
	octeon_request_info_t  *rinfo = &pkt->rinfo;
	OCTEON_DMA_MODE         dma_mode;
	OCTEON_RESPONSE_ORDER   resp_order;
	OCTEON_RESPONSE_MODE    resp_mode;
	int                     iq_no, oct_id;

	memset(r, 0, sizeof(octeon_soft_request_t));
	memset(rinfo, 0, sizeof(octeon_request_info_t));

	SOFT_REQ_INFO(r) = rinfo;

	r->ih.raw     = 1;
	r->ih.qos     = 0; //(((unsigned long)pkt >> 16) & 0x7);
	r->ih.grp     = 0;
	r->ih.rs      = 0;
	r->ih.tagtype = 1;
	r->ih.tag     = 0;

	r->irh.opcode = CVMCS_REQRESP_OP;
	r->irh.param  = 0x10;
	r->irh.dport  = 32;

	oct_id = (tinfo.octeon_id == RANDOM)?(get_random() & 0x1):tinfo.octeon_id;
	dma_mode = (tinfo.dma_mode == RANDOM)?(get_random() & 0x3):tinfo.dma_mode;
	resp_mode = (tinfo.resp_mode == RANDOM)?(get_random() & 0x1):tinfo.resp_mode;
	resp_order = (tinfo.resp_order == RANDOM)?(get_random()%3):tinfo.resp_order;
	iq_no = (tinfo.iq_no == RANDOM)?(get_random() & 0x1):tinfo.iq_no;

#ifdef ENFORCE_CONSTRAINTS
	if(resp_order == OCTEON_RESP_NORESPONSE) {
		if (dma_mode == OCTEON_DMA_SCATTER)
			dma_mode = OCTEON_DMA_DIRECT;
		if (dma_mode == OCTEON_DMA_SCATTER_GATHER)
			dma_mode = OCTEON_DMA_GATHER;
	}

#ifdef __KERNEL__
	if(resp_order == OCTEON_RESP_UNORDERED)
		resp_order = OCTEON_RESP_ORDERED;
	resp_mode = OCTEON_RESP_NON_BLOCKING;
#else
	if(resp_order == OCTEON_RESP_ORDERED)
		resp_order = OCTEON_RESP_UNORDERED;
#endif
	if(resp_order == OCTEON_RESP_NORESPONSE)
		resp_mode = OCTEON_RESP_NON_BLOCKING;
#endif

	rinfo->octeon_id           = oct_id;
	rinfo->req_mask.dma_mode   = dma_mode;
	rinfo->req_mask.resp_mode  = resp_mode;
	rinfo->req_mask.resp_order = resp_order;
	rinfo->req_mask.iq_no      = iq_no;

#ifdef __KERNEL__
	rinfo->callback            = oct_reqresp_test_callback;
	rinfo->callback_arg        = pkt;
#endif
	rinfo->request_id          = 0xff;
	rinfo->timeout             = REQUEST_TIMEOUT;

	if(setup_buffers(dma_mode, resp_order, pkt)) {
		return 1;
	}

	if(resp_order != OCTEON_RESP_NORESPONSE)
		SOFT_REQ_INFO(r)->status = 0xff;

	//print_soft_request(r);

	return 0;
}


/* $Id: setup_requests.c 59990 2011-05-07 00:45:17Z panicker $ */
