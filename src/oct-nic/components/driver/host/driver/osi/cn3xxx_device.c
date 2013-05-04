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
#include "cn3xxx_device.h"
#include "octeon_macros.h"







static int
cn3xxx_setup_device_regs(octeon_device_t  *octeon_dev)
{
   uint32_t   regval;

   OCTEON_READ_PCI_CONFIG(octeon_dev, PCI_CFG_MASTER_ARB_CTL, &regval );
   /* Master Retry Value = FF, Master TRDY timeout value = 0. */
   regval |= 0x0000FF00;
   OCTEON_WRITE_PCI_CONFIG(octeon_dev, PCI_CFG_MASTER_ARB_CTL, regval );


   /* PCI read timeout enabled and set to 0x10000 eclk cycles. */
   OCTEON_PCI_WIN_WRITE(octeon_dev, NPI_PCI_READ_TIMEOUT, 0x80010000);


   if(CN3XXX_DEVICE_IN_PCIX_MODE(octeon_dev)) {
      cavium_print(PRINT_DEBUG,"Device is in PCIX mode\n");
      OCTEON_READ_PCI_CONFIG(octeon_dev, PCI_CFG_CAPABILITIES, &regval );
      /* Max. outstanding split transaction = 3, Max. mem. byte count = 2048,
         Relaxed Ordering Enable. */
      regval |= 0x003B0000;
      OCTEON_WRITE_PCI_CONFIG(octeon_dev, PCI_CFG_CAPABILITIES,regval);

      OCTEON_READ_PCI_CONFIG(octeon_dev, PCI_CFG_19, &regval );
      /* Target delayed max. outstanding count = 4. */
      regval |= 0x00000004;
      OCTEON_WRITE_PCI_CONFIG(octeon_dev, PCI_CFG_19, regval );
   }

   /* Target Implementation */
   OCTEON_READ_PCI_CONFIG(octeon_dev, PCI_CFG_TGT_IMPL, &regval);
   regval |= (0x00000001); /* Disable Master Latency Timer */
   OCTEON_WRITE_PCI_CONFIG(octeon_dev, PCI_CFG_TGT_IMPL, regval);


   OCTEON_READ_PCI_CONFIG(octeon_dev, PCI_CFG_COMMAND_STATUS, &regval);
   /* Disable mem write & inval */
   regval &= ~(0x00000010);
   /* Parity Error Enable, Fast Back to Back Transaction enable. */
   regval |= 0x00000240;
   OCTEON_WRITE_PCI_CONFIG(octeon_dev, PCI_CFG_COMMAND_STATUS, regval);

   OCTEON_PCI_WIN_WRITE(octeon_dev, NPI_PCI_BURST_SIZE,  PCI_BURST_SIZE);

   OCTEON_PCI_WIN_WRITE(octeon_dev, NPI_INPUT_CONTROL, NPI_INPUT_CTL_MASK);
   OCTEON_PCI_WIN_WRITE(octeon_dev, NPI_OUTPUT_CONTROL, NPI_OUTPUT_CTL_MASK);

   return 0;
}




static void
cn3xxx_enable_io_queues(octeon_device_t  *oct)
{
	int         i;
	uint64_t    npi_ctl = 0;

	for(i = 0; i < oct->num_iqs; i++)
	    npi_ctl |= ( ( ((uint64_t)oct->io_qmask.iq64B & (1 << i)))  << 42);

	for(i = 0; i < oct->num_iqs; i++)
	    npi_ctl |= ( ((uint64_t)(oct->io_qmask.iq & (1 << i)))  << 46);

	for(i = 0; i < oct->num_oqs; i++)
	    npi_ctl |= ( ((uint64_t)(oct->io_qmask.oq & (1 << i)))  << 50);

	/* Enable the input and output queues */
	OCTEON_PCI_WIN_WRITE(oct, NPI_CTL_STATUS, npi_ctl);
} 






