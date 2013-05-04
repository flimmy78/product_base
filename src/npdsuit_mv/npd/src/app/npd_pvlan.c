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
* npd_pvlan.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		API used in NPD for PVLAN module.
*
* DATE:
*		05/12/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.21 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#include "npd_pvlan.h"
#include "sysdef/returncode.h"


static unsigned int 	pport  = 0;
static pvenode  		arryport[PVE_MAX_PORT];



int npd_pvlan_config_pve_info
(
	unsigned int eth_g_index,
	unsigned char slot,
	unsigned char port,
	enum uplink_type_s type
)
{
	struct eth_port_s * portInfo = NULL;
	struct pve_info_s * pveInfo = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_pvlan_err("this %d port not exist \n",eth_g_index);
		return PVLAN_RETURN_CODE_ERROR;
	}

	pveInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_PVE];
	if(NULL == pveInfo) {
		pveInfo = (struct pve_info_s*)malloc(sizeof(struct pve_info_s));
		if(NULL == pveInfo) {
			syslog_ax_pvlan_err("malloc pve_info_s error\n");
			return PVLAN_RETURN_CODE_ERROR;
		}
		memset(pveInfo,0,sizeof(struct pve_info_s));
	}


	pveInfo->type = type;
	pveInfo->up.nocscd_port.slot_no = slot;
	pveInfo->up.nocscd_port.port_no = port;

	portInfo->funcs.func_data[ETH_PORT_FUNC_PVE] = pveInfo;
	portInfo->funcs.funcs_run_bitmap &= (1<<ETH_PORT_FUNC_PVE);

	return PVLAN_RETURN_CODE_SUCCESS;
	
}


int npd_pvlan_delete_pve
(
	unsigned int eth_g_index
)
{
	struct eth_port_s * portInfo = NULL;
	struct pve_info_s * pveInfo = NULL;

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		syslog_ax_pvlan_err("this %d port not exist \n",eth_g_index);
		return PVLAN_RETURN_CODE_ERROR;
	}

	pveInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_PVE];
	if(NULL == pveInfo) {
		syslog_ax_pvlan_dbg("pve not exsit %d \n",eth_g_index);
		return PVLAN_RETURN_CODE_NO_PVE;
	}
	else {
		free(pveInfo);
		portInfo->funcs.func_data[ETH_PORT_FUNC_PVE] = NULL;
	}

	portInfo->funcs.funcs_run_bitmap &= (1<<ETH_PORT_FUNC_PVE);
	return PVLAN_RETURN_CODE_SUCCESS;
}

int npd_pvlan_check_port_ispvlan
(
	unsigned char slot,
	unsigned char port
)
{
	int ret = NPD_FALSE;
	int index = 0;

	for(index = 1; index < PVE_MAX_PORT; index++) {
		if(PVE_TRUE == arryport[index].fag) {
			if(((arryport[index].lkslot == slot) && (arryport[index].lkport == port)) ||	\
				((arryport[index].pvslot == slot) && (arryport[index].pvport == port)))
				return PVE_TRUE;
		}
	}

	return ret;
}

int npd_pvlan_check_cycle
(
	unsigned char src_slot,
	unsigned char src_port
)
{
	int ret = NPD_FALSE;
	int index = 0;

	for(index = 1; index < PVE_MAX_PORT; index++) {
		if(PVE_TRUE == arryport[index].fag) {
			if((arryport[index].lkslot == src_slot) && (arryport[index].lkport == src_port))
				return PVE_TRUE;
		}
	}

	return ret;
}

