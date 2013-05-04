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


#include "octeon_main.h"
#include "octeon-common.h"
#include "octeon-drv-opcodes.h"
#include "octeon_debug.h"
#include "octeon_macros.h"
#include "octeon_cntq.h"
#include "octeon_ddoq_list.h"
#include "octeon_ddoq.h"




struct ddoq_credit_gptr
{
	octeon_soft_instruction_t   *soft_instr;
	octeon_device_t             *octeon_dev;
};



void ddoq_create_callback(octeon_req_status_t status, void *arg);
void ddoq_delete_callback(octeon_req_status_t status, void *arg);
static int __octeon_send_ddoq_delete(octeon_device_t *oct, octeon_ddoq_t *ddoq);


extern void
oct_ddoq_add_to_deferred_list(octeon_device_t  *oct, octeon_ddoq_t   *ddoq);





void
dummy_ddoq_dispatch(octeon_recv_info_t *recv_info, void *arg)
{
  int      i;
  octeon_recv_pkt_t  *recv_pkt = recv_info->recv_pkt;

  for(i = 0 ; i < recv_pkt->buffer_count; i++) {
       cavium_free_recv_pkt_buf(recv_pkt->buffer_ptr[i], recv_pkt->buf_type);
  }
  cavium_free_dma(recv_info);
}





/** Routine to create a DDOQ INIT soft instruction. */
static octeon_soft_instruction_t  *
ddoq_create_init_soft_instr(octeon_device_t   *oct, octeon_ddoq_t     *ddoq)
{
	int  dsize;
	octeon_soft_instruction_t  *si = cavium_alloc_buffer(oct, OCT_SOFT_INSTR_SIZE);

	if(si == NULL) return NULL;

	cavium_memset(si, 0, OCT_SOFT_INSTR_SIZE);

	dsize = sizeof(ddoq_front_data_t) +
	        ((ddoq->ddoq_setup.no_of_desc - ddoq->desc_pending_refill) * OCT_DDOQ_DESC_SIZE);

	if(dsize > OCT_MAX_DIRECT_INPUT_DATA_SIZE) {
		cavium_error("OCTCNTQ: DDOQ INIT data size (%d) exceeds max(%d)\n",
		             dsize, OCT_MAX_DIRECT_INPUT_DATA_SIZE);
		cavium_free_buffer(oct, si);
		return NULL;
	}

	SET_SOFT_INSTR_DMA_MODE(si, OCTEON_DMA_DIRECT);
	SET_SOFT_INSTR_RESP_ORDER(si, OCTEON_RESP_NORESPONSE);
	SET_SOFT_INSTR_TIMEOUT(si, 6000);
	SET_SOFT_INSTR_ALLOCFLAGS(si, (OCTEON_SOFT_INSTR_ALLOCATED));

	si->req_info.octeon_id = oct->octeon_id;

	si->ih.dlengsz    = dsize;
	si->ih.tag        = ddoq->ddoq_setup.tag;
	si->ih.raw        = 1;
	si->ih.fsz        = 16;
	si->ih.tagtype    = OCTEON_TAG_TYPE_ATOMIC;
	si->irh.opcode    = DDOQ_INIT_OP;
	si->dptr          = &ddoq->front;

	ddoq->front.start.u64     = ddoq->dptr_start.u64;
	ddoq->front.next_pkt_cntr = ddoq->next_pkt_cntr;
	ddoq->front.refill_threshold = ddoq->ddoq_setup.refill_threshold;
	ddoq->front.reserved      = 0;
	ddoq->front.avl_desc      = ddoq->avl_desc;
	ddoq->front.host_status_addr = (uint64_t)octeon_pci_map_single(oct->pci_dev, (void *)&ddoq->state, sizeof(uint32_t), CAVIUM_PCI_DMA_FROMDEVICE);

	octeon_swap_8B_data((uint64_t *)&ddoq->front, 1);
	octeon_swap_8B_data((uint64_t *)&ddoq->front.host_status_addr, 1);
	octeon_swap_8B_data((uint64_t *)&ddoq->front.start, 1);

	return si;
}






/** Routine to create a DDOQ DELETE soft instruction. */
static octeon_soft_instruction_t  *
ddoq_create_delete_soft_instr(octeon_device_t   *oct, octeon_ddoq_t     *ddoq)
{
	octeon_soft_instruction_t  *si = cavium_alloc_buffer(oct, OCT_SOFT_INSTR_SIZE);

	if(si == NULL) return NULL;

	cavium_memset(si, 0, OCT_SOFT_INSTR_SIZE);

	SET_SOFT_INSTR_DMA_MODE(si, OCTEON_DMA_DIRECT);
	SET_SOFT_INSTR_RESP_ORDER(si, OCTEON_RESP_NORESPONSE);
	SET_SOFT_INSTR_TIMEOUT(si, 6000);
	SET_SOFT_INSTR_ALLOCFLAGS(si, (OCTEON_SOFT_INSTR_ALLOCATED));
	SET_SOFT_INSTR_CALLBACK(si, ddoq_delete_callback);
	SET_SOFT_INSTR_CALLBACK_ARG(si, ddoq);

	si->req_info.octeon_id = oct->octeon_id;

	si->ih.tag        = ddoq->ddoq_setup.tag;
	si->ih.raw        = 1;
	si->ih.fsz        = 16;
	si->ih.tagtype    = OCTEON_TAG_TYPE_ATOMIC;
	si->ih.dlengsz    = sizeof(ddoq_front_data_t);
	si->dptr          = &ddoq->front;
	si->irh.opcode    = DDOQ_DELETE_OP;

	ddoq->dptr_start.s.desc_count = 0;

	return si;
}





