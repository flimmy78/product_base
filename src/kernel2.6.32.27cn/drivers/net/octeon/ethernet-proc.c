/*************************************************************************
* Cavium Octeon Ethernet Driver
*
* Author: Cavium Networks info@caviumnetworks.com
*
* Copyright (c) 2003-2010  Cavium Networks (support@cavium.com). All rights
* reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*
*     * Redistributions in binary form must reproduce the above
*       copyright notice, this list of conditions and the following
*       disclaimer in the documentation and/or other materials provided
*       with the distribution.
*
*     * Neither the name of Cavium Networks nor the names of
*       its contributors may be used to endorse or promote products
*       derived from this software without specific prior written
*       permission.
*
 TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM NETWORKS MAKES NO PROMISES, REPRESENTATIONS
* OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH
* RESPECT TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET
* POSSESSION OR CORRESPONDENCE TO DESCRIPTION.  THE ENTIRE RISK ARISING OUT
* OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
*************************************************************************/
#include <linux/kernel.h>
#include <linux/phy.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <net/dst.h>

#include "octeon-ethernet.h"
#include "ethernet-proc.h"
#include "ethernet-ioctl.h"

extern cvmx_bootinfo_t *octeon_bootinfo;

static unsigned long long cvm_oct_stats_read_switch(struct mii_bus *mii_bus, int phy_id, int offset)
{
	mii_bus->write(mii_bus, phy_id, 0x1d, 0xcc00 | offset);
	return ((u64)mii_bus->read(mii_bus, phy_id, 0x1e)<<16) | (u64)mii_bus->read(mii_bus, phy_id, 0x1f);
}

static int cvm_oct_stats_switch_show(struct octeon_ethernet *priv, struct seq_file *m, void *v)
{
	static const int ports[] = {0, 1, 2, 3, 9, -1};
	int index = 0;

	while (ports[index] != -1) {

		/* Latch port */
		struct mii_bus *mii_bus = priv->mii_bus;
		if (mii_bus == NULL)
			break;
		mutex_lock(&mii_bus->mdio_lock);

		priv->mii_bus->write(mii_bus, 0x1b, 0x1d, 0xdc00 | ports[index]);
		seq_printf(m, "\nSwitch Port %d\n", ports[index]);
		seq_printf(m, "InGoodOctets:   %12llu\t"
			   "OutOctets:      %12llu\t"
			   "64 Octets:      %12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x00) | (cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x01)<<32),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x0E) | (cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x0F)<<32),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x08));

		seq_printf(m, "InBadOctets:    %12llu\t"
			   "OutUnicast:     %12llu\t"
			   "65-127 Octets:  %12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x02),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x10),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x09));

		seq_printf(m, "InUnicast:      %12llu\t"
			   "OutBroadcasts:  %12llu\t"
			   "128-255 Octets: %12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x04),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x13),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x0A));

		seq_printf(m, "InBroadcasts:   %12llu\t"
			   "OutMulticasts:  %12llu\t"
			   "256-511 Octets: %12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x06),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x12),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x0B));

		seq_printf(m, "InMulticasts:   %12llu\t"
			   "OutPause:       %12llu\t"
			   "512-1023 Octets:%12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x07),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x15),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x0C));

		seq_printf(m, "InPause:        %12llu\t"
			   "Excessive:      %12llu\t"
			   "1024-Max Octets:%12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x16),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x11),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x0D));

		seq_printf(m, "InUndersize:    %12llu\t"
			   "Collisions:     %12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x18),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x1E));

		seq_printf(m, "InFragments:    %12llu\t"
			   "Deferred:       %12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x19),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x05));

		seq_printf(m, "InOversize:     %12llu\t"
			   "Single:         %12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x1A),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x14));

		seq_printf(m, "InJabber:       %12llu\t"
			   "Multiple:       %12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x1B),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x17));

		seq_printf(m, "In RxErr:       %12llu\t"
			   "OutFCSErr:      %12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x1C),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x03));

		seq_printf(m, "InFCSErr:       %12llu\t"
			   "Late:           %12llu\n",
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x1D),
			   cvm_oct_stats_read_switch(mii_bus, 0x1b, 0x1F));
		index++;
		mutex_unlock(&mii_bus->mdio_lock);
	}
	return 0;
}

/*
  *
  *return value:0 for success; 1 for failed
  *
  */
int reset_if_statistics(if_sta_statistics_str *tmp_ptr)
{
	unsigned char i, flag = 0;
	struct octeon_ethernet *priv;
		list_for_each_entry(priv, &cvm_oct_list, list) {
			if (strcmp(tmp_ptr->dev_name, priv->netdev->name) == 0) {
				for (i=0; i<tmp_ptr->param_count; i++) {
					if (strcmp(tmp_ptr->name_value_arry[i].name, "rx_packets") == 0) {
						priv->netdev->stats.rx_packets = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "tx_packets") == 0) {
						priv->netdev->stats.tx_packets = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "rx_bytes") == 0) {
						priv->netdev->stats.rx_bytes = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "tx_bytes") == 0) {
						priv->netdev->stats.tx_bytes = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "rx_errors") == 0) {
						priv->netdev->stats.rx_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "tx_errors") == 0) {
						priv->netdev->stats.tx_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "rx_dropped") == 0) {
						priv->netdev->stats.rx_dropped = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "tx_dropped") == 0) {
						priv->netdev->stats.tx_dropped = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "rx_length_errors") == 0) {
						priv->netdev->stats.rx_length_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "tx_aborted_errors") == 0) {
						priv->netdev->stats.tx_aborted_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "rx_over_errors") == 0) {
						priv->netdev->stats.rx_over_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "tx_carrier_errors") == 0) {
						priv->netdev->stats.tx_carrier_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "rx_crc_errors") == 0) {
						priv->netdev->stats.rx_crc_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "tx_fifo_errors") == 0) {
						priv->netdev->stats.tx_fifo_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "rx_frame_errors") == 0) {
						priv->netdev->stats.rx_frame_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "tx_heartbeat_errors") == 0) {
						priv->netdev->stats.tx_heartbeat_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "rx_fifo_errors") == 0) {
						priv->netdev->stats.rx_fifo_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "tx_window_errors") == 0) {
						priv->netdev->stats.tx_window_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "rx_missed_errors") == 0) {
						priv->netdev->stats.rx_missed_errors = tmp_ptr->name_value_arry[i].value;
					} else if (strcmp(tmp_ptr->name_value_arry[i].name, "multicast") == 0) {
						priv->netdev->stats.multicast = tmp_ptr->name_value_arry[i].value;
					} else {
						printk("bad parameters\n");
						flag = 1;
					}
				}
			}
		}
		return flag;
}

