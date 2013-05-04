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
#include "octeon_debug.h"
#include "octeon_macros.h"
#include "octeon_hw.h"


//#define PERF_MODE




uint32_t
octeon_droq_refill(octeon_device_t  *octeon_dev, octeon_droq_t   *droq);

oct_poll_fn_status_t
check_droq_refill(void  *octptr, unsigned long q_no);


#ifdef  USE_DROQ_THREADS
static int oct_droq_thread(void* arg);
#endif

#if defined(ENABLE_PCIE_2G4G_FIX)
extern uint8_t *__get_droq_buffer_block(octeon_device_t  *, octeon_droq_t  *);
extern uint8_t *__get_droq_info_block(octeon_device_t  *, octeon_droq_t  *);
extern void __free_droq_buffer_block(octeon_droq_t  *);
extern void __free_droq_info_block(octeon_droq_t  *);
#endif


struct niclist {
	cavium_list_t   list;
	void           *ptr;
};


struct __dispatch {
	cavium_list_t          list;
	octeon_recv_info_t    *rinfo;
	octeon_dispatch_fn_t   disp_fn;
};





	

int
octeon_droq_check_hw_for_pkts(octeon_device_t  *oct, octeon_droq_t  *droq)
{
	int pkt_count = 0;

	pkt_count = OCTEON_READ32(droq->pkts_sent_reg);
	if(pkt_count) {
		cavium_atomic_add(pkt_count, &droq->pkts_pending);
		if(oct->chip_id < OCTEON_CN56XX)
			OCTEON_WRITE32(droq->pkts_credit_reg, (pkt_count << 16));
		else
			OCTEON_WRITE32(droq->pkts_sent_reg, pkt_count);
	}

	return pkt_count;
}






void
oct_dump_droq_state(octeon_droq_t   *oq)
{

	cavium_print_msg("DROQ[%d] state dump\n", oq->q_no);
	cavium_print_msg("Attr: Size: %u Pkts/intr: %u  refillafter: %u\n",
		oq->max_count, oq->pkts_per_intr, oq->refill_threshold);
	cavium_print_msg("Attr: fastpath: %s poll_mode: %s drop_on_max: %s fptr: %p napi_fn: %p\n",
		 (oq->fastpath_on)?"ON":"OFF",
		 (oq->ops.poll_mode)?"ON":"OFF",
		 (oq->ops.drop_on_max)?"ON":"OFF", oq->ops.fptr, oq->ops.napi_fn);

	cavium_print_msg("idx:  read: %u write: %u  refill: %u\n",
		oq->host_read_index, oq->octeon_write_index, oq->host_refill_index);

	cavium_print_msg("Pkts: pending: %u forrefill: %u\n",
		cavium_atomic_read(&oq->pkts_pending), oq->refill_count);

	cavium_print_msg("Stats: PktsRcvd: %llu BytesRcvd: %llu\n",
		oq->stats.pkts_received, oq->stats.bytes_received);
	cavium_print_msg("Stats: Dropped: Nodispatch: %llu NoMem: %llu TooMany: %llu\n",
		oq->stats.dropped_nodispatch, oq->stats.dropped_nomem,
		oq->stats.dropped_toomany);
}





static void
octeon_droq_compute_max_packet_bufs(octeon_droq_t  *droq)
{
	uint32_t    count=0;

	/* max_empty_descs is the max. no. of descs that can have no buffers.
	   If the empty desc count goes beyond this value, we cannot safely 
	   read in a 64K packet sent by Octeon (64K is max pkt size from Octeon) */
	droq->max_empty_descs = 0;

	do {
			droq->max_empty_descs++;
			count += droq->buffer_size;
	} while (count < (64 * 1024));

	droq->max_empty_descs = droq->max_count - droq->max_empty_descs;
}




static void
octeon_droq_reset_indices(octeon_droq_t  *droq)
{
	droq->host_read_index     = 0;
	droq->octeon_write_index  = 0;
	droq->host_refill_index   = 0;
	droq->refill_count        = 0;
	cavium_atomic_set(&droq->pkts_pending, 0);
}






static void
octeon_droq_destroy_ring_buffers(octeon_device_t  *oct, octeon_droq_t  *droq)
{
	int  i;

	for(i = 0; i < droq->max_count; i++)  {
		if(droq->recv_buf_list[i].buffer) {
			if(droq->desc_ring) {
				octeon_pci_unmap_single(oct->pci_dev, (unsigned long)droq->desc_ring[i].info_ptr, OCT_DROQ_INFO_SIZE, CAVIUM_PCI_DMA_FROMDEVICE);
				octeon_pci_unmap_single(oct->pci_dev, (unsigned long)droq->desc_ring[i].buffer_ptr, droq->buffer_size, CAVIUM_PCI_DMA_FROMDEVICE );
			}
#if !defined(ENABLE_PCIE_2G4G_FIX)
			free_recv_buffer(droq->recv_buf_list[i].buffer);
#endif
		}
	}

#if defined(ENABLE_PCIE_2G4G_FIX)
	if(droq->buffer_block)
		__free_droq_buffer_block(droq);
#endif

	octeon_droq_reset_indices(droq);
}






static int
octeon_droq_setup_ring_buffers(octeon_device_t  *oct, octeon_droq_t  *droq)
{
	int                  i;
	void                *buf;
	octeon_droq_desc_t  *desc_ring = droq->desc_ring;

#if defined(ENABLE_PCIE_2G4G_FIX)
	buf = __get_droq_buffer_block(oct, droq);
	if(buf == NULL) {
		cavium_error("OCTEON[%d]: DROQ big buffer alloc failed\n", oct->octeon_id);
		return -ENOMEM;
	}

	droq->buffer_block = buf;
#endif

	for(i = 0; i < droq->max_count; i++)  {
#if defined(ENABLE_PCIE_2G4G_FIX)
		droq->recv_buf_list[i].buffer  = buf + (i * droq->buffer_size);
		droq->recv_buf_list[i].data    = droq->recv_buf_list[i].buffer;
#else
		buf = get_new_recv_buffer(droq->buffer_size);
		if(!buf) {
			cavium_error("%s buffer alloc failed\n", __CVM_FUNCTION__);
			return -ENOMEM;
		}

		droq->recv_buf_list[i].buffer  = buf;
		droq->recv_buf_list[i].data    = get_recv_buffer_data(buf);
#endif

		droq->info_list[i].length = 0;

		desc_ring[i].info_ptr   = (uint64_t)octeon_pci_map_single(oct->pci_dev, &droq->info_list[i], OCT_DROQ_INFO_SIZE, CAVIUM_PCI_DMA_FROMDEVICE);

#if defined(ETHERPCI)
		/* infoptr bytes = 0 doesnt work in 56xx. To workaround that, we copy
		   8 bytes of data into infoptr for all Octeon devices. We'll copy it
		   back into data buffer during pkt processing. */
		droq->recv_buf_list[i].data += 8;
#endif
		desc_ring[i].buffer_ptr = (uint64_t)octeon_pci_map_single(oct->pci_dev, droq->recv_buf_list[i].data, droq->buffer_size, CAVIUM_PCI_DMA_FROMDEVICE );
   }

	octeon_droq_reset_indices(droq);

	octeon_droq_compute_max_packet_bufs(droq);

	return 0;
}











