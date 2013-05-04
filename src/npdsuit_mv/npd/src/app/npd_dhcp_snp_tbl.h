#ifndef __NPD_DHCP_SNP_TBL_H__
#define __NPD_DHCP_SNP_TBL_H__

/*********************************************************
*	head files														*
**********************************************************/
#include "npd_dhcp_snp_com.h"

/*********************************************************
*	macro define													*
**********************************************************/
#define NPD_DHCP_SNP_HASH_TABLE_SIZE    256
#define NPD_DHCP_SNP_MAC_ADD_LEN        6                        /* length of mac address			*/


#define NPD_DHCP_SNP_CHASSIS_SLOT_COUNT	(16)


typedef enum NPD_DHCP_SNP_BIND_STATE_S {
	NPD_DHCP_SNP_BIND_STATE_REQUEST         = 0,
	NPD_DHCP_SNP_BIND_STATE_BOUND           = 1,
} NPD_DHCP_SNP_BIND_STATE_T;

typedef enum NPD_DHCP_SNP_BIND_TYPE_S {
	NPD_DHCP_SNP_BIND_TYPE_DYNAMIC         = 0,
	NPD_DHCP_SNP_BIND_TYPE_STATIC          = 1,
} NPD_DHCP_SNP_BIND_TYPE_T;


/*********************************************************
*	struct define													*
**********************************************************/
typedef struct NPD_DHCP_SNP_USER_ITEM_S
{
	unsigned int  bind_type;
	unsigned char state;
	unsigned char haddr_len;
	unsigned char chaddr[NPD_DHCP_SNP_MAC_ADD_LEN];
	unsigned short vlanId;
	unsigned int ip_addr;
	unsigned int lease_time;
	unsigned int sys_escape; /*添加绑定表项时系统启动以来所过的时间 */
	unsigned int cur_expire;	   /* 当前使用的有效的IP地址状态超时时间,仅显示时使用*/
	unsigned int ifindex;
	unsigned int flags;
}NPD_DHCP_SNP_USER_ITEM_T;

typedef struct NPD_DHCP_SNP_TBL_ITEM_S
{
	struct NPD_DHCP_SNP_TBL_ITEM_S *next;
	unsigned int   bind_type;
	unsigned char  state;
	unsigned char  haddr_len;
	unsigned char  chaddr[NPD_DHCP_SNP_MAC_ADD_LEN];
	unsigned short vlanId;
	unsigned int   ip_addr;
	unsigned int   lease_time;
	unsigned int   sys_escape;
	unsigned int   cur_expire;
	unsigned int   ifindex;
	unsigned int   flags;
}NPD_DHCP_SNP_TBL_ITEM_T;


/*********************************************************
*	function declare												*
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
);

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
);

void npd_dhcp_snp_update_logfile
(
	void
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);

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
);


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
);


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
);

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
);

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
);

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
);

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
);

/*********************************************************
*	extern Functions												*
**********************************************************/

#endif