/**
 * User is reading /proc/octeon_ethernet_stats
 *
 * @param m
 * @param v
 * @return
 */
static int cvm_oct_stats_show(struct seq_file *m, void *v)
{
	struct octeon_ethernet *priv;
	list_for_each_entry(priv, &cvm_oct_list, list) {
		seq_printf(m, "\nOcteon Port %d (%s)\n", priv->ipd_port, priv->netdev->name);
		seq_printf(m,
			   "rx_packets:             %12lu\t"
			   "tx_packets:             %12lu\n",
			   priv->netdev->stats.rx_packets,
			   priv->netdev->stats.tx_packets);
		seq_printf(m,
			   "rx_bytes:               %12lu\t"
			   "tx_bytes:               %12lu\n",
			   priv->netdev->stats.rx_bytes,
			   priv->netdev->stats.tx_bytes);
		seq_printf(m,
			   "rx_errors:              %12lu\t"
			   "tx_errors:              %12lu\n",
			   priv->netdev->stats.rx_errors,
			   priv->netdev->stats.tx_errors);
		seq_printf(m,
			   "rx_dropped:             %12lu\t"
			   "tx_dropped:             %12lu\n",
			   priv->netdev->stats.rx_dropped,
			   priv->netdev->stats.tx_dropped);
		seq_printf(m,
			   "rx_length_errors:       %12lu\t"
			   "tx_aborted_errors:      %12lu\n",
			   priv->netdev->stats.rx_length_errors,
			   priv->netdev->stats.tx_aborted_errors);
		seq_printf(m,
			   "rx_over_errors:         %12lu\t"
			   "tx_carrier_errors:      %12lu\n",
			   priv->netdev->stats.rx_over_errors,
			   priv->netdev->stats.tx_carrier_errors);
		seq_printf(m,
			   "rx_crc_errors:          %12lu\t"
			   "tx_fifo_errors:         %12lu\n",
			   priv->netdev->stats.rx_crc_errors,
			   priv->netdev->stats.tx_fifo_errors);
		seq_printf(m,
			   "rx_frame_errors:        %12lu\t"
			   "tx_heartbeat_errors:    %12lu\n",
			   priv->netdev->stats.rx_frame_errors,
			   priv->netdev->stats.tx_heartbeat_errors);
		seq_printf(m,
			   "rx_fifo_errors:         %12lu\t"
			   "tx_window_errors:       %12lu\n",
			   priv->netdev->stats.rx_fifo_errors,
			   priv->netdev->stats.tx_window_errors);
		seq_printf(m,
			   "rx_missed_errors:       %12lu\t"
			   "multicast:              %12lu\n",
			   priv->netdev->stats.rx_missed_errors,
			   priv->netdev->stats.multicast);
	}

	priv = list_first_entry(&cvm_oct_list, struct octeon_ethernet, list);
	if ((priv->ipd_port == 0) && (priv->imode == CVMX_HELPER_INTERFACE_MODE_GMII))
		cvm_oct_stats_switch_show(priv, m, v);
	return 0;
}

