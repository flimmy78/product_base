#ifndef __NAM_RX_H__
#define __NAM_RX_H__

#include <sys/types.h>
#include <sys/socket.h>

#define IN
#define OUT
#define INOUT

/*
 *    GT_FALSE - false.
 *    GT_TRUE  - true.
 */
#define GT_FALSE			0
#define GT_TRUE				1
#define GT_OK              (0x00)   /* Operation succeeded */
#define GT_FAIL            (0x01)   /* Operation failed    */

#define GT_BAD_VALUE       (0x02)   /* Illegal value        */
#define GT_OUT_OF_RANGE    (0x03)   /* Value is out of range*/
#define GT_BAD_PARAM       (0x04)   /* Illegal parameter in function called  */
#define GT_BAD_PTR         (0x05)   /* Illegal pointer value                 */
#define GT_BAD_SIZE        (0x06)   /* Illegal size                          */
#define GT_BAD_STATE       (0x07)   /* Illegal state of state machine        */
#define GT_SET_ERROR       (0x08)   /* Set operation failed                  */
#define GT_GET_ERROR       (0x09)   /* Get operation failed                  */
#define GT_CREATE_ERROR    (0x0A)   /* Fail while creating an item           */
#define GT_NOT_FOUND       (0x0B)   /* Item not found                        */
#define GT_NO_MORE         (0x0C)   /* No more items found                   */
#define GT_NO_SUCH         (0x0D)   /* No such item                          */
#define GT_TIMEOUT         (0x0E)   /* Time Out                              */
#define GT_NO_CHANGE       (0x0F)   /* The parameter is already in this value*/
#define GT_NOT_SUPPORTED   (0x10)   /* This request is not support           */
#define GT_NOT_IMPLEMENTED (0x11)   /* This request is not implemented       */
#define GT_NOT_INITIALIZED (0x12)   /* The item is not initialized           */
#define GT_NO_RESOURCE     (0x13)   /* Resource not available (memory ...)   */
#define GT_FULL            (0x14)   /* Item is full (Queue or table etc...)  */
#define GT_EMPTY           (0x15)   /* Item is empty (Queue or table etc...) */
#define GT_INIT_ERROR      (0x16)   /* Error occured while INIT process      */
#define GT_NOT_READY       (0x1A)   /* The other side is not ready yet       */
#define GT_ALREADY_EXIST   (0x1B)   /* Tried to create existing item         */
#define GT_OUT_OF_CPU_MEM  (0x1C)   /* Cpu memory allocation failed.         */
#define GT_ABORTED         (0x1D)   /* Operation has been aborted.           */

/* structure definition begin */
#define IP_ADDR_LEN     4
#define BUFF_LEN		5

/* Ioctl defines */
#define KAPSETNOCSUM  _IOW('T', 210, struct if_cfg_struct) 
#define KAPSETDEBUG   _IOW('T', 211, struct if_cfg_struct) 
#define KAPSETIFF     _IOWR('T', 212, struct if_cfg_struct) 
#define KAPSETPERSIST _IOW('T', 213, struct if_cfg_struct) 
#define KAPSETOWNER   _IOW('T', 214, struct if_cfg_struct)
#define KAPSETLINK    _IOW('T', 215, struct if_cfg_struct)
#define KAPADDIFF     _IOW('T',216,struct if_cfg_struct)
#define KAPWAITING       _IOW('T',217,int)
#define KAPSETIPADDR _IOW('T',218,struct if_cfg_struct)
#define KAPGETIPADDR _IOWR('T',219,struct if_cfg_struct)
#define KAPRETURNPKGNO _IOW('T',220,int)
#define KAPSETDEVINFO  _IOWR('T',221,struct if_cfg_struct)
#define KAPGETDEVINFO _IOWR('T',225,struct if_cfg_struct)
#define KAPSETMACADDR _IOW('T',222,struct if_cfg_struct)
#define KAPGETMACADDR _IOWR('T',223,struct if_cfg_struct)
#define KAPDELIFF _IOWR('T',224,struct if_cfg_struct)
#define KAPGETIPADDRS _IOWR('T',226,struct if_cfg_struct)
#define KAPRESETMAC _IOWR('T',229,struct if_cfg_struct)

