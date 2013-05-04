/*************************************************************************
* Cavium Octeon Ethernet Driver
*
* Author: Cavium Networks info@caviumnetworks.com
*
* Copyright (c) 2003-2007  Cavium Networks (support@cavium.com). All rights
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
* This Software, including technical data, may be subject to U.S.  export
* control laws, including the U.S.  Export Administration Act and its
* associated regulations, and may be subject to export or import regulations
* in other countries.  You warrant that You will comply strictly in all
* respects with all such regulations and acknowledge that you have the
* responsibility to obtain licenses to export, re-export or import the
* Software.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
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
//#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/mii.h>
#include <net/dst.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/autelan_product.h>

//#include "wrapper-cvmx-includes.h"
//#include "ethernet-headers.h"
#include "ethernet-common.h"
/*
extern cvmx_bootinfo_t *octeon_bootinfo;
extern int pow_send_group;
extern int always_use_pow;
extern char pow_send_list[];
extern int DSA_port_Num ;
extern char local_slotNum;
extern char is_distributed;
*/
extern product_info_t autelan_product_info;
extern struct sock *nl_sk ;
char *mac_filename="/devinfo/mac"; 
char *base_mac_filename="/devinfo/base_mac"; 

#if 0

static char *obc_name = "obc";
static unsigned char obc_mac_base[6] = {0x00, 0x1f, 0x64, 0xff, 0xff, 0xf0};

extern struct sock *nl_sk;
extern port_counter_t cvm_stats[];
#endif
extern int cvmx_mdio_x_write(int bus_id, int phy_id, int reg_addr, unsigned int location, int value);
extern int cvmx_mdio_x_read(int bus_id, int phy_id, unsigned int reg_addr, int location);

void printPacketBuffer(unsigned char *buffer,unsigned long buffLen)
{
	unsigned int i;

	if(!buffer)
		return;
	printk(":::::::::::::::::::::::::::::::::::::::::::::::\n");
	
	for(i = 0;i < buffLen ; i++)
	{
		printk("%02x ",buffer[i]);
		if(0==(i+1)%16) {
			printk("\n");
		}
	}
	if((buffLen%16)!=0)
	{
		printk("\n");
	}
	printk(":::::::::::::::::::::::::::::::::::::::::::::::\n");
}

int cvm_read_from_file(char *filename, char *buff, int len)
{
	int fd;
	mm_segment_t old_fs = get_fs();

	if((filename == NULL) || (buff == NULL))
	{
		return -1;
	}
	
	set_fs(KERNEL_DS);
	
	fd = sys_open(filename, O_RDONLY, 0);
	if (fd >= 0) 
	{
	    if(sys_read(fd, buff, len) == len)
		{
		    sys_close(fd);
	    	set_fs(old_fs);
			return 0;
	    }
		else
		{
			pr_err("Can not READ out MAC info.\n");
		    sys_close(fd);
	    	set_fs(old_fs);
			return -1;
		}
	}
	else
	{
		pr_err("Open file:%s failed!\n",filename);
		set_fs(old_fs);
		return -1;
	}

}

int cvm_get_mac_addr(unsigned char *mac_addr)
{
	int j;
	char tmp_mac_chars[13];
	unsigned char temp_2char[3] = {0x00, 0x00, 0x00};
	
	memset(tmp_mac_chars, 0x00, 13);
	if (0 == cvm_read_from_file(base_mac_filename, tmp_mac_chars, 12)) {
		pr_info("read Base MAC string from /devinfo/mac is: %s.\n", tmp_mac_chars);
	}
	else if(0 == cvm_read_from_file(mac_filename, tmp_mac_chars, 12))
	{
		pr_info("read MAC string from /devinfo/mac is: %s.\n", tmp_mac_chars);
	}
	else
	{
		return -1;
	}

	for (j=0; j<12; j+=2)
	{
		memcpy(temp_2char, tmp_mac_chars+j, 2);
		mac_addr[j/2] = (unsigned char)simple_strtoul((char *)temp_2char, 0, 16);
	}

	return 0;
}
EXPORT_SYMBOL(cvm_get_mac_addr);

