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


#include "cavium_sysdep.h"
#include "octeon_hw.h"
#include "cn63xx_device.h"
#include "octeon_macros.h"
#include "octeon-pci.h"

extern void  cn63xx_errata_sli14401_fix(octeon_device_t  *oct);

void cn63xx_dump_dpi_regs(octeon_device_t   *oct);


static void
cn63xx_set_dpi_regs(octeon_device_t   *oct)
{
	int  i;
	int  fifo_sizes[6]  = {4, 4, 2, 1, 1, 4};

	OCTEON_PCI_WIN_WRITE(oct, CN63XX_DPI_DMA_CONTROL, CN63XX_DPI_DMA_CTL_MASK);
	cavium_print(PRINT_DEBUG, "DPI_DMA_CONTROL: 0x%016llx\n", OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_DMA_CONTROL));

	for(i = 0; i < 6; i++) {
		/* Prevent service of instruction queue for all DMA engines.
		   Engine 5 will remain 0. Engines 0 - 4 will be setup by core. */
		OCTEON_PCI_WIN_WRITE(oct, CN63XX_DPI_DMA_ENG_ENB(i), 0);
		OCTEON_PCI_WIN_WRITE(oct, CN63XX_DPI_DMA_ENG_BUF(i), fifo_sizes[i]);
		cavium_print(PRINT_DEBUG, "DPI_ENG_BUF%d: 0x%016llx\n", i, OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_DMA_ENG_BUF(i)));
	}

	/* DPI_SLI_PRT_CFG has MPS and MRRS settings that will be set
	   separately. */

	OCTEON_PCI_WIN_WRITE(oct, CN63XX_DPI_CTL, 1);
	cavium_print(PRINT_DEBUG, "DPI_CTL: 0x%016llx\n", OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_CTL));
}




static  int
cn63xx_soft_reset(octeon_device_t   *oct)
{
	octeon_write_csr64(oct, CN63XX_WIN_WR_MASK_REG, 0xFF);

	cavium_print_msg("OCTEON[%d]: BIST enabled for CN63XX soft reset\n",
	                 oct->octeon_id);
	OCTEON_PCI_WIN_WRITE(oct, CN63XX_CIU_SOFT_BIST, 1);
	octeon_write_csr64(oct, CN63XX_SLI_SCRATCH1, 0x1234ULL);

	OCTEON_PCI_WIN_READ(oct, CN63XX_CIU_SOFT_RST);
	OCTEON_PCI_WIN_WRITE(oct, CN63XX_CIU_SOFT_RST, 1);

	/* Wait for 10ms as Octeon resets. */
	cavium_mdelay(10);

	if(octeon_read_csr64(oct, CN63XX_SLI_SCRATCH1) == 0x1234ULL) {
		cavium_error("OCTEON[%d]: Soft reset failed\n", oct->octeon_id);
		return 1;
	}

	cavium_print_msg("OCTEON[%d]: Reset completed\n", oct->octeon_id);
	octeon_write_csr64(oct, CN63XX_WIN_WR_MASK_REG, 0xFF);

	cn63xx_set_dpi_regs(oct);

	return 0;
}





void
cn63xx_enable_error_reporting(octeon_device_t   *oct)
{
	uint32_t   regval;

	OCTEON_READ_PCI_CONFIG(oct, CN63XX_CONFIG_PCIE_DEVCTL, &regval);
	if(regval & 0x000f0000) {
		cavium_error("PCI-E Link error detected: 0x%08x\n",regval & 0x000f0000);
	}

	regval |= 0xf;  /* Enable Link error reporting */

	cavium_print_msg("OCTEON[%d]: Enabling PCI-E error reporting..\n",
	                 oct->octeon_id);
	cavium_print(PRINT_FLOW, "Writing  0x%08x to config[0x78]\n", regval);
	OCTEON_WRITE_PCI_CONFIG(oct, CN63XX_CONFIG_PCIE_DEVCTL, regval);
}





static void
cn63xx_setup_pcie_mps(octeon_device_t   *oct, enum octeon_pcie_mps  mps)
 {
	uint32_t            regval;
	volatile uint64_t   r64;

	/* Read config register for MPS */
	OCTEON_READ_PCI_CONFIG(oct, CN63XX_CONFIG_PCIE_DEVCTL, &regval);
 
	if(mps == PCIE_MPS_DEFAULT) {
		mps = ((regval & (0x7 << 5)) >> 5);
	} else {
		regval &= ~(0x7 << 5);    // Turn off any MPS bits
		regval |=  (mps << 5);    // Set MPS
		OCTEON_WRITE_PCI_CONFIG(oct, CN63XX_CONFIG_PCIE_DEVCTL, regval);
	}

	/* Set MPS in DPI_SLI_PRT0_CFG to the same value. */
	r64 = OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_SLI_PRTX_CFG(oct->pcie_port));
	r64 |= (mps << 4);
	OCTEON_PCI_WIN_WRITE(oct, CN63XX_DPI_SLI_PRTX_CFG(oct->pcie_port), r64);
}




