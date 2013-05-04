/*
 *
 * OCTEON SDK
 *
 * Copyright (c) 2011 Cavium Networks. All rights reserved.
 *
 * This file, which is part of the OCTEON SDK which also includes the
 * OCTEON SDK Package from Cavium Networks, contains proprietary and
 * confidential information of Cavium Networks and in some cases its
 * suppliers. 
 *
 * Any licensed reproduction, distribution, modification, or other use of
 * this file or the confidential information or patented inventions
 * embodied in this file is subject to your license agreement with Cavium
 * Networks. Unless you and Cavium Networks have agreed otherwise in
 * writing, the applicable license terms "OCTEON SDK License Type 5" can be found 
 * under the directory: $OCTEON_ROOT/components/driver/licenses/
 *
 * All other use and disclosure is prohibited.
 *
 * Contact Cavium Networks at info@caviumnetworks.com for more information.
 *
 */



#include "cavium_sysdep.h"
#include "cavium_release.h"
#include "octeon_network.h"
#include "octeon_macros.h"
#include "octeon_nic.h"

#include <asm/octeon/cvmx.h>        /* cvmx_read64_uint8 */
#include <linux/autelan_product.h>      /* CPDL_REG */

MODULE_AUTHOR("Cavium Networks");
MODULE_DESCRIPTION("Octeon Host PCI Nic Driver");
MODULE_LICENSE("Cavium Networks");

extern void   octnet_napi_drv_callback(int oct_id, int oq_no, int event);

struct  octdev_props_t  *octprops[MAX_OCTEON_DEVICES];

/* add for get locat slot no, zhangdi@autelan.com 2012-06-05 */
int octeon_slot = 0;
int octeon_board = 0;
int nic_xaui_num = 0;


#define LINK_STATUS_REQUESTED    1
#define LINK_STATUS_FETCHED      2



static inline  octnet_os_devptr_t *
octnet_alloc_netdev(int sizeof_priv)
{
	return alloc_netdev(sizeof_priv, "oct%d", ether_setup);
}

static inline  void
octnet_free_netdev(octnet_os_devptr_t *dev)
{
	return free_netdev(dev);
}



#if defined(OCTEON_EXCLUDE_BASE_LOAD)
extern void get_base_compile_options(char *copts);
#endif


void
get_nic_compile_options(char *copts)
{
#if defined(OCTEON_EXCLUDE_BASE_LOAD)
  get_base_compile_options(copts);
#endif

  if(OCT_NIC_USE_NAPI)
    strcat(copts, "NAPI");
}




void
octnet_print_link_info(octnet_os_devptr_t  *pndev)
{
	octnet_priv_t  *priv = GET_NETDEV_PRIV(pndev);

	if(cavium_atomic_read(&priv->ifstate) & OCT_NIC_IFSTATE_REGISTERED) {
		oct_link_info_t   *linfo = &(priv->linfo);
		if(linfo->link.s.status)
			cavium_print_msg("OCTNIC: %s -> %d Mbps %s Duplex UP\n",
		                 octnet_get_devname(pndev), linfo->link.s.speed,
		                 (linfo->link.s.duplex)?"Full":"Half");
		else
			cavium_print_msg("OCTNIC: %s Link Down\n",
		                 octnet_get_devname(pndev));
	}
}










/* Called on receipt of a link status response from the core application to
   update each interface's link status. */
static inline void
octnet_update_link_status(octnet_os_devptr_t    *pndev,
                          oct_link_status_t     *ls)
{
	octnet_priv_t      *priv = GET_NETDEV_PRIV(pndev);
	oct_link_status_t   prev_st;

	prev_st.u64          = priv->linfo.link.u64;
	priv->linfo.link.u64 = ls->u64;

	if(prev_st.u64 != ls->u64) {
		octnet_print_link_info(pndev);
		if(priv->linfo.link.s.status) {
			netif_carrier_on(pndev);
			octnet_start_txqueue(pndev);
		} else {
			netif_carrier_off(pndev);
			octnet_stop_txqueue(pndev);
		}
	}
}











/* Callback called by BASE driver when response arrives for a link status
   instruction sent by the poll function (runtime link status monitoring). */