int 
octeon_delete_droq(octeon_device_t      *oct,
                   uint32_t              q_no)
{
	octeon_droq_t   *droq = oct->droq[q_no];

	cavium_print(PRINT_FLOW, "\n\n---#  octeon_delete_droq[%d]  #---\n", q_no);

#ifdef  USE_DROQ_THREADS

	if(CVM_KTHREAD_EXISTS(&droq->thread)) {
		cavium_print_msg("Killing DROQ[%d] thread\n", q_no);

		cavium_atomic_inc(&droq->pkts_pending);
		droq->stop_thread = 1;
		cavium_flush_write();

		cavium_kthread_destroy(&droq->thread);

		/* Wait till the droq thread has come out of its loop. */
		while(cavium_atomic_read(&droq->thread_active))
			cavium_sleep_timeout(1);
	}

#endif


	octeon_unregister_poll_fn(oct->octeon_id, check_droq_refill, q_no);

	octeon_droq_destroy_ring_buffers(oct, droq);

	if(droq->recv_buf_list)
		cavium_free_virt(droq->recv_buf_list);

#if defined(ENABLE_PCIE_2G4G_FIX)
	__free_droq_info_block(droq);
#else
	if(droq->info_base_addr)
		cavium_free_pages(droq->info_base_addr, cavium_get_order(droq->info_alloc_size));
#endif

	if(droq->desc_ring)
		octeon_pci_free_consistent(oct->pci_dev, (droq->max_count * OCT_DROQ_DESC_SIZE), droq->desc_ring, droq->desc_ring_dma);

	cavium_memset(droq, 0, OCT_DROQ_SIZE);

	return 0;
}










int 
octeon_init_droq(octeon_device_t       *oct,
                 uint32_t               q_no)
{
	octeon_droq_t     *droq;
	uint32_t           desc_ring_size=0;
	octeon_poll_ops_t  poll_ops;
	uint32_t           c_num_descs = 0, c_buf_size = 0, c_pkts_per_intr = 0, c_refill_threshold = 0;


	cavium_print(PRINT_FLOW,"\n\n----# octeon_init_droq #----\n");
	cavium_print(PRINT_DEBUG,"q_no: %d\n", q_no);

	droq = oct->droq[q_no];
	cavium_memset(droq, 0, OCT_DROQ_SIZE);

	droq->oct_dev = oct;
	droq->q_no    = q_no;

	if(oct->chip_id <= OCTEON_CN58XX) {
		cn3xxx_config_t   *conf3x = CHIP_FIELD(oct, cn3xxx, conf);
		c_num_descs         = conf3x->oq[q_no].num_descs;
		c_buf_size          = conf3x->oq[q_no].buf_size;
		c_pkts_per_intr     = conf3x->oq[q_no].pkts_per_intr;
		c_refill_threshold  = conf3x->oq[q_no].refill_threshold;
	}

	if(oct->chip_id == OCTEON_CN56XX) {
		cn56xx_config_t   *conf56 = CHIP_FIELD(oct, cn56xx, conf);
		c_num_descs         = conf56->oq[q_no].num_descs;
		c_buf_size          = conf56->oq_buf_size;
		c_pkts_per_intr     = conf56->oq[q_no].pkts_per_intr;
		c_refill_threshold  = conf56->oq[q_no].refill_threshold;
	}

	if(oct->chip_id == OCTEON_CN63XX) {
		cn63xx_config_t   *conf63 = CHIP_FIELD(oct, cn63xx, conf);
		conf63 = CHIP_FIELD(oct, cn63xx, conf);
		c_num_descs         = conf63->oq[q_no].num_descs;
		c_buf_size          = conf63->oq[q_no].buf_size;
		c_pkts_per_intr     = conf63->oq[q_no].pkts_per_intr;
		c_refill_threshold  = conf63->oq[q_no].refill_threshold;
	}

	if(oct->chip_id == OCTEON_CN66XX) {
		cn66xx_config_t   *conf66 = CHIP_FIELD(oct, cn66xx, conf);
		conf66 = CHIP_FIELD(oct, cn66xx, conf);
		c_num_descs         = conf66->oq[q_no].num_descs;
		c_buf_size          = conf66->oq[q_no].buf_size;
		c_pkts_per_intr     = conf66->oq[q_no].pkts_per_intr;
		c_refill_threshold  = conf66->oq[q_no].refill_threshold;
	}

	if(oct->chip_id == OCTEON_CN68XX) {
		cn68xx_config_t   *conf68 = CHIP_FIELD(oct, cn68xx, conf);
		conf68 = CHIP_FIELD(oct, cn68xx, conf);
		c_num_descs         = conf68->oq[q_no].num_descs;
		c_buf_size          = conf68->oq[q_no].buf_size;
		c_pkts_per_intr     = conf68->oq[q_no].pkts_per_intr;
		c_refill_threshold  = conf68->oq[q_no].refill_threshold;
	}


	droq->max_count   = c_num_descs;
	droq->buffer_size = c_buf_size;

	desc_ring_size  = droq->max_count * OCT_DROQ_DESC_SIZE;
	droq->desc_ring = octeon_pci_alloc_consistent(oct->pci_dev, desc_ring_size,
	                                              &droq->desc_ring_dma);

	if(!droq->desc_ring)  {
		cavium_error("OCTEON: Output queue %d ring alloc failed\n", q_no);
		return 1;
	}

	cavium_print(PRINT_REGS, "droq[%d]: desc_ring: virt: 0x%p, dma: %lx",
			q_no, droq->desc_ring, droq->desc_ring_dma);
	cavium_print(PRINT_REGS, "droq[%d]: num_desc: %d",
			q_no, droq->max_count);


#if defined(ENABLE_PCIE_2G4G_FIX)
	droq->info_list = (octeon_droq_info_t *)__get_droq_info_block(oct, droq);
#else
	droq->info_list =
	cavium_alloc_aligned_memory((droq->max_count * OCT_DROQ_INFO_SIZE),
	                            &droq->info_alloc_size, &droq->info_base_addr);
#endif

	if(!droq->info_list) {
		cavium_error("OCTEON: Cannot allocate memory for info list.\n");
		octeon_pci_free_consistent(oct->pci_dev,
		                           (droq->max_count * OCT_DROQ_DESC_SIZE),
		                           droq->desc_ring,
		                           droq->desc_ring_dma);
		return 1;
	}
	cavium_print(PRINT_DEBUG,"setup_droq: droq_info: 0x%p\n", droq->info_list);

	droq->recv_buf_list = (octeon_recv_buffer_t *)
	                cavium_alloc_virt(droq->max_count * OCT_DROQ_RECVBUF_SIZE);
	if(!droq->recv_buf_list)  {
		cavium_error("OCTEON: Output queue recv buf list alloc failed\n");
		goto init_droq_fail;
	}
	cavium_print(PRINT_DEBUG,"setup_droq: recv_buf_list: 0x%p\n",
	             droq->recv_buf_list); 

	if(octeon_droq_setup_ring_buffers(oct, droq)) {
		goto init_droq_fail;
	}

	droq->pkts_per_intr       = c_pkts_per_intr;
	droq->refill_threshold    = c_refill_threshold;

	cavium_print(PRINT_DEBUG,"DROQ INIT: max_empty_descs: %d\n",
			droq->max_empty_descs);


	cavium_spin_lock_init(&droq->lock);

	CAVIUM_INIT_LIST_HEAD(&droq->dispatch_list);


	/* For 56xx Pass1, this function won't be called, so no checks. */
	oct->fn_list.setup_oq_regs(oct, q_no);

	oct->io_qmask.oq |= (1 << q_no);

	poll_ops.fn     = check_droq_refill;
	poll_ops.fn_arg = (unsigned long)q_no;
	poll_ops.ticks  = 1;
	strcpy(poll_ops.name, "DROQ Refill");
	poll_ops.rsvd   = 0xff;
	octeon_register_poll_fn(oct->octeon_id, &poll_ops);


#ifdef  USE_DROQ_THREADS
	cavium_init_wait_channel(&droq->wc);
	cavium_kthread_setup(&droq->thread, oct_droq_thread, droq,
	                     "Oct DROQ Thread", 0);
	if(cavium_kthread_create(&droq->thread))
		goto init_droq_fail;
	cavium_kthread_set_cpu_affinity(&droq->thread,
	                                (3-(droq->q_no % cavium_get_cpu_count())) );
	cavium_kthread_run(&droq->thread);
#endif

	return 0;


init_droq_fail:
	octeon_delete_droq(oct, q_no);
	return 1;
}









