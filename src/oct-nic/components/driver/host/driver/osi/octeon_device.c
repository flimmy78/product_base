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


#include "octeon_device.h"
#include "octeon_macros.h"
#include "octeon_mem_ops.h"
#include "oct_config_data.h"





char   oct_dev_state_str[OCT_DEV_STATES+1][32] = {
	"BEGIN",                 "PCI-MAP-DONE",       "DISPATCH-INIT-DONE",
	"BUFPOOL-INIT-DONE",  "INSTR-QUEUE-INIT-DONE", "PENDLIST-INIT-DONE",
	"RESPLIST-INIT-DONE", "DROQ-INIT-DONE",        "HOST-READY",
	"CORE-READY",         "RUNNING",               "IN-RESET",
	"STOPPING",           "STOPPED",
	"INVALID"
};


char oct_dev_app_str[CVM_DRV_APP_COUNT+1][32] =
            { "BASE", "NIC", "CNTQ", "CNTQ-NIC", "CN56XX-PASS1", "UNKNOWN"};


octeon_device_t            *octeon_device[MAX_OCTEON_DEVICES];
uint32_t                    octeon_device_count=0;


octeon_module_handler_t     octmodhandlers[OCTEON_MAX_MODULES];
cavium_spinlock_t           octmodhandlers_lock;


octeon_core_setup_t         core_setup[MAX_OCTEON_DEVICES];

oct_poll_fn_status_t
oct_poll_module_starter(void  *octptr, unsigned long arg);



/*
   All Octeon devices use the default configuration in oct_config_data.h.
   To override the default:
   1.  The Octeon device Id must be known for customizing the octeon configuration.
   2.  Create a custom configuration based on CN3XXX or CN56XX config structure
       (see octeon_config.h) in oct_config_data.h.
   3.  Modify the config type of the octeon device in the structure below to 
       specify CN56XX or CN3XXX configuration and replace the "custom" pointer
       to point to your custom configuration in oct_config_data.h
*/

static struct octeon_config_ptr {
	uint32_t    conf_type;
	void       *custom;
} oct_conf_info[MAX_OCTEON_DEVICES] =
	{
		{OCTEON_CONFIG_TYPE_DEFAULT, NULL },
		{OCTEON_CONFIG_TYPE_DEFAULT, NULL },
		{OCTEON_CONFIG_TYPE_DEFAULT, NULL },
		{OCTEON_CONFIG_TYPE_DEFAULT, NULL },
	};



void
octeon_init_device_list(void)
{
	cavium_memset(octeon_device, 0, (sizeof(void *) * MAX_OCTEON_DEVICES));
}



static void *
__retrieve_octeon_config_info(octeon_device_t  *oct)
{
	int    oct_id = oct->octeon_id;

	if(oct_conf_info[oct_id].conf_type != OCTEON_CONFIG_TYPE_DEFAULT) {

		if( (oct->chip_id <= OCTEON_CN58XX) &&
		 (oct_conf_info[oct_id].conf_type == OCTEON_CONFIG_TYPE_CN3XXX_CUSTOM))
			return oct_conf_info[oct_id].custom;

		if( (oct->chip_id == OCTEON_CN56XX) &&
		 (oct_conf_info[oct_id].conf_type == OCTEON_CONFIG_TYPE_CN56XX_CUSTOM))
			return oct_conf_info[oct_id].custom;

		if( (oct->chip_id == OCTEON_CN63XX) &&
		 (oct_conf_info[oct_id].conf_type == OCTEON_CONFIG_TYPE_CN63XX_CUSTOM))
			return oct_conf_info[oct_id].custom;

		if( (oct->chip_id == OCTEON_CN66XX)  &&
		 (oct_conf_info[oct_id].conf_type == OCTEON_CONFIG_TYPE_CN66XX_CUSTOM))
			return oct_conf_info[oct_id].custom;

		if( (oct->chip_id == OCTEON_CN68XX) &&
		 (oct_conf_info[oct_id].conf_type == OCTEON_CONFIG_TYPE_CN68XX_CUSTOM))
			return oct_conf_info[oct_id].custom;

		cavium_error("OCTEON[%d]: Incompatible config type (%d) for chip type %x\n",
		             oct_id, oct_conf_info[oct_id].conf_type, oct->chip_id);
		return NULL;
	}

	if(oct->chip_id <= OCTEON_CN58XX)
		return (void *)&default_cn3xxx_conf;

	if(oct->chip_id == OCTEON_CN56XX)
		return (void *)&default_cn56xx_conf;

	if(oct->chip_id == OCTEON_CN63XX)
		return (void *)&default_cn63xx_conf;

	if(oct->chip_id == OCTEON_CN66XX) 
		return (void *)&default_cn66xx_conf;

	if(oct->chip_id == OCTEON_CN68XX)
		return (void *)&default_cn68xx_conf;

	return NULL;
}







static int
__verify_octeon_config_info(octeon_device_t  *oct, void *conf)
{
	switch(oct->chip_id) {
		case OCTEON_CN38XX_PASS2:
		case OCTEON_CN38XX_PASS3:
		case OCTEON_CN58XX:
			return validate_cn3xxx_config_info(conf);

		case OCTEON_CN56XX:
			return validate_cn56xx_config_info(conf);

		case OCTEON_CN63XX:
			return validate_cn63xx_config_info(conf);

		case OCTEON_CN66XX:
			return validate_cn66xx_config_info(conf);

		case OCTEON_CN68XX:
			return validate_cn68xx_config_info(conf);

		default:
			break;
	}

	return 1;
}





