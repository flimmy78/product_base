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
* npd_fdb.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for FDB module.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.98 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_log.h"
#include "npd_product.h"
#include "npd_eth_port.h"
#include "npd_c_slot.h"
#include "npd_vlan.h"
#include "npd_fdb.h"
#include "npd_trunk.h"
#include "npd_hash.h"
#include "sysdef/returncode.h"

struct fdb_entry_node_s **g_static_fdb;
struct fdb_blacklist_node_s **g_blacklist_fdb;

unsigned int g_static_fdb_count = 0;
unsigned int g_blacklist_fdb_count = 0;
unsigned int g_entry_fdb_count = 0;
struct Hash  *fdb_s = NULL;/*the fdb database hash table in sw*/
pthread_mutex_t FdbOpMutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned int g_agingtime = 300;
#define FDB_HASH_KEY_INDEX HASH_KEY_KEY1


unsigned int npd_fdb_static_get_count()
{
   return g_static_fdb_count;
}
struct fdb_entry_node_s ** npd_fdb_get_entry()
{
	return g_static_fdb;
}
struct fdb_blacklist_node_s ** npd_fdb_get_blacklist()
{
   return g_blacklist_fdb;
}
/**********************************************************************************
 * npd_fdb_static_init
 *
 * static FDB database initialization, maximum static fdb entry count is 1K
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
void npd_fdb_static_init() {

	npd_syslog_dbg("init static fdb database...\n");

	g_static_fdb = (void*)malloc(sizeof(void*)*AX_STATIC_FDB_ENTRY_SIZE);
	if(NULL == g_static_fdb) {
		npd_syslog_err("npd out of memory error in g_static_fdb malloc\n");
	}
	else{
		memset(g_static_fdb,0,(sizeof(void*)*AX_STATIC_FDB_ENTRY_SIZE));
	}
}
void npd_fdb_blacklist_init() {
	if(PRODUCT_ID_AX5K_E == PRODUCT_ID){/*5612e not support*/
		return;
	}

	npd_syslog_dbg("init blacklist fdb database...\n");

	g_blacklist_fdb = (void*)malloc(sizeof(void*)*AX_STATIC_FDB_ENTRY_SIZE);
	if(NULL == g_blacklist_fdb) {
		npd_syslog_err("npd out of memory error in g_blacklist_fdb malloc\n");
	}
	else{
		memset(g_blacklist_fdb,0,(sizeof(void*)*AX_STATIC_FDB_ENTRY_SIZE));
	}
}

#if 1
/*hash index for the fdb limit in sw*/


/**********************************************************************************
 * npd_fdb_compare
 *
 * compare two of fdb database(Hash table) items
 *
 *	INPUT:
 *		itemA	- fdb database item
 *		itemB	- fdb database item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 	- if two items are not equal.
 *		1 	- if two items are equal.
 *   NOTE:
 *           This fun compare by mac & vlan 
 *
 **********************************************************************************/
int npd_fdb_compare
(
	struct fdb_item_s *itemA,
	struct fdb_item_s *itemB
)
{
	int equal = 1;

	if((NULL==itemA)||(NULL==itemB)) {
		npd_syslog_err("npd fdb items compare null pointers error.");
		return NPD_FAIL;
	}

    if(0 != memcmp((char*)itemA->mac,(char*)itemB->mac,MAC_ADDRESS_LEN)) { /* MAC*/
		equal = 0;
	}
	else if(itemA->vid == itemB->vid){
        equal = 1;
	}

	return equal;
}


/**********************************************************************************
 * npd_fdb_index_get
 *
 * compare two of fdb database(Hash table) items
 *
 *	INPUT:
 *		item	- fdb database item
 *
 *	OUTPUT:
 *
 * 	RETURN:
 *          unsigned int 32 index --hash index ,<FDB_HASH_INDEX_SIZE
 *
 **********************************************************************************/
unsigned int npd_fdb_index_get
(
	struct fdb_item_s *item

)
{
	ETHERADDR *addrPtr = NULL;
	unsigned short vid = 0;
	unsigned int hashIndex = 0,i = 0;
	unsigned long ret = 0;
    /*
       npd_syslog_dbg("The info get:mac %02x:%02x:%02x:%02x:%02x:%02x,vid %d\n",   \
				  item->mac[0],item->mac[1],item->mac[2],item->mac[3],item->mac[4],item->mac[5],   \
				 item->vid);
      */
    if(NULL == item){
        return -NPD_ERR;
	}
    addrPtr = (ETHERADDR *)malloc(sizeof(ETHERADDR ));
    if(NULL == addrPtr){
      npd_syslog_err("Malloc for fdb mac failed!\n");
	  return -NPD_ERR; /*consider the return value*/
    }
    else{
   	  memset(addrPtr,0,sizeof(ETHERADDR)); 
	  memcpy(addrPtr->arEther,item->mac,MAC_ADDRESS_LEN);
	  vid = item->vid;
  	  ret = nam_fdb_hash_index(addrPtr,vid,&hashIndex);
	/*  
	  npd_syslog_dbg("addrPtr mac: %02x:%02x:%02x:%02x:%02x:%02x, vid %d,hashIndex %d\n",   \
	  	addrPtr->arEther[0],addrPtr->arEther[1],addrPtr->arEther[2],addrPtr->arEther[3], \
	  	addrPtr->arEther[4],addrPtr->arEther[5],vid,hashIndex);
      */
	  free(addrPtr);
	  addrPtr = NULL;
  	  return hashIndex ; 
   }
}

/**************************************************************
*
npd_fdb_init_hash
*  init the fdb hash table ,max for 4096 index
* INPUT
*        NONE
* OUTPUT
*        NONE
*
*RETURN
*      NULL
*
**************************************************************/
void npd_fdb_init_hash(
   void
)
{
	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608 == PRODUCT_ID){/*5612e not support*/
	/*	return;
	}*/

	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb initial!\n");
		return;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb initial!\n");
		return;
	}
	else {
	   npd_syslog_dbg("init fdb hash index\n");
	   fdb_s = hash_new(FDB_HASH_INDEX_SIZE);
	   fdb_s->hash_cmp = npd_fdb_compare;/*compared by mac and vlan*/
	   fdb_s->hash_key = npd_fdb_index_get;/*not only key generate ,by return the hash index generated by mac & vlan */
	   fdb_s->hash_key2 = NULL;
	}
}

void npd_fdb_hash_show_items
(	
   struct fdb_item_s *item,
   unsigned char withTitle
)
{
   npd_syslog_dbg("mac %02x:%02x:%02x:%02x:%02x:%02x,vid %d\n",   \
	  	   item->mac[0],item->mac[1],item->mac[2],item->mac[3],item->mac[4],item->mac[5],   \
	  	   item->vid);
}
void npd_fdb_hash_show_table
(
     void
)
{
    npd_syslog_dbg("***********SHOW THE HASH TABLE START*********");
    hash_show(fdb_s,npd_fdb_hash_show_items);
	npd_syslog_dbg("***********SHOW THE HASH TABLE END *********");
}
/**************************************************
*npd_fdb_item_search
*INPUT:
*    struct fdb_item_s *item
*
*OUTPUT:
*    1--if the item exist already
*
*    0--if the item not exist
*
*****************************************************/
int npd_fdb_item_search
(
	struct fdb_item_s *item
)
{
    struct fdb_item_s *data = NULL;
	/*
	npd_syslog_dbg("The item want to search is %02x:%02x:%02x:%02x:%02x:%02x,vid %d\n",
		          item->mac[0],item->mac[1],item->mac[2],item->mac[3],item->mac[4],item->mac[5],item->vid);
      */
    if((NULL != fdb_s)&&(NULL != item)){
	    data = hash_search(fdb_s,item,NULL,FDB_HASH_KEY_INDEX);
		if(NULL != data){/*the item has exist already*/
	        return NPD_TRUE;
		}
    }
	return NPD_FALSE;
}

/* fdb hash table update*/

int npd_fdb_op_item
(
	struct fdb_item_s *item,
	enum NPD_FDB_HASH_ACTION action
)
{
	struct fdb_item_s *data = NULL, *targetfdb = NULL;
	unsigned int count = 0;
	int ret = NPD_FDB_ERR_NONE;
	
	if((NULL == fdb_s)||(NULL == item)) {
		npd_syslog_err("NULL Ptr input!\n");
		return 	FDB_RETURN_CODE_GENERAL;
	}
	data =(struct fdb_item_s *)hash_search(fdb_s,item,NULL,FDB_HASH_KEY_INDEX);

	if(FDB_HASH_ADD_ITEM==action) {
		/* TODO:duplicated item may be updated !!!*/
		if(NULL != data) {
			if(NPD_TRUE == data->isStatic) {
				/*static fdb don't update*/
				npd_syslog_dbg("npd duplicated static fdb item found when add.");
				return FDB_RETURN_CODE_GENERAL;
			}
			memcpy(data, item, sizeof(struct fdb_item_s));
			return FDB_RETURN_CODE_NODE_EXIST;
		}
		#if 0
		/*if full,just occupy the item*/
		count = hash_count(fdb_s);
		if((FDB_HASH_INDEX_SIZE*4 < count ) && (FDB_HASH_ADD_ITEM == action)) {
	        npd_syslog_err("The hash table has full!\n");
			return FDB_RETURN_CODE_GENERAL;
		}
		#endif
		data = (struct fdb_item_s *) malloc(sizeof(struct fdb_item_s));
		if(NULL == data) {
			npd_syslog_err("npd add fdb alloc memory null!");
			return FDB_RETURN_CODE_GENERAL;
		}
		memset(data,0,sizeof(struct fdb_item_s));
		memcpy(data,item,sizeof(struct fdb_item_s));
		hash_push(fdb_s,data); /*insert into hash table*/
	}
	else if(FDB_HASH_DEL_ITEM==action) {
		if(NULL == data) {
			npd_syslog_dbg("npd fdb no item found when delete.");
			return FDB_RETURN_CODE_NODE_NOT_EXIST;/*if not found ,do nothing*/
		}
		if(NPD_TRUE == data->isStatic) {
			/*static fdb don't update*/
			npd_syslog_err("npd duplicated static arp item found when add.");
			return FDB_RETURN_CODE_GENERAL;
		}
		targetfdb = hash_pull(fdb_s,data); /* pull out from hash table*/
		if(targetfdb) {
			free(targetfdb);	/* release memory*/
			targetfdb = NULL;
		}
	}
	/*npd_fdb_hash_show_table();*/
	return NPD_FDB_ERR_NONE;	

}
int npd_fdb_filter_byPort
(
    struct fdb_item_s *itemA,
    struct fdb_item_s *itemB
)
{
    int ret = 0;
    if((NULL == itemA)||(NULL == itemB)) {
		return 0;
	}
	else if(itemA->dev != itemB->dev){
        return 0;
	}
	else if(itemA->port != itemB->port){
        return 0;
	}
	else{
        return 1;
	}
}
int npd_fdb_filter_byVlan
(
    struct fdb_item_s *itemA,
    struct fdb_item_s *itemB
)
{
    int ret = 0;
    if((NULL == itemA)||(NULL == itemB)) {
		return 0;
	}
	else if(itemA->vid != itemB->vid){
        return 0;
	}
	else{
        return 1;
	}
}
int npd_fdb_filter_byVlanPort
(
    struct fdb_item_s *itemA,
    struct fdb_item_s *itemB
)
{
    int ret = 0;
    if((NULL == itemA)||(NULL == itemB)) {
		return 0;
	}
	else if(itemA->vid != itemB->vid){
        return 0;
	}
	else if(itemA->dev != itemB->dev){
        return 0;
	}
	else if(itemA->port != itemB->port){
        return 0;
	}
	else{
        return 1;
	}
}
int npd_fdb_hash_process
(
     struct fdb_item_s *itemA,
     enum NPD_FDB_HASH_ACTION action
)
{
      #if 0
      npd_syslog_dbg("item: vid %d,mac %02x:%02x:%02x:%02x:%02x:%02x dev %d,port %d,aciton %d\n",itemA->vid,itemA->mac[0],\
	  	itemA->mac[1],itemA->mac[2],itemA->mac[3],itemA->mac[4],itemA->mac[5],itemA->dev,itemA->port,action);
      #endif	  
	  return  npd_fdb_op_item(itemA,action);
	 
}
int npd_fdb_hash_traversal
(
   struct Hash * hash,
   unsigned int flag,
   void * data,
   unsigned int(* filter)(void *,void *),
   int(* processor)(void *,unsigned int)  
)
{
    unsigned ret = 0;
	pthread_mutex_lock(&FdbOpMutex);
    ret =  hash_traversal(hash,flag,data,filter, processor);
	pthread_mutex_unlock(&FdbOpMutex);
	return ret;
}
#endif
 
unsigned int npd_fdb_number_port_set_check 
(
    unsigned char slotNum,
    unsigned char portNum,
    unsigned int* number
)

{
	unsigned int eth_g_index = 0;
	struct eth_port_s *portInfo = NULL;
	struct eth_port_fdb_s* fdbNode = NULL;

	if (!CHASSIS_SLOTNO_ISLEGAL(slotNum)) {
         return FDB_BOOL_FALSE;
	}
	else if (!ETH_LOCAL_PORTNO_ISLEGAL( slotNum,portNum)) {		
		return FDB_BOOL_FALSE;
    }
	eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotNum,portNum);

	/* when a port add to fdb limit ,do not care the L3
	ret = npd_check_port_switch_mode(eth_g_index);
	if(!(ret < 0) && (ret != NPD_TRUE)){
        syslog_ax_fdb_err("fdb limit found port %d/%d is not in switch mode\n",slotNum,portNum);
		return FDB_BOOL_FALSE;
	}
      */
	portInfo = npd_get_port_by_index(eth_g_index);
	/*code optimize:  Dereference null return value
	zhangcl@autelan.com 2013-1-17*/
	if(portInfo == NULL)
	{
		return FDB_BOOL_FALSE;
	}

	 
	fdbNode = portInfo->funcs.func_data[ETH_PORT_FUNC_FDB];
	if(NULL == fdbNode){
	   return FDB_BOOL_FALSE;
	}
	*number = fdbNode->fdbLimit;
	
    if(*number > 0){
      /* npd_syslog_dbg("The port-based FDB protected has been set: slotNum: %d,portNum: %d, fdbLimit: %d,fdbCount: %d  \n ",slotNum,portNum,*number,switchNode->fdbCount);*/
	   return FDB_BOOL_TRUE;
	}
    else{
       /*npd_syslog_dbg("The port-based FDB protected has not been set: slotNum: %d,portNum: %d, fdbnumber: %d,fdbCount: %d \n ",slotNum,portNum,*number,switchNode->fdbCount);*/
	   return FDB_BOOL_FALSE;
	}

	return FDB_BOOL_FALSE;

}

struct eth_port_fdb_s* npd_fdb_port_limit_init
(
    unsigned int eth_g_index
)
{
    struct eth_port_fdb_s * fdbNode = NULL;
	struct eth_port_s *eth_ports = NULL;
	unsigned int slot_index = 0,eth_local_index = 0;

	eth_ports = g_eth_ports[eth_g_index];
	if(NULL == eth_ports){
	   npd_syslog_warning("The %d portinfo does not exist,create it!\n",eth_g_index);
	   slot_index = npd_get_slot_index_from_eth_index(eth_g_index);
	   eth_local_index = npd_get_port_index_from_eth_index(eth_g_index);
       npd_create_eth_port( slot_index, eth_local_index);
	   eth_ports = g_eth_ports[eth_g_index];
	}
	
    fdbNode = (struct eth_port_fdb_s *)malloc(sizeof(struct eth_port_fdb_s));
	if(NULL == fdbNode){
		syslog_ax_eth_port_dbg("init eth-port %d struct eth_port_fdb_s memory malloc error!\n",eth_g_index);
		return NULL;
	}
	/*init the fdb node*/
	memset(fdbNode,0,sizeof(struct eth_port_fdb_s));
	
	eth_ports->funcs.func_data[ETH_PORT_FUNC_FDB] = fdbNode;
	eth_ports->funcs.funcs_run_bitmap |= (1 << ETH_PORT_FUNC_FDB);

	return fdbNode;
}

unsigned int npd_fdb_number_port_set
(
    unsigned char slotNum,
    unsigned char portNum,
    unsigned int number
)
{
	unsigned int eth_g_index = 0;
	struct eth_port_s *portInfo = NULL;
	struct eth_port_fdb_s* fdbNode = NULL;
	
	/*struct fdb_number_port_s * node = NULL;*/
    if (!CHASSIS_SLOTNO_ISLEGAL(slotNum)) {

		 return FDB_BOOL_FALSE;
	}
	
	if (!ETH_LOCAL_PORTNO_ISLEGAL( slotNum,portNum)) {
			
		return FDB_BOOL_FALSE;
    }
	
    eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotNum,portNum);

	portInfo = npd_get_port_by_index(eth_g_index);
	
	if(NULL == portInfo){
       npd_syslog_err("The portINfo node is NULL!\n");
	   return FDB_BOOL_FALSE;
	}
	else {
       fdbNode = portInfo->funcs.func_data[ETH_PORT_FUNC_FDB];
	   if(NULL == fdbNode){
         fdbNode = npd_fdb_port_limit_init(eth_g_index);
	   }
	}
	if(number > 0){
		fdbNode->fdbLimit= number;
		fdbNode->fdbCount = 0;
	}
	/*npd_fdb_reset_fdbCount_by_port(slotNum,portNum);*/
	/*printf("swithNode->fdbLimit: %d,swithNode->fdbCount: %d\n",switchNode->fdbLimit,switchNode->fdbCount);*/

	if( 0 == number){
		/*cancel the fdb limit with port, reset the ETH_PORT_FUNC_FDB*/
		#if 0
		free(fdbNode);
		fdbNode = NULL;
		portInfo->funcs.func_data[ETH_PORT_FUNC_FDB] = NULL;
		portInfo->funcs.funcs_run_bitmap &=~(1 << ETH_PORT_FUNC_FDB);
		#endif
		fdbNode->fdbLimit= number;
		fdbNode->fdbCount = 0;
		
	}

	return FDB_BOOL_TRUE;
}

unsigned int npd_fdb_reset_fdbCount_by_port
(
    unsigned int slot_no,
    unsigned int port_no
)
{ 
    unsigned int ret = 0;
	unsigned int eth_g_index = 0,vlanId = 0;
	unsigned char isTagged = 0;
	unsigned int numberport = 0,numbervlan = 0,numbervlanport = 0;
	struct eth_port_s *portInfo = NULL;
	struct eth_port_fdb_s* fdbNode = NULL;
	dot1q_vlan_list_s  *dot1qList = NULL;
	struct eth_port_dot1q_list_s *dot1qNode = NULL;
	struct list_head *list = NULL, *pos = NULL;
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *vlanportNode = NULL;
	struct port_based_vlan_s  *pvid = NULL;
	unsigned char dev = 0,port = 0;
	struct fdb_item_s item;
	memset(&item,0,sizeof(struct fdb_item_s));
	eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
	ret = npd_get_devport_by_global_index(eth_g_index,&dev,&port);
	if(0!= ret){
		return FDB_BOOL_FALSE;
	}
	item.dev = dev;
	item.port = port;
	ret = npd_fdb_hash_traversal(fdb_s,FDB_HASH_DEL_ITEM,&item,npd_fdb_filter_byPort,npd_fdb_hash_process);
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo){
        npd_syslog_err("The portInfo is Null\n");
		return FDB_BOOL_FALSE;
	}
	if(ret = npd_fdb_number_port_set_check(slot_no,port_no,&numberport)){
		fdbNode = portInfo->funcs.func_data[ETH_PORT_FUNC_FDB];
		if(NULL != fdbNode)
		   fdbNode->fdbCount = 0;
	}
	
	/*check and set vlan fdb limit ,vlan-port fdb limit--port tagged vlan*/
	dot1qList = portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN];
	if( NULL != dot1qList){
		list = &(dot1qList->list);
			__list_for_each(pos,list) {
				dot1qNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
				vlanId = dot1qNode->vid;
                vlanNode = npd_find_vlan_by_vid(vlanId);
				/*check if vlan fdb limit set */
				if(npd_fdb_number_vlan_set_check(vlanId,&numbervlan)){
                     vlanNode->brgVlan.fdbCount= 0;
					 
					/* ret = npd_fdb_limit_vlan_count_get(vlanId);*/
					 
				}
				/*check if vlan-port fdb limit set*/
				if(NPD_TRUE == npd_fdb_number_vlan_port_set_check(vlanId,slot_no,port_no,&numbervlanport)){
					ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&isTagged);
					if(NPD_TRUE != ret){
						syslog_ax_fdb_err("fdb:port %d/%d is not in vlan %d\n",slot_no,port_no,vlanId);
						 return ret;
					}
					if(NPD_TRUE == isTagged) {
						portList = vlanNode->tagPortList;
					}
					else if(NPD_FALSE == isTagged) {
						portList = vlanNode->untagPortList;
					}
					if(NULL == portList) {
						return NPD_FALSE;
					}
					else if(0 != portList->count) {
						/* syslog_ax_vlan_dbg(("npd vlan check port:: vlan %d %s port count %d",vlanId,isTagged ? "tagged":"untagged",portList->count));*/
						list = &(portList->list);
						__list_for_each(pos,list) {
							vlanportNode = list_entry(pos,struct vlan_port_list_node_s,list);
							if(eth_g_index == vlanportNode->eth_global_index) {
								vlanportNode->fdbCount = 0;
								
								/*ret = npd_fdb_limit_vlan_port_count_get(vlanId,eth_g_index);*/
					            
							}
						}
					}
                }
			}
	   }
       else{
          ; /*npd_syslog_dbg("The port has no tagged vlan info\n");*/
	   }

	/*check vlan-base,vlan-port-based fdb limit count,port untagged vlan*/	
		pvid = portInfo->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN];
		if(NULL != pvid) {
			vlanId = pvid->vid;
			vlanNode = npd_find_vlan_by_vid(vlanId);
			/*check if vlan fdb limit set */
			if(npd_fdb_number_vlan_set_check(vlanId,&numbervlan)){
                 vlanNode->brgVlan.fdbCount= 0;
				 ret = npd_fdb_limit_vlan_count_get(vlanId);
			}
			if(npd_fdb_number_vlan_port_set_check(vlanId,slot_no,port_no,&numbervlanport)){
				ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&isTagged);
				if(NPD_TRUE != ret){
					 return ret;
				}
				if(NPD_TRUE == isTagged) {
					portList = vlanNode->tagPortList;
				}
				else if(NPD_FALSE == isTagged) {
					portList = vlanNode->untagPortList;
				}
				if(NULL == portList) {
					return NPD_FALSE;
				}
				else if(0 != portList->count) {
					/* syslog_ax_vlan_dbg(("npd vlan check port:: vlan %d %s port count %d",vlanId,isTagged ? "tagged":"untagged",portList->count));*/
					list = &(portList->list);
					__list_for_each(pos,list) {
						vlanportNode = list_entry(pos,struct vlan_port_list_node_s,list);
						if(eth_g_index == vlanportNode->eth_global_index) {
							vlanportNode->fdbCount = 0;							
							/*ret = npd_fdb_limit_vlan_port_count_get(vlanId,eth_g_index);*/
				             
						}
					}
				}
			}
		}
		else {
			;/*printf("The eth port has no untagged vlan\n");*/
		}
	return FDB_BOOL_TRUE;
	 
}
unsigned int npd_fdb_number_vlan_set
(
    unsigned short vlanId,
    unsigned int number
)
{
	unsigned int ret = 0;
	struct vlan_s *vlanNode = NULL;

	ret = npd_check_vlan_exist(vlanId);
	if(VLAN_RETURN_CODE_VLAN_EXISTS!= ret){/*NPD_VLAN_EXIST == 13*/
	   npd_syslog_err("fdb:The vlan %d input does not exist \n",vlanId);
	   return FDB_RETURN_CODE_GENERAL;
	}

	vlanNode = npd_find_vlan_by_vid(vlanId);

	vlanNode->brgVlan.fdbLimit = number;
	vlanNode->brgVlan.fdbCount = 0;  

	/*npd_fdb_reset_fdbCount_by_vlan(vlanId);*/
	return NPD_FDB_ERR_NONE;
}

unsigned int npd_fdb_number_vlan_set_check 
(
    unsigned short vlanId,
    unsigned int *number
)

{
	unsigned int ret = 0;
    struct vlan_s *vlanNode = NULL;
	
	ret = npd_check_vlan_exist(vlanId);
	if(VLAN_RETURN_CODE_VLAN_EXISTS != ret){/*NPD_VLAN_EXISTS == 13*/
	   return FDB_BOOL_FALSE;
	}


	vlanNode = npd_find_vlan_by_vid(vlanId);

	if(((NULL == vlanNode->tagPortList)||(0 == vlanNode->tagPortList->count))&&(((NULL == vlanNode->untagPortList)||(0 == vlanNode->untagPortList->count)))){
		return FDB_BOOL_FALSE;/*if no port  member in vlan*/
	}
	*number = vlanNode->brgVlan.fdbLimit;
	
	if(*number > 0){
       /*syslog_ax_fdb_err("vlan %d fdb limited to %d\n ",vlanId,*number);*/
	   return FDB_BOOL_TRUE;
	}
    else{
       /*npd_syslog_dbg("The vlan-based FDB protected has not been set: vlan: %d, number: %d \n ",vlanId,*number);*/
	   return FDB_BOOL_FALSE;
	}
}

unsigned int npd_fdb_reset_fdbCount_by_vlan
(
    unsigned short vlanId  
)
{
    unsigned int numberport = 0,numbervlanport = 0;
	unsigned int i=0;
	unsigned int ret = 0;
	unsigned int isTagged = 0,mbr_count = 0;
	unsigned int slot_index = 0,slot_no = 0,port_index = 0,port_no = 0;
	unsigned int eth_g_index[64] = {0};
	struct eth_port_s *portInfo = NULL;
	struct eth_port_fdb_s* fdbNode = NULL;
	struct vlan_s *vlanNode = NULL;
	struct vlan_port_list_node_s *node = NULL;
	vlan_port_list_s *portList = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	
	struct fdb_item_s item;
	memset(&item,0,sizeof(struct fdb_item_s));
	item.vid = vlanId;
	npd_fdb_hash_traversal(fdb_s,FDB_HASH_DEL_ITEM,&item,npd_fdb_filter_byVlan,npd_fdb_hash_process);

	ret = npd_check_vlan_exist(vlanId);
	if(VLAN_RETURN_CODE_VLAN_EXISTS!= ret){/*NPD_VLAN_EXIST == 13*/
		syslog_ax_fdb_err("vlan %d not exist\n",vlanId);
	   return NPD_VLAN_NOTEXISTS;
	}

	vlanNode = npd_find_vlan_by_vid(vlanId);

	vlanNode->brgVlan.fdbCount = 0;  

	/*set the port untagged belonging to the vlan fdb limit active number to 0*/
	ret = npd_vlan_member_port_index_get( vlanId,isTagged,eth_g_index,&mbr_count);
	     if(NPD_TRUE == ret){ /* if contains ports*/
	 	    for(i=0;i<mbr_count;i++){
				slot_index = npd_get_slot_index_from_eth_index(eth_g_index[i]);
				slot_no = npd_get_slot_no_from_index(slot_index);
				port_index = npd_get_port_index_from_eth_index(eth_g_index[i]);
				port_no = npd_get_port_no_from_index(slot_index,port_index);
				/*check port set*/
                ret = npd_fdb_number_port_set_check(slot_no,port_no,&numberport);
                if(1 == ret){
					 portInfo = npd_get_port_by_index(eth_g_index[i]);
					 if(NULL != portInfo){
						 fdbNode = portInfo->funcs.func_data[ETH_PORT_FUNC_FDB];
                         if(NULL != fdbNode)
						    fdbNode->fdbCount = 0;
					 }

                }
				/*check vlan-port set*/ 
				ret = npd_fdb_number_vlan_port_set_check(vlanId,slot_no,port_no,&numbervlanport);
				if( 1 == ret){
					portList = vlanNode->untagPortList;
					if((NULL != portList)&&(0 != portList->count)) {
						/*syslog_ax_vlan_dbg("npd vlan check port:: vlan %d %s port count %d",vlanId,isTagged ? "tagged":"untagged",portList->count);*/
						list = &(portList->list);
						__list_for_each(pos,list) {
							node = list_entry(pos,struct vlan_port_list_node_s,list);
							if(eth_g_index[i] == node->eth_global_index) {
								node->fdbCount = 0;
							}
						}
					}
				}
	 	    }
	    }
		 
	isTagged = 1;
	/*set the port tagged belonging to the vlan fdb limit active number to 0*/
	ret = npd_vlan_member_port_index_get( vlanId,isTagged,eth_g_index,&mbr_count);
        if(NPD_TRUE == ret){ /* if contains ports*/
	 	    for(i=0;i<mbr_count;i++){
				slot_index = npd_get_slot_index_from_eth_index(eth_g_index[i]);
				slot_no = npd_get_slot_no_from_index(slot_index);
				port_index = npd_get_port_index_from_eth_index(eth_g_index[i]);
				port_no = npd_get_port_no_from_index(slot_index,port_index);
				/*check port set*/
                ret = npd_fdb_number_port_set_check(slot_no,port_no,&numberport);
                if(1 == ret){
					 portInfo = npd_get_port_by_index(eth_g_index[i]);
					 if(NULL != portInfo){
						 fdbNode = portInfo->funcs.func_data[ETH_PORT_FUNC_FDB];
                         if(NULL != fdbNode)
						    fdbNode->fdbCount = 0;
					 }
                }
			    /*check vlan-port set */
				ret = npd_fdb_number_vlan_port_set_check(vlanId,slot_no,port_no,&numbervlanport);
				if( 1 == ret){
					portList = vlanNode->untagPortList;
					if((NULL != portList)&&(0 != portList->count)) {
						list = &(portList->list);
						__list_for_each(pos,list) {
							node = list_entry(pos,struct vlan_port_list_node_s,list);
							if(eth_g_index[i] == node->eth_global_index) {
								node->fdbCount = 0;
							}
						}
					}
				}
	 	    }
	   }
	
	return NPD_TRUE;
}