int
octeon_shutdown_output_queue(octeon_device_t  *oct, int q_no)
{
	octeon_droq_t  *droq = oct->droq[q_no];
	volatile uint32_t *resp;
	uint32_t  *respbuf = cavium_malloc_dma(4, GFP_ATOMIC), loop_count=100;
	int             retval = 0, pkt_count=0;

	if(respbuf == NULL) {
		cavium_error("%s buffer alloc failed\n", __CVM_FUNCTION__);
		return -ENOMEM;
	}
	resp = (volatile uint32_t *)respbuf;

	*resp = 0xFFFFFFFF;

	/* Send a command to Octeon to stop further packet processing */
	if(octeon_send_short_command(oct, DEVICE_STOP_OP,
	                             (q_no << 8 | DEVICE_PKO), respbuf, 4)) {
		cavium_error("%s command failed\n", __CVM_FUNCTION__);
		retval = -EINVAL;
		goto shutdown_oq_done;
	}

	/* Wait for response from Octeon. */
	while((*resp == 0xFFFFFFFF) && (loop_count--)) {
		cavium_sleep_timeout(1);
	}

	if(*resp != 0) {
		cavium_error("%s command failed: %s\n", __CVM_FUNCTION__,
		             (*resp == 0xFFFFFFFF)?"time-out":"Failed in core");
		retval = -EBUSY;
		goto shutdown_oq_done;
	}

	/* Wait till any in-transit packets are processed. */
	pkt_count = OCTEON_READ32(droq->pkts_sent_reg);
	loop_count = 100;
	while(pkt_count && (loop_count--)){
#if !defined(USE_DROQ_THREADS)
		cavium_tasklet_schedule(&oct->droq_tasklet);
#endif
		cavium_sleep_timeout(1);
		pkt_count = OCTEON_READ32(droq->pkts_sent_reg);
	}

	if(pkt_count) {
		cavium_error("%s Pkts processing timed-out (pkt_count: %d)\n",
		             __CVM_FUNCTION__, pkt_count);
		retval = -EBUSY;
		goto shutdown_oq_done;
	}

	/* Disable the output queues */
	if(oct->chip_id <= OCTEON_CN58XX) {
		uint64_t npi_ctl = OCTEON_PCI_WIN_READ(oct, NPI_CTL_STATUS);
		npi_ctl &= ~(1ULL  << (50 + q_no));
		OCTEON_PCI_WIN_WRITE(oct, NPI_CTL_STATUS, npi_ctl);
	}

	if(oct->chip_id == OCTEON_CN56XX) {
		uint32_t   pkt_out = octeon_read_csr(oct, CN56XX_NPEI_PKT_OUT_ENB);
		pkt_out &= ~(1ULL << q_no);
		octeon_write_csr(oct, CN56XX_NPEI_PKT_OUT_ENB, pkt_out);
	}

	if( (oct->chip_id == OCTEON_CN63XX) || (oct->chip_id == OCTEON_CN66XX) ) {
		uint32_t   pkt_out = octeon_read_csr(oct, CN63XX_SLI_PKT_OUT_ENB);
		pkt_out &= ~(1ULL << q_no);
		octeon_write_csr(oct, CN63XX_SLI_PKT_OUT_ENB, pkt_out);
	}

	if(oct->chip_id == OCTEON_CN68XX) {
		uint32_t   pkt_out = octeon_read_csr(oct, CN68XX_SLI_PKT_OUT_ENB);
		pkt_out &= ~(1ULL << q_no);
		octeon_write_csr(oct, CN68XX_SLI_PKT_OUT_ENB, pkt_out);
	}

	/* Reset the credit count register after enabling the queues. */
	OCTEON_WRITE32(oct->droq[q_no]->pkts_credit_reg, 0);

shutdown_oq_done:
	if(resp)
		cavium_free_dma(respbuf);
	return retval;

}







int
octeon_restart_output_queue(octeon_device_t  *oct, int q_no)
{
	int                 retval = 0;
	uint32_t           *respbuf, loop_count=100;
	volatile uint32_t  *resp;

	respbuf = cavium_malloc_dma(4, GFP_ATOMIC);
	if(respbuf == NULL) {
		cavium_error("%s buffer alloc failed\n", __CVM_FUNCTION__);
		return -ENOMEM;
	}
	resp = (volatile uint32_t  *)respbuf;
	*resp = 0xFFFFFFFF;

	/* Enable the output queues */
	if(oct->chip_id <= OCTEON_CN58XX) {
		uint64_t npi_ctl = OCTEON_PCI_WIN_READ(oct, NPI_CTL_STATUS);
		npi_ctl |= (1ULL  << (50 + q_no));
		OCTEON_PCI_WIN_WRITE(oct, NPI_CTL_STATUS, npi_ctl);
	}

	if(oct->chip_id == OCTEON_CN56XX) {
		uint32_t   pkt_out = octeon_read_csr(oct, CN56XX_NPEI_PKT_OUT_ENB);
		pkt_out |= (1ULL << q_no);
		octeon_write_csr(oct, CN56XX_NPEI_PKT_OUT_ENB, pkt_out);
	}

	if( (oct->chip_id == OCTEON_CN63XX) || (oct->chip_id == OCTEON_CN66XX) ) {
		uint32_t   pkt_out = octeon_read_csr(oct, CN63XX_SLI_PKT_OUT_ENB);
		pkt_out |= (1ULL << q_no);
		octeon_write_csr(oct, CN63XX_SLI_PKT_OUT_ENB, pkt_out);
	}

	if(oct->chip_id == OCTEON_CN68XX) {
		uint32_t   pkt_out = octeon_read_csr(oct, CN68XX_SLI_PKT_OUT_ENB);
		pkt_out |= (1ULL << q_no);
		octeon_write_csr(oct, CN68XX_SLI_PKT_OUT_ENB, pkt_out);
	}

	cavium_flush_write();

	/* Write the credit count register after enabling the queues. */
	OCTEON_WRITE32(oct->droq[q_no]->pkts_credit_reg,
	               oct->droq[q_no]->max_count);

	cavium_sleep_timeout(1);

	/* Send a command to Octeon to START further packet processing */
	if(octeon_send_short_command(oct, DEVICE_START_OP,
	                             ((q_no << 8) | DEVICE_PKO), respbuf, 4)) {
		cavium_error("%s command failed\n", __CVM_FUNCTION__);
		retval = -EINVAL;
		goto restart_oq_done;
	}

	/* Wait for response from Octeon. */
	while((*resp == 0xFFFFFFFF) && (loop_count--)) {
		cavium_sleep_timeout(1);
	}

	if(*resp != 0) {
		cavium_error("%s command failed: %s\n", __CVM_FUNCTION__,
		             (*resp == 0xFFFFFFFF)?"time-out":"Failed in core");
		retval = -EBUSY;
		goto restart_oq_done;
	}

restart_oq_done:
	if(resp)
		cavium_free_dma(respbuf);
	return retval;

}