static void
cn63xx_setup_pcie_mrrs(octeon_device_t   *oct, enum octeon_pcie_mrrs mrrs)
{
	uint32_t            regval;
	volatile uint64_t   r64;

	/* Read config register for MRRS */
	OCTEON_READ_PCI_CONFIG(oct, CN63XX_CONFIG_PCIE_DEVCTL, &regval);

	if(mrrs == PCIE_MRRS_DEFAULT) {
		mrrs = ((regval & (0x7 << 12)) >> 12);
	} else {
		regval &= ~(0x7 << 12);   // Turn off any MRRS bits
		regval |=  (mrrs << 12);  // Set MRRS
		OCTEON_WRITE_PCI_CONFIG(oct, CN63XX_CONFIG_PCIE_DEVCTL, regval);
	}

	/* Set MRRS in SLI_S2M_PORT0_CTL to the same value. */
	r64 = octeon_read_csr64(oct, CN63XX_SLI_S2M_PORTX_CTL(oct->pcie_port));
	r64 |= mrrs;
	octeon_write_csr64(oct, CN63XX_SLI_S2M_PORTX_CTL(oct->pcie_port), r64);


	/* Set MRRS in DPI_SLI_PRT0_CFG to the same value. */
	r64 = OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_SLI_PRTX_CFG(oct->pcie_port));
	r64 |= mrrs;
	OCTEON_PCI_WIN_WRITE(oct, CN63XX_DPI_SLI_PRTX_CFG(oct->pcie_port), r64);

}








uint32_t
cn63xx_core_clock(octeon_device_t  *oct)
{
	/* Bits 35:30 of MIO_RST_BOOT holds the ref. clock multiplier for cores. */
	return (((OCTEON_PCI_WIN_READ(oct, CN63XX_MIO_RST_BOOT) >> 30) & 0x3f) * 50);
}



static uint32_t
cn63xx_coprocessor_clock(octeon_device_t  *oct)
{
	/* Bits 29:24 of MIO_RST_BOOT holds the ref. clock multiplier for SLI. */
	return (((OCTEON_PCI_WIN_READ(oct, CN63XX_MIO_RST_BOOT) >> 24) & 0x3f) * 50);
}




uint32_t
cn63xx_get_oq_ticks(octeon_device_t  *oct, uint32_t  time_intr_in_us)
{
	/* This gives the SLI clock per microsec */
	uint32_t  oqticks_per_us = cn63xx_coprocessor_clock(oct);

	/* core clock per us / oq ticks will be fractional. TO avoid that
	   we use the method below. */

	/* This gives the clock cycles per millisecond */
	oqticks_per_us *= 1000;

	/* This gives the oq ticks (1024 core clock cycles) per millisecond */
	oqticks_per_us /= 1024;

	/* time_intr is in microseconds. The next 2 steps gives the oq ticks
	   corressponding to time_intr. */
	oqticks_per_us *= time_intr_in_us;
	oqticks_per_us /= 1000;

	return oqticks_per_us;
}






void
cn63xx_setup_global_input_regs(octeon_device_t  *oct)
{
	/* Select Round-Robin Arb, ES, RO, NS for Input Queues */
	octeon_write_csr(oct, CN63XX_SLI_PKT_INPUT_CONTROL, CN63XX_INPUT_CTL_MASK);

	/* Instruction Read Size - Max 4 instructions per PCIE Read */
	octeon_write_csr64(oct, CN63XX_SLI_PKT_INSTR_RD_SIZE,
	                   0xFFFFFFFFFFFFFFFFULL);

	/* Select PCIE Port for all Input rings. */
	octeon_write_csr64(oct, CN63XX_SLI_IN_PCIE_PORT,
	   (oct->pcie_port * 0x5555555555555555ULL) );
}



