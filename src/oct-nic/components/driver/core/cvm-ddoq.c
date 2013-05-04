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

#include "cvm-drv.h"
#include "cvm-cntq.h"
#include "cvm-ddoq-list.h"
#include "cvm-ddoq.h"
#include "octeon-error.h"
#include <errno.h>
#include "cvm-driver-defs.h"
#include "cvm-drv-debug.h"
#include "cvm-pci-loadstore.h"






CVMX_SHARED   cvm_ddoq_global_stats_t    cvm_ddoqs;

extern int    cvm_drv_core_id;

/* This field holds the callbacks registered. */
CVMX_SHARED  cvm_drv_ddoq_callback_t  cvm_drv_ddoq_appcallback[MAX_DDOQ_TYPES][MAX_DDOQ_ACTIONS];

octeon_ddoq_desc_t   credit_descs[1024];

#ifdef CVM_DDOQ_PROFILE
CVMX_SHARED struct ddoq_profile  dprof[MAX_DRV_CORES];
unsigned long      c_lastcount=0, d_lastcount=0;
#endif


static int cvm_ddoq_process_credit(cvmx_wqe_t     *wqe);
static int cvm_ddoq_process_init(cvmx_wqe_t       *wqe);
static int cvm_ddoq_process_delete(cvmx_wqe_t     *wqe);



static inline cvm_drv_ddoq_callback_t
cvm_ddoq_get_appcb(int ddoq_type, int ddoq_action)
{
	if(  (ddoq_type < MAX_DDOQ_TYPES) && (ddoq_action < MAX_DDOQ_ACTIONS) )
		return cvm_drv_ddoq_appcallback[ddoq_type][ddoq_action];

	return NULL;
}






#ifdef CVM_DDOQ_PROFILE
void
cvm_drv_init_ddoq_profile(void)
{
	int i;

	memset(dprof, 0, MAX_DRV_CORES * sizeof(struct ddoq_profile));
	for(i = 0; i < MAX_DRV_CORES; i++)
		dprof[i].c_min = dprof[i].d_min = -1;
}



void
cvm_drv_print_ddoq_profile(int interval)
{
	struct ddoq_profile  p[MAX_DRV_CORES];
	int                  i;
	unsigned long        t_max = 0, t_min = -1, t_count = 0, t_time = 0, avg;

	memcpy(p, dprof, MAX_DRV_CORES * sizeof(struct ddoq_profile));

	for(i = 0; i < MAX_DRV_CORES; i++) {
		t_max = (p[i].c_max > t_max)?p[i].c_max:t_max;
		t_min = (p[i].c_min < t_min)?p[i].c_min:t_min;
		t_count += p[i].c_count;
		t_time  += p[i].c_totaltime;
	}
	if(t_count) {
		avg = (t_count - c_lastcount)/interval;	
		printf("DDOQ Create Total: %lu (avg/sec): %lu\n Time min: %lu max: %lu avg: %lu\n", t_count, avg, t_min, t_max, (t_time/t_count));
		c_lastcount = t_count;
	}

	t_max = 0; t_min = -1; t_count = 0; t_time = 0;
	for(i = 0; i < MAX_DRV_CORES; i++) {
		t_max = (p[i].d_max > t_max)?p[i].d_max:t_max;
		t_min = (p[i].d_min < t_min)?p[i].d_min:t_min;
		t_count += p[i].d_count;
		t_time  += p[i].d_totaltime;
	}
	if(t_count) {
		avg = (t_count - d_lastcount)/interval;	
		printf("DDOQ Delete Total: %lu (avg/sec): %lu\n  Time min: %lu max: %lu avg: %lu\n", t_count, avg, t_min, t_max, (t_time/t_count));
		d_lastcount = t_count;
	}
	CVMX_SYNCW;
}
#endif










/* Applications call this routine to setup the DDOQ structures. 
 */ 
int
cvm_drv_ddoq_setup()
{
	if(cvm_init_ddoq_list()) {
		printf("[ DRV ] DDOQ List initialization failed\n");
		return 1;
	}
	/* Initialize the callback data structure */
	memset(cvm_drv_ddoq_appcallback, 0, (MAX_DDOQ_TYPES * MAX_DDOQ_ACTIONS * PTR_SIZE));
	memset(&cvm_ddoqs, 0, sizeof(cvm_ddoq_global_stats_t));
	cvm_drv_register_op_handler(DDOQ_INIT_OP, cvm_ddoq_process_init);
	cvm_drv_register_op_handler(DDOQ_CREDIT_OP, cvm_ddoq_process_credit);
	cvm_drv_register_op_handler(DDOQ_DELETE_OP, cvm_ddoq_process_delete);
	CVMX_SYNCW;
	return 0;
}











/* Print the contents of a DDOQ */
void
cvm_ddoq_print(cvm_ddoq_t   *ddoq)
{

    int i=0;
    cavium_list_t  *node;

    printf("DDOQ[%d]-----\n",  ddoq->ddoq_id);
    printf("  desc_ring : 0x%p\n", ddoq->desc_ring);
    printf("  desc_count      : 0x%x\n",  (uint32_t)ddoq->desc_count);
    printf("  avl_desc        : 0x%x\n",  (uint32_t)ddoq->avl_desc);
    printf("  buf_size        : 0x%x\n",  (uint32_t)ddoq->buf_size);
    printf("  oct_wr_index    : 0x%x\n",  (uint32_t)ddoq->core_wr_index);
    printf("  host_rd_index   : 0x%x\n",  (uint32_t)ddoq->host_rd_index);
    printf("  refill_index    : 0x%x\n",  (uint32_t) ddoq->refill_index);
    printf("  tag             : 0x%x\n",  (uint32_t) ddoq->tag);
    printf("  wqe             : 0x%p\n",  ddoq->wqe);
    printf("  wait_count      : 0x%x\n",  (uint32_t)ddoq->wait_count);
    printf("Wait list contents\n");
    cavium_list_for_each(node, &(ddoq->wait_list)) {
       cvm_ddoq_pkt_t   *pkt = (cvm_ddoq_pkt_t *)node;

       printf("Node[%d]:  pkt_size: 0x%x bufs_reqrd: %d buf0: 0x%llx\n",
                ++i, (uint32_t)pkt->pkt_size, (uint32_t)pkt->bufs_reqrd,
                cast64(pkt->buf[0].u64));
    }
    printf("\n");


    printf("  ----desc_ring contents\n");
    for ( i = 0; i < ddoq->desc_count; i++)
      printf("%d.  info_ptr: 0x%llx buf_ptr: 0x%llx\n",
             i, cast64(ddoq->desc_ring[i].info_ptr), 
             cast64(ddoq->desc_ring[i].buffer_ptr));
}









/* Prepare the ddoq_info structure when sending a packet to the
 * DDOQ buffers in host memory. 
 */