void
octnet_link_change_callback(octeon_req_status_t  status, void *props_ptr)
{
	struct octdev_props_t       *props;
	oct_link_status_resp_t      *ls;
	int                          ifidx;

	props = (struct octdev_props_t  *)props_ptr;
	ls    = props->ls;

	/* Don't do anything if the status is not 0. */
	if(ls->status) {
		goto end_of_link_change_callback;
	}

	/* The link count should be swapped on little endian systems. */
	octeon_swap_8B_data(&(ls->link_count), 1);

	if(ls->link_count > MAX_OCTEON_LINKS) {
		cavium_error("%s: Link count (%llu) exceeds max (%d)\n", __FUNCTION__,
		             ls->link_count, MAX_OCTEON_LINKS);
		goto end_of_link_change_callback;
	}

	octeon_swap_8B_data((uint64_t *)ls->link_info,
	                    (ls->link_count * (OCT_LINK_INFO_SIZE >> 3)));

	for(ifidx = 0; ifidx < ls->link_count; ifidx++) {
		octnet_update_link_status(props->pndev[ifidx],
		                          &ls->link_info[ifidx].link);
	}

end_of_link_change_callback:
	cavium_atomic_set(&props->ls_flag, LINK_STATUS_FETCHED);
}







/* Callback called by BASE driver when response arrives for a link status
   instruction sent by the NIC module init routine (inittime link status). */
void
octnet_inittime_ls_callback(octeon_req_status_t  status, void *buf)
{
	oct_link_status_resp_t  *link_status;

	link_status = (oct_link_status_resp_t *)buf;
	if(link_status->status) 
		cavium_error("OCTNIC: Link status instruction failed. Status: %llx\n", CVM_CAST64(link_status->status));
	link_status->s.cond = 1;	
	cavium_wakeup(&link_status->s.wc);
}







/* Get the link status at init time. This routine sleeps till a response
   arrives from the core app. This is because the initialization cannot
   proceed till the host knows about the number of ethernet interfaces
   supported by the Octeon NIC target device. */
int
octnet_get_inittime_link_status(void  *oct,
                                void  *props_ptr)
{
	struct octdev_props_t       *props;
	octeon_soft_instruction_t   *si;
	oct_link_status_resp_t      *ls;
	octeon_instr_status_t        retval;

	props = (struct octdev_props_t  *)props_ptr;

	/* Use the link status soft instruction pre-allocated
	   for this octeon device. */
	si = props->si_link_status;

	/* Reset the link status buffer in props for this octeon device. */
	ls = props->ls;
	cavium_memset(ls, 0, OCT_LINK_STATUS_RESP_SIZE);

	cavium_init_wait_channel(&ls->s.wc);
	si->rptr           = &(ls->resp_hdr);
	si->irh.rlenssz    = ( OCT_LINK_STATUS_RESP_SIZE - sizeof(ls->s) );
	si->status_word    = (uint64_t *)&(ls->status);
	*(si->status_word) = COMPLETION_WORD_INIT;
	ls->s.cond         = 0;
	ls->s.octeon_id    = get_octeon_device_id(oct);
	SET_SOFT_INSTR_OCTEONID(si, ls->s.octeon_id);
	SET_SOFT_INSTR_CALLBACK(si, octnet_inittime_ls_callback);
	SET_SOFT_INSTR_CALLBACK_ARG(si, (void *)ls);



	retval = octeon_process_instruction(oct, si, NULL);
	if(retval.s.error) {
		cavium_error("OCTNIC: Link status instruction failed status: %x\n", retval.s.status);
		/* Soft instr is freed by driver in case of failure. */
		return EBUSY;
	}

	/* Sleep on a wait queue till the cond flag indicates that the
	   response arrived or timed-out. */
	cavium_sleep_timeout_cond(&ls->s.wc, (int *)&ls->s.cond, 1000);

	return(ls->status);
}







/* Get the link status at run time. This routine does not sleep waiting for
   a response. The link status is updated in a callback called when a response
   arrives from the core app. */

