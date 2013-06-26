#ifndef __BOARD_FEATURE_H_
#define __BOARD_FEATURE_H_

#include "board/board_common.h"
#include "board/board_define.h"

#define bool char

#define BOARD_DEFAULT_AP_COUNTER 1024

/**
  * read write phy_QT Command parameter
.
  */


typedef struct readphy_QT
{
	int regaddr;
	unsigned int location;
	int val;
}readphy_QT;

typedef struct 
{
	int type;	
	int device_num;
	int port_num;
	char name[MAX_BOARD_PORT_NAME_LEN];
	bool is_obc;
	bool is_cscd;
	unsigned int remote_slot;//slot number of this port connect to, obc port or cscd port
}obc_cscd_port_t;

struct module_info_s {
	//enum module_id_e id;
	//enum module_status_e state;
	unsigned char hw_version;
	char *modname;
	char *sn;
	void *ext_slot_data;   /* extend info could be organized as a link list.*/
};

typedef struct module_info_s board_man_param_t;


typedef struct ams_info_s
{
    unsigned char name[32];
    unsigned short type;
    unsigned short index;
    unsigned short dev_id;
    unsigned short rev_id;
} ams_info_t;

/* Mavell 88E6185 register definations */
typedef union
{
    unsigned short int u16;
    struct smi_cmd_s
    {
		unsigned int smi_busy	: 1;
		unsigned int reserved	: 2;
        unsigned int smi_mode   : 1;
        unsigned int smi_op     : 2;       /* SMI Opcode 1=write 2=read */
		unsigned int dev_addr	: 5;       /* Device Address */
		unsigned int reg_addr	: 5;	   /* PHY Register Offset */
    } s;
} smi_cmd_88e6185_reg_t;

typedef struct ams_fix_param_s
{
    unsigned short type;
    unsigned short num;
    long (*ams_pre_init)(int index);
    long (*ams_driver_init)(int index);
    long (*ams_enable)(int index);
    long (*ams_disable)(int index);
    long (*ams_info_get)(int index, ams_info_t *info);
}ams_fix_param_t;


typedef struct board_feature_s
{
    int  jumbo_size;
    int  max_macaddr;
    int  max_macapp;
    int  max_macmpls;
    int  max_ext_macaddr;

    bool stack;
    bool rspan;
    bool sflow;
}board_feature_t;

typedef struct
{
	int board_code;
	int board_type;
	int slot_id;
	char name[MAX_BOARD_NAME_LEN];
	bool is_master;
	bool is_active_master;
	bool is_use_default_master;
	unsigned int function_type;
	int board_state;
	unsigned int asic_start_no;
	unsigned int asic_port_num;
	unsigned int board_ap_counter;
}board_info_syn_t;

typedef struct board_fix_param_s board_fix_param_t;

struct board_fix_param_s
{
    int board_code;								//board hardware code on cpld
    int board_type;
	int board_id;
    char name[MAX_BOARD_NAME_LEN];
	unsigned char sn[SERIAL_NUMBER_LEN];
	int slot_id;
	bool is_master;								//is this board is master board
	bool cpu_num;                               //single CPU or dual CPU
	bool is_active_master;						//is this board is the active master board
	bool is_use_default_master;					//this symbol is used for boards using obc port and cscd port,
	unsigned int port_num_on_panel;
	unsigned int asic_port_num;
	unsigned int asic_start_no;
	board_port_t port_arr[MAX_PORT_NUM_PER_BOARD];
	unsigned int function_type;
	unsigned int obc_port_num;
	obc_cscd_port_t obc_port[MAX_OBC_PORT_NUM];
	unsigned int cscd_port_num;
	obc_cscd_port_t cscd_port[MAX_CSCD_PORT_NUM];
	int board_state;
	unsigned int board_ap_counter;
	unsigned int board_ap_max_counter;
	int (* master_active_or_backup_state_set)(unsigned int state);
	int (* get_remote_board_master_state)(unsigned int *state);
	int (* get_slot_id)(unsigned int *slot_id);
	int (* set_initialize_state)(unsigned int state);
	int (* get_initialize_state)(unsigned int *state);
	int (* slot_reset)(unsigned int mask);
	int (* get_cpld_version)(unsigned int *version);
	int (* get_board_type)(unsigned int *type);
	int (* get_product_type)(unsigned int *type);
	int (* get_product_serial)(unsigned int *serial);
	int (* cpld_test)(void);
	void (* reboot_self)(void);
	int (* hard_self_reset)(void);
	int (* board_self_init)(void);
	int (* update_local_board_state)(board_fix_param_t *board);
	int (* board_register)(board_fix_param_t *board, int sd, board_info_syn_t *board_info);
	int (* syn_board_info)(int sd, board_info_syn_t *board_info);
	int (* local_board_dbm_file_create)(board_fix_param_t *board);
	int (* get_port_name)(int slot, int port, char *ifName);
	int (* read_eth_port_info)(int slot, int port, struct eth_port_s *port_info);
	int (* get_eth_port_trans_media)(unsigned int port, unsigned int *media); 
	int (* set_port_preferr_media)(unsigned int port, unsigned int media);
	int (* set_port_admin_status)(unsigned int port, unsigned int status);
	int (* set_port_autoneg_status)(unsigned int port, unsigned int status);
	int (* set_ethport_speed)(unsigned int port, PORT_SPEED_ENT speed);		
	int (* set_port_duplex_mode)(unsigned int port, unsigned int mode);
	int (* set_ethport_flowCtrl)( unsigned char port, unsigned long state);
	int (* set_ethport_backPressure)( unsigned char port, unsigned long state);
	int (* set_ethport_speed_autoneg)(unsigned char port, unsigned long speed);
	int (* set_ethport_duplex_autoneg)(unsigned char port, unsigned long mode);		
	int (* set_ethport_fc_autoneg)(unsigned char port, unsigned long state);
	int (* set_ethport_mtu)(int slot, int port, unsigned int mtu_size);
	int (* init_local_mac_info)(void);
	int (* update_local_mac_info)(unsigned char *mac_addr);
	int (* init_local_sn_info)(void);
	int (* show_system_environment)(env_state_t* env_state);
};




#endif
