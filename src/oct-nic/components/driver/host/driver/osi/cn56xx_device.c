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
#include "cn56xx_device.h"
#include "octeon_macros.h"
#include "octeon-pci.h"




void
cn56xx_detect_pcie_phylayer(octeon_device_t   *oct)
{
	uint32_t   cfg;	

	OCTEON_READ_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_LINKCTL, &cfg);

	cavium_print_msg("OCTEON[%d]: Detected %d PCI-E lanes at %s\n",
	                 oct->octeon_id, ((cfg >> 20) & 0x3f),
	                 (((cfg >> 16) & 0xf) == 1)?"2.5 Gbps":"Unknown speed");

}




static int
cn56xx_pass2_soft_reset(octeon_device_t   *oct)
{
	octeon_write_csr64(oct, CN56XX_WIN_WR_MASK_REG, 0);

	cavium_print_msg("OCTEON[%d]: BIST enabled for CN56XX soft reset\n",
	                 oct->octeon_id);
	OCTEON_PCI_WIN_WRITE(oct, CN56XX_CIU_SOFT_BIST, 1);

	octeon_write_csr64(oct, CN56XX_NPEI_SCRATCH, 0x1234ULL);

	OCTEON_PCI_WIN_READ(oct, CN56XX_CIU_SOFT_RST);

	OCTEON_PCI_WIN_WRITE(oct, CN56XX_CIU_SOFT_RST, 1);

	/* Wait for 10ms as Octeon resets. */
	cavium_mdelay(10);

	if(octeon_read_csr64(oct, CN56XX_NPEI_SCRATCH) == 0x1234ULL) {
		cavium_error("OCTEON[%d]: Soft reset failed\n", oct->octeon_id);
		return 1;
	}

	cavium_print_msg("OCTEON[%d]: Reset completed\n", oct->octeon_id);
	octeon_write_csr64(oct, CN56XX_WIN_WR_MASK_REG, 0);

	return 0;
}





void
cn56xx_enable_error_reporting(octeon_device_t   *oct)
{
	uint32_t   regval;

	OCTEON_READ_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_DEVCTL, &regval);
	if(regval & 0x000f0000) {
		cavium_error("PCI-E Link error detected: 0x%08x\n",regval & 0x000f0000);
	}

	regval |= 0xf;  /* Enable Link error reporting */

	cavium_print_msg("OCTEON[%d]: Enabling PCI-E error reporting..\n",
	                 oct->octeon_id);
	cavium_print(PRINT_FLOW, "Writing  0x%08x to config[0x78]\n", regval);
	OCTEON_WRITE_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_DEVCTL, regval);
}





static void
cn56xx_setup_pcie_mps(octeon_device_t   *oct, enum octeon_pcie_mps  mps)
 {
 	uint32_t   regval;
	volatile uint64_t   r64;
 
	/* Read config register for MPS */
 	OCTEON_READ_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_DEVCTL, &regval);
 
	if(mps == PCIE_MPS_DEFAULT) {
		mps = ((regval & (0x7 << 5)) >> 5);
	} else {
		regval &= ~(0x7 << 5);    // Turn off any MPS bits
		regval |=  (mps << 5);    // Set MPS
		OCTEON_WRITE_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_DEVCTL, regval);
	}

	/* Set NPEI_CTL_STATUS_2 to the same setting as PCIE_DEVCTL config */
	r64  = octeon_read_csr64(oct, CN56XX_NPEI_CTL_STATUS_2);

	r64 |= (mps << 15);     // Set MPS
	octeon_write_csr64(oct, CN56XX_NPEI_CTL_STATUS_2, r64);
}




static void
cn56xx_setup_pcie_mrrs(octeon_device_t   *oct, enum octeon_pcie_mrrs mrrs)
{
	uint32_t   regval;
	volatile uint64_t   r64;

	/* Read config register for MRRS */
	OCTEON_READ_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_DEVCTL, &regval);

	if(mrrs == PCIE_MRRS_DEFAULT) {
		mrrs = ((regval & (0x7 << 12)) >> 12);
	} else {
		regval &= ~(0x7 << 12);   // Turn off any MRRS bits
		regval |=  (mrrs << 12);  // Set MRRS
		OCTEON_WRITE_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_DEVCTL, regval);
	}
 
 	/* Set NPEI_CTL_STATUS_2 to the same setting as PCIE_DEVCTL config */
 	r64  = octeon_read_csr64(oct, CN56XX_NPEI_CTL_STATUS_2);
 
 	r64 &= ~(0x7 << 12);    // Turn off any MRRS bits
 	r64 |=  (mrrs << 12);   // Set MRRS
 
 	octeon_write_csr64(oct, CN56XX_NPEI_CTL_STATUS_2, r64);
}








uint32_t
cn56xx_pass2_core_clock(octeon_device_t  *oct)
{
	uint32_t   clk;

	clk = octeon_read_csr(oct, CN56XX_NPEI_DBG_DATA);

	/* We are interested only in bits 18-22 for the clock multiplier. */
	clk >>= 18;
	clk &=  0x1F;

	/* Multiply by 50 to get the actual core clock frequency. */
	clk *=  50;

	cavium_print_msg("OCTEON[%d]: CN56XX Pass2 Core Clock: %u Mhz\n",
	                 oct->octeon_id, clk);

	return clk;
}





