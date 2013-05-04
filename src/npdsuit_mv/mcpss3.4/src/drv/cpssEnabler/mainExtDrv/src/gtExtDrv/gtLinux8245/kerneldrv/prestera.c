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
*            functions in kernel mode special for prestera.
*
* DEPENDENCIES:
*
*       $Revision: 1.1.1.1 $
*******************************************************************************/


#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif
#include <linux/module.h>
#include <linux/kernel.h>
#if CONFIG_MODEREVISION
#define MODREVISION
#include <linux/modeversions.h>
#endif

#include "sysdep.h"
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/net.h>
#include <linux/ioctl.h>
#include <linux/pci.h>

#include "marvell.h"
#include "prestera.h"
#include "prestera_glob.h"
#include "presteraIrq.h"

#define GT_DEFAULT_INT_TASK_STACK_SIZE      (_32K + _8K)  /* 40 KB    */

/* external functions */
extern void enable_irq(int irq_num);
extern void disable_irq(int irq_num);


/* external variables */
#ifdef INTERRUPT_1
INT_TASK_PARAMS intTaskParams;
#endif 

/* local variables and variables */
static int prestera_opened      = 0;
static int prestera_initialized = 0;
static struct Prestera_Dev *prestera_dev;
static struct pci_dev *pcidevs[MAX_PP_DEVICES]; /* One more for sentinel */
static struct PP_Dev *ppdevs[MAX_PP_DEVICES]; /* One more for sentinel */
static int founddevs = 0;
static unsigned long dev_size = 0;


int prestera_major =   PRESTERA_MAJOR;
int prestera_nr_devs = PRESTERA_NR_DEVS;    /* number of bare scull devices */
int prestera_quantum = PRESTERA_QUANTUM;
int prestera_qset =    PRESTERA_QSET;

/* GT_98MX625 and GT_98MX625V0 are the same */
/* ID depends of connection to the board */
/* GT_98MX625V0 is recognized at initialization */
/* and represented by GT_98MX625 in all SW structures */

#define PP_98MX625V0   0x0190

/* Legal device types that may be read from PCI device type register.   */
/* The list must end with 0xFFFFFFFF.                                   */
static GT_U32 legalDevTypes[] = {PRV_CPSS_ALL_DEVICES_MAC};

/* Forward declertions */
MODULE_PARM(prestera_major,"i");
MODULE_PARM(prestera_nr_devs,"i");
MODULE_PARM(prestera_quantum,"i");
MODULE_PARM(prestera_qset,"i");
MODULE_AUTHOR("Shlomo Pongratz");
MODULE_LICENSE("GPL");

unsigned long dma_base;
unsigned long dma_len;
unsigned long *dma_area;

/* Forward function declaratios */
static loff_t prestera_lseek(struct file *filep, loff_t off, int whence);
static ssize_t prestera_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static ssize_t prestera_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int prestera_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
static int prestera_mmap(struct file * file, struct vm_area_struct *vma);
static int prestera_open(struct inode * inode, struct file * filp);
static int prestera_release(struct inode * inode, struct file * file);
/*static int prestera_fsync(struct * file, struct dentry * dentry, int datasync);*/

struct file_operations prestera_fops =
{
    llseek: prestera_lseek,
    read:  prestera_read,
    write: prestera_write,
    ioctl: prestera_ioctl,
    mmap: prestera_mmap,
    open:  prestera_open,
    release:  prestera_release /* A.K.A close */
};


/************************************************************************
 *
 * And the init and cleanup functions come last
 */
/************************************************************************
 *
 * And the init and cleanup functions come last
 */
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
 */
static ssize_t prestera_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    return -ERESTARTSYS;
}

/************************************************************************
 *
 * prestera_write: this should be the write device function, for now in
 * current prestera driver implemention it dose nothig
 *
 */
static ssize_t prestera_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
    return -ERESTARTSYS;
}

/************************************************************************
 *
 * And the init and cleanup functions come last
 */
