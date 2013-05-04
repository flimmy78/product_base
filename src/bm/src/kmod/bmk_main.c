/* bm -- board management (i2c, gpio, bootbus, cpld, ....) setting 
 *
 */

#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/smp_lock.h>

#include <asm/octeon/cvmx.h>
#include <asm/octeon/cvmx-app-init.h>

#include "bmk_main.h"
#include "bmk_read_eeprom.h"
#include "bmk_eth_port_stats.h"
#include "bmk_operation_boot_env.h"
#include "ax_soft_i2c.h"



/*gxd*/
#include <linux/fcntl.h>
#include <linux/syscalls.h>

extern void flash_sect_erase(int );
extern int phy_QT2025();
extern int cvm_phy_enable();
extern int cvm_phy_disable();
extern int cvm_read_phy_QT(int regaddr,int location);
extern 	int read_bin_to_fpga(char *filename);

#define DRIVER_NAME "bm"
#define DRIVER_VERSION "0.1"
#define AX7_PRODUCT_SLOT_NUM 	5
#define AX71_2X12G12S_TYPE_CPLD_ADDR  0x3
#define AX71_2X12G12S_TYPE_CPLD_DATA  0x4

#define DBG(o,f, x...) \
	if (o) { printk(KERN_DEBUG DRIVER_NAME ":" f, ## x); }

extern cvmx_bootinfo_t *octeon_bootinfo;
extern flash_info_t info;
extern int get_flash_type(flash_info_t *);

struct bm_dev_s {
	long quantum;
	long qset;
	unsigned long size;
	unsigned long access_key;
	struct cdev cdev;
};

static struct bm_dev_s bm_dev;

unsigned int debug_ioctl = 0;
static unsigned int debug_bootbus = 0;
static unsigned int debug_gpio = 0;
static unsigned int debug_i2c = 0;
static unsigned int debug_rtc = 0;

char local_slot_num = -1;
int is_distributed = 0;

bm_op_args bm_ioc_op_args;

sys_mac_add sys_mac;

static unsigned long long bm_ax7_cpld_base_addr64[AX7_PRODUCT_SLOT_NUM] = {
	0x800000001D010000,	//for CRSMU cpld or Box product 
	0x800000001D040000,	//for sub slot 1
	0x800000001D050000,	//for sub slot 2
	0x800000001D060000,	//for sub slot 3
	0x800000001D070000	//for sub slot 4
};

/** 
  * data from chassis device.
  */
struct ax_sysinfo_product_t       bm_ax_sysinfo_on_backplane;
struct ax_sysinfo_single_board_t  bm_ax_sysinfo_on_module0;

extern mac_addr_stored_t stored_mac;
/**
  * data from box device.
  */
struct ax_sysinfo_product_t       bm_ax_sysinfo_on_mainboard;

static int do_read_wide(uint64_t *result,uint64_t addr,unsigned int num_bits) {
	
	switch (num_bits){
		case 8:
			*result = (uint64_t) cvmx_read64_uint8(addr);
			break;
		case 16:
			*result = (uint64_t) cvmx_read64_uint16(addr);
			break;
		case 32:
			*result = (uint64_t) cvmx_read64_uint32(addr);
			break;
		case 64:
			*result = (uint64_t) cvmx_read64_uint64(addr);
			break;
		default:
			printk (KERN_ALERT DRIVER_NAME ":read wide [%d] wrong.\n",num_bits);
			return -1;
			break;
	}
	return 0;
}
	
static int do_write_wide(void *val,uint64_t addr,unsigned int num_bits) {

	uint8_t v8;
	uint16_t v16;
	uint32_t v32;
	unsigned long long v64;
	v64 = *(uint64_t *)val;

	v8 = (uint8_t )v64;
	v16 = (uint16_t)v64;
	v32 = (uint32_t)v64;

	switch (num_bits){
		case 8:
			DBG(debug_ioctl,"64bits val[0x%016llx] [%d]bits val                [0x%02x].\n",v64,num_bits,v8);
			cvmx_write64_uint8(addr,v8);
			break;
		case 16:
			DBG(debug_ioctl,"64bits val[0x%016llx] [%d]bits val              [0x%04x].\n",v64,num_bits,v16);
			cvmx_write64_uint16(addr,v16);
			break;
		case 32:
			DBG(debug_ioctl,"64bits val[0x%016llx] [%d]bits val          [0x%08x].\n",v64,num_bits,v32);
			cvmx_write64_uint32(addr,v32);
			break;
		case 64:
			DBG(debug_ioctl,"64bits val[0x%016llx] [%d]bits val  [0x%016llx].\n",v64,num_bits,v64);
			cvmx_write64_uint64(addr,v64);
			break;
		default:
			printk (KERN_ALERT DRIVER_NAME ":write wide [%d] wrong.\n",num_bits);
			return -1;
			break;
	}
	return 0;
}

static unsigned long long bm_get_cpld_base_addr64(unsigned char slot)
{
	//check if slot number illegal
	#if 0
	if(board_info.board_type == BOARD_TYPE_AX71_2X12G12S)
	    return 0x800000001D010000;
	else
	{
		if(slot > AX7_PRODUCT_SLOT_NUM)
			return ~(0ULL);
		else
			return (bm_ax7_cpld_base_addr64[slot]);
	}
	#endif
	unsigned long long addr64 = 0x800000001D010000;
	if(slot > AX7_PRODUCT_SLOT_NUM)
			return ~(0ULL);
	else if(cvmx_read64_uint8((addr64+AX71_2X12G12S_TYPE_CPLD_ADDR)) == AX71_2X12G12S_TYPE_CPLD_DATA)
		return bm_ax7_cpld_base_addr64[0];
	else
		return (bm_ax7_cpld_base_addr64[slot]);
	/* code optmize: Overrunning array "bm_ax7_cpld_base_addr64" of 5 8-byte elements at element index 5 (byte offset 40) using index "slot" 
	 [slot -1] is not ok, wangchao@autelan.com 2013-01-17 */
	
}

int do_cpld_reg_read(bm_op_cpld_args *cpld_read_args)
{
	unsigned long long addr64;
	unsigned char value = 0;

	addr64 = bm_get_cpld_base_addr64(cpld_read_args->slot);

	if((~0ULL) == addr64) {
		DBG(debug_ioctl,"read cpld got base address error!\n");
	}
	else{
		addr64 += cpld_read_args->regAddr;
		value = cvmx_read64_uint8(addr64);
		cpld_read_args->regData = value;
	}
	
	//DBG(debug_ioctl,"read %#016llx got %#0x\n",addr64,value);
	return 0;
}

//add by zhang
static int do_cpld_reg_write(bm_op_cpld_args* cpld_write_args)
{
	unsigned long long addr64;
	unsigned char value = 0;
	int ret =0;
	addr64 = bm_get_cpld_base_addr64(cpld_write_args->slot);

	if((~0ULL) == addr64) {
		DBG(debug_ioctl,"write cpld on slot %d got base address error!\n",cpld_write_args->slot);
		ret = 1;
	}
	else{
		addr64 += cpld_write_args->regAddr;
		
		/*DBG(debug_ioctl,"write cpld on slot %d reg %x data %x",cpld_write_args->slot,	
			cpld_write_args->regAddr,cpld_write_args->regData);*/
		cvmx_write64_uint8(addr64,cpld_write_args->regData);
		value = cvmx_read64_uint8(addr64);
		
		//DBG(debug_ioctl,"read back %x\n",value);
	}	
	return ret;
}
int do_config_load_firmware_QT2025(void)
{
    int ret;
	
    ret = phy_QT2025();
	
    if(ret == 1)
    {
        return 1;
	}        
    else
    {
        return 0;
	}
}

int do_port_admin_status_powerup(void)
{
	int ret;
    ret = cvm_phy_enable();
	if(ret == 1)
	    return 1;
	else
	    return 0;
}

int do_port_admin_status_powerdown(void)
{   
	int ret;
    ret = cvm_phy_disable();
	if(ret == 1)
	    return 1;
	else
	    return 0;
}

static int do_bit64_reg_read(bm_op_rbit64_args *gpio_read_args)
{
/*	unsigned long long addr64;*/
	unsigned long long value = 0;
	value = cvmx_read64_uint64(gpio_read_args->regAddr);
	gpio_read_args->regData = value;
	return 0;
}
/* To remove the warnning*********
static int do_mainboard_sysinfo_read(bm_op_mainboard_sysinfo *sysinfo)
{
	if(NULL == sysinfo)
		return -1;
	// read base mac address from boot info
	memcpy(sysinfo->mac_addr,octeon_bootinfo->mac_addr_base, MAC_ADDRESS_LEN);
	DBG(debug_ioctl,"main board sysinfo base mac %02x:%02x:%02x:%02x:%02x:%02x\n",	\
			sysinfo->mac_addr[0],sysinfo->mac_addr[1],sysinfo->mac_addr[2],sysinfo->mac_addr[3],	\
			sysinfo->mac_addr[4],sysinfo->mac_addr[5]);

	// read board SN from bootinfo
	memcpy(sysinfo->sn,octeon_bootinfo->board_serial_number,SYSINFO_SN_LENGTH);
	DBG(debug_ioctl,"main board sysinfo SN %s\n",sysinfo->sn);

	// TODO: other sysinfo fields should append here!!!

	return 0;
}
*/
static int bm_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int retval = 0;
	int op_ret = 0;
	
	int retval_pt = -1;	
	bm_op_cpld_args cpld_read_args = {0};
	bm_op_rbit64_args gpio_read_args = {0};
	bm_op_mainboard_sysinfo mainboard_sysinfo;
	bm_op_sysinfo_eeprom_args eeprom_read_args;
	bm_op_read_eth_port_stats_args eth_port_stats_read_args;
	boot_env_t bm_op_boot_env_args;
	ax_product_sysinfo product_sysinfo;
	ax_module_sysinfo module_sysinfo;
	soft_i2c_read_write_16bit read_from_user_16bit;
	soft_i2c_read_write_8bit read_from_user_8bit;
	
	ax_soft_sysinfo sub_card_sysinfo;
	/*gxd*/
	bootrom_file bootrom;

	sys_product_type_t product_type;
	sys_power_state_t power_state;
	/*for FPGA burning bin file*/
	fpga_file bm_ioc_fpga_file;
	readphy_QT RD_QT;
	
	if (_IOC_TYPE(cmd) != BM_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > BM_IOC_MAXNR) return -ENOTTY;

	memset(&bm_ioc_op_args,0,sizeof(bm_op_args));
	memset(&product_sysinfo, 0, sizeof(product_sysinfo));
	memset(&module_sysinfo, 0, sizeof(ax_module_sysinfo));
	memset(&mainboard_sysinfo, 0,sizeof(bm_op_mainboard_sysinfo));
	memset(&eeprom_read_args, 0, sizeof(bm_op_sysinfo_eeprom_args));
	memset(&read_from_user_16bit, 0, sizeof(soft_i2c_read_write_16bit));
	memset(&read_from_user_8bit, 0, sizeof(soft_i2c_read_write_8bit));
	memset(&product_type, 0, sizeof(sys_product_type_t));
	memset(&power_state, 0, sizeof(sys_power_state_t));
	
	switch (cmd) {
		//read two bytes from eeprom through 16bit internal address
		case BM_IOC_SOFT_I2C_READ16_:
			copy_from_user(&read_from_user_16bit, (soft_i2c_read_write_16bit *)arg, sizeof(soft_i2c_read_write_16bit));
			printk("%x %x %x \n", read_from_user_16bit.slot_num, read_from_user_16bit.slave_addr, read_from_user_16bit.internal_addr);
			ax_soft_i2c_read16_eeprom(read_from_user_16bit.slot_num, read_from_user_16bit.slave_addr, 
				 			read_from_user_16bit.internal_addr, read_from_user_16bit.data, read_from_user_16bit.buf_len);
			
			copy_to_user((soft_i2c_read_write_16bit *)arg, &read_from_user_16bit, sizeof(soft_i2c_read_write_16bit));
			break;
			
		//write two bytes to eeprom through 16bit internal address
		case BM_IOC_SOFT_I2C_WRITE16_:
			copy_from_user(&read_from_user_16bit, (soft_i2c_read_write_16bit *)arg, sizeof(soft_i2c_read_write_16bit));
			
			printk("slot%x slave%x internal%x buf%x\n", read_from_user_16bit.slot_num, read_from_user_16bit.slave_addr, read_from_user_16bit.internal_addr, read_from_user_16bit.buf_len);
			ax_soft_i2c_write16_eeprom(read_from_user_16bit.slot_num, read_from_user_16bit.slave_addr, 
								read_from_user_16bit.internal_addr, read_from_user_16bit.data, read_from_user_16bit.buf_len);
			break;	

		case BM_IOC_SOFT_I2C_READ8_:
			copy_from_user(&read_from_user_8bit, (soft_i2c_read_write_8bit *)arg, sizeof(soft_i2c_read_write_8bit));
			read_from_user_8bit.port_num = 7 - read_from_user_8bit.port_num;
			ax_soft_i2c_read8_8bit(read_from_user_8bit.slot_num, read_from_user_8bit.slave_addr, read_from_user_8bit.internal_addr,
								read_from_user_8bit.data, read_from_user_8bit.port_num, read_from_user_8bit.buf_len);
			copy_to_user((soft_i2c_read_write_8bit *)arg, &read_from_user_8bit, sizeof(soft_i2c_read_write_8bit));		
			break;
		case BM_IOC_SOFT_I2C_WRITE8_:
			read_from_user_8bit.port_num = 7 - read_from_user_8bit.port_num;
			copy_from_user(&read_from_user_8bit, (soft_i2c_read_write_8bit *)arg, sizeof(soft_i2c_read_write_8bit));
			ax_soft_i2c_write8_8bit(read_from_user_8bit.slot_num, read_from_user_8bit.slave_addr, 
								read_from_user_8bit.internal_addr, read_from_user_8bit.data,
								 read_from_user_8bit.port_num, read_from_user_8bit.buf_len);
			break;
		case BM_IOC_G_:
			op_ret = copy_from_user(&bm_ioc_op_args,(bm_op_args *)arg,sizeof(bm_op_args));
			
			do_read_wide((uint64_t *)&(bm_ioc_op_args.op_value),bm_ioc_op_args.op_addr,bm_ioc_op_args.op_len);
			//DBG(debug_ioctl,"Read  addr[0x%016llx] [%d]bits got  [0x%016llx].\n",bm_ioc_op_args.op_addr, bm_ioc_op_args.op_len,bm_ioc_op_args.op_value);
			
			op_ret = copy_to_user((bm_op_args *)arg,&bm_ioc_op_args,sizeof(bm_op_args));
			
			break;
		case BM_IOC_X_:
			op_ret = copy_from_user(&bm_ioc_op_args,(bm_op_args *)arg,sizeof(bm_op_args));
			
			//DBG(debug_ioctl,"Write addr[0x%016llx] [%d]bits with [0x%016llx].\n",bm_ioc_op_args.op_addr, bm_ioc_op_args.op_len,bm_ioc_op_args.op_value);
			do_write_wide((void *)&(bm_ioc_op_args.op_value),bm_ioc_op_args.op_addr,bm_ioc_op_args.op_len);
			mdelay(10);	
			do_read_wide((uint64_t *)&(bm_ioc_op_args.op_value),bm_ioc_op_args.op_addr,bm_ioc_op_args.op_len);
			//DBG(debug_ioctl,"R & W  addr[0x%016llx] [%d]bits got  [0x%016llx].\n",bm_ioc_op_args.op_addr, bm_ioc_op_args.op_len,bm_ioc_op_args.op_value);
			
			op_ret = copy_to_user((bm_op_args *)arg,&bm_ioc_op_args,sizeof(bm_op_args));
			break;
		case BM_IOC_FPGA_W:
			op_ret = copy_from_user(&bm_ioc_op_args,(bm_op_args *)arg,sizeof(bm_op_args));
			
			//DBG(debug_ioctl,"Write addr[0x%016llx] [%d]bits with [0x%016llx].\n",bm_ioc_op_args.op_addr, bm_ioc_op_args.op_len,bm_ioc_op_args.op_value);
			do_write_wide((void *)&(bm_ioc_op_args.op_value),bm_ioc_op_args.op_addr,bm_ioc_op_args.op_len);
			//mdelay(10);
			do_read_wide((uint64_t *)&(bm_ioc_op_args.op_value),bm_ioc_op_args.op_addr,bm_ioc_op_args.op_len);
			//DBG(debug_ioctl,"R & W  addr[0x%016llx] [%d]bits got  [0x%016llx].\n",bm_ioc_op_args.op_addr, bm_ioc_op_args.op_len,bm_ioc_op_args.op_value);
			
			op_ret = copy_to_user((bm_op_args *)arg,&bm_ioc_op_args,sizeof(bm_op_args));
			break;
		case BM_IOC_CPLD_READ:
			op_ret = copy_from_user(&cpld_read_args,(bm_op_cpld_args *)arg,sizeof(bm_op_cpld_args));
			do_cpld_reg_read(&cpld_read_args);
			op_ret = copy_to_user((bm_op_cpld_args *)arg,&cpld_read_args,sizeof(bm_op_cpld_args));//for test.
			break;
		case BM_IOC_CPLD_WRITE://add by zhang
			op_ret = copy_from_user(&cpld_read_args,(bm_op_cpld_args *)arg,sizeof(bm_op_cpld_args));
			do_cpld_reg_write(&cpld_read_args);
			op_ret = copy_to_user((bm_op_cpld_args *)arg,&cpld_read_args,sizeof(bm_op_cpld_args));
			break;
		case BM_IOC_LOAD_QT2025_FIRMWARE:
		    op_ret = copy_from_user(&retval_pt, (int *)arg, sizeof(int));
            /*1 config smi0 to clause 45;2 load firmware to qt2025;3 checksum is correct*/
		    retval_pt = do_config_load_firmware_QT2025();
		    op_ret = copy_to_user((int *)arg, &retval_pt, sizeof(int));
		    break;
		case BM_IOC_READ_QT2025:
			op_ret = copy_from_user(&RD_QT, (readphy_QT *)arg, sizeof(readphy_QT));
		    RD_QT.val = cvm_read_phy_QT(RD_QT.regaddr,RD_QT.location);
		    copy_to_user((readphy_QT *)arg, &RD_QT, sizeof(readphy_QT));
		    break;	
		case BM_IOC_PORT_admin_status:
			op_ret = copy_from_user(&retval_pt, (int *)arg, sizeof(int));
			if(retval_pt ==1)
			{
                do_port_admin_status_powerup();
			}
			else
			{
                do_port_admin_status_powerdown();
			}			
		    op_ret = copy_to_user((int *)arg, &retval_pt, sizeof(int));
		    break;
		case BM_IOC_BIT64_REG_STATE:
			op_ret = copy_from_user(&gpio_read_args,(bm_op_rbit64_args *)arg,sizeof(bm_op_rbit64_args));
			//DBG(debug_ioctl,"read reg[%#016llx].\n",gpio_read_args.regAddr);
			do_bit64_reg_read(&gpio_read_args);
			//DBG(debug_ioctl,"reg[%#016llx] got data [%#016llx],\n",gpio_read_args.regAddr,gpio_read_args.regData);
			op_ret = copy_to_user((bm_op_rbit64_args *)arg,&gpio_read_args,sizeof(bm_op_rbit64_args));
			break;
			/*
		case BM_IOC_MAINBOARD_SYSINFO_READ:
			op_ret = copy_from_user(&mainboard_sysinfo, (ax_backplane_sysinfo*)arg, sizeof(ax_backplane_sysinfo));
			bm_ax_read_sysinfo_product(BM_AX_BACKPLANE_EEPROM_ADDR,&mainboard_sysinfo);
			op_ret = copy_to_user((ax_backplane_sysinfo *)arg,&mainboard_sysinfo,sizeof(ax_backplane_sysinfo));
			break;
		case BM_IOC_MODULE_SYSINFO_READ:
			op_ret = copy_from_user(&singleboard_sysinfo, (ax_sysinfo_single_board_t *)arg, sizeof(ax_sysinfo_single_board_t));
			retval = bm_ax_read_sysinfo_single_board(BM_EEPROM_MODULE0, &singleboard_sysinfo);
			op_ret = copy_to_user((ax_sysinfo_single_board_t *)arg, &singleboard_sysinfo, sizeof(ax_sysinfo_single_board_t));
			break;
			*/
		case BM_IOC_READ_PRODUCT_SYSINFO:
			op_ret = copy_from_user(&product_sysinfo, (ax_product_sysinfo *)arg, sizeof(ax_product_sysinfo));
			retval = bm_ax_read_sysinfo_product(BM_AX_BACKPLANE_EEPROM_ADDR, &product_sysinfo);
			op_ret = copy_to_user((ax_product_sysinfo *)arg, &product_sysinfo, sizeof(ax_product_sysinfo));
			break;
		case BM_IOC_READ_MODULE_SYSINFO:
			op_ret = copy_from_user(&module_sysinfo, (ax_module_sysinfo *)arg, sizeof(ax_module_sysinfo));
			retval = bm_ax_read_module_sysinfo(&module_sysinfo);
			op_ret = copy_to_user((ax_module_sysinfo *)arg, &module_sysinfo, sizeof(ax_module_sysinfo));
			break;
		/*
		case BM_IOC_SYSINFO_EEPROM_READ:
			op_ret = copy_from_user(&eeprom_read_args,	(bm_op_sysinfo_eeprom_args *)arg, sizeof(bm_op_sysinfo_eeprom_args));
			if (eeprom_read_args.eeprom_type == BM_EEPROM_BACKPLANE) {
				retval = bm_ax_read_sysinfo_product(BM_AX_BACKPLANE_EEPROM_ADDR, &bm_ax_sysinfo_on_backplane);
			} else if (eeprom_read_args.eeprom_type == BM_EEPROM_MODULE0) {
				retval = bm_ax_read_sysinfo_single_board(BM_AX_MODULE0_EEPROM_ADDR, &bm_ax_sysinfo_on_module0);
			} else if (eeprom_read_args.eeprom_type == BM_EEPROM_MAINBOARD) {
				retval = bm_ax_read_sysinfo_product(BM_AX_MAINBOARD_EEPROM_ADDR, &bm_ax_sysinfo_on_mainboard);
			}
			break;
		*/
		case BM_IOC_SOFT_SYSINFO_READ_16BIT:
			op_ret = copy_from_user(&sub_card_sysinfo, (ax_soft_sysinfo *)arg, sizeof(ax_soft_sysinfo));
 			soft_read_sysinfo_from_subcard(&sub_card_sysinfo);
			op_ret = copy_to_user((ax_soft_sysinfo *)arg, &sub_card_sysinfo, sizeof(ax_soft_sysinfo));
			break;	
		case BM_IOC_BOARD_ETH_PORT_STATS_READ:
			op_ret = copy_from_user(&eth_port_stats_read_args,	(bm_op_read_eth_port_stats_args *)arg, sizeof(bm_op_read_eth_port_stats_args));
			retval = do_read_board_eth_port_stats(eth_port_stats_read_args.portNum, \
													eth_port_stats_read_args.clear, \
													&eth_port_stats_read_args.port_counter,\
													&eth_port_stats_read_args.portOctCount);
			op_ret = copy_to_user((bm_op_read_eth_port_stats_args *)arg, &eth_port_stats_read_args, sizeof(bm_op_read_eth_port_stats_args));			
			break;
		case BM_IOC_ENV_EXCH:
			op_ret = copy_from_user(&bm_op_boot_env_args,(boot_env_t *)arg, sizeof(boot_env_t));
			retval = do_get_or_save_boot_env(bm_op_boot_env_args.name,bm_op_boot_env_args.value,bm_op_boot_env_args.operation);
			op_ret = copy_to_user((boot_env_t*)arg,&bm_op_boot_env_args,sizeof(boot_env_t));
			break;

	/*
		Get MAC addr.
	*/
		case BM_IOC_GET_MAC:
			op_ret = copy_from_user(&sys_mac,(sys_mac_add *)arg,sizeof(sys_mac_add));
			retval = bmk_get_sys_mac(&sys_mac);
			op_ret = copy_to_user((sys_mac_add *)arg,&sys_mac,sizeof(sys_mac_add));
			break;
		/*gxd*/
		case BM_IOC_BOOTROM_EXCH:
			op_ret = copy_from_user(&bootrom,(bootrom_file*)arg,sizeof(bootrom_file));
			retval = do_update_bootrom(&bootrom);
			break;
	/*
	  *get product type
	  */
	  	case BM_IOC_GET_PRODUCET_TYPE:
			op_ret = copy_from_user(&product_type, (sys_product_type_t *)arg, sizeof(sys_product_type_t));
			retval = do_get_product_type(&product_type);
			op_ret = copy_to_user((sys_product_type_t *)arg, &product_type, sizeof(sys_product_type_t));
			break;
        case BM_GET_POWER_STATE:
			op_ret = copy_from_user(&power_state, (sys_power_state_t *)arg, sizeof(sys_power_state_t));      
			retval = do_get_power_state(&power_state);
			op_ret = copy_to_user((sys_power_state_t *)arg, &power_state, sizeof(sys_power_state_t));
			break;			
	    case BM_IOC_FPGA_WRITE:
			printk(KERN_INFO"LOAD FPGA:BM_IOC_FPGA_WRITE.\n");
			op_ret = copy_from_user((&bm_ioc_fpga_file),(fpga_file *)arg,sizeof(fpga_file));
			retval = read_bin_to_fpga((char *)(bm_ioc_fpga_file.fpga_bin_name));
			bm_ioc_fpga_file.result = retval;
			op_ret = copy_to_user((fpga_file *)arg,&bm_ioc_fpga_file,sizeof(fpga_file));
			break;	
		default:
			retval = -1;
			break;
	}
	
	return retval;
}

static long bm_compat_ioctl(struct file *filp,unsigned int cmd, unsigned long arg) {
    int rval;

    //DBG(debug_ioctl,"COMPAT IOCTL cmd is [%x] IOC Type is [%d].\n",cmd, _IOC_TYPE(cmd));
    lock_kernel();
    rval = bm_ioctl(filp->f_dentry->d_inode, filp, cmd, arg);
    unlock_kernel();

    return (rval == -EINVAL) ? -ENOIOCTLCMD : rval;

}

static int bm_open(struct inode *inode, struct file *filp)
{
	struct bm_dev_s *dev;

	dev = container_of(inode->i_cdev, struct bm_dev_s, cdev);
	filp->private_data = dev;
	dev->size = 0;
	return 0;
}

static int bm_release(struct inode *inode, struct file *file)
{
	return 0;
}

static struct file_operations bm_fops = 
{
	.owner	= THIS_MODULE,
	//.llseek	= bm_llseek,
	//.read 	= bm_read,
	//.write	= bm_write,
	.ioctl	= bm_ioctl,
  	.compat_ioctl = bm_compat_ioctl,
  	.open	= bm_open,
	.release= bm_release

};


/**
* BM proc file system function.
*
*
*/
//////////////////////////////////////////////////////////////////////////////////////
static int bm_proc_module_sn_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_module_serial_no);
	
	return 0;
}

