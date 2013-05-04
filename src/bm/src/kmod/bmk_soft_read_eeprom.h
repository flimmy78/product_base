/*
 * purpose: soft read sysinfo from subcard's EEPROM.
 * author: Autelan. Co. Ltd.
 * codeby: huxf
 * 2008-11-10
 */



#ifndef _BMK_SOFT_READ_EEPROM_
#define _BMK_SOFT_READ_EEPROM_

/** 
  * Product sysinfo, the data comes from sub-board.
  * codeby: huxf
  */
typedef struct ax_soft_sysinfo
{
	int slot_num;
	int slave_addr;
	char  ax_sysinfo_module_serial_no[21]; //data should be 20 bytes 
	char  ax_sysinfo_module_name[25];  //data max length should be 24 bytes 
}ax_soft_sysinfo;


/**
  * Read subcard sysinfo from eeprom.
  * @return: -1 for failure. 0 for sucess.
  */
int soft_read_sysinfo_from_subcard(ax_soft_sysinfo *sysinfo);

#endif