static inline uint64_t
cvm_ddoq_prepare_ddoq_info(uint8_t          *buf,
                           uint32_t          bufs_reqrd,
                           uint32_t          total_bytes,
                           cvmx_resp_hdr_t  *user_resp_hdr)
{
    octeon_ddoq_info_t     *ddoq_info;
    cvmx_buf_ptr_t          lptr;

    ddoq_info  = (octeon_ddoq_info_t *)buf;
    ddoq_info->reserved = 0;
    ddoq_info->buf_size = bufs_reqrd;
    ddoq_info->pkt_size = total_bytes;
    ddoq_info->resp_hdr = user_resp_hdr->u64;

    lptr.u64 = 0;
    lptr.s.i = 1;
    CVM_DDOQ_SET_FPA_POOL(lptr);
    lptr.s.size = OCT_DDOQ_INFO_SIZE;
    lptr.s.addr = CVM_DRV_GET_PHYS(ddoq_info);

    return lptr.u64;
}






static int
__send_dma_command(cvm_dma_remote_ptr_t    *rptr,
           cvmx_oct_pci_dma_local_ptr_t    *lptr,
           int                             *ridx,
           int                             *lidx,
           cvm_dma_remote_ptr_t            *nextrptr)
{
	cvmx_oct_pci_dma_inst_hdr_t     dmacmd;

	dmacmd.u64   = 0;
	dmacmd.s.c   = 0;
	dmacmd.s.lport = OCTEON_PCIE_PORT;
	dmacmd.s.nl  = *lidx;
	dmacmd.s.nr  = *ridx;
	CVMX_SYNCWS;

	if(cvmx_unlikely(cvm_pci_dma_raw(&dmacmd, lptr, rptr))) {
		printf("%s: Failed to send DMA command: 0x%016lx\n",
		       __FUNCTION__, dmacmd.u64);
		return 1;
	}

	*lidx = 0; *ridx = 0;

	return 0;
}




int
__get_next_rptr_size(int currrsize, int ddoq_buf_size, int total_bytes)
{
	if((currrsize + ddoq_buf_size) > total_bytes)
		return (total_bytes - currrsize);
	else
		return ddoq_buf_size;
}



static inline int
__add_rptr(cvm_dma_remote_ptr_t            *rptr,
           cvmx_oct_pci_dma_local_ptr_t    *lptr,
           cvm_dma_remote_ptr_t            *nextrptr,
           cvm_ddoq_t                      *ddoq,
           int                             *ridx,
           int                             *lidx,
           uint32_t                        *currlsize,
           uint32_t                        *currrsize,
           uint32_t                        *total_bytes)
{
        int retval = 0;
 
send_remaining:
        if(*currrsize < *currlsize)  {
                while( (*currrsize < *currlsize)
                         && (*ridx < (octdev_max_dma_remoteptrs() - 1)) )  {
 
                        dbg_printf("%d: currlsize: %d currrsize: %d lidx: %d ridx: %d sgidx: %d\n",__LINE__,
                   		*currlsize, *currrsize, *lidx, *ridx, ddoq->core_wr_index);
                        rptr[*ridx].s.addr = ddoq->desc_ring[ddoq->core_wr_index].buffer_ptr;
                        rptr[*ridx].s.size = __get_next_rptr_size(*currrsize, ddoq->buf_size, *total_bytes);
                        *currrsize += rptr[*ridx].s.size;
                        INCR_INDEX_BY1(ddoq->core_wr_index, ddoq->desc_count);
                        (*ridx)++;
                }
 
        } else {
                if(*currrsize != *currlsize)  {
                        rptr[*ridx].s.size -= (*currlsize - *currrsize);
                }
        }
 
        if( cvmx_likely((*ridx < (octdev_max_dma_remoteptrs() - 1))
                         && (*lidx < octdev_max_dma_localptrs()) )) {
                return 0;
        }

	if(*currrsize > *currlsize) {
		int adjust = (*currrsize - *currlsize);
                /* We have collected enough rptr's to send the local buf contents,
		   		   but size of the last rptr added exceeds the local data size.
                   Adjust the last rptr and save the space left out from the rptr
                   to be used for next DMA. */
                dbg_printf("Adjust rptr[%d]: 0x%016llx to", *ridx-1,
                           rptr[(*ridx - 1)].u64);
                rptr[(*ridx - 1)].s.size -= adjust;
                nextrptr->s.size = adjust;
                nextrptr->s.addr = rptr[(*ridx - 1)].s.addr + rptr[(*ridx - 1)].s.size;
                dbg_printf(" 0x%016llx nextrptr: 0x%016llx\n", rptr[(*ridx - 1)].u64,
                           nextrptr->u64);
                *currrsize -= adjust;
        }


		/* We cannot add any more remote pointers. Either we have exactly enough
           space in the rptr buf (rsize == lsize), or we have reached the max
           rptrs. If we have maxed out in pointer count and we still don't have
	   	   enough rptr buf space, adjust the local buf ptrs. */
        if(*currrsize < *currlsize) {
 
                cvmx_oct_pci_dma_local_ptr_t    savedlptrs[15];
                cvmx_oct_pci_dma_inst_hdr_t     dmacmd;
                int savedidx=0;
 
                /* Got back in the local buffer list till the sum of local buffers
                   matches or just exceed by one local pointer's worth. */
                while( (*currlsize - lptr[(*lidx - 1)].s.size) >= *currrsize) {
                        (*lidx)--;
                        *currlsize -= lptr[*lidx].s.size;
                        savedlptrs[savedidx].u64 = lptr[*lidx].u64;
                        savedidx++;
                }
 
                /* At this point, either currrsize = currlsize
                   or currrsize < currlsize */
                if(*currrsize < *currlsize) {
                        int adjust = 0;
                        dbg_printf("Adjust lptr[%d]: 0x%016llx to", *lidx-1,
                                  lptr[(*lidx - 1)].u64);
                        adjust = (*currlsize - *currrsize);
                        lptr[(*lidx - 1)].s.size -= adjust;
                        lptr[(*lidx - 1)].s.i     = 0;
                        savedlptrs[savedidx].u64 = 0;
                        savedlptrs[savedidx].s.size = adjust;
                        savedlptrs[savedidx].s.addr =
                                      lptr[(*lidx - 1)].s.addr + lptr[(*lidx - 1)].s.size;
                        savedlptrs[savedidx].s.pool = lptr[(*lidx - 1)].s.pool;
                        savedlptrs[savedidx].s.back = lptr[(*lidx - 1)].s.back + ((savedlptrs[savedidx].s.addr - (lptr[(*lidx - 1)].s.addr & ~(CVMX_FPA_PACKET_POOL_SIZE - 1))) >> 7);

                        savedlptrs[savedidx].s.i = 1;
                        savedidx++;
                        dbg_printf(" 0x%016llx nextlptr: 0x%016llx\n", lptr[(*lidx - 1)].u64,
                                   savedlptrs[savedidx-1].u64);
                        *currlsize -= adjust;
                }
		
				/* We've made all the adjustments we need. Send the DMA Command now. */
                dmacmd.u64   = 0;
                dmacmd.s.c   = 0;
                dmacmd.s.nl  = *lidx;
                dmacmd.s.nr  = *ridx;
                CVMX_SYNCWS;
 
                if(cvmx_unlikely(cvm_pci_dma_raw(&dmacmd, lptr, rptr))) {
                        printf("%s: Failed to send DMA command: 0x%016llx\n",
                               __FUNCTION__, dmacmd.u64);
                        return 1;
                }
 
                *total_bytes -= *currlsize;
 
                /* Now that the DMA command was sent successfully, lets get all the
                   local pointers we rolled back. */
                *lidx = 0; *ridx = 0; *currlsize = 0; *currrsize = 0;
                while(*lidx < savedidx) {
                        lptr[*lidx].u64 = savedlptrs[*lidx].u64;
                        *currlsize += lptr[*lidx].s.size;
                        (*lidx)++;
                }
 
                /* Get rptr's for the remaining local buffers. */
                goto send_remaining;
        }
 
        /* currlsize = currsize. Send DMA now. */
        *total_bytes -= *currlsize;
        retval =  __send_dma_command(rptr, lptr, ridx, lidx, nextrptr);
 
        *currlsize = 0; *lidx = 0; *ridx = 0; *currrsize = 0;
        if(nextrptr->u64) {
                dbg_printf("nextrptr exists (0x%016llx), copied to idx %d\n",
                        nextrptr->u64, *ridx);
                rptr[*ridx].u64 = nextrptr->u64;
                *currrsize = rptr[*ridx].s.size;
                nextrptr->u64 = 0;
                (*ridx)++;
        }
 
        return retval;
 
}