void
cn63xx_setup_global_output_regs(octeon_device_t  *oct)
{
	uint32_t          time_threshold;
	uint64_t          pktctl;
	octeon_cn63xx_t  *cn63xx = (octeon_cn63xx_t *)oct->chip;

	pktctl = octeon_read_csr64(oct, CN63XX_SLI_PKT_CTL);

	if(oct->num_oqs <= 4) {
		/* Disable RING_EN if only upto 4 rings are used. */
		pktctl &= ~(1 << 4);
	} else {
		pktctl |= (1 << 4);
	}

	/* Disable per-port backpressure. */
	pktctl &= ~0xF;
	octeon_write_csr64(oct, CN63XX_SLI_PKT_CTL, pktctl);


	/// Select PCI-E Port for all Output queues
	octeon_write_csr64(oct, CN63XX_SLI_PKT_PCIE_PORT64,
	                   (oct->pcie_port * 0x5555555555555555ULL));

	/// Set Output queue watermark to 0 to disable backpressure
	octeon_write_csr64(oct, CN63XX_SLI_OQ_WMARK, 0);

	/// Select Info Ptr for length & data
	octeon_write_csr(oct, CN63XX_SLI_PKT_IPTR, 0xFFFFFFFF);

	/// Select Packet count instead of bytes for SLI_PKTi_CNTS[CNT]
	octeon_write_csr(oct, CN63XX_SLI_PKT_OUT_BMODE, 0);

	/// Select ES,RO,NS setting from register for Output Queue Packet Address
	octeon_write_csr(oct, CN63XX_SLI_PKT_DPADDR, 0xFFFFFFFF);

	/// No Relaxed Ordering, No Snoop, 64-bit swap for Output Queue ScatterList
	octeon_write_csr(oct, CN63XX_SLI_PKT_SLIST_ROR, 0);
	octeon_write_csr(oct, CN63XX_SLI_PKT_SLIST_NS, 0);
	
	/// ENDIAN_SPECIFIC CHANGES - 0 works for LE.
#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
	octeon_write_csr64(oct, CN63XX_SLI_PKT_SLIST_ES64, 0ULL);
#else
	octeon_write_csr64(oct, CN63XX_SLI_PKT_SLIST_ES64, 0x5555555555555555ULL);
#endif

	/// No Relaxed Ordering, No Snoop, 64-bit swap for Output Queue Data
	octeon_write_csr(oct, CN63XX_SLI_PKT_DATA_OUT_ROR, 0);
	octeon_write_csr(oct, CN63XX_SLI_PKT_DATA_OUT_NS, 0);
	octeon_write_csr64(oct, CN63XX_SLI_PKT_DATA_OUT_ES64, 0x5555555555555555ULL);


	/// Set up interrupt packet and time threshold
	octeon_write_csr(oct, CN63XX_SLI_OQ_INT_LEVEL_PKTS, cn63xx->conf->oq_intr_pkt);
	time_threshold = cn63xx_get_oq_ticks(oct, cn63xx->conf->oq_intr_time);
	octeon_write_csr(oct, CN63XX_SLI_OQ_INT_LEVEL_TIME, time_threshold);
#ifdef CVMCS_DMA_IC
	/* This gives the SLI clock per microsec */
	time_threshold = cn63xx_coprocessor_clock(oct);
	/* Now Calculate the DMA timer interrupt threshold */
	time_threshold *= cn63xx->conf->dma_intr_time;
	octeon_write_csr64(oct, CN63XX_DMA_INT_LEVEL_START, 
			  ((uint64_t)time_threshold << 32) | 
			   cn63xx->conf->dma_intr_pkt);
	cavium_print(PRINT_DEBUG, "CN63XX_DMA_INT_LEVEL_START: %lx\n", octeon_read_csr64(oct, CN63XX_DMA_INT_LEVEL_START));
#endif
}





static int
cn63xx_setup_device_regs(octeon_device_t  *oct)
{
	cn63xx_setup_pcie_mps(oct, PCIE_MPS_DEFAULT);
	cn63xx_setup_pcie_mrrs(oct, PCIE_MRRS_512B);

	cn63xx_enable_error_reporting(oct);

	cn63xx_setup_global_input_regs(oct);
	cn63xx_setup_global_output_regs(oct);

	return 0;
}







static void
cn63xx_setup_iq_regs(octeon_device_t   *oct, int   iq_no)
{
	octeon_instr_queue_t   *iq = oct->instr_queue[iq_no];

	/* Disable Packet-by-Packet mode; No Parse Mode or Skip length */
	octeon_write_csr64(oct, CN63XX_SLI_IQ_PKT_INSTR_HDR64(iq_no), 0);


	/* Write the start of the input queue's ring and its size  */
	octeon_write_csr64(oct,CN63XX_SLI_IQ_BASE_ADDR64(iq_no),iq->base_addr_dma);
	octeon_write_csr(oct, CN63XX_SLI_IQ_SIZE(iq_no), iq->max_count);

	/* Remember the doorbell & instruction count register addr for this queue */
	iq->doorbell_reg = (uint8_t *)oct->mmio[0].hw_addr 
	                             + CN63XX_SLI_IQ_DOORBELL(iq_no);
	iq->inst_cnt_reg = (uint8_t *)oct->mmio[0].hw_addr
	                             + CN63XX_SLI_IQ_INSTR_COUNT(iq_no);
	cavium_print(PRINT_DEBUG,"InstQ[%d]:dbell reg @ 0x%p instcnt_reg @ 0x%p\n",
	             iq_no, iq->doorbell_reg, iq->inst_cnt_reg);

	/* Store the current instruction counter (used in flush_iq calculation) */
	iq->reset_instr_cnt = OCTEON_READ32(iq->inst_cnt_reg);
 
	/* Backpressure for this queue - WMARK set to all F's. This effectively
	   disables the backpressure mechanism. */
	octeon_write_csr64(oct, CN63XX_SLI_IQ_BP64(iq_no), (0xFFFFFFFFULL << 32));
}






static void
cn63xx_setup_oq_regs(octeon_device_t   *oct, int   oq_no)
{
	uint32_t        intr;
	octeon_droq_t  *droq = oct->droq[oq_no];

	octeon_write_csr64(oct, CN63XX_SLI_OQ_BASE_ADDR64(oq_no), droq->desc_ring_dma);
	octeon_write_csr(oct, CN63XX_SLI_OQ_SIZE(oq_no), droq->max_count);

	octeon_write_csr(oct, CN63XX_SLI_OQ_BUFF_INFO_SIZE(oq_no),
	        (droq->buffer_size | (OCT_RESP_HDR_SIZE << 16)));

	/* Get the mapped address of the pkt_sent and pkts_credit regs */
	droq->pkts_sent_reg = (uint8_t *)oct->mmio[0].hw_addr + CN63XX_SLI_OQ_PKTS_SENT(oq_no);
	droq->pkts_credit_reg = (uint8_t *)oct->mmio[0].hw_addr + CN63XX_SLI_OQ_PKTS_CREDIT(oq_no);

	/* Enable this output queue to generate Packet Timer Interrupt */
	intr = octeon_read_csr(oct, CN63XX_SLI_PKT_TIME_INT_ENB);
	intr |= (1 << oq_no);
	octeon_write_csr(oct, CN63XX_SLI_PKT_TIME_INT_ENB, intr);

	/* Enable this output queue to generate Packet Count Interrupt
	intr = octeon_read_csr(oct, CN63XX_SLI_PKT_CNT_INT_ENB);
	intr |= (1 << oq_no);
	octeon_write_csr(oct, CN63XX_SLI_PKT_CNT_INT_ENB, intr); */
}