unsigned int npd_fdb_number_vlan_port_set
(
    unsigned short vlanid,
    unsigned char  slotno,
    unsigned char  portno,
    unsigned int  fdblimit
)
{   
    unsigned int ret = 0;
	unsigned int eth_g_index = 0;
	unsigned char isTagged = 0;
	unsigned int  numbervlan = 0;
	unsigned int  numberport = 0;
	struct eth_port_s *portInfo = NULL;
	struct eth_port_fdb_s* fdbNode = NULL;
    struct vlan_port_list_node_s *node = NULL;
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct list_head  *list = NULL,*pos = NULL;

    /*check vlan exist*/
	ret = npd_check_vlan_exist(vlanid);
	if(VLAN_RETURN_CODE_VLAN_EXISTS != ret){
	   npd_syslog_err("The vlan %d input does not exist \n",vlanid);
	   return NPD_VLAN_NOTEXISTS;
	}

	/*check port exist*/
	if (CHASSIS_SLOTNO_ISLEGAL(slotno)) {
	    if (ETH_LOCAL_PORTNO_ISLEGAL(slotno,portno)) {
		    eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,portno);
		
	    }
		else{
	        return  NPD_FAIL;
        }
	}
	else {
	    return  NPD_FAIL;
	}
	/*check if contains*/
	ret = npd_vlan_check_contain_port(vlanid,eth_g_index,&isTagged);
	if(NPD_TRUE != ret){
	    return ret;
	}

	vlanNode = npd_find_vlan_by_vid(vlanid);    
	if(NULL == vlanNode) {
		return NPD_FALSE;
	}

	/* tagged port list*/
	if(NPD_TRUE == isTagged) {
		portList = vlanNode->tagPortList;
	}
	else if(NPD_FALSE == isTagged) {
		portList = vlanNode->untagPortList;
	}

	if(NULL == portList) {
		return NPD_FALSE;
	}
	else if(0 != portList->count) {
		/* syslog_ax_vlan_dbg(("npd vlan check port:: vlan %d %s port count %d",vlanId,isTagged ? "tagged":"untagged",portList->count));*/
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if(eth_g_index == node->eth_global_index) {
				node->fdbLimit= fdblimit;
				node->fdbCount = 0;
			}
		}
	}
	/*set the vlan fdb limit active number to 0*/
    ret = npd_fdb_number_vlan_set_check(vlanid,&numbervlan);
	if( 1 == ret){
		vlanNode = npd_find_vlan_by_vid(vlanid);
		vlanNode->brgVlan.fdbCount = 0;
	}
	/*set the port fdb limit active number to 0*/
	ret = npd_fdb_number_port_set_check(slotno,portno,&numberport);
	if( 1 == ret){
		eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,portno);
		portInfo = npd_get_port_by_index(eth_g_index);
		if(NULL != portInfo){
			fdbNode = portInfo->funcs.func_data[ETH_PORT_FUNC_FDB];
			if(NULL != fdbNode)
			   fdbNode->fdbCount = 0;
		}

	}
	return NPD_TRUE;
 }


unsigned int npd_fdb_number_vlan_port_set_check
(
    unsigned short vlanid,
    unsigned char  slotno,
    unsigned char  portno,
    unsigned int  *fdblimit
)
{   
    unsigned int ret = 0;
	unsigned int eth_g_index = 0;
	unsigned char isTagged = 0;
    struct vlan_port_list_node_s *node = NULL;
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct list_head  *list = NULL,*pos = NULL;

    /*check vlan exist*/
	ret = npd_check_vlan_exist(vlanid);
	if( VLAN_RETURN_CODE_VLAN_EXISTS != ret){/*NPD_VLAN_EXIST == 13*/
	   return NPD_VLAN_NOTEXISTS;
	}

	/*check port exist*/
	if (CHASSIS_SLOTNO_ISLEGAL(slotno)) {
	    if (ETH_LOCAL_PORTNO_ISLEGAL(slotno,portno)) {
		    eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,portno);
		
	    }
		else{
		    return FDB_RETURN_CODE_NODE_NOT_EXIST;	
		}

    }
	else {
	     return FDB_RETURN_CODE_NODE_NOT_EXIST;
	}
	
	/*check if contains*/
	ret = npd_vlan_check_contain_port(vlanid, eth_g_index,&isTagged);
	if(NPD_TRUE != ret){
	    return ret;
	}

	vlanNode = npd_find_vlan_by_vid(vlanid);
	if(NULL == vlanNode) {
		return NPD_FALSE;
	}

	/* tagged port list*/
	if(NPD_TRUE == isTagged) {
		portList = vlanNode->tagPortList;
	}
	else if(NPD_FALSE == isTagged) {
		portList = vlanNode->untagPortList;
	}

	if(NULL == portList) {
		return NPD_FALSE;
	}
	else if(0 != portList->count) {
		/*syslog_ax_vlan_dbg("npd vlan check port:: vlan %d %s port count %d",vlanid,isTagged ? "tagged":"untagged",portList->count);*/
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if(eth_g_index == node->eth_global_index) {
				*fdblimit = node->fdbLimit;
				if(0 != node->fdbLimit)
				    return NPD_TRUE;
				else{
					/*printf("The number has not been set\n");*/
					return NPD_FALSE;
				}
			}
		}
	}
   return NPD_FALSE;
	
}


unsigned int npd_fdb_limit_port_count_get
(
   unsigned int eth_g_index
)
{
   struct eth_port_s *portInfo = NULL;
   struct eth_port_fdb_s* fdbNode = NULL;	
   
   portInfo = npd_get_port_by_index(eth_g_index); 
   if(NULL != portInfo){
	 fdbNode = portInfo->funcs.func_data[ETH_PORT_FUNC_FDB];
	 if(NULL != fdbNode)
		 return fdbNode->fdbCount;
	 else {
	 	 npd_syslog_err("fdb:port %d no fdb limit!\n",eth_g_index);
         return 0;
	 }
  }
   return NPD_FALSE;

}
unsigned int npd_fdb_limit_port_count_add
(
   unsigned int eth_g_index
)
{
   struct eth_port_s *portInfo = NULL;
   struct eth_port_fdb_s* fdbNode = NULL;	
   
   portInfo = npd_get_port_by_index(eth_g_index); 
   if(NULL != portInfo){
	   fdbNode = portInfo->funcs.func_data[ETH_PORT_FUNC_FDB];
	   if(NULL != fdbNode){
		  fdbNode->fdbCount++;
		  return fdbNode->fdbCount;
	   }
	   else{
          npd_syslog_dbg("fdbNode NULL\n");
		  return 0;
	   }  
   }
   return NPD_FALSE;

}
unsigned int npd_fdb_limit_vlan_count_get
(
   unsigned short vlanId
)
{
   struct vlan_s *vlanNode = NULL;

   vlanNode = npd_find_vlan_by_vid(vlanId);
   if(NULL != vlanNode){
      return vlanNode->brgVlan.fdbCount;
   }
   return NPD_FALSE; 
}

unsigned int npd_fdb_limit_vlan_count_add
(
    unsigned short vlanId
)
{
    struct vlan_s *vlanNode = NULL;

   vlanNode = npd_find_vlan_by_vid(vlanId);
   if(NULL != vlanNode){
	   vlanNode->brgVlan.fdbCount++;
	   return vlanNode->brgVlan.fdbCount;
   }
   return 0;
}

unsigned int npd_fdb_limit_vlan_port_count_get
(
   unsigned short vlanId,
   unsigned int  eth_g_index
)
{
       unsigned int ret = 0;
	   unsigned char isTagged = 0;
	   struct vlan_s *vlanNode = NULL;
	   struct vlan_port_list_node_s *node = NULL;
	   vlan_port_list_s *portList = NULL;
	   struct list_head  *list = NULL,*pos = NULL;

       vlanNode = npd_find_vlan_by_vid(vlanId);
	   ret = npd_vlan_check_contain_port(vlanId, eth_g_index,&isTagged);
	   if(NPD_TRUE == isTagged) {
		  portList = vlanNode->tagPortList;
	   }
	   else if(NPD_FALSE == isTagged) {
		  portList = vlanNode->untagPortList;
	   }
       list = &(portList->list);
	   if(NULL != list){
		 __list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if(eth_g_index == node->eth_global_index) {
				return node->fdbCount;
			}
		}
	   }
       return 0;
}

unsigned int npd_fdb_limit_vlan_port_count_add
(
     unsigned short vlanId,
     unsigned int  eth_g_index
)
{
       
		unsigned int ret = 0;
		unsigned char isTagged = 0;
		struct vlan_s *vlanNode = NULL;
		struct vlan_port_list_node_s *node = NULL;
		vlan_port_list_s *portList = NULL;
		struct list_head  *list = NULL,*pos = NULL;

		vlanNode = npd_find_vlan_by_vid(vlanId);
		ret = npd_vlan_check_contain_port(vlanId, eth_g_index,&isTagged);
		if(NPD_TRUE == isTagged) {
		portList = vlanNode->tagPortList;
		}
		else if(NPD_FALSE == isTagged) {
		portList = vlanNode->untagPortList;
		}
		list = &(portList->list);
		if(NULL != list){
			__list_for_each(pos,list) {
				node = list_entry(pos,struct vlan_port_list_node_s,list);
				if(eth_g_index == node->eth_global_index) {
					node->fdbCount++;
					return node->fdbCount;
				}
			}
		}
        return 0;
}

unsigned int  npd_na_msg_read
(
    CPSS_MAC_ENTRY_EXT_STC *macEntry,
    FDB_LIMIT_TYPE         *fdb_limit_type
)
{   
    unsigned int rvp=0, rv = 0,rp = 0,ret = 0;
    ETHERADDR     *macPtr = NULL;
	unsigned int  slot_no = 0;
	unsigned int  slot_index = 0;
	unsigned int  port_no = 0;
	unsigned int  port_index = 0;
	unsigned int  eth_g_index = 0;
	unsigned int  numberVlanPort = 0, numberPort = 0,numberVlan = 0;
	unsigned int actNumVlanPort= 0;
	unsigned int actNumVlan = 0;
	unsigned int actNumPort = 0;
	unsigned int learn = 0,learncmd = 0;
    macPtr = &(macEntry->key.key.macVlan.macAddr);
	ret = npd_get_global_index_by_devport(macEntry->dstInterface.devPort.devNum,macEntry->dstInterface.devPort.portNum,&eth_g_index);
    if(0 != ret){
       return NPD_FAIL;
	}
	slot_index = npd_get_slot_index_from_eth_index(eth_g_index);
	slot_no = npd_get_slot_no_from_index(slot_index);
	port_index = npd_get_port_index_from_eth_index(eth_g_index);
		/*To CPU*/
	if(63 == port_index){
           return NPD_TRUE;
	}
	port_no = npd_get_port_no_from_index(slot_index,port_index);

	rvp = npd_fdb_number_vlan_port_set_check(macEntry->key.key.macVlan.vlanId,slot_no,port_no,&numberVlanPort);
	rv = npd_fdb_number_vlan_set_check(macEntry->key.key.macVlan.vlanId,&numberVlan);
	rp = npd_fdb_number_port_set_check(slot_no,port_no,&numberPort);
	if(1 == rvp){
		actNumVlanPort = npd_fdb_limit_vlan_port_count_get(macEntry->key.key.macVlan.vlanId,eth_g_index);
	}
	if(1 == rv){
		actNumVlan = npd_fdb_limit_vlan_count_get(macEntry->key.key.macVlan.vlanId);
	}
	if(1 == rp){
		actNumPort = npd_fdb_limit_port_count_get(eth_g_index);
	}
    
    if(1 ==rvp){
		if(( 1 == rv)&&(1 == rp)){
				if((actNumVlanPort<numberVlanPort)&&(actNumVlanPort < numberVlan)&&(actNumVlanPort < numberPort)
					               &&(actNumVlan < numberVlanPort)&&(actNumVlan < numberVlan)&&(actNumVlan < numberPort)
					               &&(actNumPort < numberVlanPort)&&(actNumPort < numberVlan)&&(actNumPort < numberPort)){
                     #if 0
					  npd_syslog_dbg("VLAN-PORT-BASED: number: %d,actNum: %d *** VLAN-BASED: numberVlan: %d,actNumVlan: %d *** PORT-BASED: numberPort: %d, actNumPort: %d\n",
					 	                             numberVlanPort,actNumVlanPort,numberVlan,actNumVlan,numberPort,actNumPort);
                      npd_syslog_dbg("READ FDB　ITEM VLAN-PORT-BASED|VLAN-BASED|PORT-BASED ENTRY:\n");
					  npd_syslog_dbg("-------------------------------------------------------------------------\n");
					  npd_syslog_dbg("%-17s %-4s %-6s %-4s %-5s %-5s\n","MAC Address","VLAN","DEVICE","SLOT","PORT","ROUTE");
					  npd_syslog_dbg("---- ----------------- ---- ------ ---- ----- ------- ------\n");
					  npd_syslog_dbg("%02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-4d %-5s\n", \
						   macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2],	\
						   macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5], \
						   macEntry->key.key.macVlan.vlanId, 	\
						   macEntry->dstInterface.devPort.devNum,	\
						   slot_no,port_no,\
						  (macEntry->daRoute == 1) ? "en":"dis");
			         #endif
					 fdb_limit_type->vlan_port_type = 1;
					 fdb_limit_type->port_type = 1;
					 fdb_limit_type->vlan_type = 1; 
					 return NPD_TRUE;
				}
				else {/*The number is full*/
                     return NPD_FALSE;
				}
		}
		else if(( 1 == rv)&&( 1 != rp)) {
             if((actNumVlanPort<numberVlanPort)&&(actNumVlanPort < numberVlan)
				               &&(actNumVlan < numberVlanPort)&&(actNumVlan < numberVlan)){
                     #if 0
				  npd_syslog_dbg("VLAN-PORT-BASED: number: %d,actNum: %d *** VLAN-BASED: numberVlan: %d,actNumVlan: %d \n",
				 	                             numberVlanPort,actNumVlanPort,numberVlan,actNumVlan);
                  npd_syslog_dbg("READ FDB　ITEM VLAN-PORT-BASED|VLAN-BASED ENTRY:\n");
				  npd_syslog_dbg("-------------------------------------------------------------------------\n");
				  npd_syslog_dbg("%-17s %-4s %-6s %-4s %-5s %-5s\n","MAC Address","VLAN","DEVICE","SLOT","PORT","ROUTE");
				  npd_syslog_dbg("---- ----------------- ---- ------ ---- ----- ------- ------\n");
				  npd_syslog_dbg("%02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-4d %-5s\n", \
					   macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2],	\
					   macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5], \
					   macEntry->key.key.macVlan.vlanId, 	\
					   macEntry->dstInterface.devPort.devNum,	\
					   slot_no,port_no,\
					   (macEntry->daRoute == 1) ? "en":"dis");
		         #endif
				 fdb_limit_type->vlan_port_type = 1;
				 fdb_limit_type->vlan_type = 1;
				 return NPD_TRUE;
			 }
			 else{
                 return NPD_FALSE;
			 }
		}
		else if((1 == rp)&&(1 != rv)){
			if((actNumVlanPort<numberVlanPort)&&(actNumVlanPort < numberPort)
				               &&(actNumPort < numberVlanPort)&&(actNumPort < numberPort)){
                     #if 0
				  npd_syslog_dbg("VLAN-PORT-BASED: number: %d,actNum: %d *** PORT-BASED: numberPort: %d,actNumPort: %d\n",numberVlanPort,actNumVlanPort,numberPort,actNumPort);
				  npd_syslog_dbg("READ FDB　ITEM VLAN-PORT-BASED|PORT-BASED ENTRY:\n");
				  npd_syslog_dbg("-------------------------------------------------------------------------\n");
				  npd_syslog_dbg("%-17s %-4s %-6s %-4s %-5s %-5s\n","MAC Address","VLAN","DEVICE","SLOT","PORT","ROUTE");
				  npd_syslog_dbg("---- ----------------- ---- ------ ---- ----- ------- ------\n");
				  npd_syslog_dbg("%02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-4d %-5s\n", \
						   macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2],	\
						  macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5], \
						  macEntry->key.key.macVlan.vlanId, 	\
						  macEntry->dstInterface.devPort.devNum,	\
						  slot_no,port_no,\
						  (macEntry->daRoute == 1) ? "en":"dis");
		         #endif
				 fdb_limit_type->vlan_port_type = 1;
				 fdb_limit_type->port_type = 1;
				 return NPD_TRUE;
		   } 
		   else {
	             return NPD_FALSE;
		   }
	   }
	   else {
           if(actNumVlanPort<numberVlanPort){
			   	     #if 0
				  npd_syslog_dbg("VLAN-PORT-BASED: number: %d,actNum: %d \n",numberVlanPort,actNumVlanPort);
				  npd_syslog_dbg("READ FDB　ITEM VLAN-PORT-BASED ENTRY:\n");
				  npd_syslog_dbg("-------------------------------------------------------------------------\n");
				  npd_syslog_dbg("%-17s %-4s %-6s %-4s %-5s %-5s\n","MAC Address","VLAN","DEVICE","SLOT","PORT","ROUTE");
				  npd_syslog_dbg("---- ----------------- ---- ------ ---- ----- ------- ------\n");
				  npd_syslog_dbg("%02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-4d %-5s\n", \
						   macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2],	\
						  macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5], \
						  macEntry->key.key.macVlan.vlanId, 	\
						  macEntry->dstInterface.devPort.devNum,	\
						  slot_no,port_no,\
						  (macEntry->daRoute == 1) ? "en":"dis");
		    #endif
				 fdb_limit_type->vlan_port_type = 1;
				 return NPD_TRUE;
		   } 
		   else {
	             return NPD_FALSE;
		   }
		}

	}
    else if(1 ==rp){/*vlan port based is No,but port base is yes*/
		if( 1 == rv){
			   if((actNumPort < numberPort)&&(actNumPort<numberVlan)
		  						  &&(actNumVlan < numberPort)&& (actNumVlan < numberVlan)){
				   
               #if 0
				    npd_syslog_dbg("PORT-BASED:	numberPort: %d,actNumPort: %d *** VLAN-BASED: numberVlan: %d,actNumVlan: %d\n",
													numberPort,actNumPort,numberVlan,actNumVlan);
					npd_syslog_dbg("READ FDB　ITEM PORT-BASED|VLAN-BASED ENTRY:\n");
					npd_syslog_dbg("-----------------------------------------------\n");
					npd_syslog_dbg("%-17s %-4s %-6s %-4s %-5s %-5s\n","MAC Address","VLAN","DEVICE","SLOT","PORT","ROUTE");
					npd_syslog_dbg("---- ----------------- ----- ------ ----- -----\n");
					npd_syslog_dbg("%02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-4d %-5s\n", \
							  macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2], \
							  macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5], \
							  macEntry->key.key.macVlan.vlanId, 	\
							  macEntry->dstInterface.devPort.devNum,	\
							  slot_no,port_no,\
							  (macEntry->daRoute == 1) ? "en":"dis");
		           #endif
				   fdb_limit_type->port_type = 1;
				   fdb_limit_type->vlan_type = 1;
				   return NPD_TRUE;
			   }
			   else{
				   return NPD_FALSE;
			   }
		   }
		   else {
			   if(actNumPort < numberPort){
		   	   #if 0
					npd_syslog_dbg("PORT-BASED: 	numberPort: %d,actNumPort: %d \n",
													numberPort,actNumPort);
					npd_syslog_dbg("READ FDB　ITEM PORT-BASED ENTRY:\n");
					npd_syslog_dbg("-----------------------------------------------\n");
					npd_syslog_dbg("%-17s %-4s %-6s %-4s %-5s %-5s\n","MAC Address","VLAN","DEVICE","SLOT","PORT","ROUTE");
					npd_syslog_dbg("---- ----------------- ----- ------ ----- -----\n");
					npd_syslog_dbg("%02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-4d %-5s\n", \
							  macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2], \
							  macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5], \
							  macEntry->key.key.macVlan.vlanId, 	\
							  macEntry->dstInterface.devPort.devNum,	\
							  slot_no,port_no,\
							  (macEntry->daRoute == 1) ? "en":"dis");
		           #endif
				   fdb_limit_type->port_type = 1;
				   return NPD_TRUE;
			   }
			   else{
				return NPD_FALSE;
			   } 
		   }

	}
    else if(1 == rv){
	    if((actNumVlan < numberVlan)){
          #if 0
		  npd_syslog_dbg("VLAN-BASED: numberVlan: %d, actNumVlan: %d \n",numberVlan,actNumVlan);
		  npd_syslog_dbg("READ FDB　ITEM VLAN-BASED ENTRY:\n");
		  npd_syslog_dbg("-------------------------------------------------------------------------\n");
		  npd_syslog_dbg("%-17s %-4s %-6s %-4s %-5s %-5s\n","MAC Address","VLAN","DEVICE","SLOT","PORT","ROUTE");
		  npd_syslog_dbg("---- ----------------- ---- ------ ---- ----- ------- ------\n");
		  npd_syslog_dbg("%02X:%02X:%02X:%02X:%02X:%02X %-4d %-6d %-4d %-4d %-5s\n", \
					  macPtr->arEther[0],macPtr->arEther[1],macPtr->arEther[2],    \
					 macPtr->arEther[3],macPtr->arEther[4],macPtr->arEther[5], \
					 macEntry->key.key.macVlan.vlanId,	   \
					 macEntry->dstInterface.devPort.devNum,    \
					 slot_no,port_no,\
					 (macEntry->daRoute == 1) ? "en":"dis");
         #endif
		 fdb_limit_type->vlan_type = 1;
		 return NPD_TRUE;
	    } 
	    else{
		  return NPD_FALSE;
	    }

	} 
    return NPD_FALSE;

}


unsigned int  npd_fdb_limit_aging
(
   CPSS_MAC_ENTRY_EXT_STC      *MacEntry,
   unsigned int                *fdbCount
)
{
    unsigned int ret=0,rc = 0;
    ETHERADDR     *macPtr = NULL;
	unsigned int  slot_no = 0;
	unsigned int  slot_index = 0;
	unsigned int  port_no = 0;
	unsigned int  port_index = 0;
	unsigned int  eth_g_index = 0;
	unsigned short vlanId = 0;
	unsigned int  number = 0;
    unsigned char isTagged = 0;
	struct eth_port_s *portInfo = NULL;
	struct eth_port_fdb_s* fdbNode = NULL;
	struct vlan_s *vlanNode = NULL;
	struct vlan_port_list_node_s *node = NULL;
    vlan_port_list_s *portList = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	
    macPtr = &(MacEntry->key.key.macVlan.macAddr);
	vlanId = MacEntry->key.key.macVlan.vlanId;
	ret = npd_get_global_index_by_devport(MacEntry->dstInterface.devPort.devNum,MacEntry->dstInterface.devPort.portNum,&eth_g_index);
    if(0 != ret)
	{
       return FDB_CONFIG_FAIL;
	}
	slot_index = npd_get_slot_index_from_eth_index(eth_g_index);
	slot_no = npd_get_slot_no_from_index(slot_index);
	port_index = npd_get_port_index_from_eth_index(eth_g_index);
	port_no = npd_get_port_no_from_index(slot_index,port_index);
	
	ret = npd_fdb_number_vlan_port_set_check(vlanId,slot_no,port_no,&number);
	if( 1 == ret)
	{	
		vlanNode = npd_find_vlan_by_vid(vlanId);
	   	ret = npd_vlan_check_contain_port(vlanId, eth_g_index,&isTagged);
	   	if(NPD_TRUE == isTagged) 
	   	{
			portList = vlanNode->tagPortList;
		}
	   	else if(NPD_FALSE == isTagged) 
		{
			portList = vlanNode->untagPortList;
	   	}
       	list = &(portList->list);
		__list_for_each(pos,list) 
		{
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if(eth_g_index == node->eth_global_index) 
			{
                if(0 != node->fdbCount)
				{
				   node->fdbCount--;
				   /*npd_syslog_dbg("VLAN-PORT-BASED fdb number is %d\n",node->fdbCount);*/
                   if(npd_fdb_number_port_set_check(slot_no,port_no,&number))
				   {
                       portInfo = npd_get_port_by_index(eth_g_index); 
					   if(NULL != portInfo)
					   {
						   fdbNode = portInfo->funcs.func_data[ETH_PORT_FUNC_FDB];
						   if((NULL != fdbNode)&&(0!=fdbNode->fdbCount))
						   {
						      fdbNode->fdbCount--;
							 /* npd_syslog_dbg("PORT-BASED fdb number is %d\n",fdbNode->fdbCount);*/
						   }
					   }
				   }
				   if(npd_fdb_number_vlan_set_check(vlanId,&number))
				   {
				   		vlanNode = npd_find_vlan_by_vid(vlanId);
                       	if(0!= vlanNode->brgVlan.fdbCount)
					   	{
							vlanNode->brgVlan.fdbCount--;
                       	}
				   }
				   *fdbCount = node->fdbCount;
				   rc +=1;
				   return FDB_CONFIG_SUCCESS;
				}
			}
		}
	}
	ret = npd_fdb_number_port_set_check(slot_no,port_no,&number);
	if(1 == ret)
	{
		portInfo = npd_get_port_by_index(eth_g_index); 
		if(NULL != portInfo)
		{
			fdbNode = portInfo->funcs.func_data[ETH_PORT_FUNC_FDB];
			if((NULL != fdbNode)&&(0!= fdbNode->fdbCount))
			{
		    	fdbNode->fdbCount--;
			 /* npd_syslog_dbg("PORT-BASED fdb number is %d\n",fdbNode->fdbCount);*/
			  	if(npd_fdb_number_vlan_set_check(vlanId,&number))
			  	{
			  		vlanNode = npd_find_vlan_by_vid(vlanId);
	              	if(0!= vlanNode->brgVlan.fdbCount)
				  	{
						vlanNode->brgVlan.fdbCount--;
						/*npd_syslog_dbg("VLAN-BASED fdb number is %d\n",vlanNode->brgVlan.fdbCount);*/
	              	}
			  	}
			  	*fdbCount =  fdbNode->fdbCount;
			   	rc +=1;
			   	return FDB_CONFIG_SUCCESS;
		   	}
	   	}
	}
	ret = npd_fdb_number_vlan_set_check(vlanId,&number);
	if(1 == ret)
	{
		vlanNode = npd_find_vlan_by_vid(vlanId);
	  	if(0!= vlanNode->brgVlan.fdbCount)
		{
	  		vlanNode->brgVlan.fdbCount--;
        	*fdbCount =  vlanNode->brgVlan.fdbCount;
			/*npd_syslog_dbg("VLAN-BASED fdb number is %d\n",vlanNode->brgVlan.fdbCount);*/
	  	}
	  	rc +=1;
	  	return FDB_CONFIG_SUCCESS;
	}
	/*code optimize:  Logically dead code
	zhangcl@autelan.com 2013-1-17*/
    return FDB_CONFIG_FAIL;
}


/**********************************************************************************
 * npd_fdb_fdblimit_set
 *
 * set FDB fdblimit to 0 
 *
 *	INPUT:
 *		slotno
 *		portno
 *		fdblimit
 *
 *	OUTPUT:
 *		NULL
 *
 *      function:
 *          delete port fdb limit:
 *	add by liuxy 
 *
 **********************************************************************************/
int npd_fdb_set_port_number
(
   unsigned char slotno,
   unsigned char portno,
   unsigned int  fdblimit

)
{
	unsigned char devNum = 0,portNum = 0;
	unsigned int eth_g_index = 0;
	unsigned int isEnable = 0;/*port admin state, 1 means enable*/
	unsigned int ret = NPD_FDB_ERR_NONE;
	int 		 retVal = 0;
	unsigned int vlanid = 0,isTagged = 0,number = 0,flag = 0;
	unsigned int status = 0 ; /*for port learn status , 0 means that auto learn disable*/
	unsigned int enable = 1,disable = 0; /*for port Na enable/disable, 1 means that enable NA msg to cpu*/
	unsigned int aurate = 0,auenable = 1,audisable = 0;
	unsigned int learn = 0,learncmd = 0;
	unsigned short trunkid = 0;

	npd_learn_limit_t limit;
	memset(&limit,0,sizeof(npd_learn_limit_t));

    if( 0 == fdblimit)
	{/* cancel the fdb protect*/
        /*set the number to the sw */
        ret = npd_fdb_number_port_set(slotno,portno, fdblimit);
        if(FDB_BOOL_FALSE == ret)
		{
           npd_syslog_err("npd_fdb_number_port_set in npd_dbus_fdb_config_port_number error: slotnum %d,portnum %d,fdblimit %d",slotno,portno,fdblimit);
           ret = FDB_RETURN_CODE_GENERAL;
		}
	    else
		{/*flush fdb items*/
	        ret = npd_fdb_na_msg_per_port_set(slotno,portno,disable);
			if(0 != ret)
			{
               npd_syslog_dbg("Error to set na msg to port %d/%d disable\n",slotno,portno);
			}
			/*clear the fdb entry in hw*/
            nam_fdb_table_delete_entry_with_port(eth_g_index);
			/*recover the fdb admin status*/
			for(vlanid = 1;vlanid < 4096;vlanid++)
			{
				if(NPD_TRUE != (ret = npd_vlan_check_contain_port(vlanid,eth_g_index,&isTagged)))
				{
                   continue;
				}
				else if((NPD_TRUE == (ret = npd_fdb_number_vlan_port_set_check(vlanid,slotno,portno,&number)))||
					               (NPD_TRUE == (ret = npd_fdb_number_vlan_set_check(vlanid,&number))))
				{

				    /* fdb limit has another one,enable the na  and set auto learn flag !!*/
					ret = npd_fdb_na_msg_per_port_set(slotno,portno,enable);
					if(0!=ret)
					{
						 npd_syslog_err("npd_dbus_fdb_config_port_number:npd_fdb_na_msg_per_port_set err,%d \n",ret);
						 ret = FDB_RETURN_CODE_GENERAL;
					}
					else
					{
						 flag++;
                         break;
					}
				}
				else 
				{
                   continue;
				}
			}
			if(0 == flag)
			{
               	ret = npd_fdb_set_port_learn_status(slotno,portno,1);
		        if(0!=ret)
				{
		            npd_syslog_err("npd_dbus_fdb_config_port_number:npd_fdb_set_port_learn_status err,%d \n",ret);
				    ret = FDB_RETURN_CODE_GENERAL;
		        }
			}
		 	 limit.limit = -1;
			 limit.port = portNum;
			 limit.flags = NPD_L2_LEARN_LIMIT_PORT;
             retVal = nam_fdb_limit_set(devNum,limit);
			 if(0 != retVal)
			 {
                syslog_ax_fdb_err("set fdb limit failed for %d\n",ret);
				ret = FDB_RETURN_CODE_OCCUR_HW;
			 }
			 /*JUST IF DISABLE AU RATE LIMIT*/
			 ret = npd_fdb_check_limit_exist(devNum);
			 if(NPD_FALSE == ret)
			 {
                nam_fdb_au_rate_set(devNum,0,audisable);
			 }
		}
	}
	/*code optimize:  Missing return statement
	zhangcl@autelan.com 2013-1-17*/
	return ret;	
}

