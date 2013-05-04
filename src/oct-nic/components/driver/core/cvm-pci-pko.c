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



#include "cvm-driver-defs.h"
#include "octeon-opcodes.h"
#include <errno.h>
#include "cvmx-version.h"


CVMX_SHARED cvmx_spinlock_t  cvm_drv_pko_lock; 
extern  CVMX_SHARED  cvm_oct_dev_t    *oct;

#define CVM_PCI_PKO_MAP_ENTRIES     32


int
__get_active_pci_oq_count(void)
{
	int i, qcnt = 0;

	if(!OCTEON_IS_MODEL(OCTEON_CN68XX)) {
		printf("\n\n >>>> %s not supported for this model <<<<\n\n",
			__FUNCTION__);
		return -1;
	}

	for(i = 0; i < 32; i++) {
		if( cvmx_read_csr(CVMX_PEXP_SLI_PKTX_SLIST_BADDR(i)) && 
		    cvmx_read_csr(CVMX_PEXP_SLI_PKTX_SLIST_FIFO_RSIZE(i)) ) 
			qcnt++;
	}	

	return qcnt;
}



int 
setup_pci_pko_ports()
{
	int i, activeqcnt;
	int size = sizeof(struct __cvm_pci_pko_qmap) * CVM_PCI_PKO_MAP_ENTRIES;

	if(OCTEON_IS_MODEL(OCTEON_CN68XX)) {
		cvmx_sli_tx_pipe_t  slitxpipe;
		slitxpipe.u64 = cvmx_read_csr(CVMX_PEXP_SLI_TX_PIPE);  
		activeqcnt = __get_active_pci_oq_count();
		slitxpipe.s.nump = activeqcnt;
		cvmx_write_csr(CVMX_PEXP_SLI_TX_PIPE, slitxpipe.u64);  
		printf("[ DRV ] Active PCI Queues: %d (derived from checking queue registers)\n", activeqcnt);
	} else {
		activeqcnt = CVMX_PKO_QUEUES_PER_PORT_PCI * 4;
		printf("[ DRV ] Active PCI PKO Queues: %d (derived from cvmx-config)\n",
		       activeqcnt);
	}


	oct->pcipkomap = NULL;

	for(i = 0; i < CVMX_FPA_NUM_POOLS; i++) {
		if(cvmx_fpa_get_block_size(i) >= size) {
			oct->pcipkomap = cvmx_fpa_alloc(i);
			if(oct->pcipkomap) break;
		}
	}

	if(oct->pcipkomap == NULL)
		return 1;

	memset(oct->pcipkomap, 0, size);


	for(i = 0; i < activeqcnt; i++) {
		oct->pcipkomap[i].active = 1;
		if(OCTEON_IS_MODEL(OCTEON_CN68XX)) {
			oct->pcipkomap[i].port   = cvmx_pko_get_base_pko_port(oct->npi_if, i);
			oct->pcipkomap[i].queue  = cvmx_pko_get_base_queue_pkoid(cvmx_pko_get_base_pko_port(oct->npi_if, i));
		} else {
			oct->pcipkomap[i].port   = FIRST_PCI_PORT + i%4;
			oct->pcipkomap[i].queue  =
		       cvmx_pko_get_base_queue(oct->pcipkomap[i].port) +  (i/4);
		}
		printf("[ DRV ] PCIPKOMAP[%d] port: %d queue: %d\n", i, oct->pcipkomap[i].port,
			oct->pcipkomap[i].queue);
	}

	/* The rest of the entries are already marked inactive by memset */
	CVMX_SYNCWS;

	return 0;
}



int
cvm_pci_get_oq_pkoport(int oq)
{
	if(cvmx_unlikely(oq >= 32))
		return -1;

	if(cvmx_unlikely(oct->pcipkomap[oq].active == 0))
		return -1;

	return oct->pcipkomap[oq].port;
}



int
cvm_pci_get_oq_pkoqueue(int oq)
{
	if(cvmx_unlikely(oq >= 32))
		return -1;

	if(cvmx_unlikely(oct->pcipkomap[oq].active == 0))
		return -1;

	return oct->pcipkomap[oq].queue;
}