uint32_t
cn56xx_pass2_get_oq_ticks(octeon_device_t  *oct, uint32_t  time_intr_in_us)
{
	/* This gives the core clock per microsec */
	uint32_t  oqticks_per_us = cn56xx_pass2_core_clock(oct);

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
cn56xx_setup_global_input_regs(octeon_device_t  *oct)
{
	/// Select Round-Robin Arb, ES, RO, NS for Input Queues
	octeon_write_csr(oct, CN56XX_NPEI_PKT_INPUT_CONTROL, CN56XX_INPUT_CTL_MASK);

	/* Instruction Read Size - Max 4 instructions per PCIE Read */
	octeon_write_csr64(oct, CN56XX_NPEI_PKT_INSTR_RD_SIZE,
	                   0xFFFFFFFFFFFFFFFFULL);

	/* Select PCIE Port 0 for all Input rings. */
	octeon_write_csr64(oct, CN56XX_NPEI_IN_PCIE_PORT, 0);
}



void
cn56xx_setup_global_output_regs(octeon_device_t  *oct)
{
	uint32_t          time_threshold;
	cn56xx_config_t  *conf56xx = ((octeon_cn56xx_t *)oct->chip)->conf;

	if(conf56xx->c.num_oqs <= 4) {
		uint64_t   ctl_sts = octeon_read_csr64(oct, CN56XX_NPEI_CTL_STATUS);

		/* Disable RING_EN if only upto 4 rings are used. */
		ctl_sts &= 0xFFFFFFFFFFFF7FFFULL;
		octeon_write_csr64(oct, CN56XX_NPEI_CTL_STATUS, ctl_sts);
	} else {
		uint64_t   ctl_sts = octeon_read_csr64(oct, CN56XX_NPEI_CTL_STATUS);

		/* Enable RING_EN if more than 4 rings are used. */
		ctl_sts |= 0x8000ULL;
		octeon_write_csr64(oct, CN56XX_NPEI_CTL_STATUS, ctl_sts);
	}

	/// Select PCI-E Port 0 for all Output queues
	octeon_write_csr64(oct, CN56XX_NPEI_PKT_PCIE_PORT64, 0ULL);

	/// Select Info Ptr for length & data
	octeon_write_csr(oct, CN56XX_NPEI_PKT_IPTR, 0xFFFFFFFF);

	/// Select Packet count for Output Queue Interrupt.
	octeon_write_csr(oct, CN56XX_NPEI_PKT_OUT_BMODE, 0);

	/// Select ES,RO,NS setting from register for Output Queue Packet Address
	octeon_write_csr(oct, CN56XX_NPEI_PKT_DPADDR, 0xFFFFFFFF);

	/// No Relaxed Ordering, No Snoop, 64-bit swap for Output Queue ScatterList
	octeon_write_csr(oct, CN56XX_NPEI_PKT_SLIST_ROR, 0);
	octeon_write_csr(oct, CN56XX_NPEI_PKT_SLIST_NS, 0);
	
	/// ENDIAN_SPECIFIC CHANGES - 0 works for LE.
#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
	octeon_write_csr64(oct, CN56XX_NPEI_PKT_SLIST_ES64, 0ULL);
#else
	octeon_write_csr64(oct, CN56XX_NPEI_PKT_SLIST_ES64, 0x5555555555555555ULL);
#endif


	/// No Relaxed Ordering, No Snoop, 64-bit swap for Output Queue Data
	octeon_write_csr(oct, CN56XX_NPEI_PKT_DATA_OUT_ROR, 0);
	octeon_write_csr(oct, CN56XX_NPEI_PKT_DATA_OUT_NS, 0);
	octeon_write_csr64(oct, CN56XX_NPEI_PKT_DATA_OUT_ES64, 0x5555555555555555ULL);


	/// Buffer size is same for all output rings.
	/// Info ptr holds the response header
	octeon_write_csr(oct, CN56XX_NPEI_OQ_BUFF_INFO_SIZE, 
		(conf56xx->oq_buf_size | (OCT_RESP_HDR_SIZE << 16)));


	/// Set up interrupt packet and time threshold
	octeon_write_csr(oct, CN56XX_NPEI_OQ_INT_LEV_PKTS, conf56xx->oq_intr_pkt);

	time_threshold = cn56xx_pass2_get_oq_ticks(oct, conf56xx->oq_intr_time);
	octeon_write_csr(oct, CN56XX_NPEI_OQ_INT_LEV_TIME, time_threshold);
#ifdef CVMCS_DMA_IC
	/* This gives the core clock per microsec */
	time_threshold = cn56xx_pass2_core_clock(oct);
	time_threshold *= conf56xx->dma_intr_time;
	octeon_write_csr64 (oct, CN56XX_DMA_INT_LEVEL_START,
			    ((uint64_t)time_threshold << 32) |
			    conf56xx->dma_intr_pkt);
	cavium_print (PRINT_DEBUG, "CN56XX_DMA_INT_LEVEL_START: %lx\n",
		      octeon_read_csr64(oct, CN56XX_DMA_INT_LEVEL_START));
#endif
}






static int
cn56xx_setup_device_regs(octeon_device_t  *oct)
{
	uint64_t   val64;

	cn56xx_detect_pcie_phylayer(oct);

	cn56xx_setup_pcie_mps(oct, PCIE_MPS_DEFAULT);
	cn56xx_setup_pcie_mrrs(oct, PCIE_MRRS_512B);

	cn56xx_enable_error_reporting(oct);

	/* Disable Per-port Backpressure */
	val64 = octeon_read_csr64(oct, CN56XX_NPEI_CTL_STATUS);
	val64 &= 0xFFFFFFFFFFFFE1FFULL;
	octeon_write_csr64(oct, CN56XX_NPEI_CTL_STATUS, val64);

	cn56xx_setup_global_input_regs(oct);
	cn56xx_setup_global_output_regs(oct);

	return 0;
}







static void
cn56xx_setup_iq_regs(octeon_device_t   *oct, int   iq_no)
{
	octeon_instr_queue_t   *iq = oct->instr_queue[iq_no];

	/* Disable Packet-by-Packet mode; No Parse Mode or Skip length */
	octeon_write_csr64(oct, CN56XX_NPEI_IQ_PKT_INSTR_HDR64(iq_no), 0);


	/* Write the start of the input queue's ring and its size  */
	octeon_write_csr64(oct,CN56XX_NPEI_IQ_BASE_ADDR64(iq_no),iq->base_addr_dma);
	octeon_write_csr(oct, CN56XX_NPEI_IQ_SIZE(iq_no), iq->max_count);

	/* Remember the doorbell & instruction count register addr for this queue */
	iq->doorbell_reg = (uint8_t *)oct->mmio[0].hw_addr 
	                             + CN56XX_NPEI_IQ_DOORBELL(iq_no);
	iq->inst_cnt_reg = (uint8_t *)oct->mmio[0].hw_addr
	                             + CN56XX_NPEI_IQ_INSTR_COUNT(iq_no);
	cavium_print(PRINT_DEBUG,"InstQ[%d]:dbell reg @ 0x%p instcnt_reg @ 0x%p\n",
	             iq_no, iq->doorbell_reg, iq->inst_cnt_reg);

	/* Store the current instruction counter (used in flush_iq calculation) */
	iq->reset_instr_cnt = OCTEON_READ32(iq->inst_cnt_reg);
 
	/* Backpressure for this queue - WMARK set to all F's. This effectively
	   disables the backpressure mechanism. */
	octeon_write_csr64(oct, CN56XX_NPEI_IQ_BP64(iq_no), (0xFFFFFFFFULL << 32));
}






static void
cn56xx_setup_oq_regs(octeon_device_t   *oct, int   oq_no)
{
	octeon_droq_t  *droq = oct->droq[oq_no];

	octeon_write_csr64(oct, CN56XX_NPEI_OQ_BASE_ADDR64(oq_no), droq->desc_ring_dma);
	octeon_write_csr(oct, CN56XX_NPEI_OQ_SIZE(oq_no), droq->max_count);

	/* Get the mapped address of the pkt_sent and pkts_credit regs */
	droq->pkts_sent_reg = (uint8_t *)oct->mmio[0].hw_addr + CN56XX_NPEI_OQ_PKTS_SENT(oq_no);
	droq->pkts_credit_reg = (uint8_t *)oct->mmio[0].hw_addr + CN56XX_NPEI_OQ_PKTS_CREDIT(oq_no);

	/* Enable this output queue to generate Packet Timer Interrupt */
	octeon_write_csr(oct, CN56XX_NPEI_PKT_TIME_INT_ENB,
	     ( (1 << oq_no) | octeon_read_csr(oct, CN56XX_NPEI_PKT_TIME_INT_ENB)) );
}







static void
cn56xx_enable_io_queues(octeon_device_t  *oct)
{
	octeon_write_csr(oct, CN56XX_NPEI_PKT_INSTR_SIZE, oct->io_qmask.iq64B);
	octeon_write_csr(oct, CN56XX_NPEI_PKT_INSTR_ENB,  oct->io_qmask.iq);
	octeon_write_csr(oct, CN56XX_NPEI_PKT_OUT_ENB,    oct->io_qmask.oq);
}



static void
cn56xx_disable_io_queues(octeon_device_t  *oct)
{
	uint32_t            mask, i, loop = CAVIUM_TICKS_PER_SEC;
	volatile uint32_t   d32;

	/* Reset the Enable bits for Input Queues. */
	octeon_write_csr(oct, CN56XX_NPEI_PKT_INSTR_ENB, 0);

	/* Wait until hardware indicates that the queues are out of reset. */
	mask = oct->io_qmask.iq;
	d32 = octeon_read_csr(oct, CN56XX_NPEI_PORT_IN_RST_IQ);
	while( ((d32 & mask) != mask) && loop--) {
		d32 = octeon_read_csr(oct, CN56XX_NPEI_PORT_IN_RST_IQ);
		cavium_sleep_timeout(1);
	}

	/* Reset the doorbell register for each Input queue. */
	for(i = 0; i < oct->num_iqs; i++) {
		octeon_write_csr(oct, CN56XX_NPEI_IQ_DOORBELL(i), 0xFFFFFFFF);
		d32 = octeon_read_csr(oct, CN56XX_NPEI_IQ_DOORBELL(i));
	}


	/* Reset the Enable bits for Output Queues. */
	octeon_write_csr(oct, CN56XX_NPEI_PKT_OUT_ENB, 0);


	/* Wait until hardware indicates that the queues are out of reset. */
	loop = CAVIUM_TICKS_PER_SEC;
	mask = oct->io_qmask.oq;
	d32 = octeon_read_csr(oct, CN56XX_NPEI_PORT_IN_RST_OQ);
	while( ((d32 & mask) != mask) && loop--) {
		d32 = octeon_read_csr(oct, CN56XX_NPEI_PORT_IN_RST_OQ);
		cavium_sleep_timeout(1);
	};
	

	/* Reset the doorbell register for each Output queue. */
	for(i = 0; i < oct->num_oqs; i++) {
		octeon_write_csr(oct, CN56XX_NPEI_OQ_PKTS_CREDIT(i), 0xFFFFFFFF);
		d32 = octeon_read_csr(oct, CN56XX_NPEI_OQ_PKTS_CREDIT(i));
	}

}








static void
cn56xx_handle_pcie_error_intr(octeon_device_t  *oct, uint64_t intr64)
{
	cavium_error("OCTEON[%d]: Error Intr: 0x%016llx\n",
	              oct->octeon_id, CVM_CAST64(intr64));

	if(intr64 & CN56XX_INTR_ERR_OVERFLOW) {
		cavium_error("OCTEON[%d]: Overflow error\n", oct->octeon_id);
	}

	if(intr64 & CN56XX_INTR_ERR_MISC) {
		cavium_error("OCTEON[%d]: Misc error\n", oct->octeon_id);
	}

	if(intr64 & CN56XX_INTR_UNSUPPORTED_TLP) {
		cavium_error("OCTEON[%d]: Unsupported TLP error\n", oct->octeon_id);
	}

	if(intr64 & CN56XX_INTR_INT_A) {
		uint32_t   int_a;

		int_a = octeon_read_csr(oct, CN56XX_NPEI_INT_A_SUM64);
		octeon_write_csr64(oct, CN56XX_NPEI_INT_A_SUM64, (uint64_t)int_a);
		cavium_error("OCTEON[%d]: PCIE_INT_A error: 0x%08x\n",
		             oct->octeon_id, int_a);
	}

}




extern void
oct_dump_droq_state(octeon_droq_t   *oq);



/*
  Modify the interrupt mask for CN56XX devices. The new mask is written
  to the appropriate interrupt mask register.

  NOTE: The routine does not enable interrupts, but can trigger interrupts
        as a side-effect of enabling the packet interrupt mask register bits.
        So call this function only if you are sure you can handle interrupts
        from octeon or if interrupts are guaranteed to be disabled on octeon.

  Params:
      oct    = octeon device handle
      oq_no  = output queue index
      time   = set this param to 1 if the timer interrupt is being modified
               set it to 0 if the packet count interrupt is being modified
      state  = the new state; set to 1 if interrupt is being enabled;
               set to 0 if interrupt is being disabled.
*/

static inline void
cn56xx_modify_oq_pkt_intr(octeon_device_t   *oct, int oq_no, int time, int state)
{
	uint32_t            intrmask;
	octeon_cn56xx_t    *cn56xx = (octeon_cn56xx_t *)oct->chip;

	if(!cavium_atomic_read(&oct->in_interrupt))
		oct->fn_list.disable_interrupt(cn56xx);

	if(time) {
		intrmask = octeon_read_csr(oct, CN56XX_NPEI_PKT_TIME_INT_ENB);

		if(state) {
			if((1 << oq_no) & octeon_read_csr(oct, CN56XX_NPEI_PKT_TIME_INT))
				octeon_write_csr(oct, CN56XX_NPEI_PKT_TIME_INT, (1 << oq_no));

			/* Enable this output queue to generate Packet Timer Interrupt */
			intrmask |= (1 << oq_no);
		} else {
			/* Stop this output queue to generate Packet Timer Interrupt */
			intrmask &= ~(1 << oq_no);
		}
		octeon_write_csr(oct, CN56XX_NPEI_PKT_TIME_INT_ENB, intrmask);

	} else {
		intrmask = octeon_read_csr(oct, CN56XX_NPEI_PKT_CNT_INT_ENB);

		if(state) {
			if((1 << oq_no) & octeon_read_csr(oct, CN56XX_NPEI_PKT_CNT_INT))
				octeon_write_csr(oct, CN56XX_NPEI_PKT_CNT_INT, (1 << oq_no));

			/* Enable this output queue to generate Packet Count Interrupt */
			intrmask |= (1 << oq_no);
		} else {
			/* Stop this output queue to generate Packet Count Interrupt */
			intrmask &= ~(1 << oq_no);
		}
		octeon_write_csr(oct, CN56XX_NPEI_PKT_CNT_INT_ENB, intrmask);
	}

	if(!cavium_atomic_read(&oct->in_interrupt))
		oct->fn_list.enable_interrupt(cn56xx);
}





static inline void
cn56xx_enable_oq_pkt_time_intr(octeon_device_t   *oct, int oq_no)
{
	cn56xx_modify_oq_pkt_intr(oct, oq_no, 1, 1);
}



static inline void
cn56xx_enable_oq_pkt_cnt_intr(octeon_device_t   *oct, int oq_no)
{
	cn56xx_modify_oq_pkt_intr(oct, oq_no, 0, 1);
}



static inline void
cn56xx_disable_oq_pkt_time_intr(octeon_device_t   *oct, int oq_no)
{
	cn56xx_modify_oq_pkt_intr(oct, oq_no, 1, 0);
}


static inline void
cn56xx_disable_oq_pkt_cnt_intr(octeon_device_t   *oct, int oq_no)
{
	cn56xx_modify_oq_pkt_intr(oct, oq_no, 0, 0);
}


int
cn56xx_droq_intr_handler(octeon_device_t  *oct)
{
	octeon_droq_t    *droq;
	uint32_t          oq_no, pkt_count, schedule = 0;
	uint32_t          droq_time_mask, droq_mask;

	droq_time_mask = octeon_read_csr(oct, CN56XX_NPEI_PKT_TIME_INT);

	droq_mask = droq_time_mask;

	for (oq_no = 0; oq_no < oct->num_oqs; oq_no++) {
		if ( !(droq_mask & (1 << oq_no)) )
			continue;

		droq      = oct->droq[oq_no];
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
/*			cavium_error("OCTEON[%d]: Interrupt with no DROQ[%d] packets\n",
			             oct->octeon_id, oq_no);
			oct_dump_droq_state(droq);*/
		}

	}

	/* Reset the PKT_CNT/TIME_INT registers. */
	if(droq_time_mask)
		octeon_write_csr(oct, CN56XX_NPEI_PKT_TIME_INT, droq_time_mask);


	if(oct->napi_mask) {
		octeon_cn56xx_t  *cn56xx = (octeon_cn56xx_t  *)oct->chip;
		/* Stop any more pkt time interrupts for this DROQ. They will be
		   enabled when the poll function has fetched in all packets. */
		cn56xx->intr_mask64 &= ~(CN56XX_INTR_PKT_DATA);
	}


#if !defined(USE_DROQ_THREADS)
	if(schedule)
		cavium_tasklet_schedule(&oct->droq_tasklet);
#endif

	return 0;
}





