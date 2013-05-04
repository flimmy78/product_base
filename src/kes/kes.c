/**********************************************************************  
FILE NAME : kes.c
Author : libing
Version : V1.0
Date : 201109013 
Description : 
	用于保存内核异常信息到指定内存处，当内核异常
	发生重启后，可以查看重启前异常信息。
Dependence :
	uboot下需要申请预留内存，并且不能做初始化清零操作
Others : 
	
Function List :
1.kes_proc_init()
2.kes_proc_open()
3.kes_seq_start()
4.kes_seq_next()
5.kes_seq_show()
6.kes_seq_stop()
7.kes_addr_get()
8.print_msg_to_kes()
9.dump_msg_to_kes()
10.do_percentm()

History: 
1. Date:20110913
Author: libing
Modification: V1.0
**********************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/autelan_product.h>

#include "kes.h"
#include "cvmx-bootmem.h"
#include "cvmx.h"

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("<libing@autelan.com>");
MODULE_DESCRIPTION("Kernel Exception Saver Module");

extern product_info_t autelan_product_info;

extern int (*kes_mem_print_handle)(const char * fmt, ...);
extern int (*kes_mem_dump_handle)(char *buff, int size);
extern void (*print_current_time_handle)(void);

static char fbuf [CVT_BUF_MAX + 1] = {0};
unsigned char *kes_mem_addr = NULL;
unsigned char *kes_mem_print_addr = NULL;
kes_mem_header_type *kes_mem_header = NULL;
unsigned int kes_mem_offset = 0;

unsigned char *nvram_mem_addr  = NULL;
unsigned char *nvram_mem_log_addr = NULL;
unsigned int nvram_mem_offset = 0;
nvram_mem_header_t *nvram_mem_header = NULL;

/* Find %m in the input string and substitute an error message string. */
static int do_percentm (char *obuf, const char *ibuf)
{
	const char *s = ibuf;
	char *p = obuf;
	int infmt = 0;
	const char *m;
	int len = 0;

	while (*s) 
	{
		if (infmt)
		{
			if (*s == 'm') 
			{		
				m = "<format error>";
				
				len += strlen (m);
				if (len > CVT_BUF_MAX)
				{
					goto out;
				}
				strcpy (p - 1, m);
				p += strlen (p);
				++s;
			} 
			else 
			{
				if (++len > CVT_BUF_MAX)
					goto out;
				*p++ = *s++;
			}
			infmt = 0;
		}
		else
		{
			if (*s == '%')
				infmt = 1;
			if (++len > CVT_BUF_MAX)
				goto out;
			*p++ = *s++;
		}
	}
      out:
	*p = 0;
	return len;
}

