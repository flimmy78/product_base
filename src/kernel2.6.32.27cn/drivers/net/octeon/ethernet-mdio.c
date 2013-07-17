/**********************************************************************
 * Author: Cavium Networks
 *
 * Contact: support@caviumnetworks.com
 * This file is part of the OCTEON SDK
 *
 * Copyright (c) 2003-2007 Cavium Networks
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, Version 2, as
 * published by the Free Software Foundation.
 *
 * This file is distributed in the hope that it will be useful, but
 * AS-IS and WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE, TITLE, or
 * NONINFRINGEMENT.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this file; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * or visit http://www.gnu.org/licenses/.
 *
 * This file may also be available under a different license from Cavium.
 * Contact Cavium Networks for more information
**********************************************************************/
#include <linux/kernel.h>
#include <linux/ethtool.h>
#include <linux/phy.h>
#include <linux/of_mdio.h>
#include <linux/net_tstamp.h>
#include <linux/fs.h>
#include <net/dst.h>
#include <asm/processor.h>
#include <asm/uaccess.h>
#include <linux/err.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-helper.h>
#include <asm/octeon/cvmx-helper-board.h>
#include <asm/octeon/cvmx-srio.h>

#include <asm/octeon/cvmx-smix-defs.h>
#include <asm/octeon/cvmx-pko-defs.h>
#include <asm/octeon/cvmx-gmxx-defs.h>

#include <asm/octeon/cvmx-atomic.h>
#include <linux/autelan_product.h>

#include "ethernet-defines.h"
#include "octeon-ethernet.h"
#include "ethernet-mdio.h"
#include "ethernet-util.h"

#include <asm/octeon/cvmx-csr.h>
#include "ethernet-common.h"
extern product_info_t autelan_product_info;
struct timespec timestamp[TOTAL_NUMBER_OF_PORTS];
unsigned int up_times[TOTAL_NUMBER_OF_PORTS];
unsigned int down_times[TOTAL_NUMBER_OF_PORTS];

#define  SIZE_16K  0x4000
#define  SIZE_8K   0x2000
/*
*read the AX81-AC-1X12G12S XFP QT2025 register
*return the register val for success;-1 for fail
*/
int cvmx_mdio_x_read(int bus_id, int phy_id, int reg_addr, unsigned int location)
{
	cvmx_smix_cmd_t smi_x_cmd;
	cvmx_smix_rd_dat_t smi_x_rd;
	cvmx_smix_wr_dat_t smi_x_wr;
	int timeout = 1000;

	//step one set op address
	//write the address which will be operated to wr_dat reg.
	smi_x_wr.cn56xx.dat = location&0xffff;
	cvmx_write_csr(CVMX_SMIX_WR_DAT(bus_id), smi_x_wr.u64);
	//write cmd reg for sending the address to phy
	smi_x_cmd.u64 = 0;
	smi_x_cmd.cn56xx.phy_op = 0x00;
	smi_x_cmd.cn56xx.phy_adr = phy_id;
	smi_x_cmd.cn56xx.reg_adr = reg_addr;
	cvmx_write_csr(CVMX_SMIX_CMD(bus_id), smi_x_cmd.u64);

    do
    {
        cvmx_wait(1000);
        smi_x_wr.u64 = cvmx_read_csr(CVMX_SMIX_WR_DAT(bus_id));
    } while (smi_x_wr.s.pending && --timeout);

    if (timeout <= 0)
    {
        //printk(KERN_ERR "err in write address! timeout! cvmx_mdio_x_read");
        return -1;
    }

    if (smi_x_wr.s.val)
    {
        timeout = 1000;
	    //step two write read command 
	    smi_x_cmd.u64 = 0;
	    smi_x_cmd.cn56xx.phy_op = 0x3;
	    smi_x_cmd.cn56xx.phy_adr = phy_id;
	    smi_x_cmd.cn56xx.reg_adr = reg_addr;
	    cvmx_write_csr(CVMX_SMIX_CMD(bus_id), smi_x_cmd.u64);


	    //step three pending read rd_dat reg for the valiable data
	    do
        {
            cvmx_wait(1000);
            smi_x_rd.u64 = cvmx_read_csr(CVMX_SMIX_RD_DAT(bus_id));
        } while (smi_x_rd.s.pending && timeout--);

        if (timeout <= 0)
        {
            printk(KERN_ERR "err in write data! timeout! cvmx_mdio_x_read");
            return -1;
        }

        if (smi_x_rd.s.val)
        {
            return smi_x_rd.s.dat;
        }
        else
        {
            printk("\t\t it is wrong return -1! read data fail;\n");
            return -1;
        }
    }
    else
    {
        printk("\t\t it is wrong return -1! write address fail;\n");
        return -1;
    }
}


/**
  *comply with ieee802.3-2005 section 45
  *for xsmi write
  *huxuefeng 20111111
  *return 0 for success,-1 for fail 
  */

