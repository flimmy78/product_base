#ifndef __NPD_ARP_SNOOPING_H__
#define __NPD_ARP_SNOOPING_H__

#include <sys/socket.h>
#include <linux/netlink.h>

#define NPD_ARPSNOOP_HASH_SIZE	2048
#define NPD_ARPSNOOP_TABLE_SIZE	(4 * 1000)
#define MAC_ADDR_LEN		6
#define IP_ADDR_LEN		4
#define ETHER_IP_TYPE		0x0800
#define ETHER_ARP_TYPE		0x0806

enum NPD_ARPSNOOPING_DB_ACTION {
	ARP_SNOOPING_ADD_ITEM = 0,
	ARP_SNOOPING_DEL_ITEM,
	ARP_SNOOPING_UPDATE_ITEM,
	ARP_SNOOPING_OCCUPY_ITEM,
	ARP_SNOOPING_RESET_ITEM,
	ARP_SNOOPING_ACION_MAX
};

enum NPD_NEXTHOP_DB_ACTION {
	NEXTHOP_ADD_ITEM = 0,
	NEXTHOP_DEL_ITEM,
	NEXTHOP_ACTION_MAX
};

enum NPD_ROUTE_ACTION {
	ROUTE_ADD_ITEM = 0,
	ROUTE_DEL_ITEM,
	ROUTE_UPDATE_ITEM,
	ROUTE_ACTION_MAX
};

enum NPD_PACKET_INTF_SRC_TYPE {
	ARP_INTF_SRC_ETHPORT_E = 0,
	ARP_INTF_SRC_TRUNK_E,
	ARP_INTF_SRC_VIDX_E,
	ARP_INTF_SRC_VID_E,
	ARP_INTF_SRC_MAX
};

enum NPD_ARP_INTF_TYPE{
    ARP_INTF_SIMPLE_VLAN = 0,
    ARP_INTF_SIMPLE_ETH_PORT,
    ARP_INTF_ADVANCED_VLAN,
    ARP_INTF_ADVANCED_ETH_PORT,
    ARP_INTF_RGMII_ETH_PORT,
    ARP_INTF_SUBIF,
    ARP_INTF_QINQ_SUBIF
};
/* ethernet packet layer 2 header*/
struct ether_header_t {
	unsigned char dmac[MAC_ADDR_LEN];
	unsigned char smac[MAC_ADDR_LEN];
	unsigned short	etherType;
};

/* ethenet ARP packet*/
struct arp_packet_t {
	unsigned short 	hwType;				/*hardware type: 0x0001-ethernet*/
	unsigned short 	protocol;			/* protocol type:0x0800-IP*/
	unsigned char 	hwSize;				/* hardware size*/
	unsigned char 	protSize;			/*protocol size*/
	unsigned short 	opCode;				/* 0x0001-request 0x0002-reply*/
	unsigned char  	smac[MAC_ADDR_LEN];	/* sender's MAC address*/
	unsigned char 	sip[IP_ADDR_LEN]; 	/* sender's ip address*/
	unsigned char 	dmac[MAC_ADDR_LEN];	/* target's MAC address*/
	unsigned char 	dip[IP_ADDR_LEN];	/* target's ip address*/
};

/* ethenet packet layer 3 header*/
struct ip_header_t {
	unsigned char	version:4;			/* ip protocol version: 4 for ipv4, 6 for ipv6*/
	unsigned char  	hdrLen:4;			/* ip header length in four-byte unit*/
	unsigned char	dscp:6;				/* Differentiated Services Codepoint (DSCP)*/
	unsigned char	ect:1;				/* ECN-capable Transport*/
	unsigned char 	ecnce:1;			/* ECN-CE*/
	unsigned short	totalLen;			/* total length*/
	unsigned short 	id;					/* identification*/
	unsigned char	flag;				/* don't fragment or more fragment*/
	unsigned char 	fragOffset;		/* fragment offset*/
	unsigned char	ttl;				/* Time To Live*/
	unsigned char  	protocol;			/* ip protocol*/
	unsigned short 	checksum;			/* ip checksum*/
	unsigned char	sip[IP_ADDR_LEN];	/* source ip address*/
	unsigned char  	dip[IP_ADDR_LEN];	/* destination ip address*/
};

/* ethernet ip packet ICMP structure*/
struct icmp_header_t {
	unsigned char	type;				/* icmp type: 8-echo request 0-echo reply*/
	unsigned char 	code;				
	unsigned short 	checksum;			/* icmp packet checksum*/
	unsigned short	id;					/* identification*/
};

struct icmp_packet_t {
	struct icmp_header_t 	header;
	unsigned char			*payload;
};

/* ethernet packet structure*/
struct ip_packet_t {
	struct ip_header_t		layer3;
	unsigned char 			*payload;
};

/* common ethernet packet format*/
struct ethernet_packet_t {
	struct ether_header_t		layer2;
	union {
		struct ip_packet_t 		ip;
		struct arp_packet_t 	arp;
	}payload;
};

#ifndef AX_NEIGH_USER
struct neigh_tbl_info
{
	unsigned char mac[6];
	unsigned int  ipAddr;
	unsigned int  ifIndex;
	unsigned short state;
};
#endif

struct arp_sockaddr {
	unsigned short 	sa_family;	/* address family, AF_xxx	*/
	unsigned char		sa_data[14];	/* 14 bytes of protocol address	*/
};

struct arp_req_s {
  struct arp_sockaddr	arp_pa;		/* protocol address		*/
  struct arp_sockaddr	arp_ha;		/* hardware address		*/
  int			arp_flags;	/* flags			*/
  struct arp_sockaddr    arp_netmask;    /* netmask (only for proxy arps) */
  char			arp_dev[16];
};

