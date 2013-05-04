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
* npd_dhcp_snp_tbl.c
*
*
* CREATOR:
*		jinpc@autelan.com
*
* DESCRIPTION:
*		dhcp snooping table for NPD module.
*
* DATE:
*		06/04/2009	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.4 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************
*	head files														*
**********************************************************/
#include <sys/types.h>
#include <time.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <linux/if_ether.h>


#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_log.h"
#include "npd_product.h"
#include "npd_c_slot.h"
#include "npd_eth_port.h"
#include "npd_fdb.h"



#include "npd_dhcp_snp_com.h"
#include "npd_dhcp_snp_sqlite.h"
#include "npd_dhcp_snp_tbl.h"

/*********************************************************
*	global variable define											*
**********************************************************/
NPD_DHCP_SNP_TBL_ITEM_T *g_DHCP_Snp_Hash_Table[NPD_DHCP_SNP_HASH_TABLE_SIZE] = {0};
static sqlite3 *dhcp_snp_db = NULL;

static char *zCreateTbl = "\
						CREATE TABLE dhcpSnpDb(\
						MAC VARCHAR(12) NOT NULL UNIQUE,\
						IP INTEGER NOT NULL UNIQUE,\
						VLAN INTEGER,\
						PORT INTEGER,\
						SYSTIME INTEGER,\
						LEASE INTEGER,\
						BINDTYPE INTEGER,\
						PRIMARY KEY(MAC)\
						);";

static char *zDropTbl = "drop TABLE dhcpSnpDb;";

/*********************************************************
*	extern variable												*
**********************************************************/


/*********************************************************
*	functions define												*
**********************************************************/

/**********************************************************************************
 *npd_dhcp_snp_tbl_initialize ()
 *
 *	DESCRIPTION:
 *		initialize DHCP Snooping bind table
 *
 *	INPUTS:
 *		NULL
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK - success
 *
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_initialize
(
	void
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;

	memset(g_DHCP_Snp_Hash_Table, 0, NPD_DHCP_SNP_HASH_TABLE_SIZE * 4);

	/* open dhcp snooping db */
	ret = npd_dhcp_snp_db_open(&dhcp_snp_db);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("open DHCP-Snooping db error, ret %x", ret);
		return DHCP_SNP_RETURN_CODE_ERROR;
	}

	syslog_ax_dhcp_snp_dbg("open DHCP-Snooping db %s %p success.\n",
							NPD_DHCP_SNP_SQLITE3_DBNAME, dhcp_snp_db);

	/* create dhcp snooping table */
	ret = npd_dhcp_snp_db_create(dhcp_snp_db, zCreateTbl);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("create DHCP-Snooping db error, ret %x", ret);
		return DHCP_SNP_RETURN_CODE_ERROR;
	}

	return DHCP_SNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *npd_dhcp_snp_tbl_hash ()
 *
 *	DESCRIPTION:
 *		get the hash value according the user mac address
 *
 *	INPUTS:
 *		unsigned char *mac		- user mac address
 *
 *	OUTPUTS:
 *		h						- hash key value
 *
 *	RETURN VALUE:
 *
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_hash
(
	unsigned char *mac
)
{
    unsigned int h = NPD_DHCP_SNP_INIT_0;
	unsigned int i = NPD_DHCP_SNP_INIT_0;
	
	for(i = 0; i < 6; i++)
	{
		h = h + mac[i];
	}
	
    h = (h) % NPD_DHCP_SNP_HASH_TABLE_SIZE;

    return h;
}


