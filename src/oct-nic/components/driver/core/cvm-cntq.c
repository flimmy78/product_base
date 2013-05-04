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
#include "cvm-cntq.h"
#include "octeon-common.h"
#include "cvm-drv.h"
#include "cvm-drv-reqresp.h"
#include "cvm-drv-debug.h"
#include "cvm-ddoq.h"

#ifdef CAVIUM_DEBUG
#define CVM_CNTQ_DEBUG
#endif

CVMX_SHARED   cvm_cntq_t  cvm_cntq[2];
CVMX_SHARED   uint64_t    cvm_cntq_pkt_cnt[2] = {0,0};
CVMX_SHARED   cvm_cntq_global_stats_t  cvm_cntqs;


static int cvm_cntq_process_credit(cvmx_wqe_t  *wqe);
static int cvm_cntq_process_init(cvmx_wqe_t    *wqe);
static int cvm_cntq_process_delete(cvmx_wqe_t    *wqe);



int
cvm_cntq_get_init_status(uint32_t  cntq_no)
{
  if(cntq_no > 1)
     return -1;
  return cvm_cntq[cntq_no].initialized;
}
   


void
cvm_cntq_print(CVMX_DMA_QUEUE_TYPE   q_no)
{
    cavium_list_t  *node;
#ifdef OCTEON_DEBUG_LEVEL
    int i = 0;
#endif
     if(q_no > 1)
         return;

     DBG_PRINT(DBG_FLOW,"Printing contents of  CNTQ %u\n", q_no);
     DBG_PRINT(DBG_FLOW,"avl_size    : %d\n",(uint32_t)cvm_cntq[q_no].avl_size);
     DBG_PRINT(DBG_FLOW,"total_size  : %d\n",(uint32_t)cvm_cntq[q_no].total_size);
     DBG_PRINT(DBG_FLOW,"base_addr   : %llx\n",cast64(cvm_cntq[q_no].base_addr));
     DBG_PRINT(DBG_FLOW,"host_rd_ptr : %llx\n",cast64(cvm_cntq[q_no].host_rd_ptr));
     DBG_PRINT(DBG_FLOW,"core_wr_ptr : %llx\n",cast64(cvm_cntq[q_no].core_wr_ptr));
     DBG_PRINT(DBG_FLOW,"end_addr    : %llx\n",cast64(cvm_cntq[q_no].end_addr));
     DBG_PRINT(DBG_FLOW,"wait_count  : %d\n",(uint32_t)cvm_cntq[q_no].wait_count);
     if(cvm_cntq[q_no].wait_count)  {
        DBG_PRINT(DBG_FLOW,"Wait list contents\n");
        cavium_list_for_each(node, &(cvm_cntq[q_no].wait_list)) {
          DBG_PRINT(DBG_FLOW,"CNTQ[%d]:  pri:%d pkt:0x%p, opcode:0x%x len: 0x%lx\n",
                ++i,(uint32_t)((cvm_cntq_pkt_t *)node)->pri,
                node, (uint32_t)((cvm_cntq_pkt_t *)node)->resp_hdr.s.opcode,
                ((cvm_cntq_pkt_t *)node)->len);
        }
     }
     DBG_PRINT(DBG_FLOW, "\n");
}





void
cvm_cntq_setup(CVMX_DMA_QUEUE_TYPE   q_no)
{

     DBG_PRINT(DBG_FLOW,"Setting up CNTQ %u\n", q_no);
     memset(&cvm_cntq[q_no], sizeof(cvm_cntq_t), 0);
     memset(&cvm_cntqs, 0, sizeof(cvm_cntq_global_stats_t));
     CAVIUM_INIT_LIST_HEAD(&(cvm_cntq[q_no].wait_list));
     cvmx_spinlock_init(&cvm_cntq[q_no].lock);
}




