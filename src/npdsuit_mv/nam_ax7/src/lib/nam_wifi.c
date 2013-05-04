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
*nam_wifi.c
*
*MODIFY:
*	by hanhui@autelan.com   on 06/22/2009 revision <0.0.1..>
*       this file includs the functions will perform in send and receive packets for wifi .
*CREATOR:
*	hanhui@autelan.com
*
*DESCRIPTION:
*<some description about this file>
*      In this file The functions prepare for packet Rx from Adapter to 
*      The Virtual Network Interface  .
*
*DATE:
*	06/22/2009
*	
*******************************************************************************/
#ifdef __cplusplus
extern "C" 
{
#endif

/*include header files begin */
/*kernel part */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/un.h>
#include <sys/select.h>

/**igmp snoop**/
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/igmp.h>
#include <unistd.h>
#include <signal.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <pthread.h>
	

#ifdef DRV_LIB_CPSS
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <gtOs/gtOsSem.h>
#endif
#ifdef DRV_LIB_BCM
#include <bcm/rx.h>
#endif
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "nam_log.h"
#include "nam_rx.h"
#include "nam_wifi.h"

/*include header files end */


long nam_wifi_net_if_rx_packet
( 
    IN unsigned long      	numOfBuff,
    IN unsigned char       	*packetBuffs[],
    IN unsigned long      	buffLen[]
)
{
	int        	iterator=0;	 
	unsigned long    	buffByte, totalBytes = 0;
  
	/*struct wifi_struct   rxPktBuf[ADAPTER_RX_PACKET_BUFF_NUM_MAX];*/
	char logbuffer[1024] = {0},*bufPtr = NULL;
	int loglength = 0;
    int adptWifiRxFd = -1;
	bufPtr = logbuffer;
	struct wifi_struct wifistruct;
	int                data_len;
	/*unsigned char      data_buf[ADAPTER_RX_PACKET_LEN_MAX];*/
	wifistruct.len_addr  = (int)&data_len;
    
	if((adptWifiRxFd = open("/dev/wifi0",O_RDWR))<0)
	{
		nam_syslog_err("open wifi %s file descriptor error\n","/dev/wifi0");
		return GT_FAIL;
	}
	nam_syslog_dbg("open wifi %s file descriptor %d ok\n","/dev/wifi0",adptWifiRxFd);

	#if 0
	for(;iterator<numOfBuff;iterator++)
	{
		rxPktBuf[iterator].data_addr = packetBuffs[iterator];
		rxPktBuf[iterator].data_len = buffLen[iterator];
	}
	#endif

	for(iterator=0;iterator<numOfBuff;iterator++)
	{
		data_len = buffLen[iterator];
		wifistruct.data_addr = (int)packetBuffs[iterator];
		buffByte = write(adptWifiRxFd,&wifistruct,sizeof(wifistruct));
		if(buffByte != data_len)
		{
			loglength += sprintf(bufPtr,"pass packet(%dB) to wifi driver error(%dB)\n",		\
									data_len,buffByte);
			bufPtr = logbuffer + loglength;
			/* here should close the FD, zhangdi 2011-11-22 */
            close(adptWifiRxFd);			
			return buffByte;
		}
		loglength += sprintf(bufPtr,"pass packet(%dB) to wifi driver\n",buffByte);
		bufPtr = logbuffer + loglength;
		totalBytes += buffByte;
	}
    close(adptWifiRxFd);
	nam_syslog_dbg("%s",logbuffer);
	return totalBytes;
}

#if 1
/*unsigned long appDemoPacketTxVirtNetIfTask(){}*/
unsigned long nam_packet_wifi_tx_task
(
	void
)
{
	static char		buf[1024 * 5]		=   {0}		;
	char* 			bufptr				=   NULL	;
	int  			bufLen				=   0		;
	int  			data_len     		=   0		;
    int  			adptWifiTxFd		=   -1		;
	long 			bytesNum			= 	0		;
	unsigned char 	devNum				= 	0		;
	unsigned char	portOrTrunkNum		= 	0		;
	unsigned char 	isTag 				= 	GT_FALSE;
	unsigned char*	tmpPtr				=	NULL	;
	unsigned char*	txBufBlk 			= 	NULL	;
	unsigned char* 	macByte 			= 	NULL	;
	unsigned char*	buffArr[BUFF_LEN]	= 	{NULL}	;
	unsigned int 	ipaddr		 		= 	0		;
	unsigned int 	tblIndex 			= 	0		;
	unsigned int 	interfaceId 		= 	0		;
	unsigned int	l3Intf 				= 	0		;
	unsigned int 	isTrunk 			= 	GT_FALSE;
	unsigned long	i 					= 	0		;	/* iterator */
	unsigned long  	pktNumber 			= 	0		;
	unsigned long 	isSend 				= 	GT_FALSE;
	unsigned long	isMc 				=	GT_FALSE;
	unsigned long	isUc 				= 	GT_FALSE;
	unsigned long	isFund 				= 	GT_FALSE;
	unsigned long	buffLenArr[BUFF_LEN]= 	{0}		;
	unsigned long	tmpIpAddr			=	0;
	struct 	wifi_struct  			wifiPacket		;
	struct 	vlan_ports_memebers 	untagPorts		;
	struct 	vlan_ports_memebers  	tagPorts		;
	struct	arp_snooping_item_s 	tmpItem			;
	struct 	ether_header_t* 		layer2 			= 	NULL;
	struct 	ip_header_t* 			ipHead 			= 	NULL;
	struct 	arp_packet_t* 			arpPtr 			= 	NULL;
	struct 	route_nexthop_brief_s* 	routeData		= 	NULL;
	struct	arp_snooping_item_s* 	arpItem 		= 	NULL;
	struct	arp_snooping_item_s*	tmpArpItem		=	&tmpItem;
	enum 	NAM_INTERFACE_TYPE_ENT 	interfaceType 	= 	NAM_INTERFACE_PORT_E	;
	enum 	NAM_PACKET_TYPE_ENT 	packetType 		= 	NAM_PACKET_TYPE_OTHER	;
	
	bufptr = buf;

	/* tell my thread id */
	npd_init_tell_whoami("WifiPktTx",0);


	memset(&tmpItem,0,sizeof(struct arp_snooping_item_s));
	/*modify by zhubo@autelan.com 7.2.2008*/
	
	devNum = 0;
	txBufBlk = (unsigned char*)nam_dma_cache_malloc(devNum,(BUFF_LEN * NAM_SDMA_TX_PER_BUFFER));
	if(NULL == txBufBlk)
	{
		nam_syslog_err("post-processor malloc return NULL\n");
		return GT_NO_RESOURCE;
	}
	
	osMemSet(txBufBlk,0,(BUFF_LEN * NAM_SDMA_TX_PER_BUFFER));
	osMemSet(&wifiPacket,0,sizeof(struct wifi_struct)); /* code optimize: Out-of-bounds access. zhangdi@autelan.com 2013-01-18 */

    osMemSet(&untagPorts,0,sizeof(struct vlan_ports_memebers));
	osMemSet(&tagPorts,0,sizeof(struct vlan_ports_memebers));
	tmpPtr = txBufBlk;
	for(i=0;i<BUFF_LEN;i++)
	{
		buffArr[i] = tmpPtr;
		tmpPtr = tmpPtr + NAM_SDMA_TX_PER_BUFFER;
	}
	
	if((adptWifiTxFd = open("/dev/wifi0",O_RDWR))<0)
	{
		nam_syslog_err("open wifi %s file descriptor error\n","/dev/wifi0");
		return GT_FAIL;
	}

	while(1)
	{
		/*nam_syslog_dbg("open wifi %s file descriptor %d ok\n","/dev/wifi0",adptWifiTxFd);*/

		bytesNum = 0;
		wifiPacket.data_addr = (unsigned int)buffArr[0];
		wifiPacket.len_addr = (unsigned int)&data_len;
		devNum = 0;
		portOrTrunkNum = 0;
	
		bytesNum = read(adptWifiTxFd,&wifiPacket,sizeof(wifiPacket));
		if(bytesNum <= 0)
		{
			osMemSet(txBufBlk,0,(BUFF_LEN * NAM_SDMA_TX_PER_BUFFER));
			sleep(10000); /*free cpu for long long time,this line will be change after the interface is ok*/
			continue;
		}
		else if(bytesNum >0)
		{   
			buffArr[0] 		= (unsigned char *)wifiPacket.data_addr;
			buffLenArr[0] = *(unsigned int *)(wifiPacket.len_addr);
			packetType = nam_packet_parse_type(buffArr[0]);
			if((NAM_PACKET_TYPE_IP_TCP_TELNET_E != packetType) &&
				(NAM_PACKET_TYPE_IP_TCP_SSH_E != packetType)){
				nam_packet_tx_print_packet_buffer(buffArr[0],buffLenArr[0]);
			}			

			layer2 = (struct ether_header_t*) wifiPacket.data_addr;
			if(htons(0x0806) == layer2->etherType){
				arpPtr = (struct arp_packet_t*)(layer2 + 1);
				ipaddr = ((arpPtr->dip[0] << 24) | (arpPtr->dip[1] << 16) | (arpPtr->dip[2] << 8) |(arpPtr->dip[3]));
			}
			else {
				ipHead = (struct ip_header_t*)((unsigned char*)wifiPacket.data_addr + sizeof(struct ether_header_t));
				ipaddr = ((ipHead->dip[0] <<24) | (ipHead->dip[1] << 16) | (ipHead->dip[2] << 8) |(ipHead->dip[3]));
			}

			if(GT_OK == npd_route_get_route_tblIndex_by_dip(ipaddr,&tblIndex)){
				pthread_mutex_lock(&nexthopHashMutex);
				routeData = npd_route_nexthop_get_by_tblindex(tblIndex);
				if(NULL != routeData){
					tmpIpAddr = routeData->ipAddr;
					pthread_mutex_unlock(&nexthopHashMutex);
					pthread_mutex_lock(&arpHashMutex);
                    arpItem = npd_arp_snooping_find_item_byip_only(tmpIpAddr);
					memcpy(tmpArpItem,arpItem,sizeof(struct arp_snooping_item_s));
					pthread_mutex_unlock(&arpHashMutex);
					if(NULL != tmpArpItem){
						isFund = GT_TRUE;
						isTrunk = tmpArpItem->dstIntf.isTrunk;						
						if(isTrunk){
							devNum = tmpArpItem->dstIntf.intf.trunk.devNum;
							portOrTrunkNum = tmpArpItem->dstIntf.intf.trunk.trunkId;
						}
						else {
							devNum = tmpArpItem->dstIntf.intf.port.devNum;
							portOrTrunkNum = tmpArpItem->dstIntf.intf.port.portNum;
							
							npd_get_global_index_by_devport(tmpArpItem->dstIntf.intf.port.devNum,\
								tmpArpItem->dstIntf.intf.port.portNum,&interfaceId);
						}
                        if(NAM_PORT_L3INTF_VLAN_ID == tmpArpItem->vid){
                            interfaceType = NAM_INTERFACE_PORT_E;
						}
						else{
                            interfaceType = NAM_INTERFACE_VID_E;
						}
						switch(interfaceType)
						{
							case NAM_INTERFACE_PORT_E:
								bufLen += sprintf(bufptr,"outif PORT %#x,",interfaceId);
								bufptr = buf + bufLen;
								npd_get_global_index_by_devport(tmpArpItem->dstIntf.intf.port.devNum,\
									tmpArpItem->dstIntf.intf.port.portNum,&interfaceId);
								isTag = GT_FALSE; /*port in 4095 must be untag*/
								if(GT_OK != nam_check_port_statuts(interfaceId, tmpArpItem->vid)) {
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
								
								isMc = nam_packet_type_is_Mc((unsigned char*)wifiPacket.data_addr);
								if(!isMc) {	
									/*unicast packets*/
									isTrunk = tmpArpItem->dstIntf.isTrunk;
									
									if(isTrunk){
										devNum = tmpArpItem->dstIntf.intf.trunk.devNum;
										portOrTrunkNum = tmpArpItem->dstIntf.intf.trunk.trunkId;
									}
									else {
										devNum = tmpArpItem->dstIntf.intf.port.devNum;
										portOrTrunkNum = tmpArpItem->dstIntf.intf.port.portNum;
									}
									if((GT_TRUE == isFund)&&(GT_FALSE == isTrunk)) {
										interfaceId = (unsigned int)portOrTrunkNum;
										interfaceId =  (interfaceId << 8) |(unsigned int)devNum;
										if(0 == nam_get_port_tag(devNum,portOrTrunkNum,tmpArpItem->vid,&isTag)) {
											isUc = GT_TRUE;		/*fund dest port,send packets by unicast*/
											isSend  = GT_TRUE;

											bufLen += sprintf(bufptr,"unicast %s packet send to port(%d,%d) in vlan %d %s\n",	\
														nam_get_packet_type_str(packetType),devNum,portOrTrunkNum,tmpArpItem->vid,isTag ? "TAG" : "UNTAG");
											bufptr = buf + bufLen;
										}
										else {
											/*get tag type error ,drop packets by zhubo@autelan.com 2008.7.22*/
											isUc = GT_FALSE;
											isSend = GT_FALSE;
											bufLen += sprintf(bufptr,"unicast %s packet drop as to port(%d,%d) not in vlan %d\n",	\
													nam_get_packet_type_str(packetType),devNum,portOrTrunkNum,tmpArpItem->vid,isTag);
											bufptr = buf + bufLen;
										}
									}
									else if((GT_TRUE == isFund)&&(GT_TRUE == isTrunk)) {
										interfaceType = NAM_INTERFACE_TRUNK_E;
										interfaceId = (unsigned int)portOrTrunkNum;
										interfaceId =  (interfaceId << 8) |(unsigned int)devNum;
										isSend = GT_TRUE;
										isUc = GT_TRUE;
										bufLen += sprintf(bufptr,"unicast %s packet send to trunk(%d,%d) in vlan %d\n", \
													nam_get_packet_type_str(packetType),devNum,portOrTrunkNum,tmpArpItem->vid);
										bufptr = buf + bufLen;
									}
									else if(GT_FALSE == isFund) {
										/* flood packet if no outif found by qinhs@autelan.com 2009-4-2 */									
										osMemSet(&untagPorts,0,sizeof(struct vlan_ports_memebers));
										osMemSet(&tagPorts,0,sizeof(struct vlan_ports_memebers));
										if(npd_vlan_get_all_active_ports(tmpArpItem->vid,&untagPorts,&tagPorts)){
											bufLen += sprintf(bufptr,"unicast %s packet drop as to no port in vlan %d\n",
																nam_get_packet_type_str(packetType),tmpArpItem->vid);
											bufptr = buf + bufLen;
											
											isMc = GT_FALSE;
											isSend = GT_FALSE;
										}
										else {		
											isMc = GT_TRUE;
											isSend = GT_TRUE;
											bufLen += sprintf(bufptr,"unicast %s packet flood in vlan %d\n",
																nam_get_packet_type_str(packetType),tmpArpItem->vid);
											bufptr = buf + bufLen;
										}
									}

								}
								else  {
									/*broadcast packets*/
									osMemSet(&untagPorts,0,sizeof(struct vlan_ports_memebers));
									osMemSet(&tagPorts,0,sizeof(struct vlan_ports_memebers));
									if(npd_vlan_get_all_active_ports(tmpArpItem->vid,&untagPorts,&tagPorts)){
										bufLen += sprintf(bufptr,"multicast packet drop as to no port in vlan %d\n",tmpArpItem->vid);
										bufptr = buf + bufLen;
										
										isMc = GT_FALSE;
										isSend = GT_FALSE;
									}
									else {
										isSend = GT_TRUE;
										isMc = GT_TRUE;
										bufLen += sprintf(bufptr,"multicast packet flood in vlan %d\n",tmpArpItem->vid);
										bufptr = buf + bufLen;
									}
								}
								break;
							default:
								bufLen += sprintf(bufptr,"outif %d vlan %d l3if %d l2index %#x %s\n",
												interfaceType,tmpArpItem->vid,l3Intf,interfaceId, isSend ? "send":"drop");
								bufptr = buf + bufLen;
								break;
						}
					}
				}
				else{
					pthread_mutex_unlock(&nexthopHashMutex);
				}
			}
			

			nam_syslog_dbg("%s",buf);
			bufLen = 0;
			bufptr = buf;
			
			/*only send unicast packets*/
			if((GT_TRUE == isSend)&& (GT_TRUE == isUc)) {
				nam_tx_unicast_pkt(interfaceId,tmpArpItem->vid,interfaceType,isTag,wifiPacket.data_addr,*(unsigned int *)(wifiPacket.len_addr));
				/*go back init status*/
				isSend = GT_FALSE;isUc = GT_FALSE;isFund = GT_FALSE;isMc = GT_FALSE;
			}/*only send broadcast packets*/
			else if((GT_TRUE == isSend) && (GT_TRUE == isMc)){							
				bufLen += sprintf(bufptr,"send packets to all %d untagged ports and %d tag ports in vlan %d\n",	
								untagPorts.count,tagPorts.count, tmpArpItem->vid);
				bufptr = buf + bufLen;

				nam_tx_broadcast_pkt(&untagPorts,GT_FALSE,interfaceType,tmpArpItem->vid, wifiPacket.data_addr, *(unsigned int *)(wifiPacket.len_addr));
				nam_tx_broadcast_pkt(&tagPorts,GT_TRUE,interfaceType,tmpArpItem->vid, wifiPacket.data_addr, *(unsigned int *)(wifiPacket.len_addr));	
				isSend = GT_FALSE;isUc = GT_FALSE;isFund = GT_FALSE;isMc = GT_FALSE;
			}	
			else if(GT_FALSE == isSend) {
				/*drop packets by zhubo@autelan.com 2008.7.22*/
				isSend = GT_FALSE;isUc = GT_FALSE;isFund = GT_FALSE;isMc = GT_FALSE;
			}
			nam_syslog_dbg("%s",bufptr);
		}/* end else if(...) */
		bytesNum = 0;
		/*cpssOsCacheDmaFree(txBufBlk);*/
		osMemSet(txBufBlk,0,(BUFF_LEN* NAM_SDMA_TX_PER_BUFFER));
		nam_syslog_dbg("%s",buf);
		osMemSet(buf,0,(5*1024));
		bufptr = buf;
		bufLen = 0;
	} /* end while(..) */	
	close(adptWifiTxFd);
}		
#endif
#ifdef __cplusplus
}
#endif

