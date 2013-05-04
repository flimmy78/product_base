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
#include "cavium_proc.h"
#include "octeon_mem_ops.h"
#include "octeon_macros.h"

int   octeon_device_init(octeon_device_t *);
void  octeon_remove(struct pci_dev *pdev);

extern int   octeon_setup_poll_fn_list(octeon_device_t   *oct);
extern void  octeon_delete_poll_fn_list(octeon_device_t  *oct);

/*  State of the Octeon host driver.
    The Octeon device status is set after each sub-system of Octeon driver
    gets initialized. If failure occurs the status value indicates the cleanup
    routine which resources need to be freed.
*/
OCTEON_DRIVER_STATUS  octeon_state;











void
octeon_unmap_pci_barx(octeon_device_t  *oct, int baridx)
{
    cavium_print_msg("OCTEON[%d]: Freeing PCI mapped regions for Bar%d\n",
                     oct->octeon_id, baridx);

    if(oct->mmio[baridx].done)
       iounmap((void *)oct->mmio[baridx].hw_addr);

    if(oct->mmio[baridx].start)
          pci_release_region(oct->pci_dev, baridx*2);
}








int
octeon_map_pci_barx(octeon_device_t   *oct, int baridx, int max_map_len)
{
   unsigned long    mapped_len=0;

   if(pci_request_region(oct->pci_dev, baridx*2, DRIVER_NAME)) {
      cavium_error("OCTEON[%d]: pci_request_region failed for bar %d\n",
                   oct->octeon_id, baridx);
      return 1;
   }

   oct->mmio[baridx].start = pci_resource_start(oct->pci_dev, baridx*2);
   oct->mmio[baridx].len   = pci_resource_len(oct->pci_dev, baridx*2);

   mapped_len = oct->mmio[baridx].len;
   if(!mapped_len)
      return 1;

   if(max_map_len && (mapped_len > max_map_len)) {
       mapped_len = max_map_len;
   }

   oct->mmio[baridx].hw_addr    = ioremap(oct->mmio[baridx].start, mapped_len);
   oct->mmio[baridx].mapped_len = mapped_len;

   cavium_print_msg("OCTEON[%d]: BAR%d start: 0x%lx mapped %lu of %lu bytes\n",
                     oct->octeon_id, baridx, oct->mmio[baridx].start,
                     mapped_len, oct->mmio[baridx].len);

   if(!oct->mmio[baridx].hw_addr) {
      cavium_error("OCTEON[%d]: error ioremap for bar %d\n", oct->octeon_id,
                   baridx);
      return 1;
   }
   oct->mmio[baridx].done=1;

   return 0;
}







/* Linux wrapper for our interrupt handlers. */
cvm_intr_return_t
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
octeon_intr_handler(int              irq,
                    void            *dev,
                    struct pt_regs  *regs)
#else
octeon_intr_handler(int              irq,
                    void            *dev)
#endif
{
	octeon_device_t   *oct  = (octeon_device_t  *)dev;
	return  oct->fn_list.interrupt_handler(oct);
}









/*  Enable interrupt in Octeon device as given in the PCI interrupt mask.
*/
int
octeon_setup_interrupt(octeon_device_t  *oct)
{
	int       irqret;

	cavium_atomic_set(&oct->in_interrupt, 0);
	cavium_atomic_set(&oct->interrupts, 0);

	if(!pci_enable_msi(oct->pci_dev)) {
		oct->msi_on = 1;
		cavium_print_msg("OCTEON[%d]: MSI enabled\n", oct->octeon_id);
	}

	irqret = request_irq(oct->pci_dev->irq, octeon_intr_handler,
		                 CVM_SHARED_INTR, "octeon", oct);
	if(irqret) {
		if(oct->msi_on)
			pci_disable_msi(oct->pci_dev);
		cavium_error("OCTEON: Request IRQ failed with code: %d\n", irqret);
		return 1;
	}

   return 0;
}









