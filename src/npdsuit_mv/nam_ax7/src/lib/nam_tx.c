/*******************************************************************************
Copyright (C) Autelan Technology

This software file is owned and distributed by Autelan Technology 
********************************************************************************

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
*nam_tx.c
*
*
*CREATOR:
*	wujh@autelan.com
*
*DESCRIPTION:
*<some description about this file>
*      In this file The functions prepare for packet Rx from Adapter to 
*      The Virtual Network Interface  .
*
*DATE:
*	11/14/2007	
*	
*******************************************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif

/*kernel part*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

/*#include <linux/list.h>*/
#include <sys/types.h>
#include <sys/socket.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>

/*user part */
#ifdef DRV_LIB_CPSS
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <cpss/extServices/os/gtOs/cpssOsMem.h> /*for dma memory allocate/free*/
#include <gtOs/gtOsSem.h>
#endif
#ifdef DRV_LIB_BCM
#include <bcm/pkt.h>
#include <bcm/types.h>
#endif
#include "sysdef/returncode.h"
#include "nam_log.h"
#include "nam_utilus.h"
#include "nam_rx.h"
#include "nam_tx.h"
#include <sysdef/npd_sysdef.h>

#define NAM_TX_TRUE 1
#define NAM_TX_FALSE 0

#define UNIT0	0
#define UNIT1	1

#define ADPT_TX_THREAD_PRIO		200
#define RSTP_TX_PKT_LEN   		MAX_BPDU_LEN  
#define MSTP_TX_PKT_CACH 		(MAX_BPDU_LEN+10)
#define IGMP_SNP_TX_PKT_LEN		(sizeof(IGMP_SNP_PARAM_STC))
#define DLDP_TX_PKT_LEN			(sizeof(DLDP_PARAM_STC))

#define NAM_SDMA_TX_PER_BUFFER	1536

#define NAM_ARP_SOLICIT_PKTSIZE	80

static char *kapTxTaskName = "VirPktTx";
static char *wifiTxTaskName = "WifiPktTx";
static char *rstpTxTaskName = "RstpPktTx";
static char *igmpsnpTxTaskName = "IgmpSnpPktTx";
static char *dldpTxTaskName = "DLDPPktTx";

extern 	int                 adptVirRxFd;
extern 	int                 adptRstpRxFd;
extern 	int 				adptRstpRxClientFd;
extern 	unsigned char				rstpSocketCreated;
extern	struct sockaddr_un	clieAddr;
extern	socklen_t           clieAddrLen;

#ifdef IGMP_SNOOP_SOCKET_STREAM
extern 	int                 adptIgmpSnpRxFd;
extern 	int 				adptIgmpSnpRxCltFd;
extern 	int		            igmpSnpSockCreated;
extern	struct sockaddr_un	igmpSnpClieAddr;
extern	socklen_t           igmpSnpClieAddrLen;
#endif

extern 	int                 adpt_dldp_RxFd;
extern 	int 				adpt_dldp_RxCltFd;
extern 	unsigned char		dldpSockCreated;
extern	struct sockaddr_un	dldp_Clie_Addr;
extern	socklen_t           dldp_Clie_Addr_Len;

/********************************************************************************************
 * 	nam_dma_cache_malloc
 *
 *	DESCRIPTION:
 *             This function allocate memory-mapped DMA area
 *	INPUT:
 *        	device - asic device number
 *		size	- DMA memory size needed
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              data - memory allocated
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char *	nam_dma_cache_malloc
(       
	IN unsigned char  device,
	IN unsigned int  size
)
{
	unsigned char *data = NULL;
	#ifdef DRV_LIB_CPSS
	data = (unsigned char*)osCacheDmaMalloc(device, size);
	#endif
	#ifdef DRV_LIB_BCM
	data = (unsigned char *)soc_cm_salloc(device, size, "Tx");
	#endif

	return data;
}

/********************************************************************************************
 * 	nam_dma_cache_free
 *
 *	DESCRIPTION:
 *             This function free memory-mapped DMA area
 *	INPUT:
 *               NONE
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_FAIL - pointer null
 *		    GT_OK - DMA memory free successfully
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned int nam_dma_cache_free
(       
	IN unsigned char  device,
	IN unsigned char  *data
)
{
	if(NULL == data) {
		return GT_FAIL;
	}
	#ifdef DRV_LIB_CPSS
	osCacheDmaFree(data);
	#endif
	#ifdef DRV_LIB_BCM
	soc_cm_sfree(device, data);
	#endif

	return GT_OK;
}



/********************************************************************************************
 * 	nam_packet_type_is_Mc
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is Mc or not.
 *             ARP packet is snoopped and transmitted to virtual net interface
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is ARP packet
 *              GT_FALSE - indicate the packet is not ARP packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned long	nam_packet_type_is_Mc
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;

	layer2 = (ether_header_t*)(packetBuff);

	if((layer2->dmac[0] & layer2->dmac[1] & layer2->dmac[2] & layer2->dmac[3] & layer2->dmac[4] & layer2->dmac[5]) 	\
		== 0xff) {
		return GT_TRUE;
	}
	else if(0x01 & layer2->dmac[0]) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}

/*void cpssDxChPacketTxPrintPacketBuffer(unsigned char *buffer,unsigned long buffLen){}*/
void nam_packet_tx_print_packet_buffer(unsigned char *buffer,unsigned long buffLen)
{
	unsigned int i;
	unsigned char lineBuffer[64] = {0}, *bufPtr = NULL;
	unsigned int curLen = 0;

	if(!buffer)
		return;
	
	nam_syslog_pkt_tx("-----------------------TX-----------------------%d\n", buffLen);
	bufPtr = lineBuffer;
	curLen = 0;
	
	for(i = 0;i < buffLen; i++)
	{
		curLen += sprintf(bufPtr,"%02x ",buffer[i]);
		bufPtr = lineBuffer + curLen;
		
		if(0==(i+1)%16) {
			nam_syslog_pkt_tx("%s\n",lineBuffer);
			memset(lineBuffer,0,sizeof(lineBuffer));
			curLen = 0;
			bufPtr = lineBuffer;
		}
	}
	
	if((buffLen%16)!=0)
	{
		nam_syslog_pkt_tx("%s\n",lineBuffer);
	}
	
	nam_syslog_pkt_tx("-----------------------TX-----------------------\n");
}

/*
 *
 *  interfaceId format as follows:
 *  +-----------------------------------------------------------------+
 *  |31-29 |  28-24  |							bit23-bit0   											|
 *  +------+---------+------------------------------------------------+
 *  | Intf | device  |             trunk ID or                        |
 *  | Type | number  |              port number                       |
 *  +------+---------+------------------------------------------------+
 *  bit0	~	bit23  - trunk ID or port number 
 *  bit24	~	bit28	 - device number
 *  bit29	~	bit31	 - interface type
 *  interface type are as follows:
 *  		0	- interface port
 *  		1 - interface vlan
 *			2 - interface vidx
 *			3 - interface trunk
 * 			4 - interface subif
 *	
 */
/*unsigned long cpssDxChCheckePortStatuts(){}*/
unsigned long nam_check_port_statuts
(
	unsigned int port_index,
	unsigned int pvid
)
{
	unsigned char isTagged;
	unsigned int status = 0;
	char buf[NAM_SYSLOG_LINE_BUFFER_SIZE] = {0},*bufptr = NULL;
	int bufLen = 0;
	bufptr = buf;

	bufLen += sprintf(bufptr,"check port %#x ",port_index);
	bufptr = buf + bufLen;
	
	npd_get_port_admin_status(port_index,&status);
	if(!status) {
		bufLen += sprintf(bufptr,"admin(DISABLE) ");
		bufptr = buf + bufLen;
		nam_syslog_err("%s",buf);
		return GT_FAIL;
	}

	status = 0;
	if(0 != npd_get_port_link_status(port_index,&status))
	{
		nam_syslog_err("npd_get_port_link_status failed \n");		
	}
	
	if(!status) {
		bufLen += sprintf(bufptr,"link(DOWN) ");
		bufptr = buf + bufLen;
		nam_syslog_err("%s",buf);
		return GT_FAIL;
	}

	/* TODO: check port and vlan relationship */
	status = 0;
	status = npd_vlan_check_contain_port(pvid,port_index,&isTagged);
	if(!status) {
		bufLen += sprintf(bufptr,"not in vlan(%d)\n",pvid);
		bufptr = buf + bufLen;
		nam_syslog_err("%s",buf);
		return GT_FAIL;
	}

	nam_syslog_dbg("%s",buf);
	return GT_OK;
}

/*unsigned long cpssDxChLookUpParamByVlanid(){}*/
unsigned long nam_lookup_param_by_vlanid
(
	IN unsigned int l3_intf,
	IN unsigned short vlanId,
	IN unsigned char* pktdata,
	OUT unsigned int *isTrunk,
	OUT unsigned char* devNum,
	OUT unsigned char* portOrTrunkNum
	
)
{
	int ret;

	ret = npd_arp_snooping_lookup_arpinfo(l3_intf, vlanId,pktdata,isTrunk,devNum,portOrTrunkNum);
	if(ARP_RETURN_CODE_SUCCESS == ret)
	{
		return GT_TRUE;
	}

	return GT_FALSE;
	
}

/*unsigned long cpssDxChGetPortTag(){}*/
unsigned long nam_get_port_tag
(
	IN unsigned char devNum,
	IN unsigned char portNum,
	IN unsigned short vid,
	OUT unsigned char *isTag
)
{
	int rc = 0;
	unsigned int eth_g_index = 0;


	rc = npd_get_global_index_by_devport(devNum,portNum,&eth_g_index);
	if(0 != rc) {
		return GT_FAIL;
	}
	else if(GT_FALSE != (rc = npd_vlan_check_contain_port(vid,eth_g_index,isTag))) {
		return GT_OK;
	}

	return GT_FAIL;
}

/*unsigned char * getPacketTypeStr(){}*/
unsigned char * nam_get_packet_type_str
(
	enum NAM_PACKET_TYPE_ENT packetType
)
{
	if(packetType > NAM_PACKET_TYPE_OTHER){
		return "UNKNOWN";
	}
	
	return namPacketTypeStr[packetType];
}

unsigned int
nam_packet_fillup_vlan_tags
/*
 * Function:	nam_packet_fillup_vlan_tags
 * Purpose: Process packets with vlan tag(s) and
 *		 add all vlan tag(s) bytes to the raw packet
 * Parameters:
 *		(IN) packet - raw packet stream
 *		(IN) tpidOuter - TPID of 802.1Q
 *						outer 802.1Q if double tagged Q-in-Q packet
 *		(IN) tagOuter - vlan tag need to add to packet
 *						outer vlan tag if double tagged Q-in-Q packet
 *		(IN) tpidInner - TPID of 802.1Q
 *						inner 802.1Q if double tagged Q-in-Q packet
 *		(IN) tagInner - vlan tag extract from packet
 *						inner vlan tag if double tagged Q-in-Q packet
 *						0 if single tagged packet
 *		(IN)moveBytes - Bytes need to move backward
 * Returns: 
 *		-1 - failed
 *		0 - success
 * Notes:	
 */
(
	unsigned char *packet,
	unsigned short tpidOuter,
	unsigned short tagOuter,
	unsigned short tpidInner,
	unsigned short tagInner,
	unsigned int moveBytes
)
{
	unsigned short *wordPtr = NULL, vid = 0;
	unsigned char *newHdr = NULL;
	unsigned int i = 0, byte2move = 0;
	
	if (NULL == packet) {
		return -1;
	}

	/* move forward to skip DMAC and SMAC field*/
	newHdr = packet;
	if(moveBytes &&
		(4 == moveBytes || 8 == moveBytes)) {
		newHdr = packet - moveBytes;
	}

	if(newHdr < packet) {
		for(i = 0; i < 12; i++) {
			newHdr[i] = packet[i];
 		}
		if(4 == moveBytes) {
			wordPtr = (unsigned short*)&newHdr[12];
			*(wordPtr++) = tpidOuter;
			*(wordPtr++) = tagOuter;
		}
		else if(8 == moveBytes) {
			wordPtr = (unsigned short*)&newHdr[12];
			*(wordPtr++) = tpidOuter;
			*(wordPtr++) = tagOuter;
			*(wordPtr++) = tpidInner;
			*(wordPtr++) = tagInner;
		}
	}

	return 0;
}