void npd_dhcp_snp_print_tbl
(
	void
)
{
	unsigned int i = NPD_DHCP_SNP_INIT_0;
	NPD_DHCP_SNP_TBL_ITEM_T* item = NULL;

	syslog_ax_dhcp_snp_dbg("=================dhcp-snooping bind table ===================\n");

	for (i = 0; i < NPD_DHCP_SNP_HASH_TABLE_SIZE; i++)
	{
		item = g_DHCP_Snp_Hash_Table[i];
		while (item != NULL)
		{
			/*scan all the bind table, age the expired items*/
			syslog_ax_dhcp_snp_dbg("MAC:%02x:%02x:%02x:%02x:%02x:%02x ", item->chaddr[0], 
				item->chaddr[1], item->chaddr[2],item->chaddr[3],item->chaddr[4],item->chaddr[5]);
			syslog_ax_dhcp_snp_dbg("ip:%08x ", item->ip_addr);
			syslog_ax_dhcp_snp_dbg("vlan:%04d ", item->vlanId);
			syslog_ax_dhcp_snp_dbg("port:%08d ", item->ifindex);
			syslog_ax_dhcp_snp_dbg("systime:%08d ", item->sys_escape);
			syslog_ax_dhcp_snp_dbg("lease:%08d ", item->lease_time);
			syslog_ax_dhcp_snp_dbg("bind_type:%04d ", item->bind_type);
			syslog_ax_dhcp_snp_dbg("state:%04d ", item->state);
			syslog_ax_dhcp_snp_dbg("cur_expire:%08d ", item->cur_expire);
			syslog_ax_dhcp_snp_dbg("flags:%08d \n", item->flags);
			syslog_ax_dhcp_snp_dbg("-------------------------------------------------------------\n");

			item = item->next;
		}
	}
	syslog_ax_dhcp_snp_dbg("=============================================================\n");


	return ;
}



/**********************************************************************************
 *npd_dhcp_snp_tbl_destroy ()
 *
 *	DESCRIPTION:
 *		release DHCP Snooping bind table momery
 *
 *	INPUTS:
 *		NULL
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK - success
 *
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_destroy
(
	void
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;
	unsigned int i = NPD_DHCP_SNP_INIT_0;
	NPD_DHCP_SNP_TBL_ITEM_T *nextItem = NULL;
	NPD_DHCP_SNP_TBL_ITEM_T *item = NULL;

	for (i = 0; i < NPD_DHCP_SNP_HASH_TABLE_SIZE; i++)
	{
		item = g_DHCP_Snp_Hash_Table[i];
		while (item != NULL)
		{
			nextItem = item->next;
			free(item);
			item = nextItem;
		}
	}

	ret = npd_dhcp_snp_db_drop(dhcp_snp_db, zDropTbl);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("drop dhcp snp table error, ret %x\n", ret);
	}

	npd_dhcp_snp_db_close(dhcp_snp_db);

    return DHCP_SNP_RETURN_CODE_OK;
}


/**********************************************************************************
 *npd_dhcp_snp_tbl_item_find ()
 *
 *	DESCRIPTION:
 *		Get the item of specifical user
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_USER_ITEM_T *user
 *
 *	OUTPUTS:
 *		NPD_DHCP_SNP_TBL_ITEM_T *item
 *
 *	RETURN VALUE:
 *
 ***********************************************************************************/
void *npd_dhcp_snp_tbl_item_find
(
	NPD_DHCP_SNP_USER_ITEM_T *user
)
{
	unsigned int key = NPD_DHCP_SNP_INIT_0;
	unsigned int match = NPD_DHCP_SNP_INIT_0;
	NPD_DHCP_SNP_TBL_ITEM_T *item = NULL;
	NPD_DHCP_SNP_TBL_ITEM_T *tempItem = NULL;

	syslog_ax_dhcp_snp_dbg("find item from dhcp snooping hash table.\n");

	if (user == NULL) {
		syslog_ax_dhcp_snp_err("dhcp snp find item in bind table error, parameter is null\n");
		return NULL;
	}

	key = npd_dhcp_snp_tbl_hash(user->chaddr);
	if (key >= NPD_DHCP_SNP_HASH_TABLE_SIZE) {
		syslog_ax_dhcp_snp_err("error in calculate the hash value\n");
		return NULL;
	} 
	
	tempItem = g_DHCP_Snp_Hash_Table[key];
	while((tempItem != NULL) && (!match)) 
	{
		if (memcmp(tempItem->chaddr, user->chaddr, 6) == 0)
		{
			match = 1;
			item = tempItem;
		}
		tempItem = tempItem->next;
	}

	if (!match)	{
		syslog_ax_dhcp_snp_err("no found the special entry\n");
		return NULL;
	}

	syslog_ax_dhcp_snp_dbg("found item from dhcp snooping hash table, success.\n");

	return item;
}