static inline int
__cvm_post_pko_command(cvmx_buf_ptr_t      lptr,
                       uint32_t            pko_ptr_type,
                       uint32_t            segs,
                       uint32_t            total_bytes,
                       uint32_t            port,
                       uint32_t            q_no,
                       uint32_t            flags,
                       uint64_t            flag_data)
{
	cvmx_pko_command_word0_t    pko_command;


	CVMX_SYNCWS;
	if(cvmx_unlikely(total_bytes > 65528 || segs > 63)) {
		printf("[ DRV ] Unsupported configuration: total_bytes: %d segs: %d\n", total_bytes, segs);
		printf("[ DRV ] total_bytes cannot be > 65528; segs cannot be > 63\n");
		return 1;
	}

#ifdef SANITY_CHECKS
	pci_output_sanity_and_buffer_count_update(lptr, pko_ptr_type, segs, total_bytes, port);
#endif

	/* Prepare to send a packet to PKO. */
	if (octeon_has_feature(OCTEON_FEATURE_PKND))
		cvmx_pko_send_packet_prepare_pkoid(port, q_no, 1);
	else
		cvmx_pko_send_packet_prepare(port, q_no, 1);


	/* Build a PKO pointer to this packet */
	pko_command.u64           = 0;
	/* Setting II = 0 and DF = 1 will free all buffers whose I bit is set. */
	pko_command.s.ignore_i    = 0;
	pko_command.s.dontfree    = 1;
	pko_command.s.segs        = segs;
	pko_command.s.total_bytes = total_bytes;
	/* For linked mode data and direct mode data this field is zero. */
	pko_command.s.gather      = (pko_ptr_type == PKO_GATHER_DATA);
	if(flags & CVM_USE_HW_CKSUM_OFFLOAD)
		pko_command.s.ipoffp1 = flag_data & 0xff;

	DBG_PRINT(DBG_FLOW,"\n>>>>>pko cmd: %016llx totalbytes: %d lptr: %016llx PORT: %d Q: %d\n",
	          cast64(pko_command.u64), total_bytes, cast64(lptr.u64), port, q_no);

	if (octeon_has_feature(OCTEON_FEATURE_PKND))
		cvmx_pko_send_packet_finish_pkoid(port, q_no, pko_command,lptr, 1);
	else
		cvmx_pko_send_packet_finish(port, q_no, pko_command,lptr, 1);

	return 0;
}





int
cvm_pko_send_direct(cvmx_buf_ptr_t   lptr,
                    cvm_ptr_type_t   ptr_type,
                    uint32_t         segs,
                    uint32_t         total_bytes,
                    uint32_t         port)
{
	int q_no = cvmx_pko_get_base_queue(port);

	if(cvmx_unlikely(oct->pko_state != CVM_DRV_PKO_READY)) {
		printf("[ DRV ] Core driver PKO is in state (0x%x); PKO Send abort!\n",
		     oct->pko_state);
		return 1;
	}

	return  __cvm_post_pko_command(lptr, (uint32_t)ptr_type, segs, total_bytes, port, q_no, 0, 0);
}





int
cvm_send_pci_pko_direct(cvmx_buf_ptr_t   lptr,
                        cvm_ptr_type_t   ptr_type,
                        uint32_t         segs,
                        uint32_t         total_bytes,
                        uint32_t         oq_no)
{
//	printf("%s lptr: 0x%016lx ptrtype: %d segs: %d total_bytes: %d oq_no: %d\n", __FUNCTION__, lptr.u64, ptr_type, segs, total_bytes, oq_no);

	if(cvmx_unlikely(oct->pko_state != CVM_DRV_PKO_READY)) {
		printf("[ DRV ] Core driver PKO is in state (0x%x); PKO Send abort!\n",
		     oct->pko_state);
		return 1;
	}

	if(cvmx_unlikely(!oct->pcipkomap[oq_no].active)) {
		printf("[ DRV ] %s: OQ# %d is not active\n", __FUNCTION__, oq_no);
		return 1;
	}

	return  __cvm_post_pko_command(lptr, (uint32_t)ptr_type, segs, total_bytes,
	             oct->pcipkomap[oq_no].port, oct->pcipkomap[oq_no].queue, 0, 0);
}




int
cvm_pko_send_direct_flags(cvmx_buf_ptr_t   lptr,
                          cvm_ptr_type_t   ptr_type,
                          uint32_t         segs,
                          uint32_t         total_bytes,
                          uint32_t         port,
                          uint32_t         flags,
                          uint64_t         flag_data)
{
	int q_no = cvmx_pko_get_base_queue(port);

	if(oct->pko_state != CVM_DRV_PKO_READY) {
		printf("[ DRV ] Core driver PKO is in state (0x%x); PKO Send abort!\n",
		       oct->pko_state);
		return 1;
	}

	return   __cvm_post_pko_command(lptr, (uint32_t)ptr_type, segs, total_bytes, port, q_no, flags, flag_data);
}





#define GATHER_LIST_OFFSET    4