/**********************************************************************************
 * npd_fdb_static_entry_add
 *
 * add static FDB entry to global FDB database
 *
 *	INPUT:
 *		mac -mac address
 *		vid - vlan id
 *		eth_index - global eth port index
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 **********************************************************************************/
 
unsigned int npd_fdb_static_entry_add
(
	unsigned char *mac,
	unsigned short vid,
	unsigned char slot,
	unsigned char port,
	unsigned int  isMirror
)
{
	unsigned int i =0 ;
	unsigned int count = 0;
	unsigned int find ;
	struct fdb_entry_node_s *node = NULL;
	struct fdb_entry_node_s **arry = NULL;	

	struct fdb_blacklist_node_s **blacklistarry = NULL;
	
	arry = npd_fdb_get_entry();
	blacklistarry = npd_fdb_get_blacklist();
	
	if(g_static_fdb_count >= AX_STATIC_FDB_ENTRY_SIZE) {
		npd_syslog_err("npd static fdb database overrun\n");
		return FDB_RETURN_CODE_MAX;
	}

	/* check if in the blacklist entry list*/
    for(i = 0; i < AX_STATIC_FDB_ENTRY_SIZE; i++) {
        if(count >= g_blacklist_fdb_count) {
			break;
		}
		if(blacklistarry[i]==NULL){
			continue;
		}
		if((0==memcmp(blacklistarry[i]->mac,mac,MAC_ADDRESS_LEN))&&(vid == blacklistarry[i]->vlanid))
		{
		    npd_syslog_err("The item has exist as one blacklist item. Please delete it first \n");
			return FDB_RETURN_CODE_GENERAL;
		}
		count++;
    }
	
	/*TODO: check if same static fdb entry exists*/
	count = 0;
	for(i = 0; i < AX_STATIC_FDB_ENTRY_SIZE; i++) {
		if(count >= g_static_fdb_count) {
			break;
		}
		if(arry[i]==NULL){
			continue;
		}
		if((0==memcmp(arry[i]->mac,mac,MAC_ADDRESS_LEN))&&(arry[i]->vlanid==vid))
		{
		    if((0 == arry[i]->isMirror)&&(1 == isMirror)){
				arry[i]->isMirror = 1;
                return NPD_FDB_ERR_NONE;
			}
			npd_syslog_err("the entry already exists !!!! \n");
			return FDB_RETURN_CODE_NODE_EXIST;				
		}
		count++;
	}
	node = (struct fdb_entry_node_s *) malloc(sizeof(struct fdb_entry_node_s));
	
	if(NULL == node) {
		npd_syslog_err("npd out of memory when add static fdb entry\n");

		return FDB_RETURN_CODE_GENERAL;
	}
	memset(node,0,sizeof(struct fdb_entry_node_s));
	 	
	memcpy(node->mac,(unsigned char*)mac,MAC_ADDRESS_LEN);
	
	node->vlanid = vid;
	node->slot = slot;
	node->port = port;
	node->trunkid = 0;
	node->isMirror = isMirror;
	
	for( find =0;arry[ find] != NULL; find++)
		{ ;}
    if(find <AX_STATIC_FDB_ENTRY_SIZE){
	  	arry[ find] = node;
		g_static_fdb_count++;
	}
	else
	{
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-17*/
		free(node);
		return FDB_RETURN_CODE_GENERAL;
	}
	npd_syslog_dbg("npd static fdb database number is %d\n",g_static_fdb_count);
	return NPD_FDB_ERR_NONE;	
}


unsigned int npd_fdb_check_static_entry_exist
(
    unsigned char *mac,
	unsigned short vid,
	unsigned char *slot_no,
	unsigned char *port_no
)
{
   	struct fdb_entry_node_s **arry = NULL;
    unsigned int i = 0,count = 0;
	
    arry = npd_fdb_get_entry();
	
	if(g_static_fdb_count >= AX_STATIC_FDB_ENTRY_SIZE) {
		npd_syslog_err("npd static fdb database overrun");
		return FDB_RETURN_CODE_MAX ;
	}

	for(i = 0; i < AX_STATIC_FDB_ENTRY_SIZE; i++) {
		if(count >= g_static_fdb_count) {
			return FDB_RETURN_CODE_NODE_NOT_EXIST;	
		}
		if(arry[i]==NULL){
			continue;
		}
		if((0==memcmp(arry[i]->mac,mac,MAC_ADDRESS_LEN))&&(arry[i]->vlanid==vid))
		{
		   *slot_no = arry[i]->slot;
		   *port_no = arry[i]->port;
			return FDB_RETURN_CODE_NODE_EXIST;				
		}
		count++;
	}
	return NPD_FDB_ERR_NONE;
}
/**********************************************************************************
 * npd_fdb_static_trunk_add
 *
 * add static FDB trunk to global FDB database
 *
 *	INPUT:
 *		mac -mac address
 *		vid - vlan id
 *		trunkId - trunk ID
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 **********************************************************************************/
 
unsigned int npd_fdb_static_trunk_add
(
	unsigned char *mac,
	unsigned short vid,
    unsigned short trunkId
)
{
	unsigned int i =0 ;
	unsigned int count = 0;
	unsigned int find ;
	struct fdb_entry_node_s *node = NULL;
	struct fdb_entry_node_s **arry = NULL;	
	
	arry = g_static_fdb;	
	if(g_static_fdb_count >= AX_STATIC_FDB_ENTRY_SIZE) {
		npd_syslog_err("npd fdb trunk database overrun\n");
		return FDB_RETURN_CODE_MAX ;
	}

	for(i = 0; i < AX_STATIC_FDB_ENTRY_SIZE; i++) {
		if(count >= g_static_fdb_count) {
			break;
		}
		if(arry[i]==NULL){
			continue;
		}
		if((0==memcmp(arry[i]->mac,mac,MAC_ADDRESS_LEN))&&(arry[i]->vlanid==vid))
		{

			return FDB_RETURN_CODE_NODE_EXIST;				
		}
		count++;
	}
	node = (struct fdb_entry_node_s *) malloc(sizeof(struct fdb_entry_node_s));
	
	if(NULL == node) {
		npd_syslog_err("npd out of memory when add fdb trunk entry\n");

		return FDB_RETURN_CODE_GENERAL;
	}
	memset(node,0,sizeof(struct fdb_entry_node_s));
	 	
	memcpy(node->mac,(unsigned char*)mac,MAC_ADDRESS_LEN);
	
	node->vlanid = vid;
    node->trunkid= trunkId;
	node->port = 0;
	node->slot = 0;
	
	for( find =0;arry[ find] != NULL; find++)
		{ ;}
    if(find <AX_STATIC_FDB_ENTRY_SIZE)
	{
	  	arry[ find] = node;
		g_static_fdb_count++;
	}
	else
	{
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-17*/
		free(node);
		npd_syslog_dbg("static fdb table is full !\n");
		return FDB_RETURN_CODE_GENERAL;
	}
	npd_syslog_dbg("npd fdb trunk database number is %d \n",g_static_fdb_count);
	return NPD_FDB_ERR_NONE;	
}




/**********************************************************************************
 * npd_fdb_static_entry_del
 *
 * delete static FDB entry to global FDB database
 *
 *	INPUT:
 *		mac -mac address
 *		vid - vlan id
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 **********************************************************************************/
unsigned int npd_fdb_static_entry_del
(
	unsigned char* mac,
	unsigned short vid
)
{
	unsigned int count = 0;
	unsigned int i=0;
	
	struct fdb_entry_node_s *node = NULL;
	struct fdb_entry_node_s **arry;	
	arry = npd_fdb_get_entry();
	for( ;i<AX_STATIC_FDB_ENTRY_SIZE;i++){
		if(count >= g_static_fdb_count){
			return FDB_RETURN_CODE_NODE_NOT_EXIST;
			}
		if(arry[i]==NULL){
			continue;
			}
		if((0==memcmp(arry[i]->mac,mac,MAC_ADDRESS_LEN))&&(arry[i]->vlanid==vid))
			{break;}
		
		count++;
			
		}
	node = arry[i];
	if(NULL == node) {
		npd_syslog_err("null node found when delete static fdb entry");
		return FDB_RETURN_CODE_GENERAL;
	}
	else if (FDB_STATIC_ISMIRROR == node->isMirror){
        npd_syslog_err("The item is mirror , can not delete directly!\n");
		return FDB_RETURN_CODE_ITEM_ISMIRROR;
	}
	free(node);
	arry[i] = NULL;
	g_static_fdb_count--;

	return NPD_FDB_ERR_NONE;
}


/**********************************************************************************
 * npd_fdb_static_entry_del_with_port_vlan
 *
 * delete static FDB entry to global FDB database
 *
 *	INPUT:
 *		slotNum  - slotnum
 *		portNum - portnum
 *             vid  - vlan ID
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_FDB_ERR_NODE_NOT_EXIST  -- static fdb did not exist
 *		NPD_FDB_ERR_NONE	 -- find static fdb and delete it success 
 *		
 **********************************************************************************/
unsigned int npd_fdb_static_entry_del_with_port_vlan
(
	unsigned char slotNum,
	unsigned char portNum,
	unsigned short vid
)
{
	unsigned int count = 0;
	unsigned int i=0;
	
	struct fdb_entry_node_s *node = NULL;
	struct fdb_entry_node_s **arry;	
	arry = npd_fdb_get_entry();
	for( ;i<AX_STATIC_FDB_ENTRY_SIZE;i++)
	{
		if(count >= g_static_fdb_count)
		{
			return NPD_FDB_ERR_NODE_NOT_EXIST;
		}
		if(arry[i]==NULL)
		{
			continue;
		}
		if((portNum == arry[i]->port)&&(slotNum == arry[i]->slot)&&(arry[i]->vlanid==vid))
		{
			break;
		}
		
		count++;
			
	}
	node = arry[i];
	if(NULL == node) {
		npd_syslog_err("null node found when delete static fdb entry");
		return NPD_FDB_ERR_GENERAL;
	}
	else if (FDB_STATIC_ISMIRROR == node->isMirror){
        npd_syslog_err("The item is mirror , can not delete directly!\n");
		return NPD_FDB_ERR_ITEM_ISMIRROR;
	}
	free(node);
	arry[i] = NULL;
	g_static_fdb_count--;

	return NPD_FDB_ERR_NONE;
}

unsigned int npd_fdb_static_mirror_entry_del
(
	unsigned char* mac,
	unsigned short vid
)
{
	unsigned int count = 0;
	unsigned int i=0;
	
	struct fdb_entry_node_s *node = NULL;
	struct fdb_entry_node_s **arry;	
	arry = npd_fdb_get_entry();
	for( ;i<AX_STATIC_FDB_ENTRY_SIZE;i++){
		if(count >= g_static_fdb_count){
			return FDB_RETURN_CODE_NODE_NOT_EXIST;
			}
		if(arry[i]==NULL){
			continue;
			}
		if((0==memcmp(arry[i]->mac,mac,MAC_ADDRESS_LEN))&&(arry[i]->vlanid==vid))
			{break;}
		
		count++;
			
		}
	node = arry[i];
	if(NULL == node) {
		npd_syslog_err("null node found when delete static fdb entry");
		return FDB_RETURN_CODE_GENERAL;
	}
	else if ( FDB_STATIC_ISMIRROR == node->isMirror){
        npd_syslog_err("Yes, delete mirror item!\n");
		free(node);
		arry[i] = NULL;
		g_static_fdb_count--;
		return NPD_FDB_ERR_NONE;
	}
	return FDB_RETURN_CODE_GENERAL;

}


/**********************************************************************************
 * npd_fdb_static_entry_del_by_vlan
 *
 * delete static FDB entry to global FDB database
 *
 *	INPUT:
 *		vid - vlan id
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 **********************************************************************************/
unsigned int npd_fdb_static_entry_del_by_vlan
(
	unsigned short vid
)
{
	unsigned int count = 0;
	unsigned int i=0;
	
	struct fdb_entry_node_s *node = NULL;
	struct fdb_entry_node_s **arry;	
	arry = npd_fdb_get_entry();
	for( ;i<AX_STATIC_FDB_ENTRY_SIZE;i++){
		if(count >= g_static_fdb_count){
			break;
		}
		if(arry[i]==NULL){
			continue;
		}
		if(arry[i]->vlanid==vid){
				node = arry[i];
				if(NULL == node) {
					npd_syslog_err("null node found when delete static fdb entry");
					return FDB_RETURN_CODE_GENERAL;
				}
				
				free(node);
				arry[i] = NULL;
				g_static_fdb_count--;
				count--;
		}
					
		count++;
			
	}

	return NPD_FDB_ERR_NONE;
}


/**********************************************************************************
 * npd_fdb_static_entry_del_by_port
 *
 * delete static FDB entry to global FDB database
 *
 *	INPUT:
 *		slot_no --slot number
 *           port_no--port number
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 **********************************************************************************/
unsigned int npd_fdb_static_entry_del_by_port
(
	unsigned char slot_no,
	unsigned char port_no
)
{
	unsigned int count = 0;
	unsigned int i=0,ret = 0;
	
	struct fdb_entry_node_s *node = NULL;
	struct fdb_entry_node_s **arry;	
	char macComp[13] = {0};
	
	arry = npd_fdb_get_entry();
	for( ;i<AX_STATIC_FDB_ENTRY_SIZE;i++){
		if(count >= g_static_fdb_count){
			break;
		}
		if(arry[i]==NULL){
			continue;
		}
		if((arry[i]->slot == slot_no)&&(arry[i]->port == port_no)){
				node = arry[i];
				if(NULL == node) {
					npd_syslog_err("null node found when delete static fdb entry");
					return FDB_RETURN_CODE_GENERAL;
				}
				else{
					sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
										arry[i]->mac[0],arry[i]->mac[1],arry[i]->mac[2],arry[i]->mac[3],arry[i]->mac[4],arry[i]->mac[5]);
					ret = npd_system_verify_basemac(macComp);
					if(0 == ret){/*is not the sys mac*/
						free(node);
						arry[i] = NULL;
						g_static_fdb_count--;
						count--;
					}
				}

		}
					
		count++;
			
	}

	return NPD_FDB_ERR_NONE;
}


/**********************************************************************************
 * npd_fdb_static_trunk_del
 *
 * delete static FDB trunk from global FDB database
 *
 *	INPUT:
 *		mac -mac address
 *		vid - vlan id
 *		trunkId - Trunk Id
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 *       NOTE: Now just one static trunk fdb exist or not according to the vlan, one vlan only one trunk 
 *              item
 **********************************************************************************/
unsigned int npd_fdb_static_trunk_del
(
    char* mac,
    unsigned short vid
)
{
	unsigned int count = 0;
	unsigned int i=0;
	
	struct fdb_entry_node_s *node = NULL;
	struct fdb_entry_node_s **arry;	
	arry = g_static_fdb;
	for( ;i<AX_STATIC_FDB_ENTRY_SIZE;i++){
		if(count >= g_static_fdb_count){
			return FDB_RETURN_CODE_NODE_NOT_EXIST;
			}
		if(arry[i]==NULL){
			continue;
			}
		if((0==memcmp(arry[i]->mac,mac,MAC_ADDRESS_LEN))&&(arry[i]->vlanid==vid))
			{break;}
		
		count++;
			
		}
	node = arry[i];
	if(NULL == node) {
		npd_syslog_err("null node found when delete fdb trunk entry");
		return FDB_RETURN_CODE_GENERAL;
	}
	
	free(node);
	arry[i] = NULL;
	g_static_fdb_count--;
	
	return NPD_FDB_ERR_NONE;
}

/**********************************************************************************
 * npd_fdb_static_blacklist_entry_add
 *
 * add static FDB entry to global FDB database
 *
 *	INPUT:
 *		mac -mac address
 *		vid - vlan id
 *		eth_index - global eth port index
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 **********************************************************************************/
 
unsigned int npd_fdb_static_blacklist_entry_add
(
	unsigned char *mac,
	unsigned short vid,
	unsigned char flag,
	unsigned int  *blacklist_node_index
)
{   
	unsigned int i =0 ;
	unsigned int count = 0;
	unsigned int find =0;
	unsigned int ret = 0;
	
	struct fdb_blacklist_node_s *node = NULL;
	struct fdb_blacklist_node_s **arry = NULL;
	
	struct fdb_entry_node_s *staticnode = NULL;
	struct fdb_entry_node_s **staticarry = NULL;
	
	arry = npd_fdb_get_blacklist();
	staticarry = npd_fdb_get_entry();
	
	if(g_blacklist_fdb_count >= AX_STATIC_FDB_ENTRY_SIZE) {
		npd_syslog_err("npd blacklist fdb database overrun");
		return FDB_RETURN_CODE_MAX;
	}

	/*if it is a static entry before, delete it from static entry list*/
    count = 0;
    for(i = 0; i < AX_STATIC_FDB_ENTRY_SIZE; i++) {
		
       if(count >= g_static_fdb_count) {
			break;
		}
		if(NULL == staticarry[i]){
			continue;
		}
		if((0==memcmp(staticarry[i]->mac,mac,MAC_ADDRESS_LEN))&&(staticarry[i]->vlanid==vid))
		{
            staticnode = staticarry[i];
	        free(staticnode);
	        staticarry[i] = NULL;
	        g_static_fdb_count--;
			ret = nam_no_static_fdb_entry_mac_vlan_set(mac,vid);
					
			if(FDB_RETURN_CODE_GENERAL == ret){
				npd_syslog_err("npd_dbus_fdb_config_mac_vlan_port:: fdb entry ERROR. \n");
			}
			else if(FDB_RETURN_CODE_NODE_NOT_EXIST == ret){
				npd_syslog_err("the static FDB item is not exist. \n");
			}
			break;
		}
		
		count++;
	}
	npd_syslog_dbg("The fdb static total number is %d\n",g_static_fdb_count);

	/*deal with blacklist*/
	for(i = 0; i < AX_STATIC_FDB_ENTRY_SIZE; i++) {

		if(NULL == arry[i]){
			continue;
		}
		if((0==memcmp(arry[i]->mac,mac,MAC_ADDRESS_LEN))&&(arry[i]->vlanid==vid))
		{

		   /*flag == 1 means dmac,0 means smac*/
           if(1==flag){
		   	 if(1 == arry[i]->dmac){
			 	return FDB_RETURN_CODE_NODE_EXIST;
		   	 }	
			 else{
		   	    arry[i]->dmac = 1; /* 1 means drop*/
		   	    *blacklist_node_index = i;
		   	    return NPD_FDB_ERR_NONE;
			 }
           }
		   else if(0 == flag){
		   	  if(1 == arry[i]->smac){
			  	 return FDB_RETURN_CODE_NODE_EXIST;
		   	  } 
			  else{
		       	arry[i]->smac = 1;
				*blacklist_node_index = i;
			    return NPD_FDB_ERR_NONE;
			  }
		   }
		   else{
		   	 npd_syslog_err("Error flag number");
			 return FDB_RETURN_CODE_GENERAL;
		   }
		}
		
		count++;
		if(count >= g_blacklist_fdb_count) {
			break;
		}
	}
	node = (struct fdb_blacklist_node_s *) malloc(sizeof(struct fdb_blacklist_node_s));
	if(NULL == node) {
		npd_syslog_err("npd out of memory when add blacklist fdb entry");
		return FDB_RETURN_CODE_GENERAL;
	}
	memset(node,0,sizeof(struct fdb_blacklist_node_s));
	 	
	memcpy(node->mac,(unsigned char*)mac,MAC_ADDRESS_LEN);
	
	node->vlanid = vid;
	
	if(1==flag){
	  node->dmac = 1; /* 1 means drop*/
    }
	else if(0 == flag){
	  node->smac = 1;
	}
	else
	{
		npd_syslog_err("Error flag number");
	  /*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-17*/
		free(node);
		return FDB_RETURN_CODE_GENERAL;
	}
	
	for( find =0;arry[ find] != NULL; find++)
		{ ;}
    if(find <AX_STATIC_FDB_ENTRY_SIZE){
	  	arry[ find] = node;
		*blacklist_node_index = find;
	}
	else
	{
		/*code optimize:  Resource leak
		zhangcl@autelan.com 2013-1-17*/
		free(node);
		return FDB_RETURN_CODE_GENERAL;
	}
	g_blacklist_fdb_count++;
	npd_syslog_dbg("npd blacklist fdb database number is %d",g_blacklist_fdb_count);
	
	return NPD_FDB_ERR_NONE;	
}


/**********************************************************************************
 * npd_fdb_static_blacklist_entry_del
 *
 * delete static FDB entry to global FDB database
 *
 *	INPUT:
 *		mac -mac address
 *		vid - vlan id
 *		eth_index - global eth port index
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 **********************************************************************************/
unsigned int npd_fdb_static_blacklist_entry_del
(
	char* mac,
	unsigned short vid,
	unsigned char flag
)
{
	unsigned int count = 0;
	unsigned int i=0;
	
	struct fdb_blacklist_node_s *node = NULL;
	struct fdb_blacklist_node_s **arry;	
	
	arry = npd_fdb_get_blacklist();
	for( ;i<AX_STATIC_FDB_ENTRY_SIZE;i++){
		if(count >= g_blacklist_fdb_count){
			return FDB_RETURN_CODE_NODE_NOT_EXIST;
			}
		if(arry[i]==NULL){
			continue;
			}
		if((0==memcmp(arry[i]->mac,mac,MAC_ADDRESS_LEN))&&(arry[i]->vlanid==vid))
			{break;}
		
		count++;
			
	}
	
	node = arry[i];
	if(NULL == node) {
		npd_syslog_err("No node found when delete blacklist fdb entry");
		return FDB_RETURN_CODE_NODE_NOT_EXIST;
	}

	if( 1== flag){ /*cancel the dmac drop action*/
		if(1== node->dmac){
			if( 0 == node->smac){
			   free(node);
	           arry[i] = NULL;
	           g_blacklist_fdb_count--;
	           return NPD_FDB_ERR_NONE; 
			}
			else if( 1 == node->smac){/*if the smac is also drop action now*/
			   node->dmac = 0;
			}
		}
		else if( 0 == node->dmac){
		   npd_syslog_dbg("The dmac MAC is not in blacklist mode,please check the order parameter\n");
		   return FDB_RETURN_CODE_NODE_NOT_EXIST;
		}
	}
	else if(0 == flag){/*cancel the smac drop action*/
        if(1==node->smac){
			if(0 ==node->dmac){
               free(node);
	           arry[i] = NULL;
	           g_blacklist_fdb_count--;
	           return NPD_FDB_ERR_NONE; 
			}
			else if( 1== node->dmac){/*if the dmac is alse drop action now*/
			   node->smac = 0;
			}
		}
	    else if(0 == node->smac){
		   npd_syslog_dbg("The smac MAC is not in blacklist mode,please check the order parameter\n");
		   return FDB_RETURN_CODE_NODE_NOT_EXIST;
		}
	}	

  return NPD_FDB_ERR_NONE;
	
}



/**********************************************************************************
 * npd_fdb_static_blacklist_entry_del_with_vlan
 *
 * delete static FDB entry to global FDB database
 *
 *	INPUT:
 *		vid - vlan id
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		node - static fdb entry info stucture
 *
 **********************************************************************************/

unsigned int npd_fdb_static_blacklist_entry_del_with_vlan
(
     unsigned short vlanId
)
{
     unsigned int ret= 0 ,i = 0;
     struct fdb_blacklist_node_s **array_blacklist = NULL;

	 array_blacklist = npd_fdb_get_blacklist();
 	 for( ;i<AX_STATIC_FDB_ENTRY_SIZE;i++){
		 if(0 == g_blacklist_fdb_count){
			 return FDB_RETURN_CODE_NODE_NOT_EXIST;
		 }
		 if(i >= g_blacklist_fdb_count){
			 break;
		 }
		 if(NULL == array_blacklist[i]){
			 continue;
		 }
		 if(vlanId == array_blacklist[i]->vlanid){
             if( array_blacklist[i]->dmac){
		         ret = nam_fdb_entry_mac_vlan_no_drop( array_blacklist[i]->mac,vlanId,1);
                 if(NPD_FDB_ERR_NONE != ret){
                    npd_syslog_err("npd_fdb_static_blacklist_entry_del_with_vlan:nam_fdb_entry_mac_vlan_no_drop error,return value is %d\n",ret);
					return FDB_RETURN_CODE_OCCUR_HW;
				 }
			 }
		     if( array_blacklist[i]->smac){
                 ret = nam_fdb_entry_mac_vlan_no_drop( array_blacklist[i]->mac,vlanId,0);
                 if(NPD_FDB_ERR_NONE != ret){
                    npd_syslog_err("npd_fdb_static_blacklist_entry_del_with_vlan:nam_fdb_entry_mac_vlan_no_drop error,return value is %d\n",ret);
					return FDB_RETURN_CODE_OCCUR_HW;
				 }
			 } 
		     free( array_blacklist[i]);
		     array_blacklist[i] = NULL;
		 }
		 else{
             continue;
		 }
    }
	return NPD_FDB_ERR_NONE;
	
}

unsigned int npd_fdb_get_blacklist_item
(
    struct fdb_blacklist_node_s *blacklist_array,
    unsigned int   size
)
{ 
    unsigned int i = 0;
	unsigned int count = 0;
	struct fdb_blacklist_node_s *node = NULL;
	struct fdb_blacklist_node_s **arry = NULL;	
	
	arry = npd_fdb_get_blacklist();
	for( ;i<AX_STATIC_FDB_ENTRY_SIZE;i++){
		if(0 == g_blacklist_fdb_count){
			npd_syslog_dbg("There is no blacklist item\n");
			return g_blacklist_fdb_count;
		}
		if((count >= g_blacklist_fdb_count)||(count >= size)){
            npd_syslog_dbg("Has all items \n");
			return count;
		}
		if(arry[i]==NULL){
			continue;
			}
		else {
			node = arry[i];
			memcpy(blacklist_array[count].mac,node->mac,6);
			blacklist_array[count].vlanid = node->vlanid;
			blacklist_array[count].dmac = node->dmac;
			blacklist_array[count].smac = node->smac;
			npd_syslog_dbg("The blacklist item %d is mac: %02x %02x %02x %02x %02x %02x, vid:%d,dmac:%d,smac:%d\n",
				                        count, node->mac[0],node->mac[1],node->mac[2],node->mac[3],node->mac[4],
				                        node->mac[5],node->vlanid,node->dmac,node->smac);
		}
		count++;
			
	}
	return count;
}
unsigned int npd_fdb_set_port_learn_status
(
    unsigned char slotNum,
	unsigned char portNum,
	unsigned int  status
) 
{  
    unsigned int eth_index = 0;
	unsigned char devNum = 0;
	unsigned char port = 0;
	unsigned int ret = 0;

	if (CHASSIS_SLOTNO_ISLEGAL(slotNum)) {

		if (ETH_LOCAL_PORTNO_ISLEGAL( slotNum,portNum)) {
			eth_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotNum,portNum);
					
		}
		else{
			return FDB_RETURN_CODE_BADPARA;
		}
    }
	else{
         return FDB_RETURN_CODE_BADPARA;
	}
   ret =  npd_get_devport_by_global_index(eth_index,&devNum, &port);
   if(0!= ret){
       npd_syslog_err("npd_fdb_set_port_learn_status:err in npd_get_devport_by_global_index:eth_g_index %d,devNum %d,port %d,ret %d \n",eth_index,devNum,port,ret);
	   return FDB_RETURN_CODE_GENERAL;
   }
   ret = nam_fdb_port_learn_status_set(devNum, port, status);
   if(0!= ret){
       npd_syslog_err("npd_fdb_set_port_learn_status:err in nam_fdb_port_learn_status_set:%d \n",ret);
	   return FDB_RETURN_CODE_OCCUR_HW;
   }

   return NPD_FDB_ERR_NONE;

}

unsigned int npd_fdb_na_msg_per_port_set
(
	unsigned char slotNum,
    unsigned char portNum,
	unsigned int  status
)
{
    unsigned int eth_g_index = 0;
	unsigned char devNum = 0;
	unsigned char port = 0;
	unsigned int ret = 0;
	unsigned short vlanId;
	struct eth_port_s *portInfo = NULL;
	struct port_based_vlan_s  *pvid = NULL;
	struct eth_port_dot1q_list_s *dot1qNode = NULL;
	dot1q_vlan_list_s *dot1qList = NULL;
	struct list_head *list = NULL, *pos = NULL;
	struct vlan_s *vlanNode = NULL;

	
	if (CHASSIS_SLOTNO_ISLEGAL(slotNum)) 
	{
		if (ETH_LOCAL_PORTNO_ISLEGAL( slotNum,portNum)) 
		{
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotNum,portNum);
            /*enable port NA*/
            ret =  npd_get_devport_by_global_index(eth_g_index,&devNum, &port);
            if(NPD_SUCCESS != ret)
			{
                npd_syslog_err("npd_fdb_set_port_learn_status:err in npd_get_devport_by_global_index:%d \n",ret);
	            return FDB_RETURN_CODE_GENERAL;
            }
            ret = nam_fdb_na_msg_per_port_set( devNum,port, status);
            if(0!= ret)
			{
                 npd_syslog_err("npd_fdb_set_port_learn_status:err in nam_fdb_port_learn_status_set:%d \n",ret);
	             return FDB_RETURN_CODE_OCCUR_HW;
            }

			if( 1==status )
			{/* if enable na */
			/*enable Vlan na*/
				portInfo = g_eth_ports[eth_g_index];
			    if(NULL == portInfo) 
				{
					syslog_ax_eth_port_err("npd eth port %#0x clear pvid port null",eth_g_index);
				    ret = FDB_RETURN_CODE_GENERAL;
					/*code optimize:  Derference after null check
					zhangcl@autelan.com 2013-1-17*/
					return ret;
			    }
				dot1qList = portInfo->funcs.func_data[ETH_PORT_FUNC_DOT1Q_BASED_VLAN];
				if(NULL != dot1qList)
				{
			    	list = &(dot1qList->list);
					__list_for_each(pos,list)
					{
						dot1qNode = list_entry(pos,struct eth_port_dot1q_list_s,list);
					    vlanId = dot1qNode->vid;
			            ret = nam_fdb_na_msg_vlan_set(vlanId,status);
						if(0 != ret)
						{
					 		npd_syslog_err("The vlan %d set NA msg failed\n",vlanId);
					        return FDB_RETURN_CODE_OCCUR_HW;
					    }
				    }
			    }
				pvid = portInfo->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN];
				if(NULL != pvid) 
				{
					vlanId = pvid->vid;
					/*code optimize:  Unused pointer value vlannode
					zhangcl@autelan.com 2013-1-17*/
					#if 0
					vlanNode = npd_find_vlan_by_vid(vlanId);
					#endif
					ret = nam_fdb_na_msg_vlan_set(vlanId,status);
					if(0 != ret)
					{
						npd_syslog_err("The vlan %d set NA msg failed,return value is %d\n",vlanId,ret);
				        return FDB_RETURN_CODE_OCCUR_HW;
				    }
				} 
			}
		}
		else 
		{
			ret = FDB_RETURN_CODE_BADPARA;
		}
   }
   else 
   {
	   ret = FDB_RETURN_CODE_BADPARA;
   }

   return NPD_FDB_ERR_NONE;
}


