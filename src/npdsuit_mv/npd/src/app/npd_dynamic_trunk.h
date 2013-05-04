#ifndef __NPD_DYNAMIC_TRUNK_H__
#define __NPD_DYNAMIC_TRUNK_H__

#define POINTER_IS_NULL(pointer)		(NULL == pointer)

#define MIN_DYNAMIC_TRUNKID		0x1		/* min dynamic trunkid */
#define MAX_DYNAMIC_TRUNKID		0x8		/* max dynamic trunkid , and is the max dynamic trunk count */
#define MAX_DYNAMIC_TRUNK_MEMBER (0x8)  /*max port count of one dynamic trunk*/


#define MAX_PAYLOAD 1024
#define GRP_ID  RTNLGRP_LINK

/* netlink flag */
#define DYNAMIC_TRUNK_3AD_CHANE_IF_MSG 0x20070528
/* the dynamic trunk's mode 802.3ad*/
#define DYNAMIC_TRUNK_MODE_802_3AD	4

#define DYNAMIC_TRUNK_PROC_NET_TRUNKING_PATH "/proc/net/trunking"
#define DYNAMIC_TRUNK_TRUNKING_IFNAME_PREFIX "trunk"
#define DYNAMIC_TRUNK_TRUNK_NAME_PREFIX  "dytrunk"
#define DYNAMIC_TRUNK_SYS_CLASS_NET_PATH "/sys/class/net"
#define DYNAMIC_TRUNK_BONDING_MODE_PATH  "bonding/mode"
#define DYNAMIC_TRUNK_BONDING_LACP_RATE  "bonding/lacp_rate"

#define SLOT_PORT_SPLIT_DASH 	'-'
#define SLOT_PORT_SPLIT_SLASH	'/'


extern unsigned int dynamic_trunk_member[MAX_DYNAMIC_TRUNKID + 1][MAX_DYNAMIC_TRUNK_MEMBER];/* & 0xff0000,trunk is add, &0xff00, slot, &0xff port*/

extern unsigned int dynamic_trunk_member_count[MAX_DYNAMIC_TRUNKID + 1];/*dynamic trunk port member count */

/*check trunk id range is between MIN and MAX*/
#define DYNAMIC_TRUNK_ID_IS_AVALIABLE(trunkId)	((MIN_DYNAMIC_TRUNKID <= trunkId) && (MAX_DYNAMIC_TRUNKID >= trunkId)) 

/* check the index is less than MAX trunk member*/
#define DYNAMIC_TRUNK_PORT_INDEX_IS_AVALIABLE(index)	((0 <= index) && (MAX_DYNAMIC_TRUNK_MEMBER > index))

/*check whether the port is added to the trunk*/
#define DYNAMIC_TRUNK_PORT_IS_ADDED(trunkId,index) 	((dynamic_trunk_member[trunkId][index]>>16)&0xff)
														
/*get the slot number of the [index]th port of the trunk */												
#define DYNAMIC_TRUNK_PORT_SLOT_NUM(trunkId,index) 	((dynamic_trunk_member[trunkId][index]>>8)&0xff) 

/* get the port number of the [index]th port of the trunk */
#define DYNAMIC_TRUNK_PORT_PORT_NUM(trunkId,index) 	(dynamic_trunk_member[trunkId][index]&0xff) 

/* set the [index]th slot/port for trunk [trunkId] to [slot]/[port]*/
#define DYNAMIC_TRUNK_PORT_SET(trunkId, index, slot, port)	(dynamic_trunk_member[trunkId][index] = ((1<<16)|((slot&0xff)<<8)|(port&0xff)))

/* clear the [index]th slot/port for trunk [trunkId]*/
#define DYNAMIC_TRUNK_PORT_CLEAR(trunkId, index)	(dynamic_trunk_member[trunkId][index] = 0) 

/**/
#define DYNAMIC_TRUNK_GET_TRUNK_PORTS_INFO(portsInfo, trunkId) memcpy(portsInfo, dynamic_trunk_member[trunkId], sizeof(unsigned int)*MAX_DYNAMIC_TRUNK_MEMBER);

/* increase and decrease trunk port member count*/
#define DYNAMIC_TRUNK_MEMBER_COUNT_INCREASE(trunkId)	(dynamic_trunk_member_count[trunkId]++)

