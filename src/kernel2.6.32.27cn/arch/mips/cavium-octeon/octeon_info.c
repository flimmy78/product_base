/*
 * Simple /proc interface to Octeon Information
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004-2007 Cavium Networks
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <asm/octeon/octeon.h>
#include <asm/octeon/cvmx-app-init.h>
#include <linux/autelan_product.h>

extern cvmx_bootinfo_t *octeon_bootinfo;
#if CONFIG_CAVIUM_RESERVE32
extern uint64_t octeon_reserve32_memory;
#endif
extern product_info_t autelan_product_info;

static struct proc_dir_entry *proc_autelan_product;

/**
 * User is reading /proc/octeon_info
 *
 * @param m
 * @param v
 * @return
 */
static int octeon_info_show(struct seq_file *m, void *v)
{

	seq_printf(m, "processor_id:        0x%x\n", read_c0_prid());
	seq_printf(m, "boot_flags:          0x%x\n", octeon_bootinfo->flags);
	seq_printf(m, "dram_size:           %u\n", octeon_bootinfo->dram_size);
	seq_printf(m, "phy_mem_desc_addr:   0x%x\n",
		   octeon_bootinfo->phy_mem_desc_addr);
	seq_printf(m, "eclock_hz:           %u\n", octeon_bootinfo->eclock_hz);
	seq_printf(m, "io_clock_hz:         %llu\n", octeon_get_io_clock_rate());
	seq_printf(m, "dclock_hz:           %u\n", octeon_bootinfo->dclock_hz);
	seq_printf(m, "board_type:          %u\n", octeon_bootinfo->board_type);
	seq_printf(m, "board_rev_major:     %u\n",
		   octeon_bootinfo->board_rev_major);
	seq_printf(m, "board_rev_minor:     %u\n",
		   octeon_bootinfo->board_rev_minor);
	seq_printf(m, "board_serial_number: %s\n",
		   octeon_bootinfo->board_serial_number);
	seq_printf(m, "mac_addr_base:       %02x:%02x:%02x:%02x:%02x:%02x\n",
		   (int) octeon_bootinfo->mac_addr_base[0],
		   (int) octeon_bootinfo->mac_addr_base[1],
		   (int) octeon_bootinfo->mac_addr_base[2],
		   (int) octeon_bootinfo->mac_addr_base[3],
		   (int) octeon_bootinfo->mac_addr_base[4],
		   (int) octeon_bootinfo->mac_addr_base[5]);
	seq_printf(m, "mac_addr_count:      %u\n",
		   octeon_bootinfo->mac_addr_count);
#if CONFIG_CAVIUM_RESERVE32
	seq_printf(m, "32bit_shared_mem_base: 0x%lx\n",
		   (long int) octeon_reserve32_memory);
	seq_printf(m, "32bit_shared_mem_size: 0x%x\n",
		   (long int) octeon_reserve32_memory ? CONFIG_CAVIUM_RESERVE32 << 20 : 0);
#else
	seq_printf(m, "32bit_shared_mem_base: 0x%lx\n", 0ul);
	seq_printf(m, "32bit_shared_mem_size: 0x%x\n", 0);
#endif
	return 0;
}

static int autelan_product_type_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", autelan_product_info.product_type);
	return 0;
}

static int autelan_board_type_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", autelan_product_info.board_type);
	return 0;
}

static int autelan_product_series_num_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", autelan_product_info.product_series_num);
	return 0;
}

static int autelan_product_type_num_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", autelan_product_info.product_type_num);
	return 0;
}

static int autelan_board_type_num_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", autelan_product_info.board_type_num);
	return 0;
}

static int autelan_board_slot_id_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", autelan_product_info.board_slot_id);
	return 0;
}

static int autelan_distributed_product_show(struct seq_file *m, void *v)
{
	seq_printf(m, "%d\n", autelan_product_info.distributed_product);
	return 0;
}

/**
 * /proc/octeon_info was openned. Use the single_open iterator
 *
 * @param inode
 * @param file
 * @return
 */
static int octeon_info_open(struct inode *inode, struct file *file)
{
	return single_open(file, octeon_info_show, NULL);
}

static int autelan_product_type_open(struct inode *inode, struct file *file)
{
	return single_open(file, autelan_product_type_show, NULL);
}

static int autelan_board_type_open(struct inode *inode, struct file *file)
{
	return single_open(file, autelan_board_type_show, NULL);
}

static int autelan_product_series_num_open(struct inode *inode, struct file *file)
{
	return single_open(file, autelan_product_series_num_show, NULL);
}

static int autelan_product_type_num_open(struct inode *inode, struct file *file)
{
	return single_open(file, autelan_product_type_num_show, NULL);
}

static int autelan_board_type_num_open(struct inode *inode, struct file *file)
{
	return single_open(file, autelan_board_type_num_show, NULL);
}

