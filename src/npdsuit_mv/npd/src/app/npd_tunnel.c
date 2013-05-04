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
* npd_tunnel.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		API used in NPD dbus process for TUNNEL module
*
* DATE:
*		05/01/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.20 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/rtnetlink.h>
#include <linux/in.h>
#include <pthread.h>
#include <bits/sockaddr.h>
#include <dbus/dbus.h>
#include <sysdef/npd_sysdef.h>
#include <sys/stat.h>
#include <linux/netlink.h>
#include <linux/socket.h>

#include "dbus/npd/npd_dbus_def.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_log.h"
#include "npd_hash.h"
#include "npd_tunnel.h"
#include "npd_product.h"
#include "npd_c_slot.h"
#include "npd_eth_port.h"
#include "sysdef/returncode.h"

struct Hash  *tunnel_db_s = NULL;
unsigned int tt_table[TT_TABLE_LEN];
extern g_cpu_port_index;
extern g_spi_port_index;

#define MAX_PAYLOAD 1024
#ifdef CPU_ARM_XCAT
#define RTNLGRP_IPV4_IFADDR 5 
#endif
#define GRP_ID  (1 << (RTNLGRP_IPV4_IFADDR-1)) 

#define TUNNEL_HASH_SIZE  256/*tunnel_hash_size*/
#define TUNNEL_HASH(addr) ((addr^(addr>>8))&0xFF)/*tunnel_hash*/
#ifdef CPU_ARM_XCAT
extern unsigned int g_npd_thread_number;
#endif

extern unsigned int nam_show_fdb_one
(
	NPD_FDB			*fdb,
	unsigned char		macAddr[6],
	unsigned short	vlanId
);

/**********************************************************************************
 * npd_tunnel_compare_byip
 *
 * compare two of tunnel database(Hash table) items, by dstip add srcip
 *
 *	INPUT:
 *		itemA	- tunnel database item
 *		itemB	- tunnel database item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 	- if two items are not equal.
 *		1 	- if two items are equal.
 *
 **********************************************************************************/
int npd_tunnel_compare_byip
(
	struct tunnel_item_s *itemA,
	struct tunnel_item_s *itemB
)
{
	if ((NULL== itemA)||(NULL== itemB)) {
		npd_syslog_err("npd tunnel items compare null pointers error.");
		return TUNNEL_RETURN_CODE_NULLPOINTER_2;
	}

	if ((itemA->kmsg.dstip == itemB->kmsg.dstip) &&
		(itemA->kmsg.srcip == itemB->kmsg.srcip)) { 
		return NPD_TRUE;
	}

	return NPD_FALSE;
}

/**********************************************************************************
 * npd_tunnel_compare_byip
 *
 * compare two of tunnel database(Hash table) items, by kernel msg
 *
 *	INPUT:
 *		itemA	- tunnel database item
 *		itemB	- tunnel database item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 	- if two items are not equal.
 *		1 	- if two items are equal.
 *
 **********************************************************************************/
 int npd_tunnel_compare_kmsg
(
	struct tunnel_item_s *itemA,
	struct tunnel_item_s *itemB
)
{
	if ((NULL== itemA)||(NULL== itemB)) {
		npd_syslog_err("npd tunnel items compare null pointers error.");
		return TUNNEL_RETURN_CODE_NULLPOINTER_2;
	}

	if (!memcmp(&itemA->kmsg, &itemB->kmsg, sizeof(tunnel_kernel_msg_t))) { 
		return NPD_TRUE;
	}

	return NPD_FALSE;
}

/**********************************************************************************
 * npd_tunnel_compare_byip
 *
 * compare two of tunnel database(Hash table) items, by host ip
 *
 *	INPUT:
 *		itemA	- tunnel database item
 *		itemB	- tunnel database item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		0 	- if two items are not equal.
 *		1 	- if two items are equal.
 *
 **********************************************************************************/
int npd_tunnel_compare_byhostip
(
	struct tunnel_item_s *itemA,
	struct tunnel_item_s *itemB
)
{
	struct tunnel_host_s *hostA = NULL;
	struct tunnel_host_s *hostB = NULL;
	struct list_head         *ptr = NULL;
	if ((NULL == itemA)||(NULL == itemB)) 
	{
		npd_syslog_err("npd tunnel items compare null pointers error.");
		return TUNNEL_RETURN_CODE_NULLPOINTER_2;
	}
	/* find itemB's hostdip and hdiplen*/
	__list_for_each(ptr, &(itemB->list1)) 
	{
		hostB = list_entry(ptr, struct tunnel_host_s, list);
		if (NULL != hostB) 
		{
			break;
		}
	}
	if (NULL == hostB) 
	{
		npd_syslog_dbg("bad itemB, hostB not exist !!\n");
		return NPD_FALSE;
	}
	__list_for_each(ptr, &(itemA->list1)) 
	{
		hostA = list_entry(ptr, struct tunnel_host_s, list);
		if (NULL == hostA) 
		{
			if ((hostA->hostdip == hostB->hostdip) &&
				(hostA->hdiplen == hostB->hdiplen)) 
			{
				return NPD_TRUE;
			}
		}
	}

	return NPD_FALSE;
}

/**********************************************************************************
 * npd_tunnel_key_generate
 *
 * tunnel database(Hash table) hash key generation method
 *
 *	INPUT:
 *		item	- tunnel database item
 *
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		hash key calculated if no error occur
 *
 **********************************************************************************/
 unsigned int npd_tunnel_key_generate
(
	struct tunnel_item_s *item
)
{
	unsigned int key = 0;

	if (NULL == item) {
		npd_syslog_err("npd tunnel items make key null pointers error.");
		return TUNNEL_RETURN_CODE_ERROR;
	}
/*	
	ptr = (unsigned char*)(&(item->dstip));
	
	for (i = 0; i < IPV4_ADDRESS_LEN; i++) {
		key += ptr[i];
	}

	key %= (TUNNEL_HASH_SIZE);
*/
/*dstip>>16*/
	key = TUNNEL_HASH((item->kmsg.dstip)>>16);
	key += TUNNEL_HASH(item->kmsg.srcip); /* change this beatiful later*/
	key %= (TUNNEL_HASH_SIZE);
	npd_syslog_dbg("get hash tunnel key is %d \n", key);

	return key;
}

/**********************************************************************************
 * npd_tunnel_init
 *
 * tunnel database(Hash table) initialization
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
void npd_tunnel_init
(
	void
)
{
	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){/*5612e not support*/
	/*	return;
	}*/
	tunnel_db_s = hash_new(TUNNEL_HASH_SIZE);

	tunnel_db_s->hash_cmp = npd_tunnel_compare_byip;
	tunnel_db_s->hash_key = npd_tunnel_key_generate;
	memset(tt_table, 0, sizeof(unsigned int)*TT_TABLE_LEN);
}

/**********************************************************************************
 * npd_tunnel_release
 *
 *  	release about tunnel start add tunnel terminal res
 *	INPUT:
 *		item:	msg about tunnel start add tunnel terminal
 *		tstten:	release ts tt
 *		rten:	release rt
 *
 *	OUTPUT:
 *		NONE
 *
 * 	RETURN:
 *		NONE
 *
 *	NOTE:
 *
 **********************************************************************************/