/** Routine to create a DDOQ CREDIT soft instruction. */
static octeon_soft_instruction_t  *
ddoq_create_credit_soft_instr(octeon_device_t   *oct, octeon_ddoq_t     *ddoq)
{
	octeon_soft_instruction_t  *si = cavium_alloc_buffer(oct, OCT_SOFT_INSTR_SIZE);

	if(si == NULL) return NULL;

	cavium_memset(si, 0, OCT_SOFT_INSTR_SIZE);

	SET_SOFT_INSTR_DMA_MODE(si, OCTEON_DMA_GATHER);
	SET_SOFT_INSTR_RESP_ORDER(si, OCTEON_RESP_NORESPONSE);
	SET_SOFT_INSTR_TIMEOUT(si, 6000);
	SET_SOFT_INSTR_ALLOCFLAGS(si, (OCTEON_SOFT_INSTR_ALLOCATED));

	si->req_info.octeon_id = oct->octeon_id;

	si->ih.tag        = ddoq->ddoq_setup.tag;
	si->ih.raw        = 1;
	si->ih.fsz        = 16;
	si->ih.tagtype    = OCTEON_TAG_TYPE_ATOMIC;
	si->ih.gather     = 1;

	si->irh.opcode    = DDOQ_CREDIT_OP;


	/* Need 7 extra bytes to make gather list 8B aligned.
	 * Need additional bytes to carry the front data.
	 */
	si->gather_ptr = (octeon_sg_entry_t  *)cavium_alloc_buffer(oct, OCT_SG_ENTRY_SIZE + 7 + sizeof(struct ddoq_credit_gptr) + sizeof(ddoq_front_data_t));

	if(!si->gather_ptr) {
		cavium_free_buffer(oct, si);
		cavium_error("OCTEON_CNTQ: DDOQ gather list alloc failed\n");
		return NULL;
	}
	cavium_memset(si->gather_ptr, 0, OCT_SG_ENTRY_SIZE + 7 + sizeof(ddoq_front_data_t));

	si->dptr = si->gather_ptr;
	if((unsigned long)si->dptr & 0x07)
		si->dptr = (void *)(((unsigned long)si->dptr + 7) & ~(7UL));

	SET_SOFT_INSTR_ALLOCFLAGS(si, OCTEON_DPTR_GATHER);

	return si;
}













/* 
   Locks Held: None. This routine is called with ddoq->lock held
   11/19/2007: credit_count is guaranteed to fit in a gather buffer.
*/
static inline  void
ddoq_credit_fillgather(octeon_device_t             *octeon_dev,
                       octeon_ddoq_t               *ddoq,
                       octeon_soft_instruction_t   *soft_instr,
                       uint32_t                     credit_count,
                       uint32_t                     old_credit_index)
{
  octeon_sg_entry_t   *sg_entry  = (octeon_sg_entry_t *)soft_instr->dptr;
  ddoq_front_data_t   *ddoq_front;
  octeon_ddoq_desc_t  *desc_ring = ddoq->desc_ring;
  uint32_t             size2_count;

  ddoq_front = (ddoq_front_data_t *) ((uint8_t *)soft_instr->dptr + OCT_SG_ENTRY_SIZE + sizeof(struct ddoq_credit_gptr));

  ddoq_front->start.u64     = ddoq->dptr_start.u64;
  ddoq_front->next_pkt_cntr = ddoq->next_pkt_cntr;
  octeon_swap_8B_data((uint64_t *)ddoq_front, 1);
  octeon_swap_8B_data((uint64_t *)&ddoq_front->start, 1);

  cavium_memcpy(&ddoq_front->udd, &ddoq->ddoq_udd, MAX_DDOQ_UDD_SIZE);
  CAVIUM_ADD_SG_SIZE(sg_entry, sizeof(ddoq_front_data_t), 0);

  sg_entry->ptr[0]  = octeon_pci_map_single(octeon_dev->pci_dev, ddoq_front, sizeof(ddoq_front_data_t), CAVIUM_PCI_DMA_TODEVICE);

  if((old_credit_index + credit_count) <= ddoq->ddoq_setup.no_of_desc) {
     /* No wrap-around */
     CAVIUM_ADD_SG_SIZE(sg_entry, (credit_count * OCT_DDOQ_DESC_SIZE), 1);

     sg_entry->ptr[1] = octeon_pci_map_single(octeon_dev->pci_dev, &desc_ring[old_credit_index],(credit_count * OCT_DDOQ_DESC_SIZE), CAVIUM_PCI_DMA_TODEVICE);
     soft_instr->ih.dlengsz = 2;
   } else {
     /*
      * The read index has wrapped-around and is lower than old refill index
      */
     size2_count = ( ddoq->ddoq_setup.no_of_desc - old_credit_index);
     CAVIUM_ADD_SG_SIZE(sg_entry, (size2_count * OCT_DDOQ_DESC_SIZE), 1);

     sg_entry->ptr[1] = octeon_pci_map_single(octeon_dev->pci_dev, &desc_ring[old_credit_index], (size2_count * OCT_DDOQ_DESC_SIZE), CAVIUM_PCI_DMA_TODEVICE);
     CAVIUM_ADD_SG_SIZE(sg_entry, ((credit_count - size2_count) * OCT_DDOQ_DESC_SIZE), 2);

     sg_entry->ptr[2]  = octeon_pci_map_single(octeon_dev->pci_dev, desc_ring, ((credit_count - size2_count) * OCT_DDOQ_DESC_SIZE), CAVIUM_PCI_DMA_TODEVICE);
     soft_instr->ih.dlengsz = 3;
  }

}
















