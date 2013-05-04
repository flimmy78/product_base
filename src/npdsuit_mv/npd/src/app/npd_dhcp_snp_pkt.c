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
* npd_dhcp_snp_pkt.c
*
*
* CREATOR:
*		jinpc@autelan.com
*
* DESCRIPTION:
*		dhcp snooping packet for NPD module.
*
* DATE:
*		06/04/2009	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.3 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************
*	head files														*
**********************************************************/
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"

#include "npd_log.h"
#include "npd_dhcp_snp_com.h"
#include "npd_dhcp_snp_options.h"
#include "npd_dhcp_snp_tbl.h"
#include "npd_dhcp_snp_pkt.h"

/*********************************************************
*	global variable define											*
**********************************************************/


/*********************************************************
*	extern variable												*
**********************************************************/


/*********************************************************
*	functions define												*
**********************************************************/

/**********************************************************************************
 *npd_dhcp_snp_get_item_from_pkt()
 *
 *	DESCRIPTION:
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex
 *		NPD_DHCP_MESSAGE_T *packet
 *
 *	OUTPUTS:
 *		NPD_DHCP_SNP_USER_ITEM_T *user
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK
 *		DHCP_SNP_RETURN_CODE_ERROR
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL
 *
 ***********************************************************************************/
unsigned int npd_dhcp_snp_get_item_from_pkt
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *packet,
	NPD_DHCP_SNP_USER_ITEM_T *user
)
{
    unsigned char *temp = NULL;

	syslog_ax_dhcp_snp_dbg("get user item from packet, ifindex %d.\n", ifindex);
    if ((packet == NULL) || (user == NULL)) {
		syslog_ax_dhcp_snp_err("dhcp snp get item from pkt error, parameter is null\n");
		return DHCP_SNP_RETURN_CODE_PARAM_NULL;
    }
    
    user->vlanId    = vlanid;
    user->haddr_len = packet->hlen;

    memcpy(user->chaddr, packet->chaddr, NPD_DHCP_SNP_MAC_ADD_LEN);

    /* 为了支持从RELAY到本地SERVER申请IP地址，同时在本地支持SNOOPING的处理
     * 添加对没有通过用户MAC+VLAN定位到用户接口时从数据包取接口的处理，同时
     * 支持对响应数据包不带接口的处理
     */
    user->ifindex = ifindex;
    if (packet->op == NPD_DHCP_BOOTREPLY)
    {
		user->ifindex = 0;
		user->ip_addr = ntohl(packet->yiaddr);

		temp = (unsigned char *)npd_dhcp_snp_get_option(packet, DHCP_LEASE_TIME);
		if (!temp)
		{
			syslog_ax_dhcp_snp_err("couldn't get lease-time option from packet\n");
			return DHCP_SNP_RETURN_CODE_ERROR;
		}else {
			memcpy(&(user->lease_time), temp, 4);
			user->lease_time = ntohl(user->lease_time);
		}
    }else {
		user->ip_addr    = 0;
		user->lease_time = NPD_DHCP_SNP_REQUEST_TIMEOUT;
    }

	syslog_ax_dhcp_snp_dbg("get user item from packet success.\n");
    return DHCP_SNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *npd_dhcp_snp_discovery_process()
 *
 *	DESCRIPTION:
 *		destroy DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snp_discovery_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;
	NPD_DHCP_SNP_USER_ITEM_T user;
	NPD_DHCP_SNP_TBL_ITEM_T *item = NULL;

	syslog_ax_dhcp_snp_dbg("receive discovery packet fromvlan %d ifindex %d\n", vlanid, ifindex);
    if (dhcp == NULL) {
		syslog_ax_dhcp_snp_err("dhcp snp discovery process error, parameter is null\n");
        return DHCP_SNP_RETURN_CODE_PARAM_NULL;
    }

	memset(&user, 0, sizeof(NPD_DHCP_SNP_USER_ITEM_T));
	ret = npd_dhcp_snp_get_item_from_pkt(vlanid, ifindex, dhcp, &user);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("get item value from packet error, ret %x\n", ret);		
		return ret;
	}

	item = (NPD_DHCP_SNP_TBL_ITEM_T *)npd_dhcp_snp_tbl_item_find(&user);
	if (item == NULL) {
		syslog_ax_dhcp_snp_dbg("no found item from dhcp snooping hash table, then insert a new.\n");
		user.state = NPD_DHCP_SNP_BIND_STATE_REQUEST;
		if (!npd_dhcp_snp_tbl_item_insert(&user))
		{
			syslog_ax_dhcp_snp_err("insert item to table error\n");			
			return DHCP_SNP_RETURN_CODE_ERROR;
		}
	}else {
		syslog_ax_dhcp_snp_dbg("found item from dhcp snooping hash table.\n");
	}

	return DHCP_SNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *npd_dhcp_snp_offer_process()
 *
 *	DESCRIPTION:
 *		destroy DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 ***********************************************************************************/
