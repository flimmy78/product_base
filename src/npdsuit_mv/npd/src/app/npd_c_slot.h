#ifndef __NPD_C_SLOT_H__
#define __NPD_C_SLOT_H__

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

struct module_type_data_s *(*module_type_data)(unsigned int module_id);
struct module_type_data_s *module_type_data_old(unsigned int module_id);

extern struct chassis_slot_s **chassis_slots;
extern char* nam_err_msg[];
extern char *product_id_str(unsigned int product_id);
extern char *module_id_str(unsigned int module_id);

#if 1
extern unsigned int nam_get_slotport_by_devport
(
	unsigned int devNum,
	unsigned int virtPortNo,
	unsigned int productId,
	unsigned int* slotNo,
 	unsigned int* localPortNo
);

extern unsigned int nam_get_devport_by_slotport
(
	unsigned int slotNo,
	unsigned int localPortNo,
	unsigned int moduleType,
	unsigned int* devNum,
	unsigned int* virtPortNo
);

unsigned int (*npd_get_global_index_by_devport)
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned int* eth_g_index
);

unsigned int (*npd_get_devport_by_global_index)
(
	unsigned int eth_g_index,
	unsigned char *devNum,
	unsigned char *portNum
);

unsigned int npd_get_global_index_by_devport_old
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned int* eth_g_index
);

unsigned int npd_get_devport_by_global_index_old
(
	unsigned int eth_g_index,
	unsigned char *devNum,
	unsigned char *portNum
);
#endif
void npd_init_board_check_thread
(
	void
);
extern void (*npd_init_chassis_slots)(void);
void npd_init_chassis_slots_old(void);
/**********************************************************************************
 * npd_reset_one_chassis_slot
 *
 * reset chassis slot global info structure,dedicated slot given by chssis_slot_index
 *
 *	INPUT:
 *		chassis_slot_index - chassis slot index to initialize
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *
 **********************************************************************************/

void npd_reset_one_chassis_slot
(
	int chassis_slot_index
);
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

#define MODULE_TYPE_ON_SLOT_INDEX(slot_index) (chassis_slots[(slot_index)]->module.id)
#define MODULE_STATUS_ON_SLOT_INDEX(slot_index) (chassis_slots[(slot_index)]->module.state)
#define MODULE_HW_VER_ON_SLOT_INDEX(slot_index) (chassis_slots[(slot_index)]->module.hw_version)
#define MODULE_SN_ON_SLOT_INDEX(slot_index) (chassis_slots[(slot_index)]->module.sn)
#define MODULE_NAME_ON_SLOT_INDEX(slot_index) (chassis_slots[(slot_index)]->module.modname)

#define ETH_LOCAL_PORT_COUNT(slot_index) (module_type_data(MODULE_TYPE_ON_SLOT_INDEX(slot_index))->eth_port_count)
#define ETH_LOCAL_PORT_START_NO(slot_index) (module_type_data(MODULE_TYPE_ON_SLOT_INDEX(slot_index))->eth_port_start_no)

#define ETH_LOCAL_PORTNO_ISLEGAL(slotno,portno) (CHASSIS_SLOTNO_ISLEGAL(slotno) &&( ( (portno) >= (ETH_LOCAL_PORT_START_NO(CHASSIS_SLOT_NO2INDEX(slotno) ))  ) && ( (portno) <= ((ETH_LOCAL_PORT_COUNT(CHASSIS_SLOT_NO2INDEX(slotno))) +(ETH_LOCAL_PORT_START_NO(CHASSIS_SLOT_NO2INDEX(slotno))) -1 ) )  ))

#define ETH_LOCAL_INDEX2NO(slot_index,eth_index) (ETH_LOCAL_PORT_START_NO(slot_index) +(eth_index))
#define ETH_LOCAL_NO2INDEX(slot_index,eth_no) ((eth_no) - (ETH_LOCAL_PORT_START_NO(slot_index)) )

#define EXT_SLOT_COUNT(slot_index) (module_type_data(MODULE_TYPE_ON_SLOT_INDEX(slot_index))->ext_slot_count)

#endif