int
wait_for_oq_pkts(octeon_device_t  *oct)
{
	int i, retry = 100,  pkt_cnt = 0;

	do {
		pkt_cnt = 0;

		for(i = 0; i < oct->num_oqs; i++)
			pkt_cnt += octeon_droq_check_hw_for_pkts(oct, oct->droq[i]);

		if(pkt_cnt == 0)
			break;

#if defined(USE_DROQ_THREADS)
		cavium_wakeup(&oct->droq[i]->wc);
#else
		cavium_tasklet_schedule(&oct->droq_tasklet);
#endif
		cavium_sleep_timeout(1);

	} while(retry-- && pkt_cnt);

	return pkt_cnt;
}









int
octeon_reset_recv_buf_size(octeon_device_t  *oct, int q_no, int newsize)
{
	int  num_qs=1, oq_no = q_no;

	if(!newsize) {
		cavium_error("%s Invalid buffer size (%d)\n", __CVM_FUNCTION__, newsize);
		return -EINVAL;
	}

	/*
	If the new buffer size is smaller than the current buffer size, do not
	do anything. Else change for all rings.
	*/
	if(newsize <= oct->droq[q_no]->buffer_size)
		return 0;

	(CHIP_FIELD(oct, cn56xx, conf))->oq_buf_size = newsize;

	if(oct->chip_id == OCTEON_CN56XX) {
		/* NOTE:  The buffer size in CN56XX is a global value and affects
		all rings. So we can't change it per-ring. */
		num_qs  = oct->num_oqs;
		oq_no    = 0;
	}

	cavium_print_msg("%s changing bufsize from %d to %d for %d queues first q: %d\n",
	       __CVM_FUNCTION__,
	       oct->droq[oq_no]->buffer_size, newsize, num_qs, oq_no);

	/* This loop accomodates changes in CN3XXX and CN56XX */
	while(num_qs--) {
		octeon_droq_t  *droq = oct->droq[oq_no];
		int retval;

		retval = octeon_shutdown_output_queue(oct, oq_no);
		if(retval != 0) {
			(CHIP_FIELD(oct, cn56xx, conf))->oq_buf_size = droq->buffer_size;
			return retval;
		}

		octeon_droq_destroy_ring_buffers(oct, droq);

		droq->buffer_size = newsize;

		octeon_droq_setup_ring_buffers(oct, droq);

		oct->fn_list.setup_oq_regs(oct, oq_no);

		oq_no++;
	}

	if(oct->chip_id <= OCTEON_CN58XX) {
		num_qs = 1;
		oq_no  = q_no;
	}

	if(oct->chip_id == OCTEON_CN56XX) {
		cn56xx_setup_global_output_regs(oct);
		num_qs  = oct->num_oqs;
		oq_no    = 0;
	}

	if(oct->chip_id == OCTEON_CN63XX) {
		cn63xx_setup_global_output_regs(oct);
		num_qs  = oct->num_oqs;
		oq_no    = 0;
	}

	if(oct->chip_id == OCTEON_CN66XX) {
		cn66xx_setup_global_output_regs(oct);
		num_qs  = oct->num_oqs;
		oq_no    = 0;
	}

	if(oct->chip_id == OCTEON_CN68XX) {
		cn68xx_setup_global_output_regs(oct);
		num_qs  = oct->num_oqs;
		oq_no    = 0;
	}

	while(num_qs--) {
		int retval;

		retval = octeon_restart_output_queue(oct, oq_no);
		if(retval != 0)
			return retval;
		oq_no++;
	}

	return 0;
}






/*
  octeon_create_recv_info
  Parameters: 
    octeon_dev - pointer to the octeon device structure
    droq       - droq in which the packet arrived. 
    buf_cnt    - no. of buffers used by the packet.
    idx        - index in the descriptor for the first buffer in the packet.
  Description:
    Allocates a recv_info_t and copies the buffer addresses for packet data 
    into the recv_pkt space which starts at an 8B offset from recv_info_t.
    Flags the descriptors for refill later. If available descriptors go 
    below the threshold to receive a 64K pkt, new buffers are first allocated
    before the recv_pkt_t is created.
    This routine will be called in interrupt context.
  Returns:
    Success: Pointer to recv_info_t 
    Failure: NULL. 
  Locks:
    The droq->lock is held when this routine is called.
*/
static inline octeon_recv_info_t  *
octeon_create_recv_info(octeon_device_t  *octeon_dev,
                        octeon_droq_t    *droq,
                        uint32_t          buf_cnt,
                        uint32_t          idx)
{
   octeon_droq_info_t    *info;
   octeon_recv_pkt_t     *recv_pkt;
   octeon_recv_info_t    *recv_info;
   uint32_t               i, bytes_left;

   cavium_print(PRINT_FLOW,"\n\n----#  create_recv_pkt #----\n");
   info = &droq->info_list[idx];

   cavium_print(PRINT_DEBUG,"buf_cnt: %d  idx: %d\n", buf_cnt, idx);
   recv_info = octeon_alloc_recv_info(sizeof(struct __dispatch));
   if(!recv_info)
      return NULL;

   recv_pkt = recv_info->recv_pkt;

#if  !defined(ETHERPCI)
   /* This function is never called in EtherPCI. Also info_ptr does not have
      resp_hdr for ETHERPCI. Just don't wan't this code to compile. */
   recv_pkt->resp_hdr      =  info->resp_hdr;
#endif
   recv_pkt->length        =  info->length;
   recv_pkt->buffer_count  =  (uint16_t)buf_cnt;
   recv_pkt->octeon_id     =  (uint16_t)octeon_dev->octeon_id;
   recv_pkt->buf_type      = OCT_RECV_BUF_TYPE_2;


   cavium_print(PRINT_DEBUG, "recv_pkt: len: %x  buf_cnt: %x\n",
                recv_pkt->length, recv_pkt->buffer_count);

   i = 0;
   bytes_left = info->length;

   while(buf_cnt) {
      octeon_pci_unmap_single(octeon_dev->pci_dev, (unsigned long)droq->desc_ring[idx].buffer_ptr, droq->buffer_size, CAVIUM_PCI_DMA_FROMDEVICE);

      recv_pkt->buffer_size[i] =
      (bytes_left >= droq->buffer_size)?droq->buffer_size: bytes_left;

#if defined(ENABLE_PCIE_2G4G_FIX)
      recv_pkt->buffer_ptr[i]  = get_new_recv_buffer(recv_pkt->buffer_size[i]);
      if(recv_pkt->buffer_ptr[i] == NULL)
         goto recv_info_failed;
      memcpy(get_recv_buffer_data(recv_pkt->buffer_ptr[i]), droq->recv_buf_list[idx].buffer, recv_pkt->buffer_size[i]);
#else
      recv_pkt->buffer_ptr[i]  = droq->recv_buf_list[idx].buffer;
      droq->recv_buf_list[idx].buffer = 0;
#endif

      INCR_INDEX_BY1(idx, droq->max_count);
      bytes_left -= droq->buffer_size;
      i++; buf_cnt--;
   }

   return recv_info;

#if defined(ENABLE_PCIE_2G4G_FIX)
recv_info_failed:
   while(i--) {
      free_recv_buffer(recv_pkt->buffer_ptr[i]);
   }
   cavium_free_dma(recv_info);
   return NULL;
#endif
}