#define DYNAMIC_TRUNK_MEMBER_COUNT_DECREASE(trunkId)	(dynamic_trunk_member_count[trunkId]--)

/* clear the port member count for trunk [trunkId]*/
#define DYNAMIC_TRUNK_MEMBER_COUNT_CLEAR(trunkId)	(dynamic_trunk_member_count[trunkId] = 0) 

/* check whether the trunk member is not full, TRUE is not full, else FALSE*/
#define DYNAMIC_TRUNK_MEMBER_IS_NOT_FULL(trunkId)	(MAX_DYNAMIC_TRUNK_MEMBER > dynamic_trunk_member_count[trunkId])

/* check whether the trunk port member is not empty, TRUE if not empty,else FALSE*/
#define DYNAMIC_TRUNK_MEMBER_IS_NOT_EMPTY(trunkId)	(0 < dynamic_trunk_member_count[trunkId])

/*get the trunk port member count */
#define DYNAMIC_TRUNK_MEMBER_COUNT(trunkId)		(dynamic_trunk_member_count[trunkId])
typedef struct {
	unsigned char portNum;
	unsigned char devNum;
}NPD_DYNAMIC_TRUNK_MEMBER_STC;

typedef struct {
    unsigned char slotNum;
	unsigned char portNum;
	unsigned char status;
}NPD_DYNAMIC_TRUNK_SLOT_PORT;


/*structrue for netlink infomation*/
struct ad_event
{
	int bond_ifindex;
	int slave_ifindex;
	int state;
	int flag;
};

/*********************************************************************
 * npd_dynamic_trunk_init
 * DESCRIPTION:
 *			init asic for dynamic trunk
 * INPUT:
 *			NONE
 * OUTPUT:
 *			NONE
 * RETURN:
 *			NPD_SUCCESS  -  init success
 *			NPD_FAIL    -    init failed 
 * NOTE:
 *
 *********************************************************************/
int npd_dynamic_trunk_init();

int npd_dynamic_trunk_netlink_thread_main(void);

/******************************************************************************
 * npd_dynamic_trunk_parse_slot_port_no
 * DESCRIPTION:
 *				pase slot port string such as "1-12" or "1/10" to slot no and port no
 * INPUT:
 *				str : string - eth-port string 
 * OUTPUT:
 *				slotno : uint8   -  slot no
 *				portno : uint8   -  port no
 * RETURN:
 *				NPD_SUCCESS   -  parse success
 *				NPD_FAIL	  -  parse failed
 * NOTE:
 *
 *******************************************************************************/
int npd_dynamic_trunk_parse_slot_port_no
(
	char *str,
	unsigned char *slotno,
	unsigned char *portno
) ;

/*********************************************************************
 * npd_dynamic_trunk_add_del_ports_for_trunk
 * DESCRIPTION:
 *			for add 1-8 ports to dynamic trunk
 * INPUT:
 *			trunkId : uint16  - dynamic trunk id <1-8>
 *			portsInfo : uint32 *  -  ports info we want to add to the dynamic trunk
 *			isAdd : uint32	-  is add ports to trunk or delete ports from trunk
 * OUTPUT:
 *			NONE 
 * RETURN:
 *			NPD_SUCCESS            -         success
 *			NPD_FAIL		-	error ocuured
 * NOTE:
 *
 *********************************************************************/
int npd_dynamic_trunk_add_del_ports_for_trunk
(
	unsigned short trunkId, 
	unsigned int * portsInfo, 
	unsigned int isAdd
);

/*********************************************************************
 * npd_dynamic_trunk_slave_port_trunkId_get
 * DESCRIPTION:
 *			npd dynamic trunk slave port trunkId get in software
 * INPUT:
 *			ethIfname : string - eth interface name 
 * OUTPUT:
 *			NONE
 * RETURN:
 *			trunkId : uint32   -  0 or trunkId
 *						  0  - if port is not added to any dynamic trunk
 *						  else -  dynamic trunkId
 * NOTE:
 *
 *********************************************************************/
extern unsigned int npd_dynamic_trunk_slave_port_trunkId_get
(
	unsigned char * ethIfname
);

#endif