/* This routine does the low-level stuff required to send a DDOQ packet.
 * It creates the CNTQ packet to be sent as indication of a packet arrival
 * in the DDOQ. It creates the PCI DMA command required to send the DDOQ
 * packet. It then sends the DDOQ packet followed by the CNTQ packet via
 * the PCI DMA queues.
 */
static int
cvm_ddoq_send_dma_data(cvm_ddoq_t      *ddoq,
                       cvmx_buf_ptr_t   databufs,
                       cvm_ptr_type_t   ptr_type,
                       uint32_t         segs,
                       uint32_t         total_bytes,
                       uint32_t         bufs_reqrd,
                       cvmx_resp_hdr_t *user_resp_hdr)
{
    int                            i=0, bufidx = 0, lidx= 0, ridx = 0;
	int                            retval = 0;
	uint32_t                       currrsize = 0, currlsize = 0;
    uint8_t                       *resp_buf;
    cvm_cntq_pkt_t                *cntq_pkt;
    octeon_ddoq_ind_t             *ddoq_ind;
    cvmx_oct_pci_dma_local_ptr_t  *dptr, *gptr, lptr[OCTEON_MAX_DMA_LOCAL_POINTERS];
    cvm_dma_remote_ptr_t           rptr[OCTEON_MAX_DMA_REMOTE_POINTERS];
    cvm_dma_remote_ptr_t           nextrptr;
    cvmx_oct_pci_dma_inst_hdr_t    dma_hdr;

    dbg_printf("\n----cvm_ddoq_send_dma_data----\n");
    dbg_printf(" dptr: 0x%016llx ptr_type: %d segs: %d\n",
              cast64(databufs.u64), ptr_type, segs);
    dbg_printf("total_bytes: %d bufs_reqrd: %d resp_hdr: 0x%016llx\n",
              total_bytes, bufs_reqrd, cast64(user_resp_hdr->u64));

    if(total_bytes > (ddoq->desc_count * ddoq->buf_size)) {
        printf(" [ DRV ] ddoq_send_dma total_bytes: %d desc_count: %d  buf_size: %d\n", total_bytes, ddoq->desc_count, ddoq->buf_size);
	cvm_ddoqs.pkts_dropped_nomem++;
        return 1;
    }

	if(ptr_type == CVM_GATHER_DATA) {
		gptr = (cvmx_oct_pci_dma_local_ptr_t *)CVM_DRV_GET_PTR(databufs.s.addr);
	} else {
		gptr = (cvmx_oct_pci_dma_local_ptr_t *)&databufs;
	}

	for(i = 0; i < segs; i++) {
		if(gptr[i].s.size > 0x1fff) {
			printf("[ DRV ] Invalid size in ddoq send buffer[%d]: %d\n",
				   i, gptr[i].s.size);
			return 1;
		}
	}
	
	CVM_DDOQ_ALLOC_BUFFER(resp_buf);
    if(!resp_buf)  {
       printf("[ DRV ] DDOQ: Couldn't allocate resp buf for ddoq packet\n");
       cvm_ddoqs.pkts_dropped_nomem++;
       return 1;
    }

    /* Allocate the cntq packet before-hand so that we are ready to send the
       packet indication on CNTQ as soon as the DDOQ pkt is sent */
    cntq_pkt = cvm_alloc_cntq_packet(CVMX_HIGHP_DMA_QUEUE, 8);
    if(!cntq_pkt) {
       printf("[ DRV ] DDOQ: Couldn't allocate cntq pkt for ddoq packet\n");
       cvm_ddoqs.pkts_dropped_nomem++;
       CVM_DDOQ_FREE_BUFFER(resp_buf);
       return 1;
    }
    cntq_pkt->resp_hdr.u64         = 0;
    cntq_pkt->resp_hdr.s.opcode    = DDOQ_PKT_OP;
    cntq_pkt->resp_hdr.s.destqport = ddoq->ddoq_id;
    cntq_pkt->resp_hdr.s.request_id = ddoq->next_pkt_cntr++ & 0xffff;
    ddoq_ind = (octeon_ddoq_ind_t  *)cntq_pkt->data;
    ddoq_ind->u64 = 0;
    ddoq_ind->s.pkt_cnt=1;
    DBG_PRINT(DBG_FLOW,"Sending cntqpkt with resp_hdr: 0x%016llx\n",
              cast64(cntq_pkt->resp_hdr.u64));

   /* Initialize the dma header. */
   /* The FL and II bits remain set to 0 - buffers with I bit set will 
      be freed.  ptr is NULL; DIR is outbound; C is 0; 
      CA is 0 (DO NOT add to the dma count); */
    dma_hdr.u64  = 0;
    dma_hdr.s.ca = 0;
    lidx = ridx = 0;

	lptr[lidx].u64 = cvm_ddoq_prepare_ddoq_info(resp_buf + 8, bufs_reqrd, total_bytes, user_resp_hdr);
	currlsize += lptr[lidx].s.size;
	lidx++;

	nextrptr.u64 = 0;
	
    /* The ddoq_info bytes are written first into the remote host. */
    rptr[ridx].s.addr = ddoq->desc_ring[ddoq->core_wr_index].info_ptr;
    rptr[ridx].s.size = OCT_DDOQ_INFO_SIZE;
	currrsize = OCT_DDOQ_INFO_SIZE;
	ridx++;
	
	total_bytes += OCT_DDOQ_INFO_SIZE;
	
	dptr = (cvmx_oct_pci_dma_local_ptr_t *)&gptr[bufidx];

	while(bufidx < segs) {

		lptr[lidx].u64    = dptr->u64;
		currlsize += lptr[lidx].s.size;
		lidx++; bufidx++;

		if(__add_rptr(rptr, lptr, &nextrptr, ddoq, &ridx, &lidx, &currlsize, &currrsize, &total_bytes)) {
			return 1;
		}

		/* Get the next buffer pointer */
		dptr = (cvmx_oct_pci_dma_local_ptr_t *)&gptr[bufidx];
		dbg_printf(">> Next pkt ptr @ 0x%016lx\n", dptr->u64);
	}


	if(lidx || ridx) {
		__add_rptr(rptr, lptr, &nextrptr, ddoq, &ridx, &lidx, &currlsize, &currrsize, &total_bytes);
		if(__send_dma_command(rptr, lptr, &ridx, &lidx, &nextrptr)) {
			return 1;
		}
	}
	
	if(!retval) {
       if(cvm_cntq_send_packet(cntq_pkt)) {
          printf("[DRV] Critical Error!! CNTQ send failed for ddoq pkt\n");
          /** TODO: recover from here. */
       }
       cvm_ddoqs.pkts_sent++;
    } else {
       /* DDOQ packet was not sent here. */
       cvm_ddoqs.pkts_dropped_dmaerr++;
    }

	return retval;
}