/* If we were not able to refill all buffers, try to move around
   the buffers that were not dispatched.
*/
static inline uint32_t
octeon_droq_refill_pullup_descs(octeon_droq_t        *droq,
                                octeon_droq_desc_t   *desc_ring)
{
	uint32_t   desc_refilled=0;

#if !defined(ENABLE_PCIE_2G4G_FIX)
	uint32_t   refill_index = droq->host_refill_index;

	while (refill_index != droq->host_read_index) {
		if(droq->recv_buf_list[refill_index].buffer != 0)  {
			droq->recv_buf_list[droq->host_refill_index].buffer = droq->recv_buf_list[refill_index].buffer;
			droq->recv_buf_list[droq->host_refill_index].data   = droq->recv_buf_list[refill_index].data;
			desc_ring[droq->host_refill_index].buffer_ptr       = desc_ring[refill_index].buffer_ptr;
			droq->recv_buf_list[refill_index].buffer = 0;
			desc_ring[refill_index].buffer_ptr       = 0;
			do {
				INCR_INDEX_BY1(droq->host_refill_index, droq->max_count);
				desc_refilled++;
				droq->refill_count--;
			} while (droq->recv_buf_list[droq->host_refill_index].buffer);
		}
		INCR_INDEX_BY1(refill_index, droq->max_count);
	} /* while */
#endif
	return desc_refilled;
}






/*
  octeon_droq_refill
  Parameters: 
    droq       - droq in which descriptors require new buffers. 
  Description:
    Called during normal DROQ processing in interrupt mode or by the poll
    thread to refill the descriptors from which buffers were dispatched
    to upper layers. Attempts to allocate new buffers. If that fails, moves
    up buffers (that were not dispatched) to form a contiguous ring.
  Returns:
    No of descriptors refilled.
  Locks:
    This routine is called with droq->lock held.
*/
uint32_t
octeon_droq_refill(octeon_device_t  *octeon_dev, octeon_droq_t   *droq)
{
   octeon_droq_desc_t    *desc_ring;
   void                  *buf;
   uint8_t               *data;
   uint32_t               desc_refilled=0;

   desc_ring = droq->desc_ring;

   cavium_print(PRINT_DEBUG,"\n\n----#   octeon_droq_refill #----\n");
   cavium_print(PRINT_DEBUG,
                "refill_count: %d host_refill_index: %d, host_read_index: %d\n",
            droq->refill_count, droq->host_refill_index, droq->host_read_index);

   while(droq->refill_count && (desc_refilled < droq->max_count)) {
		/* If a valid buffer exists (happens if there is no dispatch), reuse 
		   the buffer, else allocate. */
		if(droq->recv_buf_list[droq->host_refill_index].buffer == 0)  {
			buf = get_new_recv_buffer(droq->buffer_size);
			/* If a buffer could not be allocated, no point in continuing */
			if(!buf) 
				break;
			droq->recv_buf_list[droq->host_refill_index].buffer = buf;
			data = get_recv_buffer_data(buf);
		} else {
#if defined(ENABLE_PCIE_2G4G_FIX)
			data = (uint8_t *)droq->recv_buf_list[droq->host_refill_index].buffer;
#else
			data = get_recv_buffer_data(droq->recv_buf_list[droq->host_refill_index].buffer);
#endif
		}

#if defined(ETHERPCI)
		data += 8;
#endif
		droq->recv_buf_list[droq->host_refill_index].data = data;
		desc_ring[droq->host_refill_index].buffer_ptr = (uint64_t)octeon_pci_map_single(octeon_dev->pci_dev, data, droq->buffer_size, CAVIUM_PCI_DMA_FROMDEVICE);

		/* Reset any previous values in the length field. */
		droq->info_list[droq->host_refill_index].length = 0;

		INCR_INDEX_BY1(droq->host_refill_index, droq->max_count);
		desc_refilled++;
		droq->refill_count--;
   }


   cavium_print(PRINT_DEBUG,"First pass of refill completed\n");
   cavium_print(PRINT_DEBUG,
                "refill_count: %d host_refill_index: %d, host_read_index: %d\n",
            droq->refill_count, droq->host_refill_index, droq->host_read_index);

	if(droq->refill_count) {
		desc_refilled += octeon_droq_refill_pullup_descs(droq, desc_ring);
	} /* if droq->refill_count */

	/* The refill count would not change in pass two. We only moved buffers
	   to close the gap in the ring, but we would still have the same no. of
	   buffers to refill.
	*/
	return desc_refilled;
}








static inline uint32_t
octeon_droq_get_bufcount(uint32_t   buf_size,
                         uint32_t   total_len)
{
  uint32_t  buf_cnt=0;

   while(total_len > (buf_size * buf_cnt))
        buf_cnt++;
   return buf_cnt;
}






static int
octeon_droq_dispatch_pkt(octeon_device_t     *oct,
                         octeon_droq_t       *droq,
                         octeon_resp_hdr_t   *resp_hdr,
                         octeon_droq_info_t  *info)
{
	uint32_t               cnt;
	octeon_dispatch_fn_t   disp_fn;
	octeon_recv_info_t    *rinfo;

	cnt = octeon_droq_get_bufcount(droq->buffer_size, info->length);

	disp_fn = octeon_get_dispatch(oct, (uint16_t)resp_hdr->opcode);
	if(disp_fn) {
		rinfo = octeon_create_recv_info(oct, droq, cnt, droq->host_read_index);
		if(rinfo) {
			struct __dispatch *rdisp = rinfo->rsvd;
			rdisp->rinfo    = rinfo;
			rdisp->disp_fn  = disp_fn;
			*((uint64_t *)&rinfo->recv_pkt->resp_hdr) = *((uint64_t *)resp_hdr);
			cavium_list_add_tail(&rdisp->list, &droq->dispatch_list);
		} else {
			droq->stats.dropped_nomem++;
		}
	} else {
		droq->stats.dropped_nodispatch++;
	}  /* else (dispatch_fn ... */

	return cnt;
}