unsigned int npd_fdb_na_msg_per_vlan_set
(
     unsigned short vlanId,
     unsigned int  status,
     unsigned int  flag
)
{    
	 unsigned char devNum =0;
	 unsigned char  portNum = 0;
	 unsigned int i =0;
	 unsigned int ret = 0;
	 unsigned char isTagged = 0;
	 unsigned int  mbr_count =0;
	 unsigned int eth_g_index [64]={0};

     /*check vlan */
     if(NULL == npd_find_vlan_by_vid(vlanId)){
	 	npd_syslog_err("The vlan number %d does not exist.\n",vlanId);
		ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST; /* number = 5*/
		return ret;
	 }
	 /* set the vlan na msg status */
	 ret = nam_fdb_na_msg_vlan_set(vlanId,status);
	 if(0 != ret){
	 	npd_syslog_err("The vlan %d set NA msg failed\n",vlanId);
	    return FDB_RETURN_CODE_GENERAL;
	 }

	 if(1 == flag){
	 /*set the vlan per port na msg status,only considered untagged port*/
	     ret = npd_vlan_member_port_index_get( vlanId,isTagged,eth_g_index,&mbr_count);
	     if(NPD_TRUE == ret){ /* if contains ports*/
	        ret = 0; /*all the sucess return value should be 0*/
	 	    for(i=0;i<mbr_count;i++){
			    ret =  npd_get_devport_by_global_index(eth_g_index[i],&devNum, &portNum);
                if(0!= ret){
                    npd_syslog_err("npd_fdb_set_port_learn_status:err in npd_get_devport_by_global_index:%d \n",ret);
	                return FDB_RETURN_CODE_GENERAL;
                }
                ret = nam_fdb_na_msg_per_port_set( devNum,portNum, status);
                if(0!= ret){
                    npd_syslog_err("npd_fdb_set_port_learn_status:err in nam_fdb_port_learn_status_set:%d \n",ret);
	                return FDB_RETURN_CODE_BADPARA;
                }
	 	    }
	    }
		isTagged = 1;
		ret = npd_vlan_member_port_index_get( vlanId,isTagged,eth_g_index,&mbr_count);
        if(NPD_TRUE == ret){ /* if contains ports*/
	 	    for(i=0;i<mbr_count;i++){
			    ret =  npd_get_devport_by_global_index(eth_g_index[i],&devNum, &portNum);
                if(0!= ret){
                    npd_syslog_err("npd_fdb_set_port_learn_status:err in npd_get_devport_by_global_index:%d \n",ret);
	                return FDB_RETURN_CODE_GENERAL;
                }
                ret = nam_fdb_na_msg_per_port_set( devNum,portNum, status);
                if(0!= ret){
                    npd_syslog_err("npd_fdb_set_port_learn_status:err in nam_fdb_port_learn_status_set:%d \n",ret);
	                return FDB_RETURN_CODE_BADPARA;
                }
	 	    }
	   }
	 }
	/*TODO: if in the process new port untagged added, to deal with in the vlan port member add func?*/
	/* Has implement the func in the vlan port member add/del*/
	 return NPD_FDB_ERR_NONE;
}

unsigned int npd_get_slot_index_from_eth_index
(
    unsigned int eth_index
)
{
    unsigned int slot_index;

	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_index);
	return slot_index;
}
 unsigned int npd_get_port_index_from_eth_index
(
    unsigned int eth_index
)
{
    unsigned int port_index;

	port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_index);
	return port_index;
}

unsigned int  npd_get_slot_no_from_index
(
     unsigned int slot_index
)
{   
    unsigned int slot_no;

	slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
	return slot_no;
}

unsigned int npd_get_port_no_from_index
(
   unsigned int slot_index,
   unsigned int port_index
)
{
    unsigned int port_no;
	
	port_no = ETH_LOCAL_INDEX2NO(slot_index,port_index);
	return port_no;
}
int npd_fdb_parse_mac_addr(char* input,char* macAddr) {
 	
	int i = 0;
	char cur = 0,value = 0;
	
	if((NULL == input)||(NULL == macAddr)) {
		return NPD_FAIL;
	}
	
	for(i = 0; i <6;i++) {
		cur = *(input++);
		if((cur == ':') ||(cur == '-')){
			i--;
			continue;
		}
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
		macAddr[i] = value;
		cur = *(input++);	
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
		macAddr[i] = (macAddr[i]<< 4)|value;
	}
	
	return NPD_SUCCESS;
}

int npd_fdb_check_limit_exist
(
    unsigned int dev
)
{
		unsigned char slot_no =0;
		unsigned int slot_index = 0;
		unsigned int local_port_index = 0;
		unsigned char local_port_no =0;
		unsigned int fdblimit=0;
		unsigned int ret =0;
		unsigned int count = 0;
		int i = 0;
        struct vlan_port_list_node_s *vlan_node = NULL;
	    struct vlan_s *vlanNode = NULL;
	    vlan_port_list_s *portList = NULL;
	    struct list_head  *list = NULL,*pos = NULL; 

		/*for the vlan-based*/
	    for(i = 1;i<4095;i++){
		  ret = npd_check_vlan_exist(i);
		  if(VLAN_RETURN_CODE_VLAN_EXISTS == ret){
		     ret = npd_fdb_number_vlan_set_check(i,&fdblimit);
			 if(1 == ret){ 
			     return NPD_TRUE;
			 }
			 /*for the vlan-port-based*/
			 vlanNode = npd_find_vlan_by_vid(i);
	         if(NULL == vlanNode) {
		        return NPD_FAIL;
	         }

	         /* tagged port list*/
		      portList = vlanNode->tagPortList;
              if((NULL != portList)&&(0 != portList->count)){
		        list = &(portList->list);
		        __list_for_each(pos,list) {
			       vlan_node = list_entry(pos,struct vlan_port_list_node_s,list);
				   	  slot_index=SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(vlan_node->eth_global_index);
					  slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
					  local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(vlan_node->eth_global_index);
					  local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);
					  ret = npd_fdb_number_vlan_port_set_check(i,slot_no,local_port_no,&fdblimit);
                      if(1==ret){ 
			             return NPD_TRUE; 
                      }
		       }
	        }
			 portList = vlanNode->untagPortList;
			 if((NULL != portList)&&(0 != portList->count))
             {
		        list = &(portList->list);
		        __list_for_each(pos,list) {
			       vlan_node = list_entry(pos,struct vlan_port_list_node_s,list);
				   slot_index=SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(vlan_node->eth_global_index);
				   slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
				   local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(vlan_node->eth_global_index);
				   local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);
				   ret = npd_fdb_number_vlan_port_set_check(i,slot_no,local_port_no,&fdblimit);
				   if(1==ret){ 
				   	  return NPD_TRUE;
				   }    
		       }
	        }

	      }
		  else {continue;}
	   }

		/*for the port-base*/
		for(slot_no = CHASSIS_SLOT_START_NO;slot_no < (CHASSIS_SLOT_COUNT+CHASSIS_SLOT_START_NO);slot_no++){
			if(CHASSIS_SLOTNO_ISLEGAL(slot_no)){
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				for(local_port_no = ETH_LOCAL_PORT_START_NO(slot_index);local_port_no <(ETH_LOCAL_PORT_COUNT(slot_index)+ETH_LOCAL_PORT_START_NO(slot_index));local_port_no++){
                    if(ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)){
						ret = npd_fdb_number_port_set_check(slot_no,local_port_no,&fdblimit);
						if(1 == ret){ 
                           return NPD_TRUE;
						}
                    }
				}
			}
		}	
	  return NPD_FALSE;
}


/********************************************************************/
/* functions for DLDP															*/
/********************************************************************/
/**********************************************************************************
 * npd_fdb_add_dldp_vlan_system_mac()
 *	DESCRIPTION:
 *		add a FDB item(system mac and vlanId) when enable DLDP on vlan
 *
 *	INPUTS:
 *		unsigned short vlanId
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		NPD_FDB_ERR_VLAN_NONEXIST	- vlan not exist
 *		NPD_FDB_ERR_NONE			- success
 *		NPD_FDB_ERR_OCCUR_HW		- error when add FDB item to table FDB
***********************************************************************************/
unsigned int npd_fdb_add_dldp_vlan_system_mac
(
	unsigned short vlanId
)
{
	unsigned int ret = NPD_FDB_ERR_NONE;
	unsigned int if_flag = NPD_FALSE;
	unsigned int ifindex = 0;

	if (NULL == npd_find_vlan_by_vid(vlanId)) {
		npd_syslog_err("add dldp fdb, vlan %d not exist\n", vlanId);
		ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
	}else {
		ret = npd_vlan_interface_check(vlanId, &ifindex);
		if ((NPD_TRUE == ret) && (ifindex != (~0UI))) {
			npd_syslog_dbg("add dldp fdb, vlan %d is L3 vlan interface %d\n", vlanId, ifindex);
            if_flag = NPD_TRUE;
		}

        ret = nam_fdb_static_system_source_mac_add(vlanId, if_flag);
		if (FDB_CONFIG_SUCCESS != ret) {
			npd_syslog_err("add dldp fdb vlan %d, HW error\n", vlanId);
			ret = FDB_RETURN_CODE_OCCUR_HW;
		}
	}

	return ret;
}

/**********************************************************************************
 * npd_fdb_del_dldp_vlan_system_mac()
 *	DESCRIPTION:
 *		del the  FDB item(system mac and vlanId) when enable DLDP on vlan
 *
 *	INPUTS:
 *		unsigned short vlanId
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		NPD_FDB_ERR_VLAN_NONEXIST	- vlan not exist
 *		NPD_FDB_ERR_NONE			- success
 *		NPD_FDB_ERR_OCCUR_HW		- error when del FDB item to table FDB
***********************************************************************************/
unsigned int npd_fdb_del_dldp_vlan_system_mac
(
	unsigned short vlanId
)
{
	unsigned int ret = NPD_FDB_ERR_NONE;
	unsigned int if_flag = NPD_FALSE;
	unsigned int ifindex = 0;

	if (NULL == npd_find_vlan_by_vid(vlanId)) {
		npd_syslog_err("del dldp fdb, vlan %d not exist\n", vlanId);
		ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
	}else {
		ret = npd_vlan_interface_check(vlanId, &ifindex);
		if ((NPD_TRUE == ret) && (ifindex != (~0UI))) {
			npd_syslog_dbg("del dldp fdb, vlan %d is L3 vlan interface %d\n", vlanId, ifindex);
            if_flag = NPD_TRUE;
		}

        ret = nam_fdb_static_system_source_mac_del(vlanId, if_flag);
		if (FDB_CONFIG_SUCCESS != ret) {
			npd_syslog_err("del dldp fdb vlan %d, HW error\n", vlanId);
			ret = FDB_RETURN_CODE_OCCUR_HW;
		}
	}

	return ret;
}



/**********************************************************************************
 *  npd_fdb_create_for_vrrp
 *
 *	DESCRIPTION:
 * 		this routine create vrrp and set static FDB
 *
 *	INPUT:
 *		ifindex	-- intf id
 *		name  -- dev name
 *		addr	  --  system mac address	
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_fdb_create_for_vrrp
(
	unsigned char* ifname
)
{
	int ret = 0;

	unsigned int ifIndex = 0,vid = 0,eth_g_index = 0;
	int type;

	if(NPD_OK != (ret = npd_intf_get_info(ifname,&ifIndex,&vid,&eth_g_index,&type))){
	   syslog_ax_route_err("get interface %s info error\n",ifname);
	   /*code optimize:  Missing return statement
		zhangcl@autelan.com 2013-1-17*/
	   return ret;
	}
	if(0 == ret)
	{
		if ( 0 != (ret = nam_static_fdb_entry_mac_set_for_vrrp (vid))) {
			syslog_ax_intf_err("set static mac addr ERROR ret %d\n",ret);				
			ret = NPD_DBUS_ERROR;
		}
	}

	return ret;
}


/**********************************************************************************
 *  npd_fdb_del_for_vrrp
 *
 *	DESCRIPTION:
 * 		this routine del vrrp and set static FDB
 *
 *	INPUT:
 *		ifindex	-- intf id
 *		name  -- dev name
 *		addr	  --  system mac address	
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR
 *		NPD_SUCCESS
 *		
 *
 **********************************************************************************/
int npd_fdb_del_for_vrrp
(
	unsigned char* ifname
)
{
	int ret = 0;

	unsigned int ifIndex = 0,vid = 0,eth_g_index = 0;
	int type;

	if(NPD_OK != (ret = npd_intf_get_info(ifname,&ifIndex,&vid,&eth_g_index,&type))){
	   syslog_ax_route_err("get interface %s info error\n",ifname);
	   /*code optimize:  Missing return statement
		zhangcl@autelan.com 2013-1-17*/
	   return ret;
	}
	if(0 == ret)
	{
		if ( 0 != (ret = nam_static_fdb_entry_mac_del_for_vrrp (vid))) {
			syslog_ax_intf_err("set static mac addr ERROR ret %d\n",ret);				
			ret = NPD_DBUS_ERROR;
		}
	}

	return ret;
}

/***************************************************************************************************
 *		NPD dbus operation
 *	
 ***************************************************************************************************/
 
DBusMessage * npd_dbus_fdb_config_agingtime(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	/*unsigned short	agingtime = 0;*/
	unsigned int	ret;
	DBusError err;
	
	/*npd_syslog_dbg(("Entering config no agingtime!\n"));*/

	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&g_agingtime,
										DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					npd_syslog_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
			return NULL;
		}
		npd_syslog_dbg("configure fdb table agingtime %d . \n",g_agingtime);
		if (CHASSIS_AGINGTIME_ISLEGAL(g_agingtime)) {
			 ;/*npd_syslog_dbg("legal agingtime %d",g_agingtime);*/
		}
		else {
			 npd_syslog_dbg("Illegal agingtime %d",g_agingtime);
			 ret = FDB_RETURN_CODE_BADPARA;
			/*better to tell user to input vlanId again*/
		}
		/*
		 *add fdb entry config op
		 *this op set hw chip
		*/
		ret = nam_fdb_table_agingtime_set(g_agingtime);
		if (ret != NPD_FDB_ERR_NONE) {
			 npd_syslog_dbg("npd_dbus_fdb_config_agingtime:: fdb agingtime %d set ERROR. \n",g_agingtime);
			 ret = FDB_RETURN_CODE_OCCUR_HW;
			/*ret = NPD_DBUS_ERROR; in this case dbus message can not reply*/ 
		}
		ret = NPD_FDB_ERR_NONE;
		/*to update the fdb info data struct in sw */
		/*fdb active op ,need to modify the vid_list only*/
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &g_agingtime);
	
	return reply;
	
}


DBusMessage * npd_dbus_fdb_no_config_agingtime(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	/*unsigned short	agingtime;*/
	unsigned int	ret;
	DBusError err;
	
	/*npd_syslog_dbg(("Entering config no agingtime!\n"));*/

	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&g_agingtime,
										DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					npd_syslog_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
			return NULL;
		}
		/*npd_syslog_dbg(("configure fdb table agingtime %d . \n",agingtime));*/
		
		if (CHASSIS_AGINGTIME_ISLEGAL(g_agingtime)) {
			;/*npd_syslog_err("legal agingtime %d",g_agingtime);*/
		}
		else {
			npd_syslog_err("Illegal agingtime %d",g_agingtime);
			ret = FDB_RETURN_CODE_BADPARA;
			/*better to tell user to input vlanId again*/
		}
		/*
		 *add fdb entry config op
		 *this op set hw chip
		*/
		ret = nam_fdb_table_agingtime_set(g_agingtime);

		if (ret != NPD_FDB_ERR_NONE) {
			 npd_syslog_dbg("npd_dbus_fdb_config_agingtime:: fdb agingtime %d default ERROR. \n",g_agingtime);
	         ret = FDB_RETURN_CODE_OCCUR_HW;
			/*ret = NPD_DBUS_ERROR; in this case dbus message can not reply*/ 
		}
		ret = NPD_FDB_ERR_NONE;
		/*to update the fdb info data struct in sw */
		/*fdb active op ,need to modify the vid_list only*/
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&g_agingtime  );
	return reply;
	
}