/**
   Routine to free ddoq buffers and the ddoq structure itself.
   The delete confirmation callback is called from here.
*/
void
octeon_ddoq_free_resources(octeon_device_t    *octeon_dev,
                           octeon_ddoq_t      *ddoq)
{
   uint32_t   i, count = 10;

   cavium_spin_lock_softirqsave(&ddoq->lock);
   ddoq->state = DDOQ_STATE_DELETE;
   cavium_spin_unlock_softirqrestore(&ddoq->lock);

   while( count-- && cavium_atomic_read(&ddoq->in_process_ddoq))
        cavium_sleep_timeout(1);

   if(cavium_atomic_read(&ddoq->in_process_ddoq)) {
       cavium_error("DDOQ[%d] taking too long to process pkts (resources not freed)!!\n", ddoq->ddoq_id);
       return;
   }

   if(ddoq->ddoq_setup.buf_dealloc_fn)   {
      /*
       * After a buffer has been moved to app, the entry in desc is made 0,
       * for the time that it is not refilled. Such buffers would be freed by
       * the app.
       */
      i = ddoq->host_read_index;
      while(ddoq->avl_desc) {
           if(ddoq->buffer_list[i]) {
              if(ddoq->desc_ring[i].buffer_ptr) {
                 octeon_pci_unmap_single(octeon_dev->pci_dev, ddoq->desc_ring[i].buffer_ptr, ddoq->ddoq_setup.buffer_size, CAVIUM_PCI_DMA_FROMDEVICE);  
               }
               ddoq->ddoq_setup.buf_dealloc_fn(ddoq->ddoq_id, ddoq->buffer_list[i]); 
               ddoq->avl_desc--;
           }
           INCR_INDEX_BY1(i, ddoq->ddoq_setup.no_of_desc);
      }
   }

   /*
    * If the delete happens at driver unload time, the delete confirmation 
    * routine may not exist.
    */
   if(ddoq->ddoq_setup.delete_conf_fn) {
      ddoq->ddoq_setup.delete_conf_fn(ddoq->ddoq_id, 0);
   }

#ifdef CAVIUM_SPINLOCK_DEBUG
   cavium_spin_lock_destroy(&ddoq->lock);
#endif
   octeon_pci_unmap_single(octeon_dev->pci_dev, ddoq->info_ptr_dma, ((ddoq->ddoq_setup.no_of_desc * OCT_DDOQ_INFO_SIZE) + 7), CAVIUM_PCI_DMA_FROMDEVICE);
   octeon_pci_unmap_single(octeon_dev->pci_dev, ddoq->front.host_status_addr, sizeof(uint32_t), CAVIUM_PCI_DMA_FROMDEVICE);
   cavium_free_buffer(octeon_dev, ddoq);
}











/** Routine to allocate descriptor ring for a DDOQ. */
void *
alloc_ddoq_ring(octeon_device_t      *octeon_dev,
                uint32_t              ddoq_id,
                octeon_ddoq_setup_t  *ddoq_setup)
{
	octeon_ddoq_t        *ddoq;
	octeon_ddoq_desc_t   *desc_ring;
	octeon_ddoq_info_t   *info_ring;
	void                 *buf, **buffer_list;
	uint32_t              alloc_size, info_ring_size;
	uint32_t              i, buffer_list_size, desc_ring_size;
	uint32_t              initial_fill=0;



	cavium_print(PRINT_FLOW, "\n\n----#  alloc_ddoq_ring #----\n");


	desc_ring_size = ddoq_setup->no_of_desc * OCT_DDOQ_DESC_SIZE;
	info_ring_size = (ddoq_setup->no_of_desc * OCT_DDOQ_INFO_SIZE) + 7;
	buffer_list_size = sizeof(void *) * ddoq_setup->no_of_desc;
	alloc_size = OCT_DDOQ_SIZE + desc_ring_size + info_ring_size + buffer_list_size;


	buf = cavium_alloc_buffer(octeon_dev, alloc_size);
	if(buf == NULL) {
		cavium_error("OCTEON_CNTQ: Allocation failed for ddoq creation\n");
		return NULL;
	}
	cavium_memset(buf, 0, alloc_size);

	ddoq            = (octeon_ddoq_t *)buf;
	desc_ring       = (octeon_ddoq_desc_t *) ((uint8_t *)ddoq + OCT_DDOQ_SIZE);
	ddoq->desc_ring = desc_ring;

	info_ring = (octeon_ddoq_info_t  *)((uint8_t *)desc_ring + desc_ring_size);
	if((unsigned long)info_ring & 0x07)  {
		info_ring = (octeon_ddoq_info_t *)(((unsigned long)info_ring+7) & ~(7UL));
	}
 
	buffer_list = (void **)((uint8_t *)info_ring + info_ring_size);
	ddoq->buffer_list    =  buffer_list;

	ddoq->info_ptr       =  info_ring;
	ddoq->info_ptr_dma   =  (uint64_t)octeon_pci_map_single(octeon_dev->pci_dev, info_ring, info_ring_size, CAVIUM_PCI_DMA_FROMDEVICE);

	CAVIUM_INIT_LIST_HEAD(&ddoq->list);

	cavium_atomic_set(&ddoq->pkts_pending, 0);
	cavium_atomic_set(&ddoq->in_process_ddoq, 0);

	ddoq->desc_pending_refill = ddoq_setup->no_of_desc;
	ddoq->dptr_start.s.buf_size = ddoq_setup->buffer_size;
	if(!ddoq_setup->initial_fill)
		initial_fill = ddoq_setup->no_of_desc;
	else
		initial_fill = ddoq_setup->initial_fill;

	ddoq->refill_index = 0;

	for (i = 0; i < initial_fill; i++)  {
		info_ring[i].buf_size = ddoq_setup->buffer_size;
		desc_ring[i].info_ptr = (uint64_t)(ddoq->info_ptr_dma + (OCT_DDOQ_INFO_SIZE * i));
		buf = ddoq_setup->buf_alloc_fn(ddoq_id, ddoq_setup->buffer_size);
		if(buf) {
			buffer_list[i]          = buf;
			desc_ring[i].buffer_ptr = (uint64_t)octeon_pci_map_single(octeon_dev->pci_dev,buf, ddoq_setup->buffer_size, CAVIUM_PCI_DMA_FROMDEVICE);
        } else  {
			goto DDOQ_RING_ALLOC_FAILED;
		}
		ddoq->desc_pending_refill--;
		INCR_INDEX_BY1(ddoq->refill_index,ddoq_setup->no_of_desc);
	}

	octeon_swap_8B_data((uint64_t *)desc_ring, (i * 2));

	ddoq->dptr_start.s.desc_count = ddoq_setup->no_of_desc;
	ddoq->avl_desc       = (uint32_t)initial_fill;
	ddoq->avl_info_ptrs  = (uint32_t)initial_fill;
	ddoq->credit_index   = ddoq->refill_index;

	cavium_memcpy(&ddoq->ddoq_setup, ddoq_setup, OCT_DDOQ_SETUP_SIZE);
	if(!ddoq_setup->refill_threshold)
		ddoq->ddoq_setup.refill_threshold = 1;

	ddoq->dptr_start.s.ddoq_id = ddoq_id;

	return (void *)ddoq;

DDOQ_RING_ALLOC_FAILED:
	cavium_error("OCTEON_CNTQ: Allocation failed for DDOQ buffer(desc)\n");
	while (i-- > 0) {
		octeon_pci_unmap_single(octeon_dev->pci_dev, desc_ring[i].buffer_ptr, ddoq_setup->buffer_size, CAVIUM_PCI_DMA_FROMDEVICE);
		octeon_pci_unmap_single(octeon_dev->pci_dev, desc_ring[i].info_ptr, OCT_DDOQ_INFO_SIZE, CAVIUM_PCI_DMA_FROMDEVICE);
		ddoq_setup->buf_dealloc_fn(ddoq_id, buffer_list[i]);
	}
	cavium_free_buffer(octeon_dev, ddoq);
	return NULL;
}