int cvm_get_local_mac_addr(unsigned char *mac_addr)
{
	if (0 == cvm_get_mac_addr(mac_addr))
	{
		if (autelan_product_info.board_slot_id > 0)
		{
			mac_addr[5] += (autelan_product_info.board_slot_id - 1);
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}
EXPORT_SYMBOL(cvm_get_local_mac_addr);

int cvm_get_slot_mac_addr(unsigned char *mac_addr, unsigned int slot_num)
{
	if (0 == cvm_get_mac_addr(mac_addr))
	{
		if (slot_num > 0)
		{
			mac_addr[5] += (slot_num - 1);
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}

	return 0;
}
EXPORT_SYMBOL(cvm_get_slot_mac_addr);
#if 0
static void cvm_oct_common_show_stats
(
    struct net_device *dev,
    cvmx_pip_port_status_t rx_status,
    cvmx_pko_port_status_t tx_status
)
{
    cvm_stats[dev->ifindex].pip_stats.dropped_octets  += rx_status.dropped_octets;         /**< Inbound octets marked to be dropped by the IPD */
    cvm_stats[dev->ifindex].pip_stats.dropped_packets += rx_status.dropped_packets;        /**< Inbound packets marked to be dropped by the IPD */
    cvm_stats[dev->ifindex].pip_stats.pci_raw_packets += rx_status.pci_raw_packets;        /**< RAW PCI Packets received by PIP per port */
    cvm_stats[dev->ifindex].pip_stats.octets          += rx_status.octets;                 /**< Number of octets processed by PIP */
    cvm_stats[dev->ifindex].pip_stats.packets += rx_status.packets;                        /**< Number of packets processed by PIP */
    cvm_stats[dev->ifindex].pip_stats.multicast_packets += rx_status.multicast_packets;    /**< Number of indentified L2 multicast packets.
                                                                                                Does not include broadcast packets.
                                                                                                Only includes packets whose parse mode is
                                                                                                SKIP_TO_L2 */
    cvm_stats[dev->ifindex].pip_stats.broadcast_packets += rx_status.broadcast_packets;    /**< Number of indentified L2 broadcast packets.
                                                                                                Does not include multicast packets.
                                                                                                Only includes packets whose parse mode is
                                                                                                SKIP_TO_L2 */
    cvm_stats[dev->ifindex].pip_stats.len_64_packets += rx_status.len_64_packets;                /**< Number of 64B packets */
    cvm_stats[dev->ifindex].pip_stats.len_65_127_packets += rx_status.len_65_127_packets;        /**< Number of 65-127B packets */
    cvm_stats[dev->ifindex].pip_stats.len_128_255_packets += rx_status.len_128_255_packets;      /**< Number of 128-255B packets */
    cvm_stats[dev->ifindex].pip_stats.len_256_511_packets += rx_status.len_256_511_packets;      /**< Number of 256-511B packets */
    cvm_stats[dev->ifindex].pip_stats.len_512_1023_packets+= rx_status.len_512_1023_packets;     /**< Number of 512-1023B packets */
    cvm_stats[dev->ifindex].pip_stats.len_1024_1518_packets += rx_status.len_1024_1518_packets;  /**< Number of 1024-1518B packets */
    cvm_stats[dev->ifindex].pip_stats.len_1519_max_packets += rx_status.len_1519_max_packets;    /**< Number of 1519-max packets */
    cvm_stats[dev->ifindex].pip_stats.fcs_align_err_packets += rx_status.fcs_align_err_packets;  /**< Number of packets with FCS or Align opcode errors */
    cvm_stats[dev->ifindex].pip_stats.runt_packets += rx_status.runt_packets;                    /**< Number of packets with length < min */
    cvm_stats[dev->ifindex].pip_stats.runt_crc_packets += rx_status.runt_crc_packets;            /**< Number of packets with length < min and FCS error */
    cvm_stats[dev->ifindex].pip_stats.oversize_packets += rx_status.oversize_packets;            /**< Number of packets with length > max */
    cvm_stats[dev->ifindex].pip_stats.oversize_crc_packets += rx_status.oversize_crc_packets;    /**< Number of packets with length > max and FCS error */
    cvm_stats[dev->ifindex].pip_stats.inb_packets += rx_status.inb_packets;                      /**< Number of packets without GMX/SPX/PCI errors received by PIP */
    cvm_stats[dev->ifindex].pip_stats.inb_octets += rx_status.inb_octets;                        /**< Total number of octets from all packets received by PIP, including CRC */
    cvm_stats[dev->ifindex].pip_stats.inb_errors += rx_status.inb_errors;

	cvm_stats[dev->ifindex].pko_stats.packets += tx_status.packets;
	cvm_stats[dev->ifindex].pko_stats.octets += tx_status.octets;
	cvm_stats[dev->ifindex].pko_stats.doorbell += tx_status.doorbell;

	return;
}


/**
 * Get the low level ethernet statistics
 *
 * @param dev    Device to get the statistics from
 * @return Pointer to the statistics
 */
static struct net_device_stats *cvm_oct_common_get_stats(struct net_device *dev)
{
    cvmx_pip_port_status_t rx_status;
    cvmx_pko_port_status_t tx_status;
    cvm_oct_private_t* priv = (cvm_oct_private_t*)dev->priv;

    if ((priv->port < CVMX_PIP_NUM_INPUT_PORTS) && priv->port != DSA_port_Num)
    {
#ifndef CONFIG_CAVIUM_OCTEON_SIMULATOR
        cvmx_pip_get_port_status(priv->port, 1, &rx_status);
        cvmx_pko_get_port_status(priv->port, 1, &tx_status);
#else
        /* The simulator doesn't support statistics */
        memset(&rx_status, 0, sizeof(rx_status));
        memset(&tx_status, 0, sizeof(tx_status));
#endif

        priv->stats.rx_packets      += rx_status.inb_packets;
        priv->stats.tx_packets      += tx_status.packets;
        priv->stats.rx_bytes        += rx_status.inb_octets;
        priv->stats.tx_bytes        += tx_status.octets;
        priv->stats.multicast       += rx_status.multicast_packets;
		priv->stats.multicast	    += rx_status.broadcast_packets;
        priv->stats.rx_crc_errors   += rx_status.inb_errors;
        priv->stats.rx_frame_errors += rx_status.fcs_align_err_packets;

        /* The drop counter must be incremented atomically since the RX
            tasklet also increments it */
#ifdef CONFIG_64BIT
        cvmx_atomic_add64_nosync(&priv->stats.rx_dropped, rx_status.dropped_packets);
#else
        cvmx_atomic_add32_nosync((int32_t*)&priv->stats.rx_dropped, rx_status.dropped_packets);
#endif

        cvm_oct_common_show_stats(dev, rx_status, tx_status);
    
    }

    return &priv->stats;
}


/**
 * Set the multicast list. Currently unimplemented.
 *
 * @param dev    Device to work on
 */
static void cvm_oct_common_set_multicast_list(struct net_device *dev)
{
	dev->flags |= IFF_PROMISC;
    cvmx_gmxx_prtx_cfg_t gmx_cfg;
    cvm_oct_private_t* priv = (cvm_oct_private_t*)dev->priv;
    int interface = INTERFACE(priv->port);
    int index = INDEX(priv->port);

    if (interface < 2)
    {
        cvmx_gmxx_rxx_adr_ctl_t control;
        control.u64 = 0;
        control.s.bcst = 1;     /* Allow broadcast MAC addresses */

        if (dev->mc_list || (dev->flags&IFF_ALLMULTI) || (dev->flags&IFF_PROMISC))
            control.s.mcst = 2; /* Force accept multicast packets */
        else
            control.s.mcst = 1; /* Force reject multicat packets */

        if(dev->flags&IFF_PROMISC)
            control.s.cam_mode = 0; /* Reject matches if promisc. Since CAM is shut off, should accept everything */
        else
            control.s.cam_mode = 1; /* Filter packets based on the CAM */

        gmx_cfg.u64 = cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));
        cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64 & ~1ull);

        cvmx_write_csr(CVMX_GMXX_RXX_ADR_CTL(index, interface), control.u64);
        if (dev->flags&IFF_PROMISC)
            cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM_EN(index, interface), 0);
        else
            cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM_EN(index, interface), 1);

        cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);
    }
}


