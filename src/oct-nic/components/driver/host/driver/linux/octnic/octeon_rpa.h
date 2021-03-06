
#ifndef __ETHERNET_RPA_H__
#define __ETHERNET_RPA_H__

#define RPA_SLOT_NUM 0xe
#define RPA_NETDEV_NUM 0x80

#define RPA_ENTER_STACK_ENABLE 1
#define RPA_ENTER_STACK_DISABLE 0

#define ETH_LEN		   16


/* RPA tag bit definitions */
#define RPA_COOKIE            0x8fff
#define RPA_COOKIE_POSITION   12
#define RPA_SRC_SLOT          0x0
#define RPA_DST_SLOT          0x4
#define RPA_SRC_PORT          0x8
#define RPA_DST_PORT          0x10
#define RPA_RESERVE           0x18
#define RPA_TAG_MASK          0xFFFFFFFF
#define RPA_SRC_SLOT_MASK     0xF
#define RPA_DST_SLOT_MASK     0xF0
#define RPA_SRC_PORT_MASK     0xFF00
#define RPA_DST_PORT_MASK     0xFF0000
#define RPA_DST_RESERVE_MASK  0xFF000000

#define VLAN_ETHER_TYPE 0x8100
#define IP_ETHER_TYPE 0x0800

#define DRIVER_NAME     "rpa"
#define DRIVER_VERSION  "v1.0"

/**
 * Command number for ioctl.
 */
#define RPA_IOC_MAGIC  245
#define RPA_IOC_MAXNR  0x16

#define RPA_IOC_REG_IF		_IOWR(RPA_IOC_MAGIC, 1, struct rpa_interface_info) // read values
#define RPA_IOC_UNREG_IF    _IOWR(RPA_IOC_MAGIC, 2, struct rpa_interface_info) // read values
#define RPA_IOC_INDEX_TABLE_ADD _IOWR(RPA_IOC_MAGIC, 3, struct cvm_rpa_dev_index_info)
#define RPA_IOC_INDEX_TABLE_DEL _IOWR(RPA_IOC_MAGIC, 4, struct cvm_rpa_dev_index_info)
#define RPA_IOC_INDEX_TABLE_MDF _IOWR(RPA_IOC_MAGIC, 5, struct cvm_rpa_dev_index_info)
#define RPA_IOC_IF_CHANGE_MTU _IOWR(RPA_IOC_MAGIC, 6, struct rpa_intf_states_update_info_s)
#define RPA_IOC_INDEX_TABLE_SHOW _IOWR(RPA_IOC_MAGIC, 7, unsigned int)

/*for tag*/
#define RPA_ENET_ETHER_ADDR_LEN 6
#define RPA_ENET_DSATAG_LEN 8
#define RPA_ENET_RPATAG_LEN 18
#define RPA_ENET_VLANTAG_LEN 4
#define RPA_ENET_ETHER_TYPE_LEN 2
#define RPATAG_START_POSITION 18
#define ETHER_TYPE_POSITION 20
#define ARP_TYPE_POSITION 36

/**
 * These enumerations are the return codes for the Ethernet
 * driver intercept callback. Depending on the return code,
 * the ethernet driver will continue processing in different
 * ways.
 */
typedef enum
{
    RPA_PASS,               /**< The ethernet driver will pass the packet
                                    to the kernel, just as if the intercept
                                    callback didn't exist */
    RPA_DROP,               /**< The ethernet driver will drop the packet,
                                    cleaning of the work queue entry and the
                                    skbuff */
    RPA_TAKE_OWNERSHIP_WORK,/**< The intercept callback takes over
                                    ownership of the work queue entry. It is
                                    the responsibility of the callback to free
                                    the work queue entry and all associated
                                    packet buffers. The ethernet driver will
                                    dispose of the skbuff without affecting the
                                    work queue entry */
    RPA_TAKE_OWNERSHIP_SKB  /**< The intercept callback takes over
                                    ownership of the skbuff. The work queue
                                    entry and packet buffer will be disposed of
                                    in a way keeping the skbuff valid */
} rpa_callback_result_t;


/**
 * The is the definition of the Ethernet driver intercept
 * callback. The callback receives three parameters and
 * returns a cvm_oct_callback_result_t code.
 *
 * The first parameter is the linux device for the ethernet
 * port the packet came in on.
 * The second parameter is the raw work queue entry from the
 * hardware.
 * Th third parameter is the packet converted into a Linux
 * skbuff.
 */
typedef rpa_callback_result_t (*rpa_callback_t)(struct net_device *dev, struct sk_buff *skb, unsigned int flag);