static int bm_proc_module_sn_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_module_sn_show, NULL);
}

static struct file_operations bm_proc_module_sn = {
    .open    = bm_proc_module_sn_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
//////////////////////////////////////////////////////////////////////////////////////
static int bm_proc_module_name_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_module_name);

	return 0;
}

static int bm_proc_module_name_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_module_name_show, NULL);
}

static struct file_operations bm_proc_module_name = {
    .open    = bm_proc_module_name_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
//////////////////////////////////////////////////////////////////////////////////////

static int bm_proc_product_sn_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_product_serial_no);

	return 0;
}

static int bm_proc_product_sn_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_product_sn_show, NULL);
}

static struct file_operations bm_proc_product_sn = {
    .open    = bm_proc_product_sn_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
//////////////////////////////////////////////////////////////////////////////////////
static int bm_proc_product_base_mac_addr_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_product_base_mac_address);

	return 0;
}

static int bm_proc_product_base_mac_addr_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_product_base_mac_addr_show, NULL);
}

static struct file_operations bm_proc_product_base_mac_addr = {
    .open    = bm_proc_product_base_mac_addr_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
//////////////////////////////////////////////////////////////////////////////////////
static int bm_proc_product_name_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_product_name);

	return 0;
}

static int bm_proc_product_name_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_product_name_show, NULL);
}