/**
 * Set the hardware MAC address for a device
 *
 * @param dev    Device to change the MAC address for
 * @param addr   Address structure to change it too. MAC address is addr + 2.
 * @return Zero on success
 */
static int cvm_oct_common_set_mac_address(struct net_device *dev, void *addr)
{
    cvm_oct_private_t* priv = (cvm_oct_private_t*)dev->priv;
    cvmx_gmxx_prtx_cfg_t gmx_cfg;
    int interface = INTERFACE(priv->port);
    int index = INDEX(priv->port);

    memcpy(dev->dev_addr, addr + 2, 6);

    if (interface < 2)
    {
        int i;
        uint8_t *ptr = addr;
        uint64_t mac = 0;
        for (i=0; i<6; i++)
            mac = (mac<<8) | (uint64_t)(ptr[i+2]);

        gmx_cfg.u64 = cvmx_read_csr(CVMX_GMXX_PRTX_CFG(index, interface));
        cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64 & ~1ull);

        cvmx_write_csr(CVMX_GMXX_SMACX(index, interface), mac);
        cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM0(index, interface), ptr[2]);
        cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM1(index, interface), ptr[3]);
        cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM2(index, interface), ptr[4]);
        cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM3(index, interface), ptr[5]);
        cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM4(index, interface), ptr[6]);
        cvmx_write_csr(CVMX_GMXX_RXX_ADR_CAM5(index, interface), ptr[7]);
        cvm_oct_common_set_multicast_list(dev);
        cvmx_write_csr(CVMX_GMXX_PRTX_CFG(index, interface), gmx_cfg.u64);
    }
    return 0;
}