static void
cn3xxx_disable_io_queues(octeon_device_t  *oct)
{
	int         i;
	uint64_t    npi_ctl;

	npi_ctl = OCTEON_PCI_WIN_READ(oct, NPI_CTL_STATUS);

	for(i = 0; i < oct->num_iqs; i++)
	    npi_ctl &= ~( ((uint64_t)(oct->io_qmask.iq64B & (1 << i)))  << 42);

	for(i = 0; i < oct->num_iqs; i++)
	    npi_ctl &= ~( ((uint64_t)(oct->io_qmask.iq & ~(1 << i)))  << 46);

	for(i = 0; i < oct->num_oqs; i++)
	    npi_ctl &= ~( ((uint64_t)(oct->io_qmask.oq & (1 << i)))  << 50);

	/* Disable the input and output queues */
	OCTEON_PCI_WIN_WRITE(oct, NPI_CTL_STATUS, npi_ctl);
}







static void
cn3xxx_setup_iq_regs(octeon_device_t   *oct, int   iq_no)
{
	octeon_instr_queue_t   *iq = oct->instr_queue[iq_no];
	uint32_t                cnt;

	OCTEON_PCI_WIN_WRITE(oct, OCT_IQ_INSTR_HDR_REG(iq_no), 0);

	/* Write the start of the input queue's ring and its size  */
	OCTEON_PCI_WIN_WRITE(oct, OCT_IQ_BASE_ADDR_REG(iq_no), iq->base_addr_dma);
	OCTEON_PCI_WIN_WRITE(oct, OCT_IQ_SIZE_REG(iq_no), iq->max_count);


	/* Remember the doorbell & instruction count register addr for this queue */
	iq->doorbell_reg = (uint8_t *)oct->mmio[0].hw_addr 
	                             +  OCT_IQ_DOORBELL_REG(iq_no);
	iq->inst_cnt_reg = (uint8_t *)oct->mmio[0].hw_addr 
	                             +  OCT_IQ_COUNT_REG(iq_no);
	cavium_print(PRINT_DEBUG,"InstQ[%d]:dbell reg @ 0x%p instcnt_reg @ 0x%p\n",
	             iq_no, iq->doorbell_reg, iq->inst_cnt_reg);

	cnt = OCTEON_READ32(iq->inst_cnt_reg);
	if(cnt) {
		cavium_error("OCTEON[%d]: Warning!! Instr count reg for IQ[%d] is non-zero (%d) at init time\n", oct->octeon_id, iq_no, cnt);

		/* Starting with CN38XX PASS3, the instr cnt reg is writable. */
		if(oct->chip_id != OCTEON_CN38XX_PASS2) {
			cavium_error("OCTEON: Fixing instruction count..");
			OCTEON_WRITE32(iq->inst_cnt_reg, cnt);
			cnt = OCTEON_READ32(iq->inst_cnt_reg);
			if(cnt == 0)
				cavium_error("done!\n");
			else
				cavium_error("\n Instr Count could not be reset\n");
		}
	}
}






static void
cn3xxx_setup_oq_regs(octeon_device_t   *oct, int   oq_no)
{
	uint32_t        base_buff_size;
	octeon_droq_t  *droq = oct->droq[oq_no];

	OCTEON_PCI_WIN_WRITE(oct, OCT_OQ_BASE_ADDR_REG(oq_no), droq->desc_ring_dma);
	OCTEON_PCI_WIN_WRITE(oct, OCT_OQ_SIZE_REG(oq_no), droq->max_count);

	/* The pkt len and buf len is always written by octeon.*/
	base_buff_size   =  droq->buffer_size & 0xffff;
	base_buff_size  |=  (OCT_RESP_HDR_SIZE) << 16;

	OCTEON_PCI_WIN_WRITE(oct, OCT_OQ_BUFF_SIZE_REG(oq_no), base_buff_size);

	/* Get the mapped address of the pkt_sent and pkts_credit regs */
	droq->pkts_sent_reg = (uint8_t *)oct->mmio[0].hw_addr + OCT_OQ_PKTS_SENT_REG(oq_no);
	droq->pkts_credit_reg = (uint8_t *)oct->mmio[0].hw_addr + OCT_OQ_PKTS_CREDITS_REG(oq_no);

	octeon_write_csr(oct, OCT_OQ_INTR_COUNT_LEV_REG(oq_no),
                         (CHIP_FIELD(oct, cn3xxx, conf))->oq[oq_no].intr_pkt);
	octeon_write_csr(oct, OCT_OQ_INTR_TIME_LEV_REG(oq_no),
         ((CHIP_FIELD(oct, cn3xxx, pci_freq)) * (CHIP_FIELD(oct, cn3xxx, conf)->oq[oq_no].intr_time)));
}