int print_msg_to_nvram(const char * fmt,...)
{
	int msg_size = 0;
	int input_len = 0;
	static int is_first = 0;
	va_list args;
	struct timeval tv;
	struct rtc_time tm;
	char time_str[32] = {0};

	if (!is_first) {
		memset(nvram_mem_addr, 0, 32*1024);
		memcpy(nvram_mem_header->flag, "new", NVRAM_MEM_FLAG_LEN);
		do_gettimeofday(&tv);
		rtc_time_to_tm(tv.tv_sec, &tm);
		sprintf(time_str, "%d%02d%02d_%02d_%02d_%02d\n", tm.tm_year+1900,tm.tm_mon+1, tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
		memcpy(nvram_mem_header->time, time_str, NVRAM_MEM_TIME_LEN);
		is_first = 1;
	}

	input_len = do_percentm(fbuf, fmt);
	if(input_len > (NVRAM_MEM_SIZE - NVRAM_MEM_HEADER_LEN - nvram_mem_offset))
	{
		nvram_mem_offset = 0;
	}

	va_start(args, fmt);
	msg_size = vsnprintf((char *)(nvram_mem_log_addr + nvram_mem_offset), 
		NVRAM_MEM_SIZE - NVRAM_MEM_HEADER_LEN - nvram_mem_offset, fbuf, args);
	va_end(args);
	
	nvram_mem_offset += msg_size;

	return msg_size;
}

/********************************************************************** 
Function : 
	dump_msg_to_kes_mem
Description : 
	dump message to the kes mem
Input : 
	message buffer and size
Output : 
	none
Return :
	successfully print message length 
Others : 
	none
**********************************************************************/
int dump_msg_to_kes_mem(char *buff, int size )
{
	if(NULL == buff)
	{
		printk(KERN_INFO "kes input buffer pointer is NULL.\n");
		return 0;
	}
	
	//if(size > (KES_MEM_BLOCK_SIZE - kes_mem_offset - KES_MEM_HEADER_LEN))
	if(size > (HALF_KES_MEM_BLOCK_SIZE - kes_mem_offset - KES_MEM_HEADER_LEN))
	{
		kes_mem_offset = 0;
		return 0;
	}
	
	memcpy((kes_mem_addr + kes_mem_offset + KES_MEM_HEADER_LEN), buff, size);
	kes_mem_offset = kes_mem_offset + size;

	return size;

}

static void print_current_time_nvram(void)
{
	struct timeval tv;

	do_gettimeofday(&tv);
	print_msg_to_nvram("[%5lu.%5lu]", tv.tv_sec, tv.tv_usec);
}

static void print_current_time(void)
{
	struct timeval tv;

	do_gettimeofday(&tv);
	print_msg_to_kes_mem("[%5lu.%5lu]", tv.tv_sec, tv.tv_usec);
}

/********************************************************************** 
Function : 
	print_msg_to_kes_mem
Description : 
	print message to the kes_mem
Input : 
	message pointer
Output : 
	none
Return :
	successfully print message length 
Others : 
	none
**********************************************************************/
int print_msg_to_kes_mem(const char * fmt,...)
{
	int msg_size = 0;
	int input_len = 0;
	static int is_first = 0;
	va_list args;

	/*initialize the kes mem and fill the header*/
	if(!is_first)
	{
		//memset(kes_mem_addr, 0, KES_MEM_BLOCK_SIZE);
		memset(kes_mem_addr, 0, HALF_KES_MEM_BLOCK_SIZE);
		memcpy(kes_mem_header->magic, "autelan", KES_MAGIC_LEN);
		memcpy(kes_mem_header->isenable, "enable", KES_ISENABLE_LEN);
		is_first = 1;
	}

	input_len = do_percentm(fbuf, fmt);
	//if(input_len > (KES_MEM_BLOCK_SIZE - KES_MEM_HEADER_LEN - kes_mem_offset))
	if(input_len > (HALF_KES_MEM_BLOCK_SIZE - KES_MEM_HEADER_LEN - kes_mem_offset))
	{
		kes_mem_offset = 0;
	}

	va_start(args, fmt);
	//msg_size = vsnprintf((char *)(kes_mem_print_addr + kes_mem_offset), KES_MEM_BLOCK_SIZE - KES_MEM_HEADER_LEN - kes_mem_offset, fbuf, args);
	msg_size = vsnprintf((char *)(kes_mem_print_addr + kes_mem_offset), HALF_KES_MEM_BLOCK_SIZE - KES_MEM_HEADER_LEN - kes_mem_offset, fbuf, args);
	va_end(args);
	
	kes_mem_offset += msg_size;

	return msg_size;
}


/********************************************************************** 
Function : 
	kes_mem_addr_get
Description : 
	get kes mem the uboot alloced
Input : 
	void
Output : 
	kes_mem_addr
Return :
	kes_mem pointer
Others : 
	none
**********************************************************************/
static void *kes_mem_addr_get(void)
{
	cvmx_bootmem_named_block_desc_t *kes_mem_desc = NULL;

	kes_mem_desc = cvmx_bootmem_find_named_block(KES_MEM_BLOCK_NAME);
	if(NULL == kes_mem_desc)
	{
		printk(KERN_INFO "kes_mem get addr error.\n");
		return NULL;
	}

	kes_mem_addr = cvmx_phys_to_ptr(kes_mem_desc->base_addr);
	
	printk(KERN_INFO "%s found at: 0x%p, size: 0x%llx\n", 
			kes_mem_desc->name, (void *)kes_mem_desc->base_addr, kes_mem_desc->size);
	
	return kes_mem_addr;
	
}

/********************************************************************** 
Function : 
	kes_mem_seq_start
Description : 
	set the start position of seq file .
Input : 
	seq_file pointer, position pointer
Output : 
	the seq file start position
Return :
	start position pointer
Others : 
	none
**********************************************************************/
static void *kes_mem_seq_start(struct seq_file *seq, loff_t *pos)
{
	unsigned char *start = (unsigned char *)kes_mem_addr + 0x10;

	//if(*pos >= 256)  /*128 give to fastfwd logsave*/
	//if(*pos >= KES_POS_LEN)
	if (*pos >= 128)
	{
		*pos = 0;
		return NULL;
	}
	else
	{
		return (void *)(start+ (*pos) * KES_MEM_SHOW_LEN);
	}
}

/********************************************************************** 
Function : 
	kes_mem_seq_next
Description : 
	set the next position of seq file .
Input : 
	seq_file pointer,next pointer,  position pointer,
Output : 
	the seq file next position
Return :
	next position pointer
Others : 
	none
**********************************************************************/
static void *kes_mem_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	void *ptr_next = NULL;
	
	ptr_next = (void *)((unsigned char *)v + KES_MEM_SHOW_LEN);

	(*pos)++;

	return ptr_next;
	
}