void npd_tunnel_release
(
	struct tunnel_item_s *item,
	unsigned int tstten,
	unsigned int rten
)
{
	int ret = TUNNEL_RETURN_CODE_SUCCESS;
	struct tunnel_host_s *host = NULL;
	struct list_head		 *ptr = NULL;
	struct tunnel_item_s *data = item;
	if (NULL == data) 
	{
		npd_syslog_err("item is NULL \n");
		return ;		/* code optimize: Dereference after null check houxx@autelan.com  2013-1-18 */
	}
	if (tstten) 
	{
		if (data->state) 
		{
			if (TUNNEL_TS_SW_EN & data->state) {
				ret = npd_tunnel_del_ts_tab(data->tsindex);
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) 
				{
					npd_syslog_err("del ts index at tunnel release!! \n");
				}
			}
			if (TUNNEL_TS_HW_EN & data->state) 
			{
				ret = nam_tunnel_start_del_entry(data->kmsg.devnum, data->tsindex, TUNNEL_IPV4_OVER_IPV4_E, data->nhindex);
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) 
				{
					npd_syslog_err("del ts fail at tunnel release!! \n");
				}
			}
			if (TUNNEL_TT_SW_EN & data->state) 
			{
				ret = npd_tunnel_del_tt_tab(tt_table, sizeof(unsigned int)*TT_TABLE_LEN, (data->ttindex - TUNNEL_TERM_FRIST_NUM));
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) 
				{
					npd_syslog_err("del tt fail at tunnel release!! \n");				
				}
			}
			if (TUNNEL_TT_HW_EN & data->state) {
				ret = nam_tunnel_term_entry_del(data->kmsg.devnum, data->ttindex, TUNNEL_IPV4_OVER_IPV4_E);
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) 
				{
					npd_syslog_err("del tt fail at tunnel release!! \n");				
				}
			}			
			if (TUNNEL_NH_SW_EN & data->state) 
			{
				ret = npd_tunnel_del_nexthop_tab(data->nhindex);
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) 
				{
					npd_syslog_err("tunnel nh del fail ! \n");
				}
			}
			if (TUNNEL_NH_HW_EN & data->state) 
			{
				ret = nam_tunnel_nh_del(data->kmsg.devnum, data->nhindex);
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) 
				{
					npd_syslog_err("tunnel nh del fail ! \n");
				}
			}				
		}
		else 
		{
			npd_syslog_dbg("tunnel ts tt nh is empty \n");
		}
	}	

	if (rten) 
	{
		__list_for_each(ptr, &(data->list1))
		{
			host = list_entry(ptr, struct tunnel_host_s, list);
			if (NULL != host)
			{
				ret = nam_tunnel_del_tcam_ipltt(host->hostdip, host->hdiplen);
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) 
				{
					npd_syslog_err("tunnel del tcam and ipltt fail at tunnel release!\n");
				}
				data->hostnum -= 1;
				__list_del((host->list).prev, (host->list).next);
			}
		}
	}
	
	if (tstten & rten) 
	{
		if (!data->hostnum) 
		{
			hash_pull(tunnel_db_s, data);
		}
		else 
		{
			npd_syslog_dbg("tunnel route host num is not");
		}
	}
	
}

/* Improve error handling */
/* !!!!!!! dmac in ts same as arp next hop mac*/
/**********************************************************************************
 * npd_tunnel_handle_tstt_msg
 *
 *  	handle the tunnel msg about tunnel start add tunnel terminal
 *
 *	INPUT:
 *		item:	msg about tunnel start add tunnel terminal
 *	
 *	OUTPUT:
 *		NONE
 *
 * 	RETURN:
 *		TUNNEL_RETURN_CODE_SUCCESS
 *		TUNNEL_RETURN_CODE_ERROR
 *	NOTE:
 *
 **********************************************************************************/
int npd_tunnel_handle_tstt_msg
(
	struct tunnel_kernel_msg_s *item
)
{
	unsigned int ret = TUNNEL_RETURN_CODE_SUCCESS, i = 0;
	unsigned int ts = ~0UI; /* tunnel start table index, valid range [0,1023] */
	unsigned int tt = ~0UI; /* tunnel termination table index, valid range [0,127] */
	unsigned int np = ~0UI; /* next hop table index, valid range [0,4095] */
	struct tunnel_item_s *data = NULL, *checkptr = NULL;
	ETHERADDRS sysmac ;
	
	if (NULL == item) {
		npd_syslog_err("tunnel handle ts tt msg item is NULL \n");
		return TUNNEL_RETURN_CODE_ERROR;
	}

	memset(&sysmac, 0, sizeof(ETHERADDRS));
	data = malloc(sizeof(struct tunnel_item_s));
	if (NULL == data) {
		npd_syslog_err("tunnel item data malloc fail \n");
		return TUNNEL_RETURN_CODE_ERROR;
	}

	memset(data, 0, sizeof(struct tunnel_item_s));
	data->kmsg.dstip = item->dstip;
	data->kmsg.srcip = item->srcip;
	data->kmsg.vid = item->vid;
	data->kmsg.devnum = item->devnum;
	data->kmsg.portnum = item->portnum;
	for (i = 0; i < 6; i++) {
		data->kmsg.mac[i] = item->mac[i];
	}
	
	/* first check item info is in tunnel_db_s yes or not */
	checkptr = hash_search(tunnel_db_s, data, NULL, 0);
	if (NULL != checkptr) {
		if (!memcmp(&checkptr->kmsg, &data->kmsg, sizeof(tunnel_kernel_msg_t))) {
			npd_syslog_dbg("the same netlink msg which recv from kernel, do nothing \n");
		}
		/* update tunnel */
		else {
			npd_syslog_dbg("portnum or some info changed, update tunnel \n");
			npd_tunnel_release(checkptr, 1, 1);
			goto handle_kmsg;
		}		
	}
	else {
	handle_kmsg:	
		INIT_LIST_HEAD(&(data->list1));
		/* 1 get ts index */
		ret = npd_tunnel_get_ts_tab(&ts);
		if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_err("get ts index fail \n");
			goto errRet;
		}

		/* 2 get tt index*/
		ret = npd_tunnel_get_tt_tab(tt_table, sizeof(unsigned int)*TT_TABLE_LEN, &tt);
		if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_err("tt table is full can not get a index \n");
			goto errRet;
		}
		/* used the same space for rt tcam so +=TUNNEL_TERM_FRIST_NUM*/
		tt += TUNNEL_TERM_FRIST_NUM;
		ret = npd_tunnel_get_nexthop_tab(&np);
		if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_err("TUNNEL_RETURN_CODE_NPFULL_8 \n");
			goto errRet;
		}
		
		data->tsindex = ts;
		data->ttindex = tt;
		data->nhindex = np;
		data->state |= TUNNEL_TS_SW_EN;
		data->state |= TUNNEL_NH_SW_EN;
		data->state |= TUNNEL_TT_SW_EN;

		/* 4 hw*/
		ret = nam_tunnel_start_set_entry(data->kmsg.devnum, ts, TUNNEL_IPV4_OVER_IPV4_E, data);
		if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_dbg("ts set entry fail \n");
			/*npd_tunnel_op_item(data, TUNNEL_DEL_ITEM);*/
			goto errRet;
		}
		else {
			data->state |= TUNNEL_TS_HW_EN;
		}

		ret = nam_tunnel_nh_set(data->kmsg.devnum, data);
		if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_dbg("nh set fail \n");
			/*npd_tunnel_op_item(data, TUNNEL_DEL_ITEM);*/
			goto errRet;
		}
		else {
			data->state |= TUNNEL_NH_HW_EN;
		}
		
		npd_tunnel_get_sys_mac(&sysmac);
		memcpy(data->sysmac, sysmac.arEther, 6);
		ret = nam_tunnel_term_entry_set(data->kmsg.devnum, tt, TUNNEL_IPV4_OVER_IPV4_E, data);
		if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_dbg("tt set entry fail \n");
			/*npd_tunnel_op_item(data, TUNNEL_DEL_ITEM);*/
			goto errRet;
		}
		else {
			data->state |= TUNNEL_TT_HW_EN;
		}
		/* add when route enable*/
		ret = nam_tunnel_ipv4_tunnel_term_port_set(data->kmsg.devnum, data->kmsg.portnum, 1);
		if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_dbg("ipv4 tunnel term port set set  fail \n");
			goto errRet;
		}	
		
		hash_push(tunnel_db_s, data); /* wait for perfect  use this api first */  /*insert into hash table*/	
	}
	
	return TUNNEL_RETURN_CODE_SUCCESS;
	
	errRet:
		if (data->state) {
			npd_tunnel_release(data, 1, 0);
		}
		else {
			if (~0UI != ts) {
				/* release tunnel start table index */
				npd_tunnel_del_ts_tab(ts);
			}
			if (~0UI != tt) {
				npd_tunnel_del_tt_tab(tt_table, sizeof(unsigned int)*TT_TABLE_LEN, (tt - TUNNEL_TERM_FRIST_NUM));
			}
			if (~0UI != np) {
				npd_tunnel_del_nexthop_tab(np);
			}
		}
		free(data);
	return TUNNEL_RETURN_CODE_ERROR;
}