struct rtnl_handle
{
	int			fd;
	struct sockaddr_nl	local;
	struct sockaddr_nl	peer;
	unsigned int			seq;
	unsigned int			dump;
};

struct arp_stats{
	unsigned long arp_new;
	unsigned long arp_change;

	unsigned long app_recv;
	unsigned long app_success;
	unsigned long app_bad;
	unsigned long app_neg;
	unsigned long app_suppressed;

	unsigned long kern_neg;
	unsigned long kern_new;
	unsigned long kern_change;

	unsigned long probes_sent;
	unsigned long probes_suppressed;
} ;

/* ARP Flag values. */
#define ATF_COM		0x02		/* completed entry (ha valid)	*/
#define ATF_PERM	0x04		/* permanent entry		*/
#define ATF_PUBL	0x08		/* publish entry		*/
#define ATF_USETRAILERS	0x10		/* has requested trailers	*/
#define ATF_NETMASK     0x20            /* want to use a netmask (only  for proxy entries) */
#define ATF_DONTPUB	0x40	
#define NPD_ARP_SNOOPING_ERR_NONE		0         /**/
#define NPD_ARP_SNOOPING_ERR_NULL_PTR	(NPD_ARP_SNOOPING_ERR_NONE + 1)
#define NPD_ARP_SNOOPING_ERR_DUPLICATED	(NPD_ARP_SNOOPING_ERR_NONE + 2)/**/
#define NPD_ARP_SNOOPING_ERR_NOTEXISTS	(NPD_ARP_SNOOPING_ERR_NONE + 3)/**/
#define NPD_ARP_SNOOPING_ERR_NORESOURCE	(NPD_ARP_SNOOPING_ERR_NONE + 4)
#define NPD_ARP_SNOOPING_ERR_ACTION_TRAP2CPU	(NPD_ARP_SNOOPING_ERR_NONE + 5)
#define NPD_ARP_SNOOPING_ERR_NOTCONSISTENT	(NPD_ARP_SNOOPING_ERR_NONE + 6)
#define NPD_ARP_SNOOPING_ERR_TABLE_FULL	(NPD_ARP_SNOOPING_ERR_NONE + 7)
#define NPD_ARP_SNOOPING_ERR_STATIC_EXIST (NPD_ARP_SNOOPING_ERR_NONE + 8)
#define NPD_ARP_SNOOPING_ERR_STATIC_NOTEXIST	(NPD_ARP_SNOOPING_ERR_NONE + 9)
#define NPD_ARP_SNOOPING_ERR_PORT_NOTMATCH      (NPD_ARP_SNOOPING_ERR_NONE + 10)
#define NPD_ARP_SNOOPING_ERR_KERN_CREATE_FAILED  (NPD_ARP_SNOOPING_ERR_NONE + 11)
#define NPD_ARP_SNOOPING_ERR_ACTION_HARD_DROP  (NPD_ARP_SNOOPING_ERR_NONE + 12)
#define NPD_ARP_SNOOPING_ERR_STATIC_ARP_FULL   (NPD_ARP_SNOOPING_ERR_NONE + 13)
#define NPD_ARP_SNOOPING_ERR_HASH_OP_FAILED    (NPD_ARP_SNOOPING_ERR_NONE + 14)
#define NPD_ARP_SNOOPING_ERR_NAM_ERROR         (NPD_ARP_SNOOPING_ERR_NONE + 101)
#define NPD_ARP_SNOOPING_ERR_UNKNOWN_ERROR     (NPD_ARP_SNOOPING_ERR_NONE + 254)
#define NPD_ARP_SNOOPING_ERR_MAX			(NPD_ARP_SNOOPING_ERR_NONE + 255)

#define NPD_ROUTE_NEXTHOP_ERR_NONE		NPD_ARP_SNOOPING_ERR_NONE
#define NPD_ROUTE_NEXTHOP_ERR_NULL_PTR	NPD_ARP_SNOOPING_ERR_NULL_PTR
#define NPD_ROUTE_NEXTHOP_ERR_DUPLICATED	NPD_ARP_SNOOPING_ERR_DUPLICATED
#define NPD_ROUTE_NEXTHOP_ERR_NOTEXISTS		NPD_ARP_SNOOPING_ERR_NOTEXISTS
#define NPD_ROUTE_NEXTHOP_ERR_NORESOURCE	NPD_ARP_SNOOPING_ERR_NORESOURCE
#define NPD_ROUTE_NEXTHOP_ERR_ACTION_TRAP2CPU	NPD_ARP_SNOOPING_ERR_ACTION_TRAP2CPU
#define NPD_ROUTE_NEXTHOP_ERR_ACTION_HARD_DROP	NPD_ARP_SNOOPING_ERR_ACTION_HARD_DROP
#define NPD_ROUTE_NEXTHOP_ERR_NOTCONSISTENT		NPD_ARP_SNOOPING_ERR_NOTCONSISTENT

#define NPD_ROUTE_HOST_ERR_NONE			NPD_ARP_SNOOPING_ERR_NONE
#define NPD_ROUTE_HOST_ERR_ACTION		(NPD_ROUTE_HOST_ERR_NONE + 5)

#define IP_CONVERT_STR2ULONG(buf) 	\
		((buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]) )
#define SYS_ARP_AGING_TIME  20


typedef enum arp_filter_type{
	ARP_FILTER_NONE,
	ARP_FILTER_IP,
	ARP_FILTER_IFINDEX,
	ARP_FILTER_MAC,
	ARP_FILTER_STATE
} arp_filter_type_e;