#define MAX_IP_COUNT 8
#define KAP_MAXNR	255


/* KAPSETIFF if flags */
#define KAP_IFF_TUN		0x0001
#define KAP_IFF_TAP		0x0002
#define KAP_IFF_NO_PI	0x1000
#define KAP_IFF_ONE_QUEUE	0x2000


/* MACRO definition begin */
#define ADAPTER_RX_PACKET_LEN_MAX 		    1536
#define ADAPTER_RX_PACKET_BUFF_NUM_MAX   	5
#define SOCK_PATH 		  					"/tmp/rstp_socket"

#define	DLDP_SKB_SOCK_PATH					"/tmp/dldp_skb.sock"


/* structure definition begin */
#define ETH_ALEN 		 6
#define IP_ADDR_LEN     4
#define MAC_ADDRESS_LEN 6
#define IGMP_SNOOPING_BUFLEN 	128/*fill in the blank in packet buff */ /*1024*/
#define MLD_SNOOPING_BUFLEN 	IGMP_SNOOPING_BUFLEN

typedef struct  
{
	unsigned char		dmac[ETH_ALEN];		/* destination eth addr	*/
	unsigned char		smac[ETH_ALEN];		/* source ether addr	*/
	unsigned short		etherType;
}ether_header_t;

typedef struct
{
	unsigned short 		tpid; 	/* vlan tagged packet TPID value */
	unsigned short		pri:3,	/* 802.1Q vlan priority */
						cfi:1,  /* 802.1Q CFI */
						vid:12;	/* 802.1Q VID */
}ether_8021q_t;

typedef struct
{
#ifdef LE_HOST
unsigned char	   hdrLength:4,
				   version:4;
unsigned char	   ecn:2,
				   dscp:6;
unsigned short	   totalLen;
unsigned short	   identifier;
unsigned short	   flag:3;
unsigned short	   fragOffset:13;
unsigned char	   ttl;
unsigned char	   ipProtocol;
unsigned short	   checkSum;
unsigned char	   sip[IP_ADDR_LEN];
unsigned char	   dip[IP_ADDR_LEN];

#else
    unsigned char      version:4,
				       hdrLength:4;
    unsigned char      dscp:6;
    unsigned char      ecn:2;
    unsigned short     totalLen;
    unsigned short     identifier;
    unsigned short     flag:3;
    unsigned short     fragOffset:13;
    unsigned char      ttl;
    unsigned char      ipProtocol;
    unsigned short     checkSum;
    unsigned char      sip[IP_ADDR_LEN];
    unsigned char      dip[IP_ADDR_LEN];
#endif
}ip_header_t;

/*
 *	NextHeader field of IPv6 header
 */

#define NEXTHDR_HOP			0	/* Hop-by-hop option header. */
#define NEXTHDR_TCP			6	/* TCP segment. */
#define NEXTHDR_UDP			17	/* UDP message. */
#define NEXTHDR_IPV6		41	/* IPv6 in IPv6 */
#define NEXTHDR_ROUTING		43	/* Routing header. */
#define NEXTHDR_FRAGMENT	44	/* Fragmentation/reassembly header. */
#define NEXTHDR_ESP			50	/* Encapsulating security payload. */
#define NEXTHDR_AUTH		51	/* Authentication header. */
#define NEXTHDR_ICMP		58	/* ICMP for IPv6. */
#define NEXTHDR_NONE		59	/* No next header */
#define NEXTHDR_DEST		60	/* Destination options header. */
#define NEXTHDR_PIMV2		0x67/* PIMv2 header */

#define NEXTHDR_MAX		255

/*
 *	ICMPv6 type field of ICMPv6 header
 */
#define ICMPV6_MGM_QUERY		130
#define ICMPV6_MGM_REPORT       131
#define ICMPV6_MGM_REDUCTION    132

#define ICMPV6_MLD2_REPORT		143

#define icmp6_is_mldv1(type) 	\
	((ICMPV6_MGM_QUERY == (type))|| \
	 (ICMPV6_MGM_REPORT == (type))|| \
	 (ICMPV6_MGM_REDUCTION == (type)) \
	 )

#define icmp6_is_mldv2(type) 	\
	((ICMPV6_MGM_QUERY == (type))|| \
	 (ICMPV6_MGM_REDUCTION == (type))|| \
	 (ICMPV6_MLD2_REPORT == (type)) \
	 )