/********************************************************************** 
Function : 
	kes_mem_seq_show
Description : 
	show the seq file .
Input : 
	seq_file pointer, data pointer,
Output : 
   none
Return :
   0:success; (-1):failed
Others : 
	none
**********************************************************************/
static int kes_mem_seq_show(struct seq_file *seq, void *v)
{
	int i = 0;
	void *pt = v;

	if(NULL == pt)
	{
		printk(KERN_INFO "kes_mem show data pointer NULL.\n");
		return -1;
	}

	for(i = 0; i < KES_MEM_SHOW_LEN; i++)
	{
		seq_printf(seq, "%c", *((unsigned char *)pt + i));
	}
	
	return 0;

} 

/********************************************************************** 
 Function : 
	 kes_mem_seq_stop
 Description : 
	 stop to show .
 Input : 
	 seq_file pointer, data pointer,
 Output : 
	none
 Return :
	void
 Others : 
	 none
 **********************************************************************/
static void  kes_mem_seq_stop(struct seq_file *seq, void *v)
{
	return;
}



struct seq_operations kes_mem_seq_ops = {
	.start = kes_mem_seq_start,
	.next  = kes_mem_seq_next,
	.show  = kes_mem_seq_show,
	.stop  = kes_mem_seq_stop,
};

/********************************************************************** 
Function : 
	kes_proc_open
Description : 
	open the kes_mem proc dir entry .
Input : 
	inode, file pointer
Output : 
	void
Return :
	0: success; !(0): failed
Others : 
	none
**********************************************************************/
static int kes_proc_open(struct inode *inode, struct file *file)
{
	int retval = -1;

	if(NULL == file)
	{
		printk(KERN_INFO "kes file pointer is NULL.\n");
		return retval;
	}

	retval = seq_open(file, &kes_mem_seq_ops);
	if(retval)
	{
		printk(KERN_INFO "kes cannot open seq_file.\n");
		remove_proc_entry(KES_PROC_NAME, NULL);
	}

	return retval;
}

static ssize_t kes_flag_proc_write(struct file *flip, const char __user *buff, unsigned long len, void *data)
{
	if(len > KES_ISENABLE_LEN)
	{
		printk(KERN_INFO "kes flag buffer is full.\n");
		return -ENOSPC;
	}

	memset(kes_mem_header->isenable, 0, KES_ISENABLE_LEN);

	if(copy_from_user(kes_mem_header->isenable, buff, len))
	{
		printk(KERN_INFO "kes flag copy_from_user error.\n");
		return -EFAULT;
	}

	return len;
	
}