oct_poll_fn_status_t  cn56xx_poll_reinit_regs(void *octptr, unsigned long val);


void
cn56xx_handle_mio_int(octeon_device_t  *oct)
{
	octeon_poll_ops_t        poll_ops;

	cavium_print_msg("OCTEON[%d]: Detected Soft reset by bootloader\n",
	                 oct->octeon_id);
	OCTEON_PCI_WIN_WRITE(oct, CN56XX_CIU_PCI_INTA, 0);

	poll_ops.fn     = cn56xx_poll_reinit_regs;
	poll_ops.fn_arg = 0;

	/*wangchong 2013-01-24 for review CID 14316
	poll_ops.rsvd = 0;*/
	
	poll_ops.ticks  = HZ * 2;
	strcpy(poll_ops.name, "Reinit CN56XX Regs");

	octeon_register_poll_fn(oct->octeon_id, &poll_ops);
}






cvm_intr_return_t
cn56xx_interrupt_handler(void  *dev)
{
	octeon_device_t  *oct    = (octeon_device_t  *)dev;
	octeon_cn56xx_t  *cn56xx = (octeon_cn56xx_t  *)oct->chip;
	uint64_t          intr64;

	cavium_print(PRINT_FLOW," In %s octeon_dev @ %p  \n", __CVM_FUNCTION__, oct);
	intr64 = OCTEON_READ64(cn56xx->intr_sum_reg64);

	/* If our device has interrupted, then proceed. */
	if (!intr64) {
		return CVM_INTR_NONE;
	}


	cavium_atomic_set(&oct->in_interrupt, 1);

	/* Disable our interrupts for the duration of ISR */
	oct->fn_list.disable_interrupt(oct->chip);

	oct->stats.interrupts++;

	cavium_atomic_inc(&oct->interrupts);

	if(intr64 & CN56XX_INTR_MIO)
		cn56xx_handle_mio_int(oct);

	if(intr64 & CN56XX_INTR_ERR)
		cn56xx_handle_pcie_error_intr(oct, intr64);

	if(intr64 & CN56XX_INTR_PKT_DATA)
		cn56xx_droq_intr_handler(oct);

#ifdef CVMCS_DMA_IC
	if (intr64 & (CN56XX_INTR_DMA0_COUNT | CN56XX_INTR_DMA0_TIME)) {
		uint64_t loc_dma_cnt = octeon_read_csr64 (oct, CN56XX_DMA_CNT_START);
		cavium_print (PRINT_DEBUG, "DMA Count: %lx intr: %lx\n",
			      loc_dma_cnt, 
			      intr64); 
		cavium_atomic_add(loc_dma_cnt, &oct->dma_cnt_to_process);
		/* Acknowledge from host, we are going to read 
                    loc_dma_cnt packets from DMA. */
		octeon_write_csr64 (oct, CN56XX_DMA_CNT_START,
				    loc_dma_cnt);
		cavium_tasklet_schedule(&oct->comp_tasklet);
	}
#else
	if(intr64 & (CN56XX_INTR_DMA0_FORCE|CN56XX_INTR_DMA1_FORCE))
		cavium_tasklet_schedule(&oct->comp_tasklet);
#endif

	if((intr64 & CN56XX_INTR_DMA_DATA) && (oct->dma_ops.intr_handler)) {
		oct->dma_ops.intr_handler((void *)oct, (intr64 & 0xffffffff));
	}

	/* Clear the current interrupts */
	OCTEON_WRITE64(cn56xx->intr_sum_reg64, intr64);

	/* Write the DMA interrupt bits twice. */
	intr64 &= (CN56XX_INTR_DMA_DATA);
	if(intr64)
		OCTEON_WRITE64(cn56xx->intr_sum_reg64, intr64);


	/* Re-enable our interrupts  */
	oct->fn_list.enable_interrupt(oct->chip);

	cavium_atomic_set(&oct->in_interrupt, 0);

	return CVM_INTR_HANDLED;
}




