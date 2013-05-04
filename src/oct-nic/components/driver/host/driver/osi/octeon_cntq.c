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
#include "octeon_debug.h"
#include "octeon_macros.h"
#include "octeon_hw.h"
#include "octeon_cntq.h"
#include "octeon_ddoq_list.h"
#include "octeon_ddoq.h"


typedef struct {
	cavium_wait_channel    wc;
	int                    cond;
	octeon_req_status_t     status;
} octeon_cntq_instr_completion_t;



extern uint32_t
cn56xx_pass2_core_clock(octeon_device_t  *oct);


void
print_cntq_instr(octeon_device_t *octeon_dev)
{
	int             i, cntq_no;
	octeon_cntq_t  *cntq;

	for (cntq_no = 0; cntq_no <= MAX_OCTEON_DMA_QUEUES; cntq_no++)  {
		cntq = (octeon_cntq_t *)octeon_dev->cntq[cntq_no];
		for ( i = 0; i < CNTQ_INSTR_TYPES; i++)   {
			if(cntq->instr[i]) {
				cavium_print(PRINT_DEBUG, "cntq %d\n dptraddr: 0x%p\n", cntq_no, cntq->instr[i]->dptr);
				cavium_print(PRINT_DEBUG, "rptraddr: 0x%p\n",
				             cntq->instr[i]->rptr);
				cavium_print(PRINT_DEBUG, "instr addr: 0x%p\n",
				             cntq->instr[i]);
			}
		} 
	}
}



void
cntq_print_pkt(octeon_cntq_t  *cntq,
               uint32_t        index,
               uint32_t        count)
{
  uint8_t *base = (uint8_t*)&cntq->base_addr[index];
  uint32_t i;

  cavium_print(PRINT_DEBUG, "\n\n----#   cntq_print_pkt #----\n");
  cavium_print(PRINT_DEBUG,"read_index: %d\n", cntq->host_read_index);
  cavium_print(PRINT_DEBUG,"Start Addr: 0x%p\n", base);
  cavium_print(PRINT_DEBUG,"count: 0x%x\n", count);
  for ( i = 0; i < count; i+= 8, base += 8) {
    /* The packet may wrap-around the end */
    if(base >= cntq->end)
      base = (uint8_t *)cntq->base_addr;
    cavium_print(PRINT_DEBUG,"%x %x %x %x %x %x %x %x\n", 
		 base[0], base[1], base[2], base[3],
		 base[4], base[5], base[6], base[7]);
  }
}




void
octeon_cntq_set_intr(octeon_device_t   *oct, octeon_cntq_t  *cntq)
{
	if(oct->chip_id <= OCTEON_CN58XX) {
		octeon_write_csr(oct, CN3XXX_DMA_PKT_INT_LEVEL(cntq->cntq_no),
		                 cntq->pkt_count_threshold);
		octeon_write_csr(oct, CN3XXX_DMA_TIME_INT_LEVEL(cntq->cntq_no),
		                 cntq->intr_time_threshold);
		cntq->dma_cnt_reg = (uint8_t *)oct->mmio[0].hw_addr
		                               + CN3XXX_DMA_CNT(cntq->cntq_no);
	}

	if(oct->chip_id == OCTEON_CN56XX) {
		octeon_write_csr(oct, CN56XX_DMA_PKT_INT_LEVEL(cntq->cntq_no),
		                 cntq->pkt_count_threshold);
		octeon_write_csr(oct, CN56XX_DMA_TIME_INT_LEVEL(cntq->cntq_no),
		                 cntq->intr_time_threshold);
		cntq->dma_cnt_reg = (uint8_t *)oct->mmio[0].hw_addr
		                               + CN56XX_DMA_CNT(cntq->cntq_no);
	}


	if( (oct->chip_id == OCTEON_CN63XX) || (oct->chip_id == OCTEON_CN66XX) ){
			uint64_t   intr_lvl = cntq->intr_time_threshold;

			intr_lvl <<= 32;
			intr_lvl |= cntq->pkt_count_threshold;

			/* The Interrupt Level register must be written using 64-bit writes. */
			octeon_write_csr64(oct, CN63XX_DMA_INT_LEVEL(cntq->cntq_no), intr_lvl);
			cntq->dma_cnt_reg = (uint8_t *)oct->mmio[0].hw_addr
			                               + CN63XX_DMA_CNT(cntq->cntq_no);
	}

	if(oct->chip_id == OCTEON_CN68XX) {
			uint64_t   intr_lvl = cntq->intr_time_threshold;

			intr_lvl <<= 32;
			intr_lvl |= cntq->pkt_count_threshold;

			/* The Interrupt Level register must be written using 64-bit writes. */
			octeon_write_csr64(oct, CN68XX_DMA_INT_LEVEL(cntq->cntq_no), intr_lvl);
			cntq->dma_cnt_reg = (uint8_t *)oct->mmio[0].hw_addr
			                               + CN68XX_DMA_CNT(cntq->cntq_no);
	}
}