oct_poll_fn_status_t
octnet_get_runtime_link_status(void            *oct,
                               unsigned long    props_ptr)
{
	struct octdev_props_t       *props;
	octeon_soft_instruction_t   *si;
	oct_link_status_resp_t      *ls;
	octeon_instr_status_t        retval;


	props = (struct octdev_props_t  *)props_ptr;

	si = props->si_link_status;
	ls = props->ls;

	/* Do nothing if the status is not ready to be fetched. */
	if(cavium_atomic_read(&props->ls_flag) != LINK_STATUS_FETCHED)
		return OCT_POLL_FN_CONTINUE;

	cavium_memset(ls, 0, OCT_LINK_STATUS_RESP_SIZE);


	si->rptr           = &(ls->resp_hdr); 
	si->irh.rlenssz    = ( OCT_LINK_STATUS_RESP_SIZE - sizeof(ls->s) );
	si->status_word    = (uint64_t *)&(ls->status);
	*(si->status_word) = COMPLETION_WORD_INIT;
	ls->s.cond         = 0;
	ls->s.octeon_id    = get_octeon_device_id(oct);
	SET_SOFT_INSTR_OCTEONID(si, ls->s.octeon_id);
	SET_SOFT_INSTR_CALLBACK(si, octnet_link_change_callback);
	SET_SOFT_INSTR_CALLBACK_ARG(si, (void *)props);

	cavium_atomic_set(&props->ls_flag, LINK_STATUS_REQUESTED);

	retval = octeon_process_instruction(oct, si, NULL);
	if(retval.s.error) {
		/* Set state to fetched so that a request can be sent the next
		  time this poll fn is called. */
		cavium_atomic_set(&props->ls_flag, LINK_STATUS_FETCHED);
	}
	props->last_check = cavium_jiffies;

	return OCT_POLL_FN_CONTINUE;
}







#ifdef ETHERPCI
/* The link status information is not read from the Octeon core app when
   EtherPCI is used since there is no control traffic sent across PCI in
   EtherPCI mode. We use a fixed setting instead. */
void
octnet_prepare_etherpci_links(oct_link_status_resp_t   *link_status,
                              int                       count)
{
	int i;

	link_status->link_count = count;

	for(i = 0; i < count; i++) {
		uint8_t  *hw_addr;

		link_status->link_info[i].link.u64      = 0;
		link_status->link_info[i].link.s.speed  = 1000;
		link_status->link_info[i].link.s.duplex = 1;
		link_status->link_info[i].link.s.status = 1;
		link_status->link_info[i].link.s.mtu    = 1500;
		hw_addr = (uint8_t *)&link_status->link_info[i].hw_addr;
		hw_addr[2]    = 0x00;
		hw_addr[3]    = 0x01;
		hw_addr[4]    = 0x02;
		hw_addr[5]    = 0x03;
		hw_addr[6]    = 0x04;
		hw_addr[7]    = 0x05 + i;
#if  __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
		/* HW Addr is maintained in network-byte order; so swap the address if we are on
		   a little endian host. */
		link_status->link_info[i].hw_addr =
		 ENDIAN_SWAP_8_BYTE(link_status->link_info[i].hw_addr);
#endif
		link_status->link_info[i].gmxport  = 16 + i;
		link_status->link_info[i].ifidx    = i;
		link_status->link_info[i].rxpciq   = 0;
		link_status->link_info[i].txpciq   = 0;
	}
}
#endif









/* Register droq_ops for each interface. By default all interfaces for a
   Octeon device uses the same Octeon output queue, but this can be easily
   changed by setting priv->rxq to the output queue you want to use. */
int
octnet_setup_net_queues(int octeon_id, octnet_priv_t  *priv)
{
	octeon_droq_ops_t    droq_ops;

	memset(&droq_ops, 0, sizeof(octeon_droq_ops_t));

	droq_ops.fptr        = octnet_push_packet;

	if(OCT_NIC_USE_NAPI) {
		droq_ops.poll_mode  = 1;
		droq_ops.napi_fn    = octnet_napi_drv_callback;
	} else {
		droq_ops.drop_on_max = 1;
	}

	cavium_print(PRINT_DEBUG, "Setting droq ops for q %d poll_mode: %d napi_fn: %p fptr: %p drop: %d\n", priv->rxq, droq_ops.poll_mode, droq_ops.napi_fn, droq_ops.fptr,
		droq_ops.drop_on_max);

	/* Register the droq ops structure so that we can start handling packets
	 * received on the Octeon interfaces. */
	if(octeon_register_droq_ops(octeon_id, priv->rxq, &droq_ops)) {
		cavium_error("OCTNIC: Failed to register DROQ function\n");
		return -ENODEV;
	}

	return 0;
}







void
octnet_delete_glist(octnet_priv_t  *priv)
{
	struct octnic_gather *g;

	do{
		g = (struct octnic_gather *) cavium_list_delete_head(&priv->glist);
		if(g) {
			if(g->sg) {
				cavium_free_dma( (void *)((unsigned long)g->sg - g->adjust) );
			}
			cavium_free_dma(g);
		}
	}while(g);
}





