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
*nam_rx.c
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
	
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"


#ifdef DRV_LIB_CPSS
#include <cpss/dxCh/dxChxGen/networkIf/cpssDxChNetIfTypes.h>
#include <gtOs/gtOsSem.h>
#endif
#ifdef DRV_LIB_BCM
#include <bcm/rx.h>
#endif

#include "nam_log.h"
#include "nam_rx.h"

/*include header files end */

/* NPD startup state*/
extern int npd_startup_end;
extern unsigned int promis_port_add2_intf;

extern unsigned int npd_dynamic_trunk_slave_port_trunkId_get
(
	unsigned char * ethIfname
);


#ifdef IGMP_SNOOP_SOCKET_STREAM
#define	NPD_IGMP_SNOOP_PKT_SOCK_PATH		"/tmp/igmp_snp_skb.sock"
#endif

char * namPacketTypeStr[] = {
		"BPDU",	/* NAM_PACKET_TYPE_BPDU_E = 0 */
		"GVRP",	/* NAM_PACKET_TYPE_GVRP_E */
		"ARP",	/* NAM_PACKET_TYPE_ARP_E */	
		"IPv4",	/* NAM_PACKET_TYPE_IPv4_E */
		"ICMP",	/* NAM_PACKET_TYPE_IP_ICMP_E */		
		"TCP",	/* NAM_PACKET_TYPE_IP_TCP_E */		
		"UDP",	/* NAM_PACKET_TYPE_IP_UDP_E */		
		"IGMP",	/* NAM_PACKET_TYPE_IP_IGMP_E */	 	
		"TELNET", /* NAM_PACKET_TYPE_IP_TCP_TELNET_E */	
		"SSH",	/* NAM_PACKET_TYPE_IP_TCP_SSH_E */	
		"FTP",	/* NAM_PACKET_TYPE_IP_TCP_FTP_E */
		"FTP-DA", /* NAM_PACKET_TYPE_IP_TCP_FTP_DATA_E */
		"DHCP", /* NAM_PACKET_TYPE_IP_UDP_DHCP_E */
		"IPIP", /* NAM_PACKET_TYPE_IP_IPIP_E */
		"DLDP",	/* NAM_PACKET_TYPE_DLDP_E */	
		"VRRP", /* NAM_PACKET_TYPE_VRRP_E */
		"IPv6",	/* NAM_PACKET_TYPE_IPv6_E */
		"MLD",	/* NAM_PACKET_TYPE_IP6_MLD_E */
        "ISIS", /* NAM_PACKET_TYPE_ISIS_E */
		"LACP", /* NAM_PACKET_TYPE_LACP_E */
		"UNKOWN"	/* NAM_PACKET_TYPE_OTHER */		
};

/*local variables definition begin */
unsigned short adpterRxPktLenMax = ADAPTER_RX_PACKET_LEN_MAX; 
int    adptRstpRxFd = 0;
int    adptRstpRxClientFd = 0;
/*local variables definition end */

#ifdef IGMP_SNOOP_SOCKET_STREAM
int	adptIgmpSnpRxFd = 0;
int adptIgmpSnpRxCltFd = 0;
struct sockaddr_un 	igmpSnpServAddr;
struct sockaddr_un	igmpSnpClieAddr;
socklen_t    igmpSnpServAddrLen,igmpSnpClieAddrLen;

/* IgmpSnoop Sock Stream synchronization sem */
#if 0
unsigned char	igmpSnpSockCreated = GT_FALSE; /*added by wujh*/
#else
int	igmpSnpSockCreated = GT_FALSE;  /* code optimize: Function call "accept" maybe -1. zhangdi@autelan.com 2013-01-18 */
#endif

#endif
/**************************************************/
/* for DLDP												*/
int	adpt_dldp_RxFd = 0;
int adpt_dldp_RxCltFd = 0;
struct sockaddr_un dldp_Serv_Addr;
struct sockaddr_un dldp_Clie_Addr;
socklen_t dldp_Serv_Addr_Len;
socklen_t dldp_Clie_Addr_Len;

/* DLDP Sock Stream synchronization sem */
unsigned char dldpSockCreated = GT_FALSE; 
/**************************************************/

/**************************************************/
/* for vrrp										*/
int	adpt_vrrp_RxFd = 0;
int adpt_vrrp_RxCltFd = 0;
struct sockaddr_un vrrp_Serv_Addr;
struct sockaddr_un vrrp_Clie_Addr;
socklen_t vrrp_Serv_Addr_Len;
socklen_t vrrp_Clie_Addr_Len;

/* VRRP Sock Stream synchronization sem */
unsigned char vrrpSockCreated = GT_TRUE; 
/**************************************************/

#define  ADPT_THREAD_PRIO 	200
void    *userMutexId;/* signal new packet received */
void    *varLockId;/* global variable lock */
pthread_mutex_t streamSockId = PTHREAD_MUTEX_INITIALIZER;/*igmp socket stream*/
unsigned char  rstpSocketCreated = GT_FALSE;/* RSTP Sock Stream synchronization sem */

pthread_mutex_t semPktRxMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semKapIoMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t semRtRwMutex = PTHREAD_MUTEX_INITIALIZER;

/* semaphore for exclusively access socket I/O 
  * write igmp snooping and MLD snooping packets
  * to igmp snooping module */
pthread_mutex_t semWr2IgmpMutex = PTHREAD_MUTEX_INITIALIZER; 


int       adptVirRxFd = -1;
int		  adptTaskId = -1;
int       sockeTaskId = -1;
/********************************************************************************************
 * 	nam_packet_type_is_ISIS
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is ISIS or not.
 *             ISIS packet is transmitted with the special multicast MAC DA 01-80-c2-00-00-14
 *																			01-80-c2-00-00-15.
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE -indicate the packet is ISIS Packet in ISIS
 *              GT_FALSE -indicate the packet is not associated with ISIS.
 *
 *	COMMENTS:
 *             NONE.
 **********************************************************************************************/
unsigned char nam_packet_type_is_ISIS
(
	IN unsigned char  *packetBuff
){
	ether_header_t  *layer2 = NULL;
	unsigned char 	*macByte = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	macByte = layer2->dmac;

	if((macByte[3]==0x00)&&(macByte[4]==0x00)&& ((macByte[5]==0x14)||
		(macByte[5]==0x15))) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}

/********************************************************************************************
 * 	nam_packet_type_is_BPDU
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is STP or not.
 *             BPDU packet is transmitted with the special multicast MAC DA 01-80-c2-00-00-00.
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE -indicate the packet is STP Packet in BPDU
 *              GT_FALSE -indicate the packet is not associated with STP.
 *
 *	COMMENTS:
 *             NONE.
 **********************************************************************************************/
unsigned char nam_packet_type_is_BPDU
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	unsigned char 	*macByte = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	macByte = layer2->dmac;

	if((macByte[0]==0x01)&&(macByte[1]==0x80)&& (macByte[2]==0xc2)) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}
/********************************************************************************************
 * 	nam_packet_type_is_ARP
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is ARP or not.
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
unsigned char	nam_packet_type_is_ARP
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;

	layer2 = (ether_header_t*)(packetBuff);

	if(htons(0x0806) == layer2->etherType) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}

/********************************************************************************************
 * 	nam_packet_type_is_IPv4
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is IPv4 packet
 *              GT_FALSE - indicate the packet is not IPv4 packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_IPv4
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ip_header_t 	*layer3 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));

	if((htons(0x0800) == layer2->etherType) && 
		(4 == layer3->version)) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}

/********************************************************************************************
 * 	nam_packet_type_is_ICMP
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 ICMP or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is ICMP packet
 *              GT_FALSE - indicate the packet is not ICMP packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_ICMP
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ip_header_t 	*layer3 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));

	if((htons(0x0800) == layer2->etherType) && 
		 (4 == layer3->version) && 
			(0x1 == layer3->ipProtocol)) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}

/********************************************************************************************
 * 	nam_packet_type_is_TCP
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 TCP or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is TCP packet
 *              GT_FALSE - indicate the packet is not TCP packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_TCP
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ip_header_t 	*layer3 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));

	if((htons(0x0800) == layer2->etherType) && 
		 (4 == layer3->version) && 
			(0x6 == layer3->ipProtocol)) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}

/********************************************************************************************
 * 	nam_packet_type_is_Telnet
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 TCP telnet packet or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is TELNET packet
 *              GT_FALSE - indicate the packet is not TELNET packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_Telnet
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ip_header_t 	*layer3 = NULL;
	tcp_header_t	*layer4 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));
	layer4 = (tcp_header_t*)((unsigned int *)layer3 + layer3->hdrLength);

	if((htons(0x0800) == layer2->etherType) && 
		 (4 == layer3->version) && 
		  	(0x6 == layer3->ipProtocol)) {
			if((htons(0x17) == layer4->dest) ||
				(htons(0x17) == layer4->source)){/* tcp sport or dport 23 */
				return GT_TRUE;
			}
	}

	return GT_FALSE;
}

/********************************************************************************************
 * 	nam_packet_type_is_Ssh
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 TCP SSH packet or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is SSH packet
 *              GT_FALSE - indicate the packet is not SSH packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_Ssh
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ip_header_t 	*layer3 = NULL;
	tcp_header_t	*layer4 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));
	layer4 = (tcp_header_t*)((unsigned int *)layer3 + layer3->hdrLength);

	if((htons(0x0800) == layer2->etherType) && 
		(4 == layer3->version) && 
		(0x6 == layer3->ipProtocol)) {
		if((htons(0x16) == layer4->dest)||
			(htons(0x16) == layer4->source)){/* tcp sport or dport 22 */
			return GT_TRUE;
		}
	}

	return GT_FALSE;
}

/********************************************************************************************
 * 	nam_packet_type_is_Ftp
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 TCP FTP protocol packet or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is SSH packet
 *              GT_FALSE - indicate the packet is not SSH packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_Ftp
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ip_header_t 	*layer3 = NULL;
	tcp_header_t	*layer4 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));
	layer4 = (tcp_header_t*)((unsigned int *)layer3 + layer3->hdrLength);

	if((htons(0x0800) == layer2->etherType) && 
		(4 == layer3->version) && 
		(0x6 == layer3->ipProtocol)) {
		if((htons(0x15) == layer4->dest)||
			(htons(0x15) == layer4->source)){/* tcp sport or dport 21 */
			return GT_TRUE;
		}
	}

	return GT_FALSE;
}

/********************************************************************************************
 * 	nam_packet_type_is_Ftp_Data
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 TCP FTP data packet or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is SSH packet
 *              GT_FALSE - indicate the packet is not SSH packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_Ftp_Data
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ip_header_t 	*layer3 = NULL;
	tcp_header_t	*layer4 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));
	layer4 = (tcp_header_t*)((unsigned int *)layer3 + layer3->hdrLength);

	if((htons(0x0800) == layer2->etherType) && 
		(4 == layer3->version) && 
		(0x6 == layer3->ipProtocol)) {
		if((htons(0x14) == layer4->dest)||
			(htons(0x14) == layer4->source)){/* tcp sport or dport 20 */
			return GT_TRUE;
		}
	}

	return GT_FALSE;
}

