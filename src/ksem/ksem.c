#include <linux/autoconf.h>
#include <linux/kernel.h>
#include <linux/types.h> 
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/delay.h>
#include <net/netlink.h>
#include <net/sock.h> 
#include <linux/init.h>      
#include <linux/module.h>      
#include <linux/timer.h>    
#include <linux/time.h> 
#include <linux/errno.h>
#include <linux/spinlock.h>
#include <asm/irq_cpu.h>
#include <asm/mipsregs.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/netlink.h>

#include <asm/octeon/cvmx.h>
#include "asm/octeon/cvmx-app-init.h"
#include "ksem.h"

static int major_no = 246;
static int minor_no = 0;

static sem_dev_t *sem_devices;

DEFINE_RWLOCK(octeon_irq_ciu0_rwlock);
DEFINE_RWLOCK(octeon_gpio_intclr_rwlock);

struct sock *nl_sk_ksem = NULL;
struct sock *nl_sk_distributed = NULL;
unsigned long product_id;
unsigned long board_id;
int ksem_debug = 0;

module_param(ksem_debug, int, 0644);

extern void ksem_netlink_rcv(struct sk_buff *skb);

#if 1
/**
 * Interrupt handler function that is scheduled on a core when an interrupt occurs.
 */
static irqreturn_t sem_irq_interrupt(int irq, void *dev_id)
{
	cvmx_gpio_int_clr_t gpio_intclr;
    unsigned long flags;
	unsigned long intState1, intState2, intState3;
	
	ksemDBG("**********************start*******************\n");
    ksemDBG("GPIO5 INTERRUPT  irq = %d\n", irq);

	printk(KERN_ERR"**********************start*******************\n");
    printk(KERN_ERR"GPIO5 INTERRUPT  irq = %d\n", irq);
       
       
    switch(product_id)
    {
		case PRODUCT_AX7605I: 
			break;
		case PRODUCT_AX8610:
			if(board_id == AX81_CRSMU)
			{
				udelay(10000);
                read_lock_irqsave(&octeon_gpio_intclr_rwlock, flags);
            	gpio_intclr.u64 = 0;
            	gpio_intclr.u64 = cvmx_read64_uint64(GPIO_INT_CLR);
            	gpio_intclr.s.type = 0x20;
            	cvmx_write64_uint64(GPIO_INT_CLR, gpio_intclr.u64);
                intState1 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG1);
				udelay(1);
            	intState2 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG2);
				udelay(1);
            	intState3 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG3);
                read_lock_irqsave(&octeon_gpio_intclr_rwlock, flags);
    			
                ksemDBG("intState1 = %#lx\n", intState1);
            	ksemDBG("intState2 = %#lx\n", intState2);
            	ksemDBG("intState3 = %#lx\n", intState3);
    			
    			if(intState1) {
    			    check_on_intState1_for_ax86xx(intState1);
    			}
				if(intState2) {
    			    check_on_intState2_for_ax86xx(intState2);
    			}
				if(intState3) {
    			    check_on_intState3_for_ax86xx(intState3);
    			}
			}
			break;
		case PRODUCT_AX8606:
			if(board_id == AX81_CRSMU)
			{
				udelay(10000);
                read_lock_irqsave(&octeon_gpio_intclr_rwlock, flags);
            	gpio_intclr.u64 = 0;
            	gpio_intclr.u64 = cvmx_read64_uint64(GPIO_INT_CLR);
            	gpio_intclr.s.type = 0x20;
            	cvmx_write64_uint64(GPIO_INT_CLR, gpio_intclr.u64);
                intState1 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG1);
				udelay(1);
            	intState2 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG2);
				udelay(1);
            	intState3 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG3);
                read_lock_irqsave(&octeon_gpio_intclr_rwlock, flags);
    			
                ksemDBG("intState1 = %#lx\n", intState1);
            	ksemDBG("intState2 = %#lx\n", intState2);
            	ksemDBG("intState3 = %#lx\n", intState3);
    			
    			if(intState1) {
    			    check_on_intState1_for_ax86xx(intState1);
    			}
				if(intState2) {
    			    check_on_intState2_for_ax86xx(intState2);
    			}
				if(intState3) {
    			    check_on_intState3_for_ax86xx(intState3);
    			}
			}
			break;
		case PRODUCT_AX8800:
			if(board_id == AX81_CRSMU)
			{
				udelay(10000);
                read_lock_irqsave(&octeon_gpio_intclr_rwlock, flags);
            	gpio_intclr.u64 = 0;
            	gpio_intclr.u64 = cvmx_read64_uint64(GPIO_INT_CLR);
            	gpio_intclr.s.type = 0x20;
            	cvmx_write64_uint64(GPIO_INT_CLR, gpio_intclr.u64);
                intState1 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG1);
				udelay(1);
            	intState2 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG2);
				udelay(1);
            	intState3 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG3);
                read_lock_irqsave(&octeon_gpio_intclr_rwlock, flags);
    			
                ksemDBG("intState1 = %#lx\n", intState1);
            	ksemDBG("intState2 = %#lx\n", intState2);
            	ksemDBG("intState3 = %#lx\n", intState3);
    			
    			if(intState1) {
    			    check_on_intState1_for_ax86xx(intState1);
    			}
				if(intState2) {
    			    check_on_intState2_for_ax86xx(intState2);
    			}
				if(intState3) {
    			    check_on_intState3_for_ax86xx(intState3);
    			}
			}
			break;
		default:
			ksemDBG("Error product type [%#lx]\n", product_id);
			break;
    }
	
    ksemDBG("*********************end********************\n");
    return IRQ_HANDLED;
}