int cvmx_mdio_x_write(int bus_id, int phy_id, int reg_addr, unsigned int location, int value)
{   
	cvmx_smix_cmd_t smi_x_cmd;
	cvmx_smix_wr_dat_t smi_x_wr;
	int timeout = 1000;

	//step one set op address
	//write the address which will be operated to wr_dat reg.
	smi_x_wr.cn56xx.dat = location&0xffff;
	cvmx_write_csr(CVMX_SMIX_WR_DAT(bus_id), smi_x_wr.u64);
	//write cmd reg for sending the address to phy
	smi_x_cmd.u64 = 0;
	smi_x_cmd.cn56xx.phy_op = 0x00;
	smi_x_cmd.cn56xx.phy_adr = phy_id;
	smi_x_cmd.cn56xx.reg_adr = reg_addr;
	cvmx_write_csr(CVMX_SMIX_CMD(bus_id), smi_x_cmd.u64);

    do
    {
        cvmx_wait(1000);
        smi_x_wr.u64 = cvmx_read_csr(CVMX_SMIX_WR_DAT(bus_id));
    } while (smi_x_wr.s.pending && --timeout);

    if (timeout <= 0)
    {
        printk(KERN_ERR "err in write address! timeou! cvmx_mdio_x_write");
        return -1;
    }

    if (smi_x_wr.s.val)
    {
        timeout = 1000;
        
	    //step two write wr_dat reg
	    smi_x_wr.cn56xx.dat = value&0xffff;
	    cvmx_write_csr(CVMX_SMIX_WR_DAT(bus_id), smi_x_wr.u64);

	    //write cmd reg for writing data to the phy
	    smi_x_cmd.u64 = 0;
	    smi_x_cmd.cn56xx.phy_op = 0x1;
	    smi_x_cmd.cn56xx.phy_adr = phy_id;
	    smi_x_cmd.cn56xx.reg_adr = reg_addr;
	    cvmx_write_csr(CVMX_SMIX_CMD(bus_id), smi_x_cmd.u64);

	    //step three checking is write done
	    do
        {
            cvmx_wait(1000);
            smi_x_wr.u64 = cvmx_read_csr(CVMX_SMIX_WR_DAT(bus_id));
        } while (smi_x_wr.s.pending && --timeout);
        
        if (timeout <= 0)
        {
            printk(KERN_ERR "err in write data! timeout! cvmx_mdio_x_write");
            return -1;
        }
        
        if (smi_x_wr.s.val)
        {
            return 0;
        }

    }
    else
    {
        printk(KERN_ERR "write value fail!\n");
        return -1;
    }
    
    //--printk(KERN_ERR "come out cvmx_mdio_x_write!\n");
    return 0;
}