int
octnet_setup_glist(octnet_priv_t  *priv)
{
	int                    i;
	struct octnic_gather  *g;

	CAVIUM_INIT_LIST_HEAD(&priv->glist);

	for(i = 0; i < priv->tx_qsize; i++) {
		g = cavium_malloc_dma(sizeof(struct octnic_gather),
		                       __CAVIUM_MEM_GENERAL);
		if(g == NULL)
			break;
		memset(g, 0, sizeof(struct octnic_gather));

		g->sg_size = ((ROUNDUP4(OCTNIC_MAX_SG) >> 2)* OCT_SG_ENTRY_SIZE);

		g->sg = cavium_malloc_dma(g->sg_size + 8, __CAVIUM_MEM_GENERAL);
		if(g->sg == NULL) {
			cavium_free_dma(g);
			break;
		}

		/* The gather component should be aligned on a 64-bit boundary. */
		if( ((unsigned long)g->sg) & 7) {
			g->adjust = 8 - ( ((unsigned long)g->sg) & 7);
			g->sg = (octeon_sg_entry_t *)((unsigned long)g->sg + g->adjust);
		}
		cavium_list_add_tail(&g->list, &priv->glist);
	}


	if(i == priv->tx_qsize)
		return 0;

	octnet_delete_glist(priv);
	return 1;
}




void
octnet_send_rx_ctrl_cmd(octnet_priv_t  *priv, int start_stop)
{
	octnic_ctrl_pkt_t    nctrl;

	memset(&nctrl, 0, sizeof(octnic_ctrl_pkt_t));

	nctrl.ncmd.s.cmd    = OCTNET_CMD_RX_CTL;
	nctrl.ncmd.s.param1 = priv->linfo.ifidx;
	nctrl.ncmd.s.param2 = start_stop;
	nctrl.netpndev      = (unsigned long)priv->pndev;

#if  !defined(ETHERPCI)
	if(octnet_send_nic_ctrl_pkt(priv->oct_dev, &nctrl)) {
		cavium_error("OCTNIC: Failed to send RX Control message\n");
	}
#endif

	return;
}









/* Cleanup associated with each interface for an Octeon device  when NIC
   module is being unloaded or if initialization fails during load. */
void
octnet_destroy_nic_device(int octeon_id, int ifidx)
{
	octnet_os_devptr_t   *pndev = octprops[octeon_id]->pndev[ifidx];
	octnet_priv_t        *priv;
	int                   retval = 0;

	if(pndev == NULL) {
		cavium_error("OCTNIC: %s No netdevice ptr for index %d\n",
		              __CVM_FUNCTION__, ifidx);
		return;
	}

	priv = GET_NETDEV_PRIV(pndev);

	octnet_send_rx_ctrl_cmd(priv, 0);

	if(cavium_atomic_read(&priv->ifstate) & OCT_NIC_IFSTATE_DROQ_OPS) {
		retval = octeon_unregister_droq_ops(octeon_id, ifidx);
		if(retval) {
			cavium_error("OCTNIC: Failed to unregister DROQ function\n");
		}
	}

	if(cavium_atomic_read(&priv->ifstate) & OCT_NIC_IFSTATE_RUNNING)
		netif_stop_queue(pndev);

	if(cavium_atomic_read(&priv->ifstate) & OCT_NIC_IFSTATE_REGISTERED)
		unregister_netdev(pndev);

	octnet_delete_glist(priv);

	octnet_free_netdev(pndev);

	octprops[octeon_id]->pndev[ifidx] = NULL;
}



static void
octnet_setup_napi(octnet_priv_t *priv)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
	netif_napi_add(priv->pndev, &priv->napi, octnet_napi_poll, 64);
#else
	priv->pndev->poll    = octnet_napi_poll;
	priv->pndev->weight  = 64;
	set_bit(__LINK_STATE_START, &priv->pndev->state);
#endif
}



#if  LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
const static struct net_device_ops  octnetdevops = {
	.ndo_open                = octnet_open,
	.ndo_stop                = octnet_stop,
	.ndo_start_xmit          = octnet_xmit,
	.ndo_get_stats           = octnet_stats,
	.ndo_set_mac_address     = octnet_set_mac,
	.ndo_set_multicast_list  = octnet_set_mcast_list,
	.ndo_tx_timeout          = octnet_tx_timeout,
	.ndo_change_mtu          = octnet_change_mtu,
};
#endif