/** API exported to other kernel application to create a DDOQ. */
int
octeon_ddoq_create(uint32_t               octeon_id,
                   octeon_ddoq_setup_t   *ddoq_op)
{
   uint32_t                    ddoq_id;
   octeon_device_t            *octeon_dev;
   octeon_ddoq_t             **ddoq_pptr, *ddoq;
   octeon_soft_instruction_t  *soft_instr;
   octeon_instr_status_t       retval;

   cavium_print(PRINT_FLOW,"\n\n----#  octeon_ddoq_create #----\n");
   octeon_dev = get_octeon_device_ptr(octeon_id);
   if(octeon_dev == NULL) {
       cavium_error("OCTEON_CNTQ: No such octeon device for DDOQ create\n");
       return -1;
   }

   if( (cavium_atomic_read(&octeon_dev->status) != OCT_DEV_RUNNING) ||
       (octeon_dev->cntq_ready == 0)) {
      cavium_error("OCTCNTQ: Device not ready for DDOQ create\n");
      print_octeon_state_errormsg(octeon_dev);
      return 1;
   }


   if (   (ddoq_op->no_of_desc == 0)
       || (ddoq_op->dispatch_fn == NULL)
       || (ddoq_op->delete_conf_fn == NULL)
       || (ddoq_op->buf_alloc_fn == NULL)
       || (ddoq_op->buffer_size == 0)
       || (ddoq_op->buf_dealloc_fn == NULL)
       || (ddoq_op->initial_fill&& (ddoq_op->initial_fill > ddoq_op->no_of_desc))) {
       cavium_error("OCTEON_CNTQ: Incorrect parameters in DDOQ setup params\n");
       goto  DDOQ_FAILED_CREATION;
   }


   if(get_next_idx((octeon_ddoq_list_t *)octeon_dev->ddoq_list, &ddoq_id, (void ***)&ddoq_pptr)) {
       cavium_print(PRINT_DEBUG, "ddoq_id is %d\n", ddoq_id);
       *ddoq_pptr =  alloc_ddoq_ring(octeon_dev, ddoq_id, ddoq_op);
   }
   else {
       cavium_error("OCTEON_CNTQ: Failed to add ddoq to list\n");
       goto  DDOQ_FAILED_CREATION;
   }

   if(*ddoq_pptr == NULL) {
       cavium_error("OCTEON_CNTQ: Could not allocate memory for DDOQ\n");
       goto  DDOQ_CREATE_DELETE_DDOQ;
   }
   ddoq = *ddoq_pptr;
   ddoq->ddoq_id       = ddoq_id;
   /* The tag value would be unique to each ddoq and can be safely used here.*/
   ddoq->next_pkt_cntr = ddoq->ddoq_setup.tag;
   ddoq->state         = DDOQ_STATE_CREATE_INIT;
   ddoq->octeon_dev    = octeon_dev;
   ddoq->dptr_start.s.type   = ddoq_op->type;

   if((ddoq_op->type == DDOQ_TYPE_TCP) && (ddoq_op->ddoq_udd_size)) {

       /* Keep a local copy of the UDD */
       cavium_memcpy(&ddoq->ddoq_udd,ddoq_op->ddoq_udd, ddoq_op->ddoq_udd_size);

       /* Copy it to the front data as well to be sent with INIT */
       cavium_memcpy(&ddoq->front.udd,ddoq_op->ddoq_udd,ddoq_op->ddoq_udd_size);
   }

   cavium_spin_lock_init(&(ddoq->lock));

   /* This call is to create the INIT instruction which will be posted
      right away */
   soft_instr = ddoq_create_init_soft_instr(octeon_dev, ddoq);
   if(soft_instr) {

      retval = octeon_process_instruction(octeon_dev, soft_instr, NULL);
      if(retval.s.error) {
          cavium_error("OCTEON_CNTQ: DDOQ INIT instruction failed status: %x\n", retval.s.status);
          /* octeon_process_instruction() would have freed the instr */
          goto  DDOQ_CREATE_FREE_INSTR;
      }
   } else {
       goto  DDOQ_CREATE_DELETE_DDOQ;
   }

   ddoq_list_update_count(octeon_dev, DDOQ_INIT_OP );
   return ddoq_id;



DDOQ_CREATE_FREE_INSTR:
DDOQ_CREATE_DELETE_DDOQ:
     release_ddoq_entry(octeon_dev, ddoq_id);
DDOQ_FAILED_CREATION:
     ddoq_list_update_count(octeon_dev, DDOQ_FAIL_CODE );
     return -1;

}






void
ddoq_delete_callback(octeon_req_status_t status, void *arg)
{
	octeon_ddoq_t    *ddoq = (octeon_ddoq_t *)arg;
	octeon_device_t  *oct = ddoq->octeon_dev;

	if(status == 0) {
		if(ddoq->state != DDOQ_STATE_NULL) {
			oct_ddoq_add_to_deferred_list(oct, ddoq);
		} else {
			/* Release the DDOQ entry */
			release_ddoq_entry(oct, ddoq->ddoq_id);
			ddoq_list_update_count(oct, DDOQ_DELETE_OP );
			octeon_ddoq_free_resources(oct, ddoq);
			ddoq_list_update_count(oct, DDOQ_EOT_OP );
		}
	} else {
		cavium_error("OCTCNTQ: DDOQ[%d] delete failed in core\n", ddoq->ddoq_id);
	}
}





/** Routine to send a DDOQ delete instruction. Called by octeon_ddoq_delete()
    API and by CNTQ module during unload to cleanup existing DDOQ's.
  */