/**********************************************************************************
 * npd_route_nexthop_op_item
 *
 * ARP snooping database add/delete or other operations.
 *
 *	INPUT:
 *		item - ARP snooping DB items
 *		action - add or delete operation
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		NPD_ARP_SNOOPING_ERR_NULL_PTR - if input parameters have null pointer
 *		NPD_ARP_SNOOPING_ERR_DUPLICATED - if duplicated items found
 *		NPD_ARP_SNOOPING_ERR_NORESOURCE - if no memory allocatable
 *		NPD_ARP_SNOOPING_ERR_NOTEXISTS - if item not exists in DB
 *		
 *	NOTE:
 *		ARP snooping database can only be modified by this API.
 *		Input arguments is viewed as temporary memory,when 
 *		add		- all hash backet data is newly allocated here.
 *		delete	- memory is returned back to system(or say FREE) here.
 *
 **********************************************************************************/
int npd_route_nexthop_op_item
(
	struct route_nexthop_brief_s *item,
	enum NPD_NEXTHOP_DB_ACTION action,
	unsigned int dupIfIndex
);

/**********************************************************************************
 * npd_route_nexthop_find
 *
 * find route next-hop brief item according to L3 index and ip address
 *
 *	INPUT:
 *		ifIndex - L3 interface index
 *		ipAddr - ip address
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NULL - if no item found
 *		item  - if route nexthop brief item found
 *		
 *	NOTE:
 *
 **********************************************************************************/
struct route_nexthop_brief_s *npd_route_nexthop_find
(
	unsigned int ifIndex,
	unsigned int ipAddr
);

/**********************************************************************************
 * npd_route_nexthop_get_by_tblindex
 *
 * find route next-hop brief item according to HW table index
 *
 *	INPUT:
 *		routeEntryIndex - HW table index
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NULL - if no item found
 *		item  - if route nexthop brief item found
 *		
 *	NOTE:
 *
 **********************************************************************************/
struct route_nexthop_brief_s *npd_route_nexthop_get_by_tblindex
(
	unsigned int routeEntryIndex
);

/**********************************************************************************
 * npd_route_nexthop_tblindex_get
 *
 * find route next-hop brief item according to L3 index and ip address
 *
 *	INPUT:
 *		ifIndex - L3 interface index
 *		ipAddr - ip address
 *	
 *	OUTPUT:
 *		tblIndex - next-hop table index which hold the next-hop item info
 *
 * 	RETURN:
 *		NPD_ROUTE_NEXTHOP_ERR_NONE - if no error occurred.
 *		
 *	NOTE:
 *		if ip address and ifIndex combination is not exists in the hash table, which 
 *		means ip address has not learned before. Occupy a HW table item first, next
 *		time we learned this will re-fill Next-Hop table and ARP mac table
 *
 **********************************************************************************/
unsigned int npd_route_nexthop_tblindex_get
(
	unsigned int ifIndex,
	unsigned int ipAddr,
	unsigned int *tblIndex
);

/**********************************************************************************
 * npd_route_nexthop_tblindex_release
 *
 * 	degree route next-hop reference counter in route next-hop brief item 
 *	according to L3 index and ip address
 *
 *	INPUT:
 *		ifIndex - L3 interface index
 *		ipAddr - ip address
 *		tblIndex - next-hop table index which hold the next-hop item info
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NPD_ROUTE_NEXTHOP_ERR_NONE - if no error occurred.
 *		
 *	NOTE:
 *		if ip address and ifIndex combination is not exists in the hash table, which 
 *		means no route entry referenced this Next-Hop before. 
 *		if table index is not consistent, these may be some error occurred. HW route entry 
 *		may be incorrect.
 *
 **********************************************************************************/
unsigned int npd_route_nexthop_tblindex_release
(
	unsigned int ifIndex,
	unsigned int ipAddr,
	unsigned int tblIndex
);

/**********************************************************************************
 * npd_route_table_op_host_item
 *
 * 	add or delete host route to HW
 *
 *	INPUT:
 *		item - route Next-Hop brief info item to add
 *		action - add or delete or update to HW RT
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL	 - if there's an error
 *
 **********************************************************************************/
int npd_route_table_op_host_item
(
	struct route_nexthop_brief_s *item,
	enum NPD_ROUTE_ACTION		action
);

/**********************************************************************************
 * npd_route_nexthop_iteminfo_get
 *
 * find route next-hop table item from HW according to Next-Hop table index
 *
 *	INPUT:
 *		devNum - device number
 *		tblIndex - HW table index
 *	
 *	OUTPUT:
 *		item - ARP snooping item read from HW
 *	    refCnt - counter for ARP snooping item referenced by route entry
 *
 * 	RETURN:
 *		NPD_ROUTE_NEXTHOP_ERR_NONE - if no error occurred.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_route_nexthop_iteminfo_get
(
	unsigned char devNum,
	unsigned int tblIndex,
	struct arp_snooping_item_s *item,
	unsigned int* refCnt
);

/**********************************************************************************
 * npd_route_nexthop_show_item
 *
 * show route Next-Hop database(Hash table) item detailed info
 *
 *	INPUT:
 *		item - route Next-Hop brief info item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
void npd_route_nexthop_show_item
(
	struct route_nexthop_brief_s *item,
	unsigned char withTitle
);

/**********************************************************************************
 * npd_route_nexthop_show_table
 *
 * show route Next-Hop database(Hash table) items
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
void npd_route_nexthop_show_table
(
	void
);

/**********************************************************************************
 * npd_init_arpsnooping
 *
 * ARP snooping database(Hash table) initialization
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
void npd_init_arpsnooping(void);

/**********************************************************************************
 * npd_arp_snooping_show_item
 *
 * show ARP snooping database(Hash table) item detailed info
 *
 *	INPUT:
 *		item - ARP snooping item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
void npd_arp_snooping_show_item
(
	struct arp_snooping_item_s *item,
	unsigned char withTitle
);

/**********************************************************************************
 * npd_arp_snooping_show_table
 *
 * show ARP snooping database(Hash table) items
 *
 *	INPUT:
 *		NULL
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
void npd_arp_snooping_show_table
(
	void
);

/**********************************************************************************
 * npd_arp_snooping_op_item
 *
 * ARP snooping database add/delete or other operations.
 *
 *	INPUT:
 *		item - ARP snooping DB items
 *		action - add or delete operation
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		NPD_ARP_SNOOPING_ERR_NULL_PTR - if input parameters have null pointer
 *		NPD_ARP_SNOOPING_ERR_DUPLICATED - if duplicated items found
 *		NPD_ARP_SNOOPING_ERR_NORESOURCE - if no memory allocatable
 *		NPD_ARP_SNOOPING_ERR_NOTEXISTS - if item not exists in DB
 *		
 *	NOTE:
 *		ARP snooping database can only be modified by this API.
 *		Input arguments is viewed as temporary memory,when 
 *		add		- all hash backet data is newly allocated here.
 *		delete	- memory is returned back to system(or say FREE) here.
 *
 **********************************************************************************/
