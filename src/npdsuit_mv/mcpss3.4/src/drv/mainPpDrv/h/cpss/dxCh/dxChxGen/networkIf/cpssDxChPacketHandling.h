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
*cpssDxChPacketHandling.h
*
*MODIFY:
*	by <username@autelan.com> on MM/DD/YYYY revision <x.x.x..>
*
*CREATOR:
*	<username@autelan.com>
*
*DESCRIPTION:
*<some description about this file>
*
*DATE:
*	11/14/2007	
*	
*******************************************************************************/
#ifndef __ADPT_RX_HANDLING_H_  
#define __ADPT_RX_HANDLING_H_ 

/* include header file begin */
/* kernel or sys part */
#include <sys/types.h>
#include <sys/socket.h>

/* user or app part */

/* include header file end */

/* MACRO definition begin */
#ifndef BUFF_LEN
#define 	BUFF_LEN	5
#endif

#ifndef MIN
#define MIN(a,b) ( (a)<(b) ? (a):(b) ) 
#endif

#ifndef __AX_PLATFORM__
enum CPSS_PACKET_TYPE_ENT {
	CPSS_PACKET_TYPE_BPDU_E = 0,
	CPSS_PACKET_TYPE_GVRP_E,
	CPSS_PACKET_TYPE_ARP_E,
	CPSS_PACKET_TYPE_IPv4_E,
	CPSS_PACKET_TYPE_IP_ICMP_E,
	CPSS_PACKET_TYPE_IP_TCP_E,
	CPSS_PACKET_TYPE_IP_UDP_E,
	CPSS_PACKET_TYPE_IP_IGMP_E,	/*Added by wujh*/
	CPSS_PACKET_TYPE_IP_TCP_TELNET_E,
	CPSS_PACKET_TYPE_IP_TCP_SSH_E,
	CPSS_PACKET_TYPE_IP_TCP_FTP_E,
	CPSS_PACKET_TYPE_IP_TCP_FTP_DATA_E,
	CPSS_PACKET_TYPE_IP_UDP_DHCP_E,
	CPSS_PACKET_TYPE_IPIP_E,
	CPSS_PACKET_TYPE_DLDP_E,
	CPSS_PACKET_TYPE_VRRP_E,
	CPSS_PACKET_TYPE_IPv6_E,
	CPSS_PACKET_TYPE_IP6_MLD_E,
	CPSS_PACKET_TYPE_OTHER
};


/* CPU Rx packet traffic class */
#define CPU_PORT_BPDU_QUEUE			7
#define CPU_PORT_VRRP_QUEUE			7  	/*VRRP - 224.0.0.18 */

/* OAM traffic */
#define CPU_PORT_OAM_QUEUE		6
#define CPU_PORT_TELNET_QUEUE		CPU_PORT_OAM_QUEUE /* TCP dport 23 */
#define CPU_PORT_SSH_QUEUE		CPU_PORT_OAM_QUEUE /* TCP dport 22 */
#define CPU_PORT_HTTP_QUEUE		CPU_PORT_OAM_QUEUE /* TCP dport 80 */
#define CPU_PORT_HTTPS_QUEUE		CPU_PORT_OAM_QUEUE /* TCP dport 443 */
#define CPU_PORT_SNMP_QUEUE		CPU_PORT_OAM_QUEUE /* UDP dport 161 */

/* Unicast routing protocol traffic */
#define CPU_PORT_RIPv1_QUEUE		5
#define CPU_PORT_RIPv2_QUEUE		5	/* RIPv2 router - 224.0.0.9 / FF02::9 */
#define CPU_PORT_OSPFv1_QUEUE		5
#define CPU_PORT_OSPFv2_QUEUE		5	
#define CPU_PORT_OSPFDSG_QUEUE 	5 	/*ospf designated routers - 224.0.0.6 / FF02::6 */

/* Multicast routing protocol traffic */
#define CPU_PORT_PIM_QUEUE		4  	/*All PIM Routers - 224.0.0.13 / FF02::D */

/* Wireless control/data plane and user auth traffic */
#define CPU_PORT_CAPWAP_QUEUE		3	
#define CPU_PORT_8021x_QUEUE		3