void
octeon_cntq_stop_intr(octeon_device_t   *oct, octeon_cntq_t  *cntq)
{
	if(oct->chip_id <= OCTEON_CN58XX) {
		octeon_write_csr(oct, CN3XXX_DMA_PKT_INT_LEVEL(cntq->cntq_no), 0);
		octeon_write_csr(oct, CN3XXX_DMA_TIME_INT_LEVEL(cntq->cntq_no), 0);
	}

	if(oct->chip_id == OCTEON_CN56XX) {
		octeon_write_csr(oct, CN56XX_DMA_PKT_INT_LEVEL(cntq->cntq_no), 0);
		octeon_write_csr(oct, CN56XX_DMA_TIME_INT_LEVEL(cntq->cntq_no), 0);
	}

	if( (oct->chip_id == OCTEON_CN63XX) || (oct->chip_id == OCTEON_CN66XX) ) {
		octeon_write_csr(oct, CN63XX_DMA_PKT_INT_LEVEL(cntq->cntq_no), 0);
		octeon_write_csr(oct, CN63XX_DMA_TIME_INT_LEVEL(cntq->cntq_no), 0);
	}

	if(oct->chip_id == OCTEON_CN68XX) {
		octeon_write_csr(oct, CN68XX_DMA_PKT_INT_LEVEL(cntq->cntq_no), 0);
		octeon_write_csr(oct, CN68XX_DMA_TIME_INT_LEVEL(cntq->cntq_no), 0);
	}
}






static octeon_req_status_t 
cntq_send_instruction(octeon_device_t            *octeon_dev,
                      octeon_soft_instruction_t  *soft_instr)
{
	octeon_instr_status_t             retval;
	octeon_cntq_instr_completion_t    cntq_comp;

	soft_instr->req_info.timeout = 600;
	*(soft_instr->status_word)   = COMPLETION_WORD_INIT;
	cavium_init_wait_channel(&cntq_comp.wc);
	cntq_comp.cond   = 0;
	SET_SOFT_INSTR_CALLBACK_ARG(soft_instr, (void *)&cntq_comp);

	retval = octeon_process_instruction(octeon_dev, soft_instr, NULL);
	if(retval.s.error) {
		/* Soft instr is freed by process_instruction() on failure. */
		cavium_error("OCTCNTQ: CNTQ instruction could not be posted status: %x\n", retval.s.status);
		return 1;
	}
	/* Sleep on a wait queue till the cond flag indicates that the
	   response arrived or timed-out. */
	cavium_sleep_cond(&cntq_comp.wc, (int *)&cntq_comp.cond);
	return(cntq_comp.status);
}





static int
send_cntq_credit(octeon_device_t  *octeon_dev,
                 OCTEON_CNTQ      cntq_no,
                 uint32_t           count)
{
	octeon_cntq_t  *cntq = (octeon_cntq_t *)octeon_dev->cntq[cntq_no];
	oct_cntq_credit_instr_t    *cntq_credit;
	octeon_soft_instruction_t  *soft_instr;
	octeon_instr_status_t       retval;

	cntq->stats.credits_posted++;
	soft_instr = (octeon_soft_instruction_t *)
	             cavium_alloc_buffer(octeon_dev,
	                         OCT_SOFT_INSTR_SIZE + OCT_CNTQ_CREDIT_INSTR_SIZE);
	if(!soft_instr) {
		cavium_error("OCTCNTQ: Alloc failed for cntq[%d] credit\n",
		             cntq_no); 
		return 1;
	}
	cavium_memcpy(soft_instr,cntq->instr[CNTQ_CREDIT], OCT_SOFT_INSTR_SIZE);

	soft_instr->dptr = ((uint8_t *)soft_instr + OCT_SOFT_INSTR_SIZE);
	cntq_credit = (oct_cntq_credit_instr_t *)soft_instr->dptr;
	cntq_credit->s.q_no    = cntq_no;
	cntq_credit->s.q_size  = count >> 3;
	octeon_swap_8B_data(&cntq_credit->u64, 1);

	soft_instr->req_info.timeout = 600;
	SET_SOFT_INSTR_ALLOCFLAGS(soft_instr, OCTEON_SOFT_INSTR_ALLOCATED);
	retval = octeon_process_instruction(octeon_dev, soft_instr, NULL);
	if(retval.s.error) {
		cntq->stats.credits_failed++;
		cavium_error("OCTCNTQ: CNTQ[%d] CREDIT instruction failed status: %x\n",
		             cntq_no, retval.s.status);
		return 1;
	}
	return 0;
}







static inline void
copy_cntq_data(octeon_cntq_t *cntq,
               uint8_t       *recv_buffer,
               uint8_t       *cntq_data,
               uint32_t       data_length)
{
	cavium_print(PRINT_FLOW, "\n\n----#  copy_cntq_data #----\n");
	cavium_print(PRINT_DEBUG, "cntq_data: 0x%p, cntq->end: 0x%p len: %d\n", 
	             cntq_data, cntq->end, data_length);
	if(((unsigned long)cntq_data+data_length) <= (unsigned long)cntq->end) {
		cavium_memcpy(recv_buffer, cntq_data, data_length);
	} else {
		unsigned long  tlen;
		tlen = (unsigned long)cntq->end - (unsigned long)cntq_data;
		if(tlen)
			cavium_memcpy(recv_buffer, cntq_data, tlen);
		cavium_memcpy(recv_buffer+tlen, cntq->base_addr, data_length - tlen);
	}
}