void *
oct_get_config_info(octeon_device_t   *oct)
{
	void  *conf = NULL;

	conf = __retrieve_octeon_config_info(oct);
	if(conf == NULL)
		return NULL;

	if(__verify_octeon_config_info(oct, conf)) {
		cavium_error("\n Configuration verification failed for Octeon[%d]\n",
		             oct->octeon_id);
		return NULL;
	}

	return conf;
}







char *
get_oct_state_string(cavium_atomic_t  *state_ptr)
{
	int istate = (int)cavium_atomic_read(state_ptr);

	if(istate > OCT_DEV_STATES || istate < 0)
		return oct_dev_state_str[OCT_DEV_STATE_INVALID];
	return oct_dev_state_str[istate];
}





char *
get_oct_app_string(int  app_mode)
{
	if(app_mode >= CVM_DRV_APP_START && app_mode <= CVM_DRV_APP_END)
		return oct_dev_app_str[app_mode-CVM_DRV_APP_START];
	return oct_dev_app_str[CVM_DRV_INVALID_APP - CVM_DRV_APP_START];
}





void
octeon_free_device_mem(octeon_device_t   *oct)
{
	int   i;

	for(i = 0; i < oct->num_oqs; i++)  {
		if(oct->droq[i])
			cavium_free_virt(oct->droq[i]);
	}

	for(i = 0; i < oct->num_iqs; i++)  {
		if(oct->instr_queue[i])
			cavium_free_virt(oct->instr_queue[i]);
	}

	i = oct->octeon_id;
	cavium_free_virt(oct);

	octeon_device[i] = NULL;
	octeon_device_count--;
}






octeon_device_t   *
octeon_allocate_device_mem(int pci_id)
{
	octeon_device_t   *oct;
	uint8_t           *buf = NULL;
	int                octdevsize = 0, configsize = 0, size;

	switch(pci_id) {

		case OCTEON_CN68XX:
			configsize = sizeof(octeon_cn68xx_t); break;

		case OCTEON_CN66XX:
			configsize = sizeof(octeon_cn66xx_t); break;

		case OCTEON_CN63XX:
			configsize = sizeof(octeon_cn63xx_t); break;

		case OCTEON_CN56XX:
			configsize = sizeof(octeon_cn56xx_t); break;

		case OCTEON_CN58XX:
		case OCTEON_CN38XX_PASS3:
		case OCTEON_CN38XX_PASS2:
			configsize = sizeof(octeon_cn3xxx_t); break;

		default:
			cavium_print_msg("%s: Unknown PCI Device: 0x%x\n", __FUNCTION__,
			                 pci_id);
			return NULL;
	}

	if(configsize & 0x7)
		configsize += (8 - (configsize & 0x7));

	octdevsize = sizeof(octeon_device_t);
	if(octdevsize & 0x7)
		octdevsize += (8 - (octdevsize & 0x7));


	size = octdevsize + configsize + (sizeof(octeon_dispatch_t) * DISPATCH_LIST_SIZE);
	buf  = cavium_alloc_virt(size);
	if(buf == NULL)
		return NULL;

	cavium_memset(buf, 0, size);

	oct        = (octeon_device_t *)buf;
	oct->chip  = (void *)(buf + octdevsize);
	oct->dispatch.dlist = (octeon_dispatch_t *)(buf + octdevsize + configsize);

	return oct;
}







octeon_device_t  *
octeon_allocate_device(int pci_id)
{
	int               i, oct_idx = 0;
	octeon_device_t  *oct = NULL;

	for(i = 0; i < MAX_OCTEON_DEVICES; i++) {
		if(octeon_device[i] == NULL) {
			oct_idx = i;
			break;
		}
	}

	if(oct_idx == MAX_OCTEON_DEVICES) {
		cavium_error("OCTEON: Could not find empty slot for device pointer. octeon_device_count: %d MAX_OCTEON_DEVICES: %d\n", octeon_device_count, MAX_OCTEON_DEVICES);
		return NULL;
	}

	oct = octeon_allocate_device_mem(pci_id);
	if(oct == NULL) {
		cavium_error("OCTEON: Allocation failed for octeon device\n");
		return NULL;
	}

	octeon_device_count++;
	octeon_device[oct_idx] = oct;

	oct->octeon_id = oct_idx;
	octeon_assign_dev_name(oct);

	return oct;
}




int
octeon_setup_instr_queues(octeon_device_t  *oct)
{
	int  i, num_iqs = 0;

	if(oct->chip_id <= OCTEON_CN58XX)
		num_iqs = CHIP_FIELD(oct, cn3xxx, conf)->c.num_iqs;

	if(oct->chip_id == OCTEON_CN56XX)
		num_iqs = CHIP_FIELD(oct, cn56xx, conf)->c.num_iqs;

	if(oct->chip_id == OCTEON_CN63XX) 
		num_iqs = CHIP_FIELD(oct, cn63xx, conf)->c.num_iqs;

	if(oct->chip_id == OCTEON_CN66XX)
		num_iqs = CHIP_FIELD(oct, cn66xx, conf)->c.num_iqs;

	if(oct->chip_id == OCTEON_CN68XX)
		num_iqs = CHIP_FIELD(oct, cn68xx, conf)->c.num_iqs;

	oct->num_iqs = 0;

	for(i = 0; i < num_iqs; i++) {
		oct->instr_queue[i] = cavium_alloc_virt(sizeof(octeon_instr_queue_t));
		if(oct->instr_queue[i] == NULL)
			return 1;

		cavium_memset(oct->instr_queue[i], 0, sizeof(octeon_instr_queue_t));

		if(octeon_init_instr_queue(oct, i))
			return 1;

		oct->num_iqs++;
	}

	return 0;
}