static int prestera_ioctl (struct inode *inode, struct file *filp,
                        unsigned int cmd, unsigned long arg)
{
    struct pci_dev *dev;
    PP_PCI_REG pciReg;
    PciConfigReg_STC pciConfReg;
    GT_PCI_Dev_STC gtDev;
    GT_Intr2Vec int2vec;
    GT_VecotrCookie_STC vector_cookie;
    GT_RANGE_STC range;
    struct InterruptData * intData;
    int i, instance;
    unsigned long devfn;
    
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
            /* read and parse user data structur */
            if (copy_from_user(&vector_cookie,(PP_PCI_REG*) arg, sizeof(GT_VecotrCookie_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            intConnect(vector_cookie.vector, 0, (struct InterruptData **)&vector_cookie.cookie);
            /* USER READS */
            if (copy_to_user((PP_PCI_REG*)arg, &vector_cookie, sizeof(GT_VecotrCookie_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;
        case PRESTERA_IOC_INTENABLE:
            /* clear the mask reg on device 0x10*/
            if (arg > 64)
            {
                printk("DEBUG!!!\n");
                send_sig_info(SIGSTOP, (struct siginfo*)1, current);
            }
            enable_irq(arg);
            break;
        case PRESTERA_IOC_INTDISABLE:
            disable_irq(arg);
            break;
        case PRESTERA_IOC_WAIT:
            /* cookie */
            intData = (struct InterruptData *) arg;
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
            /* read and parse user data structurr */
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
            /* read and parse user data structurr */
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
#if 0
            switch (int2vec.intrLine)
            {
                case PCI_INTR_A:
                    int2vec.vector = PCI0_INTR_A_VEC;
                break;
                case PCI_INTR_B:
                    int2vec.vector = PCI0_INTR_B_VEC;
                break;
                case PCI_INTR_C:
                    int2vec.vector = PCI0_INTR_C_VEC;
                break;
                case PCI_INTR_D:
                    int2vec.vector = PCI0_INTR_D_VEC;
                break;
                default:
                    int2vec.vector = 0;
                break;
            }
#else
            dev = pcidevs[0];
            int2vec.vector = dev->irq;
#endif
            if (copy_to_user((GT_Intr2Vec*)arg, &int2vec, sizeof(GT_Intr2Vec)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;
        case PRESTERA_IOC_FLUSH:
            /* read and parse user data structur */
            if (copy_from_user(&range,(GT_RANGE_STC*) arg, sizeof(GT_RANGE_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            consistent_sync(range.address, range.length, PCI_DMA_TODEVICE);
        case PRESTERA_IOC_INVALIDATE:
            /* read and parse user data structur */
            if (copy_from_user(&range,(GT_RANGE_STC*) arg, sizeof(GT_RANGE_STC)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            consistent_sync(range.address, range.length, PCI_DMA_FROMDEVICE);
            break;
        case PRESTERA_IOC_GETBASEADDR:
            if (copy_to_user((long*)arg, &dma_base, sizeof(long)))
            {
                printk("IOCTL: FAULT\n");
                return -EFAULT;
            }
            break;
        default:
            printk (KERN_WARNING "Unknown ioctl (%x).\n", cmd);
            break;
    }
    return 0;
}
/*
 * open and close: just keep track of how many times the device is
 * mapped, to avoid releasing it.
 */

void prestera_vma_open(struct vm_area_struct *vma)
{
    MOD_INC_USE_COUNT;
    prestera_opened++;
}

void prestera_vma_close(struct vm_area_struct *vma)
{
    prestera_opened--;
    MOD_DEC_USE_COUNT;
}

/*
 * The nopage method: the core of the file. It retrieves the
 * page required from the scullv device and returns it to the
 * user. The count for the page must be incremented, because
 * it is automatically decremented at page unmap.
 *
 * For this reason, "order" must be zero. Otherwise, only the first
 * page has its count incremented, and the allocating module must
 * release it as a whole block. Therefore, it isn't possible to map
 * pages from a multipage block: when they are unmapped, their count
 * is individually decreased, and would drop to 0.
 */

struct page *prestera_vma_nopage(struct vm_area_struct *vma,
                                unsigned long address, int write)
{
    /*ScullV_Dev *ptr, *dev = scullv_vma_to_dev(vma);*/
    struct page *page = NOPAGE_SIGBUS;
    void *pageptr = NULL; /* default to "missing" */
    pgd_t *pgd; pmd_t *pmd; pte_t *pte;
    unsigned long lpage;
    struct pci_dev *dev;
    struct PP_Dev *ppdev;
    int ppNum;
    unsigned long phys;
    unsigned long base;
    unsigned long offset;

    /* Take sempaphore */
    if(down_interruptible(&prestera_dev->sem))
    {
        printk("interrupted\n");
        return page;
    }

    switch (vma->vm_start)
    {
    case 0x50000000:
        /* DMA memory */
        if (0 == dma_base)
        {
            printk("dma kmalloc failed\n");
            base = 0;
            goto out;
        }
        offset = (address - vma->vm_start) + VMA_OFFSET(vma);
        base = (unsigned long) dma_base + offset;
        break;
    case 0x60000000:
        /* Packet processor config space */
        offset = (address - vma->vm_start) + VMA_OFFSET(vma);
        ppNum  = offset / (4 * 1024);
        offset = offset % (4 * 1024);
        dev = pcidevs[ppNum];
        base = vma->vm_start + ppNum * (4 * 1024) + offset;
        phys = pci_resource_start(dev, 0) + offset;
        if (remap_page_range(base ,
                            phys,
                            PAGE_SIZE,
                            vma->vm_page_prot))
        {
            printk("remap_page_range failed\n");
            goto out;
        }
        page = virt_to_page(__va(phys));
        get_page(page);
        goto out;
        break;
    case 0x70000000:
        /* Packet processor Register space */
        offset = (address - vma->vm_start) + VMA_OFFSET(vma);
        /* Map registers space */
        ppNum  = offset / (64 * 1024 * 1024);
        offset = offset % (64 * 1024 * 1024);
        dev  = pcidevs[ppNum];
        ppdev = ppdevs[ppNum];
        base = vma->vm_start + ppNum * (64 * 1024 * 1024) + offset;
        phys = pci_resource_start(dev, 1) + offset;
        if (remap_page_range(base ,
                            phys,
                            PAGE_SIZE,
                            vma->vm_page_prot))
        {
            printk("remap_page_range failed\n");
            goto out;
        }
        page = virt_to_page(__va(phys));
        get_page(page);
        goto out;
    default:
        printk("unknown range (0x%0x)\n", (int)vma->vm_start);
        goto out;
    }
#if 0
    if (offset >= dev_size)
    {
        printk("out of range\n");
        goto out; /* out of range */
    }
#endif
    /*
    * Now retrieve the prestera device from the list,then the page.
    * If the device has holes, the process receives a SIGBUS when
    * accessing the hole.
    */
    if (0 == base)
    {
        /* out of range */
        printk("out of range ????\n");
        goto out;
    }

    pageptr = (void *) base;
    
    /*
     * After scullv lookup, "page" is now the address of the page
     * needed by the current process. Since it's a vmalloc address,
     * first retrieve the unsigned long value to be looked up
     * in page tables.
     */
    lpage = VMALLOC_VMADDR(pageptr);
    
    spin_lock(&init_mm.page_table_lock);
    
    pgd = pgd_offset(&init_mm, lpage);
    pmd = pmd_offset(pgd, lpage);
    pte = pte_offset(pmd, lpage);
    
    page = pte_page(*pte);
    spin_unlock(&init_mm.page_table_lock);
    
    /* got it, now increment the count */
    get_page(page);
out:
    /* Give sempaphore */
    up(&prestera_dev->sem);

    return page;
}

struct vm_operations_struct prestera_vm_ops = {
    open:     prestera_vma_open,
    close:    prestera_vma_close,
    nopage:   prestera_vma_nopage,
};


static int prestera_mmap(struct file * file, struct vm_area_struct *vma)
{
    unsigned long base;
    unsigned long phys;
    unsigned long ppNum;
    unsigned long offset;
    struct pci_dev *dev;
    unsigned long val;
    
    /* struct inode *inode = INODE_FROM_F(filp); */

    if (VMA_OFFSET(vma) & (PAGE_SIZE-1))
    {
        /* need aligned offsets */                
        printk("prestera_mmap offset not aligned\n");
        return -ENXIO;  
    }
    
    /* don't do anything here: "nopage" will fill the holes */
    vma->vm_ops = &prestera_vm_ops;
    /* VM_RESERVED - no swap, VM_IO for I/O memory */
    vma->vm_flags |= (VM_RESERVED | VM_IO);
    val = pgprot_val(vma->vm_page_prot);
    val |= _PAGE_NO_CACHE | _PAGE_GUARDED;
    vma->vm_page_prot = __pgprot(val);
    vma->vm_private_data = prestera_dev;
    /* vma->vm_private_data = scullv_devices + MINOR(inode->i_rdev); */

    switch (vma->vm_start & 0xf0000000)
    {
    case 0x50000000:
        /* DMA memory */
        base = vma->vm_start;
        phys = dma_base;
        if (remap_page_range(base ,
                            phys,
                            dma_len,
                            vma->vm_page_prot))
        {
            printk("remap_page_range failed\n");
        }
        break;
    case 0x60000000:
        /* PPS config */
        offset = (vma->vm_start - 0x60000000) + VMA_OFFSET(vma);
        ppNum = offset / PAGE_SIZE;
        dev = pcidevs[ppNum];
        base = vma->vm_start;
        phys = pci_resource_start(dev, 0);
        if (remap_page_range(base ,
                            phys,
                            PAGE_SIZE,
                            vma->vm_page_prot))
        {
            printk("remap_page_range failed\n");
        }
        break;
    case 0x70000000:
        /* PPS regs */
        offset = (vma->vm_start - 0x70000000) + VMA_OFFSET(vma);
        ppNum = offset / (64 * 1024 * 1024);
        dev = pcidevs[ppNum];
        base = vma->vm_start;
        phys = pci_resource_start(dev, 1);
        if (remap_page_range(base ,
                            phys,
                            (64 * 1024 * 1024),
                            vma->vm_page_prot))
        {
            printk("remap_page_range failed\n");
        }
        break;
    default:
        /* ??? */
        printk("unknown range (0%0x)\n", (int)vma->vm_start);
        break;
    }
        
    prestera_vma_open(vma);
    
    return 0;
}

/************************************************************************
 *
 * And the init and cleanup functions come last
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

#ifndef SHARED_MEMORY
    /* Avoid single-usage restriction for shared memory:
     * device should be accessible for multiple clients. */
    if (prestera_opened)
    {
        up(&prestera_dev->sem);
        return -EBUSY;
    }
#endif

	filp->private_data = prestera_dev;

    MOD_INC_USE_COUNT;

#ifdef PRESTERA_DEBUG
    printk("Prestera Open : inderrupts init GPIO - %d\n", CURRENT_GPP_INT_PIN);
#endif
#ifdef INTERRUPT_1
    devNum = 0;

    retval = gtDrvInterruptsInit(devNum, irq,
                                  irqMask,
                                  0 /* for now - coreSysCfg.coreSysCfgPhase1.eventsTaskPrio*/,
                                  GT_DEFAULT_INT_TASK_STACK_SIZE,
                                  &(intTaskParams));

    if (retval != INT_OK)
    {
#ifdef PRESTERA_DEBUG
        printk("PresteraOpen: Faild interrupt init scan\n");
#endif
        return -1;
    }
#endif



    prestera_opened++;
    up(&prestera_dev->sem);
#ifdef PRESTERA_DEBUG
    printk("presteraDrv: prestera device opened successfuly\n");
#endif
    return 0;
}

/************************************************************************
 *
 * And the init and cleanup functions come last
 */
static int prestera_release(struct inode * inode, struct file * file)
{
    /*free_irq(PRESTERA_IRQ, NULL); */
    prestera_opened--;
    MOD_DEC_USE_COUNT;
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
        len += sprintf(page+len,"\tbus(0x%0x) slot(0x%0x) func(0x%0lx)\n",(int)dev->bus, PCI_SLOT(dev->devfn), (unsigned long)PCI_FUNC(dev->devfn));
        len += sprintf(page+len,"\tvendor_id(0x%0x) device_id(0x%0x)\n",dev->vendor, dev->device);
        len += sprintf(page+len,"\tirq(0x%0x)\n",dev->irq);
        len += sprintf(page+len,"\tname(%s)\n",dev->name);
        for (j = 0; j < 2; j++)
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
            len += sprintf(page+len,"\t\toff(0x%0x) add(0x%0x) val(0x%0x)\n", j, (int)ptr, le32_to_cpu(value));
            address += 4;
        }
        /* Mask */
        address = ppdev->config.base + 0x118;
        ptr = (volatile unsigned long *) address;
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\tMASK(0x%0x) add(0x%0x) val(0x%0x)\n", 0x118, (int)ptr, le32_to_cpu(value));

        /* Cause */
        address = ppdev->config.base + 0x114;
        ptr = (volatile unsigned long *) address;
        value = *ptr;
        GT_SYNC;
        len += sprintf(page+len,"\tMASK(0x%0x) add(0x%0x) val(0x%0x)\n", 0x114, (int)ptr, le32_to_cpu(value));

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


/************************************************************************
 *
 * And the init and cleanup functions come last
 */
static void prestera_cleanup(void)
{
    int ret;
    int i;
    struct PP_Dev *ppdev;
    struct pci_dev *dev;


    printk("Prestera Says: Bye world from kernel\n");

    prestera_initialized = 0;

    for (i = 0; i < founddevs; i++)
    {
        ppdev = ppdevs[i];
        dev   = pcidevs[i];
        
        /* Unmap the memory regions */
        iounmap((void *)ppdev->config.base);
        iounmap((void *)ppdev->ppregs.base);
        
        /* relaese BAR0 */
        release_mem_region(pci_resource_start(dev, 0),
                           pci_resource_len(dev, 0));
        /* relaese BAR1 */
        release_mem_region(pci_resource_start(dev, 1),
                           pci_resource_len(dev, 1));
        
        pci_disable_device(dev);
    }
    founddevs = 0;

    /* free_irq(PRESTERA_IRQ, 0); */

    remove_proc_entry("mvPP", NULL);

    ret = unregister_chrdev(prestera_major, "mvPP");
    if (ret < 0)
    {
        printk("Cleanup faild with %d\n", ret);
    }
}

int pp_find_all_devices(void)
{
    struct pci_dev *dev;
    struct PP_Dev *ppdev;
    int type, i;
    unsigned long start, len, data, addrCompletion;
    
    founddevs = 0;
    dev_size = 0;

    if (!pci_present())
    {
        pcidevs[founddevs] = 0;
        return -ENODEV;
    }

    type = 0;
    dev = NULL;

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
        /* Save PCI device */
        ppdev = (struct PP_Dev *) kmalloc(sizeof(struct PP_Dev), GFP_KERNEL);
        if (NULL == ppdev)
        {
            printk("kmalloc failed\n");
            return -ENOMEM;
        }
        /* enable the device */
        if(pci_enable_device(dev))
        {
            printk("pci_enable_device fail \n");
            return -ENODEV;
        }
        /* enable bus mastering */
        pci_set_master(dev) ;
        
        ppdevs[founddevs] = ppdev;
        pcidevs[founddevs] = dev;
        founddevs++;
    }
    
    /* Map registers space */
    for (i = 0; i < founddevs; i++)
    {
        dev   = pcidevs[i];
        ppdev = ppdevs[i];
        
        start = pci_resource_start(dev, 1);
        len   = pci_resource_len(dev, 1);
        /* Reserve BAR1 region for normal IO access */
        if (!request_mem_region(start, len, "prestera")) 
        {
            printk("Cannot reserve MMI0 region (prestera_mem_reg) 0x%lx @ 0x%lx\n",
                    start, len);
            return -ENODEV;
        }
        /* Find registers space size */
        ppdev->ppregs.size = len;
        /* Map rgisters space to kernel virtual address space */
        /* Note that _nocache is not needed in PPC */
        ppdev->ppregs.base = (unsigned long)ioremap_nocache(start, len);
        dev_size += len;
        /* Default configuration for address completion */
        data = *((unsigned long *)ppdev->ppregs.base);
#if 0
        addrCompletion = cpu_to_le32(0x03020100);
#else 
        addrCompletion = cpu_to_le32(0x01000100);
#endif
        *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
        GT_SYNC;
        *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
        GT_SYNC;
        *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
        GT_SYNC;
        *((unsigned long *) ppdev->ppregs.base) = addrCompletion;
        GT_SYNC;
        
        data = *((unsigned long *)ppdev->ppregs.base);
    }
    /* Map config space */
    for (i = 0; i < founddevs; i++)
    {
        dev   = pcidevs[i];
        ppdev = ppdevs[i];
        
        start = pci_resource_start(dev, 0);
        len   = pci_resource_len(dev, 0);
        /* Reserve BAR0 region for extended PCI configuration */ 
        if (!request_mem_region(start, len, "prestera")) 
        {
            printk("Cannot reserve MMIO region (prestera_ext_pci) 0x%lx @ 0x%lx\n",
                    start, len);
            return -ENODEV;
        }
        /* Find config space size */
        ppdev->config.size = len;
        /* Map config space to kernel virtual address space */
        /* Note that _nocache is not needed in PPC */
        ppdev->config.base = (unsigned long)ioremap_nocache(start, len);
        dev_size += len;
        data = *((unsigned long *)ppdev->config.base);
    }
    for (i = 0; i < founddevs; i++)
    {
        ppdev = ppdevs[i];
        data = *((unsigned long *)ppdev->config.base);
        data = *((unsigned long *)ppdev->ppregs.base);
    }
    return 0;
}

/************************************************************************
 *
 * And the init and cleanup functions come last
 */

static int prestera_init(void)
{
    SET_MODULE_OWNER(&prestera_fops);
    int result = 0;

#if LINUX_VERSION >= KERNEL_VERSION(2,2,0)
    printk("LINUX_VERSION >= KERNEL_VERSION(2,2,0)\n");
#else
    printk("LINUX_VERSION < KERNEL_VERSION(2,4,0)\n");
#endif
    printk("Hello world from kernel\n");

    /* first thing register the device at OS */
    prestera_major = register_chrdev(0, /* dynamic */
                                  "mvPP",
                                  &prestera_fops);
    if (prestera_major < 0)
    {
        printk("PresteraDrv: can't get major %d\n",prestera_major);
        return prestera_major;
    }
    else
    {
        printk("\nPresteraDrv: registerd sucsesfuly successefuly.Major Num - %d\n",prestera_major);
    }
    prestera_dev = (struct Prestera_Dev *) kmalloc(sizeof(struct Prestera_Dev), GFP_KERNEL);
    if (!prestera_dev)
    {
        printk("\nPresteraDrv: Failed allocating memory for device\n");
        result = -ENOMEM;
        goto fail;
    }

    /* create proc entry */
    create_proc_read_entry("mvPP", 0, NULL, prestera_read_proc_mem, NULL);

    /* initialize the device main semaphore */
    sema_init(&prestera_dev->sem, 1);

    result = pp_find_all_devices();
    if (0 != result)
    {
        goto fail;
    }
    dma_base = __pa(high_memory);
    dma_len  = 2 * (1024 * 1024);
    if (!request_mem_region(dma_base, dma_len, "prestera-dma")) 
    {
        printk("Cannot reserve DMA region 0x%lx @ 0x%lx\n",
                dma_base, dma_len);
        return -ENODEV;
    }
    /* check dma area */
    dma_area = (unsigned long *)ioremap_nocache(dma_base, dma_len);
    /*iounmap((void *) dma_area);*/

    prestera_initialized = 1;

    printk("\nPresteraDrv: Init OK!\n");

    return 0;

fail:
    prestera_cleanup();

    printk("\nPresteraDrv: Init FAIL!\n");
    return result;
}


module_init(prestera_init);
module_exit(prestera_cleanup);