/* Callback called by driver when a response arrives for a CNTQ instruction
 * or if the instruction timed-out.
 */
void  
cntq_callback(octeon_req_status_t status, void *arg)
{
	octeon_cntq_instr_completion_t   *cntq_comp = (octeon_cntq_instr_completion_t *)arg;

	cavium_print(PRINT_FLOW,"\n\n----#  cntq_callback #----\n");
	cavium_print(PRINT_DEBUG,"Status: %d, waitq: %p\n", status, arg);

	cntq_comp->status = status;
	cntq_comp->cond   = 1;
	cavium_wakeup(&cntq_comp->wc);
}





static uint32_t
octeon_cntq_fill_instr(octeon_cntq_t              *cntq,
                       octeon_soft_instruction_t  *soft_instr,
                       OCTEON_CNTQ_INSTR_CODE     cntq_instr)
{
	oct_cntq_init_instr_t        *cntq_init;
	oct_cntq_delete_instr_t      *cntq_delete;

	switch(cntq_instr) {
	case CNTQ_CREDIT:
		soft_instr->ih.dlengsz      = OCT_CNTQ_CREDIT_INSTR_SIZE;
		soft_instr->rptr            = NULL;
		soft_instr->irh.rlenssz     = 0;
		soft_instr->irh.opcode      = CNTQ_CREDIT_OP;
		SET_SOFT_INSTR_CALLBACK(soft_instr, NULL);
		SET_SOFT_INSTR_RESP_ORDER(soft_instr, OCTEON_RESP_NORESPONSE);
		break;

	case CNTQ_INIT:
		soft_instr->dptr = cavium_malloc_dma(OCT_CNTQ_INIT_INSTR_SIZE,
		                                     __CAVIUM_MEM_GENERAL);
		if(soft_instr->dptr == NULL)  {
			cavium_error("OCTCNTQ: Memory allocation failed %s:%d\n",
			             __CVM_FUNCTION__, __CVM_LINE__);
			return 1;
		}
		cntq_init = (oct_cntq_init_instr_t *)soft_instr->dptr;
		cavium_memset(cntq_init, 0, OCT_CNTQ_INIT_INSTR_SIZE);
		cntq_init->word0.s.q_no   = cntq->cntq_no;
		cntq_init->word0.s.q_size = cntq->size;
		cntq_init->base_addr      = cntq->base_addr_dma;
		soft_instr->ih.dlengsz    = OCT_CNTQ_INIT_INSTR_SIZE;

		soft_instr->rptr          = cavium_malloc_dma(16, __CAVIUM_MEM_GENERAL);
		if(soft_instr->rptr == NULL) {
			cavium_error("OCTCNTQ: Memory allocation failed %s:%d\n",
			              __CVM_FUNCTION__, __CVM_LINE__);
			cavium_free_dma(soft_instr->dptr);
			return 1;
		}
		soft_instr->status_word   = (uint64_t*)((uint8_t *)soft_instr->rptr + 8);
		soft_instr->irh.rlenssz   = 16 ;
		soft_instr->irh.opcode    = CNTQ_INIT_OP;
		SET_SOFT_INSTR_CALLBACK(soft_instr, cntq_callback);
		SET_SOFT_INSTR_RESP_ORDER(soft_instr, OCTEON_RESP_ORDERED);
		break;

	case CNTQ_DELETE:
		soft_instr->dptr = cavium_malloc_dma(OCT_CNTQ_DELETE_INSTR_SIZE,
		                                     __CAVIUM_MEM_GENERAL);
		if(soft_instr->dptr == NULL)  {
			cavium_error("OCTCNTQ: Memory allocation failed %s:%d\n",
			              __CVM_FUNCTION__, __CVM_LINE__);
			return 1;
		}
		cntq_delete = (oct_cntq_delete_instr_t *)soft_instr->dptr;
		cavium_memset(cntq_delete, 0, OCT_CNTQ_DELETE_INSTR_SIZE);
		cntq_delete->s.q_no        = cntq->cntq_no;
		soft_instr->ih.dlengsz     = OCT_CNTQ_DELETE_INSTR_SIZE;
		soft_instr->rptr          = cavium_malloc_dma(16, __CAVIUM_MEM_GENERAL);
		if(soft_instr->rptr == NULL) {
			cavium_error("OCTCNTQ: Memory allocation failed %s:%d\n",
			             __CVM_FUNCTION__, __CVM_LINE__);
			cavium_free_dma(soft_instr->dptr);
			return 1;
		}
		soft_instr->status_word  = (uint64_t*)((uint8_t *)soft_instr->rptr + 8);
		soft_instr->irh.rlenssz   = 16 ;
		soft_instr->irh.opcode    = CNTQ_DELETE_OP;
		SET_SOFT_INSTR_CALLBACK(soft_instr, cntq_callback);
		SET_SOFT_INSTR_RESP_ORDER(soft_instr, OCTEON_RESP_ORDERED);
		break;
		
	default:
		cavium_error("OCTCNTQ: Unknown CNTQ instruction: %x\n", cntq_instr);
		return 1;
	}
	return 0;
}
 






