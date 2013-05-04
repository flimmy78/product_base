/**
  * Soft read sysinfo 8bit.
  * author: Autelan Tech.
  * codeby: huxf
  * 2008-11-11
  */
#include <config/blk/dev/cow/common.h>
#include <linux/config.h>
#include <net/sctp/command.h>
#include <asm/delay.h>
#include <linux/init.h>  
#include <linux/string.h>
#include <linux/kernel.h>


#include "bmk_soft_read_sysinfo_8bit.h"
#include "ax_soft_i2c.h"

#define _BMK_SOFT_I2C_READ_SYSINFO_8BIT_

#ifdef _BMK_SOFT_I2C_READ_SYSINFO_8BIT_
    #define  ASI_DEBUG  printk
#else
    #define  ASI_DEBUG  
#endif

static read_element(unsigned char *src, unsigned char *dest)
{
	int element_length = 0;
	int i = 0;
	if (src == NULL || dest == NULL)
	{
		ASI_DEBUG("parm error %s\n", __FUNCTION__);
		return -1;
	}
	for (i=0; i<9; i++)
	{
		ASI_DEBUG("%x %c\n",src[i], src[i]);	
	}
	ASI_DEBUG("\n");
	element_length = (src[3] - '0') * 100 + (src[4] - '0') * 10 + (src[5] - '0');
	ASI_DEBUG("element is %d*****\n", element_length);
	for (i=0; i<element_length; i++)
	{
		dest[i] = src[6+i];
		ASI_DEBUG("%c", dest[i]);
	}
	ASI_DEBUG("\nelement is %d\n", element_length);
	return element_length;
}


static int analysis_sysinfo(ax_soft_read_sysinfo_8bit *sysinfo, unsigned char *all_sysinfo, int element_count)
{
	int i = 0;
	unsigned char *tmp = NULL;
	int len = 0;
	int offset = 0;
	tmp = all_sysinfo;
	if (sysinfo == NULL || all_sysinfo == NULL || element_count < 0)
	{
		printk("invalid parm %s\n", __FILE__);
		return -1;
	}
	offset = 10;
	for (i=0; i<element_count; i++)
	{
		//newline begin with '\r\n'
		if (all_sysinfo[offset] == '\r')
		{
			tmp = all_sysinfo + offset + 2;
			printk("%d end with \\r\\n\n", i);			
		}
		//newline begin with '\n'
		else
		{
			tmp = all_sysinfo + offset + 1;
			printk("%d end with \\n\n", i);
		}
		if (i == 0)
		{
			//printk("%x %x %x \n", '\r', '\n', *tmp);
			len = read_element(tmp, sysinfo->ax_sysinfo_module_serial_no);
		}
		else
		{
			len = read_element(tmp, sysinfo->ax_sysinfo_module_name);
		}
		offset = len + offset + 6 + 1;
	}
}


static int read_all_sysinfo(unsigned char *all_sysinfo, int sysinfo_total_length, ax_soft_read_sysinfo_8bit *sysinfo)
{
	if (all_sysinfo == NULL || sysinfo == NULL || sysinfo_total_length < 0)
	{
		ASI_DEBUG("parm error %s\n", __FUNCTION__);
		return -1;
	}
	if (ax_soft_i2c_set_addr8(sysinfo->slot_num, sysinfo->slave_addr, 0) != AX_SOFT_I2C_OK)
	{
		ASI_DEBUG("set addr failed %s\n", __FUNCTION__);
		return -1;
	}
	if (ax_soft_i2c_read_bytes_8bit(sysinfo->slot_num, sysinfo->slave_addr, (uint8_t *)all_sysinfo, sysinfo_total_length, sysinfo->port_num) != AX_SOFT_I2C_OK)
	{
		ASI_DEBUG("read sysinfo head failed\n");
		return -1;
	}
	return 0;
}


static int read_sysinfo_from_port_eeprom(ax_soft_read_sysinfo_8bit *sysinfo)
{	
	int element_count = 0, sysinfo_total_length = 0;
	int i;
	unsigned char *all_sysinfo = NULL;
	
	unsigned char *head = NULL;
	if (sysinfo == NULL)
	{
		ASI_DEBUG("parm error: %s\n", __FUNCTION__);
		return -1;
	}
	//malloc 10bytes , read the sysinfo head
	head = kmalloc(10 * sizeof(unsigned char), GFP_KERNEL);
	if (head == NULL)
	{
		ASI_DEBUG("kmalloc failed:  %s\n",__FUNCTION__);
		return -1;
	}
	if (ax_soft_i2c_set_addr8_8bit(sysinfo->slot_num, sysinfo->slave_addr, 0, sysinfo->port_num) != AX_SOFT_I2C_OK)
	{
		ASI_DEBUG("set addr failed\n");
		return -1;
	}
	if (ax_soft_i2c_read_bytes_8bit(sysinfo->slot_num, sysinfo->slave_addr, (uint8_t *)head, 10, sysinfo->port_num) != AX_SOFT_I2C_OK)
	{
		ASI_DEBUG("read sysinfo head failed\n");
		return -1;
	}
	for (i=0; i<10; i++)
	{
		printk("**%c \n", head[i]);
	}	
	for (i=0; i<10; i++)
	{
		if (head[i] < '0' || head[i] > '9')
		{
			break;
		}
	}
	if (i != 10)
	{
		ASI_DEBUG("bad head\n");
		return -1;
	}
	
	element_count = (head[2] - '0') * 100 + (head[3] - '0') * 10 + (head[4] - '0');
	sysinfo_total_length = (head[5] - '0') * 10000 + (head[6] - '0') * 1000 +
							(head[7] - '0') * 100 + (head[8] - '0') * 10 +
							(head[9] - '0');

	kfree(head);
	ASI_DEBUG("ele_count is %d total_length is %d\n", element_count, sysinfo_total_length);
	all_sysinfo= kmalloc(sysinfo_total_length * sizeof(unsigned char), GFP_KERNEL);
	if (all_sysinfo == NULL)
	{
		ASI_DEBUG("all_sysinfo kmalloc failed\n");
		return -1;
	}
	//read all sysinfo 
	read_all_sysinfo(all_sysinfo, sysinfo_total_length, sysinfo);
	for (i=0; i<sysinfo_total_length; i++)
	{
		ASI_DEBUG("%c", all_sysinfo[i]);
	}
	ASI_DEBUG("\n*****************\n");
	//analysis the readed sysinfo
	analysis_sysinfo(sysinfo, all_sysinfo, element_count);
	return 0;
}
int soft_read_sysinfo_8bit(ax_soft_read_sysinfo_8bit *sysinfo)
{
	if (sysinfo == NULL)
	{
		ASI_DEBUG("bad parm %s\n", __FUNCTION__);
		return -1;
	}
	if (sysinfo->slot_num < 1 || sysinfo->slot_num > 4)
	{
		ASI_DEBUG("wrong slot_num.It must between 1~4\n");
		return -1;
	}
	if (sysinfo->port_num < 1 || sysinfo->port_num > 6)
	{
		ASI_DEBUG("wrong port_num.It must between 1~6\n");
		return -1;
	}

	memset(sysinfo->ax_sysinfo_module_serial_no, 0x00, 20);
	memset(sysinfo->ax_sysinfo_module_name, 0x00, 24);
	
	read_sysinfo_from_port_eeprom(sysinfo);
}