/* Called during the process of handling the credit for a DDOQ.
 * This routine checks if the packets in the wait list for the
 * DDOQ can be dispatched with the available credit and sends
 * as many as it can before the credit is exhausted.
 * This routine is called with the DDOQ lock held. 
 * It returns the number of buffers used from the available credit
 * in this invocation.
 */
int
cvm_ddoq_process_wait_list(cvm_ddoq_t   *ddoq)
{
    int contval = 0;
    int  bufs_used = 0;

    DBG_PRINT(DBG_FLOW,"\n----cvm_ddoq_process_wait_list----\n");
    while(ddoq->wait_count && !contval) {
       cvm_ddoq_pkt_t   *ddoq_pkt;
       cvmx_buf_ptr_t   lptr;

       ddoq_pkt = (cvm_ddoq_pkt_t *)ddoq->wait_list.le_next;

       if( ddoq_pkt->bufs_reqrd <= ddoq->avl_desc) {

          if(ddoq_pkt->ptr_type == CVM_DIRECT_DATA 
             || ddoq_pkt->ptr_type == CVM_LINKED_DATA)  {
               lptr.u64 = ddoq_pkt->buf[0].u64;
               /* The ddoq buf ptr could be in DMA BUF format (13-bit size) */
               lptr.s.size = ddoq_pkt->buf[0].s.size;
           } else {
               lptr.u64 = 0;
               lptr.s.addr = CVM_DRV_GET_PHYS(ddoq_pkt->buf);
               lptr.s.size = ddoq_pkt->buf_count;
           }
           if(!cvm_ddoq_send_dma_data(ddoq, lptr, ddoq_pkt->ptr_type,
                                 ddoq_pkt->buf_count, ddoq_pkt->pkt_size,
                                 ddoq_pkt->bufs_reqrd, &(ddoq_pkt->resp_hdr)))
           {
               /* Remove from the front of the wait list. */
               cavium_list_del(&(ddoq_pkt->list));
               ddoq->avl_desc -= ddoq_pkt->bufs_reqrd;
               bufs_used += ddoq_pkt->bufs_reqrd;
               ddoq->pkts_sent++;
               ddoq->wait_count--;
               cvm_release_ddoq_packet(ddoq_pkt);
               CVMX_SYNCWS;
            } else {
               drv_err("[ DRV ] DDOQ[%d]: Wait list pkt could not be sent\n",
                      ddoq->ddoq_id);
               contval = 1;
            }
       } else
          contval = 1;
   } /* end while(wait count) */
   return bufs_used;
}




static inline void
cvm_ddoq_copy_credit_desc_ring(uint32_t             total_udd_size,
                               cvmx_wqe_t          *wqe)
{
   octeon_ddoq_desc_t    *credit_desc_ring, *credit_temp_buf;
   cvmx_buf_ptr_t         next_ptr;
   cvm_ddoq_credit_t     *ddoq_credit;
   uint32_t               bufs, curr_size, curr_ring_size, ring_size;


   credit_temp_buf = credit_descs;

   ddoq_credit = (cvm_ddoq_credit_t *)(CAST64(wqe->packet_ptr.s.addr));
   bufs        = (uint32_t)wqe->word2.s.bufs;

   credit_desc_ring = (octeon_ddoq_desc_t *)&(ddoq_credit->desc_ring);
   ring_size      = ddoq_credit->dfront.start.s.desc_count * OCT_DDOQ_DESC_SIZE;
   next_ptr  = wqe->packet_ptr;
   curr_size = ( ( (cvmx_read_csr(CVMX_IPD_PACKET_MBUFF_SIZE) & 0xfff)
                 - ((cvmx_read_csr(CVMX_IPD_1ST_MBUFF_SKIP) & 0x3f) + 1) ) * 8)
                 - total_udd_size
                 - (next_ptr.s.addr & 0x7);

   /* The RAW instr bytes should be subtracted*/
   curr_ring_size = curr_size - (CVM_RAW_FRONT_SIZE );

   if(ring_size < curr_ring_size)
      curr_ring_size = ring_size;

   DBG_PRINT(DBG_NORM, "wqe @ %lx pkt_ptr: %lx ddoq_desc_ring: %lx init_desc_ring: %lx\n", (uint64_t)wqe, wqe->packet_ptr.u64, (uint64_t)credit_temp_buf, (uint64_t)credit_desc_ring);
   DBG_PRINT(DBG_NORM, "bufs: %d  curr_ring_size: %d ring_size: %d next_ptr: %lx\n", bufs, curr_ring_size, ring_size, next_ptr.u64);
   memcpy(credit_temp_buf, credit_desc_ring, curr_ring_size);



   while(--bufs){
      ring_size -= curr_ring_size;
      next_ptr = (cvmx_buf_ptr_t)
                *(((cvmx_buf_ptr_t *)cvmx_phys_to_ptr(next_ptr.s.addr)) - 1);
      credit_desc_ring = (octeon_ddoq_desc_t *)(CAST64(next_ptr.s.addr));
      credit_temp_buf = (octeon_ddoq_desc_t *)
                       ((uint8_t *)credit_temp_buf + curr_ring_size);
      curr_ring_size =
          ((cvmx_read_csr(CVMX_IPD_PACKET_MBUFF_SIZE) & 0xfff)
           - ((cvmx_read_csr(CVMX_IPD_NOT_1ST_MBUFF_SKIP) & 0x3f) + 1)) * 8;
      if(ring_size < curr_ring_size)
         curr_ring_size = ring_size;

      DBG_PRINT(DBG_NORM, "ddoq_desc_ring: %lx init_desc_ring: %lx\n", (uint64_t)credit_temp_buf, (uint64_t)credit_desc_ring);
      DBG_PRINT(DBG_NORM, "bufs: %d  curr_ring_size: %d ring_size: %d next_ptr: %lx\n", bufs, curr_ring_size, ring_size, next_ptr.u64);
      memcpy(credit_temp_buf, credit_desc_ring, curr_ring_size);
   }
}






/* This routine handles the credits received for a DDOQ.
 */