/* Group Management Protocol */
#define CPU_PORT_DHCPREQ_QUEUE	2
#define CPU_PORT_DHCPRPL_QUEUE	2
#define CPU_PORT_ALL_MC_QUEUE 	2  	/*all system on the subnet - 224.0.0.1 / FF02::1 */
#define CPU_PORT_ALL_ROUTERS_QUEUE 	2 	/*All Routers on this subnet - 224.0.0.2 / FF02::2 */
#define CPU_PORT_IGMP_QUEUE		2

/* ARP packet */
#define CPU_PORT_ARP_BC_QUEUE		1  	/* arp request */
#define CPU_PORT_ARP_UC_QUEUE		1 	/* arp reply to me */

#define CPU_PORT_DEFAULT_QUEUE	0
#define CPU_PORT_ROUTE_QUEUE		CPU_PORT_DEFAULT_QUEUE	/* packet routed to CPU */
#define CPU_PORT_BRIDGE_QUEUE		CPU_PORT_DEFAULT_QUEUE	/* packet bridged to CPU(ARP reply) */
#define CPU_PORT_TCPSYN_QUEUE		CPU_PORT_DEFAULT_QUEUE 	/* TCP SYN packet */

/* CPU Tx packet traffic class */
/* send packet via sdma queue 7 */
#define CPU_TX_BPDU_QUEUE			CPU_PORT_BPDU_QUEUE
#define CPU_TX_VRRP_QUEUE			CPU_PORT_VRRP_QUEUE

/* send packet via sdma queue 6 */
#define CPU_TX_TELNET_QUEUE		CPU_PORT_TELNET_QUEUE	/* TCP dport 23 */
#define CPU_TX_SSH_QUEUE		CPU_PORT_SSH_QUEUE		/* TCP dport 22 */
#define CPU_TX_HTTP_QUEUE		CPU_PORT_HTTP_QUEUE		/* TCP dport 80 */
#define CPU_TX_HTTPS_QUEUE		CPU_PORT_HTTPS_QUEUE	/* TCP dport 443 */
#define CPU_TX_SNMP_QUEUE		CPU_PORT_SNMP_QUEUE		/* UDP dport 161 */

/* send packet via sdma queue 5 */
#define CPU_TX_RIPv1_QUEUE		CPU_PORT_RIPv1_QUEUE
#define CPU_TX_RIPv2_QUEUE		CPU_PORT_RIPv2_QUEUE
#define CPU_TX_OSPFv1_QUEUE		CPU_PORT_OSPFv1_QUEUE
#define CPU_TX_OSPFv2_QUEUE		CPU_PORT_OSPFv2_QUEUE

/* send packet via sdma queue 3 */
#define CPU_TX_CAPWAP_QUEUE		CPU_PORT_CAPWAP_QUEUE
#define CPU_TX_8021x_QUEUE		CPU_PORT_8021x_QUEUE

/* send packet via sdma queue 2 */
#define CPU_TX_DHCPREQ_QUEUE		CPU_PORT_DHCPREQ_QUEUE
#define CPU_TX_DHCPRPL_QUEUE		CPU_PORT_DHCPRPL_QUEUE
#define CPU_TX_IGMP_QUEUE			CPU_PORT_IGMP_QUEUE

/* send packet via sdma queue 1 */
#define CPU_TX_ARP_BC_QUEUE		CPU_PORT_ARP_BC_QUEUE

/* send packet via sdma queue 0 */
#define CPU_TX_ICMP_QUEUE			CPU_PORT_ROUTE_QUEUE
#define CPU_TX_IPv4_QUEUE			CPU_PORT_DEFAULT_QUEUE
#define CPU_TX_TCP_QUEUE			CPU_PORT_DEFAULT_QUEUE
#define CPU_TX_UDP_QUEUE			CPU_PORT_DEFAULT_QUEUE
#define CPU_TX_DEFAULT_QUEUE		CPU_PORT_DEFAULT_QUEUE
#endif