DBusMessage * npd_dbus_fdb_delete_blacklist(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned short	vlanId;
	unsigned char   macAddr[6] = {0};
	char macComp[13] = {0};
	int result = 0;
	unsigned int	ret = 0;
	unsigned int flag = 0;
	struct vlan_s *vlanNode = NULL;
	DBusError err;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_UINT32,&flag,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_BYTE, &macAddr[0],
							DBUS_TYPE_BYTE, &macAddr[1],
							DBUS_TYPE_BYTE, &macAddr[2],
							DBUS_TYPE_BYTE, &macAddr[3],
							DBUS_TYPE_BYTE, &macAddr[4],
							DBUS_TYPE_BYTE, &macAddr[5],
							DBUS_TYPE_INVALID))) 
	{
		npd_syslog_err("Unable to get input args!\n ");
		if (dbus_error_is_set(&err)) 
		{
			npd_syslog_err("%s raised: %s\n",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	else 
	{
		sprintf(macComp,"%02x%02x%02x%02x%02x%02x",	\
				macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
		result = npd_system_verify_basemac(macComp);
		if(0 == result)
		{
			/*code optimize:  Function address comparison
			zhangcl@autelan.com 2013-1-17*/
			vlanNode=npd_find_vlan_by_vid(vlanId);
			if(vlanNode == NULL)
			{
				ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
			}
			else
			{
				ret = npd_fdb_static_blacklist_entry_del ((char *)macAddr,vlanId,flag);
				if(ret != NPD_FDB_ERR_NONE)
				{
					npd_syslog_err("npd_fdb_static_blacklist_entry_del ERROR return value is:%d\n",ret);
                    ret = FDB_RETURN_CODE_GENERAL;
				}
				else
				{
					ret = nam_fdb_entry_mac_vlan_no_drop(macAddr,vlanId,flag);
					if(NPD_FDB_ERR_NONE != ret)
					{
						npd_syslog_err("nam_fdb_entry_mac_vlan_no_drop: fdb entry ERROR. RETURN VALUE IS %d\n",ret);
						ret = FDB_RETURN_CODE_OCCUR_HW;		
					}
				}								
		   } 
	   }
	   else if(1 == result) 
	   {
		  ret = FDB_RETURN_CODE_SYSTEM_MAC;
	   }
    }

	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32, &ret);
	
	return reply;
	
}

DBusMessage * npd_dbus_fdb_delete_blacklist_with_name(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	DBusMessageIter	 iter;
	unsigned short	vlanId = 0;
	struct vlan_s* node = NULL;
	char* vlanname=NULL;
	unsigned int	ret=0;
	unsigned char   macAddr[6] = {0};
	char macComp[13] = {0};
	int result = 0;
	unsigned int   flag =0;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
				DBUS_TYPE_UINT32,&flag,
				DBUS_TYPE_STRING,&vlanname,
				DBUS_TYPE_BYTE, &macAddr[0],
				DBUS_TYPE_BYTE, &macAddr[1],
				DBUS_TYPE_BYTE, &macAddr[2],
				DBUS_TYPE_BYTE, &macAddr[3],
				DBUS_TYPE_BYTE, &macAddr[4],
				DBUS_TYPE_BYTE, &macAddr[5],
				
				DBUS_TYPE_INVALID))) {
				npd_syslog_err("Unable to get input args!\n ");
				if (dbus_error_is_set(&err)) {
						npd_syslog_err("%s raised: %s\n",err.name,err.message);
						dbus_error_free(&err);
					}
				return NULL;
	}

	else {
		sprintf(macComp,"%02x%02x%02x%02x%02x%02x",	\
					macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
		result = npd_system_verify_basemac(macComp);
		if(0 == result) {
			node= npd_find_vlan_by_name(vlanname);
			if (node == NULL) {
				npd_syslog_err("the vlanname is not register!\n");
				ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
			}
			else {
				vlanId = node->vid;
				if (!(CHASSIS_VLANID_ISLEGAL(vlanId))) {
					npd_syslog_err("illegal vlanId %d\n",vlanId);
					ret = FDB_RETURN_CODE_BADPARA;
				}
						
				else{
					ret = npd_fdb_static_blacklist_entry_del((char *)macAddr,vlanId,flag);
					if(ret != NPD_FDB_ERR_NONE){
						npd_syslog_err("npd_fdb_static_blacklist_entry_del node NOT exist return value is:%d\n",ret);
                        ret = FDB_RETURN_CODE_GENERAL;
					}
					else{
						ret = nam_fdb_entry_mac_vlan_no_drop(macAddr,vlanId,flag);	
						if(ret != NPD_FDB_ERR_NONE){
							npd_syslog_err("nam_fdb_entry_mac_vlan_no_drop: fdb entry ERROR.Return value is %d \n",ret);
                            ret = FDB_RETURN_CODE_OCCUR_HW;
						}
					}
				}	
			}

		}
		else if(1 == result) {
			ret = FDB_RETURN_CODE_SYSTEM_MAC;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32, &ret);
	
	return reply;
	
}



DBusMessage * npd_dbus_fdb_create_blacklist(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	DBusMessageIter	 iter;
	unsigned int   flag =0;
	unsigned short	vlanId;
	unsigned char   macAddr[6] = {0};
	char macComp[13] = {0};
	int result = 0;
	unsigned int	ret=0;
	unsigned int blacklist_node_index = 0, hash_index = 0;
    struct vlan_s * vlanNode = NULL;

	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE))
	{
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE))
	{
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else 
	{
	/*npd_syslog_dbg(("Entering npd_dbus_fdb_mach_mac_drop\n"));*/
		dbus_error_init(&err);
	
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_UINT32,&flag,
			DBUS_TYPE_UINT16,&vlanId,
			DBUS_TYPE_BYTE, &macAddr[0],
			DBUS_TYPE_BYTE, &macAddr[1],
			DBUS_TYPE_BYTE, &macAddr[2],
			DBUS_TYPE_BYTE, &macAddr[3],
			DBUS_TYPE_BYTE, &macAddr[4],
			DBUS_TYPE_BYTE, &macAddr[5],
			DBUS_TYPE_INVALID)))
		{
			npd_syslog_err("Unable to get input args ");
			
			if (dbus_error_is_set(&err)) 
			{
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
	
		else
		{
			sprintf(macComp,"%02x%02x%02x%02x%02x%02x",	\
					macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
			result = npd_system_verify_basemac(macComp);
			if(0 == result)
			{
				if((NULL==(vlanNode = npd_find_vlan_by_vid(vlanId)))||(vlanNode->isAutoCreated))
				{
					ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
			    }
				else
				{
					ret = npd_fdb_static_blacklist_entry_add(macAddr,vlanId,flag,&blacklist_node_index);
					if(ret != NPD_FDB_ERR_NONE)
					{
						if(ret == FDB_RETURN_CODE_NODE_EXIST)
						{
							npd_syslog_err("fdb blacklist exists !!! \n");
						}
						else
						{
							npd_syslog_err("npd_fdb_static_blacklist_entry_add node exist return value is:%d\n",ret);
		                    ret = FDB_RETURN_CODE_GENERAL;
						}
					}
					else
					{
						ret = nam_fdb_entry_mac_vlan_drop(macAddr,vlanId,flag,&hash_index);
						if(0 != ret)
						{
							npd_syslog_err("nam_fdb_entry_mac_vlan_no_drop: fdb entry ERROR. \n");
	                        ret = FDB_RETURN_CODE_OCCUR_HW;
						}
						else
						{
	                        g_blacklist_fdb[blacklist_node_index]->hash_index = hash_index;
						}					
					}
				}
		    } 
			else if(1 == result) 
			{
				ret = FDB_RETURN_CODE_SYSTEM_MAC;
			}
	    }
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,	DBUS_TYPE_UINT32, &ret);
	
	
	return reply;
	
}

DBusMessage * npd_dbus_fdb_create_blacklist_with_vlanname(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	DBusMessageIter	 iter;
	unsigned int    flag = 0;
	unsigned short	vlanId;
	struct vlan_s* node = NULL;
	char* vlanName=NULL;
	unsigned char   macAddr[6] = {0};
	char macComp[13] = {0};
	int result = 0;
	unsigned int blacklist_node_index = 0,hash_index = 0;
	unsigned int	ret=NPD_DBUS_SUCCESS;

	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);
		if (!(dbus_message_get_args ( msg, &err,
													DBUS_TYPE_UINT32,&flag,
													DBUS_TYPE_STRING,&vlanName,
													DBUS_TYPE_BYTE, &macAddr[0],
													DBUS_TYPE_BYTE, &macAddr[1],
													DBUS_TYPE_BYTE, &macAddr[2],
													DBUS_TYPE_BYTE, &macAddr[3],
													DBUS_TYPE_BYTE, &macAddr[4],
													DBUS_TYPE_BYTE, &macAddr[5],
													DBUS_TYPE_INVALID))) {
				npd_syslog_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) {
					npd_syslog_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				return NULL;
		}
		else {
			sprintf(macComp,"%02x%02x%02x%02x%02x%02x",	\
					macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
			result = npd_system_verify_basemac(macComp);
			if(0 == result) {
				node= npd_find_vlan_by_name(vlanName);
				if (node == NULL) {
					npd_syslog_err("the vlanname is not register!\n");
					ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
				}
				else {
					vlanId = node->vid;
					if (!(CHASSIS_VLANID_ISLEGAL(vlanId))) {
						npd_syslog_err("illegal vlanId %d\n",vlanId);
						ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
					}
					else { 
						ret = npd_fdb_static_blacklist_entry_add(macAddr,vlanId,flag,&blacklist_node_index);
						if(ret != NPD_FDB_ERR_NONE){
							npd_syslog_err("npd_fdb_static_entry_del erro return value is:%d\n",ret);
							ret = FDB_RETURN_CODE_GENERAL;
						}
					    else{
							ret = nam_fdb_entry_mac_vlan_drop(macAddr,vlanId,flag,(unsigned int *)&hash_index);
							if(0 != ret){
								npd_syslog_err("nam_fdb_entry_mac_vlan_no_drop: fdb entry ERROR. \n");
	                            ret = FDB_RETURN_CODE_OCCUR_HW;
							}
							else{
		                        g_blacklist_fdb[blacklist_node_index]->hash_index = hash_index;
						    }
						}
					}	
				}

			}
			else if(1 == result) {
				ret = FDB_RETURN_CODE_SYSTEM_MAC;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32, &ret);
	
	
	return reply;
	
}

DBusMessage * npd_dbus_fdb_show_agingtime(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	
	unsigned int	agingtime = g_agingtime ;
	unsigned int ret;
	
 	ret = nam_fdb_table_agingtime_get(&agingtime);

	if (NPD_FDB_ERR_NONE != ret) {
		npd_syslog_err("get  return_value is:%d\n",ret);
		npd_syslog_err("npd_dbus::npd_dbus_fdb_show_agingtime::ERROR.\n");
		ret = FDB_RETURN_CODE_OCCUR_HW;
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32,  &ret);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32,  &g_agingtime);
	return reply;
	
}


DBusMessage * npd_dbus_fdb_config_static_fdb_item(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusError err;
	DBusMessageIter	 iter;
	unsigned char	slot_no = 0,port_no = 0;  
	unsigned short	vlanId = 0;
	unsigned char   macAddr[6] = {0};
	char macComp[13] = {0};
	int result = 0;
	unsigned int	eth_g_index = 0;
	unsigned short  trunkId = 0;
	unsigned int     ret=0;
	unsigned char istag = 0;
    struct vlan_s * vlanNode = NULL;

	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);
		npd_syslog_dbg("enter fdb create static fdb\n");
		if (!(dbus_message_get_args ( msg, &err,
												DBUS_TYPE_UINT16,&vlanId,
												DBUS_TYPE_BYTE, &macAddr[0],
												DBUS_TYPE_BYTE, &macAddr[1],
												DBUS_TYPE_BYTE, &macAddr[2],
												DBUS_TYPE_BYTE, &macAddr[3],
												DBUS_TYPE_BYTE, &macAddr[4],
												DBUS_TYPE_BYTE, &macAddr[5],
												DBUS_TYPE_BYTE,&slot_no,
												DBUS_TYPE_BYTE,&port_no,
												DBUS_TYPE_INVALID))) {
				npd_syslog_err("Unable to get input args!\n ");
				if (dbus_error_is_set(&err)) {
						npd_syslog_err("%s raised: %s\n",err.name,err.message);
						dbus_error_free(&err);
				}
			return NULL;
		}
		else{  
			sprintf(macComp,"%02x%02x%02x%02x%02x%02x",	\
					macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
			result = npd_system_verify_basemac(macComp);
			if(0 == result) {
				if((NULL==(vlanNode = npd_find_vlan_by_vid(vlanId)))||(vlanNode->isAutoCreated)){
					npd_syslog_err("the vlan index no exist\n");
					ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
				}
				else{	
				   if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
					  if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) {
							eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
							ret = npd_vlan_check_contain_port(vlanId,eth_g_index, &istag);	 
							if (ret!=NPD_TRUE){
								npd_syslog_err("the port do not belong to the vlan!\n");
								ret =FDB_RETURN_CODE_NODE_PORT_NOTIN_VLAN;
							}
							else if(NPD_SUCCESS== npd_eth_port_get_ptrunkid(eth_g_index,&trunkId)){
	                            npd_syslog_err("The port belongs to trunk %d!\n",trunkId);
								ret =FDB_RETURN_CODE_NODE_PORT_NOTIN_VLAN;
							}
							else{
								ret =  npd_fdb_static_entry_add(macAddr,vlanId,slot_no,port_no,FDB_STATIC_NOMIRROR);
								if(ret == FDB_RETURN_CODE_NODE_EXIST)
								{
									npd_syslog_err("npd_fdb_static_entry_add erro! return value is %d\n",ret);
									ret = FDB_RETURN_CODE_NODE_EXIST;
								}
								else if(ret != NPD_FDB_ERR_NONE){
									npd_syslog_err("npd_fdb_static_entry_add erro! return value is %d\n",ret);
									ret = FDB_RETURN_CODE_GENERAL;
								}
								else{
								    ret = nam_static_fdb_entry_mac_vlan_port_set(macAddr,vlanId,eth_g_index);
								    if (ret != NPD_FDB_ERR_NONE) {
									    npd_syslog_err("npd_dbus_fdb_config_mac_vlan_port:: fdb entry ERROR. \n");
									    ret = npd_fdb_static_entry_del(macAddr,vlanId);
								        if(ret != NPD_FDB_ERR_NONE){
	                                        npd_syslog_dbg("nam_static_fdb_entry_mac_vlan_port_set failed and npd_fdb_static_entry_del erro return value is:%d\n",ret);
											ret = FDB_RETURN_CODE_OCCUR_HW;
								        }
										ret = FDB_RETURN_CODE_OCCUR_HW;
							        }
								}
							}
					  	}					
					else {
					    npd_syslog_err("the port number is error \n");
						ret = FDB_RETURN_CODE_BADPARA;
					}
					}
					else{
						npd_syslog_err("the slot number is error \n");
						ret = FDB_RETURN_CODE_BADPARA;
					}

				}		
			}
			else if(1 == result) {
				ret = FDB_RETURN_CODE_SYSTEM_MAC;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,	DBUS_TYPE_UINT32, &ret);
	
	return reply;
	
}


 DBusMessage * npd_dbus_fdb_config_static_debug_fdb_item(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	 
	 DBusMessage* reply;
	 DBusError err;
	 DBusMessageIter  iter;
	 unsigned char	 devnum = 0,port = 0;  
	 unsigned short  vlanId = 0;
	 unsigned char	 macAddr[6] = {0};
	 char macComp[13] = {0};
	 int result = 0;
	 unsigned int	  ret=0;
 
	 dbus_error_init(&err);
	 npd_syslog_dbg("enter fdb create static fdb\n");
	 if (!(dbus_message_get_args ( msg, &err,
											 DBUS_TYPE_UINT16,&vlanId,
											 DBUS_TYPE_BYTE, &macAddr[0],
											 DBUS_TYPE_BYTE, &macAddr[1],
											 DBUS_TYPE_BYTE, &macAddr[2],
											 DBUS_TYPE_BYTE, &macAddr[3],
											 DBUS_TYPE_BYTE, &macAddr[4],
											 DBUS_TYPE_BYTE, &macAddr[5],
											 DBUS_TYPE_BYTE,&devnum,
											 DBUS_TYPE_BYTE,&port,
											 DBUS_TYPE_INVALID))) 
	{
		npd_syslog_err("Unable to get input args!\n ");
		if (dbus_error_is_set(&err)) 
		{
			npd_syslog_err("%s raised: %s\n",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	else
	{	
		 sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
				 macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
		 result = npd_system_verify_basemac(macComp);
		 if(0 == result) 
		 {
			 ret = nam_static_fdb_entry_mac_vlan_port_set_for_debug(macAddr,vlanId,devnum,port);
			 if (ret != NPD_FDB_ERR_NONE) 
			 {
				 ret = FDB_RETURN_CODE_OCCUR_HW;
			 }		 
		 }
		 else if(1 == result) 
		 {
			 ret = FDB_RETURN_CODE_SYSTEM_MAC;
		 }
	 }
	 reply = dbus_message_new_method_return(msg);
	 
	 dbus_message_iter_init_append (reply, &iter);
	 
	 dbus_message_iter_append_basic (&iter,  DBUS_TYPE_UINT32, &ret);
	 
	 return reply;
	 
 }

 DBusMessage * npd_dbus_fdb_config_static_fdb_save(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	 
	 DBusMessage* reply;
	 DBusError err;
	 DBusMessageIter  iter;
	 unsigned char	 slot_no = 0,port_no = 0,trunkId = 0,flag = 0,flag_type = 0;  
	 unsigned short  vlanId = 0;
	 unsigned char	 macAddr[6] = {0};
	 unsigned int	  ret=0;
	 unsigned int blacklist_node_index = 0;

 

	 dbus_error_init(&err);
	 npd_syslog_dbg("enter fdb create static fdb\n");
	 if (!(dbus_message_get_args ( msg, &err,
											 DBUS_TYPE_UINT16,&vlanId,
											 DBUS_TYPE_BYTE, &macAddr[0],
											 DBUS_TYPE_BYTE, &macAddr[1],
											 DBUS_TYPE_BYTE, &macAddr[2],
											 DBUS_TYPE_BYTE, &macAddr[3],
											 DBUS_TYPE_BYTE, &macAddr[4],
											 DBUS_TYPE_BYTE, &macAddr[5],
											 DBUS_TYPE_BYTE,&slot_no,
											 DBUS_TYPE_BYTE,&port_no,
											 DBUS_TYPE_BYTE,&trunkId,
											 DBUS_TYPE_BYTE,&flag,
											 DBUS_TYPE_BYTE,&flag_type,
											 DBUS_TYPE_INVALID))) {
			 npd_syslog_err("Unable to get input args!\n ");
			 if (dbus_error_is_set(&err)) {
					 npd_syslog_err("%s raised: %s\n",err.name,err.message);
					 dbus_error_free(&err);
			 }
		 return NULL;
	 }
	 else
	 {
	 	if(flag_type == 0)
	 	{
			 ret =	npd_fdb_static_entry_add(macAddr,vlanId,slot_no,port_no,FDB_STATIC_NOMIRROR);
			 if(ret != NPD_FDB_ERR_NONE)
			 {
				 npd_syslog_err("npd_fdb_static_entry_add erro! return value is %d\n",ret);
				 ret = FDB_RETURN_CODE_GENERAL;
			 }
	 	}
		else if(flag_type == 1)
		{
			 ret =	npd_fdb_static_trunk_add(macAddr,vlanId,trunkId);
			 if(ret != NPD_FDB_ERR_NONE)
			 {
				 npd_syslog_err("npd_fdb_static_trunk_add erro!Return value is %d \n",ret);
                 ret = FDB_RETURN_CODE_GENERAL;
			 }
		}
		else if(flag_type == 2)
		{
			ret = npd_fdb_static_blacklist_entry_add(macAddr,vlanId,flag,&blacklist_node_index);
			if(ret != NPD_FDB_ERR_NONE){
				npd_syslog_err("npd_fdb_static_blacklist_entry_add node exist return value is:%d\n",ret);
                ret = FDB_RETURN_CODE_GENERAL;
			}
		}
		else if(flag_type == 3)
		{
			ret = npd_fdb_static_entry_del_by_port(slot_no,port_no);
			if( 0 != ret){
                npd_syslog_err("Delete fdb static by port error:%d\n",ret);
                ret = FDB_RETURN_CODE_GENERAL;
			}
		}
		else if(flag_type == 4)
		{
			ret = npd_fdb_static_entry_del(macAddr,vlanId);
			if(ret != NPD_FDB_ERR_NONE) {
				npd_syslog_err("npd_fdb_static_entry_del erro return value is:%d\n",ret);
				ret = FDB_RETURN_CODE_GENERAL;
			}
		}
		else if(flag_type == 5)
		{
			ret = npd_fdb_static_entry_del_by_vlan(vlanId);
			if( NPD_FDB_ERR_NONE != ret){
                npd_syslog_err("Delete fdb static by vlan in sw error: %d\n",ret);
                ret = FDB_RETURN_CODE_GENERAL;
			}
		}
	 }
	 
	 reply = dbus_message_new_method_return(msg);
	 
	 dbus_message_iter_init_append (reply, &iter);
	 
	 dbus_message_iter_append_basic (&iter,  DBUS_TYPE_UINT32, &ret);
	 
	 return reply;
	 
 }


  DBusMessage * npd_dbus_fdb_check_fdb_static_exists(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	 
	 DBusMessage* reply;
	 DBusError err;
	 DBusMessageIter  iter;
	 unsigned char	 slot_no = 0,port_no = 0;  
	 unsigned short  vlanId = 0;
	 unsigned char	 macAddr[6] = {0};
	 unsigned int	  ret=0;
	 unsigned int blacklist_node_index = 0;

 

	 dbus_error_init(&err);
	 if (!(dbus_message_get_args ( msg, &err,
											 DBUS_TYPE_UINT16,&vlanId,
											 DBUS_TYPE_BYTE, &macAddr[0],
											 DBUS_TYPE_BYTE, &macAddr[1],
											 DBUS_TYPE_BYTE, &macAddr[2],
											 DBUS_TYPE_BYTE, &macAddr[3],
											 DBUS_TYPE_BYTE, &macAddr[4],
											 DBUS_TYPE_BYTE, &macAddr[5],
											 DBUS_TYPE_INVALID))) {
			 npd_syslog_err("Unable to get input args!\n ");
			 if (dbus_error_is_set(&err)) {
					 npd_syslog_err("%s raised: %s\n",err.name,err.message);
					 dbus_error_free(&err);
			 }
		 return NULL;
	 }
	 else
	 {
	 	ret = npd_fdb_check_static_entry_exist(macAddr,vlanId,&slot_no,&port_no);
		if(ret != FDB_RETURN_CODE_NODE_EXIST)
		{
			npd_syslog_err("static fdb did not exists !\n");
		}
	 }
	 
	 reply = dbus_message_new_method_return(msg);
	 
	 dbus_message_iter_init_append (reply, &iter);
	 
	 dbus_message_iter_append_basic (&iter,  DBUS_TYPE_UINT32, &ret);
	 dbus_message_iter_append_basic (&iter,  DBUS_TYPE_BYTE, &slot_no);
	 dbus_message_iter_append_basic (&iter,  DBUS_TYPE_BYTE, &port_no);
	 
	 return reply;
	 
 }
 

 
 DBusMessage * npd_dbus_fdb_config_static_fdb_trunk_item(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	 
	 DBusMessage* reply;
	 DBusError err;
	 DBusMessageIter  iter;
	 unsigned short  trunkId = 0; 
	 unsigned short  vlanId = 0;
	 unsigned char  isTagged = 0;
	 unsigned char	 macAddr[6] = {0};
	 char macComp[13] = {0};
	 int result = 0;
	 unsigned int	  ret=0;
	 struct trunk_s *trunkNode = NULL;
     struct vlan_s * vlanNode = NULL;

	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
	 dbus_error_init(&err);
	 npd_syslog_dbg("enter fdb create static fdb trunk\n");
	 if (!(dbus_message_get_args ( msg, &err,
											 DBUS_TYPE_UINT16,&vlanId,
											 DBUS_TYPE_UINT16,&trunkId,
											 DBUS_TYPE_BYTE, &macAddr[0],
											 DBUS_TYPE_BYTE, &macAddr[1],
											 DBUS_TYPE_BYTE, &macAddr[2],
											 DBUS_TYPE_BYTE, &macAddr[3],
											 DBUS_TYPE_BYTE, &macAddr[4],
											 DBUS_TYPE_BYTE, &macAddr[5],
											 DBUS_TYPE_INVALID))) {
			 npd_syslog_err("Unable to get input args!\n ");
			 if (dbus_error_is_set(&err)) {
					 npd_syslog_err("%s raised: %s\n",err.name,err.message);
					 dbus_error_free(&err);
			 }
		 return NULL;
	 }
	 else{	
		 sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
				 macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
		 result = npd_system_verify_basemac(macComp);
		 if(0 == result) {
			 if((NULL==(vlanNode = npd_find_vlan_by_vid(vlanId)))||(vlanNode->isAutoCreated)){
				 npd_syslog_err("the vlan index no exist\n");
				 ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
			 }
			 else{	
			 	   #if 0 
				   if (NPD_TRUNK_EXISTS != npd_check_trunk_exist(trunkId)) {
				   	   npd_syslog_err("Error trunk no input!!\n");
					   ret = FDB_RETURN_CODE_NODE_NOT_EXIST;
				   	}
				   #endif
				   trunkNode = npd_find_trunk(trunkId);
				   if(NULL == trunkNode){
					   npd_syslog_err("Error trunk no input!!\n");
					   ret = FDB_RETURN_CODE_NODE_NOT_EXIST;
				   }
				   else if(NULL == trunkNode->portList){
					   npd_syslog_err("No member in trunk!!\n");
					   ret = NPD_FDB_ERR_PORT_NOTIN_VLAN;

				   }
				   else if((0 == trunkNode->portList->count)){
					   npd_syslog_err("No member in trunk!!\n");
					   ret = NPD_FDB_ERR_PORT_NOTIN_VLAN;
				   }
				   else{
				   	   if((npd_vlan_check_contain_trunk(vlanId,trunkId,&isTagged))||    \
					         	(1 == vlanId)){
							 ret =	npd_fdb_static_trunk_add((unsigned char *)macAddr,vlanId,trunkId);
							 if(ret != NPD_FDB_ERR_NONE){
								 npd_syslog_err("npd_fdb_static_trunk_add erro!Return value is %d \n",ret);
								 if(ret == FDB_RETURN_CODE_NODE_EXIST)
								 {
								 	ret = FDB_RETURN_CODE_NODE_EXIST;
								 }
								 else
								 {
                                 	ret = FDB_RETURN_CODE_GENERAL;
								 }
							 }
							 else{
								 npd_syslog_dbg("npd_fdb_static_trunk_add OK!The static fdb trunk number is %d. \n",g_static_fdb_count);
								 ret = nam_static_fdb_entry_mac_vlan_trunk_set((unsigned char *)macAddr,vlanId,trunkId);
								 if (ret != NPD_FDB_ERR_NONE) {
									 npd_syslog_err("npd_dbus_fdb_config_mac_vlan_trunk:: fdb entry ERROR. \n");
									 ret = npd_fdb_static_trunk_del(macAddr,vlanId);
									 if(ret != NPD_FDB_ERR_NONE){
										npd_syslog_dbg("npd_fdb_static_trunk_del erro return value is:%d\n",ret);
                                        ret = FDB_RETURN_CODE_OCCUR_HW;
									 }
									 ret = FDB_RETURN_CODE_OCCUR_HW;
								 }
							 }
						}
					   else {
                            npd_syslog_err("The vlan does not contain the trunk\n");
							ret = NPD_FDB_ERR_PORT_NOTIN_VLAN;
					   }
					}
			 	}
 
			 }		 
		 else if(1 == result) {
			 ret = FDB_RETURN_CODE_SYSTEM_MAC;
		 }
	 }
	}
	 reply = dbus_message_new_method_return(msg);
	 
	 dbus_message_iter_init_append (reply, &iter);
	 
	 dbus_message_iter_append_basic (&iter,  DBUS_TYPE_UINT32, &ret);
	 
	 return reply;
	 
 }


  DBusMessage * npd_dbus_fdb_config_static_debug_fdb_trunk_item(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	 
	 DBusMessage* reply;
	 DBusError err;
	 DBusMessageIter  iter;
	 unsigned short  trunkId = 0; 
	 unsigned short  vlanId = 0;
	 unsigned char  isTagged = 0;
	 unsigned char	 macAddr[6] = {0};
	 char macComp[13] = {0};
	 int result = 0;
	 unsigned int	  ret=0;
	 struct trunk_s *trunkNode = NULL;
	 struct vlan_s * vlanNode = NULL;


	 dbus_error_init(&err);
	 npd_syslog_dbg("enter fdb create static fdb trunk\n");
	 if (!(dbus_message_get_args ( msg, &err,
											 DBUS_TYPE_UINT16,&vlanId,
											 DBUS_TYPE_UINT16,&trunkId,
											 DBUS_TYPE_BYTE, &macAddr[0],
											 DBUS_TYPE_BYTE, &macAddr[1],
											 DBUS_TYPE_BYTE, &macAddr[2],
											 DBUS_TYPE_BYTE, &macAddr[3],
											 DBUS_TYPE_BYTE, &macAddr[4],
											 DBUS_TYPE_BYTE, &macAddr[5],
											 DBUS_TYPE_INVALID))) 
	{
		npd_syslog_err("Unable to get input args!\n ");
		if (dbus_error_is_set(&err)) 
		{
			npd_syslog_err("%s raised: %s\n",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	else
	{	
		sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
				 macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
		result = npd_system_verify_basemac(macComp);
		if(0 == result) 
		{
			ret = nam_static_fdb_entry_mac_vlan_trunk_set((unsigned char *)macAddr,vlanId,trunkId);
			if (ret != NPD_FDB_ERR_NONE) 
			{
				npd_syslog_err("npd_dbus_fdb_config_mac_vlan_trunk:: fdb entry ERROR. \n");
				ret = FDB_RETURN_CODE_OCCUR_HW;
			}
		 }
		 else if(1 == result) 
		 {
			 ret = FDB_RETURN_CODE_SYSTEM_MAC;
		 }
	}
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic (&iter,  DBUS_TYPE_UINT32, &ret);

	return reply;
	 
 }
 
  
  DBusMessage * npd_dbus_fdb_config_static_fdb_trunk_with_name(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	  
	  DBusMessage* reply;
	  DBusError err;
	  DBusMessageIter  iter;
	  unsigned short  trunkId = 0;
	  unsigned short  vlanId = 0;
      char*  vlanname = NULL;
	  unsigned char  isTagged = 0;
	  char   macAddr[6] = {0};
	  char macComp[13] = {0};
	  int result = 0;
	  unsigned int	   ret=NPD_DBUS_ERROR;
	  struct vlan_s* node= NULL;
	  struct trunk_s* trunkNode = NULL;

	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
	  dbus_error_init(&err);
	  if (!(dbus_message_get_args ( msg, &err,
											  DBUS_TYPE_STRING,&vlanname,
											  DBUS_TYPE_UINT16,&trunkId,
											  DBUS_TYPE_BYTE, &macAddr[0],
											  DBUS_TYPE_BYTE, &macAddr[1],
											  DBUS_TYPE_BYTE, &macAddr[2],
											  DBUS_TYPE_BYTE, &macAddr[3],
											  DBUS_TYPE_BYTE, &macAddr[4],
											  DBUS_TYPE_BYTE, &macAddr[5],
											  DBUS_TYPE_INVALID))) {
			  npd_syslog_err("Unable to get input args!\n ");
			  if (dbus_error_is_set(&err)) {
					  npd_syslog_err("%s raised: %s\n",err.name,err.message);
					  dbus_error_free(&err);
			  }
			  return NULL;
		  
	  }
	  else{  
		  sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
				  macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
		  result = npd_system_verify_basemac(macComp);
		  if(0 == result) {
		  	  node = npd_find_vlan_by_name(vlanname);
			  if(NULL==node){
				  npd_syslog_err("the vlan index no exist\n");
				  ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
			  }
			  else{   
				   if(NULL == (trunkNode = npd_find_trunk(trunkId))){
					   npd_syslog_err("Error trunk no input!!\n");
					   ret = FDB_RETURN_CODE_NODE_NOT_EXIST;
				   }
				   else if((NULL ==trunkNode->portList)||(0 == trunkNode->portList->count)){
					   npd_syslog_err("No member in trunk!!\n");
					   ret = NPD_FDB_ERR_PORT_NOTIN_VLAN;
				   }
					else{
						vlanId = node->vid;
						if((npd_vlan_check_contain_trunk(vlanId,trunkId,&isTagged))||	 \
														(!(npd_vlan_check_contain_trunk(vlanId,trunkId,&isTagged))&&(1 == vlanId))){
							  ret =  npd_fdb_static_trunk_add((unsigned char *)macAddr,vlanId,trunkId);
							  if(ret != NPD_FDB_ERR_NONE){
								  npd_syslog_err("npd_fdb_static_trunk_add erro! Return value is %d\n",ret);
								  ret = FDB_RETURN_CODE_GENERAL;
							  }
							  else{
								  /*npd_syslog_dbg("npd_fdb_static_trunk_add OK!The static fdb trunk number is %d. \n",g_static_fdb_count);*/
								  ret = nam_static_fdb_entry_mac_vlan_trunk_set((unsigned char *)macAddr,vlanId,trunkId);
								  if (ret != NPD_FDB_ERR_NONE) {
									  npd_syslog_err("npd_dbus_fdb_config_mac_vlan_trunk:: fdb entry ERROR. \n");
									  ret = npd_fdb_static_trunk_del(macAddr,vlanId);
									  if(ret != NPD_FDB_ERR_NONE){
										 /* npd_syslog_dbg("npd_fdb_static_trunk_del erro return value is:%d\n",ret);*/
										  ret = FDB_RETURN_CODE_OCCUR_HW;
									  }
									  ret = FDB_RETURN_CODE_OCCUR_HW;
								  }
							  }
						 }
						else {
							 npd_syslog_err("The vlan does not contain the trunk\n");
							 ret = NPD_FDB_ERR_PORT_NOTIN_VLAN;
						}
					 }
				 }
  
			  } 	  
		  else if(1 == result) {
			  ret = FDB_RETURN_CODE_SYSTEM_MAC;
		  }
	  }
	}
	  reply = dbus_message_new_method_return(msg);
	  
	  dbus_message_iter_init_append (reply, &iter);
	  
	  dbus_message_iter_append_basic (&iter,  DBUS_TYPE_UINT32, &ret);
	  
	  return reply;
	  
  }
  
   

 DBusMessage * npd_dbus_fdb_config_static_fdb_with_name(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	 
	 DBusMessage* reply;
	 DBusError err;
	 DBusMessageIter  iter;
	 unsigned char	 slot_no = 0,port_no = 0;  
	 unsigned short  vlanId = 0;
	 unsigned char	 macAddr[6] = {0};
	 char macComp[13] = {0};
	 int result = 0;
	 unsigned int	 eth_g_index = 0;
	 struct vlan_s* node = NULL;
	 char *vlanname=NULL;
	 int ret=0;
	 unsigned char istag = 0;
	 unsigned short  trunkId = 0;
	 dbus_error_init(&err);

	 if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		 if (!(dbus_message_get_args ( msg, &err,
		 										 DBUS_TYPE_STRING,&vlanname,
												 DBUS_TYPE_BYTE, &macAddr[0],
												 DBUS_TYPE_BYTE, &macAddr[1],
												 DBUS_TYPE_BYTE, &macAddr[2],
												 DBUS_TYPE_BYTE, &macAddr[3],
												 DBUS_TYPE_BYTE, &macAddr[4],
												 DBUS_TYPE_BYTE, &macAddr[5],
												 DBUS_TYPE_BYTE,&slot_no,
												 DBUS_TYPE_BYTE,&port_no,
												 DBUS_TYPE_INVALID))) {
				 npd_syslog_err("Unable to get input args!\n ");
				 if (dbus_error_is_set(&err)) {
					 npd_syslog_err("%s raised: %s\n",err.name,err.message);
					 dbus_error_free(&err);
					 }
				 return NULL;;
		 }
	 
		 else{
			sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
				 		macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
			result = npd_system_verify_basemac(macComp);
			if(0 == result) {
				if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
					if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) { 
						eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
							node= npd_find_vlan_by_name(vlanname);                  
							if (node == NULL) {
								npd_syslog_err("the vlanname is not register!\n");
								ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
							}

							else {
								vlanId = node->vid;
								if (!(CHASSIS_VLANID_ISLEGAL(vlanId))) {
									npd_syslog_err("illegal vlanId %d\n",vlanId);
									ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
								}
								else {
									ret = npd_vlan_check_contain_port(vlanId,eth_g_index, &istag);
									if (ret!=NPD_TRUE) {
										npd_syslog_err("the port do not belong to the vlan!\n");
										ret = NPD_FDB_ERR_PORT_NOTIN_VLAN;
									}
									else if(NPD_SUCCESS== npd_eth_port_get_ptrunkid(eth_g_index,&trunkId)){
										npd_syslog_err("The port belongs to trunk %d!\n",trunkId);
										ret =NPD_FDB_ERR_PORT_NOTIN_VLAN;
									}
									else {
										ret = npd_fdb_static_entry_add(macAddr,vlanId,slot_no,port_no,FDB_STATIC_NOMIRROR);
										if(ret != NPD_FDB_ERR_NONE) { 
											npd_syslog_err("npd_fdb_static_entry_add erro!return value is %d \n",ret);
											ret = FDB_RETURN_CODE_GENERAL;
										}
										else {
											ret = nam_static_fdb_entry_mac_vlan_port_set(macAddr,vlanId,eth_g_index);
											if (ret != NPD_FDB_ERR_NONE) {
												npd_syslog_err("return vlaue is: %d\n",ret);
												npd_syslog_err("nam_static_fdb_entry_mac_vlan_port_set: fdb entry ERROR. \n");
	                                            ret = FDB_RETURN_CODE_OCCUR_HW;
											}
										}
									}
								}
						     }	 
					  }
					  else{
	                      npd_syslog_err("Bad port number input!\n");
						  ret = FDB_RETURN_CODE_BADPARA;
					  }
				}
				else{
	                npd_syslog_err("Bad slot number input!\n");
					ret = FDB_RETURN_CODE_BADPARA;
				}
			}	 
			else if(1 == result) {
				ret = FDB_RETURN_CODE_SYSTEM_MAC;
			}

		 }
	}
	 reply = dbus_message_new_method_return(msg);
	 
	 dbus_message_iter_init_append (reply, &iter);
	 
	 dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &ret);
	 
	 return reply;
	 
 }


DBusMessage * npd_dbus_fdb_delete_static_fdb(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned short	vlanId = 0;
	unsigned char   macAddr[6] = {0};
	char macComp[13] = {0};
	int result = 0;
	unsigned int	ret=0;
    struct vlan_s * vlanNode = NULL;

	memset(macAddr,0,6);
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_UINT16,&vlanId,
											DBUS_TYPE_BYTE, &macAddr[0],
											DBUS_TYPE_BYTE, &macAddr[1],
											DBUS_TYPE_BYTE, &macAddr[2],
											DBUS_TYPE_BYTE, &macAddr[3],
											DBUS_TYPE_BYTE, &macAddr[4],
											DBUS_TYPE_BYTE, &macAddr[5],
											DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
	}
	
	else{
		if((NULL == (vlanNode = npd_find_vlan_by_vid(vlanId)))||(vlanNode->isAutoCreated)){
            npd_syslog_err("The vlan %d is not exist!\n",vlanId);
			ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
		}
		else{
			sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
					macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
			result = npd_system_verify_basemac(macComp);
			if(0 == result) {
				ret = npd_fdb_static_entry_del(macAddr,vlanId);
				if(ret != NPD_FDB_ERR_NONE) {
					npd_syslog_err("npd_fdb_static_entry_del erro return value is:%d\n",ret);
				}
				else {
					ret = nam_no_static_fdb_entry_mac_vlan_set(macAddr,vlanId);
					if(NPD_FDB_ERR_NONE != ret){
						npd_syslog_err("nam_no_static_fdb_entry_mac_vlan_set called error,return value is %d\n",ret);
						ret = FDB_RETURN_CODE_OCCUR_HW;
					}
			     }
		    }
			else if(1 == result) {
				ret = FDB_RETURN_CODE_SYSTEM_MAC;
			}
	    }
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32,  &ret);
	
	return reply;
	
}