int npd_arp_snooping_op_item
(
	struct arp_snooping_item_s *item,
	enum NPD_ARPSNOOPING_DB_ACTION action,
	struct arp_snooping_item_s *dupItem
);

/**********************************************************************************
 * npd_arp_snooping_ip_gateway_check
 *
 * 	ARP snooping check if ip address is compatible with gateway address given by 
 *  	L3 interface index.
 *
 *	INPUT:
 *		ifIndex - L3 interface index
 *		ipAddr - ip address
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL - if error occurred		
 *
 *	NOTE:
 *		ip address should not be gateway address
 *
 **********************************************************************************/
int npd_arp_snooping_ip_gateway_check
(
	unsigned int ipAddr,
	unsigned int * gateway,
	unsigned int * mask
);

/**********************************************************************************
 * npd_arp_snooping_dip_check
 *
 * 	ARP snooping check if ip address is compatible with gateway address given by 
 *  	L3 interface index.
 *
 *	INPUT:
 *		ifIndex - L3 interface index
 *           dip  --  destination ip address
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL - if error occurred		
 *
 *	NOTE:
 *		dip address should be one of gateway address
 *
 **********************************************************************************/
int npd_arp_snooping_dip_check
(
	unsigned int dip,
	unsigned int * gateway
);
/**********************************************************************************
 * npd_arp_snooping_mask32_dip_gateway_check
 *
 * 	ARP snooping check if ip address is compatible with gateway address given by 
 *  	L3 interface index for vrrp or others(the masklen is 32).
 *
 *	INPUT:           
 *           dip  --  destination ip address
 *           gateway  -- the ips of the interface
 *           mask  --  the masks of the interface
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL - if error occurred		
 *
 *	NOTE:
 *		dip address should be one of gateway address and the mask is 32
 *
 **********************************************************************************/
int npd_arp_snooping_mask32_dip_gateway_check
(
	unsigned int dip,
	unsigned int * gateway,
	unsigned int * mask
);

/**********************************************************************************
 * npd_arp_snooping_ip_subnet_check
 *
 * 	ARP snooping check if ip address is compatible with gateway address given by 
 *  	L3 interface index.
 *
 *	INPUT:
 *		ifIndex - L3 interface index
 *		sip  -- source ip address
 *           dip  --  destination ip address
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL - if error occurred		
 *
 *	NOTE:
 *		dip address should be one of gateway address
 *
 **********************************************************************************/
int npd_arp_snooping_ip_subnet_check
(
	unsigned int sip,
	unsigned int dip,
	unsigned int * gateway,
	unsigned int * mask
);

/**********************************************************************************
 * npd_arp_snooping_ip_check
 *
 * ARP snooping check if ip address valid
 *
 *	INPUT:
 *		ipAddr - ip address
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL - if error occurred		
 *
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_ip_check
(
	unsigned int ipAddr,
	unsigned int * gateway,
	unsigned int * mask
);
/*
*/
int npd_arp_snooping_ip_collision_check
(
    unsigned int sip,
    unsigned int * gateway
);


/**********************************************************************************
 * npd_arp_snooping_find_item_bymac
 *
 * find ARP snooping item by mac address
 *
 *	INPUT:
 *		ifIndex - L3 interface index
 *		mac - mac address
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		item - ARP snooping database item
 *
 *	NOTE:
 *
 **********************************************************************************/
struct arp_snooping_item_s *npd_arp_snooping_find_item_bymac
(
	unsigned int  ifIndex,
	unsigned char *mac
);