typedef enum
{
	NOT_RPA_PKT, /* not rpa packet */
	TO_PS_RPA_PKT, /* rpa packet to protocol stack */
	NORMAL_RPA_PKT /* normal rpa packet*/
}rpa_packet_flag;


typedef struct
{
	int flag; /* flag 0 : normal device, flag 1 : rpa_device*/
	int ifindex; /* ifindex of the netdev*/
	struct net_device *netdev;
}cvm_rpa_netdev_index_t;

typedef struct
{
	int netdevNum;
	struct net_device *netdev;
}rpa_arp_send_netdev_t;


/**
 * This is the definition of the rpa-ethernet driver's private
 * driver state stored in dev->priv.
 */
typedef struct {
	struct net_device_stats stats; /* Device statistics */
	unsigned char  sslotNum; /*source slot number*/
    unsigned char  snetdevNum; /*source port number*/
    unsigned char  dslotNum; /*destination slot Num*/
	unsigned char  dnetdevNum; /*destination netdev Num*/
	unsigned int    devNum; /*destination device number of Mavell chip*/
    unsigned int	portNum; /*destination port number of Mavell chip*/
	unsigned int	ddevNum; /* effective when type is 3, the device num of the interface which rpa agents */
	unsigned int	ddev_portNum; /* effective when type is 3, the port num on device of the interface which rpa agents */
	unsigned int    type;/* the type of the interface which rpa agents
						  *0 : virtual interface , 1 : interface on cpu
						  *2 : advanced routing interface from swiching chip , 3 : normal interface for swiching chip
						  */
	//cvm_oct_callback_t      intercept_cb;   /* Optional intecept callback defined above */
} rpa_dev_private_t;


typedef struct {
	unsigned char  ether_dhost[RPA_ENET_ETHER_ADDR_LEN];
	unsigned char  ether_shost[RPA_ENET_ETHER_ADDR_LEN];
    unsigned short  cookie;
	unsigned char  type; /*interface type and packet type
						   *0~3bit : packet type: 1 arp reply packet, 0 other packet type
						   *4~7bit : interface type : 
						   *0 : virtual interface , 1 : interface on cpu
						   *2 : advanced routing interface from swiching chip , 3 : normal interface for swiching chip
						   */
	unsigned char  dnetdevNum;
	unsigned char  snetdevNum;
	unsigned char  d_s_slotNum; /* dst port and src slot number */
}rpa_eth_header;
	
typedef struct rpa_interface_info_s
{
	unsigned char  if_name[ETH_LEN];
	unsigned char  mac_addr[6];
	unsigned int mtu;
	unsigned int   type;/* the type of the interface which rpa agents
						  *0 : virtual interface , 1 : interface on cpu
						  *2 : advanced routing interface from swiching chip , 3 : normal interface for swiching chip
						  */
	unsigned short slotNum;/* the slot num of the interface which rpa agents */
	unsigned short netdevNum;/* the netdev num of the interface in which rpa agents */
	unsigned short devNum;/* effective when type is 3, the device num of the interface which rpa agents */
	unsigned short dev_portNum;/* effective when type is 3, the port num on device of the interface which rpa agents */
}rpa_interface_info_t;

typedef struct cvm_rpa_dev_index_info
{
	int slotNum;
	int netdevNum;
	int flag;
	int ifindex;
}index_table_info;

typedef struct rpa_intf_states_update_info_s
{
	int ifindex;
	int mtu;
	int flags;
}rpa_intf_states_update_info_t;

typedef struct rpa_tx_dev_info_s
{
	int ifindex;
	unsigned char dest_mac_addr[RPA_ENET_ETHER_ADDR_LEN];
}rpa_tx_dev_info_t;

/**
 * enum cvm_oct_callback_result -  Return codes for the Ethernet* driver intercept callback.
 *
 * Depending on the return code, the ethernet driver will continue
 * processing in different ways.
 */
enum cvm_oct_callback_result {
	CVM_OCT_PASS,               /**< The ethernet driver will pass the packet
					to the kernel, just as if the intercept
					callback didn't exist */
	CVM_OCT_DROP,               /**< The ethernet driver will drop the packet,
					cleaning of the work queue entry and the
					skbuff */
	CVM_OCT_TAKE_OWNERSHIP_WORK,/**< The intercept callback takes over
					ownership of the work queue entry. It is
					the responsibility of the callback to free
					the work queue entry and all associated
					packet buffers. The ethernet driver will
					dispose of the skbuff without affecting the
					work queue entry */
	CVM_OCT_TAKE_OWNERSHIP_SKB  /**< The intercept callback takes over
					ownership of the skbuff. The work queue
					entry and packet buffer will be disposed of
					in a way keeping the skbuff valid */
};
typedef enum cvm_oct_callback_result cvm_oct_callback_result_t;
#endif