int
octeon_setup_output_queues(octeon_device_t  *oct)
{
	int  i, num_oqs = 0;

	if(oct->chip_id <= OCTEON_CN58XX)
		num_oqs = CHIP_FIELD(oct, cn3xxx, conf)->c.num_iqs;

	if(oct->chip_id == OCTEON_CN56XX)
		num_oqs = CHIP_FIELD(oct, cn56xx, conf)->c.num_oqs;

	if(oct->chip_id == OCTEON_CN63XX)
		num_oqs = CHIP_FIELD(oct, cn63xx, conf)->c.num_oqs;

	if(oct->chip_id == OCTEON_CN66XX)
		num_oqs = CHIP_FIELD(oct, cn66xx, conf)->c.num_oqs;

	if(oct->chip_id == OCTEON_CN68XX)
		num_oqs = CHIP_FIELD(oct, cn68xx, conf)->c.num_oqs;

	oct->num_oqs = 0;

	for(i = 0; i < num_oqs; i++) {
		oct->droq[i] = cavium_alloc_virt(sizeof(octeon_droq_t));
		if(oct->droq[i] == NULL)
			return 1;

		cavium_memset(oct->droq[i], 0, sizeof(octeon_droq_t));

		if(octeon_init_droq(oct, i))
			return 1;

		oct->num_oqs++;
	}

	return 0;
}





void
octeon_set_io_queues_off(octeon_device_t  *oct)
{
	/* Disable the i/p and o/p queues for this Octeon. */
	if(oct->chip_id <= OCTEON_CN58XX) {
		OCTEON_PCI_WIN_WRITE(oct, NPI_CTL_STATUS, 0);
	}

	if(oct->chip_id == OCTEON_CN56XX) {
		octeon_write_csr(oct, CN56XX_NPEI_PKT_INSTR_ENB, 0);
		octeon_write_csr(oct, CN56XX_NPEI_PKT_OUT_ENB, 0);
	}

	if( (oct->chip_id == OCTEON_CN63XX) || (oct->chip_id == OCTEON_CN66XX) ) {
		octeon_write_csr(oct, CN63XX_SLI_PKT_INSTR_ENB, 0);
		octeon_write_csr(oct, CN63XX_SLI_PKT_OUT_ENB, 0);
	}

	if(oct->chip_id == OCTEON_CN68XX) {
		octeon_write_csr(oct, CN68XX_SLI_PKT_INSTR_ENB, 0);
		octeon_write_csr(oct, CN68XX_SLI_PKT_OUT_ENB, 0);
	}
}





