/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell 
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File in accordance with the terms and conditions of the General 
Public License Version 2, June 1991 (the "GPL License"), a copy of which is 
available along with the File in the license.txt file or by writing to the Free 
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or 
on the worldwide web at http://www.gnu.org/licenses/gpl.txt. 

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED 
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY 
DISCLAIMED.  The GPL License provides additional details about this warranty 
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or 
modify this File under the following licensing terms. 
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer. 

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution. 

    *   Neither the name of Marvell nor the names of its contributors may be 
        used to endorse or promote products derived from this software without 
        specific prior written permission. 
    
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
********************************************************************************
* prestera.c
*
* DESCRIPTION:
*       functions in kernel mode special for prestera.
*
* DEPENDENCIES:
*
*       $Revision: 1.7 $
*******************************************************************************/
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
/* add for sdk2.2 */
#include <linux/sched.h>   /* for send_sig_info */
#include <linux/isapnp.h>   /* for DEVICE_COUNT_COMPATIBLE */
#include <linux/smp_lock.h>   /* for lock_kernel */
#include <linux/isapnp.h>   /* for nopage */
#include <linux/isapnp.h>   /* for DEVICE_COUNT_COMPATIBLE */
#include <linux/isapnp.h>   /* for DEVICE_COUNT_COMPATIBLE */

#include "./include/prestera_glob.h"
#include "./include/presteraIrq.h"
#include "./include/prestera.h"
#include <cpssCommon/cpssPresteraDefs.h>
#include <cpss/generic/cpssTypes.h>

/* zhangdi add */
#include <linux/fs.h>   /* flip_open() */
#include <asm/uaccess.h>    /*  get_fs() */
#include "sysdef/npd_sysdef.h"  /* for BOARD_TYPE_AX71_2X12G12S and so on*/

static int board_type_mvpp = -1;
static int slot_id_mvpp = -1;
/* add end */

/*#define PRESTERA_DEBUG*/
#ifdef PRESTERA_DEBUG
#define DBG_PRINTK(x)   printk x
#else 
#define DBG_PRINTK(x)
#endif 

#define PCI_RESOURCE_PSS_CONFIG     0
#define PCI_RESOURCE_PSS_REGS       1
#define PCI_RESOURCE_PSS_REGS_PEX   2

#define DMA_AREA_SIZE	( 4 * _1MB )

/* local variables and variables */
static int                  prestera_opened      = 0;
static int                  prestera_initialized = 0;
static struct Prestera_Dev  *prestera_dev;
static struct pci_dev       *pcidevs[PRV_CPSS_MAX_PP_DEVICES_CNS];
struct PP_Dev        		*ppdevs[PRV_CPSS_MAX_PP_DEVICES_CNS];
static int                  founddevs = 0;
static unsigned long        dev_size = 0;
static int                  prestera_major = PRESTERA_MAJOR;
static struct cdev          prestera_cdev;
static unsigned long        legalDevTypes[] = {CPSS_DEV_TYPES};
static unsigned long    	dma_base;
/**
  *  Prestera driver directory under proc file system
  */
static struct proc_dir_entry *presteraDir = NULL;
/**
 * Periodic timer to check port MAC MIB
 */
static struct timer_list prestera_mib_timer;
/**
 * Port MAC mib counter info
 */
static struct prestera_port **presteraPorts = NULL;


/*MODULE_PARM(prestera_major,"i");*/
module_param(prestera_major,int,0444);
MODULE_PARM_DESC(prestera_major," prestera character device major number. ");

/* 
  * prestera device number to config or show 
  * legal value for chassis product(e.g. AX7005)
  *	0 - 98Dx275 	
  *	1 - 98Dx804
  *  255 - for all devices
  *
  */
static int mv_dev = 255; 
module_param(mv_dev,int,0644);
MODULE_PARM_DESC(mv_dev," prestera device number to configure or read. ");

/* 
  * prestera port number to config or show 
  * legal value for chassis product(e.g. AX7005)
  *	when device number is 0 - 98Dx275
  *		0 ~ 23 for Giga Port
  *		24~26 for HyperG or XG Port
  *	when device number is 1 - 98Dx804
  *		24 ~ 27 for HyperG or XG Port
  *  255 - for all ports
  *
  */
static int mv_port = 255;
module_param(mv_port,int,0644);
MODULE_PARM_DESC(mv_port," prestera device port number to configure or read. ");
/*
  * prestera device register
  *
  */
static int mv_reg = 0;
module_param(mv_reg,int,0644);
MODULE_PARM_DESC(mv_reg," prestera device register address to configure or read. ");

/*
  *	register value to write to prestera device
  *
  */
 static unsigned int mv_data = 0;
module_param(mv_data,uint,0644);
MODULE_PARM_DESC(mv_data," prestera device register value to write. ");
/*
  * cpld register
  *
  */
static int cpld_reg = 0;
module_param(cpld_reg,int,0644);
MODULE_PARM_DESC(cpld_reg," CPLD register address to configure or read. ");

/*
  *	register value to write to CPLD
  *
  */
 static unsigned char cpld_data = 0;
module_param(cpld_data,byte,0644);
MODULE_PARM_DESC(cpld_data," CPLD register value to write. ");

/*
  *	PHY page selector
  *
  */
 static unsigned char phy_page = 0;
module_param(phy_page,byte,0644);
MODULE_PARM_DESC(phy_page," PHY control page selector 0:copper 1:fiber 2:MAC. ");

/*
  *	PHY register address
  *
  */
 static unsigned char phy_reg = 0;
module_param(phy_reg,byte,0644);
MODULE_PARM_DESC(phy_reg," PHY register address read from or write to. ");

/*
  *	PHY register value
  *
  */
 static unsigned short phy_value = 0;
module_param(phy_value,ushort,0644);
MODULE_PARM_DESC(phy_value," PHY register value write to. ");

MODULE_AUTHOR("Marvell Semi.");
MODULE_LICENSE("GPL");

static struct quirks {
    unsigned long pci_id;
    
    /* Some? PEX PP has config started at offset 0x70000 in
     * config address space, access to addresses below this
     * offset causes to hang system.
     * If packet processor is listed here add config_offset
     * when accessing configuration registers */
    unsigned long config_offset;
    int           is_pex;
    int           regs_resource;
} prestera_quirks[] = {
    { CPSS_98EX240_1_CNS, 0x70000, 1, PCI_RESOURCE_PSS_REGS_PEX },
    { CPSS_98EX240_CNS,   0x70000, 1, PCI_RESOURCE_PSS_REGS_PEX },
    { CPSS_98EX260_CNS,   0x70000, 1, PCI_RESOURCE_PSS_REGS_PEX },/*need check */
    { CPSS_98DX4122_CNS,  0x70000, 1, PCI_RESOURCE_PSS_REGS_PEX },
    { CPSS_98CX8248_CNS,  0x70000, 1, PCI_RESOURCE_PSS_REGS_PEX },
    { 0xffffffff,         0,       0, PCI_RESOURCE_PSS_REGS } /* default */
};

/* Forward declarations */
/* Forward function declarations */

/************************************************************************
 *
 * And the init and cleanup functions come last
 *
 ************************************************************************/

static struct quirks* prestera_find_quirks(struct pci_dev *dev)
{
    int k;
    for (k = 0; prestera_quirks[k].pci_id != 0xffffffff; k++)
    {
        unsigned int vendor, device;
        vendor = prestera_quirks[k].pci_id & 0x0000ffff;
        device = (prestera_quirks[k].pci_id >> 16) & 0x0000ffff;
        if (dev->vendor == vendor && dev->device == device)
            break;
    }
    return &(prestera_quirks[k]);
}

static loff_t prestera_lseek(struct file *filp, loff_t off, int whence)
{
   struct  Prestera_Dev * dev;
   loff_t newpos;

    dev = (struct Prestera_Dev *) filp->private_data;
    switch (whence)
    {
        case 0: /* SEEK_SET */
            newpos = off;
            break;
        case 1: /* SEEK_CUR */
            newpos = filp->f_pos + off;
            break;
        case 2: /* SEEK_END */
            newpos = dev->size + off;
            break;
        default : /* can't happend */
            return -EINVAL;
    }
    if (newpos < 0)
    {
        return -EINVAL;
    }
    if (newpos >= dev->size)
    {
        return -EINVAL;
    }
    filp->f_pos = newpos;
    return newpos;
}


/************************************************************************
 * prestera_read: this should be the read device function, for now in
 * current prestera driver implemention it dose nothig
 ************************************************************************/
static ssize_t prestera_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    return -ERESTARTSYS;
}

/************************************************************************
 *
 * prestera_write: this should be the write device function, for now in
 * current prestera driver implemention it dose nothig
 *
 ************************************************************************/
static ssize_t prestera_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    return -ERESTARTSYS;
}

void prestera_transmit_value
(
	struct port_mib_info 	*mib,
	GT_PKG_COUNT_STAT *data
)
{
	data->rx.goodbytesl = mib->RxGoodBytesL32;
	data->rx.goodbytesh = mib->RxGoodBytesH32; 
	data->rx.badbytes = mib->RxBadBytes;
	data->rx.uncastpkts = mib->RxUcast;
	data->rx.bcastpkts = mib->RxBcast;
	data->rx.mcastpkts = mib->RxMcast;
	data->rx.fcframe = mib->RxFc;
	data->rx.overruns = mib->RxFifoOverrun;/*fifo overrun rx*/
	data->rx.underSizeframe = mib->Undersized;/*underSize*/
	data->rx.fragments = mib->Fragmented;/*fragments*/
	data->rx.overSizeframe = mib->Oversized;/*overSize*/
	data->rx.jabber = mib->Jabber;/*Jabber*/
	data->rx.errorframe = mib->RxError;/*Error frame*/
	data->rx.CRCerrors = mib->BadCrc;/*Bad CRC*/

	data->tx.goodbytesl = mib->TxGoodBytesL32;
	data->tx.goodbytesh =	mib->TxGoodBytesH32;
	data->tx.uncastframe = mib->TxUcast;
	data->tx.mcastframe = mib->TxMcast;
	data->tx.bcastframe = mib->TxBcast;
	data->tx.fcframe = mib->TxFc;
	data->tx.crcerror_fifooverrun = mib->TxCrcError;

	if(0 == data->mode)
		data->rx.collision = mib->LateCollision + mib->CollisionEvent;
	else
		data->rx.collision = mib->LateCollision + mib->CollisionEvent + mib->TxExCollision;
	
}

/*******************************************************************************
* prestera_get_port_stat
*
* DESCRIPTION:
*		get port pkt stat
*
* INPUTS:
*		argp		- output address	
*		data	 	- input param
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		This route is hooked as procfs read routine.
*
*******************************************************************************/
int prestera_get_port_stat
(
	unsigned long argp,
	GT_PKG_COUNT_STAT *data
)
{
	struct prestera_port 	*ports = NULL;
	struct port_mib_info 	*mib = NULL;
	
	if(!presteraPorts) 
	{
		return 0;
	}
    /* code optimize: Unsigned compared against 0 */
	if(data->devNum <= (unsigned char)founddevs) 
	{
		ports = presteraPorts[data->devNum];
		if((NULL != ports) && (NULL != ports->mib)) 
		{
			if((ports->startPort + ports->count) > data->portNum) 
			{
				mib = ports->mib[data->portNum];
				prestera_transmit_value(mib,data);
			
				if (copy_to_user((GT_PKG_COUNT_STAT*)argp,data,sizeof(GT_PKG_COUNT_STAT)))
			  	{
			      	printk("IOCTL: FAULT\n");
			      	return -EFAULT;
			  	}
			}
		}
	}

	return 0;
}

/*******************************************************************************
* prestera_clear_port_stat
*
* DESCRIPTION:
*		clear port pkt stat
*
* INPUTS:
*		data	 	- input param
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		This route is hooked as procfs read routine.
*
*******************************************************************************/
int prestera_clear_port_stat
(
	GT_DEV_PORT *data
)
{
	struct prestera_port 	*ports = NULL;
	struct port_mib_info 	*mib = NULL;
	
	if(!presteraPorts) 
	{
		return 0;
	}

    /* code optimize: Unsigned compared against 0 */
	if(data->devNum<= (unsigned char)founddevs) 
	{
		ports = presteraPorts[data->devNum];
		if(ports && ports->mib) 
		{
			mib = ports->mib[data->portNum];
			memset(mib,0,sizeof(struct port_mib_info));
		}
	}

	return 0;
}

/************************************************************************
 *
 * prestera_ioctl: The device ioctl() implementation
 */