/**
 * Change the link MTU. Unimplemented
 *
 * @param dev     Device to change
 * @param new_mtu The new MTU
 * @return Zero on success
 */
static int cvm_oct_common_change_mtu(struct net_device *dev, int new_mtu)
{
    cvm_oct_private_t* priv = (cvm_oct_private_t*)dev->priv;
    int interface = INTERFACE(priv->port);
    int index = INDEX(priv->port);

    /* Limit the MTU to make sure the ethernet packets are between 64 bytes
        and 65535 bytes */
    if ((new_mtu + 14 + 4 < 64) || (new_mtu + 14 + 4 > 65392))
    {
        printk("MTU must be between %d and %d.\n", 64-14-4, 65392-14-4);
        return -EINVAL;
    }
    dev->mtu = new_mtu;

    if (interface == 1)
    {
//        int max_packet = new_mtu + 14 + 4; /* Add ethernet header and possible VLAN */
		int max_packet = 1548;/* larger packets dropped by software, hardware will not drop packet_size > 1536*/
        /* Signal errors on packets larger than the MTU */
        cvmx_write_csr(CVMX_GMXX_RXX_FRM_MAX(index, interface), max_packet);
        /* Set the hardware to truncate packets larger than the MTU. The
            jabber register must be set to a multiple of 8 bytes, so round up */
        cvmx_write_csr(CVMX_GMXX_RXX_JABBER(index, interface), (max_packet + 7) & ~7u);
		/* set the larger frame len check on interface 1 */
		cvmx_write_csr(CVMX_PIP_FRM_LEN_CHKX(interface), 0x6100040);
    }
	if(interface == 0)
	{
		/*for DSA tag length*/
//        int max_packet = new_mtu + 14 + 4 + 8; /* Add ethernet header and possible VLAN */
		int max_packet = 1548;/* larger packets dropped by software,  hardware will not dropped packet_size > 1536*/
        /* Signal errors on packets larger than the MTU */
        cvmx_write_csr(CVMX_GMXX_RXX_FRM_MAX(index, interface), max_packet);
        /* Set the hardware to truncate packets larger than the MTU. The
            jabber register must be set to a multiple of 8 bytes, so round up */
        cvmx_write_csr(CVMX_GMXX_RXX_JABBER(index, interface), (max_packet + 7) & ~7u);
		/* set the larger frame len check on interface 1 */
		cvmx_write_csr(CVMX_PIP_FRM_LEN_CHKX(interface), 0x6100040);
	}	
    return 0;
}