/*void cpssDxChTxBroadcastPkt(){}*/
void nam_tx_broadcast_pkt
(
	struct vlan_ports_memebers* portsPtr,
	unsigned int isTagged,
	unsigned int pktType,
	unsigned int vid,
	unsigned char* data,
	unsigned int   dataLen	
)
{
	unsigned long	i = 0;
	unsigned char 	devNum = 0,portNum = 0;

	#ifdef DRV_LIB_CPSS
	unsigned int interfaceId = 0;
	unsigned char	*buffArr[BUFF_LEN] = {NULL}, queueIdx = 0;
	unsigned long	buffLenArr[BUFF_LEN] = {0};
	CPSS_DXCH_NET_TX_PARAMS_STC virTxBuffParam;
	#endif
	#ifdef DRV_LIB_BCM
	bcm_pkt_t *pktData = NULL;
    bcm_pbmp_t u0tx_pbmp; 
    bcm_pbmp_t u1tx_pbmp;
	unsigned long result = 0;
	ether_8021q_t dot1q;
	unsigned char *rawPktDup = NULL;
	#endif
	
	#ifdef DRV_LIB_CPSS	
	enum NAM_PACKET_TYPE_ENT packetType = NAM_PACKET_TYPE_OTHER;

	buffArr[0] = data;
	buffLenArr[0] = dataLen;

	memset(&virTxBuffParam, 0, sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));

	packetType = nam_packet_parse_type(data);
	cpssDxChNetIfTxPacketQueueSet(packetType, &queueIdx);
	#endif
	#ifdef DRV_LIB_BCM
	memset(&u0tx_pbmp, 0, sizeof(bcm_pbmp_t));
	memset(&u1tx_pbmp, 0, sizeof(bcm_pbmp_t));
	pktData = (unsigned char*)malloc(sizeof(bcm_pkt_t));
	if(NULL == pktData) {
		nam_syslog_err("alloc memory error when send solicit arp from port(%d,%d) vlan %d\n", 
				devNum, portNum, vid);
		return ;
	}
	memset(pktData, 0,sizeof(bcm_pkt_t));

	/* fillup packet buffer*/
	pktData->dma_channel = 0;
	pktData->pkt_data = &(pktData->_pkt_data);
	pktData->blk_count = 1;
	pktData->pkt_data[0].data = data;
	pktData->pkt_data[0].len = dataLen;
	pktData->pkt_len = dataLen;

	/*setup packet vlan tag field*/
	dot1q.tpid = 0x8100;
	dot1q.cfi = 0;
	dot1q.pri = 0;
	dot1q.vid = (vid & 0xFFF);
	memcpy(pktData->_vtag, &dot1q, 4);
	
	pktData->flags |= BCM_TX_CRC_APPEND;
	pktData->flags |= BCM_PKT_F_NO_VTAG;
	if(!isTagged) {
		pktData->flags |= BCM_PKT_F_TX_UNTAG;
	}
	#endif
	
	for(i = 0; i < portsPtr->count; i++){
		#ifdef DRV_LIB_CPSS		
		interfaceId = 0;devNum = 0;portNum =0;
		npd_get_devport_by_global_index(portsPtr->ports[i],&devNum,&portNum);
		interfaceId = (unsigned int)portNum;
		interfaceId =  (interfaceId << 8) |(unsigned int)devNum;
		cpssDxChVirtNetIfTxBuildParam(interfaceId,vid,isTagged, queueIdx,pktType,dataLen,&virTxBuffParam);
		cpssDxChNetIfSdmaSyncTxPacketSend(devNum,&virTxBuffParam,buffArr, buffLenArr,1);
		#endif 

		#ifdef DRV_LIB_BCM
		devNum = 0;
		portNum = 0;
		result = npd_get_devport_by_global_index(portsPtr->ports[i], &devNum, &portNum);
		if (result != 0) {
			nam_syslog_err("global index %d get port (%d/%d) error, result %d!\n",
							portsPtr->ports[i], devNum, portNum, result);
			continue;
			/*free(pktData);*/
			/*return ;*/
		}
		nam_syslog_dbg("\tadd asic port(%d,%d) to port list\r\n", devNum, portNum);
		switch (devNum) {
			case 0:
				BCM_PBMP_PORT_ADD(u0tx_pbmp, portNum);
				break;
			case 1:
				BCM_PBMP_PORT_ADD(u1tx_pbmp, portNum);
				break;
			default:
				nam_syslog_err("global index %d get port (%d/%d), devNum is error!\n",
								portsPtr->ports[i], devNum, portNum);
		}
		#endif
	}

	#ifdef DRV_LIB_BCM
	/* check if packet need also send to unit 1*/
	if (BCM_PBMP_NOT_NULL(u0tx_pbmp)) {
		/* tx paceket to unit 0 ports*/
		pktData->src_mod = UNIT0;
		pktData->src_port = 0;
		pktData->dest_mod = UNIT0;
		pktData->dest_port = 0;

		if (isTagged) {
			memcpy(&pktData->tx_pbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));

		} else {
			memcpy(&pktData->tx_pbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));
			memcpy(&pktData->tx_upbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));
		}

		/* deliver it*/
		nam_syslog_dbg("raw packet dma buffer for unit 0 %p\n", data);
		result = bcm_tx(UNIT0, pktData, NULL);
		if (result) {
			nam_syslog_err("bcm tx arp solicit packet error %d on unit 0\n", result);
			free(pktData);
			return ;
		}
	}

	/* check if packet need also send to unit 1*/
	if (BCM_PBMP_NOT_NULL(u1tx_pbmp)) {
		pktData->flags |= BCM_TX_CRC_APPEND;
		pktData->flags |= BCM_PKT_F_NO_VTAG;
		if(!isTagged) {
			pktData->flags |= BCM_PKT_F_TX_UNTAG;
		}
	
		/* tx paceket to unit 1 ports*/
		rawPktDup = (unsigned char*)nam_dma_cache_malloc(UNIT1, dataLen);
		if (NULL == rawPktDup)
		{
			nam_syslog_err("malloc dma memory for unit 1 error when tx broadcast packet\n");
			free(pktData);
			return ;
		}
		memset(rawPktDup, 0, dataLen);
		nam_syslog_dbg("\traw packet dma buffer for unit 1 %p\n", rawPktDup);
		
		/* copy data from unit0*/
		memcpy(rawPktDup, data, dataLen);

		/* rebuilt packet data*/
		pktData->dma_channel = 0;
		pktData->pkt_data = &(pktData->_pkt_data);
		pktData->blk_count = 1;
		pktData->pkt_data[0].data = rawPktDup;
		pktData->pkt_data[0].len = dataLen;
		pktData->pkt_len = dataLen;
		pktData->src_mod = UNIT1;
		pktData->src_port = 0;
		pktData->dest_mod = UNIT1;
		pktData->dest_port = 0;		

		if(isTagged) {
			memcpy(&pktData->tx_pbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));
		}else {
			memcpy(&pktData->tx_pbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));
			memcpy(&pktData->tx_upbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));
		}

		/* deliver it*/
		result = bcm_tx(UNIT1, pktData, NULL);
		if(result) {
			nam_syslog_err("bcm tx arp solicit packet error %d\n", result);
			nam_dma_cache_free(UNIT1, rawPktDup);
			free(pktData);
			return ;
		}

		nam_dma_cache_free(UNIT1, rawPktDup);
		rawPktDup = NULL;
	}

	nam_dump_txrx_packet_brief(pktData, 0);
	free(pktData);
	#endif

	return ;
}


/*void cpssDxChTxUnicastPkt(){}*/
void nam_tx_unicast_pkt
(
	unsigned int interfaceId,
	unsigned int vid,
	unsigned int pktType,
	unsigned int isTagged,
	unsigned char* data,
	unsigned int dataLen
)
{
	#ifdef DRV_LIB_CPSS
	unsigned char	*buffArr[BUFF_LEN] = {NULL}, queueIdx = 0;
	unsigned long	buffLenArr[BUFF_LEN] = {0};
	CPSS_DXCH_NET_TX_PARAMS_STC virTxBuffParam ;
	unsigned int ret = 0;
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	#endif
	#ifdef DRV_LIB_BCM
	bcm_pkt_t *pktData = NULL;
	bcm_pbmp_t u0tx_pbmp; 
	bcm_pbmp_t u1tx_pbmp;
	unsigned char devNum = 0;
	unsigned char portNum = 0;
	ether_8021q_t dot1q;
	unsigned char *rawPktDup = NULL;
	#endif
	
	long rc = 0;
	unsigned long result = 0;
	#ifdef DRV_LIB_CPSS
	enum NAM_PACKET_TYPE_ENT packetType = NAM_PACKET_TYPE_OTHER;

	buffArr[0] = data;
	buffLenArr[0] = dataLen;

	packetType = nam_packet_parse_type(data);
	cpssDxChNetIfTxPacketQueueSet(packetType, &queueIdx);
	
	memset(&virTxBuffParam,0,sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
	cpssDxChVirtNetIfTxBuildParam(interfaceId,vid,isTagged,queueIdx,pktType,dataLen,&virTxBuffParam);
	rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&virTxBuffParam,buffArr, buffLenArr,1);		
	if(GT_OK != rc)
	{
		;/*osPrintf("cpssDxChNetIfSdmaSyncTxPacketSend error %d\n",rc);*/
		nam_syslog_err("cpssDxChNetIfSdmaSyncTxPacketSend error %d\n",rc);
	}
	#endif
	#ifdef DRV_LIB_BCM
	memset(&u0tx_pbmp, 0, sizeof(bcm_pbmp_t));
	memset(&u1tx_pbmp, 0, sizeof(bcm_pbmp_t));

	switch(pktType) {
		default:
			return;
		case NAM_INTERFACE_PORT_E:
			rc = npd_get_devport_by_global_index(interfaceId, &devNum, &portNum);
			if (rc != 0) {
				nam_syslog_err("global index %d get port (%d/%d) error, result %d!\n",
								interfaceId, devNum, portNum, rc);
				return ;
			}
			break;
		case NAM_INTERFACE_VID_E:
			pktType = NAM_INTERFACE_PORT_E;
			devNum = interfaceId & 0x000000ff;
			portNum =  interfaceId >> 8;
			break;
		case NAM_INTERFACE_TRUNK_E:
			devNum = ~0UI;
			portNum = ~0UI;
			break;
	}
	switch (devNum) {
		case 0:
			BCM_PBMP_PORT_ADD(u0tx_pbmp, portNum);
			break;
		case 1:
			BCM_PBMP_PORT_ADD(u1tx_pbmp, portNum);
			break;
		default:
			nam_syslog_err("nam tx unicast packet with wrong type %d outif %#x asic port(%d,%d)\n",
							pktType, interfaceId, devNum, portNum);
			return;
	}
	
	pktData = (unsigned char*)malloc(sizeof(bcm_pkt_t));
	if(NULL == pktData) {
		nam_syslog_err("alloc memory error when send solicit arp from port(%d,%d) vlan %d\n", 
				devNum, portNum, vid);
		return ;
	}
	memset(pktData, 0,sizeof(bcm_pkt_t));

	/* fillup packet buffer*/
	pktData->dma_channel = 0;
	pktData->pkt_data = &(pktData->_pkt_data);
	pktData->blk_count = 1;
	pktData->pkt_data[0].data = data;
	pktData->pkt_data[0].len = dataLen;
	pktData->pkt_len = dataLen;

	/*setup packet vlan tag field*/
	dot1q.tpid = 0x8100;
	dot1q.cfi = 0;
	dot1q.pri = 0;
	dot1q.vid = (vid & 0xFFF);
	memcpy(pktData->_vtag, &dot1q, 4);
	
	/* packet with 4B CRC field*/
	pktData->flags |= BCM_TX_CRC_APPEND;
	pktData->flags |= BCM_PKT_F_NO_VTAG;
	if(!isTagged) {
		pktData->flags |= BCM_PKT_F_TX_UNTAG;
	}
		
	/* tx paceket to unit 0 ports*/
	if (BCM_PBMP_NOT_NULL(u1tx_pbmp)) {
		/* tx paceket to unit 1 ports*/
		rawPktDup = (unsigned char*)nam_dma_cache_malloc(UNIT1, dataLen);
		if (NULL == rawPktDup)
		{
			nam_syslog_err("malloc dma memory for unit 1 error when tx broadcast packet\n");
			free(pktData);
			return ;
		}

		memset(rawPktDup, 0, dataLen);
		nam_syslog_dbg("raw packet dma buffer for unit 1 %p\n", rawPktDup);
		
		/* copy data from unit0*/
		memcpy(rawPktDup, data, dataLen);

		/* rebuilt packet data*/
		pktData->pkt_data = &(pktData->_pkt_data);
		pktData->pkt_data[0].data = rawPktDup;
		pktData->pkt_data[0].len = dataLen;
		pktData->src_mod = UNIT1;
		pktData->dest_mod = UNIT1;
		pktData->dest_port = 0;

		if (isTagged) {
			memcpy(&pktData->tx_pbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));
		}else {
			memcpy(&pktData->tx_pbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));
			memcpy(&pktData->tx_upbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));
		}

		/* deliver it*/
		result = bcm_tx(UNIT1, pktData, NULL);
		if (result) {
			nam_syslog_err("bcm tx arp solicit packet error %d\n", result);
			nam_dma_cache_free(UNIT1, rawPktDup);
			free(pktData);
			return ;
		}

		nam_dma_cache_free(UNIT1, rawPktDup);
		rawPktDup = NULL;
	}
	else if(BCM_PBMP_NOT_NULL(u0tx_pbmp)) {
		pktData->src_mod = UNIT0;
		pktData->src_port = 0;
		pktData->dest_mod = UNIT0;
		pktData->dest_port= portNum;

		/* setup tx port bitmap*/
		if(isTagged) {
			memcpy(&pktData->tx_pbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));
		
		}else {
			memcpy(&pktData->tx_pbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));
			memcpy(&pktData->tx_upbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));
		}

		result = bcm_tx(UNIT0, pktData, NULL);
		if (result) {
			nam_syslog_err("bcm tx arp solicit packet error %d\n", result);
			free(pktData);
			return ;
		}
	}

	nam_dump_txrx_packet_brief(pktData, 0);
	free(pktData);
	#endif

	return ;
}