static inline void
octeon_droq_drop_packets(octeon_droq_t   *droq, uint32_t   cnt)
{
	uint32_t               i = 0, buf_cnt;
	octeon_droq_info_t    *info;

	for(i = 0; i < cnt; i++)   {
		info = &(droq->info_list[droq->host_read_index]);
		octeon_swap_8B_data((uint64_t *)info, 2);

		if(info->length)  {
			info->length               -= OCT_RESP_HDR_SIZE;
			droq->stats.bytes_received += info->length;
			buf_cnt = octeon_droq_get_bufcount(droq->buffer_size, info->length);
		} else {
			cavium_error("OCTEON:DROQ: In drop: pkt with len 0\n");
			buf_cnt = 1;
		}

#if  defined(FAST_PATH_DROQ_DISPATCH)
		{
			octeon_resp_hdr_t     *resp_hdr = &info->resp_hdr;
			if( (resp_hdr->opcode & droq->ops.op_mask) != droq->ops.op_major) {
				octeon_droq_dispatch_pkt(droq->oct_dev, droq, resp_hdr, info);
			}
		}
#endif

		INCR_INDEX(droq->host_read_index, buf_cnt, droq->max_count);
    	droq->refill_count += buf_cnt; 
	}
}






static uint32_t
octeon_droq_fast_process_packets(octeon_device_t  *oct,
                                 octeon_droq_t    *droq,
                                 uint32_t          pkts_to_process)
{
	octeon_droq_info_t    *info;
	octeon_resp_hdr_t     *resp_hdr;
	uint32_t               pkt, total_len = 0, pkt_count, bufs_used=0;
#ifdef ETHERPCI
	uint64_t               etherpci_resp_hdr = 0ULL;
#endif


#if !defined(USE_DROQ_THREADS)
	if(pkts_to_process > droq->pkts_per_intr)
		pkt_count = droq->pkts_per_intr;
	else
#endif
		pkt_count = pkts_to_process;


	for(pkt = 0; pkt < pkt_count; pkt++)   {

		int               pkt_len = 0;
		cavium_netbuf_t  *nicbuf = NULL;

		info = &(droq->info_list[droq->host_read_index]);

#if defined(ETHERPCI)
		octeon_swap_8B_data((uint64_t *)&info->length, 1);
		/* infoptr bytes = 0 doesnt work in 56xx. To workaround that, we copy
		   8 bytes of data from infoptr for all Octeon devices. */
		cavium_memcpy(droq->recv_buf_list[droq->host_read_index].data-8,(char*)info,8);
#else
		octeon_swap_8B_data((uint64_t *)info, 2);
#endif

		if(!info->length)  {
			cavium_error("OCTEON:DROQ[%d] idx: %d len:0, pkt_cnt: %d \n",
			             droq->q_no, droq->host_read_index, pkt_count);
			cavium_error_print_data((uint8_t *)info, OCT_DROQ_INFO_SIZE);
			pkt++;
			break;
		}

#ifdef ETHERPCI
		resp_hdr = (octeon_resp_hdr_t *)&etherpci_resp_hdr;
		resp_hdr->dest_qport  = droq->q_no;
#else
		/* Len of resp hdr in included in the received data len. */
		info->length -= OCT_RESP_HDR_SIZE;
		resp_hdr = &info->resp_hdr;
#endif

		total_len    += info->length;


#if  defined(FAST_PATH_DROQ_DISPATCH)
		if( (resp_hdr->opcode & droq->ops.op_mask) != droq->ops.op_major) {
			int cnt = octeon_droq_dispatch_pkt(oct, droq, resp_hdr, info);
			INCR_INDEX(droq->host_read_index, cnt, droq->max_count);
			bufs_used += cnt;
			continue;
		}
#endif

#if defined(ENABLE_PCIE_2G4G_FIX)
		nicbuf = get_new_recv_buffer(info->length);
		if(nicbuf == NULL) {
			pkt_len = info->length;
			while(pkt_len) {
				bufs_used++;
				pkt_len -= (pkt_len > droq->buffer_size)?droq->buffer_size:pkt_len;
				INCR_INDEX_BY1(droq->host_read_index, droq->max_count);
			}
			continue;
		}
#endif

		if(info->length <= droq->buffer_size) {
			octeon_pci_unmap_single(oct->pci_dev,
				(unsigned long)droq->desc_ring[droq->host_read_index].buffer_ptr,
				droq->buffer_size, CAVIUM_PCI_DMA_FROMDEVICE);
			pkt_len = info->length;
#if  defined(ENABLE_PCIE_2G4G_FIX)
			memcpy(get_recv_buffer_data(nicbuf), droq->recv_buf_list[droq->host_read_index].buffer, info->length);
#else
			nicbuf = droq->recv_buf_list[droq->host_read_index].buffer;
			droq->recv_buf_list[droq->host_read_index].buffer = 0;
#endif
			INCR_INDEX_BY1(droq->host_read_index, droq->max_count);
			recv_buf_put(nicbuf, pkt_len);
			bufs_used++;
		} else {
#if  !defined(ENABLE_PCIE_2G4G_FIX)
			nicbuf  = get_new_recv_buffer(info->length);
#endif
			pkt_len = 0;
			/* nicbuf allocation can fail. We'll handle it inside the loop. */
			while(pkt_len < info->length) {
				int copy_len;

				copy_len = ((pkt_len + droq->buffer_size) > info->length)?(info->length - pkt_len):droq->buffer_size;

				if(nicbuf) {
					octeon_pci_unmap_single(oct->pci_dev, (unsigned long)droq->desc_ring[droq->host_read_index].buffer_ptr, droq->buffer_size, CAVIUM_PCI_DMA_FROMDEVICE);

#if  defined(ENABLE_PCIE_2G4G_FIX)
					cavium_memcpy(recv_buf_put(nicbuf, copy_len),
					droq->recv_buf_list[droq->host_read_index].buffer,copy_len);
#else
					cavium_memcpy(recv_buf_put(nicbuf, copy_len), get_recv_buffer_data(droq->recv_buf_list[droq->host_read_index].buffer), copy_len);
#endif
				}

				pkt_len += copy_len;
				INCR_INDEX_BY1(droq->host_read_index, droq->max_count);
				bufs_used++;
			}
		}

		if(nicbuf) {
			if(droq->ops.fptr)
				droq->ops.fptr(oct->octeon_id, nicbuf, pkt_len, resp_hdr);
			else
				free_recv_buffer(nicbuf);
		}

	}  /* for ( each packet )... */

    touch_softlockup_watchdog();

	/* Increment refill_count by the number of buffers processed. */
	droq->refill_count         += bufs_used;

	droq->stats.pkts_received  += pkt;
	droq->stats.bytes_received += total_len;

	if((droq->ops.drop_on_max) && (pkts_to_process - pkt)) {
		octeon_droq_drop_packets(droq, (pkts_to_process - pkt));
		droq->stats.dropped_toomany += (pkts_to_process - pkt);
		return pkts_to_process;
	}

	return pkt;
}









#if !defined(ETHERPCI) && !defined(PERF_MODE)

