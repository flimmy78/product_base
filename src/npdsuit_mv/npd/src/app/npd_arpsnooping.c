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
* npd_arpsnooping.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for L3 interface ARP snooping process.
*
* DATE:
*		03/10/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.131 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#include <net/if_arp.h>
#include <pthread.h>
#include <sys/time.h>

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd/nam/npd_amapi.h"
#include "npd_c_slot.h"
#include "npd_product.h"
#include "npd_log.h"
#include "npd_hash.h"
#include "npd_eth_port.h"
#include "npd_trunk.h"
#include "npd_intf.h"
#include "npd_arpsnooping.h"
#include "npd_rstp_common.h"
#include "npd_dhcp_snp_com.h"

#if 0
struct ifreq
  {
# define IFHWADDRLEN	6
# define IFNAMSIZ	IF_NAMESIZE
    union
      {
	char ifrn_name[IFNAMSIZ];	/* Interface name, e.g. "en0".  */
      } ifr_ifrn;

    union
      {
	struct sockaddr ifru_addr;
	struct sockaddr ifru_dstaddr;
	struct sockaddr ifru_broadaddr;
	struct sockaddr ifru_netmask;
	struct sockaddr ifru_hwaddr;
	short int ifru_flags;
	int ifru_ivalue;
	int ifru_mtu;
	struct ifmap ifru_map;
	char ifru_slave[IFNAMSIZ];	/* Just fits the size */
	char ifru_newname[IFNAMSIZ];
	__caddr_t ifru_data;
      } ifr_ifru;
  };

#endif

#define MAX_IP_COUNT 8
#define INVALID_HOST_IP 0xFFFFFFFF
#define MUTIIPSUPPORT (0)
#define ARP_REQUEST 0x1
#define ARP_REPLY   0x2
#define ARP_HASH_KEY_IP HASH_KEY_KEY1
#define ARP_HASH_KEY_VID_MAC HASH_KEY_KEY2
#define ROUTE_NEXTHOP_KEY_IP   HASH_KEY_KEY1
/*wangchao add*/
#define STATIC_ARP_CONFIG_FILE_PATH "/var/run/static_arp_file"
#define ADD_ENTRY 0
#define DEL_ENTRY 1
extern asic_board_t* asic_board;
int is_master;
int slot_num;
int product_info;
extern unsigned short advanced_routing_default_vid;
extern int 	errno;
struct Hash  *arp_snoop_db_s = NULL; /* hold arp snooping item(or database)*/
struct Hash	  *route_nexthop_db_s = NULL;	/* hold route nexthop brief info compatible with Next-Hop Table*/
int arpSock = 0;  /*netlink socket fd*/
unsigned int arp_aging_time = SYS_ARP_AGING_TIME;
int sysKernArpSock = 0;  /*user arp synchronization kernel */
unsigned int arp_smac_check = FALSE;
unsigned int arp_strict_check = TRUE;
unsigned int arp_for_muti_network = FALSE;
unsigned int arp_inspection	=	FALSE;
unsigned int arp_proxy		=	FALSE;
pthread_mutex_t arpKernOpMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t arpHashMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t nexthopHashMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t namItemOpMutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef CPU_ARM_XCAT
extern unsigned int g_npd_thread_number;
#endif

/**********************************************************************************
 * npd_arp_snooping_compare
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
 *		FALSE 	- if two items are not equal.
 *		TRUE 	- if two items are equal to each other.
 *
 **********************************************************************************/
int npd_arp_snooping_compare
(
	struct arp_snooping_item_s *itemA,
	struct arp_snooping_item_s *itemB
)
{
	int equal = TRUE;

	if((NULL==itemA)||(NULL==itemB)) {
		syslog_ax_arpsnooping_err("npd arp snooping items compare null pointers error.");
		return FALSE;
	}

	if(itemA->ifIndex!=itemB->ifIndex) {/* L3 intf index*/
		equal = FALSE;
	}
	else if(0 != memcmp((char*)itemA->mac,(char*)itemB->mac,MAC_ADDRESS_LEN)) { /* MAC*/
		equal = FALSE;
	}
	else if(itemA->ipAddr != itemB->ipAddr) { /* IP Address*/
		equal = FALSE;
	}
	else if(itemA->vid != itemB->vid) { /* VID*/
		equal = FALSE;
	}
	#if 0 /* to support ARP move in same vlan but different ports */
	else if(itemA->dstIntf.isTrunk != itemB->dstIntf.isTrunk) { //destination interface type
		equal = FALSE;
	}
	else if(itemA->dstIntf.isTrunk && 	\
			(itemA->dstIntf.intf.trunk.trunkId != itemA->dstIntf.intf.trunk.trunkId)) { // target trunk
		equal = FALSE;
	}
	else if(!itemA->dstIntf.isTrunk && \
			((itemA->dstIntf.intf.port.devNum != itemB->dstIntf.intf.port.devNum) || \
			 (itemA->dstIntf.intf.port.portNum != itemB->dstIntf.intf.port.portNum))) { // target port
		equal = FALSE;
	}
	#endif
	else if(itemA->isStatic != itemB->isStatic) { /* static ARP snooping item*/
		equal = FALSE;
	}
	#if 0 /* to support ARP move in same vlan but untagged or tagged ports */
	else if(itemA->isTagged != itemB->isTagged) { // tagged or untagged
		equal = FALSE;
	}
	#endif
	return equal;
}


/**********************************************************************************
 * npd_arp_snooping_compare_byip
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
 *		FALSE 	- if two items are not equal.
 *		TRUE 	- if two items are equal.
 *
 **********************************************************************************/
int npd_arp_snooping_compare_byip
(
	struct arp_snooping_item_s *itemA,
	struct arp_snooping_item_s *itemB
)
{
	int equal = TRUE;

	if((NULL==itemA)||(NULL==itemB)) {
		syslog_ax_arpsnooping_err("npd arp snooping items compare null pointers error.");
		return FALSE;
	}

	if(itemA->ipAddr != itemB->ipAddr) { /* IP Address*/
		equal = FALSE;
	}

	return equal;

}

/**********************************************************************************
 * npd_arp_snooping_compare_bymac
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
 *		FALSE 	- if two items are not equal.
 *		TRUE 	- if two items are equal.
 *
 **********************************************************************************/
int npd_arp_snooping_compare_bymac
(
	struct arp_snooping_item_s *itemA,
	struct arp_snooping_item_s *itemB
)
{
	int equal = TRUE;

	if((NULL==itemA)||(NULL==itemB)) {
		syslog_ax_arpsnooping_err("npd arp snooping items compare null pointers error.");
		return FALSE;
	}

	if((0 == memcmp((char*)itemA->mac,(char*)itemB->mac,MAC_ADDRESS_LEN)) && 	\
			(itemA->ipAddr == itemB->ipAddr)) { /* MAC*/
		equal = TRUE;
	}
	else { 
		equal = FALSE;
	}
	
	return equal;
}

/**********************************************************************************
 * npd_arp_snooping_compare_bymac_vid
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
 *		FALSE 	- if two items are not equal.
 *		TRUE 	- if two items are equal.
 *
 **********************************************************************************/
int npd_arp_snooping_compare_bymac_vid
(
	struct arp_snooping_item_s *itemA,
	struct arp_snooping_item_s *itemB
)
{
	int equal = TRUE;

	if((NULL==itemA)||(NULL==itemB)) {
		syslog_ax_arpsnooping_err("npd arp snooping items compare null pointers error.");
		return FALSE;
	}

	if((0 == memcmp((char*)itemA->mac,(char*)itemB->mac,MAC_ADDRESS_LEN)) && 	\
			(itemA->vid == itemB->vid)) { /* MAC*/
		equal = TRUE;
	}
	else { 
		equal = FALSE;
	}
	
	return equal;
}


/**********************************************************************************
 * npd_arp_snooping_key_generate
 *
 * ARP snooping database(Hash table) hash key generation method
 *
 *	INPUT:
 *		item	- ARP snooping database item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		hash key calculated if no error occur,otherwise all 1s(0xFFFFFFFF).
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_key_generate
(
	struct arp_snooping_item_s *item
)
{
	unsigned int key = 0,i = 0;
	unsigned int ipAddr = 0;

	if(NULL == item) {
		syslog_ax_arpsnooping_err("npd arp snooping items make key null pointers error.");
		return ~0UI;
	}

	/*key = item->ifIndex;*/
	ipAddr = (item->ipAddr);

	key = (ipAddr>>22)&0x07FF + (ipAddr>>11)&0x07FF + (ipAddr)&0x07FF;

	key %= (NPD_ARPSNOOP_HASH_SIZE);
	
	return key;
}
/**********************************************************************************
 * npd_arp_snooping_key2_generate
 *
 * ARP snooping database(Hash table) hash key generation method
 *
 *	INPUT:
 *		item	- ARP snooping database item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		hash key calculated if no error occur,otherwise all 1s(0xFFFFFFFF).
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_key2_generate
(
	struct arp_snooping_item_s *item
)
{
	unsigned int key = 2166136261L; /*for hash key calculate*/
	unsigned int p = 16777619;
	unsigned int vid = 0;
	unsigned char mac[MAC_ADDR_LEN] = {0};
	unsigned int tmpData = 0;
	int i = 0;

	if(NULL == item) {
		syslog_ax_arpsnooping_err("npd arp snooping items make key null pointers error.");
		return ~0UI;
	}
    vid = item->vid;
	memcpy(mac,item->mac,MAC_ADDR_LEN);
	for(i = 0;i<8;i++){
        tmpData = ((i == 0)?((vid>>8)&0xff)\
			:((i == 1)?(vid&0xff):(mac[i-2])));
		key = (key^tmpData)*p;
		key += key<<13;
		key ^= key>>7;
		key += key<<3;
		key ^= key>>17;
		key += key<<5;
	}
	key %= (NPD_ARPSNOOP_HASH_SIZE);
	
	return key;
}


/**********************************************************************************
 * npd_route_nexthop_compare
 *
 * compare two of route nexthop brief info items
 *
 *	INPUT:
 *		itemA	- route nexthop brief info item
 *		itemB	- route nexthop brief info item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE 	- if two items are not equal.
 *		TRUE 	- if two items are equal.
 *
 **********************************************************************************/
int npd_route_nexthop_compare
(
	struct route_nexthop_brief_s *itemA,
	struct route_nexthop_brief_s *itemB
)
{
	int equal = TRUE;

	if((NULL==itemA)||(NULL==itemB)) {
		syslog_ax_arpsnooping_err("npd arp snooping items compare null pointers error.");
		return FALSE;
	}

	if(itemA->ifIndex!=itemB->ifIndex) {	/* L3 intf index*/
		equal = FALSE;
	}
	else if(itemA->ipAddr != itemB->ipAddr) {	/* ip address*/
		equal = FALSE;
	}
	
	return equal;
}

/**********************************************************************************
 * npd_route_nexthop_tblindex_compare
 *
 * compare two of route nexthop brief info items with ip address and HW table index
 *
 *	INPUT:
 *		itemA	- route nexthop brief info item
 *		itemB	- route nexthop brief info item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE 	- if two items are not equal.
 *		TRUE 	- if two items are equal.
 *
 **********************************************************************************/
int npd_route_nexthop_tblindex_compare
(
	struct route_nexthop_brief_s *itemA,
	struct route_nexthop_brief_s *itemB
)
{
	int equal = TRUE;

	if((NULL==itemA)||(NULL==itemB)) {
		syslog_ax_arpsnooping_err("npd arp snooping items compare null pointers error.");
		return FALSE;
	}

	if(itemA->tblIndex != itemB->tblIndex) {	/* HW table index*/
		equal = FALSE;
	}
	
	return equal;
}

/**********************************************************************************
 * npd_route_nexthop_key_generate
 *
 * Next-Hop brief info database(Hash table) hash key generation method
 *
 *	INPUT:
 *		item	- Next-Hop brief info item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		hash key calculated if no error occur,otherwise all 1s(0xFFFFFFFF).
 *
 **********************************************************************************/
unsigned int npd_route_nexthop_key_generate
(
	struct route_nexthop_brief_s  *item
)
{
	unsigned int key = 0;

	if(NULL == item) {
		syslog_ax_arpsnooping_err("npd arp snooping items make key null pointers error.");
		return ~0UI;
	}

	key = item->ipAddr;
	
	key %= (NPD_ARPSNOOP_HASH_SIZE);
	
	return key;
}

/**********************************************************************************
 * npd_route_nexthop_op_item
 *
 * ARP snooping database add/delete or other operations.
 *
 *	INPUT:
 *		item - ARP snooping DB items
 *		action - add or delete operation
 *           dupIfIndex - old ifindex
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ROUTE_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ROUTE_RETURN_CODE_NOT_FOUND - if item not exists in DB
 *		ROUTE_RETURN_CODE_HASH_OP_FAILED - hash table operation failed 
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
)
{
	struct route_nexthop_brief_s *data = NULL,*tmpItem = NULL, *targetRT = NULL;
	struct route_nexthop_brief_s saveData;

	memcpy(&saveData,item,sizeof(struct route_nexthop_brief_s));
	
	if(NULL == item) {
		syslog_ax_arpsnooping_err("npd %s nexthop brief item null pointer error.",(NEXTHOP_ADD_ITEM==action) ? "add":"del");
		return COMMON_RETURN_CODE_NULL_PTR;
	}
	if((0 != dupIfIndex) && (dupIfIndex != item->ifIndex)) {
		item->ifIndex = dupIfIndex;
		tmpItem = item;
	}
	else {
		tmpItem = &saveData;
	}
	data = hash_search(route_nexthop_db_s,tmpItem,NULL,ROUTE_NEXTHOP_KEY_IP);

	if(NEXTHOP_ADD_ITEM == action) {
		if(NULL != data) {
			data->ifIndex = saveData.ifIndex;
			data->ipAddr = saveData.ipAddr;
			/* delete by qinhs@autelan.com Sep.3rd 2008 
			 *  old item has reference count should not be overriden
			 */
			#if 0
			data->rtUsedCnt = saveData.rtUsedCnt;
			#endif
			data->tblIndex = saveData.tblIndex;
			syslog_ax_arpsnooping_dbg("npd nexthop brief dup item ip %#0x found when add.", data->ipAddr);
			return ROUTE_RETURN_CODE_SUCCESS;
			/*return -NPD_ROUTE_NEXTHOP_ERR_DUPLICATED;*/
		}

		/*build hash data to insert into hash backet*/
		data = (struct route_nexthop_brief_s *) malloc(sizeof(struct route_nexthop_brief_s));
		if(NULL == data) {
			syslog_ax_arpsnooping_err("npd add nexthop db alloc memory null!");
			return COMMON_RETURN_CODE_NO_RESOURCE;
		}
		memset(data,0,sizeof(struct route_nexthop_brief_s));
		memcpy(data,&saveData,sizeof(struct route_nexthop_brief_s));
		
		targetRT = hash_push(route_nexthop_db_s,data); /*insert into hash table*/
		if(NULL == targetRT){
			free(data);
			data = NULL;
			syslog_ax_arpsnooping_err("error: npd route nexthop op item push FAILED!\n");
            return ROUTE_RETURN_CODE_HASH_OP_FAILED;
		}
	}
	else if(NEXTHOP_DEL_ITEM == action) {
		if(NULL == data) {
			syslog_ax_arpsnooping_err("npd nexthop brief no item found when delete.");
			return ROUTE_RETURN_CODE_NOT_FOUND;
		}

		targetRT = hash_pull(route_nexthop_db_s,data); /* pull out from hash table*/
		if(targetRT) {
			free(targetRT);	/* release memory*/
			targetRT = NULL;
		}
		else{
			syslog_ax_arpsnooping_err("error: npd route nexthop op item hash pull FAILED!\n");
			return ROUTE_RETURN_CODE_HASH_OP_FAILED;
		}
	}
	
	return ROUTE_RETURN_CODE_SUCCESS;	
}

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
)
{
	struct route_nexthop_brief_s *data = NULL, item;
	
	memset(&item, 0, sizeof(struct route_nexthop_brief_s));
	item.ifIndex = ifIndex;
	item.ipAddr  = ipAddr;
	
	data = hash_search(route_nexthop_db_s,&item,NULL,ROUTE_NEXTHOP_KEY_IP);

	if(NULL == data) {
		syslog_ax_arpsnooping_dbg("npd not found nexthop tbl index for if %#0x ip %#0x", ifIndex,ipAddr);
		return NULL;		
	}

	syslog_ax_arpsnooping_dbg("npd get nexthop tbl index %#0x for if %#0x ip %#0x ref %d",	\
						data->tblIndex,data->ifIndex,data->ipAddr, data->rtUsedCnt);
	
	return data;	
}

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
)
{
	struct route_nexthop_brief_s *datas[1]={NULL}, item;
	
	memset(&item, 0, sizeof(struct route_nexthop_brief_s));
	item.tblIndex = routeEntryIndex;
	
	hash_return(route_nexthop_db_s, &item, datas, 1, npd_route_nexthop_tblindex_compare);

	if(NULL == datas[0]){
		return NULL;
	}
	syslog_ax_arpsnooping_dbg("npd got nexthop info index %#0x by tbl index %#0x ip %#0x",	\
						datas[0]->ifIndex,datas[0]->tblIndex,datas[0]->ipAddr);
	
	return datas[0];	
}

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
 *		ROUTE_RETURN_CODE_SUCCESS - if no error occurred.
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ROUTE_RETURN_CODE_NOT_FOUND - if item not exists in DB
 *		ROUTE_RETURN_CODE_HASH_OP_FAILED - hash table operation failed 
 *		ROUTE_RETURN_CODE_ERROR - arp occupy failed
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
)
{
	unsigned int retVal = ROUTE_RETURN_CODE_SUCCESS;
	struct route_nexthop_brief_s *data = NULL,item;
	struct arp_snooping_item_s dbItem;
	unsigned int tmpIndex = 0;
	pthread_mutex_lock(&nexthopHashMutex);
	data = npd_route_nexthop_find(ifIndex,ipAddr);
	if(NULL == data) {
		memset(&dbItem,0,sizeof(struct arp_snooping_item_s));
		dbItem.ifIndex = ifIndex;
		dbItem.ipAddr = ipAddr;
		syslog_ax_arpsnooping_dbg("npd nexthop brief item no found,occupy a new one");
		/* TODO: occupy a new HW table item first*/
		pthread_mutex_lock(&namItemOpMutex);
		retVal = nam_arp_snooping_op_item_encaps(&dbItem,ARP_SNOOPING_OCCUPY_ITEM,&tmpIndex);
		pthread_mutex_unlock(&namItemOpMutex);
		if(ARP_RETURN_CODE_SUCCESS != retVal) {
			syslog_ax_arpsnooping_dbg("npd route nexthop table index get error %d when occupy a new one",retVal);
			pthread_mutex_unlock(&nexthopHashMutex);
			return ROUTE_RETURN_CODE_ERROR;
		}

		/* TODO:add to route Next-Hop brief hash table*/
		memset(&item,0,sizeof(struct route_nexthop_brief_s));
		item.ifIndex  = ifIndex;
		item.ipAddr	  = ipAddr;
		item.tblIndex = tmpIndex;
		item.rtUsedCnt = 1;
		retVal = npd_route_nexthop_op_item(&item,NEXTHOP_ADD_ITEM,0);
		if(ROUTE_RETURN_CODE_SUCCESS != retVal) {
			syslog_ax_arpsnooping_err("npd nexthop brief add to hash error %d when occupy",retVal);
			pthread_mutex_unlock(&nexthopHashMutex);
			return retVal;
		}
		
		*tblIndex = tmpIndex;
		syslog_ax_arpsnooping_dbg(
				"npd occupy new nexthop tbl index %#0x for index %#0x ip %d.%d.%d.%d",	\
				*tblIndex,ifIndex,(ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF,(ipAddr>>8)&0xFF,	\
				ipAddr &0xFF);		
	}
	else {
		data->rtUsedCnt += 1;
		*tblIndex = data->tblIndex;
		syslog_ax_arpsnooping_dbg(
				"npd got nexthop tbl index %#0x for index %#0x ip %d.%d.%d.%d ref %d",	\
				*tblIndex,ifIndex,(ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF,(ipAddr>>8)&0xFF,		\
				ipAddr &0xFF,data->rtUsedCnt);
	}
	pthread_mutex_unlock(&nexthopHashMutex);
	return ROUTE_RETURN_CODE_SUCCESS;	
}

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
 *		ROUTE_RETURN_CODE_SUCCESS - if no error occurred.
 *		ROUTE_RETURN_CODE_NOT_FOUND - route nexthop item not found
 *		ROUTE_RETURN_CODE_NOTCONSISTENT - the tblindex is not consistent
 *		ROUTE_RETURN_CODE_HASH_OP_FAILED - hash table operation failed
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
)
{
	unsigned int retVal = ROUTE_RETURN_CODE_SUCCESS;
	struct route_nexthop_brief_s *data = NULL;
	struct route_nexthop_brief_s *targetRT = NULL;
	unsigned int ret = 0;
	unsigned char devNum = 0;
	struct arp_snooping_item_s arpInfo;

	/*
	  * route nexthop entry 0 & 1 & 2 used for special purpose
	  * 	0 - action DROP for default route, 
	  *	1 - action ROUTE_and_MIRROR_CPU for ipv4 mcast route default
	  *	2 - action TRAP_CPU for ipv4 ucast route default
	  */
	if(0 == tblIndex || 1 == tblIndex || 2 == tblIndex) {
		return retVal;
	}
	memset(&arpInfo,0,sizeof(struct arp_snooping_item_s));
	arpInfo.dstIntf.intf.port.devNum = devNum;
	pthread_mutex_lock(&nexthopHashMutex);
	data = npd_route_nexthop_find(ifIndex,ipAddr);
	if(NULL == data) {
		syslog_ax_arpsnooping_err(
				"npd nexthop item no found when release tblindex %#x for if %#x ip %d.%d.%d.%d", \
				tblIndex, ifIndex,(ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF,(ipAddr>>8)&0xFF,ipAddr &0xFF);
		retVal = ROUTE_RETURN_CODE_NOT_FOUND;
		pthread_mutex_unlock(&nexthopHashMutex);
		return retVal;
	}
	else if(tblIndex != data->tblIndex) {
		syslog_ax_arpsnooping_err(
				"npd nexthop tblindex %#0x(hw) %#0x(sw) for if %#0x ip %d.%d.%d.%d,not consistent",	\
				tblIndex,data->tblIndex,ifIndex,(ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF,(ipAddr>>8)&0xFF,ipAddr &0xFF);
		retVal = ROUTE_RETURN_CODE_NOTCONSISTENT;
	}
	else {
		retVal = ROUTE_RETURN_CODE_SUCCESS;
	}
	
	if(data->rtUsedCnt > 0) 
		data->rtUsedCnt -= 1;
	else 
		data->rtUsedCnt = 0;
	
	syslog_ax_arpsnooping_dbg(
		"npd nexthop tblindex %#0x for if %#0x ip %d.%d.%d.%d referenced %d",	\
		tblIndex,ifIndex,(ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF,(ipAddr>>8)&0xFF,	\
		ipAddr &0xFF, data->rtUsedCnt);
	
	if(0 == data->rtUsedCnt){ 
		arpInfo.ipAddr = data->ipAddr;
		ret = nam_arp_snooping_get_item(devNum,tblIndex,&arpInfo);
		if(ARP_RETURN_CODE_ACTION_TRAP2CPU == ret){
            pthread_mutex_lock(&namItemOpMutex);
			ret = nam_arp_snooping_op_item(&arpInfo,ARP_SNOOPING_DEL_ITEM,&tblIndex);
			pthread_mutex_unlock(&namItemOpMutex);
			if(ARP_RETURN_CODE_SUCCESS != ret) {
				syslog_ax_arpsnooping_err("nam %s arp snooping item at %d error %d",	\
							"DEL",tblIndex,ret);
			}
			targetRT = hash_pull(route_nexthop_db_s,data);
			if(targetRT) {
				free(targetRT);
				targetRT = NULL;
			}
			else{
				syslog_ax_arpsnooping_err("error: route nexthop tblindex release hash pull FAILED!\n");
				pthread_mutex_unlock(&nexthopHashMutex);
				return ROUTE_RETURN_CODE_HASH_OP_FAILED;
			}
		}
	}
	pthread_mutex_unlock(&nexthopHashMutex);

	return retVal;	
}

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
 *		ROUTE_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_BADPARAM	 - if the parameter action is unknow action
 *		COMMON_RETURN_CODE_NULL_PTR  - 
 *		ROUTE_RETURN_CODE_SET_ERROR  - 
 *
 **********************************************************************************/
int npd_route_table_op_host_item
(
	struct route_nexthop_brief_s *item,
	enum NPD_ROUTE_ACTION		action
)
{
	unsigned int retVal = ROUTE_RETURN_CODE_SUCCESS;

	if(NULL == item){
		return COMMON_RETURN_CODE_NULL_PTR;
	}
	if(ROUTE_ADD_ITEM == action) {
		retVal = LPM_TBL_HOST_ENTRY_SET(htonl(item->ipAddr),32,item->tblIndex);
		syslog_ax_arpsnooping_dbg("npd add host route %d.%d.%d.%d to HW table %d %s",	\
				(item->ipAddr>>24) & 0xFF,(item->ipAddr>>16) & 0xFF,	\
				(item->ipAddr>>8) & 0xFF,item->ipAddr & 0xFF,item->tblIndex,(ROUTE_RETURN_CODE_SUCCESS != retVal) ? "fail":"ok");
		if(ROUTE_RETURN_CODE_SUCCESS != retVal){
			retVal = ROUTE_RETURN_CODE_ERROR;
		}
		else {
			retVal = ROUTE_RETURN_CODE_SUCCESS;
		}
	}
	else if(ROUTE_DEL_ITEM == action) {
		/* delete route item from HW*/
		retVal = del_LPM_TBL_Entry(htonl(item->ipAddr),32);
		syslog_ax_arpsnooping_dbg("npd del host route %d.%d.%d.%d from HW table %s",	\
				(item->ipAddr>>24) & 0xFF,(item->ipAddr>>16) & 0xFF,	\
				(item->ipAddr>>8) & 0xFF,item->ipAddr & 0xFF,(ROUTE_RETURN_CODE_SUCCESS != retVal) ? "fail":"ok");
		if(ROUTE_RETURN_CODE_SUCCESS != retVal){
			retVal = ROUTE_RETURN_CODE_ERROR;
		}
		else {
			retVal = ROUTE_RETURN_CODE_SUCCESS;
		}
	}
	else if(ROUTE_UPDATE_ITEM == action) {

	}
	else {
		syslog_ax_arpsnooping_err("npd route table operation unknown action %d",action);
		retVal = COMMON_RETURN_CODE_BADPARAM;
	}
	
	return retVal;
}

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
)
{
	if(NULL == item){
		return;
	}
	if(TRUE == withTitle) {
		syslog_ax_arpsnooping_dbg("%-8s %-15s %-12s\n","IFINDEX","IP ADDRESS","TABLE INDEX");
		syslog_ax_arpsnooping_dbg("-------- --------------- ------------\n");
	}
	syslog_ax_arpsnooping_dbg("%-8x %-3d.%-3d.%-3d.%-3d %-8x\n",	\
			item->ifIndex,(item->ipAddr>>24)&0xFF,(item->ipAddr>>16)&0xFF,	\
			(item->ipAddr>>8)&0xFF,item->ipAddr&0xFF,item->tblIndex);
	return;
}

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
)
{
	hash_show(route_nexthop_db_s,npd_route_nexthop_show_item);
	return;
}

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
 *
 * 	RETURN:
 *		ROUTE_RETURN_CODE_SUCCESS - if no error occurred.
 *		COMMON_RETURN_CODE_NULL_PTR - item is NULL pointer
 *		ROUTE_RETURN_CODE_ACTION_TRAP2CPU - get item cmd trap to cpu
 *		ROUTE_RETURN_CODE_ACTION_HARD_DROP -get item cmd hard drop
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_route_nexthop_iteminfo_get
(
	unsigned char devNum,
	unsigned int tblIndex,
	struct arp_snooping_item_s *item,	
	unsigned int *refCnt
)
{
	struct route_nexthop_brief_s *routeData = NULL;
	unsigned int retVal = ROUTE_RETURN_CODE_SUCCESS;

	if(NULL == item) {
		return COMMON_RETURN_CODE_NULL_PTR;
	}

	retVal = nam_arp_snooping_get_item(devNum,tblIndex,item);
	if(ARP_RETURN_CODE_ACTION_TRAP2CPU == retVal) {
		syslog_ax_arpsnooping_dbg("npd get route nexthop info from hw table at %#0x command TRAP_TO_CPU",tblIndex);
		routeData = npd_route_nexthop_get_by_tblindex(tblIndex);
		if(NULL != routeData) {
			item->ifIndex = routeData->ifIndex;
			*refCnt = routeData->rtUsedCnt;
		}
		else { /* not found */
			item->ifIndex = 0;
			*refCnt = 0;
		}
		retVal = ROUTE_RETURN_CODE_ACTION_TRAP2CPU;
	}
	else if(ARP_RETURN_CODE_ACTION_HARD_DROP == retVal) {
		syslog_ax_arpsnooping_dbg("npd get route nexthop info from hw table at %#0x command HEAD_DROP",tblIndex);
		retVal = ROUTE_RETURN_CODE_ACTION_HARD_DROP;
	}
	else if(ARP_RETURN_CODE_SUCCESS != retVal) {
		syslog_ax_arpsnooping_err("npd get route nexthop info from hw table at %#0x error",tblIndex);
		retVal = ROUTE_RETURN_CODE_ERROR;
	}
	else {
		retVal = ROUTE_RETURN_CODE_SUCCESS;
		routeData = npd_route_nexthop_get_by_tblindex(tblIndex);
		if(NULL != routeData) {
			item->ifIndex = routeData->ifIndex;
			*refCnt = routeData->rtUsedCnt;
		}
		else { /* not found */
			item->ifIndex = 0;
			*refCnt = 0;
		}

#if 0
		syslog_ax_arpsnooping_dbg("------------------------------------------------------------\n");
		syslog_ax_arpsnooping_dbg("%-18s %-16s %-4s %-6s %-4s %-10s\n","MAC Address","IP Address","VLAN","DEVICE","PORT","INTERFACE");
		syslog_ax_arpsnooping_dbg("------------------ ---------------- ---- ------ ------------\n");
		syslog_ax_arpsnooping_dbg("%02X:%02X:%02X:%02X:%02X:%02X  %-3d.%-3d.%-3d.%-3d  %-4d %-6d %-4d %#-x\n",	\
				item->mac[0],item->mac[1],item->mac[2],item->mac[3],item->mac[4],item->mac[5], \
				(item->ipAddr>>24) & 0xFF,(item->ipAddr>>16) & 0xFF,(item->ipAddr>>8) & 0xFF,	\
				item->ipAddr & 0xFF,item->vid,item->dstIntf.intf.port.devNum, \
				item->dstIntf.intf.port.portNum,item->ifIndex);
		syslog_ax_arpsnooping_dbg("------------------------------------------------------------\n");
#endif
	}
	
	return retVal;	
}

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
void npd_init_arpsnooping
(
	void
)
{
	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){5612e not support*/
	/*	return;
	}*/
	if(!((productinfo.capbmp) & FUNCTION_ARP_VLAUE)){
		npd_syslog_dbg("do not support arp initial!\n");
		return;
	}
	else if(((productinfo.capbmp) & FUNCTION_ARP_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_ARP_VLAUE)){
		npd_syslog_dbg("do not support arp initial!\n");
		return;
	}
	else{

	syslog_ax_arpsnooping_dbg("init arp snooping database\r\n");
	
	/* TODO: here we temporarily use 2K hash buckets to hold ARP snooping items*/
	arp_snoop_db_s = hash_new(NPD_ARPSNOOP_HASH_SIZE);

	/*arp_snoop_db_s->hash_cmp = npd_arp_snooping_compare;*/
	arp_snoop_db_s->hash_cmp = npd_arp_snooping_compare_byip;
	arp_snoop_db_s->hash_key = npd_arp_snooping_key_generate;
	arp_snoop_db_s->hash_key2 = npd_arp_snooping_key2_generate;

	/* TODO:here we temporarily use 2K hash buckets to hold Next-Hop brief info items*/ 
	route_nexthop_db_s = hash_new(NPD_ARPSNOOP_HASH_SIZE);

	route_nexthop_db_s->hash_cmp = npd_route_nexthop_compare;
	route_nexthop_db_s->hash_key = npd_route_nexthop_key_generate;
	route_nexthop_db_s->hash_key2 = NULL;

	/* TODO:init arp Next-Hop table index and arp Mac table index*/
	nam_arp_table_index_init();

	/*create socket deleted kernel arp */
	sysKernArpSock = socket(AF_INET,SOCK_DGRAM,0);
	nam_thread_create("ArpSync",(void *)npd_arp_snooping_sync2kern,NULL,TRUE,FALSE);
	return;	
	}
}

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
)
{
	if(NULL == item){
		return;
	}
	if(TRUE == withTitle) {
		syslog_ax_arpsnooping_dbg("%-8s %-15s %-17s %-4s %-6s %-6s %-4s\n",	\
				"IFINDEX","IP ADDRESS","MAC ADDRESS","VID","TAG","DEVICE","PORT");
		syslog_ax_arpsnooping_dbg("-------- --------------- ----------------- ---- ------ ------ ----\n");
	}
	syslog_ax_arpsnooping_dbg("%-8x %-3d.%-3d.%-3d.%-3d %02x:%02x:%02x:%02x:%02x:%02x %-4d %-6s %-6d %-4d\n",	\
			item->ifIndex,(item->ipAddr>>24)&0xFF,(item->ipAddr>>16)&0xFF,	\
			(item->ipAddr>>8)&0xFF,item->ipAddr&0xFF,item->mac[0],item->mac[1],	\
			item->mac[2],item->mac[3],item->mac[4],item->mac[5],item->vid,	\
			item->isTagged ? "TRUE":"FALSE",item->dstIntf.intf.port.devNum,	\
			item->dstIntf.intf.port.portNum);
	return;
}

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
)
{
	hash_show(arp_snoop_db_s,npd_arp_snooping_show_item);
	return;
}
/**********************************************************
 * npd_arp_snooping_update_hashtable
 *
 * update arp hash table with new item
 *
 *	INPUT:
 *		oldItem - the pointer of the item which we want to update
 *		newItem - the pointer of the item we want to update with 
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - update successed
 *		COMMON_RETURN_CODE_NULL_PTR  - null pointer error
 *		ARP_RETURN_CODE_HASH_OP_FAILED - hash operation error 
 *
 **********************************************************/
int npd_arp_snooping_update_hashtable
(
	struct arp_snooping_item_s *oldItem,
	struct arp_snooping_item_s *newItem
)
{
	int ret = ARP_RETURN_CODE_SUCCESS;

	/*memset(oldItem,0,(struct arp_snooping_item_s));*/
	/*printf("npd_arp_snooping_update_hashtable :: UPDATE\n");*/
	struct arp_snooping_item_s * tmpItem = NULL;
	if((NULL == oldItem)||(NULL == newItem)){
		return COMMON_RETURN_CODE_NULL_PTR;
	}
#if 0
	oldItem ->dstIntf.intf.port.devNum  = newItem ->dstIntf.intf.port.devNum;
	oldItem ->dstIntf.intf.port.portNum  = newItem ->dstIntf.intf.port.portNum;
	oldItem->isStatic = newItem->isStatic;
	oldItem->isTagged = newItem->isTagged;
	oldItem->vid = newItem->vid;
	oldItem->vidx = newItem->vidx;
	memcpy(oldItem->mac,newItem->mac,MAC_ADDRESS_LEN);
#endif
	/*copy all by zhubo@autelan.com 2008.8.6*/
	/*memcpy(oldItem,newItem,(sizeof(struct arp_snooping_item_s )));*/
	/*chang by hanhui for mutiple key */
    if((oldItem->vid != newItem->vid)||memcmp(oldItem->mac,newItem->mac,MAC_ADDR_LEN)){ /*pull then push new for update key2*/
        tmpItem = hash_pull(arp_snoop_db_s,oldItem);
		if((NULL != tmpItem)&&(tmpItem == oldItem)){
           memcpy(oldItem,newItem,(sizeof(struct arp_snooping_item_s )));
		   tmpItem = hash_push(arp_snoop_db_s,oldItem);
		   if(NULL == tmpItem){
               return ARP_RETURN_CODE_HASH_OP_FAILED;
		   }
		}
		else {
			return ARP_RETURN_CODE_HASH_OP_FAILED;
		}
	}
	else {
		memcpy(oldItem,newItem,(sizeof(struct arp_snooping_item_s )));
	}

	return ret;
}

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
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		ARP_RETURN_CODE_TABLE_FULL  - the hash table is full
 *		ARP_RETURN_CODE_STATIC_EXIST - static arp item already exists
 *		ARP_RETURN_CODE_DUPLICATED - if duplicated items found
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ARP_RETURN_CODE_NOTEXISTS - if item not exists in DB
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
	struct arp_snooping_item_s* dupItem
)
{
	struct arp_snooping_item_s *data = NULL, *targetArp = NULL;
	int count = 0;
	int ret = ARP_RETURN_CODE_SUCCESS;
	
	if(NULL == item) {
		syslog_ax_arpsnooping_err("npd %s arp item null pointer error.",(ARP_SNOOPING_ADD_ITEM==action) ? "add":"del");
		return COMMON_RETURN_CODE_NULL_PTR;
	}
	count = hash_count(arp_snoop_db_s);
	if((NPD_ARPSNOOP_TABLE_SIZE <= count ) && (ARP_SNOOPING_ADD_ITEM == action)) {
		syslog_ax_arpsnooping_err("npd %s add arp mac table full\n");
		return ARP_RETURN_CODE_TABLE_FULL;
	}
	data = hash_search(arp_snoop_db_s,item,NULL,ARP_HASH_KEY_IP);

	if(ARP_SNOOPING_ADD_ITEM==action) {
		/* TODO:duplicated item may be updated !!!*/
		if(NULL != data) {
			syslog_ax_arpsnooping_dbg("npd duplicated arp item found when add.");
			if(TRUE == data->isStatic) {
				/*static arp don't update*/
				syslog_ax_arpsnooping_dbg("npd duplicated static arp item found when add.");
				memcpy(dupItem, data, sizeof(struct arp_snooping_item_s));
				return ARP_RETURN_CODE_STATIC_EXIST;
			}
			
			/* TODO:update arp hash table in order to  test by zhubo@autelan.com 2008.7.17*/
			memcpy(dupItem, data, sizeof(struct arp_snooping_item_s));
			ret = npd_arp_snooping_update_hashtable(data,item);
			if(ARP_RETURN_CODE_SUCCESS == ret){
				return ARP_RETURN_CODE_DUPLICATED;
			}
			else if(COMMON_RETURN_CODE_NULL_PTR == ret){
				return COMMON_RETURN_CODE_NULL_PTR;
			}
			else {
				return ARP_RETURN_CODE_HASH_OP_FAILED;
			}
		}

		/*build hash data to insert into hash backet*/
		data = (struct arp_snooping_item_s *) malloc(sizeof(struct arp_snooping_item_s));
		if(NULL == data) {
			syslog_ax_arpsnooping_err("npd add snooping db alloc memory null!");
			return COMMON_RETURN_CODE_NO_RESOURCE;
		}
		memset(data,0,sizeof(struct arp_snooping_item_s));
		memcpy(data,item,sizeof(struct arp_snooping_item_s));
		targetArp = hash_push(arp_snoop_db_s,data); /*insert into hash table*/
		if(NULL == targetArp){
			syslog_ax_arpsnooping_err("error:npd arp hash push FAILED!\n");
			free(data);
			data = NULL;
			return ARP_RETURN_CODE_HASH_OP_FAILED;
		}
	}
	else if(ARP_SNOOPING_DEL_ITEM==action) {
		if(NULL == data) {
			syslog_ax_arpsnooping_err("npd arp snooping no item found when delete.");
			return ARP_RETURN_CODE_NOTEXISTS;
		}

		targetArp = hash_pull(arp_snoop_db_s,data); /* pull out from hash table*/
		if(targetArp) {
			free(targetArp);	/* release memory*/
			targetArp = NULL;
		}
		else {
            syslog_ax_arpsnooping_err("error:npd arp hash pull FAILED!\n");
			return ARP_RETURN_CODE_HASH_OP_FAILED;
		}
	}
	return ARP_RETURN_CODE_SUCCESS;	
}

/**********************************************************************************
 * npd_arp_snooping_ip_check
 *
 * ARP snooping check if ip address valid
 *
 *	INPUT:
 *		ipAddr - ip address
 *	
 *	OUTPUT:
 *		gateway - the pointer of interface's ips
 *		mask - the pointer of interface's masks
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_ERROR - if error occurred		
 *
 *	NOTE:
 *
 **********************************************************************************/
	int npd_arp_snooping_ip_check
	(
		unsigned int ipAddr,
		unsigned int * gateway,
		unsigned int * mask
	)
	{
		int retVal = ARP_RETURN_CODE_SUCCESS;
		int i = 0;
		
		/* legal ip address must not be broadcast,0,or multicast,*/
		/* or 169.254.x.x or 127.0.0.1*/
		if((0==ipAddr) ||
			(~0UI == ipAddr)||
			(0x7F000001 == ipAddr ) ||
			(0xA9FE == ((ipAddr >> 16) & 0xffff))) {
			retVal = ARP_RETURN_CODE_ERROR;	
		}
		return retVal;
	}

/**********************************************************************************
 * npd_arp_snooping_ip_collision_check
 *
 * ARP snooping check if ip address valid
 *
 *	INPUT:
 *		sip - the source ip address
 *		gateway - the pointer of interface's ips we want to compare with
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_ERROR - if error occurred		
 *
 *	NOTE:
 *
 **********************************************************************************/

int npd_arp_snooping_ip_collision_check
(
    unsigned int sip,
    unsigned int * gateway
)
{
	int retVal = ARP_RETURN_CODE_SUCCESS;
    int i = 0;
    for(i = 0; i < MAX_IP_COUNT;i++){
		if((INVALID_HOST_IP != gateway[i])&&\
			(gateway[i] == sip)){  /* the arp is from my self*/
			retVal = ARP_RETURN_CODE_ERROR;
			break;
		}
    }
	return retVal;
}


/**********************************************************************************
 * npd_arp_snooping_ip_gateway_check
 *
 * 	ARP snooping check if ip address is compatible with gateway address given
 *
 *	INPUT:
 *		ipAddr - ip address
 *		gateway - the pointer of interface's ips we want to check with
 *		mask  - the pointer of interface's masks we want to check with
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_ERROR - if error occurred		
 *
 *	NOTE:
 *		ip address should not be gateway address otherwise check fail
 *
 **********************************************************************************/
int npd_arp_snooping_ip_gateway_check
(
	unsigned int ipAddr,
	unsigned int * gateway,
	unsigned int * mask
)
{
	int i = 0;
	
	if((0 == ipAddr)||(~0UL == ipAddr)){
		return ARP_RETURN_CODE_ERROR;
	}
	for(i = 0; i < MAX_IP_COUNT;i++){
		if(INVALID_HOST_IP != gateway[i]){
			if((ipAddr & mask[i]) == (gateway[i] & mask[i])) { /* ip address in same range */
				if((~0UL == (ipAddr | mask[i]))&&(INVALID_HOST_IP != mask[i])) { /* specified sub network broadcast address */
				    return ARP_RETURN_CODE_ERROR;
			    }
			    else if((ipAddr == (ipAddr & mask[i]))&&(INVALID_HOST_IP != mask[i])) { /* specified sub network reserved address */
				    return ARP_RETURN_CODE_ERROR;
			    }
				return ARP_RETURN_CODE_SUCCESS;
			}
		}
	}
	return ARP_RETURN_CODE_ERROR;
}
/**********************************************************************************
 * npd_arp_snooping_dip_check
 *
 * 	ARP snooping check if ip address is compatible with gateway address given
 *
 *	INPUT:
 *           dip  --  destination ip address
 *           gateway  -- the ips of the interface
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_ERROR - if error occurred		
 *
 *	NOTE:
 *		dip address should be one of gateway address
 *
 **********************************************************************************/
int npd_arp_snooping_dip_check
(
	unsigned int dip,
	unsigned int * gateway
)
{
	int i = 0;
	
	if((0 == dip)||(~0UL == dip)){
	    syslog_ax_intf_err("arp snooping dip check fail! dip %#0x\n",dip);
		return ARP_RETURN_CODE_ERROR;
	}
	/*  do not learn gratuitous arp
	if (sip == dip){
		syslog_ax_intf_dbg("arp snooping dip check success! gratuitous arp sip %#0x\n",sip);
		return ARP_RETURN_CODE_SUCCESS;
	}
	*/
	for(i = 0; i < MAX_IP_COUNT;i++){
		if(INVALID_HOST_IP != gateway[i]){
			if(dip == gateway[i]){ /* ip address in the interface */
				syslog_ax_intf_dbg("arp snooping dip check success! dip %#0x gateway %#0x\n",dip,gateway[i]);
				return ARP_RETURN_CODE_SUCCESS;
			}
		}
	}
	/*syslog_ax_intf_err("arp snooping dip check fail! dip %#0x\n",dip);*/
	return ARP_RETURN_CODE_ERROR;
}
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
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_ERROR - if error occurred		
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
)
{
	int i = 0;
	
	if((0 == dip)||(~0UL == dip)){
	    syslog_ax_intf_err("arp snooping dip check fail! dip %#0x\n",dip);
		return ARP_RETURN_CODE_ERROR;
	}
	/*  do not learn gratuitous arp
	if (sip == dip){
		syslog_ax_intf_dbg("arp snooping dip check success! gratuitous arp sip %#0x\n",sip);
		return ARP_RETURN_CODE_SUCCESS;
	}
	*/
	for(i = 0; i < MAX_IP_COUNT;i++){
		if(INVALID_HOST_IP != gateway[i]){
			if((dip == gateway[i])&&(INVALID_HOST_IP == mask[i])){ /* ip address in the interface */
				syslog_ax_intf_dbg("arp snooping dip for 32bit mask check success! dip %#0x gateway %#0x\n",dip,gateway[i]);
				return ARP_RETURN_CODE_SUCCESS;
			}
		}
	}
	syslog_ax_intf_dbg("arp snooping dip not for 32bit mask ip, dip %#0x! go on to check gateway\n",dip);
	return ARP_RETURN_CODE_ERROR;
}

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
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_ERROR - if error occurred		
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
)
{
	int i = 0;
	
	if((0 == dip)||(~0UL == dip)|| \
	   (0 == sip)||(~0UL == sip)){
	   syslog_ax_intf_err("arp snooping ip subnet check fail! sip %#0x dip %#0x\n",sip,dip);
		return ARP_RETURN_CODE_ERROR;
	}
	for(i = 0; i < MAX_IP_COUNT;i++){
		if(INVALID_HOST_IP != gateway[i]){
			if(((dip & mask[i]) == (sip & mask[i]))&& ((sip & mask[i]) == (gateway[i] & mask[i]))){ /* ip address in different range */
                syslog_ax_intf_dbg("arp snooping ip subnet check success! sip %#0x dip %#0x gateway %#0x mask %#0x \n",\
					sip,dip,gateway[i],mask[i]);
				return ARP_RETURN_CODE_SUCCESS;
			}
		}
	}
	syslog_ax_intf_err("arp snooping ip subnet check fail! sip %#0x dip %#0x\n",sip,dip);
	return ARP_RETURN_CODE_ERROR;
}

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
 *		NULL - founded item is null or ifindex is not match
 *		item - ARP snooping database item if not null and ifindex is match
 *
 *	NOTE:
 *
 **********************************************************************************/
struct arp_snooping_item_s *npd_arp_snooping_find_item_bymac
(
	unsigned int  ifIndex,
	unsigned char *mac
)
{
	struct arp_snooping_item_s data,*item = NULL;

	memset(&data,0,sizeof(struct arp_snooping_item_s));
	memcpy(data.mac,mac,MAC_ADDRESS_LEN);
	
    hash_return(arp_snoop_db_s,&data,&item,1,npd_arp_snooping_compare_bymac);
	/*item = hash_search(arp_snoop_db_s,&data,npd_arp_snooping_compare_bymac);*/
	if(NULL == item) {
		return item; /* return null pointer*/
	}

	if(item->ifIndex != ifIndex) {
		return NULL;
	}

	if(item->dstIntf.isTrunk) {
		syslog_ax_arpsnooping_dbg("found arp %d.%d.%d.%d on trunk(%d,%d) vlan %d if %d",	\
				(item->ipAddr >> 24) & 0xFF,(item->ipAddr >> 16) & 0xFF,	\
				(item->ipAddr >> 8) & 0xFF,item->ipAddr & 0xFF,	\
				item->dstIntf.intf.trunk.devNum,	\
				item->dstIntf.intf.trunk.trunkId,item->vid,item->ifIndex);
	}
	else {
		syslog_ax_arpsnooping_dbg("found arp %d.%d.%d.%d on port(%d,%d) vlan %d if %d",	\
				(item->ipAddr >> 24) & 0xFF,(item->ipAddr >> 16) & 0xFF,	\
				(item->ipAddr >> 8) & 0xFF,item->ipAddr & 0xFF,	\
				item->dstIntf.intf.port.devNum,	\
				item->dstIntf.intf.port.portNum,item->vid,item->ifIndex);
	}

	return item;
}


/**********************************************************************************
 * npd_arp_snooping_find_item_bymac_vid
 *
 * find ARP snooping item by mac address
 *
 *	INPUT:
 *		ifIndex - L3 interface index
 *		mac - mac address
 *           vid - vlan id
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NULL - not found item matched ifindex,mac and vid
 *		item - ARP snooping database item
 *
 *	NOTE:
 *
 **********************************************************************************/
struct arp_snooping_item_s *npd_arp_snooping_find_item_bymac_vid
(
	unsigned int  ifIndex,
	unsigned char *mac,
	unsigned short vid
)
{
	struct arp_snooping_item_s data,*item = NULL;

	memset(&data,0,sizeof(struct arp_snooping_item_s));
	memcpy(data.mac,mac,MAC_ADDRESS_LEN);
    data.vid = vid;
	
    /*hash_return(arp_snoop_db_s,&data,&item,1,npd_arp_snooping_compare_bymac_vid);*/
	item = hash_search(arp_snoop_db_s,&data,npd_arp_snooping_compare_bymac_vid,ARP_HASH_KEY_VID_MAC);
	if(NULL == item) {
		return item; /* return null pointer*/
	}

	if(item->ifIndex != ifIndex) {
		return NULL;
	}

	if(item->dstIntf.isTrunk) {
		syslog_ax_arpsnooping_dbg("found arp %d.%d.%d.%d on trunk(%d,%d) vlan %d if %d",	\
				(item->ipAddr >> 24) & 0xFF,(item->ipAddr >> 16) & 0xFF,	\
				(item->ipAddr >> 8) & 0xFF,item->ipAddr & 0xFF,	\
				item->dstIntf.intf.trunk.devNum,	\
				item->dstIntf.intf.trunk.trunkId,item->vid,item->ifIndex);
	}
	else {
		syslog_ax_arpsnooping_dbg("found arp %d.%d.%d.%d on port(%d,%d) vlan %d if %d",	\
				(item->ipAddr >> 24) & 0xFF,(item->ipAddr >> 16) & 0xFF,	\
				(item->ipAddr >> 8) & 0xFF,item->ipAddr & 0xFF,	\
				item->dstIntf.intf.port.devNum,	\
				item->dstIntf.intf.port.portNum,item->vid,item->ifIndex);
	}

	return item;
}

/**********************************************************************************
 * npd_arp_snooping_find_item_byip
 *
 * find ARP snooping item by ip address
 *
 *	INPUT:
 *		ifIndex - L3 interface index
 *		ipAddr - ip address
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NULL - not found item matched ifindex,ip address
 *		item - ARP snooping database item
 *
 *	NOTE:
 *
 **********************************************************************************/
struct arp_snooping_item_s *npd_arp_snooping_find_item_byip
(
	unsigned int  ifIndex,
	unsigned int  ipAddr
)
{
	struct arp_snooping_item_s data,*item = NULL;

	memset(&data,0,sizeof(struct arp_snooping_item_s));
	data.ifIndex = ifIndex;
	data.ipAddr = ipAddr;

	item = hash_search(arp_snoop_db_s,&data,npd_arp_snooping_compare_byip,ARP_HASH_KEY_IP);
	if(NULL == item) {
		/*zhangcl@autelan.com clean the print messages*/
		#if 0
		syslog_ax_arpsnooping_dbg("not found arp item of %d.%d.%d.%d\n", \
			(ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF,(ipAddr>>8)&0xFF,(ipAddr)&0xFF);
		#endif
		return item; /* return null pointer*/
	}

	if(item->ifIndex != ifIndex) {
		syslog_ax_arpsnooping_dbg("arp snooping's ifIndex not match\n");
		return NULL;
	}

	syslog_ax_arpsnooping_dbg("found arp item %d.%d.%d.%d on dev %d port %d vlan %d if %d",	\
			(item->ipAddr >> 24) & 0xFF,(item->ipAddr >> 16) & 0xFF,	\
			(item->ipAddr >> 8) & 0xFF,item->ipAddr & 0xFF,	\
			item->dstIntf.intf.port.devNum,	\
			item->dstIntf.intf.port.portNum,item->vid,item->ifIndex);
	return item;
}


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
 *		NULL - not found item matched ip address only
 *		item - ARP snooping database item
 *
 *	NOTE:
 *
 **********************************************************************************/
struct arp_snooping_item_s *npd_arp_snooping_find_item_byip_only
(
	unsigned int  ipAddr
)
{
	struct arp_snooping_item_s data,*item = NULL;

	memset(&data,0,sizeof(struct arp_snooping_item_s));
	data.ipAddr = ipAddr;

	item = hash_search(arp_snoop_db_s,&data,npd_arp_snooping_compare_byip,ARP_HASH_KEY_IP);
	if(NULL == item) {
		/*zhangcl@autelan.com clean the print messages*/
		#if 0
		syslog_ax_arpsnooping_dbg("not found arp item of %d.%d.%d.%d by ip only\n", \
			(ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF,(ipAddr>>8)&0xFF,(ipAddr)&0xFF);
		#endif
		return item; /* return null pointer*/
	}

	syslog_ax_arpsnooping_dbg("found arp item %d.%d.%d.%d on dev %d port %d vlan %d if %d  by ip only\n",	\
			(item->ipAddr >> 24) & 0xFF,(item->ipAddr >> 16) & 0xFF,	\
			(item->ipAddr >> 8) & 0xFF,item->ipAddr & 0xFF,	\
			item->dstIntf.intf.port.devNum,	\
			item->dstIntf.intf.port.portNum,item->vid,item->ifIndex);
	return item;
}

/**********************************************************************************
 * npd_arp_snooping_find_item_bymacip
 *
 * find ARP snooping item by mac address
 *
 *	INPUT:
 *		ifIndex - L3 interface index
 *		mac - mac address
 *		ipaddr - source ip address
 *	
 *	OUTPUT:
 *
 * 	RETURN:
 *		NULL - not found item matched ifindex,mac and ip address
 *		item - ARP snooping database item
 *
 *	NOTE:
 *
 **********************************************************************************/
struct arp_snooping_item_s *npd_arp_snooping_find_item_bymacip
(
	unsigned int  ifIndex,
	unsigned char *mac,
	unsigned int ipaddr
)
{
	struct arp_snooping_item_s data,*item = NULL;

	memset(&data,0,sizeof(struct arp_snooping_item_s));
	memcpy(data.mac,mac,MAC_ADDRESS_LEN);
	data.ipAddr = ipaddr;

	item = hash_search(arp_snoop_db_s,&data,npd_arp_snooping_compare_bymac,ARP_HASH_KEY_IP);
	if(NULL == item) {
		return item; /* return null pointer*/
	}

	if(item->ifIndex != ifIndex) {
		return NULL;
	}

	if(item->dstIntf.isTrunk) {
		syslog_ax_arpsnooping_dbg("found arp %d.%d.%d.%d on trunk(%d,%d) vlan %d if %d\r\n",	\
				(item->ipAddr >> 24) & 0xFF,(item->ipAddr >> 16) & 0xFF,	\
				(item->ipAddr >> 8) & 0xFF,item->ipAddr & 0xFF,	\
				item->dstIntf.intf.trunk.devNum,	\
				item->dstIntf.intf.trunk.trunkId,item->vid,item->ifIndex);
	}
	else {
		syslog_ax_arpsnooping_dbg("found arp %d.%d.%d.%d on port(%d,%d) vlan %d if %d\r\n",	\
				(item->ipAddr >> 24) & 0xFF,(item->ipAddr >> 16) & 0xFF,	\
				(item->ipAddr >> 8) & 0xFF,item->ipAddr & 0xFF,	\
				item->dstIntf.intf.port.devNum,	\
				item->dstIntf.intf.port.portNum,item->vid,item->ifIndex);
	}
	return item;
}

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
 *		dupIfindex - the old ifindex if exists
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		ARP_RETURN_CODE_TABLE_FULL  - the hash table is full
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ARP_RETURN_CODE_TBLINDEX_GET_FAILED - table index get failed 
 *		ARP_RETURN_CODE_NAM_ERROR - asic operation failed
 *
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_learning
(
	struct arp_snooping_item_s *item,
	unsigned int	*nextHopTblIndex,
	unsigned int *dupIfindex
)
{
	unsigned int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned int tblIndex = 0;
	struct route_nexthop_brief_s *backet = NULL;
	struct arp_snooping_item_s dupItem,*tmpItem = NULL;
	memset(&dupItem,0,sizeof(struct arp_snooping_item_s));
	syslog_ax_arpsnooping_dbg(" learning, istag %d isstatic %d isvalid %d\n",item->isTagged,item->isStatic,item->isValid);
	ret = npd_arp_snooping_op_item(item,ARP_SNOOPING_ADD_ITEM,&dupItem);
	if(ARP_RETURN_CODE_SUCCESS == ret ) {
		tmpItem = item;
		*dupIfindex = item->ifIndex;
	}
	else if(ARP_RETURN_CODE_DUPLICATED == ret) {
		tmpItem = item;
		/*by zhubo@autelan.com 2008.8.11 change MAC key to IP key*/
		*dupIfindex = dupItem.ifIndex;
		syslog_ax_arpsnooping_dbg("npd add duplicated ,update other info");
	}
	else if(ARP_RETURN_CODE_STATIC_EXIST == ret){
		syslog_ax_arpsnooping_dbg("npd static arp found when learning \n");
		tmpItem = &dupItem;
		*dupIfindex = tmpItem->ifIndex;
	}
	else {
		syslog_ax_arpsnooping_err("npd add item error %d",ret);
		return ret;
	}

	/* TODO: find out if we have occupied an Next-Hop table entry before*/
	pthread_mutex_lock(&nexthopHashMutex);
	backet = npd_route_nexthop_find(*dupIfindex,tmpItem->ipAddr);
	if(NULL == backet) { /* Next-Hop info not learned before,add to HW table*/				
	    pthread_mutex_lock(&namItemOpMutex);
		ret = nam_arp_snooping_op_item(tmpItem,ARP_SNOOPING_ADD_ITEM,&tblIndex);
		pthread_mutex_unlock(&namItemOpMutex);
		pthread_mutex_unlock(&nexthopHashMutex);
		if(ARP_RETURN_CODE_SUCCESS != ret ) {
			syslog_ax_arpsnooping_err("nam add item error %d",ret);
			npd_arp_snooping_op_item(item,ARP_SNOOPING_DEL_ITEM,&dupItem);
			return ret;
		}
	}
	else { /* Next-Hop info have occupied an table entry before, update it!!!*/
		tblIndex = backet->tblIndex;			
		pthread_mutex_lock(&namItemOpMutex);
		ret = nam_arp_snooping_op_item(tmpItem,ARP_SNOOPING_UPDATE_ITEM,&tblIndex);
		pthread_mutex_unlock(&namItemOpMutex);		
		pthread_mutex_unlock(&nexthopHashMutex);
		if(ARP_RETURN_CODE_SUCCESS != ret) {
			syslog_ax_arpsnooping_err("nam update item error %d",ret);
			npd_arp_snooping_op_item(item,ARP_SNOOPING_DEL_ITEM,&dupItem);
			return ret;
		}
	}

	*nextHopTblIndex = tblIndex;
	syslog_ax_arpsnooping_dbg("npd add item at hw nexthop table %d ok",tblIndex);
	return ARP_RETURN_CODE_SUCCESS;	
}

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
 *		NONE
 *
 * 	RETURN:		
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ARP_RETURN_CODE_TBLINDEX_GET_FAILED - table index get failed 
 *		ARP_RETURN_CODE_NAM_ERROR - asic operation failed
 *
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_aging_process
(
	struct neigh_tbl_info *neighEntry
)
{
	unsigned int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned int tblIndex = 0;
	struct route_nexthop_brief_s *backet = NULL,item;
#if 0	
	ret = npd_arp_snooping_op_item(&item,ARP_SNOOPING_ADD_ITEM);
	if(ARP_RETURN_CODE_SUCCESS != ret ) {
		NPD_ERROR(("npd add arp snooping item error %d",ret));
		return ret;
	}
	NPD_DEBUG(("npd add arp snooping item ok"));

	// TODO: find out if we have occupied an Next-Hop table entry before
	backet = npd_route_nexthop_find(item.ifIndex,item.ipAddr);
	if(NULL == backet) { // Next-Hop info not learned before,add to HW table
		ret = nam_arp_snooping_op_item(item,ARP_SNOOPING_ADD_ITEM,&tblIndex);
		if(ARP_RETURN_CODE_SUCCESS != ret ) {
			NPD_ERROR(("nam add arp snooping item error %d",ret));
			return ret;
		}
	}
	else { // Next-Hop info have occupied an table entry before, update it!!!
		tblIndex = backet->tblIndex;
		ret = nam_arp_snooping_op_item(&item,ARP_SNOOPING_UPDATE_ITEM,&tblIndex);
		if(ARP_RETURN_CODE_SUCCESS != ret) {
			NPD_ERROR(("nam update arp snooping item error %d",ret));
			return ret;
		}
	}	
#endif
	NPD_DEBUG(("npd add arp snooping item at %d ok",tblIndex));
	return ARP_RETURN_CODE_SUCCESS;	
}

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
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		ARP_RETURN_CODE_TABLE_FULL  - the hash table is full
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ARP_RETURN_CODE_TBLINDEX_GET_FAILED - table index get failed 
 *		ARP_RETURN_CODE_NAM_ERROR - asic operation failed
 *		ARP_RETURN_CODE_ERROR  -  check failed drop the packet or route operation failed
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
)
{
	struct ether_header_t *layer2 = NULL;
	struct arp_packet_t	   *arpPacket = NULL;
	struct arp_snooping_item_s dbItem;
	struct route_nexthop_brief_s nextHopItem;
	unsigned int sip = 0,dip = 0;
	unsigned char devNum = 0,portNum = 0;
	unsigned int  nextHopTblIndex = 0;
	unsigned int retVal = ARP_RETURN_CODE_SUCCESS;
	unsigned int dupIfindex = 0; /*by zhubo@autelan.com when arp duplicated ,return old item ifindex*/
	unsigned int gateway[MAX_IP_COUNT]={INVALID_HOST_IP};
	unsigned int mask[MAX_IP_COUNT]={0};
	int i = 0;
	NAM_RSTP_PORT_STATE_E  state = ~0UL;
	unsigned char dipmac[MAC_ADDR_LEN] = {0};
	unsigned long arp_len = 0;
	int ret = 0;
	NPD_DHCP_SNP_SHOW_ITEM_T result;
	unsigned char isFound = 0;

	memset(&result,0,sizeof(NPD_DHCP_SNP_SHOW_ITEM_T));

	memset(&state,0,sizeof(NAM_RSTP_PORT_STATE_E));
	if(NULL == packet) {
		syslog_ax_arpsnooping_err("npd process arp packet as packet null");
		return COMMON_RETURN_CODE_NULL_PTR;
	}
	
	arp_len = sizeof(struct ether_header_t) + sizeof(struct arp_packet_t);

	if(length < arp_len) {
		syslog_ax_arpsnooping_dbg("npd found arp packet too short:length %d while arp at least %d bytes\n",length,arp_len);
	}
	layer2 = (struct ether_header_t *)packet;
	
	/* TODO:check mac address*/
	if(layer2->smac[0] & 0x1) {
		syslog_ax_arpsnooping_dbg("npd got arp packet source mac multicast %02x:%02x:%02x:%02x:%02x:%02x\n",	\
					layer2->smac[0],layer2->smac[1],layer2->smac[2],layer2->smac[3],layer2->smac[4],layer2->smac[5]);
		return ARP_RETURN_CODE_ERROR;
	}
	if(ETHER_ARP_TYPE != layer2->etherType) {
		syslog_ax_arpsnooping_dbg("npd got bad ether type %#0x for arp",layer2->etherType);
		return ARP_RETURN_CODE_ERROR;
	}
	
	arpPacket = (struct arp_packet_t *)((char*)packet + sizeof(struct ether_header_t));

	/*
	* by zhubo@autelan.com 2008.08.06
	* before learning  arp , check ethernet header SMAC and ARP SMAC 
	* only learn arp request
	*/
	if(ARP_REQUEST != arpPacket->opCode) {
		syslog_ax_arpsnooping_dbg("npd got bad arp opcode %#0x for arp\n",arpPacket->opCode);
	}

	if(TRUE == arp_smac_check){ /*if set enable*/
		retVal = npd_arp_snooping_smac_check(layer2->smac,arpPacket->smac);  /*smac is the same as the sender mac*/
		if(FALSE == retVal)
			return ARP_RETURN_CODE_ERROR;
	}
	if(TRUE == arp_strict_check){
		retVal= npd_arp_snooping_mac_legality_check(layer2->smac,layer2->dmac, \
			                                arpPacket->smac,arpPacket->dmac,\
			                                arpPacket->opCode);  /*the four macs legality check (muticast ,broadcast,zero mac ,etc)*/
		if(TRUE != retVal){
			syslog_ax_arpsnooping_err("arp packet mac legality check error \n");
	        return ARP_RETURN_CODE_ERROR;
		}
	}
	sip = IP_CONVERT_STR2ULONG(arpPacket->sip);
	dip = IP_CONVERT_STR2ULONG(arpPacket->dip);
	syslog_ax_arpsnooping_dbg("***********************************\n");
	syslog_ax_arpsnooping_dbg("%-15s:%-d(%-d,%s)\n","intf(l3if)",	\
			intfId,ifIndex,(ARP_INTF_SRC_ETHPORT_E == srcType) ? "port":	\
			(ARP_INTF_SRC_TRUNK_E == srcType) ? "trunk":"other");
	syslog_ax_arpsnooping_dbg("%-15s:%-10d\n","vid",vid);
	syslog_ax_arpsnooping_dbg("%-15s:%-10s\n","isTagged",isTagged ? "true":"false");
	syslog_ax_arpsnooping_dbg("%-15s:%d.%d.%d.%d\n","sender ip",	\
					(sip>>24) & 0xFF,(sip>>16) & 0xFF,(sip>>8) & 0xFF,sip & 0xFF);
	syslog_ax_arpsnooping_dbg("%-15s:%02x:%02x:%02x:%02x:%02x:%02x\n","sender mac",	\
			arpPacket->smac[0],arpPacket->smac[1],arpPacket->smac[2],	\
			arpPacket->smac[3],arpPacket->smac[4],arpPacket->smac[5]);
	syslog_ax_arpsnooping_dbg("%-15s:%d.%d.%d.%d\n","target ip", \
					(dip>>24) & 0xFF,(dip>>16) & 0xFF,(dip>>8) & 0xFF,dip & 0xFF);
	syslog_ax_arpsnooping_dbg("%-15s:%02x:%02x:%02x:%02x:%02x:%02x\n","target mac",	\
			arpPacket->dmac[0],arpPacket->dmac[1],arpPacket->dmac[2],	\
			arpPacket->dmac[3],arpPacket->dmac[4],arpPacket->dmac[5]);
	syslog_ax_arpsnooping_dbg("***********************************\n");
	
	retVal = npd_intf_get_ip_addrs(ifIndex,gateway,mask);
	if(INVALID_HOST_IP == gateway[0]){
        syslog_ax_arpsnooping_err("no ip address found in this interface,ifIndex %#0x \n",ifIndex);
		return ARP_RETURN_CODE_ERROR;
	}	
	if(ARP_RETURN_CODE_SUCCESS != retVal){
        syslog_ax_arpsnooping_err("get interface ips error,ifIndex %#0x \n",ifIndex);
		return ARP_RETURN_CODE_ERROR;
	}
	if(ARP_RETURN_CODE_ERROR == npd_arp_snooping_ip_check(sip,gateway,mask)) { /*normal host ip*/
		syslog_ax_arpsnooping_err("bad source ip %d.%d.%d.%d \n", \
			(sip>>24) & 0xFF,(sip>>16) & 0xFF,(sip>>8) & 0xFF,sip & 0xFF);
		return ARP_RETURN_CODE_ERROR;
	}
	if(ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_ip_collision_check(sip,gateway)){/*ip collision*/
		syslog_ax_arpsnooping_err("ip address %d.%d.%d.%d collided with %02x:%02x:%02x:%02x:%02x:%02x !\n",\
			(sip>>24) & 0xFF,(sip>>16) & 0xFF,(sip>>8) & 0xFF,sip & 0xFF,\
			arpPacket->smac[0],arpPacket->smac[1],arpPacket->smac[2],arpPacket->smac[3],arpPacket->smac[4],arpPacket->smac[5]);
		return ARP_RETURN_CODE_ERROR;
	}
	if(arp_inspection){
		if((DHCP_SNP_RETURN_CODE_OK != npd_dhcp_snp_query_arp_inspection(sip,&isFound))||(!isFound)){/*&& untrust*/
            syslog_ax_arpsnooping_err("unknown ip arp request %d.%d.%d.%d \n", \
			(sip>>24) & 0xFF,(sip>>16) & 0xFF,(sip>>8) & 0xFF,sip & 0xFF);
			return ARP_RETURN_CODE_ERROR;
		}
	}
	if((ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_dip_check(dip,gateway))&&(sip != dip)) { /*arp destination ip is not the intf ip and not gratuitous arp solicit*/
		syslog_ax_arpsnooping_dbg("dip %d.%d.%d.%d is not in the interface ifIndex %#0x \n", \
			(dip>>24) & 0xFF,(dip>>16) & 0xFF,(dip>>8) & 0xFF,dip & 0xFF,ifIndex);
		if(arp_proxy){
			if(DHCP_SNP_RETURN_CODE_OK == npd_dhcp_snp_query_arp_proxy(dip,&result)){
				if ((NPD_SUCCESS == npd_get_devport_by_global_index(intfId,&devNum,&portNum)) &&
					(ARP_RETURN_CODE_SUCCESS == npd_arpsnooping_analyse_mac(result.chaddr,dipmac)))
				{
					ret = nam_arp_reply_drv_send(devNum,portNum,vid,isTagged,dipmac,arpPacket->smac,dip,sip);
				    if(ARP_RETURN_CODE_SUCCESS != ret){
						syslog_ax_arpsnooping_err("arp reply send error, ret %d \n",ret);
				    }
					else{
						syslog_ax_arpsnooping_dbg("arp reply send form %d.%d.%d.%d to %d.%d.%d.%d \n ",
							(dip>>24)&0xff,(dip>>16)&0xff,(dip>>8)&0xff,dip&0xff,\
							(sip>>24)&0xff,(sip>>16)&0xff,(sip>>8)&0xff,sip&0xff);
					}
				}
			}
			/*
			else{
				if(untrust){
					drop;
				}
				else{
                              forward;
				}
			}*/
		}
		return ARP_RETURN_CODE_ERROR;
	}
	if(sip == dip){
        syslog_ax_arpsnooping_dbg("gratuitous arp solicit ip %d.%d.%d.%d , ifIndex %#0x \n", \
			(dip>>24) & 0xFF,(dip>>16) & 0xFF,(dip>>8) & 0xFF,dip & 0xFF,ifIndex);
	}
	if(!arp_for_muti_network){
		if(ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_mask32_dip_gateway_check(dip,gateway,mask)){
			if(ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_ip_gateway_check(sip,gateway,mask)) {/* the sip in one subnet of the intf*/
				syslog_ax_arpsnooping_err("sip %d.%d.%d.%d ifIndex %#0x gateway check error\n", \
					(sip>>24) & 0xFF,(sip>>16) & 0xFF,(sip>>8) & 0xFF,sip & 0xFF,ifIndex);
				return ARP_RETURN_CODE_ERROR;
			}

			if(ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_ip_subnet_check(sip,dip,gateway,mask)) {/* the sip and the dip in one subnet of the intf*/
				syslog_ax_arpsnooping_err("arp is illegal: sourse ip %#0x and destination ip %#0x not same subnet\n",sip,dip);
				return ARP_RETURN_CODE_ERROR;
		    }
		}
    }
	/* TODO:Build up arp snooping DB item*/
	memset(&dbItem, 0, sizeof(struct arp_snooping_item_s));
	dbItem.ifIndex 	= ifIndex;
	dbItem.isStatic = FALSE;
	dbItem.isValid = TRUE;
	dbItem.ipAddr 	= sip;	/* usually we learning source ip address*/
	dbItem.isTagged = isTagged;
	dbItem.vid		= vid;
	memcpy(dbItem.mac,arpPacket->smac,MAC_ADDRESS_LEN);
	if(ARP_INTF_SRC_ETHPORT_E == srcType) {
		dbItem.dstIntf.isTrunk = FALSE;
		npd_get_devport_by_global_index(intfId,&devNum,&portNum);
		dbItem.dstIntf.intf.port.devNum 	= devNum;
		dbItem.dstIntf.intf.port.portNum	= portNum;
	}
	else if(ARP_INTF_SRC_TRUNK_E == srcType){
		dbItem.dstIntf.isTrunk = TRUE;
		dbItem.dstIntf.intf.trunk.devNum = devNum;
		dbItem.dstIntf.intf.trunk.trunkId = intfId;
	}
	
    /*codeby zhengcs@autelan.com at 20081223,check the vlan port stp state,if discarding, do not learn
	   now only support port since stp haven`t support trunk*/
	if(ARP_INTF_SRC_ETHPORT_E == srcType) {
		retVal = npd_mstp_get_port_state(vid,intfId,&state);/*input vid & port index (intfId)*/
		if((0 == retVal)&&( NAM_STP_PORT_STATE_DISCARD_E == state)){
	        npd_syslog_dbg("The vlan %d eth_g_index %d stp state is discarding\n",vid,intfId);
			return ARP_RETURN_CODE_ERROR;
		}
	}
	pthread_mutex_lock(&arpHashMutex);
	if((sip == dip)&&(NULL == hash_search(arp_snoop_db_s,&dbItem,NULL,ARP_HASH_KEY_IP))){
		npd_syslog_dbg("a strange gratuitous arp solicit,drop it!\n");
		pthread_mutex_unlock(&arpHashMutex);
		return ARP_RETURN_CODE_ERROR;
	}
	retVal = npd_arp_snooping_learning( &dbItem ,&nextHopTblIndex,&dupIfindex );
	pthread_mutex_unlock(&arpHashMutex);
	if(ARP_RETURN_CODE_SUCCESS == retVal) {
		/* TODO: build route table hash*/
		memset(&nextHopItem, 0, sizeof(struct route_nexthop_brief_s));
		nextHopItem.ifIndex = ifIndex;
		nextHopItem.ipAddr 	= sip;
		nextHopItem.tblIndex= nextHopTblIndex;
		pthread_mutex_lock(&nexthopHashMutex);
		retVal = npd_route_nexthop_op_item(&nextHopItem,NEXTHOP_ADD_ITEM,dupIfindex);
		pthread_mutex_unlock(&nexthopHashMutex);
		if(ROUTE_RETURN_CODE_SUCCESS != retVal) {
			syslog_ax_arpsnooping_err("npd nexthop brief add to db error %d",retVal);
			return ARP_RETURN_CODE_ERROR;
		}
		else{
			retVal = ARP_RETURN_CODE_SUCCESS;
		}

		/* TODO:add host route to RT*/
		retVal = npd_route_table_op_host_item(&nextHopItem,ROUTE_ADD_ITEM);
		if(ROUTE_RETURN_CODE_SUCCESS!= retVal) {
			syslog_ax_arpsnooping_err("npd route host add to HW error %d",retVal);
			return ARP_RETURN_CODE_ERROR;
		}
		else{
			retVal = ARP_RETURN_CODE_SUCCESS;
		}
		
	}
	else {
		syslog_ax_arpsnooping_err("npd arp snooping learning error %d\n",retVal);
		return retVal;
	}
	return ARP_RETURN_CODE_SUCCESS;
}
/**************************************************
 *npd_arpsnooping_analyse_mac
 *  get mac Address from mac string
 * 
 * INPUT:
 *		macStr  - mac address string ,eg. "000102030405"
 * OUTPUT:
 *		macAddr  - the mac address we get by analyse 
 *					eg. {0x00,0x01,0x02,0x03,0x04,0x05}
 * RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if analyse success
 *		ARP_RETURN_CODE_ERROR  - if the string is illegal
 *
 **************************************************/
unsigned int npd_arpsnooping_analyse_mac
(
    unsigned char *macStr,
    unsigned char *macAddr
)
{
	int i = 0;
    unsigned char cur = '\0';
	unsigned int value = 0;
	
	for( i = 0;i < 12;i++ ) {
		   cur = macStr[i];
		   if((cur >= '0') &&(cur <='9')) {
			   value = cur - '0';
		   }
		   else if((cur >= 'A') &&(cur <='F')) {
			   value = cur - 'A';
			   value += 0xa;
		   }
		   else if((cur >= 'a') &&(cur <='f')) {
			   value = cur - 'a';
			   value += 0xa;
		   }
		   else { /* illegal character found*/
			   return ARP_RETURN_CODE_ERROR;
		   }
	
		   if(0 == i%2) {
			   macAddr[i/2] = value;
		   }
		   else {
			   macAddr[i/2] <<= 4;
			   macAddr[i/2] |= value;
		   }
	   }
	return ARP_RETURN_CODE_SUCCESS;

}

/******************************************************************
 * npd_dbus_arp_muti_network_enable
 * 		arp for different network support. (eg. form 10.1.1.1/24 to 20.2.2.2/24)
 * INPUT:
 *		uint32 - arp for muti network support or not
 *				1 - not support
 *				0 - support
 * OUTPUT:
 *		uint32 - return code 
 *				ARP_RETURN_CODE_SUCCESS - set success
 * RETURN:
 *		NULL - get args failed
 *		reply - set success
 *
 *******************************************************************/
DBusMessage * npd_dbus_arp_muti_network_enable
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter  iter;
	unsigned int isEnable = 2;
	unsigned int ret = ARP_RETURN_CODE_SUCCESS;		

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&isEnable,
		DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if(isEnable == 1){
		arp_for_muti_network = FALSE;
	}
	else{
		arp_for_muti_network = TRUE;
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	return reply;
}


/*****************************************************
 * npd_dbus_arp_inspection_enable
 * 		set arp inspection enable or disbale
 * INPUT:
 *		uint32 - set arp inspection enable or disable
 *				1 - enable
 *				0 - disable
 * OUTPUT:
 *		uint32 - return code
 *				ARP_RETURN_CODE_SUCCESS  -set success
 * RETURN:
 *		NULL - get args failed
 *		reply - set success
 *		
 *****************************************************/
DBusMessage * npd_dbus_arp_inspection_enable(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter  iter;
	unsigned int isEnable = 2;
	unsigned int ret = 0;
	unsigned int devNum = 0;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&isEnable,
		DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if(isEnable == 1){
		syslog_ax_arpsnooping_dbg("arp inspection is set enable!\n");
		arp_inspection= TRUE;
		nam_arp_broadcast_to_cpu_cmd_set(devNum,PACKET_CMD_TRAP_TO_CPU_E);
	}
	else{
		syslog_ax_arpsnooping_dbg("arp inspection is set disable!\n");
		arp_inspection = FALSE;
		if(!arp_proxy){            
			nam_arp_broadcast_to_cpu_cmd_set(devNum,PACKET_CMD_MIRROR_TO_CPU_E);
		}
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	return reply;
}
	
	/*****************************************************
	 * npd_dbus_arp_inspection_enable
	 *		set arp proxy enable or disbale
	 * INPUT:
	 *		uint32 - set arp proxy enable or disable
	 *				1 - enable
	 *				0 - disable
	 * OUTPUT:
	 *		uint32 - return code
	 *				ARP_RETURN_CODE_SUCCESS  -set success
	 * RETURN:
	 *		NULL - get args failed
	 *		reply - set success
	 *		
	 *****************************************************/

DBusMessage * npd_dbus_arp_proxy_enable(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter  iter;
	unsigned int isEnable = 2;
	unsigned int ret = 0;	
	unsigned int devNum = 0;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&isEnable,
		DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if(isEnable == 1){
		syslog_ax_arpsnooping_dbg("arp proxy is set enable!\n");
		arp_proxy = TRUE;
		nam_arp_broadcast_to_cpu_cmd_set(devNum,PACKET_CMD_TRAP_TO_CPU_E);
	}
	else{		
		syslog_ax_arpsnooping_dbg("arp proxy is set disable!\n");
		arp_proxy = FALSE;
		if(!arp_inspection){
			nam_arp_broadcast_to_cpu_cmd_set(devNum,PACKET_CMD_MIRROR_TO_CPU_E);
		}
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	return reply;
}
		

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
 *		FALSE - if ARP item has different port info from data.
 *		TRUE - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_by_port
(
	struct arp_snooping_item_s *item,
	struct	fdb_interface_info_s *data
)
{
	struct fdb_interface_info_s *comparator = NULL;

	if(NULL == item) {
		return FALSE;
	}
	if(NULL == data) {
		return TRUE;
	}
	
	comparator = &(item->dstIntf);
	if(TRUE == data->isTrunk) {
		return FALSE;
	}
	else if(data->isTrunk != comparator->isTrunk) {
		return FALSE;
	}
	else if(data->intf.port.devNum != comparator->intf.port.devNum) {
		return FALSE;
	}
	else if(data->intf.port.portNum != comparator->intf.port.portNum) {
		return FALSE;
	}
	else { /* all matched */
		return TRUE;
	}
}

/*********************************************************************
 * npd_arp_filter_by_trunk
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
 *		FALSE - if ARP item has different port info from data.
 *		TRUE - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 *********************************************************************/
unsigned int npd_arp_filter_by_trunk
(
	struct arp_snooping_item_s *item,
	struct	fdb_interface_info_s *data
)
{
	struct fdb_interface_info_s *comparator = NULL;

	if(NULL == item) {
		return FALSE;
	}
	if(NULL == data) {
		return TRUE;
	}
	
	comparator = &(item->dstIntf);
	if(TRUE != data->isTrunk) {
		return FALSE;
	}
	else if(data->isTrunk != comparator->isTrunk) {
		return FALSE;
	}
	else if(data->intf.trunk.trunkId != comparator->intf.trunk.trunkId) {
		return FALSE;
	}
	else { /* all matched */
		return TRUE;
	}
}

/**********************************************************************************
 * npd_arp_filter_by_port_ifIndex
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info and interface index used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE - if ARP item has different port info from data.
 *		TRUE - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_by_port_ifIndex
(
	struct arp_snooping_item_s *item,
	struct arp_snooping_item_s *data
)
{
	struct fdb_interface_info_s *comparatorA = NULL;
	struct fdb_interface_info_s *comparatorB = NULL;


	if(NULL == item) {
		return FALSE;
	}
	
	/*specially using,return all arp item*/
	if(NULL == data) {
		return TRUE;
	}
	
	comparatorA = &(item->dstIntf);
	comparatorB = &(data->dstIntf);
	if(TRUE == comparatorB->isTrunk) {
		return FALSE;
	}
	else if(item->ifIndex != data->ifIndex){
		return FALSE;
	}
	else if(comparatorB->isTrunk != comparatorA->isTrunk) {
		return FALSE;
	}
	else if(comparatorB->intf.port.devNum != comparatorA->intf.port.devNum) {
		return FALSE;
	}
	else if(comparatorB->intf.port.portNum != comparatorA->intf.port.portNum) {
		return FALSE;
	}
	else { /* all matched */
		return TRUE;
	}
}


/**********************************************************************************
 * npd_arp_filter_by_trunk_ifIndex
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info and interface index used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE - if ARP item has different port info from data.
 *		TRUE - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_by_trunk_ifIndex
(
	struct arp_snooping_item_s *item,
	struct arp_snooping_item_s *data
)
{
	struct fdb_interface_info_s *comparatorA = NULL;
	struct fdb_interface_info_s *comparatorB = NULL;


	if(NULL == item) {
		return FALSE;
	}
	
	/*specially using,return all arp item*/
	if(NULL == data) {
		return TRUE;
	}
	
	comparatorA = &(item->dstIntf);
	comparatorB = &(data->dstIntf);
	if(TRUE != comparatorB->isTrunk) {
		return FALSE;
	}
	else if(item->ifIndex != data->ifIndex){
		return FALSE;
	}
	else if(comparatorB->isTrunk != comparatorA->isTrunk) {
		return FALSE;
	}
	else if(comparatorB->intf.trunk.trunkId != comparatorA->intf.trunk.trunkId) {
		return FALSE;
	}
	else { /* all matched */
		return TRUE;
	}
}

/**********************************************************************************
 * npd_arp_filter_by_port_vid
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info and interface index used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE - if ARP item has different port info from data.
 *		TRUE - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_by_port_vid
(
	struct arp_snooping_item_s *item,
	struct arp_snooping_item_s *data
)
{
	struct fdb_interface_info_s *comparatorA = NULL;
	struct fdb_interface_info_s *comparatorB = NULL;


	if(NULL == item) {
		return FALSE;
	}
	
	/*specially using,return all arp item*/
	if(NULL == data) {
		return TRUE;
	}
	
	comparatorA = &(item->dstIntf);
	comparatorB = &(data->dstIntf);
	if(TRUE == comparatorB->isTrunk) {
		return FALSE;
	}
	else if(item->vid != data->vid){
		return FALSE;
	}
	else if(comparatorB->isTrunk != comparatorA->isTrunk) {
		return FALSE;
	}
	else if(comparatorB->intf.port.devNum != comparatorA->intf.port.devNum) {
		return FALSE;
	}
	else if(comparatorB->intf.port.portNum != comparatorA->intf.port.portNum) {
		return FALSE;
	}
	else { /* all matched */
		return TRUE;
	}
}

/**********************************************************************************
 * npd_arp_filter_by_port_vid
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info and interface index used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE - if ARP item has different port info from data.
 *		TRUE - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_by_trunk_vid
(
	struct arp_snooping_item_s *item,
	struct arp_snooping_item_s *data
)
{
	struct fdb_interface_info_s *comparatorA = NULL;
	struct fdb_interface_info_s *comparatorB = NULL;


	if(NULL == item) {
		return FALSE;
	}
	
	/*specially using,return all arp item*/
	if(NULL == data) {
		return TRUE;
	}
	
	comparatorA = &(item->dstIntf);
	comparatorB = &(data->dstIntf);
	if(TRUE != comparatorB->isTrunk) {
		return FALSE;
	}
	else if(item->vid != data->vid){
		return FALSE;
	}
	else if(comparatorB->isTrunk != comparatorA->isTrunk) {
		return FALSE;
	}
	else if(comparatorB->intf.trunk.trunkId != comparatorA->intf.trunk.trunkId) {
		return FALSE;
	}
	else { /* all matched */
		return TRUE;
	}
}

/**********************************************************************************
 * npd_valid_arp_filter_by_port
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
 *		FALSE - if ARP item has different port info from data.
 *		TRUE - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_valid_arp_filter_by_port
(
	struct arp_snooping_item_s *item,
	struct	fdb_interface_info_s *data
)
{
	struct fdb_interface_info_s *comparator = NULL;

	if(NULL == item) {
		return FALSE;
	}
	if((TRUE == item->isStatic)&& \
	   (FALSE == item->isValid)){
        return FALSE;
	}

	if(NULL == data) {
		return TRUE;
	}
	
	comparator = &(item->dstIntf);
	if(TRUE == data->isTrunk) {
		return FALSE;
	}
	else if(data->isTrunk != comparator->isTrunk) {
		return FALSE;
	}
	else if(data->intf.port.devNum != comparator->intf.port.devNum) {
		return FALSE;
	}
	else if(data->intf.port.portNum != comparator->intf.port.portNum) {
		return FALSE;
	}
	else { /* all matched */
		return TRUE;
	}
}

/**********************************************************************************
 * npd_valid_arp_filter_by_port_nexthop
 *
 *  	filter valid ARP snooping by eth port and the nexthop exists
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE - if ARP item has different port info from data.
 *		TRUE - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_valid_arp_filter_by_port_nexthop
(
	struct arp_snooping_item_s *item,
	struct	fdb_interface_info_s *data
)
{
    if(npd_valid_arp_filter_by_port(item,data)&&\
		(NULL != npd_route_nexthop_find(item->ifIndex,item->ipAddr))){
		return TRUE;
    }
	return FALSE;
}

/**********************************************************************************
 * npd_valid_arp_filter_by_trunk_nexthop
 *
 *  	filter valid ARP snooping by trunk and the nexthop exists
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE - if ARP item has different port info from data.
 *		TRUE - if ARP item has the same port info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_valid_arp_filter_by_trunk_nexthop
(
	struct arp_snooping_item_s *item,
	struct	fdb_interface_info_s *data
)
{
    if(npd_valid_arp_filter_by_trunk(item,data)&&\
		(NULL != npd_route_nexthop_find(item->ifIndex,item->ipAddr))){
		return TRUE;
    }
	return FALSE;
}

#if 0
/**********************************************************************************
 * npd_arp_filter_by_trunk
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
 *		FALSE - if ARP item has different trunk info from data.
 *		TRUE - if ARP item has the same trunk info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_by_trunk
(
	struct arp_snooping_item_s *item,
	struct	fdb_interface_info_s *data
)
{
	struct fdb_interface_info_s *comparator = NULL;

	/*specially using,return all arp item*/
	if(NULL == data) {
		return TRUE;
	}
	
	if(NULL == item) {
		return FALSE;
	}

	comparator = &(item->dstIntf);
	syslog_ax_arpsnooping_dbg("comparator isTrunk %d,data isTrunk %d\n" \
							,comparator->isTrunk,data->isTrunk);	
	syslog_ax_arpsnooping_dbg("comparator intf.trunk.devNum %d,data intf.trunk.devNum %d\n" \
							,comparator->intf.trunk.devNum,data->intf.trunk.devNum);
	syslog_ax_arpsnooping_dbg("comparator intf.trunk.trunkId %d,data intf.trunk.trunkId %d\n" \
							,comparator->intf.trunk.trunkId,data->intf.trunk.trunkId);	
	if(FALSE == comparator->isTrunk) {	
		return FALSE;
	}
	else if(data->isTrunk != comparator->isTrunk) {
		return FALSE;
	}
	else if(data->intf.trunk.devNum != comparator->intf.trunk.devNum) {
		return FALSE;
	}
	else if(data->intf.trunk.trunkId != comparator->intf.trunk.trunkId) {
		return FALSE;
	}
	else { // all matched 
		return TRUE;
	}
}
#endif
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
 *		FALSE - if ARP item has different trunk info from data.
 *		TRUE - if ARP item has the same trunk info as data.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_valid_arp_filter_by_trunk
(
	struct arp_snooping_item_s *item,
	struct	fdb_interface_info_s *data
)
{
	struct fdb_interface_info_s *comparator = NULL;

	if(NULL == item) {
		return FALSE;
	}
	if((TRUE == item->isStatic)&& \
	   (FALSE == item->isValid)){
        return FALSE;
	}	
	if(NULL == data) {
		return TRUE;
	}
	comparator = &(item->dstIntf);
	if(FALSE == comparator->isTrunk) {	
		return FALSE;
	}
	else if(data->isTrunk != comparator->isTrunk) {
		return FALSE;
	}
	else if(data->intf.trunk.trunkId != comparator->intf.trunk.trunkId) {
		return FALSE;
	}
	else { /* all matched*/ 
		return TRUE;
	}
}


/**********************************************************************************
 * npd_arp_filter_static_by_port
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
 *		FALSE - if ARP item has different port info from data or is not static.
 *		TRUE - if ARP item has the same port  info as data and is static.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_static_by_port
(
    struct arp_snooping_item_s *item,
    struct	fdb_interface_info_s *data
)
{
    if((TRUE == npd_arp_filter_by_port(item,data)&&(TRUE == item->isStatic))){
        return TRUE;
	}else{
	    return FALSE;
	}
}

/**********************************************************************************
 * npd_arp_filter_static_by_trunk
 *
 *  	filter static arp snooping by trunkId
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE - if ARP item has different port info from data or is not static.
 *		TRUE - if ARP item has the same port  info as data and is static.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_static_by_trunk
(
    struct arp_snooping_item_s *item,
    struct	fdb_interface_info_s *data
)
{
    if((TRUE == npd_arp_filter_by_trunk(item,data)&&(TRUE == item->isStatic))){
        return TRUE;
	}else{
	    return FALSE;
	}
}

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
 *		FALSE - if ARP item has different port info from data or is not static.
 *		TRUE - if ARP item has the same port  info as data and is static.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_static_by_port_vid
(
    struct arp_snooping_item_s *itemA,
    struct	arp_snooping_item_s *itemB
)
{
    if((TRUE == npd_arp_filter_by_port_vid(itemA,itemB)&&(TRUE == itemA->isStatic))){
        return TRUE;
	}else{
	    return FALSE;
	}
}
/**********************************************************************************
 * npd_arp_filter_static_by_trunk_vid
 *
 *  	filter static arp snooping by trunk and vid
 *
 *	INPUT:
 *		item - ARP snooping item info
 *		data - port info used as filter
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE - if ARP item has different port info from data or is not static.
 *		TRUE - if ARP item has the same port  info as data and is static.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_static_by_trunk_vid
(
    struct arp_snooping_item_s *itemA,
    struct	arp_snooping_item_s *itemB
)
{
    if((TRUE == npd_arp_filter_by_trunk_vid(itemA,itemB)&&(TRUE == itemA->isStatic))){
        return TRUE;
	}else{
	    return FALSE;
	}
}

/*********************************************************
 * npd_arp_filter_by_network
 * 
 * INPUT:
 *     itemA  -- arp item 
 *     ip_mask  -- ip_mask[0]  ip address,  ip_mask[1] mask
 * RETURN:
 *     TRUE -- if matched 
 *     FALSE -- not matched
 *
 **********************************************************/
unsigned int npd_arp_filter_by_network
(
    struct arp_snooping_item_s * itemA,
    unsigned int *ip_mask
)
{
    if((NULL == itemA)||(NULL == ip_mask)){
		return FALSE;
    }
	if(((itemA->ipAddr) & ip_mask[1]) == (ip_mask[0] & ip_mask[1])){
		return TRUE;
	}
	return FALSE;
}

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
 *		FALSE - if there are no static arp items in the eth_port.
 *		TRUE - if there are static arp items in the eth_port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_count_static_arp_in_port
(
    unsigned int eth_g_index
)
{
    unsigned int count=0;
	struct fdb_interface_info_s data;
	unsigned char devNum;
	unsigned char portNum;
	memset(&data, 0,sizeof(struct fdb_interface_info_s));
	npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	data.intf.port.devNum=devNum;
	data.intf.port.portNum=portNum;
	count=hash_traversal(arp_snoop_db_s,0,&data,npd_arp_filter_static_by_port,NULL);
	if(count==0){
         return FALSE;
	}else{
        return TRUE;
	}
}

/**********************************************************************************
 * npd_count_static_arp_in_port_vlan
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *		eth_g_index - eth_port index
 *		vlanId - the vlan id
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE - if there are no static arp items in the eth_port.
 *		TRUE - if there are static arp items in the eth_port.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_count_static_arp_in_port_vlan
(
    unsigned int eth_g_index,
    unsigned int vlanId
)
{
    unsigned int count=0;
	struct arp_snooping_item_s item;
	unsigned char devNum;
	unsigned char portNum;
	memset(&item, 0,sizeof(struct fdb_interface_info_s));
	npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	item.dstIntf.intf.port.devNum = devNum;
	item.dstIntf.intf.port.portNum = portNum;
	item.vid = vlanId;
	count=hash_traversal(arp_snoop_db_s,0,&item,npd_arp_filter_static_by_port_vid,NULL);
	if(count==0){
         return FALSE;
	}else{
        return TRUE;
	}
}

/**********************************************************************************
 * npd_count_all_static_arp
 *
 *  	filter ARP snooping by eth port
 *
 *	INPUT:
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		count - the count of static arp 
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_count_all_static_arp
(
    void
)
{
    unsigned int count = 0;
	unsigned char isStatic = TRUE;
	count = hash_traversal(arp_snoop_db_s,NULL,&isStatic,npd_arp_filter_by_static,NULL);
	return count;
}


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
)
{
    unsigned int count = 0;
	struct arp_snooping_item_s item;
    item.isStatic = TRUE;
    item.ifIndex = ifindex;
	count = hash_traversal(arp_snoop_db_s,NULL,&item,npd_arp_filter_by_static_ifindex,NULL);
	return count;
}


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
 *		FALSE - find out ARP item not matched
 *		TRUE - find out ARP item matched(with the same static attribute
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_filter_by_static
(
	struct arp_snooping_item_s *item,
	void* param
)
{
	unsigned char isStatic  = 0;
	
	if((NULL == item)||(NULL == param))
		return FALSE;

	isStatic = (unsigned char)(*(unsigned char*)param);
	if(isStatic == item->isStatic)
		return TRUE;
	else 
		return FALSE;
}


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
)
{
	unsigned char isStatic  = 0;
	
	if((NULL == itemA)||(NULL == itemB))
		return FALSE;

	if((itemA->isStatic == itemB->isStatic)&&\
        (itemA->ifIndex == itemB->ifIndex)){
		return TRUE;
	}
	else{
		return FALSE;
	}
}

/**********************************************************************************
 * npd_arp_snooping_filter_by_vid
 *
 * compare two of ARP snooping database(Hash table) items
 *
 *	INPUT:
 *		itemA	- ARP snooping database item
 *		itemB	- ARP snooping filtering item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		FALSE 	- if two items are not equal.
 *		TRUE 	- if two items are equal.
 *
 **********************************************************************************/

unsigned int npd_arp_snooping_filter_by_vid_ifIndex
(
	struct arp_snooping_item_s *itemA,
	struct arp_snooping_item_s *itemB
)
{
	int equal = TRUE;

	if((NULL==itemA)||(NULL==itemB)) {
		syslog_ax_arpsnooping_err("npd arp snooping items null pointers error.");
		return FALSE;
	}

	if((itemA->vid != itemB->vid)||\
		(itemA->ifIndex != itemB->ifIndex)){ /* ifindex and static*/
		equal = FALSE;
	}
	return equal;
}


/**********************************************************************************
 * npd_arp_delete
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
 *		ARP_RETURN_CODE_SUCCESS  - if success
 *		COMMON_RETURN_CODE_NULL_PTR  - if the item is null
 *		ARP_RETURN_CODE_NAM_ERROR  - nam operation failed
 *		ARP_RETURN_CODE_HASH_OP_FAILED  - hash table operation failed
 *		ARP_RETURN_CODE_NOTEXISTS  -  the arp not a dynamic one
 *		
 *	NOTE:
 *		this procedure treat ARP snooping items found in hash tables, so the 'item' is already found
 *		when calling the procedure, no hash table search done here.
 *
 **********************************************************************************/
int npd_arp_delete
(
	struct arp_snooping_item_s *item,
	unsigned int kern_del_flag
)
{
	/*static arp don't delete*/
	if(TRUE == item->isStatic) {
		return ARP_RETURN_CODE_NOTEXISTS;
	}
    return npd_all_arp_delete(item,kern_del_flag);
}

/**********************************************************************************
 * npd_static_arp_delete
 *
 *  	Delete ARP snooping info in both sw and hw if it is a static arp
 *
 *	INPUT:
 *		item - ARP snooping item info
 *           kern_del_flag - delete the arp from kern or not
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS  - if success
 *		COMMON_RETURN_CODE_NULL_PTR  - if the item is null
 *		ARP_RETURN_CODE_NAM_ERROR  - nam operation failed
 *		ARP_RETURN_CODE_HASH_OP_FAILED  - hash table operation failed
 *		ARP_RETURN_CODE_STASTIC_NOTEXIST  - the static arp not exists
 *		
 *	NOTE:
 *		this procedure treat ARP snooping items found in hash tables, so the 'item' is already found
 *		when calling the procedure, no hash table search done here.
 *
 **********************************************************************************/
int npd_static_arp_delete
(
	struct arp_snooping_item_s *item,
	unsigned int kern_del_flag
)
{
	/*not static arp don't delete*/
	if(TRUE != item->isStatic) {
		return ARP_RETURN_CODE_STASTIC_NOTEXIST;
	}
    return npd_all_arp_delete(item,kern_del_flag);
}

/**********************************************************************************
 * npd_all_arp_delete
 *
 *  	Delete static and not static ARP snooping info in both sw and hw
 *
 *	INPUT:
 *		item - static ARP snooping item info
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS  - if success
 *		COMMON_RETURN_CODE_NULL_PTR  - if the item is null
 *		ARP_RETURN_CODE_NAM_ERROR  - nam operation failed
 *		ARP_RETURN_CODE_HASH_OP_FAILED  - hash table operation failed
 *		
 *	NOTE:
 * 
 *
 **********************************************************************************/
int npd_all_arp_delete
(
	struct arp_snooping_item_s *item,
	unsigned int kern_del_flag    
)
{
	struct route_nexthop_brief_s *routeNextHop = NULL, *targetRT = NULL;
	struct arp_snooping_item_s *targetArp = NULL;
	int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned int action = ARP_SNOOPING_DEL_ITEM, eth_g_index = 0,result = 0;
	int i = 0;
	
	if(NULL == item) {
		return COMMON_RETURN_CODE_NULL_PTR;
	}
	/*first del kernel arp*/
	if((kern_del_flag == TRUE)&&\
		((FALSE == item->isStatic)||(TRUE == item->isValid))) {		
		npd_arp_snooping_del_kern_arp(item);
		item->isValid = FALSE;
	}
	pthread_mutex_lock(&nexthopHashMutex);
	routeNextHop = npd_route_nexthop_find(item->ifIndex,item->ipAddr);
	if(NULL == routeNextHop) {
		syslog_ax_arpsnooping_err("npd delete ifindex %#x ip %d.%d.%d.%d not found in sw hash table", \
				item->ifIndex,(item->ipAddr>>24) & 0xFF,(item->ipAddr>>16) & 0xFF,	\
				(item->ipAddr>>8) & 0xFF,item->ipAddr & 0xFF);
		targetArp = hash_pull(arp_snoop_db_s,item);
		if(targetArp) {
			free(targetArp);
			targetArp = NULL;
		}
		pthread_mutex_unlock(&nexthopHashMutex);
		return ARP_RETURN_CODE_SUCCESS;
	}
	syslog_ax_arpsnooping_dbg("npd delete ifindex %#x ip %d.%d.%d.%d found at %d of HW table", \
			item->ifIndex,(item->ipAddr>>24) & 0xFF,(item->ipAddr>>16) & 0xFF,	\
			(item->ipAddr>>8) & 0xFF,item->ipAddr & 0xFF,routeNextHop->tblIndex);

	if(routeNextHop->rtUsedCnt) {
		action = ARP_SNOOPING_RESET_ITEM;
	}
	pthread_mutex_lock(&namItemOpMutex);
	ret = nam_arp_snooping_op_item(item,action,&(routeNextHop->tblIndex));
	pthread_mutex_unlock(&namItemOpMutex);
	if(ARP_RETURN_CODE_SUCCESS != ret) {
		syslog_ax_arpsnooping_err("nam %s arp snooping item at %d error %d",	\
					(ARP_SNOOPING_RESET_ITEM==action) ? "RESET":"DEL",routeNextHop->tblIndex,ret);
		ret = ARP_RETURN_CODE_NAM_ERROR;
	}
	else {
		syslog_ax_arpsnooping_dbg("nam %s arp snooping item at %d of HW table ok",	\
					(ARP_SNOOPING_RESET_ITEM==action) ? "RESET":"DEL",routeNextHop->tblIndex);
	}

	/* TODO: delete host route from RT in hw*/
	npd_route_table_op_host_item(routeNextHop,ROUTE_DEL_ITEM);

	/* delete route_nexthop item only if not used by any route entry*/
	if(!routeNextHop->rtUsedCnt) {
		targetRT = hash_pull(route_nexthop_db_s,routeNextHop);
		if(targetRT) {
			free(targetRT);
			targetRT = NULL;
		}
		else{
			syslog_ax_arpsnooping_err("error: npd arp delete hash pull FAILED!\n");
			return ARP_RETURN_CODE_HASH_OP_FAILED;
		}
	}
	else {/* check if this delete is triggered by Link DOWN event*/
		result = npd_get_global_index_by_devport(item->dstIntf.intf.port.devNum,	\
										item->dstIntf.intf.port.portNum, &eth_g_index);
        /*  
                when the port link down but the interface is not down,we should not set "rtUsedCnt" to 0,
                otherwise the "rtUsedCnt" won't be correct again.
                
		if((0 == result)&&
			(ETH_ATTR_LINKDOWN == npd_check_eth_port_status(eth_g_index))) {*/
			/*  	release reference count when delete action triggered by link DOWN envent
 			  *	because kernel will notify me to add route when link status change to UP
 			  *	by qinhs@autelan.com Sep. 4th 2008
			  *//*
			  routeNextHop->rtUsedCnt = 0;
			  
		}
	    */
	}
	pthread_mutex_unlock(&nexthopHashMutex);
	targetArp = hash_pull(arp_snoop_db_s, item);
	if(targetArp) {
		free(targetArp);
		targetArp = NULL;
	}
	else{
		syslog_ax_arpsnooping_err("error: npd arp delete hash pull FAILED!\n");
		return ARP_RETURN_CODE_HASH_OP_FAILED;
	}

	return ret;	

}
/**********************************************************************************
 * npd_static_arp_valid_set
 *
 *  	Set the static ARP snooping to invalid
 *
 *	INPUT:
 *		item - ARP snooping item info
 *           flag  - (0 - set invalid, 1 - set valid)
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		TRUE  -  1 item was set
 *		FALSE  -  no items was set
 *		
 *	NOTE:
 *		this procedure treat ARP snooping items found in hash tables, so the 'item' is already found
 *		when calling the procedure, no hash table search done here.
 *
 **********************************************************************************/
int npd_static_arp_valid_set
(
	struct arp_snooping_item_s *item,
	unsigned int flag
)
{
	unsigned int ret = 0;
	if(NULL == item){
         return FALSE;
	}
    if(FALSE == flag){
		if((TRUE == item->isStatic)&&(TRUE == item->isValid)) {
			if(ARP_RETURN_CODE_SUCCESS == npd_static_arp_set_invalid(item)){
                syslog_ax_arpsnooping_dbg("set static arp %d.%d.%d.%d invalid!\n",\
					     (item->ipAddr>>24)&0xff,(item->ipAddr>>16)&0xff,(item->ipAddr>>8)&0xff,item->ipAddr&0xff);
			    return TRUE;
			}
			else{
                syslog_ax_arpsnooping_err("set static arp invalid FAILED!\n");
			}
		}else{
			syslog_ax_arpsnooping_err("valid static arp %d.%d.%d.%d not found while static arp invalidate! the arp is %s %s \n",\
				     (item->ipAddr>>24)&0xff,(item->ipAddr>>16)&0xff,(item->ipAddr>>8)&0xff,item->ipAddr&0xff,
				     item->isValid ? "VALID" : "INVALID",item->isStatic ? "STATIC" : "NOT STATIC");
		}
		return FALSE;
	}else{
		if((TRUE == item->isStatic)&&(FALSE == item->isValid)) {
			ret = npd_static_arp_set_valid(item);
			if(ARP_RETURN_CODE_SUCCESS == ret){
				syslog_ax_arpsnooping_dbg("set static arp %d.%d.%d.%d valid!\n",\
					     (item->ipAddr>>24)&0xff,(item->ipAddr>>16)&0xff,(item->ipAddr>>8)&0xff,item->ipAddr&0xff);
			    return TRUE;
			}
			else {
                syslog_ax_arpsnooping_dbg("set static arp valid FAILED!\n");
			}
		}
		else {
			syslog_ax_arpsnooping_err("invalid static arp %d.%d.%d.%d not found while static arp validate! the arp is %s %s \n",\
				     (item->ipAddr>>24)&0xff,(item->ipAddr>>16)&0xff,(item->ipAddr>>8)&0xff,item->ipAddr&0xff,
				     item->isValid ? "VALID" : "INVALID",item->isStatic ? "STATIC" : "NOT STATIC");
		}
        return FALSE;
	}
}

/***************************************************************
 *npd_static_arp_set_valid
 *		set static arp valid,add to hardware and kernal if necessary
 * INPUT:
 *		item  - the arp item we want to set valid
 * OUTPUT:
 *		item  - the arp item we set
 * RETURN:
 *		ARP_RETURN_CODE_SUCCESS
 *		COMMON_RETURN_CODE_NULL_PTR
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ARP_RETURN_CODE_TBLINDEX_GET_FAILED - table index get failed 
 *		ARP_RETURN_CODE_NAM_ERROR - asic operation failed
 *		ARP_RETURN_CODE_ERROR
 * NOTE:
 *
 *****************************************************************/
int npd_static_arp_set_valid
(
    struct arp_snooping_item_s *item 
)
{

    int retVal = 0,i = 0;
	unsigned int nextHopTblIndex = 0,dupIfindex = 0;	
	struct route_nexthop_brief_s nextHopItem;	
    unsigned int defaultMask = 32;
	unsigned int eth_g_index = 0;
	unsigned int setHardWare = FALSE;
	unsigned int ifIndex = ~0UI;
    if(NULL == item){
        return COMMON_RETURN_CODE_NULL_PTR;
	}
	if((TRUE != item->isStatic)||(TRUE == item->isValid)){
		return ARP_RETURN_CODE_ERROR;
	}
	retVal = npd_arp_snooping_create_kern_static_arp(item->ipAddr,defaultMask,item->mac);
	if(ARP_RETURN_CODE_SUCCESS == retVal){/*kernal success , intf is up or promis intf */
	    item->isValid = TRUE;
	}
	if(NPD_PORT_L3INTF_VLAN_ID == item->vid){
		if((TRUE != item->dstIntf.isTrunk)&&\
			(NPD_SUCCESS == npd_get_global_index_by_devport\
			(item->dstIntf.intf.port.devNum,item->dstIntf.intf.port.portNum,&eth_g_index))&&\
			(TRUE == npd_eth_port_interface_check(eth_g_index,&ifIndex))&&\
			(~0UI != ifIndex)&&(ifIndex == item->ifIndex)){
		    setHardWare = TRUE;/*for port interface*/
		}
	}
	else if((TRUE == npd_vlan_interface_check(item->vid,&ifIndex))&&(~0UI != ifIndex)&&(ifIndex == item->ifIndex)){
            setHardWare = TRUE;/*for vlan interface*/
	}
	if(TRUE != setHardWare ){
        return ARP_RETURN_CODE_SUCCESS;
	}
	retVal = npd_arp_snooping_learning( item ,&nextHopTblIndex,&dupIfindex );
	if(ARP_RETURN_CODE_SUCCESS == retVal) {
		/* TODO: build route table hash*/
		memset(&nextHopItem, 0, sizeof(struct route_nexthop_brief_s));
		nextHopItem.ifIndex = item->ifIndex;
		nextHopItem.ipAddr 	= item->ipAddr;
		nextHopItem.tblIndex= nextHopTblIndex;
		pthread_mutex_lock(&nexthopHashMutex);
		retVal = npd_route_nexthop_op_item(&nextHopItem,NEXTHOP_ADD_ITEM,dupIfindex);
		pthread_mutex_unlock(&nexthopHashMutex);
		if(ROUTE_RETURN_CODE_SUCCESS != retVal) {
			syslog_ax_arpsnooping_err("npd nexthop brief add to db error %d",retVal);
			return ARP_RETURN_CODE_ERROR;
		}

		/* TODO:add host route to RT*/
		retVal = npd_route_table_op_host_item(&nextHopItem,ROUTE_ADD_ITEM);
		if(ROUTE_RETURN_CODE_SUCCESS!= retVal) {
			syslog_ax_arpsnooping_err("npd route host add to HW error %d",retVal);
			return ARP_RETURN_CODE_ERROR;
		}
		retVal = ARP_RETURN_CODE_SUCCESS;
		
	}
	else {
		syslog_ax_arpsnooping_err("npd arp snooping learning error %d",retVal);
		return retVal;
	}
	return ARP_RETURN_CODE_SUCCESS;

}
/*****************************************************
*npd_static_arp_set_invalid
*		set static arp invalid,delete from hardware and kernal
* INPUT:
*		item  - the arp item we want to set 
* OUTPUT:
*		item  - the arp item we set
* RETURN:
*		ARP_RETURN_CODE_SUCCESS   - set success
*		COMMON_RETURN_CODE_NULL_PTR  - the item is null
*		ARP_RETURN_CODE_ERROR  - the arp is not static or already invalid
*		ARP_RETURN_CODE_HASH_OP_FAILED  - hash table operation failed
* NOTE:
*
******************************************************/
int npd_static_arp_set_invalid
(
	struct arp_snooping_item_s *item
)
{
		struct route_nexthop_brief_s *routeNextHop = NULL, *targetRT = NULL;
		int ret = ARP_RETURN_CODE_SUCCESS;
		unsigned int action = ARP_SNOOPING_DEL_ITEM, eth_g_index = 0,result = 0;
		int i = 0;
		
		if(NULL == item) {
			return COMMON_RETURN_CODE_NULL_PTR;
		}
		
		if((TRUE != item->isStatic)||(TRUE != item->isValid)){
			return ARP_RETURN_CODE_ERROR;
		}
		/*first del kernel arp*/
		ret = npd_arp_snooping_del_kern_arp(item);
		item->isValid = FALSE;
		
		pthread_mutex_lock(&nexthopHashMutex);
		routeNextHop = npd_route_nexthop_find(item->ifIndex,item->ipAddr);
		if(NULL == routeNextHop) {
			syslog_ax_arpsnooping_err("npd arp set invalid ifindex %#x ip %d.%d.%d.%d not found in sw hash table", \
					item->ifIndex,(item->ipAddr>>24) & 0xFF,(item->ipAddr>>16) & 0xFF,	\
					(item->ipAddr>>8) & 0xFF,item->ipAddr & 0xFF);
			pthread_mutex_unlock(&nexthopHashMutex);
			return ARP_RETURN_CODE_SUCCESS;
		}
		syslog_ax_arpsnooping_dbg("npd arp set invalid ifindex %#x ip %d.%d.%d.%d found at %d of HW table", \
				item->ifIndex,(item->ipAddr>>24) & 0xFF,(item->ipAddr>>16) & 0xFF,	\
				(item->ipAddr>>8) & 0xFF,item->ipAddr & 0xFF,routeNextHop->tblIndex);
	
		if(routeNextHop->rtUsedCnt) {
			action = ARP_SNOOPING_RESET_ITEM;
		}
		pthread_mutex_lock(&namItemOpMutex);
		ret = nam_arp_snooping_op_item(item,action,&(routeNextHop->tblIndex));
		pthread_mutex_unlock(&namItemOpMutex);
		if(ARP_RETURN_CODE_SUCCESS != ret) {
			syslog_ax_arpsnooping_err("nam %s arp snooping item at %d error %d",	\
						(ARP_SNOOPING_RESET_ITEM==action) ? "RESET":"DEL",routeNextHop->tblIndex,ret);
		}
		else {
			syslog_ax_arpsnooping_dbg("nam %s arp snooping item at %d of HW table ok",	\
						(ARP_SNOOPING_RESET_ITEM==action) ? "RESET":"DEL",routeNextHop->tblIndex);
		}
	
		/* TODO: delete host route from RT in hw*/
		npd_route_table_op_host_item(routeNextHop,ROUTE_DEL_ITEM);
	
		/*by zhubo@autelan.com del RT secondly*/
		for(i = 0; i < 20; i++);
		npd_route_table_op_host_item(routeNextHop,ROUTE_DEL_ITEM);
	
		/* delete route_nexthop item only if not used by any route entry*/
		if(!routeNextHop->rtUsedCnt) {
			targetRT = hash_pull(route_nexthop_db_s,routeNextHop);
			if(targetRT) {
				free(targetRT);
				targetRT = NULL;
			}
			else{
				syslog_ax_arpsnooping_err("error: npd static arp set invalid hash pull FAILED!\n");
				return ARP_RETURN_CODE_HASH_OP_FAILED;
			}
		}
		else {/* check if this delete is triggered by Link DOWN event*/
			result = npd_get_global_index_by_devport(item->dstIntf.intf.port.devNum,	\
											item->dstIntf.intf.port.portNum, &eth_g_index);
			if((0 == result)&&
				(ETH_ATTR_LINKDOWN == npd_check_eth_port_status(eth_g_index))) {
				/*		release reference count when delete action triggered by link DOWN envent
				  * because kernel will notify me to add route when link status change to UP
				  * by qinhs@autelan.com Sep. 4th 2008
				  */
				  routeNextHop->rtUsedCnt = 0;
			}
		}	
		pthread_mutex_unlock(&nexthopHashMutex);
		return ARP_RETURN_CODE_SUCCESS; 
	
}

/********************************************************
 * npd_static_arp_valid_set_by_port
 *
 *  	set the static ARP items on this port to valid or invalid
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *		isVaild  - set valid or invalid
 *				0 - set invalid
 *				1 - set valid	
 *	OUTPUT:
 *		NULL
 * 	RETURN:
 *		ARP item number set on this eth port.
 *		
 *	NOTE:
 *
 *********************************************************/
unsigned int npd_static_arp_valid_set_by_port
(
	unsigned int   eth_g_index,
	unsigned int isValid
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;
	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd static arp set %s on eth-port %#x get asic port error %d\n",isValid ? "Valid":"Invalid",eth_g_index,result);
		retVal = 0;
		return retVal;
	}

	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,isValid,&comparator,npd_arp_filter_static_by_port,npd_static_arp_valid_set);
	pthread_mutex_unlock(&arpHashMutex);
	if(retVal > 0){
	    syslog_ax_arpsnooping_dbg("npd static arp set %s on eth-port %#x dev %d port %d total %d items set\n", \
					    isValid ? "Valid":"Invalid",eth_g_index,devNum,portNum, retVal);
	}
	return retVal;
}



/********************************************************
 * npd_static_arp_valid_set_by_trunk
 *
 *  	set the static ARP items on this trunk to valid or invalid
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *		isVaild  - set valid or invalid
 *				0 - set invalid
 *				1 - set valid	
 *	OUTPUT:
 *		NULL
 * 	RETURN:
 *		ARP item number set on this trunk
 *		
 *	NOTE:
 *
 *********************************************************/

unsigned int npd_static_arp_valid_set_by_trunk
(
	unsigned int   trunkId,
	unsigned int isValid
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0;
	struct fdb_interface_info_s comparator;
	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	comparator.isTrunk = TRUE;
	comparator.intf.trunk.trunkId = trunkId;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,isValid,&comparator,npd_arp_filter_static_by_trunk,npd_static_arp_valid_set);
	pthread_mutex_unlock(&arpHashMutex);
	if(retVal > 0){
	    syslog_ax_arpsnooping_dbg("npd static arp set %s on trunk %d total %d items set\n", \
					    isValid ? "Valid":"Invalid",trunkId, retVal);
	}
	return retVal;
}

/*********************************************************
 * npd_static_arp_invalidate_by_port
 *
 *  	set the static ARP items on this port to invalid 
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	OUTPUT:
 *		NULL
 * 	RETURN:
 *		ARP item number set on this eth port.
 *		
 *	NOTE:
 *
 *********************************************************/
unsigned int npd_static_arp_invalidate_by_port
(
     unsigned int eth_g_index
)
{
    return npd_static_arp_valid_set_by_port(eth_g_index,FALSE);
}
/*********************************************************
 * npd_static_arp_validate_by_port
 *
 *  	set the static ARP items on this port to valid 
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	OUTPUT:
 *		NULL
 * 	RETURN:
 *		ARP item number set on this eth port.
 *		
 *	NOTE:
 *
 *********************************************************/
unsigned int npd_static_arp_validate_by_port
(
     unsigned int eth_g_index
)
{
    return npd_static_arp_valid_set_by_port(eth_g_index,TRUE);
}


/*********************************************************
 * npd_static_arp_invalidate_by_trunk
 *
 *  	set the static ARP items on this trunk to valid 
 *
 *	INPUT:
 *		 trunkId - the trunk id
 *	OUTPUT:
 *		NULL
 * 	RETURN:
 *		ARP item number set on this trunk.
 *		
 *	NOTE:
 *
 *********************************************************/

unsigned int npd_static_arp_invalidate_by_trunk
(
     unsigned int trunkId
)
{
    return npd_static_arp_valid_set_by_trunk(trunkId,FALSE);
}

/*********************************************************
 * npd_static_arp_invalidate_by_trunk
 *
 *  	set the static ARP items on this trunk to invalid 
 *
 *	INPUT:
 *		 trunkId - the trunk id
 *	OUTPUT:
 *		NULL
 * 	RETURN:
 *		ARP item number set on this trunk.
 *		
 *	NOTE:
 *
 *********************************************************/
unsigned int npd_static_arp_validate_by_trunk
(
     unsigned int trunkId
)
{
    return npd_static_arp_valid_set_by_trunk(trunkId,TRUE);
}

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
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int kern_del_flag = TRUE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd clear arp on eth-port %#x get asic port error %d\n",eth_g_index,result);
		retVal = 0;
		return retVal;
	}

	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	kern_del_flag = TRUE;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&comparator,npd_arp_filter_by_port,npd_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	
	syslog_ax_arpsnooping_dbg("npd clear arp on eth-port %#x dev %d port %d total %d items deleted\n", \
					eth_g_index,devNum,portNum, retVal);
	return retVal;
}

/**********************************************************************************
 * npd_arp_clear_by_trunk
 *
 *  	clear ARP info learned on this trunk
 *
 *	INPUT:
 *		trunkId - the trunk id
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
unsigned int npd_arp_clear_by_trunk
(
	unsigned int   trunkId
)
{
	unsigned int retVal = 0;
	unsigned char devNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int kern_del_flag = TRUE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));

	comparator.isTrunk = TRUE;
	comparator.intf.trunk.trunkId = trunkId;

	kern_del_flag = TRUE;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&comparator,npd_arp_filter_by_trunk,npd_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	
	syslog_ax_arpsnooping_dbg("npd clear arp on trunk %d total %d items deleted\n", \
					trunkId, retVal);
	return retVal;
}

/**********************************************************************************
 * npd_all_arp_clear_by_port_ifIndex
 *
 *  	clear dynamic and static ARP info on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *          ifIndex - interface index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port and matched the ifindex.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_all_arp_clear_by_port_ifIndex
(
	unsigned int   eth_g_index,
	unsigned int ifIndex
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0, portNum = 0;
	struct arp_snooping_item_s data;
	unsigned int kern_del_flag = TRUE;

	memset(&data, 0,sizeof(struct arp_snooping_item_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd clear arp on eth-port %#x get asic port error %d\n",eth_g_index,result);
		retVal = 0;
		return retVal;
	}

	data.dstIntf.intf.port.devNum = devNum;
	data.dstIntf.intf.port.portNum = portNum;
	data.ifIndex = ifIndex;

	kern_del_flag = TRUE;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&data,npd_arp_filter_by_port_ifIndex,npd_all_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	
	syslog_ax_arpsnooping_dbg("npd clear arp on eth-port %#x dev %d port %d total %d items deleted\n", \
					eth_g_index,devNum,portNum, retVal);
	return retVal;
}


/**********************************************************************************
 * npd_all_arp_clear_by_trunk_ifIndex
 *
 *  	clear dynamic and static ARP info this trunk and the interface
 *
 *	INPUT:
 *		trunkId - the trunk id 
 *          ifIndex - interface index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this trunk and matched the ifindex.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_all_arp_clear_by_trunk_ifIndex
(
	unsigned int  trunkId,
	unsigned int  ifIndex
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0;
	struct arp_snooping_item_s data;
	unsigned int kern_del_flag = TRUE;

	memset(&data, 0,sizeof(struct arp_snooping_item_s));
	
    data.dstIntf.isTrunk = TRUE;
	data.dstIntf.intf.trunk.trunkId = trunkId;
	data.ifIndex = ifIndex;

	kern_del_flag = TRUE;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&data,npd_arp_filter_by_trunk_ifIndex,npd_all_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	
	syslog_ax_arpsnooping_dbg("npd clear arp on trunk %d total %d items deleted\n", \
					trunkId,retVal);
	return retVal;
}

/**********************************************************************************
 * npd_arp_clear_by_port_vid
 *
 *  	clear ARP info learned on this port and this vlan 
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *          vid - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this eth port and vlan.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_clear_by_port_vid
(
	unsigned int   eth_g_index,
	unsigned short vid
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0, portNum = 0;
	struct arp_snooping_item_s data;
	unsigned int kern_del_flag = TRUE;

	memset(&data, 0,sizeof(struct arp_snooping_item_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd clear arp on eth-port %#x get asic port error %d\n",eth_g_index,result);
		retVal = 0;
		return retVal;
	}

	data.dstIntf.intf.port.devNum = devNum;
	data.dstIntf.intf.port.portNum = portNum;
	data.vid = vid;

	kern_del_flag = TRUE;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&data,npd_arp_filter_by_port_vid,npd_all_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	
	syslog_ax_arpsnooping_dbg("npd clear arp on eth-port %#x dev %d port %d total %d items deleted\n", \
					eth_g_index,devNum,portNum, retVal);
	return retVal;
}
/**********************************************************************************
 * npd_arp_clear_by_trunk_vid
 *
 *  	clear ARP info learned on this trunk and this vid
 *
 *	INPUT:
 *		trunkId - the trunk id we want to clear
 *          vid - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this trunk and vlan
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_clear_by_trunk_vid
(
	unsigned int   trunkId,
	unsigned short vid
)
{
	unsigned int result = 0, retVal = 0;
	struct arp_snooping_item_s data;
	unsigned int kern_del_flag = TRUE;

	memset(&data, 0,sizeof(struct arp_snooping_item_s));

	data.dstIntf.isTrunk = TRUE;
	data.dstIntf.intf.trunk.trunkId = trunkId;
	data.vid = vid;

	kern_del_flag = TRUE;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&data,npd_arp_filter_by_trunk_vid,npd_all_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	
	syslog_ax_arpsnooping_dbg("npd clear arp on trunk %d total %d items deleted\n", \
					trunkId, retVal);
	return retVal;
}

/**********************************************************************************
 * npd_all_arp_clear_by_port
 *
 *  	clear dynamic and static ARP info on this port
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
unsigned int npd_all_arp_clear_by_port
(
	unsigned int   eth_g_index
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int kern_del_flag = TRUE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd clear arp on eth-port %#x get asic port error %d\n",eth_g_index,result);
		retVal = 0;
		return retVal;
	}

	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	kern_del_flag = TRUE;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&comparator,npd_arp_filter_by_port,npd_all_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	
	syslog_ax_arpsnooping_dbg("npd clear arp on eth-port %#x dev %d port %d total %d items deleted\n", \
					eth_g_index,devNum,portNum, retVal);
	return retVal;
}


/**********************************************************************************
 * npd_all_arp_clear_by_trunk
 *
 *  	clear dynamic and static ARP info on this trunk
 *
 *	INPUT:
 *		trunkId  -  the trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this trunk
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_all_arp_clear_by_trunk
(
	unsigned int   trunkId
)
{
	unsigned int retVal = 0;
	unsigned char devNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int kern_del_flag = TRUE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	
	comparator.isTrunk = TRUE;
	comparator.intf.port.portNum = trunkId;

	kern_del_flag = TRUE;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&comparator,npd_arp_filter_by_trunk,npd_all_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	
	syslog_ax_arpsnooping_dbg("npd clear arp on trunk %d total %d items deleted\n", \
					devNum,trunkId, retVal);
	return retVal;
}

/**********************************************************************************
 * npd_static_arp_clear_by_port
 *
 *  	clear static arp info on this port
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
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int kern_del_flag = TRUE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd clear static arp on eth-port %#x get asic port error %d\n",eth_g_index,result);
		retVal = 0;
		return retVal;
	}

	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	kern_del_flag = TRUE;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&comparator,npd_arp_filter_static_by_port,npd_all_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	
	syslog_ax_arpsnooping_dbg("npd clear static arp on eth-port %#x dev %d port %d total %d items deleted\n", \
					eth_g_index,devNum,portNum, retVal);
	return retVal;
}


/**********************************************************************************
 * npd_static_arp_clear_by_trunk
 *
 *  	clear static ARP info on this trunk
 *
 *	INPUT:
 *		trunkId  - the trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP item number cleared on this trunk
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_static_arp_clear_by_trunk
(
	unsigned int   trunkId
)
{
	unsigned int retVal = 0;
	unsigned char devNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int kern_del_flag = TRUE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	comparator.isTrunk = TRUE;
	comparator.intf.port.portNum = trunkId;

	kern_del_flag = TRUE;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&comparator,npd_arp_filter_static_by_trunk,npd_all_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	
	syslog_ax_arpsnooping_dbg("npd clear static arp on trunk %d total %d items deleted\n", \
					trunkId, retVal);
	return retVal;
}


/**********************************************************************************
 * npd_arp_clear_by_vid_ifIndex
 *
 *  	clear static and dynamic arp info in this vlan and this layer 3 interface
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
)
{
	unsigned int retVal = 0;
	struct  arp_snooping_item_s data;
	unsigned int kern_del_flag = TRUE;
	memset(&data, 0,sizeof(struct arp_snooping_item_s));
	
	data.vid = vid;
	data.ifIndex = ifIndex;
	pthread_mutex_lock(&arpHashMutex);
	retVal = hash_traversal(arp_snoop_db_s,kern_del_flag,&data,npd_arp_snooping_filter_by_vid_ifIndex,npd_all_arp_delete);	
	pthread_mutex_unlock(&arpHashMutex);
	syslog_ax_arpsnooping_dbg("npd clear %d arp items on vid: %d ifIndex %#0x \n",retVal,vid,ifIndex);
	return retVal;
}
#if 1

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
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int flag = FALSE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd show arp on eth-port %#x get asic port error %d",eth_g_index,result);
		retVal = 0;
		return retVal;
	}

	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	retVal = hash_traversal(arp_snoop_db_s,flag,&comparator,npd_arp_filter_by_port,NULL);
	
	syslog_ax_arpsnooping_dbg("npd show arp on eth-port %#x dev %d port %d total %d items", \
					eth_g_index,devNum,portNum, retVal);
	return retVal;
}
#endif
/**********************************************************************************
 * npd_arp_snooping_get_valid_num_byport
 *
 *  	get valid ARP info learned on this port
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
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int flag = FALSE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd show valid arp on eth-port %#x get asic port error %d",eth_g_index,result);
		retVal = 0;
		return retVal;
	}

	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	retVal = hash_traversal(arp_snoop_db_s,flag,&comparator,npd_valid_arp_filter_by_port,NULL);
	
	syslog_ax_arpsnooping_dbg("npd show valid arp on eth-port %#x dev %d port %d total %d items", \
					eth_g_index,devNum,portNum, retVal);
	return retVal;
}
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
 *		valid ARP items number on this eth port and the nexthop is exist.
 *		
 *	NOTE:
 *
 **********************************************************************************/
unsigned int npd_arp_snooping_get_valid_num_byport_for_nexthop
(
	unsigned int   eth_g_index
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int flag = FALSE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd show valid arp on eth-port %#x get asic port error %d",eth_g_index,result);
		retVal = 0;
		return retVal;
	}

	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	retVal = hash_traversal(arp_snoop_db_s,flag,&comparator,npd_valid_arp_filter_by_port_nexthop,NULL);
	
	syslog_ax_arpsnooping_dbg("npd show valid arp on eth-port %#x dev %d port %d total %d items", \
					eth_g_index,devNum,portNum, retVal);
	return retVal;
}
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
)
{
	unsigned int result = 0, retVal = 0;
	struct fdb_interface_info_s comparator;
	unsigned int flag = FALSE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	comparator.intf.trunk.trunkId = trunkId;
	comparator.isTrunk = TRUE;

	retVal = hash_traversal(arp_snoop_db_s,flag,&comparator,npd_valid_arp_filter_by_trunk_nexthop,NULL);
	
	syslog_ax_arpsnooping_dbg("npd show valid arp on trunk %d total %d items", \
					trunkId,retVal);
	return retVal;
}

/***************************************************************
 * npd_arp_snooping_find_item_byport
 *
 *  	find ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		arpInfo  - the arp items we got
 *		arrayLen - arp item number we want to get
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if get success.
 *		ARP_RETURN_CODE_ERROR  - if get failed
 *		
 *	NOTE:
 *
 ****************************************************************/
int npd_arp_snooping_find_item_byport
(
	unsigned int   eth_g_index,
	struct arp_snooping_item_s * arpInfo[],
	unsigned int arrayLen
)
{
	unsigned int result = 0, retVal = ARP_RETURN_CODE_SUCCESS;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;


	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd find arp on eth-port %#x get asic port error %d",eth_g_index,result);
		return ARP_RETURN_CODE_ERROR;
	}

	comparator.isTrunk = FALSE;
	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	/*printf("npd find arp on eth-port %#x dev %d port %d\n",eth_g_index,devNum,portNum);*/
	hash_return(arp_snoop_db_s,&comparator,arpInfo,arrayLen,npd_arp_filter_by_port);
	
	syslog_ax_arpsnooping_dbg("npd find arp on eth-port %#x dev %d port %d\n",eth_g_index,devNum,portNum);
	return retVal;
}


/***************************************************************
 * npd_arp_snooping_find_valid_item_byport
 *
 *  	find valid ARP info learned on this port
 *
 *	INPUT:
 *		eth_g_index - global eth-port index
 *	
 *	OUTPUT:
 *		arpInfo  - the arp items we got
 *		arrayLen - arp item number we want to get
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if get success.
 *		ARP_RETURN_CODE_ERROR  - if get failed
 *		
 *	NOTE:
 *
 ****************************************************************/

int npd_arp_snooping_find_valid_item_byport
(
	unsigned int   eth_g_index,
	struct arp_snooping_item_s * arpInfo[],
	unsigned int arrayLen
)
{
	unsigned int result = 0, retVal = ARP_RETURN_CODE_SUCCESS;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd find arp on eth-port %#x get asic port error %d",eth_g_index,result);
		return ARP_RETURN_CODE_ERROR;
	}

	comparator.isTrunk = FALSE;
	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	/*printf("npd find arp on eth-port %#x dev %d port %d\n",eth_g_index,devNum,portNum);*/
	hash_return(arp_snoop_db_s,&comparator,arpInfo,arrayLen,npd_valid_arp_filter_by_port);
	
	syslog_ax_arpsnooping_dbg("npd find arp on eth-port %#x dev %d port %d\n",eth_g_index,devNum,portNum);
	return retVal;
}

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
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int flag = FALSE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));

	comparator.isTrunk = TRUE;
	comparator.intf.trunk.devNum = devNum;
	comparator.intf.trunk.trunkId = trunkId;

	retVal = hash_traversal(arp_snoop_db_s,flag,&comparator,npd_arp_filter_by_trunk,NULL);
	
	syslog_ax_arpsnooping_dbg("npd show arp dev %d on trunk %d total %d items", \
					devNum,trunkId, retVal);
	return retVal;
}

/**********************************************************************************
 * npd_arp_snooping_get_valid_num_bytrunk
 *
 *  	get valid ARP info learned on this trunk
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
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0;
	struct fdb_interface_info_s comparator;
	unsigned int flag = FALSE;

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));

	comparator.isTrunk = TRUE;
	comparator.intf.trunk.trunkId = trunkId;

	retVal = hash_traversal(arp_snoop_db_s,flag,&comparator,npd_valid_arp_filter_by_trunk,NULL);
	
	syslog_ax_arpsnooping_dbg("npd show arp on trunk %d total %d items", \
					trunkId, retVal);
	return retVal;
}

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
)
{
	unsigned int result = 0, retVal = 0;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;


	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));

	comparator.isTrunk = TRUE;
	comparator.intf.trunk.devNum = devNum;
	comparator.intf.trunk.trunkId = trunkId;

	hash_return(arp_snoop_db_s,&comparator,arpInfo,arrayLen,npd_arp_filter_by_trunk);
	
	syslog_ax_arpsnooping_dbg("npd find arp on trunk %#x\n",trunkId);
	return retVal;
}
/**********************************************************************************
 * npd_arp_snooping_find_valid_item_bytrunk
 *
 *  	find valid ARP info learned on this trunk
 *
 *	INPUT:
 *		trunkId - the trunk id we want to find arp on
 *	
 *	OUTPUT:
 *		arpInfo  - the arp items we got
 *		arrayLen - arp item number we want to get
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS  - anyway
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_find_valid_item_bytrunk
(
	unsigned int   trunkId,
	struct arp_snooping_item_s * arpInfo[],
	unsigned int arrayLen
)
{
	unsigned int result = 0, retVal = ARP_RETURN_CODE_SUCCESS;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;


	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));

	comparator.isTrunk = TRUE;
	comparator.intf.trunk.trunkId = trunkId;

	hash_return(arp_snoop_db_s,&comparator,arpInfo,arrayLen,npd_valid_arp_filter_by_trunk);
	
	syslog_ax_arpsnooping_dbg("npd find arp on trunk %#x\n",trunkId);
	return retVal;
}
#if 0
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
)
{	
	unsigned int result = 0, i = 0;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;
	struct arp_snooping_item_s ** arpInfo = NULL;
	struct route_nexthop_brief_s *backet = NULL;
	unsigned int tbIndex = 0;

	arpInfo = (struct arp_snooping_item_s **)malloc((arrayLen * sizeof(struct arp_snooping_item_s *)));
	if(arpInfo == NULL) {
		return -1;
	}

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd find arp on eth-port %#x get asic port error %d",eth_g_index,result);
		return -1;
	}

	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	//printf("npd find nexthop on eth-port %#x dev %d port %d\n",eth_g_index,devNum,portNum);
	hash_return(arp_snoop_db_s,&comparator,arpInfo,arrayLen,npd_arp_filter_by_port);
	for(i = 0; i < arrayLen; i++) {
		backet = npd_route_nexthop_find(arpInfo[i]->ifIndex,arpInfo[i]->ipAddr);
		if(NULL != backet) {
			tbIndex = backet->tblIndex;
			item[i].ipAddr = arpInfo[i]->ipAddr;
			item[i].isStatic = arpInfo[i]->isStatic;			
			item[i].isTagged = arpInfo[i]->isTagged;
			npd_route_nexthop_iteminfo_get(0,tbIndex,&item[i],&(refCntArray[i]));
		}
	}

	free(arpInfo);
	return 0;
}
#endif

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
 *		ARP_RETURN_CODE_SUCCESS  - get success
 *		ARP_RETURN_CODE_ERROR  -	get failed
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
)
{	
	unsigned int result = 0, i = 0;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;
	struct arp_snooping_item_s ** arpInfo = NULL;
	struct route_nexthop_brief_s *backet = NULL;
	unsigned int tbIndex = 0;

	arpInfo = (struct arp_snooping_item_s **)malloc((arrayLen * sizeof(struct arp_snooping_item_s *)));
	if(arpInfo == NULL) {
		return ARP_RETURN_CODE_ERROR;
	}

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	result = npd_get_devport_by_global_index(eth_g_index, &devNum, &portNum);
	if(NPD_SUCCESS != result) {
		syslog_ax_arpsnooping_err("npd find arp on eth-port %#x get asic port error %d",eth_g_index,result);
		free(arpInfo);	/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
		arpInfo = NULL;
		return ARP_RETURN_CODE_ERROR;
	}

	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	/*printf("npd find nexthop on eth-port %#x dev %d port %d\n",eth_g_index,devNum,portNum);*/
	
	pthread_mutex_lock(&nexthopHashMutex);
	hash_return(arp_snoop_db_s,&comparator,arpInfo,arrayLen,npd_valid_arp_filter_by_port_nexthop);
	for(i = 0; i < arrayLen; i++) {
		backet = npd_route_nexthop_find(arpInfo[i]->ifIndex,arpInfo[i]->ipAddr);
		if(NULL != backet) {
			tbIndex = backet->tblIndex;
			item[i].ipAddr = arpInfo[i]->ipAddr;
			item[i].isStatic = arpInfo[i]->isStatic;
			item[i].isTagged = arpInfo[i]->isTagged;
			item[i].dstIntf.isTrunk = arpInfo[i]->dstIntf.isTrunk;			
			if(TRUE == arpInfo[i]->dstIntf.isTrunk){
				item[i].dstIntf.intf.trunk.devNum = arpInfo[i]->dstIntf.intf.trunk.devNum;
				item[i].dstIntf.intf.trunk.trunkId = arpInfo[i]->dstIntf.intf.trunk.trunkId;
			}
			else{
				item[i].dstIntf.intf.port.devNum = arpInfo[i]->dstIntf.intf.port.devNum;
				item[i].dstIntf.intf.port.portNum = arpInfo[i]->dstIntf.intf.port.portNum;
			}		
			npd_route_nexthop_iteminfo_get(0,tbIndex,&item[i],&(refCntArray[i]));
		}
	}
	pthread_mutex_unlock(&nexthopHashMutex);	
	free(arpInfo);
	arpInfo = NULL;
	return ARP_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * npd_arp_snooping_get_valid_nexthop_item_bytrunk
 *
 *  	find ARP info learned on this trunk
 *
 *	INPUT:
 *		trunkId - trunk Id
 *		arrayLen - ARP snooping item array size
 *	
 *	OUTPUT:
 *		item - ARP snooping item array
 *		refCntArray - ARP snooping item reference count array
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - get item success
 *		COMMON_RETURN_CODE_NULL_PTR - malloc error
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_get_valid_nexthop_item_bytrunk
(
	unsigned int trunkId,
	struct arp_snooping_item_s *item,
	unsigned int arrayLen,
	unsigned int *refCntArray
)
{	
	unsigned int result = 0, i = 0;
	struct fdb_interface_info_s comparator;
	struct arp_snooping_item_s ** arpInfo = NULL;
	struct route_nexthop_brief_s *backet = NULL;
	unsigned int tbIndex = 0;

	arpInfo = (struct arp_snooping_item_s **)malloc((arrayLen * sizeof(struct arp_snooping_item_s *)));
	if(arpInfo == NULL) {
		return COMMON_RETURN_CODE_NULL_PTR;
	}

	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	
	comparator.intf.trunk.trunkId = trunkId;
	comparator.isTrunk = TRUE;
	
	pthread_mutex_lock(&nexthopHashMutex);
	hash_return(arp_snoop_db_s,&comparator,arpInfo,arrayLen,npd_valid_arp_filter_by_trunk_nexthop);
	for(i = 0; i < arrayLen; i++) {
		backet = npd_route_nexthop_find(arpInfo[i]->ifIndex,arpInfo[i]->ipAddr);
		if(NULL != backet) {
			tbIndex = backet->tblIndex;
			item[i].ipAddr = arpInfo[i]->ipAddr;
			item[i].isStatic = arpInfo[i]->isStatic;
			item[i].isTagged = arpInfo[i]->isTagged;
			item[i].dstIntf.isTrunk = arpInfo[i]->dstIntf.isTrunk;		
			if(TRUE == arpInfo[i]->dstIntf.isTrunk){
				item[i].dstIntf.intf.trunk.devNum = arpInfo[i]->dstIntf.intf.trunk.devNum;
				item[i].dstIntf.intf.trunk.trunkId = arpInfo[i]->dstIntf.intf.trunk.trunkId;
			}
			else{
				item[i].dstIntf.intf.port.devNum = arpInfo[i]->dstIntf.intf.port.devNum;
				item[i].dstIntf.intf.port.portNum = arpInfo[i]->dstIntf.intf.port.portNum;
			}		
			npd_route_nexthop_iteminfo_get(0,tbIndex,&item[i],&(refCntArray[i]));
		}
	}
	pthread_mutex_unlock(&nexthopHashMutex);	
	free(arpInfo);
	arpInfo = NULL;
	return ARP_RETURN_CODE_SUCCESS;
}


/**********************************************************************************
 * npd_arp_snooping_del_kern_arp
 *
 *  	del kern ARP info learned 
 *
 *	INPUT:
 *		item  - the item which we want to delete from the kernal
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS  - delete success
 *		ARP_RETURN_CODE_ERROR  -  delete failed
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_del_kern_arp
(
	struct arp_snooping_item_s *item
)
{

	int ret = 0;
	struct arpreq arp_cfg;
	struct sockaddr_in* sin = NULL;
	int flags = 0,deleted = 0;

	if(NULL == item) {
		syslog_ax_arpsnooping_err("npd del kern arp item is NULL");
		return ARP_RETURN_CODE_ERROR;
	}
	
	if(sysKernArpSock <= 0) {
		syslog_ax_arpsnooping_err("npd del kern arp sysKernArpSock error %d",sysKernArpSock);
		return sysKernArpSock;
	}

	memset(&arp_cfg,0,sizeof(arp_cfg));
    sin = (struct sockaddr_in *) &(arp_cfg.arp_pa);
    sin->sin_family = AF_INET;
	sin->sin_port = 0;
    sin->sin_addr.s_addr = item->ipAddr;

	arp_cfg.arp_flags = ATF_PERM;
	arp_cfg.arp_ha.sa_family = 0;  /* Ethernet 10Mbps*/

    /* if neighter priv nor pub is given, work on both*/
    if (flags == 0)
		flags = 3;
    /* unfortuatelly the kernel interface does not allow us to
       delete private entries anlone, so we need this hack
       to avoid "not found" errors if we try both. */
    deleted = 0;

    /* Call the kernel. */
	pthread_mutex_lock(&arpKernOpMutex);
    if (flags & 2) {
		if (ioctl(sysKernArpSock, SIOCDARP, &arp_cfg) < 0) {
		    if ((errno == ENXIO) || (errno == ENOENT)) {
				/*printf("No ARP entry for del\n");*/
				if (flags & 1)
				    goto dontpub;
				pthread_mutex_unlock(&arpKernOpMutex);
				return (ARP_RETURN_CODE_ERROR);
		    }
			pthread_mutex_unlock(&arpKernOpMutex);
		    return (ARP_RETURN_CODE_ERROR);
		} 
		else
			deleted = 1;
    }
		
    if (!deleted && (flags & 1)) {
	    dontpub:
		arp_cfg.arp_flags |= ATF_PUBL;
		if (ioctl(sysKernArpSock, SIOCDARP, &arp_cfg) < 0) {
		    if ((errno == ENXIO) || (errno == ENOENT)) {
				/*printf("++  sec del ++\n");*/
				pthread_mutex_unlock(&arpKernOpMutex);
				return (ARP_RETURN_CODE_ERROR);
		    }
			pthread_mutex_unlock(&arpKernOpMutex);
		    return (ARP_RETURN_CODE_ERROR);
		}
    }
	pthread_mutex_unlock(&arpKernOpMutex);
    syslog_ax_arpsnooping_dbg("delete kernal arp : %d.%d.%d.%d success.\n",\
		(item->ipAddr>>24&0xff),(item->ipAddr>>16&0xff),\
		(item->ipAddr>>8&0xff),(item->ipAddr&0xff));
	return ARP_RETURN_CODE_SUCCESS;
}

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
 *		ARP_RETURN_CODE_ERROR  - lookup failed no item found
 *		ARP_RETURN_CODE_SUCCESS  - lookup success found one item
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
)

{
	struct ether_header_t* layer2 = NULL;
	unsigned char 	*macByte = NULL;
	struct arp_snooping_item_s* p_arp_info = NULL;
	struct ip_header_t *ipHead = NULL;
	struct arp_packet_t* arpPtr = NULL;
	unsigned int ipaddr = 0;
	unsigned int tblIndex = 0;
	unsigned int ret = 0;
	struct route_nexthop_brief_s * routeData = NULL;
	unsigned int nexthopIP = 0;

	if((NULL == pktdata)||(NULL == isTrunk)||(NULL == devNum)||(NULL == portOrTrunkNum)){
		return ARP_RETURN_CODE_ERROR;
	}
	if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(vlanId))
	{
		#if 0
		if(VLAN_STATE_UP_E == npd_check_vlan_status(vlanId))
		#endif
		{
			layer2 = (struct ether_header_t*) pktdata;
			macByte = layer2->dmac;
			if(htons(0x0806) == layer2->etherType) {
				arpPtr = (struct arp_packet_t*)(layer2 + 1);
				ipaddr = ((arpPtr->dip[0] << 24) | (arpPtr->dip[1] << 16) | (arpPtr->dip[2] << 8) |(arpPtr->dip[3]));
			}
			else {
				ipHead = (struct ip_header_t*)((unsigned char*)pktdata + sizeof(struct ether_header_t));
				ipaddr = ((ipHead->dip[0] <<24) | (ipHead->dip[1] << 16) | (ipHead->dip[2] << 8) |(ipHead->dip[3]));
			}
			#if 0 /* by qinhs@autelan.com for too many logs when send packets from kernel*/
			syslog_ax_intf_dbg("arp lookup  %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d\n",	\
												macByte[0],macByte[1],macByte[2],macByte[3],macByte[4],macByte[5],	\
												((ipaddr&0xff000000)>>24),((ipaddr&0xff0000)>>16),((ipaddr&0xff00)>>8),(ipaddr&0xff));
			#endif
			pthread_mutex_lock(&arpHashMutex);
			p_arp_info = npd_arp_snooping_find_item_bymacip(ifindex, macByte,ipaddr);
			if(p_arp_info)
			{
				if(p_arp_info->dstIntf.isTrunk) {
					*isTrunk = TRUE;
					*portOrTrunkNum = p_arp_info->dstIntf.intf.trunk.trunkId;
					*devNum = p_arp_info->dstIntf.intf.trunk.devNum;					
				}
				else {
					*isTrunk = FALSE;
					*portOrTrunkNum = p_arp_info->dstIntf.intf.port.portNum;
					*devNum = p_arp_info->dstIntf.intf.port.devNum;
				}
				pthread_mutex_unlock(&arpHashMutex);
				return ARP_RETURN_CODE_SUCCESS;
			}
			pthread_mutex_unlock(&arpHashMutex);
#if MUTIIPSUPPORT
				ret = npd_route_get_route_tblIndex_by_dip(ipaddr,&tblIndex);
				if(NPD_SUCCESS == ret){
					pthread_mutex_lock(&nexthopHashMutex);
					routeData = npd_route_nexthop_get_by_tblindex(tblIndex);
					if(NULL != routeData){
						nexthopIP = routeData->ipAddr;
					}
					pthread_mutex_unlock(&nexthopHashMutex);
					pthread_mutex_lock(&arpHashMutex);
					p_arp_info = npd_arp_snooping_find_item_bymacip(ifindex, macByte,nexthopIP);
					if(p_arp_info)
					{
						if(p_arp_info->dstIntf.isTrunk) {
							*isTrunk = TRUE;
							*portOrTrunkNum = p_arp_info->dstIntf.intf.trunk.trunkId;
							*devNum = p_arp_info->dstIntf.intf.trunk.devNum;					
						}
						else {
							*isTrunk = FALSE;
							*portOrTrunkNum = p_arp_info->dstIntf.intf.port.portNum;
							*devNum = p_arp_info->dstIntf.intf.port.devNum;
						}
						pthread_mutex_unlock(&arpHashMutex);
						return ARP_RETURN_CODE_SUCCESS;
					}
					pthread_mutex_unlock(&arpHashMutex);

				}
#else
				pthread_mutex_lock(&arpHashMutex);
				p_arp_info = npd_arp_snooping_find_item_bymac_vid(ifindex,macByte,vlanId);
				if(p_arp_info)
				{
					if(p_arp_info->dstIntf.isTrunk) {
						*isTrunk = TRUE;
						*portOrTrunkNum = p_arp_info->dstIntf.intf.trunk.trunkId;
						*devNum = p_arp_info->dstIntf.intf.trunk.devNum;					
					}
					else {
						*isTrunk = FALSE;
						*portOrTrunkNum = p_arp_info->dstIntf.intf.port.portNum;
						*devNum = p_arp_info->dstIntf.intf.port.devNum;
					}
					pthread_mutex_unlock(&arpHashMutex);
					return ARP_RETURN_CODE_SUCCESS;
				}
				pthread_mutex_unlock(&arpHashMutex);
#endif
		}

	}
	
	return ARP_RETURN_CODE_ERROR;
}

/**********************************************************************************
 * npd_arp_snooping_get_all_item
 *
 *  	get all hash ARP info learned 
 *
 *	INPUT:
 *		allItem[] -- all arp info
 *	
 *	OUTPUT:
 *		allItem[] -- all arp info we get
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
)
{
	unsigned int retVal = ARP_RETURN_CODE_SUCCESS;
	unsigned char devNum = 0, portNum = 0;
	struct fdb_interface_info_s comparator;


	memset(&comparator, 0,sizeof(struct fdb_interface_info_s));
	comparator.intf.port.devNum = devNum;
	comparator.intf.port.portNum = portNum;

	/*printf("npd find arp on eth-port %#x dev %d port %d\n",eth_g_index,devNum,portNum);*/
	hash_return(arp_snoop_db_s,NULL,allItem,NPD_ARPSNOOP_HASH_SIZE,npd_arp_filter_by_port);
	
	syslog_ax_arpsnooping_dbg("npd find arp on dev %d port %d\n",devNum,portNum);
	return retVal;
}

/**********************************************************************************
 * npd_arp_snooping_aging_delay
 *
 *  	This method is used to delay a specified time slice.
 *
 *	INPUT:
 *		NULL	
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
void npd_arp_snooping_aging_delay
(
	void
)
{
	struct timeval tv;
    tv.tv_sec = 0; /* arp_aging_time is minutes*/
    tv.tv_usec = 30000; /*500000;*/
		
   select(0, NULL, NULL, NULL, &tv);
}


/**********************************************************************************
 * npd_arp_snooping_sys_arp_process
 *
 *  	hash arp and kernel arp are synchronization
 *
 *	INPUT:
 *		fd  -- ioctl fd
 *		eth_g_index -- port_index
 *		arpInfo		-- arp item
 *		arpCount    -- arp Num
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_sys_arp_process
(
	int fd,
	unsigned int eth_g_index,
	struct arp_snooping_item_s* arpInfo[],
	unsigned int arpCount	
)
{
	unsigned int  port_index = 0;
	struct arp_req_s arp_cfg;
	char name[16] = {'\0'};
	unsigned char slot_no = 0,slot_index =0, local_port_no = 0;
	struct sockaddr_in* sin = NULL;
	int j = 0;
	unsigned int kern_del_flag = FALSE;

	/*ten sec return*/
	/*printf("please waiting ....\n");*/


	/*get all arp from hash table
	memset(allItem,0,sizeof(allItem));
	npd_arp_snooping_get_all_item(allItem); */

	for(j = 0 ; j < arpCount; j++) {

		if(NULL == arpInfo[j]) 
			break;   /*indicate next is NULL*/
			
		memset(&arp_cfg,0,sizeof(arp_cfg));
	    sin = (struct sockaddr_in *) &arp_cfg.arp_pa;
	    memset(sin, 0, sizeof(struct sockaddr_in));
	    sin->sin_family = AF_INET;
	    sin->sin_addr.s_addr = arpInfo[j]->ipAddr;
		arp_cfg.arp_flags = ATF_PERM;
		arp_cfg.arp_ha.sa_family = 1;  /* Ethernet 10Mbps*/
			
		if(4095 > arpInfo[j]->vid) {  /*vlan interface*/
			sprintf(name,"vlan%d",arpInfo[j]->vid);
			strcpy(arp_cfg.arp_dev,name);
		}
		else { /* port interface*/
			/*npd_get_global_index_by_devport(allItem[j]->dstIntf.intf.port.devNum,allItem[j]->dstIntf.intf.port.portNum,&eth_g_index);*/

			slot_index 		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
			slot_no			= CHASSIS_SLOT_INDEX2NO(slot_index);
			port_index 		= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
			local_port_no 	= ETH_LOCAL_INDEX2NO(slot_index,port_index);
			sprintf(name,"eth%d-%d",slot_no,local_port_no);
			strcpy(arp_cfg.arp_dev,name);
		}

		syslog_ax_arpsnooping_dbg("dev name %s, vid %d\n ip %d.%d.%d.%d \ndev %d, port %d\n",		\
			name,arpInfo[j]->vid,(arpInfo[j]->ipAddr>>24) & 0xff, (arpInfo[j]->ipAddr>>16) & 0xff, (arpInfo[j]->ipAddr>>8) & 0xff, arpInfo[j]->ipAddr & 0xff,	\
			arpInfo[j]->dstIntf.intf.port.devNum,arpInfo[j]->dstIntf.intf.port.portNum);
		
		if (ioctl(fd, SIOCGARP, &arp_cfg) < 0) {
			 if(ENXIO == errno) { /* ENXIO		 6	*//* No such device or address */
			 	syslog_ax_arpsnooping_dbg("@@@@@@@@@@ ENXIO :: del  arp hash table j %d\n",j);
				/*printf("@@@@@@@@@@ ENXIO :: del  arp hash table j %d\n",j);*/
				kern_del_flag = FALSE;
				npd_all_arp_delete(arpInfo[j],kern_del_flag);
			 }
		}
		else {
			/*arp incomplete*/
			if(0 == arp_cfg.arp_flags) {
				syslog_ax_arpsnooping_dbg("************ Incomplete :: del  arp hash table j %d\n",j);
				/*printf("************ Incomplete :: del  arp hash table j %d\n",j);*/
				kern_del_flag = FALSE;
				if(TRUE != arpInfo[j]->isStatic){
       				npd_arp_delete(arpInfo[j],kern_del_flag);
				}
				else {
					npd_static_arp_set_invalid(arpInfo[j]);
				}
			}
			else {
				
			}
		#if 0
			for(i = 0; i < 14 ; i++) {
				printf("i %d :: %02x\n",i,arp_cfg.arp_ha.sa_data[i]);
			}
			printf("flag %02x, family %02x\n",arp_cfg.arp_flags,arp_cfg.arp_ha.sa_family);
		#endif
		}

	}
	
	
	return ARP_RETURN_CODE_SUCCESS;	
}

/**********************************************************************************
 * npd_arp_snooping_sys_timer
 *
 *  	set arp aging time 
 *
 *	INPUT:
 *		fd - the ioctl fd we will use
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_sys_eachport_arp
(
	int fd
)
{
	int ret = 0;
	unsigned int eth_g_index = 0,isIfindex = FALSE;
	unsigned int slot_index = 0,local_port_index = 0,arpCount = NPD_ARPSNOOP_TABLE_SIZE;
	struct arp_snooping_item_s* arpInfo[NPD_ARPSNOOP_TABLE_SIZE] = {NULL};

	for (slot_index = 0; slot_index < CHASSIS_SLOT_COUNT; slot_index++ ) {
		for (local_port_index = 0; local_port_index < ETH_LOCAL_PORT_COUNT(slot_index); local_port_index++) 
		{
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,local_port_index);
			if(ETH_ATTR_LINKDOWN == npd_check_eth_port_status(eth_g_index))
				continue;
			
			isIfindex = FALSE;
			ret = npd_intf_check_by_index(eth_g_index,&isIfindex);
			/*only l3 interface have arp*/
			if((0 == ret ) && (TRUE == isIfindex)) {
				memset(arpInfo,0,NPD_ARPSNOOP_TABLE_SIZE*sizeof(struct arp_snooping_item_s*));
				pthread_mutex_lock(&arpHashMutex);
				npd_arp_snooping_find_item_byport(eth_g_index,arpInfo,arpCount);
				/*aging arp by port*/
				npd_arp_snooping_aging_delay();
				npd_arp_snooping_sys_arp_process(fd,eth_g_index,arpInfo,arpCount);
				pthread_mutex_unlock(&arpHashMutex);
			}
		}
	}


	return ARP_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * npd_arp_snooping_sys_timer
 *
 *  	set arp aging time 
 *
 *	INPUT:
 *		NONE
 *	
 *	OUTPUT:
 *		NONE
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
)
{
	struct timeval tv;
    tv.tv_sec = 60 * arp_aging_time; /* arp_aging_time is minutes*/
    tv.tv_usec = 0;
		
   select(0, NULL, NULL, NULL, &tv);
}


/**********************************************************************************
 * npd_arp_snooping_deal
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
 *		ARP_RETURN_CODE_SUCCESS
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_deal
(
	int fd
)
{
	
	while(1) {

		/*ten sec return*/
		/*printf("please waiting ....\n");*/
		npd_arp_snooping_sys_timer();
		npd_arp_snooping_sys_eachport_arp(fd);
		
	}
	
	return ARP_RETURN_CODE_SUCCESS;	
}

/**********************************************************************************
 * npd_arp_snooping_gratuitous_send
 *	This method is used to send arp solicit packet
 *
 *	INPUT:
 *		ifindex	-- dev index
 *		vid		-- vlan id
 *		eth_g-index -- port index
 *		intf	--   dev type
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS  - if no error occurred
 *		ARP_RETURN_CODE_ERROR - if error occurs
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
)
{
	int ret = ARP_RETURN_CODE_SUCCESS,i =0,j=0;
	struct vlan_ports  untagPorts,tagPorts;
	unsigned char sysMac[MAC_ADDR_LEN + 1] = {0};
	unsigned int gateway[MAX_IP_COUNT] = {INVALID_HOST_IP};
	unsigned int mask[MAX_IP_COUNT] = {0};
	struct arp_snooping_item_s item;
	memset(&untagPorts,0,sizeof(struct vlan_ports));
	memset(&tagPorts,0,sizeof(struct vlan_ports));


		/* 
	  * Get system mac address
	  */
	if(PRODUCT_MAC_ADDRESS) {
		/*ret = npd_system_get_basemac(sysMac,MAC_ADDR_LEN);*/
		ret = npd_intf_get_l3_intf_mac(ifindex,vid,eth_g_index,sysMac);
		if(INTERFACE_RETURN_CODE_SUCCESS != ret) {
			syslog_ax_arpsnooping_err("get system mac error when send arp solicit\n");
			return ARP_RETURN_CODE_ERROR;
		}
		else{
			ret = ARP_RETURN_CODE_SUCCESS;
		}
	}
	else {
		syslog_ax_arpsnooping_err("no system mac found when send arp solicit\n");
		return ARP_RETURN_CODE_ERROR;
	}

	/*
	 *  Get gateway ip address
	 */
	ret = npd_intf_get_ip_addrs(ifindex,gateway,mask);
	if(INTERFACE_RETURN_CODE_SUCCESS == ret) {
		syslog_ax_arpsnooping_err("get gateway ip error when send arp solicit\n");
		return ARP_RETURN_CODE_ERROR;
	}
	else{
		ret = ARP_RETURN_CODE_SUCCESS;
	}
    for(j = 0;j<MAX_IP_COUNT;j++){
		if(INVALID_HOST_IP != gateway[j]){
		    syslog_ax_arpsnooping_dbg("send arp solicit from %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d\n", \
						    sysMac[0],sysMac[1],sysMac[2],sysMac[3],sysMac[4],sysMac[5],	\
						    (gateway[j]>>24)&0xFF,(gateway[j]>>16)&0xFF,(gateway[j]>>8)&0xFF,gateway[j]&0xFF);
		}
	}

	sleep(1);
	switch(intfType) {
		case KAP_INTERFACE_PORT_E:
			syslog_ax_arpsnooping_dbg("port intf send gratuitous arp ifindex %d eth %d \n",ifindex,eth_g_index);
			memset(&item,0,sizeof(struct arp_snooping_item_s));
			item.ifIndex = ifindex;
			item.dstIntf.isTrunk = FALSE;
			npd_get_devport_by_global_index(eth_g_index,&(item.dstIntf.intf.port.devNum),&(item.dstIntf.intf.port.portNum));
			item.vid = NPD_PORT_L3INTF_VLAN_ID;
			item.isTagged = FALSE;
			memset(item.mac,0xff,MAC_ADDR_LEN);
			for(j=0;j<MAX_IP_COUNT;j++){
			    item.ipAddr = gateway[j];
				if((INVALID_HOST_IP != gateway[j])&&(0 != gateway[j])){
					ret = nam_arp_solicit_send(&item,sysMac,gateway[j]);
					syslog_ax_arpsnooping_err("gratuitous arp send,ip: %d.%d.%d.%d ret %d\n",\
						(gateway[j]>>24)&0xff,(gateway[j]>>16)&0xff,(gateway[j]>>8)&0xff,(gateway[j]&0xff),ret);
				}
			}
			break;
		case KAP_INTERFACE_TRUNK_E:
			syslog_ax_arpsnooping_dbg("KAP TRUNK ");
			break;
		case KAP_INTERFACE_VIDX_E:
			syslog_ax_arpsnooping_dbg("KAP VIDX ");
			break;
		case KAP_INTERFACE_VID_E:
			syslog_ax_arpsnooping_dbg("vlan intf send gratuitous arp ifindex %d eth %d \n",ifindex,eth_g_index);
			ret = npd_vlan_get_all_active_ports(vid,&untagPorts,&tagPorts);
			if(NPD_SUCCESS != ret) {
				syslog_ax_arpsnooping_err("npd_vlan_get_all_ports error %d",ret);
				return ARP_RETURN_CODE_ERROR;
			}
			else{
				ret = ARP_RETURN_CODE_SUCCESS;
			}

			for(i = 0; i < untagPorts.count; i++) {
				memset(&item,0,sizeof(struct arp_snooping_item_s));
				item.ifIndex = ifindex;
				item.dstIntf.isTrunk = FALSE;
				npd_get_devport_by_global_index(untagPorts.ports[i],&(item.dstIntf.intf.port.devNum),&(item.dstIntf.intf.port.portNum));
				item.vid = vid;
				item.isTagged = FALSE;
				memset(item.mac,0xff,MAC_ADDR_LEN);
				for(j=0;j<MAX_IP_COUNT;j++){
					item.ipAddr = gateway[j];
					if((INVALID_HOST_IP != gateway[j])&&(0 != gateway[j])){
					    ret = nam_arp_solicit_send(&item,sysMac,gateway[j]);
						syslog_ax_arpsnooping_err("gratuitous arp send,ip: %d.%d.%d.%d ret %d\n",\
							(gateway[j]>>24)&0xff,(gateway[j]>>16)&0xff,(gateway[j]>>8)&0xff,(gateway[j]&0xff),ret);
					}
				}

			}

			for(i = 0; i < tagPorts.count; i++) {
				memset(&item,0,sizeof(struct arp_snooping_item_s));
				item.ifIndex = ifindex;
				item.dstIntf.isTrunk = FALSE;
				npd_get_devport_by_global_index(tagPorts.ports[i],&(item.dstIntf.intf.port.devNum),&(item.dstIntf.intf.port.portNum));
				item.vid = vid;
				item.isTagged = TRUE;
				memset(item.mac,0xff,MAC_ADDR_LEN);
				for(j=0;j<MAX_IP_COUNT;j++){
				    item.ipAddr = gateway[j];
					if((INVALID_HOST_IP != gateway[j])&&(0 != gateway[j])){
					    ret = nam_arp_solicit_send(&item,sysMac,gateway[j]);
						syslog_ax_arpsnooping_err("gratuitous arp send,ip: %d.%d.%d.%d ret %d\n",\
							(gateway[j]>>24)&0xff,(gateway[j]>>16)&0xff,(gateway[j]>>8)&0xff,(gateway[j]&0xff),ret);
					}
				}				
			}
			break;
		default:
			syslog_ax_arpsnooping_dbg("dev type error %d",intfType);
			break;
						
	}

	return ret;
}

/**********************************************************************************
 * npd_arp_snooping_gratuitous_send_for_new_ipaddr
 *	This method is used to send arp solicit packet
 *
 *	INPUT:
 *		ifindex	-- dev index
 *		vid		-- vlan id
 *		eth_g-index -- port index
 *		intfType	--   dev type
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS  - if no error occurred
 *		ARP_RETURN_CODE_ERROR - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_gratuitous_send_for_new_ipaddr
(
	unsigned int ifindex,
	unsigned int ipAddr,
	unsigned short vid,
	unsigned int eth_g_index,
	unsigned int     intfType
)
{
	int ret = ARP_RETURN_CODE_SUCCESS,i =0,j=0;
	struct vlan_ports  untagPorts,tagPorts;
	unsigned char sysMac[MAC_ADDR_LEN + 1] = {0};
	struct arp_snooping_item_s item;
	unsigned char ifStatus;
	unsigned int tmpIfIndex = ~0UI;
	memset(&untagPorts,0,sizeof(struct vlan_ports));
	memset(&tagPorts,0,sizeof(struct vlan_ports));

    if(KAP_INTERFACE_PORT_E == intfType){
		if((TRUE == npd_eth_port_interface_check(eth_g_index,&tmpIfIndex))&&\
			(~0UI != tmpIfIndex)&&(ifindex == tmpIfIndex)){
	        if(!((NPD_SUCCESS == npd_eth_port_l3intf_ustatus_get((unsigned short)eth_g_index,&ifStatus))&&\
				(ifStatus))){
				syslog_ax_arpsnooping_dbg("port interface status down when config new ip address\n");
				return ARP_RETURN_CODE_SUCCESS;
	        }
		}
		else{
			syslog_ax_arpsnooping_dbg("port intf unknown intf status, ifindex %d eth_g_index %d \n",tmpIfIndex,eth_g_index);
		}
    }
	else if(KAP_INTERFACE_VID_E == intfType){
		if((TRUE == npd_vlan_interface_check(vid,&tmpIfIndex))&&\
			(~0UI != tmpIfIndex)&&(ifindex == tmpIfIndex)){
			if(!((NPD_SUCCESS == npd_vlan_l3intf_get_ustatus(vid,&ifStatus))&&\
				(ifStatus))){
				syslog_ax_arpsnooping_dbg("vlan interface status down when config new ip address\n");
				return ARP_RETURN_CODE_SUCCESS;
			}
		}
		else{
			syslog_ax_arpsnooping_dbg("vlan intf unknown intf status,ifindex %d vid %d \n",tmpIfIndex,vid);
		}
	}
		/* 
	  * Get system mac address
	  */
	if(PRODUCT_MAC_ADDRESS) {
		/*ret = npd_system_get_basemac(sysMac,MAC_ADDR_LEN);*/
		ret = npd_intf_get_l3_intf_mac(ifindex,vid,eth_g_index,sysMac);
		if(INTERFACE_RETURN_CODE_SUCCESS!= ret) {
			syslog_ax_arpsnooping_err("get system mac error when send arp solicit for new ip address\n");
			return ARP_RETURN_CODE_ERROR;
		}
		else{
			ret = ARP_RETURN_CODE_SUCCESS;
		}
		syslog_ax_arpsnooping_dbg("get interface mac %02x:%02x:%02x:%02x:%02x:%02x, ifindex %#0x vid %d eth_g_index %#0x\n",\
				sysMac[0],sysMac[1],sysMac[2],sysMac[3],sysMac[4],sysMac[5],ifindex,vid,eth_g_index);
	}
	else {
		syslog_ax_arpsnooping_err("no system mac found when send arp solicit\n");
		return ARP_RETURN_CODE_ERROR;
	}

	syslog_ax_arpsnooping_dbg("send arp solicit from %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d\n", \
						    sysMac[0],sysMac[1],sysMac[2],sysMac[3],sysMac[4],sysMac[5],	\
						    (ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF,(ipAddr>>8)&0xFF,ipAddr&0xFF);

	switch(intfType) {
		case KAP_INTERFACE_PORT_E:
			syslog_ax_arpsnooping_dbg("port intf send gratuitous arp ifindex %d eth %d ",ifindex,eth_g_index);
			memset(&item,0,sizeof(struct arp_snooping_item_s));
			item.ifIndex = ifindex;
			item.dstIntf.isTrunk = FALSE;
			npd_get_devport_by_global_index(eth_g_index,&(item.dstIntf.intf.port.devNum),&(item.dstIntf.intf.port.portNum));
			item.vid = NPD_PORT_L3INTF_VLAN_ID;
			item.isTagged = FALSE;
			memset(item.mac,0xff,MAC_ADDR_LEN);
			item.ipAddr = ipAddr;
			nam_arp_solicit_send(&item,sysMac,ipAddr);
			break;
		case KAP_INTERFACE_TRUNK_E:
			syslog_ax_arpsnooping_dbg("KAP TRUNK ");
			break;
		case KAP_INTERFACE_VIDX_E:
			syslog_ax_arpsnooping_dbg("KAP VIDX ");
			break;
		case KAP_INTERFACE_VID_E:
		
			ret = npd_vlan_get_all_active_ports(vid,&untagPorts,&tagPorts);
			if(0 != ret) {
				syslog_ax_arpsnooping_err("npd_vlan_get_all_ports error %d",ret);
				return ARP_RETURN_CODE_ERROR;
			}

			for(i = 0; i < untagPorts.count; i++) {
				memset(&item,0,sizeof(struct arp_snooping_item_s));
				item.ifIndex = ifindex;
				item.dstIntf.isTrunk = FALSE;
				npd_get_devport_by_global_index(untagPorts.ports[i],&(item.dstIntf.intf.port.devNum),&(item.dstIntf.intf.port.portNum));
				item.vid = vid;
				item.isTagged = FALSE;
				memset(item.mac,0xff,MAC_ADDR_LEN);
				item.ipAddr = ipAddr;
				nam_arp_solicit_send(&item,sysMac,ipAddr);
				

			}

			for(i = 0; i < tagPorts.count; i++) {
				memset(&item,0,sizeof(struct arp_snooping_item_s));
				item.ifIndex = ifindex;
				item.dstIntf.isTrunk = FALSE;
				npd_get_devport_by_global_index(tagPorts.ports[i],&(item.dstIntf.intf.port.devNum),&(item.dstIntf.intf.port.portNum));
				item.vid = vid;
				item.isTagged = TRUE;
				memset(item.mac,0xff,MAC_ADDR_LEN);
				item.ipAddr = ipAddr;
				nam_arp_solicit_send(&item,sysMac,ipAddr);
			}
			break;
			default:
				syslog_ax_arpsnooping_dbg("dev type error %d",intfType);
				break;
						
	}
	npd_syslog_dbg("gratuitous arp solicit send for new ip %d.%d.%d.%d ret %d\n", \
											(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,(ipAddr>>8)&0xff,ipAddr&0xff,ret);

	return ret;
}


/**********************************************************************************
 * npd_arp_snooping_solicit_send
 *	This method is used to send arp solicit packet
 *
 *	INPUT:
 *		item - arp snooping SW hash table items.
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS  - if no error occurred
 *		COMMON_RETURN_CODE_NULL_PTR - if item is null
 *		ARP_RETURN_CODE_ERROR - if other error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_solicit_send
(
	struct arp_snooping_item_s *item
)
{
	int result = ARP_RETURN_CODE_SUCCESS;
	unsigned char sysMac[MAC_ADDR_LEN + 1] = {0};
	unsigned int gateway[MAX_IP_COUNT] = {INVALID_HOST_IP};
	unsigned int mask[MAX_IP_COUNT] = {0};
	int i = 0;
	unsigned int eth_g_index = 0;
	
	if(NULL == item) {
		return COMMON_RETURN_CODE_NULL_PTR;
	}

	/* 
	  * Get system mac address
	  */
	if(PRODUCT_MAC_ADDRESS) {
		if(NPD_SUCCESS == npd_get_global_index_by_devport
			(item->dstIntf.intf.port.devNum,item->dstIntf.intf.port.portNum,&eth_g_index)){
			result = npd_intf_get_l3_intf_mac(item->ifIndex,item->vid,eth_g_index,sysMac);
			/*result = npd_system_get_basemac(sysMac,MAC_ADDR_LEN);*/
			if(INTERFACE_RETURN_CODE_SUCCESS != result) {
				syslog_ax_arpsnooping_err("get interface mac error when send arp solicit\n");
				return ARP_RETURN_CODE_ERROR;
			}
			syslog_ax_arpsnooping_dbg("get interface mac %02x:%02x:%02x:%02x:%02x:%02x \n",\
				sysMac[0],sysMac[1],sysMac[2],sysMac[3],sysMac[4],sysMac[5]);
		}
		else{
			syslog_ax_arpsnooping_err("get port global index FAILED when send arp solicit\n");
            return ARP_RETURN_CODE_ERROR;
		}
	}
	else {
		syslog_ax_arpsnooping_err("no system mac found when send arp solicit\n");
		return ARP_RETURN_CODE_ERROR;
	}

	/*
	 *  Get gateway ip address
	 */
	result = npd_intf_get_ip_addrs(item->ifIndex,gateway,mask);
	if(INTERFACE_RETURN_CODE_SUCCESS != result) {
		syslog_ax_arpsnooping_err("get gateway ip error when send arp solicit,ret %#0x \n",result);
		return ARP_RETURN_CODE_ERROR;
	}

	/*
	 * Start send packet
	 */
	for(i=0;i<MAX_IP_COUNT;i++){
		if((INVALID_HOST_IP != gateway[i])&&((item->ipAddr&mask[i]) == (gateway[i]&mask[i]))){
		    syslog_ax_arpsnooping_dbg("send arp solicit from %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d\n", \
				    sysMac[0],sysMac[1],sysMac[2],sysMac[3],sysMac[4],sysMac[5],	\
				    (gateway[i]>>24)&0xFF,(gateway[i]>>16)&0xFF,(gateway[i]>>8)&0xFF,gateway[i]&0xFF);
		    result = nam_arp_solicit_send(item,sysMac,gateway[i]);
			syslog_ax_arpsnooping_dbg("nam arp solicit send %s ret %#0x \n",\
				(result == ARP_RETURN_CODE_SUCCESS)?"SUCCESS":"FAILED",result);
		}
	}	
	return ARP_RETURN_CODE_SUCCESS;
}

/************************************************************************************
 *npd_arpsnooping_parse_rtattr
 *		NPD GET KERN ARP MSG
 * INPUT:
 * 		max	-	rta type max bound
 *		rta  -	rtattr we want to parse
 *		len -	check length
 * OUTPUT:
 *		tb	- data parse get
 * RETURN:
 * 		COMMON_RETURN_CODE_NULL_PTR  - parameter null pointer
 *		ARP_RETURN_CODE_SUCCESS  - parse finish
 *
 ************************************************************************************/
struct arp_stats g_arp_stats;

int npd_arp_snooping_parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta, int len)
{
	if((NULL == tb)||(NULL == rta)){
		return COMMON_RETURN_CODE_NULL_PTR;
	}
	memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
	while (RTA_OK(rta, len)) {
		if (rta->rta_type <= max)
			tb[rta->rta_type] = rta;
		rta = RTA_NEXT(rta,len);
	}
	if (len)
		fprintf(stderr, "!!!Deficit %d, rta_len=%d\n", len, rta->rta_len);
	return ARP_RETURN_CODE_SUCCESS;
}

#ifndef AX_NEIGH_USER
/************************************************************************
 * npd_arp_snooping_neigh_get_info
 *	This method is used to parse kernel netlink message and get neighbour info.
 *
 *	INPUT:
 *		msgPtr - RT Netlink message data
 *		payloadlen - message payload length
 *	
 *	OUTPUT:
 *		neighEntry - ARP snooping neigh table brief info.
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_ERROR - if error occurs
 *		
 *	NOTE:
 *
 **************************************************************************/
int npd_arp_snooping_neigh_get_info
(
	struct ndmsg *	msgPtr,
	int payloadlen,
	struct neigh_tbl_info *neighEntry
) 
{
	struct rtattr *rtattrptr = NULL;
	struct in_addr *dst;
	unsigned char *tmp;

	if(!msgPtr || !neighEntry)
	{
		syslog_ax_arpsnooping_err("npd get neigh info param NULL\n");
		return ARP_RETURN_CODE_ERROR;
	}

	if(AF_INET!=msgPtr->ndm_family)
		return ARP_RETURN_CODE_ERROR;

	neighEntry->ifIndex = msgPtr->ndm_ifindex;
	/*zhangcl@autelan.com clean the print message*/
	#if 0
	syslog_ax_arpsnooping_dbg("neigh ifindex %x",neighEntry->ifIndex);
	#endif
	neighEntry->state = msgPtr->ndm_state;
	
	rtattrptr = (struct rtattr *) RTM_RTA(msgPtr);

	for(;RTA_OK(rtattrptr, payloadlen);rtattrptr=RTA_NEXT(rtattrptr,payloadlen))
	{
		switch(rtattrptr->rta_type) 
		{
			/* ip address */
			case NDA_DST:
				dst = (struct in_addr*)(RTA_DATA(rtattrptr));
				neighEntry->ipAddr = dst->s_addr;
				/*zhangcl@autelan.com clean the print message*/
				#if 0
				syslog_ax_arpsnooping_dbg("neigh ip address %d.%d.%d.%d",(neighEntry->ipAddr>>24)&0xFF,	\
					(neighEntry->ipAddr>>16)&0xFF,(neighEntry->ipAddr>>8)&0xFF,neighEntry->ipAddr&0xFF);
				#endif
				break;
			/* MAC address */
			case NDA_LLADDR:
				tmp = (unsigned char*)RTA_DATA(rtattrptr);
				memcpy(neighEntry->mac,tmp,6);
				/*zhangcl@autelan.com clean the print message*/
				#if 0
				syslog_ax_arpsnooping_dbg("neigh mac %02x:%02x:%02x:%02x:%02x:%02x",tmp[0],tmp[1],tmp[2],tmp[3],tmp[4],tmp[5]);
				#endif
				break;
			default:
				/*zhangcl@autelan.com clean the print message*/
				#if 0
				syslog_ax_arpsnooping_err("ignore attributes %d\n",rtattrptr->rta_type);
				#endif
				break;
		}
	}
	
	return ARP_RETURN_CODE_SUCCESS;	
}
#endif

/**********************************************************************************
 * npd_arp_snooping_synchronize_item
 *	This method is used to synchronizing ARP items to kernel neigh table entry.
 *
 *	INPUT:
 *		n - RT Netlink handler
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_ERROR - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_synchronize_item
(
	struct nlmsghdr *n
)
{
	struct ndmsg *ndm = NLMSG_DATA(n);
	int len = n->nlmsg_len;
	struct rtattr * tb[NDA_MAX+1];
	struct in_addr* dst = NULL;
#ifndef AX_NEIGH_USER
	struct ndmsg *neighEntry = NULL;
	struct neigh_tbl_info neighInfo;
	struct arp_snooping_item_s *arpInfo = NULL;
	struct route_nexthop_brief_s routeItem;
	int payloadoff = 0;
#endif
	

	if (n->nlmsg_type == NLMSG_DONE) {
		return ARP_RETURN_CODE_SUCCESS;
	}

	if (n->nlmsg_type != RTM_GETNEIGH && n->nlmsg_type != RTM_NEWNEIGH)
		return ARP_RETURN_CODE_SUCCESS;

	len -= NLMSG_LENGTH(sizeof(*ndm));
	if (len < 0)
		return ARP_RETURN_CODE_ERROR;

	if (ndm->ndm_family != AF_INET ||
	    ndm->ndm_flags ||
	    ndm->ndm_type != RTN_UNICAST ||
	    !(ndm->ndm_state&~NUD_NOARP))
		return ARP_RETURN_CODE_SUCCESS;

/*zhangcl@autelan.com clean the print message*/
#if 0
	syslog_ax_arpsnooping_dbg("get msg family %d, ifindex %d, flags %d, type %d, state %d\n ",		\
					ndm->ndm_family,ndm->ndm_ifindex,ndm->ndm_flags,
				    ndm->ndm_type,ndm->ndm_state);
#endif
	npd_arp_snooping_parse_rtattr(tb, NDA_MAX, NDA_RTA(ndm), len);

	if (!tb[NDA_DST])
		return ARP_RETURN_CODE_SUCCESS;

	switch(n->nlmsg_type) {
		case RTM_GETNEIGH:
		{
			if (!(n->nlmsg_flags&NLM_F_REQUEST))
				return ARP_RETURN_CODE_SUCCESS;

			if (!(ndm->ndm_state&(NUD_PROBE|NUD_INCOMPLETE))) {
				g_arp_stats.app_bad++;
				return ARP_RETURN_CODE_SUCCESS;
			}

			if (ndm->ndm_state&NUD_PROBE) {
				/* If we get this, kernel still has some valid
				 * address, but unicast probing failed and host
				 * is either dead or changed its mac address.
				 * Kernel is going to initiate broadcast resolution.
				 * OK, we invalidate our information as well.
				 */
				syslog_ax_arpsnooping_err("invalidate our information\n");
				
			} 
			else if(NUD_INCOMPLETE & ndm->ndm_state) {
				dst = (struct in_addr*)(RTA_DATA(tb[NDA_DST]));
				syslog_ax_arpsnooping_dbg("host %d.%d.%d.%d state incomplete\n",	\
						(dst->s_addr>>24)&0xFF,(dst->s_addr>>16)&0xFF,(dst->s_addr>>8)&0xFF,dst->s_addr&0xFF);
			}
			else {
				syslog_ax_arpsnooping_dbg("get message indicates kernel does not have any information\n");
			}
		}
		break;
#ifndef AX_NEIGH_USER
		case RTM_NEWNEIGH:
		case RTM_DELNEIGH:
			/*zhangcl@autelan.com clean the print message*/
			#if 0
			syslog_ax_arpsnooping_dbg("message for new neighbour entry %s",RTM_NEWNEIGH ? "attached":"detached");
			#endif
			neighEntry = (struct ndmsg *) NLMSG_DATA(n);
			payloadoff = RTM_PAYLOAD(n);
			memset(&neighInfo,0,sizeof(struct neigh_tbl_info));
			memset(&routeItem,0,sizeof(struct route_nexthop_brief_s));
			
			if(ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_neigh_get_info(neighEntry,payloadoff,&neighInfo))
				break;
			if(NUD_FAILED == neighInfo.state) {
				/* TODO: call arp snooping delete entry*/
				/*
 				 * When state NUD_FAILED, no mac address provided by kernel message
				 */
				pthread_mutex_lock(&arpHashMutex);
				arpInfo = npd_arp_snooping_find_item_byip(neighInfo.ifIndex,neighInfo.ipAddr);
				if(NULL == arpInfo) {
					/*zhangcl@autelan.com clean the print message*/
					#if 0
					syslog_ax_arpsnooping_err("no arp item for %#0x %d.%d.%d.%d in FAILED\n", \
						neighInfo.ifIndex,(neighInfo.ipAddr >> 24)&0xFF, \
						(neighInfo.ipAddr >> 16)&0xFF,(neighInfo.ipAddr >> 8)&0xFF,neighInfo.ipAddr&0xFF);
					#endif
					pthread_mutex_unlock(&arpHashMutex);
					break;
				}
				else {
					if(TRUE != arpInfo->isStatic){
         				npd_arp_delete(arpInfo,TRUE);
						syslog_ax_arpsnooping_dbg("call arp snooping delete entry:if %d ip %d.%d.%d.%d!", \
						       neighInfo.ifIndex,(neighInfo.ipAddr>>24)&0xff,(neighInfo.ipAddr>>16)&0xff, \
						       (neighInfo.ipAddr>>8)&0xff,neighInfo.ipAddr&0xff);
					}
					else{
						npd_static_arp_set_invalid(arpInfo);
						syslog_ax_arpsnooping_dbg("call static arp valid set entry:if %d ip %d.%d.%d.%d!", \
						       neighInfo.ifIndex,(neighInfo.ipAddr>>24)&0xff,(neighInfo.ipAddr>>16)&0xff, \
						       (neighInfo.ipAddr>>8)&0xff,neighInfo.ipAddr&0xff);
					}
				}
				pthread_mutex_unlock(&arpHashMutex);
			}
			else if(NUD_STALE == neighInfo.state){
				/* TODO: call arp snooping send ARP_REQUEST*/
				if(0 == neighInfo.ipAddr){
					break;
				}
				pthread_mutex_lock(&arpHashMutex);
				arpInfo = npd_arp_snooping_find_item_bymacip(neighInfo.ifIndex,neighInfo.mac,neighInfo.ipAddr);
				if(NULL == arpInfo) {
					/*zhangcl@autelan.com clean the print message*/
					#if 0
					syslog_ax_arpsnooping_err("no arp item for %#0x %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d in STALE\n", \
						neighInfo.ifIndex,neighInfo.mac[0],neighInfo.mac[1],neighInfo.mac[2],neighInfo.mac[3], \
						neighInfo.mac[4],neighInfo.mac[5],(neighInfo.ipAddr >> 24)&0xFF, \
						(neighInfo.ipAddr >> 16)&0xFF,(neighInfo.ipAddr >> 8)&0xFF,neighInfo.ipAddr&0xFF);
					#endif
					pthread_mutex_unlock(&arpHashMutex);
					break;
				}
				else {
					npd_arp_snooping_solicit_send(arpInfo);
					pthread_mutex_unlock(&arpHashMutex);
				}
			}
			else {
				/*zhangcl@autelan.com clean the print message*/
				#if 0
				syslog_ax_arpsnooping_dbg("ignore neigh message as state %d",neighInfo.state);
				#endif
			}
			break;
#endif
		default:
			break;
	}
	return ARP_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * npd_arp_snooping_get_kmsg
 *	This method is used to get rtnetlink message from kernel.
 *
 *	INPUT:
 *		rth - RT Netlink handler
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *		
 *	NOTE:
 *
 **********************************************************************************/
void npd_arp_snooping_get_kmsg
(
	struct rtnl_handle* rth
)
{
	int status;
	struct nlmsghdr *h;
	struct sockaddr_nl nladdr;
	struct iovec iov;
	static char   buf[8192] = {0};
	struct msghdr msg = {
		(void*)&nladdr,sizeof(nladdr),&iov, 1,NULL, 0, 0
	};

	memset(&iov, 0, sizeof(struct iovec));
	memset(&nladdr, 0, sizeof(nladdr));

	iov.iov_base = buf;
	iov.iov_len = sizeof(buf);

	status = recvmsg(rth->fd, &msg, MSG_DONTWAIT);

	if (status <= 0)
		return;

	if (msg.msg_namelen != sizeof(nladdr))
		return;

	if (nladdr.nl_pid) /* 0 means message from kernel*/
		return;

	for (h = (struct nlmsghdr*)buf; status >= sizeof(*h); ) {
		int len = h->nlmsg_len;
		int l = len - sizeof(*h);

		if (l < 0 || len > status)
			return;

		if (npd_arp_snooping_synchronize_item(h) < 0)
			return;

		status -= NLMSG_ALIGN(len);
		h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
	}
}
/**********************************************************************************
 * npd_arp_snooping_synchronize_start
 *	This method is used to start synchronizing ARP items to kernel neigh table entry every time interval,
 * which default as 30ms.
 *
 *	INPUT:
 *		rth - RT Netlink handler
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS 
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_synchronize_start
(
	struct rtnl_handle* rth
)
{
	while(1) {
		npd_arp_snooping_get_kmsg(rth);		
		npd_arp_snooping_aging_delay();
	}

	return ARP_RETURN_CODE_SUCCESS ;
}

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
 *		ARP_RETURN_CODE_SUCCESS  - if no error occurred
 *		ARP_RETURN_CODE_FD_ERROR - fd < 0
 *		ARP_RETURN_CODE_ERROR  - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_sync2kern
(
	void
)
{
	struct rtnl_handle rth;
	socklen_t addr_len;
	int sndbuf = 32768;
	int rcvbuf = 32768;

	memset(&rth, 0, sizeof(struct rtnl_handle));

	/* tell my thread id*/
	npd_init_tell_whoami("ArpSync",0);
	
	rth.fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (rth.fd < 0) {
		npd_syslog_err("Cannot open netlink socket \n");
		return ARP_RETURN_CODE_FD_ERROR;
	}

	if (setsockopt(rth.fd,SOL_SOCKET,SO_SNDBUF,&sndbuf,sizeof(sndbuf)) < 0) {
		npd_syslog_err(" setsockopt SO_SNDBUF fail \n");
		return ARP_RETURN_CODE_ERROR;
	}

	if (setsockopt(rth.fd,SOL_SOCKET,SO_RCVBUF,&rcvbuf,sizeof(rcvbuf)) < 0) {
		npd_syslog_err("setsockopt SO_RCVBUF fail \n");
		return ARP_RETURN_CODE_ERROR;
	}

	memset(&rth.local, 0, sizeof(rth.local));
	rth.local.nl_family = AF_NETLINK;
#ifndef AX_NEIGH_USER
	rth.local.nl_groups |= (RTMGRP_NEIGH|RTM_NEWNEIGH|RTM_DELNEIGH);
	rth.local.nl_pid = getpid();
#endif
#ifdef CPU_ARM_XCAT
	rth.local.nl_pid += g_npd_thread_number;
#endif
	if (bind(rth.fd, (struct sockaddr*)&rth.local, sizeof(rth.local)) < 0) {
		npd_syslog_err("Cannot bind netlink socket \n");
		return ARP_RETURN_CODE_ERROR;
	}
	addr_len = sizeof(rth.local);
	if (getsockname(rth.fd, (struct sockaddr*)&rth.local, &addr_len) < 0) {
		npd_syslog_err("Cannot getsockname \n");
		return ARP_RETURN_CODE_ERROR;
	}
	if (addr_len != sizeof(rth.local)) {
		fprintf(stderr, "Wrong address length %d\n", addr_len);
		return ARP_RETURN_CODE_ERROR;
	}
	if (rth.local.nl_family != AF_NETLINK) {
		fprintf(stderr, "Wrong address family %d\n", rth.local.nl_family);
		return ARP_RETURN_CODE_ERROR;
	}
	rth.seq = time(NULL);

 	/*rtnl_wilddump_request(&rth);*/

	npd_arp_snooping_synchronize_start(&rth);

	close(rth.fd);
	return ARP_RETURN_CODE_SUCCESS;
}


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
 *		TURE - if no error occurred
 *		FALSE - if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/


int npd_arp_snooping_smac_check
(
	unsigned char* etherSmac,
	unsigned char* arpSmac
)
{
	if((etherSmac == NULL) || (arpSmac == NULL))
		return FALSE;
		
	if((etherSmac[0] != arpSmac[0]) ||	\
		   (etherSmac[1] != arpSmac[1]) ||	\
		   (etherSmac[2] != arpSmac[2]) ||	\
		   (etherSmac[3] != arpSmac[3]) ||	\
		   (etherSmac[4] != arpSmac[4]) ||	\
		   (etherSmac[5] != arpSmac[5]))
		{
			syslog_ax_arpsnooping_err("npd process arp packet as ethernet smac don't equal to arp smac\n");
			/*printf("npd process arp packet as ethernet smac don't equal to arp smac\n");*/
			syslog_ax_arpsnooping_dbg("ethernet smac %02x:%02x:%02x:%02x:%02x:%02x",	\
						etherSmac[0],etherSmac[1],etherSmac[2],etherSmac[3],etherSmac[4],etherSmac[5]);
			syslog_ax_arpsnooping_dbg("arp smac %02x:%02x:%02x:%02x:%02x:%02x", \
						arpSmac[0],arpSmac[1],arpSmac[2],arpSmac[3],arpSmac[4],arpSmac[5]);
			return FALSE;
		}
	else
		return TRUE;

}
/*****************************************************
 *npd_arp_snooping_is_brc_mac
 *		check whether the mac address is a broadcast address
 * INPUT:
 *		mac  - the mac address we want to check
 * OUTPUT:
 *		NONE
 * RETURN:
 *		TRUE  - the mac is a broadcast address
 *		FALSE - the mac is not a broadcast address
 * NOTE:
 *
 *****************************************************/
int npd_arp_snooping_is_brc_mac(char * mac)
{
    if(NULL == mac){
        return FALSE;
	}
	if(~(mac[0]&mac[1]&mac[2]&mac[3]&mac[4]&mac[5])){
		return FALSE;
	}
	return TRUE;
}

/*****************************************************
 *npd_arp_snooping_is_brc_mac
 *		check whether the mac address is a muticast address
 * INPUT:
 *		mac  - the mac address we want to check
 * OUTPUT:
 *		NONE
 * RETURN:
 *		TRUE  - the mac is a muticast address
 *		FALSE - the mac is not a muticast address
 * NOTE:
 *
 *****************************************************/

int npd_arp_snooping_is_muti_cast_mac
(
    char * mac
)
{
  if(mac[0] & 0x1){
      return TRUE;
  }
  else{
      return FALSE;
  }
}

/*****************************************************
 *npd_arp_snooping_is_brc_mac
 *		check whether the mac address is a zero address
 * INPUT:
 *		mac  - the mac address we want to check
 * OUTPUT:
 *		NONE
 * RETURN:
 *		TRUE  - the mac is a zero address
 *		FALSE - the mac is not a zero address
 * NOTE:
 *
 *****************************************************/

int npd_arp_snooping_is_zero_mac(char * mac)
{
    if(NULL == mac){
        return FALSE;
	}
	if(mac[0]||mac[1]||mac[2]||mac[3]||mac[4]||mac[5]){
		return FALSE;
	}
	return TRUE;
}

/*****************************************************
 *npd_arp_snooping_is_brc_mac
 *		check whether the mac address is a zero address
 * INPUT:
 *		mac1  - the first mac address we want to compare
 *		mac2  - the second mac address we want to compare
 * OUTPUT:
 *		NONE
 * RETURN:
 *		TRUE  - the two mac are equal to each other 
 *		FALSE - the two mac are not equal
 * NOTE:
 *
 *****************************************************/

int npd_arp_snooping_are_equal_macs
(
    char * mac1,
    char * mac2
)
{
    int i=0;
    if(NULL == mac1||NULL == mac2){
		return FALSE;
    }
#if 1
	if((mac1[0]^mac2[0])||
		(mac1[1]^mac2[1])||
		(mac1[2]^mac2[2])||
		(mac1[3]^mac2[3])||
		(mac1[4]^mac2[4])||
		(mac1[5]^mac2[5])){
		return FALSE;
	}
#else
	if(0 != memcmp(mac1,mac2,MAC_ADDR_LEN)){
		return FALSE;
	}
#endif
	return TRUE;
}

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
*    FALSE --the macs are illegal
*    TRUE -- the macs are legal
******************************************************************/
int npd_arp_snooping_mac_legality_check
(
    char * ethSmac,
    char * ethDmac,
    char * arpSmac,
    char * arpDmac,
    unsigned short opCode
)
{
    if((NULL == ethSmac)||(NULL == ethDmac)||\
		(NULL == arpSmac)||(NULL == arpDmac)){
        return FALSE;
	}
	if(ARP_REQUEST == opCode){
		if(((!(npd_arp_snooping_is_brc_mac(ethDmac)))&&(npd_arp_snooping_is_muti_cast_mac(ethDmac)))||\
			((npd_arp_snooping_is_zero_mac(ethDmac))||(npd_arp_snooping_is_muti_cast_mac(ethSmac)))||\
			((npd_arp_snooping_is_zero_mac(ethSmac))||(npd_arp_snooping_is_muti_cast_mac(arpDmac)))||\
			(!((npd_arp_snooping_is_zero_mac(arpDmac))||(npd_arp_snooping_are_equal_macs(arpDmac,arpSmac))))||\
			((npd_arp_snooping_is_muti_cast_mac(arpSmac))||(npd_arp_snooping_is_zero_mac(arpSmac)))){
            return FALSE; 
		}
	}
	else if(ARP_REPLY == opCode){
        if((npd_arp_snooping_is_muti_cast_mac(ethDmac))||(npd_arp_snooping_is_zero_mac(ethDmac))||\
			(npd_arp_snooping_is_muti_cast_mac(ethSmac))||(npd_arp_snooping_is_zero_mac(ethSmac))||\
			(npd_arp_snooping_is_muti_cast_mac(arpDmac))||(npd_arp_snooping_is_zero_mac(arpDmac))||\
			(npd_arp_snooping_is_muti_cast_mac(arpSmac))||(npd_arp_snooping_is_zero_mac(arpSmac))){
            return FALSE; 
		}
	}
    return TRUE;
	
}

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
 *		ARP_RETURN_CODE_SUCCESS 	- 	if no error occurred
 *		ARP_RETURN_CODE_ERROR		-	if error occurs
 *		
 *	NOTE:
 *
 **********************************************************************************/
int npd_arp_snooping_create_kern_static_arp
(
	unsigned int ipAddr,
	unsigned int ipMasklen,
	unsigned char* mac
)
{

	int ret = ARP_RETURN_CODE_SUCCESS;
	struct arpreq arp_cfg;
	struct sockaddr_in* sin = NULL,*mask = NULL;

	
	if(sysKernArpSock <= 0) {
		syslog_ax_arpsnooping_err("npd del kern arp sysKernArpSock error %d",sysKernArpSock);
		return ARP_RETURN_CODE_ERROR;
	}

	memset(&arp_cfg,0,sizeof(arp_cfg));
    sin = (struct sockaddr_in *) &arp_cfg.arp_pa;
    sin->sin_family = AF_INET;
	sin->sin_port = 0;
    sin->sin_addr.s_addr =ntohl(ipAddr);
	arp_cfg.arp_flags = ATF_PERM | ATF_COM;
	arp_cfg.arp_ha.sa_family = 1;  /* Ethernet 10Mbps*/
	memcpy(arp_cfg.arp_ha.sa_data,mac,MAC_ADDRESS_LEN);
	mask = (struct sockaddr_in *) &arp_cfg.arp_netmask;
	mask->sin_addr.s_addr = 0xffffffff;

    pthread_mutex_lock(&arpKernOpMutex);
	if ((ret = ioctl(sysKernArpSock, SIOCSARP, &arp_cfg) )< 0) {
		syslog_ax_arpsnooping_err("npd create kern static arp  error %d",errno);
		pthread_mutex_unlock(&arpKernOpMutex);
		return ARP_RETURN_CODE_ERROR;
	}
	pthread_mutex_unlock(&arpKernOpMutex);
    syslog_ax_arpsnooping_dbg("npd create kern static arp success \n");
	return ARP_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * npd_arp_snooping_create_static_arp
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
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		ARP_RETURN_CODE_STATIC_ARP_FULL  - if the static arp items are equal to 1024 or more
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		ARP_RETURN_CODE_TABLE_FULL  - the hash table is full
 *		ARP_RETURN_CODE_STATIC_EXIST - static arp item already exists
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ARP_RETURN_CODE_ERROR  - if other error occurs,get dev port failed or nexthop op failed
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
)
{
	int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned char devNum = 0,portNum = 0;
	struct arp_snooping_item_s arpItem,*arpInfo = NULL;
	struct route_nexthop_brief_s nextHopItem;
	unsigned int nextHopindex = 0, dupIfindex = 0;
	unsigned char isTagged = FALSE;
	unsigned int arpCount = 0;
	struct arp_snooping_item_s dupItem;
	if(intfType > ARP_INTF_QINQ_SUBIF){
        return ARP_RETURN_CODE_ERROR;
	}
	arpCount = npd_count_all_static_arp();
	if( 1024 <= arpCount){
		syslog_ax_arpsnooping_err("get static arp count %d \n",arpCount);
		return ARP_RETURN_CODE_STATIC_ARP_FULL;
	}
	memset(&arpItem,0,sizeof(struct arp_snooping_item_s));
	memset(&dupItem,0,sizeof(struct arp_snooping_item_s));
	memset(&nextHopItem,0,sizeof(struct route_nexthop_brief_s));
	if(ARP_INTF_ADVANCED_VLAN != intfType){
	    ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
        if(NPD_SUCCESS != ret){
    		return ARP_RETURN_CODE_ERROR;
        }
	}
    else{
        devNum = 0;
        portNum = 255;
    }
	
	isTagged = npd_vlan_check_port_membership(vid,eth_g_index,TRUE);
	syslog_ax_arpsnooping_dbg("static ip info ifindex %d,ipAddr %#02x ,ipMask %d,",ifindex,ipAddr,ipMask);
	arpItem.ifIndex = ifindex;
	arpItem.ipAddr = ipAddr;
	arpItem.isStatic = TRUE;
	arpItem.isTagged = isTagged;
	arpItem.isValid = TRUE;
	arpItem.vid = vid;
    if(vid2){
        arpItem.vid2 = vid2;
    }
	arpItem.dstIntf.intf.port.devNum = devNum;
	arpItem.dstIntf.intf.port.portNum = portNum;
	memcpy(arpItem.mac,mac,MAC_ADDRESS_LEN);
	arpInfo = npd_arp_snooping_find_item_byip(ifindex,ipAddr);
	/*arpInfo=hash_search(arp_snoop_db_s,arpItem,NULL);*/
	if((NULL == arpInfo) || (FALSE == arpInfo->isStatic)) {
		if((ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_create_kern_static_arp(ipAddr,ipMask,mac))){/*kernal success */
			arpItem.isValid = FALSE;
		}
		if((FALSE == arpItem.isValid)||(intfType >= ARP_INTF_ADVANCED_VLAN)){/* intf is not up or promis intf */			    
				ret = npd_arp_snooping_op_item(&arpItem,ARP_SNOOPING_ADD_ITEM,&dupItem);
				if(ARP_RETURN_CODE_DUPLICATED == ret){
					ret = ARP_RETURN_CODE_SUCCESS;
				}
		}
		else{
			ret = npd_arp_snooping_learning(&arpItem,&nextHopindex,&dupIfindex);
			if(ARP_RETURN_CODE_SUCCESS == ret) {
				/* TODO: build route table hash*/
				nextHopItem.ifIndex = ifindex;
				nextHopItem.ipAddr 	= ipAddr;
				nextHopItem.tblIndex= nextHopindex;
				pthread_mutex_lock(&nexthopHashMutex);
				ret = npd_route_nexthop_op_item(&nextHopItem,NEXTHOP_ADD_ITEM,dupIfindex);
				pthread_mutex_unlock(&nexthopHashMutex);
				if(ROUTE_RETURN_CODE_SUCCESS != ret) {
					syslog_ax_arpsnooping_err("npd nexthop brief add to db error %d",ret);
					return ARP_RETURN_CODE_ERROR;
				}

				/* TODO:add host route to RT*/
				ret = npd_route_table_op_host_item(&nextHopItem,ROUTE_ADD_ITEM);
				if(ROUTE_RETURN_CODE_SUCCESS != ret) {
					syslog_ax_arpsnooping_err("npd route host add to HW error %d",ret);
					return ARP_RETURN_CODE_ERROR;
				}
				ret = ARP_RETURN_CODE_SUCCESS;
			}
		}
	}
	else{
		ret = ARP_RETURN_CODE_STATIC_EXIST;
	}
	return ret;
}


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
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ARP_RETURN_CODE_STATIC_ARP_FULL  - if the static arp items are equal to 1024 or more
 *		ARP_RETURN_CODE_TABLE_FULL  - the hash table is full
 *		ARP_RETURN_CODE_STATIC_EXIST - static arp item already exists
 *		ARP_RETURN_CODE_ERROR  - if other error occurs,get dev port failed or nexthop op failedf
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
)
{
	int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned char devNum = 0;
	struct arp_snooping_item_s arpItem,*arpInfo = NULL;
	struct route_nexthop_brief_s nextHopItem;
	unsigned int nextHopindex = 0, dupIfindex = 0;
	unsigned char isTagged = NPD_FALSE;
	unsigned int arpCount = 0;
	unsigned int tmpifindex = ~0UI;
	struct arp_snooping_item_s dupItem;

	arpCount = npd_count_all_static_arp();
	if( 1024 <= arpCount){
		return ARP_RETURN_CODE_STATIC_ARP_FULL;
	}
	memset(&arpItem,0,sizeof(struct arp_snooping_item_s));
	memset(&nextHopItem,0,sizeof(struct route_nexthop_brief_s));
	
	isTagged = npd_vlan_check_trunk_membership(vid,trunkId,TRUE);
	syslog_ax_arpsnooping_dbg("static ip info ifindex %d,ipAddr %#02x ,ipMask %d,",ifindex,ipAddr,ipMask);
	arpItem.ifIndex = ifindex;
	arpItem.ipAddr = ipAddr;
	arpItem.isStatic = TRUE;
	arpItem.isTagged = isTagged;
	arpItem.isValid = TRUE;
	arpItem.vid = vid;
	arpItem.dstIntf.isTrunk = TRUE;
	arpItem.dstIntf.intf.trunk.devNum = devNum;
	arpItem.dstIntf.intf.trunk.trunkId = trunkId;
	memcpy(arpItem.mac,mac,MAC_ADDRESS_LEN);
	arpInfo = npd_arp_snooping_find_item_byip(ifindex,ipAddr);
	/*arpInfo=hash_search(arp_snoop_db_s,arpItem,NULL);*/
	if((NULL == arpInfo) || (FALSE == arpInfo->isStatic)) {
		if(((TRUE == npd_vlan_interface_check(vid,&tmpifindex))&&\
			(~0UI != tmpifindex)&&(ifindex == tmpifindex))){
			if((DEV_LINK_UP != npd_check_vlan_status(vid))||\
		   (ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_create_kern_static_arp(ipAddr,ipMask,mac))){
			    arpItem.isValid = FALSE;
				ret = npd_arp_snooping_op_item(&arpItem,ARP_SNOOPING_ADD_ITEM,&dupItem);
				if(ARP_RETURN_CODE_DUPLICATED == ret){
					ret = ARP_RETURN_CODE_SUCCESS;
				}
				return ret;
			}
		}
		else{
			return L3_INTF_EXIST;
		}
		ret = npd_arp_snooping_learning(&arpItem,&nextHopindex,&dupIfindex);
		if(ARP_RETURN_CODE_SUCCESS == ret) {
			/* TODO: build route table hash*/
			nextHopItem.ifIndex = ifindex;
			nextHopItem.ipAddr 	= ipAddr;
			nextHopItem.tblIndex= nextHopindex;
			pthread_mutex_lock(&nexthopHashMutex);
			ret = npd_route_nexthop_op_item(&nextHopItem,NEXTHOP_ADD_ITEM,dupIfindex);
			pthread_mutex_unlock(&nexthopHashMutex);
			if(ROUTE_RETURN_CODE_SUCCESS != ret) {
				syslog_ax_arpsnooping_err("npd nexthop brief add to db error %d",ret);
				return ARP_RETURN_CODE_ERROR;
			}

			/* TODO:add host route to RT*/
			ret = npd_route_table_op_host_item(&nextHopItem,ROUTE_ADD_ITEM);
			if(ROUTE_RETURN_CODE_SUCCESS!= ret) {
				syslog_ax_arpsnooping_err("npd route host add to HW error %d",ret);
				return ARP_RETURN_CODE_ERROR;
			}
			ret = ARP_RETURN_CODE_SUCCESS;
		}
	}
	else{
		ret = ARP_RETURN_CODE_STATIC_EXIST;
	}
	return ret;
}

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
 *		ARP_RETURN_CODE_SUCCESS  - if success
 *		COMMON_RETURN_CODE_NULL_PTR  - if the item is null
 *		ARP_RETURN_CODE_NAM_ERROR  - nam operation failed
 *		ARP_RETURN_CODE_HASH_OP_FAILED  - hash table operation failed
 *		ARP_RETURN_CODE_STASTIC_NOTEXIST - the static arp does not exist
 *		ARP_RETURN_CODE_PORT_NOTMATCH  - the dev number and port number are not matched
 *		ARP_RETURN_CODE_ERROR			 - if other error occurs,get dev and port failed
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
)
{
	int ret = 0;
	unsigned char devNum = 0,portNum = 0;
	struct arp_snooping_item_s *arpInfo = NULL;
	struct route_nexthop_brief_s nextHopItem;

	memset(&nextHopItem,0,sizeof(struct route_nexthop_brief_s));
    if(ARP_INTF_ADVANCED_VLAN != intfType){
	    ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
        if(0 != ret){
    		return ARP_RETURN_CODE_ERROR;
    	}
    }
    else{
        devNum = 0;
        portNum = 255;
	}
	pthread_mutex_lock(&arpHashMutex);
	arpInfo = npd_arp_snooping_find_item_bymacip(ifindex,mac,ipAddr);
	if((NULL == arpInfo)||(NPD_TRUE != arpInfo->isStatic)) {
		ret = ARP_RETURN_CODE_STASTIC_NOTEXIST;
	}
	else if((arpInfo->dstIntf.intf.port.devNum != devNum)||  \
			(arpInfo->dstIntf.intf.port.portNum != portNum)){
		ret = ARP_RETURN_CODE_PORT_NOTMATCH;/* arp item port not consistent*/
	}
	else { /* all check success*/
		ret = npd_all_arp_delete(arpInfo,NPD_TRUE);
	}
	pthread_mutex_unlock(&arpHashMutex);
	return ret;
}


/**********************************************************************************
 * npd_arp_snooping_remove_static_arp_for_trunk
 *	This routine is used to remove static arp entry.
 *
 *	INPUT:
 *		ifindex 		--	dev index
 *		ipAddr		-- 	ip address
 *		ipmask		--	ip mask length
 *		mac			--	MAC address
 *		vid			--	vlan id
 *		trunkId	 -- the trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ARP_RETURN_CODE_SUCCESS  - if success
 *		COMMON_RETURN_CODE_NULL_PTR  - if the item is null
 *		ARP_RETURN_CODE_NAM_ERROR  - nam operation failed
 *		ARP_RETURN_CODE_HASH_OP_FAILED  - hash table operation failed
 *		ARP_RETURN_CODE_STASTIC_NOTEXIST - the static arp does not exist
 *		ARP_RETURN_CODE_TRUNK_NOTMATCH  - the trunk id is not matched
 *		ARP_RETURN_CODE_ERROR			 - if other error occurs,get dev and port failed
 *		
 *	NOTE:
 *
 **********************************************************************************/
 int npd_arp_snooping_remove_static_arp_for_trunk
(
	unsigned int ifindex,
	unsigned int ipAddr,
	unsigned int ipMask,
	unsigned char* mac,
	unsigned short vid,
	unsigned int trunkId
)
{
	int ret = 0;
	unsigned char devNum = 0;
	struct arp_snooping_item_s *arpInfo = NULL;
	struct route_nexthop_brief_s nextHopItem;

	memset(&nextHopItem,0,sizeof(struct route_nexthop_brief_s));
	
	pthread_mutex_lock(&arpHashMutex);
	arpInfo = npd_arp_snooping_find_item_bymacip(ifindex,mac,ipAddr);
	if((NULL == arpInfo)||(NPD_TRUE != arpInfo->isStatic)) {
		ret = ARP_RETURN_CODE_STASTIC_NOTEXIST;
	}
	else if((arpInfo->dstIntf.intf.trunk.trunkId != trunkId)){
		ret = ARP_RETURN_CODE_TRUNK_NOTMATCH;/* arp item port not consistent*/
	}
	else { /* all check success*/
		ret = npd_all_arp_delete(arpInfo,NPD_TRUE);
	}
	pthread_mutex_unlock(&arpHashMutex);
	return ret;
}


/************************************************************************************
 *		NPD dbus operation
 *
 ************************************************************************************/
DBusMessage * npd_dbus_set_arp_aging_time(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter  iter;
	int ret;
	unsigned int agingTime = 10;	
		

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT32,&agingTime,
		DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	arp_aging_time = agingTime;
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	return reply;

}

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
 *		showStr
 *		currentLen
 *
 * 	RETURN:
 *		
 *	NOTE:
 *
 **********************************************************************************/
 void npd_dbus_save_static_arp
(
	char* showStr,
	int totalLength,
	int* currentLen
)
{
	int len = 0,i = 0,j = 0;
	unsigned int ret = 0;
	struct arp_snooping_item_s** itemArray = NULL;
	struct arp_snooping_item_s* tmpArp = NULL;
	char* tmp = NULL,*buf = NULL;
	unsigned char flag = NPD_TRUE;
	unsigned int staticArpCount = 0;
	
	if(NULL == showStr ) 
	{
		return ;
	}
	buf = showStr + (*currentLen);
	tmp = buf;
	pthread_mutex_lock(&arpHashMutex);
	staticArpCount = npd_count_all_static_arp();
	itemArray = (struct arp_snooping_item_s **)malloc(staticArpCount*sizeof(struct arp_snooping_item_s *));
	memset(itemArray,0,staticArpCount*sizeof(struct arp_snooping_item_s *));
	hash_return(arp_snoop_db_s,&flag,itemArray,staticArpCount,npd_arp_filter_by_static);
    for(j = 0;j<staticArpCount;j++)
	{
		tmpArp = (struct arp_snooping_item_s*)malloc(sizeof(struct arp_snooping_item_s));
		memset(tmpArp,0,sizeof(struct arp_snooping_item_s));
        memcpy(tmpArp,itemArray[j],sizeof(struct arp_snooping_item_s));
		itemArray[j] = tmpArp;
		tmpArp = NULL;
    }
	pthread_mutex_unlock(&arpHashMutex);
	for(i = 0; i < staticArpCount; i++) 
	{
		if(NULL != itemArray[i]) 
		{
			syslog_ax_intf_dbg("matched item[%d] found.\n",i);
			if((*currentLen + len + 65) >totalLength) 
			{
				*currentLen += len;
				for(i= 0; i< staticArpCount; i++)	/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
				{
					free(itemArray[i]);
					itemArray[i] = NULL;
				}
				free(itemArray);
				itemArray = NULL;
				return;
			}
			else 
			{
				unsigned char slot_no = 0,port_no = 0,slot_index = 0,port_index = 0;
				unsigned int eth_g_index = 0;
				if(TRUE != itemArray[i]->dstIntf.isTrunk)
				{
					ret = npd_get_global_index_by_devport(itemArray[i]->dstIntf.intf.port.devNum, itemArray[i]->dstIntf.intf.port.portNum, &eth_g_index);
	                if(NPD_SUCCESS != ret)
					{
	                   syslog_ax_arpsnooping_err("get global index failed from dev and port in arpsnooping!\n");
	                   continue;
					}
					slot_index 		= SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					slot_no			= CHASSIS_SLOT_INDEX2NO(slot_index);
					port_index 		= ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					port_no 	= ETH_LOCAL_INDEX2NO(slot_index,port_index);
					if(TRUE == npd_eth_port_rgmii_type_check(slot_no,port_no))
					{
						len += sprintf(tmp,"ip static-arp %d/%d %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d/32\n",		\
							slot_no,port_no,itemArray[i]->mac[0],itemArray[i]->mac[1],itemArray[i]->mac[2],itemArray[i]->mac[3],itemArray[i]->mac[4],itemArray[i]->mac[5],		\
							(itemArray[i]->ipAddr>>24) & 0xff,(itemArray[i]->ipAddr>>16) & 0xff,(itemArray[i]->ipAddr>>8) & 0xff,itemArray[i]->ipAddr & 0xff);
						tmp = buf + len;
					}
					else
					{
						if((PRODUCT_ID_AX7K != PRODUCT_ID)&&(0 == slot_no)&&(port_no > 0)&&(port_no <= 2)&&(0 != nbm_cvm_query_card_on(port_no - 1)))
						{
							continue;
						}
						len += sprintf(tmp,"ip static-arp %d/%d %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d/32 %d\n",		\
							slot_no,port_no,itemArray[i]->mac[0],itemArray[i]->mac[1],itemArray[i]->mac[2],itemArray[i]->mac[3],itemArray[i]->mac[4],itemArray[i]->mac[5],		\
							(itemArray[i]->ipAddr>>24) & 0xff,(itemArray[i]->ipAddr>>16) & 0xff,(itemArray[i]->ipAddr>>8) & 0xff,itemArray[i]->ipAddr & 0xff,itemArray[i]->vid);
						tmp = buf + len;
					}
				}
				else 
				{
										
					len += sprintf(tmp,"ip static-arp %d %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d/32 %d\n",		\
						itemArray[i]->dstIntf.intf.trunk.trunkId,itemArray[i]->mac[0],itemArray[i]->mac[1],itemArray[i]->mac[2],itemArray[i]->mac[3],itemArray[i]->mac[4],itemArray[i]->mac[5],		\
						(itemArray[i]->ipAddr>>24) & 0xff,(itemArray[i]->ipAddr>>16) & 0xff,(itemArray[i]->ipAddr>>8) & 0xff,itemArray[i]->ipAddr & 0xff,itemArray[i]->vid);
					tmp = buf + len;
				}
			}
			free(itemArray[i]);
			itemArray[i] = NULL;
		}
		else 
			continue;
	}
	free(itemArray);
	itemArray = NULL;
	*currentLen += len;
	return ;
}

 void npd_dbus_save_interrupt_rxmax
(
	char* showStr,
	int totalLength,
	int interrupt_rxmax
)
{
	
	if(NULL == showStr ) {
		return ;
	}

	if(interrupt_rxmax != 0)
		sprintf(showStr,"set interrupt rxmax %d \n",interrupt_rxmax);
	
	return ;
}


/**********************************************************************************
 * npd_dbus_save_arp_cfg
 *	This method is used to save static arp config
 *
 *	INPUT:
 *		showStr
 *		avalidLen	
 *	OUTPUT:
 *		showStr
 *		avalidLen	
 *
 * 	RETURN:
 *		
 *	NOTE:
 *
 **********************************************************************************/
void npd_dbus_save_arp_cfg
(
	char** showStr,
	int* avalidLen
)
{
	int len = 0;
	char * arpCheckStr =		"config arp smac-check enable\n";
	char * arpStrictCheckStr =  "config arp strict-check disable\n";
	char * arpInspectionStr = 	"config arp inspection enable\n";
	char * arpProxyStr = 		"config arp proxy enable\n";
	if((NULL == showStr )||(NULL == *showStr)||(NULL == avalidLen)) {
		return ;
	}
	else {
		if(SYS_ARP_AGING_TIME != arp_aging_time) {
			if(20 > *avalidLen)
				return ;
			len = sprintf((*showStr),"config arp aging %d\n",arp_aging_time);
			*showStr += len;
			*avalidLen -= len;
		}
		if(NPD_TRUE == arp_smac_check) {				
			if((strlen(arpCheckStr)+1) > *avalidLen){
	            return ;
			}
			len = sprintf((*showStr),arpCheckStr);
			*showStr += len;
			*avalidLen -= len;
		}
		if(NPD_FALSE == arp_strict_check) {	/* code optimize: Copy-paste error houxx@autelan.com  2013-1-18 */			
			if((strlen(arpStrictCheckStr)+1) > *avalidLen){
	            return ;
			}
			len = sprintf((*showStr),arpStrictCheckStr);
			*showStr += len;
			*avalidLen -= len;
		}
		if(NPD_TRUE == arp_inspection) {	
			if((strlen(arpInspectionStr)+1) > *avalidLen){
	            return ;
			}
			len = sprintf((*showStr),arpInspectionStr);
			*showStr += len;
			*avalidLen -= len;
		}
		if(NPD_TRUE == arp_proxy) {
			if((strlen(arpProxyStr)+1) > *avalidLen){
	            return ;
			}
			len = sprintf((*showStr),arpProxyStr);
			*showStr += len;
			*avalidLen -= len;
		}
	}
    
	return ;
}
/**********************************************************************************
 * npd_dbus_save_advanced_routing_default_vid_cfg
 *	This method is used to save static arp config
 *
 *	INPUT:
 *		showStr
 *		avalidLen	
 *	OUTPUT:
 *		showStr
 *		avalidLen	
 *
 * 	RETURN:
 *		
 *	NOTE:
 *
 **********************************************************************************/
void npd_dbus_save_advanced_routing_default_vid_cfg
(
	char** showStr,
	int* avalidLen
)
{
	int len = 0;
	char * cfgStr =		"config advanced-routing default-vid";
	if((NULL == showStr )||(NULL == *showStr)||(NULL == avalidLen)) {
		return ;
	}
	else if(npd_product_adv_routing_cmd_support()){
		if(NPD_ADV_ROUTING_DEFAULT_VID != advanced_routing_default_vid) {
			if(strlen(cfgStr)+strlen(" empty\n") > *avalidLen)
				return ;
			if(advanced_routing_default_vid){
				len = sprintf((*showStr),"%s %d\n",cfgStr,advanced_routing_default_vid);
			}
			else{
				len = sprintf((*showStr),"%s empty\n",cfgStr);
			}
			*showStr += len;
			*avalidLen -= len;
		}
	}
    
	return ;
}

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
unsigned int npd_arp_snooping_arp_del_by_ip_and_mask(unsigned int ip,unsigned int mask){
			
	struct arp_snooping_item_s *arpItem = NULL;
	struct arp_snooping_item_s item;
	unsigned int ip_mask[2] = {0};
	unsigned int delCount = 0;
	ip_mask[0] = ip;
	ip_mask[1] = mask;
	memset(&item,0,sizeof(struct arp_snooping_item_s));
	pthread_mutex_lock(&arpHashMutex);
	delCount = hash_traversal(arp_snoop_db_s,NPD_TRUE,ip_mask,npd_arp_filter_by_network,npd_all_arp_delete);
	pthread_mutex_unlock(&arpHashMutex);
	if(delCount > 0){
		syslog_ax_arpsnooping_dbg("arp delete by ip and mask,delete %d items \n",delCount);
	}
	return ARP_RETURN_CODE_SUCCESS;
}
/*********************************************************************************
 * npd_dbus_ip_static_arp
 * DESCRIPTION:
 *      config static arp for intf,but this command not support sub interface or sub sub interface
 * INPUT:
 *
 * OUTPUT:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_BADPARAM
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ARP_RETURN_CODE_NO_SUCH_PORT
 *		ARP_RETURN_CODE_VLAN_NOTEXISTS
 *		ARP_RETURN_CODE_UNSUPPORTED_COMMAND
 *		ARP_RETURN_CODE_PORT_NOT_IN_VLAN
 *		ARP_RETURN_CODE_INTERFACE_NOTEXIST
 *          ARP_RETURN_CODE_NO_HAVE_ANY_IP - l3intf no have any ip address
 *          ARP_RETURN_CODE_HAVE_THE_IP       - l3intf already have the ip address
 *          ARP_RETURN_CODE_NOT_SAME_SUB_NET - l3intf no have the same sub net ip with ipAddress
 *          ARP_RETURN_CODE_CHECK_IP_ERROR - check ip address error or no have l3intf
 *		ARP_RETURN_CODE_MAC_MATCHED_BASE_MAC
 *		ARP_RETURN_CODE_MAC_MATCHED_INTERFACE_MAC
 *		ARP_RETURN_CODE_STATIC_ARP_FULL  - if the static arp items are equal to 1024 or more
 *		ARP_RETURN_CODE_TABLE_FULL  - the hash table is full
 *		ARP_RETURN_CODE_STATIC_EXIST - static arp item already exists
 *          ARP_RETURN_CODE_NO_VID      -   create static arp for not rgmii,but not input vid          
 *		ARP_RETURN_CODE_ERROR  - if other error occurs,get dev port failed or nexthop op failed
 * NOTICE:
 *
 **********************************************************************************/
DBusMessage * npd_dbus_ip_static_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned char slot_no = 0,port_no = 0;
	unsigned int ipno = 0,ipmaskLen = 0;
	unsigned short vlanId = 0;
	unsigned char macAddr[6]= {0};
	unsigned int ifindex = ~0UI,eth_g_index = 0;
	unsigned char isTagged = FALSE;
	unsigned char baseMac[6]={0};
	enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
    enum NPD_ARP_INTF_TYPE intfType = 0;


	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_BYTE,&slot_no,
				 DBUS_TYPE_BYTE,&port_no,
				 DBUS_TYPE_BYTE,&macAddr[0],
				 DBUS_TYPE_BYTE,&macAddr[1],
				 DBUS_TYPE_BYTE,&macAddr[2],
				 DBUS_TYPE_BYTE,&macAddr[3],
				 DBUS_TYPE_BYTE,&macAddr[4],
				 DBUS_TYPE_BYTE,&macAddr[5],
				 DBUS_TYPE_UINT32,&ipno,
				 DBUS_TYPE_UINT32,&ipmaskLen,
				 DBUS_TYPE_UINT32,&vlanId,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_intf_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_intf_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	syslog_ax_intf_dbg("NPD_INTF: ip static-arp %d/%d %02x:%02x:%02x:%02x:%02x:%02x ipmaskLen: %d vid: %d\n",
			                        slot_no,port_no,macAddr[0],macAddr[1],macAddr[2],
			                        macAddr[3],macAddr[4],macAddr[5],ipmaskLen,vlanId);
    syslog_ax_intf_dbg("%-15s:%ld.%ld.%ld.%ld\n"," IP",(ipno>>24)&0xFF,(ipno>>16)&0xFF, \
				(ipno>>8)&0xFF,ipno&0xFF);

	ret = ARP_RETURN_CODE_NO_SUCH_PORT;	
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) {
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
			syslog_ax_eth_port_dbg("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);
			ret = ARP_RETURN_CODE_SUCCESS;
		}
	}		
	if((ARP_RETURN_CODE_NO_SUCH_PORT == ret)&&\
		(PRODUCT_ID != PRODUCT_ID_AX7K)&&\
		0 == slot_no){
		if(0 == (nbm_cvm_query_card_on(port_no-1))){
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
            ret = ARP_RETURN_CODE_SUCCESS;
		}		
	}
#if 1
    if(ARP_RETURN_CODE_SUCCESS == ret){
        if(0 == vlanId){ /*without vlanid,must rgmii port*/
            if(TRUE == npd_eth_port_rgmii_type_check(slot_no,port_no)){
               intfType = ARP_INTF_ADVANCED_ETH_PORT;
               sprintf(ifname,"eth%d-%d",slot_no,port_no);
			    if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifindex)){
					memset(ifname,0,MAX_IFNAME_LEN + 1);
					sprintf(ifname,"e%d-%d",slot_no,port_no);
					if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifindex)){
						ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
					}
				}
				if(ARP_RETURN_CODE_SUCCESS == ret){
                	ret = npd_arpsnooping_create_static_arp_for_intf(ifindex,ifname,eth_g_index,vlanId,0,ipno,ipmaskLen,macAddr,intfType);
				}
            }
            else {
                ret = ARP_RETURN_CODE_NO_VID;
            }
        }
        else{/*with vlanid*/
            if(TRUE == npd_eth_port_rgmii_type_check(slot_no,port_no)){/*subif for rgmii,not supported by this command */
                ret = ARP_RETURN_CODE_PORT_NOT_IN_VLAN;
            }
            else if(TRUE != npd_check_vlan_real_exist(vlanId)){/*not exists */
                ret = ARP_RETURN_CODE_VLAN_NOTEXISTS;
            }
            else if(TRUE != npd_vlan_check_contain_port(vlanId,eth_g_index,&isTagged)){/*not exists */
                ret = ARP_RETURN_CODE_PORT_NOT_IN_VLAN;
            }
            else{
                if(NPD_PORT_L3INTF_VLAN_ID == vlanId){/*port intf vlanid,port must be eth-port interface */
                    if((TRUE == npd_eth_port_interface_check(eth_g_index,&ifindex))&&\
                        (~0UI != ifindex)){
                        intfType = ARP_INTF_SIMPLE_ETH_PORT;
                        sprintf(ifname,"eth%d-%d",slot_no,port_no);
						if(ARP_RETURN_CODE_SUCCESS == ret){
                        ret = npd_arpsnooping_create_static_arp_for_intf(ifindex,ifname,eth_g_index,vlanId,0,ipno,ipmaskLen,macAddr,intfType);
						}
                    }
                    else{                        
                        ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
                    }
                }
                else if(advanced_routing_default_vid == vlanId){/*advanced-routing vlanid, port must eth advanced-routing port*/
                    if(TRUE == npd_check_port_promi_mode(eth_g_index)){
                        intfType = ARP_INTF_ADVANCED_ETH_PORT;
                        sprintf(ifname,"eth%d-%d",slot_no,port_no);
						if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifindex)){
							memset(ifname,0,MAX_IFNAME_LEN + 1);
							sprintf(ifname,"e%d-%d",slot_no,port_no);
							if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifindex)){
								ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
							}
						}
						if(ARP_RETURN_CODE_SUCCESS == ret){
                        	ret = npd_arpsnooping_create_static_arp_for_intf(ifindex,ifname,eth_g_index,vlanId,0,ipno,ipmaskLen,macAddr,intfType);
						}
                    }
                    else{
                        ret = ARP_RETURN_CODE_ERROR;
                    }
                }
                else{
                    ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;/*interface not exist*/                    
                    if((TRUE == npd_vlan_interface_check(vlanId,&ifindex))&&\
                        (~0UI != ifindex)){/* vlan interface */
                        intfType = ARP_INTF_SIMPLE_VLAN;
				        sprintf(ifname,"vlan%d",vlanId);
                        ret = npd_arpsnooping_create_static_arp_for_intf(ifindex,ifname,eth_g_index,vlanId,0,ipno,ipmaskLen,macAddr,intfType);
                    }
                    else if((NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag))&&\
				        (flag == VLAN_PORT_SUBIF_EXIST)){
				        ret = ARP_RETURN_CODE_UNSUPPORTED_COMMAND;
                    }
                }
            }
        }
    }
#else
	if(ARP_RETURN_CODE_SUCCESS == ret){
		if(((TRUE == npd_eth_port_rgmii_type_check(slot_no,port_no))&&\
			(0 != vlanId))){			
			ret = ARP_RETURN_CODE_PORT_NOT_IN_VLAN;
		}
		else if((0 == vlanId)&&\
			((TRUE != npd_eth_port_rgmii_type_check(slot_no,port_no)))){
			ret = ARP_RETURN_CODE_NO_VID;
		}
	}
	if(ARP_RETURN_CODE_SUCCESS == ret) {
		if(TRUE != npd_eth_port_rgmii_type_check(slot_no,port_no)){/**/
			ret = npd_check_vlan_real_exist(vlanId);
			if(ret != TRUE){
				syslog_ax_intf_err("vid %d is bad paramter!\n",vlanId);
				ifindex = ~0UI;
				ret = ARP_RETURN_CODE_VLAN_NOTEXISTS;
			}
			else if(FALSE == npd_vlan_check_contain_port(vlanId,eth_g_index,&isTagged)){
	            syslog_ax_intf_err("the port %d/%d is not in the vlan %d.\n",slot_no,port_no,vlanId);
				ret = ARP_RETURN_CODE_PORT_NOT_IN_VLAN;
			}
			else{
                ret = ARP_RETURN_CODE_SUCCESS;
			}
		}
		if(ARP_RETURN_CODE_SUCCESS == ret){
			if(((TRUE == npd_eth_port_rgmii_type_check(slot_no,port_no))||\
				(TRUE == npd_check_port_promi_mode(eth_g_index)))&&\
				((0 == vlanId)||(advanced_routing_default_vid == vlanId))){/**/
	            syslog_ax_intf_err("port advanced-routing interface checked when config static arp\n");
				sprintf(ifname,"eth%d-%d",slot_no,port_no);
				if(ARP_RETURN_CODE_NO_HAVE_THE_IP !=(ret = npd_arp_snooping_static_arp_check_ip_address_by_ifname(ipno,ifname,&ifindex))){
		            if(ARP_RETURN_CODE_HAVE_THE_IP == ret){
		                syslog_ax_intf_err("ip is the same as system!\n");
					}else if(ARP_RETURN_CODE_CHECK_IP_ERROR == ret){
		                syslog_ax_intf_err("ip check error!\n");
					}else if(ARP_RETURN_CODE_NOT_SAME_SUB_NET == ret){
					    syslog_ax_intf_err("ip not in the same subnet!\n");
					}else if(ARP_RETURN_CODE_NO_HAVE_ANY_IP == ret){
		 			    syslog_ax_intf_err("interface ip not set!\n");
					}else if(ARP_RETURN_CODE_CHECK_IP_ERROR == ret){
		                syslog_ax_intf_err("check interface ip failed!\n");
					}
				}
				else{
					ret = npd_system_get_basemac(baseMac,MAC_ADDRESS_LEN);
					if((0 == ret)&&(0 == memcmp(macAddr,baseMac,MAC_ADDRESS_LEN))){
						syslog_ax_intf_err("try to add static-arp with system mac address FAILED!\n");
		                ret = ARP_RETURN_CODE_MAC_MATCHED_BASE_MAC;
					}else{
					    if(ret != 0){
						    syslog_ax_intf_dbg("get the system mac address FAILED!\n");
					    }
						ret = npd_arp_snooping_create_static_arp(ifindex,ipno,ipmaskLen,macAddr,vlanId,eth_g_index);
					}
			    }
			}
			else if(TRUE != npd_intf_port_check(vlanId,eth_g_index,&ifindex)) {
				syslog_ax_intf_err("layer 3 interface not exist!\n");
				ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
			}
			else if(NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag)&&\
				(flag == VLAN_PORT_SUBIF_EXIST)){
				syslog_ax_intf_err("vlan advanced-routing interface not support static arp\n");
				ret = ARP_RETURN_CODE_UNSUPPORTED_COMMAND;
			}
			else if(ARP_RETURN_CODE_NO_HAVE_THE_IP !=(ret = npd_arp_snooping_check_ip_address(ipno,vlanId,eth_g_index))){
	            if(ARP_RETURN_CODE_HAVE_THE_IP == ret){
	                syslog_ax_intf_err("ip is the same as system!\n");
				}else if(ARP_RETURN_CODE_CHECK_IP_ERROR == ret){
	                syslog_ax_intf_err("ip check error!\n");
				}else if(ARP_RETURN_CODE_NOT_SAME_SUB_NET == ret){
				    syslog_ax_intf_err("ip not in the same subnet!\n");
				}else if(ARP_RETURN_CODE_NO_HAVE_ANY_IP == ret){
	 			    syslog_ax_intf_err("interface ip not set!\n");
				}else if(ARP_RETURN_CODE_CHECK_IP_ERROR == ret){
	                syslog_ax_intf_err("check interface ip failed!\n");
				}
			}
			else{
				ret = npd_system_get_basemac(baseMac,MAC_ADDRESS_LEN);
				if((0 == ret)&&(0 == memcmp(macAddr,baseMac,MAC_ADDRESS_LEN))){
					syslog_ax_intf_err("try to add static-arp with system mac address FAILED!\n");
	                ret = ARP_RETURN_CODE_MAC_MATCHED_BASE_MAC;
				}else{
					if(ret != 0){
						syslog_ax_intf_dbg("get the system mac address FAILED!\n");
					}
					if((INTERFACE_RETURN_CODE_SUCCESS == (ret = npd_intf_get_l3_intf_mac(ifindex,vlanId,eth_g_index,baseMac)))&&\
						(0 == memcmp(macAddr,baseMac,MAC_ADDRESS_LEN))){	
						syslog_ax_intf_err("try to add static-arp with interface mac address FAILED!\n");
		                ret = ARP_RETURN_CODE_MAC_MATCHED_INTERFACE_MAC;
					}else{
						if(INTERFACE_RETURN_CODE_SUCCESS != ret){
			                syslog_ax_intf_dbg("get the interface mac address FAILED!\n");
						}
				    	ret = npd_arp_snooping_create_static_arp(ifindex,ipno,ipmaskLen,macAddr,vlanId,eth_g_index);
					}
				}
		    }
		}
	}
#endif
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);									 
	return reply;								 

}
/**/

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
)
{/* eth interface or vlan interface*/
    unsigned int ret = 0;    
	unsigned char baseMac[6]={0};
    
    if((intfType >= ARP_INTF_ADVANCED_VLAN)&&(!ifname)){
        return ARP_RETURN_CODE_ERROR;
    }
    if(ARP_RETURN_CODE_NO_HAVE_THE_IP !=(ret = ((intfType >= ARP_INTF_ADVANCED_VLAN) ? \
        npd_arp_snooping_static_arp_check_ip_address_by_ifname(ipno,ifname,&ifindex) : \
        npd_arp_snooping_check_ip_address(ipno,vlanId,eth_g_index)))){
        if(ARP_RETURN_CODE_HAVE_THE_IP == ret){
            syslog_ax_intf_err("ip is the same as system!\n");
		}else if(ARP_RETURN_CODE_CHECK_IP_ERROR == ret){
            syslog_ax_intf_err("ip check error!\n");
		}else if(ARP_RETURN_CODE_NOT_SAME_SUB_NET == ret){
		    syslog_ax_intf_err("ip not in the same subnet!\n");
		}else if(ARP_RETURN_CODE_NO_HAVE_ANY_IP == ret){
			    syslog_ax_intf_err("interface ip not set!\n");
		}else if(ARP_RETURN_CODE_CHECK_IP_ERROR == ret){
            syslog_ax_intf_err("check interface ip failed!\n");
		}
	}
	else{
		ret = npd_system_get_basemac(baseMac,MAC_ADDRESS_LEN);
		if((0 == ret)&&(0 == memcmp(macAddr,baseMac,MAC_ADDRESS_LEN))){
			syslog_ax_intf_err("try to add static-arp with system mac address FAILED!\n");
            ret = ARP_RETURN_CODE_MAC_MATCHED_BASE_MAC;
		}else{
			if(ret != 0){
				syslog_ax_intf_dbg("get the system mac address FAILED!\n");
			}
			if((intfType <= ARP_INTF_SIMPLE_ETH_PORT)&&(INTERFACE_RETURN_CODE_SUCCESS == (ret = npd_intf_get_l3_intf_mac(ifindex,vlanId,eth_g_index,baseMac)))&&\
				(0 == memcmp(macAddr,baseMac,MAC_ADDRESS_LEN))){	
				syslog_ax_intf_err("try to add static-arp with interface mac address FAILED!\n");
                ret = ARP_RETURN_CODE_MAC_MATCHED_INTERFACE_MAC;
			}else{
				if((intfType <= ARP_INTF_SIMPLE_ETH_PORT)&&(INTERFACE_RETURN_CODE_SUCCESS != ret)){
	                syslog_ax_intf_dbg("get the interface mac address FAILED!\n");
				}
		    	ret = npd_arp_snooping_create_static_arp(ifindex,ipno,ipmaskLen,macAddr,vlanId,vlanId2,eth_g_index,intfType);
			}
		}
    }
    return ret;    
}


/*********************************************************************************
 *
 * OUTPUT:
 *		ARP_RETURN_CODE_SUCCESS - if no error occurred
 *		COMMON_RETURN_CODE_NULL_PTR - if input parameters have null pointer
 *		COMMON_RETURN_CODE_NO_RESOURCE - if no memory allocatable
 *		ARP_RETURN_CODE_TRUNK_NOT_EXISTS
 *		ARP_RETURN_CODE_VLAN_NOTEXISTS
 *		ARP_RETURN_CODE_TRUNK_NOT_IN_VLAN
 *		ARP_RETURN_CODE_UNSUPPORTED_COMMAND
 *		ARP_RETURN_CODE_INTERFACE_NOTEXIST
 *          ARP_RETURN_CODE_NO_HAVE_ANY_IP - l3intf no have any ip address
 *          ARP_RETURN_CODE_HAVE_THE_IP       - l3intf already have the ip address
 *          ARP_RETURN_CODE_NOT_SAME_SUB_NET - l3intf no have the same sub net ip with ipAddress
 *          ARP_RETURN_CODE_CHECK_IP_ERROR - check ip address error or no have l3intf
 *		ARP_RETURN_CODE_MAC_MATCHED_BASE_MAC
 *		ARP_RETURN_CODE_MAC_MATCHED_INTERFACE_MAC
 *		ARP_RETURN_CODE_STATIC_ARP_FULL  - if the static arp items are equal to 1024 or more
 *		ARP_RETURN_CODE_TABLE_FULL  - the hash table is full
 *		ARP_RETURN_CODE_STATIC_EXIST - static arp item already exists
 *		ARP_RETURN_CODE_ERROR  - if other error occurs,get dev port failed or nexthop op failedf
 *
 **********************************************************************************/

DBusMessage * npd_dbus_ip_static_arp_for_trunk(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned int trunkId = 0;
	unsigned int ipno = 0,ipmaskLen = 0;
	unsigned short vlanId = 0;
	unsigned char macAddr[6]= {0};
	unsigned int ifindex = ~0UI;
	unsigned char isTagged = FALSE;
	unsigned char baseMac[6]={0};
	enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;
	unsigned char ifname[21] = {0};


	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_UINT32,&trunkId,
				 DBUS_TYPE_BYTE,&macAddr[0],
				 DBUS_TYPE_BYTE,&macAddr[1],
				 DBUS_TYPE_BYTE,&macAddr[2],
				 DBUS_TYPE_BYTE,&macAddr[3],
				 DBUS_TYPE_BYTE,&macAddr[4],
				 DBUS_TYPE_BYTE,&macAddr[5],
				 DBUS_TYPE_UINT32,&ipno,
				 DBUS_TYPE_UINT32,&ipmaskLen,
				 DBUS_TYPE_UINT32,&vlanId,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	syslog_ax_arpsnooping_dbg("NPD_INTF: ip static-arp trunk %d %02x:%02x:%02x:%02x:%02x:%02x ipmaskLen: %d vid: %d\n",
			                        trunkId,macAddr[0],macAddr[1],macAddr[2],
			                        macAddr[3],macAddr[4],macAddr[5],ipmaskLen,vlanId);
    syslog_ax_arpsnooping_dbg("%-15s:%ld.%ld.%ld.%ld\n"," IP",(ipno>>24)&0xFF,(ipno>>16)&0xFF, \
				(ipno>>8)&0xFF,ipno&0xFF);

	/*ret = npd_arp_snooping_create_kern_static_arp(ipno,&macAddr[0]);*/
	sprintf(ifname,"vlan%d",vlanId);
	ret = ARP_RETURN_CODE_TRUNK_NOT_EXISTS;	
	if (NPD_TRUNK_EXISTS == npd_check_trunk_exist((unsigned short)trunkId)) {
		ret = ARP_RETURN_CODE_SUCCESS;
	}
	else {
		ret = ARP_RETURN_CODE_TRUNK_NOT_EXISTS;
	}

	if(ARP_RETURN_CODE_SUCCESS == ret) {
		ret = npd_check_vlan_real_exist(vlanId);
		if(ret != TRUE){
			syslog_ax_arpsnooping_err("vid %d is bad paramter!\n",vlanId);
			ifindex = ~0UI;
			ret = ARP_RETURN_CODE_VLAN_NOTEXISTS;
		}
		else if(FALSE == npd_vlan_check_contain_trunk(vlanId,trunkId,&isTagged)) {
            syslog_ax_arpsnooping_err("the trunk %d is not in the vlan %d.\n",trunkId,vlanId);
			ret = ARP_RETURN_CODE_TRUNK_NOT_IN_VLAN;
		}
		else if(NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag)&&\
			(flag == VLAN_PORT_SUBIF_EXIST)){
			syslog_ax_arpsnooping_err("vlan advanced-routing interface not support static arp\n");
			ret = ARP_RETURN_CODE_UNSUPPORTED_COMMAND;
		}
		else if(!((NPD_TRUE == npd_vlan_interface_check(vlanId,&ifindex))&&(~0UI != ifindex))){
            syslog_ax_arpsnooping_err("vlan interface not exists when create static arp on trunk %d\n",trunkId);
			ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
		}
		else if(ARP_RETURN_CODE_NO_HAVE_THE_IP !=(ret = npd_arp_snooping_static_arp_check_ip_address_by_ifname(ipno,ifname,&ifindex))){
            if(ARP_RETURN_CODE_HAVE_THE_IP == ret){
                syslog_ax_arpsnooping_err("ip is the same as system!\n");
			}else if(ARP_RETURN_CODE_CHECK_IP_ERROR == ret){
                syslog_ax_arpsnooping_err("ip check error!\n");
			}else if(ARP_RETURN_CODE_NOT_SAME_SUB_NET == ret){
			    syslog_ax_arpsnooping_err("ip not in the same subnet!\n");
			}else if(ARP_RETURN_CODE_NO_HAVE_ANY_IP == ret){
 			    syslog_ax_arpsnooping_err("interface ip not set!\n");
			}else if(ARP_RETURN_CODE_CHECK_IP_ERROR == ret){
                syslog_ax_arpsnooping_err("check interface ip failed!\n");
			}
		}
		else{
			ret = npd_system_get_basemac(baseMac,MAC_ADDRESS_LEN);
			if((0 == ret)&&(0 == memcmp(macAddr,baseMac,MAC_ADDRESS_LEN))){
				syslog_ax_arpsnooping_err("try to add static-arp with system mac address FAILED!\n");
                ret = ARP_RETURN_CODE_MAC_MATCHED_BASE_MAC;
			}else{
				if(ret != 0){
					syslog_ax_arpsnooping_err("get the system mac address FAILED!\n");
				}
				if((INTERFACE_RETURN_CODE_SUCCESS == (ret = npd_intf_get_l3_vlan_mac(vlanId,ifindex,baseMac)))&&\
					(0 == memcmp(macAddr,baseMac,MAC_ADDRESS_LEN))){	
					syslog_ax_intf_err("try to add static-arp with interface mac address FAILED!\n");
	                ret = ARP_RETURN_CODE_MAC_MATCHED_INTERFACE_MAC;
				}else{
					if(INTERFACE_RETURN_CODE_SUCCESS != ret){
		                syslog_ax_arpsnooping_err("get the interface mac address FAILED!\n");
					}
			    	ret = npd_arp_snooping_create_static_arp_for_trunk(ifindex,ipno,ipmaskLen,macAddr,vlanId,trunkId);
				}
			}
	    }
	}
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);									 
	return reply;								 

}
/********************************************************************
 *
 * OUTPUT:
 *		ARP_RETURN_CODE_SUCCESS  - if success
 *		COMMON_RETURN_CODE_NULL_PTR  - if the item is null
 *		ARP_RETURN_CODE_NO_SUCH_PORT
 *		ARP_RETURN_CODE_VLAN_NOTEXISTS
 *		ARP_RETURN_CODE_PORT_NOT_IN_VLAN
 *		ARP_RETURN_CODE_INTERFACE_NOTEXIST
 *		ARP_RETURN_CODE_UNSUPPORTED_COMMAND
 *		ARP_RETURN_CODE_NAM_ERROR  - nam operation failed
 *		ARP_RETURN_CODE_HASH_OP_FAILED  - hash table operation failed
 *		ARP_RETURN_CODE_STASTIC_NOTEXIST - the static arp does not exist
 *		ARP_RETURN_CODE_PORT_NOTMATCH  - the dev number and port number are not matched
 *		ARP_RETURN_CODE_ERROR			 - if other error occurs,get dev and port failed
 *
 ********************************************************************/
DBusMessage * npd_dbus_no_ip_static_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned char slot_no = 0,port_no = 0;
	unsigned long ipno = 0,ipmaskLen = 0;
	unsigned short vlanId = 0;
	unsigned char macAddr[6]= {0};
	unsigned int ifindex = ~0UI,eth_g_index = 0;
	unsigned char isTagged = FALSE;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
	struct arp_snooping_item_s item;
	enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;
    enum NPD_ARP_INTF_TYPE intfType = 0;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_BYTE,&slot_no,
				 DBUS_TYPE_BYTE,&port_no,
				 DBUS_TYPE_BYTE,&macAddr[0],
				 DBUS_TYPE_BYTE,&macAddr[1],
				 DBUS_TYPE_BYTE,&macAddr[2],
				 DBUS_TYPE_BYTE,&macAddr[3],
				 DBUS_TYPE_BYTE,&macAddr[4],
				 DBUS_TYPE_BYTE,&macAddr[5],
				 DBUS_TYPE_UINT32,&ipno,
				 DBUS_TYPE_UINT32,&ipmaskLen,
				 DBUS_TYPE_UINT32,&vlanId,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_intf_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_intf_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	/*ret = npd_arp_snooping_create_kern_static_arp(ipno,&macAddr[0]);*/
	ret = ARP_RETURN_CODE_NO_SUCH_PORT;	
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) {
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
			syslog_ax_intf_err("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);
			ret = ARP_RETURN_CODE_SUCCESS;
		}
	}
	if((ARP_RETURN_CODE_NO_SUCH_PORT == ret)&&\
		(PRODUCT_ID != PRODUCT_ID_AX7K)&&\
		0 == slot_no){		 
		if(0 == (nbm_cvm_query_card_on(port_no-1))){
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
            ret = ARP_RETURN_CODE_SUCCESS;
		}		
	}
#if 1
  if(ARP_RETURN_CODE_SUCCESS == ret){
     if(0 == vlanId){ /*without vlanid,must rgmii port*/
         if(TRUE == npd_eth_port_rgmii_type_check(slot_no,port_no)){
            intfType = ARP_INTF_ADVANCED_ETH_PORT;
            sprintf(ifname,"eth%d-%d",slot_no,port_no);
            if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifindex)){
				memset(ifname,0,MAX_IFNAME_LEN + 1);
				sprintf(ifname,"e%d-%d",slot_no,port_no);
				if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifindex)){
					ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
            }
			}
            if(ARP_RETURN_CODE_SUCCESS == ret){
                ret = npd_arp_snooping_remove_static_arp(ifindex,ipno,ipmaskLen,macAddr,vlanId,eth_g_index,intfType);
            }
         }
         else {
             ret = ARP_RETURN_CODE_NO_VID;
         }
     }
     else{/*with vlanid*/
         if(TRUE == npd_eth_port_rgmii_type_check(slot_no,port_no)){/*subif for rgmii,not supported by this command */
             ret = ARP_RETURN_CODE_UNSUPPORTED_COMMAND;
         }
         else if(TRUE != npd_check_vlan_real_exist(vlanId)){/*not exists */
             ret = ARP_RETURN_CODE_VLAN_NOTEXISTS;
         }
         else if(TRUE != npd_vlan_check_contain_port(vlanId,eth_g_index,&isTagged)){/*not exists */
             ret = ARP_RETURN_CODE_PORT_NOT_IN_VLAN;
         }
         else{
             if(NPD_PORT_L3INTF_VLAN_ID == vlanId){/*port intf vlanid,port must be eth-port interface */
                 if((TRUE == npd_eth_port_interface_check(eth_g_index,&ifindex))&&\
                     (~0UI != ifindex)){
                     intfType = ARP_INTF_SIMPLE_ETH_PORT;
                     ret = npd_arp_snooping_remove_static_arp(ifindex,ipno,ipmaskLen,macAddr,vlanId,eth_g_index,intfType);
                 }
                 else{                        
                     ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
                 }
             }
             else if(advanced_routing_default_vid == vlanId){/*advanced-routing vlanid, port must eth advanced-routing port*/
                 if(TRUE == npd_check_port_promi_mode(eth_g_index)){
                     intfType = ARP_INTF_ADVANCED_ETH_PORT;
                     sprintf(ifname,"eth%d-%d",slot_no,port_no);
                     if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifindex)){
							memset(ifname,0,MAX_IFNAME_LEN + 1);
							sprintf(ifname,"e%d-%d",slot_no,port_no);
							if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifindex)){
								ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
                     }
						}
                     	if(ARP_RETURN_CODE_SUCCESS == ret){
                        ret = npd_arp_snooping_remove_static_arp(ifindex,ipno,ipmaskLen,macAddr,vlanId,eth_g_index,intfType);
                     }
                 }
                 else{
                     ret = ARP_RETURN_CODE_ERROR;
                 }
             }
             else{
                 ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;/*interface not exist*/                    
                 if((TRUE == npd_vlan_interface_check(vlanId,&ifindex))&&\
                     (~0UI != ifindex)){/* vlan interface */
                     intfType = ARP_INTF_SIMPLE_VLAN;
                     ret = npd_arp_snooping_remove_static_arp(ifindex,ipno,ipmaskLen,macAddr,vlanId,eth_g_index,intfType);
                 }
                 else if((ARP_RETURN_CODE_INTERFACE_NOTEXIST == ret)&&\
                     (NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag))&&\
                     (flag == VLAN_PORT_SUBIF_EXIST)){
                     ret = ARP_RETURN_CODE_UNSUPPORTED_COMMAND;
                 }
             }
         }
     }
 }

#else
	if(ARP_RETURN_CODE_SUCCESS == ret) {
		if(TRUE != npd_eth_port_rgmii_type_check(slot_no,port_no)){
			ret = npd_check_vlan_real_exist(vlanId);
			if(ret != TRUE)
			{
				syslog_ax_intf_err("vid %d is bad param\n",vlanId);
				ifindex = ~0UI;
				ret = ARP_RETURN_CODE_VLAN_NOTEXISTS;
			}
			else if(FALSE == npd_vlan_check_contain_port(vlanId,eth_g_index,&isTagged)) {
	            syslog_ax_intf_err("the vlan does not contain the port\n");
				ret = ARP_RETURN_CODE_PORT_NOT_IN_VLAN;
			}
			else if(NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag)&&\
				(flag == VLAN_PORT_SUBIF_EXIST)){
				syslog_ax_intf_err("vlan advanced-routing interface not support static arp\n");
				ret = ARP_RETURN_CODE_UNSUPPORTED_COMMAND;
			}
			else if((TRUE != npd_intf_port_check(vlanId,eth_g_index,&ifindex))&&\
				(TRUE != npd_check_port_promi_mode(eth_g_index))) {
				syslog_ax_intf_err("not devexist \n");
				ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
			}
			else{
				ret = ARP_RETURN_CODE_SUCCESS;
			}
		}
		if(ARP_RETURN_CODE_SUCCESS == ret){
		    if((TRUE == npd_eth_port_rgmii_type_check(slot_no,port_no))||\
				(TRUE == npd_check_port_promi_mode(eth_g_index))){
				sprintf(ifname,"eth%d-%d",slot_no,port_no);
			    ret = npd_intf_ifindex_get_by_ifname(ifname,&ifindex);
				if(ARP_RETURN_CODE_SUCCESS != ret ){
					syslog_ax_intf_err("get ifindex by ifname FAILED,ifname %s\n",ifname);
			        ret = ARP_RETURN_CODE_ERROR;
				}
			}
			if(ARP_RETURN_CODE_SUCCESS == ret){
				ret = npd_arp_snooping_remove_static_arp(ifindex,ipno,ipmaskLen,macAddr,vlanId,eth_g_index);
			}
		}
	}
#endif
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);			
									 
	return reply;								 

}

/*********************************************************************************
 *
 * OUTPUT:
 *		ARP_RETURN_CODE_SUCCESS  - if success
 *		COMMON_RETURN_CODE_NULL_PTR  - if the item is null
 *		ARP_RETURN_CODE_TRUNK_NOT_EXISTS
 *		ARP_RETURN_CODE_VLAN_NOTEXISTS
 *		ARP_RETURN_CODE_TRUNK_NOT_IN_VLAN
 *		ARP_RETURN_CODE_UNSUPPORTED_COMMAND
 *		ARP_RETURN_CODE_INTERFACE_NOTEXIST
 *		ARP_RETURN_CODE_NAM_ERROR  - nam operation failed
 *		ARP_RETURN_CODE_HASH_OP_FAILED  - hash table operation failed
 *		ARP_RETURN_CODE_STASTIC_NOTEXIST - the static arp does not exist
 *		ARP_RETURN_CODE_TRUNK_NOTMATCH  - the trunk id is not matched
 *		ARP_RETURN_CODE_ERROR			 - if other error occurs,get dev and port failed
 *
 *
 **********************************************************************************/

DBusMessage * npd_dbus_no_ip_static_arp_for_trunk(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned int trunkId;
	unsigned long ipno = 0,ipmaskLen = 0;
	unsigned short vlanId = 0;
	unsigned char macAddr[6]= {0};
	unsigned int ifindex = ~0UI,eth_g_index = 0;
	unsigned char isTagged = FALSE;
	unsigned char ifname[MAX_IFNAME_LEN + 1] = {0};
	enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_UINT32,&trunkId,
				 DBUS_TYPE_BYTE,&macAddr[0],
				 DBUS_TYPE_BYTE,&macAddr[1],
				 DBUS_TYPE_BYTE,&macAddr[2],
				 DBUS_TYPE_BYTE,&macAddr[3],
				 DBUS_TYPE_BYTE,&macAddr[4],
				 DBUS_TYPE_BYTE,&macAddr[5],
				 DBUS_TYPE_UINT32,&ipno,
				 DBUS_TYPE_UINT32,&ipmaskLen,
				 DBUS_TYPE_UINT32,&vlanId,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_intf_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_intf_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	sprintf(ifname,"vlan%d",vlanId);
	ret = ARP_RETURN_CODE_TRUNK_NOT_EXISTS;	
	if (NPD_TRUNK_EXISTS == npd_check_trunk_exist((unsigned short)trunkId)) {
		ret = ARP_RETURN_CODE_SUCCESS;
	}
	else {
		ret = ARP_RETURN_CODE_TRUNK_NOT_EXISTS;
	}

	if(ARP_RETURN_CODE_SUCCESS == ret) {
		ret = npd_check_vlan_real_exist(vlanId);
		if(ret != TRUE){
			syslog_ax_intf_err("vid %d is bad paramter!\n",vlanId);
			ifindex = ~0UI;
			ret = ARP_RETURN_CODE_VLAN_NOTEXISTS;
		}
		else if(FALSE == npd_vlan_check_contain_trunk(vlanId,trunkId,&isTagged)) {
            syslog_ax_intf_err("the trunk %d is not in the vlan %d.\n",trunkId,vlanId);
			ret = ARP_RETURN_CODE_TRUNK_NOT_IN_VLAN;
		}
		else if(NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag)&&\
			(flag == VLAN_PORT_SUBIF_EXIST)){
			syslog_ax_intf_err("vlan advanced-routing interface not support static arp\n");
			ret = ARP_RETURN_CODE_UNSUPPORTED_COMMAND;
		}
		else if(!((TRUE == npd_vlan_interface_check(vlanId,&ifindex))&&(~0UI != ifindex))){
            syslog_ax_intf_err("vlan interface not exists when delete static arp on trunk %d\n",trunkId);
			ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
		}
		else{
			ret = npd_arp_snooping_remove_static_arp_for_trunk(ifindex,ipno,ipmaskLen,macAddr,vlanId,trunkId);
		}
	}
	

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);			
									 
	return reply;								 

}

int host_board_info_get(void)
{
	FILE *fd;
	int ret = 0;

	fd = fopen("/dbm/local_board/is_master", "r");
	if (fd == NULL)
	{
		return -1;
	}
	ret = fscanf(fd, "%d", &is_master);
	fclose(fd);
	if(1 != ret)		/* code optimize: unchk return vlaue houxx@autelan.com  2013-1-18 */
	{
		return -1;
	}

	fd = fopen("/dbm/local_board/slot_id", "r");
	if (fd == NULL) {
		return -1;
	}
	ret = fscanf(fd,"%d", &slot_num);
	fclose(fd);
	if(1 != ret)		/* code optimize: unchk return vlaue houxx@autelan.com  2013-1-18 */
	{
		return -1;
	}

	fd = fopen("/dbm/product/product_serial","r");
	if (fd == NULL) {
		return -1;
	}
	ret = fscanf(fd, "%d", &product_info);
	fclose(fd);
	if(1 != ret)		/* code optimize: unchk return vlaue houxx@autelan.com  2013-1-18 */
	{
		return -1;
	}
	return 0;
}

/*wangchao add*/
#if 1
int npd_arpsnooping_check_mac_and_ipaddr_of_intf
(
    unsigned char * ifname,
    unsigned int ipno,
    unsigned int ipmaskLen,
    unsigned char * macAddr
)
{
    unsigned int ret = 0;    
	unsigned char baseMac[7]={0};
	unsigned int ifindex;
    

    if(ARP_RETURN_CODE_NO_HAVE_THE_IP !=(ret = npd_arp_snooping_static_arp_check_ip_address_by_ifname(ipno,ifname,&ifindex))){

		if(ARP_RETURN_CODE_HAVE_THE_IP == ret){
            syslog_ax_intf_err("ip is the same as system!\n");
		}else if(ARP_RETURN_CODE_CHECK_IP_ERROR == ret){
            syslog_ax_intf_err("ip check error!\n");
		}else if(ARP_RETURN_CODE_NOT_SAME_SUB_NET == ret){
		    syslog_ax_intf_err("ip not in the same subnet!\n");
		}else if(ARP_RETURN_CODE_NO_HAVE_ANY_IP == ret){
			    syslog_ax_intf_err("interface ip not set!\n");
		}else if(ARP_RETURN_CODE_CHECK_IP_ERROR == ret){
            syslog_ax_intf_err("check interface ip failed!\n");
		}
	}
	else{
		ret = npd_system_get_basemac(baseMac,MAC_ADDRESS_LEN);
		   /*syslog_ax_intf_dbg("baseMac == %s \n", baseMac);*/
		if((0 == ret)&&(0 == memcmp(macAddr,baseMac,MAC_ADDRESS_LEN))){
			syslog_ax_intf_err("try to add static-arp with system mac address FAILED!\n");
            ret = ARP_RETURN_CODE_MAC_MATCHED_BASE_MAC;
		}else{
			if(ret != 0){
				syslog_ax_intf_dbg("get the system mac address FAILED!\n");
			}
		}
    }
    return ret;    
}
#endif
#if 0
/***************
0: is not rpa interface
1:is rpa interface
 ***************/
	
/*********************hanhui upgrade***************
#define SIOCGIFPRIVFLAGS 0x89b0
***************************************************/
#define SIOCGIFPRIVFLAGS 0x89b1
/**************************end*********************/
#define IFF_RPA 0x20

unsigned int is_rpa_interface(const char *ifname)
{
	int sock = -1;
	/*int board_slot = -1;*/
	/*int slot_id = -1;*/
	struct ifreq tmp;

	if (NULL == ifname) {
		return -1;
	}

	/* ve
	if (0 == strncmp(ifname, "ve", 2)) {
		board_slot = dcli_dhcp_get_board_slot_id();
		slot_id = get_slot_id_by_ifname(ifname);

		return (board_slot == slot_id);
	} */
	
	memset(&tmp, 0, sizeof(tmp));
	strcpy(tmp.ifr_name, ifname);

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		return -1;
	}

	if (ioctl(sock, SIOCGIFPRIVFLAGS , &tmp) < 0) {
		close(sock);
		return -1;
	}
	close(sock);
	if (IFF_RPA == tmp.ifr_flags) 
		syslog_ax_arpsnooping_err("the %s is rpa interface\n");
	
	/*printf("%s\n", (IFF_RPA == tmp.ifr_flags) ? "rpa interface" : " ");*/
	
	if (IFF_RPA == tmp.ifr_flags) {
		syslog_ax_arpsnooping_err("the %s is rpa interface\n", ifname);
		return 1;
	} else {
		syslog_ax_arpsnooping_err("the %s not rpa interface\n", ifname);
		return 0;
	}
	/*turn ((tmp.ifr_flags & IFF_RPA) != IFF_RPA);*/
}

/****************/

#endif
DBusMessage * npd_dbus_set_stale_time_for_interface(DBusConnection *conn, DBusMessage *msg, void *user_dat)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	
	#define IS_ETH_PORT_INTERFACE	0
	#define IS_VLAN_INTERFACE 		1
    #define IS_VLAN_ADVANCED 		2
    #define IS_MNG_INTERFACE 		3
    #define IS_VE_INTERFACE 		4
    #define IS_EBR_INTERFACE 		5
    #define IS_EBRL_INTERFACE 		6
    #define IS_WLAN_INTERFACE 		7
    #define IS_WLANL_INTERFACE 		8
	#define IS_ETH_SUBINTERFACE		9

	int stale_time;
	unsigned char slot_no = 0,port_no = 0;
	unsigned char cpu_no = 0, cpu_port_no = 0;
	unsigned int tag2 = 0;
	char *cpu_name[2]={"f","s"};
	
	unsigned short vlanId;
    unsigned int isVlanIntf = 0;

	char save_cwd[1024];
    char buf[1024];
	char cmdstr[512];
	int ret = ARP_RETURN_CODE_SUCCESS;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_UINT32,&isVlanIntf,
		         DBUS_TYPE_BYTE,&slot_no,
		         DBUS_TYPE_BYTE,&port_no,
				 DBUS_TYPE_UINT16,&vlanId,
				 DBUS_TYPE_UINT32,&stale_time,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
			return NULL;
	}
	syslog_ax_arpsnooping_dbg("isvlanintf =%d \n slot_no = %d\n, port_no = %d\n vlanid = %d\n stale_time = %d\n",isVlanIntf,slot_no,port_no,vlanId,stale_time);

	if (isVlanIntf == IS_ETH_PORT_INTERFACE) {
		
        if (getcwd(save_cwd, sizeof(save_cwd)) == NULL){
              syslog_ax_arpsnooping_err("getcwd error");
			  ret = ARP_RETURN_CODE_SET_STALE_TIME_ERR;
        }
		syslog_ax_arpsnooping_dbg("current director %s\n",save_cwd);
		sprintf(cmdstr,"/proc/sys/net/ipv4/neigh/eth%d-%d/", slot_no, port_no);
		if (chdir(cmdstr) < 0) {
                syslog_ax_arpsnooping_err("the eth interface don't exist");
				ret = ARP_RETURN_CODE_NO_SUCH_PORT;

		}
		
		sprintf(cmdstr, "echo %d > base_reachable_time", stale_time);
		system(cmdstr);
		syslog_ax_arpsnooping_dbg("current director %s\n",cmdstr);
		
        if (chdir(save_cwd) < 0){
                syslog_ax_arpsnooping_err("getcwd error");
				ret = ARP_RETURN_CODE_SET_STALE_TIME_ERR;
        }		

	} else if (isVlanIntf == IS_VLAN_INTERFACE) {
	
		if (getcwd(save_cwd, sizeof(save_cwd)) == NULL){
			  syslog_ax_arpsnooping_err("getcwd error");
			  ret = ARP_RETURN_CODE_SET_STALE_TIME_ERR;
		}
		sprintf(cmdstr,"/proc/sys/net/ipv4/neigh/vlan%d", vlanId);
		if (chdir(cmdstr) < 0){
				syslog_ax_arpsnooping_err("the vlan interafce don't exist");
				ret = ARP_RETURN_CODE_NO_SUCH_PORT;
		}
		sprintf(cmdstr, "echo %d > base_reachable_time", stale_time);
		system(cmdstr);
		if (chdir(save_cwd) < 0){
				syslog_ax_arpsnooping_err("getcwd error");
				ret = ARP_RETURN_CODE_SET_STALE_TIME_ERR;
		}
	}else if (isVlanIntf == IS_ETH_SUBINTERFACE){
		sprintf(cmdstr, "echo %d > /proc/sys/net/ipv4/neigh/eth%d-%d.%d/base_reachable_time",
				stale_time, slot_no,port_no,vlanId);
		system(cmdstr);		
	}
	else if (isVlanIntf == IS_VE_INTERFACE) {
		/*wangchong for AXSSZFI-1539 ,the "port" used for "cpu_no&cpu_port_no"*/
		cpu_no = (unsigned char)((port_no >> 4) & 0xf);
		cpu_port_no = (unsigned char)(port_no & 0xf);

		sprintf(cmdstr, "echo %d > /proc/sys/net/ipv4/neigh/ve%02d%s%d.%d/base_reachable_time",
				stale_time, slot_no,cpu_name[cpu_no-1],cpu_port_no,vlanId);
		system(cmdstr);
	}
	else if (isVlanIntf == IS_EBR_INTERFACE)
	{
		sprintf(cmdstr, "echo %d > /proc/sys/net/ipv4/neigh/ebr%d-%d-%d/base_reachable_time",
			stale_time,slot_no,port_no, vlanId);
		system(cmdstr);

	}
	else if (isVlanIntf == IS_EBRL_INTERFACE)
	{
		sprintf(cmdstr, "echo %d > /proc/sys/net/ipv4/neigh/ebrl%d-%d-%d/base_reachable_time", 
			stale_time,slot_no,port_no,vlanId);
		system(cmdstr);

	}
	else if (isVlanIntf == IS_WLAN_INTERFACE)
	{
		sprintf(cmdstr, "echo %d > /proc/sys/net/ipv4/neigh/wlan%d-%d-%d/base_reachable_time", 
			stale_time,slot_no,port_no,vlanId);
		system(cmdstr);
	}
	else if (isVlanIntf == IS_WLANL_INTERFACE)
	{
		sprintf(cmdstr, "echo %d > /proc/sys/net/ipv4/neigh/wlanl%d-%d-%d/base_reachable_time", 
			stale_time,slot_no,port_no, vlanId);
		system(cmdstr);
	}

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);									 
	return reply;							
}
/*WangChao 07/11/11 */
int check_arp_entry_existed(unsigned int ipno, unsigned char* mac, unsigned char *check_ifname)
{

		int fd = -1;
		int status = 0;
		int len = 0;
		int ret = ARP_RETURN_CODE_SUCCESS;
		int sndbuf = _1K*32;
		int rcvbuf = _1K*32;
		int seq = time(NULL);
		int dump = 0;
		static char buf[_1K*16] = {0};
		char abuf[256] = {0};
		socklen_t addr_len;
		unsigned int ipAddr = 0;
     	unsigned char *ifName = NULL;
		unsigned int state = 0;
		unsigned char macAddr[MAC_ADDR_LEN] = {0};
		unsigned int signal = FALSE;
		unsigned int filterType = 0;
		unsigned int filter = 0;
		unsigned char f_macAddr[MAC_ADDR_LEN] = {0};
		int count = 0;
		struct nlmsghdr *h = NULL;
		struct ndmsg *ndm = NULL;
		struct rtattr * rta_tb[NDA_MAX + 1] = {NULL};
		struct rtattr *rta = NULL;
		struct sockaddr_nl	local;
		struct sockaddr_nl nladdr;
		struct sockaddr_nl nladdr2;
		struct iovec iov;
		struct msghdr msgh = {
			.msg_name = &nladdr,
			.msg_namelen = sizeof(struct sockaddr_nl),
			.msg_iov = &iov,
			.msg_iovlen = 1,
		};
		struct {
			struct nlmsghdr nlh;
			struct rtgenmsg g;
		} req;


		ifName = (unsigned char *)malloc(MAX_IFNAME_LEN + 1);
		if(!ifName){
			npd_syslog_err("memory malloc failed!\n");
			return NULL;
		}
		memset(ifName, 0, MAX_IFNAME_LEN + 1);
	    memset(&nladdr, 0, sizeof(struct sockaddr_nl));
        memset(&iov, 0, sizeof(struct iovec));
       
        iov.iov_base = buf;
        iov.iov_len = sizeof(buf);
        h = (struct nlmsghdr*)buf;
        fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (fd < 0) {
            npd_syslog_err(" socket error %d \n", fd);
            ret = ARP_RETURN_CODE_ERROR;

			free(ifName);	  /* code optimize: Argument cannot be negative houxx@autelan.com  2013-1-18 */
			ifName = NULL;			
			return 0;
        }
        if ((ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf))) < 0) {
            npd_syslog_err("setsocketopt SO_SNDBUF error %d \n", ret);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }

        if ((ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf))) < 0) {
            npd_syslog_err("setsockopt SO_RCVBUF error %d\n", ret);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }

        memset(&local, 0, sizeof(local));
        local.nl_family = AF_NETLINK;
        local.nl_groups = RTMGRP_IPV4_IFADDR;

        if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0) {
            npd_syslog_err("Cannot bind netlink socket\n");
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
        addr_len = sizeof(local);
        if (getsockname(fd, (struct sockaddr*)&local, &addr_len) < 0) {
            npd_syslog_err("Cannot getsockname");
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
        if (addr_len != sizeof(local)) {
            npd_syslog_err("Wrong address length %d\n", addr_len);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
        if (local.nl_family != AF_NETLINK) {
            npd_syslog_err("Wrong address family %d\n", local.nl_family);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }

        memset(&nladdr2, 0, sizeof(nladdr2));
        nladdr2.nl_family = AF_NETLINK;

        memset(&req, 0, sizeof(req));
        req.nlh.nlmsg_len = sizeof(req);
        req.nlh.nlmsg_type = RTM_GETNEIGH;
        req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
        req.nlh.nlmsg_pid = 0;
        req.nlh.nlmsg_seq = dump = ++seq;
        req.g.rtgen_family = AF_INET;

        if ((ret = sendto(fd, (void*) & req, sizeof(req), 0,
                          (struct sockaddr*) & nladdr2, sizeof(nladdr2))) < 0) {
            npd_syslog_err("Send request error %d\n", ret);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
		npd_syslog_dbg("send request to kernel\n");


		while(1){

			iov.iov_len = sizeof(buf);
			status = recvmsg(fd, &msgh, 0);
			npd_syslog_dbg("receive msg from kernel %d times \n",++count);

			if (status < 0) {
				continue;
			}

			if (status == 0) {
				ret = ARP_RETURN_CODE_ERROR;
				goto end;
			}

			h = (struct nlmsghdr*)buf;
			while (NLMSG_OK(h, status)) {
				if (nladdr.nl_pid != 0 ||
				    h->nlmsg_pid != local.nl_pid) {
					goto skip_it;
				}

				if (h->nlmsg_type == NLMSG_DONE){
					ret = ARP_RETURN_CODE_SUCCESS;
					goto end;
				}
				if (h->nlmsg_type == NLMSG_ERROR) {
					struct nlmsgerr *nlerr = (struct nlmsgerr*)NLMSG_DATA(h);
					if (h->nlmsg_len >= NLMSG_LENGTH(sizeof(struct nlmsgerr))) { 
						errno = -nlerr->error;
					}
					ret = ARP_RETURN_CODE_ERROR;
					goto end;
				}			
	            if ((h->nlmsg_type != RTM_NEWNEIGH) && (h->nlmsg_type != RTM_DELNEIGH)) {
	                goto skip_it;
	            }
	            if ((len = (h->nlmsg_len - NLMSG_LENGTH(sizeof(*ndm)))) < 0) {
	                npd_syslog_err("wrong nlmsg len %d\n", len);
	            }
	            ndm = NLMSG_DATA(h);
                memset(rta_tb, 0, sizeof(struct rtattr *) *(NDA_MAX + 1));
                rta = NDA_RTA(ndm);
                while (RTA_OK(rta, len)) {
                    if (rta->rta_type <= NDA_MAX) {
                        rta_tb[rta->rta_type] = rta;
                    }
                    rta = RTA_NEXT(rta, len);
                }
				signal = TRUE;
				ipAddr = 0;
				memset (ifName, 0, (MAX_IFNAME_LEN + 1));
				memset (macAddr, 0, MAC_ADDR_LEN);
				state = 0;
				
                if(rta_tb[NDA_DST]){
					ipAddr = *((unsigned int *)RTA_DATA(rta_tb[NDA_DST]));
                }
				if(ndm->ndm_ifindex){
					if_indextoname(ndm->ndm_ifindex, ifName);
				}
				else{
					strcpy (ifName, "null");
				}
				if(rta_tb[NDA_LLADDR]){
					memcpy(macAddr, RTA_DATA(rta_tb[NDA_LLADDR]), MAC_ADDR_LEN);
				}
				if(ndm->ndm_state){
					state = ndm->ndm_state;
				}
					
				
				npd_syslog_dbg("get ip neigh item: ip %#x ifname %s mac %.2x:%.2x:%.2x:%.2x:%.2x:%.2x state %d\n",\
					ipAddr,ifName,macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],state);

				syslog_ax_arpsnooping_dbg("kenrnel interface %02x:%02x:%02x:%02x:%02x:%02x ip: %d.%d.%d.%d\n config %02x:%02x:%02x:%02x:%02x:%02x ip: %d.%d.%d.%d\n",
    			                        macAddr[0],macAddr[1],macAddr[2],
    			                        macAddr[3],macAddr[4],macAddr[5],
    			                        (ipAddr>>24)&0xFF,(ipAddr>>16)&0xFF, \
    				                    (ipAddr>>8)&0xFF,ipAddr&0xFF,mac[0],mac[1],mac[2],
    			                        mac[3],mac[4],mac[5],
    			                        (ipno>>24)&0xFF,(ipno>>16)&0xFF, \
    				                    (ipno>>8)&0xFF,ipno&0xFF);
				if (NULL == check_ifname) {
					check_ifname = ifName;
					npd_syslog_dbg("check_ifname == %s\n", check_ifname);
				}
				if ((0 ==strcmp(ifName, check_ifname)) && (ipAddr == ipno) 
					&& (macAddr[0] == mac[0] && macAddr[1] == mac[1] && macAddr[2] == mac[2]
					 && macAddr[3] == mac[3] && macAddr[4] == mac[4] && macAddr[5] == mac[5]))
				{
					npd_syslog_dbg("check (strcmp(ifName, check_ifname) ==0)&& ipAddr == ipno && strncmp (macAddr,mac,6) match\n");
					free(ifName);
					ifName = NULL;/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
					close(fd);
					return 1;
					
				} 
				/*else 
				{
					npd_syslog_dbg("the arp entry not exist!!!!\n");
					return 0;
				} */
skip_it:
				h = NLMSG_NEXT(h, status);
			}

			if (msgh.msg_flags & MSG_TRUNC) {
				continue;
			}
			
			if (status) {
				ret = ARP_RETURN_CODE_ERROR;
				goto end;
			}
		}
		/*ret = ARP_RETURN_CODE_SUCCESS;*/
		
end:
			free(ifName);
			ifName = NULL;
			close(fd);
			
		return 0;
		
}

/*wangchao add to delete staitc arp config  2012/5/2*/
char *delete_sub_str(const char *str, const char *sub_str)
{
	char *result_str = NULL;
	int count = 0;
	int tmp = 0;
	int sub_tmp = 0;
	int start_ptr = 0;
	int result_ptr = 1;
	
    result_str = (char*)malloc(1);
    memset(result_str,'\0',1);
	
	while(*(str+tmp) != '\0'){
		if(*(str+tmp) == *(sub_str+sub_tmp)){
			tmp++;
			sub_tmp++;
			while(*(sub_str+sub_tmp) != '\0'){
				if(*(str+tmp) != *(sub_str+sub_tmp))
					break;
				else{
					tmp++;
					sub_tmp++;
				}
			}
			if(*(sub_str+sub_tmp) != '\0'){
				sub_tmp = 0;
				continue;
			}
			else{
				count++;
				if(tmp-sub_tmp-start_ptr != 0){
					
					result_str = (char*)realloc(result_str,strlen(result_str)+tmp-sub_tmp-start_ptr+1);
					
					memset(result_str+result_ptr-1, '\0', tmp-sub_tmp-start_ptr);
					strncat(result_str, str+start_ptr, tmp-sub_tmp-start_ptr);
				}
				result_ptr += tmp-sub_tmp-start_ptr;
				start_ptr += tmp-start_ptr;
				sub_tmp = 0;
			}
		}
		else
			tmp++;
	}
	
	result_str = (char *)realloc(result_str,strlen(result_str)+tmp-sub_tmp-start_ptr+1);
	memset(result_str+result_ptr-1, '\0', tmp-sub_tmp-start_ptr);
	strncat(result_str, str+start_ptr, tmp-sub_tmp-start_ptr);


	return result_str;
}



/*wangchao add for save running config 2012/5/2*/
int npd_arp_save_running_config(unsigned int ipAddr, unsigned char * macAddr, char*ifname,int flag)
{
	char *result_str;
	int fd;
	struct stat sb;
	void* data;
	char* str = NULL;
	int str_len;
	char * result = NULL;
	int ret = 0;

#define MACADDR macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]
#define IPADDR	(ipAddr>>24)&0xff,(ipAddr>>16)&0xff,(ipAddr>>8)&0xff,ipAddr&0xff

	/*str_len == strlen("config static-arp 00:00:00:00:00:00 255.255.255.255\n");*/
	unsigned char * staticArpStr = "interface %s\n config static-arp %02x:%02x:%02x:%02x:%02x:%02x %d.%d.%d.%d\n exit\n";

	str = (char*)malloc(256);
	sprintf(str,staticArpStr,ifname,MACADDR,IPADDR);/* notice: no '\n' here*/

	switch(flag){
		case ADD_ENTRY:
      	 	fd=open(STATIC_ARP_CONFIG_FILE_PATH,O_WRONLY | O_CREAT | O_APPEND);

			if(fd < 0)
	       	{	
				syslog_ax_arpsnooping_err("open fd error\n");
				free(str);
	       		return -1;
	       	}

			write(fd, str, strlen(str));
			
			close(fd);
			free(str);
			
			break;
			
		case DEL_ENTRY:
			fd=open(STATIC_ARP_CONFIG_FILE_PATH, O_RDWR);
			if (fd < 0){
				syslog_ax_arpsnooping_err("open fd error\n");
				free(str);
				return -1;
			}

			ret = fstat(fd,&sb);		/* code optimize: Unchecked return value from library houxx@autelan.com  2013-1-18 */
			if(-1 == ret)
			{
				syslog_ax_arpsnooping_err("check fd stat error\n");
				close(fd);
				free(str);
				return -1;
			}
			printf("sb.st_size = %d\n",sb.st_size);
			data = mmap(NULL,sb.st_size,PROT_WRITE| PROT_READ ,MAP_SHARED,fd,0);
			if(MAP_FAILED==data)
			{
				syslog_ax_arpsnooping_err("mmap error\n");
				close(fd);
				free(str);
				return -1;
			}
			
			result_str=delete_sub_str(data, str);
			munmap(data,sb.st_size);
			close(fd);

			fd=open(STATIC_ARP_CONFIG_FILE_PATH, O_RDWR|O_TRUNC);
			if(fd < 0)
	       	{	
				syslog_ax_arpsnooping_err("open fd error\n");
				free(result_str);	/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
				free(str);
				result_str = NULL;
				str = NULL;
	       		return -1;
	       	}
			
			write(fd, result_str, strlen(result_str));
			
			free(result_str);
			free(str);
			result_str = NULL;
			str = NULL;
			close(fd);
			
			break;
  	}
	return 0;
}





DBusMessage * npd_dbus_interface_static_arp(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
/*	extern BoardInfo board_info; */

	unsigned int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned int ipno = 0;
	unsigned short vlanId = 0;
	unsigned char macAddr[6]= {0};
	unsigned int ifindex = ~0UI;
	unsigned char isTagged = FALSE;
	unsigned char baseMac[6]={0};
    unsigned int isVlanIntf = 0;
    unsigned char slot_no = 0,port_no = 0;
    unsigned int eth_g_index = 0;
	unsigned char cpu_no = 0, cpu_port_no = 0;
	char *cpu_name[2]={"f","s"};
    unsigned int tag2 = 0;
    unsigned int intfType = 0;
	int rpa_interface = -1;
	enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;
	unsigned char ifname[21] = {0};
    unsigned int isAdd = 0;
	int ifnameType = 0;
	unsigned char * ifnamePrefix = NULL;
	struct arp_snooping_item_s* item;
	int is_active_master = -1;

#define IS_ETH_PORT_INTERFACE 0
#define IS_VLAN_INTERFACE 1
#define IS_VLAN_ADVANCED 2
/*wangchao add*/
#define IS_MNG_INTERFACE 		3
#define IS_VE_INTERFACE 		4
#define IS_EBR_INTERFACE 		5
#define IS_EBRL_INTERFACE 		6
#define IS_WLAN_INTERFACE 		7
#define IS_WLANL_INTERFACE 		8
#define IS_ETH_SUBINTERFACE		9
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_UINT32,&isVlanIntf,
		         DBUS_TYPE_UINT32,&ifnameType,
		         DBUS_TYPE_BYTE,&slot_no,
		         DBUS_TYPE_BYTE,&port_no,
				 DBUS_TYPE_UINT16,&vlanId,
				 DBUS_TYPE_UINT32,&tag2,
		         DBUS_TYPE_BYTE,&macAddr[0],
				 DBUS_TYPE_BYTE,&macAddr[1],
				 DBUS_TYPE_BYTE,&macAddr[2],
				 DBUS_TYPE_BYTE,&macAddr[3],
				 DBUS_TYPE_BYTE,&macAddr[4],
				 DBUS_TYPE_BYTE,&macAddr[5],
				 DBUS_TYPE_UINT32,&ipno,
				 DBUS_TYPE_UINT32,&isAdd,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	syslog_ax_arpsnooping_dbg("isVlanIntf == %d, ifnametype == %d, slot_no == %d, port_no == %d,vlanid == %d,tag2 == %d\n",
			isVlanIntf,ifnameType,slot_no,port_no,vlanId,tag2);


	if(ARP_RETURN_CODE_SUCCESS != host_board_info_get() ) {
		syslog_ax_arpsnooping_err("get host board info failed\n");
		ret = ARP_RETURN_CODE_ERROR;
	}
	/* for the 7605i product*/	
	if((ret == ARP_RETURN_CODE_SUCCESS)&&(PRODUCT_TYPE == AUTELAN_PRODUCT_AX7605I) && (is_master || (slot_no != slot_num))) 
	{
		syslog_ax_arpsnooping_dbg(" The path of 7605i !!!\n");
		
		ret = ARP_RETURN_CPU_INTERFACE_CODE_SUCCESS;
		if (isVlanIntf == IS_VE_INTERFACE){
		/*wangchong for AXSSZFI-1540 ,the "port" used for "cpu_no&cpu_port_no"*/		
    		cpu_no = (unsigned char)((port_no >> 4) & 0xf);
    		cpu_port_no = (unsigned char)(port_no & 0xf);
			sprintf(ifname, "ve%02d%s%d.%d",slot_no,cpu_name[cpu_no-1],cpu_port_no,vlanId);
		}else if (isVlanIntf == IS_ETH_PORT_INTERFACE){
		    sprintf(ifname,"%s%d-%d","eth",slot_no,port_no);
		}else if (isVlanIntf == IS_ETH_SUBINTERFACE){
			sprintf(ifname,"%s%d-%d.%d","eth",slot_no,port_no,vlanId);
		}else if (isVlanIntf == IS_EBR_INTERFACE){
			sprintf(ifname, "%s%d-%d-%d","ebr",slot_no,port_no,vlanId);	
		}else if(isVlanIntf == IS_VLAN_ADVANCED){
			sprintf(ifname,"vlan%d",vlanId);
		}else if (isVlanIntf == IS_EBRL_INTERFACE){
			sprintf(ifname, "%s%d-%d-%d","ebrl",slot_no,port_no,vlanId);
		}else if (isVlanIntf == IS_WLAN_INTERFACE){
			sprintf(ifname, "%s%d-%d-%d","wlan",slot_no,port_no,vlanId);		
		}else if (isVlanIntf == IS_WLANL_INTERFACE){
			sprintf(ifname, "%s%d-%d-%d","wlanl",slot_no,port_no,vlanId);					
		}

		syslog_ax_arpsnooping_dbg("in 7605 the ifname == %s\n", ifname);		
		if (isAdd) {
			 if (ARP_RETURN_CODE_SUCCESS ==(ret = npd_arpsnooping_check_mac_and_ipaddr_of_intf(ifname, ipno, 32, macAddr))){
		
					 if (ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_create_kern_static_arp(ipno,32,macAddr)) {
						 ret = ARP_RETURN_CODE_ERROR;
					 } else {

						 if (is_active_master = get_num_from_file("/dbm/local_board/is_active_master")){
							 if (npd_arp_save_running_config(ipno, macAddr, ifname,ADD_ENTRY) < 0){
								 syslog_ax_arpsnooping_err("npd_arp_save_running_config error\n");
							 }
					 	 }
						 
						 ret = 	ARP_RETURN_CPU_INTERFACE_CODE_SUCCESS;
					 }
			  }
			 
		} else {
			if (check_arp_entry_existed(ipno,macAddr,ifname)) { 				
    			 item = (struct arp_snooping_item_s*)malloc(sizeof(struct arp_snooping_item_s));
    			 memset(item,0, sizeof (struct arp_snooping_item_s));
    			 item->ipAddr = ipno;
    			 memcpy((void*)item->mac, macAddr, sizeof(item->mac));
    
    			 if (ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_del_kern_arp(item)) {
    				 ret = ARP_RETURN_CODE_ERROR;
    			 } else {
					 if (is_active_master = get_num_from_file("/dbm/local_board/is_active_master")){
						 if (npd_arp_save_running_config(ipno, macAddr, ifname,DEL_ENTRY) < 0){
							 syslog_ax_arpsnooping_err("npd_arp_save_running_config error\n");
						 }
					 }
					 
    				 ret = ARP_RETURN_CPU_INTERFACE_CODE_SUCCESS;
    			 }
				 free(item);
			} else {
   				syslog_ax_arpsnooping_dbg("arp entry not exist£¬can't delete\n");
   				ret = ARP_RETURN_ARP_ENTRY_NOT_EXISTED;
		    }
		}

	}
	
	/* for the 8610 product*/	
	else if((ret == ARP_RETURN_CODE_SUCCESS)\
		&&((PRODUCT_TYPE == AUTELAN_PRODUCT_AX8610)||(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8606)||(PRODUCT_TYPE == AUTELAN_PRODUCT_AX8800))\
		  	    &&((IS_MNG_INTERFACE == isVlanIntf)||(port_no < asic_board->asic_port_start_no) || (slot_no != slot_num) || (isVlanIntf == IS_VE_INTERFACE)
		   			/*|| ((slot_no != slot_num) && (BOARD_TYPE_AX71_2X12G12S != board_info.board_type))*/))
	{
		
		syslog_ax_arpsnooping_dbg(" The path of 8610 or 8606 or 8800!!!\n");
	    ret = ARP_RETURN_CPU_INTERFACE_CODE_SUCCESS;    
			if (isVlanIntf == IS_MNG_INTERFACE) {
				sprintf(ifname,"%s%d-%d","mng",slot_no,port_no);
			}else if (isVlanIntf == IS_VE_INTERFACE){
    		cpu_no = (unsigned char)((port_no >> 4) & 0xf);
    		cpu_port_no = (unsigned char)(port_no & 0xf);
			sprintf(ifname, "ve%02d%s%d.%d",slot_no,cpu_name[cpu_no-1],cpu_port_no,vlanId);
			}else if(isVlanIntf == IS_VLAN_ADVANCED){
				sprintf(ifname,"vlan%d",vlanId);
			}else if (isVlanIntf == IS_ETH_PORT_INTERFACE){				
				sprintf(ifname,"%s%d-%d","eth",slot_no,port_no);
			}else if (isVlanIntf == IS_ETH_SUBINTERFACE){
				sprintf(ifname,"%s%d-%d.%d","eth",slot_no,port_no,vlanId);
			}else if (isVlanIntf == IS_EBR_INTERFACE){
    			sprintf(ifname, "%s%d-%d-%d","ebr",slot_no,port_no,vlanId);	
    		}else if (isVlanIntf == IS_EBRL_INTERFACE){
    			sprintf(ifname, "%s%d-%d-%d","ebrl",slot_no,port_no,vlanId);
    		}else if (isVlanIntf == IS_WLAN_INTERFACE){
    			sprintf(ifname, "%s%d-%d-%d","wlan",slot_no,port_no,vlanId);		
    		}else if (isVlanIntf == IS_WLANL_INTERFACE){
    			sprintf(ifname, "%s%d-%d-%d","wlanl",slot_no,port_no,vlanId);					
    		}
			
			syslog_ax_arpsnooping_dbg("in 8610 the ifname == %s\n", ifname);
			syslog_ax_arpsnooping_dbg("isVlanIntf == %d,slot_no == %d, port_no == %d,vlanid == %d,tag2 == %d\n",
			isVlanIntf,slot_no,port_no,vlanId,tag2);
			if (isAdd) {
				 if (ARP_RETURN_CODE_SUCCESS ==(ret = npd_arpsnooping_check_mac_and_ipaddr_of_intf(ifname, ipno, 32, macAddr))){
						 if (ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_create_kern_static_arp(ipno,32,macAddr)) {
							 ret = ARP_RETURN_CODE_ERROR;
						 } else {

							 if (is_active_master = get_num_from_file("/dbm/local_board/is_active_master")){
								 if (npd_arp_save_running_config(ipno, macAddr, ifname,ADD_ENTRY) < 0){
									 syslog_ax_arpsnooping_err("npd_arp_save_running_config error\n");
								 }
					 		 }
													 
							 ret = 	ARP_RETURN_CPU_INTERFACE_CODE_SUCCESS;
						 }
				}
			} else {
        	     if (check_arp_entry_existed(ipno,macAddr,ifname)) { 
	    				 item = (struct arp_snooping_item_s*)malloc(sizeof(struct arp_snooping_item_s));
	    				 memset(item,0, sizeof (struct arp_snooping_item_s));
	    				 item->ipAddr = ipno;
	    				 memcpy((void*)item->mac, macAddr, sizeof(item->mac));
	    	
	    				 if (ARP_RETURN_CODE_SUCCESS != npd_arp_snooping_del_kern_arp(item)) {
	    					 ret = ARP_RETURN_CODE_ERROR;
	    				 } else {
        					 if (is_active_master = get_num_from_file("/dbm/local_board/is_active_master")){
        						 if (npd_arp_save_running_config(ipno, macAddr, ifname,DEL_ENTRY) < 0){
        							 syslog_ax_arpsnooping_err("npd_arp_save_running_config error\n");
        						 }
        					 }
							 
	    					 ret = ARP_RETURN_CPU_INTERFACE_CODE_SUCCESS;
	    			     }
	    				 free(item);
    		      }
				  else
    			  {
				         syslog_ax_arpsnooping_dbg("arp entry not exist£¬can't delete\n");
				         ret = ARP_RETURN_ARP_ENTRY_NOT_EXISTED;
			      }

		    }

     }
	else {
			if((IS_VLAN_ADVANCED != isVlanIntf) && IS_MNG_INTERFACE != isVlanIntf){
		    ret = ARP_RETURN_CODE_NO_SUCH_PORT;
	    	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
	    		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) {
	    			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
	    			syslog_ax_intf_err("index for %d/%d is %#0x\n",slot_no,port_no,eth_g_index);
	    			ret = ARP_RETURN_CODE_SUCCESS;
	    		}
	    	}
	    	if((ARP_RETURN_CODE_NO_SUCH_PORT == ret)&&\
	    		(PRODUCT_ID != PRODUCT_ID_AX7K)&&\
	    		0 == slot_no){		
	    		if(0 == (nbm_cvm_query_card_on(port_no-1))){
	    			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
	                ret = ARP_RETURN_CODE_SUCCESS;
	    		}		
	    	}
	    }
    }
    if(ARP_RETURN_CODE_SUCCESS == ret){

    	syslog_ax_arpsnooping_dbg(" interface static-arp %02x:%02x:%02x:%02x:%02x:%02x ip: %d.%d.%d.%d\n",
    			                        macAddr[0],macAddr[1],macAddr[2],
    			                        macAddr[3],macAddr[4],macAddr[5],
    			                        (ipno>>24)&0xFF,(ipno>>16)&0xFF, \
    				                    (ipno>>8)&0xFF,ipno&0xFF);
		if(!ifnameType){
        	ifnamePrefix = "eth";
		}
		else{
        	ifnamePrefix = "e";
		}

    	/*ret = npd_arp_snooping_create_kern_static_arp(ipno,&macAddr[0]);*/
        if(IS_VLAN_ADVANCED == isVlanIntf){        /*should be vlan advanced-routing*/
    	    sprintf(ifname,"vlan%d",vlanId);
            if(NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag)&&\
    			(flag == VLAN_PORT_SUBIF_EXIST)){
                intfType = ARP_INTF_ADVANCED_VLAN;
            }
            else if((TRUE == npd_vlan_interface_check(vlanId,&ifindex))&&\
                (~0UI != ifindex)){
                ret = ARP_RETURN_CODE_PORT_OR_TRUNK_NEEDED;
            }
            else{
                ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
            }
        }
        else if(IS_VLAN_INTERFACE == isVlanIntf){
            sprintf(ifname,"vlan%d",vlanId);
            if((TRUE == npd_vlan_interface_check(vlanId,&ifindex))&&\
                (~0UI != ifindex)){
                eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
                intfType = ARP_INTF_SIMPLE_VLAN;
                if(TRUE != npd_vlan_check_contain_port(vlanId,eth_g_index,&isTagged)){
                    ret = ARP_RETURN_CODE_PORT_NOT_IN_VLAN;
                }
            }
            else if(NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag)&&\
    			(flag == VLAN_PORT_SUBIF_EXIST)){
                ret = ARP_RETURN_CODE_PORT_OR_TRUNK_NOT_NEEDED;
            }
            else{
                ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
            }
        }
        else{
            if(!vlanId){/*not subif*/
                sprintf(ifname,"%s%d-%d",ifnamePrefix,slot_no,port_no);
                eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
                if((TRUE == npd_eth_port_rgmii_type_check(slot_no,port_no))||\
                    (TRUE == npd_eth_port_promis_interface_check(eth_g_index,&ifindex))){
                    if(INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifindex)){
                        syslog_ax_arpsnooping_err("get ifindex failed when config interface static arp \n");
                        ret = ARP_RETURN_CODE_ERROR;
                    }
                    intfType = ARP_INTF_ADVANCED_ETH_PORT;
                    vlanId = advanced_routing_default_vid;
                }
                else if((TRUE == npd_eth_port_interface_check(eth_g_index,&ifindex))&&\
                    (~0UI != ifindex)){
                    intfType = ARP_INTF_SIMPLE_ETH_PORT;
                    vlanId = NPD_PORT_L3INTF_VLAN_ID;
                }
                else{
                    ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
                }
            }
            else if(!tag2){            
                sprintf(ifname,"%s%d-%d.%d",ifnamePrefix,slot_no,port_no,vlanId);
                intfType = ARP_INTF_SUBIF;
            }
            else{
                sprintf(ifname,"%s%d-%d.%d.%d",ifnamePrefix,slot_no,port_no,vlanId,tag2);
                intfType = ARP_INTF_QINQ_SUBIF;
            }
        }
        if(ARP_RETURN_CODE_SUCCESS == ret){
            if(isAdd){
                ret = npd_arpsnooping_create_static_arp_for_intf(ifindex,ifname,eth_g_index,vlanId,0,ipno,32,macAddr,intfType);
            }
            else{                
                    if(((0 == ifindex)||(~0UI == ifindex))&&\
                        (INTERFACE_RETURN_CODE_SUCCESS != npd_intf_ifindex_get_by_ifname(ifname,&ifindex))){
                        ret = ARP_RETURN_CODE_ERROR;
                    }
                    else{
                       ret = npd_arp_snooping_remove_static_arp(ifindex,ipno,32,macAddr,vlanId,eth_g_index,intfType);
                    }
            }
        }
    }
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);									 
	return reply;								 

}

/*wangchao*/
DBusMessage * npd_dbus_dynamic_arp(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* 	 reply;
	DBusError 		 err;	
	DBusMessageIter	 iter;
	unsigned long  	 ipno;
	char cmdstr[128];
	int ret, ret2;
	unsigned char macAddr[6]= {0};
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
				 DBUS_TYPE_UINT32,&ipno,
				 DBUS_TYPE_INVALID))) {
	
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	if (0 == check_arp_entry_existed(ipno,macAddr, NULL)) {
	 sprintf(cmdstr,"/usr/sbin/arp -d %d.%d.%d.%d\n",(ipno>>24)&0xFF,(ipno>>16)&0xFF,(ipno>>8)&0xFF,ipno&0xFF);
	 /*sprintf(cmdstr,"/usr/sbin/arp -d %s\n", ipno);*/
	 ret = system(cmdstr);
     ret2 = WEXITSTATUS(ret);
	 if (ret2 != 0) {
		ret = ARP_RETURN_CODE_ERROR;
	 }else{
	 	ret = ARP_RETURN_CODE_SUCCESS;
    	 }
	}
	else 
	{
		 ret2 = ARP_RETURN_ARP_ENTRY_NOT_EXISTED;
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret2); 								 
	return reply;	
	
}


DBusMessage * npd_dbus_interface_static_arp_trunk(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ret = ARP_RETURN_CODE_SUCCESS;
	unsigned int ipno = 0;
	unsigned short vlanId = 0;
	unsigned char macAddr[6]= {0};
	unsigned int ifindex = ~0UI;
	unsigned char isTagged = FALSE;
	unsigned char baseMac[6]={0};
    unsigned int isVlanIntf = 0;
    unsigned int trunkId = 0;
    unsigned int intfType = 0;
	enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;
	unsigned char ifname[21] = {0};
    unsigned int isAdd = 0;

#define IS_ETH_PORT_INTERFACE 0
#define IS_VLAN_INTERFACE 1
#define IS_VLAN_ADVANCED 2
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		         DBUS_TYPE_UINT32,&isVlanIntf,
				 DBUS_TYPE_UINT32,&vlanId,
				 DBUS_TYPE_UINT32,&trunkId,		         
		         DBUS_TYPE_BYTE,&macAddr[0],
				 DBUS_TYPE_BYTE,&macAddr[1],
				 DBUS_TYPE_BYTE,&macAddr[2],
				 DBUS_TYPE_BYTE,&macAddr[3],
				 DBUS_TYPE_BYTE,&macAddr[4],
				 DBUS_TYPE_BYTE,&macAddr[5],
				 DBUS_TYPE_UINT32,&ipno,
				 DBUS_TYPE_UINT32,&isAdd,
				 DBUS_TYPE_INVALID))) {
		syslog_ax_arpsnooping_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_arpsnooping_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	syslog_ax_arpsnooping_dbg(" interface %s static-arp %02x:%02x:%02x:%02x:%02x:%02x ip: %d.%d.%d.%d\n",
			                        ifname,macAddr[0],macAddr[1],macAddr[2],
			                        macAddr[3],macAddr[4],macAddr[5],
			                        (ipno>>24)&0xFF,(ipno>>16)&0xFF, \
				                    (ipno>>8)&0xFF,ipno&0xFF);

	/*ret = npd_arp_snooping_create_kern_static_arp(ipno,&macAddr[0]);*/
    if(IS_VLAN_INTERFACE == isVlanIntf){
        sprintf(ifname,"vlan%d",vlanId);
        if((TRUE == npd_vlan_interface_check(vlanId,&ifindex))&&\
            (~0UI != ifindex)){
            intfType = ARP_INTF_SIMPLE_VLAN;
            if(TRUE != npd_check_trunk_exist(trunkId)){
                ret = ARP_RETURN_CODE_TRUNK_NOT_EXISTS;
            }
            else if(TRUE != npd_vlan_check_contain_trunk(vlanId,trunkId,&isTagged)){
                ret = ARP_RETURN_CODE_TRUNK_NOT_IN_VLAN;
            }
        }
        else if(NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag)&&\
			(flag == VLAN_PORT_SUBIF_EXIST)){
            ret = ARP_RETURN_CODE_PORT_OR_TRUNK_NOT_NEEDED;
        }
        else{
            ret = ARP_RETURN_CODE_INTERFACE_NOTEXIST;
        }
    }
    else{
        ret = COMMON_RETURN_CODE_BADPARAM;
    }
    if(ARP_RETURN_CODE_SUCCESS == ret){
        if(isAdd){
            ret = npd_arp_snooping_create_static_arp_for_trunk(ifindex,ipno,32,macAddr,vlanId,trunkId);
        }
        else{
            ret = npd_arp_snooping_remove_static_arp_for_trunk(ifindex,ipno,32,macAddr,vlanId,trunkId);
        }
    }
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);									 
	return reply;								 

}

/*************************************************************
 *
 * OUTPUT:
 *		showStr : String - the result of static arp running-config 
 *					e.g. "ip static-arp 1/1 00:00:00:00:00:01 192.168.0.2/32 1\n"
 *
 *************************************************************/
DBusMessage *npd_dbus_static_arp_show_running(DBusConnection *conn, DBusMessage *msg, void *user_data) {
		DBusMessage* reply;
		DBusMessageIter  iter;
	
		char *showStr = NULL;
		int currentLen = 0;
		showStr = (char*)malloc(NPD_INTF_SAVE_STATIC_ARP_MEM);
		if(NULL == showStr) {
			return NULL;
		}
		memset(showStr,0,NPD_INTF_SAVE_STATIC_ARP_MEM);

		syslog_ax_intf_dbg("static arp show running-config\n");
		npd_dbus_save_static_arp(showStr,NPD_INTF_SAVE_STATIC_ARP_MEM,&currentLen);
		reply = dbus_message_new_method_return(msg);
		
		dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_STRING,
                                         &showStr); 
									 
		free(showStr);
		showStr = NULL;
		
		return reply;
}



DBusMessage *npd_dbus_interrupt_rxmax_show_running(DBusConnection *conn, DBusMessage *msg, void *user_data) {
		DBusMessage* reply;
		DBusMessageIter  iter;
		DBusError err;
	
		char *showStr = NULL;
		int interrupt_rxmax=0;
		showStr = (char*)malloc(NPD_INTF_SAVE_INTERRUPT_RXMAX_MEM);
		if(NULL == showStr) {
			return NULL;
		}
		memset(showStr,0,NPD_INTF_SAVE_INTERRUPT_RXMAX_MEM);

		dbus_error_init(&err);

		if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&interrupt_rxmax,
										DBUS_TYPE_INVALID)))
			{
		   	 syslog_ax_intf_err("Unable to get input args ");
		   	 if (dbus_error_is_set(&err)) {
			   	 syslog_ax_intf_err("%s raised: %s", err.name, err.message);
			   	 dbus_error_free(&err);
			}
			 
			free(showStr);	/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
			showStr = NULL;
			return NULL;
		}
		syslog_ax_intf_dbg("interrupt rxmax show running-config\n");
		npd_dbus_save_interrupt_rxmax(showStr,NPD_INTF_SAVE_INTERRUPT_RXMAX_MEM,interrupt_rxmax);
		reply = dbus_message_new_method_return(msg);
		
		dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_STRING,
                                         &showStr); 

									 
		free(showStr);
		showStr = NULL;
		
		return reply;
}


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
)
{
	int ret = ARP_RETURN_CODE_CHECK_IP_ERROR;
	unsigned int ipAddrs[MAX_IP_COUNT] = {INVALID_HOST_IP};
	unsigned int masks[MAX_IP_COUNT] = {0};
	int i = 0;
	unsigned int haveIp = 0,haveTheIp = 0,haveSameSubnet = 0;
	if((NULL == ifname )||(NULL == ifindex)){
        return ARP_RETURN_CODE_CHECK_IP_ERROR;
	}
	ret = npd_intf_netlink_get_ip_addrs(ifname,ifindex,ipAddrs,masks);
	if(INTERFACE_RETURN_CODE_SUCCESS != ret){
        return ARP_RETURN_CODE_CHECK_IP_ERROR;
	}
	for(i=0;i<MAX_IP_COUNT;i++){
		if(INVALID_HOST_IP != ipAddrs[i]){
		     haveIp = 1;
			 if(ipAddrs[i] == ipAddress){
			 	haveTheIp = 1;
				break;
			 }
			 if((ipAddrs[i] & masks[i]) == (ipAddress & masks[i])){
                 haveSameSubnet = 1;
				 break;
			 }
			 
		}
	}
	if(haveIp == 0){
		syslog_ax_intf_dbg("npd_intf_check_ip_address:: l3intf don't have any ip address!\n");
		ret = ARP_RETURN_CODE_NO_HAVE_ANY_IP;
	}else if(haveTheIp == 1){
        syslog_ax_intf_dbg("npd_intf_check_ip_address:: already have the ip address!\n");
		ret = ARP_RETURN_CODE_HAVE_THE_IP;
	}else if(haveSameSubnet == 1){
        syslog_ax_intf_dbg("npd_intf_check_ip_address:: l3intf don't have the ip and have the same sub net ip!\n");	
		ret = ARP_RETURN_CODE_NO_HAVE_THE_IP;
	}else{
        syslog_ax_intf_dbg("npd_intf_check_ip_address:: don't have same subnet ip address!\n");
		syslog_ax_intf_dbg("\t the ipAddress is %d.%d.%d.%d\n",\
			(ipAddress>>24)&0xff,(ipAddress>>16)&0xff,(ipAddress>>8)&0xff,ipAddress&0xff);
		syslog_ax_intf_dbg("\t the intf ips are \n");
		for (i=0;i<MAX_IP_COUNT;i++){
			if(INVALID_HOST_IP != ipAddrs[i]){
	            syslog_ax_intf_dbg("\t ip %d.%d.%d.%d mask %d.%d.%d.%d\n",\
					(ipAddrs[i]>>24)&0xff,(ipAddrs[i]>>16)&0xff,(ipAddrs[i]>>8)&0xff,ipAddrs[i]&0xff,\
					(masks[i]>>24)&0xff,(masks[i]>>16)&0xff,(masks[i]>>8)&0xff,masks[i]&0xff);
			}
		}
		
		ret = ARP_RETURN_CODE_NOT_SAME_SUB_NET;
	}
	
    return ret;
}

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
)
{
	unsigned int ifindex = ~0UI;
	int ret = ARP_RETURN_CODE_CHECK_IP_ERROR;
	unsigned int ipAddrs[MAX_IP_COUNT] = {INVALID_HOST_IP};
	unsigned int masks[MAX_IP_COUNT] = {0};
	int i = 0;
	unsigned int haveIp = 0,haveTheIp = 0,haveSameSubnet = 0;
	if(TRUE != npd_intf_port_check(vid,eth_g_index,&ifindex)){
		return ARP_RETURN_CODE_CHECK_IP_ERROR;
	}
	if(ifindex != ~0UI){
		ret = npd_intf_get_ip_addrs(ifindex,ipAddrs,masks);
		if(INTERFACE_RETURN_CODE_SUCCESS != ret){
	        return ARP_RETURN_CODE_CHECK_IP_ERROR;
		}
		for(i=0;i<MAX_IP_COUNT;i++){
			if(INVALID_HOST_IP != ipAddrs[i]){
			     haveIp = 1;
				 if(ipAddrs[i] == ipAddress){
				 	haveTheIp = 1;
					break;
				 }
				 if((ipAddrs[i] & masks[i]) == (ipAddress & masks[i])){
	                 haveSameSubnet = 1;
					 break;
				 }
				 
			}
		}
		if(haveIp == 0){
			syslog_ax_intf_dbg("npd_intf_check_ip_address:: l3intf don't have any ip address!\n");
			ret = ARP_RETURN_CODE_NO_HAVE_ANY_IP;
		}else if(haveTheIp == 1){
	        syslog_ax_intf_dbg("npd_intf_check_ip_address:: already have the ip address!\n");
			ret = ARP_RETURN_CODE_HAVE_THE_IP;
		}else if(haveSameSubnet == 1){
	        syslog_ax_intf_dbg("npd_intf_check_ip_address:: l3intf don't have the ip and have the same sub net ip!\n");	
			ret = ARP_RETURN_CODE_NO_HAVE_THE_IP;
		}else{
	        syslog_ax_intf_dbg("npd_intf_check_ip_address:: don't have same subnet ip address!\n");
			syslog_ax_intf_dbg("\t the ipAddress is %d.%d.%d.%d\n",\
				(ipAddress>>24)&0xff,(ipAddress>>16)&0xff,(ipAddress>>8)&0xff,ipAddress&0xff);
			syslog_ax_intf_dbg("\t the intf ips are \n");
			for (i=0;i<MAX_IP_COUNT;i++){
				if(INVALID_HOST_IP != ipAddrs[i]){
		            syslog_ax_intf_dbg("\t ip %d.%d.%d.%d mask %d.%d.%d.%d\n",\
						(ipAddrs[i]>>24)&0xff,(ipAddrs[i]>>16)&0xff,(ipAddrs[i]>>8)&0xff,ipAddrs[i]&0xff,\
						(masks[i]>>24)&0xff,(masks[i]>>16)&0xff,(masks[i]>>8)&0xff,masks[i]&0xff);
				}
			}
			
			ret = ARP_RETURN_CODE_NOT_SAME_SUB_NET;
		}
	}
    return ret;
}


/*******************************************************************************
* npd_system_arp_strict_check_enable
*
* DESCRIPTION:
*      Set whether the CHIP do the strict mac check or not.
*
* INPUTS:
*	  isenable :
*                     1--set enable
*                     0--set disalbe
*
* OUTPUTS:
*       ret - ARP_RETURN_CODE_SUCCESS  -- enable success
*		   ARP_RETURN_CODE_NAM_ERROR  -- enable failed
* RETURNS:
*       reply.
*
* COMMENTS:
*       
*
*******************************************************************************/
DBusMessage * npd_system_arp_strict_check_enable(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	
	unsigned int	isenable  = 0;
	unsigned int	ret = ARP_RETURN_CODE_SUCCESS;
	
	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&isenable,
									DBUS_TYPE_INVALID))) {
		syslog_ax_product_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_product_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		ret = ARP_RETURN_CODE_ERROR;
	}

	arp_strict_check = isenable;
	
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_append_args(reply,
							 DBUS_TYPE_UINT32,&ret,
							 DBUS_TYPE_INVALID);
	return reply;
	
}


int arp_entry_count(unsigned int filterType, unsigned int filter, unsigned char*f_macAddr)
{

		int fd = -1;
		int status = 0;
		int len = 0;
		int ret = 0;
		int sndbuf = _1K*32;
		int rcvbuf = _1K*32;
		int seq = time(NULL);
		int dump = 0;
		static char buf[_1K*16] = {0};
		/*char abuf[256] = {0};*/
		socklen_t addr_len;
		unsigned int ipAddr = 0;
     	unsigned char *ifName = NULL;
		unsigned int state = 0;
		unsigned char macAddr_1[MAC_ADDR_LEN] = {0};
		unsigned int signal = FALSE;
		int count = 0;
		struct nlmsghdr *h = NULL;
		struct ndmsg *ndm = NULL;
		struct rtattr * rta_tb[NDA_MAX + 1] = {NULL};
		struct rtattr *rta = NULL;
		struct sockaddr_nl	local;
		struct sockaddr_nl nladdr;
		struct sockaddr_nl nladdr2;
		struct iovec iov;
		struct msghdr msgh = {
			.msg_name = &nladdr,
			.msg_namelen = sizeof(struct sockaddr_nl),
			.msg_iov = &iov,
			.msg_iovlen = 1,
		};
		struct {
			struct nlmsghdr nlh;
			struct rtgenmsg g;
		} req;

		ifName = (unsigned char *)malloc(MAX_IFNAME_LEN + 1);
		if(!ifName){
			npd_syslog_err("memory malloc failed!\n");
			return NULL;
		}
		memset(ifName, 0, MAX_IFNAME_LEN + 1);
	    memset(&nladdr, 0, sizeof(struct sockaddr_nl));
        memset(&iov, 0, sizeof(struct iovec));
       
        iov.iov_base = buf;
        iov.iov_len = sizeof(buf);
        h = (struct nlmsghdr*)buf;
        fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (fd < 0) {
            npd_syslog_err(" socket error %d \n", fd);
            ret = -1;
			goto end_without_close_fd;/*_without_close_fd*/;
        }
        if ((ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf))) < 0) {
            npd_syslog_err("setsocketopt SO_SNDBUF error %d \n", ret);
            ret = -1;
			goto end;
        }

        if ((ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf))) < 0) {
            npd_syslog_err("setsockopt SO_RCVBUF error %d\n", ret);
            ret = -1;
			goto end;
        }

        memset(&local, 0, sizeof(local));
        local.nl_family = AF_NETLINK;
        local.nl_groups = RTMGRP_IPV4_IFADDR;

        if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0) {
            npd_syslog_err("Cannot bind netlink socket\n");
            ret = -1;
			goto end;
        }
        addr_len = sizeof(local);
        if (getsockname(fd, (struct sockaddr*)&local, &addr_len) < 0) {
            npd_syslog_err("Cannot getsockname");
            ret = -1;
			goto end;
        }
        if (addr_len != sizeof(local)) {
            npd_syslog_err("Wrong address length %d\n", addr_len);
            ret = -1;
			goto end;
        }
        if (local.nl_family != AF_NETLINK) {
            npd_syslog_err("Wrong address family %d\n", local.nl_family);
            ret = -1;
			goto end;
        }

        memset(&nladdr2, 0, sizeof(nladdr2));
        nladdr2.nl_family = AF_NETLINK;

        memset(&req, 0, sizeof(req));
        req.nlh.nlmsg_len = sizeof(req);
        req.nlh.nlmsg_type = RTM_GETNEIGH;
        req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
        req.nlh.nlmsg_pid = 0;
        req.nlh.nlmsg_seq = dump = ++seq;
        req.g.rtgen_family = AF_INET;

        if ((ret = sendto(fd, (void*) & req, sizeof(req), 0,
                          (struct sockaddr*) & nladdr2, sizeof(nladdr2))) < 0) {
            npd_syslog_err("Send request error %d\n", ret);
            ret = -1;
			goto end;
        }
		npd_syslog_dbg("send request to kernel\n");


		while(1){

			iov.iov_len = sizeof(buf);
			status = recvmsg(fd, &msgh, 0);

			if (status < 0) {
				continue;
			}

			if (status == 0) {
				ret = -1;
				goto end;
			}

			h = (struct nlmsghdr*)buf;
			while (NLMSG_OK(h, status)) {
				if (nladdr.nl_pid != 0 ||
				    h->nlmsg_pid != local.nl_pid) {
					goto skip_it;
				}

				if (h->nlmsg_type == NLMSG_DONE){		
					close(fd);
					free(ifName);	/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
					ifName = NULL;
					return count;
				}
				if (h->nlmsg_type == NLMSG_ERROR) {
					struct nlmsgerr *nlerr = (struct nlmsgerr*)NLMSG_DATA(h);
					if (h->nlmsg_len >= NLMSG_LENGTH(sizeof(struct nlmsgerr))) { 
						errno = -nlerr->error;
					}
					ret = -1;
					goto end;
				}			
	            if ((h->nlmsg_type != RTM_NEWNEIGH) && (h->nlmsg_type != RTM_DELNEIGH)) {
	                goto skip_it;
	            }
	            if ((len = (h->nlmsg_len - NLMSG_LENGTH(sizeof(*ndm)))) < 0) {
	                npd_syslog_err("wrong nlmsg len %d\n", len);
	            }
	            ndm = NLMSG_DATA(h);
                memset(rta_tb, 0, sizeof(struct rtattr *) *(NDA_MAX + 1));
                rta = NDA_RTA(ndm);
                while (RTA_OK(rta, len)) {
                    if (rta->rta_type <= NDA_MAX) {
                        rta_tb[rta->rta_type] = rta;
                    }
                    rta = RTA_NEXT(rta, len);
                }
				signal = TRUE;
				ipAddr = 0;
				memset (ifName, 0, (MAX_IFNAME_LEN + 1));
				memset (macAddr_1, 0, MAC_ADDR_LEN);
				state = 0;
				
                if(rta_tb[NDA_DST]){
					ipAddr = *((unsigned int *)RTA_DATA(rta_tb[NDA_DST]));
                }
				if(ndm->ndm_ifindex){
					if_indextoname(ndm->ndm_ifindex, ifName);
				}
				else{
					strcpy (ifName, "null");
				}
				if(rta_tb[NDA_LLADDR]){
					memcpy(macAddr_1, RTA_DATA(rta_tb[NDA_LLADDR]), MAC_ADDR_LEN);
				}
				if(ndm->ndm_state){
					state = ndm->ndm_state;
				}			
				
				npd_syslog_dbg("macAddr_1: ip %#x ifname %s mac %.2x:%.2x:%.2x:%.2x:%.2x:%.2x state %d\n",\
					ipAddr,ifName,macAddr_1[0],macAddr_1[1],macAddr_1[2],macAddr_1[3],macAddr_1[4],macAddr_1[5],state);

				if (!(macAddr_1[0]==0 && macAddr_1[1] == 0 && macAddr_1[2] == 0 && macAddr_1[3] == 0 && macAddr_1[4] == 0&& macAddr_1[5] == 0)) {
            		if(ipAddr && ((!state) || (state & (~0x21))))
				{
					/*state is none, or not only incomplete and failed*/
            		if (ARP_FILTER_NONE == filterType) {
                        ++count;
                    }	

                    if(ARP_FILTER_MAC == filterType && f_macAddr != NULL) 
    				{	
    					if(memcmp(macAddr_1, f_macAddr, MAC_ADDR_LEN) == 0){
                            ++count;
    					}
    				}
    				else if(ARP_FILTER_NONE != filterType)
    				{
    					if((ARP_FILTER_IP == filterType)&&(ipAddr == filter)){
        					++count;
    					}
    					else if((ARP_FILTER_IFINDEX == filterType)&&(filter == ndm->ndm_ifindex)){
            				++count;
    					}
    					else if((ARP_FILTER_STATE == filterType)&&(!(filter&state))){
    						++count;
    					}
    				}

					npd_syslog_dbg("arp entry count == %d\n", count);
            		}
				}				
				
skip_it:
				h = NLMSG_NEXT(h, status);
			}

			if (msgh.msg_flags & MSG_TRUNC) {
				continue;
			}
			
			if (status) {
				ret = -1;
				goto end;
			}
		}
		close(fd);
		free(ifName);
		ifName = NULL;
		npd_syslog_err("arp entry count == %d\n", count);
		return count;
end:
			
		close(fd);
		free(ifName);		
end_without_close_fd:		
	    return ret;
		
}

	DBusMessage * npd_dbus_show_ip_neigh(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	    DBusMessage*     reply;
        DBusMessageIter  iter;
		DBusMessageIter  iter_array;
		DBusMessageIter iter_struct;
        DBusError err;
		
        int fd = -1;
        int status = 0;
        int len = 0;
        int ret = ARP_RETURN_CODE_SUCCESS;
        int sndbuf = _1K*32;
        int rcvbuf = _1K*32;
        int seq = time(NULL);
		int dump = 0;
        static char buf[_1K*16] = {0};
        char abuf[256] = {0};
        socklen_t addr_len;
		unsigned int filter = 0;
		unsigned int ipAddr = 0;
		unsigned char *ifName = NULL,*dst_ifname = NULL;
		unsigned int state = 0;
		unsigned char macAddr[MAC_ADDR_LEN] = {0};
		unsigned int signal = FALSE;
		unsigned int filterType = 0;
		unsigned char f_macAddr[MAC_ADDR_LEN] = {0};
		int count = 0;
		int ret_count = -1;
        unsigned char* mac = NULL;
		
        struct nlmsghdr *h = NULL;
        struct ndmsg *ndm = NULL;
        struct rtattr * rta_tb[NDA_MAX + 1] = {NULL};
        struct rtattr *rta = NULL;
        struct sockaddr_nl  local;
        struct sockaddr_nl nladdr;
        struct sockaddr_nl nladdr2;
        struct iovec iov;
        struct msghdr msgh = {
            .msg_name = &nladdr,
            .msg_namelen = sizeof(struct sockaddr_nl),
            .msg_iov = &iov,
            .msg_iovlen = 1,
        };
        struct {
            struct nlmsghdr nlh;
            struct rtgenmsg g;
        } req;

		
        dbus_error_init(&err);
		dbus_message_iter_init(msg, &iter);
		
		dbus_message_iter_get_basic(&iter, &filterType);

		dbus_message_iter_next(&iter);
		if(ARP_FILTER_MAC == filterType)
		{
			dbus_message_iter_get_basic(&iter, &f_macAddr[0]);
			dbus_message_iter_next(&iter);
			
			dbus_message_iter_get_basic(&iter, &f_macAddr[1]);
			dbus_message_iter_next(&iter);
			
			dbus_message_iter_get_basic(&iter, &f_macAddr[2]);
			dbus_message_iter_next(&iter);
			
			dbus_message_iter_get_basic(&iter, &f_macAddr[3]);
			dbus_message_iter_next(&iter);
			
			dbus_message_iter_get_basic(&iter, &f_macAddr[4]);
			dbus_message_iter_next(&iter);
			
			dbus_message_iter_get_basic(&iter, &f_macAddr[5]);
			dbus_message_iter_next(&iter);

            mac = (char*)&f_macAddr;  
		}
		else if(ARP_FILTER_NONE != filterType)
		{			
			dbus_message_iter_get_basic(&iter, &filter);
			dbus_message_iter_next(&iter);
			if(ARP_FILTER_IFINDEX == filterType)
			{
				dst_ifname = (char *)malloc(MAX_IFNAME_LEN+1);
				memset(dst_ifname,0,MAX_IFNAME_LEN+1);
				dbus_message_iter_get_basic(&iter, &dst_ifname);
				dbus_message_iter_next(&iter); 
				filter = if_nametoindex(dst_ifname);


				npd_syslog_dbg("filter = %d,dst_ifname = %s\n",filter,dst_ifname);
				free(dst_ifname);
				dst_ifname = NULL;
			}
		}
			
		ifName = (unsigned char *)malloc(MAX_IFNAME_LEN + 1);
		if(!ifName)
		{
			npd_syslog_err("memory malloc failed!\n");
			return NULL;
		}
		memset(ifName, 0, MAX_IFNAME_LEN + 1);
        reply = dbus_message_new_method_return(msg);
		npd_syslog_dbg("enter show ip neigh,reply %s\n",reply ? "not null":"null");
		if(!reply)
		{
			free(ifName);
			return NULL;
		}
        dbus_message_iter_init_append(reply, &iter);
		
		ret_count  = arp_entry_count(filterType, filter, mac);

		if (ret_count < 0) 
		{
			npd_syslog_err("arp_entry_count error!\n");
			ret = ARP_RETURN_CODE_ERROR;
			goto end;
		}
		
		npd_syslog_dbg("receive msg from kernel %d times \n",ret_count);

		dbus_message_iter_append_basic(&iter,
							   DBUS_TYPE_INT32,
							   &ret_count);

        memset(&nladdr, 0, sizeof(struct sockaddr_nl));
        memset(&iov, 0, sizeof(struct iovec));
       
        iov.iov_base = buf;
        iov.iov_len = sizeof(buf);
        h = (struct nlmsghdr*)buf;
        fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
        if (fd < 0) {
            npd_syslog_err(" socket error %d \n", fd);
            ret = ARP_RETURN_CODE_ERROR;
			goto end_without_close_fd;
        }
        if ((ret = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf))) < 0) {
            npd_syslog_err("setsocketopt SO_SNDBUF error %d \n", ret);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }

        if ((ret = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rcvbuf, sizeof(rcvbuf))) < 0) {
            npd_syslog_err("setsockopt SO_RCVBUF error %d\n", ret);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }

        memset(&local, 0, sizeof(local));
        local.nl_family = AF_NETLINK;
        local.nl_groups = RTMGRP_IPV4_IFADDR;

        if (bind(fd, (struct sockaddr*)&local, sizeof(local)) < 0) {
            npd_syslog_err("Cannot bind netlink socket\n");
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
        addr_len = sizeof(local);
        if (getsockname(fd, (struct sockaddr*)&local, &addr_len) < 0) {
            npd_syslog_err("Cannot getsockname");
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
        if (addr_len != sizeof(local)) {
            npd_syslog_err("Wrong address length %d\n", addr_len);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
        if (local.nl_family != AF_NETLINK) {
            npd_syslog_err("Wrong address family %d\n", local.nl_family);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }

        memset(&nladdr2, 0, sizeof(nladdr2));
        nladdr2.nl_family = AF_NETLINK;

        memset(&req, 0, sizeof(req));
        req.nlh.nlmsg_len = sizeof(req);
        req.nlh.nlmsg_type = RTM_GETNEIGH;
        req.nlh.nlmsg_flags = NLM_F_ROOT | NLM_F_MATCH | NLM_F_REQUEST;
        req.nlh.nlmsg_pid = 0;
        req.nlh.nlmsg_seq = dump = ++seq;
        req.g.rtgen_family = AF_INET;

        if ((ret = sendto(fd, (void*) & req, sizeof(req), 0,
                          (struct sockaddr*) & nladdr2, sizeof(nladdr2))) < 0) {
            npd_syslog_err("Send request error %d\n", ret);
            ret = ARP_RETURN_CODE_ERROR;
			goto end;
        }
		npd_syslog_dbg("send request to kernel\n");
		dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
											DBUS_TYPE_UINT32_AS_STRING /*signal*/
											DBUS_TYPE_UINT32_AS_STRING /*ipaddr*/
											DBUS_TYPE_STRING_AS_STRING /*ifname*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[0]*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[1]*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[2]*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[3]*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[4]*/
										   	DBUS_TYPE_BYTE_AS_STRING /*mac[5]*/
										   	DBUS_TYPE_UINT32_AS_STRING /*state*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
		while (1) {

			iov.iov_len = sizeof(buf);
			status = recvmsg(fd, &msgh, 0);
			npd_syslog_dbg("receive msg from kernel %d times \n",++count);

			if (status < 0) {
				continue;
			}

			if (status == 0) {
				ret = ARP_RETURN_CODE_ERROR;
				goto end;
			}

			h = (struct nlmsghdr*)buf;
			while (NLMSG_OK(h, status)) {
				if (nladdr.nl_pid != 0 ||
				    h->nlmsg_pid != local.nl_pid) {
					goto skip_it;
				}

				if (h->nlmsg_type == NLMSG_DONE){
					ret = ARP_RETURN_CODE_SUCCESS;
					goto end;
				}
				if (h->nlmsg_type == NLMSG_ERROR) {
					struct nlmsgerr *nlerr = (struct nlmsgerr*)NLMSG_DATA(h);
					if (h->nlmsg_len >= NLMSG_LENGTH(sizeof(struct nlmsgerr))) { 
						errno = -nlerr->error;
					}
					ret = ARP_RETURN_CODE_ERROR;
					goto end;
				}			
	            if ((h->nlmsg_type != RTM_NEWNEIGH) && (h->nlmsg_type != RTM_DELNEIGH)) {
	                goto skip_it;
	            }
	            if ((len = (h->nlmsg_len - NLMSG_LENGTH(sizeof(*ndm)))) < 0) {
	                npd_syslog_err("wrong nlmsg len %d\n", len);
	            }
	            ndm = NLMSG_DATA(h);
                memset(rta_tb, 0, sizeof(struct rtattr *) *(NDA_MAX + 1));
                rta = NDA_RTA(ndm);
                while (RTA_OK(rta, len)) {
                    if (rta->rta_type <= NDA_MAX) {
                        rta_tb[rta->rta_type] = rta;
                    }
                    rta = RTA_NEXT(rta, len);
                }
				signal = TRUE;
				ipAddr = 0;
				memset (ifName, 0, (MAX_IFNAME_LEN + 1));
				memset (macAddr, 0, MAC_ADDR_LEN);
				state = 0;
				
                if(rta_tb[NDA_DST]){
					ipAddr = *((unsigned int *)RTA_DATA(rta_tb[NDA_DST]));
                }
				if(ndm->ndm_ifindex){
					if_indextoname(ndm->ndm_ifindex, ifName);
				}
				else{
					strcpy (ifName, "null");
				}
				if(rta_tb[NDA_LLADDR]){
					memcpy(macAddr, RTA_DATA(rta_tb[NDA_LLADDR]), MAC_ADDR_LEN);
				}
				if(ndm->ndm_state){
					state = ndm->ndm_state;
				}
				if(ARP_FILTER_MAC == filterType){
					if(memcmp(macAddr, f_macAddr, MAC_ADDR_LEN)){
						goto skip_it;
					}
				}
				else if(ARP_FILTER_NONE != filterType){
					if((ARP_FILTER_IP == filterType)&&(ipAddr != filter)){
						goto skip_it;
					}
					else if((ARP_FILTER_IFINDEX == filterType)&&(filter != ndm->ndm_ifindex)){
						goto skip_it;
					}
					else if((ARP_FILTER_STATE == filterType)&&(!(filter&state))){
						goto skip_it;
					}
				}
					
				
				dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);
				
				dbus_message_iter_append_basic(&iter_struct,
									   DBUS_TYPE_UINT32,
									   &signal);
				
				dbus_message_iter_append_basic(&iter_struct,
									   DBUS_TYPE_UINT32,
									   &ipAddr);
				
				dbus_message_iter_append_basic(&iter_struct,
									   DBUS_TYPE_STRING,
									   &ifName);
				
				dbus_message_iter_append_basic(&iter_struct,
									   DBUS_TYPE_BYTE,
									   &macAddr[0]);
				
				dbus_message_iter_append_basic(&iter_struct,
									   DBUS_TYPE_BYTE,
									   &macAddr[1]);
				
				dbus_message_iter_append_basic(&iter_struct,
									   DBUS_TYPE_BYTE,
									   &macAddr[2]);
				
				dbus_message_iter_append_basic(&iter_struct,
									   DBUS_TYPE_BYTE,
									   &macAddr[3]);
				
				dbus_message_iter_append_basic(&iter_struct,
									   DBUS_TYPE_BYTE,
									   &macAddr[4]);
				
				dbus_message_iter_append_basic(&iter_struct,
									   DBUS_TYPE_BYTE,
									   &macAddr[5]);
				
				dbus_message_iter_append_basic(&iter_struct,
									   DBUS_TYPE_UINT32,
									   &state);
				
				dbus_message_iter_close_container (&iter_array, &iter_struct);
				
				npd_syslog_dbg("get ip neigh item: ip %#x ifname %s mac %.2x:%.2x:%.2x:%.2x:%.2x:%.2x state %d\n",\
					ipAddr,ifName,macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],state);
skip_it:
				h = NLMSG_NEXT(h, status);
			}
			if (msgh.msg_flags & MSG_TRUNC) {
				continue;
			}
			if (status) {
				ret = ARP_RETURN_CODE_ERROR;
				goto end;
			}
		}

		

        ret = ARP_RETURN_CODE_SUCCESS;

end:
	
		close(fd);
		
end_without_close_fd:
	
		signal = FALSE;


		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		
		dbus_message_iter_append_basic(&iter_struct,
							   DBUS_TYPE_UINT32,
							   &signal);
		
		dbus_message_iter_append_basic(&iter_struct,
							   DBUS_TYPE_UINT32,
							   &ipAddr);
		
		dbus_message_iter_append_basic(&iter_struct,
							   DBUS_TYPE_STRING,
							   &ifName);
		
		dbus_message_iter_append_basic(&iter_struct,
							   DBUS_TYPE_BYTE,
							   &macAddr[0]);
		
		dbus_message_iter_append_basic(&iter_struct,
							   DBUS_TYPE_BYTE,
							   &macAddr[1]);
		
		dbus_message_iter_append_basic(&iter_struct,
							   DBUS_TYPE_BYTE,
							   &macAddr[2]);
		
		dbus_message_iter_append_basic(&iter_struct,
							   DBUS_TYPE_BYTE,
							   &macAddr[3]);
		
		dbus_message_iter_append_basic(&iter_struct,
							   DBUS_TYPE_BYTE,
							   &macAddr[4]);
		
		dbus_message_iter_append_basic(&iter_struct,
							   DBUS_TYPE_BYTE,
							   &macAddr[5]);
		
		dbus_message_iter_append_basic(&iter_struct,
							   DBUS_TYPE_UINT32,
							   &state);
		
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		
		dbus_message_iter_close_container (&iter, &iter_array);
		
		free(ifName);	
		

		dbus_message_iter_append_basic(&iter,
									   DBUS_TYPE_UINT32,
									   &ret);
		return reply;		

	}


#ifdef __cplusplus
}
#endif