static int
cvm_ddoq_process_credit(cvmx_wqe_t           *wqe)
{
	int                    i,idx, retval = 0;
	cvm_ddoq_credit_t     *ddoq_credit; 
	cvm_ddoq_t            *ddoq;
	octeon_ddoq_desc_t    *desc_ring;
	ddoq_front_data_t      ddoq_front;

	DBG_PRINT(DBG_FLOW,"\n----cvm_ddoq_process_credit----\n");

	ddoq_credit = (cvm_ddoq_credit_t *)CVM_DRV_GET_PTR(wqe->packet_ptr.s.addr);
#ifdef SANITY_CHECKS
	if(wqe->word1.tag_type   != CVMX_POW_TAG_TYPE_ATOMIC) {
		printf("[ DRV ] CRITICAL!!!  DDOQ[%d] CREDIT has tag_type set to %d\n", ddoq_credit->dfront.start.s.ddoq_id, wqe->word1.tag_type);
		retval = 1; goto DDOQ_CREDIT_FAILED;
	}
#endif

	DBG_PRINT(DBG_FLOW,"Credit %d descs for DDOQ[%d]\n", ddoq_credit->dfront.start.s.desc_count, ddoq_credit->dfront.start.s.ddoq_id);

	cvm_ddoqs.credit_pkts++;
	ddoq = cvm_get_ddoq_ptr(ddoq_credit->dfront.start.s.ddoq_id);

	if(ddoq) {
		if(ddoq->tag != wqe->word1.tag) {
			printf("[ DRV ] CRITICAL!!! DDOQ[%d] CREDIT expected tag 0x%x received 0x%x\n", ddoq->ddoq_id, ddoq->tag, (uint32_t)wqe->word1.tag);
			retval = 1; goto DDOQ_CREDIT_FAILED;
		}

		cvmx_spinlock_lock(&(ddoq->lock));
		if(ddoq->delete_active) {
			printf("[ DRV ] DDOQ[%d]: Credit received awaiting deletion\n", ddoq->ddoq_id);
			cvmx_spinlock_unlock(&(ddoq->lock));
			cvm_ddoqs.credit_on_delete++;
			retval = 1; goto DDOQ_CREDIT_FAILED;
		}

		if(ddoq_credit->dfront.start.s.desc_count <= (ddoq->desc_count - ddoq->avl_desc)) {
			idx       = ddoq->refill_index;
			desc_ring = &ddoq_credit->desc_ring;
			cvm_ddoq_copy_credit_desc_ring(sizeof(ddoq_front_data_t), wqe);
			for ( i = 0; i < ddoq_credit->dfront.start.s.desc_count; i++) {
				ddoq->desc_ring[idx].info_ptr   = credit_descs[i].info_ptr;
				ddoq->desc_ring[idx].buffer_ptr = credit_descs[i].buffer_ptr;
				INCR_INDEX_BY1(idx, ddoq->desc_count);
			}
			ddoq->avl_desc    +=  ddoq_credit->dfront.start.s.desc_count;
			ddoq->refill_index = idx;
			ddoq->credits_recvd++;

			if(ddoq->wait_count)  {
				/* If there are packets in wait list, anything sent as
				credit to an application callback should be whatever's
				left after wait queue processing. */
				ddoq_credit->dfront.start.s.desc_count -= cvm_ddoq_process_wait_list(ddoq);
			}
				
			cvmx_spinlock_unlock(&(ddoq->lock));

			if(!ddoq->wait_count) {
				cvm_drv_ddoq_callback_t  appcb;
				appcb = cvm_ddoq_get_appcb(ddoq->type, DDOQ_ACTION_CREDIT);
 				if(appcb) {
					memcpy(&ddoq_front, &ddoq_credit->dfront, sizeof(ddoq_front_data_t));
					if(appcb(ddoq->tag, ddoq->ddoq_id, &(ddoq_front.start))) {
						printf("[ DRV ] DDOQ CREDIT callback failed. Front data was ->\n");
						cvm_drv_print_data(&ddoq_front.start, 40);
					}
				}
			}
		} else {
			printf("[ DRV ] DDOQ[%d]: Host credited :%d Reqrd:%d\n", ddoq->ddoq_id, ddoq_credit->dfront.start.s.desc_count, (ddoq->desc_count - ddoq->avl_desc));
			cvm_ddoqs.invalid_credit++;
			cvmx_spinlock_unlock(&(ddoq->lock));
			retval = 1; goto DDOQ_CREDIT_FAILED;
		}
		//cvm_ddoq_print(ddoq);
	} else {
		printf("[ DRV ] DDOQ: Credit for unknown ddoq: %d\n", ddoq_credit->dfront.start.s.ddoq_id);
		cvm_ddoqs.invalid_credit++;
		cvm_drv_print_wqe(wqe);
		printf("[ DRV ] Packet ptr contents\n");
		cvm_drv_print_data(((uint8_t *)CVM_DRV_GET_PTR(wqe->packet_ptr.s.addr)), 64);
	}
DDOQ_CREDIT_FAILED:
	cvm_free_host_instr(wqe);
	return retval;
}









/* Macro to copy the descriptors received in a DDOQ INIT instruction into
 * the descriptor ring of a newly created DDOQ.
 */
static inline void
cvm_ddoq_copy_desc_ring(octeon_ddoq_desc_t  *ddoq_desc_ring,
                        uint32_t             total_udd_size,
                        cvmx_wqe_t          *wqe)
{
   octeon_ddoq_desc_t    *init_desc_ring;
   cvmx_buf_ptr_t         next_ptr;
   cvm_ddoq_init_t       *ddoq_init;
   uint32_t               bufs, curr_size, curr_ring_size, ring_size;


   ddoq_init = (cvm_ddoq_init_t *)CVM_DRV_GET_PTR(wqe->packet_ptr.s.addr);
   bufs      = (uint32_t)wqe->word2.s.bufs;

   init_desc_ring = (octeon_ddoq_desc_t *)&(ddoq_init->desc_ring);
   ring_size      = ddoq_init->dfront.avl_desc * OCT_DDOQ_DESC_SIZE;
   next_ptr  = wqe->packet_ptr;
   curr_size = ( ( (cvmx_read_csr(CVMX_IPD_PACKET_MBUFF_SIZE) & 0xfff)
                 - ((cvmx_read_csr(CVMX_IPD_1ST_MBUFF_SKIP) & 0x3f) + 1) ) * 8)
                 - total_udd_size
                 - (cast64(CVM_DRV_GET_PTR(next_ptr.s.addr)) & 0x7);

   /* The RAW instr bytes should be subtracted*/
   curr_ring_size = curr_size - (CVM_RAW_FRONT_SIZE );

   if(ring_size < curr_ring_size)
      curr_ring_size = ring_size;

   DBG_PRINT(DBG_NORM, "wqe @ %p pkt_ptr: %llx ddoq_desc_ring: %p init_desc_ring: %p\n", wqe, cast64(wqe->packet_ptr.u64), ddoq_desc_ring, init_desc_ring);
   DBG_PRINT(DBG_NORM, "bufs: %d  curr_ring_size: %d ring_size: %d next_ptr: %llx\n", bufs, curr_ring_size, ring_size, cast64(next_ptr.u64));
   memcpy(ddoq_desc_ring, init_desc_ring, curr_ring_size);



   while(--bufs){
      ring_size -= curr_ring_size;
      next_ptr = *( ((cvmx_buf_ptr_t *) CVM_DRV_GET_PTR(next_ptr.s.addr)) - 1);
      init_desc_ring = (octeon_ddoq_desc_t *)CVM_DRV_GET_PTR(next_ptr.s.addr);
      ddoq_desc_ring = (octeon_ddoq_desc_t *)
                       ((uint8_t *)ddoq_desc_ring + curr_ring_size);
      curr_ring_size =
          ((cvmx_read_csr(CVMX_IPD_PACKET_MBUFF_SIZE) & 0xfff)
           - ((cvmx_read_csr(CVMX_IPD_NOT_1ST_MBUFF_SKIP) & 0x3f) + 1)) * 8;
      if(ring_size < curr_ring_size)
         curr_ring_size = ring_size;

      DBG_PRINT(DBG_NORM, "ddoq_desc_ring: %p init_desc_ring: %p\n", ddoq_desc_ring,
                init_desc_ring);
      DBG_PRINT(DBG_NORM, "bufs: %d  curr_ring_size: %d ring_size: %d next_ptr: %llx\n",
                bufs, curr_ring_size, ring_size, cast64(next_ptr.u64));
      memcpy(ddoq_desc_ring, init_desc_ring, curr_ring_size);
   } 
}