static int kes_flag_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	unsigned char tmp[8] = {0};
	int len = 0;

	if(off > 0)
	{
		*eof = 1;
		return 0;
	}

	memset(page, 0, PAGE_SIZE);

	memcpy(tmp, kes_mem_header->isenable, KES_ISENABLE_LEN);
	len = sprintf(page, "%s", tmp);

	*eof = 1;
	
	return len;
	
}

/* fast-fwd group add for exception_msg logsave */
/********************************************************************** 
Function : 
	fwd_mem_seq_start
Description : 
	set the start position of seq file .
Input : 
	seq_file pointer, position pointer
Output : 
	the seq file start position
Return :
	start position pointer
Others : 
	none
**********************************************************************/
static void *fwd_mem_seq_start(struct seq_file *seq, loff_t *pos)
{
	if (NULL == kes_mem_addr)
	{
		kes_mem_addr_get();
	}
	unsigned char *start = (unsigned char *)kes_mem_addr + HALF_KES_MEM_BLOCK_SIZE + QUARTER_KES_MEM_BLOCK_SIZE;

	if(*pos >= 64)
	{
		*pos = 0;
		return NULL;
	}
	else
	{
		return (void *)(start+ (*pos) * KES_MEM_SHOW_LEN);
	}
}

/********************************************************************** 
Function : 
	fwd_mem_seq_next
Description : 
	set the next position of seq file .
Input : 
	seq_file pointer,next pointer,  position pointer,
Output : 
	the seq file next position
Return :
	next position pointer
Others : 
	none
**********************************************************************/
static void *fwd_mem_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	void *ptr_next = NULL;
	
	ptr_next = (void *)((unsigned char *)v + KES_MEM_SHOW_LEN);

	(*pos)++;

	return ptr_next;
	
}

/********************************************************************** 
Function : 
	fwd_mem_seq_show
Description : 
	show the seq file .
Input : 
	seq_file pointer, data pointer,
Output : 
   none
Return :
   0:success; (-1):failed
Others : 
	none
**********************************************************************/
static int fwd_mem_seq_show(struct seq_file *seq, void *v)
{
	int i = 0;
	void *pt = v;

	if(NULL == pt)
	{
		printk(KERN_INFO "fwd_mem show data pointer NULL.\n");
		return -1;
	}

	for(i = 0; i < KES_MEM_SHOW_LEN; i++)
	{
		seq_printf(seq, "%c", *((unsigned char *)pt + i));
	}
	
	return 0;

} 

/********************************************************************** 
 Function : 
	 fwd_mem_seq_stop
 Description : 
	 stop to show .
 Input : 
	 seq_file pointer, data pointer,
 Output : 
	none
 Return :
	void
 Others : 
	 none
 **********************************************************************/
static void  fwd_mem_seq_stop(struct seq_file *seq, void *v)
{
	return;
}

struct seq_operations fwd_mem_seq_ops = {
	.start = fwd_mem_seq_start,
	.next  = fwd_mem_seq_next,
	.show  = fwd_mem_seq_show,
	.stop  = fwd_mem_seq_stop,
};

static int fwd_proc_open(struct inode *inode, struct file *file)
{
	int retval = -1;

	if(NULL == file)
	{
		printk(KERN_INFO "fwd file pointer is NULL.\n");
		return retval;
	}

	retval = seq_open(file, &fwd_mem_seq_ops);
	if(retval)
	{
		printk(KERN_INFO "fwd cannot open seq_file.\n");
		remove_proc_entry(FWD_PROC_NAME, NULL);
	}

	return retval;
}

struct file_operations fwd_ops = {
	.owner   = THIS_MODULE,
	.open    = fwd_proc_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release_private,
};
/* fast-fwd group add for exception_msg logsave */

static int nvram_mem_show(struct seq_file *m, void *v)
{
	int i;
	unsigned char *nvram_mem_ptr = nvram_mem_addr;

	for (i = 0; i < NVRAM_MEM_SIZE; i++) {
		seq_printf(m, "%c", *(nvram_mem_ptr + i));
	}
	
	return 0;
}