static struct file_operations bm_proc_product_name = {
    .open    = bm_proc_product_name_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
//////////////////////////////////////////////////////////////////////////////////////
static int bm_proc_software_name_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_software_name);

	return 0;
}

static int bm_proc_software_name_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_software_name_show, NULL);
}

static struct file_operations bm_proc_software_name = {
    .open    = bm_proc_software_name_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
//////////////////////////////////////////////////////////////////////////////////////
static int bm_proc_enterprise_name_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_enterprise_name);

	return 0;
}

static int bm_proc_enterprise_name_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_enterprise_name_show, NULL);
}

static struct file_operations bm_proc_enterprise_name = {
    .open    = bm_proc_enterprise_name_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
//////////////////////////////////////////////////////////////////////////////////////
static int bm_proc_enterprise_snmp_oid_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_enterprise_snmp_oid);

	return 0;
}

static int bm_proc_enterprise_snmp_oid_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_enterprise_snmp_oid_show, NULL);
}

static struct file_operations bm_proc_enterprise_snmp_oid = {
    .open    = bm_proc_enterprise_snmp_oid_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
//////////////////////////////////////////////////////////////////////////////////////
static int bm_proc_snmp_sys_oid_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_snmp_sys_oid);

	return 0;
}

static int bm_proc_snmp_sys_oid_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_snmp_sys_oid_show, NULL);
}