static void
cn56xx_reinit_regs(octeon_device_t  *oct)
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
cn56xx_bar1_idx_setup(octeon_device_t  *oct, uint64_t core_addr, int idx, int valid)
{
	if(valid == 0) {
		uint32_t  bar1 = octeon_read_csr(oct, CN56XX_BAR1_INDEX_REG(idx));
		octeon_write_csr(oct, CN56XX_BAR1_INDEX_REG(idx), (bar1 & 0xFFFFFFFE));
		return;
	}

	/* Bits 17:4 of the PCI_BAR1_INDEXx stores bits 35:22 of the Core Addr */
	octeon_write_csr(oct, CN56XX_BAR1_INDEX_REG(idx),
	                 (((core_addr >> 22) << 4) | PCI_BAR1_MASK) );
}



static void
cn56xx_bar1_idx_write(octeon_device_t  *oct, int idx, uint32_t  mask)
{
	octeon_write_csr(oct, CN56XX_BAR1_INDEX_REG(idx), mask);
}




static uint32_t
cn56xx_bar1_idx_read(octeon_device_t  *oct, int idx)
{
	return octeon_read_csr(oct, CN56XX_BAR1_INDEX_REG(idx));
}




static uint32_t
cn56xx_pass2_update_read_index(octeon_instr_queue_t  *iq)
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
cn56xx_enable_interrupt(void  *chip)
{
	octeon_cn56xx_t  *cn56xx = (octeon_cn56xx_t  *)chip;

	/* Enable Interrupt */
#ifdef CVMCS_DMA_IC
	OCTEON_WRITE64(cn56xx->intr_enb_reg64, cn56xx->intr_mask64);
#else
	OCTEON_WRITE64(cn56xx->intr_enb_reg64,
	                (cn56xx->intr_mask64|CN56XX_INTR_DMA0_FORCE) );
#endif

	OCTEON_WRITE64(cn56xx->intr_a_enb_reg64, CN56XX_INT_A_MASK);
}