static uint32_t
octeon_droq_slow_process_packets(octeon_device_t  *oct,
                                 octeon_droq_t    *droq,
                                 uint32_t          pkts_to_process)
{
	octeon_resp_hdr_t     *resp_hdr;
	uint32_t               desc_processed=0;
	octeon_droq_info_t    *info;
	uint32_t               pkt, pkt_count, buf_cnt=0;



	if(pkts_to_process > droq->pkts_per_intr)
		pkt_count = droq->pkts_per_intr;
	else
		pkt_count = pkts_to_process;


	for(pkt = 0; pkt < pkt_count; pkt++)   {
		info = &(droq->info_list[droq->host_read_index]);
		resp_hdr = (octeon_resp_hdr_t *)&info->resp_hdr;

		octeon_swap_8B_data((uint64_t *)info, 2);

		if(!info->length)  {
			cavium_error("OCTEON:DROQ[idx: %d]: len:%llx, pkt_cnt: %d \n",
			             droq->host_read_index, CVM_CAST64(info->length), pkt_count);
			cavium_error_print_data((uint8_t *)info, OCT_DROQ_INFO_SIZE);

			pkt++;
			break;
		}

		/* Len of resp hdr in included in the received data len. */
		info->length -= OCT_RESP_HDR_SIZE;
		droq->stats.bytes_received += info->length;

		buf_cnt = octeon_droq_dispatch_pkt(oct, droq, resp_hdr, info);

		INCR_INDEX(droq->host_read_index, buf_cnt, droq->max_count);
		droq->refill_count += buf_cnt;
		desc_processed += buf_cnt;
	}  /* for ( each packet )... */

	droq->stats.pkts_received += pkt;

	if((droq->ops.drop_on_max) && (pkts_to_process - pkt)) {
		octeon_droq_drop_packets(droq, (pkts_to_process - pkt));
		droq->stats.dropped_toomany += (pkts_to_process - pkt); 
		return pkts_to_process;
	} 

	return pkt;
}


#endif






#ifdef  PERF_MODE

static int droq_pkts = 0;

static inline int
octeon_droq_process_packets(octeon_device_t   *oct, octeon_droq_t  *droq)
{
	uint32_t    pkt_count=0, i = 0;


	pkt_count = cavium_atomic_read(&droq->pkts_pending);
	if(!pkt_count)
		return 0;

	/* Grab the lock */
	cavium_spin_lock(&droq->lock);

	for(i = 0; i < pkt_count; i++)   {
		octeon_droq_info_t   *info;
		uint32_t              buf_cnt;

		droq_pkts++;
		info = &(droq->info_list[droq->host_read_index]);
		octeon_swap_8B_data((uint64_t *)info, 2);

		buf_cnt = 0;
		if(info->length)  {
			droq->stats.bytes_received += info->length;
			info->length               -= OCT_RESP_HDR_SIZE;
			buf_cnt = octeon_droq_get_bufcount(droq->buffer_size, info->length);
		}

		INCR_INDEX(droq->host_read_index, buf_cnt, droq->max_count);
    	droq->refill_count += buf_cnt; 
	}

	droq->stats.pkts_received   += pkt_count;
	droq->stats.dropped_toomany += pkt_count;
	cavium_atomic_sub(pkt_count, &droq->pkts_pending);

	if(droq->refill_count >= droq->refill_threshold) {
		uint32_t  desc_refilled = octeon_droq_refill(oct, droq);
		cavium_flush_write();
		OCTEON_WRITE32(droq->pkts_credit_reg, (desc_refilled));
	}

	/* Release the spin lock */
	cavium_spin_unlock(&droq->lock);

	/* If there are packets pending. schedule tasklet again */
	if(cavium_atomic_read(&droq->pkts_pending))
		return 1;

	return 0;
}



#else


static inline int
octeon_droq_process_packets(octeon_device_t   *oct, octeon_droq_t  *droq)
{
	uint32_t        pkt_count=0, pkts_processed=0, desc_refilled=0;
	cavium_list_t   *tmp, *tmp2;

	pkt_count = cavium_atomic_read(&droq->pkts_pending);
	if(!pkt_count)
		return 0;

	/* Grab the lock */
	cavium_spin_lock(&droq->lock);

	if(droq->fastpath_on)
		pkts_processed = octeon_droq_fast_process_packets(oct, droq, pkt_count);
	else {
#if defined(ETHERPCI)
		octeon_droq_drop_packets(droq, pkt_count);
		droq->stats.dropped_toomany += pkt_count;
		pkts_processed = pkt_count;
#else
		pkts_processed = octeon_droq_slow_process_packets(oct, droq, pkt_count);
#endif
	}
	cavium_atomic_sub(pkts_processed, &droq->pkts_pending);

	if(droq->refill_count >= droq->refill_threshold) {
		desc_refilled = octeon_droq_refill(oct, droq);
		cavium_flush_write();
		OCTEON_WRITE32(droq->pkts_credit_reg, (desc_refilled));
	}

	/* Release the spin lock */
	cavium_spin_unlock(&droq->lock);


	cavium_list_for_each_safe(tmp, tmp2, &droq->dispatch_list) {
		struct __dispatch *rdisp = (struct __dispatch *)tmp;
		cavium_list_del(tmp);
		rdisp->disp_fn(rdisp->rinfo,
			octeon_get_dispatch_arg(oct, rdisp->rinfo->recv_pkt->resp_hdr.opcode));
	}
	
	/* If there are packets pending. schedule tasklet again */
	if(cavium_atomic_read(&droq->pkts_pending))
		return 1;

	return 0;
}


#endif






#ifdef  USE_DROQ_THREADS

static int 
oct_droq_thread(void* arg)
{
	char   name[] = "Octeon DROQ Thread";
	octeon_droq_t   *droq = (octeon_droq_t *)arg;


	cavium_atomic_set(&droq->thread_active, 1);

	cavium_print_msg("\n-- OCTEON: %s starting execution now on cpu %d!\n",
		 name, smp_processor_id());

	while(!droq->stop_thread && !cavium_kthread_signalled())  {
		
		while(cavium_atomic_read(&droq->pkts_pending))
			octeon_droq_process_packets(droq->oct_dev, droq);

		cavium_sleep_atomic_cond(&droq->wc, &droq->pkts_pending);
	}

	cavium_print_msg("\n-- OCTEON: DROQ thread quitting now\n");
	cavium_atomic_set(&droq->thread_active, 0);

	return 0;
}



#else 


void
octeon_droq_bh(unsigned long  pdev)
{
	uint32_t               q_no;
	volatile int           reschedule=0;
	octeon_device_t       *oct = (octeon_device_t  *)pdev;

	oct->stats.droq_tasklet_count++;

	for(q_no = 0; q_no < oct->num_oqs; q_no++) {
		reschedule |= octeon_droq_process_packets(oct, oct->droq[q_no]);
	}

	if(reschedule)
		cavium_tasklet_schedule(&oct->droq_tasklet);
}






#endif