/********************************************************************************************
 * 	nam_packet_type_is_UDP
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 UDP or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is UDP packet
 *              GT_FALSE - indicate the packet is not UDP packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_UDP
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ip_header_t 	*layer3 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));

	if((htons(0x0800) == layer2->etherType) && 
		 (4 == layer3->version) && 
		 	(0x11 == layer3->ipProtocol)) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}


/********************************************************************************************
 * 	nam_packet_type_is_IPIP
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 IPIP or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is IPIP packet
 *              GT_FALSE - indicate the packet is not IPIP packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_IPIP
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ip_header_t 	*layer3 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));

	if((htons(0x0800) == layer2->etherType) && 
		 (4 == layer3->version) && 
		 	(0x04 == layer3->ipProtocol)) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}


/********************************************************************************************
 * 	nam_packet_type_is_Dhcp
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is dhcp or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is IPv4 packet
 *              GT_FALSE - indicate the packet is not IPv4 packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_Dhcp
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ip_header_t 	*layer3 = NULL;
	udp_header_t	*layer4 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));
	layer4 = (udp_header_t*)((unsigned int *)layer3 + layer3->hdrLength);
	

	if ((htons(0x0800) == layer2->etherType) && 
			(4 == layer3->version) &&
				(0x11 == layer3->ipProtocol)) {
		if (((htons(67) == layer4->dest) && (htons(68) == layer4->source)) /* DHCP request */
			|| ((htons(67) == layer4->source) && (htons(68) == layer4->dest))){ /* DHCP reply<ack/nak> */

			return GT_TRUE;
		}
	}
	return GT_FALSE;
	
}
/********************************************************************************************
 * 	nam_packet_type_is_IGMP
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IGMP Snooping or not.
 *             IGMP packet is transmitted with the special multicast MAC DA 01-00-5e-xx-xx-xx.
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE -indicate the packet is IGMP Packet
 *              GT_FALSE -indicate the packet is not associated with IGMP.
 *
 *	COMMENTS:
 *             NONE.
 **********************************************************************************************/
unsigned char	nam_packet_type_is_IGMP
(       
	IN unsigned char  *packetBuff
)
{
	unsigned char ret = GT_FALSE;
	ether_header_t  *layer2 = NULL;
	ip_header_t		*layer3 = NULL;
	unsigned char 	*macByte = NULL;
	unsigned int	protcl;
	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));

	macByte = layer2->dmac;
	protcl = layer3->ipProtocol;
	if((macByte[0]==0x01)&&(macByte[1]==0x00)&&(macByte[2]==0x5e)) {
		if(!nam_packet_type_is_IPv4((unsigned char*)packetBuff)) {
			ret = GT_FALSE;
		}
		else if(2 == protcl) { /*ip protocol type IGMP*/
			ret = GT_TRUE;
		}
		else {
			ret = GT_FALSE;
		}
	}
	else {
		ret = GT_FALSE;
	}

	return ret;
}

/********************************************************************************************
 * 	nam_packet_type_is_IPv6
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv6 or not.
 *   
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is IPv6 packet
 *              GT_FALSE - indicate the packet is not IPv6 packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char	nam_packet_type_is_IPv6
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;
	ipv6_header_t 	*layer3 = NULL;

	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ipv6_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));

	if((htons(0x86DD) == layer2->etherType) && 
		(6 == layer3->version)) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}

/********************************************************************************************
 * 	nam_packet_type_is_MLD
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is MLD Snooping or not.
 *             MLD packet is transmitted with the special multicast MAC DA 33-33-xx-xx-xx-xx.
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE -indicate the packet is IGMP Packet
 *              GT_FALSE -indicate the packet is not associated with IGMP.
 *
 *	COMMENTS:
 *             NONE.
 **********************************************************************************************/
unsigned char	nam_packet_type_is_MLD
(       
	IN unsigned char  *packetBuff
)
{
	unsigned char ret = GT_FALSE;
	ether_header_t  *layer2 = NULL;
	ipv6_header_t	*layer3 = NULL;
	icmp6_header	*icmp6 = NULL;
	ipv6_opt_hdr	*option6 = NULL;	
	unsigned char 	*macByte = NULL;

	layer2 = (ether_header_t *)(packetBuff);
	layer3 = (ipv6_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));
	macByte = layer2->dmac;

	if((macByte[0]==0x33)&&(macByte[1]==0x33)) {
		if(!nam_packet_type_is_IPv6((unsigned char*)packetBuff)) {
			ret = GT_FALSE;
		}
		else if(NEXTHDR_HOP == layer3->nexthdr) { /*nextheader is ICMPv6 with  protocol type IGMP*/
			option6 = (ipv6_opt_hdr *)((unsigned char*)layer3 + sizeof(ipv6_header_t));
			if(NEXTHDR_ICMP == option6->nexthdr) {
				icmp6 = (icmp6_header *)((unsigned char *)option6 + ipv6_optlen(option6));
			}
		}
		else if(NEXTHDR_ICMP == layer3->nexthdr){
			icmp6 = (icmp6_header *)((unsigned char *)layer3 + sizeof(ipv6_header_t));
		}
		else {
			ret = GT_FALSE;
		}

		if(icmp6) {
			if(icmp6_is_mld(icmp6->icmp6_type)) {
				ret = GT_TRUE;
			}
		}
	}
	else {
		ret = GT_FALSE;
	}

	return ret;
}

/********************************************************************************************
 * 	nam_packet_type_is_PIMv2
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is PIMv2 or not.
 *             PIMv2 packet is transmitted with the special multicast MAC DA 33-33-xx-xx-xx-xx.
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE -indicate the packet is IGMP Packet
 *              GT_FALSE -indicate the packet is not associated with IGMP.
 *
 *	COMMENTS:
 *             NONE.
 **********************************************************************************************/
unsigned char	nam_packet_type_is_PIMv2
(       
	IN unsigned char  *packetBuff
)
{
	unsigned char ret = GT_FALSE;
	ether_header_t  *layer2 = NULL;
	ipv6_header_t	*layer3 = NULL;
	unsigned char 	*macByte = NULL;

	layer2 = (ether_header_t *)(packetBuff);
	layer3 = (ipv6_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));
	macByte = layer2->dmac;

	if((macByte[0]==0x33)&&(macByte[1]==0x33)) {
		if(!nam_packet_type_is_IPv6((unsigned char*)packetBuff)) {
			ret = GT_FALSE;
		}
		else if(NEXTHDR_PIMV2 == layer3->nexthdr) { /*nextheader is PIMV2 with  protocol type PIM*/
			ret = GT_TRUE;
		}
		else {
			ret = GT_FALSE;
		}
	}
	else {
		ret = GT_FALSE;
	}

	return ret;
}

/********************************************************************************************
 * 	nam_packet_type_is_DLDP
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 DLDP or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is DLDP packet
 *              GT_FALSE - indicate the packet is not DLDP packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char nam_packet_type_is_DLDP
(       
	IN unsigned char  *packetBuff
)
{
	ether_header_t  *layer2 = NULL;

	layer2 = (ether_header_t*)(packetBuff);

	if (htons(0x9003) == layer2->etherType) {
		return GT_TRUE;
	}
	else {
		return GT_FALSE;
	}
}


/********************************************************************************************
 * 	nam_packet_type_is_vrrp
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 VRRP or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is vrrp packet
 *              GT_FALSE - indicate the packet is not vrrp packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/
unsigned char nam_packet_type_is_VRRP
(       
	IN unsigned char  *packetBuff
)
{
	unsigned char ret = GT_FALSE;
	ether_header_t  *layer2 = NULL;
	ip_header_t		*layer3 = NULL;
	unsigned char 	*macByte = NULL;
	unsigned int	protcl;
	layer2 = (ether_header_t*)(packetBuff);
	layer3 = (ip_header_t *)((unsigned char*)packetBuff + sizeof(ether_header_t));

	macByte = layer2->dmac;
	protcl = layer3->ipProtocol;
	if((htons(0x0800) == layer2->etherType) && 
		(4 == layer3->version)) {
		if((macByte[0]==0x01)&&(macByte[1]==0x00)&&(macByte[2]==0x5e)) {
	       if(IPPROTO_VRRP == protcl){
	          ret = GT_TRUE;
		   }
	    }
	}

	return ret;

}
/********************************************************************************************
 * 	nam_packet_type_is_LACP
 *
 *	DESCRIPTION:
 *             This function check out whether the packet is IPv4 VRRP or not.
 *
 *	INPUT:
 *             packetBuff - points to the packet's first buffer' head
 *	OUTPUT:
 *               NONE
 *	RETURNS:
 *              GT_TRUE - indicate the packet is vrrp packet
 *              GT_FALSE - indicate the packet is not vrrp packet
 *
 *	COMMENTS:
 *             NONE.
 *
 **********************************************************************************************/

unsigned char nam_packet_type_is_LACP
(		
	IN unsigned char  *packetBuff
)
{
	unsigned char ret = GT_FALSE;
	ether_header_t	*layer2 = NULL;
	unsigned char	*macByte = NULL;
	unsigned char lacpMac[ETH_ALEN] = {0x01,0x80,0xc2,0x0,0x0,0x02};
	
	layer2 = (ether_header_t*)(packetBuff);
	macByte = layer2->dmac;	
	if(htons(0x8809) == layer2->etherType){
		if(!memcmp(lacpMac, macByte, ETH_ALEN)){
			ret = GT_TRUE;
		}
	}
	return ret;
}


/********************************************************************************************
 * 	nam_PacketParseType
 *
 *	DESCRIPTION:
 *             This function parse the packet and give out packet type
 *             All common packet types are supported,such as:
 *				- BPDU
 *				- GVRP
 *				- ARP
 *				- IP
 *				- ICMP
 *				- TCP
 *				- UDP
 *
 *	INPUT:
 *             packet - points to the packet buffer
 *
 *	OUTPUT:
 *               NONE
 *
 *	RETURNs:
 *              NAM_PACKET_TYPE_BPDU_E - if packet is BPDU
 *              NAM_PACKET_TYPE_GVRP_E - if packet is GVRP
 *		    NAM_PACKET_TYPE_ARP_E - if packet is ARP
 *		    NAM_PACKET_TYPE_DLDP_E - if packet is DLDP
 *		    NAM_PACKET_TYPE_IP_E - if packet is IP
 *		    NAM_PACKET_TYPE_IP_ICMP_E - if packet is ICMP
 *		    NAM_PACKET_TYPE_IP_TCP_E - if packet is TCP
 *		    NAM_PACKET_TYPE_IP_UDP_E - if packet is UDP
 *		    NAM_PACKET_TYPE_IP_IGMP_E - if packet is IGMP
 *		    NAM_PACKET_TYPE_IP_TCP_TELNET_E - if packet is TELNET
 *		    NAM_PACKET_TYPE_IP_TCP_SSH_E - if packet is SSH
 *
 *	COMMENTS:
 *             NONE.
 **********************************************************************************************/