int
octeon_hot_reset(octeon_device_t  *oct)
{
	int                 i, status;
	octeon_poll_ops_t   poll_ops;

	cavium_print_msg("\n\n OCTEON[%d]: Starting Hot Reset...\n",oct->octeon_id);

	status = (int)cavium_atomic_read(&oct->status);

	if(status < OCT_DEV_HOST_OK || status > OCT_DEV_IN_RESET) {
		cavium_error("OCTEON: Hot Reset received when device state is %s\n",
		             get_oct_state_string(&oct->status));
		cavium_error("OCTEON: Device state will remain at %s (0x%x)\n",
                     get_oct_state_string(&oct->status),
		             (int)cavium_atomic_read(&oct->status));
		return 1;
	}

	cavium_print_msg("OCTEON: Stopping modules...\n");


	/* Stop any driver modules that are running. Do this before sending the hot
	   reset command so that the modules get a chance to stop their traffic. */
	if((oct->app_mode != CVM_DRV_INVALID_APP) && (oct->app_mode != CVM_DRV_BASE_APP)) {
		if(octeon_reset_module(oct->app_mode, oct->octeon_id)) {
			cavium_error("OCTEON: Module for app_type: %s is busy\n",
			             get_oct_app_string(oct->app_mode)); 
			cavium_error("OCTEON: Hot Reset aborted. Try again after unloading the module\n");
			return 1;
		}
	}



	if(status == OCT_DEV_RUNNING) {
		/* The core application is known to be running only in this state. */
		/* Sent instruction to core indicating that the host is about to reset.
		*/
		cavium_print_msg("OCTEON: Modules stopped. Sending Reset command..\n");
		if(octeon_send_short_command(oct, HOT_RESET_OP, 0, NULL, 0)) {
			cavium_error("Failed to send HOT RESET instruction\n");
			cavium_error("OCTEON: Device state will remain at %s (0x%x)\n",
			             get_oct_state_string(&oct->status),
			             (int)cavium_atomic_read(&oct->status));
			return 1;
		} else {
			cavium_print_msg("OCTEON: HotReset command sent.\n");
		}
	}

	/* No more instructions will be forwarded. */ 
	cavium_atomic_set(&oct->status, OCT_DEV_IN_RESET);

	oct->app_mode = CVM_DRV_INVALID_APP;
	cavium_print_msg("OCTEON: Device state is now %s\n",
	                 get_oct_state_string(&oct->status));

	/* Sleep a short while to allow for in-flight requests to be setup
	   correctly. No new requests would be allowed once the RESET state
	   is set above. */
	cavium_sleep_timeout(100);


	cavium_print_msg("OCTEON[%d]: In Reset: Waiting for pending requests to finish\n", oct->octeon_id);

	if(wait_for_pending_requests(oct))
		goto hot_reset_failed;

	cavium_print_msg("OCTEON[%d]: In Reset: No pending requests. Checking Input Queues\n", oct->octeon_id);

	if(wait_for_instr_fetch(oct)) {
		cavium_error("OCTEON[%d]: Input Queue not empty; Hot Reset Aborted\n",
		             oct->octeon_id);
		goto hot_reset_failed;
	}


	/* Disable the input and output queues now. No more packets will arrive
	   from Octeon, but we should wait for all packet processing to finish. */
	oct->fn_list.disable_io_queues(oct);

	cavium_print_msg("OCTEON[%d]: In Reset: Input & Output queues stopped\n", oct->octeon_id);
	cavium_print_msg("OCTEON[%d]: In Reset: Waiting to finish packet processing on Output Queues\n", oct->octeon_id);

	if(wait_for_oq_pkts(oct)) {
		cavium_error("OCTEON[%d]: Output Queues not empty; Hot Reset Aborted\n",
		             oct->octeon_id);
		goto hot_reset_failed;
	}

	cavium_print_msg("OCTEON[%d]: In Reset. No more packets pending in Output Queues\n", oct->octeon_id);


	/* Reset is now complete in bringing the octeon device to its init state. */ 
	cavium_atomic_set(&oct->status, OCT_DEV_RESET_CLEANUP_DONE);

	oct->fn_list.soft_reset(oct);


	cavium_print_msg("OCTEON[%d]: Performing device initialization\n",
	                 oct->octeon_id);

	for(i = 0; i < oct->num_iqs; i++) {
		octeon_instr_queue_t   *iq = oct->instr_queue[i];

		iq->fill_cnt          = 0;
		iq->host_write_index  = 0;
		iq->octeon_read_index = 0;
		iq->flush_index       = 0;
		iq->last_db_time      = 0;
		cavium_atomic_set(&iq->instr_pending, 0);
	}

	for(i = 0; i < oct->num_oqs; i++) {
		octeon_droq_t   *droq = oct->droq[i];

		octeon_droq_refill(oct, droq);
		droq->host_read_index     = 0;
		droq->octeon_write_index  = 0;
		droq->host_refill_index   = 0;
		droq->refill_count        = 0;
		cavium_atomic_set(&droq->pkts_pending, 0);
	}

	oct->fn_list.reinit_regs(oct);

	cavium_print_msg("OCTEON[%d]: Reset Done. Load a new core application to continue...\n", oct->octeon_id);
    cavium_atomic_set(&oct->status, OCT_DEV_HOST_OK);

	poll_ops.fn     = oct_poll_module_starter;
	poll_ops.fn_arg = 0UL;

	/*wangchong 2013-01-24 for review CID 14317
	poll_ops.rsvd = 0;*/
	
	poll_ops.ticks  = CAVIUM_TICKS_PER_SEC;
	strcpy(poll_ops.name, "Module Starter");
	octeon_register_poll_fn(oct->octeon_id, &poll_ops);


	return 0;

hot_reset_failed:
	cavium_error("OCTEON[%d]: Device will remain in RESET state\n Try again!",
	             oct->octeon_id);
	return 1;
}









int
octeon_init_dispatch_list(octeon_device_t  *oct)
{
	int i;

	oct->dispatch.count = 0;

	for (i = 0; i < DISPATCH_LIST_SIZE; i++) {
		oct->dispatch.dlist[i].opcode = 0;
		CAVIUM_INIT_LIST_HEAD(&(oct->dispatch.dlist[i].list));
	}

	cavium_spin_lock_init(&oct->dispatch.lock);

	return 0;
}







void
octeon_delete_dispatch_list(octeon_device_t   *oct)
{
	int i;
	cavium_list_t  freelist, *temp, *tmp2;

	CAVIUM_INIT_LIST_HEAD(&freelist);

	cavium_spin_lock_softirqsave(&oct->dispatch.lock);

	for (i = 0 ; i < DISPATCH_LIST_SIZE; i++) {
		cavium_list_t   *dispatch;

		dispatch = &(oct->dispatch.dlist[i].list);
		while(dispatch->le_next != dispatch) {
			temp = dispatch->le_next;
			cavium_list_del(temp);
			cavium_list_add_tail(temp, &freelist);
		}

		oct->dispatch.dlist[i].opcode = 0;
	}

	oct->dispatch.count = 0;

	cavium_spin_unlock_softirqrestore(&oct->dispatch.lock);

	cavium_list_for_each_safe(temp, tmp2, &freelist) {
		cavium_list_del(temp);
		cavium_free_virt(temp);
	}

}