static void
cn63xx_enable_io_queues(octeon_device_t  *oct)
{
	octeon_write_csr(oct, CN63XX_SLI_PKT_INSTR_SIZE, oct->io_qmask.iq64B);
	octeon_write_csr(oct, CN63XX_SLI_PKT_INSTR_ENB,  oct->io_qmask.iq);
	octeon_write_csr(oct, CN63XX_SLI_PKT_OUT_ENB,    oct->io_qmask.oq);
}



static void
cn63xx_disable_io_queues(octeon_device_t  *oct)
{
	uint32_t            mask, i, loop = CAVIUM_TICKS_PER_SEC;
	volatile uint32_t   d32;

	/* Reset the Enable bits for Input Queues. */
	octeon_write_csr(oct, CN63XX_SLI_PKT_INSTR_ENB, 0);

	/* Wait until hardware indicates that the queues are out of reset. */
	mask = oct->io_qmask.iq;
	d32 = octeon_read_csr(oct, CN63XX_SLI_PORT_IN_RST_IQ);
	while( ((d32 & mask) != mask) && loop--) {
		d32 = octeon_read_csr(oct, CN63XX_SLI_PORT_IN_RST_IQ);
		cavium_sleep_timeout(1);
	}

	/* Reset the doorbell register for each Input queue. */
	for(i = 0; i < oct->num_iqs; i++) {
		octeon_write_csr(oct, CN63XX_SLI_IQ_DOORBELL(i), 0xFFFFFFFF);
		d32 = octeon_read_csr(oct, CN63XX_SLI_IQ_DOORBELL(i));
	}

	/* Reset the Enable bits for Output Queues. */
	octeon_write_csr(oct, CN63XX_SLI_PKT_OUT_ENB, 0);

	/* Wait until hardware indicates that the queues are out of reset. */
	loop = CAVIUM_TICKS_PER_SEC;
	mask = oct->io_qmask.oq;
	d32 = octeon_read_csr(oct, CN63XX_SLI_PORT_IN_RST_OQ);
	while( ((d32 & mask) != mask) && loop--) {
		d32 = octeon_read_csr(oct, CN63XX_SLI_PORT_IN_RST_OQ);
		cavium_sleep_timeout(1);
	};
	

	/* Reset the doorbell register for each Output queue. */
	for(i = 0; i < oct->num_oqs; i++) {
		octeon_write_csr(oct, CN63XX_SLI_OQ_PKTS_CREDIT(i), 0xFFFFFFFF);
		d32 = octeon_read_csr(oct, CN63XX_SLI_OQ_PKTS_CREDIT(i));

		d32 = octeon_read_csr(oct, CN63XX_SLI_OQ_PKTS_SENT(i));
		octeon_write_csr(oct, CN63XX_SLI_OQ_PKTS_SENT(i), d32);
	}

	d32 = octeon_read_csr(oct, CN63XX_SLI_PKT_CNT_INT);
	if(d32)
		octeon_write_csr(oct, CN63XX_SLI_PKT_CNT_INT, d32);

	d32 = octeon_read_csr(oct, CN63XX_SLI_PKT_TIME_INT);
	if(d32)
		octeon_write_csr(oct, CN63XX_SLI_PKT_TIME_INT, d32);
}








static void
cn63xx_handle_pcie_error_intr(octeon_device_t  *oct, uint64_t intr64)
{
	cavium_error("OCTEON[%d]: Error Intr: 0x%016llx\n",
	              oct->octeon_id, CVM_CAST64(intr64));
}






int
cn63xx_droq_intr_handler(octeon_device_t  *oct)
{
	octeon_droq_t   *droq;
	uint32_t         oq_no, pkt_count, schedule = 0;
	uint32_t         droq_time_mask, droq_mask;

	droq_time_mask = octeon_read_csr(oct, CN63XX_SLI_PKT_TIME_INT);

	droq_mask = droq_time_mask;

	for (oq_no = 0; oq_no < oct->num_oqs; oq_no++) {
		if ( !(droq_mask & (1 << oq_no)) )
			continue;

		droq = oct->droq[oq_no];
		pkt_count = octeon_droq_check_hw_for_pkts(oct, droq);
		if(pkt_count) {
			if(droq->ops.poll_mode) {
				schedule = 0;
				droq->ops.napi_fn(oct->octeon_id, oq_no, POLL_EVENT_INTR_ARRIVED);
				oct->napi_mask |= (1 << oq_no); 
			} else {
				schedule = 1;
				#if defined(USE_DROQ_THREADS)
				cavium_wakeup(&droq->wc);
				#endif
			}
		} else {
			//cavium_error("OCTEON[%d]: Interrupt with no DROQ[%d] packets\n",
			//             oct->octeon_id, oq_no);
		}
	}

	/* Reset the PKT_CNT/TIME_INT registers. */
	if(droq_time_mask)
		octeon_write_csr(oct, CN63XX_SLI_PKT_TIME_INT, droq_time_mask);

	if(oct->napi_mask) {
		octeon_cn63xx_t  *cn63xx = (octeon_cn63xx_t  *)oct->chip;
		/* Stop any more pkt time interrupts for this DROQ. They will be
		   enabled when the poll function has fetched in all packets. */
		cn63xx->intr_mask64 &= ~(CN63XX_INTR_PKT_DATA);
	}

#if !defined(USE_DROQ_THREADS)
	if(schedule)
		cavium_tasklet_schedule(&oct->droq_tasklet);
#endif

	return 0;
}