/**********************************************************************************
 * npd_arp_snooping_learning
 *
 * ARP snooping learning process 
 *	add mac address to ARP mac table,and
 *	add route next-hop table item to Next-Hop Table
 *
 *	INPUT:
 *		item - ARP snooping DB items
 *	
 *	OUTPUT:
 *		nextHopTblIndex - Next-Hop table index which hold the item
 *
 * 	RETURN:
 *		NPD_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		other - if error occurred		
 *
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_learning
(
	struct arp_snooping_item_s *item,
	unsigned int	*nextHopTblIndex,
	unsigned int* dupIfindex
);

/**********************************************************************************
 * npd_arp_snooping_aging_process
 *
 * ARP snooping aging process 
 *	delete mac address from ARP mac table,and
 *	delete route next-hop table item from Next-Hop Table
 *
 *	INPUT:
 *		neighEntry - neighbour table entry brief info items
 *	
 *	OUTPUT:
 *		nextHopTblIndex - Next-Hop table index which hold the item
 *
 * 	RETURN:
 *		NPD_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		other - if error occurred		
 *
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_aging_process
(
	struct neigh_tbl_info *neighEntry
);

/**********************************************************************************
 * npd_arp_packet_rx_process
 *
 *  	process ARP packets and learning arp info
 *
 *	INPUT:
 *		srcType - interface type (0- eth-port 1-trunk 2-vidx 3-vid)
 *		intfId - interface id(eth global index or trunk id)
 *		ifIndex - L3 interface index
 *		vid - vlan id
 *		isTagged - need have 802.1Q tag or not
 *		packet - packet buffer
 *		length - packet length
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		NPD_ARP_SNOOPING_ERR_NULL_PTR - if input parameters have null pointer
 *		NPD_ARP_SNOOPING_ERR_DUPLICATED - if duplicated items found
 *		NPD_ARP_SNOOPING_ERR_NORESOURCE - if no memory allocatable
 *		NPD_ARP_SNOOPING_ERR_NOTEXISTS - if item not exists in DB
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_packet_rx_process
(
	unsigned int   srcType,
	unsigned int   intfId,
	unsigned int   ifIndex,
	unsigned short vid,
	unsigned char  isTagged,
	unsigned char  *packet,
	unsigned long  length
);

/**********************************************************************************
 * npd_arp_filter_by_port
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if ARP item has different port info from data.
 *		1 - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_by_port
(
	struct arp_snooping_item_s *item,
	struct	fdb_interface_info_s *data
);

/**********************************************************************************
 * npd_arp_snooping_filter_by_vid
 *
 * compare two of ARP snooping database(Hash table) items
 *
 *	INPUT:
 *		itemA	- ARP snooping database item
 *		itemB	- ARP snooping database item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 	- if two items are not equal.
 *		1 	- if two items are equal.
 *
 **********************************************************************************/

unsigned int npd_arp_snooping_filter_by_vid_ifIndex
(
	struct arp_snooping_item_s *itemA,
	struct arp_snooping_item_s *itemB
);


/**********************************************************************************
 * npd_count_static_arp_in_port
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *		eth_g_index - eth_port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if there are no static arp items in the eth_port.
 *		1 - if there are static arp items in the eth_port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_count_static_arp_in_port
(
    unsigned int eth_g_index
);

/**********************************************************************************
 * npd_arp_filter_by_port
 *
 *  	Delete ARP snooping info in both sw and hw
 *
 *	INPUT:
 *		item - ARP snooping item info
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		-1  -  if item NULL error occurred.
 *		-2  -  if no sw hash items found.
 *		0  - if delete ok both SW and HW sides
 *		
 *	NOTE:
 *		this procedure treat ARP snooping items found in hash tables, so the 'item' is already found
 *		when calling the procedure, no hash table search done here.
 *
 **********************************************************************************/
int npd_arp_delete
(
	struct arp_snooping_item_s *item,
	unsigned int del_flag
);

