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




#include "cvmcs-test.h"

#if  defined(CVMCS_TEST_PKO) || defined(MAX_PACKET_RATE_TEST)



#ifdef  MAX_PACKET_RATE_TEST
void  cvmcs_max_pkt_pko_test(void);
#endif

extern CVMX_SHARED    uint64_t         cpu_freq;
extern               uint32_t      core_id;


/* The cycle count when the PKO test was run last on this core. */
uint64_t  last_pko_run=0;


uint32_t  test_pkt_sent=0;

/* Used in sequential and random data size calculation. */
uint32_t  last_data_size = CVM_MIN_DATA;


#if  defined(PROGRAM_THRU_SCRATCH)
CVMX_SHARED static uint32_t  pkts_per_sec       = 0; // Tunable
#else
CVMX_SHARED static uint32_t  pkts_per_sec       = PKO_TEST_PKT_RATE; // Tunable
#endif

CVMX_SHARED static uint32_t  pko_test_core_mask = 0xffff;

CVMX_SHARED static uint32_t  pko_test_oq_mask   = 0xf;

CVMX_SHARED static uint64_t  pkt_time_delay     = 1;



/* Decision on when to send a PKO packet from each core. */
static inline int
cvmcs_ok_to_send_pko_pkt(void)
{
	if(!(pko_test_core_mask & (1 << cvmx_get_core_num())))
		return 0;

	if(!pkts_per_sec || !pko_test_oq_mask)
		return 0;

	if(octdev_get_pko_state() != CVM_DRV_PKO_READY)
		return 0;

	if(cvmx_get_cycle() > (last_pko_run + pkt_time_delay)) {
		last_pko_run = cvmx_get_cycle();
		return 1;
	}

	return 0;
}






/* Get the packet size to use for the next PKO packet. */
static inline int
cvmcs_get_pko_pkt_size(void)
{
	uint32_t  data_size;

#if  (CVMCS_TEST_PKO == PKO_TEST_RANDOM_DATA)
	data_size = CVM_MIN_DATA + (cvmx_get_cycle() % (CVM_MAX_DATA - CVM_MIN_DATA + 1));
#elif (CVMCS_TEST_PKO == PKO_TEST_SEQUENTIAL_DATA)
	data_size = last_data_size++;
	if(last_data_size > CVM_MAX_DATA)
		last_data_size = CVM_MIN_DATA;
#elif (CVMCS_TEST_PKO == PKO_TEST_FIXED_DATA)
	data_size = CVM_MAX_DATA;
#endif

	return data_size;
}





#if  defined(PROGRAM_THRU_SCRATCH)
static int
cvmcs_test_pko_update_test_params(void *arg)
{
	uint64_t   scratch = cvmx_read_csr(CVMX_PEXP_NPEI_SCRATCH_1);
	uint32_t   op = scratch >> 32;
	uint32_t   value = scratch & 0xffffffff;

	switch(op) {
		case 1: /* pkts/sec */
			if(value != pkts_per_sec) {
				printf("PKO_TEST: Changing pkts/sec to %u\n", value);
				pkts_per_sec = value;
				pkt_time_delay = (pkts_per_sec)?cpu_freq/pkts_per_sec:0;
			}
			break;
		case 2: /* Queue mask */
			if(value != pko_test_oq_mask) {
				printf("PKO_TEST: Changing Queue Mask to 0x%08x\n", value);
				pko_test_oq_mask = value;
			}
			break;
		case 3: /* Core mask */
			if(value != pko_test_core_mask) {
				printf("PKO_TEST: Changing Core Mask to 0x%08x\n", value);
				pko_test_core_mask = value;
			}
			break;
		case 4:
			printf("PKO Test: %u pkts/sec coremask: 0x%08x queuemask: 0x%08x\n",
			      pkts_per_sec, pko_test_core_mask, pko_test_oq_mask);
			break;
		default:
			break;
	}
	cvmx_write_csr(CVMX_PEXP_NPEI_SCRATCH_1, value);
	CVMX_SYNCWS;

	return 0;
}
#endif







void
cvmcs_test_pko_global_init(void)
{
	if(pkts_per_sec)
		pkt_time_delay = cpu_freq/pkts_per_sec;

	pko_test_core_mask = (cvmx_sysinfo_get())->core_mask;

	if(OCTEON_IS_MODEL(OCTEON_CN56XX) || OCTEON_IS_MODEL(OCTEON_CN63XX) ||
	   OCTEON_IS_MODEL(OCTEON_CN66XX) ) {
		#if  defined(PROGRAM_THRU_SCRATCH)
		cvmcs_common_add_task(cpu_freq, cvmcs_test_pko_update_test_params, NULL);
		#endif
	}
}




static inline void
cvmcs_test_pko_sign_data(cvmx_buf_ptr_t  *gptr, int bufcount, int adjust)
{
#ifdef CVMCS_SIGN_DATA
	if(bufcount == 1) {
		cvmcs_sign_data(cvmx_phys_to_ptr(gptr->s.addr + adjust),
		                            gptr->s.size - adjust, 0);
	} else {
		int i, sign_byte = 0;

		for(i = 0; i < bufcount; i++) {
			sign_byte = cvmcs_sign_data(cvmx_phys_to_ptr(gptr[i].s.addr + adjust),
			                            gptr[i].s.size - adjust, sign_byte);
			adjust = 0;
		}
	}
#endif
}