static struct file_operations bm_proc_snmp_sys_oid = {
    .open    = bm_proc_snmp_sys_oid_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
//////////////////////////////////////////////////////////////////////////////////////
static int bm_proc_admin_username_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_built_in_admin_username);

	return 0;
}

static int bm_proc_admin_username_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_admin_username_show, NULL);
}

static struct file_operations bm_proc_admin_username = {
    .open    = bm_proc_admin_username_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
//////////////////////////////////////////////////////////////////////////////////////
static int bm_proc_admin_passwd_show(struct seq_file *file, void *ptr)
{
	ax_sysinfo_product_t sysinfo;
	int rval = 0;
	
	memset(&sysinfo, 0, sizeof(sysinfo));
	rval = ax_read_sysinfo_from_eeprom_proc(BM_AX_BACKPLANE_EEPROM_ADDR, &sysinfo);

	if (rval < 0)
		seq_printf(file, "Read sysinfo error\n");
	else
		seq_printf(file, "%s\n", sysinfo.ax_sysinfo_built_in_admin_password);

	return 0;
}

static int bm_proc_admin_passwd_open(struct inode *inode, struct file *file)
{
    return single_open(file, bm_proc_admin_passwd_show, NULL);
}

static struct file_operations bm_proc_admin_passwd = {
    .open    = bm_proc_admin_passwd_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};


/* sysinfo dir entry in proc fs */
struct proc_dir_entry *bm_proc_dir_entry = NULL;

static int bm_proc_init(void)
{
	/* sysinfo element entry */
	struct proc_dir_entry *bm_proc_module_sn_entry = NULL;
	struct proc_dir_entry *bm_proc_module_name_entry = NULL;
	struct proc_dir_entry *bm_proc_product_sn_entry = NULL;
	struct proc_dir_entry *bm_proc_product_base_mac_addr_entry = NULL;
	struct proc_dir_entry *bm_proc_product_name_entry = NULL;
	struct proc_dir_entry *bm_proc_software_name_entry = NULL;
	struct proc_dir_entry *bm_proc_enterprise_name_entry = NULL;
	struct proc_dir_entry *bm_proc_enterprise_snmp_oid_entry = NULL;
	struct proc_dir_entry *bm_proc_snmp_sys_oid_entry = NULL;
	struct proc_dir_entry *bm_proc_admin_username_entry = NULL;
	struct proc_dir_entry *bm_proc_admin_passwd_entry = NULL;
	unsigned char dir_name[] = "sysinfo";
	
	bm_proc_dir_entry = proc_mkdir(dir_name, NULL);
	if (!bm_proc_dir_entry) {
		printk("bm: proc_mkdir failed.\n");
		return -1;
	}
	
	bm_proc_module_sn_entry = create_proc_entry("module_sn", 0, bm_proc_dir_entry);
	if (bm_proc_module_sn_entry) {
		bm_proc_module_sn_entry->proc_fops = &bm_proc_module_sn;
	} else {
		printk("bm: bm_proc_module_sn_entry initialize failed.\n");
		return -1;
	}

	bm_proc_module_name_entry = create_proc_entry("module_name", 0, bm_proc_dir_entry);
	if (bm_proc_module_name_entry) {
		bm_proc_module_name_entry->proc_fops = &bm_proc_module_name;
	} else {
		printk("bm: bm_proc_module_name_entry initialize failed.\n");
		return -1;
	}

	bm_proc_product_sn_entry = create_proc_entry("product_sn", 0, bm_proc_dir_entry);
	if (bm_proc_product_sn_entry) {
		bm_proc_product_sn_entry->proc_fops = &bm_proc_product_sn;
	} else {
		printk("bm: bm_proc_product_sn_entry initialize failed.\n");
		return -1;
	}
    
	bm_proc_product_base_mac_addr_entry = create_proc_entry("product_base_mac_addr", 0, bm_proc_dir_entry);
	if (bm_proc_product_base_mac_addr_entry) {
		bm_proc_product_base_mac_addr_entry->proc_fops = &bm_proc_product_base_mac_addr;
	} else {
		printk("bm: bm_proc_product_base_mac_addr_entry initialize failed.\n");
		return -1;
	}
  
	bm_proc_product_name_entry = create_proc_entry("product_name", 0, bm_proc_dir_entry);
	if (bm_proc_product_name_entry) {
		bm_proc_product_name_entry->proc_fops = &bm_proc_product_name;
	} else {
		printk("bm: bm_proc_product_name_entry initialize failed.\n");
		return -1;
	}
   
	bm_proc_software_name_entry = create_proc_entry("software_name", 0, bm_proc_dir_entry);
	if (bm_proc_software_name_entry) {
		bm_proc_software_name_entry->proc_fops = &bm_proc_software_name;
	} else {
		printk("bm: bm_proc_software_name_entry initialize failed.\n");
		return -1;
	}
  
	bm_proc_enterprise_name_entry = create_proc_entry("enterprise_name", 0, bm_proc_dir_entry);
	if (bm_proc_enterprise_name_entry) {
		bm_proc_enterprise_name_entry->proc_fops = &bm_proc_enterprise_name;
	} else {
		printk("bm: bm_proc_enterprise_name_entry initialize failed.\n");
		return -1;
	}
  
	bm_proc_enterprise_snmp_oid_entry = create_proc_entry("enterprise_snmp_oid", 0, bm_proc_dir_entry);
	if (bm_proc_enterprise_snmp_oid_entry) {
		bm_proc_enterprise_snmp_oid_entry->proc_fops = &bm_proc_enterprise_snmp_oid;
	} else {
		printk("bm: bm_proc_enterprise_snmp_oid_entry initialize failed.\n");
		return -1;
	}

	bm_proc_snmp_sys_oid_entry = create_proc_entry("snmp_sys_oid", 0, bm_proc_dir_entry);
	if (bm_proc_snmp_sys_oid_entry) {
		bm_proc_snmp_sys_oid_entry->proc_fops = &bm_proc_snmp_sys_oid;
	} else {
		printk("bm: bm_proc_snmp_sys_oid_entry initialize failed.\n");
		return -1;
	}

	bm_proc_admin_username_entry = create_proc_entry("admin_username", S_IRUSR, bm_proc_dir_entry);
	if (bm_proc_admin_username_entry) {
		bm_proc_admin_username_entry->proc_fops = &bm_proc_admin_username;
	} else {
		printk("bm: bm_proc_admin_username_entry initialize failed.\n");
		return -1;
	}

	bm_proc_admin_passwd_entry = create_proc_entry("admin_passwd", S_IRUSR, bm_proc_dir_entry);
	if (bm_proc_admin_passwd_entry) {
		bm_proc_admin_passwd_entry->proc_fops = &bm_proc_admin_passwd;
	} else {
		printk("bm: bm_proc_admin_passwd_entry initialize failed.\n");
		return -1;
	}

	return 0;
}


static int bm_proc_clearup(void)
{
	/* Remove entry */
	remove_proc_entry("module_sn", bm_proc_dir_entry);
	remove_proc_entry("module_name", bm_proc_dir_entry);
	remove_proc_entry("product_sn", bm_proc_dir_entry);
	remove_proc_entry("product_base_mac_addr", bm_proc_dir_entry);
	remove_proc_entry("product_name", bm_proc_dir_entry);
	remove_proc_entry("software_name", bm_proc_dir_entry);
	remove_proc_entry("enterprise_name", bm_proc_dir_entry);
	remove_proc_entry("enterprise_snmp_oid", bm_proc_dir_entry);
	remove_proc_entry("snmp_sys_oid", bm_proc_dir_entry);
	remove_proc_entry("admin_username", bm_proc_dir_entry);
	remove_proc_entry("admin_passwd", bm_proc_dir_entry);
	
	/* Remove dir entry */
	remove_proc_entry("sysinfo", NULL);
	bm_proc_dir_entry = NULL;
	
	return 0;
}

/*
* to get slot_id and is_distributed information from proc files 
* caojia, 20110812
*/
int proc_distributed_info_get(void)
{
/*	struct file *filp = NULL;*/
	/*mm_segment_t old_fs;
	int result;
	char temp[4];*/

	local_slot_num = cvmx_read64_uint8(0x1d010000 + 0x28 + (1ull<<63)) + 0x1;

	#if 0
	memset(temp, 0, 4);
	filp = filp_open("/proc/board/is_distributed", O_RDONLY, 0);
	if (!filp)
	{
		printk("open /proc/board/is_distributed fail!\n");
		return -1;
	}
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	result = filp->f_op->read(filp, (char *)temp, 1, &filp->f_pos);
	if (result < 0)
	{
		printk("read is_distributed fail!\n");
		return result;
	}
	set_fs(old_fs);
	filp_close(filp, NULL);
	is_distributed = temp[0] - '0';
	#endif

	is_distributed = 1;
	
	return 0;
}


static int bm_init(void)
{
//	static struct file_operations fops;
	int result;
	dev_t bm_devt;

//	result = alloc_chrdev_region(&bm_devt, 232,1,DRIVER_NAME);
//	in order to use the major number as majic number of ioctl, we use a fixed major num.
	bm_devt = MKDEV(BM_MAJOR_NUM,BM_MINOR_NUM);
	result = register_chrdev_region(bm_devt,1,DRIVER_NAME);
	printk(KERN_INFO DRIVER_NAME ":register major dev [%d] with debug_ioctl[%d]\n",MAJOR(bm_devt),debug_ioctl);

	if (result < 0)	{
		printk(KERN_WARNING DRIVER_NAME ":register_chrdev_region err[%d]\n",result);
		return 0;
	} 
	cdev_init(&(bm_dev.cdev),&bm_fops);
	bm_dev.cdev.owner = THIS_MODULE;
        result = cdev_add(&(bm_dev.cdev),bm_devt,1);
	if (result < 0)	{
		printk(KERN_WARNING DRIVER_NAME ":cdev_add err[%d]\n",result);
	} else {
		printk(KERN_INFO DRIVER_NAME ":loaded successfully.\n");
	}
	/* Init mac addr struct. */
	memset(&stored_mac,0,sizeof(mac_addr_stored_t));
	
	bm_proc_init();
//	get_mac_from_devinfo();
	get_flash_type(&info);

	result = proc_distributed_info_get();
	if (result < 0)
	{
		printk("get slot_id and is_distributed information failed!\n");
	}
	else
	{
		printk("bm : local_slot_num = %d\n", local_slot_num);
		printk("bm : is_distributed = %d\n", is_distributed);
	}
	
	return 0;
}

static void bm_cleanup(void)
{
	printk(KERN_INFO DRIVER_NAME ":Unregister MajorNum[%d]\n",MAJOR(bm_dev.cdev.dev));	
	unregister_chrdev_region(bm_dev.cdev.dev,1);	
	cdev_del(&(bm_dev.cdev));
	printk(KERN_INFO DRIVER_NAME ":unloaded\n");
	bm_proc_clearup();
	
	return;
}

module_init(bm_init);
module_exit(bm_cleanup);
module_param(debug_ioctl,uint,0644);
module_param(debug_bootbus, uint, 0444);
module_param(debug_gpio,uint, 0444);
module_param(debug_i2c,uint, 0444);
module_param(debug_rtc,uint, 0444);


MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("Autelan");
MODULE_AUTHOR("AuteWare Software Development Team");

