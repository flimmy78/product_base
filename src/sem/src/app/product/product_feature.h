#ifndef __PRODUCT_FEATURE_H_
#define __PRODUCT_FEATURE_H_


#include "board/board_feature.h"

#define PRODUCT_INIT_IN_SINGLE_BOARD_MODE "/dbm/parameters/product_init_in_single_board_mode"

enum
{
	PRODUCT_AX7605I,
	PRODUCT_AX8610,
	PRODUCT_AX8606,
	PRODUCT_AX8603,
	PRODUCT_AX8800,
	PRODUCT_SINGLE_BOARD,
	PRODUCT_MAX_NUM
};

enum
{
	XXX_YYY_SINGLE_BOARD,
	XXX_YYY_AX7605I = 1,
	XXX_YYY_AX8610 = 0x4,
	XXX_YYY_AX8606 = 0x5,
	XXX_YYY_AX8603 = 0x6,
	XXX_YYY_AX8800 = 0x7
};


enum software_syn_result
{
	SOFTWARE_SYN_SUCCESS,
	SOFTWARE_SYN_FAILED,
	SOFTWARE_SYN_NO_NEED
};

#define MAX_SUPPORT_BOARD_NUM	14
#define MASTER_SLOT_MAX_NUM 10
#define MAX_MASTER_SLOT_NUM 2

#define SERIAL_NUMBER_LEN	10
#define PRODUCT_NAME_LEN	30

#define MAC_ADDR_LEN 6

#define RESET_CMD_FROM_SEM 0
#define RESET_CMD_FROM_VTYSH 1

#define SOFTWARE_VERSION_SEND_TRY_COUNT	3
//#define NULL 	0

typedef struct product_sysinfo_s {
	char *sn;
	char *name;
	char *basemac;  /* 12 char of mac address  with no : or - spliter.*/
	char *sw_name;
	char *enterprise_name;
	char *enterprise_snmp_oid;
	char *snmp_sys_oid;
	char *built_in_admin_username;
	char *built_in_admin_passwd;
}product_sysinfo_t;

typedef struct board_sub_info
{
	char *name;
	int index;
}board_sub_info_t;


typedef struct
{
	int product_serial;
	int product_type;
	char name[PRODUCT_NAME_LEN];
	unsigned char sn[SERIAL_NUMBER_LEN];
	unsigned char base_mac_addr[MAC_ADDR_LEN];
	int slotcount;
	int master_slot_count;
	int master_slot_id[MAX_MASTER_SLOT_NUM];
	int default_master_slot_id;
	int active_master_slot_id;
	int more_than_one_master_board_on;
	int is_distributed;
	int fan_num;
	int product_state;
	unsigned int board_on_mask;
	board_info_syn_t product_slot_board_info[MAX_SUPPORT_BOARD_NUM];
}product_info_syn_t;


typedef struct product_fix_param_s product_fix_param_t;

struct product_fix_param_s
{
	int product_serial;
	int product_type;
	int product_id;
	char name[PRODUCT_NAME_LEN];
	unsigned char sn[SERIAL_NUMBER_LEN];
	unsigned char base_mac_addr[MAC_ADDR_LEN];
    int slotcount;
	int master_slot_count;
	int master_slot_id[MAX_MASTER_SLOT_NUM];
	int default_master_slot_id;
	int more_than_one_master_board_on;
	int active_master_slot_id;
	int is_distributed;
	int fan_num;
	int product_state;
	product_sysinfo_t sys_info;
	board_fix_param_t **support_board_arr;
	unsigned int board_on_mask;
	//board_sub_info_t board_sub_info_arr[MAX_SUPPORT_BOARD_NUM];
	board_info_syn_t product_slot_board_info[MAX_SUPPORT_BOARD_NUM];
	int (* get_slot_id)(bool is_master, int *slot_id);
	int (* is_remote_master_on)(int *insert_state);
	int (* force_active_master)(void);
	int (* remote_master_state)(int *state);
	int (* get_board_on_mask)(unsigned int *mask, int slot_id, int default_slot_id);
	int (* board_inserted)(int slot_id);
	int (* board_removed)(int slot_id);
	int (* tipc_init)(board_fix_param_t *board);
	int	(* synchronise_software_version)(int sd, board_fix_param_t *board);
	int (* master_active_or_stby_select)(int *sd, board_fix_param_t *board);
	int (* active_stby_switch)(void *arg);
	int (* syn_software_version_response)(int sd, sem_tlv_t *temp_tlv_head, board_fix_param_t *board, int slot_id);
	int (* product_dbm_file_create)(product_info_syn_t *product);
	int (* update_product_state)(void);
	int (* syn_product_info)(board_info_syn_t *temp);
	//int (* fan_speed_adjust)();
	//int (* fan_enable)();
	//int (* business_board_electric_on_order)();
	int (* get_product_sys_info)(product_sysinfo_t *sys_info);	
	int (* sync_send_file)(int slot_id, char *filename, int syn_to_blk);
	int (* sync_recv_file)(char *recvbuf, int iflag, int slot_id);
	int (* hardware_reset_slot)(unsigned int bit_mask, unsigned int flag, unsigned int operater_flag); /* operater_flag : 0 for sem,other for manual */
};




typedef enum chassis_state_e
{
    CHASSIS_INITIALIZING,
    CHASSIS_INITDONE,
    CHASSIS_RUNNING
}chassis_state_t;


#endif