static uint32_t
octeon_cntq_create_instruction(octeon_device_t          *octeon_dev,
                               octeon_cntq_t            *cntq,
                               OCTEON_CNTQ_INSTR_CODE   cntq_instr)
{
	octeon_soft_instruction_t  *soft_instr = cntq->instr[cntq_instr];

	cavium_print(PRINT_FLOW,"\n\n--#cntq_create_instruction instr:%d #--\n",
	             cntq_instr);

	cavium_memset(soft_instr, 0, OCT_SOFT_INSTR_SIZE);
	soft_instr->ih.raw      = 1;
	soft_instr->ih.rs       = 1;
	soft_instr->ih.tag      = cntq->tag;
	soft_instr->ih.tagtype  = ATOMIC_TAG;
	soft_instr->ih.fsz      = 16;
	SET_SOFT_INSTR_IQ_NO(soft_instr, cntq->iq);
	SET_SOFT_INSTR_DMA_MODE(soft_instr, OCTEON_DMA_DIRECT);
	SET_SOFT_INSTR_TIMEOUT(soft_instr, 100);
	SET_SOFT_INSTR_OCTEONID(soft_instr, octeon_dev->octeon_id);
	return (octeon_cntq_fill_instr(cntq, soft_instr, cntq_instr));
}




/* Initialize the cntq structure in the host for cntq given by "cntq_no".
 * The configuration for this cntq is given by "cntq_conf". This routine
 * sets up the cntq structure in host according to "cntq_conf", allocates
 * memory for the cntq ring, selects the input queue for communication 
 * with the core, creates soft instructions for each cntq instruction and 
 * writes the cntq size and start address into Octeon registers.
 */
int
octeon_init_cntq(int  octeon_id, int    cntq_no,  void  *conf)
{
	octeon_cntq_config_t        *cntq_conf = (octeon_cntq_config_t *)conf;
	octeon_cntq_t               *cntq;
	octeon_device_t             *octeon_dev;
	octeon_soft_instruction_t   *soft_instr;
	octeon_req_status_t          status;
	uint32_t                     size;
	int                          i;

	cavium_print(PRINT_FLOW,"\n\n----#  octeon_init_cntq : %d#----\n",
	             cntq_no);

	/* Get the octeon device pointer */
	octeon_dev = get_octeon_device_ptr(octeon_id);
	if(!octeon_dev) {
		cavium_error("OCTCNTQ: CNTQ INIT Invalid Octeon id %d\n",
		             octeon_id);
		return ENODEV;
	}

	/* Check that the device is in a state to proceed */
	if( (cavium_atomic_read(&octeon_dev->status) < OCT_DEV_CORE_OK)
        || (cavium_atomic_read(&octeon_dev->status) > OCT_DEV_RUNNING)) {
		print_octeon_state_errormsg(octeon_dev);
		return EBUSY;
	}

	/* Check that the initialization if for a valid Counter Queue */
	if(cntq_no >= MAX_OCTEON_DMA_QUEUES) {
		cavium_error("OCTCNTQ: Invalid CNTQ id %d for init\n", cntq_no);
		return ENODEV;
	}

	cntq = (octeon_cntq_t *)cavium_malloc_dma(OCT_CNTQ_SIZE, __CAVIUM_MEM_GENERAL);
	if(!cntq) {
                cavium_error("OCTCNTQ: Memory allocation failed %s:%d\n",
		             __CVM_FUNCTION__, __CVM_LINE__);
                return ENOMEM;
	}
	cavium_memset(cntq, 0, OCT_CNTQ_SIZE);
	octeon_dev->cntq[cntq_no] = (void *)cntq;

	/* Allocate memory for Counter Queue. */
	size = ROUNDUP8(cntq_conf->size);
	cntq->base_addr = octeon_pci_alloc_consistent(octeon_dev->pci_dev, size, &cntq->base_addr_dma); 
	if(!cntq->base_addr) {
		cavium_error("OCTCNTQ: Memory allocation failed %s:%d\n",
		             __CVM_FUNCTION__, __CVM_LINE__);
		return ENOMEM;
	}
	cavium_memset(cntq->base_addr, 0, cntq_conf->size);

	/* Initialize the Octeon device Counter Queue structure. */
	cntq->cntq_no              = cntq_no;
	cntq->host_read_index      = 0;
	cntq->size                 = (size >> 3); /* size is in 8B blocks */
	cntq->end                  = (uint8_t *)cntq->base_addr + size;
	cntq->pkt_count_threshold  = cntq_conf->pkt_count_threshold;

	cntq->intr_time_threshold  = cntq_conf->intr_time_threshold * octeon_get_cycles_per_usec(octeon_dev);

	cntq->pkts_per_interrupt   = cntq_conf->pkts_per_interrupt;
	cntq->credit_threshold     = cntq_conf->credit_threshold;
	cavium_atomic_set(&cntq->pkts_pending, 0);

	cavium_print(PRINT_REGS,"cntq[%d] base : 0x%p end: 0x%p, size: %u\n",
	             cntq_no, cntq->base_addr, cntq->end, cntq->size);

	cntq->iq_mode = IQ_MODE_32;
	cntq->iq      = 0;
	/* The tag to be used for all instructions for this CNTQ */
	cavium_get_random_bytes(&cntq->tag, 4);

	/* Pre-allocate all instructions for the CNTQ */
	for(i = 0; i < CNTQ_INSTR_TYPES; i++) {
		cntq->instr[i] = cavium_malloc_dma(OCT_SOFT_INSTR_SIZE, __CAVIUM_MEM_GENERAL);
		if(cntq->instr[i] == NULL) {
			while (i-- > 0 ) {
				cavium_free_dma(cntq->instr[i]);
				cntq->instr[i] = NULL;
			}
			octeon_pci_free_consistent(octeon_dev->pci_dev, (cntq->size << 3),
			                           cntq->base_addr,  cntq->base_addr_dma);
			cntq->base_addr = NULL;
			cavium_error("OCTCNTQ: Memory allocation failed %s:%d\n",
			             __CVM_FUNCTION__, __CVM_LINE__);
			return ENOMEM;
		}
		if(octeon_cntq_create_instruction(octeon_dev, cntq, i)) {
			for(i = 0; i < CNTQ_INSTR_TYPES; i++) {
				cavium_free_dma(cntq->instr[i]);
				cntq->instr[i] = NULL;
			}
			octeon_pci_free_consistent(octeon_dev->pci_dev, (cntq->size << 3),
			                           cntq->base_addr,  cntq->base_addr_dma);
			cntq->base_addr = NULL;
			return ENOMEM;
		}
	}
	cavium_spin_lock_init(&(cntq->lock));

	soft_instr = cntq->instr[CNTQ_INIT];
	octeon_swap_8B_data(soft_instr->dptr, (OCT_CNTQ_INIT_INSTR_SIZE >> 3));

	status = cntq_send_instruction(octeon_dev, soft_instr);
	if(status)  {
		cavium_error("OCTCNTQ: Failed to post CNTQ INIT instruction. Status: 0x%x\n", status);
		octeon_delete_cntq(octeon_id, cntq->cntq_no);
		return ENOMEM;
	}

	cntq->initialized = CNTQ_ENABLE;

	octeon_cntq_set_intr(octeon_dev, cntq);
	return 0;
}