/*
   octeon_register_dispatch_fn
   Parameters:
     octeon_id - id of the octeon device.
     opcode    - opcode for which driver should call the registered function
     fn        - The function to call when a packet with "opcode" arrives in
                 octeon output queues.
     fn_arg    - The argument to be passed when calling function "fn".
   Description:
     Registers a function and its argument to be called when a packet
     arrives in Octeon output queues with "opcode".
   Returns:
     Success: 0
     Failure: 1
   Locks:
     No locks are held.
*/
uint32_t
octeon_register_dispatch_fn(uint32_t              octeon_id,
                            octeon_opcode_t       opcode,
                            octeon_dispatch_fn_t  fn,
                            void                  *fn_arg)
{

   int                    idx;
   octeon_device_t       *oct;
   octeon_dispatch_fn_t   pfn;

   oct = get_octeon_device(octeon_id);
   if(oct == NULL) {
      cavium_error("OCTEON: No device with id %d to register dispatch\n",
                   octeon_id);
      return 1;
   }

   idx = opcode & OCTEON_OPCODE_MASK;

   cavium_spin_lock_softirqsave(&oct->dispatch.lock);
   /* Add dispatch function to first level of lookup table */
   if(oct->dispatch.dlist[idx].opcode == 0) {
       oct->dispatch.dlist[idx].opcode      = opcode;
       oct->dispatch.dlist[idx].dispatch_fn = fn;
       oct->dispatch.dlist[idx].arg         = fn_arg;
       oct->dispatch.count++;
       cavium_spin_unlock_softirqrestore(&oct->dispatch.lock);
       return 0;
   }

   cavium_spin_unlock_softirqrestore(&oct->dispatch.lock);

   /* Check if there was a function already registered for this opcode. */
   pfn = octeon_get_dispatch(oct, opcode);
   if(pfn == NULL) {
      octeon_dispatch_t  *dispatch;
      cavium_print(PRINT_DEBUG,"Adding opcode to dispatch list linked list\n");
      dispatch = (octeon_dispatch_t *)
                 cavium_alloc_virt(sizeof(octeon_dispatch_t));
      if(dispatch == NULL) {
          cavium_error("OCTEON[%d]: No memory to add dispatch function\n",
                       octeon_id);
          return 1;
      }
      dispatch->opcode      = opcode;
      dispatch->dispatch_fn = fn;
      dispatch->arg         = fn_arg;

      /* Add dispatch function to linked list of fn ptrs at the hashed index. */
      cavium_spin_lock_softirqsave(&oct->dispatch.lock);
      cavium_list_add_head(&(dispatch->list),&(oct->dispatch.dlist[idx].list));
      oct->dispatch.count++;
      cavium_spin_unlock_softirqrestore(&oct->dispatch.lock);

   } else {
      cavium_error("OCTEON[%d]: Found previously registered dispatch fn for opcode: %x\n",
                   octeon_id, opcode);
      return 1;
   }

   return 0;
}









/*
   octeon_unregister_dispatch_fn
   Parameters:
     octeon_id - id of the octeon device.
     opcode    - driver should unregister the function for this opcode
   Description:
     Unregister the function set for this opcode.
   Returns:
     Success: 0
     Failure: 1
   Locks:
     No locks are held.
*/
uint32_t
octeon_unregister_dispatch_fn(uint32_t          octeon_id,
                              octeon_opcode_t   opcode)
{
   int                 idx, retval=0;
   octeon_device_t    *octeon_dev;
   cavium_list_t      *dispatch, *dfree = NULL, *tmp2;
 

   cavium_print(PRINT_FLOW,"#### Unregister dispatch\n");
   octeon_dev = get_octeon_device(octeon_id);
   if(octeon_dev == NULL) {
     cavium_error("OCTEON: No device with id %d to unregister dispatch\n",
                   octeon_id);
     return 1;
   }

   idx = opcode & OCTEON_OPCODE_MASK;
   cavium_print(PRINT_DEBUG,"idx is %d, opcode is 0x%x\n", idx, opcode);

   cavium_spin_lock_softirqsave(&octeon_dev->dispatch.lock);

   if(octeon_dev->dispatch.count == 0) {
     cavium_spin_unlock_softirqrestore(&octeon_dev->dispatch.lock);
     cavium_error("OCTEON[%d]: No dispatch functions registered for this device\n",
                   octeon_id);
     return 1;
   }

   if(octeon_dev->dispatch.dlist[idx].opcode == opcode) {
      cavium_print(PRINT_DEBUG,"--get_dispatch: found entry in main list\n");
      dispatch = &(octeon_dev->dispatch.dlist[idx].list);
      if(dispatch->le_next != dispatch)  {
         dispatch = dispatch->le_next;
         octeon_dev->dispatch.dlist[idx].opcode      =
                                 ((octeon_dispatch_t *)dispatch)->opcode ;
         octeon_dev->dispatch.dlist[idx].dispatch_fn =
                             ((octeon_dispatch_t *)dispatch)->dispatch_fn;
         octeon_dev->dispatch.dlist[idx].arg  =
                                     ((octeon_dispatch_t *)dispatch)->arg;
         cavium_list_del(dispatch);
         dfree = dispatch;
      }
      else  {
         octeon_dev->dispatch.dlist[idx].opcode      = 0;
         octeon_dev->dispatch.dlist[idx].dispatch_fn = NULL;
         octeon_dev->dispatch.dlist[idx].arg         = NULL;
      }
   }
   else  {
      retval = 1;
      cavium_list_for_each_safe(dispatch, tmp2, &(octeon_dev->dispatch.dlist[idx].list)) {
          if(((octeon_dispatch_t *)dispatch)->opcode == opcode)  {
            cavium_list_del(dispatch);
            dfree = dispatch;
            retval = 0;
          }
      }
   }

   if(!retval)
      octeon_dev->dispatch.count--;

   cavium_spin_unlock_softirqrestore(&octeon_dev->dispatch.lock);

   if(dfree)
      cavium_free_virt(dfree);

   return (retval);
}