DBusMessage * npd_dbus_fdb_delete_static_fdb_with_name(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	struct vlan_s* node = NULL;
	unsigned short	vlanId = 0;
	unsigned char   macAddr[6] = {0};
	char macComp[13] = {0};
	int result = 0;
	char* vlanname = NULL;
	/*unsigned int g_index = 0;*/
	int	ret=0;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
											/*DBUS_TYPE_STRING,&vlanname,*/
											DBUS_TYPE_BYTE, &macAddr[0],
											DBUS_TYPE_BYTE, &macAddr[1],
											DBUS_TYPE_BYTE, &macAddr[2],
											DBUS_TYPE_BYTE, &macAddr[3],
											DBUS_TYPE_BYTE, &macAddr[4],
											DBUS_TYPE_BYTE, &macAddr[5],
											DBUS_TYPE_STRING,&vlanname,
											DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
	}

	else{
		sprintf(macComp,"%02x%02x%02x%02x%02x%02x", \
				macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
		result = npd_system_verify_basemac(macComp);
		if(0 == result) {
			node = npd_find_vlan_by_name(vlanname);
			if(NULL == node){
               npd_syslog_err("No this vlan:%s\n",vlanname);
			   ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
			}
			else {
				
				vlanId = node->vid;						
				if (!(CHASSIS_VLANID_ISLEGAL(vlanId))) {
					npd_syslog_err("illegal vlanId %d",vlanId);
					ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
				}
				else {
					ret= npd_fdb_static_entry_del(macAddr,vlanId);
					if(ret != NPD_FDB_ERR_NONE) {
						npd_syslog_err("npd_fdb_static_entry_del erro return value is:%d\n",ret);
					}
					else {
						ret = nam_no_static_fdb_entry_mac_vlan_set(macAddr,vlanId);					
                        if(NPD_FDB_ERR_NONE != ret){
							npd_syslog_err("nam_no_static_fdb_entry_mac_vlan_set called error! Return value is %d\n",ret);
							ret = FDB_RETURN_CODE_OCCUR_HW;
                        }
					}
				}
			}
		}
		else if(1 == result) {
			ret = FDB_RETURN_CODE_SYSTEM_MAC;
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32,  &ret);
	
	return reply;
	
}



DBusMessage * npd_dbus_fdb_show_fdb_one(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{

	DBusMessage* reply;
	DBusError err;
	DBusMessageIter	 iter;
	
	unsigned int dcli_flag =0;
	unsigned int slot_index =0,slot_no = 0;
	unsigned int local_port_index =0,local_portno = 0;
	unsigned int	ret = 0,number = 0;;
	unsigned short	vlanId = 0;
	unsigned char   macAddr[6]={0};
	NPD_FDB dcli_show;
	
	dbus_error_init(&err);
	memset(&dcli_show,0,sizeof(NPD_FDB));
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_UINT16,&vlanId,
								DBUS_TYPE_BYTE, &macAddr[0],
								DBUS_TYPE_BYTE, &macAddr[1],
								DBUS_TYPE_BYTE, &macAddr[2],
								DBUS_TYPE_BYTE, &macAddr[3],
								DBUS_TYPE_BYTE, &macAddr[4],
								DBUS_TYPE_BYTE, &macAddr[5],
								DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
   if (CHASSIS_VLANID_ISLEGAL(vlanId)) {
	   ret = npd_check_vlan_exist(vlanId);
	   if(VLAN_RETURN_CODE_VLAN_EXISTS == ret){
	       ret = nam_show_fdb_one( &dcli_show,macAddr, vlanId);
	       npd_syslog_dbg("dcli_show address %p, dcli_show.value is: %d\n",&dcli_show,dcli_show.value);
	       if (ret != 0) {
		      npd_syslog_err("NO FDB ITEM! \n");
			  number = 0;
			  ret = FDB_RETURN_CODE_GENERAL;
	       }
           else{
	         /* npd_syslog_dbg ("npd_dbus_fdb_show_fdb_one interface type is: %d\n",dcli_show.inter_type);*/
		     /* npd_syslog_dbg ("npd_dbus_fdb_show_fdb_one vlanid is: %d\n",dcli_show.vlanid);*/
		      if (0 == dcli_show.inter_type){
			      dcli_flag = 0;
				  if(g_cpu_port_index == dcli_show.value){
					  slot_no = CPU_PORT_VIRTUAL_SLOT;
					  local_portno = CPU_PORT_VIRTUAL_PORT;
				  }
				  else if(g_spi_port_index == dcli_show.value){
					  slot_no = SPI_PORT_VIRTUAL_SLOT;
					  local_portno = SPI_PORT_VIRTUAL_PORT;
				  }

			      else {
			          npd_syslog_dbg ("npd_dbus_fdb_show_fdb_one dcli_show.value is: %d\n",dcli_show.value);
			          slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(dcli_show.value);
			          slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
			          local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(dcli_show.value);
			          local_portno = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);
			      }
				  number = 1;
		      }
		      else if ((dcli_show .inter_type > 0)&&(dcli_show .inter_type <4)){
				  if (dcli_show .inter_type == 1){ 
					  dcli_flag = 1;
				  }	  
				  else if (dcli_show .inter_type == 2){
					  dcli_flag = 2;
				  }
				  else if (dcli_show .inter_type == 3){
					  dcli_flag = 3;
				  }
				  slot_no = dcli_show.value;
				  local_portno = 0;
				  number = 1;
			  }
			  else {
				  npd_syslog_err ("interface type is: %d\n",dcli_show.inter_type);
				  npd_syslog_err ("sorry interface type wrong !\n");
				  number = 0;
				  ret = FDB_RETURN_CODE_GENERAL;
			 }
			 ret = NPD_FDB_ERR_NONE;
		  }
       }
	   else{
		 	npd_syslog_err("The vlanId %d is not exists",vlanId);
			number  = 0;
			ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
	   }
   }
   else {
		npd_syslog_err("Illegal vlanId %d",vlanId);
		number = 0;
		ret = FDB_RETURN_CODE_BADPARA;
		/*better to tell user to input vlanId again*/
   }

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);

	dbus_message_iter_append_basic
		  (&iter,
		   DBUS_TYPE_UINT32,
		    &ret);

	dbus_message_iter_append_basic
		  (&iter,
		   DBUS_TYPE_UINT32,
		    &number);
	dbus_message_iter_append_basic
		   (&iter,
		    DBUS_TYPE_UINT32,
		     &dcli_flag);
	dbus_message_iter_append_basic
			(&iter,
			 DBUS_TYPE_UINT16,
			 &(dcli_show.vlanid));
	dbus_message_iter_append_basic
			(&iter,
			 DBUS_TYPE_UINT32,
			 &slot_no);
	dbus_message_iter_append_basic
			(&iter,
			 DBUS_TYPE_UINT32,
			 &local_portno);
	dbus_message_iter_append_basic
			(&iter,
			 DBUS_TYPE_BYTE,
			 &(dcli_show.ether_mac[0]));
	dbus_message_iter_append_basic
			(&iter,
			 DBUS_TYPE_BYTE,
			 &(dcli_show.ether_mac[1]));
	dbus_message_iter_append_basic
			(&iter,
			 DBUS_TYPE_BYTE,
			 &(dcli_show.ether_mac[2]));
	dbus_message_iter_append_basic
			(&iter,
			 DBUS_TYPE_BYTE,
			&(dcli_show.ether_mac[3]));
	dbus_message_iter_append_basic
			(&iter,
			 DBUS_TYPE_BYTE,
			 &(dcli_show.ether_mac[4]));
	dbus_message_iter_append_basic
			(&iter,
			 DBUS_TYPE_BYTE,
			 &(dcli_show.ether_mac[5]));
    return reply;
			
}


DBusMessage * npd_dbus_fdb_show_fdb_mac(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	DBusError err;
	unsigned int dcli_flag=0;
	unsigned int trans_value1=0;
	unsigned int trans_value2=0;
	static NPD_FDB dcli_show [FDB_TABLE_LEN_32K];
	unsigned char   macAddr[6];
	unsigned int	ret = 0;
	unsigned int i=0;
    unsigned int eth_g_index = 0,slot_index = 0,slot_no = 0,local_port_index = 0,local_portno = 0;

	memset(dcli_show,0,sizeof(NPD_FDB)*FDB_TABLE_LEN_32K);
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_BYTE, &macAddr[0],
											DBUS_TYPE_BYTE, &macAddr[1],
											DBUS_TYPE_BYTE, &macAddr[2],
											DBUS_TYPE_BYTE, &macAddr[3],
											DBUS_TYPE_BYTE, &macAddr[4],
											DBUS_TYPE_BYTE, &macAddr[5],
											DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
	     return NULL;
	}

	else{
		ret = nam_show_fdb_mac(dcli_show,macAddr,FDB_TABLE_LEN_32K);
			
		if (ret == 0) {
			npd_syslog_err("NO FDB ITEM! \n");
			ret = NPD_FDB_ERR_NONE;
		}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
												DBUS_TYPE_UINT16_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for (; i < ret; i++)
	{
	       
			if (dcli_show[i].inter_type == CPSS_INTERFACE_PORT_E){
				
				dcli_flag = CPSS_INTERFACE_PORT_E;
				if(g_cpu_port_index == dcli_show[i].value){
					trans_value1 = CPU_PORT_VIRTUAL_SLOT;
					trans_value2 = CPU_PORT_VIRTUAL_PORT;
				}
				else if(g_spi_port_index == dcli_show[i].value){
					trans_value1 = SPI_PORT_VIRTUAL_SLOT;
					trans_value2 = SPI_PORT_VIRTUAL_PORT;
				}
				else{
					dcli_flag = CPSS_INTERFACE_PORT_E;
					eth_g_index = dcli_show[i].value;
					slot_index=SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
					local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					local_portno = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);
					trans_value1 = slot_no;
					trans_value2 = local_portno;
				}
			}
		    else if (dcli_show[i] .inter_type ==CPSS_INTERFACE_TRUNK_E ){ 
				dcli_flag = CPSS_INTERFACE_TRUNK_E;
				trans_value1 = dcli_show[i].value;
			    trans_value2 = 0;
			}
			else if (dcli_show[i] .inter_type == CPSS_INTERFACE_VIDX_E){
				dcli_flag = CPSS_INTERFACE_VIDX_E;
				trans_value1 = dcli_show[i].value;
			    trans_value2 = 0;
			}
			else if (dcli_show[i] .inter_type == CPSS_INTERFACE_VID_E){
				dcli_flag = CPSS_INTERFACE_VID_E;
				trans_value1 = dcli_show[i].value;
			    trans_value2 = 0;
			}
			npd_syslog_err("dcli_show[i].inter_type is:%d\n",dcli_show[i].inter_type);
			DBusMessageIter iter_struct;
			
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);			
		
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &dcli_flag);

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT16,
					 &(dcli_show[i].vlanid));

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value1);

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value2);
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[0]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[1]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[2]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					&(dcli_show[i].ether_mac[3]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[4]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[5]));
			dbus_message_iter_close_container (&iter_array, &iter_struct);
			
			
	}
	
	dbus_message_iter_close_container (&iter, &iter_array);
	
return reply;
}



DBusMessage * npd_dbus_fdb_show_fdb(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	unsigned int dcli_flag=0;
	unsigned int trans_value1=0;
	unsigned int trans_value2=0;
	static NPD_FDB dcli_show [FDB_TABLE_LEN_32K];
	unsigned int	ret = 0;
	unsigned int i=0,eth_g_index = 0,slot_index = 0,slot_no = 0;
	unsigned int local_port_index = 0,local_port_no = 0;
	
	memset(dcli_show,0,sizeof(NPD_FDB)*FDB_TABLE_LEN_32K);

	ret = nam_show_fdb (dcli_show,FDB_TABLE_LEN_32K);	
	if (ret == 0) {
		npd_syslog_err("NO FDB ITEM! \n");
		ret = NPD_FDB_ERR_NONE; 
	}	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
												DBUS_TYPE_UINT16_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for (i = 0; i < ret; i++)
	{
			
			if (CPSS_INTERFACE_PORT_E == dcli_show[i].inter_type){
				
					dcli_flag = CPSS_INTERFACE_PORT_E;
					if(g_cpu_port_index == dcli_show[i].value){
						trans_value1 = CPU_PORT_VIRTUAL_SLOT;
						trans_value2 = CPU_PORT_VIRTUAL_PORT;
					}
					else if(g_spi_port_index == dcli_show[i].value){
						trans_value1 = SPI_PORT_VIRTUAL_SLOT;
						trans_value2 = SPI_PORT_VIRTUAL_PORT;
					}
					else{
						eth_g_index = dcli_show[i].value;
						slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
						slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
						local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
						local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);

						trans_value1 = slot_no;
						trans_value2 = local_port_no;
					}
			}
			else if (dcli_show[i].inter_type == CPSS_INTERFACE_TRUNK_E){ 
						dcli_flag = CPSS_INTERFACE_TRUNK_E;
						trans_value1 = dcli_show[i].value;
						trans_value2 = 0;
			}
			else if (dcli_show[i].inter_type == CPSS_INTERFACE_VIDX_E){
				dcli_flag = CPSS_INTERFACE_VIDX_E;
				trans_value1 = dcli_show[i].value;
				trans_value2 = 0;
				
			}			
			else if (dcli_show[i].inter_type == CPSS_INTERFACE_VID_E){
				dcli_flag = CPSS_INTERFACE_VID_E;
				trans_value1 = dcli_show[i].value;
				trans_value2 = 0;
				
			}			
			DBusMessageIter iter_struct;
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			/* fdb destination interface type */
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &dcli_flag);
			/* VLAN ID*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT16,
					 &(dcli_show[i].vlanid));

			/* SLOT number or VLAN ID or VIDX or Trunk ID*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value1);

			/* PORT number*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value2);
			/*ITEM type*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &dcli_show[i].type_flag);

			/*MAC address*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[0]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[1]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[2]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					&(dcli_show[i].ether_mac[3]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[4]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[5]));
			dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	
	dbus_message_iter_close_container (&iter, &iter_array);
	
return reply;
}

/* for show debug fdb */
DBusMessage * npd_dbus_fdb_show_fdb_debug(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	unsigned int dcli_flag=0;
	unsigned int trans_value1=0;
	unsigned int trans_value2=0;
	static NPD_FDB_DBG dcli_show[32768];
	unsigned int	ret = 0;
	unsigned int i=0,eth_g_index = 0,slot_index = 0,slot_no = 0,j=0;
	unsigned int local_port_index = 0,local_port_no = 0;
	
	memset(dcli_show,0,sizeof(NPD_FDB_DBG)*32768);

	ret = nam_show_fdb_debug (dcli_show,32768);	
	if (ret == 0) {
		npd_syslog_err("NO FDB ITEM! \n");
		ret = NPD_FDB_ERR_NONE; 
	}	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_open_container (&iter,
									   	DBUS_TYPE_ARRAY,
									   	DBUS_STRUCT_BEGIN_CHAR_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_UINT16_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
									   	DBUS_TYPE_UINT32_AS_STRING
									   	DBUS_TYPE_UINT32_AS_STRING
									   	DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for (i = 0; i < ret; i++)
	{
			
			if (CPSS_INTERFACE_PORT_E == dcli_show[i].inter_type){
				
					dcli_flag = CPSS_INTERFACE_PORT_E;
					if(g_cpu_port_index == dcli_show[i].value){
						trans_value1 = CPU_PORT_VIRTUAL_SLOT;
						trans_value2 = CPU_PORT_VIRTUAL_PORT;
					}
					else if(g_spi_port_index == dcli_show[i].value){
						trans_value1 = SPI_PORT_VIRTUAL_SLOT;
						trans_value2 = SPI_PORT_VIRTUAL_PORT;
					}
					else{
						#if 0
						eth_g_index = dcli_show[i].value;
						slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
						slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
						local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
						local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);

						trans_value1 = slot_no;
						trans_value2 = local_port_no;
						#endif
						trans_value1 = 0;
						trans_value2 = dcli_show[i].value;
						
					}
			}
			else if (dcli_show[i].inter_type == CPSS_INTERFACE_TRUNK_E){ 
						dcli_flag = CPSS_INTERFACE_TRUNK_E;
						trans_value1 = dcli_show[i].value;
						trans_value2 = 0;
			}
			else if (dcli_show[i].inter_type == CPSS_INTERFACE_VIDX_E){
				dcli_flag = CPSS_INTERFACE_VIDX_E;
				trans_value1 = dcli_show[i].value;
				trans_value2 = 0;
				
			}			
			else if (dcli_show[i].inter_type == CPSS_INTERFACE_VID_E){
				dcli_flag = CPSS_INTERFACE_VID_E;
				trans_value1 = dcli_show[i].value;
				trans_value2 = 0;
				
			}			
			DBusMessageIter iter_struct;
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			/* fdb destination interface type */
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &dcli_flag);
				/*devNum*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(dcli_show[i].dev));
				/*asic_num*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(dcli_show[i].asic_num));
			/* VLAN ID*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT16,
					 &(dcli_show[i].vlanid));

			/* SLOT number or VLAN ID or VIDX or Trunk ID*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value1);

			/* PORT number*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value2);
			/*ITEM type*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &dcli_show[i].type_flag);

			/*MAC address*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[0]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[1]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[2]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					&(dcli_show[i].ether_mac[3]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[4]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[5]));
			dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	
	dbus_message_iter_close_container (&iter, &iter_array);
	
return reply;
}



DBusMessage * npd_dbus_fdb_syn_fdb_table(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter2;
	DBusError err;
	DBusMessageIter	 iter_array;
	unsigned int trans_value1=0;
	unsigned int trans_value2=0;
	static NPD_FDB_DBG syn_fdb[32768];
	unsigned int	ret = 0;
	unsigned int i=0,m=0;
	unsigned int total_fdb_count = 0;
	unsigned char* type = NULL;
	unsigned int flag = 0;
	memset(syn_fdb,0,sizeof(NPD_FDB_DBG)*32768);

	dbus_message_iter_init(msg,&iter);
	dbus_message_iter_get_basic(&iter,&total_fdb_count);
	if(total_fdb_count == 0)
	{
		npd_syslog_err("NO FDB ITEMS \n");
	}
	dbus_message_iter_next(&iter);

	dbus_message_iter_recurse(&iter,&iter_array);
	for ( i = 0; total_fdb_count > i; i++)
	{
		DBusMessageIter iter_struct;
	
		dbus_message_iter_recurse(&iter_array,&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].inter_type));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].dev));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].asic_num));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].slot_id));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].vlanid));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&trans_value1);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&trans_value2);
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].type_flag));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].ether_mac[0]));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].ether_mac[1]));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].ether_mac[2]));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].ether_mac[3]));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].ether_mac[4]));
		dbus_message_iter_next(&iter_struct);
		dbus_message_iter_get_basic(&iter_struct,&(syn_fdb[i].ether_mac[5]));
		if(i == 0)
		{
			npd_syslog_dbg("Codes:  CPU - target port which connect to cpu via pci interface\n");
			npd_syslog_dbg("        HSC - high speed channel direct to cpu rgmii interface\n\n");
			npd_syslog_dbg("%-17s  %-4s  %-4s  %-9s  %-5s  %-4s  %-4s  %-8s\n","=================",	"====","====", "=========","=====","====","====","========");
			npd_syslog_dbg("%-17s  %-4s  %-4s  %-9s  %-5s  %-4s  %-4s  %-8s\n","MAC","VLAN","ASIC","DEV/PORT","TRUNK","VID","SLOT","TYPE");
			npd_syslog_dbg("%-17s  %-4s  %-4s  %-9s  %-5s  %-4s  %-4s  %-8s\n","=================",	"====","====", "=========","=====","====","====","========");
		}
		if(syn_fdb[i].inter_type == CPSS_INTERFACE_PORT_E)
		{
			syn_fdb[i].value = trans_value2;
		}
		else
		{
			syn_fdb[i].value = trans_value1;
		}

		if( 0 == syn_fdb[i].type_flag)
		{
	   	    type = "DYNAMIC";
		}
		else if(1 == syn_fdb[i].type_flag)
		{
			type = "STATIC";
		}
		else 
		{
			npd_syslog_dbg("sorry fdb item type wrong !\n");
			return -1;
		}
		if (syn_fdb[i].inter_type == CPSS_INTERFACE_PORT_E)
		{
			if(CPU_PORT_VIRTUAL_SLOT == trans_value1) 
			{ /*static FDB to CPU or SPI (has the same virtual slot number)*/
				npd_syslog_dbg("%02x:%02x:%02x:%02x:%02x:%02x  %-4d  %-4d  %-9s  %-5s  %4s  %-4d  %-8s\n",	\
						syn_fdb[i].ether_mac[0],syn_fdb[i].ether_mac[1],syn_fdb[i].ether_mac[2],syn_fdb[i].ether_mac[3],syn_fdb[i].ether_mac[4],syn_fdb[i].ether_mac[5],	\
						syn_fdb[i].vlanid,syn_fdb[i].asic_num,(CPU_PORT_VIRTUAL_PORT == trans_value2) ? "   CPU   ":	\
						(SPI_PORT_VIRTUAL_PORT == trans_value2) ? "   HSC   ":"   ERR   ","  -  "," - ",syn_fdb[i].slot_id," - ");
			}
			else 
			{
				if(trans_value1 <= 10)
				{
					npd_syslog_dbg("%02x:%02x:%02x:%02x:%02x:%02x  %-4d  %-4d  %4d/%-4d  %-5s  %-4s  %-4d  %-8s\n",   \
								syn_fdb[i].ether_mac[0],syn_fdb[i].ether_mac[1],syn_fdb[i].ether_mac[2],syn_fdb[i].ether_mac[3],syn_fdb[i].ether_mac[4],syn_fdb[i].ether_mac[5],	\
								syn_fdb[i].vlanid,syn_fdb[i].asic_num,syn_fdb[i].dev,trans_value2," - "," - ",syn_fdb[i].slot_id,type);
				}
						
			}
		}
		else if (syn_fdb[i].inter_type == CPSS_INTERFACE_TRUNK_E)
		{ 				
			npd_syslog_dbg("%02x:%02x:%02x:%02x:%02x:%02x  %-4d  %-4d  %9s  %-5d  %-4s  %-4d  %-8s\n",    \
			       	 syn_fdb[i].ether_mac[0],syn_fdb[i].ether_mac[1],syn_fdb[i].ether_mac[2],syn_fdb[i].ether_mac[3],syn_fdb[i].ether_mac[4],syn_fdb[i].ether_mac[5],	\
			       	 syn_fdb[i].vlanid,syn_fdb[i].asic_num,"    -    ",trans_value1," - ",syn_fdb[i].slot_id,type);
		}
		dbus_message_iter_next(&iter_array);
	
	}

	for(i = 0;i< total_fdb_count;i++)
	{
		if(SLOT_ID != syn_fdb[i].slot_id)
		{
			for(m = 0;m<total_fdb_count;m++)
			{
				if((syn_fdb[m].slot_id == SLOT_ID)&&(0==memcmp(syn_fdb[m].ether_mac,syn_fdb[i].ether_mac,MAC_ADDRESS_LEN))&&(syn_fdb[i].vlanid==syn_fdb[m].vlanid))
				{
					flag = 1;/*used to find if local board has the fdb already*/
					break;
				}
			}
			if(flag == 1)
			{
				flag = 0;
				npd_syslog_dbg("Slot %d already have fdb item mac %02x:%02x:%02x:%02x:%02x:%02x  vlan %d \n",SLOT_ID,syn_fdb[i].ether_mac[0],syn_fdb[i].ether_mac[1],syn_fdb[i].ether_mac[2],\
					syn_fdb[i].ether_mac[3],syn_fdb[i].ether_mac[4],syn_fdb[i].ether_mac[5],syn_fdb[i].vlanid);
				continue;
			}
			if(syn_fdb[i].inter_type == CPSS_INTERFACE_PORT_E)
			{
				ret = nam_static_fdb_entry_mac_vlan_port_set_for_debug(syn_fdb[i].ether_mac,syn_fdb[i].vlanid,syn_fdb[i].dev,syn_fdb[i].value);
				if(ret != NPD_FDB_ERR_NONE)
				{
					npd_syslog_err("set static mac addr for dev %d port %d fail\n",syn_fdb[i].dev,syn_fdb[i].value);
					break;
				}
			}
			else if(syn_fdb[i].inter_type == CPSS_INTERFACE_TRUNK_E)
			{
				ret = nam_static_fdb_entry_mac_vlan_trunk_set(syn_fdb[i].ether_mac,syn_fdb[i].vlanid,syn_fdb[i].value);
				if(ret != NPD_FDB_ERR_NONE)
				{
					npd_syslog_err("set static mac addr for dev %d vlan %d  trunk %d fail\n",syn_fdb[i].dev,syn_fdb[i].vlanid,syn_fdb[i].value);
					break;
				}
			}
			else
			{
				npd_syslog_err("unknown type !\n");
				break;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter2);
	
	dbus_message_iter_append_basic (&iter2,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}


DBusMessage * npd_dbus_fdb_show_dynamic_fdb(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	unsigned int dcli_flag=0;
	unsigned int trans_value1=0;
	unsigned int trans_value2=0;
	static NPD_FDB dcli_show [FDB_TABLE_LEN_32K];
	unsigned int	ret = 0;
	unsigned int i=0,eth_g_index = 0,slot_index = 0,slot_no = 0;
	unsigned int local_port_index = 0,local_port_no = 0;
	
	memset(dcli_show,0,sizeof(NPD_FDB)*FDB_TABLE_LEN_32K);

	ret = nam_show_dynamic_fdb (dcli_show,FDB_TABLE_LEN_32K);
	
	if (ret == 0) {
		npd_syslog_err("NO FDB ITEM! \n");
		ret = NPD_FDB_ERR_NONE; 
	}
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
												DBUS_TYPE_UINT16_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for (i = 0; i < ret; i++)
	{
			
			if (CPSS_INTERFACE_PORT_E == dcli_show[i].inter_type){
				
					dcli_flag = CPSS_INTERFACE_PORT_E;
					if(g_cpu_port_index == dcli_show[i].value){
						trans_value1 = CPU_PORT_VIRTUAL_SLOT;
						trans_value2 = CPU_PORT_VIRTUAL_PORT;
					}
					else if(g_spi_port_index == dcli_show[i].value){
						trans_value1 = SPI_PORT_VIRTUAL_SLOT;
						trans_value2 = SPI_PORT_VIRTUAL_PORT;
					}
					else{
						eth_g_index = dcli_show[i].value;
						slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
						slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
						local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
						local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);

						trans_value1 = slot_no;
						trans_value2 = local_port_no;
					}
			}
			else if (dcli_show[i] .inter_type == CPSS_INTERFACE_TRUNK_E){ 
						dcli_flag = CPSS_INTERFACE_TRUNK_E;
						trans_value1 = dcli_show[i].value;
						trans_value2 = 0;
			}
			else if (dcli_show[i] .inter_type == CPSS_INTERFACE_VIDX_E){
				dcli_flag = CPSS_INTERFACE_VIDX_E;
				trans_value1 = dcli_show[i].value;
				trans_value2 = 0;
				
			}
			
			else if (dcli_show[i] .inter_type == CPSS_INTERFACE_VID_E){
				dcli_flag = CPSS_INTERFACE_VID_E;
				trans_value1 = dcli_show[i].value;
				trans_value2 = 0;
				
			}			
			DBusMessageIter iter_struct;
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			/* fdb destination interface type */
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &dcli_flag);
			/* VLAN ID*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT16,
					 &(dcli_show[i].vlanid));

			/* SLOT number or VLAN ID or VIDX or Trunk ID*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value1);

			/* PORT number*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value2);

			/*MAC address*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[0]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[1]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[2]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					&(dcli_show[i].ether_mac[3]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[4]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[5]));
			dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	
	dbus_message_iter_close_container (&iter, &iter_array);
	
return reply;
}



DBusMessage * npd_dbus_fdb_show_fdb_count(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int	ret = 0;
	
	ret = nam_show_fdb_count( );
	
	if (ret == 0) {
		npd_syslog_err("NO FDB ITEM! \n");
		ret = NPD_FDB_ERR_NONE; 
	}
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	
	
return reply;
}

DBusMessage * npd_dbus_fdb_delete_fdb_with_vlan(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int	ret = 0;
	DBusError err;
	unsigned short vlanid;
    struct vlan_s * vlanNode = NULL;
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_UINT16,&vlanid,
											DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		return NULL;
	}
	
	else{
		if((NULL==(vlanNode = npd_find_vlan_by_vid(vlanid)))||(vlanNode->isAutoCreated)){
			ret= FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
		}
		else{
			ret =  nam_fdb_table_delete_entry_with_vlan(vlanid);
			if (ret != NPD_FDB_ERR_NONE) {
				npd_syslog_err("nam_fdb_table_delete_entry_with_vlan:erro !\n");
				ret = FDB_RETURN_CODE_OCCUR_HW;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	
return reply;
}

DBusMessage * npd_dbus_fdb_delete_fdb_with_port(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int	ret = NPD_DBUS_ERROR;
	DBusError err;
	unsigned char slot_no;
	unsigned char port_no;
	unsigned int eth_g_index;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_BYTE,&slot_no,
											DBUS_TYPE_BYTE,&port_no,
											DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					npd_syslog_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
			}
			return NULL;
	}
	
	else{
			if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) {
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
					ret =  nam_fdb_table_delete_entry_with_port(eth_g_index);
					if (ret!= NPD_FDB_ERR_NONE) {
						npd_syslog_err("nam_fdb_table_delete_entry_with_port:err! \n");
                        ret = FDB_RETURN_CODE_OCCUR_HW;
					}	
			    }
				else {
                     npd_syslog_err("Error slot/port number! \n");
					 ret = FDB_RETURN_CODE_BADPARA;
				}
			}
			else{
                 npd_syslog_err("Error slot number\n");
				 ret = FDB_RETURN_CODE_BADPARA;	
			}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &ret);

return reply;
}


DBusMessage * npd_dbus_fdb_delete_static_fdb_with_vlan(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int	ret = NPD_DBUS_ERROR;
	DBusError err;
	unsigned short vlanid;
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_UINT16,&vlanid,
											DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					npd_syslog_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
			}
			return NULL;
	}
	
	else{
			if(NULL==npd_find_vlan_by_vid(vlanid)){

				ret= FDB_RETURN_CODE_NODE_VLAN_NONEXIST;

			}
			else{
				ret = npd_fdb_static_entry_del_by_vlan(vlanid);
				if( NPD_FDB_ERR_NONE != ret){
                    npd_syslog_err("Delete fdb static by vlan in sw error: %d\n",ret);
                    ret = FDB_RETURN_CODE_GENERAL;
				}
				else{
					ret =  nam_fdb_static_table_delete_entry_with_vlan(vlanid);
					if (ret != NPD_FDB_ERR_NONE) {
						npd_syslog_err("nam_fdb_table_delete_entry_with_vlan:erro !\n");
                        ret = FDB_RETURN_CODE_OCCUR_HW;
					}
				}
			}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	
   return reply;
}

DBusMessage * npd_dbus_fdb_delete_static_debug_fdb_with_vlan(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int	ret = NPD_DBUS_ERROR;
	DBusError err;
	unsigned short vlanid;
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args (	msg, &err,
								 	DBUS_TYPE_UINT16,&vlanid,
									DBUS_TYPE_INVALID))) 
	{
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	else
	{
			ret =  nam_fdb_static_table_delete_entry_with_vlan(vlanid);
			if (ret != NPD_FDB_ERR_NONE) 
			{
				npd_syslog_err("nam_fdb_table_delete_entry_with_vlan:erro !\n");
                ret = FDB_RETURN_CODE_OCCUR_HW;
			}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	
   return reply;
}


DBusMessage * npd_dbus_fdb_delete_static_fdb_with_port(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int	ret = NPD_DBUS_ERROR;
	DBusError err;
	unsigned char slot_no;
	unsigned char port_no;
	unsigned int eth_g_index;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_BYTE,&slot_no,
											DBUS_TYPE_BYTE,&port_no,
											DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					npd_syslog_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
			}
			return NULL;
	}
	
	else{
			if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) {
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
                    ret = npd_fdb_static_entry_del_by_port(slot_no,port_no);
					if( 0 != ret){
                        npd_syslog_err("Delete fdb static by port error:%d\n",ret);
                        ret = FDB_RETURN_CODE_GENERAL;
					}
					else{
						ret =  nam_fdb_static_table_delete_entry_with_port(eth_g_index);
						if (ret!= NPD_FDB_ERR_NONE) {
							npd_syslog_err("nam_fdb_table_delete_entry_with_port:err! \n");
                            ret = FDB_RETURN_CODE_OCCUR_HW;
						}
					}
			    }
				else {
                     npd_syslog_err("Error slot/port number! \n");
					 ret = FDB_RETURN_CODE_BADPARA;
				}
			}
			else{
                 npd_syslog_err("Error slot number\n");
				 ret = FDB_RETURN_CODE_BADPARA;
				 	
			}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &ret);

    return reply;
}

DBusMessage * npd_dbus_fdb_delete_fdb_with_trunk(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int	ret = NPD_DBUS_ERROR;
	DBusError err;
	unsigned short trunk_no = 0;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_UINT16,&trunk_no,
											DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					npd_syslog_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
			}
	}
	
	else{
		npd_syslog_dbg("trunk_no = %d\n",trunk_no);
		if(TRUNK_RETURN_CODE_TRUNK_EXISTS == npd_check_trunk_exist(trunk_no)){
			ret =  nam_fdb_table_delete_entry_with_trunk(trunk_no);
			if (ret!= NPD_FDB_ERR_NONE) {
				npd_syslog_err("nam_fdb_table_delete_entry_with_port:err! \n");
                ret = FDB_RETURN_CODE_OCCUR_HW;
			}
		}
		else{
            npd_syslog_err("The trunk does not exist\n");
            ret = FDB_RETURN_CODE_GENERAL;
		}
	}

	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &ret);

return reply;
}



DBusMessage * npd_dbus_fdb_show_static_fdb(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	unsigned int dcli_flag=0;
	unsigned int trans_value1=0;
	unsigned int trans_value2=0;
	struct fdb_entry_node_s **array;
	unsigned int i=0;
	unsigned int eth_g_index = 0,slot_index = 0,slot_no = 0,local_port_index = 0,local_portno = 0;
	unsigned int number = 0;
	unsigned  int slot = 0,port = 0;
	array = npd_fdb_get_entry();
	if(array == NULL)
	{
		npd_syslog_dbg("static fdb is null !\n");
	}
	npd_syslog_dbg("get port interface static fdb count %d\n",g_static_fdb_count);

	number = g_static_fdb_count;
	/*ret value respect the number not exist in software*/
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&number);
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT16_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_UINT32_AS_STRING
									   DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);

	for (i = 0;i<AX_STATIC_FDB_ENTRY_SIZE;i++)
	{
		   if(array[i] == NULL)
		   {
		   		continue;
		   }
		   if(array[i]->trunkid !=0)
		   {
		   		dcli_flag = CPSS_INTERFACE_TRUNK_E;
				slot = array[i]->trunkid;
				npd_syslog_dbg("trunk id = %d\n",slot);
		   }
		   else
		   {
		   		dcli_flag = CPSS_INTERFACE_PORT_E;
				slot = array[i]->slot;
				port = array[i]->port;    
				npd_syslog_dbg("slot = %d,port = %d\n",slot,port);
		   }
		   DBusMessageIter iter_struct;
	       dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);	
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &dcli_flag);
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT16,
					 &(array[i]->vlanid));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &slot);
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &port);
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(array[i]->mac[0]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(array[i]->mac[1]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(array[i]->mac[2]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					&(array[i]->mac[3]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(array[i]->mac[4]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(array[i]->mac[5]));
			dbus_message_iter_close_container (&iter_array, &iter_struct);		
	}
	dbus_message_iter_close_container (&iter, &iter_array);	
    return reply;
}
DBusMessage * npd_dbus_fdb_show_blacklist_fdb(DBusConnection *conn, DBusMessage *msg, void *user_data) 
	{
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	static  struct fdb_blacklist_node_s  blacklist_show[FDB_TABLE_LEN];
	unsigned int	ret = 0;
	unsigned int i=0;
	
	memset(blacklist_show,0,sizeof(struct fdb_blacklist_node_s)*FDB_TABLE_LEN);

	/*ret = nam_show_static_fdb (dcli_show,FDB_TABLE_LEN);*/
	
	/*TODO: SHOW FDB BLACKLIST FROM SOFTWARE*/
	ret = npd_fdb_get_blacklist_item(blacklist_show,FDB_TABLE_LEN);
	if (ret == 0) {
		npd_syslog_err("NO FDB BLACKLIST ITEM! \n");
		ret = NPD_FDB_ERR_NONE;
	}
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
                                                DBUS_TYPE_BYTE_AS_STRING /*dmac*/
	                                            DBUS_TYPE_BYTE_AS_STRING  /*smac*/
												DBUS_TYPE_UINT16_AS_STRING /*vlanid*/
											   	DBUS_TYPE_BYTE_AS_STRING   /*mac address[6]*/
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for (; i < ret; i++)
	{
			
			
			DBusMessageIter iter_struct;
			
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			/*dmac*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(blacklist_show[i].dmac));
			/*printf("The blacklist_show[%d].dmac is %d \n",i,blacklist_show[i].dmac);*/
			/*snac*/
			dbus_message_iter_append_basic
			       (&iter_struct,
			         DBUS_TYPE_BYTE,
			         &(blacklist_show[i].smac));
			/*printf("The blacklist_show[%d].smac is %d \n",i,blacklist_show[i].smac);*/
            /*vlanid*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT16,
					 &(blacklist_show[i].vlanid));
			
            /*mac address*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(blacklist_show[i].mac[0]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(blacklist_show[i].mac[1]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(blacklist_show[i].mac[2]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					&(blacklist_show[i].mac[3]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(blacklist_show[i].mac[4]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(blacklist_show[i].mac[5]));
			dbus_message_iter_close_container (&iter_array, &iter_struct);		
	}
	
	dbus_message_iter_close_container (&iter, &iter_array);
	
return reply;
}



DBusMessage * npd_dbus_fdb_show_fdb_port(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	DBusError err;
	unsigned int dcli_flag=0;
	unsigned int trans_value1=0;
	unsigned int trans_value2=0;
	unsigned int	ret = NPD_DBUS_SUCCESS; ;
	unsigned int i=0;
	unsigned char slot;
	unsigned char port;
    unsigned int eth_g_index = 0,slot_index = 0,slot_no = 0,local_port_index = 0,local_portno = 0;

	static NPD_FDB dcli_show [FDB_TABLE_LEN_32K] ;

	memset(dcli_show,0,sizeof(NPD_FDB)*FDB_TABLE_LEN_32K);
    dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		                                    DBUS_TYPE_BYTE,&slot,
											DBUS_TYPE_BYTE,&port,
											DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	else{
		if (CHASSIS_SLOTNO_ISLEGAL(slot)) {
				if (ETH_LOCAL_PORTNO_ISLEGAL( slot,port)) {
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);						
			        ret = nam_show_fdb_port(dcli_show,FDB_TABLE_LEN_32K,eth_g_index);
			        if (ret == 0) {
					     npd_syslog_err("NO FDB ITEM! \n");
						 ret = NPD_FDB_ERR_NONE;
			        }
			     }
				 else {
				 	 npd_syslog_err("The port input is error \n");
				 	 ret = NPD_FDB_ERR_NONE;
				 }						
		}
		else{
			npd_syslog_err("The slot input is error \n");
			 ret = NPD_FDB_ERR_NONE;
		}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32,&ret);
	
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
												DBUS_TYPE_UINT16_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for (; i<ret; i++)
	{
			if (dcli_show[i].inter_type == CPSS_INTERFACE_PORT_E){
					dcli_flag = CPSS_INTERFACE_PORT_E;
					slot_index=SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
					local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					local_portno = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);
					trans_value1 = slot_no;
					trans_value2 = local_portno;
			}
			else if ((dcli_show[i] .inter_type > CPSS_INTERFACE_PORT_E)&&(dcli_show[i] .inter_type <CPSS_INTERFACE_MAX)){
					 if (dcli_show[i] .inter_type == CPSS_INTERFACE_TRUNK_E){ 
						dcli_flag = CPSS_INTERFACE_TRUNK_E;
						
						}
					else if (dcli_show[i] .inter_type == CPSS_INTERFACE_VIDX_E){
						dcli_flag = CPSS_INTERFACE_VIDX_E;
						
						}
					
					else if (dcli_show[i] .inter_type == CPSS_INTERFACE_VID_E){
						dcli_flag = CPSS_INTERFACE_VID_E;
						
						}
					trans_value1 = dcli_show[i].value;
					trans_value2 = 0;
		 }

			DBusMessageIter iter_struct;
			
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			
		
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &dcli_flag);

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT16,
					 &(dcli_show[i].vlanid));

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value1);

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value2);
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[0]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[1]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[2]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					&(dcli_show[i].ether_mac[3]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[4]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[5]));
			dbus_message_iter_close_container (&iter_array, &iter_struct);
			
			
	}
	
	dbus_message_iter_close_container (&iter, &iter_array);
	
return reply;
}



DBusMessage * npd_dbus_fdb_show_fdb_vlan(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	DBusError err;
	unsigned int dcli_flag=0;
	unsigned int trans_value1=0;
	unsigned int trans_value2=0;
	static NPD_FDB  dcli_show [FDB_TABLE_LEN_32K] ;
	unsigned int	ret = NPD_FDB_ERR_NONE;
	unsigned int dnumber;
	unsigned int i=0;
    unsigned int eth_g_index = 0,slot_index = 0,slot_no = 0,local_port_index = 0,local_portno = 0;
	unsigned short vlan;
    struct vlan_s * vlanNode = NULL;
	memset(dcli_show,0,sizeof(NPD_FDB)*FDB_TABLE_LEN_32K);
	
    dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		                                    DBUS_TYPE_UINT16,&vlan,
											DBUS_TYPE_INVALID))) 
	{
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			npd_syslog_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
	}
	else
	{	
		if((NULL==(vlanNode = npd_find_vlan_by_vid(vlan)))||(vlanNode->isAutoCreated))
		{
			ret= FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
		}
		else
		{
			dnumber= nam_show_fdb_vlan(dcli_show,FDB_TABLE_LEN_32K,vlan);
			if(0 == dnumber) 
			{
				npd_syslog_err("NO FDB ITEM! \n");
				ret = NPD_FDB_ERR_NONE;
			}
		}
	}
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);

	/* if the vlan does not exist, return ret only*/
	if(FDB_RETURN_CODE_NODE_VLAN_NONEXIST == ret)
	{
		dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&ret);
	    return reply;
	}

	/*else*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&ret);

	/*if the fdb number find is 0*/
	if(FDB_RETURN_CODE_GENERAL == ret)
	{	
	    dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&dnumber);
	    return reply;
	}

	/*else*/
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&dnumber);
	
	dbus_message_iter_open_container (&iter,
										DBUS_TYPE_ARRAY,
										DBUS_STRUCT_BEGIN_CHAR_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_UINT32_AS_STRING
										DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_TYPE_BYTE_AS_STRING 
									   	DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for (; dnumber > i; i++)
	{
			if (dcli_show[i].inter_type == CPSS_INTERFACE_PORT_E)
			{
				dcli_flag = CPSS_INTERFACE_PORT_E;
				if(g_cpu_port_index == dcli_show[i].value)
				{
					trans_value1 = CPU_PORT_VIRTUAL_SLOT;
					trans_value2 = CPU_PORT_VIRTUAL_PORT;
				}
				else if(g_spi_port_index == dcli_show[i].value)
				{
					trans_value1 = SPI_PORT_VIRTUAL_SLOT;
					trans_value2 = SPI_PORT_VIRTUAL_PORT;
				}
				else
				{
					eth_g_index = dcli_show[i].value;
					slot_index=SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
					local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					local_portno = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);
					trans_value1 = slot_no;
					trans_value2 = local_portno;
				}
			}
			else if ((dcli_show[i] .inter_type > CPSS_INTERFACE_PORT_E)&&(dcli_show[i] .inter_type <CPSS_INTERFACE_MAX))
			{
				if (dcli_show[i] .inter_type == CPSS_INTERFACE_TRUNK_E)
				{ 
					dcli_flag = CPSS_INTERFACE_TRUNK_E;	
				}
				else if (dcli_show[i] .inter_type == CPSS_INTERFACE_VIDX_E)
				{
					dcli_flag = CPSS_INTERFACE_VIDX_E;
				}
				
				else if (dcli_show[i] .inter_type == CPSS_INTERFACE_VID_E)
				{
					dcli_flag = CPSS_INTERFACE_VID_E;	
				}
					trans_value1 = dcli_show[i].value;
					trans_value2 = 0;
			}
			else 
			{
					npd_syslog_err ("sorry interface type wrong !\n");
					npd_syslog_err ("interface type is: %d\n",dcli_show->inter_type);
			}

			DBusMessageIter iter_struct;
			
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);		
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &dcli_flag);

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(dcli_show[i].vlanid));

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value1);

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value2);
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[0]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[1]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[2]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					&(dcli_show[i].ether_mac[3]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[4]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[5]));
			dbus_message_iter_close_container (&iter_array, &iter_struct);			
	}
	
	dbus_message_iter_close_container (&iter, &iter_array);
	
