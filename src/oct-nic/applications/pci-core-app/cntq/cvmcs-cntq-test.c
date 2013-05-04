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


#include <errno.h>
#include "cvmx.h"
#include "cvmx-config.h"
#include "cvmcs-cntq-defs.h"
#include "cvmcs-cntq-test.h"
#include "cvm-driver-defs.h"
#include "cvm-cntq.h"
#include "cvm-ddoq.h"
#include "octeon-ddoq-testpattern.h"



extern               uint32_t   core_id;
extern CVMX_SHARED   uint64_t   cvmcs_pkt_count[MAX_CORES];
extern CVMX_SHARED   uint64_t   cpu_freq;
       CVMX_SHARED   uint64_t   ddoq_send_wait_count=0, total_ddoq_pkt_sent=0;
       CVMX_SHARED   uint64_t   remote_test_ptr=0;
                     uint64_t   send_fail_wait_cycles=0xffffffff;
                     uint64_t   pko_test_pkt_cnt = 0;
                     int        ddoq_pkt_count[MAX_TEST_DDOQS];

extern CVMX_SHARED int ok_to_test_ddoq;

uint64_t  test_time=0;


#if   defined(CVMCS_TEST_DDOQ)

void 
cvmcs_init_ddoq_test(void)
{
	int i;
	for(i = 0; i < MAX_TEST_DDOQS; i++)
		ddoq_pkt_count[i] = 0;
}



static inline int
cvmcs_test_stamp_data(uint8_t  *buf, int size, int idx)
{
	int i;
	
	for(i = 0; i < size; i++) {
		buf[i] = ddoqsign[idx];
		INCR_SIGNIDX(idx);
	}
	
	return idx;
}








/* The signature array is in octeon-ddoq-testpattern.h. This routine is used to
   write the signature in a stream to every buffer in the packet. */
static void
create_signature(cvmx_buf_ptr_t  lptr, int bufs)
{
	volatile int idx = 0;

/*	printf("Signature: lptr: 0x%016lx (addr: %lx size: %d) bufs: %d\n",	lptr.u64, lptr.s.addr, lptr.s.size, bufs);*/
	
	if(bufs > 1) {
		int i;
		cvmx_buf_ptr_t *gptr = CVM_DRV_GET_PTR(lptr.s.addr);
		for(i = 0; i < bufs; i++) {
			idx = cvmcs_test_stamp_data(CVM_DRV_GET_PTR(gptr[i].s.addr), gptr[i].s.size, idx);
		}
	} else {
		idx = cvmcs_test_stamp_data(CVM_DRV_GET_PTR(lptr.s.addr), lptr.s.size, idx);
	}
}





static uint64_t
cvmcs_test_create_direct_ddoq_data(int total_bytes)
{
	uint8_t          *buf;
	uint32_t         len;
	cvmx_buf_ptr_t   lptr;

	lptr.u64 = 0;
	buf = cvmx_fpa_alloc(CVMX_FPA_PACKET_POOL);
	if(total_bytes > CVMX_FPA_PACKET_POOL_SIZE) {
		printf("\nWarning!!!%s  total_bytes (%d) exceeds pkt pool size\n",
			__FUNCTION__, total_bytes);
	}
	len = (total_bytes > CVMX_FPA_PACKET_POOL_SIZE)?CVMX_FPA_PACKET_POOL_SIZE:total_bytes;
	if(buf) {
		lptr.s.i    = 1;
		lptr.s.pool = CVMX_FPA_PACKET_POOL;
		lptr.s.size = len;
		lptr.s.addr = CVM_DRV_GET_PHYS(buf);
	}
	
	return lptr.u64;
}












static void
__adjust_data_sizes(int curr_size, int total_bytes, int bufs, cvmx_buf_ptr_t *gptr)
{
	int  i = 0, remainder = total_bytes % bufs;

	while( (curr_size < total_bytes) && (remainder) && (i < bufs)) {
		if(gptr[i].s.size < CVMX_FPA_PACKET_POOL_SIZE) {
			gptr[i].s.size++;
			remainder--;
			curr_size++;
		}
		i++;
	}
	
	if( (i == bufs) && (curr_size != total_bytes) ) {
		printf("Current size (%d) does not match total_bytes (%d)\n",
			   curr_size, total_bytes);
		for( i = 0; i < bufs; i++) {
			printf("gptr[%d]: addr: %lx size: %d\n", i, (uint64_t)gptr[i].s.addr,
			   gptr[i].s.size);
		}
	} else {
/*		for( i = 0; i < bufs; i++) {
			printf("gptr[%d]: addr: %lx size: %d\n", i, (uint64_t)gptr[i].s.addr,
			   gptr[i].s.size);
		}*/
	}
}