/** This routine sends a packet to a PCI output queue. The size is passed
  * as an argument by the caller. The routine allocated multiple buffers,
  * if required, and calls a PCI core driver routine to send it on the
  * PCI output port specified.
  */
int
cvmcs_test_pko(uint32_t  oq_no, uint32_t size)
{
	cvmx_resp_hdr_t    *resp_hdr;
	uint8_t            *firstbuf;
	cvmx_buf_ptr_t      lptr;
	cvm_ptr_type_t      ptr_type = CVM_DIRECT_DATA;
	uint32_t            i, bufcount = 0, data_bytes = 0;


	/* Include space for the response header. */
	size += sizeof(cvmx_resp_hdr_t);

	/* Get the number of buffers required. */ 
	for(i = 0; i < size; i += CVM_FPA_TEST_POOL_SIZE) {
		bufcount++;
	}

	/* Atleast one buffer is always required. It will hold the response header
	   and data directly when bufcount = 1. For gather, it will hold the gather
	   list first followed by response header and data. */
	firstbuf = cvmx_fpa_alloc(CVM_FPA_TEST_POOL);

	if(!firstbuf) {
		printf("%s: First buf alloc failed\n",__FUNCTION__);
		return 1;
	}

	lptr.u64    = 0;
	lptr.s.addr = cvmx_ptr_to_phys(firstbuf);
	lptr.s.pool = CVM_FPA_TEST_POOL;
	lptr.s.i    = 1;


	if(bufcount == 1) {

		lptr.s.size = size;
		data_bytes  = size;

		cvmcs_test_pko_sign_data(&lptr, bufcount, sizeof(cvmx_resp_hdr_t));

		resp_hdr    = (cvmx_resp_hdr_t *)firstbuf;

	} else {
		cvmx_buf_ptr_t     *gptr;
		uint32_t            glistsize = 0, headroom = 0;

		ptr_type  = CVM_GATHER_DATA;

		glistsize = (bufcount * sizeof(cvmx_buf_ptr_t));

		/* If the bufcount calculated above does not have space for
		   the gather list, add another buffer to the count.*/
		if((bufcount * CVM_FPA_TEST_POOL_SIZE) < (size + glistsize))
			bufcount++;

		/* Recalculate glistsize */
		glistsize = (bufcount * sizeof(cvmx_buf_ptr_t));
		headroom  = glistsize + sizeof(cvmx_resp_hdr_t);

		/* The start of the buffer holds the gather list. */
		gptr = (cvmx_buf_ptr_t *)firstbuf;

		gptr[0].u64    = 0;
		gptr[0].s.addr = CVM_DRV_GET_PHYS(firstbuf) + glistsize;
		gptr[0].s.pool = CVM_FPA_TEST_POOL;
		gptr[0].s.back = glistsize/CVMX_CACHE_LINE_SIZE;
		gptr[0].s.size = CVM_FPA_TEST_POOL_SIZE - glistsize;

		data_bytes += gptr[0].s.size;

		/* Allocate the additional buffers. */
		for(i = 1; i < bufcount; i++) {
			uint8_t     *buf;
			uint32_t   thisbuf_size=0;

			/* Allocate from the TEST FPA pool. */
			buf = cvm_common_alloc_fpa_buffer(CVMCS_TEST_BUF_PTR, CVM_FPA_TEST_POOL);
			if(!buf) {
				printf("# cvmcs: Buffer[%d/%d] allocation failed in %s\n",
				       i, bufcount, __FUNCTION__);
				while(--i) {
					unsigned long ptr = ((gptr[i].s.addr >> 7) - gptr[i].s.back) << 7;
					cvm_common_free_fpa_buffer(cvmx_phys_to_ptr(ptr),
			                           gptr[i].s.pool, 0);
				}

				return 1;
			}

			thisbuf_size = CVM_FPA_TEST_POOL_SIZE;
	 
			if((data_bytes + thisbuf_size) > size)
				thisbuf_size = size - data_bytes;

			data_bytes += thisbuf_size;

			/* Create a gather list of buffers. */
			gptr[i].u64    = 0;
			gptr[i].s.addr = CVM_DRV_GET_PHYS(buf);
			gptr[i].s.pool = CVM_FPA_TEST_POOL;
			gptr[i].s.size = thisbuf_size;
			gptr[i].s.i    = 1;
		}


		/*for(i = 0; i < bufcount; i++) {
			printf("gptr[%d]: addr: %lx size: %d pool: %d back: %d free: %d\n", i,
			    (uint64_t)gptr[i].s.addr, gptr[i].s.size, gptr[i].s.pool,
			    gptr[i].s.back, gptr[i].s.i);
		}*/

		cvmcs_test_pko_sign_data(gptr, bufcount, headroom);

		lptr.s.size = bufcount;

		/* Response header follows the gather list but precedes the data. */
		resp_hdr = (cvmx_resp_hdr_t *)(firstbuf + glistsize);
	}


	/* Create a response header. If a dispatch function is registered
	   with the host PCI driver for this opcode, it will be processed,
	   else this packet will be dropped in the host driver. */
	resp_hdr->u64 = 0;
	/* Packets can one of two opcodes which are generated randomly. */
	if(cvmx_get_cycle() & 0x1)
		resp_hdr->s.opcode    = DROQ_PKT_OP2;
	else
		resp_hdr->s.opcode    = DROQ_PKT_OP1;
	resp_hdr->s.sport     = oq_no;
	resp_hdr->s.destqport = oq_no;


	CVMX_SYNCWS;

	/* Send a single buffer to PKO */
	if (cvm_send_pci_pko_direct(lptr, ptr_type, bufcount, data_bytes, oq_no)) {
		cvmx_buf_ptr_t  *gptr;
		i    = bufcount;
		gptr = (i == 1)?&lptr:cvmx_phys_to_ptr(lptr.s.addr);
		while(--i) {
			unsigned long ptr = ((gptr[i].s.addr >> 7) - gptr[i].s.back) << 7;
			cvm_common_free_fpa_buffer(cvmx_phys_to_ptr(ptr), gptr[i].s.pool,0);
		}
	}

	test_pkt_sent++;
	return 0;
}