/**********************************************************************************
 * npd_tunnel_handle_rt_msg
 *
 *  	handle the tunnel msg about route info
 *
 *	INPUT:
 *		item:	msg about route
 *		dip:		destination ip
 *	
 *	OUTPUT:
 *		NONE
 *
 * 	RETURN:
 *		TUNNEL_RETURN_CODE_SUCCESS
 *		TUNNEL_RETURN_CODE_ERROR
 *	NOTE:
 *
 **********************************************************************************/
int npd_tunnel_handle_rt_msg
(
	struct tunnel_kernel_msg_s *item,
	unsigned int dip
)
{
	int ret = TUNNEL_RETURN_CODE_SUCCESS;
	struct tunnel_item_s *data = NULL, *tmpdata = NULL;
	struct tunnel_host_s *host = NULL, *hosttmp = NULL;
	struct list_head         *ptr = NULL;

	if (NULL == item) {
		npd_syslog_err("tunnel handle rt msg item is NULL \n");
		return TUNNEL_RETURN_CODE_ERROR;
	}
	tmpdata = malloc(sizeof(struct tunnel_item_s));
	if (NULL == tmpdata) {
		npd_syslog_err("tmpItem malloc fail \n");
		return TUNNEL_RETURN_CODE_ERROR;
	}
	memset(tmpdata, 0, sizeof(struct tunnel_item_s));
	
	host = malloc(sizeof(struct tunnel_host_s));
	if (NULL == host) {
		npd_syslog_err("host malloc fail \n");
		free(tmpdata);
		return TUNNEL_RETURN_CODE_ERROR;
	}
	memset(host, 0, sizeof(struct tunnel_host_s));
	/*test*/
	tmpdata->kmsg.srcip = item->srcip;
	tmpdata->kmsg.dstip = item->dstip;

	data = hash_search(tunnel_db_s, tmpdata, NULL, 0);
	free(tmpdata);
	if(NULL != data) {
		npd_syslog_dbg("nh is %d! \n", data->nhindex);
		__list_for_each(ptr, &(data->list1)) {
			hosttmp = list_entry(ptr, struct tunnel_host_s, list);
			if (hosttmp->hostdip == dip) {
				npd_syslog_dbg("host route exist !! \n");
				free(host);
				return TUNNEL_RETURN_CODE_RT_HOST_EXISTS_9;
			}
		}
		host->hostdip = dip;
		host->hdiplen = 32;
		ret = nam_tunnel_set_tcam_ipltt(data->kmsg.devnum, host->hostdip, host->hdiplen, data);
		if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_err("tunnel set tcam and ipltt fail !\n");
			free(host);
			return TUNNEL_RETURN_CODE_ERROR;
		}
		/*data->state |= TUNNEL_STATES_RT;*/   /*data->hostnum  do the same work*/
		data->hostnum += 1;
		list_add_tail(&(host->list), &(data->list1));
		/*npd_tunnel_update_hashtable(data, tmpItem);*/
	}
	else {
		npd_syslog_dbg("tunnel not exist  !!!\n");
		free(host);
		return TUNNEL_RETURN_CODE_ERROR;
	}
/*
	else {
		INIT_LIST_HEAD(&(tmpItem->list1));
		// add information about tunnel route
//		host->hostdip = item->hostdip;
//		host->hdiplen = item->hdiplen;
		ret = nam_tunnel_set_tcam_ipltt(0, host->hostdip, host->hdiplen, tmpItem);
		if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_err("tunnel set tcam and ipltt fail !\n");
			return TUNNEL_RETURN_CODE_ERROR;
		}
		//data->state |= TUNNEL_STATES_RT;
		data->hostnum += 1;
		list_add_tail(&(host->list), &(tmpItem->list1));
		hash_push(tunnel_db_s, tmpItem);
		npd_syslog_dbg("add new item in tunnel_db_s  for add tunnel route information\n");
		return TUNNEL_RETURN_CODE_SUCCESS;
	}
*/
	return TUNNEL_RETURN_CODE_SUCCESS;

}

/**********************************************************************************
 * npd_tunnel_del_rt_host
 *
 *  	del the tunnel msg about route info
 *
 *	INPUT:
 *		item:	msg about route
 *		dip:		destination ip
 *	
 *	OUTPUT:
 *		NONE
 *
 * 	RETURN:
 *		TUNNEL_RETURN_CODE_SUCCESS
 *		TUNNEL_RETURN_CODE_ERROR
 *	NOTE:
 *
 **********************************************************************************/
int npd_tunnel_del_rt_host
(
	struct tunnel_kernel_msg_s *item,
	unsigned int dip
)
{
	int ret = TUNNEL_RETURN_CODE_SUCCESS;
	struct tunnel_item_s *data = NULL, *tmpdata = NULL;
	struct list_head		 *ptr = NULL;
	struct tunnel_host_s *host = NULL;
	
	if (NULL == item) {
		npd_syslog_err("tunnel del rt msg item is NULL \n");
		return TUNNEL_RETURN_CODE_ERROR;
	}	
	tmpdata = malloc(sizeof(struct tunnel_item_s));
	if (NULL == tmpdata) {
		npd_syslog_err("tmpItem malloc fail \n");
		return TUNNEL_RETURN_CODE_ERROR;
	}
	memset(tmpdata,0,sizeof(struct tunnel_item_s));
	
	tmpdata->kmsg.srcip = item->srcip;
	tmpdata->kmsg.dstip = item->dstip;	
	data = hash_search(tunnel_db_s, tmpdata, NULL, 0);
	free(tmpdata);
	if(NULL != data) {
		__list_for_each(ptr, &(data->list1)) {
			host = list_entry(ptr, struct tunnel_host_s, list);
			if ((NULL != host) &&
				(host->hostdip == dip)) {
				ret = nam_tunnel_del_tcam_ipltt(host->hostdip, host->hdiplen);
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
					npd_syslog_err("tunnel set tcam and ipltt fail !\n");
					return TUNNEL_RETURN_CODE_ERROR;
				}
				data->hostnum -= 1;
				__list_del((host->list).prev, (host->list).next);
			}
			else {
				npd_syslog_dbg("tunnel host del fail host ip is 0x%x \n", host->hostdip);
			}
		}
	}
	else {
		npd_syslog_dbg("tunnel not exist host ip dip sip !! \n");
		return TUNNEL_RETURN_CODE_ERROR;
/* what happen ??*/
	}

	return TUNNEL_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * npd_tunnel_del_tsttnp
 *
 *  	del the tunnel msg about tunnel start add tunnel terminal
 *
 *	INPUT:
 *		item:	msg about tunnel start add tunnel terminal
 *	
 *	OUTPUT:
 *		NONE
 *
 * 	RETURN:
 *		TUNNEL_RETURN_CODE_SUCCESS
 *		TUNNEL_RETURN_CODE_ERROR
 *	NOTE:
 *
 **********************************************************************************/