enum NAM_PACKET_TYPE_ENT nam_packet_parse_type 
(
	IN unsigned char  *packet
)
{
	enum NAM_PACKET_TYPE_ENT packetType = NAM_PACKET_TYPE_OTHER;

	if(nam_packet_type_is_BPDU(packet)) {
		packetType = NAM_PACKET_TYPE_BPDU_E;		
		if(nam_packet_type_is_ISIS(packet)) {
			packetType = NAM_PACKET_TYPE_ISIS_E;
		}
		else if(nam_packet_type_is_LACP(packet)){
			packetType = NAM_PACKET_TYPE_LACP_E;
		}
	}
	else if(nam_packet_type_is_ARP(packet)) {
		packetType = NAM_PACKET_TYPE_ARP_E;
	}
	else if(nam_packet_type_is_DLDP(packet)) {
		packetType = NAM_PACKET_TYPE_DLDP_E;
	}
	else if(nam_packet_type_is_VRRP(packet)) {
		packetType = NAM_PACKET_TYPE_VRRP_E;
	}
	else if(nam_packet_type_is_IPv4(packet)) {
		packetType = NAM_PACKET_TYPE_IPv4_E;
		if(nam_packet_type_is_IGMP(packet)) {
			packetType = NAM_PACKET_TYPE_IP_IGMP_E;
		}
		else if(nam_packet_type_is_ICMP(packet)) {
			packetType =  NAM_PACKET_TYPE_IP_ICMP_E;
		}
		else if(nam_packet_type_is_TCP(packet)) {
			packetType =  NAM_PACKET_TYPE_IP_TCP_E;
			if(nam_packet_type_is_Telnet(packet)){
				packetType = NAM_PACKET_TYPE_IP_TCP_TELNET_E;
			}
			else if(nam_packet_type_is_Ssh(packet)) {
				packetType = NAM_PACKET_TYPE_IP_TCP_SSH_E;
			}
			else if(nam_packet_type_is_Ftp(packet)) {
				packetType = NAM_PACKET_TYPE_IP_TCP_FTP_E;
			}
			else if(nam_packet_type_is_Ftp_Data(packet)) {
				packetType = NAM_PACKET_TYPE_IP_TCP_FTP_DATA_E;
			}			
		}
		else if(nam_packet_type_is_UDP(packet)) {
			packetType =  NAM_PACKET_TYPE_IP_UDP_E;
			if(nam_packet_type_is_Dhcp(packet)){
				packetType = NAM_PACKET_TYPE_IP_UDP_DHCP_E;
			}
		}
		else if (nam_packet_type_is_IPIP(packet)){
			packetType = NAM_PACKET_TYPE_IP_IPIP_E;
		}
	}
	else if(nam_packet_type_is_IPv6(packet)) {
		packetType = NAM_PACKET_TYPE_IPv6_E;
		if(nam_packet_type_is_MLD(packet)) {
			packetType = NAM_PACKET_TYPE_IP6_MLD_E;
		}
		else if(nam_packet_type_is_PIMv2(packet)) {
			packetType = NAM_PACKET_TYPE_IP6_MLD_E;
		}
	}	
	
	return packetType;
}


void nam_dump_rxtx_packet_detail(unsigned char *buffer,unsigned long buffLen)
{
	unsigned int i;
	unsigned char lineBuffer[64] = {0}, *bufPtr = NULL;
	unsigned int curLen = 0;

	if(!buffer)
		return;
	
	nam_syslog_pkt_rx(".......................RX.......................%d\n",buffLen);
	bufPtr = lineBuffer;
	curLen = 0;
	for(i = 0;i < buffLen ; i++)
	{
		curLen += sprintf(bufPtr,"%02x ",buffer[i]);
		bufPtr = lineBuffer + curLen;
		
		if(0==(i+1)%16) {
			nam_syslog_pkt_rx("%s\n",lineBuffer);
			memset(lineBuffer,0,sizeof(lineBuffer));
			curLen = 0;
			bufPtr = lineBuffer;
		}
	}
	
	if((buffLen%16)!=0)
	{
		nam_syslog_pkt_rx("%s\n",lineBuffer);
	}
	
	nam_syslog_pkt_rx(".......................RX.......................\n");
}

long nam_virt_net_if_rx_packet
(
    IN unsigned char      	devNum,
    IN unsigned char    	queueIdx,
    IN unsigned long      	numOfBuff,
    IN unsigned char       	*packetBuffs[],
    IN unsigned long      	buffLen[],
    IN unsigned short		interfaceId
)
{
	int        	iterator=0;	 
	unsigned long    	buffByte, totalBytes = 0;
  
	VIRT_PKT_INFO_STC   rxPktBuf[ADAPTER_RX_PACKET_BUFF_NUM_MAX]= {0};/* Code Optimize: Uninitialized scalar variable Houxx@Autelan.Com  2013-1-20 */
	char logbuffer[1024] = {0},*bufPtr = NULL;
	int loglength = 0;

	bufPtr = logbuffer;

	for(;iterator<numOfBuff;iterator++)
	{
		rxPktBuf[iterator].l3_index = interfaceId;
		rxPktBuf[iterator].data_addr = packetBuffs[iterator];
		rxPktBuf[iterator].data_len = buffLen[iterator];
	}

	for(iterator=0;iterator<numOfBuff;iterator++)
	{
		buffByte = write(adptVirRxFd,&rxPktBuf[iterator],sizeof(VIRT_PKT_INFO_STC));
		if(buffByte != rxPktBuf[iterator].data_len)
		{
			loglength += sprintf(bufPtr,"pass packet(%dB) to kap driver error(%dB) errno %#x\n",		\
									rxPktBuf[iterator].data_len,buffByte,errno);
			bufPtr = logbuffer + loglength;
			nam_syslog_dbg("%s",logbuffer);
			return buffByte;
		}
		loglength += sprintf(bufPtr,"pass packet(%dB) to kap driver\n",buffByte);
		bufPtr = logbuffer + loglength;
		totalBytes += buffByte;
	}

	nam_syslog_dbg("%s",logbuffer);
	return totalBytes;
}


struct sockaddr_un 	servAddr;
struct sockaddr_un	clieAddr;
socklen_t    servAddrLen,clieAddrLen;

long nam_rstp_rx_packet
(
    IN unsigned char       	devNum,
    IN unsigned char      	queueIdx,
    IN unsigned long     	numOfBuff,
    IN unsigned char      	*packetBuffs[],
    IN unsigned long      	buffLen[],
    IN unsigned short		interfaceId

)
{ 
	int    totalLen = 0,i;
	BPDU_PKT_PARAM_STC   *rxPacket;
	int  length = 0;
	int  stcLen = sizeof(BPDU_PKT_PARAM_STC);
	char logbuffer[1024] = {0},*bufPtr = NULL;
	int loglength = 0;

	bufPtr = logbuffer;
	loglength += sprintf(bufPtr,"nam_rstp_rx_packet::");
	bufPtr = logbuffer + loglength;
	
	rxPacket = (BPDU_PKT_PARAM_STC *)malloc(MAX_BPDU_LEN);
	if(NULL == rxPacket) {
		loglength += sprintf(bufPtr,"memory allocate error!\n");
		bufPtr = logbuffer + loglength;
		nam_syslog_dbg("%s",logbuffer);
		return GT_NO_RESOURCE;
	}
	osMemSet(rxPacket,0,MAX_BPDU_LEN);

	for (i = 0; i<numOfBuff;i++) {    
		rxPacket->intfId= interfaceId;
		rxPacket->pktLen = (unsigned int)buffLen[i];
		rxPacket->pktData = (char*)(rxPacket + 1);
		 
		if(NULL == osMemCpy(rxPacket->pktData,packetBuffs[i],buffLen[i])) {	
			loglength += sprintf(bufPtr,"copy pakcet to buffer error!\n");
			bufPtr = logbuffer + loglength;
			nam_syslog_dbg("%s",logbuffer);
			free(rxPacket);
			return GT_NO_RESOURCE;
		}

	    #ifdef RSTP_SOCKET_STREAM
		length = send(adptRstpRxClientFd,(const void*)rxPacket,buffLen[i] + stcLen,0);
		if(length <= 0) {
			loglength += sprintf(bufPtr,"sendto() error fd(%d) packet(%dB) send(%dB)!\n",	\
											adptRstpRxFd, buffLen[i],length);
			bufPtr = logbuffer + loglength;
			nam_syslog_dbg("%s",logbuffer);
			free(rxPacket);
			return GT_FAIL;
		}	
		else {
			totalLen += length;
			loglength += sprintf(bufPtr,"sendto() fd(%d) packet(%dB) total send(%dB)\n",	\
											adptRstpRxFd,buffLen[i],totalLen);
			bufPtr = logbuffer + loglength;
		}
		#endif

		#ifdef RSTP_SOCKET_DGRAM
		if((length = sendto(adptRstpRxFd,(const void*)rxPacket,buffLen[i]+stcLen,0, \
								(struct sockaddr*)&clieAddr,sizeof(clieAddr)))== -1) {
			loglength += sprintf(bufPtr,"sendto() error fd(%d) packet(%dB) send(%dB)!\n",	\
											adptRstpRxFd, buffLen[i],length);
			bufPtr = logbuffer + loglength;
			nam_syslog_dbg("%s",logbuffer);
			free(rxPacket);
			return GT_FAIL;
		}
		else {
			totalLen += length;
			loglength += sprintf(bufPtr,"sendto() fd(%d) packet(%dB) total send(%dB)\n",	\
											adptRstpRxFd,buffLen[i],totalLen);
			bufPtr = logbuffer + loglength;
		}
		#endif
	} /*end for ...*/	 

	nam_syslog_dbg("%s",logbuffer);
	free(rxPacket);
	return totalLen;
}


/**********************************************************************************
 * nam_vrrp_rx_packet()
 *	DESCRIPTION:
 *		receive packets for vrrp rx 
 *
 *	INPUTS:
 *		unsigned long numOfBuff		- number of buffer
 *		unsigned char *packetBuffs[]	- array of buffer
 *		unsigned long buffLen[]			- array of buffer length
 *		unsigned char* ifname			- interface name
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
long nam_vrrp_rx_packet
(
	unsigned long numOfBuff,
	unsigned char *packetBuffs[],
	unsigned long buffLen[],
	unsigned int ifindex
)
{
	#if 1
	/* VRRP_SOCKET_DGRAM*/
	unsigned int i = 0;
	unsigned int totalLen = 0;
	unsigned int length = 0;
	unsigned int msgSize = 0;
	unsigned int stcLen = sizeof(VRRP_PKT_PARAM_STC);
	unsigned int loglength = 0;
	unsigned char *bufPtr = NULL;
	unsigned char logbuffer[1024] = {0};
	VRRP_PKT_PARAM_STC *rxPacket = NULL;

	bufPtr = logbuffer;

	loglength += sprintf(bufPtr, "vrrp receive ");
	bufPtr = logbuffer + loglength;
	
	rxPacket = (VRRP_PKT_PARAM_STC *)malloc(MAX_VRRP_LEN);
	if (NULL == rxPacket) {
		loglength += sprintf(bufPtr, "memory allocate error!\n");
		bufPtr = logbuffer + loglength;
		nam_syslog_dbg("%s", logbuffer);
		return GT_NO_RESOURCE;
	}
	memset(rxPacket, 0, MAX_VRRP_LEN);

	for (i = 0; i < numOfBuff; i++) {
		rxPacket->ifindex= ifindex;
		rxPacket->pktLen = (unsigned int)buffLen[i];
		rxPacket->pktData = (char*)(rxPacket + 1);
		if (buffLen[i] > MAX_VRRP_LEN)
		{
			loglength += sprintf(bufPtr, "packet %d(B) oversized\n", buffLen[i]);
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s", logbuffer);
			free(rxPacket);
			return GT_BAD_SIZE;
		}

		if (NULL == memcpy(rxPacket->pktData, packetBuffs[i], buffLen[i]))
		{	
			loglength += sprintf(bufPtr, "copy pakcet to buffer error!\n");
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s", logbuffer);
			free(rxPacket);
			return GT_NO_RESOURCE;
		}
		if((length = sendto(adpt_vrrp_RxFd, (const void*)rxPacket, buffLen[i]+stcLen, 0,
							(struct sockaddr*)&vrrp_Clie_Addr, sizeof(vrrp_Clie_Addr))) == -1)
		{
			loglength += sprintf(bufPtr,"sendto() error fd(%d) packet(%dB) send(%dB)!\n",
								adpt_vrrp_RxFd, buffLen[i], length);
			bufPtr = logbuffer + loglength;
			nam_syslog_dbg("%s", logbuffer);
			free(rxPacket);
			return GT_FAIL;
		}else {
			totalLen += length;
			loglength += sprintf(bufPtr,"sendto() fd(%d) packet(%dB) total send(%dB)\n",
								adpt_vrrp_RxFd, buffLen[i], totalLen);
			bufPtr = logbuffer + loglength;
		}
	}

	nam_syslog_dbg("%s", logbuffer);
	free(rxPacket);

	return totalLen;
	#endif
}