void
octnet_napi_enable(octnet_priv_t  *priv);


/*
   Called during init time for each interface. This routine after the NIC
   module receives the link status information from core app at init time.
   The link information for each interface is passed in link_info.
*/
static int
octnet_setup_nic_device(int octeon_id, oct_link_info_t  *link_info, int ifidx)
{
	octnet_priv_t       *priv;
	octnet_os_devptr_t  *pndev;
	uint8_t              macaddr[6], i;

	pndev = octnet_alloc_netdev(OCTNET_PRIV_SIZE);
	if(!pndev) {
		cavium_error("OCTNIC: Device allocation failed\n");
		return -ENOMEM;
	}

	octprops[octeon_id]->pndev[ifidx] = pndev;

	/* Associate the routines that will handle different netdev tasks. */
#if  LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
	pndev->netdev_ops          = &octnetdevops;
#else
	pndev->open                = octnet_open;
	pndev->stop                = octnet_stop;
	pndev->hard_start_xmit     = octnet_xmit;
	pndev->get_stats           = octnet_stats;
	pndev->set_mac_address     = octnet_set_mac;
	pndev->set_multicast_list  = octnet_set_mcast_list;
	pndev->tx_timeout          = octnet_tx_timeout;
	pndev->change_mtu          = octnet_change_mtu;
#endif

#if !defined(ETHERPCI)
	pndev->features  = NETIF_F_HW_CSUM;
	pndev->features |=  NETIF_F_SG;
#endif

	priv            = GET_NETDEV_PRIV(pndev);
	cavium_memset(priv, 0, sizeof(octnet_priv_t));

	priv->ifidx     = ifidx;

	/* Point to the  properties for octeon device to which this interface
	   belongs. */
	priv->oct_dev   = get_octeon_device_ptr(octeon_id);
	priv->octprops  = octprops[octeon_id];
	priv->pndev     = pndev;
	cavium_spin_lock_init(&(priv->lock));

	/* Record the ethernet port number on the Octeon target for this
       interface. */
	priv->linfo.gmxport = link_info->gmxport;

	/* Record the pci port that the core app will send and receive packets
	   from host for this interface. */
	priv->linfo.ifidx   = link_info->ifidx;
	priv->linfo.hw_addr = link_info->hw_addr;
	priv->linfo.txpciq  = link_info->txpciq;
	priv->linfo.rxpciq  = link_info->rxpciq;


	if(OCT_NIC_USE_NAPI) {
		octnet_setup_napi(priv);
	}

	cavium_print(PRINT_DEBUG, "OCTNIC: if%d gmx: %d hw_addr: 0x%llx\n",
	             ifidx, priv->linfo.gmxport, CVM_CAST64(priv->linfo.hw_addr));

	/* 64-bit swap required on LE machines */
	octeon_swap_8B_data(&priv->linfo.hw_addr, 1);
	for(i = 0; i < 6; i++)
		macaddr[i] = *((uint8_t *)(((uint8_t *)&priv->linfo.hw_addr) + 2 + i));

	/* Copy MAC Address to OS network device structure */
	cavium_memcpy(pndev->dev_addr, &macaddr, ETH_ALEN);

	priv->linfo.link.u64 = link_info->link.u64;


	priv->tx_qsize = octeon_get_tx_qsize(octeon_id, priv->txq);
	priv->rx_qsize = octeon_get_rx_qsize(octeon_id, priv->rxq);

	if(octnet_setup_glist(priv)) {
		cavium_error("OCTNIC: Gather list allocation failed\n");
		goto setup_nic_dev_fail;
	}

	OCTNET_IFSTATE_SET(priv, OCT_NIC_IFSTATE_DROQ_OPS);

	/* Reane oct to eth or ve, zhangdi@autelan.com 2012-06-05 */
	if(priv->linfo.link.s.speed == 1000)
	{
		sprintf(pndev->name, "eth%d-%d", octeon_slot, (priv->linfo.ifidx +1));		
	}
	else
	{
		/* we can use octeon_id to different first or second. */
    	sprintf(pndev->name, "ve%02ds%d", octeon_slot, ++nic_xaui_num);		
	}

	/* Register the network device with the OS */
	if(register_netdev(pndev)) {
		cavium_error("OCTNIC: Device registration failed\n");
		goto setup_nic_dev_fail;
	}

	netif_carrier_off(pndev);

	if(priv->linfo.link.s.status) {
		netif_carrier_on(pndev);
		octnet_start_txqueue(pndev);
	} else {
		netif_carrier_off(pndev);
	}

	/* Register the fast path function pointers after the network device
	   related activities are completed. We should be ready for Rx at this
	   point. */
#ifdef ETHERPCI
	priv->txq = ifidx; priv->rxq = ifidx;
	if(octnet_setup_net_queues(octeon_id, priv))
		goto setup_nic_dev_fail;
#else
	/* By default all interfaces on a single Octeon uses the same tx and rx
	   queues */
	priv->txq = priv->linfo.txpciq;
	priv->rxq = priv->linfo.rxpciq;
	if(octnet_setup_net_queues(octeon_id, priv))
		goto setup_nic_dev_fail;
	if(OCT_NIC_USE_NAPI)
		octnet_napi_enable(priv);
#endif

	OCTNET_IFSTATE_SET(priv, OCT_NIC_IFSTATE_REGISTERED);

	octnet_send_rx_ctrl_cmd(priv, 1);

	octnet_print_link_info(pndev);

	return 0;

setup_nic_dev_fail:
	octnet_destroy_nic_device(octeon_id, ifidx);
	return  -ENODEV;

}