#ifdef CVM_DDOQ_USE_FPA_POOL
int
cvm_drv_alloc_ddoq_pool(char *str, int pool, int buf_size, int pool_count)
{
    void *memory;
    void* base = NULL;

    memory = cvmx_bootmem_alloc((buf_size * pool_count), CVMX_CACHE_LINE_SIZE);
    if (memory == NULL) {
        printf("Out of memory initializing %s.\n", str);
        return 1;
    }

    base = CVM_COMMON_INIT_FPA_CHECKS(memory, pool_count, buf_size);
    cvmx_fpa_setup_pool(pool, str, memory, buf_size, pool_count);
    cvm_common_fpa_add_pool_info(pool, memory, buf_size, pool_count, CAST64(base));
	return 0;
}
#endif





int
cvm_drv_setup_dma_queues()
{
	printf("[ DRV ] Setting up DMA Counter queues (CNTQ)\n");

#ifdef CVM_DDOQ_USE_FPA_POOL
	if(cvm_drv_alloc_ddoq_pool("DDOQ Buffers", CVM_FPA_DDOQ_POOL,
                            CVM_FPA_DDOQ_POOL_SIZE, FPA_DDOQ_POOL_COUNT))
		return 1;
#endif
	cvm_cntq_setup(0);
	cvm_cntq_setup(1);
	if(cvm_drv_ddoq_setup()) {
		printf("[ DRV ]  DDOQ setup failed!!!!\n");
		return 1;
	}
	cvm_drv_register_op_handler(CNTQ_INIT_OP, cvm_cntq_process_init);
	cvm_drv_register_op_handler(CNTQ_CREDIT_OP, cvm_cntq_process_credit);
	cvm_drv_register_op_handler(CNTQ_DELETE_OP, cvm_cntq_process_delete);
	cvm_drv_setup_app_mode(CVM_DRV_CNTQ_APP);
	return 0;
}







static inline int
cvm_cntq_dma_write(cvm_cntq_t       *cntq,
                   cvm_cntq_pkt_t   *cntq_pkt)
{
   int                            retval=0;
   uint32_t                       cntq_space;
   uint64_t                       old_wr_ptr;
   cvmx_oct_pci_dma_local_ptr_t   local_ptr;
   cvm_dma_remote_ptr_t           remote_ptr[2];
   cvmx_oct_pci_dma_inst_hdr_t    dma_hdr;


   DBG_PRINT(DBG_FLOW,"\n-----cvm_cntq_dma_write------\n");

   /* Initialize the dma header. */
   /* The FL and II bits set to 0 - buffers with I bit set should be freed.
      ptr is NULL; DIR is outbound; CA is 1 (Add to the dma count); */
   dma_hdr.u64  = 0;
   dma_hdr.s.c  = cntq_pkt->pri;
   dma_hdr.s.ca = 1;
   dma_hdr.s.nl = 1;
   dma_hdr.s.lport = OCTEON_PCIE_PORT;

   local_ptr.u64    = 0;
   local_ptr.s.addr = CVM_DRV_GET_PHYS(&(cntq_pkt->len));
   local_ptr.s.size = cntq_pkt->pkt_len;
   local_ptr.s.pool = cntq_pkt->pool;
   if(cntq_pkt->pkt_type == CVM_CNTQ_ALLOC_PKT) 
       local_ptr.s.i = 1;


   remote_ptr[0].s.addr = cntq->core_wr_ptr;

   old_wr_ptr = cntq->core_wr_ptr;
   cntq_space = cntq->end_addr - cntq->core_wr_ptr;

   if(cntq_space >= cntq_pkt->pkt_len)   {
      remote_ptr[0].s.size = cntq_pkt->pkt_len;
      dma_hdr.s.nr = 1;
      cntq->core_wr_ptr += cntq_pkt->pkt_len;
      if(cntq->core_wr_ptr == cntq->end_addr)
          cntq->core_wr_ptr = cntq->base_addr;
   }
   else {
      remote_ptr[0].s.size = cntq_space;
      remote_ptr[1].s.addr = cntq->base_addr;
      remote_ptr[1].s.size = (cntq_pkt->pkt_len - cntq_space);
      cntq->core_wr_ptr = cntq->base_addr + (cntq_pkt->pkt_len - cntq_space);
      dma_hdr.s.nr = 2;
   }

   retval = cvm_pci_dma_raw(&dma_hdr, &local_ptr, remote_ptr);
   if(!retval) {
      cntq->avl_size -=  (cntq_pkt->pkt_len >> 3);
      cvm_cntq_pkt_cnt[cntq_pkt->pri]++;
      //printf("CNTQ pkt send success, total: %lu\n", cvm_cntq_pkt_cnt[cntq_pkt->pri]);
   } else {
      /* Restore the write pointer where core can write the next packet. */
      cntq->core_wr_ptr = old_wr_ptr;
      printf("[ DRV ] CNTQ: pci_dma_send_direct failed, retval: %d\n", retval);
   }
   CVMX_SYNCWS;
   return retval;
}