static int
cn3xxx_soft_reset(octeon_device_t   *oct)
{
   uint32_t  rval, dev_id;

   OCTEON_READ_PCI_CONFIG(oct, 0, &dev_id);
   /*
      Errata PCI-502 08/16/2007. For each byte that will be written,
      the corresponding mask bit should be 0.
   */
   octeon_write_csr(oct, CN3XXX_WIN_WR_MASK_REG, 0);

   if(dev_id != 0x4177d) {
       cavium_print_msg("OCTEON[%d]: BIST enabled for soft reset\n",
                        oct->octeon_id);
       OCTEON_PCI_WIN_WRITE(oct, CN3XXX_CIU_SOFT_BIST, 1);
   }

   OCTEON_PCI_WIN_WRITE(oct, CN3XXX_CIU_SOFT_RST, 1);

   /* Wait for 100ms as Octeon resets. */
   cavium_mdelay(100);

   rval = octeon_read_csr(oct, PCI_CTL_STATUS_2);
   if(!(rval & PCI_SOFT_RESET_DONE_MASK)) {
      cavium_error("OCTEON[%d]: Soft reset failed\n", oct->octeon_id);
      return 1;
   }

   octeon_write_csr(oct, CN3XXX_WIN_WR_MASK_REG, 0);

   return 0;
}