#define icmp6_is_mld(type) 		\
	( icmp6_is_mldv1(type) || icmp6_is_mldv2(type))
/*
  *	IPv6 address structure
  */

typedef struct
{
	union 
	{
		unsigned char		u6_addr8[16];
		unsigned short		u6_addr16[8];
		unsigned int		u6_addr32[4];
	} in6_u;
#define s6_addr			in6_u.u6_addr8
#define s6_addr16		in6_u.u6_addr16
#define s6_addr32		in6_u.u6_addr32
}in6_addr;

/*
  *	IPv6 fixed header
  *  BEWARE, it is incorrect. The first 4 bits of flow_lbl
  *  are glued to priority now, forming "traffic class".
  */
typedef struct {
#ifdef LE_HOST
	unsigned char	priority:4,
					version:4;
#else
	unsigned char	version:4,
					priority:4;
#endif
	unsigned char	flow_lbl[3];
	unsigned short	payload_len;
	unsigned char	nexthdr;
	unsigned char	hop_limit;
	in6_addr	saddr;
	in6_addr	daddr;
}ipv6_header_t;

/* IPv6 option header */
typedef struct {
	unsigned char 	nexthdr;
	unsigned char	hdrlen;
	/* 
	 * TLV encoded option data follows.
	 */
}ipv6_opt_hdr;
#define ipv6_optlen(p)  (((p)->hdrlen+1) << 3)

/* PIMv2 header */
typedef struct {
	unsigned char version:4,
				  type:4;
	unsigned char resv;
	unsigned short checksum;
	/*
	 * PIM parameters
	 */
}pimv2_header;

/* ICMPv6 header */
typedef struct {
	unsigned char	icmp6_type;
	unsigned char	icmp6_code;
	unsigned short	icmp6_cksum;

	union {
		unsigned int	un_data32[1];
		unsigned short	un_data16[2];
		unsigned char	un_data8[4];

		struct icmpv6_echo {
			unsigned short	identifier;
			unsigned short	sequence;
		} u_echo;

        struct icmpv6_nd_advt {
		#ifdef LE_HOST
			unsigned int	reserved:5,
                        	override:1,
                        	solicited:1,
                        	router:1,
							reserved2:24;
		#else
			unsigned int	router:1,
							solicited:1,
                        	override:1,
                        	reserved:29;
		#endif
		} u_nd_advt;

		struct icmpv6_nd_ra {
			unsigned char	hop_limit;
		#ifdef LE_HOST
			unsigned char	reserved:6,
							other:1,
							managed:1;

		#else
			unsigned char	managed:1,
							other:1,
							reserved:6;
		#endif
			unsigned short		rt_lifetime;
		} u_nd_ra;
	}icmp6_dataun;
} icmp6_header;

/* ARP header */
typedef struct {
	unsigned short 	hwType;				/* hardware type: 0x0001-ethernet */
	unsigned short 	protType;			/* protocol type:0x0800-IP */
	unsigned char 	hwSize;				/* hardware size */
	unsigned char 	protSize;			/* protocol size */
	unsigned short 	opCode;				/* 0x0001-request 0x0002-reply */
	unsigned char  	smac[MAC_ADDRESS_LEN];/* sender's MAC address */
	unsigned char 	sip[IP_ADDR_LEN];	/* sender's ip address */
	unsigned char 	dmac[MAC_ADDRESS_LEN];/* target's MAC address */
	unsigned char 	dip[IP_ADDR_LEN];	/* target's ip address */
}arp_header_t;

typedef struct
{
	unsigned char	type;
	unsigned char	code;
	unsigned short	checksum;
	unsigned long	groupaddr;
}igmp_header_t;

typedef struct 
{
	unsigned short source;
	unsigned short	dest;
	unsigned short len;
	unsigned short check;
}udp_header_t;

typedef struct
{
	unsigned short	source;
	unsigned short	dest;
	unsigned short	seq;
	unsigned short	ack_seq;
	unsigned short	doff:4,
					res1:4,
					cwr:1,
					ece:1,
					urg:1,
					ack:1,
					psh:1,
					rst:1,
					syn:1,
					fin:1;
	unsigned short	window;
	unsigned short	check;
	unsigned short	urg_ptr;
}tcp_header_t;