/*
 *  Desc:  Update the avl_size field in CNTQ when a CNTQ_CREDIT is received.
 *         Also update the perceived host read pointer.
 *
 *
 *
 *   Locks: This routine is called WITHOUT any locks held.
 *          This routine HOLDS the cntq lock while it updates its fields.
 */
static int
cvm_cntq_process_credit(cvmx_wqe_t  *wqe)
{
   uint32_t                      q_no, size;
   int                           retval=0;
   cvm_cntq_t                   *cntq;
   cvm_cntq_credit_t            *cntq_credit;

   DBG_PRINT(DBG_FLOW, "---cvm_cntq_process_credit---\n");

   cntq_credit = (cvm_cntq_credit_t *)wqe->packet_data;
   q_no = cntq_credit->cmd.s.q_no;
   if(q_no > 1) {
       printf("[ DRV ] Received CNTQ credit for incorrect queue: %d\n", q_no);
       cvm_drv_print_wqe(wqe);
       cvm_cntqs.credits_invalid++;
       cvm_free_host_instr(wqe);
       return 1;
    }

   cntq = (cvm_cntq_t *) &cvm_cntq[q_no];
   size = cntq_credit->cmd.s.q_size;

   DBG_PRINT(DBG_FLOW,"CNTQ[%d]: Received credit %d words, avl_size: %d\n",
             q_no, size, cntq->avl_size);

   cvmx_spinlock_lock(&(cntq->lock));

   /* credit statistics */
   cntq->credits_recvd += size;

    /* This is a paranoid check in place to ensure host pkts dont screw up
       the core. */
    if(size <= (cntq->total_size - cntq->avl_size)) {
        cntq->avl_size     += size;
        cntq->host_rd_ptr  += (size << 3);
        if(cntq->host_rd_ptr >= cntq->end_addr)
            cntq->host_rd_ptr = cntq->base_addr +
                                (cntq->host_rd_ptr - cntq->end_addr);
    } else {
        printf("[ DRV ] CNTQ:ERROR!Host credited %d blocks; we need %d\n",
               size, (cntq->total_size - cntq->avl_size));
        size = (cntq->total_size - cntq->avl_size);
        cvm_cntqs.credits_invalid++;
    }
    CVMX_SYNCWS;

    while(cntq->wait_count && !retval) {
      cvm_cntq_pkt_t   *cntq_pkt;

      cntq_pkt = (cvm_cntq_pkt_t *)cntq->wait_list.le_next;

      if( cntq_pkt->pkt_len <= (cntq->avl_size << 3)) {
           if(! (retval = cvm_cntq_dma_write(cntq, cntq_pkt)) ) {
               cavium_list_del(&(cntq_pkt->list));
               cntq->wait_count--;
           } else {
               drv_err("[ DRV ] CNTQ: Failed to send wait list packet\n");
               drv_err("[ DRV ] CNTQ: Pkt length: %d avl_size: %d\n", 
                      cntq_pkt->pkt_len, cntq->avl_size);
           }
      } 
      else 
          break;
    } /* while wait_count */
    cvmx_spinlock_unlock(&(cntq->lock));
    cvm_free_host_instr(wqe);
    return retval;
}