#ifdef  MAX_PACKET_RATE_TEST
void
cvmcs_do_pko_test()
{
	cvmcs_max_pkt_pko_test();
}
#else
void
cvmcs_do_pko_test()
{
	if(cvmcs_ok_to_send_pko_pkt()) {
		int  i;
		for(i = 0; i < 32; i++) {
			if(pko_test_oq_mask & (1 << i))
				cvmcs_test_pko(i, cvmcs_get_pko_pkt_size());
		}
	}
}
#endif



static inline int
cvm_pko_get_db(int port_num)
{
	cvmx_pko_reg_read_idx_t pko_reg_read_idx;

	pko_reg_read_idx.u64 = 0;
	pko_reg_read_idx.s.index = port_num;
	cvmx_write_csr(CVMX_PKO_REG_READ_IDX, pko_reg_read_idx.u64);
	return cvmx_read_csr(CVMX_PKO_MEM_DEBUG9);
}



#ifdef  MAX_PACKET_RATE_TEST

//#define MAX_PKT_REUSE_BUF

#ifdef MAX_PKT_REUSE_BUF
static void *reuse_buf = NULL;
#endif

#define MAX_PKT_TEST_SIZES  8
#define TEST_TIME_PER_SIZE  20

CVMX_SHARED int pkt_size[MAX_PKT_TEST_SIZES] = { 64, 128, 256, 360, 512, 1024, 2048, 4096};
CVMX_SHARED int size_idx=0;
int max_pkt_loop_cnt=0;

void
cvmcs_max_pkt_pko_test(void)
{
    uint8_t              *buf;
    uint32_t              size, oq_no;
    cvmx_resp_hdr_t      *resp_hdr;
    cvmx_buf_ptr_t        lptr;
    static unsigned long  last_db_check=0;
    extern CVMX_SHARED uint64_t   cpu_freq;

    if(octdev_get_pko_state() != CVM_DRV_PKO_READY)
        return;

    oq_no = 0; //core_id;

    if(cvmx_get_core_num() == 0) {
       if(cvmx_get_cycle() > (last_db_check + cpu_freq))  {
          last_db_check = cvmx_get_cycle();
          if(max_pkt_loop_cnt++ >= TEST_TIME_PER_SIZE)  {
              max_pkt_loop_cnt = 0;
              size_idx++;
              if(size_idx == MAX_PKT_TEST_SIZES)
                 size_idx = 0;
              printf("Starting Max Packet rate PKO test for size: %d bytes\n",
                      pkt_size[size_idx]);
          }
       }

    }


    size = pkt_size[size_idx];

#ifdef MAX_PKT_REUSE_BUF
	if(cvmx_likely(reuse_buf)) {
		buf = reuse_buf;
	} else {
		buf = cvmx_fpa_alloc(CVM_FPA_TEST_POOL);
		if(buf == NULL)
			return;
		reuse_buf = buf;
	}
#else
	buf = cvmx_fpa_alloc(CVM_FPA_TEST_POOL);
    if(buf == NULL)
        return;
#endif


    lptr.u64    = 0;
    lptr.s.addr = CVM_DRV_GET_PHYS(buf);
    lptr.s.size = size + 8;
    lptr.s.pool = CVM_FPA_TEST_POOL;
#if !defined(MAX_PKT_REUSE_BUF)
    lptr.s.i    = 1;
#endif

    /* Send a single buffer to PKO */
    cvm_send_pci_pko_direct(lptr, CVM_DIRECT_DATA, 1, size, oq_no);
}
#endif


#endif



/* $Id: cvmcs-pko-test.c 66448 2011-10-25 02:35:14Z mchalla $ */