/*unsigned long appDemoPacketTxVirtNetIfTask(){}*/
unsigned long nam_packet_kap_tx_task
(
	void
)
{
	unsigned long	i = 0, skipI = 0;	/* iterator */
	unsigned char 	devNum = 0,portNum = 0,portOrTrunkNum = 0;
	unsigned char		*buffArr[BUFF_LEN] = {NULL},*tmpPtr = NULL,*txBufBlk = NULL;
	unsigned long	buffLenArr[BUFF_LEN] = {0};
	unsigned long  bytesNum;
	unsigned long  pktNumber = 0;
	VIRT_PKT_INFO_STC  virPkt;
	unsigned int 		interfaceId = 0 ,l3Intf = 0;
	unsigned int mbr_index[64] = {0};
	unsigned int mbr_count = 0,portNum_index = 0,hash_trunk = 0;
	int tmpNum = 1;
	unsigned short vid = 0;
	enum NAM_INTERFACE_TYPE_ENT virRePktType = NAM_INTERFACE_PORT_E;
	enum NAM_PACKET_TYPE_ENT packetType = NAM_PACKET_TYPE_OTHER;
	unsigned long isSend = GT_FALSE,isMc = GT_FALSE,isUc = GT_FALSE,isFund = GT_FALSE;
	struct vlan_ports_memebers untagPorts,tagPorts;
	unsigned char isTag = GT_FALSE;
	unsigned int isTrunk = GT_FALSE;
	static char buf[NAM_SYSLOG_LINE_BUFFER_SIZE] = {0}; 
	char *bufptr =NULL;
	int bufLen = 0;
	bufptr = buf;

	/* tell my thread id */
	npd_init_tell_whoami("VirPktTx",0);

	/*modify by zhubo@autelan.com 7.2.2008*/
	
	devNum = 0;
	txBufBlk = (unsigned char*)nam_dma_cache_malloc(devNum,(BUFF_LEN * NAM_SDMA_TX_PER_BUFFER));
	if(NULL == txBufBlk)
	{
		nam_syslog_err("post-processor malloc return NULL\n");
		return GT_NO_RESOURCE;
	}
	
	osMemSet(txBufBlk,0,(BUFF_LEN * NAM_SDMA_TX_PER_BUFFER));
	osMemSet(&virPkt,0,sizeof(VIRT_PKT_INFO_STC));

    osMemSet(&untagPorts,0,sizeof(struct vlan_ports_memebers));
	osMemSet(&tagPorts,0,sizeof(struct vlan_ports_memebers));
	tmpPtr = txBufBlk;
	for(i=0;i<BUFF_LEN;i++)
	{
		buffArr[i] = tmpPtr;
		tmpPtr = tmpPtr + NAM_SDMA_TX_PER_BUFFER;
	}

	nam_syslog_dbg("kap tx task ioctl fd %d\n", adptVirRxFd);
	while(1)
	{
		if(ioctl(adptVirRxFd,KAPWAITING,&tmpNum))
		{
			nam_syslog_dbg("kap wait for packet ioctl skip %d\r\n", skipI++);
			sleep(1);
			continue;
		}
		else if ((pktNumber = ioctl(adptVirRxFd,KAPRETURNPKGNO,&tmpNum)) > 0)
		{
			if(BUFF_LEN < pktNumber)
			{
				nam_syslog_dbg("kap get packet count %d\r\n", pktNumber);				
				pktNumber = BUFF_LEN;
			}

			bytesNum = 0;
			for(i=0;i<pktNumber;i++)
			{
				virPkt.data_addr = (unsigned int)buffArr[0];
				virPkt.data_len = NAM_SDMA_TX_PER_BUFFER;
				bufLen += sprintf(bufptr,"post-processor %d packet need send,",pktNumber);
				bufptr = buf + bufLen;

			
				devNum = 0;
				portOrTrunkNum = 0;
			
				bytesNum = read(adptVirRxFd,&virPkt,tmpNum);
				if(bytesNum <= 0)
				{
					bufLen += sprintf(bufptr,"read tx buffer error %d\n",bytesNum);
					bufptr = buf + bufLen;
					/*cpssOsCacheDmaFree(txBufBlk);*/
					/*return GT_FAIL;*/
					nam_syslog_err("%s",buf);
					osMemSet(buf,0,NAM_SYSLOG_LINE_BUFFER_SIZE);
					bufptr = buf;
					bufLen = 0;
					osMemSet(txBufBlk,0,(BUFF_LEN * NAM_SDMA_TX_PER_BUFFER));
					continue;
				}
				else if(bytesNum >0)
				{
					buffArr[0] 		= (unsigned char *)virPkt.data_addr;
					buffLenArr[0] = virPkt.data_len;
					packetType = nam_packet_parse_type(buffArr[0]);
					if((NAM_PACKET_TYPE_IP_TCP_TELNET_E != packetType) &&
						(NAM_PACKET_TYPE_IP_TCP_SSH_E != packetType)){
						nam_packet_tx_print_packet_buffer(buffArr[0],buffLenArr[0]);
					}
					#if 0 /* delete by qinhuasong on 08/03/03 */
					interfaceId = cpssDxChVirtNetIfMaskId(CPSS_INTERFACE_PORT_E,0,0);
					#else
					l3Intf = virPkt.l3_index;
					virRePktType =  virPkt.dev_type;
					/* here interface ID must be L3 Intf,so we need to convert it to eth_g_index */
					#endif
					switch(virRePktType)
					{
						case NAM_INTERFACE_PORT_E:							
							interfaceId = virPkt.l2_index;
							bufLen += sprintf(bufptr,"outif PORT %#x,",interfaceId);
							bufptr = buf + bufLen;
							
							isTag = GT_FALSE; /*port in 4095 must be untag*/
							if(packetType == NAM_PACKET_TYPE_LACP_E)
							{
								if(GT_OK != npd_vlan_port_pvid_get(virPkt.l2_index,&vid))
								{
									nam_syslog_err("get %d pvid  failed !\n",virPkt.l2_index);
								}
								virPkt.vId = vid;
							}
							if(GT_OK != nam_check_port_statuts(virPkt.l2_index, virPkt.vId)) {
								isSend = GT_FALSE;
								isUc = GT_FALSE;
								bufLen += sprintf(bufptr,"port status check fail so packet drop!");
								bufptr = buf + bufLen;
							}
							else {
								isSend = GT_TRUE;
								isUc = GT_TRUE;
								bufLen += sprintf(bufptr,"send as unicast\n");
								bufptr = buf + bufLen;
							}
							nam_syslog_dbg("%s\n",bufptr);
							break;
							
						case NAM_INTERFACE_TRUNK_E:
							bufLen += sprintf(bufptr,"outif TRUNK not support,packet %s\n", 
											isSend ? "send":"drop");
							bufptr = buf + bufLen;
							break;
							
						case NAM_INTERFACE_VIDX_E:
						case NAM_INTERFACE_VID_E:
							bufLen += sprintf(bufptr,"outif VLAN,");
							bufptr = buf + bufLen;
							
							isMc = nam_packet_type_is_Mc((unsigned char*)virPkt.data_addr);
							if(!isMc) {	
								/*unicast packets*/
								isFund = nam_lookup_param_by_vlanid(
															l3Intf, 
															virPkt.vId,
															(unsigned char*)virPkt.data_addr,
															&isTrunk,
															&devNum,
															&portOrTrunkNum);
								if((GT_TRUE == isFund)&&(GT_FALSE == isTrunk)) {
									interfaceId = (unsigned int)portOrTrunkNum;
									interfaceId = (interfaceId << 8) | (unsigned int)devNum;
									if(0 == nam_get_port_tag(devNum,portOrTrunkNum,virPkt.vId,&isTag)) {
										isUc = GT_TRUE;		/*fund dest port,send packets by unicast*/
										isSend  = GT_TRUE;

										bufLen += sprintf(bufptr,"unicast %s packet send to port(%d,%d) in vlan %d %s interfaceId %d\n",	\
													nam_get_packet_type_str(packetType),devNum,portOrTrunkNum,virPkt.vId,isTag ? "TAG" : "UNTAG",interfaceId);
										bufptr = buf + bufLen;
									}
									else {
										/*get tag type error ,drop packets by zhubo@autelan.com 2008.7.22*/
										isUc = GT_FALSE;
										isSend = GT_FALSE;
										bufLen += sprintf(bufptr,"unicast %s packet drop as to port(%d,%d) not in vlan %d\n",	\
												nam_get_packet_type_str(packetType),devNum,portOrTrunkNum,virPkt.vId,isTag);
										bufptr = buf + bufLen;
									}
								}
								else if((GT_TRUE == isFund)&&(GT_TRUE == isTrunk)) {
									#if 0
									virRePktType = NAM_INTERFACE_TRUNK_E;
									interfaceId = (unsigned int)portOrTrunkNum;
									interfaceId =  (interfaceId << 8) |(unsigned int)devNum;
									bufLen += sprintf(bufptr,"unicast %s packet send to trunk(%d,%d) in vlan %d\n", \
												nam_get_packet_type_str(packetType),devNum,portOrTrunkNum,virPkt.vId);
									bufptr = buf + bufLen;
									#endif
									/*virRePktType = NAM_INTERFACE_TRUNK_E;*/
									bufLen += sprintf(bufptr,"unicast %s packet send to trunk(%d,%d) in vlan %d\n", \
												nam_get_packet_type_str(packetType),devNum,portOrTrunkNum,virPkt.vId);
									bufptr = buf + bufLen;
									if(npd_vlan_check_contain_trunk(virPkt.vId,portOrTrunkNum,&isTag)) {
										bufLen += sprintf(bufptr,"trunk %d is a member of vlan %d as %s\n",portOrTrunkNum,virPkt.vId,isTag ? "tag" : "untag");
										bufptr = buf + bufLen;
										
										/*choose a port which belongs to this trunk
										  mbr_count : get the number of ports which link up and belong to the trunk*/
										if(npd_trunk_endis_member_port_index_get(portOrTrunkNum,mbr_index,GT_TRUE,&mbr_count)) {
											isUc = GT_TRUE;
											isSend	= GT_TRUE;
											hash_trunk = portOrTrunkNum & 0x003f;/*get trunkId[5:0] for index*/
											portNum_index = hash_trunk % mbr_count;/*choose a port in these trunk ports which link up*/
											interfaceId = (unsigned int)mbr_index[portNum_index];/*get the port_globe_index of the chosen port*/
											npd_get_devport_by_global_index(interfaceId,&devNum,&portNum);/*get port devNo portNo*/
											interfaceId = (unsigned int)(portNum << 8) | (unsigned int)devNum;/*make the devNO and portNO together*/
											bufLen += sprintf(bufptr,"unicast %s packet send to port(%d,%d) in trunk %d interfaceID %d\n",	\
														nam_get_packet_type_str(packetType),devNum,portNum,portOrTrunkNum,interfaceId);
											bufptr = buf + bufLen;

										}
										else {
											bufLen += sprintf(bufptr,"get trunk %d member ports error.\n",portOrTrunkNum);
											bufptr = buf + bufLen;
										}
									}
								}
								else if(GT_FALSE == isFund) {
									/* flood packet if no outif found by qinhs@autelan.com 2009-4-2 */									
									osMemSet(&untagPorts,0,sizeof(struct vlan_ports_memebers));
									osMemSet(&tagPorts,0,sizeof(struct vlan_ports_memebers));
									if(npd_vlan_get_all_active_ports(virPkt.vId,&untagPorts,&tagPorts)){
										bufLen += sprintf(bufptr,"unicast %s packet drop as to no port in vlan %d\n",
															nam_get_packet_type_str(packetType),virPkt.vId);
										bufptr = buf + bufLen;
										
										isMc = GT_FALSE;
										isSend = GT_FALSE;
									}
									else {		
										isMc = GT_TRUE;
										isSend = GT_TRUE;
										bufLen += sprintf(bufptr,"unicast %s packet flood in vlan %d\n",
															nam_get_packet_type_str(packetType),virPkt.vId);
										bufptr = buf + bufLen;
									}
								}

							}
							else  {
								/*broadcast packets*/
								osMemSet(&untagPorts,0,sizeof(struct vlan_ports_memebers));
								osMemSet(&tagPorts,0,sizeof(struct vlan_ports_memebers));
								if(npd_vlan_get_all_active_ports(virPkt.vId,&untagPorts,&tagPorts)){
									bufLen += sprintf(bufptr,"multicast packet drop as to no port in vlan %d\n",virPkt.vId);
									bufptr = buf + bufLen;
									
									isMc = GT_FALSE;
									isSend = GT_FALSE;
								}
								else {
									isSend = GT_TRUE;
									isMc = GT_TRUE;
									bufLen += sprintf(bufptr,"multicast packet flood in vlan %d\n",virPkt.vId);
									bufptr = buf + bufLen;
								}
							}
							break;
						default:
							bufLen += sprintf(bufptr,"outif %d vlan %d l3if %d l2index %#x %s\n",
											virRePktType,virPkt.vId,l3Intf,virPkt.l2_index, isSend ? "send":"drop");
							bufptr = buf + bufLen;
							break;
					}

					nam_syslog_dbg("%s",buf);
					bufLen = 0;
					bufptr = buf;
					
					/*only send unicast packets*/
					if((GT_TRUE == isSend) && (GT_TRUE == isUc)) {
						nam_tx_unicast_pkt(interfaceId,virPkt.vId,virRePktType,isTag,virPkt.data_addr,virPkt.data_len);
						/*go back init status*/
						isSend = GT_FALSE;isUc = GT_FALSE;isFund = GT_FALSE;isMc = GT_FALSE;
					}/*only send broadcast packets*/
					else if((GT_TRUE == isSend) && (GT_TRUE == isMc)){							
						bufLen += sprintf(bufptr,"send packets to all %d untagged ports and %d tag ports in vlan %d\n",	
										untagPorts.count,tagPorts.count, virPkt.vId);
						bufptr = buf + bufLen;

						nam_tx_broadcast_pkt(&untagPorts,GT_FALSE,virRePktType,virPkt.vId, virPkt.data_addr, virPkt.data_len);
						nam_tx_broadcast_pkt(&tagPorts,GT_TRUE,virRePktType,virPkt.vId, virPkt.data_addr, virPkt.data_len);	
						isSend = GT_FALSE;isUc = GT_FALSE;isFund = GT_FALSE;isMc = GT_FALSE;
					}	
					else if(GT_FALSE == isSend) {
						/*drop packets by zhubo@autelan.com 2008.7.22*/
						isSend = GT_FALSE;isUc = GT_FALSE;isFund = GT_FALSE;isMc = GT_FALSE;
					}
					nam_syslog_dbg("%s",bufptr);
				}/* end else if(...) */
				bytesNum = 0;
			} /* end for(...) */
			/*cpssOsCacheDmaFree(txBufBlk);*/
			osMemSet(txBufBlk,0,(BUFF_LEN* NAM_SDMA_TX_PER_BUFFER));
			nam_syslog_dbg("%s",buf);
			osMemSet(buf,0,NAM_SYSLOG_LINE_BUFFER_SIZE);
			bufptr = buf;
			bufLen = 0;
		} /* end if(...) */
	} /* end while(..) */		
}		

/*unsigned long  appDemoPacketTxRs(){}*/