unsigned long nam_packet_rx_handling
(
	unsigned char device,
	unsigned char channel,
	void* data
)
{
#ifdef DRV_LIB_CPSS
	CPSS_DXCH_NET_RX_PARAMS_STC	*rxParams = NULL;
	PKT_INFO_PARAM *adptpktInfo = (PKT_INFO_PARAM *)data; 
#endif
#ifdef DRV_LIB_BCM
	BCM_PACKET_RX_PARAM_T *rxParams = NULL;
	BCM_PACKET_RX_INFO_T *adptpktInfo = (BCM_PACKET_RX_INFO_T *)data; 
#endif

#define MAX_STATIC_TRUNK_ID		119		/*<1-119> is for static trunk id and <120-127> is for dynamic */

	unsigned long	ret;
	long    	byteCount;
	unsigned long     	numOfBuff;
	unsigned char		willDrop = GT_FALSE; /*packet will be dropped or not */	
	unsigned char		needSnoop = GT_FALSE; /* ARP packet will be snooped or not */
	unsigned char		tx2VIf = GT_TRUE; /* transmit to kernel virtual net interface or not */
    unsigned char		tx2Wifi = GT_FALSE;

	unsigned int	intfId = 0;/* eth global index or trunk id */
	unsigned int 	l3Intf = 0;/* L3 interface index */
	unsigned char 	l3state = 0; /* L3 interface ustatus: 1 - up, 0 - down */
	unsigned short	vid = 0,trunkId = 0;
	unsigned char 	isTagged = GT_FALSE;
	unsigned int	status = 0;
	unsigned int    op_ret = 0;
	int prcess_ret = 0;
	enum NAM_PACKET_TYPE_ENT	packetType;
	enum NAM_INTERFACE_TYPE_ENT type;
	static char logbuffer[1024] = {0};
	char *bufPtr = NULL;
	int loglength = 0, i = 0;
	unsigned int dynamic_trunk = FALSE;
	unsigned int ifindex = ~0UI;
	unsigned char ifname[IF_NAMESIZE + 1] = {0};

	#ifdef DRV_LIB_BCM
	if(0 == npd_startup_end) {
		return GT_OK;
	}
	#endif
	
	if(NULL == adptpktInfo)
	{
		return GT_OK;
	}

#ifdef DRV_LIB_CPSS
	rxParams 	= &(adptpktInfo->rxRxParamPtr);
	vid = rxParams->dsaParam.commonParams.vid;
	numOfBuff 	= adptpktInfo->buffsNum;
#endif
#ifdef DRV_LIB_BCM
	numOfBuff 	= adptpktInfo->buffsNum;
	rxParams = &(adptpktInfo->rxparams);
	vid = rxParams->outerTag;
#endif
	/*****************************************************
	 * if smac is system-mac or default system base-mac, drop the packet
	 *****************************************************/
	ether_header_t *layer2 = NULL;
	unsigned char sysMac[MAC_ADDRESS_LEN + 1] = {0};
	unsigned char smac[MAC_ADDRESS_LEN + 1] = {0};
	unsigned char default_basemac[MAC_ADDRESS_LEN + 1]
		= {0x00, 0x0D, 0xEF, 0x00, 0x0D, 0xEF};

	/* get layer 2 head of packet */
	layer2 = (ether_header_t *)(adptpktInfo->buffPtr[0]);
	
	packetType = nam_packet_parse_type(adptpktInfo->buffPtr[0]);

	/* get system mac*/
	ret = npd_system_get_basemac(sysMac, MAC_ADDRESS_LEN);
	if (NAM_OK!= ret) {
		nam_syslog_err("get system mac error when rx packet!\n");
		return GT_FAIL;
	}
	nam_syslog_dbg("system mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
		sysMac[0], sysMac[1], sysMac[2], sysMac[3], sysMac[4], sysMac[5]);
    if ((NAM_PACKET_TYPE_BPDU_E != packetType) &&
		(NAM_PACKET_TYPE_DLDP_E != packetType)) {/*filter the bpdu and DLDP packet*/
		if (0 == memcmp(layer2->smac, sysMac, MAC_ADDRESS_LEN)) {
			nam_syslog_warning("packet's smac is system mac: %02x:%02x:%02x:%02x:%02x:%02x, drop it!\n",
								layer2->smac[0], layer2->smac[1], layer2->smac[2],
								layer2->smac[3], layer2->smac[4], layer2->smac[5]);
			return GT_OK;
		}
		if (0 == memcmp(layer2->smac, default_basemac, MAC_ADDRESS_LEN)) {
			nam_syslog_warning("packet's smac is default system base-mac: 00:0D:EF:00:0D:EF, drop it!\n");
			return GT_OK;
		}
    }
	/* OAM packet such as: telnet, ssh, ...etc
	  *  should not debug out, it may lead to debug flooding and dead loop
	  */
		
	nam_syslog_dbg("%d packet type %s received on device %d dma channel %d vid %d\n" , \
			numOfBuff, namPacketTypeStr[packetType], device, channel, vid);
	if((NAM_PACKET_TYPE_IP_TCP_TELNET_E != packetType)&&
		(NAM_PACKET_TYPE_IP_TCP_SSH_E != packetType)){
		for(i = 0;(i < numOfBuff && i < BUFF_LEN);i++)
		{
			nam_dump_rxtx_packet_detail(adptpktInfo->buffPtr[i], adptpktInfo->buffLength[i]);
		}
	}
	bufPtr = logbuffer;
	loglength += sprintf(bufPtr,"driver receive packet ");
	bufPtr = logbuffer + loglength;

#ifdef DRV_LIB_CPSS
	ret = cpssDxChPacketRxGetEthIndexViaDSA(device,rxParams,&type,&intfId);
#endif
#ifdef DRV_LIB_BCM
	if(rxParams->src_trunk > 0) {
		type = NAM_INTERFACE_TRUNK_E;
	}
	else {
		type = NAM_INTERFACE_PORT_E;
	}
	ret = npd_get_global_index_by_devport(device, rxParams->src_port, &intfId);
#endif
	if(ret != GT_OK){
		loglength += sprintf(bufPtr,"get mux via DSA error %d\n",ret);
		bufPtr = logbuffer + loglength;
		nam_syslog_err("%s",logbuffer);
		return GT_FAIL;
	}
	
	loglength += sprintf(bufPtr,"mux type %s %#x",	\
					(NAM_INTERFACE_PORT_E == type)? "port":	\
					(NAM_INTERFACE_TRUNK_E == type)? "trunk":	\
					(NAM_INTERFACE_VID_E == type)? "vid":"vidx", intfId);
	bufPtr = logbuffer + loglength;

	/* TODO: check if vlan legal and status up */
	status = npd_check_vlan_status(vid);
	if((packetType == NAM_PACKET_TYPE_LACP_E) && (vid == 1))
		status = 1;
	if(status >= 2) {
		loglength += sprintf(bufPtr," vlan %d(%s)",vid, "NOT-EXiST");
		bufPtr = logbuffer + loglength;
		willDrop = GT_TRUE;
	}
    else if(0 == status){
		loglength += sprintf(bufPtr," vlan %d(%s)",vid, "DOWN");
		bufPtr = logbuffer + loglength;
		willDrop = GT_TRUE;
    }
	else {
		loglength += sprintf(bufPtr," vlan %d(%s)",vid, "UP");
		bufPtr = logbuffer + loglength;
	}
	
	/* check interface type */
	status = 0;
	if(NAM_INTERFACE_PORT_E == type) {
		/* TODO: check port admin or link status */
		loglength += sprintf(bufPtr," port %d(",intfId);
		bufPtr = logbuffer + loglength;
		npd_get_port_admin_status(intfId,&status);
		if(!status) {
			loglength += sprintf(bufPtr,"DISABLED");
			bufPtr = logbuffer + loglength;
			willDrop = GT_TRUE;
		}
		else {
			loglength += sprintf(bufPtr,"ENABLED");
			bufPtr = logbuffer + loglength;
		}
		status = 0;
		npd_get_port_link_status(intfId,&status);
		if(!status) {
			loglength += sprintf(bufPtr,",LINK-DOWN");
			bufPtr = logbuffer + loglength;
			willDrop = GT_TRUE;
		}
		else {
			loglength += sprintf(bufPtr,",LINK-UP");
			bufPtr = logbuffer + loglength;
		}
		/* TODO: check port and vlan relationship */
		status = 0;
				/* check if port in LAG(trunk group)*/
		dynamic_trunk = FALSE;
		if((NPD_TRUE == npd_eth_port_interface_check(intfId, &ifindex))&&(~0UI != ifindex)){
			if_indextoname(ifindex, ifname);
			if(trunkId = npd_dynamic_trunk_slave_port_trunkId_get(ifname)){
				loglength += sprintf(bufPtr, ",IN dynamic-trunk %d", trunkId);
				bufPtr = logbuffer + loglength;
				dynamic_trunk = TRUE;
			}
		}
		if(!dynamic_trunk){
			op_ret = npd_eth_port_get_ptrunkid(intfId,&trunkId);
			if(0 != op_ret){
				loglength += sprintf(bufPtr, ",NOT-IN trunk");
				bufPtr = logbuffer + loglength;
			}
			else {
				if(trunkId <= MAX_STATIC_TRUNK_ID){
					loglength += sprintf(bufPtr, ",IN trunk %d",trunkId);
					bufPtr = logbuffer + loglength;
					intfId = trunkId;
					type = NAM_INTERFACE_TRUNK_E;
				}
			}
		}
		if(NAM_INTERFACE_PORT_E == type){
			status = npd_vlan_check_contain_port(vid,intfId,&isTagged);
			if(!status) {
				loglength += sprintf(bufPtr,",NOT-IN vlan");
				bufPtr = logbuffer + loglength;
				willDrop = GT_TRUE;		
			}
			else {
				loglength += sprintf(bufPtr,",%s-IN vlan",isTagged ? "TAG":"UNTAG");
				bufPtr = logbuffer + loglength;
				#ifdef DRV_LIB_BCM
				if(isTagged == rxParams->rx_untagged) {
					loglength += sprintf(bufPtr,"(mismatch)");
					bufPtr = logbuffer + loglength;
				}	
				#endif
			}
		}
		else if(NAM_INTERFACE_TRUNK_E == type){
			status = npd_vlan_check_contain_trunk(vid,trunkId,&isTagged);
			if(!status) {
				loglength += sprintf(bufPtr,",trunk NOT-IN vlan");
				bufPtr = logbuffer + loglength;
				willDrop = GT_TRUE;		
			}
			else {
				loglength += sprintf(bufPtr,",trunk %s-IN vlan",isTagged ? "TAG":"UNTAG");
				bufPtr = logbuffer + loglength;
				#ifdef DRV_LIB_BCM
				if(isTagged == rxParams->rx_untagged) {
					loglength += sprintf(bufPtr,"(mismatch)");
					bufPtr = logbuffer + loglength;
				}	
				#endif
			}
		}
		loglength += sprintf(bufPtr,")");
		bufPtr = logbuffer + loglength;
	}
	else if(NAM_INTERFACE_TRUNK_E == type) {
		/* TODO:need implemented */

	}
	else if(NAM_INTERFACE_VID_E == type) {
		/* TODO:need implemented */

	}

	/* drop packet if needed */
	#if 1
	if(willDrop) {
		loglength += sprintf(bufPtr," dropped!\n");
		bufPtr = logbuffer + loglength;
		nam_syslog_dbg("%s",logbuffer);
		#ifdef DRV_LIB_CPSS
		return GT_OK;
		#endif
		#ifdef DRV_LIB_BCM
		/*return GT_OK;*/
		#endif
	}
	#endif

	/* lock to prevent L3 interface modification */
	pthread_mutex_lock(&semPktRxMutex);
	loglength += sprintf(bufPtr," packet(");
	bufPtr = logbuffer + loglength;
	
	switch(packetType) {
		case NAM_PACKET_TYPE_BPDU_E:
			tx2VIf = GT_FALSE;
			loglength += sprintf(bufPtr,"BPDU");
			bufPtr = logbuffer + loglength;
			if(rstpSocketCreated) {
				loglength += sprintf(bufPtr,") send to RSTP process\n");
				bufPtr = logbuffer + loglength;
				byteCount = nam_rstp_rx_packet(device,channel,numOfBuff,&(adptpktInfo->buffPtr[0]),	\
																&(adptpktInfo->buffLength[0]),intfId);
			}
			else {
				loglength += sprintf(bufPtr,",DROP<no socket>)\n");
				bufPtr = logbuffer + loglength;
			}	
			break;
		case NAM_PACKET_TYPE_IP_IGMP_E:
			tx2VIf = GT_FALSE;
			loglength += sprintf(bufPtr,"IGMP");
			bufPtr = logbuffer + loglength;
			if(igmpSnpSockCreated) {
				loglength += sprintf(bufPtr,") send to IGMP Snooping process\n");
				bufPtr = logbuffer + loglength;
				byteCount = nam_igmpSnp_rx_packet(device,channel,numOfBuff,&(adptpktInfo->buffPtr[0]),	\
																&(adptpktInfo->buffLength[0]),intfId,vid);
			}
			else {
				loglength += sprintf(bufPtr,",DROP<no socket>");
				bufPtr = logbuffer + loglength;
			}	
			break;
		case NAM_PACKET_TYPE_IP6_MLD_E:
			tx2VIf = GT_TRUE;
			loglength += sprintf(bufPtr,"MLD");
			bufPtr = logbuffer + loglength;
			if(igmpSnpSockCreated) {
				loglength += sprintf(bufPtr,") send to MLD Snooping process\n");
				bufPtr = logbuffer + loglength;
				byteCount = nam_mldSnp_rx_packet(device,channel,numOfBuff,&(adptpktInfo->buffPtr[0]),	\
																&(adptpktInfo->buffLength[0]),intfId,vid);
			}
			else {
				loglength += sprintf(bufPtr,",DROP<no socket>");
				bufPtr = logbuffer + loglength;
			}	
			break;
		case NAM_PACKET_TYPE_DLDP_E:
			{
				tx2VIf = GT_FALSE;
				loglength += sprintf(bufPtr, "DLDP");
				bufPtr = logbuffer + loglength;
				if (dldpSockCreated) {
					loglength += sprintf(bufPtr, ") send to DLDP process\n");
					bufPtr = logbuffer + loglength;
					byteCount = nam_dldp_rx_packet( numOfBuff,
													&(adptpktInfo->buffPtr[0]),
													&(adptpktInfo->buffLength[0]),
													intfId, vid);
				}else {
					loglength += sprintf(bufPtr, ",DROP<no socket>");
					bufPtr = logbuffer + loglength;
				}
			}
			break;
				
		case NAM_PACKET_TYPE_GVRP_E:
			tx2VIf = GT_FALSE;
			loglength += sprintf(bufPtr,"GVRP");
			bufPtr = logbuffer + loglength;
			/* TODO: call NPD for GVRP process or send to GVRP module */
			loglength += sprintf(bufPtr,")\n");
			bufPtr = logbuffer + loglength;
			break;
		case NAM_PACKET_TYPE_ARP_E:
			loglength += sprintf(bufPtr,"ARP");
			bufPtr = logbuffer + loglength;
			#ifdef DRV_LIB_BCM
			needSnoop = GT_TRUE;
			#endif
			if((GT_TRUE == npd_vlan_interface_check(vid,&l3Intf))&& (~0UI != l3Intf)) {
				if(GT_FALSE == npd_check_port_promi_mode(intfId)||promis_port_add2_intf) {
					if(GT_OK == npd_vlan_l3intf_get_ustatus(vid,&l3state) && l3state){
						needSnoop = GT_TRUE;
						tx2VIf = GT_TRUE;
					}
					else {
						needSnoop = GT_FALSE;
						tx2VIf = GT_FALSE;
					}
					loglength += sprintf(bufPtr,",L3 VLAN<%d,%s>",l3Intf, l3state ? "U":"D");
					bufPtr = logbuffer + loglength;
				}
				else {
					
					needSnoop = GT_FALSE;
					tx2VIf = GT_FALSE;
				}
			}
			else if((GT_TRUE == npd_eth_port_interface_check(intfId,&l3Intf) ) && (~0UI != l3Intf)){
				if(GT_OK == npd_eth_port_l3intf_ustatus_get(intfId,&l3state) && l3state) {
					needSnoop = GT_TRUE;
					tx2VIf = GT_TRUE;
				}
				else {
					needSnoop = GT_FALSE;
					tx2VIf = GT_FALSE;
				}
				loglength += sprintf(bufPtr,",L3 Port<%d,%s>",l3Intf, l3state ? "U":"D");
				bufPtr = logbuffer + loglength;
			}
			else {
				tx2VIf = GT_FALSE;
				#ifdef DRV_LIB_CPSS
				needSnoop = GT_FALSE;
				#endif
				#ifdef DRV_LIB_BCM
				/*needSnoop = GT_FALSE;*/
				#endif
				loglength += sprintf(bufPtr,",NON-L3");
				bufPtr = logbuffer + loglength;
			}

			loglength += sprintf(bufPtr,"%s,%s)\n",needSnoop ? "learn":"discard", tx2VIf ? "kap":"non-kap");
			bufPtr = logbuffer + loglength;
			/* TODO: call NPD do arp snooping */
			if(GT_TRUE == needSnoop) {
				prcess_ret = npd_arp_packet_rx_process(type,intfId,l3Intf,vid,isTagged,adptpktInfo->buffPtr[0],adptpktInfo->buffLength[0]);
				if(ARP_RETURN_CODE_SUCCESS != prcess_ret){
                    tx2VIf = GT_FALSE;
				}
			}
			break;
		case NAM_PACKET_TYPE_IPv4_E:
		case NAM_PACKET_TYPE_IP_ICMP_E:
		case NAM_PACKET_TYPE_IP_TCP_E:
		case NAM_PACKET_TYPE_IP_UDP_E:
		case NAM_PACKET_TYPE_IP_TCP_TELNET_E:
		case NAM_PACKET_TYPE_IP_TCP_SSH_E:
		case NAM_PACKET_TYPE_IP_UDP_DHCP_E:
		case NAM_PACKET_TYPE_IP_TCP_FTP_E:
		case NAM_PACKET_TYPE_IP_TCP_FTP_DATA_E:
		case NAM_PACKET_TYPE_IP_IPIP_E:	
		case NAM_PACKET_TYPE_VRRP_E:
		case NAM_PACKET_TYPE_IPv6_E:
		case NAM_PACKET_TYPE_ISIS_E:
		case NAM_PACKET_TYPE_LACP_E:
			loglength += sprintf(bufPtr,"%s",namPacketTypeStr[packetType]);
			bufPtr = logbuffer + loglength;
			if((!dynamic_trunk)&&(GT_TRUE == npd_vlan_interface_check(vid,&l3Intf))&& (~0UI != l3Intf)) {
				if((GT_FALSE == npd_check_port_promi_mode(intfId))||promis_port_add2_intf) {
					loglength += sprintf(bufPtr,",L3 VLAN<%d>",l3Intf);
					bufPtr = logbuffer + loglength;
					tx2VIf = GT_TRUE;
				}
				else {
					tx2VIf = GT_FALSE;
				}
			}
			else if((GT_TRUE == npd_eth_port_interface_check(intfId,&l3Intf) ) && (~0UI != l3Intf)){
				loglength += sprintf(bufPtr,",L3 Port<%d>",l3Intf);
				bufPtr = logbuffer + loglength;
				tx2VIf = GT_TRUE;
			}
			else {
				loglength += sprintf(bufPtr,",NON-L3");
				bufPtr = logbuffer + loglength;
				tx2VIf = GT_FALSE;
				tx2Wifi = GT_TRUE;
			}
			loglength += sprintf(bufPtr,")\n");
			bufPtr = logbuffer + loglength;

			if (NAM_PACKET_TYPE_IP_UDP_DHCP_E == packetType) {
				loglength += sprintf(bufPtr, " send to DHCP-Snooping process\n");
				bufPtr = logbuffer + loglength;
				ret = npd_dhcp_snp_packet_rx_process(numOfBuff,
													&(adptpktInfo->buffPtr[0]),
													&(adptpktInfo->buffLength[0]),
													intfId, isTagged, vid);
				if (DHCP_SNP_RETURN_CODE_PKT_DROP == ret) {
					loglength += sprintf(bufPtr, ",DROP.");
					bufPtr = logbuffer + loglength;
					tx2VIf = GT_FALSE;
				}else {
					loglength += sprintf(bufPtr, ", ok.");
					bufPtr = logbuffer + loglength;
					tx2VIf = GT_TRUE;
				}
			}
			
		    #if 0
			if( NAM_PACKET_TYPE_VRRP_E == packetType){
				tx2VIf = GT_FALSE;
				loglength += sprintf(bufPtr, "VRRP");
				bufPtr = logbuffer + loglength;
				if (vrrpSockCreated) {
					loglength += sprintf(bufPtr, ") send to VRRP process\n");
					bufPtr = logbuffer + loglength;
					byteCount = nam_vrrp_rx_packet( numOfBuff,
													&(adptpktInfo->buffPtr[0]),
													&(adptpktInfo->buffLength[0]),
													l3Intf);
				}else {
					loglength += sprintf(bufPtr, ",DROP<no socket>");
					bufPtr = logbuffer + loglength;
				}
			}	
			#endif
			break;
		default:
			loglength += sprintf(bufPtr,"UNKNOWN)\n");
			bufPtr = logbuffer + loglength;
			if((!dynamic_trunk)&&(GT_TRUE == npd_vlan_interface_check(vid,&l3Intf))&& (~0UI != l3Intf)) {
				loglength += sprintf(bufPtr,"get L3 VLAN ifindex %d\n",l3Intf);
				bufPtr = logbuffer + loglength;
			}
			else if((GT_TRUE == npd_eth_port_interface_check(intfId,&l3Intf) ) && (~0UI != l3Intf)){
				loglength += sprintf(bufPtr,"get L3 PORT ifindex %d\n",l3Intf);
				bufPtr = logbuffer + loglength;
			}
			else{
				loglength += sprintf(bufPtr,"can't get ifindex\n");
				bufPtr = logbuffer + loglength;
			}
			tx2VIf = GT_TRUE; /* by default,deliver unknown type packet to KAP (by qinhs@autelan.com 2009-4-2)*/
			break;
	}
	
	/* transmit packet to kernel virtual net interface */
	if((GT_TRUE == tx2Wifi)||(NAM_PACKET_TYPE_IP_IPIP_E == packetType)){
		nam_syslog_dbg(" nam wifi rx packet, do nothing !!! \n");
		/* for bug: only open [/dev/wifi0], but no close!!! zhangdi 2011-11-22 */
		#if 0
       	byteCount = nam_wifi_net_if_rx_packet( numOfBuff,adptpktInfo->buffPtr,			                                                        adptpktInfo->buffLength );
		#endif
	}
	else if(GT_TRUE == tx2VIf) {
		nam_syslog_dbg(" nam virt rx packet \n");
		byteCount = nam_virt_net_if_rx_packet(device,channel,numOfBuff,adptpktInfo->buffPtr,
			                                                        adptpktInfo->buffLength,l3Intf);
	} 

	nam_syslog_dbg("%s",logbuffer);
	pthread_mutex_unlock(&semPktRxMutex);
	return GT_OK;

}

