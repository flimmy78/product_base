#ifndef __SEM_KERN_H__
#define __SEM_KERN_H__

#include <linux/cdev.h>
#include <linux/interrupt.h>

#define DRIVER_NAME "Kernel Sem"
#define DEVICE_NAME "ksem"

#define ksemDBG(format, arg...)  \
	do{if(unlikely(ksem_debug)){ \
	        printk(KERN_ERR DRIVER_NAME ": " format, ## arg); \
	}}while(0)

#define ksemERR(format, arg...) \
	printk(KERN_ERR DRIVER_NAME ": " format, ## arg)


#define GPIO_BIT_CFG5               0x8001070000000828
#define GPIO_RX_DAT                 0x8001070000000880
#define GPIO_INT_CLR                0x8001070000000898

#define CPLD_BASE_ADDR              0x800000001D010000
#define CPLD_PRODUCT_ID_REG			0x2
#define CPLD_BOARD_ID_REG			0x3
#define CPLD_INT_MASK_REG1			0x9
#define CPLD_INT_MASK_REG2			0xa
#define CPLD_INT_STATE_REG1			0xb
#define CPLD_INT_STATE_REG2			0xc
#define CPLD_INT_TYPE_REG2			0x11
#define CPLD_HW_STATE_REG			0x11
#define CPLD_INT_TYPE_REG1		    0x17
#define CPLD_MASTER_SLOT_ID_REG     0x28
#define CPLD_REMOTE_MASTER_STATE_REG	0x29
#define CPLD_INT_MASK_REG3          0x30
#define CPLD_INT_STATE_REG3			0x31
#define CPLD_INT_TYPE_REG3			0x32
#define CPLD_LOCAL_BOARD_STATE_REG	0x38

#define I2C_POWER_STATE_REG         0xd0
#define I2C_POWER1_STATE_ADDR       0x3c
#define I2C_POWER2_STATE_ADDR       0x3d
#define I2C_POWER3_STATE_ADDR       0x3e
#define I2C_POWER4_STATE_ADDR       0x3f
#define I2C_POWER1_INFO_ADDR        0x54
#define I2C_POWER2_INFO_ADDR        0x55
#define I2C_POWER3_INFO_ADDR        0x56
#define I2C_POWER4_INFO_ADDR        0x57

#define MAX_PAYLOAD                 1024

#define CPLD_PRODUCT_TYPE_MASK 0x7
#define	PRODUCT_AX7605I 0x1
#define	PRODUCT_AX8800  0x7
#define	PRODUCT_AX8610  0x4
#define PRODUCT_AX8606  0x5
#define PRODUCT_AX8603	0X6
#define AX81_CRSMU      0x0
#define AX81_AC12C		0x1
#define AX81_AC8C		0x2
#define AX81_1X12G12S   0X5
#define AX71_CRSMU      0x80  

#define AX81_AC_12X     0x3
typedef struct sem_dev_s
{
    struct cdev cdev;
	unsigned int irq;
	struct timer_list semTimer;
}sem_dev_t;

typedef struct kernel_nlmsg_s
{
    int action_type;
    int ipgport;
}kernel_nlmsg_t;

typedef struct nl_msg_head{
	int type;
	int object;
    int count;
	int pid;
}nl_msg_head_t;

typedef struct board_active_info{
	int action_type;
    int slotid;
}board_active_info_t;

typedef struct product_state_info_s{
	int action_type;
    int slotid;
}product_state_info_t;

typedef struct asic_port_update_s{
	int action_type;
	int slot_id;
	int eth_l_index;
}asic_port_update_t;

typedef struct port_bitmap_s{
    unsigned int low_bmp;
    unsigned int high_bmp;
}port_bitmap_t;
typedef struct asic_vlan_sync_s{
	int action_type;
	int slot_id;
	int vlan_id;
	char bond_slot[10];
	char vlan_name[21];
    port_bitmap_t untagPortBmp[10]; 
	port_bitmap_t tagPortBmp[10];	
}asic_vlan_sync_t;

typedef struct active_stdby_switch_s{
	int action_type;
    int active_slot_id;
}active_stdby_switch_t;

typedef struct power_state_info_s{
	int action_type;
    int power_id;
}power_state_info_t;

typedef struct fan_state_info_s{
	int action_type;
    int fan_id;
}fan_state_info_t;

typedef struct netlink_msg_s{	
	int msgType;
	union{
	    product_state_info_t productInfo;
		kernel_nlmsg_t       kernelInfo;
		asic_port_update_t   portInfo;
		board_active_info_t  boardInfo;
		asic_vlan_sync_t     vlanInfo;
		active_stdby_switch_t swInfo; 
		power_state_info_t   powerInfo;
		fan_state_info_t     fanInfo;
	}msgData;	
}netlink_msg_t;

enum netlink_type
{
    LOCAL_BOARD,
	CROSS_BOARD,
    OVERALL_UNIT,
    NL_TYPE_MAX
};

enum netlink_msg_type
{
	BOARD_STATE_NOTIFIER_EVENT,
	SYSTEM_STATE_NOTIFIER_EVENT,
	ASIC_ETHPORT_UPDATE_EVENT,
	OCTEON_ETHPORT_LINK_EVENT,
	ASIC_VLAN_SYNC_ENVET,
	ACTIVE_STDBY_SWITCH_OCCUR_EVENT,
	ACTIVE_STDBY_SWITCH_COMPLETE_EVENT,
	POWER_STATE_NOTIFIER_EVENT,
	FAN_STATE_NOTIFIER_EVENT,
	ASIC_VLAN_NOTIFIER_ENVET,
	ASIC_DYNAMIC_TRUNK_NOTIFIER_EVENT,
	NL_MSG_TYPE_MAX
};

typedef enum
{
    HOTPLUG_MODE,
    COMMAND_MODE,
    PANIC_MODE,
    SWITCH_MODE_MAX
}active_mcb_switch_type_t;

enum board_state
{
	BOARD_INSERTED_AND_REMOVED, /*to describe the slot were insert a board,and the board is removed for some reasons.*/
	BOARD_EMPTY,
	BOARD_REMOVED,
	BOARD_INSERTED,
	BOARD_INITIALIZING,
	BOARD_READY,
	BOARD_RUNNING
};

enum module_type
{
	COMMON_MODULE,
	SEM_MODULE,
	NPD_MODULE,
	MOUDLE_TYPE_MAX
};

enum power_state
{
	POWER_ON,
	POWER_OFF,
	POWER_INSERTED,		
	POWER_REMOVED,
	VCC_ALARM,
	POWER_STATE_MAX
};

enum fan_state
{
	FAN_REMOVED,
	FAN_INSERTED,
	FAN_ALARM,
	FAN_NORMAL,
	FAN_STATE_MAX
};

enum flag_type
{
    AX7605I_VCC_STATE,              
    AX7605I_POWER1_STATE,           
    AX7605I_POWER2_STATE,          
    AX7605I_FAN_STATE, 
    AX7605I_REMOTE_STATE,
    AX7605I_SLAVE_STATE,
    AX8610_POWER1_STATE,
    AX8610_POWER2_STATE,
    AX8610_POWER3_STATE,
    AX8610_POWER4_STATE,
    MAX_FLAG                       	
};

void init_board(void);
void release_board(void);
void sem_kern_netlink_notifier(char *sendBuf, int msgLen);
void check_on_board_state_for_ax7605i(void);
void check_on_hwState_for_ax7605i(void);
void check_on_hwState_for_ax8603(void);
void check_on_intState1_for_ax86xx(unsigned long intState1);
void check_on_intState2_for_ax86xx(unsigned long intState2);
void check_on_intState3_for_ax86xx(unsigned long intState3);
void check_on_power_down_for_ax86xx(void);
#endif