unsigned long nam_packet_rstp_tx_task
(
	void
)
{ 
	unsigned long	i = 0;	/* iterator */
	unsigned char 	devNum = 0;
	unsigned long  pktNumber=0;
	unsigned long rc;
	#if 0
	int       stcLen = sizeof(BPDU_PKT_PARAM_STC);
	#endif
	unsigned char *buffArr[BUFF_LEN] = {NULL};
	unsigned char *tmpPtr = NULL;

	#ifdef RSTP_SOCKET_DGRAM
	#ifdef DRV_LIB_CPSS
	CPSS_DXCH_NET_TX_PARAMS_STC rstpTxBuffParam;
	char   queueIdx = 0;
	#endif
	
	#ifdef DRV_LIB_BCM
	bcm_pkt_t *pktData = NULL;
    bcm_pbmp_t u0tx_pbmp; 
    bcm_pbmp_t u1tx_pbmp;
	unsigned char portNum = 0;
	ether_8021q_t dot1q;
	unsigned char *buffArr1[BUFF_LEN] = {NULL};
	unsigned char *tmpPtr1 = NULL;
	#endif
	
	unsigned int interfaceId = 0;
	unsigned long       bytesNum=0;
	volatile int select_ret;
	char   buff[MSTP_TX_PKT_CACH] ={0};
	fd_set fds;
	unsigned long buffLenArr[BUFF_LEN] = {0};
	#endif
	static char buf[NAM_SYSLOG_LINE_BUFFER_SIZE] = {0};
	char *bufptr =NULL;
	int bufLen = 0;
	bufptr = buf;

	/* tell my thread id */
	npd_init_tell_whoami("RstpPktTx",0);

	bufLen += sprintf(bufptr,"appDemoPacketTxRstpTask :: ");
	bufptr = buf + bufLen;

	pktNumber = BUFF_LEN;

	/* for BCM unit 0, for MV device 0*/
	tmpPtr = (unsigned char*)nam_dma_cache_malloc(UNIT0, pktNumber * RSTP_TX_PKT_LEN);
	if(NULL == tmpPtr)
	{
		bufLen += sprintf(bufptr,"malloc return NULL\n");
		bufptr = buf + bufLen;
		nam_syslog_dbg("%s",buf);
		return GT_NO_RESOURCE;
	}
	memset(tmpPtr,0,(pktNumber * RSTP_TX_PKT_LEN));

	for(i=0;i<pktNumber;i++)
	{
		buffArr[i] = tmpPtr;
		tmpPtr = tmpPtr + RSTP_TX_PKT_LEN;
	}

	#ifdef DRV_LIB_BCM
	/* for unit 1*/
	tmpPtr1 = (unsigned char*)nam_dma_cache_malloc(UNIT1, pktNumber * RSTP_TX_PKT_LEN);
	if (NULL == tmpPtr1) {
		nam_dma_cache_free(UNIT0, buffArr[0]);
		bufLen += sprintf(bufptr,"malloc return NULL\n");
		bufptr = buf + bufLen;
		nam_syslog_dbg("%s",buf);
		return GT_NO_RESOURCE;
	}
	memset(tmpPtr1, 0, (pktNumber * RSTP_TX_PKT_LEN));

	nam_syslog_dbg("nam_packet_rstp_tx_task: malloc mem for unit 1\n");

	for (i = 0; i < pktNumber; i++) {
		buffArr1[i] = tmpPtr1;
		tmpPtr1 = tmpPtr1 + RSTP_TX_PKT_LEN;
	}
	#endif

	while(1)
	{
		 /*#ifdef RSTP_SOCKET_STREAM*/
		 #if 0
		 if(GT_TRUE != rstpSocketCreated)
		 {
			if ((adptRstpRxClientFd = accept(adptRstpRxFd, (struct sockaddr *)&clieAddr, &clieAddrLen)) == -1) 
			{
				nam_syslog_err("appDemoPacketTxRstpTask::socket accept fd %d error\n",adptRstpRxClientFd);	
			}
			rstpSocketCreated = GT_TRUE;
			nam_syslog_err("appDemoPacketTxRstpTask::socket accept fd %d\n",adptRstpRxClientFd);
		 }
		 
		FD_ZERO(&fds); /*clear the fd_set*/
		FD_SET(adptRstpRxClientFd,&fds);
 
		select_ret = select(adptRstpRxClientFd+1,&fds,NULL,NULL,NULL);
		if(-1 == select_ret)
		{
			nam_syslog_dbg("appDemoPacketTxRstpTask::select() error,return value %d\n",select_ret);
			cpssOsCacheDmaFree(buffArr[0]);
			return GT_FAIL;
		}
		else if(0 == select_ret)
		{
			nam_syslog_dbg("appDemoPacketTxRstpTask::select() get %d\n",select_ret);
		}
		else
		{
			nam_syslog_dbg("appDemoPacketTxRstpTask::select() return %d get rfds %#08x\n",select_ret,fds);
			if(FD_ISSET(adptRstpRxClientFd,&fds))
			{
				nam_syslog_dbg("appDemoPacketTxRstpTask::FD_ISSET fd %d\r\n",adptRstpRxClientFd);
				/*buff = (char *)malloc(RSTP_TX_PKT_LEN + stcLen);
				if(NULL==buff)
				{
					nam_syslog_dbg("appDemoPacketTxRstpTask:: memory malloc error\n");
					nam_dma_cache_free(buffArr[0]);
					return GT_NO_RESOURCE;
				} */
				osMemSet(buff,0,MSTP_TX_PKT_CACH);

				nam_syslog_dbg("appDemoPacketTxRstpTask::before recv,get buffer %p\n",buff);
				if((bytesNum = recv(adptRstpRxClientFd,(char*)buff,(MSTP_TX_PKT_CACH),0))== -1)
				{
					/*free(buff);*/
					continue;
				}
			
				nam_syslog_dbg("appDemoPacketTxRstpTask::after recv,get %d bytes\n",bytesNum);
				nam_syslog_dbg("appDemoPacketTxRstpTask::buffer array [0][%p]\n",buffArr[0]);
				/*cpssDxChPacketTxPrintPacketBuffer((unsigned char*)&(buff->pktData),buff->pktLen);*/
				if(bytesNum > 0)
				{
					nam_syslog_dbg("appDemoPacketTxRstpTask::recv() ok,and the pakcet %d bytes\r\n",bytesNum);
					/*parse the data recv from Rstp*/
					/*cpssDxChPacketTxPrintPacketBuffer((unsigned char*)&(buff->pktData),buff->pktLen);*/
					interfaceId = ((BPDU_PKT_PARAM_STC *)buff)->intfId;
					nam_syslog_dbg("appDemoPacketTxRstpTask::recv from RSTP get intf %08x len %d\n", \
										interfaceId,((BPDU_PKT_PARAM_STC *)buff)->pktLen);
					cpssDxChVirtNetIfTxBuildParam(interfaceId,1,CPSS_INTERFACE_PORT_E,((BPDU_PKT_PARAM_STC *)buff)->pktLen,&rstpTxBuffParam);
					osMemCpy(buffArr[0],(buff+sizeof(BPDU_PKT_PARAM_STC)),((BPDU_PKT_PARAM_STC *)buff)->pktLen);
					buffLenArr[0] = ((BPDU_PKT_PARAM_STC *)buff)->pktLen;
					rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&rstpTxBuffParam,buffArr, buffLenArr,1);
					if(rc != GT_OK)
					{
						nam_syslog_err("appDemoPacketTxRstpTask::NetIfSdmaSyncTxPacketSend:error\r\n");
					}
					else
					{
						nam_syslog_dbg("appDemoPacketTxRstpTask::NetIfSdmaSyncTxPacketSend:ok\r\n");
					}
				}
				else
				{
					nam_syslog_err("appDemoPacketTxRstpTask::recv() pakcet %d bytes,error\r\n",bytesNum);
				}
				/*free(buff);*/
			} /* end if(FD_ISSET(..)) */
			else 
			{
				nam_syslog_err("appDemoPacketTxRstpTask::FD_ISSET no\r\n");
			}
		}
		#endif

		#ifdef RSTP_SOCKET_DGRAM
		rstpSocketCreated = GT_TRUE;
		clieAddrLen = sizeof(struct sockaddr);
		FD_ZERO(&fds); 							/*clear the fd_set*/
		FD_SET(adptRstpRxFd,&fds);

		select_ret = select(adptRstpRxFd+1,&fds,NULL,NULL,NULL);
		if(-1 == select_ret)
		{
			bufLen += sprintf(bufptr,"select error select_ret %d\n",select_ret);
			bufptr = buf + bufLen;
			nam_syslog_dbg("%s",buf);
			
			/* for BCM unit 0, for MV device 0*/
			nam_dma_cache_free(UNIT0, buffArr[0]);
			#ifdef DRV_LIB_BCM
			nam_dma_cache_free(UNIT1, buffArr1[0]);
			#endif		
			return GT_FAIL;
		}
		else if(0 == select_ret)
		{
			bufLen += sprintf(bufptr,"select return 0 \n");
			bufptr = buf + bufLen;
		}
		else
		{
			if(FD_ISSET(adptRstpRxFd,&fds))
			{

				memset(buff,0,MSTP_TX_PKT_CACH);
				if((bytesNum = recvfrom(adptRstpRxFd,(char*)buff,(MSTP_TX_PKT_CACH),0,(struct sockaddr*)&clieAddr,
          	       &clieAddrLen ))== -1)
				{
					bufLen += sprintf(bufptr,"rstp recvfrom error  -1\n");
					bufptr = buf + bufLen;
					/*free(buff);*/
					nam_syslog_dbg("%s",buf);
					memset(buf,0,NAM_SYSLOG_LINE_BUFFER_SIZE);
					bufptr = buf;
					bufLen = 0;
					/* delay for a while */
					sleep(1);
					continue;
				}
			
				/*cpssDxChPacketTxPrintPacketBuffer((unsigned char*)&(buff->pktData),buff->pktLen);*/
				if(bytesNum > 0)
				{
					/*parse the data recv from Rstp*/
					/*cpssDxChPacketTxPrintPacketBuffer((unsigned char*)&(buff->pktData),buff->pktLen);*/
					interfaceId = ((BPDU_PKT_PARAM_STC *)buff)->intfId;
					bufLen += sprintf(bufptr,"get stp from port_index<%d> packet length %d \n",		\
						interfaceId,((BPDU_PKT_PARAM_STC *)buff)->pktLen);
					bufptr = buf + bufLen;
					
					#ifdef DRV_LIB_CPSS
					cpssDxChNetIfTxPacketQueueSet(NAM_PACKET_TYPE_BPDU_E, &queueIdx);
					memset(&rstpTxBuffParam,0,sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
					cpssDxChVirtNetIfTxBuildParam(interfaceId,1,GT_FALSE,queueIdx,CPSS_INTERFACE_PORT_E,	\
																((BPDU_PKT_PARAM_STC *)buff)->pktLen,&rstpTxBuffParam);
					osMemCpy(buffArr[0],(buff + sizeof(BPDU_PKT_PARAM_STC)),((BPDU_PKT_PARAM_STC *)buff)->pktLen);
					buffLenArr[0] = ((BPDU_PKT_PARAM_STC *)buff)->pktLen;
					rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&rstpTxBuffParam,buffArr, buffLenArr,1);
					if(rc != GT_OK)
					{
						bufLen += sprintf(bufptr,",send  stp error %d \n", rc);
						bufptr = buf + bufLen;
					}
					#endif

					#ifdef DRV_LIB_BCM
					memset(&u0tx_pbmp, 0, sizeof(bcm_pbmp_t));
					memset(&u1tx_pbmp, 0, sizeof(bcm_pbmp_t));

					rc = npd_get_devport_by_global_index(interfaceId, &devNum, &portNum);
					if (rc != GT_OK) {
						nam_syslog_err("global eth-port %#x get asic port(%d,%d) error, result %d!\n",
										interfaceId, devNum, portNum, rc);
						continue;
					}
					nam_syslog_dbg("nam_packet_rstp_tx_task: global eth-port %#x get asic port(%d,%d) \n",
										interfaceId, devNum, portNum);
					switch (devNum) {
						case 0:
							memcpy(buffArr[0],(buff + sizeof(BPDU_PKT_PARAM_STC)),((BPDU_PKT_PARAM_STC *)buff)->pktLen);
							buffLenArr[0] = ((BPDU_PKT_PARAM_STC *)buff)->pktLen;
							BCM_PBMP_PORT_ADD(u0tx_pbmp, portNum);
							break;
						case 1:
							memcpy(buffArr1[0],(buff + sizeof(BPDU_PKT_PARAM_STC)),((BPDU_PKT_PARAM_STC *)buff)->pktLen);
							buffLenArr[0] = ((BPDU_PKT_PARAM_STC *)buff)->pktLen;
							BCM_PBMP_PORT_ADD(u1tx_pbmp, portNum);
							break;
						default:
							nam_syslog_err("nam tx rstp packet with wrong eth-port %#x asic-port(%d,%d)\n",
									interfaceId, devNum, portNum);
							continue;
					}
					
					pktData = (unsigned char*)malloc(sizeof(bcm_pkt_t));
					if(NULL == pktData) {
						nam_syslog_err("alloc memory error when send rstp from port(%d,%d) vlan 1\n", 
								devNum, portNum);
						rc = nam_dma_cache_free(UNIT0, buffArr[0]);
						rc = nam_dma_cache_free(UNIT1, buffArr1[0]);
						return GT_FAIL;
					}
					memset(pktData, 0, sizeof(bcm_pkt_t));

					nam_syslog_dbg("nam_packet_rstp_tx_task:malloc pktData\n");

					/*s fillup packet buffer*/
					pktData->dma_channel = 0;
					pktData->pkt_data = &(pktData->_pkt_data);
					pktData->blk_count = 1;
					pktData->pkt_data[0].data = buffArr[0];
					pktData->pkt_data[0].len = buffLenArr[0];
					pktData->pkt_len = buffLenArr[0];
					
					/*setup packet vlan tag field*/
					dot1q.tpid = 0x8100;
					dot1q.cfi = 0;
					dot1q.pri = 0;
					dot1q.vid = (1 & 0xFFF);	/* default vid = 1*/
					memcpy(pktData->_vtag, &dot1q, 4);
					
					/* packet with 4B CRC field*/
					pktData->flags |= BCM_TX_CRC_APPEND;
					pktData->flags |= BCM_PKT_F_NO_VTAG;
					pktData->flags |= BCM_PKT_F_TX_UNTAG;	/* untag*/
						
					if (BCM_PBMP_NOT_NULL(u1tx_pbmp)) {
						/* tx paceket to unit 1 ports*/
					
						/* rebuilt packet data*/
						pktData->pkt_data = &(pktData->_pkt_data);
						pktData->pkt_data[0].data = buffArr1[0];
						pktData->pkt_data[0].len = buffLenArr[0];
						pktData->src_mod = UNIT1;
						pktData->dest_mod = UNIT1;
						pktData->dest_port = 0;
					
						/* untag*/
						memcpy(&pktData->tx_pbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));
						memcpy(&pktData->tx_upbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));

						nam_syslog_dbg("nam_packet_rstp_tx_task:bcm_tx unit 1\n");
						/* deliver it*/
						rc = bcm_tx(UNIT1, pktData, NULL);
						if (rc) {
							nam_syslog_err("bcm tx rstp packet in unit 1 error %d\n", rc);
							rc = nam_dma_cache_free(UNIT0, buffArr[0]);
							rc = nam_dma_cache_free(UNIT1, buffArr1[0]);
							free(pktData);
							pktData = NULL;
							return GT_FAIL;
						}
					}
					else if(BCM_PBMP_NOT_NULL(u0tx_pbmp)) {		/* tx paceket to unit 0 ports*/
						pktData->src_mod = UNIT0;
						pktData->src_port = 0;
						pktData->dest_mod = UNIT0;
						pktData->dest_port= portNum;
					
						/* setup tx port bitmap*/
						/* untag*/
						memcpy(&pktData->tx_pbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));
						memcpy(&pktData->tx_upbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));

						nam_syslog_dbg("nam_packet_rstp_tx_task:bcm_tx unit 0\n");
						rc = bcm_tx(UNIT0, pktData, NULL);
						if (rc) {
							nam_syslog_err("bcm tx rstp packet in unit 0 error %d\n", rc);
							rc = nam_dma_cache_free(UNIT0, buffArr[0]);
							rc = nam_dma_cache_free(UNIT1, buffArr1[0]);
							free(pktData);
							pktData = NULL;
							return GT_FAIL;
						}
					}

					nam_dump_txrx_packet_brief(pktData, 0);
					free(pktData);
					pktData = NULL;
					#endif
				}
		    } /* end if(FD_ISSET(..)) */
		}
		nam_syslog_dbg("%s",buf);
		memset(buf,0,NAM_SYSLOG_LINE_BUFFER_SIZE);
		bufptr = buf;
		bufLen = 0;
		#endif
	} /* end while(..) */

	rc = nam_dma_cache_free(UNIT0, buffArr[0]);
	#ifdef DRV_LIB_BCM	
	rc = nam_dma_cache_free(UNIT1, buffArr1[0]);
	#endif

	return rc;
}