/**********************************************************************************
 * npd_arp_clear_by_port
 *
 *  	clear ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_clear_by_port
(
	unsigned int   eth_g_index
);
/**********************************************************************************
 * npd_arp_clear_by_port_ifIndex
 *
 *  	clear ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *           ifIndex - interface index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_all_arp_clear_by_port_ifIndex
(
	unsigned int   eth_g_index,
	unsigned int   ifInex
);

/**********************************************************************************
 * npd_arp_clear_by_port_vid
 *
 *  	clear ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *          vid - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_clear_by_port_vid
(
	unsigned int   eth_g_index,
	unsigned short vid
);

/**********************************************************************************
 * npd_arp_clear_by_vid_ifIndex
 *
 *  	clear static and dynamic arp info in this layer 3 interface
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this interface.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_clear_by_vid_ifIndex
(
	unsigned short   vid,
	unsigned int ifIndex
);

/**********************************************************************************
 * npd_arp_snooping_get_num_byport
 *
 *  	show ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_get_num_byport
(
	unsigned int   eth_g_index
);


/**********************************************************************************
 * npd_arp_snooping_find_item_byport
 *
 *  	find ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_find_item_byport
(
	unsigned int   eth_g_index,
	struct arp_snooping_item_s * arpInfo[],
	unsigned int arrayLen
);


#if 0

/**********************************************************************************
 * npd_arp_snooping_get_num_bytrunk
 *
 *  	show ARP info learned on this trunk
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number on this trunk
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_get_num_bytrunk
(
	unsigned short trunkId
);

/**********************************************************************************
 * npd_arp_snooping_find_item_bytrunk
 *
 *  	find ARP info learned on this trunk
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_find_item_bytrunk
(
	unsigned int   trunkId,
	struct arp_snooping_item_s * arpInfo[],
	unsigned int arrayLen
);

/**********************************************************************************
 * npd_arp_snooping_get_nexthop_item
 *
 *  	find ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *		arrayLen - ARP snooping item array size
 *	
 *	OUTPUT:
 *		item - ARP snooping item array
 *		refCntArray - ARP snooping item reference count array
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_get_nexthop_item
(
	unsigned int eth_g_index,
	struct arp_snooping_item_s *item,
	unsigned int arrayLen,
	unsigned int *refCntArray
);
#endif 

/**********************************************************************************
 * npd_arp_snooping_get_valid_num_byport
 *
 *  	show valid ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		valid ARP items number on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_get_valid_num_byport
(
	unsigned int   eth_g_index
);
/**********************************************************************************
 * npd_valid_arp_filter_by_port_nexthop
 *
 *  	filter valid ARP snooping by eth port
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if ARP item has different port info from data.
 *		1 - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_valid_arp_filter_by_port_nexthop
(
	struct arp_snooping_item_s *item,
	struct	fdb_interface_info_s *data
);

/**********************************************************************************
 * npd_valid_arp_filter_by_trunk
 *
 *  	filter ARP snooping by trunk
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if ARP item has different trunk info from data.
 *		1 - if ARP item has the same trunk info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_valid_arp_filter_by_trunk
(
	struct arp_snooping_item_s *item,
	struct	fdb_interface_info_s *data
);

/**********************************************************************************
 * npd_arp_snooping_get_valid_num_byport_for_nexthop
 *
 *  	show valid ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		valid ARP items number on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_get_valid_num_byport_for_nexthop
(
	unsigned int   eth_g_index
);
/**********************************************************************************
 * npd_arp_snooping_get_valid_num_bytrunk_for_nexthop
 *
 *  	show valid ARP info learned on this trunk
 *
 *	INPUT:
 *		trunkId - trunk ID
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		valid ARP items number on this trunk.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_get_valid_num_bytrunk_for_nexthop
(
	unsigned int   trunkId
);

/**********************************************************************************
 * npd_arp_snooping_find_valid_item_byport
 *
 *  	find ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_find_valid_item_byport
(
	unsigned int   eth_g_index,
	struct arp_snooping_item_s * arpInfo[],
	unsigned int arrayLen
);

/**********************************************************************************
 * npd_arp_snooping_get_num_bytrunk
 *
 *  	show ARP info learned on this trunk
 *
 *	INPUT:
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number on this trunk
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_get_valid_num_bytrunk
(
	unsigned short trunkId
);

/**********************************************************************************
 * npd_arp_snooping_find_item_bytrunk
 *
 *  	find ARP info learned on this trunk
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_find_valid_item_bytrunk
(
	unsigned int   trunkId,
	struct arp_snooping_item_s * arpInfo[],
	unsigned int arrayLen
);

/**********************************************************************************
 * npd_arp_snooping_get_valid_nexthop_item
 *
 *  	find ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *		arrayLen - ARP snooping item array size
 *	
 *	OUTPUT:
 *		item - ARP snooping item array
 *		refCntArray - ARP snooping item reference count array
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_get_valid_nexthop_item
(
	unsigned int eth_g_index,
	struct arp_snooping_item_s *item,
	unsigned int arrayLen,
	unsigned int *refCntArray
);

/**********************************************************************************
 * npd_arp_snooping_del_kern_arp
 *
 *  	del kern ARP info learned 
 *
 *	INPUT:
 *		ipAddr -- global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared in kernel.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_del_kern_arp
(
	struct arp_snooping_item_s *item
);

/**********************************************************************************
 * npd_arp_snooping_get_all_item
 *
 *  	get all hash ARP info learned 
 *
 *	INPUT:
 *		allItem[] -- all arp info
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared in kernel.
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_get_all_item
(
	struct arp_snooping_item_s *allItem[]
);

/**********************************************************************************
 * npd_arp_snooping_sys_timer
 *
 *  	set arp aging time 
 *
 *	INPUT:
 *		ms -- aging time
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 *	NOTE:
 *
 **********************************************************************************/
void npd_arp_snooping_sys_timer
(
	void
);

/**********************************************************************************
 * npd_arp_snooping_del_kern_arp
 *
 *  	del kern ARP info learned 
 *
 *	INPUT:
 *		fd -- socket fd
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_deal
(
	int fd
);

/**********************************************************************************
 * npd_arp_snooping_sync2kern
 *	This method is used to synchronize ARP items to kernel neigh table entry.
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred
 *		-1 - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_sync2kern
(
	void
);

/**********************************************************************************
 * npd_arp_snooping_gratuitous_send
 *	This method is used to send arp solicit packet
 *
 *	INPUT:
 *		item - arp snooping SW hash table items.
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS  - if no error occurred
 *		NPD_FAIL - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_gratuitous_send
(
	unsigned int ifindex,
	unsigned short vid,
	unsigned int eth_g_index,
	unsigned int     intfType
);

/**********************************************************************************
 *  npd_arp_snooping_lookup_arpinfo
 *
 *	DESCRIPTION:
 * 		this routine lookup arp inform in order to send packets
 *
 *	INPUT:
 *		ifindex  -- dev index
 *		vlanId  -- vlan id
 *		pktdata	--  packet data	
 *	
 *	OUTPUT:
 *		isTrunk	- destination is port or trunk
 *		devNum - device number
 *		portOrTrunkNum - port or trunk number 
 *
 * 	RETURN:
 *		NPD_FAIL
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_arp_snooping_lookup_arpinfo
(
	unsigned int ifindex,
	unsigned short vlanId,
	unsigned char* pktdata,
	unsigned int *isTrunk,
	unsigned char* devNum,
	unsigned char* portOrTrunkNum
);


/**********************************************************************************
 * npd_arp_snooping_smac_check
 *	This method is used to compare the source mac and the sender mac address.
 *
 *	INPUT:
 *		etherSmac--source mac address
 *	       arpSmac--sender mac address
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TURE - if no error occurred
 *		NPD_FALSE - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/

int npd_arp_snooping_smac_check
(
	unsigned char* etherSmac,
	unsigned char* arpSmac
);
/**************************************************************
*npd_arp_snooping_mac_legality_check
*  to check the macs of the packet
* INPUT:
*    ethSmac -- source mac
*    ethDmac -- destination mac
*    arpSmac --sender mac
*    arpDmac -- target mac
*    opCode -- opration code
*  RETURN :
*    NPD_FALSE --the macs are illegal
*    NPD_TRUE -- the macs arp legal
******************************************************************/
int npd_arp_snooping_mac_legality_check
(
    char * ethSmac,
    char * ethDmac,
    char * arpSmac,
    char * arpDmac,
    unsigned short opCode
);