return reply;
}


DBusMessage * npd_dbus_fdb_show_fdb_vlan_with_name(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusMessageIter	 iter_array;
	DBusError err;
	unsigned int dcli_flag=0;
	unsigned int trans_value1=0;
	unsigned int trans_value2=0;
	static NPD_FDB  dcli_show [FDB_TABLE_LEN_32K] ;
	unsigned int i=0;
	unsigned short vlan = 0;
	struct vlan_s* node = NULL;
    unsigned int eth_g_index = 0,slot_index = 0,slot_no = 0,local_port_index = 0,local_portno = 0;
	
	char* vlanname=NULL;
	int	ret=NPD_DBUS_SUCCESS;
	unsigned int dnumber=0;
	memset(dcli_show,0,sizeof(NPD_FDB)*FDB_TABLE_LEN_32K);
    dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		                                    DBUS_TYPE_STRING,&vlanname,
											DBUS_TYPE_INVALID))) {
											
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
	}
	else{
			node= npd_find_vlan_by_name(vlanname);
			if (node == NULL){
				ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
			}
			else {
				vlan = node->vid;
				if (!(CHASSIS_VLANID_ISLEGAL(vlan))) {
						npd_syslog_err("illegal vlanId %d",vlan);
						ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
				}
				else{
						dnumber = nam_show_fdb_vlan(dcli_show,FDB_TABLE_LEN_32K,vlan);
						if (dnumber == 0) {
							npd_syslog_err("NO FDB ITEM! \n");	
							ret = NPD_FDB_ERR_NONE;
					}
				}
		}
	}
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&dnumber);
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
												DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_UINT32_AS_STRING
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for (; dnumber> i; i++)
	{
			if (dcli_show[i].inter_type == CPSS_INTERFACE_PORT_E){
					dcli_flag = CPSS_INTERFACE_PORT_E;
					if(g_cpu_port_index == dcli_show[i].value){
						trans_value1 = CPU_PORT_VIRTUAL_SLOT;
						trans_value2 = CPU_PORT_VIRTUAL_PORT;
					}
					else if(g_spi_port_index == dcli_show[i].value){
						trans_value1 = SPI_PORT_VIRTUAL_SLOT;
						trans_value2 = SPI_PORT_VIRTUAL_PORT;
					}
					else{
						eth_g_index = dcli_show[i].value;
						slot_index=SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
						slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
						local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
						local_portno = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);
						trans_value1 = slot_no;
						trans_value2 = local_portno;
				}
			}
			else if ((dcli_show[i] .inter_type > CPSS_INTERFACE_PORT_E)&&(dcli_show[i] .inter_type <CPSS_INTERFACE_MAX)){
					 if (dcli_show[i] .inter_type == CPSS_INTERFACE_TRUNK_E){ 
						dcli_flag = CPSS_INTERFACE_TRUNK_E;
						
						}
					else if (dcli_show[i] .inter_type == CPSS_INTERFACE_VIDX_E){
						dcli_flag = CPSS_INTERFACE_VIDX_E;
						
						}
					
					else if (dcli_show[i] .inter_type == CPSS_INTERFACE_VID_E){
						dcli_flag = CPSS_INTERFACE_VID_E;
						
						}
					trans_value1 = dcli_show[i].value;
					trans_value2 = 0;
					}
			else {
					npd_syslog_err ("sorry interface type wrong !\n");
					npd_syslog_err ("interface type is: %d\n",dcli_show->inter_type);
			}
	
			DBusMessageIter iter_struct;
			
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
	
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &dcli_flag);

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(dcli_show[i].vlanid));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value1);

			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &trans_value2);
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[0]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[1]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[2]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					&(dcli_show[i].ether_mac[3]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[4]));
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(dcli_show[i].ether_mac[5]));
			dbus_message_iter_close_container (&iter_array, &iter_struct);		
	}
	
	dbus_message_iter_close_container (&iter, &iter_array);
	
return reply;
}

DBusMessage * npd_dbus_fdb_config_system_mac(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned short	vlanid = 0;
	unsigned int	ret = NPD_DBUS_ERROR;
	unsigned int    flag =1;
	struct vlan_s* vid=NULL;
	DBusError err;

	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
												DBUS_TYPE_UINT16,&vlanid,
												DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					npd_syslog_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
			}
			return NULL;
		}
		
	    else{
			if((NULL==(vid = npd_find_vlan_by_vid(vlanid)))||(vid->isAutoCreated)){
				ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
				}
			else{
				ret = nam_static_fdb_entry_mac_set_for_system(vlanid, flag );
				if (ret != NPD_FDB_ERR_NONE) {
					 npd_syslog_err(" nam_static_fdb_entry_mac_set_for_system ERROR. \n");
					 ret = FDB_RETURN_CODE_OCCUR_HW;
				}
			}
	    }
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	/*dbus_message_iter_init_append (reply, &iter);*/
	
	return reply;
	
}


DBusMessage *npd_dbus_fdb_config_port_number(DBusConnection *conn, DBusMessage *msg, void *user_data) {

        DBusMessage* reply;
		DBusMessageIter  iter;
        DBusError err;

		unsigned char slotno = 0,portno = 0;
		unsigned char devNum = 0,portNum = 0;
		unsigned int eth_g_index = 0;
		unsigned int isEnable = 0;/*port admin state, 1 means enable*/
		unsigned int fdblimit = 0;
		unsigned int ret = NPD_FDB_ERR_NONE;
		int          retVal = 0;
		unsigned int vlanid = 0,isTagged = 0,number = 0,flag = 0;
		unsigned int status = 0 ; /*for port learn status , 0 means that auto learn disable*/
		unsigned int enable = 1,disable = 0; /*for port Na enable/disable, 1 means that enable NA msg to cpu*/
		unsigned int aurate = 0,auenable = 1,audisable = 0;
		unsigned int learn = 0,learncmd = 0;
		unsigned short trunkid = 0;

		npd_learn_limit_t limit;
		memset(&limit,0,sizeof(npd_learn_limit_t));
        dbus_error_init(&err);
	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
	    if (!(dbus_message_get_args (msg, &err,
											DBUS_TYPE_BYTE,&slotno,
											DBUS_TYPE_BYTE,&portno,
											DBUS_TYPE_UINT32,&fdblimit,
											DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
	    }
		
		else{
			if (CHASSIS_SLOTNO_ISLEGAL(slotno)) {
		       if (ETH_LOCAL_PORTNO_ISLEGAL( slotno,portno)) {
			      eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,portno);
				  npd_get_devport_by_global_index(eth_g_index,&devNum, &portNum);
				  if(fdblimit > 0){	
				  	 /*check the port if a trunk member*/
					 for(trunkid = 1;trunkid <= CHASSIS_TRUNK_RANGE_MAX;trunkid++){
			            ret = npd_trunk_check_port_membership(trunkid,eth_g_index);
					    if(NPD_TRUE == ret){
						  ret = npd_fdb_number_port_set(slotno,portno, 0);/*delete fdb protect */
					   	  ret =  NPD_FDB_ERR_PORT_NOTIN_VLAN;
						  break;
					    }
					 }
					 if(NPD_FDB_ERR_NONE == ret){
						  /*after deleting,recover the trigger learn status and forward the packets*/
						  ret = npd_fdb_set_port_learn_status(slotno,portno,status);
						  if(NPD_FDB_ERR_NONE!=ret){
							   npd_syslog_err("npd_dbus_fdb_config_port_number:npd_fdb_set_port_learn_status err,%d \n",ret);
							   ret = FDB_RETURN_CODE_GENERAL;
						  }
						  else {
					         ret = npd_fdb_na_msg_per_port_set(slotno,portno,disable);
				             if(NPD_FDB_ERR_NONE!=ret){
				                  npd_syslog_err("npd_dbus_fdb_config_port_number:npd_fdb_na_msg_per_port_set err,%d \n",ret);
		                          ret = FDB_RETURN_CODE_GENERAL;
							 }
							 else{
								nam_fdb_table_delete_entry_with_port(eth_g_index);
								/*set the fdblimit value in sw to control the fdb number*/
								ret = npd_fdb_number_port_set(slotno,portno, fdblimit);
				                if(FDB_BOOL_TRUE != ret)
				                {
				                    npd_fdb_set_port_learn_status(slotno,portno,1);
				                    npd_syslog_err("npd_fdb_number_port_set in npd_dbus_fdb_config_port_number error: slotnum %d,portnum %d,fdblimit %d",slotno,portno,fdblimit);
								    ret = FDB_RETURN_CODE_GENERAL;
				                }
								else{
									 ret = npd_fdb_na_msg_per_port_set(slotno,portno,enable);
						             if(NPD_FDB_ERR_NONE !=ret){
									 	  npd_fdb_set_port_learn_status(slotno,portno,1);
						                  npd_syslog_err("npd_dbus_fdb_config_port_number:npd_fdb_na_msg_per_port_set err,%d \n",ret);
				                          ret = FDB_RETURN_CODE_GENERAL;
									 }
									 else{
										 ret = nam_fdb_au_rate_set(devNum,aurate,auenable);
										 if(NPD_FDB_ERR_NONE != ret){
											  npd_syslog_err("npd set au rate failed,ret %d \n",ret);
											  ret = FDB_RETURN_CODE_GENERAL;								  
										 }								 	
									 }
								 } 
							   }
						 	}
						 	 limit.limit = fdblimit;
							 limit.port = portNum;
							 limit.flags = NPD_L2_LEARN_LIMIT_PORT;
                             retVal = nam_fdb_limit_set(devNum,limit);
							 if(0 != retVal){
                                syslog_ax_fdb_err("set fdb limit failed for %d\n",ret);
								ret = FDB_RETURN_CODE_OCCUR_HW;
							 }						  
					 	}
					}
				    else if( 0 == fdblimit){/* cancel the fdb protect*/
				  	  /*check the port if a trunk member*/
					   for(trunkid = 1;trunkid <= CHASSIS_TRUNK_RANGE_MAX;trunkid++){
			              ret = npd_trunk_check_port_membership(trunkid,eth_g_index);
					      if(NPD_TRUE == ret){
					   	      ret = NPD_FDB_ERR_PORT_NOTIN_VLAN;
							  break;
					      }
					    }	
						if(NPD_FDB_ERR_NONE == ret){
	                        /*set the number to the sw */
		                    ret = npd_fdb_number_port_set(slotno,portno, fdblimit);
			                if(FDB_BOOL_FALSE == ret){
			                   npd_syslog_err("npd_fdb_number_port_set in npd_dbus_fdb_config_port_number error: slotnum %d,portnum %d,fdblimit %d",slotno,portno,fdblimit);
	                           ret = FDB_RETURN_CODE_GENERAL;
							}
						    else{/*flush fdb items*/
						        ret = npd_fdb_na_msg_per_port_set(slotno,portno,disable);
								if(0 != ret){
	                               npd_syslog_dbg("Error to set na msg to port %d/%d disable\n",slotno,portno);
								}
								/*clear the fdb entry in hw*/
				                nam_fdb_table_delete_entry_with_port(eth_g_index);
								/*recover the fdb admin status*/
								for(vlanid = 1;vlanid < 4096;vlanid++){
									if(NPD_TRUE != (ret = npd_vlan_check_contain_port(vlanid,eth_g_index,&isTagged))){
	                                   continue;
									}
									else if((NPD_TRUE == (ret = npd_fdb_number_vlan_port_set_check(vlanid,slotno,portno,&number)))||
										               (NPD_TRUE == (ret = npd_fdb_number_vlan_set_check(vlanid,&number)))){

									    /* fdb limit has another one,enable the na  and set auto learn flag !!*/
										ret = npd_fdb_na_msg_per_port_set(slotno,portno,enable);
										if(0!=ret){
											 npd_syslog_err("npd_dbus_fdb_config_port_number:npd_fdb_na_msg_per_port_set err,%d \n",ret);
											 ret = FDB_RETURN_CODE_GENERAL;
										}
										else{
											 flag++;
	                                         break;
										}
									}
									else {
	                                   continue;
									}
								}
								if(0 == flag){
	                               	ret = npd_fdb_set_port_learn_status(slotno,portno,1);
							        if(0!=ret){
							            npd_syslog_err("npd_dbus_fdb_config_port_number:npd_fdb_set_port_learn_status err,%d \n",ret);
									    ret = FDB_RETURN_CODE_GENERAL;
							        }
								}
							 	 limit.limit = -1;
								 limit.port = portNum;
								 limit.flags = NPD_L2_LEARN_LIMIT_PORT;
	                             retVal = nam_fdb_limit_set(devNum,limit);
								 if(0 != retVal){
	                                syslog_ax_fdb_err("set fdb limit failed for %d\n",ret);
									ret = FDB_RETURN_CODE_OCCUR_HW;
								 }
								 /*JUST IF DISABLE AU RATE LIMIT*/
								 ret = npd_fdb_check_limit_exist(devNum);
								 if(NPD_FALSE == ret){
                                    nam_fdb_au_rate_set(devNum,0,audisable);
								 }
						}
				     }
				  }
		      }
			  else{
                  ret = FDB_RETURN_CODE_BADPARA;
				  npd_syslog_err("The port number input is error!\n");
			  }
		   }
		   else{
               ret = FDB_RETURN_CODE_BADPARA;
			   npd_syslog_err("The slot no input is error!\n");
		   }
		}
		}
        reply = dbus_message_new_method_return(msg);
	
	    dbus_message_iter_init_append (reply, &iter);
		dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,  &ret);	
	    dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &fdblimit);
	
	    return reply;
}

