/* bm -- board management (i2c, gpio, SMI,bootbus, cpld, ....) setting 
 *
 */
#include "sysdef/npd_sysdef.h"
#include "npd/nam/npd_amapi.h"
#include "bmk_soft_read_eeprom.h"
#include "bmk_read_eeprom.h"

#ifndef BM_MAIN_H
#define BM_MAIN_H

#define BM_BIG_ENDIAN 0
#define BM_LITTLE_ENDIAN 1

#define MAC_ADDRESS_LEN	6

#define SYSINFO_SN_LENGTH	  20
#define SYSINFO_PRODUCT_NAME  24
#define SYSINFO_SOFTWARE_NAME 24
#define SYSINFO_ENTERPRISE_NAME  64
#define SYSINFO_ENTERPRISE_SNMP_OID  128
#define SYSINFO_BUILT_IN_ADMIN_USERNAME  32
#define SYSINFO_BUILT_IN_ADMIN_PASSWORD  32

#define SYSINFO_MODULE_SERIAL_NO  20
#define SYSINFO_MODULE_NAME       24 

/**
  * EEPROM addresses on chassis device.
  */
#define  BM_AX_BACKPLANE_EEPROM_ADDR         0X56
#define  BM_AX_MODULE0_EEPROM_ADDR           0x57

/**
  * EEPROM addresses on box device.
  */
#define  BM_AX_MAINBOARD_EEPROM_ADDR         0X56

extern unsigned int debug_ioctl;

typedef struct bm_op_args_s {
	unsigned long long op_addr;
	unsigned long long op_value;  // ignore on read in arg, fill read value on write return value
	unsigned short op_byteorder; // destination byte order. default is bigendiana.
	unsigned short op_len;
	unsigned short op_ret; // 0 for success, other value indicate different failure.
} bm_op_args;

typedef struct bm_op_cpld_read {
	unsigned char 	slot;		//which slot's CPLD to read from
	unsigned int	regAddr;	//CPLD register address
	unsigned char	regData;	//CPLD register value
}bm_op_cpld_args;

typedef struct bm_op_bit64_read {
	unsigned long long 	regAddr;	//GPIO register address
	unsigned long long	regData;	//GPIO register data
}bm_op_rbit64_args;

typedef struct bm_op_sysinfo_common {
	unsigned char mac_addr[MAC_ADDRESS_LEN]; // system mac address
	unsigned char sn[SYSINFO_SN_LENGTH]; // module or backplane or mainboard serial number
}bm_op_sysinfo_args;

typedef struct bm_op_module_sysinfo {
	unsigned char slotno;
	bm_op_sysinfo_args common;
}bm_op_module_sysinfo;

typedef bm_op_sysinfo_args bm_op_backplane_sysinfo;
typedef bm_op_sysinfo_args bm_op_mainboard_sysinfo;

typedef enum ax7_board_state_e {
	AX7_BOARD_STATE_ACTIVE = 0,
	AX7_BOARD_STATE_INACTIVE
}AX7_BOARD_STATE_E;

/**
  * ioctl param and cmd for read sysinfo from eeprom.
  */
enum bm_eeprom_type_t
{
	BM_EEPROM_BACKPLANE, //backplane on chassis device
	BM_EEPROM_MODULE0,  //main board on chassis device
	BM_EEPROM_MAINBOARD //main board on box device
};

typedef struct bm_op_sysinfo_eeprom_args
{
	enum bm_eeprom_type_t  eeprom_type;
}bm_op_sysinfo_eeprom_args;

typedef struct bm_op_read_eth_port_stats_args
{
	int portNum;
	int clear; /* 1 for clear stats CSRs */
	struct npd_port_counter port_counter;
	struct port_oct_s			portOctCount;
}bm_op_read_eth_port_stats_args;
#define MAX_BUF_LEN_16BIT 0x200
typedef struct soft_i2c_read_write_16bit
{
	int slot_num;
	char slave_addr;
	int internal_addr;
	int buf_len;
	char data[MAX_BUF_LEN_16BIT];
} soft_i2c_read_write_16bit;

#define MAX_BUF_LEN_8BIT 0x10
typedef struct soft_i2c_read_write_8bit
{
	int slot_num;
	char slave_addr;
	char internal_addr;
	int port_num;
	//int data;
	int buf_len;
	char data[MAX_BUF_LEN_8BIT];
} soft_i2c_read_write_8bit;

/**
  * read  phy_QT Command parameter
.
  */
typedef struct readphy_QT
{
	int regaddr;
	unsigned int location;
	int val;
}readphy_QT;

//#define PRODUCT_NAME_MAX_LEN 20
typedef struct sys_product_type
{
	unsigned char product_num;
	unsigned char module_num;
	//char product_name[PRODUCT_NAME_MAX_LEN];
}sys_product_type_t;