int npd_pvlan_add_node
(
	unsigned char slot1,
	unsigned char port1,
	unsigned char slot2,
	unsigned char port2,
	unsigned int mode
)
{
	
	unsigned int 	ret 	=1;
	unsigned int 	index	=0;
	index = (slot1-1)*6+(port1-1);
	unsigned int destIndex = (slot2 -1)*6 + (port2-1);
	
	if(arryport[index].fag == PVE_TRUE) {
		return PVLAN_RETURN_CODE_THIS_PORT_HAVE_PVE;
	}
	else if(arryport[index].fag != PVE_TRUE){
		if(port2==0){
				arryport[index].tkfag = PVE_TRUE;
			}
			else {
				arryport[index].tkfag = PVE_FAIL;
			}
			arryport[index].fag = PVE_TRUE;
			arryport[index].pvslot=slot1;
			arryport[index].pvport=port1;
			arryport[index].lkslot=slot2;
			arryport[index].lkport=port2;
			arryport[index].mode = mode;
			pport++;
			ret=0;
	}
	return ret;
}

int npd_pvlan_delete_node
(
	unsigned char slot1,
	unsigned char port1
)
{
	unsigned int 	index	=	0;
	unsigned int 	ret		=	1;
	index = (slot1-1)*6+(port1-1);
	if(arryport[index].fag == 1){
			arryport[index].fag 	= 	PVE_FAIL;
			arryport[index].tkfag 	= 	PVE_FAIL;
			arryport[index].pvslot	=	0;
			arryport[index].pvport	=	0;
			arryport[index].lkslot	=	0;
			arryport[index].lkport	=	0;
			arryport[index].mode = PVE_MODE_SINGLE_E;
			pport--;
			ret=0;
	}
	return ret;	
}

#if 0
struct eth_port_pve_s* npd_pvlan_find_pve_by_index
(
	unsigned int pvePort
)
{
	unsigned int 	ret = 0;
	struct eth_port_s* portInfo = NULL;
	struct eth_port_pve_s* pveInfo = NULL;

	portInfo = npd_get_port_by_index(pvePort);
	if(!portInfo) {
		return portInfo->funcs.func_data[ETH_PORT_FUNC_PVE];
	}
	else 
		return NULL;
}

int npd_pvlan_add_node
(
	unsigned int pvePort,
	unsigned int destPort ,
	unsigned int trunkId,
	unsigned char isTrunk
)
{
	
	unsigned int 	ret = 0;
	struct eth_port_s* portInfo = NULL;
	struct eth_port_pve_s* pveInfo = NULL;

	if(NULL != npd_pvlan_find_pve_by_index(destPort)) {
		return NPD_PVE_AVOID_CYCLE_UPLINK;
	}
	
	portInfo = npd_get_port_by_index(pvePort);
	if(!portInfo) {
		pveInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_PVE];	
	}
	else
		return PVLAN_RETURN_CODE_ERROR;

	if(NULL == pveInfo) {
		pveInfo = (struct eth_port_pve_s*)malloc(sizeof(struct eth_port_pve_s));
		if(NULL == pveInfo)
			return PVLAN_RETURN_CODE_ERROR;
		else {
			memset(pveInfo,0,sizeof(struct eth_port_pve_s));
		}
	}

	if(NPD_TRUE == isTrunk) {
		pveInfo->trg.isTrunk = NPD_TRUE;
		pveInfo->trg.dest.trunkId = trunkId;
	}
	else {
		pveInfo->trg.isTrunk = NPD_FALSE;
		pveInfo->trg.dest.trunkId = destPort;
	}
	
	return ret;
}

int npd_pvlan_delete_node
(
	unsigned int pvePort
)
{
	unsigned int 	ret = 0;
	struct eth_port_s* portInfo = NULL;
	struct eth_port_pve_s* pveInfo = NULL;

	
	portInfo = npd_get_port_by_index(pvePort);
	if(!portInfo) {
		pveInfo = portInfo->funcs.func_data[ETH_PORT_FUNC_PVE];	
	}
	else
		return PVLAN_RETURN_CODE_ERROR;

	if(NULL == pveInfo) {
		return NPD_PVE_ERROR_NOT_EXSIT;
	}

	free(pveInfo);
	portInfo->funcs.func_data[ETH_PORT_FUNC_PVE] = NULL;
	
	return ret;
}
#endif