int __devinit
octeon_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
	octeon_device_t  *oct_dev=NULL;

	cavium_print_msg("\n\nOCTEON: Found device %x:%x..Initializing...\n",
	                 (uint32_t)pdev->vendor, (uint32_t)pdev->device);

	oct_dev = octeon_allocate_device(pdev->device);
	if(oct_dev == NULL)
		return -ENOMEM;

	cavium_print_msg("OCTEON: Setting up Octeon device %d \n", oct_dev->octeon_id);

	/* Assign octeon_device for this device to the private data area. */
	pci_set_drvdata(pdev, oct_dev);

	/* set linux specific device pointer */
	oct_dev->pci_dev = (void *)pdev;

	if(octeon_device_init(oct_dev)) {
		octeon_remove(pdev);
		return -ENOMEM;
	}

	octeon_setup_driver_dispatches(oct_dev->octeon_id);

	if(oct_dev->app_mode
	   &&  (oct_dev->app_mode != CVM_DRV_BASE_APP)
	   &&  (oct_dev->app_mode != CVM_DRV_INVALID_APP))
		octeon_start_module(oct_dev->app_mode, oct_dev->octeon_id);

	octeon_state = OCT_DRV_ACTIVE;

	cavium_print_msg("OCTEON: Octeon device %d is ready\n", oct_dev->octeon_id);

	return 0;
}


















void
octeon_destroy_resources(octeon_device_t   *oct_dev)
{
	int    i;


	switch(cavium_atomic_read(&oct_dev->status)) {
		case OCT_DEV_STOPPED:
		case OCT_DEV_STOPPING:
		case OCT_DEV_IN_RESET:
		case OCT_DEV_RUNNING:
		case OCT_DEV_CORE_OK:
		case OCT_DEV_HOST_OK:

			/* No more instructions will be forwarded. */ 
			cavium_atomic_set(&oct_dev->status, OCT_DEV_IN_RESET);

			{
			int  attempts = 10;

			/* Inform core driver to stop pushing packets on output queues. */
			while(attempts--
			       && octeon_send_short_command(oct_dev, DEVICE_STOP_OP,
			                         (DEVICE_PKO), NULL, 0));
			}

			oct_dev->app_mode = CVM_DRV_INVALID_APP;
			cavium_print_msg("OCTEON: Device state is now %s\n",
				              get_oct_state_string(&oct_dev->status));

			cavium_sleep_timeout(CAVIUM_TICKS_PER_SEC/10);

			if(wait_for_pending_requests(oct_dev)) {
				cavium_error("OCTEON[%d]: There were pending requests\n",
				             oct_dev->octeon_id);
			}


			if(wait_for_instr_fetch(oct_dev)) {
				cavium_error("OCTEON[%d]: IQ had pending instructions\n",
				             oct_dev->octeon_id);
			}


			/* Disable the input and output queues now. No more packets will
			   arrive from Octeon, but we should wait for all packet processing
			   to finish. */
			oct_dev->fn_list.disable_io_queues(oct_dev);

			if(wait_for_oq_pkts(oct_dev)) {
				cavium_error("OCTEON[%d]: OQ had pending packets\n",
				             oct_dev->octeon_id);
			}

			/* Disable interrupts  */
			oct_dev->fn_list.disable_interrupt(oct_dev->chip);

			/* Release the interrupt line */
			free_irq(oct_dev->pci_dev->irq, oct_dev);

			if(oct_dev->msi_on)
				pci_disable_msi(oct_dev->pci_dev);

			/* Soft reset the octeon device before exiting */
			oct_dev->fn_list.soft_reset(oct_dev);


			/* Delete the /proc device entries */
			cavium_delete_proc(oct_dev);

		case OCT_DEV_DROQ_INIT_DONE:
			//cavium_atomic_set(&oct_dev->status, OCT_DEV_DROQ_INIT_DONE);
			cavium_mdelay(100);
			for (i = 0; i < oct_dev->num_oqs; i++)  {
				octeon_delete_droq(oct_dev, i);
			}

		case OCT_DEV_RESP_LIST_INIT_DONE:
			octeon_delete_response_list(oct_dev);

		case OCT_DEV_PEND_LIST_INIT_DONE:
			octeon_delete_pending_list(oct_dev);

		case OCT_DEV_INSTR_QUEUE_INIT_DONE:
			for (i = 0; i < oct_dev->num_iqs; i++)  {
				octeon_delete_instr_queue(oct_dev, i);
			}

#ifdef USE_BUFFER_POOL
		case OCT_DEV_BUF_POOL_INIT_DONE:
			octeon_delete_buffer_pool(oct_dev);
#endif

		case OCT_DEV_DISPATCH_INIT_DONE:
			octeon_delete_dispatch_list(oct_dev);
			octeon_delete_poll_fn_list(oct_dev);

		case OCT_DEV_PCI_MAP_DONE:
			octeon_unmap_pci_barx(oct_dev, 0);
			octeon_unmap_pci_barx(oct_dev, 1);


		case OCT_DEV_RESET_CLEANUP_DONE:
		case OCT_DEV_BEGIN_STATE:
			/* Nothing to be done here either */
			break;
	} /* end switch(oct_dev->status) */


	cavium_tasklet_kill(&oct_dev->comp_tasklet);

#if  !defined(USE_DROQ_THREADS)
	cavium_tasklet_kill(&oct_dev->droq_tasklet);
#endif
}