/**********************************************************************************
 * npd_arp_snooping_create_kern_static_arp
 *	This routine is used to create kern static arp entry.
 *
 *	INPUT:
 *		ipAddr		-- 	ip address
 *		mac			--	MAC address
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL			 - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_create_kern_static_arp
(
	unsigned int ipAddr,
	unsigned int ipMaskLen,
	unsigned char* mac
);

/**********************************************************************************
 * npd_arp_snooping_create_static_arp
 *	This routine is used to create static arp entry.
 *
 *	INPUT:
 *		ipAddr		-- 	ip address
 *		mac			--	MAC address
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL			 - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
 int npd_arp_snooping_create_static_arp
(
	unsigned int ifindex,
	unsigned int ipAddr,
	unsigned int ipMask,
	unsigned char* mac,
	unsigned short vid,
	unsigned short vid2,
	unsigned int eth_g_index,
	enum NPD_ARP_INTF_TYPE intfType
);

/**********************************************************************************
 * npd_arp_snooping_remove_static_arp
 *	This routine is used to remove static arp entry.
 *
 *	INPUT:
 *		ifindex 		--	dev index
 *		ipAddr		-- 	ip address
 *		ipmask		--	ip mask length
 *		mac			--	MAC address
 *		vid			--	vlan id
 *		eth-g_index	 -- port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL			 - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
 int npd_arp_snooping_remove_static_arp
(
	unsigned int ifindex,
	unsigned int ipAddr,
	unsigned int ipMask,
	unsigned char* mac,
	unsigned short vid,
	unsigned int eth_g_index,
	enum NPD_ARP_INTF_TYPE intfType
);

/**********************************************************************************
 * npd_dbus_save_static_arp
 *	This method is used to save static arp entry.
 *
 *	INPUT:
 *		char* showStr
 *		totalLength
 *		currentLen
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred
 *		-1 - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
 void npd_dbus_save_static_arp
(
	char* showStr,
	int totalLength,
	int* currentLen
);

/**********************************************************************************
 * npd_dbus_save_arp_cfg
 *	This method is used to save static arp config
 *
 *	INPUT:
 *		char* showStr
 *		totalLength
 *		currentLen
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if no error occurred
 *		-1 - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
void npd_dbus_save_arp_cfg
(
	char** showStr,
	int* avalidLen
);

DBusMessage * npd_dbus_set_arp_aging_time
(
	DBusConnection *conn,
	DBusMessage *msg, 
	void *user_data
); 

/**********************************************************************************
 * npd_arp_filter_static_by_port_vid
 *
 *  	filter static arp snooping by eth port
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if ARP item has different port info from data or is not static.
 *		1 - if ARP item has the same port  info as data and is static.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_static_by_port_vid
(
    struct arp_snooping_item_s *itemA,
    struct	arp_snooping_item_s *itemB
);


/**********************************************************************************
 * npd_count_static_arp_in_port_vlan
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *		eth_g_index - eth_port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - if there are no static arp items in the eth_port.
 *		1 - if there are static arp items in the eth_port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_count_static_arp_in_port_vlan
(
    unsigned int eth_g_index,
    unsigned int vlanId
);

/**********************************************************************************
 * npd_arp_filter_by_static
 *
 *  	filter ARP snooping by isStatic
 *
 *	INPUT:
 *		item - ARP snooping item info
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 - find out ARP item not matched
 *		1 - find out ARP item matched(with the same static attribute
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_by_static
(
	struct arp_snooping_item_s *item,
	void* param
);

extern int nam_arp_snooping_op_item
(
	struct arp_snooping_item_s *dbItem,
	unsigned int action,
	unsigned int *tblIndex
);

extern int nam_arp_snooping_get_item
(
	unsigned char devNum,
	unsigned int  tblIndex,
	struct arp_snooping_item_s *val
);

extern int nam_arp_table_index_init
(
	void
);
#if 1
extern unsigned int (*npd_get_devport_by_global_index)
(
	unsigned int eth_g_index,
	unsigned char *devNum,
	unsigned char *portNum
);
#else
extern unsigned int npd_get_devport_by_global_index
(
	unsigned int eth_g_index,
	unsigned char *devNum,
	unsigned char *portNum
);
#endif
extern unsigned long LPM_TBL_HOST_ENTRY_SET
(
	unsigned int DIP,
	unsigned int masklen,
	unsigned int index
);

extern void print_out_mac(unsigned char *mac);

/**********************************************************************************
 * nam_arp_solicit_send
 *	Send arp solicit packet via cpss driver
 * 
 *
 *	INPUT:
 *		item - arp snooping item info
 *		sysMac - system mac address
 *		gateway - L3 interface ip address (as source ip)
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NAM_ARP_SNOOPING_ERR_NULL_PTR - if null pointer found
 *		NAM_ARP_SNOOPING_ERR_NONE - if no error occurred
 *		NAM_ARP_SNOOPING_ERR_GENERAL - if fail to get index number
 *		
 *	NOTE:
 *
 **********************************************************************************/
extern unsigned int nam_arp_solicit_send
(
	struct arp_snooping_item_s *item,
	unsigned char *sysMac,
	unsigned int gateway
);
/****************************************************************
 *npd_arp_snooping_arp_del_by_ip_and_mask
 *		delete arp items by ip and mask 
 * INPUT:
 *		ip - the ip address
 *		mask - the mask value
 *
 * OUTPUT:
 *		NONE
 * RETURN:
 *		ARP_RETURN_CODE_SUCCESS - 
 *
 ****************************************************************/

unsigned int npd_arp_snooping_arp_del_by_ip_and_mask
(
	unsigned int ip,
	unsigned int mask
);