int npd_pvlan_get_port_number(void){
	return pport;
}

pvenode* npd_pvlan_get_first_node(void)
{
	
	if(pport==0){
		return NULL;
	}
	else{
		return arryport;
	}
}

int npd_pvlan_change_array_fashion(pvenode * arry1, pvenode * arry2)
{
	unsigned int i,num=0;
	for(i=0;i<PVE_MAX_PORT;i++)
		{
			if(arry1[i].fag == 0){
				i++;
				continue;
				}
			else {
				arry2[num]=arry1[i];
				num++;
				}
		}
	return num;
}

/*******************************************************************************
 * npd_pvlan_check_ports
 *
 * DESCRIPTION:
 *   	check all port
 *
 * INPUTS:
 * 	NONE
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	TRUE   	- on success
 * 	FALSE	- otherwise
 *
 *
 ********************************************************************************/
int npd_pvlan_check_ports
(
	unsigned int src_eth_g_index,
	unsigned int dest_eth_g_index,
	unsigned int mode
)
{
	int ret = NPD_FALSE,result = NPD_FALSE;
	unsigned short vid = 1;
	unsigned int isTag = NPD_FALSE;
	unsigned int src = NPD_FALSE,dest = NPD_FALSE;
	unsigned int count = 0;

	/*single mode, pve port must only take part in one vlan*/
	if(PVE_MODE_SINGLE_E == mode) {
		for(vid = 1; vid < NPD_MAX_VLAN_ID; vid++) {
			src = npd_vlan_check_contain_port(vid,src_eth_g_index,&isTag);
			if(NPD_TRUE == src) {
				count++;
				dest = npd_vlan_check_contain_port(vid,dest_eth_g_index,&isTag);
				if((dest == src)) {
					result = NPD_TRUE;
				}
				else {
					result = NPD_FALSE;
					break;
				}
			}
		}
		
		if((NPD_TRUE == result) && (1 == count)) {	
			ret = NPD_TRUE;
		}	
	}
	else if(PVE_MODE_MULTI_E == mode) {  /*multiple mode, pve port and uplink port must take part in all same vlan*/
		for(vid = 1; vid < 4095; vid++) {
			src = npd_vlan_check_contain_port(vid,src_eth_g_index,&isTag);
			dest = npd_vlan_check_contain_port(vid,dest_eth_g_index,&isTag);
			if(src != dest) {	
				result = NPD_FALSE;
				break;
			}
		}

		if(NPD_MAX_VLAN_ID == vid) 
			ret = NPD_TRUE;
	}
	else  {
		syslog_ax_pvlan_err("input pvlan mode  %d ,error",mode);
		ret = NPD_FALSE;
	}
	
	return ret;	
}


DBusMessage * npd_dbus_pvlan_create(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	
	unsigned int	ret = PVLAN_RETURN_CODE_ERROR;
	unsigned int	pvlanid=0;
	
	syslog_ax_pvlan_dbg("Entering config system info!\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
										DBUS_TYPE_UINT32,&pvlanid,
										DBUS_TYPE_INVALID))) {
		syslog_ax_pvlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_pvlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
	}
	
    else{
	
		ret = nam_pvlan_create(pvlanid) ;
		if (ret != 0) {
			 syslog_ax_pvlan_dbg(" nam_pvlan_create ERROR. \n");
				 
			}
		
    }
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	dbus_message_iter_init_append (reply, &iter);
	
	return reply;
	
}