static uint64_t
cvmcs_test_create_gather_ddoq_data(int total_bytes, int bufs)
{
	int               bufsize, curr_size = 0;
	cvmx_buf_ptr_t    lptr, *gptr;

	dbg_printf("%s total_bytes: %d bufs: %d\n", __FUNCTION__, total_bytes, bufs);
	
	if(total_bytes < bufs)
		bufs = total_bytes;
	
	bufsize = total_bytes/bufs;
	if(bufsize > CVMX_FPA_PACKET_POOL_SIZE) {
		printf("%s:%d bufsize (%d) > packet pool buf size\n", __FUNCTION__, __LINE__, bufsize);
		return 0;
	}
	dbg_printf("total_bytes: %d bufs: %d bufsize: %d\n", total_bytes, bufs, bufsize);
	
	gptr = cvmx_fpa_alloc(CVMX_FPA_PACKET_POOL);
	if(gptr == NULL) {
		printf("%s:%d gptr is NULL\n", __FUNCTION__, __LINE__);
		return 0;
	}

	lptr.u64 = 0;
	lptr.s.size = bufs;
	
	while(bufs) {
		bufs--;
		gptr[bufs].u64 = 0;
		gptr[bufs].s.addr = CVM_DRV_GET_PHYS(cvmx_fpa_alloc(CVMX_FPA_PACKET_POOL));
		if(gptr[bufs].s.addr) {
			gptr[bufs].s.size = bufsize;
			curr_size += gptr[bufs].s.size;
			gptr[bufs].s.i    = 1;
			gptr[bufs].s.pool = CVMX_FPA_PACKET_POOL;
			dbg_printf(">>>> gptr[%d]: 0x%016lx\n", bufs, gptr[bufs].u64);
		}
	}
	dbg_printf("curr_size: %d total_bytes: %d\n", curr_size, total_bytes);
	
	if(curr_size != total_bytes)
		__adjust_data_sizes(curr_size, total_bytes, lptr.s.size, gptr);

	lptr.s.addr = CVM_DRV_GET_PHYS(gptr);
	lptr.s.pool = CVMX_FPA_PACKET_POOL;

	return lptr.u64;
}








static uint64_t
cvmcs_test_create_ddoq_pkt(int total_bytes, int bufs)
{
	if(bufs == 1)
		return cvmcs_test_create_direct_ddoq_data(total_bytes);
	else
		return cvmcs_test_create_gather_ddoq_data(total_bytes, bufs);
}









static uint32_t  curr_test_size = 1;
static uint32_t  curr_bufs = 1;


/* BUFCOUNT_FIRST will try to split the data across the maximum possible
   buffers. If this is not defined, the function will try to fit all
   data in the fewest possible buffers.
*/
//#define BUFCOUNT_FIRST

/* Select this option to randomize the size of data and count of buffers
   used in each test packet. */ 
//#define RANDOMIZE_DATA_SIZE

void
__get_new_size_and_count(int  *size, int *count)
{
	int bufsize = 0, perbufsize = CVMX_FPA_PACKET_POOL_SIZE; //256;

	do {
#ifdef RANDOMIZE_DATA_SIZE
		curr_test_size = (cvmx_get_cycle() % TEST_DDOQ_PKT_LEN) + 1;
#else
		curr_test_size = (curr_test_size + 1) % TEST_DDOQ_PKT_LEN;
#endif

		if(curr_test_size == 0)
			curr_test_size = 1;

#ifdef BUFCOUNT_FIRST
#ifdef RANDOMIZE_DATA_SIZE
		curr_bufs = (cvmx_get_cycle() % MAX_DDOQ_GATHER_PTRS) + 1;
#else
		if(curr_test_size < MAX_DDOQ_GATHER_PTRS) {
			curr_bufs = curr_test_size;
		} else {
			curr_bufs = MAX_DDOQ_GATHER_PTRS;
		}
#endif
#else
		curr_bufs = curr_test_size/perbufsize
					 + ((curr_test_size % perbufsize)?1:0);
#endif

		bufsize = curr_test_size/curr_bufs;

	} while (bufsize == 0 || curr_test_size > (CVMX_FPA_PACKET_POOL_SIZE * curr_bufs));

	*size = curr_test_size;  *count = curr_bufs;
}