#if !defined(ETHERPCI)

static void
octnet_prepare_ls_soft_instr(void  *oct,  octeon_soft_instruction_t   *si)
{

	cavium_memset(si, 0, OCT_SOFT_INSTR_SIZE);

	si->ih.fsz     = 16;
	si->ih.tagtype = ORDERED_TAG;
	si->ih.tag     = 0x11111111;
	si->ih.raw     = 1;
	si->irh.opcode = HOST_NW_INFO_OP;
	si->irh.param  = 32;
	SET_SOFT_INSTR_DMA_MODE(si, OCTEON_DMA_DIRECT);
	SET_SOFT_INSTR_RESP_ORDER(si, OCTEON_RESP_ORDERED);
	SET_SOFT_INSTR_RESP_MODE(si, OCTEON_RESP_NON_BLOCKING);
	SET_SOFT_INSTR_IQ_NO(si, 0);
	SET_SOFT_INSTR_TIMEOUT(si, 100);

	/* Since this instruction is sent in the poll thread context, if the
	   doorbell coalescing is > 1, the doorbell will never be rung for
	   this instruction (this call has to return for poll thread to hit
	   the doorbell). So enforce the doorbell ring. */
	SET_SOFT_INSTR_ALLOCFLAGS(si, OCTEON_SOFT_INSTR_DB_NOW);
	si->dptr        = NULL;
	si->ih.dlengsz  = 0;
}

#endif








/* This routine is registered by the NIC module with the BASE driver. The BASE
   driver calls this routine for each Octeon device that runs the NIC core
   application. */