/*
*init the QT2025 for setup 1;
*return 0 for success;1 for fail
*/
int phy_QT2025_access_prepare(void)
{
    //SFP+ Limiting SR/LR/ER Applications
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xc300, 0x0000))
	{
		printk(KERN_ERR "write phy reg 0xc300 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xc302, 0x0004))
	{
		printk(KERN_ERR "write phy reg 0xc302 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xc319, 0x0008))
	{
		printk(KERN_ERR "write phy reg 0xc319 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xc31a, 0x0098))
	{
		printk(KERN_ERR "write phy reg 0xc31a failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 3, 0x0026, 0x0e00))
	{
		printk(KERN_ERR "write phy reg 0x0026 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 3, 0x0027, 0x0093))
	{
		printk(KERN_ERR "write phy reg 0x0027 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 3, 0x0028, 0xa528))
	{
		printk(KERN_ERR "write phy reg 0x0028 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 3, 0x0029, 0x0003))
	{
		printk(KERN_ERR "write phy reg 0x0029 failed\n");
		return 1;
	}
	
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xc300, 0x0002))
	{
		printk(KERN_ERR "write phy reg 0xc300 failed.\n");
		return 1;
	}	
	if (cvmx_mdio_x_write(0, 0x18, 3, 0xe854, 0x00c0))
	{
		printk(KERN_ERR "write phy reg 0xe854 failed\n");
		return 1;
	}
    /*control led*/
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xd006, 0x0009))
	{
		printk(KERN_ERR "write phy reg 0xe854 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xd007, 0x0002))
	{
		printk(KERN_ERR "write phy reg 0xe854 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xd008, 0x000a))
	{
		printk(KERN_ERR "write phy reg 0xe854 failed\n");
		return 1;
	}
	/*end*/

#if 0
    // SFP+ LRM Applications
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xc300, 0x0000))
	{
		printk(KERN_ERR "write phy reg 0xc300 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xc302, 0x0004))
	{
		printk(KERN_ERR "write phy reg 0xc302 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xc319, 0x0000))
	{
		printk(KERN_ERR "write phy reg 0xc319 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xc31a, 0x0098))
	{
		printk(KERN_ERR "write phy reg 0xc31a failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 3, 0x0026, 0x0e00))
	{
		printk(KERN_ERR "write phy reg 0x0026 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 3, 0x0027, 0x0093))
	{
		printk(KERN_ERR "write phy reg 0x0027 failed\n");
		return 1;
	}
	if (cvmx_mdio_x_write(0, 0x18, 3, 0x0028, 0xa528))
	{
		printk(KERN_ERR "write phy reg 0x0028 failed\n");
		return 1;
	}
	
	if (cvmx_mdio_x_write(0, 0x18, 1, 0xc300, 0x0002))
	{
		printk(KERN_ERR "write phy reg 0xc300 failed.\n");
		return 1;
	}	
	if (cvmx_mdio_x_write(0, 0x18, 3, 0xe854, 0x00c0))
	{
		printk(KERN_ERR "write phy reg 0xe854 failed\n");
		return 1;
	}
#endif
	return 0;
}

/*
*check the QT2025 load success;
*return 0 for success;1 for fail
*/
int phy_QT2025_config(void)
{
	int reg_val =0;
	int i=0,j=0;
    int num1,num2,num3,num4,num5;
	
	if (cvmx_mdio_x_write(0, 0x18, 3, 0xe854, 0x0040))
	{
		printk(KERN_ERR "write phy reg 0xe854 failed\n");
		return 1;
	}

	while(i<100)
	{
	   mdelay(4);
	   i++;
	}
	i=0;
//check the heartbeat
#if 0
    printk(KERN_ERR "begin to check the heartbeat!\n");
    printk(KERN_ERR "\t\t first time = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd7ee));
    mdelay(4);
    printk(KERN_ERR "\t\t second time = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd7ee));
    mdelay(4);
    printk(KERN_ERR "\t\t third time = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd7ee));
    mdelay(4);
    printk(KERN_ERR "\t\t fourth time = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd7ee));
    mdelay(4);
    printk(KERN_ERR "\t\t fivth time = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd7ee));
    mdelay(4);
    printk(KERN_ERR "\t\t sixth time = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd7ee));
    mdelay(4);
    printk(KERN_ERR "finish checking the heartbeat!\n");
#endif
    num1 = cvmx_mdio_x_read(0,0x18,3,0xd7ee);
    mdelay(4);
    num2 = cvmx_mdio_x_read(0,0x18,3,0xd7ee);
    mdelay(4);
    num3 = cvmx_mdio_x_read(0,0x18,3,0xd7ee);
    mdelay(4);
    num4 = cvmx_mdio_x_read(0,0x18,3,0xd7ee);
    mdelay(4);
    num5 = cvmx_mdio_x_read(0,0x18,3,0xd7ee);
	if((num1 != num2) && (num2 != num3) && (num3 != num4) &&(num4 != num5))
	{
            printk(KERN_ERR "Right:having the heartbeat!\n");
        	printk(KERN_ERR "\t\t num1:num2:num3:num4:num5 = %d:%d:%d:%d:%d\n",num1,num2,num3,num4,num5);
            printk(KERN_ERR "\t\t Checksum Register 3.D716 = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd716));
            printk(KERN_ERR "\t\t Checksum Register 3.D717 = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd717));	
       	
#if 0
        	//firmware checksum
        	//printk(KERN_ERR "\t\t Checksum Register 3.8a00 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8a00));
            //printk(KERN_ERR "\t\t Checksum Register 3.8a01 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8a01));
            //printk(KERN_ERR "\t\t Checksum Register 3.8a02 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8a02));
            //printk(KERN_ERR "\t\t Checksum Register 3.8a03 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8a03));
            //printk(KERN_ERR "\t\t Checksum Register 3.D718 = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd718));
            //printk(KERN_ERR "\t\t Checksum Register 3.D719 = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd719));
            printk(KERN_ERR "\t\t Checksum Register 3.D716 = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd716));
            printk(KERN_ERR "\t\t Checksum Register 3.D717 = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd717));	
            //printk(KERN_ERR "\t\t Checksum Register 3.D7FD = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd7fd));
#endif

#if 0
            // test LED
        	if (cvmx_mdio_x_write(0, 0x18, 3, 0xd70d, 0x39))
        	{
        		printk(KERN_ERR "write phy reg 0x39 failed\n");
        		return 1;
        	}
#endif
        	if ((reg_val = cvmx_mdio_x_read(0, 0x18, 3, 0xd7fd)) == -1)
        	{
        		printk(KERN_ERR "read 0xd7fd failed\n");
        		return 1;
        	}
        	if (reg_val == 0x00f0)
        	{
        		printk(KERN_ERR "check sum failed\n");
        		return 1;
        	}
        	
        	while(reg_val == 0x10 || reg_val == 0x0)
        	{
        		while(j<100)
        		{
                    mdelay(4);
        			j++;
        		}
        	    reg_val = cvmx_mdio_x_read(0, 0x18, 3, 0xd7fd);
        	    i++;
        	    if(i == 20)
        	    {
        	        printk(KERN_ERR "time out!\n");
                    break;
        	    }
                continue;
        	}
			
			i=0;
            while(i<1000)
        	{
        	   mdelay(4);
        	   i++;
        	}
        	
	        printk(KERN_ERR "\t\t Checksum Register 3.D7FD = %x\n", cvmx_mdio_x_read(0,0x18,3,0xd7fd));

#if 1
            //read the firmware version.
            printk(KERN_ERR "\t\t Firmware Version = %x\n", cvmx_mdio_x_read(0,0x18,3,0xD7F3));
            printk(KERN_ERR "\t\t Firmware Version = %x\n", cvmx_mdio_x_read(0,0x18,3,0xD7F4));
            printk(KERN_ERR "\t\t Firmware Version = %x\n", cvmx_mdio_x_read(0,0x18,3,0xD7F5));
#endif

        	return 0;
	}
	else
	{
        printk(KERN_ERR "Wrong:no heartbeat!\n");
		return 1;
	}
}

/*
*init the QT2025 ;
*return 0 for success;1 for fail
*/
int phy_QT2025_firmware_load(void)
{    
    int ret = -1;
	int i = 0;
    int val;
    struct file *fp = NULL;
    mm_segment_t fs;
    loff_t pos = 0;
    ssize_t num =0;
    
    unsigned int address_size = (SIZE_16K + SIZE_8K);
    char *buf = kmalloc((SIZE_16K + SIZE_8K),GFP_KERNEL);
    if(buf == NULL)
    {
        printk(KERN_ERR "kmalloc fail!");
        return 1;
    }
    memset(buf, 0, sizeof(char)*address_size);
	
    printk(KERN_ERR "init the firmware register,wait...");
    ret = phy_QT2025_access_prepare();
    if(0 != ret)
    {
        printk("phy_QT2225_access_prepare fail, ret=%d\n", ret);
        return 1;
    }
    printk(KERN_ERR "    OK!\n");
	
	printk(KERN_ERR "load the firmware bin file ,wait...");
    fp = filp_open("/opt/bin/QT2x25D_module_firmware_v2_0_3_0_MDIO.bin",O_RDONLY,0);
    if(IS_ERR(fp)){
        printk(KERN_ERR "open file error\n");
        return 1;
    }
    fs = get_fs();
    set_fs(KERNEL_DS);
    num = vfs_read(fp, buf,address_size,&pos);
    if(num < 0)
    {
        printk("vfs_read fail");
		return 1;
    }
    set_fs(fs);
    
    for (i=0; i<num; i++)
    {
        val = *(buf+i);
        if (i<SIZE_16K)
        {
            ret=cvmx_mdio_x_write(0, 0x18, 3, 0x8000+i, val); /* 3.8000 - 3.BFFF */
            if(ret != 0)
            {
                printk(KERN_ERR "firmware_byte_write failed at first 16k\n");
                return 1;
            }
        }
        else
        {
            ret=cvmx_mdio_x_write(0, 0x18, 4, 0x8000+(i-SIZE_16K), val); /* 4.8000 - 4.9FFF */
            if(ret != 0)
            {
                printk(KERN_ERR "firmware_byte_write failed at last 8k\n");
                return 1;
            }
        }
    }
    printk(KERN_ERR "    OK!\n");
    #if 0
	//test whether write into the register!
    printk(KERN_ERR "\t\t read register 3.8000 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8000));
    printk(KERN_ERR "\t\t read register 3.8001 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8001));
    printk(KERN_ERR "\t\t read register 3.8002 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8002));
    printk(KERN_ERR "\t\t read register 3.8003 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8003));
    printk(KERN_ERR "\t\t read register 3.8004 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8004));
    printk(KERN_ERR "\t\t read register 3.8005 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8005));
    printk(KERN_ERR "\t\t read register 3.8006 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8006));
    printk(KERN_ERR "\t\t read register 3.8007 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8007));
    printk(KERN_ERR "\t\t read register 3.8008 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8008));
    printk(KERN_ERR "\t\t read register 3.8009 = %x\n", cvmx_mdio_x_read(0,0x18,3,0x8009));
    printk(KERN_ERR "\t\t read register 3.800a = %x\n", cvmx_mdio_x_read(0,0x18,3,0x800a));
    printk(KERN_ERR "\t\t read register 3.800b = %x\n", cvmx_mdio_x_read(0,0x18,3,0x800b));
    printk(KERN_ERR "\t\t read register 3.800c = %x\n", cvmx_mdio_x_read(0,0x18,3,0x800c));
    printk(KERN_ERR "\t\t read register 3.800d = %x\n", cvmx_mdio_x_read(0,0x18,3,0x800d));
    printk(KERN_ERR "\t\t read register 3.8ffe = %x\n", cvmx_mdio_x_read(0,0x18,3,0x800e));
    #endif
	printk(KERN_ERR "Are testing load correctly,wait ...");
    ret = phy_QT2025_config();
    if(0 != ret)
    {
        printk("phy_QT2025_config fail, ret=%d\n", ret);
        return 1;
    }
    printk(KERN_ERR "testing already ,OK!\n");
	
    return 0;
}

EXPORT_SYMBOL(phy_QT2025_firmware_load);

/*
*load the QT2025 ;
*return 0 for success;1 for fail
*/
int phy_QT2025()
{
    int ret;
	
    printk(KERN_ERR "begin to load the phy QT2025 firmware,wait...\n");
    cvmx_write_csr(CVMX_SMI_EN, 1);
    cvmx_write_csr(CVMX_SMI_CLK, cvmx_read_csr(CVMX_SMI_CLK) | (1 << 24) | (1 << 12) | 0xff);

    #if 0
	//test read and write operation!
    printk("\t\t PMA/PMD device identifier1.2 = %x\n", cvmx_mdio_x_read(0,24,1,2));
    printk("\t\t PMA/PMD device identifier1.3 = %x\n", cvmx_mdio_x_read(0,24,1,3));
    cvmx_mdio_x_write(0,24,3,0x26,cvmx_mdio_x_read(0,24,3,0x26)|0xff);
    printk("\t\t read test 3.26 ff = %x\n", cvmx_mdio_x_read(0,24,3,0x26));
    cvmx_mdio_x_write(0,24,3,0x26,cvmx_mdio_x_read(0,24,3,0x26)&0xf0);
    printk("\t\t read test 3.26 f0 = %x\n", cvmx_mdio_x_read(0,24,3,0x26));
    #endif

	ret = phy_QT2025_firmware_load();
	if(ret == 0)
	{
        printk(KERN_ERR "loading the phy QT2025 firmware success\n");
        return 1; 
	}
	else
	{
        return 0;
	}
}
EXPORT_SYMBOL(phy_QT2025);

static void cvm_oct_get_drvinfo(struct net_device *dev,
				struct ethtool_drvinfo *info)
{
	strcpy(info->driver, "octeon-ethernet");
	strcpy(info->version, OCTEON_ETHERNET_VERSION);
	strcpy(info->bus_info, "Builtin");
}

static int cvm_oct_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (priv->phydev)
		return phy_ethtool_gset(priv->phydev, cmd);

	return -EINVAL;
}

static int cvm_oct_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (priv->phydev)
		return phy_ethtool_sset(priv->phydev, cmd);

	return -EINVAL;
}

static int cvm_oct_nway_reset(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	if (priv->phydev)
		return phy_start_aneg(priv->phydev);

	return -EINVAL;
}

const struct ethtool_ops cvm_oct_ethtool_ops = {
	.get_drvinfo = cvm_oct_get_drvinfo,
	.get_settings = cvm_oct_get_settings,
	.set_settings = cvm_oct_set_settings,
	.nway_reset = cvm_oct_nway_reset,
	.get_link = ethtool_op_get_link,
	.get_sg = ethtool_op_get_sg,
	.get_tx_csum = ethtool_op_get_tx_csum,
};

/**
 * cvm_oct_ioctl_hwtstamp - IOCTL support for timestamping
 * @dev:    Device to change
 * @rq:     the request
 * @cmd:    the command
 *
 * Returns Zero on success
 */
static int cvm_oct_ioctl_hwtstamp(struct net_device *dev,
	struct ifreq *rq, int cmd)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	struct hwtstamp_config config;
	union cvmx_mio_ptp_clock_cfg ptp;
	int have_hw_timestamps = 0;

	if (copy_from_user(&config, rq->ifr_data, sizeof(config)))
		return -EFAULT;

	if (config.flags) /* reserved for future extensions */
		return -EINVAL;

	/* Check the status of hardware for tiemstamps */
	if (OCTEON_IS_MODEL(OCTEON_CN6XXX)) {
		/* Write TX timestamp into word 4 */
		cvmx_write_csr(CVMX_PKO_REG_TIMESTAMP, 4);
		/* Get the current state of the PTP clock */
		ptp.u64 = octeon_read_ptp_csr(CVMX_MIO_PTP_CLOCK_CFG);
		if (!ptp.s.ext_clk_en) {
			/*
			 * The clock has not been configured to use an
			 * external source.  Program it to use the main clock
			 * reference.
			 */
			unsigned long long clock_comp = (NSEC_PER_SEC << 32) /
				octeon_get_io_clock_rate();
			if (!ptp.s.ptp_en)
				cvmx_write_csr(CVMX_MIO_PTP_CLOCK_COMP, clock_comp);
			pr_info("PTP Clock: Using sclk reference at %lld Hz\n",
				(NSEC_PER_SEC << 32) / clock_comp);
		} else {
			/* The clock is already programmed to use a GPIO */
			unsigned long long clock_comp = octeon_read_ptp_csr(
				CVMX_MIO_PTP_CLOCK_COMP);
			pr_info("PTP Clock: Using GPIO %d at %lld Hz\n",
				ptp.s.ext_clk_in,
				(NSEC_PER_SEC << 32) / clock_comp);
		}

		/* Enable the clock if it wasn't done already */
		if (!ptp.s.ptp_en) {
			ptp.s.ptp_en = 1;
			cvmx_write_csr(CVMX_MIO_PTP_CLOCK_CFG, ptp.u64);
		}
		have_hw_timestamps = 1;
		switch(priv->imode) {
		case CVMX_HELPER_INTERFACE_MODE_XAUI:
		case CVMX_HELPER_INTERFACE_MODE_RXAUI:
		case CVMX_HELPER_INTERFACE_MODE_SGMII:
			break;
		default:
			have_hw_timestamps = 0;
			break;
		}
	}

	/* Require hardware if ALLOW_TIMESTAMPS_WITHOUT_HARDWARE=0 */
	if (!ALLOW_TIMESTAMPS_WITHOUT_HARDWARE && !have_hw_timestamps)
		return -EINVAL;

	switch (config.tx_type) {
		case HWTSTAMP_TX_OFF:
			priv->tx_timestamp_sw = 0;
			priv->tx_timestamp_hw = 0;
			break;
		case HWTSTAMP_TX_ON:
			if (have_hw_timestamps)
				priv->tx_timestamp_hw = 1;
			else
				priv->tx_timestamp_sw = 1;
			break;
		default:
			return -ERANGE;
	}

	switch (config.rx_filter) {
		case HWTSTAMP_FILTER_NONE:
			priv->rx_timestamp_hw = 0;
			priv->rx_timestamp_sw = 0;
			if (have_hw_timestamps) {
				union cvmx_gmxx_rxx_frm_ctl gmxx_rxx_frm_ctl;
				union cvmx_pip_prt_cfgx pip_prt_cfgx;

				gmxx_rxx_frm_ctl.u64 = cvmx_read_csr(CVMX_GMXX_RXX_FRM_CTL(priv->interface_port, priv->interface));
				gmxx_rxx_frm_ctl.s.ptp_mode = 0;
				cvmx_write_csr(CVMX_GMXX_RXX_FRM_CTL(priv->interface_port, priv->interface), gmxx_rxx_frm_ctl.u64);

				pip_prt_cfgx.u64 = cvmx_read_csr(CVMX_PIP_PRT_CFGX(priv->ipd_pkind));
				pip_prt_cfgx.s.skip = 0;
				cvmx_write_csr(CVMX_PIP_PRT_CFGX(priv->ipd_pkind), pip_prt_cfgx.u64);
			}
			break;
		case HWTSTAMP_FILTER_ALL:
		case HWTSTAMP_FILTER_SOME:
		case HWTSTAMP_FILTER_PTP_V1_L4_EVENT:
		case HWTSTAMP_FILTER_PTP_V1_L4_SYNC:
		case HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ:
		case HWTSTAMP_FILTER_PTP_V2_L4_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_L4_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ:
		case HWTSTAMP_FILTER_PTP_V2_L2_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_L2_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ:
		case HWTSTAMP_FILTER_PTP_V2_EVENT:
		case HWTSTAMP_FILTER_PTP_V2_SYNC:
		case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
			if (have_hw_timestamps)
				priv->rx_timestamp_hw = 1;
			else
				priv->rx_timestamp_sw = 1;
			config.rx_filter = HWTSTAMP_FILTER_ALL;
			if (have_hw_timestamps) {
				union cvmx_gmxx_rxx_frm_ctl gmxx_rxx_frm_ctl;
				union cvmx_pip_prt_cfgx pip_prt_cfgx;

				gmxx_rxx_frm_ctl.u64 = cvmx_read_csr(CVMX_GMXX_RXX_FRM_CTL(priv->interface_port, priv->interface));
				gmxx_rxx_frm_ctl.s.ptp_mode = 1;
				cvmx_write_csr(CVMX_GMXX_RXX_FRM_CTL(priv->interface_port, priv->interface), gmxx_rxx_frm_ctl.u64);

				pip_prt_cfgx.u64 = cvmx_read_csr(CVMX_PIP_PRT_CFGX(priv->ipd_pkind));
				pip_prt_cfgx.s.skip = 8;
				cvmx_write_csr(CVMX_PIP_PRT_CFGX(priv->ipd_pkind), pip_prt_cfgx.u64);
			}
			break;
		default:
			return -ERANGE;
	}

	if (copy_to_user(rq->ifr_data, &config, sizeof(config)))
		return -EFAULT;

	return 0;
}

/**
 * cvm_oct_ioctl - IOCTL support for PHY control
 * @dev:    Device to change
 * @rq:     the request
 * @cmd:    the command
 *
 * Returns Zero on success
 */
int cvm_oct_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	cvmx_srio_tx_message_header_t tx_header;
	int ivalue;

	switch (cmd) {
	case CAVIUM_NET_IOCTL_SETPRIO:
		ivalue = rq->ifr_ifru.ifru_ivalue;
		if ((ivalue >= 0) && (ivalue < 4)) {
			tx_header.u64 = priv->srio_tx_header;
			tx_header.s.prio = ivalue;
			priv->srio_tx_header = tx_header.u64;
			return 0;
		}
		return -EINVAL;

	case CAVIUM_NET_IOCTL_GETPRIO:
		tx_header.u64 = priv->srio_tx_header;
		rq->ifr_ifru.ifru_ivalue = tx_header.s.prio;
		return 0;

	case CAVIUM_NET_IOCTL_SETIDSIZE:
		ivalue = rq->ifr_ifru.ifru_ivalue;
		if ((ivalue >= 0) && (ivalue < 2)) {
			tx_header.u64 = priv->srio_tx_header;
			tx_header.s.tt = ivalue;
			priv->srio_tx_header = tx_header.u64;
			return 0;
		}
		return -EINVAL;

	case CAVIUM_NET_IOCTL_GETIDSIZE:
		tx_header.u64 = priv->srio_tx_header;
		rq->ifr_ifru.ifru_ivalue = tx_header.s.tt;
		return 0;

	case CAVIUM_NET_IOCTL_SETSRCID:
		ivalue = rq->ifr_ifru.ifru_ivalue;
		if ((ivalue >= 0) && (ivalue < 2)) {
			tx_header.u64 = priv->srio_tx_header;
			tx_header.s.sis = ivalue;
			priv->srio_tx_header = tx_header.u64;
			return 0;
		}
		return -EINVAL;

	case CAVIUM_NET_IOCTL_GETSRCID:
		tx_header.u64 = priv->srio_tx_header;
		rq->ifr_ifru.ifru_ivalue = tx_header.s.sis;
		return 0;

	case CAVIUM_NET_IOCTL_SETLETTER:
		ivalue = rq->ifr_ifru.ifru_ivalue;
		if ((ivalue >= -1) && (ivalue < 4)) {
			tx_header.u64 = priv->srio_tx_header;
			tx_header.s.lns = (ivalue == -1);
			if (tx_header.s.lns)
				tx_header.s.letter = 0;
			else
				tx_header.s.letter = ivalue;
			priv->srio_tx_header = tx_header.u64;
			return 0;
		}
		return -EINVAL;

	case CAVIUM_NET_IOCTL_GETLETTER:
		tx_header.u64 = priv->srio_tx_header;
		if (tx_header.s.lns)
			rq->ifr_ifru.ifru_ivalue = -1;
		else
			rq->ifr_ifru.ifru_ivalue = tx_header.s.letter;
		return 0;

	case SIOCSHWTSTAMP:
		return cvm_oct_ioctl_hwtstamp(dev, rq, cmd);

	default:
		if (priv->phydev)
			return phy_mii_ioctl(priv->phydev, if_mii(rq), cmd);
		return -EINVAL;
	}
}

/**
 * cvm_oct_set_carrier - common wrapper of netif_carrier_{on,off}
 *
 * @priv: Device struct.
 * @link_info: Current state.
 */
void cvm_oct_set_carrier(struct octeon_ethernet *priv, cvmx_helper_link_info_t link_info)
{
	int port;
	nl_msg_head_t *head = NULL;
	kernel_nlmsg_t *nl_msg = NULL;
	char sendBuf[512];
	int msgLen = 0;
	head = (nl_msg_head_t*)sendBuf;
	head->type = OCTEON_ETHPORT_LINK_EVENT;
	nl_msg = (kernel_nlmsg_t*)(sendBuf + sizeof(nl_msg_head_t));	
	msgLen = sizeof(kernel_nlmsg_t) + sizeof(nl_msg_head_t);
	
	port = priv->ipd_port;
	
    if (priv->imode == CVMX_HELPER_INTERFACE_MODE_SGMII){
    	
        	if (autelan_product_info.board_type== AUTELAN_BOARD_AX81_SMU)
            {
                if(priv->ipd_port == 2)
                {
        			port = 0;
                }
        	}
        	if (autelan_product_info.board_type== AUTELAN_BOARD_AX81_SMUE)
            {
                if(priv->ipd_port == 1)
        			port = 0;
				if(priv->ipd_port == 2)
					port = 1;
        	}
            if(autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C)
            {
        		port = priv->ipd_port + 2;        /* master cpu: ipprt 2 -> index 4 on panel */
        	}
    		
    }
	
	if (priv->imode == CVMX_HELPER_INTERFACE_MODE_RGMII){
    		port = priv->ipd_port - 16;

	}
	if (priv->imode == CVMX_HELPER_INTERFACE_MODE_XAUI) {
		if(autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S || autelan_product_info.board_type == AUTELAN_BOARD_AX71_1X12G12S) {
			port = 62;
		}
	}

	nl_msg->ipgport = port;
	if (link_info.s.link_up) {
		if (!netif_carrier_ok(priv->netdev))
			netif_carrier_on(priv->netdev);
		
		cvmx_atomic_add32_nosync((int32_t*)&up_times[priv->ipd_port], 1);
		timestamp[priv->ipd_port] = current_kernel_time();
		
		if (priv->num_tx_queues)
			DEBUGPRINT(KERN_NOTICE"%s Link up: %u Mbps %s duplex, port %2d, queue %2d\n",
				   priv->netdev->name, link_info.s.speed,
				   (link_info.s.full_duplex) ? "Full" : "Half",
				   priv->ipd_port, priv->tx_queue[0].queue);
		else
			DEBUGPRINT(KERN_NOTICE"%s Link up: %u Mbps %s duplex, port %2d, POW\n",
				   priv->netdev->name, link_info.s.speed,
				   (link_info.s.full_duplex) ? "Full" : "Half",
				   priv->ipd_port);
		
		nl_msg->action_type = LINK_UP;
        cvm_oct_netlink_notifier(sendBuf, msgLen);
	} else {
	
		if (netif_carrier_ok(priv->netdev))
			netif_carrier_off(priv->netdev);
		
		DEBUGPRINT(KERN_NOTICE"%s: Link down\n", priv->netdev->name);

		cvmx_atomic_add32_nosync((int32_t*)&down_times[priv->ipd_port], 1);		
		timestamp[priv->ipd_port] = current_kernel_time();/*wc add*/
		
		nl_msg->action_type = LINK_DOWN;
        cvm_oct_netlink_notifier(sendBuf, msgLen);
	}
	#if 0  /* no use in sp6c huangjing */
	if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S || autelan_product_info.board_type == AUTELAN_BOARD_AX71_1X12G12S) {
		if((nl_msg->ipgport == 0)||(nl_msg->ipgport == 1)||(nl_msg->ipgport == 62)) {
			return;
		}
	}
	cvm_oct_netlink_notifier(sendBuf, msgLen);
	#endif
}

static void cvm_oct_adjust_link(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	cvmx_helper_link_info_t link_info;

	if (priv->last_link != priv->phydev->link) {
		priv->last_link = priv->phydev->link;
		link_info.u64 = 0;
		link_info.s.link_up = priv->last_link ? 1 : 0;
		link_info.s.full_duplex = priv->phydev->duplex ? 1 : 0;
		link_info.s.speed = priv->phydev->speed;
		cvmx_helper_link_set(priv->ipd_port, link_info);
		cvm_oct_set_carrier(priv, link_info);
	}
}

static void cvm_oct_mdio_dummy_write(struct net_device *dev, int phy_id, int location, int val)
{
}

static int cvm_oct_mdio_dummy_read(struct net_device *dev, int phy_id, int location)
{
    return 0xffff;
}

void cvm_oct_mdio_write(struct net_device *dev, int phy_id, int location, int val)
{
    cvmx_smi_cmd_t          smi_cmd;
    cvmx_smi_wr_dat_t       smi_wr;
    int timeout = 1000;

    smi_wr.u64 = 0;
    smi_wr.s.dat = val;
    cvmx_write_csr(CVMX_SMI_WR_DAT, smi_wr.u64);

    smi_cmd.u64 = 0;
    smi_cmd.s.phy_op = 0;
    smi_cmd.s.phy_adr = phy_id;
    smi_cmd.s.reg_adr = location;
    cvmx_write_csr(CVMX_SMI_CMD, smi_cmd.u64);

    do
    {	
    	/*
		huxuefeng note.20100907
		cancel cpu preemptive when reading phy.
    		*/
        //if (!in_interrupt())
        //    yield();
        smi_wr.u64 = cvmx_read_csr(CVMX_SMI_WR_DAT);
    } while (smi_wr.s.pending && timeout--);
}

/**
 * Perform an MII read. Called by the generic MII routines
 *
 * @param dev      Device to perform read for
 * @param phy_id   The MII phy id
 * @param location Register location to read
 * @return Result from the read or zero on failure
 */
int cvm_oct_mdio_read(struct net_device *dev, int phy_id, int location)
{
    cvmx_smi_cmd_t          smi_cmd;
    cvmx_smi_rd_dat_t       smi_rd;
    int timeout = 1000;

    smi_cmd.u64 = 0;
    smi_cmd.s.phy_op = 1;
    smi_cmd.s.phy_adr = phy_id;
    smi_cmd.s.reg_adr = location;
    cvmx_write_csr(CVMX_SMI_CMD, smi_cmd.u64);

    do
    {
    	/*
		huxuefeng note.20100907
		cancel cpu preemptive when reading phy.
    		*/
        //if (!in_interrupt())
        //    yield();
        smi_rd.u64 = cvmx_read_csr(CVMX_SMI_RD_DAT);
    } while (smi_rd.s.pending && timeout--);

    if (timeout <= 0)
    {
        return 0;
    }

    if (smi_rd.s.val)
        return smi_rd.s.dat;
    else
        return 0;
}

/**
 * cvm_oct_phy_setup_device - setup the PHY
 *
 * @dev:    Device to setup
 *
 * Returns Zero on success, negative on failure
 */
int cvm_oct_phy_setup_device(struct net_device *dev)
{
	struct octeon_ethernet *priv = netdev_priv(dev);
	struct device_node *phy_node;
	int phy_id;
	static int phy_id_count = 0;

	

	if (!priv->of_node || priv->imode == CVMX_HELPER_INTERFACE_MODE_XAUI)
		return 0;

	phy_node = of_parse_phandle(priv->of_node, "phy-handle", 0);
	if (!phy_node)
		return 0;

	if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C && (priv->ipd_port == 0 || priv->ipd_port == 1))
	{
		priv->phydev = NULL;
		return -ENODEV;
	}
#if 0    /* no use in sp6c huangjing  XG phy */
	if ((autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S && (priv->ipd_port == 0 || priv->ipd_port == 1)) || (autelan_product_info.board_type == AUTELAN_BOARD_AX71_1X12G12S && (priv->ipd_port == 0 || priv->ipd_port == 1)))
	{
		priv->phydev = NULL;
		return -ENODEV;
	}
#endif
	priv->phydev = of_phy_connect(dev, phy_node, cvm_oct_adjust_link, 0,
				      PHY_INTERFACE_MODE_GMII);

	if (priv->phydev == NULL)
		return -ENODEV;

	priv->last_link = 0;
	phy_start_aneg(priv->phydev);
	


    switch (priv->imode)
    {
        case CVMX_HELPER_INTERFACE_MODE_RGMII:
            /* Make a guess that RGMII ports just increment from 0 */
			phy_id = phy_id_count++;			
            break;

        case CVMX_HELPER_INTERFACE_MODE_GMII:
            /* All of the Cavium boards in GMII mode have a Marvel switch
                connected that supports extended state info from. MDIO
                addresses 0-3 are the switch ports, 4 is the RGMII, and 9
                is the port connected using GMII */
            phy_id = (priv->pko_port == 0) ? 4 : 9;
            break;
			
        default:
            /* Only support MDIO in RGMII and GMII ports */
            phy_id = -1;
            break;
    }

    if (phy_id != -1)
    {
        priv->mii_info.dev = dev;
        priv->mii_info.phy_id = phy_id;
        priv->mii_info.phy_id_mask = 0xff;
        priv->mii_info.supports_gmii = 1;
        priv->mii_info.reg_num_mask = 0x1f;
        priv->mii_info.mdio_read = cvm_oct_mdio_read;
        priv->mii_info.mdio_write = cvm_oct_mdio_write;
    }
    else
    {
        /* Supply dummy MDIO routines so the kernel won't crash
            if the user tries to read them */
        priv->mii_info.mdio_read = cvm_oct_mdio_dummy_read;
        priv->mii_info.mdio_write = cvm_oct_mdio_dummy_write;
    }	

	return 0;
}