/**********************************************************************************
 *npd_dhcp_snp_tbl_fill_item ()
 *
 *	DESCRIPTION:
 *		fill the dhcp bind table according user information
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_USER_ITEM_T *user
 *
 *	OUTPUTS:
 *		NPD_DHCP_SNP_TBL_ITEM_T *item
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- fail
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_fill_item
(
	NPD_DHCP_SNP_USER_ITEM_T *user,
	NPD_DHCP_SNP_TBL_ITEM_T *item
)
{
	syslog_ax_dhcp_snp_dbg("fill item in dhcp snooping hash table.\n");

    if((user == NULL) || (item == NULL)) {
		syslog_ax_dhcp_snp_err("dhcp snp fill bind table item error, pointer NULL\n");
        return DHCP_SNP_RETURN_CODE_PARAM_NULL;
    }

    memcpy(item->chaddr, user->chaddr, 6);
    item->haddr_len     = user->haddr_len;
    item->bind_type     = user->bind_type;
    item->state         = user->state;
    item->ifindex       = user->ifindex;
    item->ip_addr       = user->ip_addr;
    item->lease_time    = user->lease_time;
    item->vlanId        = user->vlanId;
    item->sys_escape    = time(0);

	syslog_ax_dhcp_snp_dbg("fill item in dhcp snooping hash table, success.\n");

    return DHCP_SNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *npd_dhcp_snp_tbl_item_insert ()
 *
 *	DESCRIPTION:
 *		insert the user bind information into the bind table
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_USER_ITEM_T *user
 *
 *	OUTPUTS:
 *		NPD_DHCP_SNP_TBL_ITEM_T *item
 *
 *	RETURN VALUE:
 *
 ***********************************************************************************/
void *npd_dhcp_snp_tbl_item_insert
(
	NPD_DHCP_SNP_USER_ITEM_T *user
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;
	unsigned int key = NPD_DHCP_SNP_INIT_0;
	NPD_DHCP_SNP_TBL_ITEM_T *item = NULL;

	syslog_ax_dhcp_snp_dbg("insert item into dhcp snooping hash table.\n");		  

	if (user == NULL) {
		syslog_ax_dhcp_snp_err("dhcp snp insert item in bind table error, parameter is null\n");
		return NULL;
	}

	item = malloc(sizeof(NPD_DHCP_SNP_TBL_ITEM_T));
	if (!item) {
		syslog_ax_dhcp_snp_err("can not malloc the memory\n");			
		return NULL;
	}
	memset(item, 0, sizeof(NPD_DHCP_SNP_TBL_ITEM_T));

	ret = npd_dhcp_snp_tbl_fill_item(user, item);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("dhcp snooping table fill error, ret %x\n", ret);			
		free(item);
		return NULL;
	}

	key = npd_dhcp_snp_tbl_hash(user->chaddr);
	if (key >= NPD_DHCP_SNP_HASH_TABLE_SIZE)	{
		syslog_ax_dhcp_snp_err("error in calculate the hash value\n");
		return NULL;
	}

	item->next = g_DHCP_Snp_Hash_Table[key];
	g_DHCP_Snp_Hash_Table[key] = item;

	/* for debug */
	npd_dhcp_snp_print_tbl();

	/* insert item to db */
/*
	ret = npd_dhcp_snp_db_insert(dhcp_snp_db, item);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("insert dhcp snooping table error, ret %x\n", ret);			
		return NULL;
	}
*/

	syslog_ax_dhcp_snp_dbg("insert item into dhcp snooping hash table, success.\n");		  

    return item;
}