cvm_intr_return_t
cn63xx_interrupt_handler(void  *dev)
{
	octeon_device_t  *oct    = (octeon_device_t  *)dev;
	octeon_cn63xx_t  *cn63xx = (octeon_cn63xx_t  *)oct->chip;
	uint64_t          intr64;

	cavium_print(PRINT_FLOW," In %s octeon_dev @ %p  \n", __CVM_FUNCTION__, oct);
	intr64 = OCTEON_READ64(cn63xx->intr_sum_reg64);

	/* If our device has interrupted, then proceed. */
	if (!intr64)
		return CVM_INTR_NONE;

	cavium_atomic_set(&oct->in_interrupt, 1);

	/* Disable our interrupts for the duration of ISR */
	oct->fn_list.disable_interrupt(oct->chip);

	oct->stats.interrupts++;

	cavium_atomic_inc(&oct->interrupts);

	if(intr64 & CN63XX_INTR_ERR)
		cn63xx_handle_pcie_error_intr(oct, intr64);

	if(intr64 & CN63XX_INTR_PKT_DATA)
		cn63xx_droq_intr_handler(oct);

#ifdef CVMCS_DMA_IC

	if (intr64 & (CN63XX_INTR_DMA0_COUNT | CN63XX_INTR_DMA0_TIME)) {
		uint64_t loc_dma_cnt = octeon_read_csr(oct, CN63XX_DMA_CNT_START);
		cavium_print (PRINT_DEBUG, "DMA Count %lx timer: %lx intr: %lx\n", 
				octeon_read_csr(oct, CN63XX_DMA_CNT_START), 
				octeon_read_csr(oct, CN63XX_DMA_TIM_START), intr64);
		cavium_atomic_add(loc_dma_cnt, &oct->dma_cnt_to_process);
		/* Acknowledge from host, we are going to read 
		   loc_dma_cnt packets from DMA. */
		octeon_write_csr (oct, CN63XX_DMA_CNT_START, loc_dma_cnt);
		cavium_tasklet_schedule(&oct->comp_tasklet);
	}
#else
	if(intr64 & (CN63XX_INTR_DMA0_FORCE|CN63XX_INTR_DMA1_FORCE))
		cavium_tasklet_schedule(&oct->comp_tasklet);
#endif

	if((intr64 & CN63XX_INTR_DMA_DATA) && (oct->dma_ops.intr_handler)) {
		oct->dma_ops.intr_handler((void *)oct, intr64);
	}

	/* Clear the current interrupts */
	OCTEON_WRITE64(cn63xx->intr_sum_reg64, intr64);

	/* Re-enable our interrupts  */
	oct->fn_list.enable_interrupt(oct->chip);

	cavium_atomic_set(&oct->in_interrupt, 0);

	return CVM_INTR_HANDLED;
}





static void
cn63xx_reinit_regs(octeon_device_t  *oct)
{
	int               i;

	cavium_print_msg("-- %s =--\n", __CVM_FUNCTION__);

	for(i = 0; i < oct->num_iqs; i++)
		oct->fn_list.setup_iq_regs(oct, i);

	for(i = 0; i < oct->num_oqs; i++)
		oct->fn_list.setup_oq_regs(oct, i);

	oct->fn_list.setup_device_regs(oct);

	oct->fn_list.enable_interrupt(oct->chip);

	oct->fn_list.enable_io_queues(oct);

	for(i = 0; i < oct->num_oqs; i++)
		OCTEON_WRITE32(oct->droq[i]->pkts_credit_reg, oct->droq[i]->max_count);

}






static void
cn63xx_bar1_idx_setup(octeon_device_t  *oct, uint64_t core_addr, int idx, int valid)
{
	volatile uint64_t bar1;
 
	if(valid == 0) {
		bar1 = OCTEON_PCI_WIN_READ(oct, CN63XX_BAR1_INDEX_REG(idx));
		OCTEON_PCI_WIN_WRITE(oct, CN63XX_BAR1_INDEX_REG(idx), (bar1 & 0xFFFFFFFEULL));
		bar1 = OCTEON_PCI_WIN_READ(oct, CN63XX_BAR1_INDEX_REG(idx));
		return;
	}

	/* Bits 17:4 of the PCI_BAR1_INDEXx stores bits 35:22 of the Core Addr */
	OCTEON_PCI_WIN_WRITE(oct, CN63XX_BAR1_INDEX_REG(idx),
	                 (((core_addr >> 22) << 4) | PCI_BAR1_MASK) );

	bar1 = OCTEON_PCI_WIN_READ(oct, CN63XX_BAR1_INDEX_REG(idx));
}





static void
cn63xx_bar1_idx_write(octeon_device_t  *oct, int idx, uint32_t  mask)
{
	OCTEON_PCI_WIN_WRITE(oct, CN63XX_BAR1_INDEX_REG(idx), mask);
}