int
octeon_change_cntq_status(int octeon_id, int status)
{
	octeon_device_t  *octeon_dev;
	octeon_cntq_t    *cntq;
	int               i;

	octeon_dev = get_octeon_device_ptr(octeon_id);
	if(!octeon_dev) {
		cavium_error("OCTCNTQ: Invalid Octeon id %d for status change\n",
		             octeon_id);
		return ENODEV;
	} 

	for( i = 0 ; i < MAX_OCTEON_CNTQ; i++) {
		cntq = (octeon_cntq_t *)octeon_dev->cntq[i];
		if(!cntq) {
			cavium_error("OCTCNTQ: CNTQ[%d] not found for status change\n",i);
			return ENODEV;
		}
		cntq->initialized = status;
	}
	return 0;
}





int
octeon_get_cntq_status(int octeon_id, int cntq_no)
{
	octeon_device_t   *octeon_dev;
	octeon_cntq_t     *cntq;

	octeon_dev = get_octeon_device_ptr(octeon_id);
	if(!octeon_dev) {
		cavium_error("OCTCNTQ: Invalid Octeon id %d for get status\n",
		             octeon_id);
		return ENODEV;
	}

	cntq = (octeon_cntq_t *)octeon_dev->cntq[cntq_no];
	if(!cntq) {
		cavium_error("OCTCNTQ: CNTQ[%d] not found for get status\n",cntq_no);
		return ENODEV;
	}
	return cntq->initialized;
}






int 
octeon_delete_cntq(int   octeon_id,
                   int   cntq_no)
{
	int i;
	octeon_device_t            *octeon_dev;
	octeon_cntq_t              *cntq;
	octeon_soft_instruction_t  *soft_instr;
	octeon_req_status_t         status;

	octeon_dev = get_octeon_device_ptr(octeon_id);
	if(!octeon_dev) {
		cavium_error("OCTCNTQ: Invalid Octeon id %d in CNTQ delete\n",
		             octeon_id);
		return ENODEV;
	}

	if (cntq_no > MAX_OCTEON_DMA_QUEUES) {
		cavium_error("OCTCNTQ: Invalid CNTQ id %d for delete\n",cntq_no);
		return ENODEV;
	}

	cntq = (octeon_cntq_t *)octeon_dev->cntq[cntq_no];
	if(!cntq) {
		cavium_error("OCTCNTQ: Invalid CNTQ id %d for delete\n",cntq_no);
		return ENODEV;
	}
	
	soft_instr = cntq->instr[CNTQ_DELETE];
	octeon_swap_8B_data(soft_instr->dptr,(OCT_CNTQ_DELETE_INSTR_SIZE >> 3));

	status = cntq_send_instruction(octeon_dev, soft_instr);
	if(status)  {
		cavium_error("OCTCNTQ: CNTQ DELETE failed. Status: 0x%x\n", status);
		return ENOMEM;
	}

	octeon_cntq_stop_intr(octeon_dev, cntq);

	/* Free the CNTQ ring. */
	if(cntq->base_addr) {
		octeon_pci_free_consistent(octeon_dev->pci_dev, (cntq->size << 3), cntq->base_addr,  cntq->base_addr_dma);
	}
 
	/* Free the soft instructions created for this CNTQ. */
	for ( i = 0; i < CNTQ_INSTR_TYPES; i++)   {
		if(cntq->instr[i]) {
			if((cntq->instr[i])->dptr)
				cavium_free_dma((cntq->instr[i])->dptr); 
			if((cntq->instr[i])->rptr)
				cavium_free_dma((cntq->instr[i])->rptr);
			cavium_free_dma(cntq->instr[i]);
		}
	}
	cavium_free_dma(cntq);
	octeon_dev->cntq[cntq_no] = NULL;	
	return 0;
}