static void
cn3xxx_pci_error(octeon_device_t  *oct, uint32_t intr)
{
	cavium_error("OCTEON[%d]: PCI Error detected, intr: 0x%x\n",
	             oct->octeon_id,  intr);
	cavium_error("---~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	/* There are PCI transaction errors */
	if(intr & 0x00000012)  {
		uint32_t  ctlst;
		cavium_error("OCTEON[%d]: Master Abort detected\n", oct->octeon_id);
		OCTEON_READ_PCI_CONFIG(oct, PCI_CFG_COMMAND_STATUS, &ctlst);
		cavium_error("OCTEON[%d]: command status reg: 0x%x\n",
		             oct->octeon_id, ctlst);
		if(ctlst & 0x20000000)  {
			ctlst = ctlst | 0x20000000;
			OCTEON_WRITE_PCI_CONFIG(oct, 0x4, ctlst);
		}
	}
	cavium_atomic_set(&oct->status, OCT_DEV_STOPPING);
}




/*
  Modify the interrupt mask for CN38XX/CN58XX devices. The new mask is written
  to the device only if this routine was called out of interrupt context. If
  called from an interrupt context, it would be the caller's responsibility to
  write the new mask.

  NOTE: Since this routine can enable interrupts when called from outside
        of interrupt context, do not call this routine if you are not ready
        to handle interrupts.

  Params:
      oct    = octeon device handle
      oq_no  = output queue index
      time   = set this param to 1 if the timer interrupt is being modified
               set it to 0 if the packet count interrupt is being modified
      state  = the new state; set to 1 if interrupt is being enabled;
               set to 0 if interrupt is being disabled.
*/
static void
cn3xxx_modify_oq_pkt_intr(octeon_device_t   *oct, int oq_no, int time, int state)
{
	uint32_t            intr;
	octeon_cn3xxx_t    *cn3xxx = (octeon_cn3xxx_t *)oct->chip;

	if(!cavium_atomic_read(&oct->in_interrupt))
		oct->fn_list.disable_interrupt(cn3xxx);

	intr = (uint32_t) cavium_atomic_read(&(CHIP_FIELD(oct, cn3xxx, intr_mask)));
	if(time) {
		if(state)
			intr |= (DROQ0_TIME_INT << oq_no); // Enable timer interrupt
		else
			intr &= ~(DROQ0_TIME_INT << oq_no); // Disable timer interrupt
	} else {
		if(state)
			intr |= (DROQ0_CNT_INT << oq_no);  // Enable packet count interrupt
		else
			intr &= ~(DROQ0_CNT_INT << oq_no); // Disable packet count interrupt
	}
	cavium_atomic_set(&(CHIP_FIELD(oct, cn3xxx, intr_mask)), intr);

	if(!cavium_atomic_read(&oct->in_interrupt))
		oct->fn_list.enable_interrupt(cn3xxx);
}



static void
cn3xxx_enable_oq_pkt_time_intr(octeon_device_t   *oct, int oq_no)
{
	cn3xxx_modify_oq_pkt_intr(oct, oq_no, 1, 1);
}




static void
cn3xxx_disable_oq_pkt_time_intr(octeon_device_t   *oct, int oq_no)
{
	cn3xxx_modify_oq_pkt_intr(oct, oq_no, 1, 0);
}


#if 0
static void
cn3xxx_enable_oq_pkt_cnt_intr(octeon_device_t   *oct, int oq_no)
{
	cn3xxx_modify_oq_pkt_intr(oct, oq_no, 0, 1);
}



static void
cn3xxx_disable_oq_pkt_cnt_intr(octeon_device_t   *oct, int oq_no)
{
	cn3xxx_modify_oq_pkt_intr(oct, oq_no, 0, 0);
}

#endif



int
cn3xxx_droq_intr_handler(octeon_device_t   *oct,
                         uint32_t           intr)
{

	octeon_droq_t         *droq;
	uint32_t               q_no, pkt_count, schedule = 0;
	uint32_t               droq_mask = PCI_INT_DROQ0_MASK;

	/* Shift mask every time to check interrupt for next droq */
	for(q_no = 0; q_no < oct->num_oqs; q_no++,droq_mask <<= 1) {
		if(!(intr & droq_mask)) 
			continue;

		droq      = oct->droq[q_no];
		pkt_count = octeon_droq_check_hw_for_pkts(oct, droq);

		if(pkt_count == 0) {
			cavium_print(PRINT_DEBUG, "OCTEON: Interrupt 0x%x with no DROQ[%d] packets\n", intr, q_no);
			continue;
		}

		if(droq->ops.poll_mode) {
			schedule = 0;
			droq->ops.napi_fn(oct->octeon_id, q_no, POLL_EVENT_INTR_ARRIVED);
			oct->napi_mask |= (1 << q_no); 
		} else {
			schedule = 1;
			#if defined(USE_DROQ_THREADS)
			cavium_wakeup(&droq->wc);
			#endif
		}

	} /* for each queue */

	if(oct->napi_mask) {
		octeon_cn3xxx_t  *cn3xxx = (octeon_cn3xxx_t  *)oct->chip;
		/* Stop any more pkt time interrupts for this DROQ. They will be
		   enabled when the poll function has fetched in all packets. */
		cavium_atomic_set(&cn3xxx->intr_mask,
			(cavium_atomic_read(&cn3xxx->intr_mask) & ~(PCI_INT_DROQ_MASK)));
	}

#if !defined(USE_DROQ_THREADS)
	if(schedule)
		cavium_tasklet_schedule(&oct->droq_tasklet);
#endif

	return 0;
}








cvm_intr_return_t
cn3xxx_interrupt_handler(void  *dev)
{
	octeon_device_t  *oct    = (octeon_device_t  *)dev;
	octeon_cn3xxx_t  *cn3xxx = (octeon_cn3xxx_t *)oct->chip;
	uint32_t          intr;

	cavium_print(PRINT_FLOW,"In %s octeon_dev @ %p  \n", __CVM_FUNCTION__, oct);
	intr = OCTEON_READ32(cn3xxx->intr_sum_reg);

	/* If our device has interrupted, then proceed. */
	if (!intr)
		return CVM_INTR_NONE;

	cavium_atomic_set(&oct->in_interrupt, 1);

	/* Disable our interrupts for the duration of ISR */
	oct->fn_list.disable_interrupt(oct->chip);

	/* Clear the current interrupts */
	OCTEON_WRITE32(cn3xxx->intr_sum_reg, intr);

	oct->stats.interrupts++;
	cavium_atomic_inc(&oct->interrupts);

	if(intr & PCI_INT_PCI_ERR_MASK)
		cn3xxx_pci_error(oct, intr);

	if(intr & PCI_INT_DROQ_MASK)
		cn3xxx_droq_intr_handler(oct, intr);

	if(intr & PCI_DMA_FORCE_INT)
		cavium_tasklet_schedule(&oct->comp_tasklet);

	if((intr & PCI_INT_DMA_MASK) && (oct->dma_ops.intr_handler)) {
		oct->dma_ops.intr_handler((void *)oct, intr);
	}


	/* Re-enable our interrupts  */
	oct->fn_list.enable_interrupt(oct->chip);

	cavium_atomic_set(&oct->in_interrupt, 0);

	return CVM_INTR_HANDLED;
}





static void
cn3xxx_reinit_regs(octeon_device_t  *oct)
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
cn3xxx_bar1_idx_setup(octeon_device_t  *oct, uint64_t core_addr, int idx, int valid)
{
	if(valid == 0) {
		uint32_t  bar1 = octeon_read_csr(oct, OCT_BAR1_INDEX_REG(idx));
		octeon_write_csr(oct, OCT_BAR1_INDEX_REG(idx), (bar1 & 0xFFFFFFFE));
		return;
	}

	/* Bits 17:4 of the PCI_BAR1_INDEXx stores bits 35:22 of the Core Addr */
	octeon_write_csr(oct, OCT_BAR1_INDEX_REG(idx),
	                 (((core_addr >> 22) << 4) | PCI_BAR1_MASK) );
}



static void
cn3xxx_bar1_idx_write(octeon_device_t  *oct, int idx, uint32_t  mask)
{
	octeon_write_csr(oct, OCT_BAR1_INDEX_REG(idx), mask);
}




static uint32_t
cn3xxx_bar1_idx_read(octeon_device_t  *oct, int idx)
{
	return octeon_read_csr(oct, OCT_BAR1_INDEX_REG(idx));
}





static uint32_t
cn3xxx_update_read_index(octeon_instr_queue_t  *iq)
{
	int         diff = 0, inst_cnt = OCTEON_READ32(iq->inst_cnt_reg);

	diff= iq->host_write_index - iq->fill_cnt - inst_cnt;
	return ((diff >= 0 )?diff: (iq->max_count + diff));
}





static void
cn3xxx_enable_interrupt(void  *chip)
{
	octeon_cn3xxx_t  *cn3xxx = (octeon_cn3xxx_t  *)chip;
	OCTEON_WRITE32(cn3xxx->intr_enb_reg,cavium_atomic_read(&cn3xxx->intr_mask));
}





static void
cn3xxx_disable_interrupt(void  *chip)
{
	octeon_cn3xxx_t  *cn3xxx = (octeon_cn3xxx_t  *)chip;
	OCTEON_WRITE32(cn3xxx->intr_enb_reg, 0);
}





static void
cn3xxx_setup_reg_address(octeon_device_t   *oct)
{
	octeon_cn3xxx_t    *cn3xxx = (octeon_cn3xxx_t *)oct->chip;
	uint8_t            *bar0_pciaddr = (uint8_t *)oct->mmio[0].hw_addr;

	bar0_pciaddr = (uint8_t *)oct->mmio[0].hw_addr;

	cn3xxx->intr_sum_reg = bar0_pciaddr + PCI_INT_SUM;
	cn3xxx->intr_enb_reg = bar0_pciaddr + PCI_INT_ENABLE;
	cavium_atomic_set(&cn3xxx->intr_mask, (PCI_INT_ENB_MASK | PCI_DMA_FORCE_INT));

	oct->reg_list.pci_win_wr_addr_hi = (uint32_t *)(bar0_pciaddr + CN3XXX_WIN_WR_ADDR_HI);
	oct->reg_list.pci_win_wr_addr_lo = (uint32_t *)(bar0_pciaddr + CN3XXX_WIN_WR_ADDR_LO);
	oct->reg_list.pci_win_wr_addr    = (uint64_t *)(bar0_pciaddr + CN3XXX_WIN_WR_ADDR_LO);

	oct->reg_list.pci_win_rd_addr_hi = (uint32_t *)(bar0_pciaddr + CN3XXX_WIN_RD_ADDR_HI);
	oct->reg_list.pci_win_rd_addr_lo = (uint32_t *)(bar0_pciaddr + CN3XXX_WIN_RD_ADDR_LO);
	oct->reg_list.pci_win_rd_addr    = (uint64_t *)(bar0_pciaddr + CN3XXX_WIN_RD_ADDR_LO);

	oct->reg_list.pci_win_wr_data_hi = (uint32_t *)(bar0_pciaddr + CN3XXX_WIN_WR_DATA_HI);
	oct->reg_list.pci_win_wr_data_lo = (uint32_t *)(bar0_pciaddr + CN3XXX_WIN_WR_DATA_LO);
	oct->reg_list.pci_win_wr_data    = (uint64_t *)(bar0_pciaddr + CN3XXX_WIN_WR_DATA_LO);

	oct->reg_list.pci_win_rd_data_hi = (uint32_t *)(bar0_pciaddr + CN3XXX_WIN_RD_DATA_HI);
	oct->reg_list.pci_win_rd_data_lo = (uint32_t *)(bar0_pciaddr + CN3XXX_WIN_RD_DATA_LO);
	oct->reg_list.pci_win_rd_data    = (uint64_t *)(bar0_pciaddr + CN3XXX_WIN_RD_DATA_LO);
}








static int
setup_common_cn3xxx_resources(octeon_device_t  *oct)
{
	octeon_cn3xxx_t    *cn3xxx = (octeon_cn3xxx_t *)oct->chip;

	if(octeon_map_pci_barx(oct, 0, 0))
		return 1;

	if(octeon_map_pci_barx(oct, 1, MAX_BAR1_IOREMAP_SIZE)) {
		octeon_unmap_pci_barx(oct, 0);
		return 1;
	}


	cn3xxx->conf  = (cn3xxx_config_t  *)oct_get_config_info(oct);
	if(cn3xxx->conf == NULL) {
		octeon_unmap_pci_barx(oct, 0);
		octeon_unmap_pci_barx(oct, 1);
		return 1;
	}

	oct->fn_list.setup_iq_regs      = cn3xxx_setup_iq_regs;
	oct->fn_list.setup_oq_regs      = cn3xxx_setup_oq_regs;

	oct->fn_list.interrupt_handler  = cn3xxx_interrupt_handler;
	oct->fn_list.soft_reset         = cn3xxx_soft_reset;
	oct->fn_list.setup_device_regs  = cn3xxx_setup_device_regs;
	oct->fn_list.reinit_regs        = cn3xxx_reinit_regs;
	oct->fn_list.update_iq_read_idx = cn3xxx_update_read_index;

	oct->fn_list.bar1_idx_setup     = cn3xxx_bar1_idx_setup;
	oct->fn_list.bar1_idx_write     = cn3xxx_bar1_idx_write;
	oct->fn_list.bar1_idx_read      = cn3xxx_bar1_idx_read;

	oct->fn_list.enable_interrupt   = cn3xxx_enable_interrupt;
	oct->fn_list.disable_interrupt  = cn3xxx_disable_interrupt;

	oct->fn_list.enable_oq_pkt_time_intr  = cn3xxx_enable_oq_pkt_time_intr;
	oct->fn_list.disable_oq_pkt_time_intr = cn3xxx_disable_oq_pkt_time_intr;

	oct->fn_list.enable_io_queues   = cn3xxx_enable_io_queues;
	oct->fn_list.disable_io_queues  = cn3xxx_disable_io_queues;

	cn3xxx_setup_reg_address(oct);

	return 0;
}







int
setup_cn38xx_octeon_device(octeon_device_t  *oct)
{
	if(setup_common_cn3xxx_resources(oct))
		return 1;

	((octeon_cn3xxx_t *)oct->chip)->pci_freq = OCTEON_PCI_BUS_HZ;
	cavium_print_msg("OCTEON: Compiled with PCI frequency: %d Mhz\n",
	                 ((octeon_cn3xxx_t *)oct->chip)->pci_freq);

	return 0;
}






int
setup_cn58xx_octeon_device(octeon_device_t  *oct)
{
	int        pcix = 0, pci_freq = 0;
	uint32_t   regval;

	if(setup_common_cn3xxx_resources(oct))
		return 1;

	regval = octeon_read_csr(oct, PCI_CNT_REG_HI);
	switch(regval & 0x3) {
		case 0: pci_freq = 33; break;
		case 1: pci_freq = 66; break;
		case 2: pci_freq = 100; break;
		case 3: pci_freq = 133; break;
	}
	((octeon_cn3xxx_t *)oct->chip)->pci_freq = pci_freq;
	pcix = (regval & 0x4);

	cavium_print_msg("OCTEON[%d]: PCI%s frequency: %d Mhz\n",
		         oct->octeon_id, (pcix)?"(X)":"", pci_freq);

	return 0;
}







int
validate_cn3xxx_config_info(cn3xxx_config_t  *conf3xxx)
{
	int   i, total_instrs = 0;

	if(conf3xxx->c.num_iqs > CN3XXX_MAX_INPUT_QUEUES) {
		cavium_error("%s: Num IQ (%d) exceeds Max (%d)\n", __CVM_FUNCTION__,
		             conf3xxx->c.num_iqs, CN3XXX_MAX_INPUT_QUEUES);
		return 1;
	}

	if(conf3xxx->c.num_oqs > CN3XXX_MAX_OUTPUT_QUEUES) {
		cavium_error("%s: Num OQ (%d) exceeds Max (%d)\n", __CVM_FUNCTION__,
		             conf3xxx->c.num_oqs, CN3XXX_MAX_OUTPUT_QUEUES);
		return 1;
	}

	for(i = 0; i < conf3xxx->c.num_iqs; i++) {

		if( (conf3xxx->iq[i].instr_type != 32) &&
		    (conf3xxx->iq[i].instr_type != 64) ) {
			cavium_error("%s: Invalid instr type (%d) for IQ[%d]\n",
			              __CVM_FUNCTION__, conf3xxx->iq[i].instr_type, i);
			return 1;
		}

		if(!(conf3xxx->iq[i].num_descs) || !(conf3xxx->iq[i].db_min) ||
		   !(conf3xxx->iq[i].db_timeout) ) {
			cavium_error("%s: Invalid parameter for IQ[%d]\n",
			             __CVM_FUNCTION__, i);
			return 1;
		}
		total_instrs += conf3xxx->iq[i].num_descs;
	}

	if(conf3xxx->c.pending_list_size < total_instrs) {
		cavium_error("%s Pending list size (%d) should be >= total instructions queue size (%d)\n",
		           __CVM_FUNCTION__, conf3xxx->c.pending_list_size, total_instrs);
		return 1;
	}

	for(i = 0; i < conf3xxx->c.num_oqs; i++) {

		if(!(conf3xxx->oq[i].info_ptr)  || !(conf3xxx->oq[i].pkts_per_intr) ||
		   !(conf3xxx->oq[i].num_descs) || !(conf3xxx->oq[i].buf_size) ||
		   !(conf3xxx->oq[i].intr_time) || !(conf3xxx->oq[i].refill_threshold)){
			cavium_error("%s: Invalid parameter for OQ[%d]\n",
			             __CVM_FUNCTION__, i);
			return 1;
		}
	}

	return 0;
}


/* $Id: cn3xxx_device.c 61122 2011-06-07 00:12:54Z panicker $ */
