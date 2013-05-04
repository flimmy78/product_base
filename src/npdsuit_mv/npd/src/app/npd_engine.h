#ifndef __NPD_ENGINE_H__
#define __NPD_ENGINE_H__

/* 2 - for bi-direction; 128 - for per line size; 1024 - for maximum decriptor per queue*/
#define NPD_ENGINE_CPU_PERQUEUE_SHOWSIZE	(2*128*1024) 

int npd_fw_engines_init(void);
int npd_fw_engine_initialization_check(void);

/*******************************************************************************
* nam_thread_create
*
* DESCRIPTION:
*       Create Linux thread and start it.
*
* INPUTS:
*       name    - task name, valid when accessChip is TRUE
*       start_addr - task/thread Function to execute
*       arglist    - pointer to list of parameters for task/thread function
*	  accessChip - this thread/task need access chip register or not
*	  needJoin - this thread need pthread_join operation
*
* OUTPUTS:
*
* RETURNS:
*       0   - on success
*       1 - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
extern void nam_thread_create
(
	char	*name,
	unsigned (*entry_point)(void*),
	void	*arglist,
	unsigned char accessChip,
	unsigned char needJoin
);

/**********************************************************************************
 * nam_asic_set_cpufc_queue_quota
 * 
 * DESCRIPTION:
 *	This method set CPU port queue quota.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *		quota - quota assigned to the queue
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
extern unsigned long nam_asic_set_cpufc_queue_quota
(
	unsigned int queue,
	unsigned int quota
);

/**********************************************************************************
 * nam_asic_get_cpufc_queue_quota
 * 
 * DESCRIPTION:
 *	This method get CPU port queue quota.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *	
 *	OUTPUT:
 *		quota - quota assigned to the queue
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
extern unsigned long nam_asic_get_cpufc_queue_quota
(
	unsigned int queue,
	unsigned int *quota
);

/**********************************************************************************
 * nam_asic_set_cpufc_queue_shaper
 * 
 * DESCRIPTION:
 *	This method set CPU port queue shaper value.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *		shaper - shaper value assigned to the queue (unit is PPS)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
extern unsigned long nam_asic_set_cpufc_queue_shaper
(
	unsigned int queue,
	unsigned int shaper
);

/**********************************************************************************
 * nam_asic_set_cpufc_port_shaper
 * 
 * DESCRIPTION:
 *	This method set CPU port shaper value.	 
 *
 *
 *	INPUT:
 *		queue - queue number
 *		shaper - shaper value assigned to the queue (unit is PPS)
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
extern unsigned long nam_asic_set_cpufc_port_shaper
(
	unsigned int shaper
);

/**********************************************************************************
 * nam_asic_show_cpu_sdma_info
 * 
 * DESCRIPTION:
 *	This method show CPU sdma channel descriptor info
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *		queue - queue number
 *		direction - cpu SDMA channel direction: Rx or Tx or Both
 *		showSize - show buffer size
 *	
 *	OUTPUT:
 *		outStr - output string buffer
 *
 * 	RETURN:
 *		
 *		
 **********************************************************************************/
extern unsigned long nam_asic_show_cpu_sdma_info
(
	unsigned char devNum,
	unsigned char queue,
	unsigned char direction,
	unsigned char **outStr,
	unsigned int showSize
);

/**********************************************************************************
 * nam_asic_get_cpu_sdma_mib
 * 
 * DESCRIPTION:
 *	This method get CPU sdma channel Rx packets/octets and resource error counter
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *	
 *	OUTPUT:
 *		stats - cpu port dma channel mib info
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		other - if error found.
 *		
 **********************************************************************************/
extern unsigned long nam_asic_get_cpu_sdma_mib
(
	unsigned char devNum,
	CPU_PORT_DMA_CHANNEL_MIB_S **stats
);

/**********************************************************************************
 * nam_asic_get_cpu_mac_mib
 * 
 * DESCRIPTION:
 *	This method get CPU port Ethernet MAC mib counter
 *
 *
 *	INPUT:
 *		devNum - asic device number
 *	
 *	OUTPUT:
 *		stats - cpu port ethernet mac mib info
 *
 * 	RETURN:
 *		0 - if no error occurred.
 *		other - if error found.
 *		
 **********************************************************************************/
extern unsigned long nam_asic_get_cpu_mac_mib
(
	unsigned char devNum,
	CPU_PORT_MAC_MIB_S *stats
);
/**********************************************************************************
 * nam_asic_get_cpu_interface_type
 * 
 * get asic CPU interface type :using SDMA or using Ethernet MAC
 *
 *
 *	INPUT:
 *		devNum - device number
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - using SDMA (PCI interface)
 *		1 - using Ethernet MAC (SMI interface)
 *		
 **********************************************************************************/
extern unsigned long nam_asic_get_cpu_interface_type
(
	unsigned char devNum
);

/**********************************************************************************
 * nam_asic_get_instance_num
 * 
 * get asic instance number
 *
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		number of asic instance
 *		
 **********************************************************************************/ 
extern unsigned long nam_asic_get_instance_num
(
	void
);
extern int nam_asic_init_completion_check(void);
#endif