static void
cn56xx_disable_interrupt(void  *chip)
{
	octeon_cn56xx_t  *cn56xx = (octeon_cn56xx_t  *)chip;

	/* Disable Interrupts */
	OCTEON_WRITE64(cn56xx->intr_enb_reg64, 0);

	OCTEON_WRITE64(cn56xx->intr_a_enb_reg64, 0);
}



static void
cn56xx_setup_reg_address(octeon_device_t   *oct)
{
	octeon_cn56xx_t   *cn56xx       = (octeon_cn56xx_t *)oct->chip;
	uint8_t           *bar0_pciaddr = (uint8_t *)oct->mmio[0].hw_addr;

	cn56xx->intr_sum_reg64   = bar0_pciaddr + CN56XX_NPEI_INT_SUM64;
	cn56xx->intr_enb_reg64   = bar0_pciaddr + CN56XX_NPEI_INT_ENB64;
	cn56xx->intr_a_sum_reg64 = bar0_pciaddr + CN56XX_NPEI_INT_A_SUM64;
	cn56xx->intr_a_enb_reg64 = bar0_pciaddr + CN56XX_NPEI_INT_A_ENB64;
	cn56xx->intr_mask64      = CN56XX_INTR_MASK;


	oct->reg_list.pci_win_wr_addr_hi = (uint32_t *)(bar0_pciaddr + CN56XX_WIN_WR_ADDR_HI);
	oct->reg_list.pci_win_wr_addr_lo = (uint32_t *)(bar0_pciaddr + CN56XX_WIN_WR_ADDR_LO);
	oct->reg_list.pci_win_wr_addr    = (uint64_t *)(bar0_pciaddr + CN56XX_WIN_WR_ADDR64);

	oct->reg_list.pci_win_rd_addr_hi = (uint32_t *)(bar0_pciaddr + CN56XX_WIN_RD_ADDR_HI);
	oct->reg_list.pci_win_rd_addr_lo = (uint32_t *)(bar0_pciaddr + CN56XX_WIN_RD_ADDR_LO);
	oct->reg_list.pci_win_rd_addr    = (uint64_t *)(bar0_pciaddr + CN56XX_WIN_RD_ADDR64);

	oct->reg_list.pci_win_wr_data_hi = (uint32_t *)(bar0_pciaddr + CN56XX_WIN_WR_DATA_HI);
	oct->reg_list.pci_win_wr_data_lo = (uint32_t *)(bar0_pciaddr + CN56XX_WIN_WR_DATA_LO);
	oct->reg_list.pci_win_wr_data    = (uint64_t *)(bar0_pciaddr + CN56XX_WIN_WR_DATA64);

	oct->reg_list.pci_win_rd_data_hi = (uint32_t *)(bar0_pciaddr + CN56XX_WIN_RD_DATA_HI);
	oct->reg_list.pci_win_rd_data_lo = (uint32_t *)(bar0_pciaddr + CN56XX_WIN_RD_DATA_LO);
	oct->reg_list.pci_win_rd_data    = (uint64_t *)(bar0_pciaddr + CN56XX_WIN_RD_DATA64);
}