DBusMessage * npd_dbus_pvlan_add_port(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	slot1,port1;
	unsigned int	ret = PVLAN_RETURN_CODE_ERROR;
	unsigned int	eth_g_index1=0;
	
	syslog_ax_pvlan_dbg("Entering config system info!\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_BYTE,&slot1,
											DBUS_TYPE_BYTE,&port1,
											DBUS_TYPE_INVALID))) {
		syslog_ax_pvlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_pvlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
	}
	
    else{
		if (CHASSIS_SLOTNO_ISLEGAL(slot1)) {
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot1,port1)) {
					eth_g_index1 = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot1,port1);
					 syslog_ax_pvlan_dbg("index for %d/%d is %#0x\n",slot1,port1,eth_g_index1);
					ret = PVLAN_RETURN_CODE_SUCCESS;
				}
			}
		ret = nam_pvlan_add_port(eth_g_index1) ;
		if (ret != 0) {
			 syslog_ax_pvlan_dbg(" nam_pvlan_add_port %d\n",ret);
				 
			}
		
    }
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	dbus_message_iter_init_append (reply, &iter);
	
	return reply;
	
}

DBusMessage * npd_dbus_pvlan_delete_port(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	slot1,port1;
	unsigned int	ret = PVLAN_RETURN_CODE_ERROR;
	unsigned int	eth_g_index1=0;
	
	syslog_ax_pvlan_dbg("Entering config system info!\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_BYTE,&slot1,
											DBUS_TYPE_BYTE,&port1,
											DBUS_TYPE_INVALID))) {
		syslog_ax_pvlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_pvlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
	}
	
    else{
		if (CHASSIS_SLOTNO_ISLEGAL(slot1)) {
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot1,port1)) {
					eth_g_index1 = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot1,port1);
					 syslog_ax_pvlan_dbg("index for %d/%d is %#0x\n",slot1,port1,eth_g_index1);
					ret = PVLAN_RETURN_CODE_SUCCESS;
				}
			}
		ret = nam_pvlan_delete_port(eth_g_index1) ;
		if (ret != 0) {
			 syslog_ax_pvlan_dbg(" nam_pvlan_delete_port %d\n",ret);
				 
			}
		
    }
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	dbus_message_iter_init_append (reply, &iter);
	
	return reply;
	
}

DBusMessage * npd_dbus_pvlan_cpu_port(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* 		reply;
	DBusMessageIter	 	iter;
	DBusError 			err;
	unsigned int		flag=0;
	unsigned int		ret = PVLAN_RETURN_CODE_ERROR;
	
	syslog_ax_pvlan_dbg("Entering config system info!\n");
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
											DBUS_TYPE_UINT32,&flag,
											DBUS_TYPE_INVALID))) {
		syslog_ax_pvlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
				syslog_ax_pvlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
	}
	
    else{
		
		ret = nam_pvlan_cpu_port(flag) ;
		if (ret != 0) {
			 syslog_ax_pvlan_dbg(" nam_pvlan_delete_port %d\n",ret);
				 
			}
		
    }
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	
	return reply;
	
}