static int
__octeon_send_ddoq_delete(octeon_device_t   *oct, octeon_ddoq_t  *ddoq)
{
	octeon_soft_instruction_t  *si = ddoq_create_delete_soft_instr(oct, ddoq);
	octeon_instr_status_t       retval;

	retval.u64 = OCTEON_REQUEST_FAILED;

	if(si) {
		retval = octeon_process_instruction(oct, si, NULL);
		if(!retval.s.error)
			return 0;
	}
 
	if(si == NULL)
		cavium_error("OCTEON_CNTQ: DDOQ DELETE instruction alloc failed !\n");

	if(retval.s.error)
		cavium_error("OCTEON_CNTQ: Sending DDOQ DELETE instruction failed! status: %x\n", retval.s.status);
	/* octeon_process_instruction would have freed the instr */

	return 1;
}







/** API exported to other kernel application to create a DDOQ.
   Return Value:
        Success:  0
        Failure:  1
*/
uint32_t
octeon_ddoq_delete(uint32_t    octeon_id,
                   uint32_t    ddoq_id,
                   void       *udd,
                   uint32_t    udd_size)
{
   octeon_device_t            *octeon_dev;
   octeon_ddoq_t              *ddoq;

   cavium_print(PRINT_FLOW,"\n\n--# octeon_ddoq_delete; id: %d #--\n", ddoq_id);
   octeon_dev = get_octeon_device_ptr(octeon_id);
   if(octeon_dev == NULL) {
      cavium_error("OCTCNTQ: Unknown octeon device for DDOQ delete\n");
      return 1;
   }

   if( (cavium_atomic_read(&octeon_dev->status) != OCT_DEV_RUNNING) ||
       (octeon_dev->cntq_ready == 0)) {
      cavium_error("OCTCNTQ: Device not ready for DDOQ delete\n");
      print_octeon_state_errormsg(octeon_dev);
      return 1;
   }


   if((int)ddoq_id == -1 || ddoq_id < DDOQ_ID_OFFSET)  {
      cavium_error("OCTCNTQ: Invalid DDOQ Id: %d for ddoq delete\n", ddoq_id);
      return 1;
   }


   ddoq = (octeon_ddoq_t *)get_ddoq_ptr(octeon_dev, ddoq_id);
   if(ddoq == NULL ) {
      cavium_error("OCTCNTQ: No DDOQ to delete with id: %d\n", ddoq_id);
      return 1;
   }

   if(ddoq->state >= DDOQ_STATE_READY) {
proceed_with_delete:
      if(udd_size && udd)
          cavium_memcpy(&ddoq->front.udd, udd, udd_size);
       ///// STATE SHOULD BE ATOMIC /////
       ddoq->state = DDOQ_STATE_DELETE_INIT;
       return __octeon_send_ddoq_delete(octeon_dev, ddoq);
   } else {
       cavium_sleep_timeout(1);
       if(ddoq->state >= DDOQ_STATE_READY)
          goto proceed_with_delete;
       else
           cavium_error("OCTCNTQ: DDOQ[%d] delete called in state %x\n",
                    ddoq_id, ddoq->state);
   }

   return 1;
}