/**********************************************************************************
 *npe_dhcp_snp_tbl_item_delete ()
 *
 *	DESCRIPTION:
 *		delete the user bind item from the bind table
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_USER_ITEM_T *user
 *
 *	OUTPUTS:
 *		NPD_DHCP_SNP_TBL_ITEM_T *item
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, paramter is null
 *	
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_item_delete
(
	NPD_DHCP_SNP_TBL_ITEM_T *item
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;
    unsigned int key = NPD_DHCP_SNP_INIT_0;
    NPD_DHCP_SNP_TBL_ITEM_T *tempItem = NULL;

	syslog_ax_dhcp_snp_dbg("delete item into dhcp snooping hash table, success.\n");

	if (item  == NULL) {
		syslog_ax_dhcp_snp_err("dhcp snp delete bind table item error, parameter is null\n");
		return DHCP_SNP_RETURN_CODE_PARAM_NULL;
    }

	key = npd_dhcp_snp_tbl_hash(item->chaddr);
	if (key >= NPD_DHCP_SNP_HASH_TABLE_SIZE) {
		syslog_ax_dhcp_snp_err("error in calculate the hash value\n");
		return DHCP_SNP_RETURN_CODE_ERROR;
	} 

	if (g_DHCP_Snp_Hash_Table[key] == item)
	{
		g_DHCP_Snp_Hash_Table[key] = item->next;
		ret = npd_dhcp_snp_db_delete(dhcp_snp_db, item);
		if (DHCP_SNP_RETURN_CODE_OK != ret) {
			syslog_ax_dhcp_snp_err("delete dhcp snooping table error, ret %x\n", ret);			
			return DHCP_SNP_RETURN_CODE_ERROR;
		}
		free(item);
		/* for debug */
		npd_dhcp_snp_print_tbl();
		return DHCP_SNP_RETURN_CODE_OK;
	}

	tempItem = g_DHCP_Snp_Hash_Table[key];
	while((tempItem->next != NULL))
	{
		if (tempItem->next == item)
		{
			tempItem->next = item->next;
			ret = npd_dhcp_snp_db_delete(dhcp_snp_db, item);
			if (DHCP_SNP_RETURN_CODE_OK != ret) {
				syslog_ax_dhcp_snp_err("delete dhcp snooping table error, ret %x\n", ret);			
				return DHCP_SNP_RETURN_CODE_ERROR;
			}
			free(item);
			/* for debug */
			npd_dhcp_snp_print_tbl();
			return DHCP_SNP_RETURN_CODE_OK;
		}

		tempItem = tempItem->next;
	}

	syslog_ax_dhcp_snp_warning("no found the special entry, delete fail\n");	
	return DHCP_SNP_RETURN_CODE_ERROR;
}

/**********************************************************************************
 *npd_dhcp_snp_tbl_get_item ()
 *
 *	DESCRIPTION:
 *		 get the dhcp bind table item information
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_USER_ITEM_T *user
 *
 *	OUTPUTS:
 *		NPD_DHCP_SNP_TBL_ITEM_T *item
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, paramter is null
 *	
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_get_item
(
	NPD_DHCP_SNP_USER_ITEM_T *user,
	NPD_DHCP_SNP_TBL_ITEM_T *item
)
{
	syslog_ax_dhcp_snp_dbg("get item into dhcp snooping hash table.\n");		  

    if ((user  == NULL) || (item == NULL)) {
		syslog_ax_dhcp_snp_err("dhcp snp get bind table item error, parameter is null\n");
		return DHCP_SNP_RETURN_CODE_PARAM_NULL;
    }

    memset(user, 0, sizeof(NPD_DHCP_SNP_USER_ITEM_T));

    user->bind_type    = item->bind_type;
    user->state        = item->state;
    user->haddr_len    = item->haddr_len;
    user->vlanId       = item->vlanId;
    user->ip_addr      = item->ip_addr;
    user->lease_time   = item->lease_time;
    user->sys_escape   = item->sys_escape;
    user->cur_expire   = item->cur_expire;
    user->ifindex      = item->ifindex;
    user->flags        = item->flags;    
    memcpy(user->chaddr, item->chaddr, item->haddr_len);

	syslog_ax_dhcp_snp_dbg("get item into dhcp snooping hash table, success.\n");		  

    return DHCP_SNP_RETURN_CODE_OK;
}


/**********************************************************************************
 *npd_dhcp_snp_tbl_fill_bind ()
 *
 *	DESCRIPTION:
 *		fill the bind table according user information
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_USER_ITEM_T *user
 *
 *	OUTPUTS:
 *		NPD_DHCP_SNP_TBL_ITEM_T *item
 *
 *	RETURN VALUE:
 *
 ***********************************************************************************/
void npd_dhcp_snp_tbl_fill_bind
(
	NPD_DHCP_SNP_USER_ITEM_T *user,
	NPD_DHCP_SNP_TBL_ITEM_T *item
)
{
	syslog_ax_dhcp_snp_dbg("fill bind item in dhcp snooping hash table.\n");		  

    if ((user == NULL) || (item  == NULL)) {
		syslog_ax_dhcp_snp_err("dhcp snp fill bind table item error, parameter is null\n");
		return ;
    }
   
    item->state      = user->state;
    item->ip_addr    = user->ip_addr;
    item->lease_time = user->lease_time;
    item->sys_escape = time(0);    

	syslog_ax_dhcp_snp_dbg("fill bind item in dhcp snooping hash table, success.\n");		  

    return ;
}