int
octnet_init_nic_module(int octeon_id, void *octeon_dev)
{
	oct_link_status_resp_t     *ls = NULL;
	octeon_soft_instruction_t  *si = NULL;
	int                         ifidx, retval = 0;

	cavium_print_msg("OCTNIC: Initializing network interfaces for Octeon %d\n",
	                 octeon_id);

	/* Allocate the local NIC properties structure for this octeon device. */
	octprops[octeon_id] = cavium_alloc_virt(sizeof(struct octdev_props_t));
	if(octprops[octeon_id] == NULL) {
		cavium_error("OCTNIC: Alloc failed at %s:%d\n", __CVM_FUNCTION__, __CVM_LINE__);
		return -ENOMEM;
	} 
	cavium_memset(octprops[octeon_id], 0, sizeof(struct octdev_props_t));

	/* Allocate a buffer to collect link status from the core app. */
	ls = cavium_malloc_dma(sizeof(oct_link_status_resp_t),
		                    __CAVIUM_MEM_GENERAL);
	if(ls == NULL) {
		cavium_error("OCTNIC: Alloc failed at %s:%d\n", __CVM_FUNCTION__, __CVM_LINE__);
		cavium_free_virt(octprops[octeon_id]);
		octprops[octeon_id] = NULL;
		return -ENOMEM;
	}

	octprops[octeon_id]->ls = ls;


	/* Allocate a soft instruction to be used to send link status requests
	   to the core app. */
	si = (octeon_soft_instruction_t *)
	      cavium_alloc_buffer(octeon_dev, OCT_SOFT_INSTR_SIZE);
	if(si == NULL) {
		cavium_error("OCTNIC: soft instr allocation failed in net setup\n");
		cavium_free_dma(ls);
		cavium_free_virt(octprops[octeon_id]);
		octprops[octeon_id] = NULL;
		return ENOMEM;
	}

	octprops[octeon_id]->si_link_status = si;


#ifdef  ETHERPCI

	/* For ETHERPCI the link status uses fixed settings. */
	octnet_prepare_etherpci_links(ls, MAX_OCTEON_LINKS);

#else

	octnet_prepare_ls_soft_instr(octeon_dev, si);

	/* Send an instruction to get the link status information from core. */
	if(octnet_get_inittime_link_status(octeon_dev, octprops[octeon_id])) {
		cavium_error("OCTNIC: Link initialization failed\n");
		goto octnet_init_failure;
	}

	/* The link count should be swapped on little endian systems. */
	octeon_swap_8B_data(&(ls->link_count), 1);

	octeon_swap_8B_data((uint64_t *)ls->link_info,
		(ls->link_count * (OCT_LINK_INFO_SIZE >> 3)));

	for(ifidx = 0; ifidx < ls->link_count; ifidx++) {
		printk("OCTNIC: if%d rxq: %d txq: %d gmx: %d hw_addr: 0x%llx\n",
	             ifidx, ls->link_info[ifidx].rxpciq, ls->link_info[ifidx].txpciq, ls->link_info[ifidx].gmxport, CVM_CAST64(ls->link_info[ifidx].hw_addr));
	}
#endif

	octprops[octeon_id]->ifcount = ls->link_count;

	octeon_register_noresp_buf_free_fn(octeon_id, NORESP_BUFTYPE_NET,
		octnic_free_netbuf);

	octeon_register_noresp_buf_free_fn(octeon_id, NORESP_BUFTYPE_NET_SG,
		octnic_free_netsgbuf);


	/* For each ethernet port on the Octeon target, setup a NIC interface on
	   the host. */
	for(ifidx = 0; ifidx < ls->link_count; ifidx++) {
		retval =
		     octnet_setup_nic_device(octeon_id, &ls->link_info[ifidx], ifidx);
		if(retval) {
			while(ifidx--) {
				octnet_destroy_nic_device(octeon_id, ifidx);
				goto octnet_init_failure;
			}
		}
	}

	cavium_atomic_set(&octprops[octeon_id]->ls_flag, LINK_STATUS_FETCHED);
	octprops[octeon_id]->last_check = cavium_jiffies;

#if !defined(ETHERPCI)
	/* Register a poll function to run every second to collect and update
	   link status. */
	{
	octeon_poll_ops_t           poll_ops;
	poll_ops.fn     = octnet_get_runtime_link_status;
	poll_ops.fn_arg = (unsigned long)octprops[octeon_id];
	poll_ops.ticks  = CAVIUM_TICKS_PER_SEC*30;
	strcpy(poll_ops.name, "NIC Link Status");
	octeon_register_poll_fn(octeon_id, &poll_ops);
	}
#endif

	cavium_print_msg("OCTNIC: Network interfaces ready for Octeon %d\n",
	                 octeon_id);

	return  retval;

octnet_init_failure:
	cavium_error("OCTNIC: Initialization Failed\n");
	if(si)
		cavium_free_buffer(octeon_dev, si);
	if(ls)
		cavium_free_dma(ls);
	cavium_free_virt(octprops[octeon_id]);
	octprops[octeon_id] = NULL;

	return retval;
}











/* This routine is registered by the NIC module with the BASE driver. The BASE
   driver calls this routine before stopping each Octeon device that runs
   the NIC core application. */