/**
  *  Call this routine to send a data packet to the Octeon output 
  *  queues. For a linked mode or direct mode data, this routine
  *  adds another buffer to the start of the linked list into which the
  *  response header is copied. For a gather mode data sent by user,
  *  this routine creates a new gather list and makes the user
  *  response header the first element of the gather list. Needless
  *  to say, if performance is important, you should make space
  *  to add 8 bytes of response header before your data and call
  *  cvm_post_pko_command() directly. 
  *
  *  @param cvmx_resp_hdr_t  - the response header that identifies the
  *                            user data.
  *  @param total_bytes      - amount of user data (excludes response
  *                            header)
  *  @param segs  - number of user buffers (in linked and gather mode)
  *  @param port  - PKO port on which to send data ( 32 <= port <= 35)
  *  @param dptr  - pointer to the user data.
  *  @param flags - type of data pointed by dptr( linked/gather/direct)
  *
  *  @return  Success: returns 0, else 1.
  */
int
cvm_pko_send_data(cvmx_resp_hdr_t  *user_resp_hdr,
                  cvmx_buf_ptr_t    dptr,
                  cvm_ptr_type_t    ptr_type,
                  uint32_t          total_bytes,
                  uint32_t          segs,
                  uint32_t          port)
{
    cvmx_buf_ptr_t    lptr;
    cvmx_resp_hdr_t  *resp_hdr;
    uint16_t          gcount=0;
    cvmx_buf_ptr_t   *glist,*send_buf;

	if(oct->pko_state != CVM_DRV_PKO_READY) {
		printf("[ DRV ] Core driver PKO is in state (0x%x); PKO Send abort!\n",
		       oct->pko_state);
		return 1;
	}

    CVM_PCI_PKO_ALLOC_BUFFER(send_buf);
    if(!send_buf) {
       printf("[ DRV ] CVM_PKO: Allocation failed in sending PKO data\n");
       return 1;
    }

    resp_hdr = (cvmx_resp_hdr_t *)&send_buf[1];
    memcpy(resp_hdr, user_resp_hdr, CVMX_RESP_HDR_SIZE);


    if(ptr_type != CVM_GATHER_DATA)  {
          DBG_PRINT(DBG_FLOW, "Direct or linked mode data\n");

          /*  The response header adds 8 bytes to pko send data size. */
          total_bytes += 8; 
          segs ++;

          /* Creating a linked mode data packet for user data that is in
             DIRECT or LINKED mode. */
          /* The start of the buffer points to the data sent by user. */
          send_buf->u64    = dptr.u64;

          lptr.u64    = 0;
          lptr.s.size = 8;
          lptr.s.addr = CVM_DRV_GET_PHYS(&send_buf[1]);
          lptr.s.pool = CVM_PCI_PKO_FPA_POOL;
          lptr.s.i    = 1;  /* PKO SHOULD FREE THIS BUFFER */
          DBG_PRINT(DBG_FLOW,"lptr: %016llx segs: %d total_bytes: %d\n", cast64(lptr.u64), segs, total_bytes);
          CVMX_SYNCWS;
          cvm_pko_send_direct(lptr, PKO_LINKED_DATA, segs, total_bytes, port);

    } else {
          uint16_t           i;
          cvmx_buf_ptr_t    *ptr = (cvmx_buf_ptr_t *)CVM_DRV_GET_PTR(dptr.s.addr);

          glist = (send_buf + GATHER_LIST_OFFSET);

          /* Prepare the gather list */
          /* First element points to the response header contents (8Bytes)*/
          glist[gcount].u64    = 0;
          glist[gcount].s.addr = CVM_DRV_GET_PHYS(resp_hdr);
          glist[gcount].s.size = 8;
          total_bytes   += 8;
          gcount++;

          DBG_PRINT(DBG_FLOW,"cvm_pko_send: Data as a gather list @ %p\n", ptr);
          /* If dptr points to a gather list of user data, the size field
             holds the number of elements in the gather list  */
          for(i = 0; i < dptr.s.size; i++, ptr++) {
              DBG_PRINT(DBG_NORM,"copying Val: 0x%016llx from 0x%p into 0x%p\n",
                       cast64(ptr->u64), ptr, &glist[gcount]);
              glist[gcount++].u64 = ptr->u64;
          }
          DBG_PRINT(DBG_NORM,"glist @ %p, gcount: %d, total_bytes: %d\n",
                    glist, gcount, total_bytes);

          lptr.u64    = 0;
          lptr.s.size = gcount;
          lptr.s.addr = CVM_DRV_GET_PHYS(glist);
          lptr.s.pool = CVM_PCI_PKO_FPA_POOL;
          lptr.s.i    = 1;  /* PKO SHOULD FREE THIS BUFFER */
          CVMX_SYNCWS;
          cvm_pko_send_direct(lptr, PKO_GATHER_DATA, gcount, total_bytes, port);
    }
    return 0;
}




/* $Id: cvm-pci-pko.c 67233 2011-11-19 03:23:54Z panicker $ */