/**********************************************************************************
 *npd_dhcp_snp_tbl_item_insert ()
 *
 *	DESCRIPTION:
 *		insert the user bind information into the bind table
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_USER_ITEM_T *user
 *		NPD_DHCP_SNP_TBL_ITEM_T *item
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_identity_item
(
	NPD_DHCP_SNP_TBL_ITEM_T *item,
	NPD_DHCP_SNP_USER_ITEM_T *user
)
{
	syslog_ax_dhcp_snp_dbg("identity item in dhcp snooping hash table.\n");		  

    if ((item == NULL) || (user  == NULL)) {
		syslog_ax_dhcp_snp_err("dhcp snp identity bind table item error, parameter is null\n");
		return DHCP_SNP_RETURN_CODE_PARAM_NULL;
    }

    if ((item->vlanId != user->vlanId) ||
		(item->haddr_len != user->haddr_len))
    {
        syslog_ax_dhcp_snp_err("vid is not the same\n");
		return DHCP_SNP_RETURN_CODE_ERROR;
    }
	
	if ((user->ifindex != 0) && (item->ifindex != user->ifindex)) {
    /*if (item->ifindex != user->ifindex) {*/
		syslog_ax_dhcp_snp_err("have different ifindex\n");
		return DHCP_SNP_RETURN_CODE_ERROR;
    }

    if (memcmp(item->chaddr, user->chaddr, item->haddr_len) != 0) {
		syslog_ax_dhcp_snp_err("have different chaddr value\n");        
		return DHCP_SNP_RETURN_CODE_ERROR;
    }

	syslog_ax_dhcp_snp_dbg("identity item in dhcp snooping hash table, success.\n");		  
    return DHCP_SNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *npd_dhcp_snp_tbl_refresh_bind ()
 *
 *	DESCRIPTION:
 *		fill the bind table according user information
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_USER_ITEM_T *user
 *
 *	OUTPUTS:
 *		NPD_DHCP_SNP_TBL_ITEM_T *item
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, paramter is null
 *
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_refresh_bind
(
	NPD_DHCP_SNP_USER_ITEM_T *user,
	NPD_DHCP_SNP_TBL_ITEM_T *item
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;

	syslog_ax_dhcp_snp_dbg("refresh item in dhcp snooping hash table.\n");		  

    if ((item == NULL) || (user  == NULL)) {
		syslog_ax_dhcp_snp_err("dhcp snp refresh item in bind table error, parameter is null\n");
		return DHCP_SNP_RETURN_CODE_PARAM_NULL;
    }

	ret = npd_dhcp_snp_tbl_identity_item(item, user);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("dhcp snp identity item error, ret %x\n", ret);
		return DHCP_SNP_RETURN_CODE_ERROR;
	}
	
	npd_dhcp_snp_tbl_fill_bind(user, item);

	/* for debug */
	npd_dhcp_snp_print_tbl();

	/* first, delete */
	ret = npd_dhcp_snp_db_delete(dhcp_snp_db, item);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("delete dhcp snooping table error, ret %x\n", ret);			
	}

	/* then, insert record to db*/
	ret = npd_dhcp_snp_db_insert(dhcp_snp_db, item);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("dhcp snp insert item error, ret %x\n", ret);
		return DHCP_SNP_RETURN_CODE_ERROR;
	}

	/* update record to db */
/*
	ret = npd_dhcp_snp_db_update(dhcp_snp_db, item);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("dhcp snp update item error, ret %x\n", ret);
		return DHCP_SNP_RETURN_CODE_ERROR;
	}
*/
	syslog_ax_dhcp_snp_dbg("refresh item in dhcp snooping hash table, success.\n"); 	  

	return DHCP_SNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *npd_dhcp_snp_tbl_query ()
 *
 *	DESCRIPTION:
 *		query from dhcp snp db
 *
 *	INPUTS:
 *		char *sql									- sql of insert a item to dhcp snp db
 *	 	unsigned int count,						- count of records
 *		NPD_DHCP_SNP_SHOW_ITEM_T array[]		- array of records
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- sql is null
 *		DHCP_SNP_RETURN_CODE_OK			- exec success
 *		DHCP_SNP_RETURN_CODE_ERROR			- exec fail
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_query
(
	char *sql,
	unsigned int *count,
	NPD_DHCP_SNP_SHOW_ITEM_T array[]
)
{
	return npd_dhcp_snp_db_query(dhcp_snp_db, sql, count, array);
}