#ifdef IGMP_SNOOP_SOCKET_STREAM
extern pthread_mutex_t streamSockId;
/*unsigned long nam_appdemo_packet_tx_rstp_task(){}*/
unsigned long  nam_packet_igmpsnp_tx_task
(
	void
)
{ 
	unsigned long	i = 0;	/* iterator */
	unsigned char 	devNum = 0;
	unsigned int  interfaceId = 0;
	unsigned short vlanId;

	#ifdef DRV_LIB_CPSS
	CPSS_DXCH_NET_TX_PARAMS_STC igmpSnpTxBuffParam;
	char  queueIdx = 0;
	#endif

	#ifdef DRV_LIB_BCM
	bcm_pkt_t *pktData = NULL;
	bcm_pbmp_t u0tx_pbmp; 
	bcm_pbmp_t u1tx_pbmp;
	unsigned char portNum = 0;
	ether_8021q_t dot1q;
	unsigned char *buffArr1[BUFF_LEN] = {NULL};
	unsigned char *tmpPtr1 = NULL;
	#endif
	
	unsigned int  ret = 0;
	unsigned long  bytesNum=0,pktNumber=0;
	unsigned long  	rc;
	volatile  int	select_ret;
	IGMP_SNP_PARAM_STC	*igmpPktBuff = NULL;
	fd_set 	fds;
	unsigned char	*buffArr[BUFF_LEN] = {NULL},*tmpPtr = NULL;
	unsigned long	buffLenArr[BUFF_LEN] = {0};
	unsigned char	byteRecv = 0,byteSend = 0;
	static char buf[NAM_SYSLOG_LINE_BUFFER_SIZE] = {0};
	char *bufptr =NULL;
	int bufLen = 0;
	bufptr = buf;

	/* tell my thread id */
	npd_init_tell_whoami("IgmpSnpPktTx",0);

	bufLen += sprintf(bufptr,"npd igmp snooping tx task ");
	bufptr = buf + bufLen;

	pktNumber = BUFF_LEN;

	/* for BCM unit 0, for MV device 0*/
	tmpPtr = (unsigned char*)nam_dma_cache_malloc(UNIT0, pktNumber * IGMP_SNP_TX_PKT_LEN);
	if(NULL == tmpPtr)
	{
		bufLen += sprintf(bufptr,"malloc return NULL\n");
		bufptr = buf + bufLen;
		nam_syslog_dbg("%s",buf);
		return GT_NO_RESOURCE;
	}
	memset(tmpPtr,0,(pktNumber * IGMP_SNP_TX_PKT_LEN));

	for(i=0;i<pktNumber;i++)
	{
		buffArr[i] = tmpPtr;
		tmpPtr = tmpPtr + IGMP_SNP_TX_PKT_LEN;
	}

	#ifdef DRV_LIB_BCM
	/* for BCM unit 1*/
	tmpPtr1 = (unsigned char*)nam_dma_cache_malloc(UNIT1, pktNumber * IGMP_SNP_TX_PKT_LEN);
	if (NULL == tmpPtr1) {
		ret = nam_dma_cache_free(UNIT0, buffArr[0]);
		bufLen += sprintf(bufptr, "malloc return NULL\n");
		bufptr = buf + bufLen;
		nam_syslog_dbg("%s", buf);
		return GT_NO_RESOURCE;
	}
	memset(tmpPtr1, 0, (pktNumber * IGMP_SNP_TX_PKT_LEN));

	nam_syslog_dbg("nam_packet_igmpsnp_tx_task: malloc mem for unit 1\n");

	for (i = 0; i < pktNumber; i++) {
		buffArr1[i] = tmpPtr1;
		tmpPtr1 = tmpPtr1 + IGMP_SNP_TX_PKT_LEN;
	}
	#endif

	pthread_mutex_lock(&streamSockId);/*osSemWait(streamSockId,0);*//*0 :OS_WAIT_FOREVER*/
	if(GT_TRUE != igmpSnpSockCreated)
	{
		if ((adptIgmpSnpRxCltFd = accept(adptIgmpSnpRxFd, \
										(struct sockaddr *)&igmpSnpClieAddr, \
										&igmpSnpClieAddrLen)) == -1) 
		{
			bufLen += sprintf(bufptr,"accept socket from igmp snooping side error\n");
			bufptr = buf + bufLen;	
		}
		else {
			igmpSnpSockCreated = GT_TRUE;
			bufLen += sprintf(bufptr,"accept socket(%d) from igmp snooping side\n", adptIgmpSnpRxCltFd);
			bufptr = buf + bufLen;				
		}
		pthread_mutex_destroy(&streamSockId);/*osSemDelete(streamSockId);*/
	}

	nam_syslog_dbg("%s", buf);
	memset(buf, 0, NAM_SYSLOG_LINE_BUFFER_SIZE);
	bufptr = buf;
	bufLen = 0;
	
	while(1) {
	   	FD_ZERO(&fds); /*clear the fd_set*/
	   	FD_SET(adptIgmpSnpRxCltFd,&fds);
	   
	   	select_ret = select(adptIgmpSnpRxCltFd+1,&fds,NULL,NULL,NULL);
	   	if(-1 == select_ret)
	   	{
	   		bufLen += sprintf(bufptr,"select error select_ret %d\n",select_ret);
			bufptr = buf + bufLen;
			nam_syslog_dbg("%s",buf);

			/* for BCM unit 0, for MV device 0*/
			ret = nam_dma_cache_free(UNIT0, buffArr[0]);
			#ifdef DRV_LIB_BCM
			ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
			#endif	
	   		return GT_FAIL;
	   	}
	   	else if(0 == select_ret)
	   	{
	   		bufLen += sprintf(bufptr,"select return 0 \n");
			bufptr = buf + bufLen;
	   	}
	   	else
	   	{
	   		if(FD_ISSET(adptIgmpSnpRxCltFd,&fds))
	   		{
	   			igmpPktBuff = (IGMP_SNP_PARAM_STC *)malloc(IGMP_SNP_TX_PKT_LEN);
	   			if(NULL==igmpPktBuff)
	   			{
	   				bufLen += sprintf(bufptr,"malloc NULL \n");
					bufptr = buf + bufLen;
					nam_syslog_dbg("%s",buf);

					/* for BCM unit 0, for MV device 0*/
					ret = nam_dma_cache_free(UNIT0, buffArr[0]);
					#ifdef DRV_LIB_BCM
					ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
					#endif	
	   				return GT_NO_RESOURCE;
	   			}
	   			memset(igmpPktBuff, 0, IGMP_SNP_TX_PKT_LEN);
	   
	   			if((bytesNum = recv(adptIgmpSnpRxCltFd,(char*)igmpPktBuff,IGMP_SNP_TX_PKT_LEN,0))== -1)
	   			{
	   				bufLen += sprintf(bufptr,"igmp snooping recvfrom error  -1\n");
					bufptr = buf + bufLen;
					nam_syslog_dbg("%s",buf);
					memset(buf, 0, NAM_SYSLOG_LINE_BUFFER_SIZE);
					bufptr = buf;
					bufLen = 0;
					/* delay for a while */
					sleep(1);
					free(igmpPktBuff);
	   				continue;
	   			}
				if( 0 == bytesNum ) {
					free(igmpPktBuff);
					break;
				}




	   			/*cpssDxChPacketTxPrintPacketBuffer((unsigned char*)(buff->buf+8),1024);*/
	   			if(bytesNum > 0)
	   			{
	   				/*parse the data recv from IGMP*/
					byteRecv = igmpPktBuff->nlh.nlmsg_len;
					byteSend = byteRecv - sizeof(struct nlmsghdr) - 8;
					/*
	   				cpssDxChPacketTxPrintPacketBuffer((unsigned char*)(buff->buf+8),byteSend);
					*/
					#if 0
	   				interfaceId = *((unsigned int*)(buff->buf));
					vlanId = (unsigned short)*((unsigned int*)(buff->buf+4));
					#endif
					interfaceId	= (unsigned int)igmpPktBuff->ifindex;
					vlanId		= (unsigned short)igmpPktBuff->vlanid;

					bufLen += sprintf(bufptr,"get igmp from port_index<%d> vlan<%d> packet length %d ::appDemoPacketTxIgmpSnpTask \n",		\
						interfaceId,vlanId,byteRecv);
					bufptr = buf + bufLen;

					#ifdef DRV_LIB_CPSS
					cpssDxChNetIfTxPacketQueueSet(NAM_PACKET_TYPE_IP_IGMP_E, &queueIdx);
					memset(&igmpSnpTxBuffParam,0,sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
	   				cpssDxChVirtNetIfTxBuildParam(interfaceId,vlanId,GT_FALSE,queueIdx, 	\
															CPSS_INTERFACE_PORT_E,byteSend,&igmpSnpTxBuffParam);
	   				osMemCpy(buffArr[0], igmpPktBuff->buf, byteSend);

					/* packet padding to 64B */
					if(byteSend < 64) {
						byteSend = 64;
					}
					buffLenArr[0] = byteSend;
	   				rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&igmpSnpTxBuffParam,buffArr, buffLenArr,1);
	   				if(rc != GT_OK)
	   				{
	   					bufLen += sprintf(bufptr,",send  igmp error  %d \n", rc);
						bufptr = buf + bufLen;
	   				}
					#endif
					#ifdef DRV_LIB_BCM
					memset(&u0tx_pbmp, 0, sizeof(bcm_pbmp_t));
					memset(&u1tx_pbmp, 0, sizeof(bcm_pbmp_t));

					ret = npd_get_devport_by_global_index(interfaceId, &devNum, &portNum);
					if (ret != GT_OK) {
						nam_syslog_err("global eth-port %#x get asic port(%d,%d) error, result %d!\n",
										interfaceId, devNum, portNum, ret);
						continue;
					}
					nam_syslog_dbg("nam_packet_igmpsnp_tx_task: global eth-port %#x get asic port(%d,%d) \n",
										interfaceId, devNum, portNum);
					switch (devNum) {
						case 0:
							memcpy(buffArr[0], igmpPktBuff->buf, byteSend);
							/* packet padding to 64B */
							if(byteSend < 64) {
								byteSend = 64;
							}
							buffLenArr[0] = byteSend;
							BCM_PBMP_PORT_ADD(u0tx_pbmp, portNum);
							break;
						case 1:
							memcpy(buffArr1[0], igmpPktBuff->buf, byteSend);
							/* packet padding to 64B */
							if(byteSend < 64) {
								byteSend = 64;
							}
							buffLenArr[0] = byteSend;
							BCM_PBMP_PORT_ADD(u1tx_pbmp, portNum);
							break;
						default:
							nam_syslog_err("nam tx igmpsnp packet with wrong eth-port %#x asic-port(%d,%d)\n",
									interfaceId, devNum, portNum);
							continue;
					}
					
					pktData = (unsigned char*)malloc(sizeof(bcm_pkt_t));
					if(NULL == pktData) {
						nam_syslog_err("alloc memory error when send rstp from port(%d,%d) vlan 1\n", 
								devNum, portNum);
						ret = nam_dma_cache_free(UNIT0, buffArr[0]);
						ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
						return GT_FAIL;
					}
					memset(pktData, 0, sizeof(bcm_pkt_t));

					nam_syslog_dbg("nam_packet_igmpsnp_tx_task:malloc pktData\n");

					/* fillup packet buffer*/
					pktData->dma_channel = 0;
					pktData->pkt_data = &(pktData->_pkt_data);
					pktData->blk_count = 1;
					pktData->pkt_data[0].data = buffArr[0];
					pktData->pkt_data[0].len = buffLenArr[0];
					pktData->pkt_len = buffLenArr[0];
					
					/*setup packet vlan tag field*/
					dot1q.tpid = 0x8100;
					dot1q.cfi = 0;
					dot1q.pri = 0;
					dot1q.vid = (vlanId & 0xFFF);
					memcpy(pktData->_vtag, &dot1q, 4);

					/* packet with 4B CRC field*/
					pktData->flags |= BCM_TX_CRC_APPEND;
					pktData->flags |= BCM_PKT_F_NO_VTAG;
					pktData->flags |= BCM_PKT_F_TX_UNTAG;	/* untag*/
						
					if (BCM_PBMP_NOT_NULL(u1tx_pbmp)) {
						/* tx paceket to unit 1 ports*/
						/* rebuilt packet data*/
						pktData->pkt_data = &(pktData->_pkt_data);
						pktData->pkt_data[0].data = buffArr1[0];
						pktData->pkt_data[0].len = buffLenArr[0];
						pktData->src_mod = UNIT1;
						pktData->dest_mod = UNIT1;
						pktData->dest_port = portNum;
					
						/* untag*/
						memcpy(&pktData->tx_pbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));
						memcpy(&pktData->tx_upbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));

						nam_syslog_dbg("nam_packet_igmpsnp_tx_task:bcm_tx unit 1\n");
						/* deliver it*/
						ret = bcm_tx(UNIT1, pktData, NULL);
						if (ret) {
							nam_syslog_err("bcm tx igmpsnp packet in unit 1 vlanid %d error %d\n", vlanId, ret);
							ret = nam_dma_cache_free(UNIT0, buffArr[0]);
							ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
							free(pktData);
							pktData = NULL;
							return GT_FAIL;
						}
					}
					else if(BCM_PBMP_NOT_NULL(u0tx_pbmp)) {		/* tx paceket to unit 0 ports*/
						pktData->src_mod = UNIT0;
						pktData->src_port = 0;
						pktData->dest_mod = UNIT0;
						pktData->dest_port= portNum;
					
						/* setup tx port bitmap*/
						/* untag*/
						memcpy(&pktData->tx_pbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));
						memcpy(&pktData->tx_upbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));

						nam_syslog_dbg("nam_packet_igmpsnp_tx_task:bcm_tx unit 0\n");
						ret = bcm_tx(UNIT0, pktData, NULL);
						if (ret) {
							nam_syslog_err("bcm tx igmpsnp packet in unit 0 vlanid %d error %d\n", vlanId, ret);
							ret = nam_dma_cache_free(UNIT0, buffArr[0]);
							ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
							free(pktData);
							pktData = NULL;
							return GT_FAIL;
						}
					}

					nam_dump_txrx_packet_brief(pktData, 0);
					free(pktData);
					pktData = NULL;
					#endif
	   			}
	   			free(igmpPktBuff);
	   		} /* end if(FD_ISSET(..)) */
   		}
		nam_syslog_dbg("%s",buf);
		memset(buf, 0, NAM_SYSLOG_LINE_BUFFER_SIZE);
		bufptr = buf;
		bufLen = 0;
	}
	close(adptIgmpSnpRxCltFd);
	/* for BCM unit 0, for MV device 0*/
	ret = nam_dma_cache_free(UNIT0, buffArr[0]);
	#ifdef DRV_LIB_BCM
	ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
	#endif	

	return rc;
}
#endif