unsigned long nam_packet_rx_rstp_socket_init(void)
{
	#ifdef RSTP_SOCKET_STREAM
	if ((adptRstpRxFd= socket(AF_LOCAL, SOCK_STREAM, 0)) <0) 
	{
		nam_syslog_err("create adptRstpRxFd socket fail \n");
	}

	servAddr.sun_family = AF_LOCAL;
	strcpy(servAddr.sun_path, SOCK_PATH);
	unlink(servAddr.sun_path);
	servAddrLen = strlen(servAddr.sun_path) + sizeof(servAddr.sun_family);

	if (bind(adptRstpRxFd, (struct sockaddr *)&servAddr, servAddrLen) <0) 
	{
		nam_syslog_err("bind adptRstpRxFd fail \n");
		return GT_FAIL;
	}

	if (listen(adptRstpRxFd, 5) == -1) 
	{
		nam_syslog_err("listen adptRstpRxFd fail \n");
		return GT_FAIL;
	}
	#endif

	#ifdef RSTP_SOCKET_DGRAM
	memset(&clieAddr,0,sizeof(clieAddr));
	memset(&servAddr,0,sizeof(servAddr));

	if((adptRstpRxFd = socket(AF_LOCAL,SOCK_DGRAM,0))== -1)
	{ 
		nam_syslog_err("rstp rx socket create error\n");
		return GT_FAIL;
	}
	nam_syslog_dbg("create rstp rx socket fd %d\n",adptRstpRxFd);

	clieAddr.sun_family = AF_LOCAL;
	strcpy(clieAddr.sun_path, "/tmp/MSTP_CLIENT");
	servAddr.sun_family = AF_LOCAL;
	strcpy(servAddr.sun_path,"/tmp/MSTP_SERVER");					
	if(unlink(servAddr.sun_path) != 0) 
	{
		nam_syslog_err("unlink servAddr fail \n");
	}
	
	if(bind(adptRstpRxFd,(struct sockaddr *)&servAddr,sizeof(servAddr))== -1)
	{	
		nam_syslog_err("bind adptRstpRxFd fail \n");
		close(adptRstpRxFd);
		return GT_FAIL;
	}
	if(-1 == chmod(servAddr.sun_path, 0777))
	{			
		/* code optimize:  Unchecked return value from library  houxx@autelan.com  2013-1-20 */
		nam_syslog_err("chmod dldp_Serv_ADDR fail \n");
		close(adpt_dldp_RxFd);
		return GT_FAIL;
	}
	#endif

	return GT_OK;
}




