/*
 * purpose: read sysinfo from EEPROM.
 * author: Autelan. Co. Ltd.
 * codeby: baolc
 * 2008-06-19
 */

#ifndef _BMK_READ_EEPROM_
#define _BMK_READ_EEPROM_

#include "bmk_main.h"

#define DRIVER_NAME "bm"

 #define DBG(o,f, x...) \
	if (o) { printk(KERN_DEBUG DRIVER_NAME ":" f, ## x); } 

extern unsigned int debug_ioctl;

#define I2C_POWER_STATE_REG         0xd0

/** 
  * Product sysinfo, the data comes from backplane-board.
  * codeby: baolc
  */
typedef struct ax_sysinfo_product_t
{
	char  ax_sysinfo_module_serial_no[21]; //data should be 20 bytes the last byte is '\0'
	char  ax_sysinfo_module_name[25];  //data max length should be 24 bytes the last byte is '\0' 
	char  ax_sysinfo_product_serial_no[21]; //data should be 20 bytes the last byte is '\0' 
	char  ax_sysinfo_product_base_mac_address[13]; //data should be 12 bytes the last byte is '\0' 
	char  ax_sysinfo_product_name[25]; //data max length should be 24 bytes the last byte is '\0' 
	char  ax_sysinfo_software_name[25]; //data max length should be 24 bytes the last byte is '\0' 
	char  ax_sysinfo_enterprise_name[65]; //data max length should be 64 bytes the last byte is '\0' 
	char  ax_sysinfo_enterprise_snmp_oid[129]; //data max length should be 128 bytes the last byte is '\0' 
	char  ax_sysinfo_snmp_sys_oid[129]; //data max length should be 128 bytes the last byte is '\0' 
	char  ax_sysinfo_built_in_admin_username[33]; //data max length should be 32 bytes the last byte is '\0' 
	char  ax_sysinfo_built_in_admin_password[33]; //data max length should be 32 bytes the last byte is '\0'  
}ax_sysinfo_product_t;

typedef ax_sysinfo_product_t ax_product_sysinfo;

typedef struct ax_read_module_sysinfo
{
	int product_id;					//the product's id must be 3~7 for 3000~7000
	int slot_num;							//0~4 
	char  ax_sysinfo_module_serial_no[21]; //data should be 21 bytes the last byte is '\0'
	char  ax_sysinfo_module_name[25];  //data max length should be 24 bytes the last byte is '\0'
}ax_module_sysinfo;

/**
  * Board info, the data comes from mainboard or child-board.
  * code by baolc
  */
typedef struct ax_sysinfo_single_board_t
{
	char  ax_sysinfo_module_serial_no[21]; //data should be 20 bytes the last byte is '\0'
	char  ax_sysinfo_module_name[25];  //data max length should be 24 bytes the last byte is '\0'
}ax_sysinfo_single_board_t;

typedef union
{
    unsigned char u8;
	struct ds650_power_state_s
	{
		unsigned int ocp        : 1;
    	unsigned int uvp        : 1;
    	unsigned int ovp        : 1;
    	unsigned int fan_ok     : 1;
    	unsigned int ac_ok      : 1;
    	unsigned int temp_ok    : 1;
    	unsigned int v33sb_ok   : 1;
    	unsigned int v12_ok     : 1; 
	}s;
}ds650_power_state_t;

typedef struct sys_power_state
{
	unsigned int power_no;
	unsigned int power_supply;
}sys_power_state_t;

int bm_ax_read_sysinfo_product(unsigned char eeprom_addr, ax_sysinfo_product_t* sysinfo);
/**
  * Read single board sysinfo from eeprom.
  * @return: -1 for failure. 0 for sucess.
  */
int bm_ax_read_sysinfo_single_board(unsigned char eeprom_addr, ax_sysinfo_single_board_t* sysinfo);

/**
  * Read module sysinfo from eeprom.
  * @return: -1 for failure. 0 for sucess.
  */
int bm_ax_read_module_sysinfo(ax_module_sysinfo* sysinfo);  

/**
* Read sysinfo for proc entry
* @return : -1 is failure, 0 is success.
*/
int ax_read_sysinfo_from_eeprom_proc(unsigned char  eeprom_addr, ax_sysinfo_product_t* sysinfo);

/**
* Read power state form eeprom on DS650/DS850
* @return : -1 is failure, 0 is success.
*/
int do_get_power_state(sys_power_state_t *power_state);

#endif 