/*config port uplink*/
DBusMessage * npd_dbus_pvlan_config_port(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	src_slot=0;
	unsigned char	src_port=0;
	unsigned char	dest_slot = 0;
	unsigned char	dest_port = 0;
	unsigned int	ret = PVLAN_RETURN_CODE_ERROR;
	unsigned int	src_eth_g_index = 0,dest_eth_g_index = 0;
	unsigned short vid = 2;
	unsigned int isTag = 0;
	unsigned int src = NPD_FALSE, dest = NPD_FALSE,result = NPD_TRUE;
	unsigned int mode = PVE_MODE_SINGLE_E;
	
	syslog_ax_pvlan_dbg("Entering config system info!\n");

	if(!((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support pvlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support qos!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
												DBUS_TYPE_BYTE,&src_slot,
												DBUS_TYPE_BYTE,&src_port,
												DBUS_TYPE_BYTE,&dest_slot,
												DBUS_TYPE_BYTE,&dest_port,
												DBUS_TYPE_UINT32,&mode,
												DBUS_TYPE_INVALID))) {
			syslog_ax_pvlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					syslog_ax_pvlan_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
		}


		syslog_ax_pvlan_dbg("src_slot/src_port %d/%d  dest_slot/dest_port %d/%d\n",src_slot,src_port,dest_slot,dest_port);
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		if ((CHASSIS_SLOTNO_ISLEGAL(src_slot))&&(CHASSIS_SLOTNO_ISLEGAL(dest_slot)))  {
			if ((ETH_LOCAL_PORTNO_ISLEGAL(src_slot,src_port)) && (ETH_LOCAL_PORTNO_ISLEGAL(dest_slot,dest_port))) {
				src_eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(src_slot,src_port);
				/*printf("index for %d/%d is %#0x\n",slot1,port1,eth_g_index1);*/
				dest_eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(dest_slot,dest_port);
				/*check srcPort whether int default vlan*/
				result = npd_pvlan_check_ports(src_eth_g_index, dest_eth_g_index, mode);
				ret  = PVLAN_RETURN_CODE_SUCCESS;
			}
		}

		/* create uplink mode*/				
		if((PVLAN_RETURN_CODE_SUCCESS == ret) && (NPD_TRUE == result)) {
			 if(PVE_TRUE == npd_pvlan_check_cycle(src_slot,src_port)) {
				syslog_ax_pvlan_dbg("have create uplink on the port dest %d/%d\n",src_slot, src_port);
				ret = PVLAN_RETURN_CODE_AVOID_CYCLE_UPLINK;
			}
			else if(0 != (ret = npd_pvlan_add_node(src_slot, src_port, dest_slot, dest_port,mode))) {
				syslog_ax_pvlan_dbg("have create pvlan on the ports src %d/%d, dest %d/%d\n",src_slot, src_port, dest_slot, dest_port);
			}
			else {
				syslog_ax_pvlan_dbg("index for %d/%d is %#0x\n",dest_slot, dest_port,dest_eth_g_index);				
				ret = nam_pvlan_port_config(src_eth_g_index, dest_eth_g_index ) ;
				if (ret != 0) {
					npd_pvlan_delete_pve(src_eth_g_index);
					syslog_ax_pvlan_dbg(" nam_pvlan_port_config ERROR. \n");
					 
				}					
				ret=PVLAN_RETURN_CODE_SUCCESS;		
			}
		}
		else if((NPD_FALSE == result) && (PVE_MODE_SINGLE_E == mode)) {
			ret = PVLAN_RETURN_CODE_PORT_NOT_IN_ONLY_ONE_VLAN;
		}
		else if((NPD_FALSE == result) && (PVE_MODE_MULTI_E == mode)) {
			ret = PVLAN_RETURN_CODE_UPLINK_PORT_NOT_IN_SAME_VLAN;
		}
	}
 
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	
	
	return reply;
	
}

/*send all packets to cpu*/
DBusMessage * npd_dbus_pvlan_config_spi_port(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	slot1=0;
	unsigned char	port1=0;
	unsigned char	slot2=0;
	unsigned char	port2=0;
	unsigned int	ret = PVLAN_RETURN_CODE_ERROR;
	unsigned int	eth_g_index1;
	unsigned int mode = 0;
	
	syslog_ax_pvlan_dbg("Entering config system info!\n");

	if(!((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support pvlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support pvlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
												DBUS_TYPE_BYTE,&slot1,
												DBUS_TYPE_BYTE,&port1,
												DBUS_TYPE_BYTE,&slot2,
												DBUS_TYPE_BYTE,&port2,
												DBUS_TYPE_INVALID))) {
			syslog_ax_pvlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					syslog_ax_pvlan_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
		}
		
	    else{
			if ((CHASSIS_SLOTNO_ISLEGAL(slot1))&&(CHASSIS_SLOTNO_ISLEGAL(slot2))) 
				{
				if(PVLAN_RETURN_CODE_ERROR==npd_pvlan_add_node( slot1,port1,slot2,port2,mode)){
					
						ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
				}
				
				else{
						if (ETH_LOCAL_PORTNO_ISLEGAL(slot1,port1)) 
							{
							eth_g_index1 = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot1,port1);
							 syslog_ax_pvlan_dbg("index for %d/%d is %#0x\n",slot1,port1,eth_g_index1);
							
							ret = nam_pvlan_port_config_spi(eth_g_index1, slot2,port2) ;
							if (ret != 0) {
								 syslog_ax_pvlan_dbg(" nam_pvlan_port_config_spi ERROR. \n");
								 
							}
							ret=PVLAN_RETURN_CODE_SUCCESS;
						}
				}
			}
			
	    }
	}
	 syslog_ax_pvlan_dbg("retrun value is %d",ret);
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	
	
	return reply;
	
}




