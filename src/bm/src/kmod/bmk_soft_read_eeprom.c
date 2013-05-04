/**
  * Soft read sysinfo from subcard.
  * author: Autelan Tech.
  * codeby: huxf
  * 2008-11-10
  */
#include <net/sctp/command.h>
#include <asm/delay.h>
#include <linux/init.h>  
#include <linux/string.h>
#include <linux/kernel.h>


#include "bmk_soft_read_eeprom.h"
#include "ax_soft_i2c.h"

#define _BMK_SOFT_I2C_READ_SYSINFO_16BIT_

#ifdef _BMK_SOFT_I2C_READ_SYSINFO_16BIT_
    #define  ASI_DEBUG  printk
#else
    #define  ASI_DEBUG  
#endif


static int read_element(unsigned char *src, unsigned char *dest)
{
	int element_length = 0;
	int i = 0;
	if (src == NULL || dest == NULL)
	{
		printk("parm error %s\n", __FUNCTION__);
		return -1;
	}
	for (i=0; i<9; i++)
	{
		printk("%x %c\n",src[i], src[i]);	
	}
	printk("\n");
	element_length = (src[3] - '0') * 100 + (src[4] - '0') * 10 + (src[5] - '0');
	printk("element is %d*****\n", element_length);
	for (i=0; i<element_length; i++)
	{
		dest[i] = src[6+i];
		printk("%c", dest[i]);
	}
	printk("\nelement is %d\n", element_length);
	return element_length;
}
static int analysis_sysinfo(ax_soft_sysinfo *sysinfo, unsigned char *all_sysinfo, int element_count)
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
	return 0;
}

static int read_all_sysinfo(unsigned char *all_sysinfo, int sysinfo_total_length, ax_soft_sysinfo *sysinfo)
{
	if (all_sysinfo == NULL || sysinfo == NULL || sysinfo_total_length < 0)
	{
		printk("parm error %s\n", __FUNCTION__);
		return -1;
	}
	if (ax_soft_i2c_set_addr16(sysinfo->slot_num, sysinfo->slave_addr, 0) != AX_SOFT_I2C_OK)
	{
		printk("set addr failed %s\n", __FUNCTION__);
		return -1;
	}
	if (ax_soft_i2c_read_bytes(sysinfo->slot_num, sysinfo->slave_addr, (uint8_t *)all_sysinfo, sysinfo_total_length) != AX_SOFT_I2C_OK)
	{
		printk("read sysinfo head failed\n");
		return -1;
	}
	return 0;
}
static int soft_read_sysinfo_from_eeprom(ax_soft_sysinfo *sysinfo)
{
	int element_count = 0, sysinfo_total_length = 0;
	int i;
	unsigned char *all_sysinfo = NULL;
	
	unsigned char *head = NULL;
	if (sysinfo == NULL)
	{
		printk("parm error: %s\n", __FUNCTION__);
		return -1;
	}
	//malloc 10bytes , read the sysinfo head
	head = kmalloc(10 * sizeof(unsigned char), GFP_KERNEL);
	if (head == NULL)
	{
		printk("kmalloc failed:  %s\n",__FUNCTION__);
		return -1;
	}
	if (ax_soft_i2c_set_addr16(sysinfo->slot_num, sysinfo->slave_addr, 0) != AX_SOFT_I2C_OK)
	{
		printk("set addr failed\n");
	    kfree(head); /* code optmize: Resource leak (RESOURCE_LEAK)
		              wangchao@autelan.com 2013-01-17 */
		return -1;
	}
	//read head
	if (ax_soft_i2c_read_bytes(sysinfo->slot_num, sysinfo->slave_addr, (uint8_t *)head, 10) != AX_SOFT_I2C_OK)
	{
		printk("read sysinfo head failed\n");
		/* code optmize: Resource leak (RESOURCE_LEAK)
		   wangchao@autelan.com 2013-01-17 */
		kfree(head);   
		return -1;
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
		printk("bad head\n");
		kfree(head);
		return -1;
	}
	
	element_count = (head[2] - '0') * 100 + (head[3] - '0') * 10 + (head[4] - '0');
	sysinfo_total_length = (head[5] - '0') * 10000 + (head[6] - '0') * 1000 +
							(head[7] - '0') * 100 + (head[8] - '0') * 10 +
							(head[9] - '0');

	kfree(head);
	printk("ele_count is %d total_length is %d\n", element_count, sysinfo_total_length);
	all_sysinfo= kmalloc(sysinfo_total_length * sizeof(unsigned char), GFP_KERNEL);
	if (all_sysinfo == NULL)
	{
		printk("all_sysinfo kmalloc failed\n");
		return -1;
	}
	//read all sysinfo 
	read_all_sysinfo(all_sysinfo, sysinfo_total_length, sysinfo);
	for (i=0; i<sysinfo_total_length; i++)
	{
		printk("%c", all_sysinfo[i]);
	}
	printk("\n*****************\n");
	//analysis the readed sysinfo
	analysis_sysinfo(sysinfo, all_sysinfo, element_count);

	kfree(all_sysinfo);
	/* code optmize:resource leak
          wangchao@autelan.com 2013-01-17 */
	return 0;
}
/**
  * Read sysinfo from subcard eeprom.
  *@return: -1 for failure. 0 for success.
  */
int soft_read_sysinfo_from_subcard(ax_soft_sysinfo *sysinfo)
{
	if (sysinfo == NULL)
	{
		printk("parm error %s\n", __FUNCTION__);
		return -1;
	}
	if (sysinfo->slot_num < 1 || sysinfo->slot_num > 4)
	{
		printk("slot_num error slot_num = %x\n", sysinfo->slot_num);
		return -1;
	}
	
	//read sysifo from subcard's eeprom
	//memset(sysinfo, 0x00, sizeof(ax_soft_sysinfo));
	memset(sysinfo->ax_sysinfo_module_serial_no, 0x00, 20);
	memset(sysinfo->ax_sysinfo_module_name, 0x00, 24);
	printk("begain to read sub card sysinfo\n");
	printk("slot %x slave %x\n", sysinfo->slot_num, sysinfo->slave_addr);
	soft_read_sysinfo_from_eeprom(sysinfo);
	printk("subcard sysinfo read over\n");
	return 0;
}