int
octnet_stop_nic_module(int octeon_id, void *octeon_dev)
{
	int   i;

	cavium_print_msg("OCTNIC: Stopping network interfaces for Octeon device %d\n", octeon_id );
	if(octprops[octeon_id] == NULL) {
		cavium_error("OCTNIC: Init for Octeon%d was not completed\n",octeon_id);
		return 1;
	}

	octeon_unregister_poll_fn(octeon_id, octnet_get_runtime_link_status,
	                          (unsigned long)octprops[octeon_id]);

	for(i = 0; i < octprops[octeon_id]->ifcount; i++) {
		octnet_destroy_nic_device(octeon_id, i);
	}


	/* Free the link status buffer allocated for this Octeon device. */
	if(octprops[octeon_id]->ls) {
		cavium_free_dma(octprops[octeon_id]->ls);
	}

	/* Free the soft instruction buffer used for sending the link status to the core app. */
	if(octprops[octeon_id]->si_link_status) {
		cavium_free_buffer(octeon_dev, octprops[octeon_id]->si_link_status );
	}

	/* Free the props structures for this octeon device. */
	cavium_free_virt(octprops[octeon_id]);

	octprops[octeon_id] = NULL;

	cavium_print_msg("OCTNIC: Network interfaces stopped for Octeon %d\n",
	                 octeon_id);
	return 0;
}








/* This routine is registered by the NIC module with the BASE driver. The BASE
   driver calls this routine before performing hot-reset for each Octeon device
   that runs the NIC core application. */
int
octnet_reset_nic_module(int octeon_id, void *octeon_dev)
{
	if(CVM_MOD_IN_USE)
		return 1;
	return octnet_stop_nic_module(octeon_id, octeon_dev);
}


/**********************************************************************
 * get_num_from_file
 *
 *  DESCRIPTION:
 *          Get number from proc file.
 *  INPUT:
 *          filename    - the path of proc file
 *  OUTPUT:
 *          None
 *  RETURN:
 *          number
 *          1
 *  COMMENTS: none
 *
 **********************************************************************/
int octnet_get_num_from_file(const char *filename)
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
	if(result < 0)
	{
		printk("read value fail!\n");
	}
	set_fs(old_fs);
	filp_close(filp, NULL);
	value = (int)(temp[0] - '0');
	return value;
}


extern int  octeon_base_init_module(void);
extern void octeon_base_exit_module(void);

int
init_module()
{
	const char   *nic_cvs_tag = "$Name: PCI_NIC_RELEASE_2_2_0_build_83 ";
	char          nic_version[80];
	char          copts[160];
	octeon_module_handler_t   nethandler;

	cavium_print_msg("-- OCTNIC: Starting Network module for Octeon\n");
	cavium_parse_cvs_string(nic_cvs_tag, nic_version, 80);
	cavium_print_msg("Version: %s\n", nic_version);

    /* get my local slot to init ve name */
	octeon_slot = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_BOARD_SLOT_ID_REG + (1ull<<63)) + 1;
	octeon_board = cvmx_read64_uint8(CPLD_BASE_ADDR + CPLD_BOARD_TYPE_REG + (1ull<<63));
	cavium_print_msg("OCTNIC: ==== get my local slot %d.\n",octeon_slot);
	cavium_print_msg("OCTNIC: ==== get my local board %d.\n",octeon_board);
	
	copts[0] = '\0';
	get_nic_compile_options(copts);
	if(strlen(copts))
		cavium_print_msg("OCTNIC: Driver compile options: %s\n", copts);
	else
		cavium_print_msg("OCTNIC: Driver compile options: NONE\n");


	cavium_memset(octprops, 0, sizeof(void *) * MAX_OCTEON_DEVICES);

#if defined(OCTEON_EXCLUDE_BASE_LOAD)
	if(octeon_base_init_module()) {
		cavium_error("OCTNIC: Octeon initialization failed\n");
		return -EINVAL;
	}
#endif

	/* Register handlers with the BASE driver. For each octeon device that
	   runs the NIC core app, the BASE driver would call the functions
	   below for initialization, reset and shutdown operations. */
	nethandler.startptr = octnet_init_nic_module;
	nethandler.resetptr = octnet_reset_nic_module;
	nethandler.stopptr  = octnet_stop_nic_module;
	nethandler.app_type = CVM_DRV_NIC_APP;
	if(octeon_register_module_handler(&nethandler))
		return -EINVAL;

	cavium_print_msg("-- OCTNIC: Network module loaded for Octeon\n");
	return 0;
}









void
cleanup_module()
{
	cavium_print_msg("-- OCTNIC: Stopping Octeon Network module\n");
	octeon_unregister_module_handler(CVM_DRV_NIC_APP);
#if defined(OCTEON_EXCLUDE_BASE_LOAD)
	octeon_base_exit_module();
#endif
	cavium_print_msg("-- OCTNIC: Octeon Network module is now unloaded\n");
}


/* $Id: octeon_netmain.c 67244 2011-11-19 19:30:15Z mchalla $ */