/**********************************************************************************
 * nam_packet_dldp_tx_task()
 *	DESCRIPTION:
 *		send packets for dldp tx 
 *
 *	INPUTS:
 *		NULL
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		GT_FAIL			- build fail
 *		GT_NO_RESOURCE	- alloc memory error
 *		GT_BAD_SIZE		- packet length is invalid
 *		GT_OK			- build ok
 *		totalLen			- receive packet length
***********************************************************************************/
unsigned long nam_packet_dldp_tx_task
(
	void
)
{
#if 0
	// DLDP_SOCKET_STREAM
	unsigned long i = 0;	/* iterator */
	unsigned char devNum = 0;
	unsigned int interfaceId = 0;
	unsigned short vlanId = 0;

	#ifdef DRV_LIB_CPSS
	CPSS_DXCH_NET_TX_PARAMS_STC dldpTxBuffParam;
	char  queueIdx = 0;
	#endif

	unsigned int ret = 0;
	unsigned long bytesNum=0,pktNumber=0;
	unsigned long rc = 0;
	volatile int select_ret = 0;
	DLDP_PARAM_STC *dldpPktBuff = NULL;
	fd_set 	fds;
	unsigned char *buffArr[BUFF_LEN] = {NULL};
	unsigned char *tmpPtr = NULL;
	unsigned long buffLenArr[BUFF_LEN] = {0};
	unsigned char byteRecv = 0;
	unsigned char byteSend = 0;
	char buf[NAM_SYSLOG_LINE_BUFFER_SIZE] = {0};
	char *bufptr = NULL;
	int bufLen = 0;

	bufptr = buf;

	/* tell my thread id */
	npd_init_tell_whoami("DLDPPktTx", 0);

	bufLen += sprintf(bufptr, "Packet Tx DLDP Task ");
	bufptr = buf + bufLen;

	pktNumber = BUFF_LEN;

	// for BCM unit 0, for MV device 0
	tmpPtr = (unsigned char*)nam_dma_cache_malloc(UNIT0, pktNumber * DLDP_TX_PKT_LEN);
	if (NULL == tmpPtr)	{
		bufLen += sprintf(bufptr,"malloc return NULL\n");
		bufptr = buf + bufLen;
		nam_syslog_dbg("%s",buf);
		return GT_NO_RESOURCE;
	}
	memset(tmpPtr, 0, (pktNumber * DLDP_TX_PKT_LEN));

	for (i = 0; i < pktNumber; i++)
	{
		buffArr[i] = tmpPtr;
		tmpPtr = tmpPtr + DLDP_TX_PKT_LEN;
	}

	pthread_mutex_lock(&streamSockId);
	if (GT_TRUE != dldpSockCreated)
	{
		#if 0 //
		if ((adpt_dldp_RxCltFd = accept(adpt_dldp_RxFd,
										(struct sockaddr *)&dldp_Clie_Addr,
										&dldp_Clie_Addr_Len)) == -1) 
		{
			bufLen += sprintf(bufptr, "accept error\n");
			bufptr = buf + bufLen;
		}
		#endif

		while ((adpt_dldp_RxCltFd = accept(adpt_dldp_RxFd,
										(struct sockaddr *)&dldp_Clie_Addr,
										&dldp_Clie_Addr_Len)) == -1) 
		{
			nam_syslog_err("tx dldp accept error\n");
		}
		dldpSockCreated = GT_TRUE;
		pthread_mutex_destroy(&streamSockId);
	}

	while(1) {
		nam_syslog_dbg("tx dldp while loop\n");
	   	FD_ZERO(&fds); /*clear the fd_set*/
	   	FD_SET(adpt_dldp_RxCltFd, &fds);
	   
	   	select_ret = select(adpt_dldp_RxCltFd + 1, &fds, NULL, NULL, NULL);
		nam_syslog_dbg("tx dldp select\n");
	   	if (-1 == select_ret)
	   	{
	   		bufLen += sprintf(bufptr, "select error select_ret %d\n", select_ret);
			bufptr = buf + bufLen;
			nam_syslog_dbg("%s", buf);

			// for BCM unit 0, for MV device 0
			ret = nam_dma_cache_free(UNIT0, buffArr[0]);
	   		return GT_FAIL;
	   	}else if(0 == select_ret) {
	   		bufLen += sprintf(bufptr, "select return 0 \n");
			bufptr = buf + bufLen;
	   	}else {
	   		nam_syslog_dbg("tx dldp FD_ISSET\n");
	   		if (FD_ISSET(adpt_dldp_RxCltFd, &fds))
	   		{
	   			dldpPktBuff = (DLDP_PARAM_STC *)malloc(DLDP_TX_PKT_LEN);
	   			if (NULL == dldpPktBuff)
	   			{
	   				bufLen += sprintf(bufptr, "malloc NULL \n");
					bufptr = buf + bufLen;
					nam_syslog_dbg("%s",buf);

					// for BCM unit 0, for MV device 0
					ret = nam_dma_cache_free(UNIT0, buffArr[0]);
	   				return GT_NO_RESOURCE;
	   			}
	   			memset(dldpPktBuff, 0, DLDP_TX_PKT_LEN);
	   	   		nam_syslog_dbg("tx dldp recv\n");
	   			if ((bytesNum = recv(adpt_dldp_RxCltFd, (char*)dldpPktBuff, DLDP_TX_PKT_LEN, 0)) == -1)
	   			{
	   				bufLen += sprintf(bufptr, "DLDP recvfrom error -1\n");
					bufptr = buf + bufLen;
					nam_syslog_dbg("%s", buf);
					memset(buf, 0, NAM_SYSLOG_LINE_BUFFER_SIZE);
					bufptr = buf;
					bufLen = 0;
					
					free(dldpPktBuff);
	   				continue;
	   			}
				if (0 == bytesNum) {
					nam_syslog_dbg("(0 == bytesNum)\n");
					free(dldpPktBuff);
					break;
				}

				nam_syslog_dbg("(0 != bytesNum)\n");

	   			/*cpssDxChPacketTxPrintPacketBuffer((unsigned char*)(buff->buf+8),1024);*/
	   			if (bytesNum > 0)
	   			{
	   				/*parse the data recv from IGMP*/
					byteRecv = dldpPktBuff->nlh.nlmsg_len;
					byteSend = byteRecv - sizeof(struct nlmsghdr) - 12;
					/*
	   				cpssDxChPacketTxPrintPacketBuffer((unsigned char*)(buff->buf+8),byteSend);
					*/
					interfaceId	= (unsigned int)dldpPktBuff->ifindex;
					vlanId		= (unsigned short)dldpPktBuff->vlanid;

					bufLen += sprintf(bufptr,"get DLDP from port_index %d vlan %d packet length %d\n",
									interfaceId, vlanId, byteRecv);
					bufptr = buf + bufLen;

					#ifdef DRV_LIB_CPSS
					cpssDxChNetIfTxPacketQueueSet(NAM_PACKET_TYPE_DLDP_E, &queueIdx);
					memset(&dldpTxBuffParam, 0, sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
	   				cpssDxChVirtNetIfTxBuildParam(interfaceId, vlanId, GT_FALSE, queueIdx,
												CPSS_INTERFACE_PORT_E, byteSend, &dldpTxBuffParam);
	   				memcpy(buffArr[0], dldpPktBuff->buf, byteSend);

					/* packet padding to 64B */
					if (byteSend < 64) {
						byteSend = 64;
					}
					buffLenArr[0] = byteSend;
	   				rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&dldpTxBuffParam,buffArr, buffLenArr,1);
	   				if (rc != GT_OK)
	   				{
	   					bufLen += sprintf(bufptr,",send dldp error %d \n", rc);
						bufptr = buf + bufLen;
	   				}
					#endif
	   			}
	   			free(dldpPktBuff);
	   		}
   		}
		nam_syslog_dbg("%s", buf);
		memset(buf, 0, (NAM_SYSLOG_LINE_BUFFER_SIZE));
		bufptr = buf;
		bufLen = 0;
	}

	close(adpt_dldp_RxCltFd);
	// for BCM unit 0, for MV device 0
	ret = nam_dma_cache_free(UNIT0, buffArr[0]);

	return rc;
#endif

#if 1
	/* DLDP_SOCKET_DGRAM*/
	unsigned long i = 0;	/* iterator */
	unsigned char devNum = 0;
	unsigned int interfaceId = 0;
	unsigned short vlanId = 0;
	unsigned int isTagged = 0;

	#ifdef DRV_LIB_CPSS
	CPSS_DXCH_NET_TX_PARAMS_STC dldpTxBuffParam;
	char  queueIdx = 0;
	#endif

	#ifdef DRV_LIB_BCM
	bcm_pkt_t *pktData = NULL;
	bcm_pbmp_t u0tx_pbmp; 
	bcm_pbmp_t u1tx_pbmp;
	unsigned char portNum = 0;
	ether_8021q_t dot1q;
	unsigned char *buffArr1[BUFF_LEN] = {NULL};
	unsigned char *tmpPtr1 = NULL;
	#endif

	unsigned int ret = 0;
	unsigned long bytesNum=0,pktNumber=0;
	unsigned long rc = 0;
	volatile int select_ret = 0;
	DLDP_PARAM_STC *dldpPktBuff = NULL;
	fd_set 	fds;
	unsigned char *buffArr[BUFF_LEN] = {NULL};
	unsigned char *tmpPtr = NULL;
	unsigned long buffLenArr[BUFF_LEN] = {0};
	unsigned char byteRecv = 0;
	unsigned char byteSend = 0;
	static char buf[NAM_SYSLOG_LINE_BUFFER_SIZE] = {0};
	char *bufptr = NULL;
	int bufLen = 0;
	bufptr = buf;

	/* tell my thread id */
	npd_init_tell_whoami("DLDPPktTx", 0);

	bufLen += sprintf(bufptr, "Packet Tx DLDP Task ");
	bufptr = buf + bufLen;

	pktNumber = BUFF_LEN;

	/* for BCM unit 0, for MV device 0*/
	tmpPtr = (unsigned char*)nam_dma_cache_malloc(UNIT0, pktNumber * DLDP_TX_PKT_LEN);
	if (NULL == tmpPtr)	{
		bufLen += sprintf(bufptr,"malloc return NULL\n");
		bufptr = buf + bufLen;
		nam_syslog_dbg("%s",buf);
		return GT_NO_RESOURCE;
	}
	memset(tmpPtr, 0, (pktNumber * DLDP_TX_PKT_LEN));

	for (i = 0; i < pktNumber; i++)
	{
		buffArr[i] = tmpPtr;
		tmpPtr = tmpPtr + DLDP_TX_PKT_LEN;
	}

	#ifdef DRV_LIB_BCM
	/* for BCM unit 1*/
	tmpPtr1 = (unsigned char*)nam_dma_cache_malloc(UNIT1, pktNumber * DLDP_TX_PKT_LEN);
	if (NULL == tmpPtr1) {
		ret = nam_dma_cache_free(UNIT0, buffArr[0]);
		bufLen += sprintf(bufptr, "malloc return NULL\n");
		bufptr = buf + bufLen;
		nam_syslog_dbg("%s", buf);
		return GT_NO_RESOURCE;
	}
	memset(tmpPtr1, 0, (pktNumber * DLDP_TX_PKT_LEN));

	nam_syslog_dbg("nam_packet_dldp_tx_task: malloc mem for unit 1\n");

	for (i = 0; i < pktNumber; i++) {
		buffArr1[i] = tmpPtr1;
		tmpPtr1 = tmpPtr1 + DLDP_TX_PKT_LEN;
	}
	#endif

	while(1) {
		dldpSockCreated = GT_TRUE;
		dldp_Clie_Addr_Len = sizeof(struct sockaddr);
	   	FD_ZERO(&fds); /*clear the fd_set*/
	   	FD_SET(adpt_dldp_RxFd, &fds);
	   
	   	select_ret = select(adpt_dldp_RxFd + 1, &fds, NULL, NULL, NULL);
	   	if (-1 == select_ret)
	   	{
	   		bufLen += sprintf(bufptr, "select error select_ret %d\n", select_ret);
			bufptr = buf + bufLen;
			nam_syslog_dbg("%s", buf);

			/* for BCM unit 0, for MV device 0*/
			ret = nam_dma_cache_free(UNIT0, buffArr[0]);
			#ifdef DRV_LIB_BCM
			ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
			#endif
	   		return GT_FAIL;
	   	}else if(0 == select_ret) {
	   		bufLen += sprintf(bufptr, "nam_dldp_tx: select return 0 \n");
			bufptr = buf + bufLen;
	   	}else {
	   		if (FD_ISSET(adpt_dldp_RxFd, &fds))
	   		{
	   			dldpPktBuff = (DLDP_PARAM_STC *)malloc(DLDP_TX_PKT_LEN);
	   			if (NULL == dldpPktBuff)
	   			{
	   				bufLen += sprintf(bufptr, "malloc NULL \n");
					bufptr = buf + bufLen;
					nam_syslog_dbg("%s",buf);

					/* for BCM unit 0, for MV device 0*/
					ret = nam_dma_cache_free(UNIT0, buffArr[0]);
					#ifdef DRV_LIB_BCM
					ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
					#endif
	   				return GT_NO_RESOURCE;
	   			}
	   			memset(dldpPktBuff, 0, DLDP_TX_PKT_LEN);

				if ((bytesNum = recvfrom(adpt_dldp_RxFd,
										(char*)dldpPktBuff,
										DLDP_TX_PKT_LEN,
										0,
										(struct sockaddr*)&dldp_Clie_Addr,
          	       						&dldp_Clie_Addr_Len))== -1)
	   			{
	   				bufLen += sprintf(bufptr, "DLDP recvfrom error -1\n");
					bufptr = buf + bufLen;
					nam_syslog_dbg("%s", buf);
					memset(buf, 0, NAM_SYSLOG_LINE_BUFFER_SIZE);
					bufptr = buf;
					bufLen = 0;
					/* delay for a while */
					sleep(1);
					free(dldpPktBuff);
	   				continue;
	   			}
				if (0 == bytesNum) {
					nam_syslog_dbg("(0 == bytesNum)\n");
					free(dldpPktBuff);
					break;
				}

	   			if (bytesNum > 0)
	   			{
	   				/*parse the data recv from DLDP*/
					byteRecv = dldpPktBuff->nlh.nlmsg_len;
					byteSend = byteRecv - sizeof(struct nlmsghdr) - 12;
					interfaceId	= (unsigned int)dldpPktBuff->ifindex;
					vlanId		= (unsigned short)dldpPktBuff->vlanid;
					isTagged	= (unsigned int)dldpPktBuff->tagmode;

					bufLen += sprintf(bufptr,"get DLDP from port_index %d vlan %d packet length %d\n",
									interfaceId, vlanId, byteRecv);
					bufptr = buf + bufLen;

					#ifdef DRV_LIB_CPSS
					cpssDxChNetIfTxPacketQueueSet(NAM_PACKET_TYPE_DLDP_E, &queueIdx);
					memset(&dldpTxBuffParam, 0, sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
	   				cpssDxChVirtNetIfTxBuildParam(interfaceId, vlanId, isTagged, queueIdx,
												CPSS_INTERFACE_PORT_E, byteSend, &dldpTxBuffParam);
	   				memcpy(buffArr[0], dldpPktBuff->buf, byteSend);

					/* packet padding to 64B */
					if (byteSend < 64) {
						byteSend = 64;
					}
					buffLenArr[0] = byteSend;
	   				rc = cpssDxChNetIfSdmaSyncTxPacketSend(0,&dldpTxBuffParam,buffArr, buffLenArr,1);
	   				if (rc != GT_OK)
	   				{
	   					bufLen += sprintf(bufptr,",send dldp error %d \n", rc);
						bufptr = buf + bufLen;
	   				}
					#endif
					
					#ifdef DRV_LIB_BCM
					memset(&u0tx_pbmp, 0, sizeof(bcm_pbmp_t));
					memset(&u1tx_pbmp, 0, sizeof(bcm_pbmp_t));

					ret = npd_get_devport_by_global_index(interfaceId, &devNum, &portNum);
					if (ret != GT_OK) {
						nam_syslog_err("global eth-port %#x get asic port(%d,%d) error, result %d!\n",
										interfaceId, devNum, portNum, ret);
						continue;
					}
					nam_syslog_dbg("nam_packet_dldp_tx_task: global eth-port %#x get asic port(%d,%d) \n",
										interfaceId, devNum, portNum);
					switch (devNum) {
						case 0:
							memcpy(buffArr[0], dldpPktBuff->buf, byteSend);
							/* packet padding to 64B */
							if(byteSend < 64) {
								byteSend = 64;
							}
							buffLenArr[0] = byteSend;
							BCM_PBMP_PORT_ADD(u0tx_pbmp, portNum);
							break;
						case 1:
							memcpy(buffArr1[0], dldpPktBuff->buf, byteSend);
							/* packet padding to 64B */
							if(byteSend < 64) {
								byteSend = 64;
							}
							buffLenArr[0] = byteSend;
							BCM_PBMP_PORT_ADD(u1tx_pbmp, portNum);
							break;
						default:
							nam_syslog_err("nam tx dldp packet with wrong eth-port %#x asic-port(%d,%d)\n",
									interfaceId, devNum, portNum);
							continue;
					}
					
					pktData = (unsigned char*)malloc(sizeof(bcm_pkt_t));
					if(NULL == pktData) {
						nam_syslog_err("alloc memory error when send dldp from port(%d,%d) vlan %d\n", 
								devNum, portNum, vlanId);
						ret = nam_dma_cache_free(UNIT0, buffArr[0]);
						ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
						return GT_FAIL;
					}
					memset(pktData, 0, sizeof(bcm_pkt_t));

					/* fillup packet buffer*/
					pktData->dma_channel = 0;
					pktData->pkt_data = &(pktData->_pkt_data);
					pktData->blk_count = 1;
					pktData->pkt_data[0].data = buffArr[0];
					pktData->pkt_data[0].len = buffLenArr[0];
					pktData->pkt_len = buffLenArr[0];
					
					/*setup packet vlan tag field*/
					dot1q.tpid = 0x8100;
					dot1q.cfi = 0;
					dot1q.pri = 0;
					dot1q.vid = (vlanId & 0xFFF);
					memcpy(pktData->_vtag, &dot1q, 4);

					/* packet with 4B CRC field*/
					pktData->flags |= BCM_TX_CRC_APPEND;
					pktData->flags |= BCM_PKT_F_NO_VTAG;
					if (!isTagged) {
						pktData->flags |= BCM_PKT_F_TX_UNTAG;
					}else {
						pktData->flags &= (~BCM_PKT_F_TX_UNTAG);
					}
						
					if (BCM_PBMP_NOT_NULL(u1tx_pbmp)) {
						/* tx paceket to unit 1 ports*/
						/* rebuilt packet data*/
						pktData->pkt_data = &(pktData->_pkt_data);
						pktData->pkt_data[0].data = buffArr1[0];
						pktData->pkt_data[0].len = buffLenArr[0];
						pktData->src_mod = UNIT1;
						pktData->dest_mod = UNIT1;
						pktData->dest_port = portNum;
					
						/* untag*/
						if (isTagged) {
							memcpy(&pktData->tx_pbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));						
						} else {
							memcpy(&pktData->tx_pbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));
							memcpy(&pktData->tx_upbmp, &u1tx_pbmp, sizeof(u1tx_pbmp));
						}

						nam_syslog_dbg("nam_packet_dldp_tx_task:bcm_tx unit 1\n");
						/* deliver it*/
						ret = bcm_tx(UNIT1, pktData, NULL);
						if (ret) {
							nam_syslog_err("bcm tx dldp packet in unit 1 vlanid %d error %d\n", vlanId, ret);
							ret = nam_dma_cache_free(UNIT0, buffArr[0]);
							ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
							free(pktData);
							pktData = NULL;
							return GT_FAIL;
						}
					}
					else if(BCM_PBMP_NOT_NULL(u0tx_pbmp)) {		/* tx paceket to unit 0 ports*/
						pktData->src_mod = UNIT0;
						pktData->src_port = 0;
						pktData->dest_mod = UNIT0;
						pktData->dest_port= portNum;
					
						/* setup tx port bitmap*/
						if (isTagged) {
							memcpy(&pktData->tx_pbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));						
						} else {
							memcpy(&pktData->tx_pbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));
							memcpy(&pktData->tx_upbmp, &u0tx_pbmp, sizeof(u0tx_pbmp));
						}

						nam_syslog_dbg("nam_packet_dldp_tx_task:bcm_tx unit 0\n");
						ret = bcm_tx(UNIT0, pktData, NULL);
						if (ret) {
							nam_syslog_err("bcm tx dldp packet in unit 0 vlanid %d error %d\n", vlanId, ret);
							ret = nam_dma_cache_free(UNIT0, buffArr[0]);
							ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
							free(pktData);
							pktData = NULL;
							return GT_FAIL;
						}
					}

					nam_dump_txrx_packet_brief(pktData, 0);
					free(pktData);
					pktData = NULL;
					#endif
	   			}
	   			free(dldpPktBuff);
	   		}
   		}
		nam_syslog_dbg("%s", buf);
		memset(buf, 0, NAM_SYSLOG_LINE_BUFFER_SIZE);
		bufptr = buf;
		bufLen = 0;
	}

	/* for BCM unit 0, for MV device 0*/
	ret = nam_dma_cache_free(UNIT0, buffArr[0]);
	#ifdef DRV_LIB_BCM
	ret = nam_dma_cache_free(UNIT1, buffArr1[0]);
	#endif	

	return rc;