/*********************************************************
 * npd_arp_filter_by_network
 * 
 * INPUT:
 *     itemA  -- arp item 
 *     ip_mask  -- ip_mask[0]  ip address,  ip_mask[1] mask
 * RETURN:
 *     1 -- if matched 
 *     0 -- not matched
 *
 **********************************************************/
unsigned int npd_arp_filter_by_network
(
    struct arp_snooping_item_s * itemA,
    unsigned int *ip_mask
);

int npd_arp_snooping_gratuitous_send_for_new_ipaddr
(
	unsigned int ifindex,
	unsigned int ipAddr,
	unsigned short vid,
	unsigned int eth_g_index,
	unsigned int     intfType
);
/**********************************************************************************
 * npd_static_arp_clear_by_port
 *
 *  	clear ARP info learned and static on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_static_arp_clear_by_port
(
	unsigned int   eth_g_index
);
/**********************************************************************************
 * npd_arp_snooping_find_item_byip_only
 *
 * find ARP snooping item by ip address
 *
 *	INPUT:
 *		ipAddr - ip address
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		item - ARP snooping database item
 *
 *	NOTE:
 *
 **********************************************************************************/
struct arp_snooping_item_s *npd_arp_snooping_find_item_byip_only
(
	unsigned int  ipAddr
);
/**********************************************************************************
 * npd_arp_snooping_create_static_arp_for_trunk
 *	This routine is used to create static arp entry.
 *
 *	INPUT:
 *		ifindex 		--	dev index
 *		ipAddr		-- 	ip address
 *		ipmask		--	ip mask length
 *		mac			--	MAC address
 *		vid			--	vlan id
 *		eth-g_index	 -- port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS - if no error occurred
 *		NPD_FAIL			 - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
 int npd_arp_snooping_create_static_arp_for_trunk
(
	unsigned int ifindex,
	unsigned int ipAddr,
	unsigned int ipMask,
	unsigned char* mac,
	unsigned short vid,
	unsigned int trunkId
);
unsigned int npd_arpsnooping_analyse_mac
(
    unsigned char *sysMac,
    unsigned char *macAddr
);

DBusMessage * npd_dbus_ip_static_arp
( 
    DBusConnection *conn, 
    DBusMessage *msg, 
    void *user_data
);
DBusMessage * npd_dbus_no_ip_static_arp
(   DBusConnection *conn, 
    DBusMessage *msg, 
    void *user_data
);
DBusMessage *npd_dbus_static_arp_show_running
(
    DBusConnection *conn, 
    DBusMessage *msg, 
    void *user_data
);


/**********************************************************************************
 * npd_arp_snooping_static_arp_check_ip_address_by_ifname
 *
 *  DESCRIPTION:
 *          this routine check the layer 3 interface's ip address
 *  INPUT:
 *          ipAddress - the ip address which we want to check
 *          ifname  -   the interface name 
 *  OUTPUT:
 *          ifindex  -  the interface name we got
 *  RETURN:
 *          ARP_RETURN_CODE_NO_HAVE_THE_IP - l3intf no have the ip but have the same sub net ip with ipAddress
 *          ARP_RETURN_CODE_NO_HAVE_ANY_IP - l3intf no have any ip address
 *          ARP_RETURN_CODE_HAVE_THE_IP       - l3intf already have the ip address
 *          ARP_RETURN_CODE_NOT_SAME_SUB_NET - l3intf no have the same sub net ip with ipAddress
 *          ARP_RETURN_CODE_CHECK_IP_ERROR - check ip address error or no have l3intf
 *
 ***********************************************************************************/
int npd_arp_snooping_static_arp_check_ip_address_by_ifname
(
    unsigned int ipAddress,
    unsigned char * ifname,
    unsigned int * ifindex
);

/**********************************************************************************
 * npd_arp_snooping_check_ip_address
 *
 *  DESCRIPTION:
 *          this routine check the layer 3 interface's ip address
 *  INPUT:
 *          ipAddress - the ip address which we want to check
 *          vid          - the vid we want to check in
 *  OUTPUT:
 *          NULL
 *  RETURN:
 *          ARP_RETURN_CODE_NO_HAVE_THE_IP - l3intf no have the ip but have the same sub net ip with ipAddress
 *          ARP_RETURN_CODE_NO_HAVE_ANY_IP - l3intf no have any ip address
 *          ARP_RETURN_CODE_HAVE_THE_IP       - l3intf already have the ip address
 *          ARP_RETURN_CODE_NOT_SAME_SUB_NET - l3intf no have the same sub net ip with ipAddress
 *          ARP_RETURN_CODE_CHECK_IP_ERROR - check ip address error or no have l3intf
 *
 ***********************************************************************************/
int npd_arp_snooping_check_ip_address
(
    unsigned int ipAddress,
    unsigned short vid,
    unsigned int eth_g_index
);

int npd_arpsnooping_create_static_arp_for_intf
(
    unsigned int ifindex,
    unsigned char * ifname,
    unsigned int eth_g_index,
    unsigned int vlanId,
    unsigned int vlanId2,
    unsigned int ipno,
    unsigned int ipmaskLen,
    unsigned char * macAddr,
	enum NPD_ARP_INTF_TYPE intfType
);
/**********************************************************************************
 * npd_arp_filter_by_static_ifindex
 *
 *  	filter ARP snooping by isStatic
 *
 *	INPUT:
 *		item - ARP snooping item info
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE - find out ARP item not matched
 *		TRUE - find out ARP item matched(with the same static attribute
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_by_static_ifindex
(
	struct arp_snooping_item_s *itemA,
	struct arp_snooping_item_s *itemB
);
/**********************************************************************************
 * npd_count_static_arp_by_ifindex
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *	        ifindex - interface index (from kernal)
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		count - the count of static arp 
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_count_static_arp_by_ifindex
(
    unsigned int ifindex
);


#endif