static int prestera_ioctl(struct inode *inode, struct file *filp,
                          unsigned int cmd, unsigned long arg)
{
    struct pci_dev          *dev = NULL;
    PP_PCI_REG              pciReg;
    PciConfigReg_STC        pciConfReg;
    GT_PCI_Dev_STC          gtDev;
    GT_Intr2Vec             int2vec;
    GT_VecotrCookie_STC     vector_cookie;
    struct intData          *intData;
    int                     i, instance;
    unsigned long           devfn;
	#ifndef __AX_PLATFORM__
	GT_Dma_Base_STC			dmaBaseSTC;
	GT_RAWIOWR_STC			pciRawIOWR;
	unsigned long 			data64;
	unsigned long 			cookiePtr;
	GT_Cookie_Ptr_STC		cookiePtrVal;
	GT_PKG_COUNT_STAT  pkgStat;
	GT_DEV_PORT  dev_port_s;
	#endif
    
    /* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
    if (_IOC_TYPE(cmd) != PRESTERA_IOC_MAGIC)
    {
        printk("wrong ioctl magic key\n");
        return -ENOTTY;
    }

    /* GETTING DATA */
    switch(cmd)
    {
        case PRESTERA_IOC_READREG:
            /* read and parse user data structur */
            if (copy_from_user(&pciReg,(PP_PCI_REG*) arg, sizeof(PP_PCI_REG)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            /* USER READS */
            if (copy_to_user((PP_PCI_REG*)arg, &pciReg, sizeof(PP_PCI_REG)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_WRITEREG:
            /* read and parse user data structur */
            if (copy_from_user(&pciReg,(PP_PCI_REG*) arg, sizeof(PP_PCI_REG)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_HWRESET:
            break;

        case PRESTERA_IOC_INTCONNECT:
            /* read and parse user data structure */
            if (copy_from_user(&vector_cookie,(GT_VecotrCookie_STC*) arg, sizeof(GT_VecotrCookie_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            intConnect(vector_cookie.vector, 0, (struct intData **)&cookiePtr);
			vector_cookie.cookieLow32 = cookiePtr & 0xFFFFFFFF;
			vector_cookie.cookieHigh32 = (cookiePtr >> 32) & 0xFFFFFFFF;
			/* bind interrupt info to PP device,as weed need it when cleanup */
            for (i = 0; i < founddevs; i++)
            {
                dev = pcidevs[i];
				if(!dev) 
					continue;
				
				if(vector_cookie.vector == dev->irq)
				{
					if(ppdevs[i]) 
						ppdevs[i]->cookie = (struct intData*)cookiePtr;
				}				
            }
			
            /* USER READS */
            if (copy_to_user((PP_PCI_REG*)arg, &vector_cookie, sizeof(GT_VecotrCookie_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_INTENABLE:
            /* clear the mask reg on device 0x10*/
            if (arg > 131)
            {
                printk("DEBUG!!!\n");
                send_sig_info(SIGSTOP, (struct siginfo*)1, current);
            }
            /*enable_irq(arg)*/
            break;

        case PRESTERA_IOC_INTDISABLE:
            disable_irq(arg);
            break;

        case PRESTERA_IOC_WAIT:
            if (copy_from_user(&cookiePtrVal,(GT_Cookie_Ptr_STC*) arg, sizeof(GT_Cookie_Ptr_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
 
			
            /* cookie */
			#if 0
            intData = (struct intData *) arg;
			#else
			cookiePtr = 0UL;
			cookiePtr |= cookiePtrVal.high32;
			intData = (struct intData *)(( cookiePtr << 32) | cookiePtrVal.low32);
			#endif
			/* enable the interrupt vector */
			/*enable_irq(intData->intVec);*/
			
            if (down_interruptible(&intData->sem))
            {
                return -ERESTARTSYS;
            }
            break;

        case PRESTERA_IOC_FIND_DEV:
            /* read and parse user data structurr */
            if (copy_from_user(&gtDev,(GT_PCI_Dev_STC*) arg, sizeof(GT_PCI_Dev_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            instance = 0;
            
            for (i = 0; i < founddevs; i++)
            {
                dev = pcidevs[i];
                if (gtDev.vendorId != dev->vendor)
                {
                    continue;
                }
                if (gtDev.devId != dev->device)
                {
                    continue;
                }
                if (instance < gtDev.instance)
                {
                    instance++;
                    continue;
                }
                /* Found */
                gtDev.busNo  = (unsigned long)dev->bus->number;
                gtDev.devSel = PCI_SLOT(dev->devfn); 
                gtDev.funcNo = PCI_FUNC(dev->devfn);
				printk("IOCTL:find device 0x%x verdor %s on bus %d instance %d[in] %d\r\n",
					gtDev.devId,(0x11AB==gtDev.vendorId)?"marvell":"unknown",gtDev.busNo,gtDev.instance,instance);
				printk("IOCTL:pci device BAR0 start 0x%16lx end 0x%16lx len 0x%8lx\r\n", \
					pci_resource_start(dev,0),pci_resource_end(dev,0),pci_resource_len(dev,0));
				printk("IOCTL:pci device BAR1 start 0x%16lx end 0x%16lx len 0x%8lx\r\n", \
					pci_resource_start(dev,1),pci_resource_end(dev,1),pci_resource_len(dev,1));
				printk("IOCTL:mv  device BAR0 start 0x%16lx len 0x%8lx\r\n", \
					ppdevs[i]->config.base,ppdevs[i]->config.size);
				printk("IOCTL:mv  device BAR1 start 0x%16lx len 0x%8lx\r\n", \
					ppdevs[i]->ppregs.base,ppdevs[i]->ppregs.size);
				break;
            }
            if (i == founddevs)
            {
                return -ENODEV;
            }
            /* READ */
            if (copy_to_user((GT_PCI_Dev_STC*)arg, &gtDev, sizeof(GT_PCI_Dev_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_PCICONFIGWRITEREG:
            /* read and parse user data structure */
            if (copy_from_user(&pciConfReg,(PciConfigReg_STC*) arg, sizeof(PciConfigReg_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            devfn = PCI_DEVFN(pciConfReg.devSel, pciConfReg.funcNo);
            for (i = 0; i < founddevs; i++)
            {
                dev = pcidevs[i];
                if (pciConfReg.busNo != (unsigned long)dev->bus->number)
                {
                    continue;
                }
                if (devfn != dev->devfn)
                {
                    continue;
                }
                /* Found */
                pci_write_config_dword(dev, pciConfReg.regAddr, pciConfReg.data);
                break;
            }
            if (i == founddevs)
            {
                return -ENODEV;
            }
            break;

        case PRESTERA_IOC_PCICONFIGREADREG:
            /* read and parse user data structure */
            if (copy_from_user(&pciConfReg,(PciConfigReg_STC*) arg, sizeof(PciConfigReg_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            /* READ */
            devfn = PCI_DEVFN(pciConfReg.devSel, pciConfReg.funcNo);
            for (i = 0; i < founddevs; i++)
            {
                dev = pcidevs[i];
                if (pciConfReg.busNo != (unsigned long)dev->bus->number)
                {
                    continue;
                }
                if (devfn != dev->devfn)
                {
                    continue;
                }
                /* Found */
                pci_read_config_dword(dev, pciConfReg.regAddr, (u32 *)&pciConfReg.data);
				printk("IOCTL:read pci 0x%02x data 0x%016x\r\n",pciConfReg.regAddr,pciConfReg.data);
                break;
            }
            if (i == founddevs)
            {
                return -ENODEV;
            }
            if (copy_to_user((PciConfigReg_STC*)arg, &pciConfReg, sizeof(PciConfigReg_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_GETINTVEC:
            if (copy_from_user(&int2vec,(GT_Intr2Vec*) arg, sizeof(GT_Intr2Vec)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }

            devfn = PCI_DEVFN(int2vec.deviceNo, int2vec.funcNo);

            #ifdef __AX_DEBUG__
            printk("IOCTL PRESTERA_IOC_GETINTVEC:\n");
            printk("   busNo    = %d\n", int2vec.busNo);
            printk("   deviceNo = %d\n", int2vec.deviceNo);
            printk("   funcNo   = %d\n", int2vec.funcNo);
            printk("   devfn    = %d\n", devfn);
            #endif
            
            for (i = 0; i < founddevs; i++)
            {
                dev = pcidevs[i];
                #ifdef __AX_DEBUG__
                printk("     for: i = %d\n", i);
                printk("          dev->bus->number = %d\n", dev->bus->number);
                printk("          dev->devfn = %d\n", dev->devfn);
                #endif
                if (int2vec.busNo != (unsigned long)dev->bus->number)
                {
                    continue;
                }
                if (devfn != dev->devfn)
                {
                    continue;
                }
                /* Found */
                break;
            }
            if (i == founddevs)
            {
                return -ENODEV;
            }

            int2vec.vector = dev->irq;
			printk("IOCTL:find irq %d for bus %d dev %d fun %d\r\n", \
						int2vec.vector,int2vec.busNo,int2vec.deviceNo,int2vec.funcNo);

            if (copy_to_user((GT_Intr2Vec*)arg, &int2vec, sizeof(GT_Intr2Vec)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;

        case PRESTERA_IOC_FLUSH:
#if 0
            /* read and parse user data structure */
            if (copy_from_user(&range,(GT_RANGE_STC*) arg, sizeof(GT_RANGE_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            /*consistent_sync((void*)range.address, range.length, PCI_DMA_TODEVICE);*/
            dma_cache_sync((void*)range.address, range.length, PCI_DMA_TODEVICE);
#endif
	    break;

        case PRESTERA_IOC_INVALIDATE:
#if 0
	    /* read and parse user data structure */
            if (copy_from_user(&range,(GT_RANGE_STC*) arg, sizeof(GT_RANGE_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            /*consistent_sync((void*)range.address, range.length, PCI_DMA_FROMDEVICE);*/
            dma_cache_sync((void*)range.address, range.length, PCI_DMA_FROMDEVICE);
#endif
	    break;

        case PRESTERA_IOC_GETBASEADDR:
            if (copy_to_user((long*)arg, &dma_base, sizeof(long)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;
		#ifndef __AX_PLATFORM__
		case PRESTERA_IOC_RAWMEMRW:
			/* got user parameters */
			if(copy_from_user(&pciRawIOWR,(GT_RAWIOWR_STC*)arg,sizeof(GT_RAWIOWR_STC)))
			{
				printk("IOCTL:Raw io mem read from user fault\r\n");
				return -EFAULT;
			}
			printk("IOCTL:Raw io mem %s 0x%08x 0x%08x\r\n",(0==pciRawIOWR.isRead)?"write to":"read from", \
					pciRawIOWR.address,(pciRawIOWR.isRead)? 0:pciRawIOWR.data);
			/* do real read /write op
			if(pciRawIOWR.address > 0x00011bffffffffff)
			{
				printk("IOCTL:Raw io mem address 0x%016lx illegal\r\n",pciRawIOWR.address);
				return -EFAULT;
			}
			 read op */
			if(0!=pciRawIOWR.isRead)
			{
				pciRawIOWR.address |= (1UL<<63);
				pciRawIOWR.data = (unsigned long)(*(volatile unsigned long *)(pciRawIOWR.address));
				printk("IOCTL:Raw io read data 0x%08x at 0x%08x\r\n",pciRawIOWR.data,pciRawIOWR.address);
			}
			/* write op */
			else if(0==pciRawIOWR.isRead)
			{
				pciRawIOWR.address |= (1UL<<63);
				*((volatile unsigned long*)pciRawIOWR.address) = pciRawIOWR.data;
				data64 = (unsigned long)(*(volatile unsigned long*)(pciRawIOWR.address));
				pciRawIOWR.data = data64;
				printk("IOCTL:Raw io mem write data 0x%08x to 0x%08x got 0x%016lx\r\n",  \
											pciRawIOWR.data,pciRawIOWR.address,data64);
			}
			/* write back to user */
			if(copy_to_user((GT_RAWIOWR_STC*)arg,&pciRawIOWR,sizeof(GT_RAWIOWR_STC)))
			{
				printk("IOCTL:Raw io mem read from user fault\r\n");
				return -EFAULT;
			}
			break;
		case PRESTERA_IOC_GETDEVBASEADDR:
			printk("\n************** GETDEVBASEADDR ***************\n");
            if (copy_from_user(&dmaBaseSTC,(GT_Dma_Base_STC *) arg, sizeof(GT_Dma_Base_STC)))
            {
                printk("IOCTL:PRESTERA_IOC_GETDEVBASEADDR copy from user error\n");
                return -EFAULT;
            }

            devfn = PCI_DEVFN(dmaBaseSTC.devSel, dmaBaseSTC.funcNo);
            for (i = 0; i < founddevs; i++)
            {
                dev = pcidevs[i];
                if (dmaBaseSTC.busNo != (unsigned long)dev->bus->number)
                {
                    continue;
                }
                if (devfn != dev->devfn)
                {
                    continue;
                }
                /* Found */
                break;
            }
            if (i == founddevs)
            {
            	printk("IOCTL:PRESTERA_IOC_GETDEVBASEADDR device not found\n");
                return -ENODEV;
            }

            dmaBaseSTC.dma_base 	= ppdevs[i]->ppdma.base;
			dmaBaseSTC.device_base 	= ppdevs[i]->ppdma.device_base;
			printk("IOCTL:dev %d dma base %#0x device dma base %#0x\r\n", \
									i,dmaBaseSTC.dma_base,dmaBaseSTC.device_base);

			if (copy_to_user((long*)arg, &dmaBaseSTC, sizeof(GT_Dma_Base_STC)))
			{
				printk("IOCTL: FAULT\n");
				return -EFAULT;
			}
			break;	
		case PRESTERA_IOC_GETPKGSTACOUNT:
			if (copy_from_user(&pkgStat,(GT_PKG_COUNT_STAT*) arg, sizeof(GT_PKG_COUNT_STAT)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
			return prestera_get_port_stat(arg,&pkgStat);
			
		case PRESTERA_IOC_CLAERPKGCOUNT:
			if (copy_from_user(&dev_port_s,(GT_DEV_PORT*) arg, sizeof(GT_DEV_PORT)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
			return prestera_clear_port_stat(&dev_port_s);

		#endif
        default:
            printk (KERN_WARNING "Unknown ioctl (%x).\n", cmd);
            break;
    }
    return 0;
}
/************************************************************************
 *
 * prestera_compat_ioctl: 
 * The device ioctl() implementation portable for 32-bit process call ioctl on 64-bit system. 
 */

long prestera_compat_ioctl(struct file *filp,unsigned int cmd, unsigned long arg) {
    int rval;

    //printk("<3>""COMPAT IOCTL cmd is [%x] IOC Type is [%d].\n",cmd, _IOC_TYPE(cmd));
    lock_kernel();
    rval = prestera_ioctl(filp->f_dentry->d_inode, filp, cmd, arg);
    unlock_kernel();

    return (rval == -EINVAL) ? -ENOIOCTLCMD : rval;
}

/*
 * open and close: just keep track of how many times the device is
 * mapped, to avoid releasing it.
 */

void prestera_vma_open(struct vm_area_struct *vma)
{
    prestera_opened++;
}

void prestera_vma_close(struct vm_area_struct *vma)
{
    prestera_opened--;
}

struct vm_operations_struct prestera_vm_ops = {
    .open   = prestera_vma_open,
    .close  = prestera_vma_close,
    .fault = NULL,
    .page_mkwrite=NULL,
    .access = NULL,
};


/************************************************************************
 *
 * prestera_mmap: The device mmap() implementation
 */
static int prestera_mmap(struct file * file, struct vm_area_struct *vma)
{
    unsigned long 	phys;
    unsigned long   ppNum;
    unsigned long   offset;
    unsigned long   pageSize;
    struct pci_dev  *dev;
    struct quirks   *quirks;

	#if 0   /* code optimize: Wrong operator used. (vma->vm_pgoff << 12) & 4095UL is always 0 2013-01-18 */
    if (((vma->vm_pgoff)<<PAGE_SHIFT) & (PAGE_SIZE-1))
    {
        /* need aligned offsets */                
        printk("prestera_mmap offset not aligned\n");
        return -ENXIO;  
    }
	#endif
	
    /* bind the prestera_vm_ops */
    vma->vm_ops = &prestera_vm_ops;

    /* VM_IO for I/O memory */
    vma->vm_flags |= VM_IO;

    /* disable caching on mapped memory */
    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
    vma->vm_private_data = prestera_dev;

	printk("MMAP:vma start %#lx pageoffset %#lx\r\n",vma->vm_start,vma->vm_pgoff);
    
    switch (vma->vm_start & 0xf0000000)
    {
        case ASIC_DMA_VIRT_ADDR:
            /* DMA memory */
			/*
			  * 98DX275 and 98DX804 need DMA memory of themselves
			  * [98DX275] range:0x50000000~0x503FFFFF,size:4MB
			  * [98DX804] range:0x50400000~0x507FFFFF,size:4MB
			  */
			#ifndef __AX_PLATFORM__
            offset = (vma->vm_start - ASIC_DMA_VIRT_ADDR);
            offset -= (vma->vm_pgoff<<PAGE_SHIFT);
            ppNum = offset / DMA_AREA_SIZE ;
            phys = ppdevs[ppNum]->ppdma.base ;
            pageSize = DMA_AREA_SIZE ;
			#else
            phys = dma_base;
            pageSize = dma_len;			
			#endif
			printk("MMAP:phys %#lx size %#lx\r\n",phys,pageSize);
            break;

        case ASIC_CFG_VIRT_ADDR:
            /* PSS config */
            offset = (vma->vm_start - ASIC_CFG_VIRT_ADDR);
            offset -= (vma->vm_pgoff<<PAGE_SHIFT);
            ppNum = offset / PAGE_SIZE;
            dev = pcidevs[ppNum];
            quirks = prestera_find_quirks(dev);
            
            phys = pci_resource_start(dev, PCI_RESOURCE_PSS_CONFIG);
            pageSize = pci_resource_len(dev, PCI_RESOURCE_PSS_CONFIG);

            /* skip quirks->config_offset (0x70000) in this region */
            if (pageSize > quirks->config_offset) /* be paranoid */
            {
                phys += quirks->config_offset;
                pageSize -= quirks->config_offset;
            }

            if (pageSize > vma->vm_end - vma->vm_start)
                pageSize = vma->vm_end - vma->vm_start;
			printk("MMAP:phys %#lx size %#lx\r\n",phys,pageSize);
            break;
		/* For lion-1 on AX81-SMU, the reg base is 0x40000000 - 0x50000000. zhangdi@autelan.com 2012-02-27 */
        case 0x40000000:    
		/* For lion-0 is 0x30000000 - 0x4000000 */
        case ASIC_REG_VIRT_ADDR:
            /* PSS regs */
            offset = (vma->vm_start - ASIC_REG_VIRT_ADDR) + ((vma->vm_pgoff)<<PAGE_SHIFT);
            ppNum = offset / _64MB;
            dev = pcidevs[ppNum];
            quirks = prestera_find_quirks(dev);
            printk("offset=%x  ppNum=%d  quirks->regs_resource=%d\n", offset, ppNum, quirks->regs_resource);
            #if 1   /*cpss1.3*/
            phys = pci_resource_start(dev, quirks->regs_resource);
            #else
            phys = pci_resource_start(dev, 2);
            #endif
            pageSize = _64MB;
			printk("MMAP:phys %#lx size %#lx\r\n",phys,pageSize);
			break;

        default:
            /* ??? */
            printk("unknown range (0%0x)\n", (int)vma->vm_start);
            return 1;
    }

    DBG_PRINTK(("remap_pfn_range(0x%X, 0x%X, 0x%X, 0x%X)\n",
                (int)(vma->vm_start), (int)(phys >> PAGE_SHIFT),
                (int)pageSize, (int)(vma->vm_page_prot)));

    printk("MMAP:remap_pfn_range(0x%X, 0x%X, 0x%X, 0x%X)\n",
                (int)(vma->vm_start), (int)(phys >> PAGE_SHIFT),
                (int)pageSize, (int)pgprot_val(vma->vm_page_prot));
    if (remap_pfn_range(vma, vma->vm_start, phys >> PAGE_SHIFT, pageSize,
                        vma->vm_page_prot))
    {
        printk("remap_pfn_range failed\n");
        return 1;
    }

    prestera_vma_open(vma);
    return 0;
}


/************************************************************************
 *
 * prestera_open: The device open() implementation
 */
static int prestera_open(struct inode * inode, struct file * filp)
{
    if (down_interruptible(&prestera_dev->sem))
    {
        return -ERESTARTSYS;
    }

    if (!prestera_initialized)
    {
        up(&prestera_dev->sem);
        return -EIO;
    }

    if (prestera_opened)
    {
        up(&prestera_dev->sem);
        return -EBUSY;
    }
    filp->private_data = prestera_dev;

    prestera_opened++;
    up(&prestera_dev->sem);

    DBG_PRINTK(("presteraDrv: prestera device opened successfuly\n"));

    return 0;
}


/************************************************************************
 *
 * prestera_release: The device close() implementation
 */
static int prestera_release(struct inode * inode, struct file * file)
{
    printk(KERN_DEBUG"prestera_release\n");
    prestera_opened--;
    return 0;
}

/************************************************************************
 *
 * proc read data rooutine
 */
int prestera_read_proc_mem(char * page, char **start, off_t offset, int count, int *eof, void *data)
{
    int len;
    struct pci_dev *dev;
    struct PP_Dev *ppdev;
    unsigned long address, value;
    volatile unsigned long *ptr;
    int i, j;
    extern int short_bh_count;
    
    len = 0;
    len += sprintf(page+len,"short_bh_count %d\n", short_bh_count);
    for (i = 0; i < founddevs; i++)
    {
        dev = pcidevs[i];
        ppdev = ppdevs[i];
        len += sprintf(page+len,"Device %d\n", i);
        len += sprintf(page+len,"\tbus(0x%0lx) slot(0x%0x) func(0x%0lx)\n",(unsigned long)dev->bus, PCI_SLOT(dev->devfn), (unsigned long)PCI_FUNC(dev->devfn));
        len += sprintf(page+len,"\tvendor_id(0x%0x) device_id(0x%0x)\n",dev->vendor, dev->device);
        len += sprintf(page+len,"\tirq(0x%0x)\n",dev->irq);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,14)
        len += sprintf(page+len,"\tname(%s)\n",pci_pretty_name(dev));
#else
        len += sprintf(page+len,"\tname(%s)\n",pci_name(dev));
#endif
        for (j = 0; j < DEVICE_COUNT_COMPATIBLE; j++)
        {
            len += sprintf(page+len,"\t\t%d start(0x%0lx) end(0x%0lx)\n",j, dev->resource[j].start, dev->resource[j].end);
        }
        address = ppdev->config.base;
        len += sprintf(page+len,"\tconfig map to (0x%0lx)\n", address);
        for (j = 0; j < 0x14; j += 4)
        {
            ptr = (volatile unsigned long *) address;
            value = *ptr;
            GT_SYNC;
            len += sprintf(page+len,"\t\toff(0x%0x) add(0x%0lx) val(0x%0x)\n", j, (unsigned long)ptr, le32_to_cpu(value));
            address += 4;
        }
        /* Mask */
        address = ppdev->config.base + 0x118;
        ptr = (volatile unsigned long *) address;
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\tMASK(0x%0x) add(0x%0lx) val(0x%0x)\n", 0x118, (unsigned long)ptr, le32_to_cpu(value));

        /* Cause */
        address = ppdev->config.base + 0x114;
        ptr = (volatile unsigned long *) address;
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\tMASK(0x%0x) add(0x%0lx) val(0x%0x)\n", 0x114, (unsigned long)ptr, le32_to_cpu(value));

        address = ppdev->ppregs.base;
        len += sprintf(page+len,"\tregs map to (0x%0lx)\n", address);

        ptr = (unsigned long *) (address + 0x0);
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\t\toff(0x%0x) val(0x%0x)\n", 0x0, le32_to_cpu(value));

        ptr = (unsigned long *) (address + 0x1000000);
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\t\toff(0x%0x) val(0x%0x)\n", 0x1000000, le32_to_cpu(value));
        
        ptr = (unsigned long *) (address + 0x2000000);
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\t\toff(0x%0x) val(0x%0x)\n", 0x2000000, le32_to_cpu(value));
        
        ptr = (unsigned long *) (address + 0x3000000);
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\t\toff(0x%0x) val(0x%0x)\n", 0x3000000, le32_to_cpu(value));
    } 

    *eof = 1;

    return len;
}

/**
  * Setup prestera port structure for port MIB read
  */
static int prestera_port_mib_init
(
	void
)
{
    struct PP_Dev   *ppdev = NULL;
	struct prestera_port *ports = NULL;
	struct port_mib_info *mib = NULL;
	unsigned int instances = 0; /* total port count */
	int i = 0, j = 0;
	unsigned char devNum = 0,initPort = 0;

	if(!founddevs) 
		return -EINVAL;

	/**
	  *  phase 1: setup global prestera device port info descriptor
	  */
	presteraPorts = (void *)kmalloc(sizeof(void *)*founddevs,GFP_KERNEL);
    if (NULL == presteraPorts)
    {
        printk("kmalloc prestera device port descriptor failed\n");
        goto error;
    }
	memset(presteraPorts,0,sizeof(void*)*founddevs);

	/**
	  *	phase 2: setup individual device ports
	  */
    for (i = 0; i < founddevs; i++)
    {
        ppdev = ppdevs[i];
		switch(ppdev->device_type) 
		{
			default:
				instances = 0;
				break;
				
            case 0xdd74:   /* zhangdi add for test!!!!  */
				printk("zhangdi add 24 + 4 + 3\n");

                instances = 24 + 4 + 3; /* 24 Giga ports + 4 Hyper.G ports + 3 CPU port */
                devNum = i;
                initPort = 0;
                break;
			
			case CPSS_98DX275_CNS:
				instances = 24 + 3 + 1; /* 24 Giga ports + 3 Hyper.G ports + 1 CPU port */
				devNum = i;
				initPort = 0;
				break;
			case CPSS_98DX804_CNS:
				instances = 0 + 4 + 1;  /* 0 Giga port + 4 Hyper.G ports + 1 CPU port */
				devNum = i;
				initPort = 24;
				break;
			case CPSS_98DX265_CNS:
				instances = 24 + 2 + 1; /* 24 Giga ports + 2 Hyper.G ports + 1 CPU port */
				devNum = i;
				initPort = 0;
				break;
			case CPSS_98DX260_CNS:
			case CPSS_98DX263_CNS:
				instances = 24 + 2 + 1; /* 24 Giga ports + 2 Hyper.G ports + 1 CPU port */
				devNum = i;
				initPort = 0;
                break;
            case CPSS_98CX8248_CNS:
                printk("prestera_port_mib_init(): case Lion\n");
				instances = 24 + 1; /* 24  Hyper.G ports + 1 CPU port */
				devNum = i;
				initPort = 0;
                break;
		}
		if(!instances) /* no ports or unrecognized pp deivces */  
			continue;

		/**
		  *	phase 2 step 1: setup all ports' descriptors
		  */
		ports = (struct prestera_port *)kmalloc(sizeof(struct prestera_port), GFP_KERNEL);
		if(NULL == ports) 
		{
			printk("kmalloc prestera device port container failed\n");
			goto error;
		}
		/* code optimize: Wrong sizeof argument. zhangdi@autelan.com 2013-01-18 */
		memset(ports,0,sizeof(struct prestera_port));
		presteraPorts[i] = ports;

		ports->mib = (void *)kmalloc(sizeof(void*)*instances, GFP_KERNEL);
		if(NULL == ports->mib) 
		{
			printk("kmalloc prestera port mib descriptor failed\n");
			goto error;
		}
		memset(ports->mib,0,sizeof(void*)*instances);
		ports->count  	 = instances;
		ports->devNum 	 = devNum; 
		ports->startPort = initPort;

		/**
		  *  	phase 2 step 2: setup all ports' containors
		  */
		for(j = 0; j < instances; j++) 
		{
			mib = (struct port_mib_info*)kmalloc(sizeof(struct port_mib_info), GFP_KERNEL);
			if(NULL == mib) 
			{
				printk("kmalloc prestera port mib container failed\n");
				goto error;
			}
			memset(mib,0,sizeof(struct port_mib_info));
			ports->mib[j] = mib;
		}		
    }

	return 0;
	
error:
	if(!presteraPorts) return -ENOMEM;
	
	for(i = 0; i < founddevs; i++) 
	{
		ports = presteraPorts[i];
		if(!ports)
			continue;
		if(ports->mib)
		{
			for(j = 0; j < ports->count; j++) 
			{
				mib = ports->mib[j];
				if(mib) 
				{
					kfree(mib);
					ports->mib[j] = NULL;
				}
			}			
		}
		if(ports->mib) { kfree(ports->mib); ports->mib = NULL; }
		if(ports) { kfree(ports); ports = NULL; }
		presteraPorts[i] = NULL;
	}
	kfree(presteraPorts);
	presteraPorts = NULL;
	return -ENOMEM;
}

static int pp_find_all_devices(void)
{
    struct pci_dev  *dev;
    struct PP_Dev   *ppdev;
    int             type, i,j;
    unsigned long   start; 
    unsigned long   len;
    unsigned long   data;
    unsigned long   addrCompletion;
	unsigned long	dmaBase;
    
    founddevs   = 0;
    dev_size    = 0;
    type        = 0;
    dev         = NULL;

    while (legalDevTypes[type] != CPSS_DEV_END)
    {
        dev = pci_find_device(legalDevTypes[type] & 0xFFFF,
                              (legalDevTypes[type] & 0xFFFF0000) >> 16, dev);

        if (NULL == dev)
        {
            /* no more devices */
            type++;
            continue;
        }
		printk("\npci find device %#lx name %s\n",(legalDevTypes[type] & 0xFFFF0000) >> 16, \
			(CPSS_98DX255_CNS == legalDevTypes[type]) ? "98Dx255": \
			(CPSS_98DX260_CNS == legalDevTypes[type]) ? "98Dx260 or 98Dx263": \
			(CPSS_98DX263_CNS == legalDevTypes[type]) ? "98Dx263": \
			(CPSS_98DX265_CNS == legalDevTypes[type]) ? "98Dx265": \
			(CPSS_98DX275_CNS == legalDevTypes[type]) ? "98Dx275": \
			(CPSS_98DX285_CNS == legalDevTypes[type]) ? "98Dx285": \
			(CPSS_98DX804_CNS == legalDevTypes[type]) ? "98Dx804": \
			(CPSS_98DX4122_CNS == legalDevTypes[type]) ? "98Dx4122(Xcat)": \
			(CPSS_98CX8248_CNS == legalDevTypes[type]) ? "98Cx8248(Lion)": "unkown");

		printk("\npci find device bus:%d\n",dev->bus->number);
        /* Save PCI device */
        ppdev = (struct PP_Dev *) kmalloc(sizeof(struct PP_Dev), GFP_KERNEL);
        if (NULL == ppdev)
        {
            printk("kmalloc failed\n");
            return -ENOMEM;
        }
        memset(ppdev,0,sizeof(struct PP_Dev));
		
        /* enable the device */
        if(pci_enable_device(dev))
        {
            printk("pci enable device failed \n");
            return -ENODEV;
        }

        /* enable bus mastering */
        pci_set_master(dev);
        
		/* setup dma region */
		dmaBase = (unsigned long)pci_alloc_consistent(dev,DMA_AREA_SIZE,&(ppdev->ppdma.device_base));
    	//dmaBase = (unsigned long)dma_alloc_coherent(&(dev->dev), DMA_AREA_SIZE, &(ppdev->ppdma.device_base), GFP_KERNEL);

		if(0 == dmaBase)
		{
			printk("Cannot alloc dma region for prestera device\r\n");
		}
		else
		{
			/* check dma area */
			ppdev->ppdma.base = (unsigned long *)ioremap_nocache(dmaBase, DMA_AREA_SIZE);
		}
        printk("dmaBase:0x%p\n",dmaBase);
        printk("ppdev->ppdma.device_base:%p\n",ppdev->ppdma.device_base);
        printk("ppdev->ppdma.base:%p\n",ppdev->ppdma.base);
		printk("setup pci dma region base %#lx, asic view base %#lx\r\n",dmaBase,ppdev->ppdma.device_base);
        printk("funddevs %d dev->irq %d dev->devfn %#x\n",founddevs,dev->irq,dev->devfn);
        
		ppdev->device_type = legalDevTypes[type];
        ppdevs[founddevs] = ppdev;
        pcidevs[founddevs] = dev;
        founddevs++;
    }

	/* Swap 2 lion on AX81-SMU for L3 vlan. lion1-bus:3 4 5 6, lion0-bus: 9 10 11 12 */
	if(board_type_mvpp==BOARD_TYPE_AX81_SMU)
	{
        struct PP_Dev *ppdevs_swap[4];
        static struct pci_dev *pcidevs_swap[4];
       
		ppdevs_swap[0]=ppdevs[0];
		pcidevs_swap[0]=pcidevs[0];		
		ppdevs_swap[1]=ppdevs[1];
		pcidevs_swap[1]=pcidevs[1];
		ppdevs_swap[2]=ppdevs[2];
		pcidevs_swap[2]=pcidevs[2];
		ppdevs_swap[3]=ppdevs[3];
		pcidevs_swap[3]=pcidevs[3];

		ppdevs[0]=ppdevs[4];
		pcidevs[0]=pcidevs[4];		
		ppdevs[1]=ppdevs[5];
		pcidevs[1]=pcidevs[5];
		ppdevs[2]=ppdevs[6];
		pcidevs[2]=pcidevs[6];
		ppdevs[3]=ppdevs[7];
		pcidevs[3]=pcidevs[7];

		ppdevs[4]=ppdevs_swap[0];
		pcidevs[4]=pcidevs_swap[0];		
		ppdevs[5]=ppdevs_swap[1];
		pcidevs[5]=pcidevs_swap[1];
		ppdevs[6]=ppdevs_swap[2];
		pcidevs[6]=pcidevs_swap[2];
		ppdevs[7]=ppdevs_swap[3];
		pcidevs[7]=pcidevs_swap[3];	
	}
	
    printk(KERN_DEBUG "\nfounddevs = %d\n", founddevs);
    #if 1 
    dev = pcidevs[0];
    printk("**************************************\n");
    printk("*** BAR0 name=%s\n", (dev)->resource[(0)].name);
    printk("*** BAR0 start=0x%lx\n", pci_resource_start(dev, 0));
    printk("*** BAR0   end=0x%lx\n", pci_resource_end(dev, 0));
    printk("*** BAR0 flags=0x%lx\n", pci_resource_flags(dev, 0));
    printk("*** BAR0   len=0x%lx\n", pci_resource_len(dev, 0));
    printk("*** BAR1 start=0x%lx\n", pci_resource_start(dev, 1));
    printk("*** BAR1   end=0x%lx\n", pci_resource_end(dev, 1));
    printk("*** BAR1 flags=0x%lx\n", pci_resource_flags(dev, 1));
    printk("*** BAR1   len=0x%lx\n", pci_resource_len(dev, 1));
    printk("*** BAR2 start=0x%lx\n", pci_resource_start(dev, 2));
    printk("*** BAR2   end=0x%lx\n", pci_resource_end(dev, 2));
    printk("*** BAR2 flags=0x%lx\n", pci_resource_flags(dev, 2));
    printk("*** BAR2   len=0x%lx\n", pci_resource_len(dev, 2));
    printk("**************************************\n");
    #endif
    /*****************Xcat*****************
        *** BAR0 name=0000:00:00.0
        *** BAR0 start=0x11b00F4000000
        *** BAR0  end=0x11B00F40FFFFF
        *** BAR0 flags=0x120c
        *** BAR0   len=0x100000
        *** BAR1 start=0x0
        *** BAR1   end=0x0
        *** BAR1 flags=0x0
        *** BAR1   len=0x0
        *** BAR2 start=0x11B00F0000000
        *** BAR2  end=0x11B00F3FFFFFF
        *** BAR2 flags=0x120c
        *** BAR2   len=0x4000000
        **************************************/

    /* Map registers space */
    for (i = 0; i < founddevs; i++)
    {
        struct quirks *quirks;

        dev   = pcidevs[i];
        ppdev = ppdevs[i];
        quirks = prestera_find_quirks(dev);

        #if 1   /*cpss1.3*/
        start = pci_resource_start(dev, quirks->regs_resource);
        len   = pci_resource_len(dev, quirks->regs_resource);
        #endif
        #if 0   /*for xcat*/
        start = pci_resource_start(dev, 2);
        len   = pci_resource_len(dev, 2);
        #endif
        printk("start=0x%lx\n", start);
        printk("len=%x\n", len);

        /* Reserve BAR1 region for normal IO access */
        if (!request_mem_region(start, len, "prestera")) 
        {
            printk("Cannot reserve MMI0 region (prestera_mem_reg) %#0lx @ %#0lx\n",
                    start, len);
            return -ENODEV;
        }
		
		//printk("device %d BAR1 reserve region base %#0lx size %#0lx\n",i,start, len);

        /* Find registers space size */
        ppdev->ppregs.size = len;

        /* Map rgisters space to kernel virtual address space */
        ppdev->ppregs.base = (unsigned long)ioremap_nocache(start, len);
        dev_size += len;
		//printk("device %d BAR1 region base remap %#0lx size %#0lx\n",i,ppdev->ppregs.base,len);
		data = *((unsigned long *)ppdev->ppregs.base+0x50);
		printk("device %d BAR2 region base remap %#0lx size %#0lx data %#0lx\n",i,ppdev->ppregs.base+0x50,len,data);

        if (!quirks->is_pex)
        {
            /* Default configuration for address completion */
            data = *((unsigned long *)ppdev->ppregs.base);

            addrCompletion = cpu_to_le32(0x01000100);

            *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
            GT_SYNC;
            *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
            GT_SYNC;
            *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
            GT_SYNC;
            *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
            GT_SYNC;
        }
        
        data = *((unsigned long *)ppdev->ppregs.base+0x50);
		printk("BAR1 test read %#0lx data %#0lx \n",ppdev->ppregs.base+0x50,data);
        for(j=0; j<0x7c; j=j+4)
        {
            data = *((unsigned int *)(ppdev->ppregs.base+j));
		    printk("BAR1 test read %#0lx data %#0lx \n",ppdev->ppregs.base+j,data);
        }
    }

    /* Map config space */
    for (i = 0; i < founddevs; i++)
    {
        struct quirks *quirks;

        dev   = pcidevs[i];
        ppdev = ppdevs[i];
        quirks = prestera_find_quirks(dev);
        
        start = pci_resource_start(dev, PCI_RESOURCE_PSS_CONFIG);
        len   = pci_resource_len(dev, PCI_RESOURCE_PSS_CONFIG);
        printk("start=0x%lx\n", start);
        printk("len=%x\n", len);

        /* Reserve BAR0 region for extended PCI configuration */ 
        if (!request_mem_region(start, len, "prestera")) 
        {
            printk("Cannot reserve MMIO region (prestera_ext_pci) %#0lx @ %#0lx\n",
                    start, len);
            return -ENODEV;
        }
        //data = *((unsigned long *)ppdev->config.base);
		printk("device %d BAR0 reserve region base %#0lx size %#0lx\n",i,start, len);

        /* skip quirks->config_offset (0x70000) in this region */
        if (len > quirks->config_offset) /* be paranoid */
        {
            start += quirks->config_offset;
            len -= quirks->config_offset;
        }

        /* Find config space size */
        ppdev->config.size = len;

        /* Map config space to kernel virtual address space */
        ppdev->config.base = (unsigned long)ioremap_nocache(start, len);
        dev_size += len;
		printk("device %d BAR0 region base remap %#0lx size %#0lx\n",i,ppdev->config.base,len);
        data = *((unsigned long *)ppdev->config.base);
		printk("BAR0 test read %#0lx data %#0lx \n",ppdev->config.base,data);
		/* jump the following code, zhangdi@autelan.com 2012-01-07 */
		/* to avoid "ERROR PEXP_NPEI_INT_SUM[C0_SE]: System Error, RC Mode Only." */
		printk("sdk2.2 BAR0 test read jump -ERROR PEXP_NPEI_INT_SUM[C0_SE]: System Error, RC Mode Only\n");        
		#if 0
        for(j=0; j<0x7c; j=j+4)
        {
            data = *((unsigned int *)(ppdev->config.base+j));
		    printk("BAR0 test read %#0lx data %#0lx \n",ppdev->config.base+j,data);
        }
		#endif
    }

    #if 1   /* zhangdi open form 'if 0' */
	if(prestera_port_mib_init())
	{
		printk("initialize port mib error\n");
		return -ENOMEM;
	}
    #endif
	
    return 0;
}


static struct file_operations prestera_fops =
{
    .llseek = prestera_lseek,
    .read   = prestera_read,
    .write  = prestera_write,
    .ioctl  = prestera_ioctl,
    .compat_ioctl = prestera_compat_ioctl,
    .mmap   = prestera_mmap,
    .open   = prestera_open,
    .release= prestera_release /* A.K.A close */
};

/************************************************************************
 *
 * prestera_cleanup: 
 */
static void prestera_cleanup(void)
{
    int i,j;
    struct PP_Dev *ppdev;
    struct pci_dev *dev;
	struct intData *cookieIsr = NULL; /* PP interrupt info */
	struct port_mib_info *mib = NULL;
	struct prestera_port *ports = NULL;

    printk("Prestera Says: Bye world from kernel\n");

    prestera_initialized = 0;

    for (i = 0; i < founddevs; i++)
    {
        struct quirks *quirks;

        ppdev = ppdevs[i];
        dev   = pcidevs[i];
        quirks = prestera_find_quirks(dev);
        
        /* Unmap the memory regions */
        iounmap((void *)ppdev->config.base);
        iounmap((void *)ppdev->ppregs.base);
        
        /* relaese BAR0 */
        release_mem_region(pci_resource_start(dev, PCI_RESOURCE_PSS_CONFIG),
                           pci_resource_len(dev, PCI_RESOURCE_PSS_CONFIG));
        #if 1   /*cpss1.3*/
        /* relaese BAR1 */
        release_mem_region(pci_resource_start(dev, quirks->regs_resource),
                           pci_resource_len(dev, quirks->regs_resource));
        #endif
        #if 0   /*for xcat*/
        /* relaese BAR2 */
        release_mem_region(pci_resource_start(dev, 2),
                           pci_resource_len(dev, 2));
        #endif
        
		/* Unmap the dma memory region */
		iounmap((void *)ppdev->ppdma.base);
        
        pci_disable_device(dev);

		/* free irq so that no interrupt reported */
		cookieIsr = ppdev->cookie;
		
		if((cookieIsr) && (cookieIsr->tasklet)) /* has bound ISR to irq before */
		{
			free_irq(cookieIsr->intVec,(void*)cookieIsr->tasklet);
			printk("release IRQ-%-d for device %s\n",cookieIsr->intVec,  \
				(CPSS_98DX260_CNS == ppdev->device_type) ? "98Dx260": \
				(CPSS_98DX263_CNS == ppdev->device_type) ? "98Dx263": \
				(CPSS_98DX265_CNS == ppdev->device_type) ? "98Dx265": \
				(CPSS_98DX275_CNS == ppdev->device_type) ? "98Dx275": \
				(CPSS_98DX804_CNS == ppdev->device_type) ? "98Dx804":"other");
		}
    }
    founddevs = 0;
#if 0
    /* unmap dma area */
    iounmap((void *)dma_area);
    release_mem_region(dma_base, dma_len);
#endif    
    remove_proc_entry("mvPP", presteraDir);

	remove_proc_entry("show_stats",presteraDir);

	remove_proc_entry("clear_stats",presteraDir);

	remove_proc_entry("read_reg",presteraDir);

	remove_proc_entry("write_reg",presteraDir);

	remove_proc_entry("cpld_read",presteraDir);

	remove_proc_entry("cpld_write",presteraDir);	

	remove_proc_entry("phy_read",presteraDir);	

	remove_proc_entry("phy_write",presteraDir);	

	remove_proc_entry("prestera",NULL);
	
    unregister_chrdev_region(MKDEV(prestera_major, 0), 1);

	cdev_del(&prestera_cdev);

	del_timer(&prestera_mib_timer);
	
	if(presteraPorts)
	{
		for(i = 0; i < founddevs; i++) 
		{
			ports = presteraPorts[i];
			if(!ports)
				continue;
			if(ports->mib)
			{
				for(j = 0; j < ports->count; j++) 
				{
					mib = ports->mib[j];
					if(mib) 
					{
						kfree(mib);
						ports->mib[j] = NULL;
					}
				}			
			}
			if(ports->mib) { kfree(ports->mib); ports->mib = NULL; }
			if(ports) { kfree(ports); ports = NULL; }
			presteraPorts[i] = NULL;
		}
		kfree(presteraPorts);
		presteraPorts = NULL;
	}
}

#ifndef __AX_PLATFORM__
unsigned int mibGeRegBaseArr[] = {		\
	0x04010000,/* Port<0-5> */	\
	0x04810000,/* Port<6-11> */ \
	0x05010000,/* Port<12-17>*/ \
	0x05810000 /* Port<18-23>*/ \
};
unsigned int mibXgRegBaseArr[] = { 		\
	0x01C00000,/* Port<24> */	\
	0x01C40000,/* Port<25> */	\
	0x01C80000,/* Port<26> */	\
	0x01DC0000,/* Port<27> */	\
};

unsigned int mibRegOffset[] = { 			\
	0x10,/* Port<n> Rx Ucast Pkt */ 			\
	0x40,/* Port<n> Tx Ucast Pkt */ 			\
	0x6C,/* Port<n> Jabber */					\
	0x18,/* Port<n> Rx Bcast Pkt */ 			\
	0x4C,/* Port<n> Tx Bcast Pkt */ 			\
	0x60,/* Port<n> Undersize */				\
	0x1C,/* Port<n> Rx Mcast Pkt */ 			\
	0x48,/* Port<n> Tx Mcast Pkt */ 			\
	0x68,/* Port<n> Oversize */ 				\
	0x70,/* Port<n> Rx Error  Pkt */			\
	0x0C,/* Port<n> Tx CRC Error */ 			\
	0x74,/* Port<n> BadCRC	*/				\
	0x5C,/* Port<n> ReceivedFIFO overrun */ 	\
	0x64 /* Port<n> Fragments */				\
}; /* register address */

char *mibRegDesc[] = { 	\
	"RxUcast",		\
	"TxUcast",		\
	"Jabber",		\
	"RxBcast",		\
	"TxBcast",		\
	"Undersize",	\
	"RxMcast",		\
	"TxMcast",		\
	"Oversize", 	\
	"RxError",		\
	"TxCRCError",	\
	"BadCRC",		\
	"RxFifoOverrun", \
	"Fragment"
}; /* register address description */

unsigned char mibRegCount = sizeof(mibRegOffset)/sizeof(unsigned int);

unsigned int mibCpuRegBase = 0x00000060;

unsigned int mibCpuRegOffset[] = { \
	0x10,/* Rx Good Pkt */ 			\
	0x0 ,/* Tx Good Pkt */ 			\
	0x18,/* Rx Good Byte */ 			\
	0x8 ,/* Tx Good Byte */			\
	0x14,/* Rx Bad Pkt */				\
	0x4 ,/* Tx MAC Trans Error Pkt */ 	\
	0x1C,/* Rx Bad Byte */ 			\
	0xC  /* Rx Drop Pkt */ 			\
}; /* register address */

char *mibCpuRegDesc[] = { 	\
	"RxGoodPkt",		\
	"TxGoodPkt",		\
	"RxGoodByte",		\
	"TxGoodByte",		\
	"RxBadPkt",			\
	"TxMACErrPkt",		\
	"RxBadByte",		\
	"RxDropPkt"			\
}; /* register address description */
unsigned char mibCpuRegCount = sizeof(mibCpuRegOffset)/sizeof(unsigned int);

/* CPLD read write base address */
unsigned long cpld_rw_base = 0x800000001D010000;

/* port number to PHY address map table */
static unsigned char phyPortAddrMapAx7005[][2] = {
	/* {virtual port ,phy port} */
	/* SLOT 1 SMI1 */
	{23,0x0 },{22,0x1 },{21,0x2 },{20,0x3 },{19,0x4 },{18,0x5 },
	/* SLOT 2 SMI0 */
	{11,0x0 },{10,0x1 },{9 ,0x2 },{8 ,0x3 },{7 ,0x4 },{6 ,0x5 },	
	/* SLOT 3 SMI1 */	
	{17,0x10},{16,0x11},{15,0x12},{14,0x13},{13,0x14},{12,0x15},
	/* SLOT 4 SMI0 */	
	{5 ,0x10},{4 ,0x11},{3 ,0x12},{2 ,0x13},{1 ,0x14},{0 ,0x15}
};
static unsigned char phyPortAddrMapAu4624[][2] = {
	/* {virtual port ,phy port} */
	/* SMI0 */
	{0 ,0x0 },{1 ,0x1 },{2 ,0x2 },{3 ,0x3 },{4 ,0x4 },{5 ,0x5 },
	{6 ,0x6 },{7 ,0x7 },{8 ,0x8 },{9 ,0x9 },{10,0xA },{11,0xB },
	/* SMI1 */	
	{12,0x0 },{13,0x1 },{14,0x2 },{15,0x3 },{16,0x4 },{17,0x5 },
	{18,0x6 },{19,0x7 },{20,0x8 },{21,0x9 },{22,0xA },{23,0xB }
};

#define BYTESWAP(data)                      \
        (((data) << 24))  |                 \
        (((data) & 0xff00)      << 8)   |   \
        (((data) & 0xff0000)    >> 8)   |   \
        (((data) >> 24))
#define ADDRESS_COMPLETION_OFFSET 0

/*******************************************************************************
* prestera_register_read
*
* DESCRIPTION:
*       This is the Prestera Routine Read HW registers in kernel space 
*
* INPUTS:
*       devNum		- the device number
*       regAddr  	- the register address to read
*
* OUTPUTS:
*       data		-register data to get.
*
* RETURNS:
*	  NULL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void prestera_register_read
(
	unsigned char devNum,
	unsigned int  regAddr,
	unsigned int  *data
)
{
	struct PP_Dev *device = NULL;
	unsigned long baseAddr  = 0UL,address = 0UL;
	unsigned int value  = 0,writeData = 0;
	unsigned char addrRegion = 0;
	static unsigned char compIdx = 3;
	
	if(devNum >= founddevs) return;
	
	device = ppdevs[devNum];
	if(NULL == device)  return;

	baseAddr = device->ppregs.base;
	
	/* Address Region Selection */
	if(0 != (regAddr & 0xFF000000))
	{
		/* First select region and write address completion register */
		addrRegion = (unsigned char)(regAddr >> 24);
		compIdx = (compIdx - 1)%2 + 2;
		writeData = addrRegion << (compIdx * 8);
		writeData = (BYTESWAP(writeData));
		//printk("<3>address completion write before swap %#0x,compIdx %d\n",writeData,compIdx);
		address = baseAddr + ADDRESS_COMPLETION_OFFSET;
		*(volatile unsigned int *)address = writeData;
		GT_SYNC;
		*(volatile unsigned int *)address = writeData;
		GT_SYNC;
		*(volatile unsigned int *)address = writeData;
		GT_SYNC;
		/* read after write */
		value = *(volatile unsigned int*)address;
		value = (BYTESWAP(value));
		if((BYTESWAP(writeData)) != value)
		{
			printk("<3>address completion write error,write %#0x get %#0x\n",writeData,value);
			return;
		}
		else
		{
			//printk("<3>address completion write %#0x ok\n",writeData);				
		}

		/* read registers you want */
		address = baseAddr + ((compIdx << 24)|(regAddr & 0x00FFFFFF));
		value = *(volatile unsigned int *)address;
		value = (BYTESWAP(value));
	}
	else/* Address Region 0 */
	{
		address = baseAddr + (regAddr & 0x00FFFFFF);
		value = *(volatile unsigned int*)address;
		value = (BYTESWAP(value));
	}
	//printk("ADDR %#0x :%#0x\n",regAddr,value);	
	*data = value;
	return;
}

/*******************************************************************************
* prestera_register_read
*
* DESCRIPTION:
*       This is the Prestera Routine write register value to HW registers in kernel space 
*
* INPUTS:
*       devNum	- the device number
*       regAddr  	- the register address to write
*	   data 		- register value to write
*
* OUTPUTS:
*	  NULL
*
* RETURNS:
*	  0 - if write successfully
*	  data - data read back after write
*
* COMMENTS:
*       None.
*
*******************************************************************************/
unsigned int prestera_register_write
(
	unsigned char devNum,
	unsigned int  regAddr,
	unsigned int  data
)
{
	struct PP_Dev *device = NULL;
	unsigned long baseAddr  = 0UL,address = 0UL;
	unsigned int value  = 0,writeData = 0;
	unsigned char addrRegion = 0;
	static unsigned char compIdx = 3;
	
	if(devNum >= founddevs) return -1;
	
	device = ppdevs[devNum];
	if(NULL == device)  return -1;

	baseAddr = device->ppregs.base;
	
	/* Address Region Selection */
	if(0 != (regAddr & 0xFF000000))
	{
		/* First select region and write address completion register */
		addrRegion = (unsigned char)(regAddr >> 24);
		compIdx = (compIdx - 1)%2 + 2;
		writeData = addrRegion << (compIdx * 8);
		writeData = (BYTESWAP(writeData));
		//printk("<3>address completion write before swap %#0x,compIdx %d\n",writeData,compIdx);
		address = baseAddr + ADDRESS_COMPLETION_OFFSET;
		*(volatile unsigned int *)address = writeData;
		GT_SYNC;
		*(volatile unsigned int *)address = writeData;
		GT_SYNC;
		*(volatile unsigned int *)address = writeData;
		GT_SYNC;
		/* read after write */
		value = *(volatile unsigned int*)address;
		value = (BYTESWAP(value));
		if((BYTESWAP(writeData)) != value)
		{
			printk("address completion write error,write %#0x get %#0x\n",writeData,value);
			return -1;
		}

		/* write to registers you want */
		address = baseAddr + ((compIdx << 24)|(regAddr & 0x00FFFFFF));
		data = (BYTESWAP(data));
		*(volatile unsigned int *)address = data;
		GT_SYNC;
		*(volatile unsigned int *)address = data;
		GT_SYNC;
		*(volatile unsigned int *)address = data;
		GT_SYNC;
		/* read after write */
		value = *(volatile unsigned int*)address;
		value = (BYTESWAP(value));
		if((BYTESWAP(data)) != value)
		{
			printk("register write(select region) value %#x read back %#x,not match!\n",data,value);
			return value;
		}
	}
	else/* Address Region 0 */
	{
		address = baseAddr + (regAddr & 0x00FFFFFF);
		data = (BYTESWAP(data));
		*(volatile unsigned int*)address = data;
		GT_SYNC;
		*(volatile unsigned int*)address = data;
		GT_SYNC;
		*(volatile unsigned int*)address = data;
		GT_SYNC;
		/* read after write */
		value = *(volatile unsigned int*)address;
		value = (BYTESWAP(value));
		if((BYTESWAP(data)) != value)
		{
			printk("register write(region 0) value %#x read back %#x,not match!\n",data,value);
			return value;
		}
	}
	
	return 0;
}

/*******************************************************************************
*  prestera_port_stat_get
*
* DESCRIPTION:
*       This routine get port MAC MIB info
*
* INPUTS:
*       devNum	- prestera device number
*       portNum  	- prestera port number 
*	  buffer 		- buffer for print
* 	  buffLen 	- buffer length
*
* OUTPUTS:
*       buffLen 	- actually used buffer length
*
* RETURNS:
*	  NULL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void prestera_port_stat_get
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char *buffer,
	unsigned int *buffLen
)
{
	unsigned int length = 0;
	unsigned char *buffPtr = NULL, portIdx = 0;
	struct prestera_port *ports = NULL;
	struct port_mib_info *mib = NULL;

	if(!buffer) 
	{
		*buffLen = length;
		return;
	}

	buffPtr = buffer;
	if(!presteraPorts) /* statistic info null */
	{
		length += sprintf(buffPtr,"no port mib statistic ready!\n");
		buffPtr = buffer + length;
		*buffLen = length;
		return;
	}

	ports = presteraPorts[devNum];
	if(!ports) /* device associated statistic info null */
	{
		length += sprintf(buffPtr,"no ports mib statistic for device %d\n",devNum);
		buffPtr = buffer + length;
		*buffLen = length;
		return;
	}

	if(ports->devNum != devNum) /* device number not match up */
	{
		length += sprintf(buffPtr,"device number %d not match with SW\n",devNum);
		buffPtr = buffer + length;
		*buffLen = length;
		return;
	}

	/**
	  * 	CPU port is located at the end of the port array 
	  *   other ports located at position: (port_number - start_number)
	  */
	if((portNum != 63) &&		\
		((portNum - ports->startPort) > ports->count)) 
	{
		length += sprintf(buffPtr,"port number %d is illegal\n",portNum);
		buffPtr = buffer + length;
		*buffLen = length;
		return;
	}
	
	if(ports->mib) 
	{
		portIdx = (63 == portNum) ? (ports->count - 1):(portNum - ports->startPort);
		mib = ports->mib[portIdx];
		if(!mib)
		{
			length += sprintf(buffPtr,"Port(%-d,%-d) mib null,skip\n",devNum,portNum);
			buffPtr = buffer + length;
			*buffLen = length;
			return;
		}
		if(63 == portNum) 
		{			
			length += sprintf(buffPtr,"CPU Port(%-d,%-d):\n\t",devNum,portNum);
			buffPtr = buffer + length;		
			length += sprintf(buffPtr,"%-11s %-14lu","RxGoodPkt",mib->RxUcast);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","TxGoodPkt",mib->TxUcast);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu\n\t","RxGoodByte",mib->RxGoodBytesL32);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","TxGoodByte",mib->TxGoodBytesL32);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","RxBadPkt",mib->RxFifoOverrun);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu\n\t","TxMACErrPkt",mib->TxCrcError);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","RxBadByte",mib->RxBadBytes);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","RxDropPkt",mib->RxError);
			buffPtr = buffer + length;
		}
		else 
		{
			length += sprintf(buffPtr,"Port(%-d,%-d):\n\t",devNum,portNum);
			buffPtr = buffer + length;		
			length += sprintf(buffPtr,"%-11s %-14lu","RxUcast",mib->RxUcast);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","TxUcast",mib->TxUcast);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu\n\t","FifoOverRun",mib->RxFifoOverrun);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","RxBcast",mib->RxBcast);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","TxBcast",mib->TxBcast);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu\n\t","Undersized",mib->Undersized);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","RxMcast",mib->RxMcast);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","TxMcast",mib->TxMcast);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu\n\t","Oversized",mib->Oversized);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","RxBytesL32",mib->RxGoodBytesL32);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","TxBytesL32",mib->TxGoodBytesL32);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu\n\t","Fragmented",mib->Fragmented);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","RxBytesH32",mib->RxGoodBytesH32);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","TxBytesH32",mib->TxGoodBytesH32);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu\n\t","Jabber",mib->Jabber);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","RxBadBytes",mib->RxBadBytes);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","TxCrcError",mib->TxCrcError);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu\n\t","BadCrcEvt",mib->BadCrc);
			buffPtr = buffer + length;
			length += sprintf(buffPtr,"%-11s %-14lu","RxError",mib->RxError);
			buffPtr = buffer + length;
		}
	}
	
	*buffLen = length;
	return;
}

/*******************************************************************************
*  prestera_port_mac_mib_read
*
* DESCRIPTION:
*       This routine get port MAC MIB info
*
* INPUTS:
*       devNum	- prestera device number
*       portNum  	- prestera port number 
*	  buffer 		- buffer for print
* 	  buffLen 	- buffer length
*
* OUTPUTS:
*       buffLen 	- actually used buffer length
*
* RETURNS:
*	  NULL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void prestera_port_mac_mib_read
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned char *buffer,
	unsigned int *buffLen
)
{
	unsigned int data = 0,length = 0;
	unsigned int regBase = 0,regAddr = 0,regMacStatus = 0;
	unsigned char i = 0,*buffPtr = NULL;
	
	if(!buffer) return;

	buffPtr = buffer;

	length += sprintf(buffPtr,"Port(%-d,%-d):\n\t",devNum,portNum);
	buffPtr = buffer + length;
	if(portNum <= 23) { /* Gigabit ports */
		regBase = mibGeRegBaseArr[portNum / 6] + (portNum % 6) * 0x80;
		regMacStatus = 0x0A800010 + portNum * 0x100;
	}
	else if(portNum <= 27) { /* XG ports */
		regBase = mibXgRegBaseArr[portNum % 24];
		regMacStatus = 0x0A80000C + portNum * 0x100;
	}
	else if(portNum == 63) { /* CPU port */
		regBase = 0x00000060;
		regMacStatus = 0x0A803F10;
		prestera_register_read(devNum,regMacStatus,&data);
		if(0 == (data & 0x1)) { /* port MAC is link down */
			length += sprintf(buffPtr,"Link Status down\n");
			*buffLen = length;
			return;
		}
		data = 0;
		for(i = 0; i < mibCpuRegCount; i++) {
			regAddr = regBase + mibCpuRegOffset[i];
			prestera_register_read(devNum,regAddr,&data);
			if(length + 26 > *buffLen) {
				*buffLen = length;
				return;
			}
			length += sprintf(buffPtr,"%-13s %-10d%s",mibCpuRegDesc[i],data,(i + 1) % 2 ? " ":"\n\t");
			buffPtr = buffer + length;
			data = 0;
		}
		*buffLen = length;
		return;
	}
	else {
		*buffLen = 0;
		return;
	}

	prestera_register_read(devNum,regMacStatus,&data);
	if(0 == (data & 0x1)) { /* port MAC is link down */
		length += sprintf(buffPtr,"Link Status DOWN\n");
		*buffLen = length;
		return;
	}

	for(i = 0; i < mibRegCount; i++) {
		regAddr = regBase + mibRegOffset[i];
		prestera_register_read(devNum,regAddr,&data);
		if(length + 26 > *buffLen) {
			*buffLen = length;
			return;
		}
		length += sprintf(buffPtr,"%-13s %-10d%s",mibRegDesc[i],data,(i + 1) % 3 ? " ":"\n\t");
		buffPtr = buffer + length;
	}
	*buffLen = length;
	return;
}

#define PP_DEVICE_NAME_BY_TYPE(type) 	\
	(CPSS_98DX260_CNS == type) ? "98Dx260" :		\
	(CPSS_98DX263_CNS == type) ? "98Dx263" :		\
	(CPSS_98DX265_CNS == type) ? "98Dx265" :		\
	(CPSS_98DX275_CNS == type) ? "98Dx275" :		\
	(CPSS_98DX285_CNS == type) ? "98Dx285" :		\
	(CPSS_98DX804_CNS == type) ? "98Dx804" : "unknown"
	
/*******************************************************************************
*  prestera_device_port_range
*
* DESCRIPTION:
*       This routine get the specified device's start and end port number
*	   device is specified via device type
*
* INPUTS:
*       type	- prestera device type(or device id)
*
* OUTPUTS:
*       start 	- start port number
*	   end 	- end port number
*
* RETURNS:
*	   NULL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void prestera_device_port_range
(
	unsigned int type,
	unsigned int *start,
	unsigned int *end
)
{
	unsigned int tmpStart = 0, tmpEnd = 0;
	
	switch(type) {
		default:
			break;
		case CPSS_98DX260_CNS:
		case CPSS_98DX263_CNS:
		case CPSS_98DX265_CNS:
			tmpStart = 0;
			tmpEnd = 25;
			break;
		case CPSS_98DX275_CNS:
			tmpStart = 0;
			tmpEnd = 26;
			break;
		case CPSS_98DX285_CNS:
			tmpStart = 0;
			tmpEnd = 27;
			break;
		case CPSS_98DX804_CNS:
			tmpStart = 24;
			tmpEnd = 27;
			break;
	}

	*start	= tmpStart;
	*end 	= tmpEnd;
	return;
}
/*******************************************************************************
*  prestera_device_port_check
*
* DESCRIPTION:
*       This routine check if specified device number and port number legal or not.
*
* INPUTS:
*       devNum	- prestera device number
*       portNum  	- prestera port number
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*	   0    - if no error occurred
*	  -1 	  - device not exists or not found by pci scanner
*	  -2 	  - device is not prestera device(device vendor ID not 0x11AB for Marvell Technology)
*	  -3 	  - device is not supported(or unrecognized)
*	  -4   - port number is out of range
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static int prestera_device_port_check
(
	unsigned char devNum,
	unsigned char portNum
)
{
    struct pci_dev  *pciDev = NULL;
	struct PP_Dev *ppDev = NULL;
	unsigned int start = 0, end = 0;
	int device_type = 0;

	ppDev = ppdevs[devNum];
	if(NULL == ppDev) {
		return -1;
	}
	device_type = ppdevs[devNum]->device_type;
	pciDev = pcidevs[devNum];
	if((device_type & 0xFFFF) != pciDev->vendor) {
		return -2;
	}

	prestera_device_port_range(device_type,&start,&end);
	
	if(!start && !end) {
		return -3;
	}
	else if((portNum == 63) || (portNum >= start && portNum <= end)) {
		return 0;
	}
	else {
		return -4;
	}
}

/*******************************************************************************
* prestera_packet_stats_seq_show
*
* DESCRIPTION:
*		/proc/prestera/show_stats was openned. Use the single_open iterator
*
* INPUTS:
*		m  	- 
*		v  	- 
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		None.
*
*******************************************************************************/
static int prestera_packet_stats_seq_show
(
	struct seq_file *m, 
	void *v
)
{
	char usage[] = "\
Usage:\n\
  mv_dev\t0 for 98Dx275\n\
        \t1 for 98Dx804\n\
        \t255 for both devices\n\
  mv_port\t0-26 for 98Dx275\n\
         \t24-26 for 98Dx804\n\
         \t63 for CPU port\n\
         \t255 for all ports\n";
	unsigned int i = 0,j = 0,start = 0,end = 0;
	int result = 0;
	char buffer[1024] = {0};
	unsigned int length = 1024,totalLen = 0;

	/* 
	 * case 1: show specified port statistics 
	 */
	if((0xFF != mv_dev) && (0xFF != mv_port)) {
		result = prestera_device_port_check(mv_dev,mv_port);
		if(-1 == result) { /* device not found */
			seq_printf(m,"no pci device found for given device number %d\n",mv_dev);
		}
		else if(-2 == result) { /* device not supported */
			seq_printf(m,"non-prestera device for given device number %d\n",mv_dev);	
		}
		else if(-3 == result) { /* prestera device not supported */
			seq_printf(m,"unsupported prestera device for given device number %d\n",mv_dev);
		}
		else if(-4 == result) { /* port number out of range */
			seq_printf(m,"device %s with illegal port %d\n",	\
				PP_DEVICE_NAME_BY_TYPE(ppdevs[mv_dev]->device_type),mv_port);
		}
		else if(0 == result) { /* device and port correct */
			prestera_port_stat_get(mv_dev,mv_port,buffer,&length);
			if(length) {
				seq_printf(m,"%s\n",buffer);
				return 0;
			}
		}
	}
	/*
	 * case 2:show all ports statistics on all devices 
	 */
	else if((0xFF == mv_dev) && (0xFF == mv_port)) {
		for(i = 0; i < founddevs; i++) {
			if(ppdevs[i]) {
				prestera_device_port_range(ppdevs[i]->device_type, &start,&end);
			}
			if(!start && !end) {// skip  unsupported device
				seq_printf(m,"skip unsupported prestera device for given device number %d\n",i);
				continue;
			}
			for(j = start;j <= end; j++) {
				prestera_port_stat_get(i,j,buffer,&length);
				if(length) {
					seq_printf(m,"%s\n",buffer);
					totalLen += length;
					length = 1024;
				}
			}
			/* CPU port */
			prestera_port_stat_get(i,63,buffer,&length);
			if(length) {
				seq_printf(m,"%s\n",buffer);
				totalLen += length;
				length = 1024;
			}			
		}
		return 0;
	}
	/*
	 * case 3:show all ports on device(s)
	 */
	else if((0xFF != mv_dev) && (0xFF == mv_port)) {
		if(!ppdevs[mv_dev]) {
			seq_printf(m,"no pci device found for given device number %d\n",mv_dev);
		}
		else {
			if(ppdevs[mv_dev]) {
				prestera_device_port_range(ppdevs[i]->device_type, &start,&end);
			}
			if(!start && !end) // unsupported device
				seq_printf(m,"unsupported prestera device for given device number %d\n",mv_dev);
			else {
				for(j = start;j <= end; j++) {
					prestera_port_stat_get(mv_dev,j,buffer,&length);
					if(length) {
						seq_printf(m,"%s\n",buffer);
						totalLen += length;
						length = 1024;
					}
				}
				/* CPU port */
				prestera_port_stat_get(mv_dev,63,buffer,&length);
				if(length) {
					seq_printf(m,"%s\n",buffer);
					totalLen += length;
				}	
			}
			return 0;
		}
	}
	/* 
	 * case 4:show specified ports on all devices 
	 */
	else if((0xFF == mv_dev) && (0xFF != mv_port)) {
		for(i = 0; i < founddevs; i++) {
			if(ppdevs[i]) {
				prestera_device_port_range(ppdevs[i]->device_type, &start,&end);
			}
			if(!start && !end) {// skip  unsupported device
				seq_printf(m,"skip unsupported prestera device for given device number %d\n",i);
				continue;
			}
			if((63 == mv_port)||((start <= mv_port) && (mv_port <= end))) {
				prestera_port_stat_get(i,mv_port,buffer,&length);
				if(length) {
					seq_printf(m,"%s\n",buffer);
					totalLen += length;
					length = 1024;
				}
			}
		}
		return 0;
	}
	seq_printf(m, "%s", usage);

	return 0;
}

/*******************************************************************************
* prestera_stats_open
*
* DESCRIPTION:
*		/proc/prestera/show_stats was openned. Use the single_open iterator
*
* INPUTS:
*		inode  	- file system iNode structure
*		file	 	- file pointer
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		None.
*
*******************************************************************************/
static int prestera_stats_open
(
	struct inode *inode, 
	struct file *file
)
{
	return single_open(file, prestera_packet_stats_seq_show, NULL);
}


static struct file_operations prestera_stats_operations = {
	.open		= prestera_stats_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

/*******************************************************************************
* prestera_register_value_get
*
* DESCRIPTION:
*		/proc/prestera/read_reg was openned.
*
* INPUTS:
*		page  	- 
*		start  	- 
*		offset	-
*		count	-
*		eof		-
*		data		-
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		This route is hooked as procfs read routine.
*
*******************************************************************************/
int prestera_register_value_get
(
	char * page, 
	char **start, 
	off_t offset, 
	int count, 
	int *eof, 
	void *data
)
{
	char usage[] = "\
Usage:\n\
  mv_dev\t0 for 98Dx275\n\
        \t1 for 98Dx804\n\
        \t255 for both devices\n\
  mv_reg\tall registers in config space\n";

	unsigned char i = 0;
	unsigned int value = 0,len = 0;

	if((mv_dev>=founddevs) && (0xFF != mv_dev)) {
		len += sprintf(page+len,"%s",usage);
	}
	else if(mv_reg<0 || mv_reg > 0x0FFFFFFF) {
		len += sprintf(page+len,"%s",usage);
	}
	else if(0xFF == mv_dev) {
		for(i = 0; i < founddevs ; i++) {
			prestera_register_read(i,mv_reg,&value);
			len += sprintf(page+len,"%-10s:%-8d\n","DEVICE",i);
			len += sprintf(page+len,"%-10s:%#-8x\n","REGISTER",mv_reg);
			len += sprintf(page+len,"%-10s:%#-8x\n","VALUE",value);			
		}
	}
	else {
		prestera_register_read(mv_dev,mv_reg,&value);
		len += sprintf(page+len,"%-10s:%-8d\n","DEVICE",mv_dev);
		len += sprintf(page+len,"%-10s:%#-8x\n","REGISTER",mv_reg);
		len += sprintf(page+len,"%-10s:%#-8x\n","VALUE",value);			
	}

    *eof = 1;

	return len;
}
/*******************************************************************************
* prestera_register_value_set
*
* DESCRIPTION:
*		/proc/prestera/write_reg was openned.
*
* INPUTS:
*		page  	- 
*		start  	- 
*		offset	-
*		count	-
*		eof		-
*		data		-
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		This route is hooked as procfs read routine.
*
*******************************************************************************/
int prestera_register_value_set
(
	char * page, 
	char **start, 
	off_t offset, 
	int count, 
	int *eof, 
	void *data
)
{
	char usage[] = "\
Usage:\n\
  mv_dev\t0 for 98Dx275\n\
        \t1 for 98Dx804\n\
        \t255 for both devices\n\
  mv_reg\tall registers in config space\n";

	unsigned char i = 0;
	unsigned int value = 0,len = 0;

	if((mv_dev>=founddevs) && (0xFF != mv_dev)) {
		len += sprintf(page+len,"%s",usage);
	}
	else if(mv_reg<0 || mv_reg > 0x0FFFFFFF) {
		len += sprintf(page+len,"%s",usage);
	}
	else if(0xFF == mv_dev) {
		for(i = 0; i < founddevs ; i++) {
			value = prestera_register_write(i,mv_reg,mv_data);
			len += sprintf(page+len,"%-10s:%-8d\n","DEVICE",i);
			len += sprintf(page+len,"%-10s:%#-8x\n","REGISTER",mv_reg);
			len += sprintf(page+len,"%-10s:%#-8x\n","VALUE",mv_data);	
			len += sprintf(page+len,"%-10s:%#-8x\n","RETURN",value ? value : mv_data);
		}
	}
	else {
		value = prestera_register_write(mv_dev,mv_reg,mv_data);
		len += sprintf(page+len,"%-10s:%-8d\n","DEVICE",mv_dev);
		len += sprintf(page+len,"%-10s:%#-8x\n","REGISTER",mv_reg);
		len += sprintf(page+len,"%-10s:%#-8x\n","VALUE",value);			
		len += sprintf(page+len,"%-10s:%#-8x\n","RETURN",value ? value : mv_data);
	}

    *eof = 1;

	return len;
}

/*******************************************************************************
*  prestera_timer_stat_get
*
* DESCRIPTION:
*       This routine get port MAC mib counter via specified device number and port number.
*
* INPUTS:
*       devNum	- prestera device number
*       portNum  	- prestera port number
*	   mib 		- port mib information
*
* OUTPUTS:
*       NULL
*
* RETURNS:
*	  NULL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
static void prestera_timer_stat_get
(
	unsigned char devNum,
	unsigned char portNum,
	struct port_mib_info *mib
)
{
	unsigned int regBase = 0,regAddr = 0,regMacStatus = 0;
	unsigned int data = 0;

	if(portNum <= 23) { /* Gigabit ports */
		regBase = mibGeRegBaseArr[portNum / 6] + (portNum % 6) * 0x80;
		regMacStatus = 0x0A800010 + portNum * 0x100;
	}
	else if(portNum <= 27) { /* XG ports */
		regBase = mibXgRegBaseArr[portNum % 24];
		regMacStatus = 0x0A80000C + portNum * 0x100;
	}
	else if(63 == portNum) { /* CPU port */
		regBase = 0x00000060;
		regMacStatus = 0x0A803F10;
	}
	
	prestera_register_read(devNum,regMacStatus,&data);
	if(0 == (data & 0x1)) { /* port MAC is link down */
		return;
	}

	if(63 == portNum ) /* CPU port MIB */
	{
		/* CPU port good frame sent */
		data = 0;
		regAddr = regBase + 0x0;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxUcast += data;

		/* CPU port MAC trans error frame sent */
		data = 0;
		regAddr = regBase + 0x4;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxCrcError += data;

		/* CPU port good octets sent */
		data = 0;
		regAddr = regBase + 0x8;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxGoodBytesL32 += data;
			
		/* CPU port Rx internal drop */
		data = 0;
		regAddr = regBase + 0xC;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxError += data;

		/* CPU port good frame received */
		data = 0;
		regAddr = regBase + 0x10;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxUcast += data;
				
		/* CPU port bad frame received */
		data = 0;
		regAddr = regBase + 0x14;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxFifoOverrun += data;
		
		/* CPU port good octets received */
		data = 0;
		regAddr = regBase + 0x18;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxGoodBytesL32 += data;
			
		/* CPU port bad octets received */
		data = 0;
		regAddr = regBase + 0x1C;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxBadBytes += data;			
	}
	else  /* Giga port or Hyper.G port MIB */
	{
		/* Port<n> Rx Good Bytes 32 LSB */
		data = 0;
		regAddr = regBase + 0x0;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxGoodBytesL32 += data;

		/* Port<n> Rx Good Bytes 32 MSB */
		data = 0;
		regAddr = regBase + 0x4;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxGoodBytesH32 += data;

		/* Port<n> Rx Bad Bytes */
		data = 0;
		regAddr = regBase + 0x8;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxBadBytes += data;

		/* Port<n> Tx CRC error */
		data = 0;
		regAddr = regBase + 0xC;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxCrcError += data;

		/* Port<n> Rx Ucast Pkt */
		data = 0;
		regAddr = regBase + 0x10;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxUcast += data;

		/* Port<n> Sent Defered */
		data = 0;
		regAddr = regBase + 0x14;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxDefer += data;
		
		/* Port<n> Rx Bcast Pkt */
		data = 0;
		regAddr = regBase + 0x18;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxBcast += data;

		/* Port<n> Rx Mcast Pkt */
		data = 0;
		regAddr = regBase + 0x1C;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxMcast += data;

		/* Port<n> Tx Good Bytes 32 LSB */
		data = 0;
		regAddr = regBase + 0x38;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxGoodBytesL32 += data;

		/* Port<n> Tx Good Bytes 32 MSB */
		data = 0;
		regAddr = regBase + 0x3C;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxGoodBytesH32 += data;

		/* Port<n> Tx Ucast Pkt */
		data = 0;
		regAddr = regBase + 0x40;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxUcast += data;

		/* Port<n> Excessive Collision Tx Drop *//* valid only in Half-Duplex mode */
		data = 0;
		regAddr = regBase + 0x44;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxExCollision += data;

		/* Port<n> Tx Mcast Pkt */
		data = 0;
		regAddr = regBase + 0x48;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxMcast += data;

		/* Port<n> Tx Bcast Pkt */
		data = 0;
		regAddr = regBase + 0x4C;
		prestera_register_read(devNum,regAddr,&data);
		mib->TxBcast += data;

		/* Port<n> Rx Fifo Overrun Pkt */
		data = 0;
		regAddr = regBase + 0x5C;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxFifoOverrun += data;

		/* Port<n> Rx Undersized Pkt */
		data = 0;
		regAddr = regBase + 0x60;
		prestera_register_read(devNum,regAddr,&data);
		mib->Undersized += data;

		/* Port<n> Rx Fragmented Pkt */
		data = 0;
		regAddr = regBase + 0x64;
		prestera_register_read(devNum,regAddr,&data);
		mib->Fragmented += data;

		/* Port<n> Rx Oversized Pkt */
		data = 0;
		regAddr = regBase + 0x68;
		prestera_register_read(devNum,regAddr,&data);
		mib->Oversized += data;

		/* Port<n> Rx Jabber Pkt */
		data = 0;
		regAddr = regBase + 0x6C;
		prestera_register_read(devNum,regAddr,&data);
		mib->Jabber += data;

		/* Port<n> Rx Error Pkt */
		data = 0;
		regAddr = regBase + 0x70;
		prestera_register_read(devNum,regAddr,&data);
		mib->RxError += data;

		/* Port<n> CRC error events */
		data = 0;
		regAddr = regBase + 0x74;
		prestera_register_read(devNum,regAddr,&data);
		mib->BadCrc += data;

		/* Port<n> Collision Event  */
		data = 0;
		regAddr = regBase + 0x78;
		prestera_register_read(devNum,regAddr,&data);
		mib->CollisionEvent += data;

		/* Port<n> Late Collision */
		data = 0;
		regAddr = regBase + 0x7C;
		prestera_register_read(devNum,regAddr,&data);
		mib->LateCollision += data;
	}
	return;
}

/*******************************************************************************
* prestera_do_timer
*
* DESCRIPTION:
*		prestera timer function, call once every HZ.
*
* INPUTS:
*		arg  - no used in this implementation
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	NULL
*
* COMMENTS:
*		
*
*******************************************************************************/
static void prestera_do_timer
(
	unsigned long arg
) 
{
	unsigned char i = 0, j = 0,devNum = 0, portNum = 0;
	struct prestera_port	*ports = NULL;
	struct port_mib_info 	*mib = NULL;
	
	if(!presteraPorts) return;

	for(i = 0; i < founddevs; i++) 
	{
		ports = presteraPorts[i];
		if(!ports) continue;
		if(!ports->mib) continue;

		devNum = ports->devNum;
		for(j = 0; j < ports->count; j++) 
		{
			mib = ports->mib[j];
			if(j == (ports->count - 1)) /* CPU port info at the end */
				portNum = 63;
			else 
				portNum = j + ports->startPort;
			prestera_timer_stat_get(devNum,portNum,mib);
		}
	}

    /* Repeat every second */
    //mod_timer(&prestera_mib_timer, jiffies + HZ);

	return;
}
/*******************************************************************************
* prestera_port_stat_clear
*
* DESCRIPTION:
*		/proc/prestera/clear_stats was openned. Use the single_open iterator
*
* INPUTS:
*		page  	- 
*		start  	- 
*		offset	-
*		count	-
*		eof		-
*		data		-
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		This route is hooked as procfs read routine.
*
*******************************************************************************/
int prestera_port_stat_clear
(
	char * page, 
	char **start, 
	off_t offset, 
	int count, 
	int *eof, 
	void *data
)
{
	unsigned int len = 0;
	unsigned char i = 0, j = 0;
	struct prestera_port 	*ports = NULL;
	struct port_mib_info 	*mib = NULL;
	
	if(!presteraPorts) 
	{
		*eof = 1;
		return 0;
	}

	for(i = 0; i < founddevs; i++) 
	{
		ports = presteraPorts[i];
		if(!ports) continue;
		if(!ports->mib) continue;

		for(j = 0; j < ports->count; j++) 
		{
			mib = ports->mib[j];
			memset(mib,0,sizeof(struct port_mib_info));
		}
	}
	
	len += sprintf(page+len,"packet statistic for all devices and ports cleared!\n");

	*eof = 1;

	return len;
}

/*******************************************************************************
* cpld_register_read
*
* DESCRIPTION:
*       This is the Prestera Routine Read CPLD registers in kernel space 
*
* INPUTS:
*       regAddr  	- the register address to read
*
* OUTPUTS:
*       data		-register data to get.
*
* RETURNS:
*	  NULL
*
* COMMENTS:
*       None.
*
*******************************************************************************/
void cpld_register_read
(
	unsigned int  regAddr,
	unsigned char  *data
)
{
	unsigned long address = 0UL;
	unsigned char value	= 0;	

	address = cpld_rw_base + regAddr;
	value = *(volatile unsigned char*)address;

	*data = value;
	return;
}

/*******************************************************************************
* cpld_register_write
*
* DESCRIPTION:
*       This is the Prestera Routine write register value to CPLD registers in kernel space 
*
* INPUTS:
*       regAddr  	- the register address to write
*	   data 		- register value to write
*
* OUTPUTS:
*	  NULL
*
* RETURNS:
*	  0 - if write successfully
*	  data - data read back after write
*
* COMMENTS:
*       None.
*
*******************************************************************************/
unsigned int cpld_register_write
(
	unsigned int  regAddr,
	unsigned char  data
)
{
	unsigned long address = 0UL;
	unsigned char value  = 0;	

	address = cpld_rw_base + regAddr;
	*(volatile unsigned char*)address = data;
	GT_SYNC;
	*(volatile unsigned char*)address = data;
	GT_SYNC;
	*(volatile unsigned char*)address = data;
	GT_SYNC;

	/* read after write */
	value = *(volatile unsigned char*)address;
	if(data != value) 
		return value;
	else 
		return 0;
}

/*******************************************************************************
* cpld_register_value_get
*
* DESCRIPTION:
*		/proc/prestera/cpld_read was openned.
*
* INPUTS:
*		page  	- 
*		start  	- 
*		offset	-
*		count	-
*		eof		-
*		data		-
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		This route is hooked as procfs read routine.
*
*******************************************************************************/
int cpld_register_value_get
(
	char * page, 
	char **start, 
	off_t offset, 
	int count, 
	int *eof, 
	void *data
)
{
	char usage[] = "\
Usage:\n\
  cpld_reg\t0x0-0xF for common registers\n\
        \t0x10-0x27 for function specified registers\n";

	unsigned char value = 0;
	unsigned int len = 0;

	if(cpld_reg < 0 || cpld_reg > 0x27) {
		len += sprintf(page+len,"%s",usage);
		*eof = 1;
		return len;
	}
	cpld_register_read(cpld_reg,&value);
	len += sprintf(page+len,"%-10s:%#-8x\n","REGISTER",cpld_reg);
	len += sprintf(page+len,"%-10s:%#-8x\n","VALUE",value);			

    *eof = 1;

	return len;
}
/*******************************************************************************
* cpld_register_value_set
*
* DESCRIPTION:
*		/proc/prestera/cpld_write was openned.
*
* INPUTS:
*		page  	- 
*		start  	- 
*		offset	-
*		count	-
*		eof		-
*		data		-
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		This route is hooked as procfs read routine.
*
*******************************************************************************/
int cpld_register_value_set
(
	char * page, 
	char **start, 
	off_t offset, 
	int count, 
	int *eof, 
	void *data
)
{
	char usage[] = "\
Usage:\n\
  cpld_reg\t0x0-0xF for common registers\n\
        \t0x10-0x27 for function specified registers\n\
  cpld_data\tregister value to write\n";

	unsigned int value = 0,len = 0;

	if(cpld_reg < 0 || cpld_reg > 0x27) {
		len += sprintf(page+len,"%s",usage);
		*eof = 1;
		return len;
	}
	
	value = cpld_register_write(cpld_reg,cpld_data);
	len += sprintf(page+len,"%-10s:%#-8x\n","REGISTER",cpld_reg);
	len += sprintf(page+len,"%-10s:%#-8x\n","VALUE",value);			
	len += sprintf(page+len,"%-10s:%#-8x\n","RETURN",value ? value : cpld_data);

    *eof = 1;

	return len;
}
static char *phy_read_errmsg[] = {
	"no error",
	"pp device no registered",
	"SMI write page timeout",
	"SMI read operation timeout"
};
static char *phy_write_errmsg[] = {
	"no error",
	"pp device no registered",
	"SMI write page timeout",
	"SMI write operation timeout"
};

/*******************************************************************************
* prestera_phy_register_read
*
* DESCRIPTION:
*       This is the Prestera Routine Read PHY registers in kernel space 
*
* INPUTS:
*       devNum  	- the PP device number
*	  portNum 	- device port number
*	  phyPage 	- phy register page selector
*	  phyReg		- phy register address
*
* OUTPUTS:
*       data		-register data to get.
*
* RETURNS:
* 	  0  - phy register read success
*	  -1 - device number error,no pp device registered
*	  -2 - SMI operation timeout
*
* COMMENTS:
*       None.
*
*******************************************************************************/
int prestera_phy_register_read
(
	unsigned char  devNum,
	unsigned char  portNum,
	unsigned char  phyPage,
	unsigned char  phyReg,
	unsigned short *data
)
{
	unsigned int regAddr = 0,writeData = 0,readData = 0;
	unsigned int device_type = 0;
	struct PP_Dev *pp = NULL;
	unsigned char (*aPtr)[2] = NULL,phyAddr = 0;
	unsigned int i = 0, count = 0;
	unsigned int retryCount = 0,busy = 0,readValid = 0;

	pp = ppdevs[devNum];
	if(!pp) return -1;

	device_type = pp->device_type;
	if(CPSS_98DX275_CNS == device_type) {
		aPtr = phyPortAddrMapAx7005;
		count = sizeof(phyPortAddrMapAx7005)/(2 * sizeof(unsigned char));
	}
	else if(CPSS_98DX265_CNS == device_type) {
		aPtr = phyPortAddrMapAu4624;
		count = sizeof(phyPortAddrMapAu4624)/(2 * sizeof(unsigned char));
	}
	regAddr = 0x04004054 + (portNum/12) * 0x01000000;

	for(i=0;i<count;i++,aPtr++) {
		if(portNum == (*aPtr)[0]) {
			phyAddr = (*aPtr)[1];
			printk("read phy find addr %d for port %d\n",phyAddr,portNum);
			break;
		}
	}

	/* phase 1 step 1: select phy register page */
	writeData |= (phyPage & 0xFFFF);
	writeData |= ((phyAddr & 0x1F) << 16);
	writeData |= (0x16 << 21); /* set PHY page selector register 22 */
	writeData |= (0x0 << 26); /* write operation */
	printk("select page:write %#x value %#x\n",regAddr,writeData);
	prestera_register_write(devNum,regAddr,writeData);

	/* phase 1 step 2: check if write operation complete */
	prestera_register_read(devNum,regAddr,&readData);
	retryCount = 1000;
	busy = (readData & 0x10000000)>>28;
	while(retryCount && busy) {
		readData = 0;
		prestera_register_read(devNum,regAddr,&readData);
		busy = (readData & 0x10000000)>>28;
		retryCount--;
	}
	printk("busy check count %d\n",1000-retryCount);
	if(0 == retryCount) return -2;
	
	/* phase 2 step 1:read phy register */
	writeData = 0;
	writeData |= ((phyAddr & 0x1F)<< 16); /* set phy address */
	writeData |= ((phyReg & 0x1F)<< 21); /* set phy register */
	writeData |= (0x1 << 26); /* read operation */
	printk("write phy reg:write %#x value %#x\n",regAddr,writeData);
	prestera_register_write(devNum,regAddr,writeData);

	/* phase 1 step 2: check if read operation complete */
	readData = 0;
	prestera_register_read(devNum,regAddr,&readData);
	retryCount = 1000;
	readValid = (readData & 0x10000000)>>27;
	while(retryCount && !readValid) {
		readData = 0;
		prestera_register_read(devNum,regAddr,&readData);
		readValid = (readData & 0x10000000)>>27;
		retryCount--;
	}

	printk("read valid check count %d\n",1000-retryCount);

	if(0 == retryCount) return -3;

	printk("read dev %d port %d page %d reg %d value %#x\n",devNum,portNum,phyPage,phyReg,readData&0xFFFF);
	*data = (readData & 0xFFFF);

	return 0;
}

/*******************************************************************************
* prestera_phy_register_value_get
*
* DESCRIPTION:
*		/proc/prestera/phy_read was openned.
*
* INPUTS:
*		page  	- 
*		start  	- 
*		offset	-
*		count	-
*		eof		-
*		data		-
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		This route is hooked as procfs read routine.
*
*******************************************************************************/
int prestera_phy_register_value_get
(
	char * page, 
	char **start, 
	off_t offset, 
	int count, 
	int *eof, 
	void *data
)
{
	char usage[] = "\
Usage:\n\
  mv_dev\t0 for 98Dx275 on AX7000, 98Dx265 on AU4000\n\
        \t1 for 98Dx804 on AX7000, none on AU4000\n\
        \t255 for all devices\n\
  mv_port\t0-23 for giga port 0 ~ 23 on device\n\
  phy_page\t0:copper 1:fiber 2:MAC\n\
  phy_addr\tphy register address given by PHY specification\n";

	unsigned short value = 0;
	unsigned int len = 0;
	int result = 0;

	if(mv_dev >= founddevs || mv_port > 23 || phy_page > 2 || phy_reg > 26) {
		len += sprintf(page+len,"%s",usage);
		*eof = 1;
		return len;
	}
	result = prestera_phy_register_read(mv_dev,mv_port,phy_page,phy_reg,&value);
	if(!result) {
		len += sprintf(page+len,"%-10s:%#-8x\n","DEVICE",mv_dev);
		len += sprintf(page+len,"%-10s:%#-8x\n","PORT",mv_port);
		len += sprintf(page+len,"%-10s:%#-8x\n","PAGE",phy_page);
		len += sprintf(page+len,"%-10s:%#-8x\n","REGISTER",phy_reg);
		len += sprintf(page+len,"%-10s:%#-8x\n","VALUE",value);			
	}
	else {
		len += sprintf(page+len,"phy register read error:%s\n",phy_read_errmsg[-result]);
	}
    *eof = 1;

	return len;
}

/*******************************************************************************
* prestera_phy_register_write
*
* DESCRIPTION:
*       This is the Prestera Routine Write PHY registers in kernel space 
*
* INPUTS:
*       devNum  	- the PP device number
*	  portNum 	- device port number
*	  phyPage 	- phy register page selector
*	  phyReg		- phy register address
*       data		-register data to set.
*
* OUTPUTS:
*
* RETURNS:
* 	  0  - phy register write success
*	  -1 - device number error,no pp device registered
*	  -2 - SMI write page timeout
*	  -3 - SMI write operation timeout
*
* COMMENTS:
*       None.
*
*******************************************************************************/
int prestera_phy_register_write
(
	unsigned char  devNum,
	unsigned char  portNum,
	unsigned char  phyPage,
	unsigned char  phyReg,
	unsigned short data
)
{
	unsigned int regAddr = 0,writeData = 0,readData = 0;
	unsigned int device_type = 0;
	struct PP_Dev *pp = NULL;
	unsigned char (*aPtr)[2] = NULL,phyAddr = 0;
	unsigned int i = 0, count = 0;
	unsigned int retryCount = 0,busy = 0;

	pp = ppdevs[devNum];
	if(!pp) return -1;

	device_type = pp->device_type;
	if(CPSS_98DX275_CNS == device_type) {
		aPtr = phyPortAddrMapAx7005;
		count = sizeof(phyPortAddrMapAx7005)/(2 * sizeof(unsigned char));
	}
	else if(CPSS_98DX265_CNS == device_type) {
		aPtr = phyPortAddrMapAu4624;
		count = sizeof(phyPortAddrMapAu4624)/(2 * sizeof(unsigned char));
	}
	regAddr = 0x04004054 + (portNum/12) * 0x01000000;

	for(i=0;i<count;i++,aPtr++) {
		if(portNum == (*aPtr)[0]) {
			phyAddr = (*aPtr)[1];
			printk("write phy find addr %d for port %d\n",phyAddr,portNum);
		}
	}

	/* phase 1 step 1: select phy register page */
	writeData |= (phyPage & 0xFFFF);
	writeData |= ((phyAddr & 0x1F) << 16);
	writeData |= (0x16 << 21); /* set PHY page selector register 22 */
	writeData |= (0x0 << 26); /* write operation */
	printk("select page:write %#x value %#x\n",regAddr,writeData);
	prestera_register_write(devNum,regAddr,writeData);

	/* phase 1 step 2: check if write operation complete */
	prestera_register_read(devNum,regAddr,&readData);
	retryCount = 1000;
	busy = (readData & 0x10000000)>>28;
	while(retryCount && busy) {
		readData = 0;
		prestera_register_read(devNum,regAddr,&readData);
		busy = (readData & 0x10000000)>>28;
		retryCount--;
	}
	printk("busy check count %d\n",1000-retryCount);
	if(0 == retryCount) return -2;
	
	/* phase 2 step 1:read phy register */
	writeData = 0;
	writeData |= (data & 0xFFFF);
	writeData |= ((phyAddr & 0x1F)<< 16); /* set phy address */
	writeData |= ((phyReg & 0x1F)<< 21); /* set phy register */
	writeData |= (0x0 << 26); /* write operation */
	printk("write phy reg:write %#x value %#x\n",regAddr,writeData);
	prestera_register_write(devNum,regAddr,writeData);

	/* phase 1 step 2: check if write operation complete */
	readData = 0;
	prestera_register_read(devNum,regAddr,&readData);
	retryCount = 1000;
	busy = (readData & 0x10000000)>>28;
	while(retryCount && busy) {
		readData = 0;
		prestera_register_read(devNum,regAddr,&readData);
		busy = (readData & 0x10000000)>>28;
		retryCount--;
	}
	printk("write busy check count %d\n",1000-retryCount);
	if(0 == retryCount) return -3;

	return 0;
}

/*******************************************************************************
* prestera_phy_register_value_set
*
* DESCRIPTION:
*		/proc/prestera/phy_write was openned.
*
* INPUTS:
*		page  	- 
*		start  	- 
*		offset	-
*		count	-
*		eof		-
*		data		-
*
* OUTPUTS:
*		NULL
*
* RETURNS:
*	 	
*
* COMMENTS:
*		This route is hooked as procfs read routine.
*
*******************************************************************************/
int prestera_phy_register_value_set
(
	char * page, 
	char **start, 
	off_t offset, 
	int count, 
	int *eof, 
	void *data
)
{
	char usage[] = "\
Usage:\n\
  mv_dev\t0 for 98Dx275 on AX7000, 98Dx265 on AU4000\n\
        \t1 for 98Dx804 on AX7000, none on AU4000\n\
        \t255 for all devices\n\
  mv_port\t0-23 for giga port 0 ~ 23 on device\n\
  phy_page\t0:copper 1:fiber 2:MAC\n\
  phy_addr\tphy register address given by PHY specification\n\
  phy_value\tphy register value to write\n";

	unsigned int len = 0;
	int result = 0;
    /* code optimize: "phy_value >> 16" is always 0 */
	if(mv_dev >= founddevs || mv_port > 23 || phy_page > 2 || phy_reg > 26) {
		len += sprintf(page+len,"%s",usage);
		*eof = 1;
		return len;
	}
	result = prestera_phy_register_write(mv_dev,mv_port,phy_page,phy_reg,phy_value);
	if(!result) {
		len += sprintf(page+len,"%-10s:%#-8x\n","DEVICE",mv_dev);
		len += sprintf(page+len,"%-10s:%#-8x\n","PORT",mv_port);
		len += sprintf(page+len,"%-10s:%#-8x\n","PAGE",phy_page);
		len += sprintf(page+len,"%-10s:%#-8x\n","REGISTER",phy_reg);
		len += sprintf(page+len,"%-10s:%#-8x\n","VALUE",phy_value);
		len += sprintf(page+len,"%-10s:%s\n","STATUS","ok");
	}
	else {
		len += sprintf(page+len,"phy register write error:%s\n",phy_write_errmsg[-result]);
	}
    *eof = 1;

	return len;
}

static int get_num_from_file_mvpp(const char *filename)
{
    char temp[4]={0};
	int result;
	int value = -1;
	struct file *filp=NULL;
	filp = filp_open(filename, O_RDONLY, 0);
	if (!filp)
	{
		printk("open file %s fail!\n",filename);
		return -1;
	}
	mm_segment_t old_fs = get_fs();
	set_fs(KERNEL_DS);
	result = filp->f_op->read(filp, temp, 1, &filp->f_pos);
	if (result < 0)
	{
		printk("read value fail!\n");
	}
	set_fs(old_fs);
	filp_close(filp, NULL);
	value = (int)(temp[0] - '0');
	return value;
}

static void mvpp_get_boardinfo()
{
    board_type_mvpp = get_num_from_file_mvpp("/proc/product_info/board_type_num");
    slot_id_mvpp = get_num_from_file_mvpp("/proc/product_info/board_slot_id");
}

#endif
/************************************************************************
 *
 * prestera_init: 
 */
static int prestera_init(void)
{
    int         result = 0;
	struct proc_dir_entry *entry = NULL;

    printk(KERN_DEBUG "mvPpDrv: prestera_init\n");
    mvpp_get_boardinfo();
    printk(KERN_DEBUG "board_type_mvpp:%d\n",board_type_mvpp);	
    printk(KERN_DEBUG "slot_id_mvpp:%d\n",slot_id_mvpp);
	if(board_type_mvpp==BOARD_TYPE_AX71_2X12G12S)   /* for debug */
	{
        printk(KERN_DEBUG "this is %s\n","BOARD_TYPE_AX71_2X12G12S");		
	}
    /* first thing register the device at OS */
    
    /* Register your major. */
    result = register_chrdev_region(MKDEV(prestera_major, 0), 1, "mvPP");
    if (result < 0)
    {
        printk("prestera_init: register_chrdev_region err= %d\n", result);
        return result;
    }

    cdev_init(&prestera_cdev, &prestera_fops);

    prestera_cdev.owner = THIS_MODULE;

    result = cdev_add(&prestera_cdev, MKDEV(prestera_major, 0), 1);
    if (result)
    {
        unregister_chrdev_region(MKDEV(prestera_major, 0), 1);
        printk("prestera_init: cdev_add err= %d\n", result);
        return result;
    }

    printk(KERN_DEBUG "prestera_major = %d\n", prestera_major);

    prestera_dev = (struct Prestera_Dev *) kmalloc(sizeof(struct Prestera_Dev), GFP_KERNEL);
    if (!prestera_dev)
    {
        printk("\nPresteraDrv: Failed allocating memory for device\n");
        result = -ENOMEM;
        goto fail;
    }
    #ifndef __AX_PLATFORM__
	/* create proc direcotry and sub entry for port packet statistics */
	presteraDir = proc_mkdir("prestera",NULL);
	if(!presteraDir) {
		printk("create proc directory prestera null\n");
		goto fail;
	}
	#if 0
	result = proc_register(NULL,presteraDir);
	if(result) {
		printk("register proc directory err %d\n",result);
		goto fail;
	}
	#endif
    entry = create_proc_entry("show_stats", 0, presteraDir);
    if (entry)
        entry->proc_fops = &prestera_stats_operations;

	create_proc_read_entry("read_reg",0,presteraDir, prestera_register_value_get,NULL);
	create_proc_read_entry("write_reg",0,presteraDir, prestera_register_value_set,NULL);
	create_proc_read_entry("clear_stats",0,presteraDir, prestera_port_stat_clear,NULL);
	create_proc_read_entry("cpld_read",0,presteraDir, cpld_register_value_get,NULL);
	create_proc_read_entry("cpld_write",0,presteraDir, cpld_register_value_set,NULL);
	create_proc_read_entry("phy_read",0,presteraDir, prestera_phy_register_value_get, NULL);
	create_proc_read_entry("phy_write",0,presteraDir, prestera_phy_register_value_set, NULL);
	#endif

    /* create proc entry */
    create_proc_read_entry("mvPP", 0, presteraDir, prestera_read_proc_mem, NULL);


    /* initialize the device main semaphore */
    sema_init(&prestera_dev->sem, 1);

    result = pp_find_all_devices();
    if (0 != result)
    {
        goto fail;
    }
	#if 0
    dma_base = __pa(high_memory);
    dma_len  = 2 * _1MB;
    dma_base |= 0x1000000000000ULL;
    printk(KERN_DEBUG"io_memresource start 0x%016lx end 0x%016lx\n",
    		iomem_resource.start,iomem_resource.end);
    if (!request_mem_region(dma_base, dma_len, "prestera-dma")) 
    {
        printk("Cannot reserve DMA region 0x%016lx size 0x%016lx\n",
                dma_base, dma_len);
        return -ENODEV;
    }
	#endif	
    #if 0		/* remove for virtual-branch  2012-07-28 */
    /* GroupId is needed for Lion.
        * Need someone modify laterly.
        * luoxun@autelan.com
        **/
    if((BOARD_TYPE_AX81_SMU != board_type_mvpp)\
		&&(BOARD_TYPE_AX81_12X != board_type_mvpp)\
		&&(BOARD_TYPE_AX81_AC4X != board_type_mvpp))
    {
        /* Enable the poll timer for checking port MAC counter */
    	init_timer(&prestera_mib_timer);
    	prestera_mib_timer.data = 0;
    	prestera_mib_timer.function = prestera_do_timer;
    	mod_timer(&prestera_mib_timer, jiffies + HZ);
    }
	#endif
    prestera_initialized = 1;

    printk(KERN_DEBUG "prestera_init finished\n");

    return 0;

fail:
    prestera_cleanup();

    printk("\nPresteraDrv: Init FAIL!\n");
    return result;
}


module_init(prestera_init);
module_exit(prestera_cleanup);