/**
 * Per network device initialization
 *
 * @param dev    Device to initialize
 * @return Zero on success
 */

#define MAC_COUNT_PER_SLOT	2

int cvm_oct_common_init(struct net_device *dev)
{
    static int count = 0;
	char mac[8];
	unsigned char mac_addr[6];
	
	if (!memcmp(dev->name, obc_name, 3))
	{
		mac[0] = 0x00;
		mac[1] = 0x00;
       	mac[2] = obc_mac_base[0];
	    mac[3] = obc_mac_base[1];
	    mac[4] = obc_mac_base[2];
		mac[5] = obc_mac_base[3];
		mac[6] = obc_mac_base[4];
		if (is_distributed && (!(local_slotNum < 0)))
		{
			mac[7] = obc_mac_base[5] + local_slotNum - 0x1;
		}
	    else
		{
			mac[7] = obc_mac_base[5];
	    }
	}
	else if(0 == cvm_get_local_mac_addr(mac_addr))
	{
	     mac[0] = 0x00;
	 	 mac[1] = 0x00;
       	 mac[2] = mac_addr[0];
	     mac[3] = mac_addr[1];
	     mac[4] = mac_addr[2];
		 mac[5] = mac_addr[3];
		 mac[6] = mac_addr[4];
	     mac[7] = mac_addr[5];
        //octeon_bootinfo->mac_addr_base[5] + count};
	}
	else
	{
		
		mac[0] = 0x00;
		mac[1] = 0x00;
		mac[2] = octeon_bootinfo->mac_addr_base[0];
		mac[3] = octeon_bootinfo->mac_addr_base[1];
		mac[4] = octeon_bootinfo->mac_addr_base[2];
		mac[5] = octeon_bootinfo->mac_addr_base[3];
		mac[6] = octeon_bootinfo->mac_addr_base[4];
		mac[7] = octeon_bootinfo->mac_addr_base[5];
		
        //octeon_bootinfo->mac_addr_base[5] + count};
	}
    cvm_oct_private_t* priv = (cvm_oct_private_t*)dev->priv;


    /* Force the interface to use the POW send if always_use_pow was
        specified or it is in the pow send list */
    if ((pow_send_group != -1) && (always_use_pow || strstr(pow_send_list, dev->name)))
        priv->queue = -1;

    if (priv->queue != -1)
    {
        dev->hard_start_xmit= cvm_oct_xmit;
        if (USE_HW_TCPUDP_CHECKSUM)
            dev->features |= NETIF_F_IP_CSUM;
        count++;
    }
    else
    {
        dev->hard_start_xmit= cvm_oct_xmit_pow;
        memset(mac, 0, sizeof(mac));
        mac[7] = pow_send_group;
    }
	dev->flags |= IFF_PROMISC;

    dev->get_stats          = cvm_oct_common_get_stats;
    dev->weight             = 16;
    dev->set_mac_address    = cvm_oct_common_set_mac_address;
    dev->set_multicast_list = cvm_oct_common_set_multicast_list;
    dev->change_mtu         = cvm_oct_common_change_mtu;
    dev->do_ioctl           = cvm_oct_ioctl;
    dev->features           |= NETIF_F_LLTX; /* We do our own locking, Linux doesn't need to */
    SET_ETHTOOL_OPS(dev, &cvm_oct_ethtool_ops);
#ifdef CONFIG_NET_POLL_CONTROLLER
    dev->poll_controller    = cvm_oct_poll_controller;
#endif

    cvm_oct_mdio_setup_device(dev);
    dev->set_mac_address(dev, mac);
    dev->change_mtu(dev, dev->mtu);

    /* Zero out stats for port so we won't mistakenly show counters from the
        bootloader */
    memset(dev->get_stats(dev), 0, sizeof(struct net_device_stats));

    return 0;
}

 
 /**
  * Per network device initialization for virtuor
  *
  * @param dev	  Device to initialize
  * @return Zero on success
  */
 int cvm_oct_common_ve_init(struct net_device *dev)
 {
	 static int count = 0;
	 char mac[8];
	 
	 unsigned char mac_addr[6];
	 if (!memcmp(dev->name, obc_name, 3))
	 {
		 mac[0] = 0x00;
		 mac[1] = 0x00;
       	 mac[2] = obc_mac_base[0];
	     mac[3] = obc_mac_base[1];
	     mac[4] = obc_mac_base[2];
		 mac[5] = obc_mac_base[3];
		 mac[6] = obc_mac_base[4];
		if (is_distributed && (!(local_slotNum < 0)))
		{
			mac[7] = obc_mac_base[5] + local_slotNum - 0x1;
		}
	    else
		{
			mac[7] = obc_mac_base[5];
	    }
	 }
	 else if(0 == cvm_get_local_mac_addr(mac_addr))
		 {
		  mac[0] = 0x00;
		  mac[1] = 0x00;
		  mac[2] = mac_addr[0];
		  mac[3] = mac_addr[1];
		  mac[4] = mac_addr[2];
		  mac[5] = mac_addr[3];
		  mac[6] = mac_addr[4];
		  mac[7] = mac_addr[5];
		 //octeon_bootinfo->mac_addr_base[5] + count};
		 }
	 else
		 {
		 
		 mac[0] = 0x00;
		 mac[1] = 0x00;
		 mac[2] = octeon_bootinfo->mac_addr_base[0];
		 mac[3] = octeon_bootinfo->mac_addr_base[1];
		 mac[4] = octeon_bootinfo->mac_addr_base[2];
		 mac[5] = octeon_bootinfo->mac_addr_base[3];
		 mac[6] = octeon_bootinfo->mac_addr_base[4];
		 mac[7] = octeon_bootinfo->mac_addr_base[5];
		 
		 //octeon_bootinfo->mac_addr_base[5] + count};
		 }
//	 char mac[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x34,0x02};
//	 cvm_oct_private_t* priv = (cvm_oct_private_t*)dev->priv;
 
 
	 /* Force the interface to use the POW send if always_use_pow was
		 specified or it is in the pow send list */
		 dev->hard_start_xmit= cvm_oct_xmit_ve;
		 if (USE_HW_TCPUDP_CHECKSUM)
			 dev->features |= NETIF_F_IP_CSUM;
		 count++;
 
	 dev->get_stats 		 = cvm_oct_common_get_stats;
	 dev->weight			 = 16;
	 dev->set_mac_address	 = cvm_oct_common_set_mac_address;
	 dev->set_multicast_list = cvm_oct_common_set_multicast_list;
	 dev->change_mtu		 = cvm_oct_common_change_mtu;
//	 dev->do_ioctl			 = cvm_oct_ioctl;
	 dev->do_ioctl			 = NULL;
	 dev->features			 |= NETIF_F_LLTX; /* We do our own locking, Linux doesn't need to */
	 SET_ETHTOOL_OPS(dev, &cvm_oct_ethtool_ops);
#ifdef CONFIG_NET_POLL_CONTROLLER
	 dev->poll_controller	 = cvm_oct_poll_controller;
#endif
 
//	 cvm_oct_mdio_setup_device(dev);
	 dev->set_mac_address(dev, mac);
	 dev->change_mtu(dev, dev->mtu);
 
	 /* Zero out stats for port so we won't mistakenly show counters from the
		 bootloader */
	 memset(dev->get_stats(dev), 0, sizeof(struct net_device_stats));
 
	 return 0;
 }

 /**
  * Per network device initialization for virtuor
  *
  * @param dev	  Device to initialize
  * @return Zero on success
  */
 int cvm_oct_common_ve_uninit(struct net_device *dev)
 {
	 /* Force the interface to use the POW send if always_use_pow was
		 specified or it is in the pow send list */
	 dev->hard_start_xmit= NULL;
	 dev->features &= ~NETIF_F_IP_CSUM;
 
	 dev->get_stats 		 = NULL;
	 dev->weight			 = 0;
	 dev->set_mac_address	 = NULL;
	 dev->set_multicast_list = NULL;
	 dev->change_mtu		 = NULL;
	 dev->do_ioctl			 = NULL;
	 dev->features			 &= ~NETIF_F_LLTX; /* We do our own locking, Linux doesn't need to */
	 SET_ETHTOOL_OPS(dev, NULL);
#ifdef CONFIG_NET_POLL_CONTROLLER
	 dev->poll_controller	 = NULL;
#endif
 
	 return 0;
 }