unsigned int npd_dhcp_snp_offer_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
)
{
	syslog_ax_dhcp_snp_dbg("receive offer packet from vlan %d ifindex %d\n", vlanid, ifindex);
	return DHCP_SNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *npd_dhcp_snp_request_process()
 *
 *	DESCRIPTION:
 *		destroy DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snp_request_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;
	NPD_DHCP_SNP_USER_ITEM_T user;
	NPD_DHCP_SNP_TBL_ITEM_T *item = NULL;

	syslog_ax_dhcp_snp_dbg("receive request packet fromvlan %d ifindex %d\n", vlanid, ifindex);
    if (dhcp == NULL) {
		syslog_ax_dhcp_snp_err("dhcp snp request process error, parameter is null\n");
        return DHCP_SNP_RETURN_CODE_PARAM_NULL;
	}

	memset(&user, 0, sizeof(NPD_DHCP_SNP_USER_ITEM_T));
	ret = npd_dhcp_snp_get_item_from_pkt(vlanid, ifindex, dhcp, &user);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("get item value from packet error, ret %x\n", ret);
		return ret;
	}

	item = (NPD_DHCP_SNP_TBL_ITEM_T *)npd_dhcp_snp_tbl_item_find(&user);
	if (item == NULL) {
		syslog_ax_dhcp_snp_dbg("request: no found item from dhcp snooping hash table, then insert a new.\n");
		user.state = NPD_DHCP_SNP_BIND_STATE_REQUEST;
		if (!npd_dhcp_snp_tbl_item_insert(&user))
		{
			syslog_ax_dhcp_snp_err("insert item to table error\n");
			return DHCP_SNP_RETURN_CODE_ERROR;
		}
	}else {
		syslog_ax_dhcp_snp_dbg("request: found item from dhcp snooping hash table.\n");
	}

	return DHCP_SNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *npd_dhcp_snp_ack_process()
 *
 *	DESCRIPTION:
 *		destroy DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snp_ack_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;
	NPD_DHCP_SNP_USER_ITEM_T user;
	NPD_DHCP_SNP_TBL_ITEM_T *item = NULL;

	syslog_ax_dhcp_snp_dbg("receive ack packet from vlan %d ifindex %d\n", vlanid, ifindex);
    if (dhcp == NULL) {
		syslog_ax_dhcp_snp_err("dhcp snp ack process error, parameter is null\n");
        return DHCP_SNP_RETURN_CODE_PARAM_NULL;
	}

	memset(&user, 0, sizeof(NPD_DHCP_SNP_USER_ITEM_T));
	ret = npd_dhcp_snp_get_item_from_pkt(vlanid, ifindex, dhcp, &user);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("get item value from packet error, ret %x\n", ret);
		return ret;
	}

	item = (NPD_DHCP_SNP_TBL_ITEM_T *)npd_dhcp_snp_tbl_item_find(&user);
	if (item == NULL) {
		return DHCP_SNP_RETURN_CODE_OK; /*from trust to trust interface*/
	}
	
    if (item->bind_type == NPD_DHCP_SNP_BIND_TYPE_STATIC) {
		/* maybe here need add some codes */
        return DHCP_SNP_RETURN_CODE_OK;
    }

	user.state = NPD_DHCP_SNP_BIND_STATE_BOUND;
	ret = npd_dhcp_snp_tbl_refresh_bind(&user, item);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("refresh bind table item value error, ret %x\n", ret);
		return DHCP_SNP_RETURN_CODE_ERROR;
	}

	return DHCP_SNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *npd_dhcp_snp_nack_process()
 *
 *	DESCRIPTION:
 *		destroy DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snoop_nack_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;
	NPD_DHCP_SNP_USER_ITEM_T user;
	NPD_DHCP_SNP_TBL_ITEM_T *item = NULL;

	syslog_ax_dhcp_snp_dbg("receive nack packet from vlan %d ifindex %d\n", vlanid, ifindex);
	if (dhcp == NULL) {
		syslog_ax_dhcp_snp_err("dhcp snp nack process error, parameter is null\n");
		return DHCP_SNP_RETURN_CODE_PARAM_NULL;
	}

	ret = npd_dhcp_snp_get_item_from_pkt(vlanid, ifindex, dhcp, &user);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("get item value from packet error, ret %x\n", ret);
		return ret;
	}

	item = (NPD_DHCP_SNP_TBL_ITEM_T *)npd_dhcp_snp_tbl_item_find(&user);
	if (item == NULL) {
		return DHCP_SNP_RETURN_CODE_OK; /*from trust to trust interface*/
	}	
	
	if (item->bind_type == NPD_DHCP_SNP_BIND_TYPE_STATIC) {
		return DHCP_SNP_RETURN_CODE_OK;
	}

	ret = npd_dhcp_snp_tbl_identity_item(item, &user);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		return DHCP_SNP_RETURN_CODE_OK;
	}

	if (item->bind_type == NPD_DHCP_SNP_BIND_TYPE_DYNAMIC) {
		ret = npd_dhcp_snp_tbl_item_delete(item);
		if (DHCP_SNP_RETURN_CODE_OK != ret) {
			syslog_ax_dhcp_snp_err("delete item from bind table error, ret %x\n", ret);
			return DHCP_SNP_RETURN_CODE_ERROR;
		}
	}

	return DHCP_SNP_RETURN_CODE_OK;
}