int
octeon_core_drv_init(octeon_recv_info_t  *recv_info,
                     void                *buf)
{
    int                 i, oct_id;
    char                app_name[16];
    octeon_device_t    *oct       = (octeon_device_t *)buf;
    octeon_recv_pkt_t  *recv_pkt  = recv_info->recv_pkt;

    if(cavium_atomic_read(&oct->status) >= OCT_DEV_RUNNING) {
        cavium_error("OCTEON[%d]: Received CORE OK when device state is 0x%x\n",
                     oct->octeon_id, cavium_atomic_read(&oct->status));
        goto core_drv_init_err;
    }

    strcpy(app_name, get_oct_app_string(recv_pkt->resp_hdr.dest_qport));
    cavium_print_msg("OCTEON[%d]: Received active indication from core\n",
                     oct->octeon_id);
    oct->app_mode = recv_pkt->resp_hdr.dest_qport;
    cavium_atomic_set(&oct->status, OCT_DEV_CORE_OK);

    if(recv_pkt->buffer_size[0] != sizeof(octeon_core_setup_t)) {
        cavium_error("OCTEON[%d]: Core setup bytes expected %u found %d\n",
                     oct->octeon_id, (uint32_t)sizeof(octeon_core_setup_t),
                     recv_pkt->buffer_size[0]);
    }

    oct_id = oct->octeon_id;
    cavium_memcpy(&core_setup[oct_id],
                  get_recv_buffer_data(recv_pkt->buffer_ptr[0]),
                  sizeof(octeon_core_setup_t));

    octeon_swap_8B_data((uint64_t *)&core_setup[oct_id],
                        (sizeof(octeon_core_setup_t) >> 3));

    cavium_print_msg("OCTEON[%d] is running %s application (core clock: %llu Hz)\n",
                     oct->octeon_id, app_name,
                     CVM_CAST64(core_setup[oct_id].corefreq));

core_drv_init_err:
    for(i = 0 ; i < recv_pkt->buffer_count; i++) {
        free_recv_buffer(recv_pkt->buffer_ptr[i]);
    }
    cavium_free_dma(recv_info);
    return 0;
}






void
octeon_setup_driver_dispatches(uint32_t oct_id)
{
	octeon_register_dispatch_fn(oct_id, CORE_DRV_ACTIVE_OP, octeon_core_drv_init,
	                            get_octeon_device_ptr(oct_id));
}







int
octeon_get_tx_qsize(int octeon_id, int q_no)
{
	octeon_device_t   *oct_dev = get_octeon_device(octeon_id);
	
	if(oct_dev && (q_no < oct_dev->num_iqs))
		return oct_dev->instr_queue[q_no]->max_count;

	return -1;
}







int
octeon_get_rx_qsize(int octeon_id, int q_no)
{
	octeon_device_t   *oct_dev = get_octeon_device(octeon_id);
	
	if(oct_dev && (q_no < oct_dev->num_oqs))
		return oct_dev->droq[q_no]->max_count;
	return -1;
}








oct_poll_fn_status_t
oct_poll_module_starter(void  *octptr, unsigned long arg)
{
	octeon_device_t  *oct = (octeon_device_t *)octptr;

	if(cavium_atomic_read(&oct->status) == OCT_DEV_RUNNING) {
		return OCT_POLL_FN_FINISHED;
	}


	/* If the status of the device is CORE_OK, the core
	   application has reported its application type. Call
	   any registered handlers now and move to the RUNNING
	   state. */
	if(cavium_atomic_read(&oct->status) != OCT_DEV_CORE_OK)
		return OCT_POLL_FN_CONTINUE;

	cavium_atomic_set(&oct->status,OCT_DEV_RUNNING);

	if( oct->app_mode && oct->app_mode != CVM_DRV_BASE_APP) {
		cavium_print_msg("OCTEON[%d]: Starting module for app type: %s\n",
		                 oct->octeon_id, get_oct_app_string(oct->app_mode));
		if(octeon_start_module(oct->app_mode, oct->octeon_id)) {
			cavium_error("OCTEON[%d]: Start Handler failed for app_mode: %s\n",
			             oct->octeon_id, get_oct_app_string(oct->app_mode));
		}
	}

	return OCT_POLL_FN_CONTINUE;
}








static inline int
__octeon_module_action(uint32_t  app_type,
                       uint32_t  operation,
                       uint32_t  octeon_id)
{
	int                        i, retval=0;
	octeon_module_handler_t   *handler = NULL;
	octeon_device_t           *octeon_dev=NULL;



	octeon_dev = get_octeon_device(octeon_id);
	if(octeon_dev == NULL) {
		cavium_error("OCTEON: No octeon device (id:%d) found in %s\n",
		             octeon_id, __CVM_FUNCTION__);
		return -ENODEV;
	}


	cavium_spin_lock(&octmodhandlers_lock);

	for(i = 0; i < OCTEON_MAX_MODULES; i++)  {

		/* Check if a handler exists for the given app_type. */
		if(!(octmodhandlers[i].app_type & app_type))
			continue;

		handler = &octmodhandlers[i];


		/* If no handler is found, return without error. */
		if(handler == NULL) {
			cavium_error("OCTEON: No handler found for application type %s\n",
			             get_oct_app_string(app_type));
			continue;
		}


		cavium_print_msg("OCTEON: Found handler for app_type: %s\n",
		                 get_oct_app_string(handler->app_type));

		cavium_spin_unlock(&octmodhandlers_lock);

		/* If a handler exists, call the start or stop routine based on
		   the operation specified. */
		switch(operation) {
			case OCTEON_START_MODULE:
				retval = handler->startptr(octeon_id, octeon_device[octeon_id]);
				break;
			case OCTEON_RESET_MODULE:
				retval = handler->resetptr(octeon_id, octeon_device[octeon_id]);
				break;
			case OCTEON_STOP_MODULE:
				retval = handler->stopptr(octeon_id, octeon_device[octeon_id]);
				break;
			default:
				cavium_error("OCTEON: Unknown operation %d in %s\n", operation,
		              __CVM_FUNCTION__);
				return -EINVAL;
		}

		cavium_spin_lock(&octmodhandlers_lock);
	}

	cavium_spin_unlock(&octmodhandlers_lock);

	return retval;
}