DBusMessage * npd_dbus_pvlan_config_trunk(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	slot	=0;
	unsigned char	port	=0;
	unsigned char  	trunkid	=0;
	unsigned char	lkflag	=0;
	unsigned int	ret = PVLAN_RETURN_CODE_ERROR;
	unsigned int	eth_g_index;
	unsigned int mode = 0;
	
	syslog_ax_pvlan_dbg("Entering config system info!\n");

	if(!((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support pvlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support pvlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
												DBUS_TYPE_BYTE,&slot,
												DBUS_TYPE_BYTE,&port,
												DBUS_TYPE_BYTE,&trunkid,
												DBUS_TYPE_INVALID))) {
			syslog_ax_pvlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					syslog_ax_pvlan_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
		}
		
	    else{
			if (CHASSIS_SLOTNO_ISLEGAL(slot)) {
				if(PVLAN_RETURN_CODE_ERROR==npd_pvlan_add_node( slot,port,trunkid,lkflag,mode)){
					ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
					}
				else{
					if (ETH_LOCAL_PORTNO_ISLEGAL(slot,port)) {
						eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
						 syslog_ax_pvlan_dbg("index for %d/%d is %#0x\n",slot,port,eth_g_index);
						ret = nam_pvlan_trunk_config(eth_g_index,trunkid) ;
						if (ret != 0) {
							 syslog_ax_pvlan_err(" nam_static_fdb_entry_mac_set_for_system ERROR. \n");
							 
						}
						ret = PVLAN_RETURN_CODE_SUCCESS;
					}
				}
			}
	    }
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	
	
	return reply;
	
}

DBusMessage * npd_dbus_pvlan_show_pvlan(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* 			reply;
	DBusMessageIter	 		iter;
	DBusMessageIter	 		iter_array;
	DBusError err;
	unsigned int 			number=0;
	unsigned int 			num;
	unsigned int 			i=0;
	
	pvenode* 				anode=NULL;
	syslog_ax_pvlan_dbg("Entering config system info!\n");
	
	number = npd_pvlan_get_port_number();
	 syslog_ax_pvlan_dbg("the number is %d\n",number);
	anode=npd_pvlan_get_first_node();
	if(anode==NULL){
		syslog_ax_pvlan_dbg("there no node!\n");
		number=0;
	}
	#if 0
	else{
		number=npd_pvlan_change_array_fashion(anode,node);
		}
	#endif
	dbus_error_init(&err);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &number);
	
	dbus_message_iter_open_container (&iter,
									   DBUS_TYPE_ARRAY,
									   DBUS_STRUCT_BEGIN_CHAR_AS_STRING
									   			DBUS_TYPE_UINT16_AS_STRING
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING 
											   	DBUS_TYPE_BYTE_AS_STRING  
											   	DBUS_TYPE_UINT32_AS_STRING
									   DBUS_STRUCT_END_CHAR_AS_STRING,
									   &iter_array);
	
	for (num = 0,i=0; num < PVE_MAX_PORT && number >i; num++)
	{
			
			DBusMessageIter iter_struct;
			 syslog_ax_pvlan_dbg("anode[%d].fag= %d\n",num,anode[num].fag);
			if(anode[num].fag == PVE_TRUE){
				
				 syslog_ax_pvlan_dbg("%7d  %7d  %7d  %7d	%7s\n", anode[num].pvslot,anode[num].pvport,  \
														anode[num].lkslot,anode[num].lkport," --- ");
				
				dbus_message_iter_open_container (&iter_array,
												   DBUS_TYPE_STRUCT,
												   NULL,
												   &iter_struct);
				dbus_message_iter_append_basic
						(&iter_struct,
						 DBUS_TYPE_UINT16,
						 &(anode[num].tkfag));
				
				dbus_message_iter_append_basic
						(&iter_struct,
						 DBUS_TYPE_BYTE,
						 &(anode[num].pvslot));
				dbus_message_iter_append_basic
						(&iter_struct,
						 DBUS_TYPE_BYTE,
						 &(anode[num].pvport));
				dbus_message_iter_append_basic
						(&iter_struct,
						 DBUS_TYPE_BYTE,
						 &(anode[num].lkslot));
				dbus_message_iter_append_basic
						(&iter_struct,
						 DBUS_TYPE_BYTE,
						&(anode[num].lkport));
				dbus_message_iter_append_basic
						(&iter_struct,
						 DBUS_TYPE_UINT32,
						&(anode[num].mode));
				
				 syslog_ax_pvlan_dbg("i= %d\n",i);
				
				i++;
			}
			else{
				/*num++;*/
				 syslog_ax_pvlan_dbg("num= %d\n",num);
				continue;
				}
			
			dbus_message_iter_close_container (&iter_array, &iter_struct);
	}
	
	dbus_message_iter_close_container (&iter, &iter_array);
	