/**********************************************************************************
 *npd_dhcp_snp_release_process()
 *
 *	DESCRIPTION:
 *		release DHCP Snooping packet receive
 *
 *	INPUTS:
 *		unsigned short vlanid,
 *		unsigned int ifindex,
 *		NPD_DHCP_MESSAGE_T *dhcp
 *
 *	OUTPUTS:
 *		NULL
 *
 *	RETURN VALUE:
 *		DHCP_SNP_RETURN_CODE_OK			- success
 *		DHCP_SNP_RETURN_CODE_ERROR			- fail
 *		DHCP_SNP_RETURN_CODE_PARAM_NULL	- error, parameter is null
 ***********************************************************************************/
unsigned int npd_dhcp_snp_release_process
(
	unsigned short vlanid,
	unsigned int ifindex,
	NPD_DHCP_MESSAGE_T *dhcp
)
{
	unsigned int ret = DHCP_SNP_RETURN_CODE_OK;
	NPD_DHCP_SNP_USER_ITEM_T user;
	NPD_DHCP_SNP_TBL_ITEM_T *item = NULL;

	syslog_ax_dhcp_snp_dbg("receive release packet from vlan %d ifindex %d\n", vlanid, ifindex);
	if (dhcp == NULL) {
		syslog_ax_dhcp_snp_err("dhcp snp release process error, parameter is null\n");
		return DHCP_SNP_RETURN_CODE_PARAM_NULL;
	}

	ret = npd_dhcp_snp_get_item_from_pkt(vlanid, ifindex, dhcp, &user);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		syslog_ax_dhcp_snp_err("get item value from packet error, ret %x\n", ret);
		return ret;
	}

	item = (NPD_DHCP_SNP_TBL_ITEM_T *)npd_dhcp_snp_tbl_item_find(&user);
	if (item == NULL) {
		return DHCP_SNP_RETURN_CODE_OK; /*from trust to trust interface*/
	}	
	
	if (item->bind_type == NPD_DHCP_SNP_BIND_TYPE_STATIC) {
		return DHCP_SNP_RETURN_CODE_OK;
	}

	ret = npd_dhcp_snp_tbl_identity_item(item, &user);
	if (DHCP_SNP_RETURN_CODE_OK != ret) {
		return DHCP_SNP_RETURN_CODE_OK;
	}

	if (item->bind_type == NPD_DHCP_SNP_BIND_TYPE_DYNAMIC) {
		ret = npd_dhcp_snp_tbl_item_delete(item);
		if (DHCP_SNP_RETURN_CODE_OK != ret) {
			syslog_ax_dhcp_snp_err("delete item from bind table error, ret %x\n", ret);
			return DHCP_SNP_RETURN_CODE_ERROR;
		}
	}

	return DHCP_SNP_RETURN_CODE_OK;
}

unsigned short npd_dhcp_snp_checksum
(
	void *addr,
	int count
)
{
	/* Compute Internet Checksum for "count" bytes
	*         beginning at location "addr".
	*/
	register int sum = 0;
	unsigned short *source = (unsigned short *)addr;

	while (count > 1)  
	{
		/*  This is the inner loop */
		sum += *source++;
		count -= 2;
	}

	/*  Add left-over byte, if any */
	if (count > 0) 
	{
		/* Make sure that the left-over byte is added correctly both
		* with little and big endian hosts */
		unsigned short tmp = 0;
		*(unsigned char *) (&tmp) = * (unsigned char *) source;
		sum += tmp;
	}
	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
	{
		sum = (sum & 0xffff) + (sum >> 16);
	}
	return ~sum;
}


#ifdef __cplusplus
}
#endif