/**********************************************************************************
 *npd_dhcp_snp_query_arp_inspection ()
 *
 *	DESCRIPTION:
 *		query from dhcp snp db for arp inspection by sip
 *
 *	INPUTS:
 *		unsigned int dip			- sip
 *		unsigned char *isFound		- found or not found, 1: found 0: not found
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- query success
 *		DHCP_SNP_RETURN_CODE_ERROR			- query fail
 ***********************************************************************************/
unsigned int npd_dhcp_snp_query_arp_inspection
(
	unsigned int sip,
	unsigned char *isFound
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;
	unsigned int record_count = NPD_DHCP_SNP_INIT_0;
	char select_sql[1024] = {0};
	char *bufPtr = NULL;
	int loglength = NPD_DHCP_SNP_INIT_0;
	static NPD_DHCP_SNP_SHOW_ITEM_T show_items[MAX_ETHPORT_PER_BOARD * NPD_DHCP_SNP_CHASSIS_SLOT_COUNT];

	if (!isFound) {
		syslog_ax_dhcp_snp_err("query dhcp snp db for arp proxy error, parameter is null.\n");
		return DHCP_SNP_RETURN_CODE_ERROR;
	}

	memset(show_items, 0, sizeof(show_items));

	/*
	 ****************************
	 *cat query string 
	 ****************************
	 */
	bufPtr = select_sql;

	/*	"Type", "IP Address", "MAC Address", "Lease", "VLAN", "PORT" */
	loglength += sprintf(bufPtr, "SELECT BINDTYPE, IP, MAC, LEASE, VLAN, PORT FROM dhcpSnpDb WHERE IP=");
	bufPtr = select_sql + loglength;

	loglength += sprintf(bufPtr, "%d", sip);
	bufPtr = select_sql + loglength;

	loglength += sprintf(bufPtr, ";");
	bufPtr = select_sql + loglength;

	ret = npd_dhcp_snp_check_global_status();
	if (ret == DHCP_SNP_RETURN_CODE_ENABLE_GBL)
	{
		ret = npd_dhcp_snp_tbl_query(select_sql, &record_count, show_items);
		if (DHCP_SNP_RETURN_CODE_OK != ret) {
			syslog_ax_dhcp_snp_err("get DHCP-Snooping bind table error, ret %x.\n", ret);
			ret = DHCP_SNP_RETURN_CODE_ERROR;
		}
		syslog_ax_dhcp_snp_dbg("query %d records from dhcpSnpDb success.\n", record_count);

		if (record_count > 0) {
			*isFound = 1;/* found */
			ret = DHCP_SNP_RETURN_CODE_OK;
		}
		else {
			*isFound = 0;/* not found */
			ret = DHCP_SNP_RETURN_CODE_OK;
		}
	}
	else {	
		syslog_ax_dhcp_snp_dbg("check DHCP-Snooping global status not enabled global.\n");
		ret = DHCP_SNP_RETURN_CODE_ERROR;
	}

	return ret;
}

/**********************************************************************************
 *npd_dhcp_snp_query_arp_proxy ()
 *
 *	DESCRIPTION:
 *		query from dhcp snp db for arp proxy by dip
 *
 *	INPUTS:
 *		unsigned int dip						- dip
 *		NPD_DHCP_SNP_SHOW_ITEM_T *result	- result of query
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- query success
 *		DHCP_SNP_RETURN_CODE_ERROR			- query fail
 ***********************************************************************************/