void
cvm_ddoq_set_ddoq_host_state(cvmx_wqe_t    *wqe,
                             uint64_t       host_status_addr,
                             uint32_t       ddoq_status,
                             int            free_wqe)
{
	cvm_pci_mem_writel(host_status_addr, ddoq_status);
	if(free_wqe)
		cvm_free_host_instr(wqe);
}







/* This routine handles the creation of a new DDOQ on receiving a
 * DDOQ INIT instruction from host. It checks if the DDOQ already exists and 
 * of doesn't, it allocates memory for the DDOQ structure, adds the structure
 * to the lookup table and responds to the instruction. No response is sent
 * if the DDOQ could not be created in the core driver.
 */
static int
cvm_ddoq_process_init(cvmx_wqe_t       *wqe)
{
      int                    retval=0;
      cvm_ddoq_init_t       *ddoq_init = (cvm_ddoq_init_t *)wqe->packet_data;
      cvm_ddoq_t            *ddoq = NULL;
      ddoq_front_data_t      ddoq_front;

#ifdef CVM_DDOQ_PROFILE
      unsigned long          c_start = cvmx_get_cycle(), c_time;
#endif

      DBG_PRINT(DBG_FLOW,"\n----cvm_ddoq_process_init_instr----\n");
#ifdef SANITY_CHECKS
      if(wqe->word1.tag_type   != CVMX_POW_TAG_TYPE_ATOMIC) {
          printf("CRITICAL!!!  DDOQ INIT has tag_type %d\n", wqe->word1.tag_type);
         retval = 1; goto DDOQ_INIT_FAILED;
      }
#endif

      CVM_DDOQ_ALLOC_BUFFER(ddoq);
      if(ddoq) {
          memset(ddoq, 0, CVM_DDOQ_SIZE);

          ddoq->desc_count    = ddoq_init->dfront.start.s.desc_count;
          ddoq->desc_ring     = cvm_ddoq_get_buffer(ddoq->desc_count * sizeof(octeon_ddoq_desc_t));
          if(ddoq->desc_ring == NULL) {
              printf("[ DRV ] Descriptor ring allocation failed for DDOQ[%d]\n", ddoq_init->dfront.start.s.ddoq_id);
              CVM_DDOQ_FREE_BUFFER(ddoq);
              cvm_ddoqs.create_fail_nomem++;
              retval = 1; goto DDOQ_INIT_FAILED;
          }
          cvm_ddoq_copy_desc_ring(ddoq->desc_ring, sizeof(ddoq_front_data_t), wqe);

          /* Once the copy is done, we have no more use for the packet_ptr
             buffers. Free them now. */
          cvm_free_wqe_packet_bufs(wqe);

          /* The avl_desc field in front data informs core of actual
             descriptors available at INIT time. */
          if(cvmx_wqe_get_len(wqe) - CVM_RAW_FRONT_SIZE > 8)
             ddoq->avl_desc   = (uint16_t)ddoq_init->dfront.avl_desc;

          /* If not all descriptors were populated in INIT, the next 
             credit should start its copy after the available descriptors. */
          if(ddoq->avl_desc != ddoq->desc_count)
              ddoq->refill_index = ddoq->avl_desc;

          ddoq->buf_size      = ddoq_init->dfront.start.s.buf_size;
          ddoq->ddoq_id       = ddoq_init->dfront.start.s.ddoq_id;
          ddoq->type          = ddoq_init->dfront.start.s.type;
          ddoq->next_pkt_cntr = (uint32_t)ddoq_init->dfront.next_pkt_cntr;
          ddoq->refill_threshold = ddoq_init->dfront.refill_threshold;
          ddoq->tag           = wqe->word1.tag;
          CAVIUM_INIT_LIST_HEAD(&(ddoq->wait_list));
          if(ddoq->type == DDOQ_TYPE_TCP) 
             memcpy(&ddoq_front, &ddoq_init->dfront, sizeof(ddoq_front_data_t));
          cvmx_spinlock_init(&(ddoq->lock));

          DBG_PRINT(DBG_FLOW,"DDOQ: ddoq created. Adding to list..");
          if(cvm_add_to_ddoq_list(ddoq)) {
              printf("[ DRV ] DDOQ: Failed to add DDOQ[%d] to list\n",ddoq->ddoq_id);
              /* Packet pool buffers were freed in copy_desc_ring();
                 ddoq and desc_ring were allocated. */
              cvm_ddoq_put_buffer(ddoq->desc_ring);
              cvm_ddoqs.create_fail_list++;
              retval = 1; goto DDOQ_INIT_FAILED;
          }
          ddoq->host_status_addr = ddoq_init->dfront.host_status_addr;
          //cvm_ddoq_print(ddoq);
      } else {
          /* desc_ring was not allocated; ddoq alloc failed;
             at return the wqe and packet ptr will be freed.
             Nothing to free here.*/
          printf("[ DRV ] DDOQ[%d] init allocation failed \n", ddoq_init->dfront.start.s.ddoq_id);
          cvm_ddoqs.create_fail_nomem++;
          retval = 1; goto DDOQ_INIT_FAILED;
      }

      cvm_ddoq_set_ddoq_host_state(wqe, ddoq->host_status_addr,
                                   DDOQ_STATE_READY, 1);

      CVMX_SYNCWS;

      if(!retval) {
          cvm_drv_ddoq_callback_t  appcb;
          appcb = cvm_ddoq_get_appcb(ddoq->type, DDOQ_ACTION_INIT);
          if(appcb) {
               /* The value advertised to application should be the actual available
                  descriptors. */
              ddoq_front.start.s.desc_count = ddoq->avl_desc;
              if(appcb(ddoq->tag, ddoq->ddoq_id, &(ddoq_front.start))) {
                  printf("[ DRV ] DDOQ INIT callback failed. Front data ->\n");
                  cvm_drv_print_data(&ddoq_front.start, 40);
              }
          }
      }


DDOQ_INIT_FAILED:
      if(retval) {
           /* Set the ddoq state in host to failed and free the WQE */
           cvm_ddoq_set_ddoq_host_state(wqe, ddoq_init->dfront.host_status_addr,
                                   DDOQ_STATE_CORE_FAILED, 1);
           if(ddoq) {
                if(ddoq->desc_ring)
                     cvm_ddoq_put_buffer(ddoq->desc_ring);
                CVM_DDOQ_FREE_BUFFER(ddoq);
           }
      }
#ifdef CVM_DDOQ_PROFILE
      c_time = cvmx_get_cycle() - c_start;
      if(c_time < dprof[cvm_drv_core_id].c_min)
         dprof[cvm_drv_core_id].c_min = c_time;
      if(c_time > dprof[cvm_drv_core_id].c_max)
         dprof[cvm_drv_core_id].c_max = c_time;
      dprof[cvm_drv_core_id].c_totaltime += c_time;
      dprof[cvm_drv_core_id].c_count++;
#endif
      return retval;
}