static int
octeon_cntq_dispatch_packet(octeon_device_t     *octeon_dev,
                            octeon_cntq_t       *cntq,
                            octeon_resp_hdr_t   *resp_hdr,
                            uint8_t             *data,
                            uint32_t             data_length)
{
	octeon_recv_info_t    *recv_info;
	octeon_recv_pkt_t     *recv_pkt;
	octeon_dispatch_fn_t   dispatch_fn;

	dispatch_fn = octeon_get_dispatch(octeon_dev,(uint16_t)resp_hdr->opcode);
	if(dispatch_fn) {
		recv_info = octeon_alloc_recv_info(0);
		if(recv_info == NULL) {
			cavium_error("OCTCNTQ: Allocation failed, %s:%d\n",
			             __CVM_FUNCTION__, __CVM_LINE__);
			cntq->stats.dropped_nomem++;
			return 1;
		}
		recv_pkt = recv_info->recv_pkt;
		cavium_memset(recv_pkt, 0, OCT_RECV_PKT_SIZE);
		/* CNTQ should be able to accept 0 len pkt,since the resp hdr
		   itself may carry some info. */
		if(data_length) {
			recv_pkt->buffer_ptr[0] = cavium_malloc_dma(data_length, __CAVIUM_MEM_ATOMIC);
			if(recv_pkt->buffer_ptr[0] == NULL) {
				cavium_error("OCTCNTQ:Allocation failed %s:%d\n",
				             __CVM_FUNCTION__, __CVM_LINE__);
				cavium_free_dma(recv_info);
				cntq->stats.dropped_nomem++;
				return 1;
			}
			recv_pkt->buffer_count = 1;
			copy_cntq_data(cntq, recv_pkt->buffer_ptr[0], data, data_length);
			recv_pkt->buffer_size[0] = data_length;
		}
		recv_pkt->resp_hdr   = *resp_hdr;
		recv_pkt->length     = data_length;
		recv_pkt->octeon_id  = (uint16_t)octeon_dev->octeon_id;
		recv_pkt->buf_type   = OCT_RECV_BUF_TYPE_1;
		dispatch_fn(recv_info, octeon_get_dispatch_arg(octeon_dev,(uint16_t)resp_hdr->opcode));
	} else {
		cntq->stats.dropped_nodispatch++;
	}
	return 0;
}









static inline void
octeon_cntq_process_ddoq_ind(octeon_device_t   *octeon_dev,
                             uint32_t           ddoq_id,
                             uint32_t           request_id,
                             char              *data)
{
	octeon_ddoq_t     *ddoq = (octeon_ddoq_t *)get_ddoq_ptr(octeon_dev, ddoq_id);
	octeon_ddoq_ind_t *ddoq_ind = (octeon_ddoq_ind_t *)data;

	/* Not checking the state anymore; its being done in process_ddoq. */
	if(ddoq) {
		if(((ddoq->next_pkt_cntr & 0xffff) == request_id)) {
			octeon_swap_8B_data((uint64_t *)ddoq_ind, 1);
			octeon_process_ddoq(octeon_dev, ddoq, ddoq_ind->s.pkt_cnt);
		} else {
			cavium_error("ddoq[%d] expected pkt no. 0x%x received: 0x%x\n",
			             ddoq_id, (uint32_t)(ddoq->next_pkt_cntr&0xffff),
			             request_id);
		}
	}
}





 /* This routine is called in softirq context 
  * CNTQ packet processing happens here. For every packet, if a dispatch
  * function exists for the opcode in the packet's resp_hdr, a recv_pkt
  * is created, the packet contents copied from cntq ring to recv_pkt
  * buffers and the dispatch fn is called with the recv_pkt as an arg.
  * When all packets have been processed, the space in cntq ring used
  * by the processed packets is made available to Octeon. If the number
  * of words available for credit exceeds a threshold, a credit 
  * instruction is sent to Octeon.
  */
