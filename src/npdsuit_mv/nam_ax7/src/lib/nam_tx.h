#ifndef __NAM_TX_H__
#define __NAM_TX_H__

#include <sys/types.h>
#include <sys/socket.h>



struct vlan_ports_memebers {
	unsigned int		count;
	unsigned int 	ports[128];
};

typedef enum
{
	NAM_RSTP_PORT_STATE_DISABLE_E = 0,
	NAM_RSTP_PORT_STATE_DISCARD_E,
	NAM_RSTP_PORT_STATE_LEARN_E,
	NAM_RSTP_PORT_STATE_FORWARD_E
}RSTP_PORT_STATE_E;

#ifdef DRV_LIB_CPSS
extern void *osMalloc
(
    IN unsigned long size
);
extern int osPrintf
(
	const char* format,...
);
extern void * osMemSet
(
    IN void * start,
    IN int    symbol,
    IN unsigned long size
);
extern void * osMemCpy
(
    IN void *       destination,
    IN const void * source,
    IN unsigned long       size
);
extern int osPrintfPktTx
(
	const char* format,
	...
);
extern int osPrintfEvt
(
	const char* format,
	...
);
extern int osPrintfErr
(
	const char* format,
	...
);
extern int osPrintfDbg
(
	const char* format,
	...
);

extern int osPrintfPktTx
(
	const char* format, 
	...
);

extern void * osCacheDmaMalloc
(
	unsigned long device,
	unsigned long size
);

extern unsigned long osCacheDmaFree
(
    IN void *ptr
);

extern unsigned long cpssDxChNetIfSdmaSyncTxPacketSend
(
    IN unsigned char                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN unsigned char                                    *buffList[],
    IN unsigned long                                   buffLenList[],
    IN unsigned long                                   numOfBufs
);
extern void cpssDxChVirtNetIfTxBuildParam
(
	IN unsigned int interfaceId,
	IN unsigned short vid,
	IN unsigned long	isTagged,
	IN unsigned char queueIdx,
	IN CPSS_INTERFACE_TYPE_ENT vifType,
	IN unsigned int sendBytes,
	INOUT CPSS_DXCH_NET_TX_PARAMS_STC* txParams
);
extern unsigned long cpssDxChNetIfSdmaSyncTxPacketSend
(
    IN unsigned char                                    devNum,
    IN CPSS_DXCH_NET_TX_PARAMS_STC              *pcktParamsPtr,
    IN unsigned char                                    *buffList[],
    IN unsigned long                                   buffLenList[],
    IN unsigned long                                   numOfBufs
);
#endif

#ifdef DRV_LIB_BCM
#define osMemSet memset
extern void
nam_dump_txrx_packet_brief
/*
 * Function:	nam_dump_txrx_packet_brief
 * Purpose: Dump packets control info
 * Parameters:
 *		(IN) packet - bcm_pkt_t info
 * Returns: 
 *		None. 
 *
 * Notes:	
 */
(
	bcm_pkt_t *packet,
	unsigned char op
);

extern int
bcm_tx
(
	int	unit,
	bcm_pkt_t *	tx_pkt,
	void *	cookie
);

#endif

#if 1
extern unsigned int (*npd_get_global_index_by_devport)
(
	unsigned char 		devNum,
	unsigned char		portNum,
	unsigned int		*eth_g_index
);
extern unsigned int (*npd_get_devport_by_global_index)
(
	unsigned int 		eth_g_index,
	unsigned char 		*devNum,
	unsigned char 		*portNum
);
#else
extern unsigned int npd_get_global_index_by_devport
(
	unsigned char 		devNum,
	unsigned char		portNum,
	unsigned int		*eth_g_index
);
extern unsigned int npd_get_devport_by_global_index
(
	unsigned int 		eth_g_index,
	unsigned char 		*devNum,
	unsigned char 		*portNum
);

#endif
extern int npd_arp_snooping_lookup_arpinfo
(
	unsigned int ifindex,
	unsigned short vlanId,
	unsigned char* pktdata,
	unsigned int *isTrunk,
	unsigned char* devNum,
	unsigned char* portOrTrunkNum
);
extern int npd_vlan_get_all_ports
(
	unsigned short vlanId,
	struct vlan_ports_memebers* untagPortlist,
	struct vlan_ports_memebers* tagPortlist
);
extern int npd_vlan_get_all_active_ports
(
	unsigned short vlanId,
	struct vlan_ports_memebers* untagPortlist,
	struct vlan_ports_memebers* tagPortlist
);

extern void npd_get_port_admin_status
(
	IN unsigned int eth_g_index,
	OUT unsigned int *status
);
extern unsigned int npd_get_port_link_status
(
	IN unsigned int eth_g_index,
	OUT unsigned int *status
);
extern void npd_init_tell_whoami
(
	unsigned char *tName,
	unsigned char lastTeller
);
extern unsigned long nam_packet_wifi_tx_task
(
	void
);

void nam_tx_unicast_pkt
(
	unsigned int interfaceId,
	unsigned int vid,
	unsigned int pktType,
	unsigned int isTagged,
	unsigned char* data,
	unsigned int dataLen
);

void nam_tx_dhcp_broadcast_pkt
(
	void *ports,
	unsigned int isTagged,
	unsigned int pktType,
	unsigned int vid,
	unsigned char* data,
	unsigned int   dataLen	
);

void nam_tx_dhcp_unicast_pkt
(
	unsigned int interfaceId,
	unsigned int vid,
	unsigned int pktType,
	unsigned int isTagged,
	unsigned char* data,
	unsigned int dataLen
);

#endif