int npd_tunnel_del_tsttnp
(
	struct tunnel_kernel_msg_s *item
)
{
	int ret = TUNNEL_RETURN_CODE_SUCCESS;
	struct tunnel_item_s *data = NULL, *tmpdata = NULL;
	struct tunnel_host_s *host = NULL;
	struct list_head		 *ptr = NULL;
	
	if (NULL == item) {
		npd_syslog_err("tunnel del ts tt msg item is NULL \n");
		return TUNNEL_RETURN_CODE_ERROR;
	}	
	tmpdata = malloc(sizeof(struct tunnel_item_s));
	if (NULL == tmpdata) {
		npd_syslog_err("tmpItem malloc fail \n");
		return TUNNEL_RETURN_CODE_ERROR;
	}
	memset(tmpdata,0,sizeof(struct tunnel_item_s));
/*	del it in anther func 	
	host = malloc(sizeof(struct tunnel_host_s));
	if (NULL == host) {
		npd_syslog_err("host malloc fail \n");
		free(checkdata);
		return TUNNEL_RETURN_CODE_ERROR;
	}
	memset(host, 0, sizeof(struct tunnel_host_s));
*/
	
	tmpdata->kmsg.dstip = item->dstip;
	tmpdata->kmsg.srcip = item->srcip;

	data = hash_search(tunnel_db_s, tmpdata, NULL, 0);
	free(tmpdata);/*no use later*/
	if (NULL != data) {
		/*frist del host , del tunnel later*/
		if (0 == data->hostnum) {
			ret = nam_tunnel_start_del_entry(0, data->tsindex, 0);
			if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
				npd_syslog_err("tunnel start del fail ! \n");
			}
			/*nam_tunnel_term_entry_invalidate(0, data->ttindex);*/
			ret = nam_tunnel_term_entry_del(0, data->ttindex, 0);
			if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
				npd_syslog_err("tunnel term del fail ! \n");
			}
			ret = nam_tunnel_nh_del(0, data->nhindex);
			if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
				npd_syslog_err("tunnel nh del fail ! \n");
			}
			/* route disable it */
			nam_tunnel_ipv4_tunnel_term_port_set(0, data->kmsg.portnum, 0);
			if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
				npd_syslog_err("tunnel port disable fail ! \n");
			}
			ret = npd_tunnel_del_ts_tab(data->tsindex);
			if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
				npd_syslog_err("tunnel port disable fail ! \n");
			}
			ret = npd_tunnel_del_tt_tab(tt_table, sizeof(unsigned int)*TT_TABLE_LEN, (data->ttindex - TUNNEL_TERM_FRIST_NUM));
			if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
				npd_syslog_err("tunnel port disable fail ! \n");
			}
			ret = npd_tunnel_del_nexthop_tab(data->nhindex);
			if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
				npd_syslog_err("tunnel port disable fail ! \n");
			}

			/*hash_pull(tunnel_db_s, data);*/
			hash_pull(tunnel_db_s, data);
		}
		else {
			npd_syslog_dbg("host ip exist can not remove tunnel information ! \n");
			return TUNNEL_RETURN_CODE_ERROR;
		}
	}
	else {
		npd_syslog_err("not find dstip %dsrcip %d\n");
/* what happen ??*/
	}

	return TUNNEL_RETURN_CODE_SUCCESS;
}

#if 0 /*not use the two func*/
int npd_tunnel_op_item
(
	struct tunnel_item_s *item,
	enum TUNNEL_DB_ACTION action
)
{
	struct tunnel_item_s *data = NULL, *targetdata = NULL;
	int count = 0, ret = TUNNEL_RETURN_CODE_SUCCESS;
	
	if(NULL == item) {
		npd_syslog_err("npd %s tunnel item null pointer error.",(TUNNEL_ADD_ITEM == action) ? "add":"del");
		return TUNNEL_RETURN_CODE_NULLPOINTER_2;
	}
	count = hash_count(tunnel_db_s);
	if((TUNNEL_TABLE_SIZE <= count ) && (TUNNEL_ADD_ITEM == action)) {
		npd_syslog_err("npd %s add arp mac table full\n");
		return TUNNEL_RETURN_CODE_TABLE_FULL_3;
	}
	count = hash_count(tunnel_db_s);
	if((TUNNEL_TABLE_SIZE <= count ) && (TUNNEL_ADD_ITEM == action)) {
		npd_syslog_err("npd %s add arp mac table full\n");
		return TUNNEL_RETURN_CODE_TABLE_FULL_3;
	}
	data = hash_search(tunnel_db_s, item, NULL, 0);

	if (TUNNEL_ADD_ITEM == action) {
		if (NULL != data) {
			npd_syslog_dbg("npd duplicated arp item found when add.");
			/*
			if(NPD_TRUE == data->isStatic) {
				//static arp don't update
				npd_syslog_err("npd duplicated static arp item found when add.");
				memcpy(dupItem, data, sizeof(struct arp_snooping_item_s));
				return -NPD_ARP_SNOOPING_ERR_STATIC_EXIST;
			}
			not need*/
			/*memcpy(dupItem, data, sizeof(struct tunnel_item_s));*/
			ret = npd_tunnel_update_hashtable(data, item);
			return TUNNEL_RETURN_CODE_DUPLICATED_4;
		}

		/*build hash data to insert into hash backet*/
		data = (struct tunnel_item_s *) malloc(sizeof(struct tunnel_item_s));
		if(NULL == data) {
			npd_syslog_err("npd add snooping db alloc memory null!");
			return TUNNEL_RETURN_CODE_NULLPOINTER_2;
		}
		memset(data, 0, sizeof(struct tunnel_item_s));
		memcpy(data, item, sizeof(struct tunnel_item_s));
		hash_push(tunnel_db_s, data); /*insert into hash table*/
	}
	else if (TUNNEL_DEL_ITEM == action) {
		if (NULL == data) {
			npd_syslog_err("npd arp snooping no item found when delete.");
			return TUNNEL_RETURN_CODE_NULLPOINTER_2;
		}

		targetdata = hash_pull(tunnel_db_s, data); /* pull out from hash table*/
		if(targetdata) {
			free(targetdata);	/* release memory*/
			targetdata = NULL;
		}
	}
	
	return TUNNEL_RETURN_CODE_SUCCESS;
}