int
octeon_process_cntq_pkts(octeon_device_t  *octeon_dev,
                         octeon_cntq_t    *cntq)
{
	octeon_resp_hdr_t     *resp_hdr;
	uint8_t               *data;
	uint64_t              *pkt_start, *cntq_base;
	uint32_t               pkt_length, data_length, pkt, cntq_pkts;
	uint32_t               rd_idx, bytes_processed = 0;
#ifdef USE_DDOQ_THREADS
	int     i, is_pkts = 0;
	int     act_threads = 0;
#endif

	cavium_print(PRINT_FLOW,"\n\n--# octeon_process_cntq %d #--\n",
	             cntq->cntq_no);
	cntq_base = cntq->base_addr;
	rd_idx    = cntq->host_read_index;

	cntq_pkts = cavium_atomic_read(&cntq->pkts_pending);
	if(cntq_pkts > cntq->pkts_per_interrupt)
		cntq_pkts = cntq->pkts_per_interrupt;

	for ( pkt = 0;  pkt < cntq_pkts; pkt++) {

		pkt_start = (uint64_t *)&(cntq_base[rd_idx]);
		octeon_swap_8B_data(pkt_start, 1);

		data_length = (uint32_t)(*pkt_start);
		if(!data_length)  {
			cavium_error("OCTCNTQ: Received CNTQ pkt with len: 0\n");
			break;
		}

		/* Octeon always pads packet data to a 8B boundary. */
		/* 16 bytes for Response Header and Length field of the packet*/
		pkt_length =  ROUNDUP8(data_length) + 16;

		INCR_INDEX_BY1(rd_idx, cntq->size);
		resp_hdr = (octeon_resp_hdr_t *)&(cntq_base[rd_idx]);
		octeon_swap_8B_data((uint64_t *)resp_hdr, 1);

		INCR_INDEX_BY1(rd_idx, cntq->size);
		data = (uint8_t *)&(cntq_base[rd_idx]);

		if(resp_hdr->opcode == DDOQ_PKT_OP) {
#ifdef USE_DDOQ_THREADS
			octeon_ddoq_ind_t *ddoq_ind = (octeon_ddoq_ind_t *)data;
			int th_id = resp_hdr->dest_qport % cvm_ddoq_num_threads;
			cvm_ddoq_thread_t *thr = &(octeon_dev->ddoq_thread[th_id]);

			if (thr->th_write_idx == thr->th_read_idx) {
				printk ("%s no space to process this packet\n", __FUNCTION__);
				cntq_pkts = pkt;
				DECR_INDEX(rd_idx, 2, cntq->size);
				break;
			}
			is_pkts += 1;
			act_threads |= (1 << th_id);
			thr->th_info[thr->th_write_idx].req_id = resp_hdr->request_id;
			thr->th_info[thr->th_write_idx].ddoq_id = resp_hdr->dest_qport;
			octeon_swap_8B_data((uint64_t *)ddoq_ind, 1);
			thr->th_info[thr->th_write_idx].num_pkts = ddoq_ind->s.pkt_cnt;
			thr->th_write_idx = (thr->th_write_idx + 1) % CVM_DDOQ_MAX_THREAD_PKTS;
			cavium_atomic_inc(&thr->ddoq_pkts_queued);
#else
			octeon_cntq_process_ddoq_ind(octeon_dev, resp_hdr->dest_qport, resp_hdr->request_id, data);
#endif
		} else {
			octeon_cntq_dispatch_packet(octeon_dev, cntq, resp_hdr, data, data_length); 
		}

		/* The read_index was already moved to read the resp_hdr and length.
		   We now only need to move it across the data for this packet */
		INCR_INDEX(rd_idx, ((ROUNDUP8(data_length))>>3),cntq->size);
		bytes_processed += pkt_length ; 
	}
	cntq->host_read_index       = rd_idx;
	cntq->stats.pkts_received  += pkt;
	cntq->stats.bytes_received += bytes_processed;
	cntq->credit_store         += bytes_processed;

	cavium_atomic_sub(cntq_pkts, &cntq->pkts_pending);
	if(cntq->credit_store >= cntq->credit_threshold)  {
		cavium_print(PRINT_FLOW,"CNTQ[%d]: Sending credit for %d bytes\n", cntq->cntq_no, cntq->credit_store);
		if(send_cntq_credit(octeon_dev, cntq->cntq_no, cntq->credit_store)) {
			cavium_error("OCTCNTQ: Failed to send CNTQ CREDIT instruction\n"); 
		} else {
			cntq->credit_store = 0;
		}
	}
#ifdef USE_DDOQ_THREADS
	if (is_pkts) {
		for (i = 0; i < cvm_ddoq_num_threads; i++)
			if ((act_threads & (1 << i)))
				cavium_wakeup(&octeon_dev->ddoq_thread[i].wc);
	}
#endif
	return pkt;
}