#endif

void check_on_hwState_for_ax86xx(void)
{
	unsigned long intState1, intState2, intState3;
	udelay(10000);
	
    intState1 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG1);
	//udelay(1);
	//intState2 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG2);
	udelay(1);
	intState3 = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_INT_STATE_REG3);

    ksemDBG("intState1 = %#lx\n", intState1);
	//ksemDBG("intState2 = %#lx\n", intState2);
	ksemDBG("intState3 = %#lx\n", intState3);
	if(intState1) {
	    check_on_intState1_for_ax86xx(intState1);
	}
	//if(intState2) {
	//    check_on_intState2_for_ax86xx(intState2);
	//}
	if(intState3) {
	    check_on_intState3_for_ax86xx(intState3);
	}
}




static void sem_poll_timer(unsigned long arg)
{	
    switch(product_id)
    {
		case PRODUCT_AX7605I:
			if(board_id == AX71_CRSMU)
			{
    			check_on_board_state_for_ax7605i();
    			check_on_hwState_for_ax7605i();
			}
			break;
		case PRODUCT_AX8610:
			if(board_id == AX81_CRSMU || board_id == AX81_CRSMUE)
			{
			    check_on_power_down_for_ax86xx();
				check_on_hwState_for_ax86xx();

			}
			break;
		case PRODUCT_AX8606:
			if(board_id == AX81_CRSMU || board_id == AX81_CRSMUE)
			{
			    check_on_power_down_for_ax86xx();
				check_on_hwState_for_ax86xx();

			}
			break;
		case PRODUCT_AX8603:
			if (board_id == AX81_AC12C || board_id == AX81_1X12G12S)
			{
			    //check_on_power_down_for_ax86xx();	
				check_on_hwState_for_ax86xx();
			}
			break;
		case PRODUCT_AX8800:
			if(board_id == AX81_CRSMU || board_id == AX81_CRSMUE)
			{
			    check_on_power_down_for_ax86xx();
				check_on_hwState_for_ax86xx();
			}
			break;
		default:
			ksemDBG("Error product type [%#lx]\n", product_id);
			break;
    }

    /* Repeat every second */
	mod_timer(&sem_devices->semTimer, jiffies + HZ);
}

static struct file_operations sem_fops = 
{
    .owner = THIS_MODULE,
};

/**
 * Set up the cdev structure for the device.
 */ 
static void sem_setup_cdev(sem_dev_t *dev)
{
	int err;
	dev_t dev_no = MKDEV(major_no, minor_no);

	cdev_init(&dev->cdev, &sem_fops);
	dev->cdev.owner = THIS_MODULE;
	err = cdev_add(&dev->cdev, dev_no, 1);
	if(err < 0)
        ksemERR("Error [%d] cdev_add failed.\n", err);
}

/**
 * Module/driver initialization. 
 * @return Zero on success
 */