/**********************************************************************************
 * nam_packet_rx_vrrp_socket_init()
 *	DESCRIPTION:
 *		builder stream socket for vrrp rx 
 *
 *	INPUTS:
 *		NULL
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		GT_FAIL		- build fail
 *		GT_OK		- build ok
***********************************************************************************/
unsigned long nam_packet_rx_vrrp_socket_init
(
	void
)
{
	#if 1
	memset(&vrrp_Clie_Addr, 0, sizeof(vrrp_Clie_Addr));
	memset(&vrrp_Serv_Addr, 0, sizeof(vrrp_Serv_Addr));

	if ((adpt_vrrp_RxFd = socket(AF_LOCAL, SOCK_DGRAM, 0)) == -1) { 
		nam_syslog_err("vrrp rx socket create error\n");
		return GT_FAIL;
	}
	nam_syslog_dbg("create vrrp rx socket fd %d\n", adpt_vrrp_RxFd);

	vrrp_Clie_Addr.sun_family = AF_LOCAL;
	strcpy(vrrp_Clie_Addr.sun_path, "/tmp/VRRP_CLIENT");
	vrrp_Serv_Addr.sun_family = AF_LOCAL;
	strcpy(vrrp_Serv_Addr.sun_path, "/tmp/VRRP_SERVER");					
	if (unlink(vrrp_Serv_Addr.sun_path) != 0) {
		nam_syslog_err("unlink vrrp_Serv_Addr fail \n");
	}

	if (bind(adpt_vrrp_RxFd, (struct sockaddr *)&vrrp_Serv_Addr, sizeof(vrrp_Serv_Addr))== -1)
	{
		nam_syslog_err("bind vrrp_Serv_Addr fail \n");
		close(adpt_vrrp_RxFd);
		return GT_FAIL;
	}
	if(-1 == chmod(vrrp_Serv_Addr.sun_path, 0777))
	{	
		/* code optimize:  Unchecked return value from library  houxx@autelan.com  2013-1-20 */
		nam_syslog_err("chmod dldp_Serv_ADDR fail \n");
		close(adpt_dldp_RxFd);
		return GT_FAIL;
	}	
	#endif

	return GT_OK;
}

#ifdef IGMP_SNOOP_SOCKET_STREAM
long nam_igmpSnp_rx_packet
(
    IN unsigned char       	devNum,
    IN unsigned char      	queueIdx,
    IN unsigned long     	numOfBuff,
    IN unsigned char      	*packetBuffs[],
    IN unsigned long      	buffLen[],
    IN unsigned int interfaceId,/*portOrTrunkId  has been transfer to eth_g_index*/
    IN unsigned short		vid
)
{ 
	int	length = 0,totalLen = 0,i;
	IGMP_SNP_PARAM_STC   *rxPacket;
	unsigned int iphdrLen = 0x14;/* no ip header option as default */
	char logbuffer[NAM_SYSLOG_LINE_BUFFER_SIZE] = {0},*bufPtr = NULL;
	int loglength = 0;
	unsigned int msgSize = 0;

	bufPtr = logbuffer;

	loglength += sprintf(bufPtr,"igmp snooping receive ");
	bufPtr = logbuffer + loglength;

	ether_header_t  *layer2 = (ether_header_t*)packetBuffs[0];
	ip_header_t 	*layer3 = (ip_header_t*)(layer2 + 1);
	iphdrLen = (layer3->hdrLength)<<2;/*ip header length: 20/24*/
	igmp_header_t	*layer4 = (igmp_header_t*)((char*)layer3 + iphdrLen);
	rxPacket = (IGMP_SNP_PARAM_STC*)malloc(sizeof(IGMP_SNP_PARAM_STC));
	if(NULL == rxPacket)
	{
		loglength += sprintf(bufPtr,"memory allocate error!\n");
		bufPtr = logbuffer + loglength;
		nam_syslog_err("%s",logbuffer);
		return GT_NO_RESOURCE;
	}
	osMemSet(rxPacket,0,sizeof(IGMP_SNP_PARAM_STC));

	rxPacket->nlh.nlmsg_type = IGMP_SNP_TYPE_PACKET_MSG;
	switch(layer4->type){
		case 0x11:/*membership Query*/
		case 0x12:/*V1 membership Report*/
		case 0x16:/*V2 membership Report*/
		case 0x22:/*v3 membership Report*/
		case 0x17:/*group leave*/
			rxPacket->nlh.nlmsg_flags = IGMP_SNP_FLAG_PKT_IGMP;
			break;
		default :
			rxPacket->nlh.nlmsg_flags = IGMP_SNP_FLAG_PKT_UNKNOWN;
				
	}

	loglength += sprintf(bufPtr,"packet type(%d) flag(%d) intf(%d) vid(%d) ",\
							rxPacket->nlh.nlmsg_type,rxPacket->nlh.nlmsg_flags,	\
							(unsigned int)interfaceId,(unsigned int)vid);
	bufPtr = logbuffer + loglength;
	nam_syslog_dbg("%s\n",logbuffer);
	bufPtr = logbuffer;
	loglength = 0;

	for (i = 0; i<numOfBuff;i++)
	{    
		rxPacket->ifindex	= interfaceId;
		rxPacket->vlanid	= (unsigned int )vid;
		/*rxPacket->trunkflag = 0;*/ /*not support trunk interface*/
	
		if(buffLen[i] > IGMP_SNOOPING_BUFLEN) {
			loglength += sprintf(bufPtr,"packet %d(B) oversized\n",buffLen[i]);
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s",logbuffer);
			free(rxPacket);
			return GT_BAD_SIZE;
		}
		
		if(NULL == osMemCpy((rxPacket->buf),packetBuffs[i],buffLen[i]))
		{	
			loglength += sprintf(bufPtr,"copy pakcet to buffer error!\n");
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s",logbuffer);
			free(rxPacket);
			return GT_NO_RESOURCE;
		}

		/*
		 * Unix socket data size:nlmsghdr + ifindex + vlanid + actual packet size 
		 */
		msgSize = buffLen[i]+8+sizeof(struct nlmsghdr);

		/*8-length of append vid & ifindex*/
		rxPacket->nlh.nlmsg_len = msgSize;

		pthread_mutex_lock(&semWr2IgmpMutex);
		length = send(adptIgmpSnpRxCltFd,(const void*)rxPacket,sizeof(IGMP_SNP_PARAM_STC),0);
		pthread_mutex_unlock(&semWr2IgmpMutex);
		if(length <= 0)  
		{
			loglength += sprintf(bufPtr,"send to igmp snooping error fd(%d) packet(%dB) send(%dB)!\n",
				                  adptIgmpSnpRxCltFd,buffLen[i],length);
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s",logbuffer);
			free(rxPacket);
			return GT_FAIL;
		}	
		else
		{
			totalLen += length;
			loglength += sprintf(bufPtr,"send to igmp snooping fd(%d) packet(%dB) total send(%dB).\n",	\
								adptIgmpSnpRxCltFd,buffLen[i],totalLen);
			bufPtr = logbuffer + loglength;
		}
		
		nam_syslog_dbg("%s\n",logbuffer);
		bufPtr = logbuffer;
		loglength = 0;
	} /*end for ...*/	 

	free(rxPacket);
	return totalLen;
}