int
octeon_start_module(uint32_t app_type, uint32_t octeon_id)
{
	return __octeon_module_action(app_type, OCTEON_START_MODULE, octeon_id);
}




int
octeon_reset_module(uint32_t app_type, uint32_t octeon_id)
{
	return __octeon_module_action(app_type, OCTEON_RESET_MODULE, octeon_id);
}




int
octeon_stop_module(uint32_t app_type, uint32_t octeon_id)
{
	return __octeon_module_action(app_type, OCTEON_STOP_MODULE, octeon_id);
}





void
octeon_init_module_handler_list(void)
{
	cavium_memset(&octmodhandlers, 0, sizeof(octeon_module_handler_t));
	cavium_spin_lock_init(&octmodhandlers_lock);
}






int
octeon_register_module_handler(octeon_module_handler_t  *handler)
{
	int    modidx, octidx, retval=0;

	if(!handler || !handler->startptr || !handler->stopptr || !handler->resetptr
	   || !handler->app_type) {
		cavium_error("OCTEON: Invalid arguments in module handler\n");
		return -EINVAL;
	}

	cavium_spin_lock(&octmodhandlers_lock);
	/* Check if a handler has already been registered for this app type. */
	for(modidx = 0; modidx < OCTEON_MAX_MODULES; modidx++) {
		if(octmodhandlers[modidx].app_type == handler->app_type) {
			cavium_error("OCTEON: Module Handler exists for application type 0x%x\n", handler->app_type);
			cavium_spin_unlock(&octmodhandlers_lock);
			return -EINVAL;
		}
	}



	/* Check if space exists in handler array to register this handler. */
	for(modidx = 0; modidx < OCTEON_MAX_MODULES; modidx++) {
		if(octmodhandlers[modidx].app_type == 0)
			break;
	}

	if(modidx == OCTEON_MAX_MODULES) {
		cavium_error("OCTEON: Module handler registration failed (Max handlers reached)\n");
		cavium_spin_unlock(&octmodhandlers_lock);
		return -ENOMEM;
	}

	/* Add this handler to the module handlers array. */
	cavium_memcpy(&octmodhandlers[modidx], handler, sizeof(octeon_module_handler_t));

	cavium_spin_unlock(&octmodhandlers_lock);

	cavium_print_msg("OCTEON: Registered handler for app_type: %s\n",
	                 get_oct_app_string(handler->app_type));

	/* Call the start method for all existing octeon devices. */
	for (octidx = 0; octidx < MAX_OCTEON_DEVICES; octidx++) {
		octeon_device_t  *oct_dev = octeon_device[octidx];

		if(oct_dev == NULL)
			continue;

#ifdef  ETHERPCI
		oct_dev->app_mode = CVM_DRV_NIC_APP; // Emulate NIC PCI Device
		cavium_atomic_set(&oct_dev->status, OCT_DEV_RUNNING);
#endif
		if(oct_dev->app_mode & handler->app_type) {
			cavium_print_msg("OCTEON[%d]: Starting modules for app_type: %s\n",
			                 oct_dev->octeon_id,
			                 get_oct_app_string(handler->app_type)); 
			retval = handler->startptr(octidx, oct_dev);
			if(retval) {
				cavium_spin_lock(&octmodhandlers_lock);
				cavium_memset(&octmodhandlers[modidx], 0, sizeof(octeon_module_handler_t));
				cavium_spin_unlock(&octmodhandlers_lock);
				return retval;
			}
		}
	}

	return retval;
}






int
octeon_unregister_module_handler(uint32_t  app_type)
{
	int                        modidx, octidx, retval=0;
	octeon_module_handler_t    handler;
	
	handler.app_type = CVM_DRV_NO_APP;

	cavium_spin_lock(&octmodhandlers_lock);
	/* Check if a handler exists for this app type. */
	for(modidx = 0; modidx < OCTEON_MAX_MODULES; modidx++) {
		if(octmodhandlers[modidx].app_type == app_type) {
			cavium_memcpy(&handler, &octmodhandlers[modidx], sizeof(octeon_module_handler_t));
			cavium_memset(&octmodhandlers[modidx], 0 ,sizeof(octeon_module_handler_t));
			break;
		}
	}
	cavium_spin_unlock(&octmodhandlers_lock);



	if(modidx == OCTEON_MAX_MODULES) {
		cavium_error("OCTEON: No handler for application type 0x%x\n",
		             app_type);
		return -ENODEV;
	}



	/* Call the stop method for all existing octeon devices. */
	for (octidx = 0; octidx < MAX_OCTEON_DEVICES; octidx++) {
		octeon_device_t  *oct_dev = octeon_device[octidx];

		if((oct_dev) && (oct_dev->app_mode & handler.app_type)) {
			cavium_print_msg("OCTEON[%d]: Stopping modules for app_type: %s\n",
			                 oct_dev->octeon_id,
			                 get_oct_app_string(handler.app_type)); 
			retval |= handler.stopptr(octidx, oct_dev);
		}
	}


	cavium_print_msg("OCTEON: Unregistered handler for app_type: %s\n",
	                 get_oct_app_string(app_type));

	return retval;
}