/* This routine handles DDOQ DELETE instructions that arrive from host. If
 * the DDOQ exists, it removes the structure from the lookup table, frees 
 * all memory allocated for the DDOQ and responds to the instruction. On 
 * error, no response is sent to the host.
 */
static int
cvm_ddoq_process_delete(cvmx_wqe_t         *wqe)
{
      cvm_ddoq_delete_t        *ddoq_delete;
      cvm_ddoq_t               *ddoq;
      cvm_drv_ddoq_callback_t   appcb;
      ddoq_front_data_t         ddoq_front;
      int                       retval=0;
      uint32_t                  ddoq_id;
#ifdef CVM_DDOQ_PROFILE
      unsigned long             d_start = cvmx_get_cycle(), d_time;
#endif

      DBG_PRINT(DBG_FLOW,"\n----cvm_ddoq_process_delete_instr----\n");

      ddoq_delete = (cvm_ddoq_delete_t *)&(wqe->packet_data);
      ddoq_id     = ddoq_delete->dfront.start.s.ddoq_id;
      ddoq        = cvm_get_ddoq_ptr(ddoq_id);

#ifdef SANITY_CHECKS
      if(wqe->word1.tag_type != CVMX_POW_TAG_TYPE_ATOMIC) {
         printf("[ DRV ] CRITICAL!!!  DDOQ[%d] DELETE has tag_type set to %d\n", ddoq->ddoq_id, wqe->word1.tag_type);
         retval = 1; goto DDOQ_DELETE_FAILED;
      }
#endif

      DBG_PRINT(DBG_FLOW,"DDOQ delete received for ddoq %d\n", ddoq_id);
      if(ddoq)  {
         if(ddoq->tag != wqe->word1.tag) {
             printf("[ DRV ] CRITICAL!!! DDOQ[%d] DELETE expected tag 0x%x received 0x%x\n", ddoq_id, ddoq->tag, (uint32_t)wqe->word1.tag);
             retval = 1; goto DDOQ_DELETE_FAILED;
         }

         /* Delete DDOQ from the global list. */
         cvm_delete_from_ddoq_list(ddoq);

         /* Once the delete_active flag is set, no packets OR credits
            are accepted for this DDOQ. Time to cleanup the ddoq. */
         cvmx_spinlock_lock(&ddoq->lock);

         ddoq->delete_active = 1;

         cvm_ddoq_set_ddoq_host_state(wqe, ddoq->host_status_addr,
                                      DDOQ_STATE_NULL, 0);
         CVMX_SYNCWS;

         cvmx_spinlock_unlock(&ddoq->lock);

         DBG_PRINT(DBG_FLOW,"DDOQ %d deleted from list\n", ddoq_id);
         cvm_ddoq_put_buffer(ddoq->desc_ring);
         DBG_PRINT(DBG_FLOW,"DDOQ %d memory freed\n", ddoq_id);
         CVMX_SYNCWS;

         appcb = cvm_ddoq_get_appcb(ddoq->type, DDOQ_ACTION_DELETE);
         if(appcb) {
            memcpy(&ddoq_front, &ddoq_delete->dfront, sizeof(ddoq_front_data_t));
            if(appcb(ddoq->tag, ddoq->ddoq_id, &(ddoq_front.start))) {
               printf("[ DRV ] DDOQ DELETE callback failed. Front data ->\n");
               cvm_drv_print_data(&ddoq_front.start, 40);
            }
         }
         CVM_DDOQ_FREE_BUFFER(ddoq);

      } else {
          printf("[ DRV ] Delete request for unknown DDOQ[%d]\n",ddoq_id);
          cvm_ddoqs.delete_unknown++;
          retval = 1;
      }

DDOQ_DELETE_FAILED:
      cvm_free_host_instr(wqe);
#ifdef CVM_DDOQ_PROFILE
      d_time = cvmx_get_cycle() - d_start;
      if(d_time < dprof[cvm_drv_core_id].d_min)
         dprof[cvm_drv_core_id].d_min = d_time;
      if(d_time > dprof[cvm_drv_core_id].d_max)
         dprof[cvm_drv_core_id].d_max = d_time;
      dprof[cvm_drv_core_id].d_totaltime += d_time;
      dprof[cvm_drv_core_id].d_count++;
#endif
      return retval;
}