int
setup_cn56xx_octeon_device(octeon_device_t  *oct)
{
	octeon_cn56xx_t   *cn56xx = (octeon_cn56xx_t *)oct->chip;

	if(octeon_map_pci_barx(oct, 0, 0))
		return 1;

	if(octeon_map_pci_barx(oct, 1, MAX_BAR1_IOREMAP_SIZE)) {
		octeon_unmap_pci_barx(oct, 0);
		return 1;
	}

	cn56xx->conf  = (cn56xx_config_t  *)oct_get_config_info(oct);
	if(cn56xx->conf == NULL) {
		octeon_unmap_pci_barx(oct, 0);
		octeon_unmap_pci_barx(oct, 1);
		return 1;
	}

	oct->fn_list.setup_iq_regs      = cn56xx_setup_iq_regs;
	oct->fn_list.setup_oq_regs      = cn56xx_setup_oq_regs;

	oct->fn_list.interrupt_handler  = cn56xx_interrupt_handler;
	oct->fn_list.soft_reset         = cn56xx_pass2_soft_reset;
	oct->fn_list.setup_device_regs  = cn56xx_setup_device_regs;
	oct->fn_list.reinit_regs        = cn56xx_reinit_regs;
	oct->fn_list.update_iq_read_idx = cn56xx_pass2_update_read_index;

	oct->fn_list.bar1_idx_setup     = cn56xx_bar1_idx_setup;
	oct->fn_list.bar1_idx_write     = cn56xx_bar1_idx_write;
	oct->fn_list.bar1_idx_read      = cn56xx_bar1_idx_read;

	oct->fn_list.enable_interrupt   = cn56xx_enable_interrupt;
	oct->fn_list.disable_interrupt  = cn56xx_disable_interrupt;

	oct->fn_list.enable_oq_pkt_time_intr  = cn56xx_enable_oq_pkt_time_intr;
	oct->fn_list.disable_oq_pkt_time_intr = cn56xx_disable_oq_pkt_time_intr;

	oct->fn_list.enable_io_queues   = cn56xx_enable_io_queues;
	oct->fn_list.disable_io_queues  = cn56xx_disable_io_queues;

	cn56xx_setup_reg_address(oct);

	return 0;
}








/* Poll function that runs once to reinitialize CN56XX registers if the
   bootloader has reset octeon. */ 
oct_poll_fn_status_t
cn56xx_poll_reinit_regs(void *octptr, unsigned long val)
{
	octeon_device_t  *oct  = (octeon_device_t  *)octptr;
	oct->fn_list.reinit_regs(oct);
	return OCT_POLL_FN_FINISHED;
}






#ifdef  CN56XX_PEER_TO_PEER

static octeon_soft_instruction_t  *
cn56xx_prepare_p2p_map_instr(octeon_device_t  *oct, int device_count)
{
	octeon_soft_instruction_t  *si;
	cn56xx_map_data_t          *data;
	cn56xx_pci_map_t           *map;
	int                         i, dsize, mapsize;


	mapsize = sizeof(cn56xx_pci_map_t) * device_count;
	dsize = mapsize + sizeof(cn56xx_pci_map_hdr_t);

	si = cavium_alloc_buffer(oct, OCT_SOFT_INSTR_SIZE + dsize);

	if(si == NULL) return NULL;

	cavium_memset(si, 0, OCT_SOFT_INSTR_SIZE + dsize);

	data = (cn56xx_map_data_t *)&si[1];

	SET_SOFT_INSTR_DMA_MODE(si, OCTEON_DMA_DIRECT);
	SET_SOFT_INSTR_RESP_ORDER(si, OCTEON_RESP_NORESPONSE);
	SET_SOFT_INSTR_TIMEOUT(si, 6000);
	SET_SOFT_INSTR_ALLOCFLAGS(si, (OCTEON_SOFT_INSTR_ALLOCATED));

	si->req_info.octeon_id = oct->octeon_id;

	si->ih.dlengsz    = dsize;
	si->ih.tag        = 0x10101010;
	si->ih.raw        = 1;
	si->ih.fsz        = 16;
	si->ih.tagtype    = OCTEON_TAG_TYPE_ATOMIC;
	si->irh.opcode    = PCIE_MAP_OP;
	si->dptr          = data;

	data->hdr.my_device_id = oct->octeon_id;
	data->hdr.dev_count    = device_count;
#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
	data->hdr.swap_mode    = 1;
#endif

	map = (cn56xx_pci_map_t *)&data->map;
	
	for(i = 0 ; i < device_count; i++) {
		octeon_device_t   *o = get_octeon_device_ptr(i);

		/* PCI Address available on Octeon Host for BAR0 & BAR1 
		   uses a prefix that is not sent on the PCI Bus. Actual
		   PCI addresses on the bus are 32-bit. */
		/* The fix below will be a problem only if used on a
		   system that maps Octeon targets to a > 4GB space in PCI.*/
		map[i].octeon_id       = (o->octeon_id & 0xFFFFFFFF);
		map[i].bar0_pci_addr   = (o->mmio[0].start & 0xFFFFFFFF);
		map[i].bar1_pci_addr   = (o->mmio[1].start & 0xFFFFFFFF);
	}

	octeon_swap_8B_data((uint64_t *)&data->map, (mapsize >> 3));

	//cavium_error_print((uint8_t *)data, dsize);

	return si;

}