/* This routine is called for each octeon device during driver
    unload time. */
void 
octeon_remove(struct pci_dev *pdev)
{
	octeon_device_t  *oct_dev = pci_get_drvdata(pdev);
	int               oct_idx;

	oct_idx = oct_dev->octeon_id;
	cavium_print_msg("OCTEON: Stopping octeon device %d\n", oct_idx);

	/* Call the module handler for each module attached to the
	   base driver. */
	if(oct_dev->app_mode 
	   && (oct_dev->app_mode != CVM_DRV_INVALID_APP)
	   && (oct_dev->app_mode != CVM_DRV_BASE_APP))  {
		octeon_stop_module(oct_dev->app_mode, oct_dev->octeon_id);
	}

	/* Reset the octeon device and cleanup all memory allocated for
	   the octeon device by driver. */
	octeon_destroy_resources(oct_dev);

	/* This octeon device has been removed. Update the global
	   data structure to reflect this. Free the device structure. */
	octeon_free_device_mem(oct_dev);

	cavium_print_msg("OCTEON: Octeon device %d removed\n", oct_idx);
}









/* The open() entry point for the octeon driver. This routine is called
   every time the /dev/octeon_device file is opened. */
int 
octeon_open (struct inode *inode, struct file *file)
{
	CVM_MOD_INC_USE_COUNT;
	return 0;
}





/* The close() entry point for the octeon driver. This routine is called
   every time the /dev/octeon_device file is closed. */
int 
octeon_release(struct inode *inode, struct file *file)
{
	CVM_MOD_DEC_USE_COUNT;
	return 0;
}







/* Routine to identify the Octeon device and to map the BAR address space */
static int
octeon_chip_specific_setup(octeon_device_t  *oct)
{
	uint32_t    dev_id, rev_id;


	OCTEON_READ_PCI_CONFIG(oct, 0, &dev_id);
	OCTEON_READ_PCI_CONFIG(oct, 8, &rev_id);
	oct->rev_id = rev_id & 0xff;

	switch(dev_id) {

		case OCTEON_CN68XX_PCIID:
			cavium_print_msg("OCTEON[%d]: CN68XX PASS%d.%d\n",
			                 oct->octeon_id, OCTEON_MAJOR_REV(oct), OCTEON_MINOR_REV(oct));
			oct->chip_id = OCTEON_CN68XX;
			return setup_cn68xx_octeon_device(oct);
		
		case OCTEON_CN66XX_PCIID:
			cavium_print_msg("OCTEON[%d]: CN66XX PASS%d.%d\n",
			                 oct->octeon_id, OCTEON_MAJOR_REV(oct), OCTEON_MINOR_REV(oct));
			oct->chip_id = OCTEON_CN66XX;
			return setup_cn66xx_octeon_device(oct);


		case OCTEON_CN63XX_PCIID:
			cavium_print_msg("OCTEON[%d]: CN63XX PASS%d.%d\n",
			                 oct->octeon_id, OCTEON_MAJOR_REV(oct), OCTEON_MINOR_REV(oct));
			oct->chip_id = OCTEON_CN63XX;
			return setup_cn63xx_octeon_device(oct);

		case OCTEON_CN56XX_PCIID:

			if(oct->rev_id >= 8) {
				cavium_print_msg("OCTEON[%d]: CN56XX PASS2.%d\n",
				       oct->octeon_id, OCTEON_MINOR_REV(oct));
				oct->chip_id = OCTEON_CN56XX;
				return setup_cn56xx_octeon_device(oct);
			}
			break;

		case OCTEON_CN58XX_PCIID:
			cavium_print_msg("OCTEON[%d]: CN58XX PASS1\n", oct->octeon_id);
			oct->chip_id = OCTEON_CN58XX;
			return setup_cn58xx_octeon_device(oct);

		case OCTEON_CN38XX_PASS2_PCIID:
			cavium_print_msg("OCTEON[%d]: CN38XX PASS2\n", oct->octeon_id);
			oct->chip_id = OCTEON_CN38XX_PASS2;
			return setup_cn38xx_octeon_device(oct);

		case OCTEON_CN38XX_PASS3_PCIID:
			cavium_print_msg("OCTEON[%d]: CN38XX PASS3\n", oct->octeon_id);
			oct->chip_id = OCTEON_CN38XX_PASS3;
			return setup_cn38xx_octeon_device(oct);

		default:
	 		cavium_error("OCTEON: Unknown device found (dev_id: %x)\n",dev_id);
	}

	return 1;
}