int
cvmcs_test_ddoq_send(uint32_t  ddoq_id)
{
	cvmx_buf_ptr_t   lptr;
	cvmx_resp_hdr_t  resp_hdr;
	int              retval = 0;
	int              total_bytes = 0, bufs = 0;
	int              ptrtype;

#if 0
	if(ddoq_pkt_count[ddoq_id - 36]) {
		printf("Just sending 1 pkt\n");
		return 0;
	}
#endif

	__get_new_size_and_count(&total_bytes, &bufs);
//	printf("total_bytes: %d bufs: %d\n", total_bytes, bufs);

	if(bufs == 1)
		ptrtype = CVM_DIRECT_DATA;
	else
		ptrtype = CVM_GATHER_DATA;
	lptr.u64 = cvmcs_test_create_ddoq_pkt(total_bytes, bufs);
	
	if(lptr.u64 == 0) {
		printf("test_ddoq: buffer alloc failed totalbytes: %d bufs: %d\n",
			   total_bytes, bufs);
		return -ENOMEM;
	}
	create_signature(lptr, bufs);
	
	resp_hdr.u64         = 0;
	resp_hdr.s.opcode    = 0x1234;
	resp_hdr.s.destqport = ddoq_id;

/*	printf("DDOQ Send data: lptr: 0x%016lx (addr: %lx size: %d) ptrtype: %d bufs: %d total_bytes: %d\n",	lptr.u64, lptr.s.addr, lptr.s.size, ptrtype, bufs, total_bytes);*/

	retval = cvm_ddoq_send_data(ddoq_id,lptr, ptrtype, bufs, total_bytes, &resp_hdr);

	if(retval > 0) {
		ddoq_pkt_count[ddoq_id - 36]++;
	} else {
		if(ptrtype == CVM_DIRECT_DATA)
			cvm_drv_fpa_free(castptr(void *, CVM_DRV_GET_PTR(lptr.s.addr)),lptr.s.pool, 0);

		if(ptrtype == CVM_GATHER_DATA) {
			cvmx_buf_ptr_t  *gptr = CVM_DRV_GET_PTR(lptr.s.addr);
			while(bufs--)
				cvmx_fpa_free(CVM_DRV_GET_PTR(gptr[bufs].s.addr), gptr[bufs].s.pool, 0);
		}
	}

	if(ptrtype == CVM_GATHER_DATA)
		cvm_drv_fpa_free(castptr(void *, CVM_DRV_GET_PTR(lptr.s.addr)),lptr.s.pool, 0);

	return retval;
}





int
cvmcs_do_ddoq_test(void)
{
	int  i, retval;

	if(cvmx_get_cycle() < test_time)
		return 1;

	if(ok_to_test_ddoq == 0)
		return 1;

	for(i = 36; i < (36 + MAX_TEST_DDOQS); i++)  {
		retval = cvmcs_test_ddoq_send(i);
		test_time = cvmx_get_cycle() + (cpu_freq >> CNTQ_TEST_SHIFT_FREQ);
		if(retval <= 0)
			return retval;
	}
	return 0;
}
#endif



#if   defined(CVMCS_TEST_CNTQ)

int
cvmcs_do_cntq_test(void)
{
	cvm_cntq_pkt_t    *cntq_pkt;
	static uint32_t    cntq_send_cnt=0;
	uint64_t          *buf;

#define CNTQ_TEST_PKT_SIZE  64
	cntq_pkt = cvm_alloc_cntq_packet(CVMX_HIGHP_DMA_QUEUE,CNTQ_TEST_PKT_SIZE);
	if(cntq_pkt == NULL) {
		printf("CNTQ test packet allocation failed\n");
		return 1;
	} 
	cntq_pkt->resp_hdr.s.opcode = CORE_TO_HOST_PKT;
	buf = (uint64_t *)cntq_pkt->data;
	*((uint64_t *)buf) = 0x0123456789abcdef;
	switch(cvm_cntq_send_packet(cntq_pkt)) {
	case 0:
		cntq_send_cnt++;
		if(!(cntq_send_cnt & 0xffff)) 
			printf("Sent %d cntq packets so far\n", cntq_send_cnt);
		break;
	case ENODEV:
		printf("Device not initialised\n");
	case ENOMEM:
		printf("CNTQ test packet not sent..wait 1 sec and continue\n");
		cvm_cntq_free_packet(cntq_pkt);
		cvmx_wait(cpu_freq);
		break;
	}
	return 0; 
}

#endif
/* $Id: cvmcs-cntq-test.c 63359 2011-08-10 22:08:08Z panicker $ */