/* Bottom half processing for DMA Counter Queues. */
void
octeon_cntq_bh(unsigned long pdev)
{
	octeon_cntq_t    *cntq;
	uint32_t            q_no, pkts_pending=0, pkts_processed=0;
	octeon_device_t  *octeon_dev = (octeon_device_t *)pdev;

	/* Call ordered list processing here to ensure that response for any
           DDOQ related operation is delivered to host. */
	process_ordered_list(octeon_dev);
	octeon_dev->stats.cntq_tasklet_count++;

	for(q_no = 0; q_no < MAX_OCTEON_DMA_QUEUES; q_no++)  {
		cntq = (octeon_cntq_t *)octeon_dev->cntq[q_no];
		pkts_pending = cavium_atomic_read(&cntq->pkts_pending);

		if(pkts_pending) {
			cavium_spin_lock(&cntq->lock);
			pkts_processed = octeon_process_cntq_pkts(octeon_dev, cntq);
			cavium_spin_unlock(&cntq->lock);
		}

		if(pkts_processed < pkts_pending)  {
			process_ordered_list(octeon_dev);
			cavium_tasklet_schedule(&octeon_dev->cntq_tasklet);
		}
	}
}






/* Interrupt Handler for DMA Counter Queue interrupts. 
   Checks the DMA queue packet count registers and atomically
   updates internal packet count. Actual packet handling happens
   in bottom half.
*/
int
octeon_cntq_intr_handler(void       *dev,
                         uint64_t    intr)
{
	octeon_cntq_t    *cntq;
	octeon_device_t  *octeon_dev = (octeon_device_t *)dev;
	uint32_t          q_no, pkt_count, schedule=0;
	uint64_t          cntq_mask;

	if(octeon_dev->chip_id <= OCTEON_CN58XX)
		cntq_mask = PCI_INT_DMA0_MASK;

	if(octeon_dev->chip_id == OCTEON_CN56XX)
		cntq_mask = CN56XX_INTR_DMA0_DATA;

	if( (octeon_dev->chip_id == OCTEON_CN63XX) || (octeon_dev->chip_id == OCTEON_CN66XX) )
		cntq_mask = CN63XX_INTR_DMA0_DATA;

	if(octeon_dev->chip_id == OCTEON_CN68XX)
		cntq_mask = CN68XX_INTR_DMA0_DATA;

	for(q_no = 0; q_no < MAX_OCTEON_DMA_QUEUES; q_no++, cntq_mask <<= 1)  {
		if(intr & cntq_mask) {
			cntq = (octeon_cntq_t *)octeon_dev->cntq[q_no];
			pkt_count = OCTEON_READ32(cntq->dma_cnt_reg);
			if(pkt_count) {
				cavium_atomic_add(pkt_count, &cntq->pkts_pending);
				OCTEON_WRITE32(cntq->dma_cnt_reg, pkt_count);
				schedule=1;
			} else
				cavium_error("OCTCNTQ: Interrupt 0x%llx with no CNTQ[%d] packets\n", CVM_CAST64(intr), q_no);
		}
	}
	if(schedule)
		cavium_tasklet_schedule(&octeon_dev->cntq_tasklet);
	return 0;
}





int
octeon_disable_cntq_ops(int   octeon_id)
{
	octeon_device_t   *octeon_dev = get_octeon_device_ptr(octeon_id);

	if(octeon_dev == NULL) {
		cavium_error("OCTCNTQ: Unknown Octeon device %d in %s\n",
		             octeon_id, __CVM_FUNCTION__);
		return ENODEV;
	}
	octeon_dev->dma_ops.intr_handler = NULL; 
	octeon_dev->dma_ops.read_stats   = NULL;
	octeon_dev->dma_ops.read_statsb  = NULL;
	octeon_dev->cntq_ready           = 1;
	return 0;
}





int
octeon_enable_cntq_ops(int   octeon_id, octeon_dma_ops_t  *dma_ops)
{
	octeon_device_t   *octeon_dev = get_octeon_device_ptr(octeon_id);

	if(octeon_dev == NULL) {
		cavium_error("OCTCNTQ: Unknown Octeon device %d in %s\n",
		              octeon_id, __CVM_FUNCTION__);
		return ENODEV;
	}
	cavium_tasklet_init(&octeon_dev->cntq_tasklet, octeon_cntq_bh, (unsigned long)octeon_dev);
	octeon_dev->dma_ops.intr_handler = octeon_cntq_intr_handler; 
	octeon_dev->dma_ops.read_stats   = dma_ops->read_stats;
	octeon_dev->dma_ops.read_statsb  = dma_ops->read_statsb;
	octeon_dev->cntq_ready           = 1;
	return 0;
}







int
octeon_cntq_get_stats(int octeon_id, int cntq_no, oct_cntq_stats_t  *cntq_stats)
{
	octeon_device_t   *octeon_dev = get_octeon_device_ptr(octeon_id);
	octeon_cntq_t     *cntq = NULL;

	if(octeon_dev == NULL) {
		cavium_error("OCTCNTQ: %s: No Octeon device\n", __CVM_FUNCTION__);
		return ENODEV;
	}
	if(cntq_no > MAX_OCTEON_DMA_QUEUES) {
		cavium_error("OCTCNTQ: %s: No CNTQ device\n", __CVM_FUNCTION__);
		return ENODEV;
	}
	cntq = (octeon_cntq_t *)octeon_dev->cntq[cntq_no];
	cavium_memcpy(cntq_stats, &cntq->stats, sizeof(oct_cntq_stats_t));
	return 0;
}




/* $Id: octeon_cntq.c 67088 2011-11-15 19:39:17Z mchalla $ */