/* OS-specific initialization for each Octeon device. */
static int
octeon_pci_os_setup(octeon_device_t  *octeon_dev)
{

	/* setup PCI stuff first */
	if(pci_enable_device(octeon_dev->pci_dev)) {
		cavium_error("OCTEON: pci_enable_device failed\n");
		return 1;
	}

	/* Octeon device supports DMA into a 64-bit space */
	if(pci_set_dma_mask(octeon_dev->pci_dev, PCI_DMA_64BIT)) {
		cavium_error("OCTEON: pci_set_dma_mask(64bit) failed\n");
		return 1;
	}

	/* Enable PCI DMA Master. */
	pci_set_master(octeon_dev->pci_dev);

	return 0;
}







#ifdef  CN56XX_PEER_TO_PEER

oct_poll_fn_status_t
oct_poll_p2p_fn(void  *octptr, unsigned long arg)
{
	octeon_device_t  *oct = (octeon_device_t *)octptr;

	if(octeon_all_devices_active()) {
		if(cn56xx_send_peer_to_peer_map(oct->octeon_id)) {
			cavium_error("\nOCTEON[%d] : Octeon Peer to Peer send failed\n",
			             oct->octeon_id);
		}
		return OCT_POLL_FN_FINISHED;
	}

	return OCT_POLL_FN_CONTINUE;
}

#endif








extern oct_poll_fn_status_t
oct_poll_module_starter(void  *octptr, unsigned long arg);