DBusMessage *npd_dbus_fdb_config_vlan_number(DBusConnection *conn, DBusMessage *msg, void *user_data) {

        DBusMessage* reply;
		DBusMessageIter  iter;
        DBusError err;

		unsigned short vlanid = 0;
		unsigned int fdblimit = 0,number = 0;
		unsigned int ret= 0,flag = 0;
		int retVal = 0;
		unsigned int autolearn = 0,disauto = 1;
		unsigned int enable = 1,disable = 0;
		unsigned int devNum = 0;
	    unsigned char slot_index = 0,port_index = 0,slotno = 0,portno = 0;
		unsigned int aurate = 0,auenable = 1,audisable = 0;
		struct vlan_s *vlanNode = NULL ;
		struct vlan_ports  untagPorts, tagPorts;
	    unsigned int i = 0;

		npd_learn_limit_t limit;
		memset(&limit,0,sizeof(npd_learn_limit_t));
        dbus_error_init(&err);
	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		memset(&untagPorts,0,sizeof(struct vlan_ports));
		memset(&tagPorts,0,sizeof(struct vlan_ports));
	    if (!(dbus_message_get_args ( msg, &err,
                                            DBUS_TYPE_UINT16,&vlanid,
											DBUS_TYPE_UINT32,&fdblimit,
											DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
		  
	    }
		
		else{
			
			if((NULL == (vlanNode = npd_find_vlan_by_vid(vlanid)))||(vlanNode->isAutoCreated)){
				ret= FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
			}
			else{
				vlanNode = npd_find_vlan_by_vid(vlanid);
                if((NULL == vlanNode->tagPortList)&&(NULL == vlanNode->untagPortList)){
					ret = NPD_FDB_ERR_VLAN_NO_PORT;
				}
				else{
					if(fdblimit > 0){
					    /* set vlan autolearn disable*/
					    if(NPD_FDB_ERR_NONE != npd_vlan_autolearn_set(devNum,vlanid,autolearn)){
					        ret = FDB_RETURN_CODE_GENERAL;
				        }
						else{
	                        /* set vlan NA to CPU disable*/
						    ret = npd_fdb_na_msg_per_vlan_set(vlanid,disable,1);
						    if(NPD_FDB_ERR_NONE != ret ){
							     npd_syslog_err("Error accured when enable NA msg to CPU: %d \n",ret);
							     ret = FDB_RETURN_CODE_GENERAL;
						     }
							else{
	                            /* set the number 0  to sw to clear the sw.*/
				                ret = npd_fdb_number_vlan_set(vlanid,0);
							    if( NPD_FDB_ERR_NONE !=ret){
								     npd_syslog_err("Error accured when set the fdb number: %d \n",ret);
								     ret = FDB_RETURN_CODE_GENERAL;
							    }
							    else{
									 /*delete the fdb entries before change the value*/
									 nam_fdb_table_delete_entry_with_vlan(vlanid);
									 
									 ret = npd_fdb_number_vlan_set(vlanid,fdblimit);
									 if( NPD_FDB_ERR_NONE !=ret){
									 	 npd_vlan_autolearn_set(devNum,vlanid,disauto);
									     npd_syslog_err("Error accured when set the fdb number: %d \n",ret);
									     ret = FDB_RETURN_CODE_GENERAL;
									  }
									 else {
										 /* set vlan NA to CPU enable*/
										 ret = npd_fdb_na_msg_per_vlan_set(vlanid,enable,1);
										 if(NPD_FDB_ERR_NONE != ret ){
										 	  npd_vlan_autolearn_set(devNum,vlanid,disauto);
											  npd_syslog_err("Error accured when enable NA msg to CPU: %d \n",ret);
											  ret = FDB_RETURN_CODE_GENERAL;
										  }
                                          else{
											  limit.limit = fdblimit;
											  limit.vlan = vlanid;
											  limit.flags = NPD_L2_LEARN_LIMIT_VLAN;
											  retVal = nam_fdb_limit_set(devNum,limit);
											  if(0 != retVal){
											  	 if(NPD_L2_NOT_SUPPORT == retVal){
                                                    ret = NPD_FDB_ERR_HW_NOT_SUPPORT;
												 }
												 else{
												    syslog_ax_fdb_err("set fdb limit failed for %d\n",ret);
												    ret = FDB_RETURN_CODE_OCCUR_HW;
												 }
											  }
											  else{
												 ret = nam_fdb_au_rate_set(devNum,aurate,auenable);
												 if(NPD_FDB_ERR_NONE != ret){
													  npd_syslog_err("npd set au rate failed,ret %d \n",ret);
													  ret = FDB_RETURN_CODE_GENERAL;								  
												 }
											 }											  
										  }
									 }
							    }
							   
							}
						}

					}
					else if( 0 == fdblimit){
                        /* set the number to sw*/
		                ret = npd_fdb_number_vlan_set(vlanid,fdblimit);
					    if( NPD_FDB_ERR_NONE !=ret){
						     npd_syslog_err("Error accured when set the fdb number: %d \n",ret);
						     ret = FDB_RETURN_CODE_GENERAL;
					    }
						ret = npd_vlan_get_all_ports(vlanid,&untagPorts,&tagPorts);
						if(NPD_SUCCESS == ret){
							for(i = 0;i < untagPorts.count;i++){
								slot_index = npd_get_slot_index_from_eth_index(untagPorts.ports[i]);
								slotno = npd_get_slot_no_from_index(slot_index);
								port_index = npd_get_port_index_from_eth_index(untagPorts.ports[i]);
                                portno = npd_get_port_no_from_index(slot_index,port_index);
								if((NPD_TRUE !=(ret = npd_fdb_number_port_set_check(slotno,portno,&number)))&&
									             (NPD_TRUE != (ret = npd_fdb_number_vlan_port_set_check(vlanid,slotno,portno,&number)))){

                                     npd_fdb_na_msg_per_port_set(slotno,portno,NPD_FALSE);
									 flag++;
						        }
							}
							for(i = 0;i < tagPorts.count;i++){
								slot_index = npd_get_slot_index_from_eth_index(tagPorts.ports[i]);
								slotno = npd_get_slot_no_from_index(slot_index);
								port_index = npd_get_port_index_from_eth_index(tagPorts.ports[i]);
                                portno = npd_get_port_no_from_index(slot_index,port_index);
								if((NPD_TRUE !=(ret = npd_fdb_number_port_set_check(slotno,portno,&number)))&&
									             (NPD_TRUE != (ret = npd_fdb_number_vlan_port_set_check(vlanid,slotno,portno,&number)))){

                                     npd_fdb_na_msg_per_port_set(slotno,portno,NPD_FALSE);
									 flag++;
						        }
							}
						}
						if(flag == (untagPorts.count + tagPorts.count)){
                              npd_fdb_na_msg_per_vlan_set(vlanid,0,0);
						}

						ret = nam_fdb_table_delete_entry_with_vlan(vlanid);
				        if( 0 != ret){
				 	         npd_syslog_err("nam_fdb_table_delete_entry_with_vlan in npd_dbus_fdb_config_vlan_number error: slotnum %d,portnum %d,fdblimit %d",slotno,portno,fdblimit);
							 ret = FDB_RETURN_CODE_OCCUR_HW;
					    }
						/* set vlan autolearn enable*/
						if(0 != (ret =npd_vlan_autolearn_set(devNum,vlanid,disauto))){
				          ret = FDB_RETURN_CODE_GENERAL;
			            }
						else{
							limit.limit = -1;
							limit.vlan = vlanid;
							limit.flags = NPD_L2_LEARN_LIMIT_VLAN;
							retVal= nam_fdb_limit_set(devNum,limit);
							if(0 != retVal){
							   if(NPD_L2_NOT_SUPPORT == retVal){
                                  ret = NPD_FDB_ERR_HW_NOT_SUPPORT;
							   }
							   else{
							      syslog_ax_fdb_err("set fdb limit failed for %d\n",ret);
							      ret = FDB_RETURN_CODE_OCCUR_HW;
							   }
							}
							 else{
								 /*JUST IF DISABLE AU RATE LIMIT*/
								 ret = npd_fdb_check_limit_exist(devNum);
								 if(NPD_FALSE == ret){
                                    nam_fdb_au_rate_set(devNum,0,audisable);
								 }
							 }							
						}
					} 
				}
			}
		}
		}
        reply = dbus_message_new_method_return(msg);
	
	    dbus_message_iter_init_append (reply, &iter);
		dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,  &ret);
	
	    dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &fdblimit);
	
	    return reply;
}

DBusMessage *npd_dbus_fdb_config_vlan_port_number(DBusConnection *conn, DBusMessage *msg, void *user_data) {

        DBusMessage* reply;
		DBusMessageIter  iter;
        DBusError err;

		unsigned short vlanid = 0;
		unsigned char slotno = 0;
		unsigned char portno = 0;
		unsigned char devNum = 0,portNum = 0;
		unsigned int eth_g_index;
		unsigned int fdblimit = 0,number = 0;
		unsigned int ret = 0;
		int retVal = 0;
		unsigned int autolearn = 0;
		unsigned int enable = 1,disable = 0;
		unsigned int aurate = 0,auenable = 1,audisable = 0;
		unsigned char tagMode = 0;
        struct vlan_s * vlanNode = NULL;

	if(!((productinfo.capbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_FDB_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_FDB_VLAUE)){
		npd_syslog_dbg("do not support fdb!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		npd_learn_limit_t limit;
		memset(&limit,0,sizeof(npd_learn_limit_t));
		
        dbus_error_init(&err);
	
	    if (!(dbus_message_get_args ( msg, &err,
                                            DBUS_TYPE_UINT16,&vlanid,
                                            DBUS_TYPE_BYTE, &slotno,
                                            DBUS_TYPE_BYTE, &portno,
											DBUS_TYPE_UINT32,&fdblimit,
											DBUS_TYPE_INVALID))) {
		   npd_syslog_err("Unable to get input args ");
		   if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
	    }
		
		else{
			if (CHASSIS_SLOTNO_ISLEGAL(slotno)) {
		       if (ETH_LOCAL_PORTNO_ISLEGAL( slotno,portno)) {
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,portno);
					if((NULL == (vlanNode = npd_find_vlan_by_vid(vlanid)))||(vlanNode->isAutoCreated)){
		               ret = FDB_RETURN_CODE_NODE_VLAN_NONEXIST;
					}
					else if( 0 == npd_vlan_check_contain_port(vlanid,eth_g_index,&tagMode)){
		               ret = NPD_FDB_ERR_PORT_NOTIN_VLAN;
					}
					else{
						if(fdblimit > 0){
							npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	                            ret = npd_fdb_na_msg_per_vlan_set(vlanid,disable,1);
								if(0 != ret ){
									npd_syslog_err("Error accured when enable NA msg to CPU: %d \n",ret);
									 ret = FDB_RETURN_CODE_GENERAL;
								}
								else{
								    /* trigger learning and forwarding*/
									ret = npd_fdb_set_port_learn_status(slotno,portno,autolearn);
							        if(0!=ret){
							              npd_syslog_err("npd_dbus_fdb_config_port_number:npd_fdb_set_port_learn_status err,%d \n",ret);
									      ret = FDB_RETURN_CODE_GENERAL;
							        }
									else {
										/*clear the fdb entry in hw*/
										nam_fdb_table_delete_entry_with_vlan_port(vlanid,eth_g_index);

										/*set the fdb limit vlaue in software to control fdb learning*/
										ret = npd_fdb_number_vlan_port_set(vlanid,slotno,portno,fdblimit);
										if( NPD_TRUE !=ret){
											 npd_fdb_set_port_learn_status(slotno,portno,1);
											 npd_syslog_err("Error accured when set the fdb number: %d \n",ret);
											 ret = FDB_RETURN_CODE_GENERAL;
										}
										ret = npd_fdb_na_msg_per_port_set(slotno,portno,enable);
								        if(0!=ret){
											  npd_fdb_set_port_learn_status(slotno,portno,1);
								              npd_syslog_err("npd_dbus_fdb_config_port_number:npd_fdb_na_msg_per_port_set err,%d \n",ret);
										      ret = FDB_RETURN_CODE_GENERAL;
								        }
										 else{    
										 ret = nam_fdb_au_rate_set(devNum,aurate,auenable);
										 if(NPD_FDB_ERR_NONE != ret){
											  npd_syslog_err("npd set au rate failed,ret %d \n",ret);
											  ret = FDB_RETURN_CODE_GENERAL;								  
										 }
										  limit.limit = fdblimit;
										  limit.vlan= vlanid;
										  limit.port = portNum;
										  limit.flags = NPD_L2_LEARN_LIMIT_PORT | NPD_L2_LEARN_LIMIT_VLAN;
										  retVal = nam_fdb_limit_set(devNum,limit);
										  if(0 != retVal){
										  	 if(NPD_L2_NOT_SUPPORT == retVal){
                                                 ret = NPD_FDB_ERR_HW_NOT_SUPPORT;
											 }
											 else{
											     syslog_ax_fdb_err("set fdb limit failed for %d\n",ret);
											     ret = FDB_RETURN_CODE_OCCUR_HW;
											  }
										  }
									}
								}   
							}
					    }
						else if(0 == fdblimit){
					        ret = npd_fdb_number_vlan_port_set(vlanid,slotno,portno,fdblimit);
							if( 1 !=ret){
								npd_syslog_err("Error accured when set the fdb number: %d \n",ret);
								 ret = FDB_RETURN_CODE_GENERAL;
							}
							else {	
								/* set port autolearn enable*/
			                    if((NPD_TRUE != (ret = npd_fdb_number_port_set_check(slotno,portno,&number)))&&(NPD_TRUE != (ret = npd_fdb_number_vlan_set_check(vlanid,&number)))){
									ret = npd_fdb_set_port_learn_status(slotno,portno,1);
									if(0!=ret){
										npd_syslog_err("npd_fdb_set_port_learn_status err,%d \n",ret);
										ret = FDB_RETURN_CODE_GENERAL;
								     }
									ret = npd_fdb_na_msg_per_port_set(slotno,portno,NPD_FALSE);
									if(0!=ret){
										npd_syslog_err(" npd_fdb_na_msg_per_port_set err,%d \n",ret);
										ret = FDB_RETURN_CODE_GENERAL;
									}
							    }
					             ret = nam_fdb_table_delete_entry_with_vlan_port(vlanid,eth_g_index);
					             if( 0 != ret){
							 	     npd_syslog_err("nam_fdb_table_delete_entry_with_vlan_port in npd_dbus_fdb_config_vlan_port_number error: slotnum %d,portnum %d,fdblimit %d",slotno,portno,fdblimit);
                                     ret = FDB_RETURN_CODE_OCCUR_HW;
								 }
								 
								 else{	  
								 	 /*broadcom platform used*/
									  limit.limit = -1;
									  limit.vlan= vlanid;
									  limit.port = portNum;
									  limit.flags |= NPD_L2_LEARN_LIMIT_PORT | NPD_L2_LEARN_LIMIT_VLAN;
									  retVal = nam_fdb_limit_set(devNum,limit);
									  if(0 != retVal){
										  if(NPD_L2_NOT_SUPPORT == retVal){
											  ret = NPD_FDB_ERR_HW_NOT_SUPPORT;
										  }
										  else{
											  syslog_ax_fdb_err("set fdb limit failed for %d\n",ret);
											  ret = FDB_RETURN_CODE_OCCUR_HW;
										  }
									  }
									  else{
										  /*JUST IF DISABLE AU RATE LIMIT*/
										  ret = npd_fdb_check_limit_exist(devNum);
										  if(NPD_FALSE == ret){
											 nam_fdb_au_rate_set(devNum,0,audisable);
										  }
									  }
								  }
						    }							
						}
					}
		       	}
			    else{
                     ret = FDB_RETURN_CODE_BADPARA;
					 npd_syslog_err("The port number input is error\n");
				}
			}
			else{
                 ret = FDB_RETURN_CODE_BADPARA;
				 npd_syslog_err("The slot number input is error\n");
			}
		 }    
		}
        reply = dbus_message_new_method_return(msg);
	
	    dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,  &ret);
	
	    dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &fdblimit);
	
	    return reply;
}





DBusMessage *npd_dbus_fdb_show_limit_item(DBusConnection *conn, DBusMessage *msg, void *user_data)
{

        DBusMessage* reply;
		DBusMessageIter  iter;
		DBusMessageIter	 iter_array;

		unsigned char slot_no =0;
		unsigned int slot_index = 0;
		unsigned int local_port_index = 0;
		unsigned char local_port_no =0;
		unsigned int fdblimit=0;
		unsigned int ret =0;
		unsigned int number = 0;
		unsigned int count = 0;
		int i = 0;
        struct vlan_port_list_node_s *vlan_node = NULL;
	    struct vlan_s *vlanNode = NULL;
	    vlan_port_list_s *portList = NULL;
	    struct list_head  *list = NULL,*pos = NULL;

		/*TODO::display 50 items now,then modified it.*/
		struct fdb_number_limit_s node[50];
        memset(node,0,sizeof(struct fdb_number_limit_s)*50);

		/*for the vlan-based*/
	    for(i = 1;i<4095;i++){
		  ret = npd_check_vlan_exist(i);
		  if(VLAN_RETURN_CODE_VLAN_EXISTS == ret){
		     ret = npd_fdb_number_vlan_set_check(i,&fdblimit);
			 if(1 == ret){
                number += fdblimit;
			    node[count].vlanId = i;
			    node[count].number = fdblimit;
			    if(count < 50)
			      count++;
			 }
			 /*for the vlan-port-based*/
			 vlanNode = npd_find_vlan_by_vid(i);
	         if(NULL == vlanNode) {
		        return NPD_FALSE;
	         }
			 
	         /* tagged port list*/
		      portList = vlanNode->tagPortList;
              if((NULL != portList)&&(0 != portList->count)){
		        list = &(portList->list);
		        __list_for_each(pos,list) {
			       vlan_node = list_entry(pos,struct vlan_port_list_node_s,list);
				   	  slot_index=SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(vlan_node->eth_global_index);
					  slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
					  local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(vlan_node->eth_global_index);
					  local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);
					  ret = npd_fdb_number_vlan_port_set_check(i,slot_no,local_port_no,&fdblimit);
                      if(1==ret){
					     number += fdblimit;
			             node[count].vlanId = i;
					     node[count].slot_no = slot_no;
					     node[count].local_port_no = local_port_no;
			             node[count].number = fdblimit;
						 /*code optimize:  Out-of-bounds wirite
						zhangcl@autelan.com 2013-1-17*/
			             if(count < 49)
			                count++; 
                      }
		       }
	        }
			 portList = vlanNode->untagPortList;
			 if((NULL != portList)&&(0 != portList->count))
             {
		        list = &(portList->list);
		        __list_for_each(pos,list) {
			       vlan_node = list_entry(pos,struct vlan_port_list_node_s,list);
				   slot_index=SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(vlan_node->eth_global_index);
				   slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
				   local_port_index = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(vlan_node->eth_global_index);
				   local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_index);
				   ret = npd_fdb_number_vlan_port_set_check(i,slot_no,local_port_no,&fdblimit);
				   if(1==ret){
					  number += fdblimit;
			          node[count].vlanId = i;
					  node[count].slot_no = slot_no;
					  node[count].local_port_no = local_port_no;
			          node[count].number = fdblimit;
			          if(count < 50)
			             count++;
				   }    
		       }
	        }

	      }
		  else {continue;}
	   }

		/*for the port-base*/
		for(slot_no = CHASSIS_SLOT_START_NO;slot_no < (CHASSIS_SLOT_COUNT+CHASSIS_SLOT_START_NO);slot_no++){
			if(CHASSIS_SLOTNO_ISLEGAL(slot_no)){
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				for(local_port_no = ETH_LOCAL_PORT_START_NO(slot_index);local_port_no <(ETH_LOCAL_PORT_COUNT(slot_index)+ETH_LOCAL_PORT_START_NO(slot_index));local_port_no++){
                    if(ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)){
						ret = npd_fdb_number_port_set_check(slot_no,local_port_no,&fdblimit);
						if(1 == ret){
							number += fdblimit;
							node[count].slot_no = slot_no;
							node[count].local_port_no = local_port_no;
							node[count].number = fdblimit;
							if(count<50)
								count++;
						}
                    }
				}
			}
		}				
        reply = dbus_message_new_method_return(msg);
	    dbus_message_iter_init_append (reply, &iter);
			
	    dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,&count);
	    dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									            DBUS_TYPE_UINT16_AS_STRING
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											    DBUS_TYPE_UINT32_AS_STRING 
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for (i = 0; i < count; i++)
	{
			
			
			DBusMessageIter iter_struct;
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			/* VLAN ID*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT16,
					 &(node[i].vlanId));

			/* SLOT number or VLAN ID or VIDX or Trunk ID*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(node[i].slot_no));

			/* PORT number*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_BYTE,
					 &(node[i].local_port_no));

			/*FDB limit number*/
			dbus_message_iter_append_basic
					(&iter_struct,
					 DBUS_TYPE_UINT32,
					 &(node[i].number));

			
			dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	
	dbus_message_iter_close_container (&iter, &iter_array);
	
    return reply;
}


DBusMessage * npd_dbus_create_vrrp_by_ifname(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;

	unsigned int ifindex1 = ~0UI,ifindex2 = ~0UI,ret = NPD_DBUS_SUCCESS;
	char* pname1 = NULL;
	char* pname2 = NULL;
	unsigned int i = 0;
	static unsigned int callTime = 0;
	int add = 0;

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_STRING,&pname1,
								DBUS_TYPE_STRING,&pname2,
								DBUS_TYPE_UINT32,&add,
								DBUS_TYPE_INVALID))) {
		syslog_ax_intf_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_intf_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	syslog_ax_intf_dbg("call %s line %d pname1 %s, ifindex1 %d,pname2 %s,ifindex2 %d\r\n", __func__, __LINE__,pname1,ifindex1,pname2,ifindex2);
    if(add){
		if(0 == strcmp(pname1,pname2)){
			ret = npd_fdb_create_for_vrrp(pname1);
	    }
		else if(0 != (ret = npd_fdb_create_for_vrrp(pname1))){
		   syslog_ax_intf_err("call npd_create_vlan_intf_by_vid ret %d,ifindex %d\n",ret ,ifindex1);
		}
		else if(0 != (ret = npd_fdb_create_for_vrrp(pname2))){
		   syslog_ax_intf_err("call npd_create_vlan_intf_by_vid ret %d,ifindex %d\n",ret ,ifindex2);
		}
    }
	else{
		if(0 == strcmp(pname1,pname2)){
			ret = npd_fdb_del_for_vrrp(pname1);
	    }
		else if(0 != (ret = npd_fdb_del_for_vrrp(pname1))){
		   syslog_ax_intf_err("call npd_create_vlan_intf_by_vid ret %d,ifindex %d\n",ret ,ifindex1);
		}
		else if(0 != (ret = npd_fdb_del_for_vrrp(pname2))){
		   syslog_ax_intf_err("call npd_create_vlan_intf_by_vid ret %d,ifindex %d\n",ret ,ifindex2);
		}

	}
    reply = dbus_message_new_method_return(msg);

    dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
								DBUS_TYPE_UINT32,  &ret);

    return reply;	
}


DBusMessage *npd_dbus_fdb_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data) {
		DBusMessage* reply;
		DBusMessageIter  iter;
	
		char *showStr = NULL,*cursor = NULL;
		struct fdb_entry_node_s *staticnode = NULL;
		struct fdb_entry_node_s **staticarray = NULL;

		struct fdb_blacklist_node_s *blacklistnode = NULL;
		struct fdb_blacklist_node_s **blacklistarray = NULL;
		unsigned int totalCount =0;
		int i = 0, j = 0,entryCount = 0;
		unsigned int lenth = 0;
    		
		struct vlan_s* node = NULL;
		struct vlan_port_list_node_s *portNode = NULL;
		vlan_port_list_s* portList = NULL;
		struct list_head *pos = NULL,*list	= NULL;
		unsigned int eth_g_index = 0;
		unsigned char slot_index = 0,slot_no = 0,local_port_no = 0;
		unsigned int ret = 0;
		unsigned int fdblimit = 0,fdbnumber = 0;
		unsigned int  number = 0;
		
		/*showStr = (char*)malloc(NPD_FDB_SHOWRUN_CFG_SIZE);*/
		showStr = (char*)malloc(sizeof(struct fdb_entry_node_s)*1024+sizeof(struct fdb_blacklist_node_s)*1024+1024*1024*2);
		if(NULL == showStr){ 
			npd_syslog_err("memory malloc error\n");
			return NULL;
		}
		memset(showStr,0,(sizeof(struct fdb_entry_node_s)*1024+sizeof(struct fdb_blacklist_node_s)*1024+1024*1024*2));
		cursor = showStr;
		cursor += sprintf(cursor,"\n");
		totalCount = g_static_fdb_count + g_blacklist_fdb_count ;
		staticarray = npd_fdb_get_entry();
		blacklistarray = npd_fdb_get_blacklist();

		#if 1
		for (i = 0; i<AX_STATIC_FDB_ENTRY_SIZE; i++) {
			
            char tmpBuf[256] = {0};
			
			if(entryCount >= totalCount) { /* valid entry count reach*/
				break;
			}
			staticnode = staticarray[i];
			blacklistnode = blacklistarray[i];
			
			if(staticnode != NULL) {
			   if(1 != staticnode->isMirror){
				   entryCount++;
				   if(0 == staticnode->trunkid){
					   lenth = sprintf(tmpBuf,"create fdb static mac %02x:%02x:%02x:%02x:%02x:%02x vlan %d port %d/%d\n",	\
											staticnode->mac[0],staticnode->mac[1],staticnode->mac[2],staticnode->mac[3],	\
											staticnode->mac[4],staticnode->mac[5],staticnode->vlanid,staticnode->slot,staticnode->port);
				   }
				   else{
	                  lenth = sprintf(tmpBuf,"create fdb static mac %02x:%02x:%02x:%02x:%02x:%02x vlan %d trunk %d\n",	\
											staticnode->mac[0],staticnode->mac[1],staticnode->mac[2],staticnode->mac[3],	\
											staticnode->mac[4],staticnode->mac[5],staticnode->vlanid,staticnode->trunkid); 
				   }
	               if((cursor+lenth-showStr) >= NPD_FDB_SHOWRUN_CFG_SIZE - 2) /* 2 - two '\n' in the head and bottom*/
	                  { break;}
				   sprintf(cursor,"%s",tmpBuf);
				   cursor +=lenth;
			   }
			}
			
            if(blacklistnode != NULL){
				if(1 == (blacklistnode->dmac)){
				   entryCount++;
				   lenth = sprintf(tmpBuf,"create fdb blacklist %4s %02x:%02x:%02x:%02x:%02x:%02x vlan %d \n", "dmac",	\
										blacklistnode->mac[0],blacklistnode->mac[1],blacklistnode->mac[2],blacklistnode->mac[3],blacklistnode->mac[4],blacklistnode->mac[5],blacklistnode->vlanid);
                   if((cursor+lenth-showStr) >= (NPD_FDB_SHOWRUN_CFG_SIZE - 2)) /* 2 - two '\n' in the head and bottom*/
                      { break;}
			        sprintf(cursor,"%s",tmpBuf);
			       cursor +=lenth;
				}
				if(1 == (blacklistnode->smac)){
				   if(!blacklistnode->dmac){
                          entryCount++; /*if is dmac & smac , one node*/
				   	}
				   lenth = sprintf(tmpBuf,"create fdb blacklist %4s %02x:%02x:%02x:%02x:%02x:%02x vlan %d \n", "smac",	\
										blacklistnode->mac[0],blacklistnode->mac[1],blacklistnode->mac[2],blacklistnode->mac[3],blacklistnode->mac[4],blacklistnode->mac[5],blacklistnode->vlanid);
                   if((cursor+lenth-showStr) >= (NPD_FDB_SHOWRUN_CFG_SIZE - 2)) /* 2 - two '\n' in the head and bottom*/
                      { break;}
			        sprintf(cursor,"%s",tmpBuf);
			        cursor +=lenth;
				}
			}
		}

		for(i = 1; i < 4095; i++) {
			char tmpBuf[128] = {0};
			if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i)) {
				node = npd_find_vlan_by_vid(i);
				if(NULL != node) {
					portList = node->untagPortList;
					if(NULL != portList) {
						if(0 != portList->count) {				
							list = &(portList->list);
							__list_for_each(pos,list) {
								portNode = list_entry(pos,struct vlan_port_list_node_s,list);
								if(portNode) {
									eth_g_index = portNode->eth_global_index;
									slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
									slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
									local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
									local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_no);
									ret = npd_fdb_number_vlan_port_set_check(i, slot_no,local_port_no,&fdblimit);
									if(1 == ret){
										if((lenth + 30 )<(NPD_FDB_SHOWRUN_CFG_SIZE - 2)){
											lenth = sprintf(tmpBuf,"config fdb vlan %d port %d/%d %d\n",i,slot_no,local_port_no,fdblimit);
											syslog_ax_vlan_dbg("%s",tmpBuf);
											sprintf(cursor,"%s",tmpBuf);
											cursor = cursor + lenth;
										}
									}
								}
							}
						}
					}
	
					portList = node->tagPortList;
					if(NULL != portList) {
						if(0 != portList->count) {
							list = &(portList->list);
							__list_for_each(pos,list) {
								portNode = list_entry(pos,struct vlan_port_list_node_s,list);
								if(portNode) {
									eth_g_index = portNode->eth_global_index;
									slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
									slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
									local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
									local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_no);

									ret = npd_fdb_number_vlan_port_set_check(i, slot_no,local_port_no,&fdblimit);
									if(1 == ret){
										if((lenth + 30 )< (NPD_FDB_SHOWRUN_CFG_SIZE - 2)){
											lenth = sprintf(tmpBuf,"config fdb vlan %d port %d/%d %d\n",i,slot_no,local_port_no,fdblimit);
											syslog_ax_vlan_dbg("%s",tmpBuf);
											sprintf(cursor,"%s",tmpBuf);
											cursor = cursor + lenth;
										}
									}
								}
							}
						}
					}

	
					/*added by zhengcs,save config fdb number per vlan*/
					ret = npd_fdb_number_vlan_set_check(i, &number);
					if( NPD_TRUE != ret){
						;/* syslog_ax_vlan_err("vlan %d does not set the fdb limit \n",i);*/
					}
					else {
	
						if((lenth + 23) < (NPD_FDB_SHOWRUN_CFG_SIZE - 2)) {
							lenth = sprintf(tmpBuf,"config fdb vlan %d %d\n",i,number);
							syslog_ax_vlan_dbg("%s",tmpBuf);
							sprintf(cursor,"%s",tmpBuf);
							cursor = cursor + lenth;
						}
					}
			   }
		    }
	    }

		if((PRODUCT_ID_AX7K_I == PRODUCT_ID) || (PRODUCT_ID_AX7K == PRODUCT_ID)) {
			i = 1; /* AX7K CRSMU has no fdb limit settings*/
		}
		else {
			i = 0;
		}
        for (; i < CHASSIS_SLOT_COUNT; i++ ) {
			unsigned char local_port_count = ETH_LOCAL_PORT_COUNT(i);
			for (j = 0; j < local_port_count; j++ ) {
				unsigned char local_port_no = ETH_LOCAL_INDEX2NO(i,j);
				unsigned int slot_no = CHASSIS_SLOT_INDEX2NO(i);
				unsigned int ret;
				ret = npd_fdb_number_port_set_check(slot_no,local_port_no,&fdbnumber);
                if( NPD_TRUE != ret){
						;/* syslog_ax_vlan_err("port %d/%d does not set the fdb limit \n",slot_no, local_port_no);*/
				}
				else {
                    char tmpBuf[128] = {0};
					if((lenth + 23) < (NPD_FDB_SHOWRUN_CFG_SIZE - 2)) {
						lenth = sprintf(tmpBuf,"config fdb port %d/%d %d\n",slot_no,local_port_no,fdbnumber);
						syslog_ax_vlan_dbg("%s",tmpBuf);
						sprintf(cursor,"%s",tmpBuf);
						cursor = cursor + lenth;
					}
				}

			}
		}

		#endif
		if(!((cursor+25-showStr) >= NPD_FDB_SHOWRUN_CFG_SIZE - 2)&&(g_agingtime != 300)){
                char tmpBuf[128] = {0};
				lenth =  sprintf(tmpBuf,"config fdb agingtime %d\n",g_agingtime);
				 npd_syslog_dbg("agingtim changed\n");
				sprintf(cursor,"%s",tmpBuf);
				cursor +=lenth;
			}
		 sprintf(cursor,"\n");
		/* npd_syslog_dbg("%s",showStr);*/
		reply = dbus_message_new_method_return(msg);
		
		dbus_message_iter_init_append (reply, &iter);
		
		dbus_message_iter_append_basic (&iter,
										 DBUS_TYPE_STRING,
										 &showStr); 
		free(showStr);
		showStr = NULL;
		
		return reply;
}

#define SFD_ON		1
#define SFD_OFF		0

int sfd_flag = SFD_OFF;
int sfd_debug_flag = SFD_OFF;

/*sfd command*/
DBusMessage *npd_dbus_sfd_show_info(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusError err;
	dbus_error_init(&err);
	
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_INT32,&sfd_flag);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_INT32,&sfd_debug_flag);
	
   return reply;
}

DBusMessage *npd_dbus_sfd_service_debug(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusError err;
	int flag=0;
	int ret=0;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_INT32,&flag,
											DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					npd_syslog_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
			}
			ret=1;
	}
	sfd_debug_flag = flag;
	npd_syslog_dbg("sfd_debug_flag = %d\n",sfd_debug_flag);
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_INT32,&ret);
	
   return reply;
}

DBusMessage *npd_dbus_sfd_service(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	DBusError err;
	int flag=0;
	int ret=0;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_INT32,&flag,
											DBUS_TYPE_INVALID))) {
			npd_syslog_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					npd_syslog_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
			}
			ret=1;
	}
	sfd_flag = flag;
	npd_syslog_dbg("sfd_flag = %d\n",sfd_flag);
	reply = dbus_message_new_method_return(msg);
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_INT32,&ret);
	
   return reply;
}

#ifdef __cplusplus
}
#endif