static int __init sem_init_module(void)
{
    int result = 0;
	dev_t dev_no;
	int reg_data;
	
    if(major_no)
    {
    	dev_no = MKDEV(major_no, minor_no);
    	ksemDBG("Register major dev [%d].\n", MAJOR(dev_no));
        result = register_chrdev_region(dev_no, 1, DEVICE_NAME);
	}
	else
	{
        result = alloc_chrdev_region(&dev_no, minor_no, 1, DEVICE_NAME);
        major_no = MAJOR(dev_no);
	}
	
    if(result < 0)
    {
        ksemERR("Get major number Failed.\n");
        return result;
	}

    sem_devices = kmalloc(sizeof(struct sem_dev_s), GFP_KERNEL);
	if(!sem_devices)
	{
        result = ENOMEM;
		ksemERR("kmalloc failed.\n");
		goto fail_kmalloc;
	}

	sem_devices->irq = OCTEON_IRQ_GPIO0+5;
    sem_setup_cdev(sem_devices);
	reg_data = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_PRODUCT_ID_REG);
	product_id = (reg_data >> 2) & CPLD_PRODUCT_TYPE_MASK;
	board_id = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_BOARD_ID_REG);
	/*if (product_id == 0x7)
	{
		product_id = PRODUCT_AX8610;
	}*/

	if (product_id == 0x3)
	{
		product_id = PRODUCT_AX7605I;
	}
	ksemDBG("product_id = %#lx\n", product_id);
	ksemDBG("board_id = %#lx\n", board_id);
	
	init_board();
#if 1	
	/*wc change two macro*/	
	if ((product_id == PRODUCT_AX8610 || product_id == PRODUCT_AX8606 || product_id == PRODUCT_AX8603)&& board_id != AX81_AC_12X) {	
		result = request_irq(sem_devices->irq, sem_irq_interrupt, IRQF_SHARED | IRQF_DISABLED, DEVICE_NAME, (void*)sem_devices);
   
     	if(result)
     	{
             printk(KERN_ERR"Request_irq failed  [%d].\n", result);
     	}
     	else
     	{
             printk(KERN_ERR"Request_irq [%d].\n", sem_devices->irq);
     	}
	}
#endif	
   // init_timer(&sem_devices->semTimer);
	//sem_devices->semTimer.data = 0;
	//sem_devices->semTimer.function = sem_poll_timer;
	//sem_devices->semTimer.expires = jiffies + HZ;
	//add_timer(&sem_devices->semTimer);

	/*wc change two param*/ 
	nl_sk_ksem = netlink_kernel_create(&init_net, NETLINK_KSEM, 0, NULL, NULL, THIS_MODULE);   
    if(!nl_sk_ksem){   
        ksemERR("Failed to create netlink socket!\n");   
    }else {
	    ksemDBG("Create netlink socket OK.\n");
    }

	nl_sk_distributed = netlink_kernel_create(&init_net, NETLINK_DISTRIBUTED, 0, ksem_netlink_rcv, NULL, THIS_MODULE);   
    if(!nl_sk_distributed){   
        ksemERR("Failed to create netlink socket!\n");   
    }else {
	    ksemDBG("Create netlink socket OK.\n");
    }
	
	return 0;
	
fail_kmalloc:
    unregister_chrdev_region(dev_no, 1);
	
    return 0;
}

/**
 * The cleanup function is used to handle initialization failures as well. 
 * Thefore, it must be careful to work correctly even if some of the items 
 * have not been initialized 
 */
static void __exit sem_cleanup_module(void)
{
    dev_t dev_no;

	release_board();
	
	free_irq(sem_devices->irq, (void*)sem_devices);

    if(nl_sk_ksem != NULL)
		sock_release(nl_sk_ksem->sk_socket);

	if(nl_sk_distributed != NULL)
		sock_release(nl_sk_distributed->sk_socket);
	
	dev_no = MKDEV(major_no, minor_no);
	if(sem_devices)
	{
		del_timer(&sem_devices->semTimer);
        cdev_del(&sem_devices->cdev);
		kfree(sem_devices);
	}
	
    ksemDBG("Unregister the driver\n");
	unregister_chrdev_region(dev_no, 1);	
}


module_init(sem_init_module);
module_exit(sem_cleanup_module);

MODULE_AUTHOR("Autelan Corporation, <jialh@autelan.com>");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Kernel Sem Driver.");