/*
   Return Value:
        Success:  0
        Failure:  1

   Locks held: The ddoq lock is held throughout this routine.
*/
uint32_t
octeon_ddoq_credit(octeon_device_t    *octeon_dev,
                   octeon_ddoq_t      *ddoq,
                   uint32_t            no_of_desc,
                   void              **buf_list,
                   uint32_t           *size_list)
{
	octeon_ddoq_desc_t         *desc_ring;
	octeon_ddoq_setup_t        *ddoq_setup;
	octeon_soft_instruction_t  *soft_instr;
	void                       *buf;
	struct ddoq_credit_gptr    *gptr;
	int                         credit_count, credit_retry_count = 0;
	octeon_instr_status_t       retval;

#define MAX_CREDIT_RETRY_COUNT  10

	cavium_spin_lock_softirqsave(&ddoq->lock);

	ddoq_setup  = &(ddoq->ddoq_setup);
	desc_ring   = ddoq->desc_ring;
	cavium_print(PRINT_FLOW,"\n\n--#  octeon_ddoq_credit id: %d #--\n", ddoq->ddoq_id);

	/* Since we wait for the lock (above), the tasklet could have run and
	   completed a credit leaving no descriptors to refill. Check here
	   before we proceed. */
	if( ((ddoq->state != DDOQ_STATE_READY) && (ddoq->state != DDOQ_STATE_CREATE_INIT))
	   || !(ddoq->desc_pending_refill)) {
		cavium_spin_unlock_softirqrestore(&ddoq->lock);
		return 1;
	}
	

	if(ddoq_setup->buf_alloc_fn)   {
		/* Delayed filling of info pointers in case where initial_fill
		   was less than no. of desc. */
		if(ddoq->avl_info_ptrs != ddoq_setup->no_of_desc) {
   			octeon_ddoq_info_t  *info_ring = ddoq->info_ptr;
			int i;
			for (i = ddoq->avl_info_ptrs; i < ddoq_setup->no_of_desc; i++)  {
				info_ring[i].buf_size  = ddoq_setup->buffer_size;
				desc_ring[i].info_ptr = (uint64_t)(ddoq->info_ptr_dma + (OCT_DDOQ_INFO_SIZE * i));
				octeon_swap_8B_data(&desc_ring[i].info_ptr, 1);
			}
			ddoq->avl_info_ptrs = ddoq_setup->no_of_desc;
		}

		while (ddoq->desc_pending_refill) {
			buf = ddoq_setup->buf_alloc_fn(ddoq->ddoq_id, ddoq_setup->buffer_size);
			if(!buf) {
				cavium_error("OCTEON_CNTQ: DDOQ[%d] refill failed (%d pending)\n",ddoq->ddoq_id, ddoq->desc_pending_refill);
				break;
			}
			ddoq->buffer_list[ddoq->refill_index] = buf;
			desc_ring[ddoq->refill_index].buffer_ptr = (uint64_t)octeon_pci_map_single(octeon_dev->pci_dev, buf, ddoq_setup->buffer_size, CAVIUM_PCI_DMA_FROMDEVICE);
			octeon_swap_8B_data(&desc_ring[ddoq->refill_index].buffer_ptr, 1);
			INCR_INDEX_BY1(ddoq->refill_index, ddoq_setup->no_of_desc);
			ddoq->desc_pending_credit++;
			ddoq->desc_pending_refill--;
		}
	} else  {
		int  i;
		for (i = 0; i < no_of_desc; i++)  {
			desc_ring[ddoq->refill_index].buffer_ptr =  (uint64_t)octeon_pci_map_single(octeon_dev->pci_dev, buf_list[i], size_list[i], CAVIUM_PCI_DMA_FROMDEVICE);
			octeon_swap_8B_data(&desc_ring[ddoq->refill_index].buffer_ptr, 1);
			ddoq->buffer_list[ddoq->refill_index]       = buf_list[i];
			ddoq->info_ptr[ddoq->refill_index].buf_size = size_list[i];
			INCR_INDEX_BY1(ddoq->refill_index, ddoq_setup->no_of_desc);    
		}
		ddoq->desc_pending_refill  -= no_of_desc;
		ddoq->desc_pending_credit  += no_of_desc;
	}

	if(!ddoq->desc_pending_credit) {
		cavium_error("OCTEON_CNTQ: Credit failed for DDOQ[%d]\n",ddoq->ddoq_id);
		cavium_spin_unlock_softirqrestore(&ddoq->lock);
		return 1;
	}

	ddoq->dptr_start.s.desc_count =  ddoq->desc_pending_credit;
	ddoq->avl_desc +=  ddoq->desc_pending_credit;

retry_ddoq_credit:
	if((ddoq->state != DDOQ_STATE_READY ) && (ddoq->state != DDOQ_STATE_CREATE_INIT )) {
		cavium_spin_unlock_softirqrestore(&ddoq->lock);
		return 1;
	}

	soft_instr = ddoq_create_credit_soft_instr(octeon_dev, ddoq);
	if(soft_instr == NULL) {
		cavium_error("OCTEON_CNTQ: Soft instr alloc for DDOQ[%d] CREDIT failed!\n", ddoq->ddoq_id);
		return 1;
	}

	/* Limit the credit to what can be fit into a gather buffer. */
	if( (ddoq->desc_pending_credit * OCT_DDOQ_DESC_SIZE) < OCT_MAX_GATHER_DATA_SIZE)
		credit_count = ddoq->desc_pending_credit;
	else
		credit_count = (OCT_MAX_GATHER_DATA_SIZE / OCT_DDOQ_DESC_SIZE);


	ddoq_credit_fillgather(octeon_dev, ddoq, soft_instr, credit_count, ddoq->credit_index);

	gptr = (struct ddoq_credit_gptr *)((uint8_t *)soft_instr->dptr + OCT_SG_ENTRY_SIZE);
	gptr->octeon_dev = octeon_dev;
	gptr->soft_instr = soft_instr;
	SET_SOFT_INSTR_CALLBACK_ARG(soft_instr, gptr);

	retval = octeon_process_instruction(octeon_dev, soft_instr, NULL);
	if(retval.s.error) {
		ddoq->stats.credit_instr_failed++;
		if(credit_retry_count++ < MAX_CREDIT_RETRY_COUNT) {
			goto retry_ddoq_credit;
		}
		cavium_error("OCTEON_CNTQ: Sending CREDIT for DDOQ[%d] failed!\n", ddoq->ddoq_id);
		cavium_spin_unlock_softirqrestore(&ddoq->lock);
		return 1;
	}
	ddoq->stats.credit_instr_cnt++;
	ddoq->stats.credit_desc_cnt += credit_count;
	ddoq->desc_pending_credit   -= credit_count;
	INCR_INDEX(ddoq->credit_index, credit_count, ddoq_setup->no_of_desc);
	cavium_spin_unlock_softirqrestore(&ddoq->lock);

	return 0;
}









/* API to restart ddoq packet processing if it was stopped earlier by the 
   called application. */
int
octeon_ddoq_restart_rx(uint32_t   octeon_id,
                       uint32_t   ddoq_id)
{
	octeon_device_t   *octeon_dev;
	octeon_ddoq_t     *ddoq;

	octeon_dev = get_octeon_device_ptr(octeon_id);
	if(octeon_dev == NULL) {
		cavium_error("OCTEON_CNTQ: ddoq_restart_rx for unknown octeon device: %d\n", octeon_id);
		return ENODEV;
	}

	ddoq = (octeon_ddoq_t *)get_ddoq_ptr(octeon_dev, ddoq_id);
	if(ddoq == NULL ) {
		cavium_error("OCTEON_CNTQ: ddoq_restart_rx for unknown ddoq: %d\n", ddoq_id);
		return ENODEV;
	}

	if(ddoq->state != DDOQ_STATE_STOP_RX) {
		cavium_error("OCTEON_CNTQ: ddoq_restart_rx called for ddoq[%d] in state 0x%x\n", ddoq_id, ddoq->state);
		return EBUSY;
	}

	cavium_spin_lock_softirqsave(&ddoq->lock);
	ddoq->state = DDOQ_STATE_READY;
	cavium_spin_unlock_softirqrestore(&ddoq->lock);

	if(cavium_atomic_read(&ddoq->pkts_pending)) {
		octeon_process_ddoq(octeon_dev, ddoq, 0);
	}
	if((ddoq->ddoq_setup.buf_alloc_fn) && (ddoq->desc_pending_refill >= ddoq->ddoq_setup.refill_threshold)) {
		octeon_ddoq_credit(octeon_dev, ddoq, ddoq->desc_pending_refill, NULL, NULL);
	}

	return 0;
}



int
octeon_ddoq_change_tag(uint32_t   octeon_id,
                       uint32_t   ddoq_id,
                       uint32_t   tag)
{
	octeon_device_t   *octeon_dev;
	octeon_ddoq_t     *ddoq;

	octeon_dev = get_octeon_device_ptr(octeon_id);
	if(octeon_dev == NULL) {
		cavium_error("OCTEON_CNTQ: ddoq_change_tag for unknown octeon device: %d\n", octeon_id);
		return ENODEV;
	}

	ddoq = (octeon_ddoq_t *)get_ddoq_ptr(octeon_dev, ddoq_id);
	if(ddoq == NULL ) {
		cavium_error("OCTEON_CNTQ: ddoq_change_tag for unknown ddoq: %d\n", ddoq_id);
		return ENODEV;
	}
	cavium_spin_lock_softirqsave(&ddoq->lock);
	ddoq->ddoq_setup.tag = tag;
	cavium_spin_unlock_softirqrestore(&ddoq->lock);
	return 0;
}