long nam_mldSnp_rx_packet
(
    IN unsigned char       	devNum,
    IN unsigned char      	queueIdx,
    IN unsigned long     	numOfBuff,
    IN unsigned char      	*packetBuffs[],
    IN unsigned long      	buffLen[],
    IN unsigned int 		interfaceId,/*portOrTrunkId  has been transfer to eth_g_index*/
    IN unsigned short		vid
)
{ 
	int	length = 0,totalLen = 0,i;
	IGMP_SNP_PARAM_STC   *rxPacket = NULL;
	char logbuffer[NAM_SYSLOG_LINE_BUFFER_SIZE] = {0},*bufPtr = NULL;
	int loglength = 0;
	unsigned int msgSize = 0;

	bufPtr = logbuffer;

	loglength += sprintf(bufPtr,"mld snooping receive ");
	bufPtr = logbuffer + loglength;

	ether_header_t  *layer2 = (ether_header_t *)packetBuffs[0];
	ipv6_header_t 	*layer3 = (ipv6_header_t *)(layer2 + 1);
	ipv6_opt_hdr 	*option6 = NULL;
	icmp6_header	*icmp6 = NULL;
	pimv2_header	*pim2 = NULL;
	
	rxPacket = (IGMP_SNP_PARAM_STC*)malloc(sizeof(IGMP_SNP_PARAM_STC));
	if(NULL == rxPacket)
	{
		loglength += sprintf(bufPtr,"memory allocate error!\n");
		bufPtr = logbuffer + loglength;
		nam_syslog_err("%s",logbuffer);
		return GT_NO_RESOURCE;
	}
	osMemSet(rxPacket,0,sizeof(IGMP_SNP_PARAM_STC));

	if(NEXTHDR_HOP == layer3->nexthdr) {
		option6 = (ipv6_opt_hdr *)((char*)layer3 + sizeof(ipv6_header_t));
		if(NEXTHDR_ICMP == option6->nexthdr) {
			icmp6 = (icmp6_header *)((char *)option6 + ipv6_optlen(option6));
		}
	}
	else if(NEXTHDR_ICMP == layer3->nexthdr) {
		icmp6 = (icmp6_header *)((char*)layer3 + sizeof(ipv6_header_t));
	}
	else if(NEXTHDR_PIMV2 == layer3->nexthdr) {
		pim2 = (pimv2_header *)((char*)layer3 + sizeof(ipv6_header_t));
	}

	if((NULL == icmp6)&&(NULL == pim2)){
		loglength += sprintf(bufPtr,"no icmp6 header found!\n");
		bufPtr = logbuffer + loglength;
		nam_syslog_err("%s",logbuffer);
		free(rxPacket);
		rxPacket = NULL;
		return GT_FAIL;
	}
	
	if(NULL != icmp6){
		switch(icmp6->icmp6_type){
			case ICMPV6_MGM_QUERY: /*membership Query*/
			case ICMPV6_MGM_REPORT:/*V1 membership Report*/
			case ICMPV6_MLD2_REPORT:/*V2 membership Report*/
			case ICMPV6_MGM_REDUCTION:/*group leave*/
				rxPacket->nlh.nlmsg_flags = MLD_SNP_FLAG_PKT_MLD;
				break;
			default:
				rxPacket->nlh.nlmsg_flags = MLD_SNP_FLAG_PKT_UNKNOWN;
				break;
		}
	}
	else if(pim2 && (0x2 == pim2->version)) {
			rxPacket->nlh.nlmsg_flags = MLD_SNP_FLAG_PKT_MLD;
	}
	
	rxPacket->nlh.nlmsg_type = MLD_SNP_TYPE_PACKET_MSG;

	loglength += sprintf(bufPtr,"packet type(%d) flag(%d) intf(%d) vid(%d) ",\
							rxPacket->nlh.nlmsg_type,rxPacket->nlh.nlmsg_flags,	\
							(unsigned int)interfaceId,(unsigned int)vid);
	bufPtr = logbuffer + loglength;
	nam_syslog_dbg("%s\n",logbuffer);
	bufPtr = logbuffer;
	loglength = 0;

	for (i = 0; i<numOfBuff;i++)
	{    
		rxPacket->ifindex	= interfaceId;
		rxPacket->vlanid	= (unsigned int )vid;
		/*rxPacket->trunkflag = 0;*/ /*not support trunk interface*/
	
		if(buffLen[i] > MLD_SNOOPING_BUFLEN) {
			loglength += sprintf(bufPtr,"packet %d(B) oversized\n",buffLen[i]);
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s",logbuffer);
			free(rxPacket);
			return GT_BAD_SIZE;
		}
		
		if(NULL == osMemCpy((rxPacket->buf),packetBuffs[i],buffLen[i]))
		{	
			loglength += sprintf(bufPtr,"copy pakcet to buffer error!\n");
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s",logbuffer);
			free(rxPacket);
			return GT_NO_RESOURCE;
		}

		/*
		 * Unix socket data size:nlmsghdr + ifindex + vlanid + actual packet size 
		 */
		msgSize = buffLen[i]+8+sizeof(struct nlmsghdr);

		/*8-length of append vid & ifindex*/
		rxPacket->nlh.nlmsg_len = msgSize;

		pthread_mutex_lock(&semWr2IgmpMutex);
		length = send(adptIgmpSnpRxCltFd,(const void*)rxPacket,sizeof(IGMP_SNP_PARAM_STC),0);
		pthread_mutex_unlock(&semWr2IgmpMutex);
		if(length <= 0)  
		{
			loglength += sprintf(bufPtr,"send to mld snooping error fd(%d) packet(%dB) send(%dB)!\n",
				                  adptIgmpSnpRxCltFd,buffLen[i],length);
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s",logbuffer);
			free(rxPacket);
			return GT_FAIL;
		}	
		else
		{
			totalLen += length;
			loglength += sprintf(bufPtr,"send to mld snooping fd(%d) packet(%dB) total send(%dB).\n",	\
								adptIgmpSnpRxCltFd,buffLen[i],totalLen);
			bufPtr = logbuffer + loglength;
		}
		
		nam_syslog_dbg("%s\n",logbuffer);
		bufPtr = logbuffer;
		loglength = 0;
	} /*end for ...*/	 

	free(rxPacket);
	return totalLen;
}

#endif

unsigned long nam_packet_rx_igmpsnp_socket_init(void)
{
	#ifdef IGMP_SNOOP_SOCKET_STREAM
	if ((adptIgmpSnpRxFd= socket(AF_LOCAL, SOCK_STREAM, 0)) <0) 
	{
		nam_syslog_err("igmp snooping rx socket create error\n");
		return GT_FAIL;
	}
	nam_syslog_dbg("create igmp snooping rx socket fd %d\n",adptIgmpSnpRxFd);

	memset(&igmpSnpServAddr,0,sizeof(struct sockaddr_un));
	igmpSnpServAddr.sun_family = AF_LOCAL;
	strcpy(igmpSnpServAddr.sun_path, NPD_IGMP_SNOOP_PKT_SOCK_PATH);
	if (unlink(igmpSnpServAddr.sun_path) != 0) {
		nam_syslog_err("unlink igmpSnpServAddr fail \n");
	}
	igmpSnpServAddrLen = strlen(igmpSnpServAddr.sun_path) + sizeof(igmpSnpServAddr.sun_family);

	if (bind(adptIgmpSnpRxFd,(struct sockaddr *)&igmpSnpServAddr,igmpSnpServAddrLen)<0) 
	{
		nam_syslog_err("bind adptIgmpSnpRxFd fail \n");
		return GT_FAIL;
	}

	if (listen(adptIgmpSnpRxFd, 5) == -1) 
	{
		nam_syslog_err("listen adptIgmpSnpRxFd fail \n");
		return GT_FAIL;
	}
	pthread_mutex_unlock(&streamSockId);/*osSemSignal(streamSockId);*/	
	#endif
	
	#ifdef IGMP_SNOOP_SOCKET_DGRAM
	memset(&igmpSnpClieAddr,0,sizeof(igmpSnpClieAddr));
	memset(&igmpSnpServAddr,0,sizeof(igmpSnpServAddr));

	if((adptIgmpSnpRxFd = socket(AF_LOCAL,SOCK_DGRAM,0))== -1)
	{ 
		nam_syslog_err("create adptIgmpSnpRxFd socket fail \n");
		return GT_FAIL;
	}

	igmpSnpClieAddr.sun_family = AF_LOCAL;
	strcpy(igmpSnpClieAddr.sun_path, "/tmp/igmp_client");
	igmpSnpServAddr.sun_family = AF_LOCAL;
	strcpy(igmpSnpServAddr.sun_path,"/tmp/igmp_serv");					
	if(unlink(igmpSnpServAddr.sun_path) != 0) 
	{
		nam_syslog_err("igmp_unlink adptIgmpSnpRxFd fail \n");
	}
	
	if(bind(adptIgmpSnpRxFd,(struct sockaddr *)&igmpSnpServAddr,sizeof(igmpSnpServAddr))== -1)
	{	
		nam_syslog_err("bind adptIgmpSnpRxFd fail \n");
		close(adptIgmpSnpRxFd);
		return GT_FAIL;
	}
	if(-1 == chmod(igmpSnpServAddr.sun_path, 0777))
	{	
		/* code optimize:  Unchecked return value from library  houxx@autelan.com  2013-1-20 */
		nam_syslog_err("chmod chmod(igmpSnpServAddr.sun_path, 0777) fail \n");
		close(adpt_dldp_RxFd);
		return GT_FAIL;
	}	
	#endif

	return GT_OK;
}