void
print_octeon_state_errormsg(octeon_device_t  *oct)
{
	cavium_error("Octeon device (%d) is in state (0x%x)\n",
	             oct->octeon_id, cavium_atomic_read(&oct->status));
	cavium_error("\n Did you follow the driver load sequence in components/driver/README.txt?\n");
}







/** Get the octeon device pointer.
 *  @param octeon_id  - The id for which the octeon device pointer is required.
 *  @return Success: Octeon device pointer.
 *  @return Failure: NULL.
 */
octeon_device_t *
get_octeon_device(uint32_t octeon_id)
{
    if(octeon_id >= MAX_OCTEON_DEVICES)
        return NULL;
    else
        return octeon_device[octeon_id];
}



/** Gets the octeon device id when the device structure is given.
 *  @return - The octeon device id.
 */
uint32_t
get_octeon_id(octeon_device_t *octeon_dev)
{
    return octeon_dev->octeon_id;
}


/** Get the number of Octeon devices currently in the system.
 *  This function is exported to other modules.
 *  @return  Count of octeon devices.
 */
uint32_t
get_octeon_count(void)
{
  return octeon_device_count;
}


uint32_t
octeon_get_cycles_per_usec(octeon_device_t  *oct)
{
	if(oct->chip_id <= OCTEON_CN58XX)
		return OCTEON_PCI_BUS_HZ;

	if(oct->chip_id == OCTEON_CN56XX)
		return cn56xx_pass2_core_clock(oct);

	if(oct->chip_id == OCTEON_CN63XX)
		return cn63xx_core_clock(oct);

	if(oct->chip_id == OCTEON_CN66XX) 
		return cn66xx_core_clock(oct);

	if(oct->chip_id == OCTEON_CN68XX)
		return cn68xx_core_clock(oct);

	return 0;
}





/** Get the octeon id assigned to the octeon device passed as argument.
 *  This function is exported to other modules.
 *  @param dev - octeon device pointer passed as a void *.
 *  @return octeon device id
 */
int
get_octeon_device_id(void *dev)
{
	octeon_device_t   *octeon_dev = (octeon_device_t *)dev;
	int i;

	for(i = 0; i < MAX_OCTEON_DEVICES; i++) {
		if(octeon_device[i] == octeon_dev)
			return (octeon_dev->octeon_id);
	}
	return -1;
}


/** Get the octeon device from the octeon id passed as argument.
 *  This function is exported to other modules.
 *  @param octeon_id - octeon device id.
 *  @return octeon device pointer as a void *.
 */
void *
get_octeon_device_ptr(int octeon_id)
{
	return (void *)get_octeon_device(octeon_id);
}


unsigned long
octeon_map_single_buffer(int octeon_id, void *virt_addr, uint32_t  size,
                         int direction)
{
	octeon_device_t  *oct_dev = get_octeon_device(octeon_id);

	if(oct_dev == NULL)
		return 0UL;

	return octeon_pci_map_single(oct_dev->pci_dev, virt_addr, size, direction);
}


void
octeon_unmap_single_buffer(int octeon_id,  unsigned long dma_addr,
                           uint32_t  size, int direction)
{
	octeon_device_t  *oct_dev = get_octeon_device(octeon_id);

	if(oct_dev == NULL)
		return;

	octeon_pci_unmap_single(oct_dev->pci_dev, dma_addr, size, direction);
}



unsigned long
octeon_map_page(int octeon_id, cavium_page_t  *page, unsigned long offset,
                uint32_t size, int direction)
{
	octeon_device_t  *oct_dev = get_octeon_device(octeon_id);

	if(oct_dev == NULL)
		return 0UL;

	return octeon_pci_map_page(oct_dev->pci_dev, page, offset, size, direction);
}


void
octeon_unmap_page(int octeon_id,  unsigned long dma_addr, uint32_t  size,
                  int direction)
{
	octeon_device_t  *oct_dev = get_octeon_device(octeon_id);

	if(oct_dev == NULL)
		return;

	octeon_pci_unmap_page(oct_dev->pci_dev, dma_addr, size, direction);
}


extern int
octeon_reset_recv_buf_size(octeon_device_t  *, int , int);

int
octeon_reset_oq_bufsize(int octeon_id, int q_no, int newsize)
{
	octeon_device_t  *oct = get_octeon_device(octeon_id);

	if(oct == NULL)
		return -ENODEV;

	return octeon_reset_recv_buf_size(oct, q_no, newsize);
}



int
octeon_is_active(int oct_id)
{
	octeon_device_t   *oct = get_octeon_device(oct_id);

	if(oct == NULL)
		return -ENODEV;

	return (cavium_atomic_read(&oct->status) == OCT_DEV_RUNNING);
}



int
octeon_active_dev_count(void)
{
	int  i, cnt = 0;

	for(i = 0; i < octeon_device_count;  i++)
		cnt += octeon_is_active(i);

	return cnt;
}



int
octeon_all_devices_active(void)
{
	return (octeon_active_dev_count() ==  octeon_device_count);
}



/* $Id: octeon_device.c 66446 2011-10-25 02:31:59Z mchalla $ */