#ifdef USE_DDOQ_THREADS

static inline void
octeon_cntq_process_ddoq_local(octeon_device_t   *octeon_dev,
                             uint32_t           ddoq_id,
                             uint32_t           request_id,
                             uint32_t		pkt_cnt)
{
	octeon_ddoq_t     *ddoq = (octeon_ddoq_t *)get_ddoq_ptr(octeon_dev, ddoq_id);

	/* Not checking the state anymore; its being done in process_ddoq. */
	if(ddoq) {
		if(((ddoq->next_pkt_cntr & 0xffff) == request_id)) {
			octeon_process_ddoq(octeon_dev, ddoq, pkt_cnt);
		} else {
			cavium_error("ddoq[%d] expected pkt no. 0x%x received: 0x%x\n",
			             ddoq_id, (uint32_t)(ddoq->next_pkt_cntr&0xffff),
			             request_id);
		}
	}
}

int cvm_ddoq_num_threads = CVM_MAX_DDOQ_THREADS;

int octeon_process_ddoq_thread_pkts(cvm_ddoq_thread_t *dthread, int npkts)
{
	int i;
	octeon_ddoq_thread_info_t *dthread_info;

	for (i = 0; i < npkts; i++) {
		dthread->th_read_idx = (dthread->th_read_idx + 1) % CVM_DDOQ_MAX_THREAD_PKTS;
		dthread_info = &(dthread->th_info[dthread->th_read_idx]);
		octeon_cntq_process_ddoq_local (dthread->oct_dev, dthread_info->ddoq_id, 
		                      dthread_info->req_id, dthread_info->num_pkts);
	}

	return 0;
}

int oct_ddoq_thread(void* arg)   
{  
	cvm_ddoq_thread_t *dthread = (cvm_ddoq_thread_t *)arg;
	char   name[] = "Octeon DDOQ Thread";
	int npkts = 0; 

	cavium_atomic_set(&dthread->thread_active, 1);

	cavium_print_msg("\n-- OCTEON: %s starting execution now on cpu %d!\n",
	              name, smp_processor_id());

	while(!dthread->stop_thread && !cavium_kthread_signalled())  {

		npkts = cavium_atomic_read(&dthread->ddoq_pkts_queued);
		cavium_atomic_sub(npkts, &dthread->ddoq_pkts_queued);

		if (npkts)
			octeon_process_ddoq_thread_pkts(dthread, npkts);

		npkts = cavium_atomic_read(&dthread->ddoq_pkts_queued);
		if (!npkts)
			cavium_sleep_atomic_cond(&dthread->wc, &dthread->ddoq_pkts_queued);
	}

	cavium_print_msg("\n-- OCTEON: DROQ thread quitting now\n");
	cavium_atomic_set(&dthread->thread_active, 0);

	return 0;
}

void octeon_ddoq_stop_threads (void *octeon_dev)
{
	octeon_device_t *oct_dev = (octeon_device_t *) octeon_dev;
	int i;

	for (i = 0; i < cvm_ddoq_num_threads; i++) {
		cvm_ddoq_thread_t *dthread = &(oct_dev->ddoq_thread[i]);

		if (CVM_KTHREAD_EXISTS(&dthread->thread)) {
			cavium_print_msg("Killing DDOQ[%d] thread\n", i);

			cavium_atomic_inc(&dthread->ddoq_pkts_queued);
			dthread->stop_thread = 1; 
			cavium_flush_write();

			cavium_kthread_destroy(&dthread->thread); 
		
			/* Wait till the ddoq thread has come out of its loop. */
			while(cavium_atomic_read(&dthread->thread_active))
				cavium_sleep_timeout(1);
		}
	}
}

#endif