static uint32_t
cn63xx_bar1_idx_read(octeon_device_t  *oct, int idx)
{
	return OCTEON_PCI_WIN_READ(oct, CN63XX_BAR1_INDEX_REG(idx));
}





static uint32_t
cn63xx_update_read_index(octeon_instr_queue_t  *iq)
{
	uint32_t  new_idx = OCTEON_READ32(iq->inst_cnt_reg);

	/* The new instr cnt reg is a 32-bit counter that can roll over. We have
	   noted the counter's initial value at init time into reset_instr_cnt */
	if(iq->reset_instr_cnt < new_idx)
		new_idx -= iq->reset_instr_cnt;
	else
		new_idx +=  ( 0xffffffff - iq->reset_instr_cnt) + 1;

	/* Modulo of the new index with the IQ size will give us the new index. */
	new_idx %= iq->max_count;

	return new_idx;
}






static void
cn63xx_enable_interrupt(void  *chip)
{
	octeon_cn63xx_t  *cn63xx = (octeon_cn63xx_t  *)chip;

	/* Enable Interrupt */
#ifdef CVMCS_DMA_IC
	OCTEON_WRITE64(cn63xx->intr_enb_reg64, cn63xx->intr_mask64);
#else
	OCTEON_WRITE64(cn63xx->intr_enb_reg64,
		(cn63xx->intr_mask64 | CN63XX_INTR_DMA0_FORCE));
#endif
}




static void
cn63xx_disable_interrupt(void  *chip)
{
	octeon_cn63xx_t  *cn63xx = (octeon_cn63xx_t  *)chip;

	/* Disable Interrupts */
	OCTEON_WRITE64(cn63xx->intr_enb_reg64, 0);
}







static void
cn63xx_get_pcie_qlmport(octeon_device_t  *oct)
{
	if(oct->rev_id >= 8) {
		/* CN63xx Pass2 implements the SLI_MAC_NUMBER register to determine the
		   PCIE port # */
		oct->pcie_port = octeon_read_csr(oct, CN63XX_SLI_MAC_NUMBER) & 0xff;
	} else {
		/* CN63xx Pass1 - The expansion ROM always returns 0 on reads on PCIE
		   port 1. We use this to distinguish between the ports. */
		uint32_t   regval;

		OCTEON_READ_PCI_CONFIG(oct, CN63XX_CONFIG_XPANSION_BAR, &regval);
		OCTEON_WRITE_PCI_CONFIG(oct, CN63XX_CONFIG_XPANSION_BAR, (regval | (0x1234 << 16)));
		OCTEON_READ_PCI_CONFIG(oct, CN63XX_CONFIG_XPANSION_BAR, &regval);
	
		/* Expansion ROM on pcieport 1 always returns 0 */
		oct->pcie_port = (regval)?0:1;

		OCTEON_WRITE_PCI_CONFIG(oct, CN63XX_CONFIG_XPANSION_BAR, regval);
	}

	cavium_print_msg("OCTEON: CN63xx uses PCIE Port %d\n", oct->pcie_port);
}







static void
cn63xx_setup_reg_address(octeon_device_t   *oct)
{
	octeon_cn63xx_t *cn63xx = (octeon_cn63xx_t *)oct->chip;
	uint8_t         *bar0_pciaddr = (uint8_t *)oct->mmio[0].hw_addr;

	oct->reg_list.pci_win_wr_addr_hi = (uint32_t *)(bar0_pciaddr + CN63XX_WIN_WR_ADDR_HI);
	oct->reg_list.pci_win_wr_addr_lo = (uint32_t *)(bar0_pciaddr + CN63XX_WIN_WR_ADDR_LO);
	oct->reg_list.pci_win_wr_addr    = (uint64_t *)(bar0_pciaddr + CN63XX_WIN_WR_ADDR64);

	oct->reg_list.pci_win_rd_addr_hi = (uint32_t *)(bar0_pciaddr + CN63XX_WIN_RD_ADDR_HI);
	oct->reg_list.pci_win_rd_addr_lo = (uint32_t *)(bar0_pciaddr + CN63XX_WIN_RD_ADDR_LO);
	oct->reg_list.pci_win_rd_addr    = (uint64_t *)(bar0_pciaddr + CN63XX_WIN_RD_ADDR64);

	oct->reg_list.pci_win_wr_data_hi = (uint32_t *)(bar0_pciaddr + CN63XX_WIN_WR_DATA_HI);
	oct->reg_list.pci_win_wr_data_lo = (uint32_t *)(bar0_pciaddr + CN63XX_WIN_WR_DATA_LO);
	oct->reg_list.pci_win_wr_data    = (uint64_t *)(bar0_pciaddr + CN63XX_WIN_WR_DATA64);

	oct->reg_list.pci_win_rd_data_hi = (uint32_t *)(bar0_pciaddr + CN63XX_WIN_RD_DATA_HI);
	oct->reg_list.pci_win_rd_data_lo = (uint32_t *)(bar0_pciaddr + CN63XX_WIN_RD_DATA_LO);
	oct->reg_list.pci_win_rd_data    = (uint64_t *)(bar0_pciaddr + CN63XX_WIN_RD_DATA64);

	cn63xx_get_pcie_qlmport(oct);

	cn63xx->intr_sum_reg64 = bar0_pciaddr + CN63XX_SLI_INT_SUM64;
	cn63xx->intr_mask64    = CN63XX_INTR_MASK;
	cn63xx->intr_enb_reg64 = bar0_pciaddr + CN63XX_SLI_INT_ENB64(oct->pcie_port);
}