void cvm_oct_common_uninit(struct net_device *dev)
{
    cvm_oct_private_t *priv = (cvm_oct_private_t*)dev->priv;

    /* Free buffers */
    while (skb_queue_len(&priv->tx_free_list))
        dev_kfree_skb(skb_dequeue(&priv->tx_free_list));
}
#endif

/**
 * send netlink message to user-space
 *
 * @param sendBuf: message farmat
 * @param msgLen : message length
 * modify:      <jialh@autelan.com>
 */
void cvm_oct_netlink_notifier(char *sendBuf, int msgLen)     
{   
    struct sk_buff *skb;   
    struct nlmsghdr *nlh;  
	int size = NLMSG_SPACE(msgLen);
	
    if(!sendBuf || !nl_sk){   
        return;   
    }   

    /* Allocate a new sk_buffer */  
    skb = alloc_skb(size, GFP_ATOMIC);   
    if(!skb){   
        printk(KERN_ERR "netlink: alloc_skb Error.\n");   
        return;   
    }   

    /* Initialize the header of netlink message */  
    nlh = NLMSG_PUT(skb, 0, 0, 0, size-sizeof(*nlh));   
  
    NETLINK_CB(skb).pid = 0;   
    NETLINK_CB(skb).dst_group = 1;   
    
    memcpy(NLMSG_DATA(nlh), sendBuf, msgLen);
	
    /* send message by broadcast */  
    netlink_broadcast(nl_sk, skb, 0, 1, GFP_ATOMIC);   
	
    return; 
	
nlmsg_failure: 
    if(skb)
        kfree_skb(skb);
    return;
}