/*
  This routine will be called to process packets received on a DDOQ.
*/
void
octeon_process_ddoq(octeon_device_t     *octeon_dev,
                    octeon_ddoq_t       *ddoq,
                    uint32_t             pkt_cnt)
{
	octeon_ddoq_setup_t *ddoq_setup;
	octeon_ddoq_desc_t  *desc_ring;
	octeon_ddoq_info_t  *info_ptr;
	uint32_t             bytes_left, pkts_processed=0;
	uint16_t             j;
	uint32_t             ddoq_id, ddoq_bufsize=0;
	octeon_recv_info_t  *recv_info;
	octeon_recv_pkt_t   *recv_pkt;

	cavium_print(PRINT_FLOW, "\n\n----# octeon_process_ddoq [%d]#----\n", ddoq->ddoq_id); 

	cavium_spin_lock_softirqsave(&ddoq->lock);
	ddoq->next_pkt_cntr       += pkt_cnt;
	ddoq->stats.packets_recvd += pkt_cnt;

	/* Add the received packet count to the packets pending. */
	cavium_atomic_add(pkt_cnt, &ddoq->pkts_pending);

	cavium_atomic_set(&ddoq->in_process_ddoq, 1);

	cavium_print(PRINT_DEBUG, "host_read_index: %d\n", ddoq->host_read_index);
	ddoq_id      = ddoq->ddoq_id;
	ddoq_setup   = &ddoq->ddoq_setup;
	desc_ring    = ddoq->desc_ring;
	ddoq_bufsize = ddoq_setup->buffer_size;

	/* The state can change everytime we release the lock, so check state for
	 * each packet. */
	while( ((ddoq->state == DDOQ_STATE_READY ) || (ddoq->state == DDOQ_STATE_CREATE_INIT ))
            && cavium_atomic_read(&ddoq->pkts_pending)) {

		info_ptr     = &ddoq->info_ptr[ddoq->host_read_index];
		octeon_swap_8B_data((uint64_t *)info_ptr, 2);

		ddoq->stats.bytes_received += info_ptr->pkt_size;
		if(info_ptr->pkt_size  > (MAX_RECV_BUFS * ddoq_bufsize)) {
			cavium_spin_unlock_softirqrestore(&ddoq->lock);
			cavium_error("OCTEON_CNTQ: ddoq id: %d ddoq_bufsize: %d \n", ddoq_id, ddoq_bufsize); 
			cavium_error("OCTEON_CNTQ: info pkt_size: %d  buf_size: %d\n", info_ptr->pkt_size, info_ptr->buf_size);
			return;
		}

		recv_info = octeon_alloc_recv_info(0); 
		if(!recv_info) {
			cavium_spin_unlock_softirqrestore(&ddoq->lock);
			cavium_error("OCTEON_CNTQ: DDOQ[%d] recv_info allocation failed.\n", ddoq->ddoq_id);
			return ;
		}
		recv_pkt           = recv_info->recv_pkt;
		recv_pkt->length   = (uint32_t)info_ptr->pkt_size;
		*((uint64_t *)&recv_pkt->resp_hdr) = info_ptr->resp_hdr;
		recv_pkt->buf_type = OCT_RECV_BUF_TYPE_1;


		j = 0;
		bytes_left=(uint32_t)info_ptr->pkt_size;
		do {
			recv_pkt->buffer_ptr[j] = ddoq->buffer_list[ddoq->host_read_index];
			octeon_pci_unmap_single(octeon_dev->pci_dev, desc_ring[ddoq->host_read_index].buffer_ptr, ddoq_bufsize, CAVIUM_PCI_DMA_FROMDEVICE); 
			desc_ring[ddoq->host_read_index].buffer_ptr = 0;
			ddoq->buffer_list[ddoq->host_read_index]    = 0;
			recv_pkt->buffer_size[j] = (bytes_left > ddoq_bufsize)?ddoq_bufsize:bytes_left;
			bytes_left              -=  recv_pkt->buffer_size[j];
			INCR_INDEX_BY1(ddoq->host_read_index, ddoq_setup->no_of_desc);    
			info_ptr  = &ddoq->info_ptr[ddoq->host_read_index];
			j++;
		} while(bytes_left);
		recv_pkt->buffer_count     = j;
		ddoq->stats.buffers_recvd += j;
		ddoq->desc_pending_refill += j;
		ddoq->avl_desc            -= j;

		cavium_atomic_dec(&ddoq->pkts_pending);
		pkts_processed++;
		cavium_spin_unlock_softirqrestore(&ddoq->lock);

		if(ddoq_setup->dispatch_fn(recv_info, ddoq_setup->fn_arg)) {
			cavium_spin_lock_softirqsave(&ddoq->lock);
			ddoq->state = DDOQ_STATE_STOP_RX; 
			break;
		}
		cavium_spin_lock_softirqsave(&ddoq->lock);
	} /* While pkts left for processing. */

	cavium_spin_unlock_softirqrestore(&ddoq->lock);

	if((ddoq_setup->buf_alloc_fn) && (ddoq->desc_pending_refill >= ddoq_setup->refill_threshold)) {
		octeon_ddoq_credit(octeon_dev, ddoq, ddoq->desc_pending_refill, NULL, NULL);
	}

	cavium_atomic_set(&ddoq->in_process_ddoq, 0);

	return;
}






int
octeon_ddoq_get_stats(int                octeon_id,
                      int                ddoq_id,
                      oct_ddoq_stats_t  *ddoq_stats)
{
	octeon_device_t   *octeon_dev = get_octeon_device_ptr(octeon_id);
	octeon_ddoq_t     *ddoq;

	if(octeon_dev == NULL) {
		cavium_error("OCTEON_CNTQ: %s: No octeon device\n", __CVM_FUNCTION__);
		return ENODEV;
	}

	ddoq = get_ddoq_ptr(octeon_dev, ddoq_id);
	if(ddoq == NULL) {
		cavium_error("OCTEON_CNTQ: %s: No DDOQ device\n", __CVM_FUNCTION__);
		return ENODEV;
	}

	cavium_spin_lock_softirqsave(&ddoq->lock);
	cavium_memcpy(ddoq_stats, &ddoq->stats, OCT_DDOQ_STATS_SIZE);
	ddoq_stats->state               = ddoq->state;
	ddoq_stats->pkts_pending        = cavium_atomic_read(&ddoq->pkts_pending);
	ddoq_stats->desc_pending_refill = ddoq->desc_pending_refill;
	ddoq_stats->desc_pending_credit = ddoq->desc_pending_credit;
	cavium_spin_unlock_softirqrestore(&ddoq->lock);
	return 0;
}








void
octeon_cleanup_active_ddoqs(int octeon_id)
{
	octeon_device_t       *octeon_dev;
	octeon_ddoq_list_t    *ddoq_list;
	uint32_t                 i, j, k;
	ddoq_entry_t         **dlist2, *dlist3;

	octeon_dev = get_octeon_device_ptr(octeon_id);
	if(octeon_dev == NULL) {
		cavium_error("OCTEON_CNTQ: %s: Invalid octeon id %d\n",
		             __CVM_FUNCTION__, octeon_id);
		return;
	}
	ddoq_list = octeon_dev->ddoq_list;
	if(ddoq_list == NULL) {
		cavium_error("OCTEON_CNTQ: No ddoq list for octeon device %d\n",
		              octeon_id);
		return;
	}

	/* Traverse the DDOQ list and cleanup all existing DDOQ structures.
	   Do not call the dealloc function ptr or delete conf function ptr
	   since the application that created this DDOQ may not exist.
	*/
	for( i = 0; i < ddoq_list->l2_count; i++)  {
		dlist2 = ddoq_list->level1[i];
		for( j = 0; j < ddoq_list->l3_count[i]; j++)  {
			dlist3 = dlist2[j];
			for( k = 0; k < (1 << LEVEL3_BITS); k++)  {
				octeon_ddoq_t  *ddoq = dlist3[k].ddoq_ptr;
				if(ddoq) {
					ddoq_list_update_count(octeon_dev, DDOQ_DELETE_OP );
					ddoq->ddoq_setup.buf_dealloc_fn = NULL;
					ddoq->ddoq_setup.delete_conf_fn = NULL;
					release_ddoq_entry(octeon_dev, ddoq->ddoq_id);
					ddoq_list_update_count(octeon_dev, DDOQ_EOT_OP );
					octeon_ddoq_free_resources(octeon_dev, ddoq);
				}
			}  /* for  k */
		}  /* for j */
	} /* for i */

}




/* $Id: octeon_ddoq.c 67088 2011-11-15 19:39:17Z mchalla $ */