enum NAM_PACKET_TYPE_ENT {
	NAM_PACKET_TYPE_BPDU_E = 0,
	NAM_PACKET_TYPE_GVRP_E,
	NAM_PACKET_TYPE_ARP_E,
	NAM_PACKET_TYPE_IPv4_E,
	NAM_PACKET_TYPE_IP_ICMP_E,
	NAM_PACKET_TYPE_IP_TCP_E,
	NAM_PACKET_TYPE_IP_UDP_E,
	NAM_PACKET_TYPE_IP_IGMP_E,
	NAM_PACKET_TYPE_IP_TCP_TELNET_E,
	NAM_PACKET_TYPE_IP_TCP_SSH_E,
	NAM_PACKET_TYPE_IP_TCP_FTP_E,
	NAM_PACKET_TYPE_IP_TCP_FTP_DATA_E,
	NAM_PACKET_TYPE_IP_UDP_DHCP_E,
	NAM_PACKET_TYPE_IP_IPIP_E,
	NAM_PACKET_TYPE_DLDP_E,
	NAM_PACKET_TYPE_VRRP_E,
	NAM_PACKET_TYPE_IPv6_E,
	NAM_PACKET_TYPE_IP6_MLD_E,
	NAM_PACKET_TYPE_ISIS_E,
	NAM_PACKET_TYPE_LACP_E,
	NAM_PACKET_TYPE_OTHER
};

enum NAM_INTERFACE_TYPE_ENT{
    NAM_INTERFACE_PORT_E = 0,
    NAM_INTERFACE_TRUNK_E,
    NAM_INTERFACE_VIDX_E,
    NAM_INTERFACE_VID_E
};

/*****RSTP BPDU PKT********/
#define MAX_BPDU_LEN    1200
typedef struct 
{
	unsigned int 		intfId;
	unsigned int		pktLen;
	unsigned char*    	pktData;
}BPDU_PKT_PARAM_STC;

/*****IGMP SNOOP PKT******/

/*IGMP Snoop kernel message type*/
#define	IGMP_SNP_TYPE_MIN				0
#define	IGMP_SNP_TYPE_NOTIFY_MSG		1	/* IGMP notify message*/
#define	IGMP_SNP_TYPE_PACKET_MSG		2	/* IGMP Packet message*/
#define IGMP_SNP_TYPE_DEVICE_EVENT		3	/* IGMP device message*/
#define MLD_SNP_TYPE_NOTIFY_MSG			4   /* MLD notfiy message */
#define MLD_SNP_TYPE_PACKET_MSG			5	/* MLD packet message */
#define MLD_SNP_TYPE_DEVICE_EVENT		6	/* MLD device message */
#define	IGMP_SNP_TYPE_MAX				9

/*IGMP Snoop message flag*/
#define	IGMP_SNP_FLAG_MIN				0
#define	IGMP_SNP_FLAG_PKT_UNKNOWN		1	/*Unknown packet*/
#define	IGMP_SNP_FLAG_PKT_IGMP			2	/*IGMP,PIM packet*/
#define	IGMP_SNP_FLAG_ADDR_MOD			3	/*notify information for modify address*/
#define	MLD_SNP_FLAG_PKT_UNKNOWN		4	/* MLD Unknown packet*/
#define	MLD_SNP_FLAG_PKT_MLD			5	/* MLD packet*/
#define	MLD_SNP_FLAG_ADDR_MOD			6	/* MLD notify information for modify address*/
#define	IGMP_SNP_FLAG_MAX				9	

struct nlmsghdr
{
	unsigned int		nlmsg_len;		/* Length of message including header */
	unsigned short		nlmsg_type;		/* Message content */
	unsigned short		nlmsg_flags;	/* Additional flags */
	unsigned int		nlmsg_seq;		/* Sequence number */
	unsigned int		nlmsg_pid;		/* Sending process PID */
};

typedef struct
{
	struct nlmsghdr		nlh;
	unsigned int 		ifindex;
	unsigned int 		vlanid;
	unsigned char 		buf[IGMP_SNOOPING_BUFLEN];
}IGMP_SNP_PARAM_STC;