/* Improve error handling*/ 
int npd_tunnel_handle_tstt_msg_redirect
(
	struct tunnel_item_s *item
)
{
	unsigned int ret = TUNNEL_RETURN_CODE_SUCCESS, ts = 0, tt = 0, np = 0, tstten = 0, rten = 0;
	struct tunnel_item_s *data = NULL, *tmpdata = NULL;
/* first check item info is in tunnel_db_s yes or not*/ 
	tmpdata = hash_search(tunnel_db_s, item, NULL, 0);
	if (NULL != tmpdata) {
		npd_tunnel_release(tmpdata, 1, 0);
		memcpy(data, tmpdata, sizeof(struct tunnel_item_s));
	}
	else {
		data = malloc(sizeof(struct tunnel_item_s));
		if (NULL == data) {
			npd_syslog_err("tmpItem malloc fail \n");
			return TUNNEL_RETURN_CODE_ERROR;
		}
		memset(data, 0, sizeof(struct tunnel_item_s));
		INIT_LIST_HEAD(&(data->list1));
	}
	
	ret = npd_tunnel_get_ts_tab(&ts);
	if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
		npd_syslog_err("TUNNEL_RETURN_CODE_TSFULL_6 \n");
		goto errRet;
	}
	ret = npd_tunnel_get_tt_tab(tt_table, sizeof(unsigned int)*TT_TABLE_LEN, &tt);
	if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
		npd_syslog_err("TUNNEL_RETURN_CODE_TTFULL_7 \n");
		goto errRet;
	}
	tt += TUNNEL_TERM_FRIST_NUM;
	ret = npd_tunnel_get_nexthop_tab(&np);
	if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
		npd_syslog_err("TUNNEL_RETURN_CODE_NPFULL_8 \n");
		goto errRet;
	}
	data->tsindex = ts;
	data->ttindex = tt;
	data->nhindex = np;

	ret = nam_tunnel_start_set_entry(0, ts, 0, data);
	if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
		npd_syslog_dbg("ts set entry fail \n");
		npd_tunnel_op_item(data, TUNNEL_DEL_ITEM);
		goto errRet;
	}
	else {
		/*data->state |= TUNNEL_STATES_TS;*/
	}

	data->istuact= 1; /* add action tunnel interface is tunnel start*/
	data->tunact.istunnelstart = 1;
	data->tunact.tunnelstartidx = ts;
	ret = nam_tunnel_term_entry_set(0, tt, 0, data);
	if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
		npd_syslog_dbg("tt set entry fail \n");
		npd_tunnel_op_item(data, TUNNEL_DEL_ITEM);
		goto errRet;
	}
	else {
		data->state |= TUNNEL_STATES_TT;
	}
	
	hash_push(tunnel_db_s, data); /*insert into hash table*/		
	
	return TUNNEL_RETURN_CODE_SUCCESS;
	
	errRet:
		npd_tunnel_release(data, 1, 0);
	return TUNNEL_RETURN_CODE_ERROR;
}
#endif

/**********************************************************************************
 * npd_tunnel_show_table
 *
 * show tunnel database(Hash table) item detailed info
 *
 *	INPUT:
 *		hash:		tunnel hash item
 *		strbuff:		the buffer of show string
 *		
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL
 *
 **********************************************************************************/
void npd_tunnel_show_table
(
	struct Hash *hash,
	char *strbuff
)
{
	int i = 0,count = 0;
	unsigned char withTitle = 0;
	struct hash_bucket *backet = NULL, *next = NULL;
	struct tunnel_item_s *item = NULL;
	struct tunnel_host_s *host = NULL, *hosttmp = NULL;
	struct list_head         *ptr = NULL;
	char *showStr = NULL,*cursor = NULL;
	int totalLen = 0;

	showStr = (char*)malloc(102400);
	if(NULL == showStr) {
		npd_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(showStr, 0, 102400);	
	cursor = showStr;
	
	for(i = 0; i < hash->size; i++) {
		for(backet = hash->index[i]; backet; backet = next) {
			next = backet->next;
			if(count==0) {
				withTitle = 1;
			}
			else {
				withTitle = 0;
			}
			
			item =  backet->data;

			totalLen += sprintf(cursor, "\n---------------------------------------------------------------\n");
			cursor = showStr + totalLen;
			totalLen += sprintf(cursor, "show one tunnel:\n");
			cursor = showStr + totalLen;
			totalLen += sprintf(cursor, "tunnel termination source IP address is %d.%d.%d.%d \n", 
				(item->kmsg.srcip>>24) & 0xFF, (item->kmsg.srcip>>16) & 0xFF,(item->kmsg.srcip>>8) & 0xFF,item->kmsg.srcip & 0xFF);
			cursor = showStr + totalLen;
			totalLen += sprintf(cursor, "tunnel termination destination IP address is %d.%d.%d.%d \n",
				(item->kmsg.dstip>>24) & 0xFF, (item->kmsg.dstip>>16) & 0xFF,(item->kmsg.dstip>>8) & 0xFF,item->kmsg.dstip & 0xFF);
			cursor = showStr + totalLen;
			totalLen += sprintf(cursor, "tunnel termination vlan id is %d \n", item->kmsg.vid);
			cursor = showStr + totalLen;

			__list_for_each(ptr, &(item->list1)) {
				hosttmp = list_entry(ptr, struct tunnel_host_s, list);
				totalLen += sprintf(cursor, "tunnel start host destination IP address is %d.%d.%d.%d \n",
					(hosttmp->hostdip>>24) & 0xFF, (hosttmp->hostdip>>16) & 0xFF,(hosttmp->hostdip>>8) & 0xFF,hosttmp->hostdip & 0xFF);
				cursor = showStr + totalLen;
			}
			totalLen += sprintf(cursor, "\n---------------------------------------------------------------\n");
			cursor = showStr + totalLen;
			count++;
		}
	}
	if(count != hash->count) {
		npd_syslog_dbg("actual count %d not match with hash count %d\n", count, hash->count);
	}
	
	strncpy(strbuff, showStr, strlen(showStr));
	free(showStr);	
}

int npd_tunnel_update_hashtable
(
	struct tunnel_item_s *oldItem,
	struct tunnel_item_s *newItem
)
{
#if 0
	oldItem ->dstIntf.intf.port.devNum  = newItem ->dstIntf.intf.port.devNum;
	oldItem ->dstIntf.intf.port.portNum  = newItem ->dstIntf.intf.port.portNum;
	oldItem->isStatic = newItem->isStatic;
	oldItem->isTagged = newItem->isTagged;
	oldItem->vid = newItem->vid;
	oldItem->vidx = newItem->vidx;
	memcpy(oldItem->mac,newItem->mac,MAC_ADDRESS_LEN);
#endif
	memcpy(oldItem, newItem, (sizeof(struct tunnel_item_s )));

	return TUNNEL_RETURN_CODE_SUCCESS;
}

int npd_tunnel_get_sys_mac
(
	ETHERADDRS *sysMac
)
{
	ETHERADDRS sysmac;
	int ret = 0;
	memset(&sysmac, 0, sizeof(ETHERADDRS));

	ret = parse_mac_addr(PRODUCT_MAC_ADDRESS, &sysmac);
	if(NPD_SUCCESS != ret)	/* code optimize: Unchecked return value houxx@autelan.com  2013-1-18 */
	{
		npd_syslog_dbg("parse_mac_addr error\n");
		return -1;
	}
	memcpy(sysMac, &sysmac, sizeof(ETHERADDRS));
	npd_syslog_dbg("%-15s:%02x:%02x:%02x:%02x:%02x:%02x\n","system mac",	\
		sysMac->arEther[0], sysMac->arEther[1], sysMac->arEther[2],	\
		sysMac->arEther[3], sysMac->arEther[4], sysMac->arEther[5]);

	return TUNNEL_RETURN_CODE_SUCCESS;
}

int npd_tunnel_get_ts_tab
(
	unsigned int *index
)
{
	unsigned int Index = 0, ret = TUNNEL_RETURN_CODE_SUCCESS;
	unsigned int i = 0, j = 0;
	for (j = 0; j < TS_MAX_TABLE_NUM; j++) { /* when failed   release the Index*/
		ret = nam_arp_mactbl_index_get(Index);
		if (TUNNEL_RETURN_CODE_SUCCESS == ret) {
			/*magic 3 add new index*/
			for (i = 0; i < 3; i++) {
				Index += 1;
				ret = nam_arp_mactbl_index_get(Index);
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
					break;
				}
			}
			if ((3 == i) && (TUNNEL_RETURN_CODE_SUCCESS == ret)) {
				break;
			}
			else {
				while(Index%4) {
					Index -= 1;
					nam_arp_free_mactbl_index(Index);
				}
				Index += 4;
			}
		}
		else {
			Index += 4;
		}
	}
	if ((TS_MAX_TABLE_NUM == j) && (TUNNEL_RETURN_CODE_SUCCESS != ret)) {
		npd_syslog_dbg("there is no ts table index to get \n");
		return TUNNEL_RETURN_CODE_TSFULL_6;
	}
	/*Index = Index /4;*/
	Index = (Index - 3)/4;
	npd_syslog_dbg("get ts index is %d \n", Index);
	*index = Index;
	
	return TUNNEL_RETURN_CODE_SUCCESS;
}