typedef struct fpga_file
{
	char fpga_bin_name[256];		//FPGA burning filename
	int result;
}fpga_file;
#define BM_MINOR_NUM 0
#define BM_MAJOR_NUM 0xEC

#define BM_IOC_MAGIC 0xEC 
#define BM_IOC_RESET	_IO(BM_IOC_MAGIC,0)


/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 * X means "eXchange": switch G and S atomically
 * H means "sHift": switch T and Q atomically
 */


#define BM_IOC_G_  			_IOWR(BM_IOC_MAGIC,1,bm_op_args) // read values
#define BM_IOC_X_ 				_IOWR(BM_IOC_MAGIC,2,bm_op_args) // write and readout wrote value
#define BM_IOC_CPLD_READ		_IOWR(BM_IOC_MAGIC,3,bm_op_cpld_args) //read cpld registers
#define BM_IOC_BIT64_REG_STATE 			_IOWR(BM_IOC_MAGIC,4,bm_op_rbit64_args) //read 64-bit status register
#define BM_IOC_CPLD_WRITE   				_IOWR(BM_IOC_MAGIC,5,bm_op_cpld_args) //write cpld registers by zhang

#define BM_IOC_MODULE_SYSINFO_READ		_IOWR(BM_IOC_MAGIC, 7, bm_op_module_sysinfo) // read chassis module sysinfo
#define BM_IOC_SYSINFO_EEPROM_READ		_IOWR(BM_IOC_MAGIC, 8, bm_op_sysinfo_eeprom_args) //read sysinfo from eeprom

#define BM_IOC_READ_MODULE_SYSINFO _IOWR(BM_IOC_MAGIC, 6, ax_module_sysinfo)//read module sysinfo for series 7 or series 6

#define BM_IOC_BOARD_ETH_PORT_STATS_READ  _IOWR(BM_IOC_MAGIC, 9, bm_op_read_eth_port_stats_args) //read board eth-port statistics
#define BM_IOC_ENV_EXCH  		_IOWR(BM_IOC_MAGIC, 10,boot_env_t) //read board eth-port statistics
#define BM_IOC_SOFT_SYSINFO_READ_16BIT _IOWR(BM_IOC_MAGIC, 10, ax_soft_sysinfo)//read sysinfo from sub-card through soft i2c(16bit)

#define BM_IOC_SOFT_I2C_READ8_		_IOWR(BM_IOC_MAGIC, 11, soft_i2c_read_write_8bit)  //read data from eeprom
#define BM_IOC_SOFT_I2C_WRITE8_		_IOWR(BM_IOC_MAGIC, 12, soft_i2c_read_write_8bit)  //write data to eeprom
#define BM_IOC_SOFT_I2C_READ16_		_IOWR(BM_IOC_MAGIC, 13, soft_i2c_read_write_16bit)  //read data from sub card eeprom 
#define BM_IOC_SOFT_I2C_WRITE16_	_IOWR(BM_IOC_MAGIC, 14, soft_i2c_read_write_16bit)  //write data to sub card eeprom 

#define BM_IOC_READ_PRODUCT_SYSINFO  _IOWR(BM_IOC_MAGIC, 15, ax_product_sysinfo)//read backplane or mainboard sysinfo from eeprom
#define BM_IOC_BACKPLANE_SYSINFO_READ	BM_IOC_READ_PRODUCT_SYSINFO	// read back plane sysinfo
#define BM_IOC_MAINBOARD_SYSINFO_READ  BM_IOC_READ_PRODUCT_SYSINFO  //read main board sysinfo

#define BM_IOC_GET_MAC	_IOWR(BM_IOC_MAGIC, 16, sys_mac_add) 

#define BM_IOC_BOOTROM_EXCH    _IOWR(BM_IOC_MAGIC, 17,bootrom_file)/*gxd update bootrom based on cli*/

#define BM_IOC_GET_PRODUCET_TYPE	_IOWR(BM_IOC_MAGIC, 18, sys_product_type_t)
#define BM_GET_POWER_STATE          _IOWR(BM_IOC_MAGIC, 19, sys_power_state_t)
#define BM_IOC_FPGA_WRITE 				_IOWR(BM_IOC_MAGIC,20,fpga_file) //FPGA online burning
#define BM_IOC_LOAD_QT2025_FIRMWARE     _IOWR(BM_IOC_MAGIC, 21, int)    /*cofigure SMI0 apply to IEEE802.3-2005 clause 45
                                                                        and load firmware for phy QT2025*/
#define BM_IOC_PORT_admin_status     _IOWR(BM_IOC_MAGIC, 22, int)
#define BM_IOC_READ_QT2025     _IOWR(BM_IOC_MAGIC, 23, int)
#define BM_IOC_FPGA_W 				_IOWR(BM_IOC_MAGIC,24,bm_op_args)//write fpga reg no delay

#define BM_IOC_MAXNR 32

#endif