#endif
}

void nam_tx_dhcp_broadcast_pkt
(
	void *ports,
	unsigned int isTagged,
	unsigned int pktType,
	unsigned int vid,
	unsigned char* data,
	unsigned int   dataLen	
)
{
	struct vlan_ports_memebers *portsPtr;
	portsPtr = (struct vlan_ports_memebers*)ports;

	nam_tx_broadcast_pkt(portsPtr, isTagged, pktType, vid, data, dataLen);
	return ;
}

void nam_tx_dhcp_unicast_pkt
(
	unsigned int interfaceId,
	unsigned int vid,
	unsigned int pktType,
	unsigned int isTagged,
	unsigned char* data,
	unsigned int dataLen
)
{
	nam_tx_unicast_pkt(interfaceId, vid, pktType, isTagged, data, dataLen);
	return ;
}

unsigned long nam_packet_tx_adapter_init()
{
	unsigned long 		rc = GT_OK;
	
    nam_thread_create(
					kapTxTaskName,                 /* Task Name             	*/
                	nam_packet_kap_tx_task,		/* Starting Point 				*/
                	NULL,          							/* there is no arguments 	*/
                	NAM_TX_TRUE,
                	NAM_TX_FALSE);
	
	nam_syslog_event("kap tx task %s created\n",kapTxTaskName);

    nam_thread_create(
					rstpTxTaskName,               /* Task Name                */
                    nam_packet_rstp_tx_task,     /* Starting Point           */
                    NULL,                  		 /* there is no arguments    */
					NAM_TX_TRUE,
					NAM_TX_FALSE);
	nam_syslog_event("rstp tx task %s created\n",rstpTxTaskName);

    nam_thread_create(
					igmpsnpTxTaskName,          	/* Task Name                */
                    nam_packet_igmpsnp_tx_task, /* Starting Point           */
                    NULL,                  		/* there is no arguments    */
					NAM_TX_TRUE,
					NAM_TX_FALSE);
	nam_syslog_event("igmp snooping tx task %s created\n",igmpsnpTxTaskName);

	nam_thread_create(
					dldpTxTaskName,          	/* Task Name                */
                    nam_packet_dldp_tx_task, 	/* Starting Point           */
                    NULL,                  		/* there is no arguments    */
					NAM_TX_TRUE,
					NAM_TX_FALSE);
	nam_syslog_event("DLDP tx task %s created\n", dldpTxTaskName);
#if 0
	nam_thread_create(
					wifiTxTaskName,                 /* Task Name             	*/
                	nam_packet_wifi_tx_task,		/* Starting Point 				*/
                	NULL,          							/* there is no arguments 	*/
                	NAM_TX_TRUE,
                	NAM_TX_FALSE);
	
	nam_syslog_event("wifi tx task %s created\n",wifiTxTaskName);
#endif
	
	return rc;
}