int npd_tunnel_del_ts_tab
(
	unsigned int index
)
{
	unsigned int i = 0, ret = TUNNEL_RETURN_CODE_SUCCESS, arpindex = 0;

	arpindex = index * 4;
	for (i = 0; i < 4; i++) {
		ret = nam_arp_free_mactbl_index(arpindex);
		if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
			npd_syslog_err("free arp mactal index fail index is %d, ret is %d \n", arpindex, ret);
		}
		arpindex += 1;
	}

	return TUNNEL_RETURN_CODE_SUCCESS;
}

/* len is byte of table*/
int npd_tunnel_get_tt_tab
(
	void *table,
	unsigned int len,
	unsigned int *index
)
{
	unsigned int Index = 0,  i = 0, j = 0;
	unsigned char *data = NULL;
	if ((NULL == table) || (!index))	
	{		/* code optimize: Dereference after null check houxx@autelan.com  2013-1-18 */
		return -1;
	}
	data = (unsigned char *)table;
	for (i = 0; i < len; i++) 
	{
#if 0		
		if (~(*(data + i))) 
#else
		if(*(data + i))	/* code optimize: Logical vs. bitwise operator houxx@autelan.com  2013-1-18 */
#endif
		{
			for (j = 0; j < 8; j++)
			{
				if (0 == (*(data + i) & (1<<j))) 
				{
					Index = 8*i + j;
					*(data + i) |= 1<<j;
					npd_syslog_dbg("get tt index is %d \n", Index);
					*index = Index;
					return TUNNEL_RETURN_CODE_SUCCESS;
				}
			}
		}
	}
	
	return TUNNEL_RETURN_CODE_TTFULL_7;
}

int npd_tunnel_del_tt_tab
(
	void *table,
	unsigned int len,
	unsigned int index
)
{
	unsigned int i = 0, j = 0;
	unsigned char *data = NULL;

	data = (unsigned char *)table;
	i = index / 8;
	j = index % 8;

	if (*(data + i) & (1<<j)) {
		*(data + i) &= ~(1<<j);
	}
	else {
		npd_syslog_err("tt table %d empty\n", index);
		return TUNNEL_RETURN_CODE_ERROR;
	}

	return TUNNEL_RETURN_CODE_SUCCESS;
}

int npd_tunnel_get_nexthop_tab
(
	unsigned int *index
)
{
	unsigned int Index = 0, ret = TUNNEL_RETURN_CODE_SUCCESS;
	unsigned int i = 0;

	ret = nam_arp_get_nexthop_tbl_index(&Index);
	if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
		npd_syslog_err("there is no nexthop index to get \n");
		return TUNNEL_RETURN_CODE_NHFULL_8;
	}			

	npd_syslog_dbg("get nh index is %d \n", Index);
	*index = Index;
	
	return TUNNEL_RETURN_CODE_SUCCESS;
}

int npd_tunnel_del_nexthop_tab
(
	unsigned int index
)
{
	unsigned int ret = 0;

	ret = nam_arp_free_nexthop_tbl_index(index);
	if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
		npd_syslog_err("free nexthop index fail index is %d, ret is %d \n", index, ret);
		return TUNNEL_RETURN_CODE_ERROR;
	}

	return TUNNEL_RETURN_CODE_SUCCESS;
}

int npd_tunnel_eth_get_vid
(
	unsigned int eth_g_index,
	unsigned short *vlanid
)
{
	struct eth_port_s *portInfo = NULL;
	struct port_based_vlan_s  *pvid = NULL;
	int ret = TUNNEL_RETURN_CODE_SUCCESS;
	
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		npd_syslog_err("npd eth port %#0x clear pvid port null", eth_g_index);
		return TUNNEL_RETURN_CODE_ERROR;
	}

	pvid = portInfo->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN];
	if(NULL != pvid) {
		*vlanid = pvid->vid;
	}
	else {
		npd_syslog_err("The vlan set NA msg failed \n");
		return TUNNEL_RETURN_CODE_ERROR;
	}

	return TUNNEL_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * npd_tunnel_recv_netlink_msg
 *
 *  	receive kernel netlink msg for tunnel 
 *
 *	INPUT:
 *		NONE
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NONE
 *		
 *	NOTE:
 *
 **********************************************************************************/
void *npd_tunnel_recv_netlink_msg
(
	void
)
{
	struct sockaddr_nl src_addr, dest_addr;
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	int sock_fd = -1, ret = TUNNEL_RETURN_CODE_SUCCESS;
	struct msghdr msg;
	
	/* tell my thread id */
	npd_init_tell_whoami("TunnelNetlink", 0);	
	
	/* Initialize data field */
	memset(&src_addr, 0, sizeof(src_addr));
	memset(&dest_addr, 0, sizeof(dest_addr));
	memset(&iov, 0, sizeof(iov));
	memset(&msg, 0, sizeof(msg));
	/* Create netlink socket use NETLINK_SELINUX(7) */
	if ((sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE)) < 0) {
		npd_syslog_err("create netlink socket for tunnel error\n");
		return TUNNEL_RETURN_CODE_ERROR;
	}

	/* Fill in src_addr */
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid(); /* Thread method */
#ifdef CPU_ARM_XCAT
	src_addr.nl_pid += g_npd_thread_number;
#endif

	src_addr.nl_groups = GRP_ID;
	
	if (bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr)) < 0) {
		npd_syslog_err("bind tunnel socket error\n");
		close(sock_fd);		/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
		return TUNNEL_RETURN_CODE_ERROR;
	}
	
	/* Fill in dest_addr */
	dest_addr.nl_pid = 0; /* From kernel */
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_groups = GRP_ID;

	/* Initialize buffer */
	if((nlh = (struct nlmsghdr*)malloc(NLMSG_SPACE(MAX_PAYLOAD))) == NULL) {
		npd_syslog_err("malloc tunnel socket buffer error\n");
		close(sock_fd);		/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
		return TUNNEL_RETURN_CODE_ERROR;
	}
	
	iov.iov_base = (void *)nlh;
	iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	while (1) {

		memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

		/* Recv message from kernel */	
		ret = recvmsg(sock_fd, &msg, 0);	/* code optimize:Unchecked return value from library  houxx@autelan.com  2013-1-18 */	
		if (ret <= 0) 
		{
			npd_syslog_dbg("npd tunnel recv netlink kernel msg fail \n");
			continue;
		} 

		/*npd_syslog_dbg("Recv message from kernel: %s\n", NLMSG_DATA(nlh));*/
		ret = npd_tunnel_handle_netlink_msg((struct wifi_nl_msg *)NLMSG_DATA(nlh));
		if (0 != ret)
		{
			npd_syslog_dbg("npd tunnel handle netlink kernel msg fail \n");
			/* npd_syslog_dbg("kernel msg is: %s \n", NLMSG_DATA(nlh)); */
		} 
	}

	close(sock_fd);
	free(nlh);
	return TUNNEL_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 * npd_tunnel_handle_netlink_msg
 *
 *  	handle the netlink msg which for kernel
 *
 *	INPUT:
 *		nl_msg:	struct of wifi_nl_msg
 *	
 *	OUTPUT:
 *		NONE
 *
 * 	RETURN:
 *		TUNNEL_RETURN_CODE_SUCCESS
 *		TUNNEL_RETURN_CODE_ERROR
 *		TUNNEL_RETURN_CODE_DSTIP_NOT_EXISTS_10: can not get arp info from dstip
 *	NOTE:
 *
 **********************************************************************************/