static void
cn56xx_enable_all_bar1_indexes(octeon_device_t   *oct)
{
	int i;

	/* BAR 1 is 64 MB for 56XX */
	for(i = 0; i < 16; i++) {
		octeon_write_csr(oct, CN56XX_BAR1_INDEX_REG(i), ((i << 4) | 0xb));
	}
}





int
cn56xx_send_peer_to_peer_map(int oct_id)
{
	octeon_soft_instruction_t  *si;
	octeon_device_t            *oct = get_octeon_device_ptr(oct_id);
	
	cavium_print_msg("OCTEON[%d]: Sending PCI Map\n",
	                 oct_id);
	si = cn56xx_prepare_p2p_map_instr(oct, octeon_active_dev_count());
	if(si) {
		octeon_instr_status_t  r;
		r = octeon_process_instruction(oct, si, NULL);
		if(r.s.error) {
			cavium_error("%s Send instruction failed status: %x\n", __CVM_FUNCTION__, r.s.status);
			return 1;
		}
	}

	cn56xx_enable_all_bar1_indexes(oct);

	/* Add a small delay so that the peers don't manipulate each other's
	   registers at the same time. */
	cavium_sleep_timeout(100);

	return 0;
}

#endif







static void
cn56xx_bar0_reg_dump(octeon_device_t  *oct, uint32_t  start, uint32_t end)
{
	uint64_t   rval;
	int        i;

	for(i = start; i <= end; i+=16) {
		rval = octeon_read_csr64(oct, i);
		cavium_print_msg("BAR0[0x%x]: 0x%016llx\n", i, CVM_CAST64(rval));
	}
}


void
cn56xx_debug_dump_all_bar0_regs(octeon_device_t  *oct)
{
	uint64_t   rval;
	uint32_t   r32;
	int        i;


	cavium_print_msg("\n >>>>> CN56XX (Octeon%d) Register Dump\n",
	                 oct->octeon_id);

	cavium_print_msg("\n--- Config registers ---\n");
	for(i = 0; i < 64; i+=4) {
		OCTEON_READ_PCI_CONFIG(oct, i, &r32);
		cavium_print_msg("Config[0x%x] : 0x%08x\n", i, r32);
	}

	cavium_print_msg("\n------  BAR0 registers -----\n");

	cn56xx_bar0_reg_dump(oct, 0x200, 0x270);

	cn56xx_bar0_reg_dump(oct, 0x380, 0x380);

	cn56xx_bar0_reg_dump(oct, 0x3A0, 0x590);

	cn56xx_bar0_reg_dump(oct, 0x5B0, 0x6B0);

	cn56xx_bar0_reg_dump(oct, 0x1000, 0x1050);

	cn56xx_bar0_reg_dump(oct, 0x1070, 0x10B0);

	cn56xx_bar0_reg_dump(oct, 0x10D0, 0x10E0);

	cn56xx_bar0_reg_dump(oct, 0x1100, 0x1160);

	cn56xx_bar0_reg_dump(oct, 0x1180, 0x11A0);

	/* PKT[0 .. 31] SLIST Base Address */
	cavium_print_msg("\n PKT[0 .. 31] SLIST Base Address\n");
	cn56xx_bar0_reg_dump(oct, 0x1400, 0x1430);

	/* PKT[0 .. 31] SLIST BAOFF_DBELL - O/Q pkt credits */
	cavium_print_msg("\n PKT[0 .. 31] SLIST BAOFF_DBELL - O/Q pkt credits\n");
	cn56xx_bar0_reg_dump(oct, 0x1800, 0x1830);

	/* PKT[0 .. 31] SLIST FIFO RSIZE - O/Q size */
	cavium_print_msg("\n PKT[0 .. 31] SLIST FIFO RSIZE - O/Q size\n");
	cn56xx_bar0_reg_dump(oct, 0x1C00, 0x1C30);

	/* PKT[0 .. 31] PKT IN DONE CNTS - I/Q instr cnt */
	cavium_print_msg("\n PKT[0 .. 31] PKT IN DONE CNTS - I/Q instr cnt\n");
	cn56xx_bar0_reg_dump(oct, 0x2000, 0x2030);

	/* PKT[0 .. 31] PKT CNTS - O/Q pkts sent */
	cavium_print_msg("\n PKT[0 .. 31] PKT CNTS - O/Q pkts sent\n");
	cn56xx_bar0_reg_dump(oct, 0x2400, 0x2430);

	/* PKT[0 .. 31] INSTR Base Address */
	cavium_print_msg("\n PKT[0 .. 31] INSTR Base Address\n");
	cn56xx_bar0_reg_dump(oct, 0x2800, 0x2830);

	/* PKT[0 .. 31] INSTR BAOFF_DBELL - I/Q Doorbell */
	cavium_print_msg("\n PKT[0 .. 31] INSTR BAOFF_DBELL - I/Q Doorbell\n");
	cn56xx_bar0_reg_dump(oct, 0x2C00, 0x2C30);

	/* PKT[0 .. 31] INSTR FIFO Size - I/Q size */
	cavium_print_msg("\n PKT[0 .. 31] INSTR FIFO Size - I/Q size\n");
	cn56xx_bar0_reg_dump(oct, 0x3000, 0x3030);

	/* PKT[0 .. 31] INSTR HEADER - I/Q IH format and computation */
	cavium_print_msg("\n PKT[0 .. 31] INSTR HEADER - I/Q IH format\n");
	cn56xx_bar0_reg_dump(oct, 0x3400, 0x3430);

	/* PKT[0 .. 31] IN BP - I/Q Backpressure */
	cavium_print_msg("\n PKT[0 .. 31] IN BP - I/Q Backpressure\n");
	cn56xx_bar0_reg_dump(oct, 0x3800, 0x3830);

	/* CTL_STATUS2 (MPS/MRRS) */
	cavium_print_msg("\n CTL_STATUS2 (MPS/MRRS)\n");
	cn56xx_bar0_reg_dump(oct, 0x3C00, 0x3C00);


	rval = OCTEON_PCI_WIN_READ(oct, 0x00014F0000000000ULL);
	cavium_print_msg("PCI WIN (IPD_1st_MBUFF_SKIP) reg : 0x%016llx\n", CVM_CAST64(rval));

	rval = OCTEON_PCI_WIN_READ(oct, CN56XX_CIU_SOFT_RST);
	cavium_print_msg("PCI WIN (CIU_SOFT_RST) reg : 0x%016llx\n", CVM_CAST64(rval));
}