static int nvram_mem_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, nvram_mem_show, NULL);
}

ssize_t kes_seq_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	ssize_t len = 0;
	len = seq_read(file, buf, size, ppos);
	memset(kes_mem_print_addr, 0, HALF_KES_MEM_BLOCK_SIZE);
	return len;
}

struct file_operations kes_ops = {
	.owner   = THIS_MODULE,
	.open    = kes_proc_open,
	.read    = kes_seq_read,
	.llseek  = seq_lseek,
	.release = seq_release_private,
};

ssize_t nvram_seq_read(struct file *file, char __user *buf, size_t size, loff_t *ppos)
{
	ssize_t len = 0;
	len = seq_read(file, buf, size, ppos);
	memset(nvram_mem_log_addr, 0, NVRAM_MEM_SIZE-NVRAM_MEM_HEADER_LEN);
	return len;
}

/* caojia add  */
static struct file_operations nvram_mem_proc_operations = {
	.owner = THIS_MODULE,
	.open = nvram_mem_proc_open,
	.read = nvram_seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static ssize_t nvram_mem_flag_proc_write(struct file *flip, const char __user *buff, unsigned long len, void *data)
{
	if(len > NVRAM_MEM_FLAG_LEN)
	{
		printk(KERN_INFO "nvram mem flag buffer is full.\n");
		return -ENOSPC;
	}

	memset(nvram_mem_header->flag, 0, NVRAM_MEM_FLAG_LEN);

	if(copy_from_user(nvram_mem_header->flag, buff, len))
	{
		printk(KERN_INFO "nvram mem flag copy_from_user error.\n");
		return -EFAULT;
	}

	return len;
	
}

static int nvram_mem_flag_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	unsigned char tmp[8] = {0};
	int len = 0;

	if(off > 0)
	{
		*eof = 1;
		return 0;
	}

	memset(page, 0, PAGE_SIZE);

	memcpy(tmp, nvram_mem_header->flag, NVRAM_MEM_FLAG_LEN);
	len = sprintf(page, "%s", tmp);

	*eof = 1;
	
	return len;
	
}

static int nvram_mem_time_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	unsigned char tmp[28] = {0};
	int len = 0;

	if(off > 0)
	{
		*eof = 1;
		return 0;
	}

	memset(page, 0, PAGE_SIZE);

	memcpy(tmp, nvram_mem_header->time, NVRAM_MEM_TIME_LEN);
	len = sprintf(page, "%s", tmp);

	*eof = 1;
	
	return len;
	
}