/*enable phy QT2025 firmware*/
int cvm_phy_enable(void)
{
    int ret;
    ret=cvmx_mdio_x_write(0, 0x18, 1, 0xc319,cvmx_mdio_x_read(0, 0x18, 1, 0xc319) & 0xd);
    if(ret == 0)
    {
        printk("Powerup the firmware done!\n");
        return 1;
    } 
	return 0;
}
EXPORT_SYMBOL(cvm_phy_enable);

/*disable phy QT2025 firmware*/
int cvm_phy_disable(void)
{
    int ret;
    ret=cvmx_mdio_x_write(0, 0x18, 1, 0xc319,cvmx_mdio_x_read(0, 0x18, 1, 0xc319)|0x2);
    if(ret == 0)
    {
        printk("Powerdown the firmware done!\n");
        return 1;
    }
	return 0;
}
EXPORT_SYMBOL(cvm_phy_disable);

/*read the phy QT2025 firmware register*/
int cvm_read_phy_QT(int regaddr,unsigned int location)
{
    int ret;
    ret=cvmx_mdio_x_read(0, 0x18, regaddr, location);
	return ret;
}
EXPORT_SYMBOL(cvm_read_phy_QT);

/*write the phy QT2025 firmware register*/
int cvm_write_phy_QT(int regaddr,unsigned int location,unsigned int val)
{
    int ret;
    ret=cvmx_mdio_x_write(0, 0x18, regaddr,location,val);
    if(ret != 0)
    {
		printk(KERN_ERR "write the firmware register fail!\n");
		printk(KERN_ERR "regaddr=%d  location=%x\n",regaddr,location);
        return 0;
    }
	return 1;
}
EXPORT_SYMBOL(cvm_write_phy_QT);