int
setup_cn63xx_octeon_device(octeon_device_t  *oct)
{
	octeon_cn63xx_t *cn63xx = (octeon_cn63xx_t *)oct->chip;

	if(octeon_map_pci_barx(oct, 0, 0))
		return 1;

	if(octeon_map_pci_barx(oct, 1, MAX_BAR1_IOREMAP_SIZE)) {
		cavium_error("%s CN63XX BAR1 map failed\n", __FUNCTION__);
		octeon_unmap_pci_barx(oct, 0);
		return 1;
	}

	cn63xx->conf  = (cn63xx_config_t  *)oct_get_config_info(oct);
	if(cn63xx->conf == NULL) {
		cavium_error("%s No Config found for CN63XX\n", __FUNCTION__);
		octeon_unmap_pci_barx(oct, 0);
		octeon_unmap_pci_barx(oct, 1);
		return 1;
	}

	oct->fn_list.setup_iq_regs      = cn63xx_setup_iq_regs;
	oct->fn_list.setup_oq_regs      = cn63xx_setup_oq_regs;

	oct->fn_list.interrupt_handler  = cn63xx_interrupt_handler;
	oct->fn_list.soft_reset         = cn63xx_soft_reset;
	oct->fn_list.setup_device_regs  = cn63xx_setup_device_regs;
	oct->fn_list.reinit_regs        = cn63xx_reinit_regs;
	oct->fn_list.update_iq_read_idx = cn63xx_update_read_index;

	oct->fn_list.bar1_idx_setup     = cn63xx_bar1_idx_setup;
	oct->fn_list.bar1_idx_write     = cn63xx_bar1_idx_write;
	oct->fn_list.bar1_idx_read      = cn63xx_bar1_idx_read;

	oct->fn_list.enable_interrupt   = cn63xx_enable_interrupt;
	oct->fn_list.disable_interrupt  = cn63xx_disable_interrupt;

	oct->fn_list.enable_io_queues   = cn63xx_enable_io_queues;
	oct->fn_list.disable_io_queues  = cn63xx_disable_io_queues;

	cn63xx_setup_reg_address(oct);

	cn63xx_errata_sli14401_fix(oct);


	return 0;
}





int
validate_cn63xx_config_info(cn63xx_config_t  *conf63xx)
{
	int i, total_instrs = 0;

	if(conf63xx->c.num_iqs > CN63XX_MAX_INPUT_QUEUES) {
		cavium_error("%s: Num IQ (%d) exceeds Max (%d)\n", __CVM_FUNCTION__,
		             conf63xx->c.num_iqs, CN63XX_MAX_INPUT_QUEUES);
		return 1;
	}


	if(conf63xx->c.num_oqs > CN63XX_MAX_OUTPUT_QUEUES) {
		cavium_error("%s: Num IQ (%d) exceeds Max (%d)\n", __CVM_FUNCTION__,
		             conf63xx->c.num_iqs, CN63XX_MAX_INPUT_QUEUES);
		return 1;
	}


	for(i = 0; i < conf63xx->c.num_iqs; i++) {
		if( (conf63xx->iq[i].instr_type != 32) &&
		    (conf63xx->iq[i].instr_type != 64) ) {
			cavium_error("%s: Invalid instr type (%d) for IQ[%d]\n",
			              __CVM_FUNCTION__, conf63xx->iq[i].instr_type, i);
			return 1;
		}
		if( !(conf63xx->iq[i].num_descs) || !(conf63xx->iq[i].db_min)
		    || !(conf63xx->iq[i].db_timeout) ) {
			cavium_error("%s: Invalid parameter for IQ[%d]\n",
			             __CVM_FUNCTION__, i);
			return 1;
		}
		total_instrs += conf63xx->iq[i].num_descs;
	}

	if(conf63xx->c.pending_list_size < total_instrs) {
		cavium_error("%s Pending list size (%d) should be >= total instructions queue size (%d)\n",
		           __CVM_FUNCTION__, conf63xx->c.pending_list_size, total_instrs);
		return 1;
	}

	for(i = 0; i < conf63xx->c.num_oqs; i++) {
		if(!(conf63xx->oq[i].info_ptr)  || !(conf63xx->oq[i].pkts_per_intr) ||
		   !(conf63xx->oq[i].num_descs) || !(conf63xx->oq[i].refill_threshold) ||
		   !(conf63xx->oq[i].buf_size)   ){
			cavium_error("%s: Invalid parameter for OQ[%d]\n",
			             __CVM_FUNCTION__, i);
			return 1;
		}
	}

	if(!(conf63xx->oq_intr_time) ) {
		cavium_error("%s: No Time Interrupt for OQ[%d]\n", __CVM_FUNCTION__, i);
		return 1;
	}

	return 0;
}