int npd_tunnel_handle_netlink_msg
(
	struct wifi_nl_msg * nl_msg
)
{
	unsigned int ret = TUNNEL_RETURN_CODE_SUCCESS, eth_g_index = 0;
	NPD_FDB	fdb;
	struct tunnel_kernel_msg_s *store = NULL;
	struct arp_snooping_item_s *data = NULL;

	memset(&fdb, 0, sizeof(NPD_FDB));	
	
	store=(struct tunnel_kernel_msg_s *)malloc(sizeof(struct tunnel_kernel_msg_s));
	if(NULL == store)  {
		npd_syslog_err("npd tunnel handle netlink msg malloc store fail \n");
		return TUNNEL_RETURN_CODE_ERROR;
	}
	memset(store, 0, sizeof(struct tunnel_kernel_msg_s));	

	switch(nl_msg->op) {
		case IP_ADD:
			/* INNER_IP is 1 EXT_IP is 0 */
			if (EXT_IP == nl_msg->type) {
				store->srcip = nl_msg->u.extMsg.sip;
				store->dstip = nl_msg->u.extMsg.dip;
				
				data = npd_arp_snooping_find_item_byip_only(store->dstip);
				if (NULL != data) {
					/**/
					memcpy(store->mac, data->mac, 6);
					store->vid = data->vid;
					npd_syslog_dbg("TS dstip is 0x%x, srcip is 0x%x \n", store->dstip, store->srcip);
					npd_syslog_dbg("%-15s:%02x:%02x:%02x:%02x:%02x:%02x\n","port mac",	\
									store->mac[0], store->mac[1], store->mac[2],	\
									store->mac[3], store->mac[4], store->mac[5]);
					npd_syslog_dbg("port vid is %d\n", store->vid);
					
					ret = nam_show_fdb_one(&fdb, store->mac, store->vid);
					if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
						npd_syslog_err("use mac add vid get fdb fail !!!\n");
						free(store);
						return TUNNEL_RETURN_CODE_ERROR;
					}
					eth_g_index = fdb.value;
					if ((g_cpu_port_index == eth_g_index) ||
						(g_spi_port_index == eth_g_index)){
						free(store);
						npd_syslog_err("get eth_g_index for fdb is cpu port or spi port, can not used for tunnel \n");
						return TUNNEL_RETURN_CODE_ERROR;
					}
					ret = npd_get_devport_by_global_index(eth_g_index, &(store->devnum), &(store->portnum));
					if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
						npd_syslog_err("npd tunnel handle tstt msg fail !\n");
						free(store);
						return TUNNEL_RETURN_CODE_ERROR;
					}
					npd_syslog_dbg("devnum is %d, portnum is %d \n", store->devnum, store->portnum);
					ret = npd_tunnel_handle_tstt_msg(store);
					if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
						npd_syslog_err("npd tunnel handle tstt msg fail !\n");
						free(store);
						return TUNNEL_RETURN_CODE_ERROR;
					}
				}
				else {
					npd_syslog_dbg("from arp table, check by dstip, data is null \n");
					free(store);
					return TUNNEL_RETURN_CODE_DSTIP_NOT_EXISTS_10;
				}
			}
			else if (INNER_IP == nl_msg->type) {
				store->dstip = nl_msg->u.innerMsg.ext_dip;
				store->srcip = nl_msg->u.innerMsg.ext_sip;
				npd_syslog_dbg("TS dstip is 0x%x, srcip is 0x%x \n", store->dstip, store->srcip);		
				ret = npd_tunnel_handle_rt_msg(store, nl_msg->u.innerMsg.inner_IP);
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
					npd_syslog_dbg("npd tunnel handle rt msg fail !\n");
					free(store);
					return TUNNEL_RETURN_CODE_ERROR;
				}
			}
			else {
				npd_syslog_dbg("bad netlink msg nl_msg->type is %d \n", nl_msg->type);
				free(store);
				return TUNNEL_RETURN_CODE_ERROR;
			}
			break;
		case IP_DEL:
			/* INNER_IP is 1 EXT_IP is 0  and more*/
			if (EXT_IP == nl_msg->type) {
				store->srcip = nl_msg->u.extMsg.sip;
				store->dstip = nl_msg->u.extMsg.dip;

				npd_syslog_dbg("TS dstip is 0x%x, srcip is 0x%x \n", store->dstip, store->srcip);

				ret = npd_tunnel_del_tsttnp(store);
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
					npd_syslog_dbg("npd tunnel del ts tt np fail !\n");
					free(store);
					return TUNNEL_RETURN_CODE_ERROR;
				}
			}
			else if (INNER_IP == nl_msg->type) {
				store->dstip = nl_msg->u.innerMsg.ext_dip;
				store->srcip = nl_msg->u.innerMsg.ext_sip;
				npd_syslog_dbg("TS dstip is 0x%x, srcip is 0x%x \n", store->dstip, store->srcip);		
				ret = npd_tunnel_del_rt_host(store, nl_msg->u.innerMsg.inner_IP);	
				if (TUNNEL_RETURN_CODE_SUCCESS != ret) {		/* code optimize: Logically dead code houxx@autelan.com  2013-1-18 */
					npd_syslog_dbg("npd tunnel del rt host fail !\n");
					free(store);
					return TUNNEL_RETURN_CODE_ERROR;
				}
			}
			else {
				npd_syslog_dbg("bad netlink msg nl_msg->type is %d \n", nl_msg->type);
				free(store);
				return TUNNEL_RETURN_CODE_ERROR;
			}
			break;
		default:
			npd_syslog_dbg("bad netlink msg nl_msg->op is %d \n", nl_msg->op);
			break;
	}

	free(store);
	return TUNNEL_RETURN_CODE_SUCCESS;	
}

DBusMessage * npd_dbus_config_ip_tunnel_add
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	dipaddr = 0, sipaddr = 0;
	unsigned int	sipmaskLen = 0, dipmaskLen = 0, ret = TUNNEL_RETURN_CODE_SUCCESS, eth_g_index = 0;
	unsigned char   mslot = 0, mport = 0;
	struct tunnel_kernel_msg_s *store = NULL;
	struct arp_snooping_item_s *data = NULL;

	if(!((productinfo.capbmp) & FUNCTION_TUNNEL_VLAUE)){
		npd_syslog_dbg("do not support tunnel!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TUNNEL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TUNNEL_VLAUE)){
		npd_syslog_dbg("do not support tunnel!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		store=(struct tunnel_kernel_msg_s *)malloc(sizeof(struct tunnel_kernel_msg_s));
		if(store == NULL)  {
			ret = TUNNEL_RETURN_CODE_ERROR;
		}
		else {
			memset(store, 0, sizeof(struct tunnel_kernel_msg_s));
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32, &sipmaskLen,
										DBUS_TYPE_UINT32, &dipmaskLen,
										DBUS_TYPE_UINT32, &dipaddr,
										DBUS_TYPE_UINT32, &sipaddr,
										DBUS_TYPE_BYTE,  &mslot,
										DBUS_TYPE_BYTE,  &mport,
										DBUS_TYPE_INVALID))) {
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) {
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				free(store);	/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
				store = NULL;
				return NULL;
			}
		
			store->srcip = sipaddr;
			store->dstip = dipaddr;

			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(mslot, mport);
			npd_get_devport_by_global_index(eth_g_index, &(store->devnum), &(store->portnum));
			npd_syslog_dbg("devnum is %d, portnum is %d \n", store->devnum, store->portnum);
			data = npd_arp_snooping_find_item_byip_only(store->dstip);
			if (NULL != data) {
				memcpy(store->mac, data->mac, 6);
				store->vid = data->vid;
				npd_syslog_dbg("TS dstip is 0x%x, srcip is 0x%x \n", store->dstip, store->srcip);
				npd_syslog_dbg("%-15s:%02x:%02x:%02x:%02x:%02x:%02x\n","port mac",	\
					store->mac[0], store->mac[1], store->mac[2],	\
					store->mac[3], store->mac[4], store->mac[5]);
				npd_syslog_dbg("port vid is %d\n", store->vid);
				npd_tunnel_handle_tstt_msg(store);
			}
			else {
				free(store);	/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
				store = NULL;
				ret = 2;
			}
		}	
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32, &ret);
	return reply;	
}