typedef struct packetInfoParam
{
	GT_U32		buffsNum;
	GT_U8* 		buffPtr[BUFF_LEN];
	GT_U32  	buffLength[BUFF_LEN];
	CPSS_DXCH_NET_RX_PARAMS_STC	rxRxParamPtr;
}PKT_INFO_PARAM;

#ifdef __APP_ICMP_REPLY__
#define ETH_MAC_LEN     6
#define IP_ADDR_LEN     4
#define GT_ICMP_ERR		0x20

typedef struct _ether_header_t_
{
    GT_U8      dmac[ETH_MAC_LEN];
    GT_U8      smac[ETH_MAC_LEN];
    GT_U16     etherType;
}ETH_HEADER_T;

typedef struct _ip_header_t_
{
    GT_U8      version:4;
    GT_U8      hdrLength:4;
    GT_U8      dscp:6;
    GT_U8      ecn:2;
    GT_U16     totalLen;
    GT_U16     identifier;
    GT_U16     flag:3;
    GT_U16     fragOffset:13;
    GT_U8      ttl;
    GT_U8      ipProtocol;
    GT_U16     checkSum;
    GT_U8      sip[IP_ADDR_LEN];
    GT_U8      dip[IP_ADDR_LEN];
}IP_HEADER_T;

typedef struct _icmp_header_t_
{
    GT_U8      type;
    GT_U8      code;
    GT_U16     checkSum;
    GT_U16     identifier;
    GT_U16     sequence;
}ICMP_HEADER_T;

typedef struct _ether_icmp_packet_t_
{
    ETH_HEADER_T    layer2;
    IP_HEADER_T     layer3;
    ICMP_HEADER_T   icmp;
    GT_U8           payload[32];
}ETHER_ICMP_PACKET_T;
#endif

/* structure definition end */

/* local functions declearations begin */

 /*******************************************************************************
 * cpssDxChPacketRxGetEthIndexViaDSA
 *
 * DESCRIPTION:
 *		 This function parse DSA tag and get global ethernet port index
 *
 * INPUTS:
 *		 rxParams - Rx packet params.
 *
 * OUTPUTS:
 *		type	 - interface type
 *		 intfId  -  interface ID such as ethernet port global index or trunk id or vid or vidx
 *
 * RETURNS:
 *		GT_OK	-	if no error occurred
 *		GT_FAIL	-	if error occurred
 *
 * COMMENTS:
 *		 This function is call after packet received.
 *
 *******************************************************************************/
GT_STATUS cpssDxChPacketRxGetEthIndexViaDSA
(
     IN   unsigned char					device,
     IN   CPSS_DXCH_NET_RX_PARAMS_STC   *rxParams,
     OUT  CPSS_INTERFACE_TYPE_ENT 		*type,
     OUT  unsigned int					*intfId
);

/********wujh_start******** 
*dxChTxPortIDGetViaDSATag
*
*DESCRIPTION:
*       This function returns Port ID of the Packet for the Tx process. The DSA Tag Supportted is both Extended Fromat
* and 4-bytes format.
*INPUT:
*       devNum  - Device on which Tx Process 
*       dsaTagPtr - point to the DSA-Tag in the packet Buffer
*RETURN:
*       portID  - Port on which Tx Process, portID including  in the DSA-Tag.
*
*COMMENTS:
*       DSA-Tag in packet of Tx Process is supported all 4 Types of DSA-Tag: TO_CPU ,FORWRAD,FROM_CPU,TO_ANALYZER
*/

GT_STATUS  dxChTxPortIDGetViaDSATag
( 
         IN CPSS_DXCH_NET_RX_PARAMS_STC   rxParams 
);

/*******************************************************************************
* npd_init_tell_whoami
*
* DESCRIPTION:
*      This function is used by each thread to tell its name and pid to NPD_ALL_THREAD_PID_PATH
*
* INPUTS:
*	  tName - thread name.
*	  lastTeller - is this the last teller or not, pid file should be closed if true.
*
* OUTPUTS:
*       None.
*
* RETURNS:
*	  None.
*
* COMMENTS:
*       
*
*******************************************************************************/
extern void npd_init_tell_whoami
(
	unsigned char *tName,
	unsigned char lastTeller
);

/* local functions declearations end */

#endif /* __FILENAME_H_ */