void
cn56xx_check_config_space_error_regs(octeon_device_t  *oct)
{
	uint32_t  regval, corr_err, uncorr_err;


	OCTEON_READ_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_DEVCTL, &regval);
	if(regval & 0x000f0000) {
		cavium_error("OCTEON[%d]: PCI-E error detected: 0x%08x\n",
		             oct->octeon_id,  regval&0xf0000);
		if(regval & 0x00010000) {
			OCTEON_READ_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_CORR_ERR_STATUS, &corr_err);
			corr_err &= 0x31C1;
			if(corr_err) {
				cavium_error("OCTEON[%d]: Correctable error: 0x%08x\n",
				             oct->octeon_id, corr_err);
				OCTEON_WRITE_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_CORR_ERR_STATUS, corr_err);
			}
		}
		if(regval & 0x00060000) {
			OCTEON_READ_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_UNCORR_ERR_STATUS, &uncorr_err);
			uncorr_err &= 0x1FF010;
			if(uncorr_err) {
				cavium_error("OCTEON[%d]: PCIE (Fatal/Non-Fatal) err: 0x%08x\n",
				            oct->octeon_id, uncorr_err);
				OCTEON_WRITE_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_UNCORR_ERR_STATUS, uncorr_err);
			}
		}
		if(regval & 0x00080000) {
			cavium_error("OCTEON[%d]: Unsupported Request detected\n",
			             oct->octeon_id);
		}
		OCTEON_WRITE_PCI_CONFIG(oct, CN56XX_CONFIG_PCIE_DEVCTL, regval);
	}
}










int
validate_cn56xx_config_info(cn56xx_config_t  *conf56xx)
{
	int i, total_instrs = 0;

	if(conf56xx->c.num_iqs > CN56XX_MAX_INPUT_QUEUES) {
		cavium_error("%s: Num IQ (%d) exceeds Max (%d)\n", __CVM_FUNCTION__,
		             conf56xx->c.num_iqs, CN56XX_MAX_INPUT_QUEUES);
		return 1;
	}


	if(conf56xx->c.num_oqs > CN56XX_MAX_OUTPUT_QUEUES) {
		cavium_error("%s: Num IQ (%d) exceeds Max (%d)\n", __CVM_FUNCTION__,
		             conf56xx->c.num_iqs, CN56XX_MAX_INPUT_QUEUES);
		return 1;
	}


	for(i = 0; i < conf56xx->c.num_iqs; i++) {
		if( (conf56xx->iq[i].instr_type != 32) &&
		    (conf56xx->iq[i].instr_type != 64) ) {
			cavium_error("%s: Invalid instr type (%d) for IQ[%d]\n",
			              __CVM_FUNCTION__, conf56xx->iq[i].instr_type, i);
			return 1;
		}
		if( !(conf56xx->iq[i].num_descs) || !(conf56xx->iq[i].db_min)
		    || !(conf56xx->iq[i].db_timeout) ) {
			cavium_error("%s: Invalid parameter for IQ[%d]\n",
			             __CVM_FUNCTION__, i);
			return 1;
		}
		total_instrs += conf56xx->iq[i].num_descs;
	}

	if(conf56xx->c.pending_list_size < total_instrs) {
		cavium_error("%s Pending list size (%d) should be >= total instructions queue size (%d)\n",
		           __CVM_FUNCTION__, conf56xx->c.pending_list_size, total_instrs);
		return 1;
	}

	for(i = 0; i < conf56xx->c.num_oqs; i++) {
		if(!(conf56xx->oq[i].info_ptr)  || !(conf56xx->oq[i].pkts_per_intr) ||
		   !(conf56xx->oq[i].num_descs) || !(conf56xx->oq[i].refill_threshold)){
			cavium_error("%s: Invalid parameter for OQ[%d]\n",
			             __CVM_FUNCTION__, i);
			return 1;
		}
	}

	if(!(conf56xx->oq_buf_size) || !(conf56xx->oq_intr_time) ) {
			cavium_error("%s: Invalid parameter for OQ[%d]\n",
			             __CVM_FUNCTION__, i);
		return 1;
	}

	return 0;
}



/* $Id: cn56xx_device.c 61122 2011-06-07 00:12:54Z panicker $ */