static void
cvm_cntq_flush_wait_list(cvm_cntq_t  *cntq) 
{
	cvm_cntq_pkt_t   *cntq_pkt;

	while(cntq->wait_count) {
		cntq_pkt = (cvm_cntq_pkt_t *)cntq->wait_list.le_next;
		if(cntq_pkt == NULL) {
			printf("[ DRV ]: Found NULL in cntq[%llu] wait list with wait count %llu\n", cast64(cntq->pri), cast64(cntq->wait_count));
			return;
		}
		cavium_list_del(&(cntq_pkt->list));
		cvm_cntq_free_packet(cntq_pkt);
		cntq->wait_count--;
	}
	CAVIUM_INIT_LIST_HEAD(&(cntq->wait_list));
}




static int
cvm_cntq_process_init(cvmx_wqe_t    *wqe)
{
	uint32_t                q_no;
	cvm_cntq_init_t        *cntq_init;
	cvmx_resp_hdr_t         resp_hdr;
	cvmx_buf_ptr_t          lptr;

	cntq_init = (cvm_cntq_init_t *)wqe->packet_data;
	q_no  = cntq_init->cmd.word0.s.q_no;
	if(q_no > 1) {
		printf("[ DRV ] CNTQ: Incorrect Queue No: %d\n", q_no);
		cvm_free_host_instr(wqe);
	return 1; 
	}

	if(cvm_cntq[q_no].initialized) {
		printf("[ DRV ] CNTQ: INIT for cntq(%d)already initialized\n", q_no);
		cvm_free_host_instr(wqe);
		return 1;
	}

	cvm_cntq[q_no].pri         = q_no;
	cvm_cntq[q_no].base_addr   = cntq_init->cmd.base_addr;
	cvm_cntq[q_no].host_rd_ptr = cntq_init->cmd.base_addr;
	cvm_cntq[q_no].core_wr_ptr = cntq_init->cmd.base_addr;
	cvm_cntq[q_no].end_addr    = cntq_init->cmd.base_addr + (cntq_init->cmd.word0.s.q_size << 3); 
	cvm_cntq[q_no].total_size  = cntq_init->cmd.word0.s.q_size;
	cvm_cntq[q_no].avl_size    = cntq_init->cmd.word0.s.q_size;

	lptr.u64      = 0;
	resp_hdr.u64          = 0;
	resp_hdr.s.opcode     = CNTQ_INIT_DONE_OP;
	resp_hdr.s.request_id = cntq_init->front.irh.s.rid;

	cvm_cntq[q_no].initialized = 1;
	CVMX_SYNCW;
	if(cvm_drv_pci_instr_response(DMA_BLOCKING, (cvmx_raw_inst_front_t  *)wqe->packet_data, &resp_hdr, 0, lptr, 0, CVM_NULL_DATA, NULL)) {
		printf("[ DRV ] CNTQ: INIT instruction failed\n");
		cvm_free_host_instr(wqe);
		return 1;
	}

	cvm_free_host_instr(wqe);
	return 0;
}







static int
cvm_cntq_process_delete(cvmx_wqe_t    *wqe)
{
	uint32_t                q_no;
	cvm_cntq_delete_t      *cntq_delete;
	cvmx_resp_hdr_t         resp_hdr;
	cvmx_buf_ptr_t          lptr;

	DBG_PRINT(DBG_FLOW,"\n-----cvm_cntq_process_delete------\n");
	cntq_delete = (cvm_cntq_delete_t *)wqe->packet_data;
	q_no  = cntq_delete->cmd.s.q_no;
	if(q_no > 1) {
		printf("[ DRV ] CNTQ: Incorrect Queue No: %d (%s)\n", q_no, __FUNCTION__);
		cvm_free_host_instr(wqe);
		return 1; 
	}
	cvm_cntq_flush_wait_list(&cvm_cntq[q_no]);
	memset(&cvm_cntq[q_no], 0, sizeof(cvm_cntq_t));

	lptr.u64 = 0;
	resp_hdr.u64          = 0;
	resp_hdr.s.opcode     = CNTQ_DELETE_OP;
	resp_hdr.s.request_id = cntq_delete->front.irh.s.rid;
	CVMX_SYNCW;

	if(cvm_drv_pci_instr_response(DMA_BLOCKING, (cvmx_raw_inst_front_t  *)wqe->packet_data, &resp_hdr, 0, lptr, 0, CVM_NULL_DATA, NULL)) {
		printf("[ DRV ] CNTQ: Delete instruction response failed\n");
		cvm_free_host_instr(wqe);
		return 1;
	}

	cvm_free_host_instr(wqe);
	return 0;
}