/* Device initialization for each Octeon device. */
int 
octeon_device_init(octeon_device_t         *octeon_dev)
{
   int                 j, ret;
   octeon_poll_ops_t   poll_ops;

   cavium_atomic_set(&octeon_dev->status, OCT_DEV_BEGIN_STATE);

   /* Enable access to the octeon device and make its DMA capability
      known to the OS. */
   if(octeon_pci_os_setup(octeon_dev))
      return 1;

   /* Identify the Octeon type and map the BAR address space. */
   if(octeon_chip_specific_setup(octeon_dev)) {
       cavium_error("OCTEON: Chip specific setup failed\n");
       return 1;
   }


   cavium_atomic_set(&octeon_dev->status, OCT_DEV_PCI_MAP_DONE);

   cavium_spin_lock_init(&octeon_dev->oct_lock);

   /* Do a soft reset of the Octeon device. */
   if(octeon_dev->fn_list.soft_reset(octeon_dev))
       return 1;


   /* Initialize the dispatch mechanism used to push packets arriving on
      Octeon Output queues. */
   if(octeon_init_dispatch_list(octeon_dev))
      return 1;


  /* Initialize the poll list mechanism used that allows modules to
     register functions with the driver for each Octeon device. */
   octeon_setup_poll_fn_list(octeon_dev);

   cavium_memset(&poll_ops, 0, sizeof(octeon_poll_ops_t));


   /* The driver has its own set of functions that it registers for
      each octeon device. */
   poll_ops.fn     = oct_poll_module_starter;
   poll_ops.fn_arg = 0UL;
   poll_ops.ticks  = CAVIUM_TICKS_PER_SEC;
   strcpy(poll_ops.name, "Module Starter");
   octeon_register_poll_fn(octeon_dev->octeon_id, &poll_ops);

#ifdef  CN56XX_PEER_TO_PEER
   poll_ops.fn     = oct_poll_p2p_fn;
   poll_ops.fn_arg = 0UL;
   poll_ops.ticks  = CAVIUM_TICKS_PER_SEC;
   strcpy(poll_ops.name, "Peer to Peer Map");
   octeon_register_poll_fn(octeon_dev->octeon_id, &poll_ops);
#endif

   cavium_atomic_set(&octeon_dev->status, OCT_DEV_DISPATCH_INIT_DONE);


   /* If the use of buffer pool is enabled in the Makefile, allocate
      memory and initialize the pools. */
#ifdef USE_BUFFER_POOL
   {
      octeon_bufpool_config_t  bufpool_config;

      bufpool_config.huge_buffer_max    = HUGE_BUFFER_CHUNKS;
      bufpool_config.large_buffer_max   = LARGE_BUFFER_CHUNKS;
      bufpool_config.medium_buffer_max  = MEDIUM_BUFFER_CHUNKS;
      bufpool_config.small_buffer_max   = SMALL_BUFFER_CHUNKS;
      bufpool_config.tiny_buffer_max    = TINY_BUFFER_CHUNKS;
      bufpool_config.ex_tiny_buffer_max = EX_TINY_BUFFER_CHUNKS;

      if(octeon_init_buffer_pool(octeon_dev, &bufpool_config))  {
           cavium_error("OCTEON: Buffer pool allocation failed\n");
           return 1;
      }
      cavium_atomic_set(&octeon_dev->status, OCT_DEV_BUF_POOL_INIT_DONE);
   }
#endif


   octeon_set_io_queues_off(octeon_dev);


   /*  Setup the data structures that manage this Octeon's Input queues. */
   if(octeon_setup_instr_queues(octeon_dev)) {
        cavium_error("OCTEON: instruction queue initialization failed\n");
        /* On error, release any previously allocated queues */
        for (j = 0; j < octeon_dev->num_iqs; j++)
            octeon_delete_instr_queue(octeon_dev, j);
        return 1;
   }


   cavium_atomic_set(&octeon_dev->status, OCT_DEV_INSTR_QUEUE_INIT_DONE);



   /* Initialize pending list to hold the requests for which a response is
      expected from the software running on Octeon. */
   if((ret = octeon_init_pending_list(octeon_dev))) {
        cavium_error("OCTEON: Pending list allocation failed, ret: %d\n", ret);
        return ret;
   }
   cavium_atomic_set(&octeon_dev->status, OCT_DEV_PEND_LIST_INIT_DONE);



   /* Initialize lists to manage the requests of different types that arrive
      from user & kernel applications for this octeon device. */
   if(octeon_setup_response_list(octeon_dev))  {
       cavium_error("OCTEON: Response list allocation failed\n");
       return 1;
   }
   cavium_atomic_set(&octeon_dev->status, OCT_DEV_RESP_LIST_INIT_DONE);



   if(octeon_setup_output_queues(octeon_dev)) {
        cavium_error("OCTEON: Output queue initialization failed\n");
        /* Release any previously allocated queues */
        for (j = 0; j < octeon_dev->num_oqs; j++)
           octeon_delete_droq(octeon_dev, j);
   }

   cavium_atomic_set(&octeon_dev->status, OCT_DEV_DROQ_INIT_DONE);


   /* The input and output queue registers were setup earlier (the queues were
      not enabled). Any additional registers that need to be programmed should
      be done now. */
   ret = octeon_dev->fn_list.setup_device_regs(octeon_dev);
   if(ret) {
      cavium_error("OCTEON: Failed to configure device registers\n");
      return ret;
   }


   /* Setup the /proc entries for this octeon device. */
   cavium_init_proc(octeon_dev);



   /* Initialize the tasklet that handles output queue packet processing. */
#if !defined(USE_DROQ_THREADS)
   cavium_print(PRINT_MSG, "OCTEON: Initializing droq tasklet\n");
   cavium_tasklet_init(&octeon_dev->droq_tasklet, octeon_droq_bh, (unsigned long)octeon_dev);
#endif


   /* The comp_tasklet speeds up response handling for ORDERED requests. */
   cavium_print(PRINT_MSG, "OCTEON: Initializing completion on interrupt tasklet\n");
   cavium_tasklet_init(&octeon_dev->comp_tasklet, octeon_request_completion_bh, (unsigned long)octeon_dev);


#ifdef CVMCS_DMA_IC
   cavium_atomic_set(&octeon_dev->dma_cnt_to_process, 0);
#endif


   /* Setup the interrupt handler and record the INT SUM register address */
   octeon_setup_interrupt(octeon_dev);


   /* Enable Octeon device interrupts */
   octeon_dev->fn_list.enable_interrupt(octeon_dev->chip);

   /* Enable the input and output queues for this Octeon device*/
   octeon_dev->fn_list.enable_io_queues(octeon_dev);

   /* Send Credit for Octeon Output queues. Credits are always sent after the
      output queue is enabled. */
   for(j = 0; j < octeon_dev->num_oqs; j++)  {
      OCTEON_WRITE32(octeon_dev->droq[j]->pkts_credit_reg,
                     octeon_dev->droq[j]->max_count);
   }

   /* Packets can start arriving on the output queues from this point. */
   octeon_dev->app_mode = CVM_DRV_INVALID_APP;

   cavium_atomic_set(&octeon_dev->status, OCT_DEV_HOST_OK);

   return 0;
}










/* $Id: octeon_main.c 66446 2011-10-25 02:31:59Z mchalla $ */