static int autelan_board_slot_id_open(struct inode *inode, struct file *file)
{
	return single_open(file, autelan_board_slot_id_show, NULL);
}

static int autelan_distributed_product_open(struct inode *inode, struct file *file)
{
	return single_open(file, autelan_distributed_product_show, NULL);
}

static struct file_operations octeon_info_operations = {
	.open = octeon_info_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations autelan_product_type_operations = {
	.open = autelan_product_type_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations autelan_board_type_operations = {
	.open = autelan_board_type_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations autelan_product_series_num_operations = {
	.open = autelan_product_series_num_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations autelan_product_type_num_operations = {
	.open = autelan_product_type_num_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations autelan_board_type_num_operations = {
	.open = autelan_board_type_num_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations autelan_board_slot_id_operations = {
	.open = autelan_board_slot_id_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static struct file_operations autelan_distributed_product_operations = {
	.open = autelan_distributed_product_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int octeon_autelan_product_info_proc_init(void)
{
	struct proc_dir_entry *autelan_product_type_entry = NULL;
	struct proc_dir_entry *autelan_board_type_entry = NULL;
	struct proc_dir_entry *autelan_product_series_num_entry = NULL;
	struct proc_dir_entry *autelan_product_type_num_entry = NULL;
	struct proc_dir_entry *autelan_board_type_num_entry = NULL;
	struct proc_dir_entry *autelan_board_slot_id_entry = NULL;
	struct proc_dir_entry *autelan_distributed_product_entry = NULL;

	pr_info("octeon_autelan_product_info_proc_init\n");
	
	proc_autelan_product = proc_mkdir("product_info", NULL);
	if (proc_autelan_product == NULL){
		pr_warning("Proc mkdir failed.\n");
		return -1;
	}

	autelan_product_type_entry = create_proc_entry("product_type", 0, proc_autelan_product);
	if (autelan_product_type_entry){
		autelan_product_type_entry->proc_fops = &autelan_product_type_operations;
	}
	else{
		pr_warning("Create autelan_product_type_entry failed.\n");
		return -1;
	}

	autelan_board_type_entry = create_proc_entry("board_type", 0, proc_autelan_product);
	if (autelan_board_type_entry){
		autelan_board_type_entry->proc_fops = &autelan_board_type_operations;
	}
	else{
		pr_warning("Create autelan_board_type_entry failed.\n");
		return -1;
	}

	autelan_product_series_num_entry = create_proc_entry("product_series_num", 0, proc_autelan_product);
	if (autelan_product_series_num_entry){
		autelan_product_series_num_entry->proc_fops = &autelan_product_series_num_operations;
	}
	else{
		pr_warning("Create autelan_product_series_num_entry failed.\n");
		return -1;
	}

	autelan_product_type_num_entry = create_proc_entry("product_type_num", 0, proc_autelan_product);
	if (autelan_product_type_num_entry){
		autelan_product_type_num_entry->proc_fops = &autelan_product_type_num_operations;
	}
	else{
		pr_warning("Create autelan_product_type_num_entry failed.\n");
		return -1;
	}

	autelan_board_type_num_entry = create_proc_entry("board_type_num", 0, proc_autelan_product);
	if (autelan_board_type_num_entry){
		autelan_board_type_num_entry->proc_fops = &autelan_board_type_num_operations;
	}
	else{
		pr_warning("Create autelan_board_type_num_entry failed.\n");
		return -1;
	}

	autelan_board_slot_id_entry = create_proc_entry("board_slot_id", 0, proc_autelan_product);
	if (autelan_board_slot_id_entry){
		autelan_board_slot_id_entry->proc_fops = &autelan_board_slot_id_operations;
	}
	else{
		pr_warning("Create autelan_board_slot_id_entry failed.\n");
		return -1;
	}

	autelan_distributed_product_entry = create_proc_entry("distributed_product", 0, proc_autelan_product);
	if (autelan_distributed_product_entry){
		autelan_distributed_product_entry->proc_fops = &autelan_distributed_product_operations;
	}
	else{
		pr_warning("Create distributed_product_entry failed.\n");
		return -1;
	}

	return 0;
}

/**
 * Module initialization
 *
 * @return
 */
static int __init octeon_info_init(void)
{
	struct proc_dir_entry *entry =
		create_proc_entry("octeon_info", 0, NULL);
	if (entry == NULL)
		return -1;

	entry->proc_fops = &octeon_info_operations;

	/* caojia@autelan.com add for create autelan product information proc directory and files */
	octeon_autelan_product_info_proc_init();
	
	return 0;
}


/**
 * Module cleanup
 *
 * @return
 */
static void __exit octeon_info_cleanup(void)
{
	remove_proc_entry("octeon_info", NULL);
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cavium Networks <support@caviumnetworks.com>");
MODULE_DESCRIPTION("Cavium Networks Octeon information interface.");
module_init(octeon_info_init);
module_exit(octeon_info_cleanup);