/*************************************************************/
/* for DLDP															*/
#define	NPD_DLDP_TYPE_PACKET_MSG	(2)				/*Packet message				*/

#define DLDP_BUFLEN 				(128)			/*fill in the blank in packet buff	*/
typedef struct
{
	struct nlmsghdr		nlh;
	unsigned int 		ifindex;
	unsigned int 		tagmode;
	unsigned int 		vlanid;
	unsigned char 		buf[DLDP_BUFLEN];
}DLDP_PARAM_STC;
/*************************************************************/

/*****RSTP BPDU PKT********/
#define MAX_VRRP_LEN    1200
#define IPPROTO_VRRP	112	/* IP protocol number -- rfc2338.5.2.4*/

typedef struct 
{
	unsigned int ifindex;
	unsigned int pktLen;
	char*    	pktData;
}VRRP_PKT_PARAM_STC;

typedef struct
{
	enum NAM_INTERFACE_TYPE_ENT dev_type;
	unsigned int l3_index;
	unsigned int l2_index;
	unsigned int vId;
	unsigned int data_len;		
	unsigned int data_addr;	
}VIRT_PKT_INFO_STC;

#ifdef DRV_LIB_CPSS

typedef struct packetInfoParam
{
	GT_U32		buffsNum;
	GT_U8* 		buffPtr[BUFF_LEN];
	GT_U32  	buffLength[BUFF_LEN];
	CPSS_DXCH_NET_RX_PARAMS_STC	rxRxParamPtr;
}PKT_INFO_PARAM;
#endif


typedef enum link_status{
	DEV_LINK_DOWN = 0,
	DEV_LINK_UP,
	DEV_LINK_MAX
}KAP_DEV_LINK_STATUS;

typedef struct if_cfg_struct
{
#define IFHWADDRLEN	6

	union
	{
		char	if_name[16];		/* if name, e.g. "en0" */
	} if_ifn;
	
	union {
		struct	sockaddr ifu_addr[MAX_IP_COUNT];
		struct	sockaddr ifu_dstaddr;
		struct	sockaddr ifu_broadaddr;
		struct	sockaddr ifu_netmask;
		struct  sockaddr ifu_hwaddr;

		unsigned int	ifu_ivalue;
		unsigned int	ifu_flag;
	}if_ifu;

	enum NAM_INTERFACE_TYPE_ENT dev_type;
	KAP_DEV_LINK_STATUS dev_state;
	unsigned int l3_index;
	unsigned int l2_index;
	unsigned int vId;
	unsigned int netmask[MAX_IP_COUNT];

	unsigned char mac_addr[6];

} VIRT_DEVICE_PARAM;

#define	if_name	if_ifn.if_name	/* interface name 	*/
#define	if_hwaddr	if_ifu.ifu_hwaddr	/* MAC address 		*/
#define	if_addr	if_ifu.ifu_addr	/* address		*/
#define	if_dstaddr	if_ifu.ifu_dstaddr	/* other end of p-p lnk	*/
#define	if_broadaddr	if_ifu.ifu_broadaddr	/* broadcast address	*/
#define	if_netmask	if_ifu.ifu_netmask	/* interface net mask	*/
#define	if_flag	if_ifu.ifu_flag	/* flags		*/
#define	if_owner if_ifu.ifu_ivalue /*set dev owner*/
#define	if_type if_ifu.ifu_flag      /*set dev type*/
#define	if_debug if_ifu.ifu_ivalue   /*set dev debug*/
/* structure definition end */