unsigned int npd_dhcp_snp_query_arp_proxy
(
	unsigned int dip,
	NPD_DHCP_SNP_SHOW_ITEM_T *result
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;
	unsigned int record_count = NPD_DHCP_SNP_INIT_0;
	char select_sql[1024] = {0};
	char *bufPtr = NULL;
	int loglength = NPD_DHCP_SNP_INIT_0;

	static NPD_DHCP_SNP_SHOW_ITEM_T show_items[MAX_ETHPORT_PER_BOARD * NPD_DHCP_SNP_CHASSIS_SLOT_COUNT];

	if (!result) {
		syslog_ax_dhcp_snp_err("query dhcp snp db for arp proxy error, parameter is null.\n");
		return DHCP_SNP_RETURN_CODE_ERROR;
	}

	memset(show_items, 0, sizeof(show_items));

	/*
	 ****************************
	 *cat query string 
	 ****************************
	 */
	bufPtr = select_sql;

	/*	"Type", "IP Address", "MAC Address", "Lease", "VLAN", "PORT" */
	loglength += sprintf(bufPtr, "SELECT BINDTYPE, IP, MAC, LEASE, VLAN, PORT FROM dhcpSnpDb WHERE IP=");
	bufPtr = select_sql + loglength;

	loglength += sprintf(bufPtr, "%d", dip);
	bufPtr = select_sql + loglength;

	loglength += sprintf(bufPtr, ";");
	bufPtr = select_sql + loglength;

	ret = npd_dhcp_snp_check_global_status();
	if (ret == DHCP_SNP_RETURN_CODE_ENABLE_GBL)
	{
		ret = npd_dhcp_snp_tbl_query(select_sql, &record_count, show_items);
		if (DHCP_SNP_RETURN_CODE_OK != ret) {
			syslog_ax_dhcp_snp_err("get DHCP-Snooping bind table error, ret %x.\n", ret);
			ret = DHCP_SNP_RETURN_CODE_ERROR;
		}
		syslog_ax_dhcp_snp_dbg("query %d records from dhcpSnpDb success.\n", record_count);

		if (record_count >= 1) {
			/* found */
			result->bind_type  = show_items[0].bind_type;
			memcpy(result->chaddr, show_items[0].chaddr, 12);
			result->ip_addr    = show_items[0].ip_addr;
			result->ifindex    = show_items[0].ifindex;
			result->vlanId     = show_items[0].vlanId;
			result->lease_time = show_items[0].lease_time;

			ret = DHCP_SNP_RETURN_CODE_OK;
		}
		else {
			/* not found */
			ret = DHCP_SNP_RETURN_CODE_ERROR;
		}
	}
	else {	
		syslog_ax_dhcp_snp_dbg("check DHCP-Snooping global status not enabled global.\n");
		ret = DHCP_SNP_RETURN_CODE_ERROR;
	}

	return ret;
}

/**********************************************************************************
 *npd_dhcp_snp_tbl_static_binding_insert()
 *
 *	DESCRIPTION:
 *		insert a static binding item to dhcp snp db
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_TBL_ITEM_T *item 	- item which need insert into dhcp snp db
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- sql is null
 *		DHCP_SNP_RETURN_CODE_OK			- exec success
 *		DHCP_SNP_RETURN_CODE_ERROR		- exec fail
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_static_binding_insert
(
	NPD_DHCP_SNP_TBL_ITEM_T item
)
{
	return npd_dhcp_snp_db_insert(dhcp_snp_db, &item);
}

/**********************************************************************************
 *npd_dhcp_snp_tbl_static_binding_delete()
 *
 *	DESCRIPTION:
 *		delete a static binding item from dhcp snp db
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_TBL_ITEM_T *item 	- item which need delete from dhcp snp db
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- sql is null
 *		DHCP_SNP_RETURN_CODE_OK			- exec success
 *		DHCP_SNP_RETURN_CODE_ERROR		- exec fail
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_static_binding_delete
(
	NPD_DHCP_SNP_TBL_ITEM_T item
)
{
	return npd_dhcp_snp_db_delete(dhcp_snp_db, &item);
}

/**********************************************************************************
 *npd_dhcp_snp_tbl_binding_insert()
 *
 *	DESCRIPTION:
 *		insert a binding item to dhcp snp db
 *		inner interface in dhcp snooping, for save content of bindint table
 *		by command "write" before reboot
 *
 *	INPUTS:
 *		NPD_DHCP_SNP_TBL_ITEM_T *item 	- item which need insert into dhcp snp db
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- sql is null
 *		DHCP_SNP_RETURN_CODE_OK			- exec success
 *		DHCP_SNP_RETURN_CODE_ERROR		- exec fail
 ***********************************************************************************/
unsigned int npd_dhcp_snp_tbl_binding_insert
(
	NPD_DHCP_SNP_TBL_ITEM_T item
)
{
	return npd_dhcp_snp_db_insert(dhcp_snp_db, &item);
}

#ifdef __cplusplus
}
#endif