/**********************************************************************************
 * nam_dldp_rx_packet()
 *	DESCRIPTION:
 *		receive packets for dldp rx 
 *
 *	INPUTS:
 *		unsigned long numOfBuff		- number of buffer
 *		unsigned char *packetBuffs[]	- array of buffer
 *		unsigned long buffLen[]			- array of buffer length
 *		unsigned int interfaceId			- port Or TrunkId has been transfer to eth_g_index
 *		unsigned short vid				- vlanid
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
long nam_dldp_rx_packet
(
	unsigned long numOfBuff,
	unsigned char *packetBuffs[],
	unsigned long buffLen[],
	unsigned int interfaceId,
	unsigned short vid
)
{
	#if 0
	// DLDP_SOCKET_STREAM
	unsigned int i = 0;
	unsigned int length = 0;
	unsigned int totalLen = 0;
	unsigned int msgSize = 0;
	DLDP_PARAM_STC *rxPacket = NULL;

	int loglength = 0;
	char logbuffer[1024] = {0};
	char *bufPtr = NULL;

	if (NULL == packetBuffs || NULL == buffLen) {
		nam_syslog_err("dldp receive packet, parameter is null.\n");
		return GT_FAIL;
	}
	
	bufPtr = logbuffer;

	loglength += sprintf(bufPtr, "DLDP receive ");
	bufPtr = logbuffer + loglength;

	rxPacket = (DLDP_PARAM_STC *)malloc(sizeof(DLDP_PARAM_STC));
	if (NULL == rxPacket) {
		loglength += sprintf(bufPtr, "memory allocate error!\n");
		bufPtr = logbuffer + loglength;
		nam_syslog_err("%s", logbuffer);
		return GT_NO_RESOURCE;
	}
	memset(rxPacket, 0, sizeof(DLDP_PARAM_STC));

	rxPacket->nlh.nlmsg_type = DLDP_TYPE_PACKET_MSG;

	loglength += sprintf(bufPtr, "packet type(%d) intf(%d) vid(%d) ",
						rxPacket->nlh.nlmsg_type,
						(unsigned int)interfaceId,
						(unsigned int)vid);

	bufPtr = logbuffer + loglength;
	nam_syslog_dbg("%s\n", logbuffer);
	bufPtr = logbuffer;
	loglength = 0;

	for (i = 0; i < numOfBuff; i++)
	{	 
		rxPacket->ifindex = interfaceId;
		rxPacket->vlanid = (unsigned int)vid;
		/*rxPacket->trunkflag = 0;*/ /*not support trunk interface*/
	
		if (buffLen[i] > DLDP_BUFLEN)
		{
			loglength += sprintf(bufPtr, "packet %d(B) oversized\n", buffLen[i]);
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s", logbuffer);
			free(rxPacket);
			return GT_BAD_SIZE;
		}
		
		if (NULL == memcpy((rxPacket->buf),  packetBuffs[i], buffLen[i]))
		{	
			loglength += sprintf(bufPtr, "copy pakcet to buffer error!\n");
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s", logbuffer);
			free(rxPacket);
			return GT_NO_RESOURCE;
		}

		/*
		 * Unix socket data size:nlmsghdr + ifindex + vlanid + tagmode + actual packet size 
		 */
		msgSize = buffLen[i] + 12 + sizeof(struct nlmsghdr);

		/*8-length of append vid & ifindex*/
		rxPacket->nlh.nlmsg_len = msgSize;

		length = send(  adpt_dldp_RxCltFd,
						(const void*)rxPacket,
						sizeof(DLDP_PARAM_STC),
						0);
		if (length <= 0) {
			loglength += sprintf(bufPtr, "send to DLDP error fd(%d) packet(%dB) send(%dB)!\n",
								adpt_dldp_RxCltFd, buffLen[i], length);
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s",logbuffer);
			free(rxPacket);
			return GT_FAIL;
		}else {
			totalLen += length;
			loglength += sprintf(bufPtr, "send to DLDP fd(%d) packet(%dB) total send(%dB).\n",	\
								adpt_dldp_RxCltFd, buffLen[i], totalLen);
			bufPtr = logbuffer + loglength;
		}
		
		nam_syslog_dbg("%s\n", logbuffer);
		bufPtr = logbuffer;
		loglength = 0;
	} 

	free(rxPacket);
	return totalLen;
	#endif

	#if 1
	/* DLDP_SOCKET_DGRAM*/
	unsigned int i = 0;
	unsigned int totalLen = 0;
	unsigned int length = 0;
	unsigned int msgSize = 0;
	unsigned int stcLen = sizeof(DLDP_PARAM_STC);
	unsigned int loglength = 0;
	unsigned char *bufPtr = NULL;
	unsigned char logbuffer[1024] = {0};
	DLDP_PARAM_STC *rxPacket = NULL;

	bufPtr = logbuffer;

	loglength += sprintf(bufPtr, "DLDP receive ");
	bufPtr = logbuffer + loglength;
	
	rxPacket = (DLDP_PARAM_STC *)malloc(sizeof(DLDP_PARAM_STC));
	if (NULL == rxPacket) {
		loglength += sprintf(bufPtr, "memory allocate error!\n");
		bufPtr = logbuffer + loglength;
		nam_syslog_dbg("%s", logbuffer);
		return GT_NO_RESOURCE;
	}
	memset(rxPacket, 0, sizeof(DLDP_PARAM_STC));

	for (i = 0; i < numOfBuff; i++) {
		rxPacket->ifindex = interfaceId;
		rxPacket->vlanid = (unsigned int)vid;

		if (buffLen[i] > DLDP_BUFLEN)
		{
			loglength += sprintf(bufPtr, "packet %d(B) oversized\n", buffLen[i]);
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s", logbuffer);
			free(rxPacket);
			return GT_BAD_SIZE;
		}

		if (NULL == memcpy(rxPacket->buf, packetBuffs[i], buffLen[i]))
		{	
			loglength += sprintf(bufPtr, "copy pakcet to buffer error!\n");
			bufPtr = logbuffer + loglength;
			nam_syslog_err("%s", logbuffer);
			free(rxPacket);
			return GT_NO_RESOURCE;
		}

		/*
		 * Unix socket data size:nlmsghdr + ifindex + vlanid + tagmode + actual packet size 
		 */
		msgSize = buffLen[i] + 12 + sizeof(struct nlmsghdr);

		/*8-length of append vid & ifindex*/
		rxPacket->nlh.nlmsg_len = msgSize;
		rxPacket->nlh.nlmsg_type = NPD_DLDP_TYPE_PACKET_MSG;

		if((length = sendto(adpt_dldp_RxFd, (const void*)rxPacket, buffLen[i]+stcLen, 0,
							(struct sockaddr*)&dldp_Clie_Addr, sizeof(dldp_Clie_Addr))) == -1)
		{
			loglength += sprintf(bufPtr,"sendto() error fd(%d) packet(%dB) send(%dB)!\n",
								adpt_dldp_RxFd, buffLen[i], length);
			bufPtr = logbuffer + loglength;
			nam_syslog_dbg("%s", logbuffer);
			free(rxPacket);
			return GT_FAIL;
		}else {
			totalLen += length;
			loglength += sprintf(bufPtr,"sendto() fd(%d) packet(%dB) total send(%dB)\n",
								adpt_dldp_RxFd, buffLen[i], totalLen);
			bufPtr = logbuffer + loglength;
		}
	}

	nam_syslog_dbg("%s", logbuffer);
	free(rxPacket);

	return totalLen;
	#endif
}



/**********************************************************************************
 * nam_packet_rx_dldp_socket_init()
 *	DESCRIPTION:
 *		builder stream socket for dldp rx 
 *
 *	INPUTS:
 *		NULL
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		GT_FAIL		- build fail
 *		GT_OK		- build ok
***********************************************************************************/
unsigned long nam_packet_rx_dldp_socket_init
(
	void
)
{
	#if 0
	// DLDP_SOCKET_STREAM
	if ((adpt_dldp_RxFd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
		nam_syslog_err("DLDP rx socket create error\n");
		return GT_FAIL;
	}
	nam_syslog_dbg("create DLDP rx socket fd %d\n", adpt_dldp_RxFd);

	memset(&dldp_Serv_Addr, 0, sizeof(struct sockaddr_un));
	dldp_Serv_Addr.sun_family = AF_LOCAL;
	strcpy(dldp_Serv_Addr.sun_path,  DLDP_SKB_SOCK_PATH);
	unlink(dldp_Serv_Addr.sun_path);
	dldp_Serv_Addr_Len = strlen(dldp_Serv_Addr.sun_path) + sizeof(dldp_Serv_Addr.sun_family);

	if (bind(adpt_dldp_RxFd,(struct sockaddr *)&dldp_Serv_Addr, dldp_Serv_Addr_Len) < 0) {
		return GT_FAIL;
	}

	if (listen(adpt_dldp_RxFd, 5) == -1) {
		return GT_FAIL;
	}
	pthread_mutex_unlock(&streamSockId);
	#endif

	#if 1
	/* DLDP_SOCKET_DGRAM*/
	memset(&dldp_Clie_Addr, 0, sizeof(dldp_Clie_Addr));
	memset(&dldp_Serv_Addr, 0, sizeof(dldp_Serv_Addr));

	if ((adpt_dldp_RxFd = socket(AF_LOCAL, SOCK_DGRAM, 0)) == -1) { 
		nam_syslog_err("DLDP rx socket create error\n");
		return GT_FAIL;
	}
	nam_syslog_dbg("create DLDP rx socket fd %d\n", adpt_dldp_RxFd);

	dldp_Clie_Addr.sun_family = AF_LOCAL;
	strcpy(dldp_Clie_Addr.sun_path, "/tmp/dldp_skb_client");
	dldp_Serv_Addr.sun_family = AF_LOCAL;
	strcpy(dldp_Serv_Addr.sun_path, "/tmp/dldp_skb_server");					
	if (unlink(dldp_Serv_Addr.sun_path) != 0) {
		nam_syslog_err("unlink dldp_Serv_Addr fail \n");
	}

	if (bind(adpt_dldp_RxFd, (struct sockaddr *)&dldp_Serv_Addr, sizeof(dldp_Serv_Addr))== -1)
	{
		nam_syslog_err("bind dldp_Serv_Addr fail \n");
		close(adpt_dldp_RxFd);
		return GT_FAIL;
	}
	if(-1 == chmod(dldp_Serv_Addr.sun_path, 0777))
	{	
		/* code optimize:  Unchecked return value from library  houxx@autelan.com  2013-1-20 */
		nam_syslog_err("chmod dldp_Serv_ADDR fail \n");
		close(adpt_dldp_RxFd);
		return GT_FAIL;
	}
	#endif

	return GT_OK;
}


/*******************************************************************************
* nam_packet_rx_socket_init
*
* DESCRIPTION:
*       This method initialize packet receive adapter such as sockets, threads and etc.
*
* INPUTS:
*	NONE
*
* OUTPUTS:
* 	NONE
*
* RETURNS:
*       GT_OK   - on success
*       GT_FAIL - on error
*
* COMMENTS:
*       None
*
*******************************************************************************/
unsigned long nam_packet_rx_adapter_init
(
	GT_VOID
)
{	
	unsigned long	rc;

	rstpSocketCreated = GT_FALSE;
	igmpSnpSockCreated = GT_FALSE;/*added by wujh*/
	dldpSockCreated = GT_FALSE;

	/* mutex used for packet receive and interface modify operation */

	/*if(osSemBinCreate("igmpSockInit",OS_SEMB_EMPTY,&streamSockId) != GT_OK)
	{
		nam_syslog_err("create semaphore %s error\n", "igmpSockInit");
		return GT_FAIL;
	}*/
	pthread_mutex_lock(&streamSockId);

	/*The 3rd Param of function open was 0,only when open a file in CREATE mode, the 3rd param is needed */ 
	if((adptVirRxFd = open("/dev/kap0",O_RDWR))<0)
	{
		nam_syslog_err("open kap %s file descriptor error\n","/dev/kap0");
		return GT_FAIL;
	}
	nam_syslog_dbg("open kap %s file descriptor %d ok\n","/dev/kap0",adptVirRxFd);

	/*	by qinhs@autelan.com 11/03/2008 
	  * 	remove Rx Adapter task for no use any more
	  *   Here we initialize rstp and igmp snooping socket directly.
	  *
	  */
	rc = nam_packet_rx_rstp_socket_init();
	if(GT_OK != rc) {
		nam_syslog_err("initialize receive socket for rstp error %d\n",rc);
		return rc;
	}
	
	rc = nam_packet_rx_igmpsnp_socket_init();
	if(GT_OK != rc) {
		nam_syslog_err("initialize receive socket for igmp snooping error %d\n",rc);
		return rc;
	}

	rc = nam_packet_rx_dldp_socket_init();
	if(GT_OK != rc) {
		nam_syslog_err("initialize receive socket for DLDP error %d\n", rc);
		return rc;
	}

	rc = nam_packet_rx_vrrp_socket_init();
	if(GT_OK != rc) {
		nam_syslog_err("initialize receive socket for DLDP error %d\n", rc);
		return rc;
	}

	return rc;
}


#ifdef __cplusplus
}
#endif