return reply;
	
}


DBusMessage * npd_dbus_pvlan_port_delete(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	slot,port;
	unsigned int	ret = PVLAN_RETURN_CODE_ERROR;
	unsigned int	eth_g_index = 0;
	
	syslog_ax_pvlan_dbg("Entering config system info!\n");

	if(!((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support pvlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support pvlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
		if (!(dbus_message_get_args ( msg, &err,
												DBUS_TYPE_BYTE,&slot,
												DBUS_TYPE_BYTE,&port,
												DBUS_TYPE_INVALID))) {
			syslog_ax_pvlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					syslog_ax_pvlan_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
		}
		

		ret=NPD_DBUS_ERROR_NO_SUCH_PORT;
		if (CHASSIS_SLOTNO_ISLEGAL(slot)) {			
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot,port)) {
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot,port);
				 syslog_ax_pvlan_dbg("index for %d/%d is %#0x\n",slot,port,eth_g_index);
				 ret = PVLAN_RETURN_CODE_SUCCESS;		
			}
		}

		syslog_ax_pvlan_dbg("ready to del slot/port %d/%d index %d\n",slot,port,eth_g_index);
		if(PVLAN_RETURN_CODE_SUCCESS == ret) {
			if(0 == (ret = npd_pvlan_delete_node(slot,port))){
				ret = nam_pvlan_port_delete(eth_g_index);
				if (ret != 0) {
					 syslog_ax_pvlan_dbg(" nam_static_fdb_entry_mac_set_for_system ERROR. \n");	 
				}		
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,  &ret);
	
	
	return reply;
	
}



DBusMessage * npd_dbus_pvlan_config_control(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	slot_no = 0,port_no = 0;
	unsigned int    slot_index;
	unsigned int	ret = PVLAN_RETURN_CODE_ERROR;
	unsigned int    flag =0;
	unsigned int	eth_g_index = 0;
    enum product_id_e productId = PRODUCT_ID;
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port */
	struct eth_port_s* g_ptrs = NULL;
	/*struct vlan_s*  vid=NULL;*/
	
	syslog_ax_pvlan_dbg("Entering config system info!\n");

	if(!((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support pvlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support vpvlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
												DBUS_TYPE_UINT32,&flag,
												DBUS_TYPE_BYTE,&slot_no,
												DBUS_TYPE_BYTE,&port_no,
												DBUS_TYPE_INVALID))) {
			syslog_ax_pvlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
					syslog_ax_pvlan_err("%s raised: %s",err.name,err.message);
					dbus_error_free(&err);
				}
		}
		
	    else {

			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
			if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,port_no)) {
					slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,port_no);
					ret = PVLAN_RETURN_CODE_SUCCESS;
			}	
	            if(PVLAN_RETURN_CODE_SUCCESS == ret){
				   g_ptrs = npd_get_port_by_index(eth_g_index);   /* code optimize: "g_ptrs = NULL" is always false 2013-10-18 */
				   if(g_ptrs == NULL){
						syslog_ax_pvlan_dbg("get port info error \n ");
					    ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	               }	 
			       else {    
	                   if (((PRODUCT_ID_AX7K_I == productId) || (PRODUCT_ID_AX7K == productId)) 
					   		&&(eth_g_index < 4)){
			                isBoard =NPD_TRUE;
	                   	}
	                   if(NPD_FALSE == isBoard){ 	
				            ret = nam_pvlan_control_config(eth_g_index, flag ) ;
				            if (ret != 0) {
						        syslog_ax_pvlan_dbg(" nam_static_fdb_entry_mac_set_for_system ERROR. \n");		 
				            }
					    	ret = PVLAN_RETURN_CODE_SUCCESS;
	                   }
				       
			           else if(NPD_TRUE == isBoard) {
					      ret = NPD_DBUS_NOT_SUPPORT;
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
	
	
	return reply;
	
}

DBusMessage * npd_dbus_pvlan_show_running_cfg(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned int	ret = PVLAN_RETURN_CODE_ERROR;
	char *strShow = NULL;
	int number = 0;
	int num = 0,i = 0;
	int length = 0;

	
	strShow = (char *)malloc(PVE_SAVE_BUFF);
	if(NULL == strShow) {
		ret = PVLAN_RETURN_CODE_ERROR;
	}
	else {
		memset(strShow, 0, PVE_SAVE_BUFF);
		pvenode* anode=NULL;
		char* pos = strShow;
		
		number = npd_pvlan_get_port_number();
		anode = npd_pvlan_get_first_node();
		if(anode == NULL){
			number=0;
		}

		syslog_ax_pvlan_dbg("configured pvlan number %d\n", number);
		for (num = 0, i = 0; num < PVE_MAX_PORT && number >i; num++) {
			if((length + 50 ) < PVE_SAVE_BUFF) {
				if(PVE_TRUE == anode[num].fag) {
					if(PVE_TRUE == anode[num].tkfag) {
						length += sprintf(pos,"config pvlan port %d/%d uplink %d\n",anode[num].pvslot,anode[num].pvport,anode[num].lkslot);
						pos = strShow + length;
						i++;
					}
					else if(PVE_FAIL == anode[num].tkfag) {
						length += sprintf(pos,"config pvlan port %d/%d uplink %d/%d mode %s\n",	\
							anode[num].pvslot,anode[num].pvport,anode[num].lkslot,anode[num].lkport,		\
							(PVE_MODE_SINGLE_E == anode[num].mode ) ? "single" : "multiple");
						pos = strShow + length;
						i++;
					}
				}
			} 
		}
	}

	/*printf("%s",strShow);*/
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING, &strShow);
	free(strShow);
	strShow = NULL;
	
	return reply;
	
}

int npd_pvlan_init
(
	void
)
{
	int ret = 0;
	/*if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){
		return;
	}*/
	if(!((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support pvlan initial!\n");
		return NPD_FAIL;
	}
	else if(((productinfo.capbmp) & FUNCTION_PVLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_PVLAN_VLAUE)){
		npd_syslog_dbg("do not support pvlan initial!\n");
		return NPD_FAIL;
	}
	else{
		ret= nam_pvlan_enable();
	}
	return ret;     /* code optimize: Missing return statement. zhangdi@autelan.com 2013-10-18 */
}

#ifdef __cplusplus
}
#endif
