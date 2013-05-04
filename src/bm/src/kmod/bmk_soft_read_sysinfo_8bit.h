/*
 * purpose: soft read sysinfo from port's SYSINFO.
 * author: Autelan. Co. Ltd.
 * codeby: huxf
 * 2008-11-11
 */



#ifndef _BMK_SOFT_READ_SYSINFO_8BIT_
#define _BMK_SOFT_READ_SYSINFO_8BIT_

/** 
  * Product sysinfo, the data comes from port.
  * codeby: huxf
  */
typedef struct ax_soft_read_sysinfo_8bit
{
	int slot_num;
	int slave_addr;
	int port_num;
	char  ax_sysinfo_module_serial_no[20]; //data should be 20 bytes 
	char  ax_sysinfo_module_name[24];  //data max length should be 24 bytes 
}ax_soft_read_sysinfo_8bit;


/**
  * Read subcard sysinfo from eeprom.
  * @return: -1 for failure. 0 for sucess.
  */
int soft_read_sysinfo_8bit(ax_soft_read_sysinfo_8bit *sysinfo);

#endif