DBusMessage * npd_dbus_config_ip_tunnel_delete
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	dipaddr = 0, sipaddr = 0;
	unsigned int	sipmaskLen = 0, dipmaskLen = 0, ret = TUNNEL_RETURN_CODE_SUCCESS, eth_g_index = 0;
	unsigned char   mslot = 0, mport = 0;
	struct tunnel_kernel_msg_s *store = NULL;
	struct arp_snooping_item_s *data = NULL;

	if(!((productinfo.capbmp) & FUNCTION_TUNNEL_VLAUE)){
		npd_syslog_dbg("do not support tunnel!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TUNNEL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TUNNEL_VLAUE)){
		npd_syslog_dbg("do not support tunnel!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		store=(struct tunnel_kernel_msg_s *)malloc(sizeof(struct tunnel_kernel_msg_s));
		if(store == NULL)  {
			ret = TUNNEL_RETURN_CODE_ERROR;
		}
		else {
			memset(store, 0, sizeof(struct tunnel_kernel_msg_s));
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32, &sipmaskLen,
										DBUS_TYPE_UINT32, &dipmaskLen,
										DBUS_TYPE_UINT32, &dipaddr,
										DBUS_TYPE_UINT32, &sipaddr,
										DBUS_TYPE_BYTE,  &mslot,
										DBUS_TYPE_BYTE,  &mport,
										DBUS_TYPE_INVALID))) {
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) {
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				free(store);	/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
				store = NULL;
				return NULL;
			}
		
			store->srcip = sipaddr;
			store->dstip = dipaddr;

			npd_syslog_dbg("TS dstip is 0x%x, srcip is 0x%x \n", store->dstip, store->srcip);

			ret = npd_tunnel_del_tsttnp(store);
			if (TUNNEL_RETURN_CODE_SUCCESS != ret) {
				npd_syslog_dbg("npd tunnel del ts tt np fail !\n");
			}
			else
			{
				free(store);	/* code optimize: recourse leak zhangcl@autelan.com  2013-1-22 */
				store = NULL;
			}
		
		}	
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32, &ret);
	return reply;	
}

DBusMessage * npd_dbus_config_ip_tunnel_host_add
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ripaddr = 0, lipaddr = 0, dipaddr = 0;
	unsigned int	ripmaskLen = 0, lipmaskLen = 0, dipmaskLen = 0, ret = TUNNEL_RETURN_CODE_SUCCESS;
	struct tunnel_kernel_msg_s *item = NULL; /*warning memery not free */

	if(!((productinfo.capbmp) & FUNCTION_TUNNEL_VLAUE)){
		npd_syslog_dbg("do not support tunnel!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TUNNEL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TUNNEL_VLAUE)){
		npd_syslog_dbg("do not support tunnel!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		item=(struct tunnel_kernel_msg_s *)malloc(sizeof(struct tunnel_kernel_msg_s));
		if(item == NULL)  {
			ret = TUNNEL_RETURN_CODE_ERROR;
		}
		else {
			memset(item, 0, sizeof(struct tunnel_kernel_msg_s));
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32, &ripmaskLen,
										DBUS_TYPE_UINT32, &lipmaskLen,
										DBUS_TYPE_UINT32, &dipmaskLen,
										DBUS_TYPE_UINT32, &ripaddr,
										DBUS_TYPE_UINT32, &lipaddr,							
										DBUS_TYPE_UINT32, &dipaddr,
										DBUS_TYPE_INVALID))) {
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) {
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				free(item);		/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
				item = NULL;
				return NULL;
			}

			item->dstip = ripaddr;
			item->srcip = lipaddr;
			npd_syslog_dbg("TS dstip is 0x%x, srcip is 0x%x \n", item->dstip, item->srcip);		
			npd_tunnel_handle_rt_msg(item, dipaddr);		
		}	

		free(item);
		item = NULL;
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &ret);
	
	return reply;	
}

DBusMessage * npd_dbus_config_ip_tunnel_host_delete
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage*	reply = NULL;
	DBusMessageIter iter;
	DBusError		err;
	unsigned long	ripaddr = 0, lipaddr = 0, dipaddr = 0;
	unsigned int	ripmaskLen = 0, lipmaskLen = 0, dipmaskLen = 0, ret = 0;
	struct tunnel_kernel_msg_s *item = NULL; /*warning memery not free */

	if(!((productinfo.capbmp) & FUNCTION_TUNNEL_VLAUE)){
		npd_syslog_dbg("do not support tunnel!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_TUNNEL_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_TUNNEL_VLAUE)){
		npd_syslog_dbg("do not support tunnel!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		item=(struct tunnel_kernel_msg_s *)malloc(sizeof(struct tunnel_kernel_msg_s));
		if(item == NULL) {
			ret = TUNNEL_RETURN_CODE_ERROR;
		}
		else {
			memset(item, 0, sizeof(struct tunnel_kernel_msg_s));
			dbus_error_init(&err);

			if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32, &ripmaskLen,
										DBUS_TYPE_UINT32, &lipmaskLen,
										DBUS_TYPE_UINT32, &dipmaskLen,
										DBUS_TYPE_UINT32, &ripaddr,
										DBUS_TYPE_UINT32, &lipaddr,							
										DBUS_TYPE_UINT32, &dipaddr,
										DBUS_TYPE_INVALID))) {
				syslog_ax_acl_err("Unable to get input args ");
				if (dbus_error_is_set(&err)) {
					syslog_ax_acl_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
				free(item);		/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
				item = NULL;
				return NULL;
			}

			item->dstip = ripaddr;
			item->srcip = lipaddr;
			npd_syslog_dbg("TS dstip is 0x%x, srcip is 0x%x \n", item->dstip, item->srcip);		
			npd_tunnel_del_rt_host(item, dipaddr);		
		}	

		free(item);
		item = NULL;
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter, DBUS_TYPE_UINT32, &ret);
	
	return reply;	
}

DBusMessage * npd_dbus_ip_tunnel_show_tab(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	DBusMessage* reply;
	DBusMessageIter  iter;
	unsigned int ret = TUNNEL_RETURN_CODE_SUCCESS;
	unsigned int opDevice = 0;
	unsigned int portnum = 0;
	char *showStr = NULL;
				   
	showStr = (char*)malloc(102400);
	if(NULL == showStr) {
		npd_syslog_dbg("Malloc failed!\n");
		return NULL;
	}
	memset(showStr, 0, 102400);

	DBusError err;
	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,									
									DBUS_TYPE_UINT32, &opDevice,
									DBUS_TYPE_UINT32, &portnum,
									DBUS_TYPE_INVALID))) {
		npd_syslog_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				npd_syslog_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
		}
		ret = TUNNEL_RETURN_CODE_ERROR;
	}
	npd_tunnel_show_table(tunnel_db_s, showStr);

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr); 
	
	free(showStr);
	showStr = NULL;
	return reply;
}
#ifdef __cplusplus
}
#endif