static inline cvm_cntq_pkt_t  *
cvm_cntq_clone_pkt(cvm_cntq_pkt_t   *cntq_pkt)
{
    cvm_cntq_pkt_t *cntq_pkt2= NULL;

    if(cntq_pkt->pkt_type != CVM_CNTQ_ALLOC_PKT) {
        cntq_pkt2  = cvm_alloc_cntq_packet(cntq_pkt->pri, cntq_pkt->len);
        if(cntq_pkt2) {
            uint32_t  i;
            uint8_t  *buf1 = (uint8_t *)cntq_pkt->data;
            uint8_t  *buf2 = (uint8_t *)cntq_pkt2->data;
 
            for(i = 0; i < cntq_pkt2->len; i++)
                   *(buf2++) = *(buf1++);
        }
    }
    else 
	cntq_pkt2 = cntq_pkt;

    return cntq_pkt2;
}



/*
 *  This routine takes a CNTQ packet to be sent to the host
 *  checks for available space in the requested queue and 
 *    - posts a DMA instruction to the DMA h/w if space is available.
 *    - OR keeps the packet in the CNTQ's wait list if no space
 *      is available.
 */
int 
cvm_cntq_send_packet(cvm_cntq_pkt_t   *cntq_pkt)
{
   cvm_cntq_t     *cntq = &(cvm_cntq[cntq_pkt->pri]);

   cvm_cntq_pkt_t *cntq_pkt2= NULL;
   int             retval=0;

   DBG_PRINT(DBG_FLOW,"----cvm_cntq_send_packet----\n");
   if(!cntq->initialized)  {
       printf("[ DRV ] CNTQ: Error!!! CNTQ is not initialized\n");
       cvm_cntqs.pkts_dropped_nocntq++;
       return ENODEV;
   }

   cvmx_spinlock_lock(&cntq->lock);
   if( (cntq_pkt->pkt_len <= (cntq->avl_size << 3))  && !(cntq->wait_count))  {
       if(! (retval = cvm_cntq_dma_write(cntq,  cntq_pkt)) ) {
           cvmx_spinlock_unlock(&cntq->lock);
           return 0;
       } else {
           cvm_cntqs.pkts_dropped_dmaerr++;
           printf("[ DRV ] send_cntq_pkt: DMA write returned: %d\n", retval);
       }
   } else  {
       if((cntq_pkt2 = cvm_cntq_clone_pkt(cntq_pkt)) != NULL) {
          cavium_list_add_tail(&(cntq_pkt2->list), &(cntq->wait_list));
          cntq->wait_count++;
          cvm_cntqs.pkts_waitlisted++;
          drv_err("[ DRV ] CNTQ: Pkt @ %p added to wait list\n",cntq_pkt2);
          drv_err("[ DRV ] CNTQ: sent: %lu wait_cnt:%d avl_size: %d bytes\n",
                 cvm_cntq_pkt_cnt[cntq_pkt->pri], (uint32_t)cntq->wait_count,
                 (uint32_t)cntq->avl_size);
       }
       else  {
          cvm_cntqs.pkts_dropped_nomem++;
          retval = ENOMEM;
       }
   }

   cvmx_spinlock_unlock(&cntq->lock);
       
   return retval;
}


/* $Id: cvm-cntq.c 63047 2011-08-01 22:40:43Z panicker $ */