/*******************************************************************************
* nam_arp_solicit_drv_send
*
* DESCRIPTION:
*       Send arp solicit packet from cpu via driver layer
*
* INPUTS:
*       devNum - device number
*	  portNum - port number on Dx device
*	  vid - vlan id of specified vlan to send arp
*	  isTagged - port is vlan tagged or untagged
*	  smac - source mac address
*	  dmac - destination mac address
*	  sip - source ip address
*	  dip - destination ip address
*
* OUTPUTS:
*       None
*
* RETURNS:
*       	ARP_RETURN_CODE_SUCCESS   - on success
*		COMMON_RETURN_CODE_NO_RESOURCE - malloc failed
*		ARP_RETURN_CODE_NAM_ERROR - asic operation failed
*
* COMMENTS:
*       None
*
*******************************************************************************/
unsigned long nam_arp_solicit_drv_send
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short vid,
	unsigned char isTagged,
	unsigned char *smac,
	unsigned char *dmac,
	unsigned int sip,
	unsigned int dip
)
{
	unsigned long result = ARP_RETURN_CODE_SUCCESS;
	unsigned char	*dmaCache = NULL;
	#ifdef DRV_LIB_CPSS
	unsigned char queueIdx = 0;
	unsigned int  interfaceId = 0;
	CPSS_DXCH_NET_TX_PARAMS_STC txBuffParam;
	unsigned char	*buffArr[BUFF_LEN] = {NULL};
	unsigned long	buffLenArr[BUFF_LEN] = {0};
	#endif
	#ifdef DRV_LIB_BCM
	bcm_pkt_t *pktData = NULL;
	ether_8021q_t dot1q;
	#endif
	ether_header_t *layer2 = NULL;
	arp_header_t *layer3 = NULL;
	int i = 0;

	nam_syslog_dbg("devNum [%d] port %d\n", devNum, portNum);

	dmaCache = (unsigned char*)nam_dma_cache_malloc(devNum, NAM_ARP_SOLICIT_PKTSIZE);
	if(NULL == dmaCache)
	{
		nam_syslog_dbg("malloc dma err when send solicit arp\n");
		return COMMON_RETURN_CODE_NO_RESOURCE;
	}
	memset(dmaCache,0,NAM_ARP_SOLICIT_PKTSIZE);
	nam_syslog_dbg("dmaCache %p\n", dmaCache);

	/*
	 * Build up ARP solicit packet
	 */
	/* layer 2 */
	layer2 = (ether_header_t *)dmaCache;
	memcpy(layer2->dmac,dmac,ETH_ALEN);
	memcpy(layer2->smac,smac,ETH_ALEN);
	layer2->etherType = htons(0x0806);

	/* layer 3 */
	layer3 = (arp_header_t *)(layer2 + 1);
	layer3->hwType 		= htons(0x1); /* ethernet hardware */
	layer3->protType	= htons(0x0800); /* IP */
	layer3->hwSize 		= 0x6;
	layer3->protSize 	= 0x4;
	layer3->opCode 		= htons(0x1); /* request */
	memcpy(layer3->smac,smac,ETH_ALEN);
	memset(layer3->dmac,0,ETH_ALEN);
	for(i = 0; i < 4; i++) {
		layer3->sip[i] = (sip >>((3-i)*8)) & 0xFF;
		layer3->dip[i] = (dip >>((3-i)*8)) & 0xFF;
	}
	#ifdef DRV_LIB_CPSS
	cpssDxChNetIfTxPacketQueueSet(NAM_PACKET_TYPE_ARP_E, &queueIdx);

	/* 
	 * Attention!!!
	 * Here interface type must be CPSS_INTERFACE_VID_E, convertion to CPSS_INTERFACE_PORT_E 
	 * will be done later when build parameter.
	 */
	memset(&txBuffParam,0,sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
	interfaceId = (portNum << 8) | devNum;
	cpssDxChVirtNetIfTxBuildParam(interfaceId,vid,isTagged,queueIdx,	\
					NAM_INTERFACE_VID_E,NAM_ARP_SOLICIT_PKTSIZE,&txBuffParam);

	buffArr[0] = dmaCache;
	buffLenArr[0] = NAM_ARP_SOLICIT_PKTSIZE;

	result = cpssDxChNetIfSdmaSyncTxPacketSend(devNum,&txBuffParam,buffArr,buffLenArr,1);	
	if(GT_OK != result) {
		nam_syslog_dbg("send solicit arp to %d.%d.%d.%d error %d\n",	\
				(dip>>24)&0xFF,(dip>>16)&0xFF,(dip>>8)&0xFF, dip&0xFF,result);
		result = ARP_RETURN_CODE_NAM_ERROR;
	}
	else{
		result = ARP_RETURN_CODE_SUCCESS;
	}
	nam_syslog_dbg("send solicit arp to %d.%d.%d.%d in driver, result %d\n",	\
				(dip>>24)&0xFF,(dip>>16)&0xFF,(dip>>8)&0xFF, dip&0xFF,result);
	nam_dma_cache_free(devNum, dmaCache);
	#endif
	#ifdef DRV_LIB_BCM
	pktData = (unsigned char*)malloc(sizeof(bcm_pkt_t));
	if(NULL == pktData) {
		nam_syslog_err("alloc memory error when send solicit arp from port(%d,%d) vlan %d\n", 
				devNum, portNum, vid);
		nam_dma_cache_free(devNum, dmaCache);
		return COMMON_RETURN_CODE_NO_RESOURCE;
	}
	memset(pktData, 0,sizeof(bcm_pkt_t));

	/* fillup packet buffer*/
	pktData->dma_channel = 0;
	pktData->pkt_data = &(pktData->_pkt_data);
	pktData->blk_count = 1;
	pktData->pkt_data[0].data = dmaCache;
	pktData->pkt_data[0].len = NAM_ARP_SOLICIT_PKTSIZE;
	pktData->pkt_len = NAM_ARP_SOLICIT_PKTSIZE;

	pktData->src_mod = devNum;
	pktData->src_port = 0;
	pktData->dest_mod = devNum;
	pktData->dest_port = portNum;

	BCM_PBMP_PORT_ADD(pktData->tx_pbmp, portNum);
	if(!isTagged) {
		BCM_PBMP_PORT_ADD(pktData->tx_upbmp, portNum);
	}

	pktData->flags |= BCM_TX_CRC_APPEND;
	pktData->flags |= BCM_PKT_F_NO_VTAG;
	if(!isTagged) {
		pktData->flags |= BCM_PKT_F_TX_UNTAG;
	}

	/* ALL packet send should have TPID and vlan tag fields */
	/*setup packet vlan tag field*/
	dot1q.tpid = htons(0x8100);
	dot1q.cfi = 0;
	dot1q.pri = 0;
	dot1q.vid = (vid & 0xFFF);
	memcpy(pktData->_vtag, &dot1q, 4);

	result = bcm_tx(devNum, pktData, NULL);
	if(result) {
		nam_syslog_err("bcm tx arp solicit packet error %d\n", result);
		nam_dma_cache_free(devNum, dmaCache);
		free(pktData);
		return ARP_RETURN_CODE_NAM_ERROR;
	}
	else{
		result = ARP_RETURN_CODE_SUCCESS;
	}
	nam_dump_txrx_packet_brief(pktData, 0);
	nam_dma_cache_free(devNum, dmaCache);
	free(pktData);
	#endif
	return result;
}


/*******************************************************************************
* nam_arp_reply_drv_send
*
* DESCRIPTION:
*       Send arp reply packet from cpu via driver layer
*
* INPUTS:
*       devNum - device number
*	  portNum - port number on Dx device
*	  vid - vlan id of specified vlan to send arp
*	  isTagged - port is vlan tagged or untagged
*	  smac - source mac address
*	  dmac - destination mac address
*	  sip - source ip address
*	  dip - destination ip address
*
* OUTPUTS:
*       None
*
* RETURNS:
*       ARP_RETURN_CODE_SUCCESS   - on success
*	  COMMON_RETURN_CODE_NO_RESOURCE  - if malloc failed
*       ARP_RETURN_CODE_NAM_ERROR - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
unsigned long nam_arp_reply_drv_send
(
	unsigned char devNum,
	unsigned char portNum,
	unsigned short vid,
	unsigned char isTagged,
	unsigned char *smac,
	unsigned char *dmac,
	unsigned int sip,
	unsigned int dip
)
{
	unsigned long result = ARP_RETURN_CODE_SUCCESS;
	unsigned char	*dmaCache = NULL;
	#ifdef DRV_LIB_CPSS
	unsigned char queueIdx = 0;
	unsigned int  interfaceId = 0;
	CPSS_DXCH_NET_TX_PARAMS_STC txBuffParam;
	unsigned char	*buffArr[BUFF_LEN] = {NULL};
	unsigned long	buffLenArr[BUFF_LEN] = {0};
	#endif
	#ifdef DRV_LIB_BCM
	bcm_pkt_t *pktData = NULL;
	ether_8021q_t dot1q;
	#endif
	ether_header_t *layer2 = NULL;
	arp_header_t *layer3 = NULL;
	int i = 0;

	nam_syslog_dbg("devNum [%d] port %d arp reply send\n", devNum, portNum);

	dmaCache = (unsigned char*)nam_dma_cache_malloc(devNum, NAM_ARP_SOLICIT_PKTSIZE);
	if(NULL == dmaCache)
	{
		nam_syslog_dbg("malloc dma err when send solicit arp\n");
		return COMMON_RETURN_CODE_NO_RESOURCE;
	}
	memset(dmaCache,0,NAM_ARP_SOLICIT_PKTSIZE);
	nam_syslog_dbg("dmaCache %p\n", dmaCache);

	/*
	 * Build up ARP solicit packet
	 */
	/* layer 2 */
	layer2 = (ether_header_t *)dmaCache;
	memcpy(layer2->dmac,dmac,ETH_ALEN);
	memcpy(layer2->smac,smac,ETH_ALEN);
	layer2->etherType = htons(0x0806);

	/* layer 3 */
	layer3 = (arp_header_t *)(layer2 + 1);
	layer3->hwType 		= htons(0x1); /* ethernet hardware */
	layer3->protType	= htons(0x0800); /* IP */
	layer3->hwSize 		= 0x6;
	layer3->protSize 	= 0x4;
	layer3->opCode 		= htons(0x2); /* reply */
	memcpy(layer3->smac,smac,ETH_ALEN);
	memcpy(layer3->dmac,dmac,ETH_ALEN);
	for(i = 0; i < 4; i++) {
		layer3->sip[i] = (sip >>((3-i)*8)) & 0xFF;
		layer3->dip[i] = (dip >>((3-i)*8)) & 0xFF;
	}
	#ifdef DRV_LIB_CPSS
	cpssDxChNetIfTxPacketQueueSet(NAM_PACKET_TYPE_ARP_E, &queueIdx);

	/* 
	 * Attention!!!
	 * Here interface type must be CPSS_INTERFACE_VID_E, convertion to CPSS_INTERFACE_PORT_E 
	 * will be done later when build parameter.
	 */
	memset(&txBuffParam,0,sizeof(CPSS_DXCH_NET_TX_PARAMS_STC));
	interfaceId = (portNum << 8) | devNum;
	cpssDxChVirtNetIfTxBuildParam(interfaceId,vid,isTagged,queueIdx,	\
					NAM_INTERFACE_VID_E,NAM_ARP_SOLICIT_PKTSIZE,&txBuffParam);

	buffArr[0] = dmaCache;
	buffLenArr[0] = NAM_ARP_SOLICIT_PKTSIZE;

	result = cpssDxChNetIfSdmaSyncTxPacketSend(devNum,&txBuffParam,buffArr,buffLenArr,1);	
	if(NAM_OK != result) {
		nam_syslog_dbg("send solicit arp to %d.%d.%d.%d error %d\n",	\
				(dip>>24)&0xFF,(dip>>16)&0xFF,(dip>>8)&0xFF, dip&0xFF,result);
		result = ARP_RETURN_CODE_NAM_ERROR;
	}
	nam_dma_cache_free(devNum, dmaCache);
	#endif
	#ifdef DRV_LIB_BCM
	pktData = (unsigned char*)malloc(sizeof(bcm_pkt_t));
	if(NULL == pktData) {
		nam_syslog_err("alloc memory error when send solicit arp from port(%d,%d) vlan %d\n", 
				devNum, portNum, vid);
		nam_dma_cache_free(devNum, dmaCache);
		return COMMON_RETURN_CODE_NO_RESOURCE;
	}
	memset(pktData, 0,sizeof(bcm_pkt_t));

	/* fillup packet buffer*/
	pktData->dma_channel = 0;
	pktData->pkt_data = &(pktData->_pkt_data);
	pktData->blk_count = 1;
	pktData->pkt_data[0].data = dmaCache;
	pktData->pkt_data[0].len = NAM_ARP_SOLICIT_PKTSIZE;
	pktData->pkt_len = NAM_ARP_SOLICIT_PKTSIZE;

	pktData->src_mod = devNum;
	pktData->src_port = 0;
	pktData->dest_mod = devNum;
	pktData->dest_port = portNum;

	BCM_PBMP_PORT_ADD(pktData->tx_pbmp, portNum);
	if(!isTagged) {
		BCM_PBMP_PORT_ADD(pktData->tx_upbmp, portNum);
	}

	pktData->flags |= BCM_TX_CRC_APPEND;
	pktData->flags |= BCM_PKT_F_NO_VTAG;
	if(!isTagged) {
		pktData->flags |= BCM_PKT_F_TX_UNTAG;
	}

	/* ALL packet send should have TPID and vlan tag fields */
	/*setup packet vlan tag field*/
	dot1q.tpid = htons(0x8100);
	dot1q.cfi = 0;
	dot1q.pri = 0;
	dot1q.vid = (vid & 0xFFF);
	memcpy(pktData->_vtag, &dot1q, 4);

	result = bcm_tx(devNum, pktData, NULL);
	if(NAM_OK != result) {
		nam_syslog_err("bcm tx arp solicit packet error %d\n", result);
		nam_dma_cache_free(devNum, dmaCache);
		free(pktData);
		return ARP_RETURN_CODE_NAM_ERROR;
	}
	nam_dump_txrx_packet_brief(pktData, 0);
	nam_dma_cache_free(devNum, dmaCache);
	free(pktData);
	#endif
	if(NAM_OK == result){
		result = ARP_RETURN_CODE_SUCCESS;
	}
	return result;
}

#ifdef __cplusplus
}
#endif