#ifdef DRV_LIB_CPSS
extern int osPrintf
(
	const char* format, ...
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
extern int osPrintfPktRx
(
	const char* format,
	...
);
void *osMalloc
(
    IN unsigned long size
);
extern void osFree
(
    IN void* const memblock
);
extern void * osMemCpy
(
    IN void *       destination,
    IN const void * source,
    IN unsigned long       size
);
extern void * osMemSet
(
    IN void * start,
    IN int    symbol,
    IN unsigned long size
);
extern GT_STATUS osSemBinCreate
(
	IN	char		  *name,
	IN	GT_SEMB_STATE init,
	OUT GT_SEM		  *smid
);

extern unsigned long cpssDxChPacketRxGetEthIndexViaDSA
(
     IN   unsigned char					device,
     IN   CPSS_DXCH_NET_RX_PARAMS_STC   *rxParams,
     OUT  CPSS_INTERFACE_TYPE_ENT 		*type,
     OUT  unsigned int					*intfId
);
#else
#define osPrintf 		nam_syslog_dbg
#define osPrintfErr 	nam_syslog_err
#define osPrintfDbg 	nam_syslog_dbg
#define osPrintfPktRx 	nam_syslog_dbg
#define osMalloc 	malloc
#define osFree 		free
#define osMemCpy 	memcpy
#define osMemSet 	memset

extern void *
soc_cm_salloc
(
	int dev, 
	int size, 
	const char *name
);

extern void
soc_cm_sfree
(
	int dev, 
	void *ptr
);
#endif

extern char *namPacketTypeStr[];
#if 1
extern unsigned int (*npd_get_global_index_by_devport)
(
	IN		unsigned char devNum,
	IN		unsigned char portNum,
	OUT 	unsigned int *eth_g_index
);
#else
extern unsigned int npd_get_global_index_by_devport
(
	IN		unsigned char devNum,
	IN		unsigned char portNum,
	OUT 	unsigned int *eth_g_index
);
#endif
extern int npd_check_vlan_status
(
	unsigned short vlanId
);
extern void npd_get_port_admin_status
(
	unsigned int eth_g_index,
	unsigned int *status
);
extern unsigned int npd_get_port_link_status
(
	unsigned int eth_g_index,
	unsigned int *status
);
extern unsigned int npd_vlan_check_contain_port
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  *isTagged
);
extern unsigned int npd_vlan_interface_check
(
	unsigned short vlanId,
	unsigned int   *ifIndex
);
extern unsigned int npd_eth_port_interface_check
(
	unsigned int	eth_g_index,
	unsigned int   *ifIndex
);
extern int npd_arp_packet_rx_process
(
	unsigned int   srcType,
	unsigned int   intfId,
	unsigned int   ifIndex,
	unsigned short vid,
	unsigned char  isTagged,
	unsigned char  *packet,
	unsigned long  length
);
extern int npd_check_port_promi_mode
(
	unsigned int eth_g_index
);
extern unsigned int npd_eth_port_get_ptrunkid
(
	unsigned int   eth_g_index,
	unsigned short *trunkId
);

extern enum NAM_PACKET_TYPE_ENT nam_packet_parse_type 
(
	IN unsigned char  *packet
);


/**********************************************************************************
 * npd_dhcp_snp_packet_rx_process()
 *	DESCRIPTION:
 *		receive packets for DHCP Snooping rx 
 *
 *	INPUTS:
 *		unsigned long numOfBuff		- number of buffer
 *		unsigned char *packetBuffs[]	- array of buffer
 *		unsigned long buffLen[]			- array of buffer length
 *		unsigned int interfaceId			- port Or TrunkId has been transfer to eth_g_index
 *		unsigned char isTagged			- tag flag of port
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
extern unsigned int npd_dhcp_snp_packet_rx_process
(
	unsigned long numOfBuff,
	unsigned char *packetBuffs[],
	unsigned long buffLen[],
	unsigned int ifindex,
	unsigned char isTagged,
	unsigned short vid
);


/* local functions declearations begin */

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
unsigned char	nam_packet_type_is_BPDU
(       
	IN unsigned char  *packetBuff
);

long nam_igmpSnp_rx_packet
(
    IN unsigned char       	devNum,
    IN unsigned char      	queueIdx,
    IN unsigned long     	numOfBuff,
    IN unsigned char      	*packetBuffs[],
    IN unsigned long      	buffLen[],
    IN unsigned int 		interfaceId,/*portOrTrunkId  has been transfer to eth_g_index*/
    IN unsigned short		vid
);

long nam_mldSnp_rx_packet
(
    IN unsigned char       	devNum,
    IN unsigned char      	queueIdx,
    IN unsigned long     	numOfBuff,
    IN unsigned char      	*packetBuffs[],
    IN unsigned long      	buffLen[],
    IN unsigned int 		interfaceId,/*portOrTrunkId  has been transfer to eth_g_index*/
    IN unsigned short		vid
);

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
);


#endif