void
cn63xx_dump_dpi_regs(octeon_device_t   *oct)
{
	int i;

	cavium_print_msg("DPI_CTL [0x%llx]: 0x%016llx\n",CN63XX_DPI_CTL,
		CVM_CAST64(OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_CTL)));

	cavium_print_msg("DPI_DMA_CONTROL [0x%llx]: 0x%016llx\n",CN63XX_DPI_DMA_CONTROL,
		CVM_CAST64(OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_DMA_CONTROL)));

	cavium_print_msg("DPI_REQ_GBL_EN [0x%llx]: 0x%016llx\n",CN63XX_DPI_REQ_GBL_ENB,
		CVM_CAST64(OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_REQ_GBL_ENB)));

	cavium_print_msg("DPI_REQ_ERR_RSP [0x%llx]: 0x%016llx\n",CN63XX_DPI_REQ_ERR_RSP,
		CVM_CAST64(OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_REQ_ERR_RSP)));

	cavium_print_msg("DPI_REQ_ERR_RST [0x%llx]: 0x%016llx\n",CN63XX_DPI_REQ_ERR_RST,
		CVM_CAST64(OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_REQ_ERR_RST)));

	for(i = 0; i < 6; i++) {
		cavium_print_msg("DPI_DMA_ENG%d_EN [0x%llx]: 0x%016llx\n", i, CN63XX_DPI_DMA_ENG_ENB(i),
			CVM_CAST64(OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_DMA_ENG_ENB(i))));
	}

	for(i = 0; i < 6; i++) {
		cavium_print_msg("DPI_DMA_ENG%d_BUF [0x%llx]: 0x%016llx\n", i, CN63XX_DPI_DMA_ENG_BUF(i),
			CVM_CAST64(OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_DMA_ENG_BUF(i))));
	}

	cavium_print_msg("DPI_SLI_PRT0_CFG [0x%llx]: 0x%016llx\n",CN63XX_DPI_SLI_PRT0_CFG,
		CVM_CAST64(OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_SLI_PRT0_CFG)));

}



void
cn63xx_dump_iq_regs(octeon_device_t  *oct)
{
	uint32_t  regval;

	cavium_print_msg("SLI_IQ_DOORBELL_0 [0x%x]: 0x%016llx\n",CN63XX_SLI_IQ_DOORBELL(0),
		CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_IQ_DOORBELL(0))));

	cavium_print_msg("SLI_IQ_PKT_INSTR_HDR_0 [0x%x]: 0x%016llx\n",
		CN63XX_SLI_IQ_PKT_INSTR_HDR64(0),
		CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_IQ_PKT_INSTR_HDR64(0))));

	cavium_print_msg("SLI_IQ_BASEADDR_0 [0x%x]: 0x%016llx\n",CN63XX_SLI_IQ_BASE_ADDR64(0),
		CVM_CAST64(octeon_read_csr64(oct,CN63XX_SLI_IQ_BASE_ADDR64(0))));

	cavium_print_msg("SLI_IQ_FIFO_RSIZE_0 [0x%x]: 0x%016llx\n",CN63XX_SLI_IQ_SIZE(0),
		CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_IQ_SIZE(0))));

	cavium_print_msg("SLI_IQ_BP_0 [0x%x]: 0x%016llx\n",CN63XX_SLI_IQ_BP64(0),
		CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_IQ_BP64(0))));

	cavium_print_msg("SLI_PKT_INSTR_ENB [0x%x]: 0x%016llx\n", CN63XX_SLI_PKT_INSTR_ENB,
		 CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_PKT_INSTR_ENB)));

	cavium_print_msg("SLI_PKT_INSTR_Size [0x%x]: 0x%016llx\n",CN63XX_SLI_PKT_INSTR_SIZE,
		CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_PKT_INSTR_SIZE)));

	cavium_print_msg("SLI_CTL_STATUS [0x%x]: 0x%016llx\n",CN63XX_SLI_CTL_STATUS,
		CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_CTL_STATUS)));

	cavium_print_msg("SLI_PKT_INPUT_CTL [0x%x]: 0x%016llx\n",CN63XX_SLI_PKT_INPUT_CONTROL,
		CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_PKT_INPUT_CONTROL)));

	cavium_print_msg("SLI_PKT_INSTR_RD_SIZE [0x%x]: 0x%016llx\n",CN63XX_SLI_PKT_INSTR_RD_SIZE,
		CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_PKT_INSTR_RD_SIZE)));

	cavium_print_msg("SLI_IN_PCIE_PORT [0x%x]: 0x%016llx\n",CN63XX_SLI_IN_PCIE_PORT,
		CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_IN_PCIE_PORT)));

	OCTEON_READ_PCI_CONFIG(oct, CN63XX_CONFIG_PCIE_DEVCTL, &regval);
	cavium_print_msg("Config DevCtl [0x%x]: 0x%08x\n",CN63XX_CONFIG_PCIE_DEVCTL,
	       regval);

	cavium_print_msg("SLI_PRT0_CFG [0x%llx]: 0x%016llx\n",CN63XX_DPI_SLI_PRT0_CFG,
		CVM_CAST64(OCTEON_PCI_WIN_READ(oct, CN63XX_DPI_SLI_PRT0_CFG)));

	cavium_print_msg("SLI_S2M_PORT0_CTL [0x%x]: 0x%016llx\n",CN63XX_SLI_S2M_PORT0_CTL,
		CVM_CAST64(octeon_read_csr64(oct, CN63XX_SLI_S2M_PORT0_CTL)));

}



/* $Id: cn63xx_device.c 47454 2010-02-18 22:33:46Z panicker $ */