static int cvm_oct_coremask_show(struct seq_file *m, void *v)
{

	int core_mask = octeon_bootinfo->core_mask;
	seq_printf(m, "%#04x",core_mask);
	
	return 0;
}

static int cvm_oct_coremask_all_show(struct seq_file *m, void *v)
{ 
	unsigned long long core_mask = 1;
	char *core_mask_ptr = NULL;
	
	core_mask = (0xffffffffULL & cvmx_read_csr(READ_FUSE_COREMASK));
	
	switch(core_mask)
	{		
		case 4294967295:	/* 32 core */
			core_mask_ptr = "0xffffffff";
			break;	
		case 65535:			/* 16 core */
			core_mask_ptr = "0xffff";
			break;	
		case 4095:			/* 12 core */
			core_mask_ptr = "0xfff";
			break;
		case 255:			/* 8 core */
			core_mask_ptr = "0xff";
			break;		
		case 15:			/*4 core*/
			core_mask_ptr= "0xf";
			break;		
		case 3:				/* 2 core */
			core_mask_ptr= "0x3";
			break;		
		case 1:				/*1 core*/
			core_mask_ptr= "0x1";
			break;
		default:
			printk(KERN_ERR "The core_mask from fuse register is wrong\n");
			break;	
	}

	seq_printf(m, "%s",core_mask_ptr);
	
	return 0;
}

static int cvm_oct_coremask_se_show(struct seq_file *m, void *v)
{
    unsigned int core_mask_se = (0xffffULL & cvmx_read_csr(READ_FUSE_COREMASK)) - octeon_bootinfo->core_mask;
    seq_printf(m, "%#04x",core_mask_se);
	return 0;
}


/**
 * /proc/octeon_ethernet_stats was openned. Use the single_open iterator
 *
 * @param inode
 * @param file
 * @return
 */
static int cvm_oct_stats_open(struct inode *inode, struct file *file)
{
	return single_open(file, cvm_oct_stats_show, NULL);
}

static int cvm_oct_coremask_open(struct inode *inode, struct file *file)
{
    return single_open(file, cvm_oct_coremask_show, NULL);
}

static int cvm_oct_coremask_all_open(struct inode *inode, struct file *file)
{
    return single_open(file, cvm_oct_coremask_all_show, NULL);
}

static int cvm_oct_coremask_se_open(struct inode *inode, struct file *file)
{
    return single_open(file, cvm_oct_coremask_se_show, NULL);
}

static const struct file_operations cvm_oct_stats_operations = {
	.open		= cvm_oct_stats_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static const struct file_operations cvm_oct_coremask_operations = {
	.open		= cvm_oct_coremask_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static const struct file_operations cvm_oct_coremask_all_operations = {
	.open		= cvm_oct_coremask_all_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static const struct file_operations cvm_oct_coremask_se_operations = {
	.open		= cvm_oct_coremask_se_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};

void cvm_oct_proc_initialize(void)
{
	struct proc_dir_entry *coremask_entry = NULL;
	struct proc_dir_entry *coremask_all_entry = NULL;
	struct proc_dir_entry *coremask_se_entry = NULL;
	struct proc_dir_entry *entry = NULL;

	entry = create_proc_entry("octeon_ethernet_stats", 0, NULL);
	if (entry)
		entry->proc_fops = &cvm_oct_stats_operations;
	
	//huangjing add for /proc/coremask
	coremask_entry = create_proc_entry("coremask", 0, NULL);
	if (coremask_entry)
		coremask_entry->proc_fops = &cvm_oct_coremask_operations;
	else
        printk(KERN_ERR "creat /proc/coremask fail!\n");
	
	coremask_all_entry = create_proc_entry("coremask_all", 0, NULL);
	if (coremask_all_entry)
		coremask_all_entry->proc_fops = &cvm_oct_coremask_all_operations;
	else
        printk(KERN_ERR "creat /proc/coremask fail!\n");

	coremask_se_entry = create_proc_entry("coremask_se", 0, NULL);
	if (coremask_se_entry)
		coremask_se_entry->proc_fops = &cvm_oct_coremask_se_operations;
	else
        printk(KERN_ERR "creat /proc/coremask fail!\n");
	
}

void cvm_oct_proc_shutdown(void)
{
	remove_proc_entry("octeon_ethernet_stats", NULL);
}