/********************************************************************** 
Function : 
	kes_proc_init
Description : 
	create the kes proc dir entry .
Input : 
	void
Output : 
	void
Return :
	0: success; (-1): failed
Others : 
	none
**********************************************************************/
static int kes_proc_init(void)
{
	int retval = 0;
	struct proc_dir_entry *kes_proc_entry = NULL;
	struct proc_dir_entry *kes_flag_entry = NULL;
	struct proc_dir_entry *nvram_mem_entry = NULL;
	struct proc_dir_entry *nvram_mem_flag_entry = NULL;
	struct proc_dir_entry *nvram_mem_time_entry = NULL;
	struct proc_dir_entry *fwd_proc_entry = NULL;

	if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S ||
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C ||
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC8C ||
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_4X ||
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_SMU) {
		/* caojia add nvram memory proc file */
		nvram_mem_entry = create_proc_entry("nvram_mem", 0, NULL);
		if (nvram_mem_entry){
			nvram_mem_entry->proc_fops = &nvram_mem_proc_operations;
		}
		else{
			printk(KERN_WARNING "Create nvram_mem failed.\n");
			retval = -1;
		}

		nvram_mem_flag_entry = create_proc_entry("nvram_mem_flag", 0666, NULL);
		if(nvram_mem_flag_entry)
		{
			nvram_mem_flag_entry->read_proc = nvram_mem_flag_proc_read;
			nvram_mem_flag_entry->write_proc = nvram_mem_flag_proc_write;
		}
		else
		{
			printk(KERN_WARNING "Create nvram_mem_flag failed.\n");
			remove_proc_entry("nvram_mem", NULL);
			retval = -1;
		}

		nvram_mem_time_entry = create_proc_entry("nvram_mem_time", 0444, NULL);
		if(nvram_mem_time_entry)
		{
			nvram_mem_time_entry->read_proc = nvram_mem_time_proc_read;
		}
		else
		{
			printk(KERN_WARNING "Create nvram_mem_time failed.\n");
			remove_proc_entry("nvram_mem", NULL);
			remove_proc_entry("nvram_mem_flag", NULL);
			retval = -1;
		}
	}
	else {
		kes_proc_entry = create_proc_entry(KES_PROC_NAME, 0, NULL);
		if(kes_proc_entry)
		{
			kes_proc_entry->proc_fops = &kes_ops;
			retval = 0;
		}
		else
		{
			printk(KERN_INFO "kes create %s error.\n", KES_PROC_NAME);
			retval = -1;
		}

		kes_flag_entry = create_proc_entry(KES_FLAG_NAME, 0666, NULL);
		if(kes_flag_entry)
		{
			kes_flag_entry->read_proc = kes_flag_proc_read;
			kes_flag_entry->write_proc = kes_flag_proc_write;
			//kes_flag_entry->owner = THIS_MODULE;
		}
		else
		{
			printk(KERN_INFO "kes create %s error.\n", KES_FLAG_NAME);
			remove_proc_entry(KES_PROC_NAME, NULL);
			retval = -1;
		}
	}

	/* fast-fwd group add for exception_msg logsave */
	
	fwd_proc_entry = create_proc_entry(FWD_PROC_NAME, 0, NULL);
	if(fwd_proc_entry)
	{
		fwd_proc_entry->proc_fops = &fwd_ops;
		retval = 0;
	}
	else
	{
		printk(KERN_INFO "fwd create %s error.\n", FWD_PROC_NAME);
		retval = -1;
	}
	
	return retval;
	
}

static int __init kes_init(void)
{
	if (autelan_product_info.board_type == AUTELAN_BOARD_AX81_1X12G12S ||
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC12C ||
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC8C ||
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_AC_4X ||
		autelan_product_info.board_type == AUTELAN_BOARD_AX81_SMU) {

		nvram_mem_addr = 0x800000001d020000;
		nvram_mem_header = (nvram_mem_header_t *)nvram_mem_addr;
		nvram_mem_log_addr = nvram_mem_addr + NVRAM_MEM_HEADER_LEN;

		kes_mem_print_handle = print_msg_to_nvram;
		print_current_time_handle = print_current_time_nvram;
	}
	else {
		if(!kes_mem_addr_get())
		{
			return -1;
		}

		kes_mem_header = (kes_mem_header_type *)kes_mem_addr;
		kes_mem_print_addr = kes_mem_addr + KES_MEM_HEADER_LEN;

		kes_mem_print_handle = print_msg_to_kes_mem;
		kes_mem_dump_handle = dump_msg_to_kes_mem;
		print_current_time_handle = print_current_time;
	}

	if(kes_proc_init())
	{
		return -1;
	}

	printk(KERN_INFO "kes module loaded.\n");
	
	return 0;

}

static void __exit kes_exit(void)
{
	kes_mem_print_handle = NULL;
	kes_mem_dump_handle = NULL;
	print_current_time_handle = NULL;
	
	remove_proc_entry(KES_PROC_NAME, NULL);
	remove_proc_entry(KES_FLAG_NAME, NULL);
	remove_proc_entry("nvram_mem", NULL);
	remove_proc_entry("nvram_mem_flag", NULL);
	remove_proc_entry("nvram_mem_time", NULL);
	remove_proc_entry(FWD_PROC_NAME, NULL);

	printk(KERN_INFO "kes module unloaded.\n");

}


module_init(kes_init);
module_exit(kes_exit);