/* Send a packet to a DDOQ identified by ddoq_id. Supports direct, linked
   or gather mode data buffers.
*/
int
cvm_ddoq_send_data(uint32_t         ddoq_id,
                   cvmx_buf_ptr_t   dptr,
                   cvm_ptr_type_t   ptr_type,
                   uint32_t         segs,
                   uint32_t         total_bytes,
                   cvmx_resp_hdr_t *user_resp_hdr)
{
	uint32_t          bufs_reqrd=0, bufsize=0;
	cvm_ddoq_t       *ddoq;
	cvm_ddoq_pkt_t   *ddoq_pkt;
	int               i, retval=-1;

	DBG_PRINT(DBG_FLOW,"\n----cvm_ddoq_send_data----\n");

	if(total_bytes > 0xffff) {
		drv_err("[ DRV ] DDOQ[%d]: Total bytes (%d) exceeds max\n",
				ddoq_id, total_bytes);
		return -EINVAL;
	}
	
	if(segs > MAX_DDOQ_DATA_PTRS) {
		drv_err("[ DRV ] ddoq[%d]_send - too many bufs (%d), max allowed %d\n",
				ddoq_id, segs, MAX_DDOQ_DATA_PTRS);
		return -EINVAL;
	}

	ddoq = cvm_get_ddoq_ptr(ddoq_id);
	if(!ddoq)  {
//		drv_err("[ DRV ] Packet for unknown DDOQ[%d]\n", ddoq_id);
		cvm_ddoqs.pkts_dropped_noddoq++;
		return -EINVAL;
	}


	cvmx_spinlock_lock(&(ddoq->lock));

	/* If the DDOQ is being deleted, do not sent the packet. */
	if(ddoq->delete_active) {
		drv_err("[ DRV ] DDOQ[%d]: Packet received in delete state\n", ddoq_id);
		cvmx_spinlock_unlock(&(ddoq->lock));
		return -ENODEV;
	}

	while(bufsize < total_bytes) {
		bufs_reqrd++;
		bufsize += ddoq->buf_size;
	}

	/* If the DDOQ buffers exceed the max. supported by this DDOQ, do not
	   send the packet. */
	if(bufs_reqrd > (ddoq->desc_count - ddoq->refill_threshold)) {
		drv_err("[ DRV ] ddoq[%d]_send bufs_reqrd (%d) > max_desc_count (%d)\n",
		       ddoq_id, bufs_reqrd, (ddoq->desc_count- ddoq->refill_threshold));
		cvmx_spinlock_unlock(&(ddoq->lock));
		return -ENOMEM;
	}

	DBG_PRINT(DBG_FLOW,"bufs_reqrd: %d avl-desc: %d segs: %d total_bytes: %d\n",
		   bufs_reqrd, ddoq->avl_desc, segs, total_bytes);

	/* If there are enough buffers to send this packet and there are no
	   wait listed packets, go forth and send this packet. */
	if((ddoq->avl_desc >= bufs_reqrd) && !(ddoq->wait_count)) {
		if(!cvm_ddoq_send_dma_data( ddoq, dptr, ptr_type, segs, total_bytes, bufs_reqrd, user_resp_hdr))
		{
			ddoq->avl_desc  -= bufs_reqrd;
			ddoq->bufs_sent += bufs_reqrd;
			ddoq->pkts_sent++;
			cvmx_spinlock_unlock(&(ddoq->lock));
			return bufs_reqrd;
		}
	}


	/* If there are not enough descriptors, or if the attempt to send the
	   ddoq packet failed then we should try to add it to the wait list. */
	if(ddoq->wait_count >= ddoq->desc_count) {
		drv_err("[ DRV ] DDOQ[%d] wait list at max (%d), pkts: %lu\n", ddoq_id, ddoq->wait_count, ddoq->pkts_sent);
		cvm_ddoqs.pkts_dropped_maxbufs++;
		retval = -ENOMEM;
		goto cvm_ddoq_send_data_end;
	}

	ddoq_pkt = cvm_ddoq_alloc_pkt(); 
	if(ddoq_pkt) {
		ddoq_pkt->ddoq_id  = ddoq_id;
		ddoq_pkt->pkt_size = total_bytes;
		ddoq_pkt->ptr_type = (uint16_t)ptr_type;
		ddoq_pkt->resp_hdr.u64 = user_resp_hdr->u64;
		ddoq_pkt->bufs_reqrd = bufs_reqrd;

		switch(ptr_type)  {
		case CVM_DIRECT_DATA:
			ddoq_pkt->buf[0].u64 =  dptr.u64;
			ddoq_pkt->buf_count = 1;
			break;
		case CVM_GATHER_DATA:
			{
			cvmx_buf_ptr_t *ptr=(cvmx_buf_ptr_t *)CVM_DRV_GET_PTR(dptr.s.addr);
			for(i = 0; i < segs; i++, ptr++)
				ddoq_pkt->buf[i].u64 = ptr->u64;
			}
			ddoq_pkt->buf_count = segs;
			break;
		case CVM_LINKED_DATA:
			ddoq_pkt->buf[0].u64 =  dptr.u64;
			ddoq_pkt->buf_count = segs;
			break;
		default:
			drv_err("[ DRV ] ddoq_send: Invalid ptr type %d\n", ptr_type);
			retval = -EINVAL;
			cvm_release_ddoq_packet(ddoq_pkt);
			goto cvm_ddoq_send_data_end;
		} 
		cavium_list_add_tail(&(ddoq_pkt->list), &(ddoq->wait_list));
		ddoq->wait_count++;
		retval = bufs_reqrd;
		DBG_PRINT(DBG_FLOW,"[ DRV ] Add to DDOQ[%d] waitlist, wait_cnt: %d pkt_sent: %lu\n", ddoq_id, ddoq->wait_count, ddoq->pkts_sent);
	}  else {
		drv_err("[ DRV ] Allocation failed to add to DDOQ[%d] wait list\n", ddoq_id);
		cvm_ddoqs.pkts_dropped_nomem++;
		retval =  -ENOMEM;
	}

	if(ddoq->wait_count)  
		cvm_ddoq_process_wait_list(ddoq);

cvm_ddoq_send_data_end:
	cvmx_spinlock_unlock(&(ddoq->lock));
	return retval;
}






/** Applications can registers callbacks for DDOQ's of the type given by
  * "ddoq_type" for an action given by "ddoq_action". The callback is the
  * third argument passed to this routine. Whenever the driver encounters
  * a DDOQ of the given type and is performing an action for which the
  * callback was registered, it will call the callback registered by the
  * application on the successful completion of that action.
  * @param ddoq_type - type of ddoq (currently only DDOQ_TYPE_TCP is supported)
  * @param ddoq_action - action during which the callback should be called.
  *                      e.g. INIT, DELETE or CREDIT.
  * @param  appcallback - the callback function to be invoked.
  * @return: The application is expected to return 0 on success and 1 on error.
  */
int
cvm_ddoq_setup_appcallback(uint32_t  ddoq_type, uint32_t ddoq_action, int (*appcallback)(uint32_t, uint32_t, void *))
{
	if(!ddoq_type || ddoq_type >= MAX_DDOQ_TYPES) {
		printf("[ DRV ] DDOQ callback received invalid type: 0x%x\n", ddoq_type);
		return 1;
	}

	if(!ddoq_action || ddoq_action >= MAX_DDOQ_ACTIONS) {
		printf("[ DRV ] DDOQ callback received invalid action: 0x%x\n", ddoq_action);
		return 1;
	}

	cvm_drv_ddoq_appcallback[ddoq_type][ddoq_action] = appcallback;
	printf("[ DRV ] DDOQ Application Callback registered for Type: 0x%x Action: 0x%x\n", ddoq_type, ddoq_action);

	return 0;
}






int
cvm_ddoq_change_tag(uint32_t    ddoq_id,   uint32_t  new_tag)
{
	cvm_ddoq_t    *ddoq = cvm_get_ddoq_ptr(ddoq_id);
	if(ddoq == NULL)
		return -EINVAL;
	cvmx_spinlock_lock(&(ddoq->lock));
	ddoq->tag = new_tag;
	cvmx_spinlock_unlock(&(ddoq->lock));
	return 0;
}


int
cvm_ddoq_get_wait_count(uint32_t         ddoq_id)
{
    cvm_ddoq_t    *ddoq = cvm_get_ddoq_ptr(ddoq_id);
    if(ddoq)
       return ddoq->wait_count;
    return -EINVAL;
}



/* Returns 1 if a DDOQ exists, else 0 */
int
cvm_ddoq_exists(uint32_t         ddoq_id)
{
    cvm_ddoq_t    *ddoq = cvm_get_ddoq_ptr(ddoq_id);
    if(ddoq)
       return 1;
    else
       return 0;
}


/* Returns no. of pkts sent if a DDOQ exists, else 0 */
int
cvm_ddoq_get_pkts_sent(uint32_t         ddoq_id)
{
    cvm_ddoq_t    *ddoq = cvm_get_ddoq_ptr(ddoq_id);
    if(ddoq)
       return ddoq->pkts_sent;
    else
       return -EINVAL;
}




/* $Id: cvm-ddoq.c 67088 2011-11-15 19:39:17Z mchalla $ */