static int
octeon_droq_process_poll_pkts(octeon_device_t   *oct,
                              octeon_droq_t     *droq,
                              int                budget)
{
	uint32_t   pkts_available = 0, pkts_processed = 0, total_pkts_processed = 0;

	if(budget > droq->max_count)
		budget = droq->max_count;
	
	cavium_spin_lock(&droq->lock);

	pkts_available = CVM_MIN(budget, (cavium_atomic_read(&droq->pkts_pending)));

process_some_more:
	pkts_processed = octeon_droq_fast_process_packets(oct, droq, pkts_available);
	cavium_atomic_sub(pkts_processed, &droq->pkts_pending);
	total_pkts_processed += pkts_processed;

	if(total_pkts_processed < budget) {
		octeon_droq_check_hw_for_pkts(oct, droq);
		if(cavium_atomic_read(&droq->pkts_pending)) {
			pkts_available = CVM_MIN((budget - total_pkts_processed), (cavium_atomic_read(&droq->pkts_pending)));
			goto process_some_more;
		} 
	}

	if(droq->refill_count >= droq->refill_threshold) {
		int desc_refilled = octeon_droq_refill(oct, droq);
		cavium_flush_write();
		OCTEON_WRITE32(droq->pkts_credit_reg, (desc_refilled));
	}

	cavium_spin_unlock(&droq->lock);

	return total_pkts_processed;
}




int
octeon_process_droq_poll_cmd(int  oct_id, int q_no, int cmd, uint32_t  arg)
{
	octeon_device_t   *oct = get_octeon_device(oct_id);
	octeon_droq_t     *droq;

	if(!(oct)) {
		cavium_error(" %s: No Octeon device (id: %d)\n", __CVM_FUNCTION__, oct_id);
		return -ENODEV;
	}

	if(q_no >= oct->num_oqs) {
		cavium_error(" %s: droq id (%d) exceeds MAX (%d)\n",
		             __CVM_FUNCTION__, q_no, (oct->num_oqs - 1));
		return -EINVAL;
	}

	droq = oct->droq[q_no];

	if(cmd == POLL_EVENT_PROCESS_PKTS) {
		return octeon_droq_process_poll_pkts(oct, droq, arg);
	}

	if(cmd == POLL_EVENT_ENABLE_INTR) {
		oct->napi_mask &= ~(1 << q_no);
		if(oct->napi_mask == 0) {
			/* Enable Pkt Interrupt */
			switch(oct->chip_id) {
				case OCTEON_CN38XX_PASS2:
				case OCTEON_CN38XX_PASS3:
				case OCTEON_CN58XX: {
					octeon_cn3xxx_t  *cn3xxx = (octeon_cn3xxx_t  *)oct->chip;
					cavium_atomic_set(&cn3xxx->intr_mask,
						(cavium_atomic_read(&cn3xxx->intr_mask) | (PCI_INT_DROQ_MASK)));
				} break;

				case OCTEON_CN56XX: {
					octeon_cn56xx_t  *cn56xx = (octeon_cn56xx_t  *)oct->chip;
					cn56xx->intr_mask64 |= (CN56XX_INTR_PKT_DATA);
				} break;

				case OCTEON_CN63XX: {
					octeon_cn63xx_t  *cn63xx = (octeon_cn63xx_t  *)oct->chip;
					cn63xx->intr_mask64 |= (CN63XX_INTR_PKT_DATA);
				} break;

				case OCTEON_CN66XX: {
					octeon_cn66xx_t  *cn66xx = (octeon_cn66xx_t  *)oct->chip;
					cn66xx->intr_mask64 |= (CN63XX_INTR_PKT_DATA);
				} break;

				case OCTEON_CN68XX: {
					octeon_cn68xx_t  *cn68xx = (octeon_cn68xx_t  *)oct->chip;
					cn68xx->intr_mask64 |= (CN68XX_INTR_PKT_DATA);
				} break;
			}
			oct->fn_list.enable_interrupt(oct->chip);
		}

		return 0;
	}

	cavium_error("%s Unknown command: %d\n", __FUNCTION__, cmd);
	return -EINVAL;
}




int
octeon_register_droq_ops(int oct_id, int q_no, octeon_droq_ops_t  *ops)
{
	octeon_device_t   *oct = get_octeon_device(oct_id);
	octeon_droq_t     *droq;
	unsigned long      flags;

	if(!(oct)) {
		cavium_error(" %s: No Octeon device (id: %d)\n", __CVM_FUNCTION__, oct_id);
		return -ENODEV;
	}

 	if(!(ops)) {
		cavium_error(" %s: droq_ops pointer is NULL\n", __CVM_FUNCTION__);
		return -EINVAL;
	}

	if(q_no >= oct->num_oqs) {
		cavium_error(" %s: droq id (%d) exceeds MAX (%d)\n",
		             __CVM_FUNCTION__, q_no, (oct->num_oqs - 1));
		return -EINVAL;
	}

#ifdef  USE_DROQ_THREADS
	if(ops->poll_mode) {
		cavium_error(" %s: Cannot Support Polling when BASE Driver uses DROQ Threads. \n",  __CVM_FUNCTION__);
		return -EINVAL;
	}
#endif

	droq = oct->droq[q_no];

	cavium_spin_lock_irqsave(&droq->lock, flags);

	memcpy(&droq->ops, ops, sizeof(octeon_droq_ops_t));

	if(droq->ops.fptr)
		droq->fastpath_on = 1;

	cavium_spin_unlock_irqrestore(&droq->lock, flags);

	return 0;
}






int
octeon_unregister_droq_ops(int oct_id, int q_no)
{
	octeon_device_t   *oct = get_octeon_device(oct_id);
	octeon_droq_t     *droq;

	if(!(oct)) {
		cavium_error(" %s: No Octeon device (id: %d)\n", __CVM_FUNCTION__, oct_id);
		return -ENODEV;
	}

	if(q_no >= oct->num_oqs) {
		cavium_error(" %s: droq id (%d) exceeds MAX (%d)\n",
		             __CVM_FUNCTION__, q_no, oct->num_oqs-1);
		return -EINVAL;
	}

	droq = oct->droq[q_no];

	cavium_spin_lock(&droq->lock);

	droq->fastpath_on     = 0;
	droq->ops.fptr        = NULL;
	droq->ops.drop_on_max = 0;
#if  defined(FAST_PATH_DROQ_DISPATCH)
	droq->ops.op_mask     = 0;
	droq->ops.op_major    = 0;
#endif

	cavium_spin_unlock(&droq->lock);

	return 0;
}










oct_poll_fn_status_t
check_droq_refill(void  *octptr, unsigned long q_no)
{
	octeon_device_t   *oct = (octeon_device_t  *)octptr;
	octeon_droq_t     *droq;

	droq = oct->droq[q_no];

	if (droq->refill_count >= droq->refill_threshold)  {
		uint32_t  desc_refilled;

		cavium_spin_lock_softirqsave(&droq->lock);
		desc_refilled = octeon_droq_refill(oct, droq);
		if(desc_refilled) {
			cavium_flush_write();
			OCTEON_WRITE32(droq->pkts_credit_reg, desc_refilled);
		}
		cavium_spin_unlock_softirqrestore(&droq->lock);
	}

	return OCT_POLL_FN_CONTINUE;
}


/* $Id: octeon_droq.c 67262 2011-11-22 00:43:21Z mchalla $ */
