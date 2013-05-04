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
* npd_vlan.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		API used in NPD for VLAN module.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.213 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

#include <unistd.h>  /*getpagesize(  ) */  
#include <sys/ipc.h>   
#include <sys/shm.h>
#include <unistd.h>
#include <fcntl.h>

#include <fcntl.h>
#include <sys/mman.h>  /* for mmap() */

#define MY_SHM_VLANLIST_ID 54321  

#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "dbus/npd/npd_dbus_def.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_log.h"
#include "npd_product.h"
#include "npd_eth_port.h"
#include "npd_c_slot.h"
#include "npd_vlan.h"		/* sw data structure in npd*/
#include "npd_intf.h"
#include "npd_trunk.h"
#include "npd_igmp_snp_com.h"
#include "npd_rstp_common.h"
#include "npd_pvlan.h"
#include "npd_fdb.h"
#include "npd_dhcp_snp_com.h"
#include "npd_dhcp_snp_options.h"
#include "sysdef/returncode.h"
#include "npd_mld_snp_com.h"
#include "cvm/autelan_product.h"
#include <syslog.h>

#include "board/netlink.h"   /* for netlink msg */
netlink_msg_t n_vlan_msg;
struct vlan_s 	**g_vlans = NULL;
int vlan_mtu = NPD_VLAN_MTU_VALUE;
int bcast_ratelimit = NPD_FALSE;
unsigned char g_vlan_egress_filter = NPD_TRUE;
extern struct global_ethport_s *start_fp[MAX_SLOT_COUNT];
extern dst_trunk_s *g_dst_trunk;
extern unsigned short advanced_routing_default_vid;

extern unsigned char igmpSnpEnable;
extern unsigned char mldSnpEnable;
unsigned int npd_vlan_qinq_disable(unsigned short vlanId);
unsigned int npd_vlan_trunk_qinq_disable(unsigned short trunkId,unsigned short vlanId);



int npd_parse_ve_params
(
    char *str,
    unsigned char *slotno,
    unsigned char *cpu_no,
    unsigned char *cpu_port_no,
    unsigned int *tag1, 
    unsigned int *tag2
)	
{
	char *endp = NULL;	
	char *endptr = NULL;
	char *endptr1 = NULL, *endptr2 = NULL;
	char * tmpstr = str;
	int ret = VLAN_RETURN_CODE_ERR_GENERAL;

	/*judge if the input is correct*/
    if((NULL != str)&&(!strncmp(str,"ve",2)))
	{
		tmpstr = str+2;
		if (NULL != tmpstr) 
		{
			if((tmpstr[0] >= '0')&&(tmpstr[0] <= '9'))
			{
			 	*slotno = (char)strtoul(tmpstr,&endp,10);       
			    if(*slotno <= CHASSIS_SLOT_COUNT) /* code optimize: unsigned value   houxx@autelan.com  2013-1-18 */
			    {
					ret = VLAN_RETURN_CODE_ERR_NONE;
			    }
				else
				{
					syslog_ax_vlan_err("slot nunber must be in 1 ~ %d\n",CHASSIS_SLOT_COUNT);
				}
			}
			else
			{
				syslog_ax_vlan_err("slot num must be a  number\n");
			}
		}
		else
		{
			syslog_ax_vlan_err("input param is not correct !\n");
		}
	}
	else
	{
		syslog_ax_vlan_err("input is NULL or not ve intf !\n");
	}

	if ((endp != NULL) && (ret == VLAN_RETURN_CODE_ERR_NONE)) 
	{		
		/* get cpu no */
        if('f' == endp[0])
        {
			*cpu_no = 1;			
        }
		else if('s' == endp[0])
		{
			*cpu_no = 2;			
		}
		else
		{
			syslog_ax_vlan_err("Param error ! ve name is %s\n",str);	
			ret = VLAN_RETURN_CODE_ERR_GENERAL;
		}

		if(ret == VLAN_RETURN_CODE_ERR_NONE)
		{
			ret == VLAN_RETURN_CODE_ERR_GENERAL;
			if(('0' < endp[1])&&('9' >= endp[1]))
			{
				/* get cpu port no */
	        	*cpu_port_no = strtoul((char *)&(endp[1]),&endptr,10);
				*cpu_port_no = (*cpu_no-1)*8+*cpu_port_no;
				
				if((*cpu_port_no >= 0) && (*cpu_port_no <= 16))/* code optimize: Unsigned compared against 0  houxx@autelan.com  2013-1-18 */
				{
					/* get .tag1.tag2 */
		            if(('.' == endptr[0])&&(('0' < endptr[1])&&('9' >= endptr[1])))
		            {
		            	*tag1 = strtoul((char *)&(endptr[1]),&endptr1,10);
		    			if(endptr1[0] == '\0') 
						{
		    				*tag2 = 0;
							if((*tag1 > 0) && (*tag1 <= 4096))
							{
								ret = VLAN_RETURN_CODE_ERR_NONE;
							}
		    			}
						else if(('.' == endptr1[0])&&(('0' < endptr1[1])&&('9' >= endptr1[1])))
		                {
		                	*tag2 = strtoul((char *)&(endptr1[1]),&endptr2,10);
		        			
		                	if((NULL == endptr2)||('\0' == endptr2[0]))
							{
		                		if((*tag2 > 0) && (*tag2 <= 4096))
								{
									ret = VLAN_RETURN_CODE_ERR_NONE;
								}
		                	}
		                }
		            }	
				}
				else
				{
					syslog_ax_vlan_err("cpu port no is not correct !\n");
				}
			}
			else
			{
				syslog_ax_vlan_err("get cpu port no fail !\n");
			}
		}
		else
		{
			syslog_ax_vlan_err("get cpu  no fail !\n");
		}
	}
	else
	{
		syslog_ax_vlan_err("%s is not correct ve sub intf or QinQ intf !\n",str);
		ret = VLAN_RETURN_CODE_ERR_GENERAL;
	}
	syslog_ax_vlan_dbg("vename is %s,parse result:slot %d,cpu_no %d,cpu_port_no %d,vid1 %d,vid2 %d",str,*slotno,*cpu_no,*cpu_port_no,*tag1,*tag2);
	return ret;	
}

/**********************************************************************************
 *
 * init global vlan structure:NPD_VLAN_NUMBER_MAX pointers point to detailde vlan structure.
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
 *
 **********************************************************************************/
void npd_init_vlan(void) {
    /* if this is distributed system, goto npd_init_distributed_vlan() */
    if((SYSTEM_TYPE == IS_DISTRIBUTED)||(PRODUCT_ID == PRODUCT_ID_AX7K_I))
	{
		npd_syslog_dbg("npd_init_vlan() is NULL for distributed system !\n");		
		return;
	}
	
	struct vlan_s * vlanInfo = NULL;
	g_vlans = malloc(sizeof(struct vlan_s*)*(NPD_VLAN_NUMBER_MAX));
	
	if(NULL == g_vlans) {
		 syslog_ax_vlan_err("memory alloc error for vlan init!!\n");
		return;
	}
	memset(g_vlans,0,sizeof(struct vlan_s *) * NPD_VLAN_NUMBER_MAX);
	/*
	if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608== PRODUCT_ID){/*5612e not support*/
	/*	syslog_ax_vlan_dbg("this produc do not support vlan.\n");
		return;
	}*/
	
	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan initial!\n");
		return;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan initial!\n");
		return;
	}
	else {
		create_default_vlan();
		create_port_l3intf_vlan();
	    if(npd_product_adv_routing_cmd_support()){
		    npd_vlan_activate(NPD_ADV_ROUTING_DEFAULT_VID,"4094vlan");
#if 0
			if(PRODUCT_ID_AX7K_I == PRODUCT_ID){
				int local_slot = LOCAL_CHASSIS_SLOT_NO;	
				if((0 == local_slot) || (1 == local_slot)){/* create vlan obc0 for slot 0 and slot 1*/

					npd_vlan_activate(NPD_VLAN_OBC0_VLAN_ID, "obc0");
					
					nam_asic_vlan_entry_ports_add(NPD_AX7KI_OBC0_PORT_DEV_NUM,\
												NPD_VLAN_OBC0_VLAN_ID, \
												NPD_AX7KI_OBC0_PORT_PORT_NUM,
												FALSE);
					nam_asic_set_port_vlan_ingresflt(NPD_AX7KI_OBC0_PORT_DEV_NUM,\
						NPD_AX7KI_OBC0_PORT_PORT_NUM,NPD_TRUE);
					nam_pvlan_cscd_port_config_spi(NPD_AX7KI_OBC0_PORT_DEV_NUM,\
												NPD_AX7KI_OBC0_PORT_PORT_NUM,\
												PROMISCUOUS_TARGET_DEV_NUM,\
												PROMISCUOUS_TARGET_PORT_NUM);
					#if 1
					nam_asic_vlan_entry_ports_add(ax7i_xg_conn_port_mapArr[0][0].devNum,\
												NPD_VLAN_OBC0_VLAN_ID, \
												ax7i_xg_conn_port_mapArr[0][0].portNum,
												FALSE);
					
					nam_pvlan_cscd_port_config_spi(ax7i_xg_conn_port_mapArr[0][0].devNum,\
												ax7i_xg_conn_port_mapArr[0][0].portNum,\
												PROMISCUOUS_TARGET_DEV_NUM,\
												PROMISCUOUS_TARGET_PORT_NUM);
					nam_asic_set_port_vlan_ingresflt(ax7i_xg_conn_port_mapArr[0][0].devNum,\
						ax7i_xg_conn_port_mapArr[0][0].portNum,NPD_TRUE);
					#endif
					vlanInfo = npd_find_vlan_by_vid(NPD_VLAN_OBC0_VLAN_ID);
					
					vlanInfo->brgVlan.state = VLAN_STATE_UP_E;

				}		
			}
#endif
	    }
	    else{
	        advanced_routing_default_vid = 0;
	    }
		return;
	}
}

int create_default_vlan(void)
{
	struct vlan_s *vlan_1 = NULL;
	struct eth_port_s *portInfo = NULL;
	char	vlanName[ALIAS_NAME_SIZE] = "DEFAULT";
	unsigned int eth_g_index = 0, slot_index = 0;
	unsigned int i = 0, j = 0, loop = 0, ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned char devNum = 0, virPortNum = 0, card_type = 0;
	enum product_id_e productId = PRODUCT_ID;
	
	vlan_1 = npd_create_vlan_by_vid(DEFAULT_VLAN_ID);
	vlan_1->vid = 1;
	
	for(i=0;i<ALIAS_NAME_SIZE;i++){		
		vlan_1->vlanName[i] = vlanName[i];
	}

	ret = npd_vlan_fillin_global(DEFAULT_VLAN_ID, vlan_1);
	if(VLAN_RETURN_CODE_BADPARAM == ret) {
		 syslog_ax_vlan_err("fillin default vlan to global Error,retVal %d.\n",ret);
		return ret;
	}

	if(!PRODUCT_IS_BOX) {/* chassis product */
		i = 1; /* slot 0:CRSMU port not add to default vlan*/
		if(PRODUCT_ID_AX7K_I == productId) {
			i = 2; /* slot 0 and 1 are dual-master slots */
		}
		loop = CHASSIS_SLOT_COUNT;
	}
	else { /* box product */
		i = CHASSIS_SLOT_START_NO; /* as default we assume box product has subcard */
		if(PRODUCT_ID == PRODUCT_ID_AU3K_BCAT)
		{
			loop = CHASSIS_SLOT_COUNT + CHASSIS_SLOT_START_NO;
		}
		else
		{
			loop = CHASSIS_SLOT_COUNT;
		}
	
		/* !!!excetion!!!
		 * box product without subcard
		 * override loop count */
		if(CHASSIS_SLOT_START_NO == CHASSIS_SLOT_COUNT) {
			loop = CHASSIS_SLOT_COUNT + 1;
		}
	}
	/* add for port on slot-10 can not be add in default vlan,zhangdi 2011-06-10 */
	if(IS_DISTRIBUTED == SYSTEM_TYPE)
	{
        i = CHASSIS_SLOT_START_NO;
        loop = CHASSIS_SLOT_COUNT + CHASSIS_SLOT_START_NO;
	}
	for (; i < loop; i++ ) { /* i used as slot number */
		if (CHASSIS_SLOTNO_ISLEGAL(i)) {
			slot_index = CHASSIS_SLOT_NO2INDEX(i);
			if(0 == ETH_LOCAL_PORT_COUNT(slot_index))
				continue;
						
			for (j = ETH_LOCAL_PORT_START_NO(slot_index); 	\
					j <= ( ETH_LOCAL_PORT_COUNT(slot_index) + ETH_LOCAL_PORT_START_NO(slot_index) -1); j++) {/* j used as port number*/
				if (ETH_LOCAL_PORTNO_ISLEGAL(i,j)) {
					if(BOARD_TYPE == BOARD_TYPE_AX71_2X12G12S){
						if((j == 27) || (j == 28))
							continue;
					}
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(i,j);
					portInfo = npd_get_port_by_index(eth_g_index);
					if(NULL == portInfo) {
						syslog_ax_vlan_dbg("port %d/%d index %d not create.\r\n",i,j,eth_g_index);
						ret = npd_get_devport_by_global_index(eth_g_index, &devNum, &virPortNum);
						nam_asic_vlan_entry_ports_del(DEFAULT_VLAN_ID, devNum, virPortNum, NPD_FALSE);
						continue;
					}

					/* if BOX has its subcard(s) which at its slot and its type is AX51-XFP 
					 * add this subcard to the default vlan
					 */
					if(npd_eslot_check_subcard_module(MODULE_TYPE_ON_SLOT_INDEX(slot_index))) {
						ret = npd_get_subcard_port_type(j, &card_type);
						syslog_ax_vlan_dbg("slot %d port %d to get subcard port type ret %d\n",i,j ,ret);		
						/* !!! we only care about:
						 *		- subcard inserted and 
						 *		- type is XFP 
						 */
						if(!((0 == ret) && (AX51_XFP == card_type))) {
							continue;
						}
					}
					
					/* the main control port of 5612i should not add to the vlan*/
					if((PRODUCT_ID_AX5K_I == productId) \
						&& (eth_g_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9))  \
						&& (eth_g_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) {
						syslog_ax_vlan_dbg("port(%d %d) index %d do not allow adding to vlan.\r\n",i,j,eth_g_index);
						continue;
					}
					ret = npd_vlan_port_member_add(DEFAULT_VLAN_ID, eth_g_index, NPD_FALSE);
					if(VLAN_RETURN_CODE_ERR_NONE != ret) {
						 syslog_ax_vlan_err("default vlan add port %d/%d error %d\r\n",i,j ,ret);
					}
					else {
						syslog_ax_vlan_dbg("add port %d/%d to default vlan ok\r\n",i,j);

						/*set vlan port ingress filter*/
						npd_vlan_set_port_vlan_ingresfilter(eth_g_index, NPD_TRUE);

						ret = VLAN_RETURN_CODE_ERR_NONE;
					}
				}
				else {
					 syslog_ax_vlan_err("illegal local port no!\r\n");
				}
			}
		}
		else 
		{
			 syslog_ax_vlan_err("illegal slot no!\n");
		}
	}
	return ret ;
}

/*
* make port free from all vlans that the port once belong to.
*/
int npd_port_vlan_free
(
	unsigned int eth_g_index
)
{	
	unsigned int i = 0, ret = 0;
	unsigned char isTagged = 0;
	
	/*check the ETH-port is NOT member of other active vlan as untagged Or tagged*/
	for(i = 2; i < NPD_PORT_L3INTF_VLAN_ID; i++) {
		/* syslog_ax_vlan_dbg("check port index %d is member of vlan %d or NOT.",eth_g_index,i);*/
		ret = npd_check_vlan_exist(i);
		if (VLAN_RETURN_CODE_VLAN_EXISTS == ret) {
			ret = npd_vlan_check_contain_port(i, eth_g_index, &isTagged);
			if(NPD_TRUE == ret) {
				/* syslog_ax_vlan_dbg("eth_port index %d Untagged member of vlan %d",eth_g_index,i);*/
				npd_vlan_interface_port_del(i, eth_g_index, isTagged);
			}
		}
		else {
			continue;
		}
	}
	return 0;

}

/*
* make untag port free from the vlan.
*/
int npd_untag_port_del_from_vlan
(
	unsigned int eth_g_index
)
{	
	unsigned int i = 0,ret = 0;
	
	/*check the ETH-port is NOT member of other active vlan as untagged Or tagged*/
	for(i = 2; i < NPD_PORT_L3INTF_VLAN_ID; i++) {
		/* syslog_ax_vlan_dbg("check port index %d is member of vlan %d or NOT.",eth_g_index,i);*/
		ret = npd_check_vlan_exist(i);
		if (VLAN_RETURN_CODE_VLAN_EXISTS == ret) {
			ret = npd_vlan_check_port_membership(i,eth_g_index,NPD_FALSE);
			if(NPD_TRUE == ret) {
				/* syslog_ax_vlan_dbg("eth_port index %d Untagged member of vlan %d",eth_g_index,i);*/
				npd_vlan_interface_port_del(i, eth_g_index, NPD_FALSE);
				break;
			}
		}
		else {
			continue;
		}
	}
	return 0;

}


int create_port_l3intf_vlan(void)
{
	struct vlan_s *vlan_max = NULL;
	char	vlanName[ALIAS_NAME_SIZE] = "vlan4095";
	unsigned int i = 0, ret = VLAN_RETURN_CODE_ERR_NONE;

	vlan_max = npd_create_vlan_by_vid(NPD_PORT_L3INTF_VLAN_ID);
	vlan_max->vid = NPD_PORT_L3INTF_VLAN_ID;
	for(i=0;i<ALIAS_NAME_SIZE;i++){
		vlan_max->vlanName[i] = vlanName[i];
	}

	ret = npd_vlan_fillin_global(NPD_PORT_L3INTF_VLAN_ID, vlan_max);
	if(VLAN_RETURN_CODE_BADPARAM == ret) {
		 syslog_ax_vlan_err("fillin vlan %d error %d.\n",NPD_PORT_L3INTF_VLAN_ID,ret);
		return ret;
	}
	nam_asic_port_l3intf_vlan_entry_set(PRODUCT_ID, NPD_PORT_L3INTF_VLAN_ID);

	syslog_ax_vlan_dbg("vlan %d(%s) created successfully!\n",NPD_PORT_L3INTF_VLAN_ID,vlanName);
	return ret ;
}
/**/
unsigned int npd_port_l3intf_vlan_add_port_mbr
(
	unsigned int	eth_g_index
)
{
	unsigned int  ret;
	unsigned char devNum;
	unsigned char virPortNum;
	unsigned char isTagged;	
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNum);
	if (ret != NPD_SUCCESS) 
	{
		 syslog_ax_vlan_err("npd get asic port from global index %#x error when vlan add port mbr\n",eth_g_index);
		return VLAN_RETURN_CODE_BADPARAM;
	}
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = VLAN_RETURN_CODE_ERR_NONE;
		} 
		else {
			 syslog_ax_vlan_err("npd get illegal asic port(%d,%d) from global index %#x\n",
			 		devNum, virPortNum, eth_g_index);
			ret = VLAN_RETURN_CODE_BADPARAM;
			return ret;
		}
	}
	else
	{
		 syslog_ax_vlan_err("npd get illegal asic port(%d,%d) from global index %#x\n",
					 devNum, virPortNum, eth_g_index);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	/*if NOT Steps into the (vlan-config CMD node),vlan exists or not Test is wanted.*/
	/*if Steps into (vlan-config CMD node), it does NOT need test vlan node exist or not,because*/
	ret = npd_check_vlan_exist(NPD_PORT_L3INTF_VLAN_ID);
	if (VLAN_RETURN_CODE_VLAN_EXISTS != ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return NPD_VLAN_BADPARAM.*/
	}
	/*here MUST check the ETH-port is already member of the VLAN or NOT*/
	ret = npd_vlan_check_contain_port(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,&isTagged);
	if (NPD_TRUE == ret) {
		/* syslog_ax_vlan_dbg("Port %d has already be the member of vlan % d\n",virPortNum,vlanId);*/
		return VLAN_RETURN_CODE_VLAN_EXISTS;
	}

	npd_port_vlan_free(eth_g_index);
	/*HW first,SW follow*/
	/*call nam_asic_vlan_entry_ports_add to set the Vlan table entry in dev.*/
	/*call npd_vlan_interface_add_port_member to do Record in SW.*/
	ret = nam_asic_vlan_entry_ports_add(devNum, NPD_PORT_L3INTF_VLAN_ID, virPortNum, NPD_FALSE);
	if (VLAN_CONFIG_SUCCESS == ret) {
	 
		ret = npd_vlan_port_member_add(NPD_PORT_L3INTF_VLAN_ID,eth_g_index,NPD_FALSE);
		if(VLAN_RETURN_CODE_ERR_NONE != ret) {
			 syslog_ax_vlan_err("Npd_vlan::npd_vlan_port_member_add retVal %d,ERR!",ret);
			return ret;
		}
		ret = npd_vlan_port_member_del(DEFAULT_VLAN_ID, eth_g_index, NPD_FALSE);
		if(VLAN_RETURN_CODE_ERR_NONE != ret) {
			 syslog_ax_vlan_err("Npd_vlan::port index %d NOT member of vlan 1.",eth_g_index);
		}
		/*nam_asic_port_brg_bypass_en(devNum,virPortNum,NPD_TRUE);*/
		nam_asic_vlan_port_route_en(devNum,virPortNum);
		/*to call set port Mode is L3 interface.this may called by FUN in npd_eth_port*/
	}
	return ret;
}
unsigned int npd_port_l3intf_vlan_del_port_mbr
(
	unsigned int	eth_g_index
)
{
	unsigned char	isTagged = NPD_FALSE;
	unsigned short	vlanId = NPD_PORT_L3INTF_VLAN_ID;
	npd_vlan_interface_port_del(vlanId, eth_g_index,isTagged);
	return 0;
}
/* check a vlan interface node has exists in the list or not*/
/* we use vlanId as a index for searching for the node*/
/**********************************************************************************
 *  npd_check_vlan_exist
 *
 *	DESCRIPTION:
 * 		Check out if specified vlan has been created or not
 * 		VLAN ID used as an index to find vlan structure.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		VLAN_RETURN_CODE_VLAN_EXISTS	- if vlan has been created before 
 *		VLAN_RETURN_CODE_VLAN_NOT_EXISTS	- if vlan doesn't exist,or hasn't been created before or is created automaically
 *
 **********************************************************************************/
unsigned int npd_check_vlan_exist
(
	unsigned short vlanId
)
{
	if(npd_check_vlan_real_exist(vlanId) && (!npd_find_vlan_by_vid(vlanId)->isAutoCreated)){
        return VLAN_RETURN_CODE_VLAN_EXISTS;
	}
    else{
        return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
    }
}
/* check a vlan interface node has exists in the list or not*/
/* we use vlanId as a index for searching for the node*/
/**********************************************************************************
 *  npd_check_vlan_real_exist
 *
 *	DESCRIPTION:
 * 		Check out if specified vlan has been created or not
 * 		VLAN ID used as an index to find vlan structure.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		TRUE	- if vlan has been created before 
 *		FALSE	- if vlan doesn't exist,or hasn't been created before
 *
 **********************************************************************************/
unsigned int npd_check_vlan_real_exist
(
	unsigned short vlanId
)
{
	/*vlanId should be in range of (0,4096]*/
	if((vlanId == 0)||(vlanId > NPD_VLAN_NUMBER_MAX))
		return FALSE; 
	if(NULL == g_vlans[vlanId-1]) {
		return FALSE; 
	}
	else
		return TRUE;
}

/**********************************************************************************
 *  npd_check_vlan_status
 *
 *	DESCRIPTION:
 * 		Check out if specified vlan is UP or DOWN
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		VLAN_STATE_UP_E  - if vlan is up
 *		VLAN_STATE_DOWN_E	- if vlan is down
 *		-NPD_VLAN_NOTEXISTS	- if vlan doesn't exist,or hasn't been created before
 *
 **********************************************************************************/
int npd_check_vlan_status
(
	unsigned short vlanId
)
{
	struct vlan_s 	*vlanNode = NULL;
	unsigned int ret = 0;
	unsigned int status = VLAN_STATE_UP_E;
    if(SYSTEM_TYPE == IS_DISTRIBUTED)
    {
		/* If is L3 eth-port, get the vlan status from g_vlans[] */
		if(4095 == vlanId)
		{
        	/* first check vlan exists or not*/
        	ret = npd_check_vlan_real_exist(vlanId);
        	if(TRUE == ret){
        		/*seconde check vlan state*/
        		vlanNode = g_vlans[vlanId-1];
        		if(VLAN_STATE_UP_E != vlanNode->brgVlan.state) {
        			status = VLAN_STATE_DOWN_E;
        		}
        	}	
		}
		else    /* If is L3 vlan, get from g_vlanlist[] */
		{
    		/* Get the vlan status from g_vlanlist[], not only status of local board */
            if(VLAN_DOWN == g_vlanlist[vlanId-1].updown )
    		{
    		    status = VLAN_STATE_DOWN_E;
            }
		}
		return status;
    }
	/* first check vlan exists or not*/
	ret = npd_check_vlan_real_exist(vlanId);
#if 0
	if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
	}
	else if(VLAN_RETURN_CODE_BADPARAM == ret) {
		return VLAN_RETURN_CODE_BADPARAM;
	}
	else if(VLAN_RETURN_CODE_VLAN_EXISTS == ret) {
		/*seconde check vlan state*/
		vlanNode = g_vlans[vlanId-1];
		if(VLAN_STATE_UP_E == vlanNode->brgVlan.state) {
			ret = VLAN_STATE_UP_E;
		}
		else {
			ret = VLAN_STATE_DOWN_E;
		}
	}

	return ret;
#endif
	if(TRUE == ret){
		/*seconde check vlan state*/
		vlanNode = g_vlans[vlanId-1];
		if(VLAN_STATE_UP_E != vlanNode->brgVlan.state) {
			status = VLAN_STATE_DOWN_E;
		}
	}

	return status;
}

/**********************************************************************************
 *  npd_create_vlan_by_vid
 *
 *	DESCRIPTION:
 * 		Create vlan node by VLAN ID
 * 		Vlan node should be have not been created before.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		pointer to vlan being created.
 *
 **********************************************************************************/
struct vlan_s* npd_create_vlan_by_vid
(
	unsigned short vlanId
)
{
	struct vlan_s* node = NULL;
	node = (struct vlan_s *)malloc(sizeof(struct vlan_s));
	if(NULL == node) {
		 syslog_ax_vlan_err("create vlan by vid memory alloc fail!\n");
		return NULL;
	}
	memset(node,0,sizeof(struct vlan_s));
	node->vid = vlanId;

	return node;
}
/**********************************************************************************
 *  npd_create_vlan_by_name
 *
 *	DESCRIPTION:
 * 		Create vlan node by VLAN ID
 * 		Vlan node should be have not been created before.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		pointer to vlan being created.
 *
 **********************************************************************************/
struct vlan_s* npd_create_vlan_by_name
(
	char *name
)
{
	struct vlan_s* node = NULL;
	if(NULL == name) {
		return  NULL;
	}
	node = (struct vlan_s *)malloc(sizeof(struct vlan_s));
	if(NULL == node) {
		 syslog_ax_vlan_err("error:null memory allocated!\n");
		return NULL;
	}
	memset(node,0,sizeof(struct vlan_s));
	memcpy(node->vlanName , name ,NPD_VLAN_IFNAME_SIZE );
				
	return node;
}

/**********************************************************************************
 *  npd_find_vlan_by_vid
 *
 *	DESCRIPTION:
 * 		Check out if specified vlan has been created or not
 * 		VLAN ID used as an index to find vlan structure.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL  - if parameters given are wrong
 *		vlanNode	- if vlan has been created before 
 *		
 **********************************************************************************/
struct vlan_s *npd_find_vlan_by_vid
(
	unsigned short vlanId
)
{
	struct vlan_s *vlanNode = NULL;
	/*vlanId should be in range of (0,4096]*/
	if((vlanId == 0)||(vlanId > NPD_VLAN_NUMBER_MAX))
		vlanNode = NULL;
	else
	    vlanNode = g_vlans[vlanId-1];

	return vlanNode;
}

/**********************************************************************************
 *  npd_find_vlan_by_name
 *
 *	DESCRIPTION:
 * 		Check out if specified vlan has been created or not
 * 		name is used to compare with each vlan exists.
 *
 *	INPUT:
 *		name - vlan name
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		pointer to vlan found.
 *
 **********************************************************************************/
struct vlan_s* npd_find_vlan_by_name
(
	char *name
)
{
	struct vlan_s* node = NULL;

	int iter = 0, found = 0;

	for(iter = 0;iter < NPD_VLAN_NUMBER_MAX; iter++) {
		node = g_vlans[iter];
		if(NULL == node)
			continue;	/*vlan null*/
		else if('\0' == node->vlanName[0]) {
			continue;	/*vlan name null*/
		}
		else if(strcmp(name,node->vlanName)) {			
			continue;	/*vlan name not match*/
		}
		else {
			found = 1;
			break;
		}
	}

	if(found)
		return node;
	else
		return NULL;
}

/**********************************************************************************
 *  npd_vlan_fillin_global
 *
 *	DESCRIPTION:
 * 		fill in vlan info to global vlan array for specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		vlanNode - vlan structure pointer
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE - if no error occurred.
 *		NPD_VLAN_BADPARAM - if vlan id is out of range.
 *
 **********************************************************************************/
unsigned int npd_vlan_fillin_global
(
	unsigned short vlanId,
	struct vlan_s *vlanNode
)
{
	/*vlanId should be in range of (0,4096]*/
	if((vlanId == 0)||(vlanId > NPD_VLAN_NUMBER_MAX))
		return VLAN_RETURN_CODE_BADPARAM;

	g_vlans[vlanId - 1] = vlanNode;

	return VLAN_RETURN_CODE_ERR_NONE;
}
/**********************************************************************************
 *  npd_vlan_flushout_global
 *
 *	DESCRIPTION:
 * 		flush out vlan info from global vlan array for specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE - if no error occurred.
 *		NPD_VLAN_BADPARAM - if vlan id is out of range.
 *
 **********************************************************************************/

unsigned int npd_vlan_flushout_global
(
	unsigned short vlanId
)
{
	/*vlanId should be in range of (0,4096]*/
	if((vlanId == 0)||(vlanId > NPD_VLAN_NUMBER_MAX))
		return VLAN_RETURN_CODE_BADPARAM;

	g_vlans[vlanId - 1] = NULL;

	return VLAN_RETURN_CODE_ERR_NONE;
}

/**********************************************************************************
 *  npd_delete_vlan_by_vid
 *
 *	DESCRIPTION:
 * 		Delete vlan node by VLAN ID
 * 		Vlan node should be have been created before.
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE - if no error occurred
 *		NPD_VLAN_NOTEXISTS - if vlan does not exists
 *		
 *
 **********************************************************************************/
int npd_delete_vlan_by_vid
(
	unsigned short vlanId
)
{
	struct vlan_s* node = NULL;
	int retval = VLAN_RETURN_CODE_ERR_NONE;

	node = npd_find_vlan_by_vid(vlanId);

	if (NULL == node) {
	 	retval = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
	}
	else {
		if(node->isAutoCreated){
			retval = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
		}
		else if(((NPD_PORT_L3INTF_VLAN_ID - 1) == vlanId)&&\
			(!(node->isAutoCreated))){
			node->isAutoCreated = TRUE;
			/* memcpy(node->vlanName,"4094vlan",NPD_VLAN_IFNAME_SIZE); */
			strcpy(node->vlanName,"4094vlan");/* code optimize: Out-of-bounds access houxx@autelan.com  2013-1-18 */
		}
		else{
			free(node);
			npd_vlan_flushout_global(vlanId);
			node = NULL;
		}
	}	 
	
	return retval;
}
/**********************************************************************************
 *  npd_vlan_check_port_membership
 *
 *	DESCRIPTION:
 * 		check out if a port is an untagged or tagged port member of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port member
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_check_port_membership
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  isTagged
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
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
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if(NULL != node && eth_g_index == node->eth_global_index) {
				return NPD_TRUE;
			}
		}
	}

	return NPD_FALSE;
}
 #if 1
/**********************************************************************************
 *  npd_vlan_check_port_igmp_routeport
 *
 *	DESCRIPTION:
 * 		check out if a port is an igmp route port of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		flag - route port member
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a igmp route port of vlan
 *		NPD_FALSE - if port is not a igmp route port of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_check_port_igmp_routeport
(
	unsigned short vlanId,
	unsigned char  isTagged,
	unsigned int   eth_g_index
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}
	/* tagged port list*/
	if(NPD_TRUE == isTagged) {
		portList = vlanNode->tagPortList;
	}
	else if(NPD_FALSE == isTagged) {
		portList = vlanNode->untagPortList;
	}

	if(NULL == portList) {
		return IGMPSNP_RETURN_CODE_NULL_PTR;
	}
	else if(0 != portList->count) {
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if (NULL != node && 
				NPD_TRUE == node->igmp_rport_flag &&
				eth_g_index == node->eth_global_index)
			{
				return IGMPSNP_RETURN_CODE_OK;
			}
		}
	}
	return IGMPSNP_RETURN_CODE_ERROR;
}


/**********************************************************************************
 *  npd_vlan_port_igmp_mcrouter_flag_set
 *
 *	DESCRIPTION:
 * 		check out if a port is an igmp route port of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		flag - route port member
 *		en_dis -1: add  or  0: delete
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a igmp route port of vlan
 *		NPD_FALSE - if port is not a igmp route port of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_igmp_mcrouter_flag_set
(
	unsigned short vlanId,
	unsigned int  eth_g_idx,
	unsigned char  isTagged,
	unsigned char  en_dis
)
{
	syslog_ax_vlan_dbg("vlan %d %s port %d igmp_mcrouter_flag %d.",vlanId,isTagged? "tagged":"untagged",eth_g_idx,en_dis);
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}
	/* tagged port list*/
	if(NPD_TRUE == isTagged) {
		portList = vlanNode->tagPortList;
	}
	else if(NPD_FALSE == isTagged) {
		portList = vlanNode->untagPortList;
	}

	if(NULL == portList) {
		return IGMPSNP_RETURN_CODE_NULL_PTR;
	}
	else if(0 != portList->count) {
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if(NULL != node && eth_g_idx == node->eth_global_index) {
				node->igmp_rport_flag = en_dis ;
				syslog_ax_vlan_dbg("igmp routeportd set igmp_rport_flag = %d,ok.",node->igmp_rport_flag);
				return IGMPSNP_RETURN_CODE_OK;
			}
		}
	}

	return IGMPSNP_RETURN_CODE_ERROR;
}


/**********************************************************************************
 *  npd_vlan_check_contain_igmp_routeport
 *
 *	DESCRIPTION:
 * 		check out if a port is an untagged or tagged port member of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		isTagged - ethernet port is untagged or tagged port member of vlan
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_check_contain_igmp_routeport
(
	unsigned short vlanId,
	unsigned int   eth_g_index
)
{
	unsigned int tmp_eth_index = eth_g_index;

	/* find the port in vlan's untag port list */
	if(IGMPSNP_RETURN_CODE_OK == npd_vlan_check_port_igmp_routeport(vlanId,NPD_FALSE,tmp_eth_index)){
		syslog_ax_vlan_dbg("find untag route port %d in vlan %d\n", tmp_eth_index, vlanId);
		return IGMPSNP_RETURN_CODE_OK;
	}
	/* find the port in vlan's tag port list */
	else if(IGMPSNP_RETURN_CODE_OK == npd_vlan_check_port_igmp_routeport(vlanId,NPD_TRUE,tmp_eth_index)) {
		syslog_ax_vlan_dbg("find tag route port %d in vlan %d\n", tmp_eth_index, vlanId);
		return IGMPSNP_RETURN_CODE_OK;
	}
	else {
		/* not find the port in vlan's untag/tag port list */
		syslog_ax_vlan_dbg("not find route port %d in vlan %d\n", tmp_eth_index, vlanId);		
		return IGMPSNP_RETURN_CODE_ERROR;
	}
}


/**********************************************************************************
 *  npd_vlan_port_igmp_endis_flag_set
 *
 *	DESCRIPTION:
 * 		check out igmp port of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		tagmode - tag or untag
 *		en_dis - igmp enable or disable
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		IGMPSNP_RETURN_CODE_OK  - set flag success
 *		IGMPSNP_RETURN_CODE_ERROR - there is no port in the portlist
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - can not find the vlan by vid
 *		IGMPSNP_RETURN_CODE_NULL_PTR - the portlist in vlan not exist
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_igmp_endis_flag_set
(
	unsigned short vlanId,
	unsigned int  eth_g_idx,
	unsigned char tagMode,
	unsigned char  en_dis
)
{
	syslog_ax_vlan_dbg("npd vlan port igmp endis flag set::vlan %d port %d igmp endis flag %d.\n",vlanId,eth_g_idx,en_dis);
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) 
	{
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}
	/* tagged port list*/
	if(NPD_TRUE == tagMode) 
	{
		portList = vlanNode->tagPortList;
	}
	else if(NPD_FALSE == tagMode) 
	{
		portList = vlanNode->untagPortList;
	}

	if(NULL == portList) 
	{
		return IGMPSNP_RETURN_CODE_NULL_PTR;
	}
	else if(0 != portList->count) 
	{				/* code optimize:Declaring variable "isTagged" without initializer houxx@autelan.com  2013-1-17 */
		syslog_ax_vlan_dbg("npd vlan check igmp_endis_flag:: vlan %d %s port count %d\n",vlanId,tagMode ? "tagged":"untagged",portList->count);
		list = &(portList->list);
		__list_for_each(pos,list) 
		{
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if(NULL != node && eth_g_idx == node->eth_global_index) 
			{
				node->igmpEnFlag = en_dis ;
				syslog_ax_vlan_dbg("igmp endis flag  = %d,ok.\n",node->igmpEnFlag);
				return IGMPSNP_RETURN_CODE_OK;
			}
		}
	}

	return IGMPSNP_RETURN_CODE_ERROR;
}

/**********************************************************************************
 *  npd_vlan_port_igmp_endis_flag_check
 *
 *	DESCRIPTION:
 * 		check out igmp port of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *	    tagMode - vlan untag/tag port 
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - 
 *		NPD_FALSE - 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_igmp_endis_flag_check
(
	unsigned short vlanId,
	unsigned int  eth_g_idx,
	unsigned char tagMode,
	unsigned char  *en_dis
)
{
	syslog_ax_vlan_dbg("Enter igmpSnp EnDisflag check::vlan %d %s port %d.\n",vlanId,tagMode?"tag":"untag",eth_g_idx);
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) 
	{
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}
	/* tagged port list*/
	if(NPD_TRUE == tagMode) 
	{
		portList = vlanNode->tagPortList;
	}
	else if(NPD_FALSE == tagMode) 
	{
		portList = vlanNode->untagPortList;
	}

	if(NULL == portList) 
	{
		return IGMPSNP_RETURN_CODE_NULL_PTR;
	}
	else if(0 != portList->count) 
	{			/* code optimize:Declaring variable "isTagged" without initializer houxx@autelan.com  2013-1-17 */
		syslog_ax_vlan_dbg("npd vlan check igmp endis flag:: vlan %d %s port count %d\n",vlanId,tagMode ? "tagged":"untagged",portList->count);
		list = &(portList->list);
		__list_for_each(pos,list) 
		{
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if(NULL != node && eth_g_idx == node->eth_global_index) 
			{
				*en_dis = node->igmpEnFlag ;
				syslog_ax_vlan_dbg("igmp_endis_flag  = %d,ok.\n",*en_dis);
				return IGMPSNP_RETURN_CODE_OK;
			}
		}
	}
	return IGMPSNP_RETURN_CODE_ERROR;
}
/********************************************
 *
 *
 *
 *
 ********************************************/
int npd_vlan_portmbr_igmp_flags_clear(void)
{
	unsigned int i,j;
	struct vlan_s* vlanNode =NULL;
	struct eth_port_s* ethPort = NULL;
	unsigned int eth_g_idx[24] = {0},mbr_count =0;
	unsigned char status = NPD_FALSE,devNum =0,portNum = 0;
	for(i = 1; i<NPD_VLAN_NUMBER_MAX;i++){
		/*i -- vlanId*/
		if( VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i)){
			syslog_ax_vlan_dbg("npd_vlan_portmbr_igmp_flags_clear: vlan %d exists.\n",i);
			vlanNode = npd_find_vlan_by_vid(i);
			if(NULL != vlanNode){
				/*clear vlan igmpSnpEnDis Flag*/
				if( NPD_TRUE == vlanNode->igmpSnpEnDis){
					/*for bug:AU-239, and also nam_asic_igmp_trap_set_by_devport*/
					nam_asic_vlan_igmp_enable_forword_unmc(0,i);

					vlanNode->igmpSnpEnDis = NPD_FALSE;
					syslog_ax_vlan_dbg("npd_vlan_portmbr_igmp_flags_clear: vlan %d vlanNode->igmpSnpEnDis %d.\n",
											i,vlanNode->igmpSnpEnDis);
				}
			}
			else {
				continue;
			}
			if (NPD_TRUE == npd_vlan_member_port_index_get(i,NPD_FALSE,eth_g_idx,&mbr_count)){
				for(j= 0;j<mbr_count;j++){
					/*clear vlan untag port igmp route port flag*/
					npd_vlan_port_igmp_mcrouter_flag_set(i, eth_g_idx[j],NPD_FALSE, NPD_FALSE);

					npd_vlan_port_igmp_endis_flag_set(i,eth_g_idx[j], NPD_FALSE, NPD_FALSE);
					/*find ethPort struct,clear ethPort igmp enDis Flag*/
					ethPort = npd_get_port_by_index(eth_g_idx[j]);
					if(NULL != ethPort){	
						npd_check_igmp_snp_port_status(eth_g_idx[j],&status);
						if(status)
							ethPort->funcs.funcs_run_bitmap ^= 1<<ETH_PORT_FUNC_IGMP_SNP;
					}
					/*for bug:AU-239*/
					/*set igmp trap disable on port(enabled igmp)*/
					npd_get_devport_by_global_index(eth_g_idx[j],&devNum,&portNum);
					nam_asic_igmp_trap_set_by_devport(i, devNum, portNum, 0);
				}
			}
			if (NPD_TRUE == npd_vlan_member_port_index_get(i,NPD_TRUE,eth_g_idx,&mbr_count)){
				for(j= 0;j<mbr_count;j++){
					/*clear vlan tag port igmp route port flag*/
					npd_vlan_port_igmp_mcrouter_flag_set(i, eth_g_idx[j],NPD_TRUE, NPD_FALSE);

					npd_vlan_port_igmp_endis_flag_set(i,eth_g_idx[j], NPD_TRUE, NPD_FALSE);
					/*find ethPort struct,clear ethPort igmp enDis Flag*/
					ethPort = npd_get_port_by_index(eth_g_idx[j]);
					if(NULL != ethPort){
						npd_check_igmp_snp_port_status(eth_g_idx[j],&status);
						if(status)
							ethPort->funcs.funcs_run_bitmap ^= 1<<ETH_PORT_FUNC_IGMP_SNP;
					}
					/*for bug:AU-239*/
					/*set igmp trap disable on port(enabled igmp)*/
					npd_get_devport_by_global_index(eth_g_idx[j],&devNum,&portNum);
					nam_asic_igmp_trap_set_by_devport(i, devNum, portNum, 0);
				}
			}
		}
		else {
			continue;
		}
	}
	return 0;
}

/**********************************************************************************
 *
 * change igmpsnp port memebers and fdb entry by port link status
 *
 *	INPUT:
 *		eth_g_index - port global index
 *		event - port link status
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS
 *		NPD_FAIL
 *		
 *
 **********************************************************************************/
int npd_igmpsnp_port_link_change
(
	unsigned int eth_g_index,
	struct port_based_igmpsnp_s *portIgmpSnp,
	enum ETH_PORT_NOTIFIER_ENT event
)
{
	
	if(ETH_PORT_NOTIFIER_LINKDOWN_E == event){
		npd_igmp_port_link_change(eth_g_index,event);
	}
	else if(ETH_PORT_NOTIFIER_LINKUP_E == event){
		npd_igmp_port_link_change(eth_g_index,event);
	}
	
	return NPD_SUCCESS;
}
#endif

/**********************************************************************************
 *
 * change vlan status down if all ports down in the vlan
 *
 *
 *	INPUT:
 *		vid - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS
 *		NPD_FAIL
 *		
 **********************************************************************************/
int npd_modify_vlan_status_by_portdown
(
	unsigned short vid
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	int tagCount = 0,untagCount = 0;
	unsigned int  isLast = NPD_FALSE;

	/*syslog_ax_vlan_dbg("npd_modify_vlan_status_by_portdown:: vid%d ",vid);*/
	vlanNode = npd_find_vlan_by_vid(vid);
	if(vlanNode){
		/*check untag ports*/
		 syslog_ax_vlan_dbg("npd check untag ports\n");
		portList = vlanNode->untagPortList;
		if(NULL != portList) {
			 /*syslog_ax_vlan_dbg("npd_modify_vlan_status_by_portdown:: into untag portList count %d",portList->count);*/
			list = &(portList->list);
			__list_for_each(pos,list) {
				node = list_entry(pos,struct vlan_port_list_node_s,list);
				if(node) {
					untagCount++;
					/* syslog_ax_vlan_dbg("npd_modify_vlan_status_by_portdown:: vlanNode vid %d, index %d ,untagcount %d\n", 	\*/
					/*	vid,node->eth_global_index,untagCount);*/
					if(ETH_ATTR_LINKUP == npd_check_eth_port_status(node->eth_global_index)) {
						isLast = NPD_TRUE;
						break;						
					}
				}
			}
		}
		/*check tag ports*/
		/* syslog_ax_vlan_dbg("npd check tag ports");*/
		portList = vlanNode->tagPortList;
		if((NPD_FALSE == isLast) && (NULL != portList)) {
			 /*syslog_ax_vlan_dbg("npd_modify_vlan_status_by_portdown:: into tag portList count %d",portList->count);*/
			list = &(portList->list);
			__list_for_each(pos,list) {
				node = list_entry(pos,struct vlan_port_list_node_s,list);
				if(node){
					tagCount++;
					/* syslog_ax_vlan_dbg("npd_modify_vlan_status_by_portdown:: vlanNode vid %d, index %d,tagcount %d\n",	\
						vid,node->eth_global_index,tagCount);*/
					if(ETH_ATTR_LINKUP == npd_check_eth_port_status(node->eth_global_index)) {
						isLast = NPD_TRUE;
						break;						
					}
				}
			}
		}

		if(!isLast){
			vlanNode->brgVlan.state = VLAN_STATE_DOWN_E;
			/*syslog_ax_vlan_dbg("npd_modify_vlan_status_by_portdown:: vlan  %d DOWN",vid);*/
			npd_intf_set_vlan_l3intf_status(vid,VLAN_STATE_DOWN_E);
		}
	}
	else{
		 /*syslog_ax_vlan_dbg("npd_modify_vlan_status_by_portdown:: input vid %d don't find vlan",vid);*/
		return NPD_FAIL;
	}

	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_vlan_change_status_by_port_statuss
 *
 *	DESCRIPTION:
 * 		check out if a port is an untagged or tagged port member of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port member
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_change_status_by_port_status
(
	unsigned short vid,
	unsigned int eth_g_index,
	unsigned int isadd
)
{
	struct vlan_s* vlanInfo = NULL;
	struct eth_port_s* portInfo = NULL;
	int ret = 0;
	unsigned int ifindex = ~0UI;

	vlanInfo = npd_find_vlan_by_vid(vid);
	if(NULL == vlanInfo) {
		NPD_ERROR(("vlan %d not exist\n",vid));
		return NPD_DBUS_ERROR;
	}

	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		NPD_ERROR(("port %d not exist\n",eth_g_index));
		return NPD_DBUS_ERROR;
	}

	if(VLAN_STATE_UP_E == vlanInfo->brgVlan.state) {
		/*only take care of delete port*/
		if(NPD_FALSE == isadd) {
			npd_modify_vlan_status_by_portdown(vid);
		}

	}
	else if(VLAN_STATE_DOWN_E == vlanInfo->brgVlan.state) {
		/*only take care of add port*/
		if(NPD_TRUE == isadd) {
			if(ETH_ATTR_LINKUP == npd_check_eth_port_status(eth_g_index)) {
				vlanInfo->brgVlan.state = VLAN_STATE_UP_E;
				npd_intf_set_vlan_l3intf_status(vid,VLAN_STATE_UP_E);
				if((NPD_TRUE == npd_vlan_interface_check(vid,&ifindex))&&(~0UI != ifindex)){
					ret = npd_arp_snooping_gratuitous_send(ifindex,vid,0,KAP_INTERFACE_VID_E);
					npd_syslog_dbg("vlan interface add up port gratuitous arp send %s,ret %d \n",(0==ret)?"SUCCESS":"FAILED",ret);
					ret = NPD_SUCCESS;
				}
			}
		}
	}

	return ret;
}

/**********************************************************************************
 *  npd_vlan_member_port_index_get
 *
 *	DESCRIPTION:
 * 		check out if a port is an untagged or tagged port member of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port member
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_member_port_index_get
(
	unsigned short	vlanId,
	unsigned char	isTagged,
	unsigned int	eth_g_index[],
	unsigned int	*mbr_count
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int mbrCnt = 0;
	vlanNode = npd_find_vlan_by_vid(vlanId);
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
		/* syslog_ax_vlan_dbg("npd vlan check port:: vlan %d %s port count %d",vlanId,isTagged ? "tagged":"untagged",portList->count);*/
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			eth_g_index[mbrCnt] = node->eth_global_index;
			mbrCnt++;
		}
	}
	*mbr_count = mbrCnt;
	return NPD_TRUE;
}

unsigned int npd_vlan_get_mbr_trunk_id
(
	unsigned short vid,
	unsigned char  tagMode,
	unsigned short **trunkId,
	unsigned int   *mbr_count
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_trunk_list_s * vlanTrunkList = NULL;
	struct vlan_trunk_list_node_s *trunkMbr = NULL;
	struct list_head	*list = NULL, *pos = NULL;
	unsigned int mbrCnt = 0;
	syslog_ax_vlan_dbg("Enter npd_vlan_get_mbr_trunk_id:vlanId %d.\n",vid);
	vlanNode = npd_find_vlan_by_vid(vid);
	if(NULL == vlanNode) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
	}
	if(NPD_TRUE == tagMode) {
		vlanTrunkList = vlanNode->tagTrunkList;
	}
	else if(NPD_FALSE == tagMode) {
		vlanTrunkList = vlanNode->unTagtrunkList;
	}
	if (NULL == vlanTrunkList){
		return VLAN_RETURN_CODE_TRUNK_NOTEXISTS;
	}
	else if(0 != vlanTrunkList->count) {
		syslog_ax_vlan_dbg("vlan %d %s trunkmbr count %d\n",vid,(tagMode)?"tag":"untagged\n",vlanTrunkList->count);
		list = &(vlanTrunkList->list);
		__list_for_each(pos,list) {
			trunkMbr = list_entry(pos,struct vlan_trunk_list_node_s,list);
			(*trunkId)[mbrCnt] = trunkMbr->trunkId;
			mbrCnt++;
		}
	}
	*mbr_count = mbrCnt;
	return NPD_TRUE;
}

/**********************************************************************************
 *  npd_vlan_check_contain_port
 *
 *	DESCRIPTION:
 * 		check out if a port is an untagged or tagged port member of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		isTagged - ethernet port is untagged or tagged port member of vlan
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_check_contain_port
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  *isTagged
)
{
	if(NPD_TRUE == npd_vlan_check_port_membership(vlanId,eth_g_index,NPD_FALSE)) {
		*isTagged = NPD_FALSE;
		return NPD_TRUE;
	}
	else if(NPD_TRUE == npd_vlan_check_port_membership(vlanId,eth_g_index,NPD_TRUE)) {
		*isTagged = NPD_TRUE;
		return NPD_TRUE;
	}
	else
		return NPD_FALSE;
}

/**********************************************************************************
 *  npd_vlan_check_trunk_membership
 *
 *	DESCRIPTION:
 * 		check out if a trunk is an untagged or tagged trunk member of specified vlan
 *
 *	INPUT:
 *		trunkId - trunk id
 *		vlanId -- vlan id
 *		isTagged - untagged or tagged port member
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if trunk is a member of vlan
 *		NPD_FALSE - if trunk is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_check_trunk_membership
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned char  isTagged
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_trunk_list_s *tmpTrunkList = NULL;
	struct vlan_trunk_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		return NPD_FALSE;
	}
	/* syslog_ax_vlan_dbg("find vlanNode addr %p", vlanNode);
 	// syslog_ax_vlan_dbg("find vlanNode->trunkList addr %0x",vlanNode->trunkList);
	// syslog_ax_vlan_dbg("find vlanNode->trunkList->count = %d", vlanNode->trunkList->count);lead to kill*/
	if(NPD_TRUE == isTagged){
		tmpTrunkList = vlanNode->tagTrunkList;
	}
	else if(NPD_FALSE == isTagged){
		tmpTrunkList = vlanNode->unTagtrunkList;
	}
	if(NULL == tmpTrunkList) {
		return NPD_FALSE;
	}
	else if(0 != tmpTrunkList->count) {
		syslog_ax_vlan_dbg("vlan %d trunk mbrs count %d.\n",vlanId,tmpTrunkList->count);
		list = &(tmpTrunkList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_trunk_list_node_s,list);
			if(NULL != node && trunkId == node->trunkId) {
				syslog_ax_vlan_dbg("trunk %d in vlan %d.\n",trunkId,vlanId);
				return NPD_TRUE;
			}
		}
	}
	syslog_ax_vlan_dbg("trunk %d NOT member of vlan %d!\n",trunkId,vlanId);
	return NPD_FALSE;
}

/**********************************************************************************
 *  npd_vlan_check_contain_trunk
 *
 *	DESCRIPTION:
 * 		check out if a trunk is an untagged or tagged member of specified vlan
 *
 *	INPUT:
 *		trunkId - trunk id
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		isTagged - trunk is untagged or tagged port member of vlan
 *
 * 	RETURN:
 *		NPD_TRUE  - if trunk is a member of vlan
 *		NPD_FALSE - if trunk is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_check_contain_trunk
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned char  *isTagged
)
{
	/* syslog_ax_vlan_dbg("Enter Npd: npd_vlan_check_contain_trunk...");*/
	
	if(NPD_TRUE == npd_vlan_check_trunk_membership(vlanId,trunkId,NPD_FALSE)) {
		*isTagged = NPD_FALSE;
		return NPD_TRUE;
	}
	else if(NPD_TRUE == npd_vlan_check_trunk_membership(vlanId,trunkId,NPD_TRUE)) {
		*isTagged = NPD_TRUE;
		return NPD_TRUE;
	}
	else
		return NPD_FALSE;
}

/**********************************************************************************
 *  npd_vlan_get_all_ports
 *
 *	DESCRIPTION:
 * 		get all ports in the vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		untagPorts - untag ports in the vlan
 *     tagPorts - tag ports in the vlan
 *
 * 	RETURN:
 *			
 *			NPD_SUCCESS
 *			NPD_FAIL
 *
 **********************************************************************************/
int npd_vlan_get_all_ports
(
	unsigned short vlanId,
	struct vlan_ports* untagPorts,
	struct vlan_ports* tagPorts
)
{
	struct vlan_s* node = NULL;
	struct vlan_port_list_node_s *portNode = NULL;
	vlan_port_list_s* portList = NULL;
	struct list_head *pos = NULL,*list  = NULL;

	int i = 0,ret = 0;
	untagPorts->count = 0;
	tagPorts->count = 0;
	node = npd_find_vlan_by_vid(vlanId);
	if(NULL == node)
	{
		/* syslog_ax_vlan_dbg("npd_lookup_ports_by_vlanid:: don't find vlan %d  node \n",vlanId);*/
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
	}

	portList = node->untagPortList;
	if(NULL != portList)
	{
		/* syslog_ax_vlan_dbg("npd_lookup_ports_by_vlanid :: into the untag ports list\n");*/
		if(0 != portList->count) {
			/* syslog_ax_vlan_dbg("npd vlan trans untag ports:: vlan %d  port count %d",vlanId,portList->count);*/
			untagPorts->count = portList->count;
			i = 0;
			list = &(portList->list);
			__list_for_each(pos,list) {
				portNode = list_entry(pos,struct vlan_port_list_node_s,list);
				untagPorts->ports[i++] = portNode->eth_global_index;
			}
		}
	}

	portList = node->tagPortList;
	if(NULL != portList)
	{
		/* syslog_ax_vlan_dbg("npd_lookup_ports_by_vlanid :: into the tag ports list\n");*/
		if(0 != portList->count) {
			/* syslog_ax_vlan_dbg("npd vlan trans tag ports:: vlan %d port count %d",vlanId,portList->count);*/
			tagPorts->count = portList->count;
			i = 0;
			list = &(portList->list);
			__list_for_each(pos,list) {
				portNode = list_entry(pos,struct vlan_port_list_node_s,list);
				tagPorts->ports[i++] = portNode->eth_global_index;
			}
		}
	}

	return NPD_SUCCESS;
	
}

/**********************************************************************************
 *  npd_vlan_get_all_active_ports
 *
 *	DESCRIPTION:
 * 		get all ports in the vlan with valid STP/MSTP state( in Forwarding state)
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		untagPorts - untag ports in the vlan
 *     	tagPorts - tag ports in the vlan
 *
 * 	RETURN:
 *			
 *			NPD_SUCCESS
 *			NPD_FAIL
 *
 **********************************************************************************/
int npd_vlan_get_all_active_ports
(
	unsigned short vlanId,
	struct vlan_ports* untagPorts,
	struct vlan_ports* tagPorts
)
{
	struct vlan_s* node = NULL;
	struct vlan_port_list_node_s *portNode = NULL;
	vlan_port_list_s* portList = NULL;
	struct list_head *pos = NULL,*list  = NULL;
	RSTP_PORT_STATE_E  state = CPSS_RSTP_PORT_STATE_DISABLE_E;

	int i = 0,ret = 0;
	untagPorts->count = 0;
	tagPorts->count = 0;
	node = npd_find_vlan_by_vid(vlanId);
	if(NULL == node)
	{
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
	}

	portList = node->untagPortList;
	if(NULL != portList)
	{
		if(0 != portList->count) {
			untagPorts->count = portList->count;
			i = 0;
			list = &(portList->list);
			__list_for_each(pos,list) {
				portNode = list_entry(pos,struct vlan_port_list_node_s,list);

				/* add by jinpc@autelan.com*/
				ret = npd_check_eth_port_status(portNode->eth_global_index);
				if(ret <= 0) 
					continue; /* skip eth-port link down*/

				ret = npd_mstp_get_port_state(vlanId, portNode->eth_global_index,&state);
				if(NPD_SUCCESS == ret){
					if((CPSS_RSTP_PORT_STATE_DISABLE_E == state)||
						(CPSS_RSTP_PORT_STATE_FORWARD_E == state)) {
						untagPorts->ports[i++] = portNode->eth_global_index;
					}
				}
			}
			untagPorts->count = i;
		}
	}

	portList = node->tagPortList;
	if(NULL != portList)
	{
		if(0 != portList->count) {
			tagPorts->count = portList->count;
			i = 0;
			list = &(portList->list);
			__list_for_each(pos,list) {
				portNode = list_entry(pos,struct vlan_port_list_node_s,list);

				/* add by jinpc@autelan.com*/
				ret = npd_check_eth_port_status(portNode->eth_global_index);
				if(ret <= 0) 
					continue; /* skip eth-port link down*/

				ret = npd_mstp_get_port_state(vlanId, portNode->eth_global_index,&state);
				if(NPD_SUCCESS == ret){
					if((CPSS_RSTP_PORT_STATE_DISABLE_E == state)||
						(CPSS_RSTP_PORT_STATE_FORWARD_E == state)) {
						tagPorts->ports[i++] = portNode->eth_global_index;
					}
				}
			}
			tagPorts->count = i;
		}
	}

	return NPD_SUCCESS;
	
}

/**********************************************************************************
 *  npd_vlan_port_list_addto
 *
 *	DESCRIPTION:
 * 		add port to vlan port list
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_list_addto
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char isTagged
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *node = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode)
	{
		return NPD_FALSE;
	}
	
	/* tagged port list*/
	if(NPD_TRUE == isTagged) 
	{
		portList = vlanNode->tagPortList;
	}
	else if(NPD_FALSE == isTagged) 
	{
		portList = vlanNode->untagPortList;
	}
	
	if(NULL == portList)
	{ /* create port list */
		/* syslog_ax_vlan_dbg("npd vlan %d add first %s port %#0x",vlanId,isTagged ? "tag":"untag",eth_g_index);*/
		portList = (vlan_port_list_s *)malloc(sizeof(vlan_port_list_s));
		if(NULL == portList) 
		{
			 syslog_ax_vlan_err("npd vlan %d add first %s port %#0x memory err\n",	
					vlanId,isTagged ? "tag":"untag",eth_g_index);
			return NPD_FALSE;
		}
		else 
		{
			memset(portList, 0, sizeof(vlan_port_list_s));
			INIT_LIST_HEAD(&(portList->list));
			if(NPD_TRUE == isTagged) 
			{
				vlanNode->tagPortList = portList;
			}
			else if(NPD_FALSE == isTagged)
			{
				vlanNode->untagPortList = portList;
			}
		}
	}

	node = (struct vlan_port_list_node_s *)malloc(sizeof(struct vlan_port_list_node_s));
	if(NULL == node) 
	{
		 syslog_ax_vlan_err("npd vlan %d add port %#0x to %s list error:null memory alloc\n", 
					vlanId, eth_g_index, isTagged ? "tag":"untag");
		return NPD_FALSE;
	}
	memset(node,0,sizeof(struct vlan_port_list_node_s));

	/* must init the attrubute about dhcp-snooping of this port,
	 * if the vlan already enable dhcp-snooping
	 */
	if (NPD_DHCP_SNP_ENABLE == vlanNode->dhcpSnpEnDis) 
	{
		/* DHCP-Snooping trust mode of port, default is notrust 				*/
		node->dhcp_snp_info.trust_mode = NPD_DHCP_SNP_PORT_MODE_NOTRUST;
		/* DHCP-Snooping option82 strategy type of port, default is replace 	*/
		node->dhcp_snp_info.opt82_strategy = NPD_DHCP_SNP_OPT82_STRATEGY_TYPE_REPLACE;
		/* DHCP-Snooping option82 circuit-id of port, default is vlan +portindex	*/
		node->dhcp_snp_info.opt82_circuitid = NPD_DHCP_SNP_OPT82_CIRCUITID_TYPE_DEFAULT;
		memset(node->dhcp_snp_info.opt82_circuitid_str, 0, NPD_DHCP_SNP_CIRCUITID_STR_LEN);
		/* DHCP-Snooping option82 remote-id of port, default is system mac		*/
		node->dhcp_snp_info.opt82_remoteid = NPD_DHCP_SNP_OPT82_REMOTEID_TYPE_SYSMAC;
		memset(node->dhcp_snp_info.opt82_remoteid_str, 0, NPD_DHCP_SNP_REMOTEID_STR_LEN);
		syslog_ax_vlan_dbg("npd vlan %d support dhcp-snooping, init the attrubute about dhcp-snooping on %s port %#0x\n", 
							vlanId, isTagged ? "tag" : "untag", eth_g_index);
	}

	node->eth_global_index = eth_g_index;
	list_add(&(node->list),&(portList->list));
	portList->count++;

	/* for distributed system, update the g_vlanlist[] for not cscd port */
	if(SYSTEM_TYPE == IS_DISTRIBUTED)
	{
    	unsigned char slot_no = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index)+1;
    	unsigned char local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index) + asic_board->asic_port_start_no;
    	unsigned int  ret = VLAN_RETURN_CODE_ERR_NONE;
		int i=0, asic_port=0;
        syslog_ax_vlan_dbg("######slot_no:%d \n",slot_no );
        syslog_ax_vlan_dbg("######local_port_no:%d \n",local_port_no );

		/* if is cscd port, jump out, need not update g_vlanlist[] */
		/* cscd port from npd_vlan_cscd_add() to here */
		asic_port = asic_board->asic_eth_ports[local_port_no - asic_board->asic_port_start_no].port_num;
        for(i=0; i<asic_board->asic_cscd_port_cnt; i++)
        {
        	if(asic_port == asic_board->asic_cscd_ports[i].port_num)
        	{
			    syslog_ax_vlan_dbg("$$$$$$$$$$$$$ add cscd port:%d to vlan:%d , need not update g_vlanlist .\n",asic_port,vlanId);	
            	return NPD_TRUE;				
        	}
        }

		/* port_no >32 */
        if(local_port_no>32)
        {
    		if(isTagged ==1)
    		{
    			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].high_bmp |= (unsigned int)(1<<(local_port_no-33));
    		}
    		else
    		{
    			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].high_bmp |= (unsigned int)(1<<(local_port_no-33));
    		}      					
        }
		else
		{
    		if(isTagged ==1)
    		{
    			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));
    		}
    		else
    		{
    			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));
    		}      					
			
		}    

    	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
    	if( ret!=0 )
        {
            syslog_ax_vlan_err("msync shm_vlan failed \n" );
			return NPD_FALSE;
        }
	    syslog_ax_vlan_dbg("######add port to vlan:%d , update g_vlanlist OK .\n",vlanId);
		
	}

	return NPD_TRUE;
}

/**********************************************************************************
 *  npd_vlan_port_list_delfrom
 *
 *	DESCRIPTION:
 * 		delete a port from vlan port list
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port list
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is a member of vlan
 *		NPD_FALSE - if port is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_list_delfrom
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  isTagged
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int delflag = 0;
	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		return NPD_FALSE;
	}
	/* syslog_ax_vlan_dbg("find vlanNode of vlan %d!",vlanId);*/
	/* tagged port list*/
	if(NPD_TRUE == isTagged) {
		portList = vlanNode->tagPortList;
	}
	else if(NPD_FALSE == isTagged) {
		portList = vlanNode->untagPortList;
	}
	
	if(NULL == portList) {
		 syslog_ax_vlan_err("npd del %s port %#0x from vlan %d null port list err!\n",	
						isTagged ? "tag":"untag", eth_g_index, vlanId);
		return NPD_FALSE;
	}
	else {	
		list = &(portList->list);
		if(NULL ==list){
			/* syslog_ax_vlan_dbg("No member in vlan,NULL ==portList->list !");*/
			return NPD_FALSE;
		}
		__list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if( NULL != node && eth_g_index == node->eth_global_index) 
			{
				list_del(&(node->list));
				delflag= 1;
				break;
			}
		}
		if(NULL != node && 1 == delflag){
			free(node);
			node = NULL;
			if(portList->count > 0) {
				portList->count--;
			}
		}
		/*
		 syslog_ax_vlan_dbg("npd vlan %d current %s port node count %d",	\
						vlanId, isTagged ? "tag":"untag",portList->count);
		*/
		if(0 == portList->count) {
			if(NULL != portList){
				free(portList);
				portList = NULL;
			}
			if(NPD_TRUE == isTagged) {
				vlanNode->tagPortList = NULL;
			}
			else if(NPD_FALSE == isTagged) {
				vlanNode->untagPortList = NULL;
			}
			
		}
	}
	npd_arp_clear_by_port_vid(eth_g_index,vlanId);

	/* for distributed system, update the g_vlanlist[] for not cscd port */
	if(SYSTEM_TYPE == IS_DISTRIBUTED)
	{
    	unsigned char slot_no = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index)+1;
    	unsigned char local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index) + asic_board->asic_port_start_no;
    	unsigned int  ret = VLAN_RETURN_CODE_ERR_NONE;
		int i=0, asic_port=0;
        syslog_ax_vlan_dbg("######slot_no:%d \n",slot_no );
        syslog_ax_vlan_dbg("######local_port_no:%d \n",local_port_no );

		/* if is cscd port, jump out, need not update g_vlanlist[] */
		/* cscd port from npd_vlan_cscd_del() to here */
		asic_port = asic_board->asic_eth_ports[local_port_no - asic_board->asic_port_start_no].port_num;
        for(i=0; i<asic_board->asic_cscd_port_cnt; i++)
        {
        	if(asic_port == asic_board->asic_cscd_ports[i].port_num)
        	{
			    syslog_ax_vlan_dbg("$$$$$$$$$$$$$ del cscd port:%d from vlan:%d , need not update g_vlanlist .\n",asic_port,vlanId);	
            	return NPD_TRUE;				
        	}
        }

		/* port_no >32 */
        if(local_port_no>32)
        {
    		if(isTagged ==1)    /* delete tag port from vlan list */
    		{
    			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
    		}
    		else    /* delete untag port from vlan list */
    		{
    			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
    		}      					
        }
		else
		{
    		if(isTagged ==1)
    		{
    			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
    		}
    		else
    		{
    			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
    		}      					
			
		}

    	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
    	if( ret!=0 )
        {
            syslog_ax_vlan_err("msync shm_vlan failed \n" );
			return NPD_FALSE;
        }
	    syslog_ax_vlan_dbg("######del port from vlan:%d , update g_vlanlist OK .\n",vlanId);
	
	}
	return NPD_TRUE;
}
/**********************************************************************************
 *  npd_vlan_port_member_add
 *
 *	DESCRIPTION:
 * 		add port to vlan as untagged or tagged.
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE  - if no error occurred
 *		NPD_VLAN_PORT_EXISTS - if port is already a member of vlan
 *		...
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_member_add
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  isTagged	
)
{
	unsigned int retval = VLAN_RETURN_CODE_ERR_NONE;
	const unsigned short vlan_id_default = 1;
	unsigned int slot_index = 0,slot_no = 0,port_index = 0,port_no = 0;
    unsigned int fdblimit = 0;

	slot_index = npd_get_slot_index_from_eth_index(eth_g_index);
	slot_no = npd_get_slot_no_from_index(slot_index);
	port_index = npd_get_port_index_from_eth_index(eth_g_index);
	port_no = npd_get_port_no_from_index(slot_index,port_index);
	if(NPD_TRUE == isTagged) { /* add port to vlan's  tagged port list */
		if(NPD_TRUE == npd_vlan_check_port_membership(vlanId,eth_g_index,NPD_TRUE)) {
			retval = VLAN_RETURN_CODE_PORT_EXISTS;
		}
		else {
			/* TODO:add port to vlan tagged port list*/
			retval = npd_vlan_port_list_addto(vlanId,eth_g_index,NPD_TRUE);
			if(NPD_TRUE == retval) {
				retval = VLAN_RETURN_CODE_ERR_NONE;
			}
			npd_eth_port_dot1q_add(eth_g_index,vlanId);
		}
	}
	else if(NPD_FALSE == isTagged) { /* add port to vlan's untagged port list*/
		if(NPD_TRUE == npd_vlan_check_port_membership(vlanId,eth_g_index,NPD_FALSE)) {
			retval = VLAN_RETURN_CODE_PORT_EXISTS;
		}
		else {
			/* TODO:add port to vlan untagged port list*/
			retval = npd_vlan_port_list_addto(vlanId,eth_g_index,NPD_FALSE);
			if(NPD_TRUE == retval) {
				retval = VLAN_RETURN_CODE_ERR_NONE;
			}
			else{
				 syslog_ax_vlan_err("untag port %d return add to default vlan port list Error!\n",port_no);
				return VLAN_RETURN_CODE_ERR_MAX;/*dcli not process this error code*/
			}
			/*npd_eth_port_based_vlan_add(eth_g_index,vlanId);*/
			if(NPD_SUCCESS != npd_eth_port_set_pvid(eth_g_index,vlanId)){
				syslog_ax_vlan_err("set ethport %d pvid error.\n",port_no);
			}
			/* TODO:delete port from default vlan untag port list*/
			if(vlanId != vlan_id_default){
				/*
				 syslog_ax_vlan_dbg("untag port %d Delete from vlan %d port list!\r\n",\
											eth_g_index,vlan_id_default);
				*/
				npd_vlan_port_list_delfrom(vlan_id_default, eth_g_index, NPD_FALSE);
				/*by zhubo@autelan.com 2008.7.25*/
				/*npd_eth_port_clear_pvid(eth_g_index);*/
				/*npd_eth_port_based_vlan_del(eth_g_index, vlan_id_default);*/
				npd_vlan_change_status_by_port_status(vlan_id_default,eth_g_index,NPD_FALSE);
			}
		}
	}

	/* set the vlan NA enable if port is fdb limit*/
	if(npd_fdb_number_port_set_check(slot_no,port_no,&fdblimit)){
        npd_fdb_na_msg_per_vlan_set(vlanId,1,0);
	}

	/* set the port NA enable if vlan is fdb limit*/
	if( npd_fdb_number_vlan_set_check(vlanId,&fdblimit)){
        npd_fdb_na_msg_per_port_set(slot_no,port_no,1);
	}
	/*change vlan state*/
	npd_vlan_change_status_by_port_status(vlanId,eth_g_index,NPD_TRUE);
	return retval;
}

/**********************************************************************************
 *  npd_vlan_port_member_del
 *
 *	DESCRIPTION:
 * 		delete port from vlan as untagged or tagged.
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE  - if no error occurred
 *		NPD_VLAN_PORT_EXISTS - if port is not a member of vlan
 *		...
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_port_member_del
(
	unsigned short vlanId,
	unsigned int   eth_g_index,
	unsigned char  isTagged	
)
{
	unsigned int retval = VLAN_RETURN_CODE_ERR_NONE;
	unsigned short vlan_id_default = 1;
	if(NPD_TRUE == isTagged) { /* delete port from vlan's  tagged port list */
		if(NPD_FALSE == npd_vlan_check_port_membership(vlanId,eth_g_index,NPD_TRUE)) {
			retval = VLAN_RETURN_CODE_PORT_NOTEXISTS;
		}
		else {
			/* TODO:delete port from vlan tagged port list*/
			retval = npd_vlan_port_list_delfrom(vlanId,eth_g_index,NPD_TRUE);
			if(NPD_TRUE == retval) {
				retval = VLAN_RETURN_CODE_ERR_NONE;
				npd_eth_port_dot1q_remove(eth_g_index,vlanId);
			}
		}
	}
	else if(NPD_FALSE == isTagged) { /* delete port from vlan's untagged port list*/
		if(NPD_FALSE == npd_vlan_check_port_membership(vlanId,eth_g_index,NPD_FALSE)) {
			retval =  VLAN_RETURN_CODE_PORT_NOTEXISTS;
		}
		else {
			/* TODO:delete port from vlan untagged port list*/
			retval = npd_vlan_port_list_delfrom(vlanId,eth_g_index,NPD_FALSE);
			if(NPD_TRUE == retval) {
				retval = VLAN_RETURN_CODE_ERR_NONE;
			}
			npd_eth_port_based_vlan_del(eth_g_index,vlanId);
			if(vlan_id_default != vlanId){
				#if 0
				npd_vlan_port_list_addto(vlan_id_default, eth_g_index, NPD_FALSE);
				//npd_eth_port_based_vlan_add(eth_g_index, vlan_id_default);
				npd_eth_port_set_pvid(eth_g_index, vlan_id_default);
				#endif
				npd_vlan_port_member_add(vlan_id_default,eth_g_index,isTagged);
			}
		}
	}

	/*change vlan state*/
	npd_vlan_change_status_by_port_status(vlanId,eth_g_index,NPD_FALSE);
	return retval;
}
/**********************************
*
*
**********************************/
int npd_vlan_port_return_default_vlan
(
	unsigned int eth_g_index
)
{
	unsigned int i,ret = NPD_FALSE;
	
	for(i =2;i<NPD_VLAN_NUMBER_MAX;i++) {
		/* syslog_ax_vlan_dbg("check port index %d is member of vlan %d or NOT.",eth_g_index,i);*/
		if (VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i)) {
			/* syslog_ax_vlan_dbg("vlan %d exists.",i);*/
			ret = npd_vlan_check_port_membership(i, eth_g_index, NPD_FALSE);
			if(NPD_TRUE == ret) {
				/* syslog_ax_vlan_dbg("eth_port index %d Untagged member of vlan %d",eth_g_index,i);*/
				break;/*find out the ETH-port is untag port of vlan<i>.*/
			}
		}
		else {
			continue;
		}
	}
	return ret;
}
#if 1
/**********************************************************************************
 *  npd_vlan_trunk_list_addto
 *
 *	DESCRIPTION:
 * 		add trunk to vlan port list
 *
 *	INPUT:
 *		vlanId - vlan id
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if trunk is a member of vlan
 *		NPD_FALSE - if trunk is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_trunk_list_addto
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned char	tagMode
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_trunk_list_s *trunkList = NULL;
	struct vlan_trunk_list_node_s *node = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) 
	{
		return NPD_FALSE;
	}
	if(NPD_FALSE == tagMode)
	{
		trunkList = vlanNode->unTagtrunkList;
	}
	else if(NPD_TRUE == tagMode)
	{
		trunkList = vlanNode->tagTrunkList;
	}
	if(NULL == trunkList) 
	{ 		/* create trunk list */
		/* syslog_ax_vlan_dbg("npd vlan %d add first trunk %d",vlanId,trunkId);*/
		trunkList = (vlan_trunk_list_s *)malloc(sizeof(vlan_trunk_list_s));
		if(NULL == trunkList) 
		{
			 syslog_ax_vlan_err("npd vlan %d add first trunk %#0x memory err\n",	vlanId,trunkId);
			 return NPD_FALSE;
		}
		else
		{
			memset(trunkList, 0, sizeof(vlan_trunk_list_s));
			INIT_LIST_HEAD(&(trunkList->list));
			if(NPD_FALSE == tagMode)
			{
				vlanNode->unTagtrunkList = trunkList;
			}
			else if(NPD_TRUE == tagMode)
			{
				vlanNode->tagTrunkList = trunkList;
			}
			syslog_ax_vlan_dbg("vlan %d %s trunklist %p\n",vlanId,tagMode?"tag":"untag",(tagMode ? (vlanNode->tagTrunkList):(vlanNode->unTagtrunkList)));
		}
	}

	node = (struct vlan_trunk_list_node_s *)malloc(sizeof(struct vlan_trunk_list_node_s));
	if(NULL == node) 
	{
		 syslog_ax_vlan_err("npd vlan %d add trunk %#0x list error:null memory alloc\n",vlanId,trunkId);
		return NPD_FALSE;
	}
	memset(node,0,sizeof(struct vlan_trunk_list_node_s));
	node->trunkId = trunkId;
	list_add(&(node->list),&(trunkList->list));
	trunkList->count++;
	syslog_ax_vlan_dbg("npd vlan %d current trunk node count %d\n",vlanId,trunkList->count);

	return NPD_TRUE;
}
/**********************************************************************************
 *  npd_vlan_trunk_list_delfrom
 *
 *	DESCRIPTION:
 * 		delete a trunk from vlan port list
 *
 *	INPUT:
 *		vlanId - vlan id
 *		trunkId -- trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if trunk is a member of vlan
 *		NPD_FALSE - if trunk is not a member of vlan
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_trunk_list_delfrom
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned char	tagMode
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_trunk_list_s *trunkList = NULL;
	struct vlan_trunk_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int delflag = 0;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		return NPD_FALSE;
	}

	if(NPD_FALSE == tagMode ){
		trunkList = vlanNode->unTagtrunkList;
	}
	else if(NPD_TRUE == tagMode){
		trunkList = vlanNode->tagTrunkList;
	}
	
	if(NULL == trunkList) {
		 syslog_ax_vlan_err("npd del trunk %#0x from vlan %d null trunk list err!\n",	\
						trunkId, vlanId);
		return NPD_FALSE;
	}
	else {	
		list = &(trunkList->list);
		if(NULL ==list){
			/* syslog_ax_vlan_dbg("No trunk member in vlan,NULL ==trunkList->list !");*/
			return NPD_FALSE;
		}
		__list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_trunk_list_node_s,list);
			if(NULL != node && trunkId == node->trunkId) {
				list_del(&(node->list));
				delflag = 1;
				break;
			}
		}
		if(NULL != node && 1 == delflag){
			free(node);
			node = NULL;
			if(trunkList->count > 0) {
				trunkList->count--;
			}
		}
		syslog_ax_vlan_dbg("npd vlan %d current trunk node count %d\n",	\
						vlanId, trunkList->count);
		if(0 == trunkList->count) {
			if(NULL != trunkList){
				free(trunkList);
				trunkList = NULL;
				if(NPD_FALSE == tagMode){
					vlanNode->unTagtrunkList = NULL;
				}
				else if(NPD_TRUE == tagMode){
					vlanNode->tagTrunkList = NULL;
				}
			}
		}
	}

	return NPD_TRUE;
}
/**********************************************************************************
 *  npd_vlan_trunk_member_add
 *
 *	DESCRIPTION:
 * 		add trunk to vlan as untagged .
 *
 *	INPUT:
 *		trunkId - trunk id
 *		vlanId -- vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE  - if no error occurred
 *		NPD_VLAN_PORT_EXISTS - if trunk is already a member of vlan
 *		...
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_trunk_member_add
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned char  trunkTag
)
{
	unsigned int retval = VLAN_RETURN_CODE_ERR_NONE;

	if(NPD_FALSE == trunkTag) {
		if(NPD_TRUE == npd_vlan_check_trunk_membership(vlanId,trunkId,NPD_FALSE)) {
			retval =  VLAN_RETURN_CODE_TRUNK_EXISTS;
			syslog_ax_vlan_dbg("trunk %d exists in vlan %d\n",trunkId,vlanId);
		}
		else {
			/* TODO:add port to vlan untagged port list*/
			npd_vlan_trunk_list_addto(vlanId,trunkId,NPD_FALSE);
		}
	}

	if(NPD_TRUE == trunkTag) {
		if(NPD_TRUE == npd_vlan_check_trunk_membership(vlanId,trunkId,NPD_TRUE)) {
			retval =  VLAN_RETURN_CODE_TRUNK_EXISTS;
			syslog_ax_vlan_dbg("trunk %d exists in vlan %d\n",trunkId,vlanId);
		}
		else {
			/* TODO:add port to vlan untagged port list*/
			npd_vlan_trunk_list_addto(vlanId,trunkId,NPD_TRUE);
			/*npd_eth_port_set_pvid(eth_g_index,vlanId);*/
		}
	}
	return retval;
}

/**********************************************************************************
 *  npd_vlan_trunk_member_del
 *
 *	DESCRIPTION:
 * 		delete trunk from vlan as untagged.
 *
 *	INPUT:
 *		vlanId - vlan id
 *		trunkId - trunk id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_NONE  - if no error occurred
 *		NPD_VLAN_PORT_EXISTS - if trunk is not a member of vlan
 *		...
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_trunk_member_del
(
	unsigned short vlanId,
	unsigned short trunkId,
	unsigned char 	tagMode
)
{
	unsigned int retval = VLAN_RETURN_CODE_ERR_NONE;

	if(NPD_FALSE == tagMode){
		if(NPD_FALSE == npd_vlan_check_trunk_membership(vlanId,trunkId,NPD_FALSE)) {
			retval =  VLAN_RETURN_CODE_TRUNK_NOTEXISTS;
			/* syslog_ax_vlan_dbg("trunk mbr NOT exist.");*/
		}
		else {
			/* TODO:delete port from vlan untagged port list*/
			retval = npd_vlan_trunk_list_delfrom(vlanId,trunkId,NPD_FALSE);
			if(NPD_TRUE == retval) {
				retval = VLAN_RETURN_CODE_ERR_NONE;
			}
		}
	}

	if(NPD_TRUE == tagMode){
		if(NPD_FALSE == npd_vlan_check_trunk_membership(vlanId,trunkId,NPD_TRUE)) {
			retval =  VLAN_RETURN_CODE_TRUNK_NOTEXISTS;
			/* syslog_ax_vlan_dbg("trunk mbr NOT exist.");*/
		}
		else {
			/* TODO:delete port from vlan untagged port list*/
			retval = npd_vlan_trunk_list_delfrom(vlanId,trunkId,NPD_TRUE);
			if(NPD_TRUE == retval) {
				retval = VLAN_RETURN_CODE_ERR_NONE;
			}
		}
	}
	/*change vlan state*/
	/*npd_vlan_change_status_by_trunk_status(vlanId,trunkId,NPD_FALSE);	*/
	return retval;
}
#endif

/**npd_vlan_set_port_vlan_ingresfilter**/
unsigned int npd_vlan_set_port_vlan_ingresfilter
(
	unsigned int eth_g_index,
	unsigned char enDis
)
{
	unsigned char	devNum = 0, portNum = 0;
	unsigned int	ret = VLAN_RETURN_CODE_ERR_NONE;
	
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	if (ret != 0) 
	{
		 syslog_ax_vlan_err("get port %#x device port error\n",eth_g_index);
		return VLAN_RETURN_CODE_BADPARAM;
	}
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(portNum)){
			ret = nam_asic_set_port_vlan_ingresflt(devNum,portNum,enDis);
			if(VLAN_CONFIG_FAIL == ret) {
				ret = VLAN_RETURN_CODE_ERR_HW;
			}
		} 
		else {
			 syslog_ax_vlan_err("set port %#x vlan ingress filter %s error\n",	\
			 		eth_g_index,enDis ? "enable":"disable");
			ret = VLAN_RETURN_CODE_BADPARAM;
		}
	}
	else {
		 syslog_ax_vlan_err("set port %#x vlan ingress filter %s find illegal device %d\n",	\
		 			eth_g_index,enDis ? "enable":"disable", devNum);
		ret = VLAN_RETURN_CODE_BADPARAM;
	}
	return ret;
}
/**************
**************/
unsigned int npd_vlan_set_port_pvid
(
	unsigned int eth_g_index,
	unsigned short	pvid
)
{
	unsigned char	devNum ,portNum;
	unsigned int	ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned short 	pvid_old = 0;
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	if (ret != 0) 
	{
		 syslog_ax_vlan_err("npd set pvid with illegal port index %#x\n",eth_g_index);
		return VLAN_RETURN_CODE_BADPARAM;
	}
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(portNum)){
			 syslog_ax_vlan_err("npd_vlan_interface_port_add::devNum %d,virPortNum %d\n",devNum,portNum);
			nam_asic_get_port_pvid(devNum, portNum, &pvid_old);
			ret = nam_asic_set_port_pvid(devNum,portNum,pvid);
			if(VLAN_CONFIG_FAIL == ret) {
				ret = VLAN_RETURN_CODE_ERR_HW;
			}
			else {
				if(NPD_SUCCESS != npd_eth_port_set_pvid(eth_g_index,pvid)){
					nam_asic_set_port_pvid(devNum, portNum, pvid_old);
					syslog_ax_vlan_err("set ethport %d pvid error.");
				}
			}
		} 
		else {
			 syslog_ax_vlan_err("npd_vlan_interface_port_add:: portNum ERROR.\n");
			ret = VLAN_RETURN_CODE_BADPARAM;
		}
	}
	else {
		 syslog_ax_vlan_err("npd_vlan_interface_port_add::devNum ERROR.\n");
		ret = VLAN_RETURN_CODE_BADPARAM;
	}
	return ret;
}
/*********************************
*********************************/
unsigned int npd_vlan_port_pvid_get
(
	unsigned int	eth_g_index,
	unsigned short	*pvid
)
{
	unsigned char	devNum = 0, portNum = 0;
	unsigned int	ret = VLAN_RETURN_CODE_ERR_NONE;

	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&portNum);
	if (ret != 0) 
	{
		 syslog_ax_vlan_err("npd get pvid with illegal port index %#x\n",eth_g_index);
		return VLAN_RETURN_CODE_BADPARAM;
	}
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(portNum)){
			ret = VLAN_RETURN_CODE_ERR_NONE;
		} 
		else {
			 syslog_ax_vlan_err("npd_vlan881::npd_vlan_interface_port_add:: portNum ERROR.\n");
			ret = VLAN_RETURN_CODE_BADPARAM;
			return ret;
		}
	}
	else {
		 syslog_ax_vlan_err("npd_vlan886::npd_vlan_interface_port_add:: devNum ERROR.\n");
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	ret = nam_asic_get_port_pvid(devNum,portNum,pvid);
	if(VLAN_CONFIG_FAIL == ret) {
		*pvid = 0;
		ret = VLAN_RETURN_CODE_ERR_HW;
	}
	
	else {
		 syslog_ax_vlan_dbg("get dev %d port %d pvid = %d",devNum,portNum,*pvid);
		ret = VLAN_RETURN_CODE_ERR_NONE;
	}

	return ret;
}
/**********************************************************************************
 *	npd_vlan_l3intf_create
 *
 *    create Layer 3 vlan interface(actually add L3 interface index to vlan structure)
 *
 *	INPUT:
 *		vlanId - vlan id
 *		ifIndex - L3 interface vlan index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_NOTEXISTS - if vlan is not exists
 *		NPD_VLAN_ERR_NONE - if no error occurred
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_l3intf_create
(
	unsigned short	vlanId,
	unsigned int	ifIndex
)
{
	struct vlan_s *vlanNode = NULL;
	struct vlan_intf_info_s *node = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
	}

	node = &(vlanNode->intfVlan);
	if(node->ifIndex != ~0UI) { /* vlan interface created before*/
		 syslog_ax_vlan_err("npd vlan l3 intf create index change %#0x <--> %#0x",node->ifIndex,ifIndex);
		node->ifIndex = ifIndex;
	}
	else {
		node->ifIndex = ifIndex;
	}

	node->flag = VLAN_PORT_SUBIF_NOTEXIST;
	memset(node->uniMac, 0xFF, 6);
	return VLAN_RETURN_CODE_ERR_NONE;
}

/**********************************************************************************
 *	npd_vlan_l3intf_destroy
 *
 *    destroy Layer 3 vlan interface(actually delete L3 interface index from vlan structure)
 *
 *	INPUT:
 *		vlanId - vlan id
 *		ifIndex - L3 interface vlan index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_NOTEXISTS - if vlan is not exists
 *		NPD_VLAN_ERR_NONE - if no error occurred
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_l3intf_destroy
(
	unsigned short	vlanId,
	unsigned int	ifIndex
)
{
	struct vlan_s *vlanNode = NULL;
	struct vlan_intf_info_s *node = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
	}

	node = &(vlanNode->intfVlan);
	if(node->ifIndex == ~0UI) { /* vlan interface have not created before*/
		 syslog_ax_vlan_err("npd vlan l3 intf destroy index %#0x not created!",ifIndex);
	}
	else if(node->ifIndex != ifIndex){
		 syslog_ax_vlan_err("npd vlan l3 intf destroy index %#0x but vlan has %#0x",ifIndex,node->ifIndex);
		node->ifIndex = ~0UI;
	}
	else {
		node->ifIndex = ~0UI;
	}
	
	return VLAN_RETURN_CODE_ERR_NONE;
}

/**********************************************************************************
 *  npd_vlan_interface_check
 *
 *	DESCRIPTION:
 * 		check out if specified vlan is a L3 interface
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		ifIndex - L3 interface index
 *
 * 	RETURN:
 *		COMMON_RETURN_CODE_NULL_PTR  - null pointer error
 *		TRUE  - if vlan is a L3 interface
 *		FALSE - if vlan is not a L3 interface
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_interface_check
(
	unsigned short vlanId,
	unsigned int   *ifIndex
)
{
	struct vlan_s *vlanNode = NULL;
	unsigned int   index = ~0UI;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == ifIndex){
		return COMMON_RETURN_CODE_NULL_PTR;
	}
	if(NULL == vlanNode) {
		 syslog_ax_vlan_err("npd vlan %d interface check fail for node null",vlanId);
		*ifIndex = index;
		return FALSE;
	}
	
	index = vlanNode->intfVlan.ifIndex;
	if( index != 0) { /* valid L3 interface created before*/
		*ifIndex = index;
		/*return NPD_TRUE;*/
	}
	else
		*ifIndex = ~0UI;

	/* invalid L3 interface or not created*/
	return TRUE;
}

/**********************************************************************************
 *  npd_vlan_l3intf_get_mac
 *
 *	DESCRIPTION:
 * 		This route get vlan interface mac address via KAP
 *
 *	INPUT:
 *		vlanId - vlan id
 *		
 *	
 *	OUTPUT:
 *		macAddr - mac address got
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_l3intf_get_mac
(
	unsigned short vlanId,
	unsigned char *macAddr
)
{
	struct vlan_s *vlanNode = NULL;
	char bcastMac[6] = {0xFF}; /* L3 interface default unique MAC address*/

	if(!macAddr) {
		syslog_ax_vlan_err("get vlan %d interface mac with null memory\n",vlanId);
		return NPD_DBUS_ERROR;
	}

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		 syslog_ax_vlan_err("get vlan %d interface mac found vlan null",vlanId);
		return NPD_DBUS_ERROR;
	}
	else {
		memcpy(macAddr, vlanNode->intfVlan.uniMac, 6);
		if(memcmp(vlanNode->intfVlan.uniMac, bcastMac, 6)) {
			syslog_ax_vlan_dbg("get vlan %d interface mac %02x:%02x:%02x:%02x:%02x:%02x ok\n", \
							vlanId, macAddr[0], macAddr[1], macAddr[2], \
							macAddr[3], macAddr[4], macAddr[5]);
		}
	}
	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_vlan_l3intf_set_mac
 *
 *	DESCRIPTION:
 * 		This route set vlan interface mac address via KAP
 *
 *	INPUT:
 *		vlanId - vlan id
 *		macAddr - mac address got
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_l3intf_set_mac
(
	unsigned short vlanId,
	unsigned char *macAddr
)
{
	struct vlan_s *vlanNode = NULL;

	if(!macAddr) {
		syslog_ax_vlan_err("set vlan %d interface mac with null memory\n",vlanId);
		return NPD_DBUS_ERROR;
	}

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		 syslog_ax_vlan_err("set vlan %d interface mac found vlan null",vlanId);
		return NPD_DBUS_ERROR;
	}
	else {
		memcpy(vlanNode->intfVlan.uniMac, macAddr, 6);
		syslog_ax_vlan_dbg("set vlan %d interface mac %02x:%02x:%02x:%02x:%02x:%02x ok\n", \
						vlanId, macAddr[0], macAddr[1], macAddr[2],	\
						macAddr[3], macAddr[4], macAddr[5]);
	}
	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_vlan_l3intf_get_status
 *
 *	DESCRIPTION:
 * 		This route get vlan interface status in user space(NPD scope)
 *
 *	INPUT:
 *		vlanId - vlan id
 *		
 *	
 *	OUTPUT:
 *		state - l3 interface sw state
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_l3intf_get_ustatus
(
	unsigned short vlanId,
	unsigned char *state
)
{
	struct vlan_s *vlanNode = NULL;

	if(!state) {
		syslog_ax_vlan_err("get vlan %d interface state with null memory\n",vlanId);
		return NPD_DBUS_ERROR;
	}

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		 syslog_ax_vlan_err("get vlan %d interface state found vlan null",vlanId);
		return NPD_DBUS_ERROR;
	}
	else {
		*state = vlanNode->intfVlan.state;
		syslog_ax_vlan_dbg("get vlan %d interface state %s(%d) ok\n", \
						vlanId, *state ? "UP":"DOWN", *state);
	}
	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_vlan_l3intf_set_status
 *
 *	DESCRIPTION:
 * 		This route set vlan interface status in user space(NPD scope)
 *
 *	INPUT:
 *		vlanId - vlan id
 *		state - l3 interface sw state
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_l3intf_set_ustatus
(
	unsigned short vlanId,
	unsigned char state
)
{
	struct vlan_s *vlanNode = NULL;

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		 syslog_ax_vlan_err("set vlan %d interface state found vlan null",vlanId);
		return NPD_DBUS_ERROR;
	}
	else {
		vlanNode->intfVlan.state = state;
		syslog_ax_vlan_dbg("set vlan %d interface state %s(%d) ok\n", \
						vlanId, state ? "UP":"DOWN", state);
	}
	return NPD_SUCCESS;
}


/**********************************************************************************
 *  npd_vlan_get_interface_ve_flag
 *
 *	DESCRIPTION:
 * 		get vlan l3intf flag
 *
 *	INPUT:
 *		vlanId - vlan id
 *		
 *	
 *	OUTPUT:
 *		flag - L3 interface flag
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_get_interface_ve_flag
(
	unsigned short vlanId,
	enum VLAN_PORT_SUBIF_FLAG  *flag
)
{
	struct vlan_s *vlanNode = NULL;
	/*unsigned int   index = ~0UI;*/

	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		 syslog_ax_vlan_err("npd vlan interface check fail:vlan %d node null",vlanId);
		return NPD_DBUS_ERROR;
	}
	else
		*flag = vlanNode->intfVlan.flag;

	return NPD_SUCCESS;
}

/**********************************************************************************
 *  npd_vlan_set_interface_ve_flag
 *
 *	DESCRIPTION:
 * 		get vlan l3intf flag 
 *
 *	INPUT:
 *		vlanId - vlan id
 *		flag - L3 interface flag
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *		NPD_DBUS_ERROR  
 *		NPD_SUCCESS 
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_set_interface_ve_flag
(
	unsigned int vid,
	enum VLAN_PORT_SUBIF_FLAG flag
)
{
	struct vlan_s *vlanNode = NULL;
	/*unsigned int   index = ~0UI;*/

	vlanNode = npd_find_vlan_by_vid(vid);
	if(NULL == vlanNode) {
		 syslog_ax_vlan_err("npd vlan interface check fail:vlan %d node null",vid);
		return NPD_DBUS_ERROR;
	}
	else
		vlanNode->intfVlan.flag = flag;

	return NPD_SUCCESS;
}

/**********************************************************************************
 *
 * create vlan by VLAN ID on both SW and HW side
 *
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_GENERAL - if error occurred when create vlan in SW side 
 *		NPD_VLAN_ERR_HW - if error occurred when create vlan in HW side
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_activate_with_vid
(
	unsigned short vlanId
)
{
	unsigned int	ret;
	struct vlan_s	*vlanNode = NULL ;
	ret = npd_check_vlan_exist(vlanId);
	/* syslog_ax_vlan_dbg("Npd_vlan::npd_vlan_activate_with_vid::vlan %d exist %s.",\
									vlanId,((ret==NPD_VLAN_EXISTS)?"yes":"Not"));*/
	/*vlan exsits*/
	if(VLAN_RETURN_CODE_VLAN_EXISTS == ret) {
		vlanNode = npd_find_vlan_by_vid(vlanId);
		if (NULL == vlanNode) { 
			 syslog_ax_vlan_err("npd_vlan_find error.\n");
			ret = VLAN_RETURN_CODE_ERR_GENERAL; 
		}
		else{
			ret = VLAN_RETURN_CODE_VLAN_EXISTS; /*vlan really Exists.*/
		}
		return ret;   /*NPD return HERE!!! steps into next CMD node*/
	}
	/*/vlan NOT exists*/
	else {
		vlanNode = npd_create_vlan_by_vid(vlanId);
		if (NULL == vlanNode)
		{ 
			 syslog_ax_vlan_err("vlan create error.");
			ret = VLAN_RETURN_CODE_ERR_GENERAL;  
			return ret;
		}
		else 
		{
			/* add vlan entry to asic */
			ret = nam_asic_vlan_entry_active(PRODUCT_ID, vlanId);
			if (ret != 0) {
				 syslog_ax_vlan_err("npd_dbus_vlan_config_vlan_entry_one:: vlanId %d ERROR. \n",vlanId);
				ret = VLAN_RETURN_CODE_ERR_HW;
				npd_delete_vlan_by_vid(vlanId);
			}
			else {
				/*add vlan node to global vlan array*/
				/* syslog_ax_vlan_dbg("Enter-Npd_vlan::npd_vlan_fillin_global:vid %d,ifindex %d",vlanId,vlanNode->intfVlan.ifIndex);*/
				ret = npd_vlan_fillin_global(vlanId,vlanNode);
				return ret;
			}
		}
	}
	return ret;
}

/**********************************************************************************
 *
 * create vlan by VLAN ID on both SW and HW side
 *
 *
 *	INPUT:
 *		vlanId - vlan id
 *		name - vlan alias name
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_GENERAL - if error occurred when create vlan in SW side 
 *		NPD_VLAN_ERR_HW - if error occurred when create vlan in HW side
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_activate
(
	unsigned short vlanId,
	char *name
)
{
	unsigned int ret = VLAN_RETURN_CODE_ERR_NONE;

	struct vlan_s *vlanNode = NULL ;
	ret = npd_check_vlan_real_exist(vlanId);
	/*vlan exsits*/
	if(ret == TRUE) {
		vlanNode = npd_find_vlan_by_vid(vlanId);
		if (NULL == vlanNode) { 
			 syslog_ax_vlan_err("npd_vlan_find error.\n");
			ret = VLAN_RETURN_CODE_ERR_GENERAL; 
		}
		else{
			ret = VLAN_RETURN_CODE_VLAN_EXISTS;
			if(vlanNode->isAutoCreated){
				vlanNode->isAutoCreated = FALSE;				
				memcpy(vlanNode->vlanName,name,NPD_VLAN_IFNAME_SIZE);
				ret = VLAN_RETURN_CODE_ERR_NONE;
			}
		}
		return ret;   /*NPD return HERE!!! steps into next CMD node*/
	}
	/*vlan NOT exists*/
	else {
		vlanNode = npd_find_vlan_by_name(name);
		if(NULL != vlanNode) {
			ret = VLAN_RETURN_CODE_NAME_CONFLICT;
			return ret;
		}
		
		vlanNode = npd_create_vlan_by_name(name);
		if (NULL == vlanNode) { 
			 syslog_ax_vlan_err("vlan create error.\n");
			return VLAN_RETURN_CODE_ERR_GENERAL;  
		}
		else 
		{
			if(!strcmp(name,"4094vlan") || \
				((PRODUCT_ID_AX7K_I == PRODUCT_ID)&&\
				((0 == LOCAL_CHASSIS_SLOT_NO)||(1 == LOCAL_CHASSIS_SLOT_NO))&&\
				(NPD_VLAN_OBC0_VLAN_ID == vlanId))){
				vlanNode->isAutoCreated = TRUE;
			}
			vlanNode->vid = vlanId;
            vlanNode->brgVlan.fdbLimit = 0;
			vlanNode->brgVlan.fdbCount = 0;
			/*add vlan node to global vlan array*/
			/* syslog_ax_vlan_dbg("Enter-Npd_vlan::npd_vlan_fillin_global:vid %d,ifindex %d",vlanId,vlanNode->intfVlan.ifIndex);*/

			npd_vlan_fillin_global(vlanId,vlanNode);

			ret = nam_asic_vlan_entry_active(PRODUCT_ID, vlanId);
			if (VLAN_CONFIG_SUCCESS != ret) {
				 syslog_ax_vlan_err("nam_asic_vlan_entry_active:: vlanId %d ERROR. \n",vlanId);
				ret = VLAN_RETURN_CODE_ERR_HW;
				npd_delete_vlan_by_vid(vlanId);
			}
		}
	}
	return ret;
}

/**********************************************************************************
 *
 * change vlan untag port memebers and fdb entry by port link status
 *
 *
 *	INPUT:
 *		vid - vlan id
 *		eth_g_index - port global index
 *		event - port link status
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS
 *		NPD_FAIL
 *		
 *
 **********************************************************************************/
int npd_vlan_untag_port_link_change
(
	unsigned int eth_g_index,
	struct port_based_vlan_s *portVlan,
	enum ETH_PORT_NOTIFIER_ENT event
)
{
	struct vlan_s* node = NULL;
	unsigned short vid = portVlan->vid;
	unsigned int ifindex = ~0UI;
	int ret = 0;
	
	if(ETH_PORT_NOTIFIER_LINKDOWN_E == event){
		/* TODO:check vlan status*/
		if(NPD_SUCCESS != npd_modify_vlan_status_by_portdown(vid)){
			 syslog_ax_vlan_err("npd_modify_vlan_status_by_portdown :: vid %d error\n",vid);
			/*return NPD_FAIL;*/
		}
        #if 0 
		/*move to deal in link change event func*/
		if(nam_fdb_table_delete_entry_with_port(eth_g_index)){
			 syslog_ax_vlan_err("npd_vlan_untag_port_link_change :: nam_fdb_table_delete_entry_with_port error port_index %d\n",eth_g_index);
			/*return NPD_FAIL;*/
		}
		#endif
	}
	else if(ETH_PORT_NOTIFIER_LINKUP_E == event){
		/* TODO:check vlan status*/
		 /*syslog_ax_vlan_dbg("npd_vlan_untag_port_link_change :: eth-port %d  UP, vid %d\n",eth_g_index,vid);*/
		node = npd_find_vlan_by_vid(vid);
		if(node){
			if(VLAN_STATE_DOWN_E == node->brgVlan.state){	
				node->brgVlan.state = VLAN_STATE_UP_E;
				 syslog_ax_vlan_dbg("npd_vlan_untag_port_link_change:: vlan%d UP\n",vid);
				npd_intf_set_vlan_l3intf_status(node->vid, VLAN_STATE_UP_E);
				if((NPD_TRUE == npd_vlan_interface_check(vid,&ifindex))&&(~0UI != ifindex)){
					ret = npd_arp_snooping_gratuitous_send(ifindex,vid,0,KAP_INTERFACE_VID_E);
					npd_syslog_dbg("vlan interface untag port gratuitous arp send %s,ret %d \n",(0==ret)?"SUCCESS":"FAILED",ret);
				}
			}
		}
	}
	
	return NPD_SUCCESS;
}


unsigned int npd_vlan_qinq_endis
(
	unsigned short	vlanId,
	unsigned int	eth_g_index,
	unsigned char	isEnable
)
{
	unsigned int ret = 0;
    unsigned char tagMode = 0;
	unsigned char devNum,virPortNum;

	/*check out if the port member is a member of the special vlanId*/
	if(!(ret=npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode)))
	{
		return VLAN_RETURN_CODE_PORT_NOTEXISTS;
	}
	syslog_ax_vlan_dbg("isEnable = %d,vlanId = %d\n",isEnable,vlanId);
	npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNum);
	syslog_ax_vlan_dbg("devNum = %d,virPortNum = %d\n",devNum,virPortNum);
	ret = nam_vlan_qinq_endis(devNum,vlanId,virPortNum,isEnable,tagMode);

	if(ret != VLAN_RETURN_CODE_ERR_NONE)
	{
		syslog_ax_vlan_dbg("set vlan %d, dev %d, port %d qinq err !\n",vlanId,devNum,virPortNum);
		return ret;
	}
	
	if(isEnable)
	{
		#if 0
		ret = nam_vlan_qinq_port_egress_tpid_set(devNum,virPortNum,1,1);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_dbg("set devNum %d,PortNum %d ,tag0 tpid 0x88a8 error !\n",devNum,virPortNum);
			return ret;
		}
		#endif
		/*set port default  tag1 0x8100*/
		ret = nam_vlan_qinq_port_egress_tpid_set(devNum,virPortNum,1,0);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_dbg("set devNum %d,PortNum %d ,tag1 tpid 0x8100 error !\n",devNum,virPortNum);
			return ret;
		}
	}
	else
	{
		ret = nam_vlan_qinq_port_egress_tpid_set(devNum,virPortNum,1,0);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_dbg("set devNum %d,PortNum %d ,tag0 tpid 0x8100 error !\n",devNum,virPortNum);
			return ret;
		}
	}
	return ret;
}

unsigned int npd_vlan_allcscdport_qinq_endis(unsigned short vlanId,unsigned int endis)
{
	int i=0;
	int devnum=0,portnum = 0;
	int ret = NPD_FAIL;

	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
	{
		ret = nam_vlan_qinq_endis(0,vlanId,25,endis,1);
		if(ret != COMMON_SUCCESS)
		{
			syslog_ax_vlan_err("nam vlan qinq endis FAIL !\n");
		}
		ret = nam_vlan_qinq_endis(0,vlanId,26,endis,1);
		if(ret != COMMON_SUCCESS)
		{
			syslog_ax_vlan_err("nam vlan qinq endis FAIL !\n");
		}
		ret = nam_vlan_qinq_endis(1,vlanId,27,endis,1);
		if(ret != COMMON_SUCCESS)
		{
			syslog_ax_vlan_err("nam vlan qinq endis FAIL !\n");
		}
	}
	else
	{
		for(i=0; i< asic_board->asic_cscd_port_cnt; i++)
		{

			devnum = asic_board->asic_cscd_ports[i].dev_num;
			portnum = asic_board->asic_cscd_ports[i].port_num;
			syslog_ax_vlan_dbg("devnum = %d,portnum = %d\n",devnum,portnum);
			ret = nam_vlan_qinq_endis(0,vlanId,portnum,endis,1);
			if(ret != COMMON_SUCCESS)
			{
				syslog_ax_vlan_err("nam vlan qinq endis FAIL !\n");
			}
		}
	}
	syslog_ax_vlan_dbg("set all cscd port qinq %s SUCCESS !!!\n",(endis)?"Enable":"Disable");
	return ret;
}


unsigned int npd_vlan_tocpuport_qinq_endis(unsigned short vlanId,unsigned short cpu_no,unsigned short cpu_port_no,unsigned char endis)
{
	int ret = VLAN_RETURN_CODE_ERR_NONE;
	int devnum = 0,portnum = 0;
	
	if(BOARD_TYPE != BOARD_TYPE_AX71_CRSMU)
	{
		devnum= asic_board->asic_to_cpu_ports[8*cpu_no+cpu_port_no].dev_num;
		portnum = asic_board->asic_to_cpu_ports[8*cpu_no+cpu_port_no].port_num;	
	}

	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
	{
		ret = nam_vlan_qinq_endis(1,vlanId,26,endis,1);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("QinQ add CPU port to vlan %d error\n",vlanId);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
	    }
	}
	if((BOARD_TYPE == BOARD_TYPE_AX81_AC8C)||(BOARD_TYPE == BOARD_TYPE_AX81_AC12C))
	{

		ret = nam_vlan_qinq_endis(devnum,vlanId,portnum,endis,1);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("QinQ add CPU port to vlan %d error\n",vlanId);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
	    }
	    syslog_ax_vlan_dbg("nam asic vlan entry ports add: devNum %d, vlanId %d, PortNum %d,ret %d.\n",\
    						    devnum,vlanId,portnum,ret);				
	}
	else if(BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S)
	{
		ret = nam_vlan_qinq_endis(devnum,vlanId,portnum,endis,1);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("QinQ add CPU port to vlan %d error\n",vlanId);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
	    }
	    syslog_ax_vlan_dbg("nam asic vlan entry ports add: devNum %d, vlanId %d, PortNum %d,ret %d.\n",\
    						    devnum,vlanId,portnum,ret);
		ret = nam_vlan_qinq_endis(0,vlanId,26,endis,1);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("QinQ add CPU port to vlan %d error\n",vlanId);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
	    }
	    syslog_ax_vlan_dbg("nam asic vlan entry ports add: devNum %d, vlanId %d, PortNum %d,ret %d.\n",\
    						    devnum,vlanId,portnum,ret);
	}

	syslog_ax_vlan_err("set to CPU port qinq %s SUCCESS!!!\n",(endis)?"Enable":"Disable");
	return ret;
}
/**********************************************************************************
 *
 * change vlan tag port memebers and fdb entry by port link status
 *
 *
 *	INPUT:
 *		dot1vlanlst - all vlans of port added by tag 
 *		eth_g_index - port global index
 *		event - port link status
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_SUCCESS
 *		NPD_FAIL
 *		
 *
 **********************************************************************************/
int npd_vlan_tag_port_link_change
(
	unsigned int eth_g_index,
	dot1q_vlan_list_s* dot1Vlanlst,
	enum ETH_PORT_NOTIFIER_ENT event
)
{
	
	struct vlan_s* vlanNode = NULL;
	struct eth_port_dot1q_list_s* node = NULL;
	struct list_head * list = NULL,*pos = NULL;
	int ret = 0;
	int arpSendRet = 0;
	unsigned int ifindex = ~0UI;
	
	if(dot1Vlanlst){
		if(ETH_PORT_NOTIFIER_LINKDOWN_E == event){
			list = &(dot1Vlanlst->list);
			__list_for_each(pos,list) {
				node = list_entry(pos,struct eth_port_dot1q_list_s,list);
				if(node){
					ret |= npd_modify_vlan_status_by_portdown(node->vid);
			
				}
			}
			#if 0
			/*move to link change func*/
			//ret |= npd_set_port_l3intf_status(eth_g_index,ETH_PORT_NOTIFIER_LINKDOWN_E);
			ret |= nam_fdb_table_delete_entry_with_port(eth_g_index);
			#endif
		}
		else if(ETH_PORT_NOTIFIER_LINKUP_E == event){
			list = &(dot1Vlanlst->list);
			__list_for_each(pos,list) {
				node = list_entry(pos,struct eth_port_dot1q_list_s,list);
				if(node){
					vlanNode = npd_find_vlan_by_vid(node->vid);
					if(vlanNode){
						if(VLAN_STATE_DOWN_E == vlanNode->brgVlan.state){	
							vlanNode->brgVlan.state = VLAN_STATE_UP_E;
							/* syslog_ax_vlan_dbg("npd_vlan_tag_port_link_change:: vlan%d UP\n",node->vid);*/
							ret |= npd_intf_set_vlan_l3intf_status(node->vid, VLAN_STATE_UP_E);		
							if((NPD_TRUE == npd_vlan_interface_check(node->vid,&ifindex))&&(~0UI != ifindex)){
								arpSendRet = npd_arp_snooping_gratuitous_send(ifindex,node->vid,0,KAP_INTERFACE_VID_E);
								npd_syslog_dbg("vlan interface tag port gratuitous arp send %s,ret %d \n",(0==arpSendRet)?"SUCCESS":"FAILED",ret);
							}
						}
					}			
				}
			}
			/*ret |= npd_set_port_l3intf_status(eth_g_index,ETH_PORT_NOTIFIER_LINKUP_E);*/
		}
	}
	else{
		return NPD_FAIL;
	}
	
	return ret;
}

/**********************************************************************************
 *  npd_vlan_get_port_wan
 *
 *	DESCRIPTION:
 * 		get whether the port is wan by global ethernet port index
 *
 *	INPUT:
 *		eth_g_index - global ethernet port index
 *		iswan - wan interface flag
 *				1: is a wan interface
 *				0:is not a wan interface
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ret
 *
 **********************************************************************************/

unsigned int npd_vlan_get_port_wan
(
	unsigned int eth_g_index,
	unsigned int *iswan
)
{
	unsigned int ret = 0;
	unsigned int	slot_index =0,local_port_no = 0;
	enum module_id_e module_type = MODULE_ID_NONE;
	
	slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no = ETH_LOCAL_INDEX2NO(slot_index, local_port_no);
	module_type = MODULE_TYPE_ON_SLOT_INDEX(slot_index);

	if(npd_eslot_check_subcard_module(module_type) && \
		((AX51_GTX == npd_check_subcard_type(PRODUCT_ID, local_port_no)) || \
		 (AX51_SFP == npd_check_subcard_type(PRODUCT_ID, local_port_no)))) {
		*iswan = 1;
	}
	else if(!PRODUCT_IS_BOX && (AX7_CRSMU_SLOT_NUM == slot_index)) { /* read CRSMU RGMII ethernet port info*/
		*iswan = 1;
	}
	else if((MODULE_ID_AX5_5612I == module_type) && 
		(local_port_no >= 9) && (local_port_no <= 12)) {				
		*iswan = 1;
	}			
	else if(MODULE_ID_AX5_5612E == module_type  || MODULE_ID_AX5_5608 == module_type){
		*iswan = 1;
	}
	return NPD_SUCCESS;
	
}

/**********************************************************************************
 *  npd_vlan_add_or_del_port
 *
 *	DESCRIPTION:
 * 		vlan add or delete port as tagged or untagged mode
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port member
 *      isAdd - add or delete port
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		VLAN_RETURN_CODE_ERR_NONE - add or delete port success
 *
 **********************************************************************************/
 
unsigned int npd_vlan_add_or_del_port
(
	unsigned short	vlanId,
	unsigned int	eth_g_index,
	unsigned char	isTagged,
	unsigned char   isAdd
)
{
    if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
	{
    	unsigned int ret = 0;
        unsigned char tagMode = 0;

		/* for stp, add */
    	unsigned int iswan = 0;
    	ret = npd_vlan_get_port_wan(eth_g_index, &iswan);
    	if(NPD_SUCCESS != ret){
    		syslog_ax_vlan_dbg("get vlan wan port failed!\n");
    	}
    		
    	if(isAdd){
    		/* disable DLDP on port in default vlan, before add to new vlan */
    		if (isTagged == 0) {/* need deal when port tag mode is untag*/
    			ret = npd_dldp_port_del(1, eth_g_index, isTagged);
    			if (ret) {
    				syslog_ax_vlan_dbg("dldp delete port %#x from default vlan, ret %x\n",
    									eth_g_index, ret);
    				ret = VLAN_RETURN_CODE_ERR_NONE;
    			}
    		}
    	
    		ret = npd_vlan_interface_port_add(vlanId,eth_g_index,isTagged);
    		if(VLAN_RETURN_CODE_ERR_NONE == ret || NPD_TRUE == ret) {
    			ret = VLAN_RETURN_CODE_ERR_NONE;
				#if 1    /* for distributed */
    			if(!isTagged) {
    				npd_mstp_del_port(1,eth_g_index);
    				syslog_ax_vlan_dbg("mstp delete port %#x from default vlan\n",eth_g_index);
    			}
    			
    			npd_mstp_add_port(vlanId,eth_g_index,iswan);
    			syslog_ax_vlan_dbg("mstp add port %#x to vlan %d\n",eth_g_index,vlanId);
    	        #endif
    			/* enable DLDP on port in new vlan */
    			ret = npd_dldp_port_add(vlanId, eth_g_index, isTagged);
    			if (ret) {
    				syslog_ax_vlan_dbg("dldp add port %#x from to vlan %d, ret %x\n",
    									eth_g_index, vlanId, ret);
    				ret = VLAN_RETURN_CODE_ERR_NONE;
    			}
    		}
    	}
    	else {
            ret = npd_check_vlan_real_exist(vlanId);
        	if (TRUE != ret) {
        		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return.*/
        	}
        	
        	/*here MUST check the ETH-port is already member of the VLAN or NOT*/
        	ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode);
        	if (NPD_FALSE == ret) {
        		ret = VLAN_RETURN_CODE_PORT_NOTEXISTS;
        		/* syslog_ax_vlan_dbg("Not a vlan member");*/
        	}
        	else if (isTagged != tagMode ) {
        		/* syslog_ax_vlan_dbg("tagMode %s",(tagMode==NPD_TRUE)?"tagged":"untag");*/
        		ret = VLAN_RETURN_CODE_PORT_TAG_CONFLICT;
        	}
    		else if(advanced_routing_default_vid == vlanId){
    			ret = VLAN_RETURN_CODE_PROMIS_PORT_CANNOT_DEL;
    		}
            else if((TRUE == isTagged)&&npd_eth_port_check_tag_for_other_subif(eth_g_index,0,vlanId)){
                ret = VLAN_RETURN_CODE_PORT_SUBINTF_EXISTS;
            }
            else if((FALSE == isTagged)&&(npd_prot_vlan_vid_check_by_global_index(eth_g_index,vlanId))){
                ret = VLAN_RETURN_CODE_HAVE_PROT_VLAN_CONFIG;
            }
    		else{
    			/* disable DLDP on port in vlan, before add to default vlan */
    			ret = npd_dldp_port_del(vlanId, eth_g_index, isTagged);
    			if (ret) {
    				syslog_ax_vlan_dbg("dldp delete port %#x from default vlan, ret %x\n",
    									eth_g_index, ret);
    				ret = VLAN_RETURN_CODE_ERR_NONE;
    			}
    		
    			ret = npd_vlan_interface_port_del(vlanId,eth_g_index,isTagged);
    			if(VLAN_RETURN_CODE_ERR_NONE == ret || NPD_TRUE == ret) {
					#if 1   /* for distributed */
    				npd_mstp_del_port(vlanId,eth_g_index);
    				syslog_ax_vlan_dbg("mstp delete port %#x from vlan %d\n",eth_g_index,vlanId);
    				ret = npd_vlan_port_return_default_vlan(eth_g_index);
    				if(NPD_FALSE == ret) {
    					syslog_ax_vlan_dbg(" add port %#x to default vlan OK!\n",eth_g_index);						
    					npd_mstp_add_port(1,eth_g_index,iswan);
    					syslog_ax_vlan_dbg("mstp add port %#x to default vlan\n",eth_g_index);
    				}
					/* bugfix: AXSSZFI-503 */
					ret = VLAN_RETURN_CODE_ERR_NONE;
					#else
					ret = npd_vlan_port_return_default_vlan(eth_g_index);
    				if(NPD_FALSE == ret) {
    					syslog_ax_vlan_dbg(" add port %#x to default vlan error!\n",eth_g_index);
    				}
    				ret = VLAN_RETURN_CODE_ERR_NONE;
 					#endif   		
    				/* enable DLDP on port in default vlan */
    				if (isTagged == 0) {/* need deal when port tag mode is untag*/
    					ret = npd_dldp_port_add(1, eth_g_index, isTagged);
    					if (ret) {
    						syslog_ax_vlan_dbg("dldp add port %#x from to vlan %d, ret %x\n",
    											eth_g_index, vlanId, ret);
    						ret = VLAN_RETURN_CODE_ERR_NONE;
    					}
    				}
    			}
    		}
    	}
    	return ret;	
	}
	else
	{
    	unsigned int ret = 0;
        unsigned char tagMode = 0;
    	unsigned int iswan = 0;
    	ret = npd_vlan_get_port_wan(eth_g_index, &iswan);
    	if(NPD_SUCCESS != ret){
    		syslog_ax_vlan_dbg("get vlan wan port failed!\n");
    	}
    	
    	if(isAdd){
    		/* disable DLDP on port in default vlan, before add to new vlan */
    		if (isTagged == 0) {/* need deal when port tag mode is untag*/
    			ret = npd_dldp_port_del(1, eth_g_index, isTagged);
    			if (ret) {
    				syslog_ax_vlan_dbg("dldp delete port %#x from default vlan, ret %x\n",
    									eth_g_index, ret);
    				ret = VLAN_RETURN_CODE_ERR_NONE;
    			}
    		}
    	
    		ret = npd_vlan_interface_port_add(vlanId,eth_g_index,isTagged);
    		if(VLAN_RETURN_CODE_ERR_NONE == ret || NPD_TRUE == ret) {
    			ret = VLAN_RETURN_CODE_ERR_NONE;
    			if(!isTagged) {
    				npd_mstp_del_port(1,eth_g_index);
    				syslog_ax_vlan_dbg("mstp delete port %#x from default vlan\n",eth_g_index);
    			}
    			
    			npd_mstp_add_port(vlanId,eth_g_index,iswan);
    			syslog_ax_vlan_dbg("mstp add port %#x to vlan %d\n",eth_g_index,vlanId);
    	
    			/* enable DLDP on port in new vlan */
    			ret = npd_dldp_port_add(vlanId, eth_g_index, isTagged);
    			if (ret) {
    				syslog_ax_vlan_dbg("dldp add port %#x from to vlan %d, ret %x\n",
    									eth_g_index, vlanId, ret);
    				ret = VLAN_RETURN_CODE_ERR_NONE;
    			}
    		}
    	}
    	else {
            ret = npd_check_vlan_real_exist(vlanId);
        	if (TRUE != ret) {
        		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return.*/
        	}
        	
        	/*here MUST check the ETH-port is already member of the VLAN or NOT*/
        	ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode);
        	if (NPD_FALSE == ret) {
        		ret = VLAN_RETURN_CODE_PORT_NOTEXISTS;
        		/* syslog_ax_vlan_dbg("Not a vlan member");*/
        	}
        	else if (isTagged != tagMode ) {
        		/* syslog_ax_vlan_dbg("tagMode %s",(tagMode==NPD_TRUE)?"tagged":"untag");*/
        		ret = VLAN_RETURN_CODE_PORT_TAG_CONFLICT;
        	}
    		else if(advanced_routing_default_vid == vlanId){
    			ret = VLAN_RETURN_CODE_PROMIS_PORT_CANNOT_DEL;
    		}
            else if((TRUE == isTagged)&&npd_eth_port_check_tag_for_other_subif(eth_g_index,0,vlanId)){
                ret = VLAN_RETURN_CODE_PORT_SUBINTF_EXISTS;
            }
            else if((FALSE == isTagged)&&(npd_prot_vlan_vid_check_by_global_index(eth_g_index,vlanId))){
                ret = VLAN_RETURN_CODE_HAVE_PROT_VLAN_CONFIG;
            }
    		else{
    			/* disable DLDP on port in vlan, before add to default vlan */
    			ret = npd_dldp_port_del(vlanId, eth_g_index, isTagged);
    			if (ret) {
    				syslog_ax_vlan_dbg("dldp delete port %#x from default vlan, ret %x\n",
    									eth_g_index, ret);
    				ret = VLAN_RETURN_CODE_ERR_NONE;
    			}
    		
    			ret = npd_vlan_interface_port_del(vlanId,eth_g_index,isTagged);
    			if(VLAN_RETURN_CODE_ERR_NONE == ret || NPD_TRUE == ret) {
    				npd_mstp_del_port(vlanId,eth_g_index);
    				syslog_ax_vlan_dbg("mstp delete port %#x from vlan %d\n",eth_g_index,vlanId);
    				ret = npd_vlan_port_return_default_vlan(eth_g_index);
    				if(NPD_FALSE == ret) {
    					npd_mstp_add_port(1,eth_g_index,iswan);
    					syslog_ax_vlan_dbg("mstp add port %#x to default vlan\n",eth_g_index);
    				}
    				ret = VLAN_RETURN_CODE_ERR_NONE;
    		
    				/* enable DLDP on port in default vlan */
    				if (isTagged == 0) {/* need deal when port tag mode is untag*/
    					ret = npd_dldp_port_add(1, eth_g_index, isTagged);
    					if (ret) {
    						syslog_ax_vlan_dbg("dldp add port %#x from to vlan %d, ret %x\n",
    											eth_g_index, vlanId, ret);
    						ret = VLAN_RETURN_CODE_ERR_NONE;
    					}
    				}
    			}
    		}
    	}
    	return ret;
	}
}
/*
*/
unsigned int npd_vlan_interface_cscd_add
(
	unsigned short	vlanId,
	unsigned char	port,
	unsigned char	isTagged
)
{
	unsigned int  ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned char devNum = 0,virPortNum = 0,tagMode = 0;
	unsigned int vlanIfIndex = ~0UI;
	unsigned int vlanIsL3intf = NPD_FALSE;
	struct asic_port_info_s xgport;

	memset(&xgport, 0, sizeof(struct asic_port_info_s));
	xgport = ax7i_xg_conn_port_mapArr[0][port];
	if(0== port){
	devNum = xgport.devNum;
	virPortNum = xgport.portNum;
		}
	else if(1 == port ){
		devNum = 0;
		virPortNum = 12;
		}

	
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = VLAN_RETURN_CODE_ERR_NONE;
		} 
		else {
			 syslog_ax_vlan_err("npd_vlan_interface_cscd_add:: virtual port number %d error\n",virPortNum);
			ret = VLAN_RETURN_CODE_BADPARAM;
			return ret;
		}
	}
	else{
		 syslog_ax_vlan_err("npd_vlan_interface_cscd_add:: device number %d error\n",devNum);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	/*if NOT Steps into the (vlan-config CMD node),vlan exists or not Test is wanted.*/
	/*if Steps into (vlan-config CMD node), it does NOT need test vlan node exist or not.*/
#if 0
	ret = npd_check_vlan_real_exist(vlanId);
	if (TRUE != ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return NPD_VLAN_BADPARAM.*/
	}
		/* check if the vlan is l3 interface */
		ret = npd_vlan_interface_check(vlanId,&vlanIfIndex);
		if((NPD_TRUE == ret) &&(vlanIfIndex !=(~0UI))){
			vlanIsL3intf = NPD_TRUE;
		}
#endif
		
		/*call nam_asic_vlan_entry_ports_add to set the Vlan table entry in dev.*/
		/*call npd_vlan_interface_add_port_member to do Record in SW.*/
		ret = nam_asic_vlan_entry_ports_add(devNum, vlanId, virPortNum, isTagged);
		syslog_ax_vlan_dbg("nam asic vlan entry ports add: devNum %d, vlanId %d, virPortNum %d,isTagged %d,ret %d.\n",\
						devNum,vlanId,virPortNum,isTagged,ret);
		if (VLAN_CONFIG_SUCCESS == ret) {
#if 0
			if(!isTagged){
				/* fdb delete*/
				nam_fdb_table_delete_entry_with_vlan_port(1,eth_g_index);

				/* igmp port disable*/
				npd_vlan_port_igmp_snp_endis_config(NPD_FALSE,1,eth_g_index,NPD_FALSE);
			}
			/*add the port to the list*/
			ret = npd_vlan_port_member_add(vlanId,eth_g_index,isTagged);
			if(VLAN_RETURN_CODE_ERR_NONE != ret) {
				 syslog_ax_vlan_dbg("the port add to vlan port list error ret %d\n",ret);
				return ret;
			}
			syslog_ax_vlan_dbg("port(%d %d) vlan %d correct\n",devNum,virPortNum,vlanId);
			/*set vlan port ingress filter*/
			npd_vlan_set_port_vlan_ingresfilter(eth_g_index, NPD_TRUE);
#endif
			ret = nam_asic_set_port_vlan_ingresflt(devNum,virPortNum,NPD_TRUE);
			if(VLAN_CONFIG_FAIL == ret) {
				syslog_ax_vlan_dbg("nam asic set port vlan ingresflt error! ret %d.", ret);
				ret = VLAN_RETURN_CODE_ERR_HW;
			}
#if 0
			/* vlan is L3 interface,set it's attribute*/			
			if(NPD_TRUE == vlanIsL3intf) {
				/*set intf attr*/
				ret = npd_intf_vlan_set_attr_for_cscd(vlanIfIndex,devNum,virPortNum);
				syslog_ax_vlan_dbg("npd_intf_vlan_set_attr_for_cscd ret %d.", ret);
			}
			else 
				ret = VLAN_RETURN_CODE_ERR_NONE;
#endif
			ret = VLAN_RETURN_CODE_ERR_NONE;
		}
		else {
			syslog_ax_vlan_dbg("nam asic vlan entry ports add error: ret %d.\n",ret);
			ret = VLAN_RETURN_CODE_ERR_HW;
		}
#if 0
	/*if advaced-routing vlan , update vlan memebers*/
	npd_intf_update_vlan_info_to_advanced_routing_l3intf(vlanId);
#endif
	return ret;
}

/*
*/
unsigned int npd_vlan_interface_port_add_for_dynamic_trunk
(
	unsigned short	vlanId,
	unsigned int	eth_g_index,
	unsigned char	isTagged
)
{
	unsigned int  ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned char devNum = 0,virPortNum = 0,tagMode = 0;
	unsigned int  ifIndex = ~0UI;
	unsigned int vlanIfIndex = ~0UI;
	struct eth_port_s	*ethPort = NULL;
	struct port_based_vlan_s  *pvid = NULL;
	unsigned int vlanIsL3intf = NPD_FALSE;
	unsigned int portIsPromis = NPD_FALSE;
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNum);
	if (ret != 0) 
	{
		 syslog_ax_vlan_err("npd_vlan_interface_port_add::global_index %d convert to devPort Error.\n",eth_g_index);
		return VLAN_RETURN_CODE_BADPARAM;
	}
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = VLAN_RETURN_CODE_ERR_NONE;
		} 
		else {
			 syslog_ax_vlan_err("npd_vlan_interface_port_add:: virtual port number %d error\n",virPortNum);
			ret = VLAN_RETURN_CODE_BADPARAM;
			return ret;
		}
	}
	else
	{
		 syslog_ax_vlan_err("npd_vlan_interface_port_add:: device number %d error\n",devNum);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	/*if NOT Steps into the (vlan-config CMD node),vlan exists or not Test is wanted.*/
	/*if Steps into (vlan-config CMD node), it does NOT need test vlan node exist or not.*/
	ret = npd_check_vlan_real_exist(vlanId);
	if (TRUE != ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return NPD_VLAN_BADPARAM.*/
	}
	/*here MUST check the ETH-port is already member of the VLAN or NOT*/
	ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode);
	if (NPD_TRUE == ret) {
		/* syslog_ax_vlan_dbg("Port %d has already be the member of vlan % d\n",virPortNum,vlanId);*/
		return VLAN_RETURN_CODE_PORT_EXISTS;
	}
	 syslog_ax_vlan_dbg("eth_port index %#x is not untagged or tagged port member of vlan %d\n",eth_g_index,vlanId);
	/*check the port is Layer3 interface Or NOT,if Yes, can NOT add to vlan.*/
	ret = nam_asic_vlan_entry_ports_add(devNum, vlanId, virPortNum, isTagged);
	if (VLAN_CONFIG_SUCCESS == ret) 
	{
		/*add the port to the list*/
		ret = npd_vlan_port_member_add(vlanId,eth_g_index,isTagged);
		if(VLAN_RETURN_CODE_ERR_NONE != ret) {
			 syslog_ax_vlan_dbg("the port add to vlan port list error ret %d\n",ret);
			return ret;
		}
		syslog_ax_vlan_dbg("port(%d %d) vlan %d correct\n",devNum,virPortNum,vlanId);
	}
	else 
	{
		ret = VLAN_RETURN_CODE_ERR_HW;
	}
	return ret;
}

/*
*/
unsigned int npd_vlan_interface_port_add
(
	unsigned short	vlanId,
	unsigned int	eth_g_index,
	unsigned char	isTagged
)
{
	unsigned int  ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned char devNum = 0,virPortNum = 0,tagMode = 0;
	unsigned int  ifIndex = ~0UI;
	unsigned int vlanIfIndex = ~0UI;
	struct eth_port_s	*ethPort = NULL;
	struct port_based_vlan_s  *pvid = NULL;
	unsigned int vlanIsL3intf = NPD_FALSE;
	unsigned int portIsPromis = NPD_FALSE;
	#if 0
	/******************************************
	 *process below is useless, legal testing to
	 *slot/localport make sure the eth_g_index 
	 *passed here certainly has its own STRUCT:
	 *eth_port_s 
	 ********************************************/
	struct eth_port_s *portInfo = NULL;
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo){
		 syslog_ax_vlan_err(("struct eth_port_s For eth_port %d has NOT created.",eth_g_index));
		return NPD_VLAN_BADPARAM;
	}
	 syslog_ax_vlan_dbg("eth_port %d's struct eth_port_s Addr %p.",eth_g_index,portInfo);
	#endif
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNum);
	if (ret != 0) 
	{
		 syslog_ax_vlan_err("npd_vlan_interface_port_add::global_index %d convert to devPort Error.\n",eth_g_index);
		return VLAN_RETURN_CODE_BADPARAM;
	}
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = VLAN_RETURN_CODE_ERR_NONE;
		} 
		else {
			 syslog_ax_vlan_err("npd_vlan_interface_port_add:: virtual port number %d error\n",virPortNum);
			ret = VLAN_RETURN_CODE_BADPARAM;
			return ret;
		}
	}
	else
	{
		 syslog_ax_vlan_err("npd_vlan_interface_port_add:: device number %d error\n",devNum);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	/*if NOT Steps into the (vlan-config CMD node),vlan exists or not Test is wanted.*/
	/*if Steps into (vlan-config CMD node), it does NOT need test vlan node exist or not.*/
	ret = npd_check_vlan_real_exist(vlanId);
	if (TRUE != ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return NPD_VLAN_BADPARAM.*/
	}
	/*here MUST check the ETH-port is already member of the VLAN or NOT*/
	ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode);
	if (NPD_TRUE == ret) {
		/* syslog_ax_vlan_dbg("Port %d has already be the member of vlan % d\n",virPortNum,vlanId);*/
		return VLAN_RETURN_CODE_PORT_EXISTS;
	}
	 syslog_ax_vlan_dbg("eth_port index %#x is not untagged or tagged port member of vlan %d\n",eth_g_index,vlanId);
	/*check the port is Layer3 interface Or NOT,if Yes, can NOT add to vlan.*/
	ret = npd_eth_port_interface_check(eth_g_index,&ifIndex);
	if((NPD_TRUE == ret) && (ifIndex != ~0UI)){
		return VLAN_RETURN_CODE_PORT_L3_INTF;
	}
	else {
		portIsPromis = npd_check_port_promi_mode(eth_g_index);
		if(0 == isTagged){
			ethPort = npd_get_port_by_index(eth_g_index);
			if(NULL == ethPort){
				return VLAN_RETURN_CODE_ERR_GENERAL;
			}
			pvid = ethPort->funcs.func_data[ETH_PORT_FUNC_PORT_BASED_VLAN];
			if(NULL != pvid){
				if(DEFAULT_VLAN_ID != pvid->vid){
					return VLAN_RETURN_CODE_PORT_MBRSHIP_CONFLICT;
				}
			}
			if((NPD_TRUE == portIsPromis)&&(advanced_routing_default_vid != vlanId)){
	            return VLAN_RETURN_CODE_PORT_PROMIS_PORT_CANNOT_ADD2_VLAN;
			}
		}

		/* check if the vlan is l3 interface */
		ret = npd_vlan_interface_check(vlanId,&vlanIfIndex);
		if((NPD_TRUE == ret) &&(vlanIfIndex !=(~0UI))){
			vlanIsL3intf = NPD_TRUE;
		}
		if((NPD_TRUE == vlanIsL3intf)&&(!promis_port_add2_intf)){
			/*check whether the port is promiscuous mode when vlan is l3 interface */		
			if(NPD_TRUE == portIsPromis){
				/* pormiscuous mode port can't add to l3 interface vlan if promis_port_add2_intf is 0*/
				return VLAN_RETURN_CODE_PORT_PROMISCUOUS_MODE_ADD2_L3INTF;
			}
		}
		
		/*call nam_asic_vlan_entry_ports_add to set the Vlan table entry in dev.*/
		/*call npd_vlan_interface_add_port_member to do Record in SW.*/
		ret = nam_asic_vlan_entry_ports_add(devNum, vlanId, virPortNum, isTagged);
		if (VLAN_CONFIG_SUCCESS == ret) {
			if(!isTagged){
				/* fdb delete*/
				nam_fdb_table_delete_entry_with_vlan_port(1,eth_g_index);

				/* igmp port disable*/
				npd_vlan_port_igmp_snp_endis_config(NPD_FALSE,1,eth_g_index,NPD_FALSE);
			}
			/*add the port to the list*/
			ret = npd_vlan_port_member_add(vlanId,eth_g_index,isTagged);
			if(VLAN_RETURN_CODE_ERR_NONE != ret) {
				 syslog_ax_vlan_dbg("the port add to vlan port list error ret %d\n",ret);
				return ret;
			}
			syslog_ax_vlan_dbg("port(%d %d) vlan %d correct\n",devNum,virPortNum,vlanId);
			/*set vlan port ingress filter*/
			npd_vlan_set_port_vlan_ingresfilter(eth_g_index, NPD_TRUE);
			/* vlan is L3 interface,set it's attribute*/			
			if(NPD_TRUE == vlanIsL3intf) {
				/* syslog_ax_vlan_dbg("npd_vlan_interface_check::ret %d,ifindex %d ",ret,ifIndex);*/
				/*nam_asic_vlan_port_route_en(devNum,virPortNum);*/
				/*set intf attr*/
				npd_intf_vlan_set_attr_for_newport(vlanIfIndex,eth_g_index,devNum,virPortNum);
				/*if advanced-routing create sub if*/
				/*ret = NPD_VLAN_L3_INTF;*/
			}
			else 
				ret = VLAN_RETURN_CODE_ERR_NONE;
			/* syslog_ax_vlan_dbg("vlan %d NOT L3 intf,port index NO need Route enable.",vlanId);*/
		}
		else {
			ret = VLAN_RETURN_CODE_ERR_HW;
		}
		
	}

	/*if advaced-routing vlan , update vlan memebers*/
	npd_intf_update_vlan_info_to_advanced_routing_l3intf(vlanId);
	return ret;
}

/*
*/
unsigned int npd_vlan_tag_port_add_for_subif
(
	unsigned short	vlanId,
	unsigned int	eth_g_index
)
{
	unsigned int  ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned char devNum = 0,virPortNum = 0,tagMode = 0;
	unsigned int  ifIndex = ~0UI;
	unsigned int vlanIfIndex = ~0UI;
	struct eth_port_s	*ethPort = NULL;
	struct port_based_vlan_s  *pvid = NULL;
	unsigned int vlanIsL3intf = NPD_FALSE;
	unsigned int portIsPromis = NPD_FALSE;
	#if 0
	/******************************************
	 *process below is useless, legal testing to
	 *slot/localport make sure the eth_g_index 
	 *passed here certainly has its own STRUCT:
	 *eth_port_s 
	 ********************************************/
	struct eth_port_s *portInfo = NULL;
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo){
		 syslog_ax_vlan_err(("struct eth_port_s For eth_port %d has NOT created.",eth_g_index));
		return NPD_VLAN_BADPARAM;
	}
	 syslog_ax_vlan_dbg("eth_port %d's struct eth_port_s Addr %p.",eth_g_index,portInfo);
	#endif
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNum);
	if (ret != 0) 
	{
		 syslog_ax_vlan_err("npd_vlan_interface_port_add::global_index %d convert to devPort Error.\n",eth_g_index);
		return VLAN_RETURN_CODE_BADPARAM;
	}
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)) {
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = VLAN_RETURN_CODE_ERR_NONE;
		} 
		else {
			 syslog_ax_vlan_err("npd_vlan_interface_port_add:: virtual port number %d error\n",virPortNum);
			ret = VLAN_RETURN_CODE_BADPARAM;
			return ret;
		}
	}
	else
	{
		 syslog_ax_vlan_err("npd_vlan_interface_port_add:: device number %d error\n",devNum);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	/*if NOT Steps into the (vlan-config CMD node),vlan exists or not Test is wanted.*/
	/*if Steps into (vlan-config CMD node), it does NOT need test vlan node exist or not.*/
	ret = npd_check_vlan_exist(vlanId);
	if (VLAN_RETURN_CODE_VLAN_EXISTS != ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return NPD_VLAN_BADPARAM.*/
	}
	/*here MUST check the ETH-port is already member of the VLAN or NOT*/
	ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode);
	if (NPD_TRUE == ret) {
		/* syslog_ax_vlan_dbg("Port %d has already be the member of vlan % d\n",virPortNum,vlanId);*/
		return VLAN_RETURN_CODE_PORT_EXISTS;
	}
	 syslog_ax_vlan_dbg("eth_port index %#x is not untagged or tagged port member of vlan %d",eth_g_index,vlanId);
	/*check the port is Layer3 interface Or NOT,if Yes, can NOT add to vlan.*/
	ret = npd_eth_port_interface_check(eth_g_index,&ifIndex);
	if((NPD_TRUE == ret) && (ifIndex != ~0UI)){
		return VLAN_RETURN_CODE_PORT_L3_INTF;
	}
	else {
		portIsPromis = npd_check_port_promi_mode(eth_g_index);
		/* check if the vlan is l3 interface */
		ret = npd_vlan_interface_check(vlanId,&vlanIfIndex);
		if((NPD_TRUE == ret) &&(vlanIfIndex !=(~0UI))){
			vlanIsL3intf = NPD_TRUE;
		}

		/*call nam_asic_vlan_entry_ports_add to set the Vlan table entry in dev.*/
		/*call npd_vlan_interface_add_port_member to do Record in SW.*/
		ret = nam_asic_vlan_entry_ports_add(devNum, vlanId, virPortNum, NPD_TRUE);
		if (VLAN_CONFIG_SUCCESS == ret) {

			/*add the port to the list*/
			ret = npd_vlan_port_member_add(vlanId,eth_g_index,NPD_TRUE);
			if(VLAN_RETURN_CODE_ERR_NONE != ret) {
				 syslog_ax_vlan_dbg("Npd_vlan::npd_vlan_port_member_add retVal %d,ERR!",ret);
				return ret;
			}
			syslog_ax_vlan_dbg("npd_vlan_interface_port_add: device %d port %d vlan %d correct",devNum,virPortNum,vlanId);
			/*set vlan port ingress filter*/
			npd_vlan_set_port_vlan_ingresfilter(eth_g_index, NPD_TRUE);
			/* vlan is L3 interface,set it's attribute*/			
			if(NPD_TRUE == vlanIsL3intf) {
				/* syslog_ax_vlan_dbg("npd_vlan_interface_check::ret %d,ifindex %d ",ret,ifIndex);*/
				/*nam_asic_vlan_port_route_en(devNum,virPortNum);*/
				/*set intf attr*/
				npd_intf_vlan_set_attr_for_newport(vlanIfIndex,eth_g_index,devNum,virPortNum);
				/*if advanced-routing create sub if*/
				/*ret = NPD_VLAN_L3_INTF;*/
			}
			/* syslog_ax_vlan_dbg("vlan %d NOT L3 intf,port index NO need Route enable.",vlanId);*/
		 }
		 else {
			ret = VLAN_RETURN_CODE_ERR_HW;
		 }
	}

	/*if advaced-routing vlan , update vlan memebers*/
	npd_intf_update_vlan_info_to_advanced_routing_l3intf(vlanId);
	return ret;
}

/*
*/
unsigned int npd_vlan_interface_port_del_for_dynamic_trunk
(
	unsigned short	vlanId,
	unsigned int	eth_g_index,
	unsigned char	isTagged
)
{
	unsigned int ret = 0;
	unsigned char devNum = 0;
	unsigned char virPortNum = 0;
	unsigned int  ifIndex = ~0UI;
	unsigned char tagMode = 0;
	unsigned short pvid = 0;
	unsigned char needJoin = 0;
	unsigned int defaultVlanIsL3intf = NPD_FALSE;
	unsigned int defaultVlanIfindex = ~0UI;
	
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNum);
	if (ret != 0) 
	{
		 syslog_ax_vlan_err("delete port %#x from vlan %d(%s) convert to asic port error %d\n", \
		 				eth_g_index, vlanId, isTagged ? "tag":"untag", ret);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)){
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = VLAN_RETURN_CODE_ERR_NONE;
		} 
		else {
			syslog_ax_vlan_err("delete port %#x from vlan %d(%s) converted port %d error\n", \
							   eth_g_index, vlanId, isTagged ? "tag":"untag", virPortNum);
			ret = VLAN_RETURN_CODE_BADPARAM;
			return ret;
		}
	} 
	else{
		 syslog_ax_vlan_err("delete port %#x from vlan %d(%s) converted device %d error\n", \
		 					eth_g_index, vlanId, isTagged ? "tag":"untag", devNum);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*if NOT Steps into the (vlan-config CMD node),vlan exists or not Test is wanted.*/
	/*if Steps into (vlan-config CMD node), it does NOT need test vlan node exist or not,because*/
	ret = npd_check_vlan_real_exist(vlanId);
	if (TRUE != ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return.*/
	}
	
	/*here MUST check the ETH-port is already member of the VLAN or NOT*/
	ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode);
	if (NPD_FALSE == ret) {
		return VLAN_RETURN_CODE_PORT_NOTEXISTS;
		/* syslog_ax_vlan_dbg("Not a vlan member");*/
	}
	else if (isTagged != tagMode ) {
		/* syslog_ax_vlan_dbg("tagMode %s",(tagMode==NPD_TRUE)?"tagged":"untag");*/
		return VLAN_RETURN_CODE_PORT_TAG_CONFLICT;
	}

	/* check if deleted tagged port should return to default vlan untagged*/
	if(isTagged) {
		ret = npd_eth_port_get_pvid(eth_g_index, &pvid);
		if(NPD_SUCCESS != ret) {
			syslog_ax_vlan_err("delete port %#x from vlan %d(%s) get pvid error\n", \
								eth_g_index, vlanId, isTagged ? "tag":"untag");
		}
		else if(DEFAULT_VLAN_ID == pvid){
			needJoin = 1; /*need to add default vlan untagged*/
		}
	}
	else {
        needJoin = 1;
	}

	ret = nam_asic_vlan_entry_ports_del(vlanId,devNum,virPortNum, needJoin);
	if (VLAN_CONFIG_SUCCESS == ret) 
	{		
		ret = npd_vlan_port_member_del(vlanId,eth_g_index,isTagged);
		if(VLAN_RETURN_CODE_ERR_NONE != ret) {
			 syslog_ax_vlan_err("delete port %#x from vlan %d(%s) error %d\n", \
			 					eth_g_index, vlanId, isTagged ? "tag":"untag", ret);
			return ret;
		}
	}
	else {
		ret = VLAN_RETURN_CODE_ERR_HW;
	}
	return ret;
}

/*
*/
unsigned int npd_vlan_interface_port_del
(
	unsigned short	vlanId,
	unsigned int	eth_g_index,
	unsigned char	isTagged
)
{
	unsigned int ret = 0;
	unsigned char devNum = 0;
	unsigned char virPortNum = 0;
	unsigned int  ifIndex = ~0UI;
	unsigned char tagMode = 0;
	unsigned short pvid = 0;
	unsigned char needJoin = 0;
	unsigned int defaultVlanIsL3intf = NPD_FALSE;
	unsigned int defaultVlanIfindex = ~0UI;
	
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNum);
	if (ret != 0) 
	{
		 syslog_ax_vlan_err("delete port %#x from vlan %d(%s) convert to asic port error %d\n", \
		 				eth_g_index, vlanId, isTagged ? "tag":"untag", ret);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)){
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = VLAN_RETURN_CODE_ERR_NONE;
		} 
		else {
			syslog_ax_vlan_err("delete port %#x from vlan %d(%s) converted port %d error\n", \
							   eth_g_index, vlanId, isTagged ? "tag":"untag", virPortNum);
			ret = VLAN_RETURN_CODE_BADPARAM;
			return ret;
		}
	} 
	else{
		 syslog_ax_vlan_err("delete port %#x from vlan %d(%s) converted device %d error\n", \
		 					eth_g_index, vlanId, isTagged ? "tag":"untag", devNum);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*if NOT Steps into the (vlan-config CMD node),vlan exists or not Test is wanted.*/
	/*if Steps into (vlan-config CMD node), it does NOT need test vlan node exist or not,because*/
	ret = npd_check_vlan_real_exist(vlanId);
	if (TRUE != ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return.*/
	}
	
	/*here MUST check the ETH-port is already member of the VLAN or NOT*/
	ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode);
	if (NPD_FALSE == ret) {
		return VLAN_RETURN_CODE_PORT_NOTEXISTS;
		/* syslog_ax_vlan_dbg("Not a vlan member");*/
	}
	else if (isTagged != tagMode ) {
		/* syslog_ax_vlan_dbg("tagMode %s",(tagMode==NPD_TRUE)?"tagged":"untag");*/
		return VLAN_RETURN_CODE_PORT_TAG_CONFLICT;
	}
#if 0
    ret = npd_check_port_promi_mode(eth_g_index);
	if(NPD_TRUE == ret){/* pormiscuous mode port can't add to l3 interface vlan */
        return NPD_VLAN_PORT_DEL_PROMISCUOUS_PORT_DELETE;
	}
#endif

	ret = npd_vlan_interface_check(DEFAULT_VLAN_ID,&defaultVlanIfindex);
	syslog_ax_vlan_dbg("default vlan interface check the ret is %d, ifIndex is %#x \n",ret,defaultVlanIfindex);
	if((NPD_TRUE == ret) &&(defaultVlanIfindex !=(~0UI))) {
		defaultVlanIsL3intf = NPD_TRUE;
	}
#if 0
	if((defaultVlanIsL3intf)&&(!promis_port_add2_intf)){
        	ret = npd_check_port_promi_mode(eth_g_index);		
			if((NPD_TRUE == ret)&&(!isTagged)){/* pormiscuous mode port can't add to l3 interface vlan */
                return NPD_VLAN_PORT_DEL_PROMISCUOUS_PORT_DELETE;
			}
	}
#endif
		
	/* syslog_ax_vlan_dbg("port %d was the vlan member,Check vlan is L3 intf or NOT.",virPortNum);*/
	ret = npd_vlan_interface_check(vlanId,&ifIndex);
	if((NPD_TRUE == ret)&&(ifIndex !=(~0UI))) {
		/*return default attr from intf attr*/
		npd_intf_vlan_set_default_attr_for_oldport(eth_g_index,devNum,virPortNum);
		/*if advanced-routing create sub if*/
		/*return NPD_VLAN_L3_INTF;*/
	}

	/* check if deleted tagged port should return to default vlan untagged*/
	if(isTagged) {
		ret = npd_eth_port_get_pvid(eth_g_index, &pvid);
		if(NPD_SUCCESS != ret) {
			syslog_ax_vlan_err("delete port %#x from vlan %d(%s) get pvid error\n", \
								eth_g_index, vlanId, isTagged ? "tag":"untag");
		}
		else if(DEFAULT_VLAN_ID == pvid){
			needJoin = 1; /*need to add default vlan untagged*/
		}
	}
	else {
        needJoin = 1;
	}

	ret = nam_asic_vlan_entry_ports_del(vlanId,devNum,virPortNum, needJoin);
	if (VLAN_CONFIG_SUCCESS == ret) 
	{		
		ret = npd_vlan_port_member_del(vlanId,eth_g_index,isTagged);
		if(VLAN_RETURN_CODE_ERR_NONE != ret) {
			 syslog_ax_vlan_err("delete port %#x from vlan %d(%s) error %d\n", \
			 					eth_g_index, vlanId, isTagged ? "tag":"untag", ret);
			return ret;
		}
		#if 0
		if(!isTagged) {
			/*check out the port is untag member of other active vlan.*/
			/*use when there's no untag membership restriction!*/
			ret = npd_vlan_port_return_default_vlan(eth_g_index);
			if(NPD_TRUE != ret){
				/*NOT untag member of any other vlan.*/
				ret = npd_vlan_port_member_add(DEFAULT_VLAN_ID,eth_g_index,isTagged);
				if(NPD_VLAN_ERR_NONE != ret) {
					 syslog_ax_vlan_err(("Npd_vlan::port index %d return to Add into vlan 1 Error.It has already vlan1 member."));
					return ret;
				}
			}
		}
		#endif

		if(defaultVlanIsL3intf && (0 == isTagged)) {
				/* syslog_ax_vlan_dbg("npd_vlan_interface_check::ret %d,ifindex %d ",ret,ifIndex);*/
				/* nam_asic_vlan_port_route_en(devNum,virPortNum);*/
				/*set intf attr*/
				npd_intf_vlan_set_attr_for_newport(defaultVlanIfindex,eth_g_index,devNum,virPortNum);
				/*if advanced-routing create sub if*/
				/*ret = NPD_VLAN_L3_INTF;*/
		}
		else {
			ret = VLAN_RETURN_CODE_ERR_NONE;
		}		
		nam_fdb_table_delete_entry_with_vlan_port(vlanId,eth_g_index);
		
		if(0 == isTagged){
			if(igmpSnpEnable){
				npd_vlan_port_igmp_snp_endis_config(NPD_TRUE,1,eth_g_index,NPD_FALSE);
				/*no need check vlan igmp enDis. just call: npd_vlan_port_igmp_snp_endis_config*/
				npd_vlan_port_igmp_snp_endis_config(NPD_FALSE,vlanId,eth_g_index,NPD_FALSE);
			}
		}
	}
	else {
		ret = VLAN_RETURN_CODE_ERR_HW;
	}

	/*if vlan is advacned-routing, update vlan memebers*/
	npd_intf_update_vlan_info_to_advanced_routing_l3intf(vlanId);
	return ret;
}

/*
*/
unsigned int npd_vlan_tag_port_del_for_subif
(
	unsigned short	vlanId,
	unsigned int	eth_g_index
)
{
	unsigned int ret = VLAN_RETURN_CODE_ERR_NONE, retval = 0;
	unsigned char devNum = 0;
	unsigned char virPortNum = 0;
	unsigned int  ifIndex = ~0UI;
	unsigned char tagMode = 0;
	unsigned short pvid = 0;
	unsigned char needJoin = 0;
	unsigned int defaultVlanIsL3intf = NPD_FALSE;
	unsigned int defaultVlanIfindex = ~0UI;
	
	ret = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNum);
	if (ret != 0) 
	{
		 syslog_ax_vlan_err("delete port %#x from vlan %d(%s) convert to asic port error %d\n", \
		 				eth_g_index, vlanId, "tag", ret);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*test devNum virPortNum Is Legal*/
	if (CHECK_DEV_NO_ISLEGAL(devNum)){
		if(CHECK_VIRPORT_NO_ISLEGAL(virPortNum)){
			ret = VLAN_RETURN_CODE_ERR_NONE;
		} 
		else {
			syslog_ax_vlan_err("delete port %#x from vlan %d(%s) converted port %d error\n", \
							   eth_g_index, vlanId, "tag", virPortNum);
			ret = VLAN_RETURN_CODE_BADPARAM;
			return ret;
		}
	} 
	else{
		 syslog_ax_vlan_err("delete port %#x from vlan %d(%s) converted device %d error\n", \
		 					eth_g_index, vlanId, "tag", devNum);
		ret = VLAN_RETURN_CODE_BADPARAM;
		return ret;
	}
	
	/*if NOT Steps into the (vlan-config CMD node),vlan exists or not Test is wanted.*/
	/*if Steps into (vlan-config CMD node), it does NOT need test vlan node exist or not,because*/
	ret = npd_check_vlan_exist(vlanId);
	if (VLAN_RETURN_CODE_VLAN_EXISTS != ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return.*/
	}
	
	/*here MUST check the ETH-port is already member of the VLAN or NOT*/
	ret = npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode);
	if (NPD_FALSE == ret) {
		return VLAN_RETURN_CODE_PORT_NOTEXISTS;
		/* syslog_ax_vlan_dbg("Not a vlan member");*/
	}
	else if (NPD_TRUE != tagMode ) {
		/* syslog_ax_vlan_dbg("tagMode %s",(tagMode==NPD_TRUE)?"tagged":"untag");*/
		return VLAN_RETURN_CODE_PORT_TAG_CONFLICT;
	}
	
	ret = npd_vlan_interface_check(DEFAULT_VLAN_ID,&defaultVlanIfindex);syslog_ax_vlan_dbg("default vlan interface check the ret is %d, ifIndex is %#x \n",ret,defaultVlanIfindex);
	if((NPD_TRUE == ret) &&(defaultVlanIfindex !=(~0UI))) {
		defaultVlanIsL3intf = NPD_TRUE;
	}

	/* syslog_ax_vlan_dbg("port %d was the vlan member,Check vlan is L3 intf or NOT.",virPortNum);*/
	ret = npd_vlan_interface_check(vlanId,&ifIndex);
	if((NPD_TRUE == ret)&&(ifIndex !=(~0UI))) {
		/*return default attr from intf attr*/
		npd_intf_vlan_set_default_attr_for_oldport(eth_g_index,devNum,virPortNum);
		/*if advanced-routing create sub if*/
		/*return NPD_VLAN_L3_INTF;*/
	}

	/* check if deleted tagged port should return to default vlan untagged*/
	
		ret = npd_eth_port_get_pvid(eth_g_index, &pvid);
		if(NPD_SUCCESS != ret) {
			syslog_ax_vlan_err("delete port %#x from vlan %d(%s) get pvid error\n", \
								eth_g_index, vlanId, "tag");
		}
		else if(DEFAULT_VLAN_ID == pvid){
			needJoin = 1; /*need to add default vlan untagged*/
		}


	ret = nam_asic_vlan_entry_ports_del(vlanId,devNum,virPortNum, needJoin);
	if (VLAN_CONFIG_SUCCESS == ret) 
	{		
		
		ret = npd_vlan_port_member_del(vlanId,eth_g_index,NPD_TRUE);
		if(VLAN_RETURN_CODE_ERR_NONE != ret) {
			 syslog_ax_vlan_err("delete port %#x from vlan %d(%s) error %d\n", \
			 					eth_g_index, vlanId, "tag", ret);
			return ret;
		}
		#if 0
		if(!isTagged) {
			/*check out the port is untag member of other active vlan.*/
			/*use when there's no untag membership restriction!*/
			ret = npd_vlan_port_return_default_vlan(eth_g_index);
			if(NPD_TRUE != ret){
				/*NOT untag member of any other vlan.*/
				ret = npd_vlan_port_member_add(DEFAULT_VLAN_ID,eth_g_index,isTagged);
				if(NPD_VLAN_ERR_NONE != ret) {
					 syslog_ax_vlan_err(("Npd_vlan::port index %d return to Add into vlan 1 Error.It has already vlan1 member."));
					return ret;
				}
			}
		}
		#endif
		ret = VLAN_RETURN_CODE_ERR_NONE;
		nam_fdb_table_delete_entry_with_vlan_port(vlanId,eth_g_index);
	}
	else {
		ret = VLAN_RETURN_CODE_ERR_NONE;
	}

	/*if vlan is advacned-routing, update vlan memebers*/
	npd_intf_update_vlan_info_to_advanced_routing_l3intf(vlanId);
	return ret;
}

int npd_vlan_trunk_entity_add
(
	unsigned short	vlanId,
	unsigned short	trunkId,
	unsigned char	trunkTag
)
{
	unsigned int ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned int trunkMbrBmp0 = 0, trunkMbrBmp1 = 0;
	unsigned char tagMode = 0;

	/*unsigned short brgVid[1] = {0};
	//unsigned short dot1qVid[NPD_VLAN_NUMBER_MAX] = {0};
	//unsigned int brgVlanCnt = 1;
	//unsigned int dot1qVlanCnt = NPD_VLAN_NUMBER_MAX;*/
	
	struct trunk_s	*trunkNode = NULL;

	unsigned int i = 0, j = 0, trunkMbrIndex[24] = {0}, trunkMbrCnt = 0;
	ret = npd_check_vlan_exist(vlanId);
	if (VLAN_RETURN_CODE_VLAN_EXISTS != ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return NPD_VLAN_BADPARAM.*/
	}
	ret = npd_check_trunk_exist(trunkId);
	if(TRUNK_RETURN_CODE_TRUNK_EXISTS != ret) {
		return TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;		
	}
	trunkNode = npd_find_trunk(trunkId);
	
	/*here MUST check this trunk is already member of this VLAN or NOT*/
	ret = npd_vlan_check_contain_trunk(vlanId,trunkId,&tagMode);
	if (NPD_TRUE == ret) {
		/* syslog_ax_vlan_dbg("Trunk %d has already be the member of vlan % d\n",trunkId,vlanId);*/
		return VLAN_RETURN_CODE_TRUNK_EXISTS;
	}
	
	/*check this trunk is untag member of other vlans*/
	if(0 == trunkTag && 1 != vlanId) {
		for(i=2;i<NPD_VLAN_NUMBER_MAX ;i++) {
			if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i) && i != vlanId) {
				ret = npd_vlan_check_trunk_membership(i, trunkId, NPD_FALSE);
				if(NPD_TRUE == ret ) {
					syslog_ax_vlan_dbg("Trunk %d is untagged member of vlan %d\n",trunkId,vlanId);
					return VLAN_RETURN_CODE_TRUNK_CONFLICT;
				}
				else {
					ret = VLAN_RETURN_CODE_ERR_NONE;
				}
			}
		}

		if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId, trunkMbrIndex, &trunkMbrCnt)) {
			/*if trunk add to vlan as untagged, delete the trunk from default vlan 1*/
			npd_vlan_trunk_member_del(1, trunkId,trunkTag);
			npd_trunk_vlan_list_delfrom(trunkId,1,trunkTag);		
			
			for(i = 0; i<trunkMbrCnt; i++) {
				syslog_ax_vlan_dbg("delete trunk %d port %d from vlan %d %s.\n",
									trunkId,trunkMbrIndex[i],1,(trunkTag)?"tag":"untag");
				npd_vlan_interface_port_del(1, trunkMbrIndex[i], trunkTag);
			}
		}
	}

	#if 0
	trunkMbrBmp0 = trunkNode->portsMbrBmp.mbrBmp0;
	trunkMbrBmp1 = trunkNode->portsMbrBmp.mbrBmp1;
	nam_asic_trunk_get_port_member_bmp(trunkId,&trunkMbrBmp0,&trunkMbrBmp1);
	ret = nam_asic_vlan_entry_trunk_mbr_add(vlanId, \
											trunkId, \
											trunkMbrBmp0, \
											trunkMbrBmp1,	\
											trunkTag);
	if(ret != 0){
		 syslog_ax_vlan_err("add trunk %d  to vlan %d error %d\n",
								trunkId,vlanId,ret);
		ret = NPD_VLAN_ERR_HW;
	}
	#endif
		 

	if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId, trunkMbrIndex, &trunkMbrCnt) || 1 == vlanId){
	 	/*to do some sw manitain*/
		/*for example hold a list of ports which belongs to this vlan in NPD */
		/*add the port to the list*/		
		npd_vlan_trunk_member_add(vlanId, trunkId,trunkTag);
		npd_trunk_vlan_list_addto(trunkId,vlanId, trunkTag);		
		for(i = 0; i < trunkMbrCnt; i++) {
			syslog_ax_vlan_dbg("add trunk %d port %#x to vlan %d %s\n",
								trunkId,trunkMbrIndex[i],vlanId,(trunkTag)?"tag":"untag");
			ret = npd_vlan_interface_port_add(vlanId, trunkMbrIndex[i], trunkTag);
			/*npd_vlan_interface_port_add(1, trunkMbrIndex[i], trunkTag);*/

			#if 0
			ret = npd_eth_port_get_vlan_all(trunkMbrIndex[i],brgVid,dot1qVid,&brgVlanCnt,&dot1qVlanCnt);
			if(brgVlanCnt){
				syslog_ax_vlan_dbg("delete port %#x from untag vlan %d\n",	\
						trunkMbrIndex[i],brgVid[0]);
				npd_vlan_port_member_del(brgVid[0],trunkMbrIndex[i],NPD_FALSE);
			}
			if(dot1qVlanCnt){
				for(j = 0; j < dot1qVlanCnt; j++){
					syslog_ax_vlan_dbg("delete port %#x from tag vlan %d\n",	\
						trunkMbrIndex[i],dot1qVid[j]);
					npd_vlan_port_member_del(dot1qVid[j],trunkMbrIndex[i],NPD_TRUE);
				}
			}		
			#endif
		}
	}
	else{
		syslog_ax_vlan_err("trunk %d has no port member in it\n",trunkId);
		return VLAN_RETURN_CODE_TRUNK_MEMBER_NONE;
	}		
	
	return VLAN_RETURN_CODE_ERR_NONE;
}

int npd_vlan_trunk_entity_del
(
	unsigned short	vlanId,
	unsigned short	trunkId,
	unsigned char 	trunkTag
)
{
	unsigned int  ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned int  trunkMbrBmp0 = 0,trunkMbrBmp1 = 0;
	unsigned char tagMode = 0;
	unsigned int i,trunkMbrIndex[24] = {0},trunkMbrCnt = 0;
	struct trunk_s *trunkNode =NULL;
	
	ret = npd_check_vlan_exist(vlanId);
	if (VLAN_RETURN_CODE_VLAN_EXISTS != ret) {
		return VLAN_RETURN_CODE_VLAN_NOT_EXISTS;/*can NOT directed Return NPD_VLAN_BADPARAM.*/
	}
	ret = npd_check_trunk_exist(trunkId);
	if(TRUNK_RETURN_CODE_TRUNK_EXISTS != ret) {
		return TRUNK_RETURN_CODE_TRUNK_NOTEXISTS;	
	}
	trunkNode = npd_find_trunk(trunkId);
	
	/*here MUST check the trunk is already member of the VLAN or NOT*/
	ret = npd_vlan_check_contain_trunk(vlanId,trunkId,&tagMode);
	if (NPD_FALSE == ret) 
	{
		/* syslog_ax_vlan_dbg("Trunk %d has NOT be the member of vlan % d\n",trunkId,vlanId);*/
		return VLAN_RETURN_CODE_TRUNK_NOTEXISTS;
	}
	else if(NPD_TRUE == ret && tagMode != trunkTag)
	{
		return VLAN_RETURN_CODE_TRUNK_MBRSHIP_CONFLICT;
	}

	#if 0
	trunkMbrBmp0 = trunkNode->portsMbrBmp.mbrBmp0;
	trunkMbrBmp1 = trunkNode->portsMbrBmp.mbrBmp1;
	ret = nam_asic_vlan_entry_trunk_mbr_del(vlanId,\
										trunkId, \
										trunkMbrBmp0, \
										trunkMbrBmp1, \
										trunkTag);
	if(ret != 0){
		 syslog_ax_vlan_err("npd_vlan_trunk_member_del::trunk %d port delete from vlan %d Error.\n",\
								trunkId,vlanId);
		ret = NPD_VLAN_ERR_HW;
	}
	#endif
		 
	/*delete the port to the list*/
	npd_vlan_trunk_member_del(vlanId, trunkId,trunkTag);
	npd_trunk_vlan_list_delfrom(trunkId,vlanId,trunkTag);
	ret = npd_vlan_trunk_qinq_disable(trunkId,vlanId);
	if(ret != VLAN_RETURN_CODE_ERR_NONE)
	{
		syslog_ax_vlan_dbg("npd vlan trunk qinq disable fail !\n");
	}
	if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId, trunkMbrIndex, &trunkMbrCnt)){
		for(i = 0; i<trunkMbrCnt;i++){
			syslog_ax_vlan_dbg("npd_vlan_trunk_entity_del:: delete trunk %d mbrport %d to vlan %d %s.",
								trunkId,trunkMbrIndex[i],vlanId,(trunkTag)?"tag":"untag");
			npd_vlan_interface_port_del(vlanId, trunkMbrIndex[i], trunkTag);
			/*npd_vlan_interface_port_del(1, trunkMbrIndex[i], trunkTag);*/
		}
	}
	/*if trunk delete from vlan as untagged, add this trunk to the default vlan 1 as untagged*/
	if(NPD_FALSE == trunkTag) {
		if(NPD_TRUE == npd_trunk_member_port_index_get_all(trunkId, trunkMbrIndex, &trunkMbrCnt)) {
			npd_vlan_trunk_member_add(1, trunkId,trunkTag);
			npd_trunk_vlan_list_addto(trunkId,1, trunkTag);		
			for(i = 0; i<trunkMbrCnt;i++) {
				syslog_ax_vlan_dbg("add trunk %d port %#x to vlan %d %s\n",
									trunkId,trunkMbrIndex[i],vlanId,(trunkTag)?"tag":"untag");
				npd_vlan_interface_port_add(1, trunkMbrIndex[i], trunkTag);
			}
		}
	}

	/*flush all fdb item according vlan_trunk*/
	/*nam_fdb_table_delete_entry_with_vlan_trunk(vlanId,trunkId);*/
	return VLAN_RETURN_CODE_ERR_NONE;   /* changed */
}


/* delete vlan interface node*/
/* by RE-init the struct vlan_s to be ZERO*/
/**********************************************************************************
 *
 * delete vlan by VLAN ID on both SW and HW side
 *
 *
 *	INPUT:
 *		vlanId - vlan id
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_VLAN_ERR_GENERAL - if error occurred when create vlan in SW side 
 *		NPD_VLAN_ERR_HW - if error occurred when create vlan in HW side
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_interface_destroy_node
(
	unsigned short vlanId
)
{
	unsigned int	i = 0, ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned int	untagmbrCount = 0;
	unsigned int	tagmbrCount = 0;
	unsigned int	eth_g_index[64] = {0};
	unsigned int  	ifIndex;
	unsigned char isTagged = 0;
	unsigned char devNum=0, portNum= 0;
	unsigned int slot_index = 0, slot_no = 0, port_index = 0, port_no = 0;
	unsigned int disna = 0, autolearn = 1;
	unsigned int vlanEnFlag = 0;
	static unsigned short vid[NPD_VLAN_NUMBER_MAX] = {0};
	struct vlan_s	*vlanNode = NULL;
	struct trunk_s  *trunkNode = NULL;
	struct vlan_ports untagPorts,tagPorts;
	unsigned char virPortNo = 0, needJoin = 0;
	unsigned int j = 0;
	int flag = 0;
	/*
	 *add vlan entry config op
	 *this op set hw chip
	 *check the vlan already exist::find the vlanid info in sw record not to read the dev reg.
	 *cause that if we have not config the vlan ,there exists only vlan 1.
	 *Once we config a vlan,we must update the vlan info in sw.
	 *So if we detect the vlan to be config has aready exist,we return vlanId and enter the next Command
	 *node,to config the vlan, for example add or delete the port member.
	 *And if we detect the vlan does not exist,we should call func 'nam_asic_vlan_entry_active' to init vlan.And then
	 *enter next Command node,to config vlan,for example add or delete the port member.
	*/
	syslog_ax_vlan_dbg("npd_vlan_interface_destroy_node vlan %d\n",vlanId);
	ret = npd_check_vlan_exist(vlanId);
	if(ret == VLAN_RETURN_CODE_VLAN_NOT_EXISTS)
	{ 
		/* syslog_ax_vlan_dbg("This Vlan inteface does NOT exist.\n");*/
	 	return ret;  
	 }
	ret = npd_vlan_interface_check(vlanId,&ifIndex);
	if((NPD_TRUE == ret)&&(ifIndex !=(~0UI))) {
		/* syslog_ax_vlan_dbg("vlan %d is L3 interface,can NOT delete it here.",vlanId);*/
		return VLAN_RETURN_CODE_L3_INTF;
	}
	ret = npd_vlan_get_interface_ve_flag(vlanId, &flag);
	if(NPD_SUCCESS == ret && flag == VLAN_PORT_SUBIF_EXIST) {
		ret = VLAN_RETURN_CODE_SUBINTF_EXISTS;
		return ret;
	}
	else 
	{
		if(((NPD_PORT_L3INTF_VLAN_ID - 1) == vlanId)&&\
			(!(npd_find_vlan_by_vid(vlanId)->isAutoCreated))){
			memset (&untagPorts,0,sizeof(struct vlan_ports));
			memset (&tagPorts,0,sizeof(struct vlan_ports));
			/*get all ports in this vlan*/
			ret = npd_vlan_get_all_ports(vlanId,&untagPorts,&tagPorts);
			
			/*loop the number of untagPorts*/
			for(j = 0;j < untagPorts.count;j ++) {
				devNum = 0;
				virPortNo =0;
				needJoin = 1;/*port delete from vlan as untagged need to add to default vlan*/
				if(!npd_check_port_promi_mode(untagPorts.ports[j])){
					/*get ports device number and virtual port info from ethernet global index*/
					ret = npd_get_devport_by_global_index(untagPorts.ports[j],&devNum,&virPortNo);
					if(NPD_SUCCESS == ret){
						/*delete this(these) port(s) from this vlan*/
						ret = nam_asic_vlan_entry_ports_del(vlanId,devNum,virPortNo,needJoin);
					}
				}
			}

			/*loop the number of tagPorts*/ 	
			for(j = 0;j < tagPorts.count;j ++) {
				devNum = 0;
				virPortNo = 0;
				needJoin = 0;/*port delete from vlan as tagged do not add to default vlan*/
				/*get ports device number and virtual port info from ethernet global index*/			
				ret = npd_get_devport_by_global_index(tagPorts.ports[j],&devNum,&virPortNo);
				if(NPD_SUCCESS == ret){
					/*delete this(these) port(s) from this vlan*/
					ret = nam_asic_vlan_entry_ports_del(vlanId,devNum,virPortNo,needJoin);
				}
			}
		}
		else{
			ret = nam_asic_vlan_entry_delete(vlanId);
			if (VLAN_CONFIG_SUCCESS != ret) {
				 syslog_ax_vlan_err("Npd_vlan::nam_asic_vlan_entry_delete:: vlanId %d ERROR ret %d.\n",vlanId,ret);
				ret = VLAN_RETURN_CODE_ERR_HW;
				return ret;
				/*ret = NPD_DBUS_ERROR; in this case dbus message can not reply*/ 
			}
		}

		ret = npd_vlan_member_port_index_get(vlanId,0,eth_g_index,&untagmbrCount);
		if(NPD_TRUE == ret){
			for(i=0;i<untagmbrCount;i++){
				npd_vlan_check_contain_port(vlanId,eth_g_index[i],&isTagged);
                #if 0
				/* set each port NA msg to cpu disable first*/
				npd_get_devport_by_global_index(eth_g_index[i],&devNum,&portNum);
				slot_index = npd_get_slot_index_from_eth_index(eth_g_index[i]);
				slot_no = npd_get_slot_no_from_index(slot_index);
				port_index = npd_get_port_index_from_eth_index(eth_g_index[i]);
				port_no = npd_get_port_no_from_index(slot_index,port_index);
				ret = npd_fdb_na_msg_per_port_set(slot_no,port_no,disna);
                if(0 != ret){
                    syslog_ax_vlan_err("npd_vlan_interface_destroy_node::set Na msg disable error\n");
					return NPD_VLAN_ERR_HW;
				}
				/* set autolearn enable on the port*/
				ret = npd_fdb_set_port_learn_status(slot_no,port_no,autolearn);
		        if(0!=ret){
		            syslog_ax_vlan_err("npd_vlan_interface_destroy_node:npd_fdb_set_port_learn_status err,%d \n",ret);
                    return  NPD_VLAN_ERR_HW;
				}
				/* cancel FDB port limit*/
				ret = npd_fdb_number_port_set(slot_no,port_no,0);
				#endif
				if(((NPD_PORT_L3INTF_VLAN_ID - 1) == vlanId)&&\
					(advanced_routing_default_vid == vlanId)&&\
					(npd_check_port_promi_mode(eth_g_index[i]))){
					continue;
				}
				/* disable DLDP on port in vlan, before add to default vlan */
				ret = npd_dldp_port_del(vlanId, eth_g_index[i], isTagged);
				if (ret) {
					syslog_ax_vlan_dbg("dldp delete port %#x from default vlan, ret %x\n",
										eth_g_index[i], ret);
					ret = VLAN_RETURN_CODE_ERR_NONE;
				}

				npd_vlan_port_member_del(vlanId,eth_g_index[i], isTagged);/*if not,it'll lead memory leak*/
				npd_vlan_port_member_add(DEFAULT_VLAN_ID, eth_g_index[i], 0);

                /*add to enalbe port igmp snooping when the default vlan is igmp snooping enabled. add by yangxs@autelan.com*/
				vlanEnFlag = 0;
				ret = npd_check_igmp_snp_vlan_status(DEFAULT_VLAN_ID, &vlanEnFlag);
				if(NPD_IGMP_SNP_SUCCESS == ret) {
					if(1 == vlanEnFlag) {
						ret = npd_vlan_port_igmp_snp_endis_config(1, DEFAULT_VLAN_ID, eth_g_index[i],isTagged);
						if(NPD_SUCCESS != ret){
							syslog_ax_vlan_dbg("fail to config npd vlan port igmp snp endis!");
						}
					}
				}else {
					syslog_ax_vlan_dbg("fail to npd check igmp snp vlan status when delete vlan.\n");
				}

				/* enable DLDP on port in default vlan */
				if (isTagged == 0) {/* need deal when port tag mode is untag*/
					ret = npd_dldp_port_add(1, eth_g_index[i], isTagged);
					if (ret) {
						syslog_ax_vlan_dbg("dldp add port %#x from to vlan %d, ret %x\n",
											eth_g_index[i], vlanId, ret);
						ret = VLAN_RETURN_CODE_ERR_NONE;
					}
				}
			}
		}
		ret = npd_vlan_member_port_index_get(vlanId,NPD_TRUE,eth_g_index,&tagmbrCount);
		if(NPD_TRUE == ret){
            for(i = 0;i<tagmbrCount;i++){
				/* disable DLDP on port in vlan */
				ret = npd_vlan_check_contain_port(vlanId, eth_g_index[i], &isTagged);
				ret = npd_dldp_port_del(vlanId, eth_g_index[i], isTagged);
				if (ret) {
					syslog_ax_vlan_dbg("dldp delete port %#x from default vlan, ret %x\n",
										eth_g_index[i], ret);
					ret = VLAN_RETURN_CODE_ERR_NONE;
				}

            }
		}

		/*if vlan configured IGMP snooping enabled, notify the igmp_snoop process
		 *vlan port member already delete,no need to notify igmp_snoop which port IGMP disabled.
		 */
		vlanNode = npd_find_vlan_by_vid(vlanId);
		if(vlanNode->igmpSnpEnDis){
			npd_vlan_igmp_snp_endis_config(NPD_FALSE,vlanId);
		}

  		/*add by qily@autelan.com 08 12_08*/
		/*to update trunk info data struct.*/
		for (i = 0; i<128;i++){
			ret = npd_check_trunk_exist((unsigned short)i);
			if(TRUNK_RETURN_CODE_TRUNK_EXISTS == ret){
				trunkNode = npd_find_trunk((unsigned short)i);
				if(NULL != trunkNode){
					if(NPD_TRUE == npd_trunk_check_vlan_allowship(i, vlanId)){
						vid[0] = vlanId;
						npd_vlan_check_contain_trunk(vlanId,i,&isTagged);
						npd_trunk_refuse_vlan(i,1,vid,isTagged);							
					}
				}
			}
		}
		
		/*to update the vlan info data struct in sw */
		/*vlan active op ,need to modify the vid_list only*/
		ret = npd_delete_vlan_by_vid(vlanId);
		if (VLAN_RETURN_CODE_ERR_NONE != ret) {
			 syslog_ax_vlan_err("Npd_vlan::npd_delete_vlan_by_vid:vlanId %d Error\n",vlanId);
			return ret;
		}
#if 0
  		/*add by qily@autelan.com 08 11 24*/
		/*to update trunk info data struct.*/
		for (i = 0; i<128;i++){
			ret = npd_check_trunk_exist((unsigned short)i);
			if(NPD_TRUNK_EXISTS == ret){
				trunkNode = npd_find_trunk((unsigned short)i);
				if(NULL != trunkNode){
					if(NPD_TRUE == npd_trunk_check_vlan_allowship(i, vlanId)){
						vid[0] = vlanId;
						if(NPD_TRUE == npd_trunk_allow_vlan_tagmode_get(i,tag,&vCount)){
							if(vCount>0){
								for(i=0;i<vCount;i++){
									tAllowVlan[i] = (trunkAllowVlan *)malloc(sizeof(trunkAllowVlan));
									memset(tAllowVlan[i],0,sizeof(trunkAllowVlan));
									tAllowVlan[i]->vlanId = vid[i];
									tAllowVlan[i]->tagMode = tag[i];
									vlanNode = npd_find_vlan_by_vid(vid[i]);
									tAllowVlan[i]->vlanName = vlanNode->vlanName;
									//NPD_DEBUG(("trunk %d allows vlan %d,vlanName %s.",trunkId,vid[i],vlanNode->vlanName));
								}
							}
							npd_trunk_refuse_vlan(i,1,vid);							
						}
					}
				}
			}
		}
#endif

		/* syslog_ax_vlan_dbg("delete fdb of vlan %d",vlanId);*/
		nam_fdb_table_delete_entry_with_vlan(vlanId);
		/* syslog_ax_vlan_dbg("delete fdb of vlan %d",vlanId);	*/	

		/*delete fdb blacklist*/
		npd_fdb_static_blacklist_entry_del_with_vlan(vlanId);
	}

	return ret;
}

void npd_save_vlan_cfg(char* buf,int bufLen)
{
    if((PRODUCT_ID == PRODUCT_ID_AX7K_I)||(SYSTEM_TYPE == IS_DISTRIBUTED))
	{
    	char *showStr = buf,*cursor = NULL;
    	int totalLen = 0;
    	int i = 0,j = 0,k = 0,m = 0,n = 0, slot = 0,port = 0;
    	struct vlan_s* node = NULL;
    	struct vlan_port_list_node_s *portNode = NULL;
    	vlan_port_list_s* portList = NULL;
    	struct list_head *pos = NULL,*list  = NULL;
    	unsigned int eth_g_index = 0;
    	unsigned char slot_index = 0,slot_no = 0,local_port_no = 0;
    	int rc = NPD_FALSE;
    	enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;

    	struct eth_port_s* ethPort = NULL;
    	struct port_based_trunk_s* pTrkId = NULL;
    	struct vlan_trunk_list_node_s *trunkNode = NULL;
    	vlan_trunk_list_s* trunkMemList = NULL;
    	unsigned short trunk_id = 0;
    	struct vlan_acl_info_s		 *aclVlanList=NULL;	
    	unsigned int groupId=0,egrGroupId=0,ret;
    	
    	cursor = showStr;
        /* vlan2 --> vlan4093 */
    	for(i = 1; i < 4093; i++) 
		{
			/* slot(7) * port(26) * length( 22 bytes ) = 4004 bytes bugfix: AXSSZFI-692 */
    		char tmpBuf[6500] = {0},*tmpPtr = NULL;   
    		int length = 0;
    		tmpPtr = tmpBuf;
    		int cfgflag = 0;
    		unsigned int ifIndex = ~0UI;
    		if(g_vlanlist[i].vlanStat == 1) 
    		{
				if((length + 30) < 4096)
				{
					length += sprintf(tmpPtr,"create vlan %d %s\n",i+1,g_vlanlist[i].vlanName);
					syslog_ax_vlan_dbg("%s\n",tmpPtr);
					tmpPtr = tmpBuf+length;
				}
                unsigned char is_config = 0;                
                /* untag port */
    			for(j = 0; j < CHASSIS_SLOT_COUNT; j++ )
    			{
        			for(k = 0; k < 64; k++ )
        			{
						if(k<32)
						{
                            if((g_vlanlist[i].untagPortBmp[j].low_bmp)&(1<<k))
                        	{
								is_config ++; 
        						slot = j+1;
        						port = k+1;
								/* into config node only one time */
								if(is_config ==1)
								{
            						if((length + 18) < 4096) 
									{
            							length += sprintf(tmpPtr,"config vlan %d\n",i+1);
            							syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
            							tmpPtr = tmpBuf + length;
            						}																		
								}
								/* add port to vlan */
								if((length + 20 )< 4096)
								{
									length += sprintf(tmpPtr," add port %d/%d untag\n",slot,port);
									syslog_ax_vlan_dbg("%s",tmpPtr);
									tmpPtr = tmpBuf + length;
								}
								
                        	}
        					else
        					{
        						continue;
        					}
						}
						else
						{
                            if((g_vlanlist[i].untagPortBmp[j].high_bmp)&(1<<(k-32)))
                        	{
							    is_config ++; 	
        						slot = j+1;
        						port = k+1;
								/* into config node only one time */
								if(is_config ==1)
								{
            						if((length + 18) < 4096) 
									{
            							length += sprintf(tmpPtr,"config vlan %d\n",i+1);
            							syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
            							tmpPtr = tmpBuf + length;
            						}																		
								}
								/* add port to vlan */								
								if((length + 20 )< 4096)
								{
									length += sprintf(tmpPtr," add port %d/%d untag\n",slot,port);
									syslog_ax_vlan_dbg("%s",tmpPtr);
									tmpPtr = tmpBuf + length;
								}																
                        	}
        					else
        					{
        						continue;
        					}								
						}
        			}				
    			}								
                /* tag port */
    			for(j = 0; j < CHASSIS_SLOT_COUNT; j++ )
    			{
        			for(k = 0; k < 64; k++ )
        			{
						if(k<32)
						{
                            if((g_vlanlist[i].tagPortBmp[j].low_bmp)&(1<<k))
                        	{
								is_config ++; 
        						slot = j+1;
        						port = k+1;
								/* into config node only one time */
								if(is_config ==1)
								{
            						if((length + 18) < 4096) 
									{
            							length += sprintf(tmpPtr,"config vlan %d\n",i+1);
            							syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
            							tmpPtr = tmpBuf + length;
            						}																		
								}
								/* add port to vlan */
								if((length + 20 )< 4096)
								{
									length += sprintf(tmpPtr," add port %d/%d tag\n",slot,port);
									syslog_ax_vlan_dbg("%s",tmpPtr);
									tmpPtr = tmpBuf + length;
								}
								
                        	}
        					else
        					{
        						continue;
        					}
						}
						else
						{
                            if((g_vlanlist[i].tagPortBmp[j].high_bmp)&(1<<(k-32)))
                        	{
							    is_config ++; 	
        						slot = j+1;
        						port = k+1;
								/* into config node only one time */
								if(is_config ==1)
								{
            						if((length + 18) < 4096) 
									{
            							length += sprintf(tmpPtr,"config vlan %d\n",i+1);
            							syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
            							tmpPtr = tmpBuf + length;
            						}																		
								}
								/* add port to vlan */								
								if((length + 20 )< 4096)
								{
									length += sprintf(tmpPtr," add port %d/%d tag\n",slot,port);
									syslog_ax_vlan_dbg("%s",tmpPtr);
									tmpPtr = tmpBuf + length;
								}																
                        	}
        					else
        					{
        						continue;
        					}								
						}
        			}				
    			}

				/* vlan add trunk node */
				for(j = 0; j < MAX_STATIC_TRUNK_ID; j++ )
    			{
					/* add untag trunk */
					if((g_vlanlist[i].untagTrkBmp[j]==1))
					{
						is_config ++; 
						/* into config node only one time */
						if(is_config ==1)
						{
    						if((length + 18) < 4096) 
							{
    							length += sprintf(tmpPtr,"config vlan %d\n",i+1);
    							syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
    							tmpPtr = tmpBuf + length;
    						}																		
						}
						/* add port to vlan */
						if((length + 20 )< 4096)
						{
							length += sprintf(tmpPtr," add trunk %d untag\n",j+1);
							syslog_ax_vlan_dbg("%s",tmpPtr);
							tmpPtr = tmpBuf + length;
						}								
					}

					/* add tag trunk */					
                    if((g_vlanlist[i].tagTrkBmp[j]==1))
                	{
						is_config ++; 
						/* into config node only one time */
						if(is_config ==1)
						{
    						if((length + 18) < 4096) 
							{
    							length += sprintf(tmpPtr,"config vlan %d\n",i+1);
    							syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
    							tmpPtr = tmpBuf + length;
    						}																		
						}
						/* add port to vlan */
						if((length + 20 )< 4096)
						{
							length += sprintf(tmpPtr," add trunk %d tag\n",j+1);
							syslog_ax_vlan_dbg("%s",tmpPtr);
							tmpPtr = tmpBuf + length;
						}						
                	}			
    			}

				/* qinq node */

				for(j = 0; j < CHASSIS_SLOT_COUNT; j++ )
    			{
        			for(k = 0; k < 64; k++ )
        			{
						if(k<32)
						{
                            if((g_vlanlist[i].qinq[j].low_bmp)&(1<<k))
                        	{
								is_config ++; 
        						slot = j+1;
        						port = k+1;
								/* into config node only one time */
								if(is_config ==1)
								{
            						if((length + 18) < 4096) 
									{
            							length += sprintf(tmpPtr,"config vlan %d\n",i+1);
            							syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
            							tmpPtr = tmpBuf + length;
            						}																		
								}
								/* add port to vlan */
								if((length + 20 )< 4096)
								{
									length += sprintf(tmpPtr,"config port %d/%d qinq enable\n",slot,port);
									syslog_ax_vlan_dbg("%s",tmpPtr);
									tmpPtr = tmpBuf + length;
								}
								
                        	}
        					else
        					{
        						continue;
        					}
						}
						else
						{
                            if((g_vlanlist[i].qinq[j].high_bmp)&(1<<(k-32)))
                        	{
							    is_config ++; 	
        						slot = j+1;
        						port = k+1;
								/* into config node only one time */
								if(is_config ==1)
								{
            						if((length + 18) < 4096) 
									{
            							length += sprintf(tmpPtr,"config vlan %d\n",i+1);
            							syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
            							tmpPtr = tmpBuf + length;
            						}																		
								}
								/* add port to vlan */								
								if((length + 20 )< 4096)
								{
									length += sprintf(tmpPtr,"config port %d/%d qinq enable\n",slot,port);
									syslog_ax_vlan_dbg("%s",tmpPtr);
									tmpPtr = tmpBuf + length;
								}																
                        	}
        					else
        					{
        						continue;
        					}								
						}
        			}				
    			}
				

				if(g_vlanlist[i].cscd_qinq_state == 1)
				{
					is_config ++; 
					/* into config node only one time */
					if(is_config ==1)
					{
						if((length + 18) < 4096) 
						{
							length += sprintf(tmpPtr,"config vlan %d\n",i+1);
							syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
							tmpPtr = tmpBuf + length;
						}																		
					}
					if((length + 20)<4096)
					{
						length += sprintf(tmpPtr,"config qinq enable\n");
						syslog_ax_vlan_dbg("%s",tmpPtr);
						tmpPtr = tmpBuf + length;
					}
				}

				/* exit vlan node */
                if(is_config >0)
                {
    				if((length + 5 )< 4096)
    				{
    					length += sprintf(tmpPtr," exit\n");
    					syslog_ax_vlan_dbg("%s",tmpPtr);
    					tmpPtr = tmpBuf + length;
    					
    				}
					is_config = 0;
                }
				
                /* is bond to which slot*/
				for(m = 0; m< CHASSIS_SLOT_COUNT; m++ )
    			{
    				if(g_vlanlist[i].bond_slot[m] !=0)
    				{
						for(n=0;n<MAX_CPU_PORT_NUM;n++)
						{							
							if(g_vlanlist[i].bond_slot[m]&((unsigned int)(1<<n)))
							{
            					if((length + 25 )< 4096)
            					{
             						length += sprintf(tmpPtr,"bond vlan %d slot %d cpu %d port %d\n",i+1,m+1,(n/8)+1,(n%8)+1);
            						syslog_ax_vlan_dbg("%s",tmpPtr);
            						tmpPtr = tmpBuf + length;  									
            					}
							}	
							if(g_vlanlist[i].cpu_port_qinq[m]&((unsigned int)(1<<n)))
							{
								if((length + 50)<4096)
								{
									length += sprintf(tmpPtr,"set vlan %d slot %d cpu %d port %d qinq enable\n",i+1,m+1,(n/8)+1,(n%8)+1);
									syslog_ax_vlan_dbg("%s",tmpPtr);
									tmpPtr = tmpBuf + length;
								}
							}
						}
    				}
				}
#if 0
				if(g_vlanlist[i].to_cpu_port !=0)
				{
					if((length + 45 )<4096)
					{
						length += sprintf(tmpPtr,"set vlan %d to-cpu-port qinq enable\n",i+1);
						syslog_ax_vlan_dbg("%s",tmpPtr);
						tmpPtr = tmpBuf + length;
    				}
				}
#endif			
				if((totalLen +length) > bufLen) { /* config string is > NPD_VLAN_RUNNING_CFG_MEM + advVidStrLen  */
					syslog_ax_vlan_err("save vlan buffer full\n");
					break;
				}
				
				totalLen += sprintf(cursor,"%s",tmpBuf);
				cursor = showStr + totalLen;
				
    		}
    	}
    	/*save vlan mtu*/
    	if((totalLen + 30) < bufLen){
    		if(NPD_VLAN_MTU_VALUE != vlan_mtu) {
    			totalLen += sprintf(cursor,"config vlan mtu %d\n",vlan_mtu);
    			cursor = showStr + totalLen;
    		}
    	}
        /* config advanced-routing default-vid , it should be put under the vlan create */
        /* add more ... */
	}
	else
	{
    	char *showStr = buf,*cursor = NULL;
    	int totalLen = 0;
    	int i = 0;
    	struct vlan_s* node = NULL;
    	struct vlan_port_list_node_s *portNode = NULL;
    	vlan_port_list_s* portList = NULL;
    	struct list_head *pos = NULL,*list  = NULL;
    	unsigned int eth_g_index = 0;
    	unsigned char slot_index = 0,slot_no = 0,local_port_no = 0;
    	int rc = NPD_FALSE;
    	enum VLAN_PORT_SUBIF_FLAG flag = VLAN_PORT_SUBIF_NOTEXIST;

    	struct eth_port_s* ethPort = NULL;
    	struct port_based_trunk_s* pTrkId = NULL;
    	struct vlan_trunk_list_node_s *trunkNode = NULL;
    	vlan_trunk_list_s* trunkMemList = NULL;
    	unsigned short trunk_id = 0;
    	struct vlan_acl_info_s		 *aclVlanList=NULL;	
    	unsigned int groupId=0,egrGroupId=0,ret;
    	
    	cursor = showStr;

    	for(i = 2; i < NPD_MAX_VLAN_ID; i++) {
    		char tmpBuf[2048] = {0},*tmpPtr = NULL;
    		int length = 0;
    		tmpPtr = tmpBuf;
    		int cfgflag = 0;
    		unsigned int ifIndex = ~0UI;
    		
    		if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i)) {
    			node = npd_find_vlan_by_vid(i);
    			if(NULL != node) {
    				if((length + 30) < 2048) {
    					length += sprintf(tmpPtr,"create vlan %d %s\n",i,node->vlanName);
    					 syslog_ax_vlan_dbg("%s\n",tmpPtr);
    					tmpPtr = tmpBuf+length;
    				}
    					
    				portList = node->untagPortList;
    				if(NULL != portList) {
    					if(0 != portList->count) {
    						if((length + 18) < 2048) {
    							length += sprintf(tmpPtr,"config vlan %d\n",i);
    							 syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
    							tmpPtr = tmpBuf + length;
    							cfgflag = 1;
    						}
    						
    						list = &(portList->list);
    						__list_for_each(pos,list) {
    							portNode = list_entry(pos,struct vlan_port_list_node_s,list);
    							if(portNode) {
    								eth_g_index = portNode->eth_global_index;
    								slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    								ethPort = npd_get_port_by_index(eth_g_index);
    								if(NULL != ethPort ){
    									pTrkId = ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION];
    									if(NULL != pTrkId)
    										continue;
    								}
    								slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
    								local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    								local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_no);
                                    if(npd_check_port_promi_mode(eth_g_index)){
                                        continue;
                                    }
    								if((length + 20 )< 2048) {
    									length += sprintf(tmpPtr," add port %d/%d untag\n",slot_no,local_port_no);
    									 syslog_ax_vlan_dbg("%s",tmpPtr);
    									tmpPtr = tmpBuf + length;
    								}
    							}
    						}
    					}
    				}

    				portList = node->tagPortList;
    				if(NULL != portList) {
    					if(0 != portList->count) {
    						if(0 == cfgflag && ((length + 18) < 2048)) {
    							length += sprintf(tmpPtr,"config vlan %d\n",i);
    							 syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
    							tmpPtr = tmpBuf + length;
    							cfgflag = 1;
    						}
    					
    						list = &(portList->list);
    						__list_for_each(pos,list) {
    							portNode = list_entry(pos,struct vlan_port_list_node_s,list);
    							if(portNode) {
    								eth_g_index = portNode->eth_global_index;
    								slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    								ethPort = npd_get_port_by_index(eth_g_index);
    								if(NULL != ethPort ){
    									pTrkId = ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION];
    									if(NULL != pTrkId)
    										continue;
    								}								
    								slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
    								local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
    								local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_no);

    								if((length + 20) < 2048) {
    									length += sprintf(tmpPtr," add port %d/%d tag\n",slot_no,local_port_no);
    									 syslog_ax_vlan_dbg("%s",tmpPtr);
    									tmpPtr = tmpBuf + length;
    								}
    							}
    						}
    					}
    				}

    				/*save trunk member config*/
    				trunkMemList = node->tagTrunkList;
    				if(NULL != trunkMemList) {
    					if(0 != trunkMemList->count) {
    						if(0 == cfgflag && ((length + 18) < 2048)) {
    							length += sprintf(tmpPtr,"config vlan %d\n",i);
    							 syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
    							tmpPtr = tmpBuf + length;
    							cfgflag = 1;
    						}
    					
    						list = &(trunkMemList->list);
    						__list_for_each(pos,list) {
    							trunkNode = list_entry(pos,struct vlan_trunk_list_node_s,list);
    							if(trunkNode) {
    								trunk_id = trunkNode->trunkId;

    								if((length + 16) < 2048) {
    									length += sprintf(tmpPtr," add trunk %d tag\n",trunk_id);
    									 syslog_ax_vlan_dbg("%s",tmpPtr);
    									tmpPtr = tmpBuf + length;
    								}
    							}
    						}
    					}
    				}
    				trunkMemList = node->unTagtrunkList;
    				if(NULL != trunkMemList) {
    					if(0 != trunkMemList->count) {
    						if(0 == cfgflag && ((length + 18) < 2048)) {
    							length += sprintf(tmpPtr,"config vlan %d\n",i);
    							 syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
    							tmpPtr = tmpBuf + length;
    							cfgflag = 1;
    						}
    					
    						list = &(trunkMemList->list);
    						__list_for_each(pos,list) {
    							trunkNode = list_entry(pos,struct vlan_trunk_list_node_s,list);
    							if(trunkNode) {
    								trunk_id = trunkNode->trunkId;

    								if((length + 18) < 2048) {
    									length += sprintf(tmpPtr," add trunk %d untag\n",trunk_id);
    									 syslog_ax_vlan_dbg("%s",tmpPtr);
    									tmpPtr = tmpBuf + length;
    								}
    							}
    						}
    					}
    				}
    				/**/
    				/*acl group*/
    				aclVlanList = node->aclList;
    				if(NULL!=aclVlanList){					
    					/*check if vlan acl enable*/
    					/*ingress */
    					ret = npd_vlan_acl_enable_check(i,ACL_DIRECTION_INGRESS);
    					if(NPD_DBUS_SUCCESS==ret){
    						/*
      						 * by qinhs@autelan.com on 11-11-2008
      						 *  Enter vlan node if not entered before
    						 */
    						if(0 == cfgflag && ((length + 18) < 2048)) {
    							length += sprintf(tmpPtr,"config vlan %d\n",i);
    							tmpPtr = tmpBuf + length;
    							cfgflag = 1;
    						}
    						
    						length += sprintf(tmpPtr," ingress acl enable\n");
    						tmpPtr = tmpBuf + length;

    						ret=npd_vlan_acl_bind_check(i,&groupId,ACL_DIRECTION_INGRESS);
    						if(ACL_GROUP_VLAN_BINDED==ret){
    							length += sprintf(tmpPtr," bind ingress acl-group %d\n",groupId);
    							tmpPtr = tmpBuf + length;
    						}
    					}

    					/*egress */
    					ret = npd_vlan_acl_enable_check(i,ACL_DIRECTION_EGRESS);
    					if(NPD_DBUS_SUCCESS==ret){
    						/*
      						 * by qinhs@autelan.com on 11-11-2008
      						 *  Enter vlan node if not entered before
    						 */
    						if(0 == cfgflag && ((length + 18) < 2048)) {
    							length += sprintf(tmpPtr,"config vlan %d\n",i);
    							tmpPtr = tmpBuf + length;
    							cfgflag = 1;
    						}
    						
    						length += sprintf(tmpPtr," egress acl enable\n");
    						tmpPtr = tmpBuf + length;

    						ret=npd_vlan_acl_bind_check(i,&egrGroupId,ACL_DIRECTION_EGRESS);
    						if(ACL_GROUP_VLAN_BINDED==ret){
    							length += sprintf(tmpPtr," bind egress acl-group %d\n",egrGroupId);
    							tmpPtr = tmpBuf + length;
    						}
    					}
    				}
    				
    				if((totalLen +length + 5 ) > bufLen) { /* 5 exit */
    					 syslog_ax_vlan_err("save vlan buffer full\n");
    					break;
    				}
    				
    				totalLen += sprintf(cursor,"%s",tmpBuf);
    				cursor = showStr + totalLen;
    				if(cfgflag){
    					totalLen += sprintf(cursor," exit\n");
    					cursor = showStr + totalLen;
    				}


    				
#if 0
    				if((totalLen + 80) < bufLen) { /* interface cmd*/
    					rc = npd_vlan_get_interface_ve_flag(i,&flag);
    					if(NPD_DBUS_SUCCESS == rc && VLAN_PORT_SUBIF_EXIST == flag ) {
    						totalLen += sprintf(cursor,"interface vlan%d\n",i);
    						cursor = showStr + totalLen;
    						totalLen += sprintf(cursor," advanced-routing enable\n");
    						cursor = showStr + totalLen;
    						totalLen += sprintf(cursor," exit\n");
    						cursor = showStr + totalLen;
    					}
    					else if((NPD_TRUE == (rc = npd_vlan_interface_check(i, &ifIndex))) && (ifIndex != ~0UI)){
    						totalLen += sprintf(cursor,"interface vlan%d\n exit\n",i);
    						cursor = showStr + totalLen;
    					}

    					if(0 == npd_get_dev_ip_addr(ifIndex,&addr) && 0 != addr) {
    						totalLen += sprintf(cursor,"interface vlan%d\n",i);
    						cursor = showStr + totalLen;
    						totalLen += sprintf(cursor,"  ip address %d.%d.%d.%d/%d\n",  	\
    							(addr&0xff000000)>>24,(addr&0xff0000)>>16,  \
    							(addr&0xff00)>>8,(addr&0xff),24);
    						cursor = showStr + totalLen;
    						totalLen += sprintf(cursor,"exit\n");
    						cursor = showStr + totalLen;
    					}
    				}	
#endif
    			}
    		}
    	}
    	

#if 0
    	unsigned int ifIndex = 0;
    	/*NPD_DEBUG(("Start save vlan Interface."));*/
    	if((totalLen + 80) < bufLen) { /* interface cmd*/
    		rc = npd_vlan_get_interface_ve_flag(1,&flag);
    		if(NPD_DBUS_SUCCESS == rc && VLAN_PORT_SUBIF_EXIST == flag ) {
    			totalLen += sprintf(cursor,"interface vlan%d\n",1);
    			cursor = showStr + totalLen;
    			totalLen += sprintf(cursor," advanced-routing enable\n");
    			cursor = showStr + totalLen;
    			totalLen += sprintf(cursor," exit\n");
    			cursor = showStr + totalLen;

    		}
    	else if((NPD_TRUE == (rc = npd_vlan_interface_check(1, &ifIndex))) && ( ifIndex != ~0UI)){
    			totalLen += sprintf(cursor,"interface vlan%d\n exit\n",1);
    			cursor = showStr + totalLen;
    		}
    		
    		if(0 == npd_get_dev_ip_addr(ifIndex,&vlanIp) && 0 != vlanIp) {
    			totalLen += sprintf(cursor,"interface vlan%d\n",1);
    			cursor = showStr + totalLen;
    			totalLen += sprintf(cursor,"  ip address %d.%d.%d.%d/%d\n",		\
    				(vlanIp&0xff000000)>>24,(vlanIp&0xff0000)>>16,  \
    				(vlanIp&0xff00)>>8,(vlanIp&0xff),24);
    			cursor = showStr + totalLen;
    			totalLen += sprintf(cursor,"exit\n");
    			cursor = showStr + totalLen;
    		}
    	}
#endif

    	/*save vlan mtu*/
    	if((totalLen + 30) < bufLen){
    		if(NPD_VLAN_MTU_VALUE != vlan_mtu) {
    			totalLen += sprintf(cursor,"config vlan mtu %d\n",vlan_mtu);
    			cursor = showStr + totalLen;
    		}
    	}
        /* config advanced-routing default-vid , it should be put under the vlan create */
        int avalidLen = bufLen - totalLen;
        npd_dbus_save_advanced_routing_default_vid_cfg(&cursor,&avalidLen);
	}

}


void npd_save_vlan_igmp_snp_cfg(char* buf,int bufLen,unsigned char *enDis,unsigned char Ismld)
{
	char *showStr = buf,*cursor = NULL;
	int i = 0;
	struct vlan_s* node = NULL;
	struct vlan_port_list_node_s *portNode = NULL;
	vlan_port_list_s* portList = NULL;
	struct list_head *pos = NULL,*list  = NULL;
	unsigned int eth_g_index = 0;
	unsigned char slot_index = 0,slot_no = 0,local_port_no = 0;
	int cfgflag = 0,length = 0,rc = NPD_FALSE;

	unsigned char  status = NPD_FALSE,statusm = NPD_FALSE;
	cursor = showStr;
    int sw_config_flag = 0;

	rc= npd_check_igmp_snp_status(&status);
	rc= npd_check_mld_snp_status(&statusm);

    if((NPD_TRUE == status) && (ISIGMP == Ismld))
    {
    	/* into igmp sw-board config node */
    	length += sprintf(showStr,"config igmp-snooping sw-board %d\n",SLOT_ID);                       
		showStr = buf+length;
		sw_config_flag = 1;		
    }
	else if((NPD_TRUE == statusm) && (ISMLD == Ismld))
	{
    	/* into mld sw-board config node */
    	length += sprintf(showStr,"config mld-snooping sw-board %d\n",SLOT_ID);                       
		showStr = buf+length;
        sw_config_flag = 1;
	}
	
	if(((NPD_TRUE == status) && (ISIGMP == Ismld)) || ((NPD_TRUE == statusm) && (ISMLD == Ismld)))
	{
		*enDis = NPD_TRUE ;
		if((length + 20) < 2048) {
			if((NPD_TRUE == status) && (ISIGMP == Ismld)){
				length += sprintf(showStr," config igmp-snooping enable\n");
			}
			if((NPD_TRUE == statusm) && (ISMLD == Ismld)){
				length += sprintf(showStr," config mld-snooping enable\n");
			}			
			 syslog_ax_vlan_dbg("%s\n",showStr);
			showStr = buf+length;
		}

		for(i = 2; i < NPD_MAX_VLAN_ID; i++) {
			
			if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i)) {
				node = npd_find_vlan_by_vid(i);
				if(NULL != node) {
					if(node->igmpSnpEnDis){

						cfgflag = 1;
						if((length + 21) < 2048) {
							if((NPD_TRUE == status) && (ISIGMP == Ismld))
							{
								length += sprintf(showStr," config igmp-vlan %d\n",i);
							}
							else if((NPD_TRUE == statusm) && (ISMLD == Ismld))
							{
								length += sprintf(showStr," config mld-vlan %d\n",i);								
							}
							syslog_ax_vlan_dbg("%s\n\n",showStr);
							showStr = buf + length;
						}
								
						if((length + 35) < 2048) {
							if((NPD_TRUE == status) && (ISIGMP == Ismld)){
								length += sprintf(showStr,"  config vlan igmp-snooping enable\n");
							}
							if((NPD_TRUE == statusm) && (ISMLD == Ismld)){
								length += sprintf(showStr,"  config vlan mld-snooping enable\n");
							}
							 syslog_ax_vlan_dbg("%s\n",showStr);
							showStr = buf+length;
						}

						portList = node->untagPortList;
						if(NULL != portList) {
							if(0 != portList->count) {
								list = &(portList->list);
								__list_for_each(pos,list) {
									portNode = list_entry(pos,struct vlan_port_list_node_s,list);
									if(portNode) {
										if(portNode->igmpEnFlag){
											eth_g_index = portNode->eth_global_index;
											slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
											slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
											local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
											local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_no);
											if((length + 30 )< 2048) {
												if((NPD_TRUE == status) && (ISIGMP == Ismld)){
													length += sprintf(showStr,"  config igmp-snooping %d/%d enable\n",slot_no,local_port_no);
												}
												if((NPD_TRUE == statusm) && (ISMLD == Ismld)){
													length += sprintf(showStr,"  config mld-snooping %d/%d enable\n",slot_no,local_port_no);
												}
												 syslog_ax_vlan_dbg("%s",showStr);
												showStr = buf + length;
											}
											if(portNode->igmp_rport_flag){
												if((length + 35 )< 2048) {
													if((NPD_TRUE == status) && (ISIGMP == Ismld)){
														length += sprintf(showStr,"  add igmp-snooping route-port %d/%d\n",slot_no,local_port_no);
													}
													if((NPD_TRUE == statusm) && (ISMLD == Ismld)){
														length += sprintf(showStr,"  add mld-snooping route-port %d/%d\n",slot_no,local_port_no);
													}
													 syslog_ax_vlan_dbg("%s",showStr);
													showStr = buf + length;
												}
											}
										}
									}
								}
							}
						}

						portList = node->tagPortList;
						if(NULL != portList) {
							if(0 != portList->count) {
								if(0 == cfgflag && ((length + 21) < 2048)) {
									if((NPD_TRUE == status) && (ISIGMP == Ismld))
									{
    									length += sprintf(showStr," config igmp-vlan %d\n",i);
									}
									else if((NPD_TRUE == status) && (ISMLD == Ismld))
									{
    									length += sprintf(showStr," config mld-vlan %d\n",i);
									}
									syslog_ax_vlan_dbg("%s\n\n",showStr);
									showStr = buf + length;
								}
							
								list = &(portList->list);
								__list_for_each(pos,list) {
									portNode = list_entry(pos,struct vlan_port_list_node_s,list);
									if(portNode) {
										if(portNode->igmpEnFlag) {
											eth_g_index = portNode->eth_global_index;
											slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
											slot_no = CHASSIS_SLOT_INDEX2NO(slot_index);
											local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
											local_port_no = ETH_LOCAL_INDEX2NO(slot_index,local_port_no);
											if((length + 30 )< 2048) {
												if((NPD_TRUE == status) && (ISIGMP == Ismld)){
													length += sprintf(showStr,"  config igmp-snooping %d/%d enable\n",slot_no,local_port_no);
												}
												if((NPD_TRUE == statusm) && (ISMLD == Ismld)){
													length += sprintf(showStr,"  config mld-snooping %d/%d enable\n",slot_no,local_port_no);
												}
												 syslog_ax_vlan_dbg("%s",showStr);
												showStr = buf + length;
											}
											if(portNode->igmp_rport_flag){
												if((length + 35 )< 2048) {
													if((NPD_TRUE == status) && (ISIGMP == Ismld)){
														length += sprintf(showStr,"  add igmp-snooping route-port %d/%d\n",slot_no,local_port_no);
													}
													if((NPD_TRUE == statusm) && (ISMLD == Ismld)){
														length += sprintf(showStr,"  add mld-snooping route-port %d/%d\n",slot_no,local_port_no);
													}
													 syslog_ax_vlan_dbg("%s",showStr);
													showStr = buf + length;
												}
											}
										}
									}
								}
							}
						}
						#if 0 /*trunk not support igmp snooping now*/
						unsigned short trunk_id = 0;
						struct vlan_trunk_list_node_s *trunkNode = NULL;
						vlan_trunk_list_s* trunkMemList = NULL;
						/*save trunk member config*/
						trunkMemList = node->trunkList;
						if(NULL != trunkMemList) {
							if(0 != trunkMemList->count) {
								if(0 == cfgflag && ((length + 18) < 2048)) {
									length += sprintf(tmpPtr,"config vlan %d\n",i);
									 syslog_ax_vlan_dbg("%s\n\n",tmpPtr);
									tmpPtr = tmpBuf + length;
								}
							
								list = &(trunkMemList->list);
								__list_for_each(pos,list) {
									trunkNode = list_entry(pos,struct vlan_trunk_list_node_s,list);
									if(trunkNode) {
										trunk_id = trunkNode->trunkId;

										if((length + 15) < 2048) {
											length += sprintf(tmpPtr,"  add trunk %d\n",trunk_id);
											 syslog_ax_vlan_dbg("%s",tmpPtr);
											tmpPtr = tmpBuf + length;
										}
									}
								}
							}
						}
						#endif 
						if((length + 5 ) > bufLen) { /* 5 exit */
							 syslog_ax_vlan_err("save vlan buffer full\n");
							break;
						}
						if(cfgflag){
							length += sprintf(showStr," exit\n");
							 syslog_ax_vlan_dbg("%s",showStr);
							showStr = buf + length;
						}
					}
				}
			}
		}
	}
	else {
		*enDis = NPD_FALSE;
	}

    if(sw_config_flag ==1)
    {
        /* exit distributed igmp sw-baord config node */
    	length += sprintf(showStr,"exit\n");
    	showStr = buf + length;	
    }	
} 

unsigned int npd_show_vidx_member
(
	unsigned short vlanId,
	unsigned short vlanIdx
)
{
	int ret = IGMPSNP_RETURN_CODE_OK; 
	unsigned int groupAddr = 0x0, npdret =0;
	unsigned char devNum = 0;
	PORT_MEMBER_BMP groupMbrBmp;
	unsigned short groupAddrv6[SIZE_OF_IPV6_ADDR];
	unsigned short *pnpdaddr = NULL;

	memset(&groupMbrBmp,0,sizeof(PORT_MEMBER_BMP));
	ret = npd_igmp_snp_l2mc_vlan_exist(vlanId);
	if(IGMPSNP_RETURN_CODE_OK != ret ) {
		ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}
	else{
		ret = npd_igmp_snp_l2mc_group_exist(vlanId,vlanIdx);
		if(IGMPSNP_RETURN_CODE_OK == ret ){
			pnpdaddr = groupAddrv6;
			ret = npd_igmp_get_groupip_by_vlan_vidx(vlanId,vlanIdx,&groupAddr, &pnpdaddr, &npdret);
			if(IGMPSNP_RETURN_CODE_OK != ret ){
				return IGMPSNP_RETURN_CODE_ERROR_SW;
			}
			else if (0 == groupAddr){
				 syslog_ax_vlan_dbg("Illegal groupIp on vidx %d!",vlanIdx);
				return IGMPSNP_RETURN_CODE_ERROR_SW;
			}
			ret = nam_asic_group_mbr_bmp(devNum,vlanIdx,&groupMbrBmp);
			if(BRG_MC_SUCCESS == ret) {
				ret = IGMPSNP_RETURN_CODE_OK;
				 syslog_ax_vlan_dbg("###################################################");
				 syslog_ax_vlan_dbg("dev %d vidx = %d,groupIp 0x%x,groupMbrBmp[0] = 0x%x",\
							devNum,\
							vlanIdx,\
							groupAddr,\
							groupMbrBmp.portMbr[0]);
			}
			else if (BRG_MC_FAIL == ret) {
				ret = IGMPSNP_RETURN_CODE_ERROR_HW;
			}
		}
	}
	return IGMPSNP_RETURN_CODE_OK;
}

/******************************************************
 *  show vlan port membership by read Software record.
 *
 *****************************************************/
unsigned int npd_vlan_get_mbr_bmp_via_sw
(
	unsigned short vlanId,
	unsigned int *untagMbrBmp_sp,/**/
	unsigned int *tagMbrBmp_sp
)
{
	unsigned int i,ret = 0;
	unsigned int untagmbrCont = 0,tagmbrCont = 0;
	unsigned int untagmbr_index[64] = {0},tagmbr_index[64] = {0};
	unsigned int MbrBmpTmpSP = 0;
	unsigned int slotNo = 0,localPortNo = 0,slot_index = 0;
	struct eth_port_s	*ethPort = NULL;
	/*struct DRV_VLAN_PORT_BMP_S *unMbrBmpDrv = NULL,*tagMbrBmpDrv = NULL;*/
	/*struct vlan_ports_bmp  untag_vlan_port_bmp,tag_vlan_port_bmp ;*/
	ret = npd_vlan_member_port_index_get(vlanId, NPD_FALSE, untagmbr_index, &untagmbrCont);
	if(NPD_TRUE == ret){
		for(i = 0;i<untagmbrCont;i++){
			/*npd_get_devport_by_global_index(unmbr_index[i], &devNum, &portNum);*//*no use:0604*/
			ethPort = npd_get_port_by_index(untagmbr_index[i]);
			if(NULL == ethPort){
				return VLAN_RETURN_CODE_ERR_GENERAL;
			}
			/*port as trunk member be show as trunk but not port. */
			 syslog_ax_vlan_dbg("vlan %d untag port index[%d] = %d\n",vlanId,i,untagmbr_index[i]);
			if(NULL == ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(untagmbr_index[i]);
				slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
				localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(untagmbr_index[i]);
				localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
				MbrBmpTmpSP |= 1<<((slotNo-1)*8+localPortNo);
			 syslog_ax_vlan_dbg("vlan %d untag port index[%d] = %#x bitmap %#x",vlanId,i,untagmbr_index[i],MbrBmpTmpSP);
			}
		}
		*untagMbrBmp_sp =MbrBmpTmpSP;
	}
	MbrBmpTmpSP = 0;
	ret = npd_vlan_member_port_index_get(vlanId, NPD_TRUE, tagmbr_index, &tagmbrCont);
	if(NPD_TRUE == ret)
	{
		for(i = 0;i<tagmbrCont;i++){
			/*npd_get_devport_by_global_index(unmbr_index[i], &devNum, &portNum);*//*no use:0604*/
			ethPort = npd_get_port_by_index(tagmbr_index[i]);
			if(NULL == ethPort){
				return VLAN_RETURN_CODE_ERR_GENERAL;
			}
			 syslog_ax_vlan_dbg("vlan %d tag port index[%d] = %d",vlanId,i,tagmbr_index[i]);
			if(NULL == ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(tagmbr_index[i]);
				slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
				localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(tagmbr_index[i]);
				localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
				MbrBmpTmpSP |= 1<<((slotNo-1)*8 + localPortNo);
			 syslog_ax_vlan_dbg("vlan %d tagged port index[%d] = %#x bitmap %#x",vlanId,i,tagmbr_index[i],MbrBmpTmpSP);
			}
		}
		*tagMbrBmp_sp =MbrBmpTmpSP;
	}
	return NPD_SUCCESS;
}



DBusMessage *npd_dbus_vlan_config_qinq_update_for_master
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	isEnable = 0;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index =0;
	unsigned short	vlanId = 0;	

	unsigned int	port_index = 0, eth_g_index = 0;
	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned char tagMode = 0;
	struct eth_port_s	*ethPort = NULL;	
	/*enum module_id_e module_type;*/
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isEnable,
							DBUS_TYPE_BYTE,&slot_no,
							DBUS_TYPE_BYTE,&local_port_no,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}



	syslog_ax_vlan_dbg("isEnable = %d,slotno = %d,portno = %d,vlanid = %d\n",isEnable,slot_no,local_port_no,vlanId);

	if((IS_MASTER_NPD == 1) || (BOARD_TYPE == BOARD_TYPE_AX71_CRSMU))
	{
	
		if(isEnable)
		{

       		if(local_port_no>32)
       		{
    			g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp |= (unsigned int)(1<<(local_port_no-33));   					
       	 	}
			else
			{
    			g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));
			}    
  
		}
		else
		{
        	if(local_port_no>32)
       		{
    			g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));    					
        	}
			else
			{
    			g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));  
			}
		}
		ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
		if( ret!=0 )
    	{
       		syslog_ax_vlan_err("msync shm_vlan failed \n" );   
    	}
				
		ret = VLAN_RETURN_CODE_ERR_NONE;

	}
	else
	{
		ret = VLAN_RETURN_CODE_ERR_GENERAL;
	}
	

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);


	return reply;

}




DBusMessage *npd_dbus_vlan_config_qinq_endis
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	isEnable = 0;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index =0;
	unsigned short	vlanId = 0;	

	unsigned int	port_index = 0, eth_g_index = 0;
	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned char tagMode = 0;
	struct eth_port_s	*ethPort = NULL;	
	enum product_id_e productId = PRODUCT_ID;
	/*enum module_id_e module_type;*/
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isEnable,
							DBUS_TYPE_BYTE,&slot_no,
							DBUS_TYPE_BYTE,&local_port_no,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_INVALID))) 
	{
		syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}



	syslog_ax_vlan_dbg("isEnable = %d,slotno = %d,portno = %d,vlanid = %d\n",isEnable,slot_no,local_port_no,vlanId);
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) 
	{
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) 
		{
			slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
			port_index = ETH_LOCAL_NO2INDEX(slot_index, local_port_no);
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,port_index);
			if(VLAN_RETURN_CODE_ERR_NONE != ret)
			{
				/*find out some invalid */
				reply = dbus_message_new_method_return(msg);
				
				dbus_message_iter_init_append (reply, &iter);
				
				dbus_message_iter_append_basic (&iter,
												 DBUS_TYPE_UINT32,
												 &ret);
				return reply;
			}
			else 
			{
				if(isEnable)
				{
   			 		if(local_port_no>32)
   					{
						if(g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp & ((unsigned int)(1<<(local_port_no-33))))
							ret = VLAN_RETURN_CODE_PORT_EXISTS;
    				}
					else
					{
						if(g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp & ((unsigned int)(1<<(local_port_no-1))))
							ret = VLAN_RETURN_CODE_PORT_EXISTS;
					}    

				}
				else
				{
    				if(local_port_no>32)
    				{
						if(!(g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp & ((unsigned int)(1<<(local_port_no-33)))))
							ret = VLAN_RETURN_CODE_PORT_EXISTS;
    				}
					else
					{
						if(!(g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp & ((unsigned int)(1<<(local_port_no-1)))))
							ret = VLAN_RETURN_CODE_PORT_EXISTS;
					}
				}
				
				if(ret == VLAN_RETURN_CODE_ERR_NONE)
				{
					if(!(ret=npd_vlan_check_contain_port(vlanId,eth_g_index,&tagMode)))
					{
						ret = VLAN_RETURN_CODE_PORT_NOTEXISTS;
					}
					else if(tagMode != NPD_TRUE)
					{
						syslog_ax_vlan_dbg("eth_g_index 0x%x in vlan %d tag mode is %s\n",eth_g_index,vlanId,tagMode?"Tag":"Untag");
						ret = VLAN_RETURN_CODE_PORT_TAG_CONFLICT;
					}
					else
					{
						syslog_ax_vlan_dbg("eth_g_index 0x%x in vlan %d tag mode is %s\n",eth_g_index,vlanId,tagMode?"Tag":"Untag");
						ret = npd_vlan_qinq_endis(vlanId, eth_g_index, isEnable);
						if(ret != VLAN_RETURN_CODE_ERR_NONE)
						{
							syslog_ax_vlan_err("set eth_g_index %x FAIL !\n",eth_g_index);
						}
						else
						{

							if(isEnable)
							{

		       			 		if(local_port_no>32)
		       					{
		    						g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp |= (unsigned int)(1<<(local_port_no-33));   					
		        				}
								else
								{
		    						g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));
								}    
		  
							}
							else
							{
		        				if(local_port_no>32)
		        				{
		    						g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));    					
		        				}
								else
								{
		    						g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));  
								}
							}
							ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
							if( ret != NPD_SUCCESS)
		    				{
		       					 syslog_ax_vlan_err("msync shm_vlan failed \n" );   
		    				}
						}
					}
				}
				else
				{
					syslog_ax_vlan_dbg("port %d/%d qinq has %s already !!!\n",slot_no,local_port_no,isEnable?"enabled":"disabled");
				}
			}
		}
		else 
		{
			 syslog_ax_vlan_dbg("Bad local_port_no!!\n");
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
	}
	else 
	{
		 syslog_ax_vlan_dbg("Bad slot_no!!\n");
		ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);


	return reply;

}

DBusMessage *npd_dbus_vlan_config_allbackport_qinq_endis
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	isEnable = 0;
	unsigned short	vlanId = 0;	

	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;

	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isEnable,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if((isEnable) && (g_vlanlist[vlanId-1].cscd_qinq_state))
	{
		syslog_ax_vlan_dbg("vlan qinq already enabled !\n");
		ret = VLAN_RETURN_CODE_VLAN_EXISTS;
	}
	else if((!isEnable) && (!g_vlanlist[vlanId-1].cscd_qinq_state))
	{
		syslog_ax_vlan_dbg("vlan qinq already disabled !\n");
		ret = VLAN_RETURN_CODE_VLAN_EXISTS;
	}
	else
	{
		ret = npd_vlan_allcscdport_qinq_endis(vlanId,isEnable);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_err("set vlan %d all cscd port  qinq error !\n",vlanId);
			if(isEnable)
			{
				npd_vlan_allcscdport_qinq_endis(vlanId,0);
			}
		}
		else
		{
			if(isEnable)
			{
				g_vlanlist[vlanId-1].cscd_qinq_state = 1;
			}
			else
			{
				g_vlanlist[vlanId-1].cscd_qinq_state = 0;
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
DBusMessage *npd_dbus_vlan_config_tocpuport_qinq_endis
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	isEnable = 0;
	unsigned short vlanId = 0 ,slotId = 0,cpu_no = 0,cpu_port_no = 0;
	unsigned int   bond_state = 0;

	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;

	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isEnable,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_UINT16,&slotId,
							DBUS_TYPE_UINT16,&cpu_no,
							DBUS_TYPE_UINT16,&cpu_port_no,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	/*first check bond state*/
	if(((g_vlanlist[vlanId-1].bond_slot[slotId-1])&(unsigned int)(1<<(8*cpu_no+cpu_port_no))) == 1)
		bond_state = 1;
	
	if(g_vlanlist[vlanId-1].vlanStat == 0)
    {
	    syslog_ax_vlan_err("vlan %d is not exists ",vlanId);	
		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
    }
	else if(bond_state == 0)
	{
		syslog_ax_vlan_err("please bond first !\n");
		ret = VLAN_RETURN_CODE_ERR_GENERAL;
	}
	else if((isEnable) && (((g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1])&(unsigned int)(1<<(8*cpu_no+cpu_port_no)))))
	{
		syslog_ax_vlan_dbg("vlan %d slot %d cpu_no %d cpu_port_no %d qinq already enabled !\n",vlanId,slotId,cpu_no,cpu_port_no);
		ret = VLAN_RETURN_CODE_VLAN_EXISTS;
	}
	else if((!isEnable) && (!((g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1])&(unsigned int)(1<<(8*cpu_no+cpu_port_no)))))
	{
		syslog_ax_vlan_dbg("vlan %d slot %d cpu_no %d cpu_port_no %d qinq already disabled !\n",vlanId,slotId,cpu_no,cpu_port_no);
		ret = VLAN_RETURN_CODE_VLAN_EXISTS;
	}
	else
	{
		ret = npd_vlan_tocpuport_qinq_endis(vlanId,cpu_no,cpu_port_no,isEnable);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_err("set vlan %d slot %d, cpu_no %d,cpu_port_no %d  qinq fail ! \n",vlanId,slotId,cpu_no,cpu_port_no);
			if(isEnable)
			{
				npd_vlan_tocpuport_qinq_endis(vlanId,cpu_no,cpu_port_no,0);
			}
		}
		else
		{
			if(isEnable)   /*enable*/
			{
				g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1] |= (unsigned int)(1<<(8*cpu_no+cpu_port_no));
			}
			else
			{			/*disable*/
				g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1] &= ((unsigned int)(~(1<<(8*cpu_no+cpu_port_no))));
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

DBusMessage *npd_dbus_vlan_config_tocpuport_qinq_endis_for_old
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	isEnable = 0;
	unsigned short vlanId = 0 ,slotId = 0,cpu_no = 0,cpu_port_no = 0;
	unsigned int   bond_state = 0;
	int i = 0;
	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;

	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isEnable,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	/*first check bond state*/
	for(i=0;i<CHASSIS_SLOT_COUNT;i++)
	{
		if(((g_vlanlist[vlanId-1].bond_slot[i])&(unsigned int)(1<<(8*cpu_no+cpu_port_no))) == 1)
		{
			slotId = i+1;
			bond_state = 1;
			break;
		}
	}
	syslog_ax_vlan_dbg("slotId  = %d,bond_state = %d,SLOT_ID = %d,IS_MASTER_NPD = %d\n",slotId,bond_state,SLOT_ID,IS_MASTER_NPD);
	if(((SLOT_ID == slotId) || (IS_MASTER_NPD)) && (slotId))
	{
		if(g_vlanlist[vlanId-1].vlanStat == 0)
	    {
		    syslog_ax_vlan_err("vlan %d is not exists ",vlanId);	
			ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
	    }
		else if(bond_state == 0)
		{
			syslog_ax_vlan_err("please bond first !\n");
			ret = VLAN_RETURN_CODE_ERR_GENERAL;
		}
		else if((isEnable) && (((g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1])&(unsigned int)(1<<(8*cpu_no+cpu_port_no)))))
		{
			syslog_ax_vlan_dbg("vlan %d slot %d cpu_no %d cpu_port_no %d qinq already enabled !\n",vlanId,slotId,cpu_no,cpu_port_no);
			ret = VLAN_RETURN_CODE_VLAN_EXISTS;
		}
		else if((!isEnable) && (!((g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1])&(unsigned int)(1<<(8*cpu_no+cpu_port_no)))))
		{
			syslog_ax_vlan_dbg("vlan %d slot %d cpu_no %d cpu_port_no %d qinq already disabled !\n",vlanId,slotId,cpu_no,cpu_port_no);
			ret = VLAN_RETURN_CODE_VLAN_EXISTS;
		}
		else
		{
			if(IS_MASTER_NPD)
			{
				if(isEnable)   /*enable*/
				{
					g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1] |= (unsigned int)(1<<(8*cpu_no+cpu_port_no));
				}
				else
				{			/*disable*/
					g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1] &= ((unsigned int)(~(1<<(8*cpu_no+cpu_port_no))));
				}
			}
			else
			{
				ret = npd_vlan_tocpuport_qinq_endis(vlanId,cpu_no,cpu_port_no,isEnable);
				if(ret != VLAN_RETURN_CODE_ERR_NONE)
				{
					syslog_ax_vlan_err("set vlan %d slot %d, cpu_no %d,cpu_port_no %d  qinq fail ! \n",vlanId,slotId,cpu_no,cpu_port_no);
					if(isEnable)
					{
						npd_vlan_tocpuport_qinq_endis(vlanId,cpu_no,cpu_port_no,0);
					}
				}
				else
				{
					if(isEnable)   /*enable*/
					{
						g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1] |= (unsigned int)(1<<(8*cpu_no+cpu_port_no));
					}
					else
					{			/*disable*/
						g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1] &= ((unsigned int)(~(1<<(8*cpu_no+cpu_port_no))));
					}
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


DBusMessage *npd_dbus_vlan_config_tocpuport_qinq_update_for_master
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	isEnable = 0;
	unsigned short vlanId = 0 ,slotId = 0,cpu_no = 0,cpu_port_no = 0;

	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;

	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isEnable,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_UINT16,&slotId,
							DBUS_TYPE_UINT16,&cpu_no,
							DBUS_TYPE_UINT16,&cpu_port_no,
							DBUS_TYPE_INVALID))) 
	{
		syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) 
		{
			syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if(isEnable)   /*enable*/
	{
		g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1] |= (unsigned int)(1<<(8*cpu_no+cpu_port_no));
	}
	else
	{			/*disable*/
		g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1] &= ((unsigned int)(~(1<<(8*cpu_no+cpu_port_no))));
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;

}

/******************************************************
 *  show vlan port membership by read Software record.
 *
 *****************************************************/
unsigned int npd_vlan_get_mbr_bmp_via_sw_v1
(
	unsigned short vlanId,
	PORT_MEMBER_BMP *untagMbrBmp_sp,
	PORT_MEMBER_BMP *tagMbrBmp_sp
)
{
	unsigned int i,ret = 0;
	unsigned int untagmbrCont = 0,tagmbrCont = 0;
	unsigned int untagmbr_index[64] = {0},tagmbr_index[64] = {0};
	unsigned int slotNo = 0,localPortNo = 0,slot_index = 0;	
	unsigned int wordOffset=0,bit= 0;	
	struct eth_port_s	*ethPort = NULL;
	
	ret = npd_vlan_member_port_index_get(vlanId, NPD_FALSE, untagmbr_index, &untagmbrCont);
	if(NPD_TRUE == ret){
		for(i = 0;i<untagmbrCont;i++){
			ethPort = npd_get_port_by_index(untagmbr_index[i]);
			if(NULL == ethPort){
				return VLAN_RETURN_CODE_ERR_GENERAL;
			}
			/*port as trunk member be show as trunk but not port. */
			if(NULL == ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(untagmbr_index[i]);
				slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
				localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(untagmbr_index[i]);
				localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
				if(PRODUCT_ID_AX5K_I == PRODUCT_ID){
					wordOffset = (slotNo*8+localPortNo)/32;
					bit = (slotNo*8+localPortNo)%32 - 1;
				}
				else if(PRODUCT_ID_AU3K_BCAT == PRODUCT_ID){
					wordOffset = ((slotNo-2)*8+localPortNo)/32;
				    bit = ((slotNo-2)*8+localPortNo)%32;
					
				}
				else if(PRODUCT_ID_AX7K_I == PRODUCT_ID){
					wordOffset = ((slotNo-2)*8+localPortNo)/32;
				    bit = ((slotNo-2)*8+localPortNo)%32;					
            		syslog_ax_vlan_dbg(" add debug for PRODUCT_ID_AX7K_I \n");
				}				
				else{
				wordOffset = ((slotNo-1)*8+localPortNo)/32;
				bit = ((slotNo-1)*8+localPortNo)%32;	
				}
				if(SYSTEM_TYPE == IS_DISTRIBUTED)
				{
    				wordOffset = (localPortNo-1)/32;
				    bit = (localPortNo-1)%32;	
				}
				syslog_ax_vlan_dbg("slot_index %d slotNo %d localPortNo %d wordOffset %d bit %d\n",slot_index,slotNo,localPortNo,wordOffset,bit);
				if(1 >= wordOffset) {
					untagMbrBmp_sp->portMbr[wordOffset] |= (1<<bit);
				}
				 syslog_ax_vlan_dbg("vlan %d untag port index[%d] = %d bitmap0 %#x,bitmap1 %#x\n",vlanId,i,untagmbr_index[i],\
				 							untagMbrBmp_sp->portMbr[0],untagMbrBmp_sp->portMbr[1]);
			}
		}		
	}
	ret = npd_vlan_member_port_index_get(vlanId, NPD_TRUE, tagmbr_index, &tagmbrCont);
	if(NPD_TRUE == ret){
		for(i = 0;i<tagmbrCont;i++){
			ethPort = npd_get_port_by_index(tagmbr_index[i]);
			if(NULL == ethPort){
				return VLAN_RETURN_CODE_ERR_GENERAL;
			}
			if(NULL == ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(tagmbr_index[i]);
				slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
				localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(tagmbr_index[i]);
				localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);				
				if(PRODUCT_ID_AX5K_I == PRODUCT_ID){
					wordOffset = (slotNo*8+localPortNo)/32;
					bit = (slotNo*8+localPortNo)%32 - 1;
				}
				else if(PRODUCT_ID_AU3K_BCAT == PRODUCT_ID){
					wordOffset = ((slotNo-2)*8+localPortNo)/32;
				    bit = ((slotNo-2)*8+localPortNo)%32;
					
				}
				else if(PRODUCT_ID_AX7K_I == PRODUCT_ID){
					wordOffset = ((slotNo-2)*8+localPortNo)/32;
				    bit = ((slotNo-2)*8+localPortNo)%32;					
            		syslog_ax_vlan_dbg(" add debug for PRODUCT_ID_AX7K_I \n");
				}				
				else{
				wordOffset = ((slotNo-1)*8+localPortNo)/32;
				bit = ((slotNo-1)*8+localPortNo)%32;	
				}

				if(SYSTEM_TYPE == IS_DISTRIBUTED)
				{
    				wordOffset = (localPortNo-1)/32;
				    bit = (localPortNo-1)%32;	
				}				

				syslog_ax_vlan_dbg("slot_index %d slotNo %d localPortNo %d wordOffset %d bit %d\n",slot_index,slotNo,localPortNo,wordOffset,bit);
				if(1 >= wordOffset) {
					tagMbrBmp_sp->portMbr[wordOffset] |= (1<<bit);
				}
				 syslog_ax_vlan_dbg("vlan %d tag port index[%d] = %d bitmap0 %#x,bitmap1 %#x",vlanId,i,tagmbr_index[i],\
				 							tagMbrBmp_sp->portMbr[0],tagMbrBmp_sp->portMbr[1]);
			}
		}
	}
	return NPD_SUCCESS;
}

	
/**********************************************************************************
 *	npd_vlan_get_mbr_bmp_wan
 *
 *	DESCRIPTION:
 *		get master port's sub-intf bit map
 *
 *	INPUT:
 *		vlanId
 *		tagMbrBmp_wan
 *	
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		ret
 *		
 *
 **********************************************************************************/
unsigned int npd_vlan_get_mbr_bmp_wan

(
	unsigned short vlanId,
	PORT_MEMBER_BMP *tagMbrBmp_wan
)
{
	unsigned char slotno = 0, local_port_no = 0,slot_index = 0;
	unsigned int ret = 0,i=0,j=0,port_index = 0;
	unsigned int wordOffset=0,bit= 0;

#define SYSFS_PATH_MAX 256
	FILE *f = NULL;
	char path[SYSFS_PATH_MAX] = {0};

	for (i = 0; i < CHASSIS_SLOT_COUNT; i++ ) {
		for (j = 0; j < ETH_LOCAL_PORT_COUNT(i); j++) {
			port_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i, j);
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(port_index);
			slotno = CHASSIS_SLOT_INDEX2NO(slot_index);
			local_port_no = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(port_index);
			local_port_no = ETH_LOCAL_INDEX2NO(slot_index, local_port_no);
		
		if(PRODUCT_ID_AX5K_I == PRODUCT_ID){
			wordOffset = (slotno*8+local_port_no)/32;
			bit = (slotno*8+local_port_no)%32 - 1;
		}
		/* add for vlan port dispaly ,port 0 to 2/1 on ax71-ax12g12s,zhangdi */		
		else if(PRODUCT_ID_AX7K_I == PRODUCT_ID){
			wordOffset = ((slotno-2)*8+local_port_no)/32;
		    bit = ((slotno-2)*8+local_port_no)%32;
    		syslog_ax_vlan_dbg(" add debug for PRODUCT_ID_AX7K_I \n");			
			
		}
		/* add end */		
		else{
			wordOffset = ((slotno-1)*8+local_port_no)/32;
			bit = ((slotno-1)*8+local_port_no)%32;	
		}
		syslog_ax_vlan_dbg("vlan %d port %d/%d index %d wordOffset %d bit %d\n",vlanId, slotno,local_port_no,port_index, wordOffset,bit);
		snprintf(path, SYSFS_PATH_MAX, "/sys/class/net/eth%d-%d.%d/brport", slotno, local_port_no,vlanId);
		f = fopen(path, "r");
		if (f) {
			fclose(f);
			if(1 >= wordOffset) {
				tagMbrBmp_wan->portMbr[wordOffset] |= (1<<bit);
				syslog_ax_vlan_dbg("tagbmp[%d] = %x\n",wordOffset,tagMbrBmp_wan->portMbr[wordOffset]);
			}
		}
		else{
			syslog_ax_vlan_dbg("/sys/class/net/eth%d-%d.%d not in br \n",slotno, local_port_no,vlanId);
		}

		}
	}
	
	return NPD_SUCCESS;
}
/******************************************************
 *  show vlan trunk membership by read Software record.
 *
 *****************************************************/
unsigned int npd_vlan_get_trunk_mbr_bmp_via_sw
(
	unsigned short vlanId,
	TRUNK_MEMBER_BMP *untagTrkBmp_sp,
	TRUNK_MEMBER_BMP *tagTrkBmp_sp
)
{
	unsigned int i,ret = 0;
	unsigned int untagtrkCont = 0,tagtrkCont = 0;
	static unsigned short untagtrk_id[127] = {0},tagtrk_id[127] = {0};
	unsigned int wordOffset=0,bit= 0;
	unsigned short *untagTrkPtr = untagtrk_id,*tagTrkPtr = tagtrk_id;
	ret = npd_vlan_get_mbr_trunk_id(vlanId,NPD_FALSE,&untagTrkPtr,&untagtrkCont);
	if(NPD_TRUE == ret) {
		for(i = 0; i<untagtrkCont; i++){
			wordOffset = (untagtrk_id[i]-1)/32;
			bit = (untagtrk_id[i]-1)%32;
			if(3 >= wordOffset){
				untagTrkBmp_sp->trunkMbr[wordOffset] |= (1<<bit);
			}
			 syslog_ax_vlan_dbg("vlan %d untag trunk index[%d] = %d\n",vlanId,i,untagtrk_id[i]);
		}
	}
	ret = npd_vlan_get_mbr_trunk_id(vlanId,NPD_TRUE,&tagTrkPtr,&tagtrkCont);
	if(NPD_TRUE == ret) {
		for(i = 0; i<tagtrkCont; i++){
			wordOffset = (tagtrk_id[i]-1)/32;
			bit = (tagtrk_id[i]-1)%32;
			if(3 >= wordOffset){
				tagTrkBmp_sp->trunkMbr[wordOffset] |= (1<<bit);
			}
			 syslog_ax_vlan_dbg("vlan %d tag trunk index[%d] = %d\n",vlanId,i,tagtrk_id[i]);
		}
	}
	return NPD_SUCCESS;
}

unsigned int npd_vlan_autolearn_set
(
    unsigned char devNum,
    unsigned short vlanId,
    unsigned int  autolearn

 )
{
    unsigned int ret = 0;

	ret = npd_check_vlan_exist(vlanId);
	if(VLAN_RETURN_CODE_VLAN_EXISTS != ret){ /*value 13*/
		syslog_ax_vlan_err("The vlan %d does not exist\n",vlanId);
		return NPD_FDB_ERR_GENERAL;
	}
	ret = nam_asic_vlan_autolearn_set(devNum,vlanId,autolearn);
	if(0 != ret){
		syslog_ax_vlan_err("The vlan %d does not exist\n",vlanId);
		return NPD_FDB_ERR_GENERAL;
	}
	return NPD_FDB_ERR_NONE;
} 

/*********************************************************************************************
 *			NPD dbus operation part
 *	
 *********************************************************************************************/
DBusMessage * npd_dbus_vlan_create_vlan_entry_one
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
    if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
    {
    	DBusMessage* reply = NULL;
    	DBusMessageIter	 iter = {0};
    	unsigned short	vlanId = 0;
    	char *vlanName = NULL;
    	unsigned int ret = VLAN_RETURN_CODE_ERR_NONE;
    	DBusError err = {0};
    	unsigned int untagbmp[2] = {0};
    	unsigned int tagbmp[2] = {0};

		dbus_error_init(&err);
	
		if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT16,&vlanId,
								 DBUS_TYPE_STRING,&vlanName,
								 DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		
		if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
			ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN;
		}
		/* Check out if specified vlan has been created.  bug:AXSSZFI-277 */
    	else if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(vlanId))
    	{
    		ret = VLAN_RETURN_CODE_VLAN_EXISTS;
    	}		
		else
		{
			/* call npd_vlan_interface_active, to do somthing as BELOW:*/
			ret = npd_vlan_activate(vlanId,vlanName);
			if (VLAN_RETURN_CODE_ERR_NONE != ret) {
				syslog_ax_vlan_err("npd_vlan_activate vlan:%d error !\n",vlanId);
			}
			else   /* vlan active OK */
			{
                #if 1   /* we do not need mstp */
				npd_mstp_add_vlan_on_mst(vlanId,untagbmp,tagbmp);
				#endif			
    			/* add for distributed OS, add cscd port to all vlan */
    			ret = npd_vlan_cscd_add(vlanId);
    			if (VLAN_RETURN_CODE_ERR_NONE != ret) {
    				syslog_ax_vlan_err("npd_vlan_add_cscd to vlan %d error !\n",vlanId);
    				ret = VLAN_RETURN_CODE_ERR_GENERAL;
    			}
				else    /* cscd port add OK */
				{
        	        /* we need update the g_vlanlist[] on local-board */		
        			ret = npd_vlanlist_valid(vlanId,vlanName);
        			if (VLAN_RETURN_CODE_ERR_NONE != ret) {
        				syslog_ax_vlan_err("npd set npd_vlanlist_valid error !\n");
        				ret = VLAN_RETURN_CODE_ERR_GENERAL;
        			}
        			else    /* vlanlist valid OK */
        			{
            			/* update g_vlans[] on AX81-SMU, for L3-vlan */
            			if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
            			{
            			    g_vlans[vlanId-1]->brgVlan.state = VLAN_STATE_UP_E;
            			}
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
	else
	{
    	DBusMessage* reply = NULL;
    	DBusMessageIter	 iter = {0};
    	unsigned short	vlanId = 1;
    	char *vlanName = NULL;
    	unsigned int ret = VLAN_RETURN_CODE_ERR_NONE;
    	DBusError err = {0};
    	unsigned int untagbmp[2] = {0};
    	unsigned int tagbmp[2] = {0};

    	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
    		npd_syslog_dbg("not support vlan!\n");
    		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
    	}
    	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
    			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
    		npd_syslog_dbg("not support vlan!\n");
    		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
    	}
    	else{
    		dbus_error_init(&err);
    	
    		if (!(dbus_message_get_args ( msg, &err,
    								 DBUS_TYPE_UINT16,&vlanId,
    								 DBUS_TYPE_STRING,&vlanName,
    								 DBUS_TYPE_INVALID))) {
    			 syslog_ax_vlan_err("Unable to get input args ");
    			if (dbus_error_is_set(&err)) {
    				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
    				dbus_error_free(&err);
    			}
    			return NULL;
    		}
    		
    		if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
    			ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN;
    		}
    		else {
    		if(PRODUCT_ID_AX5K_E == PRODUCT_ID || PRODUCT_ID_AX5608 == PRODUCT_ID){
    				ret = VLAN_RETURN_CODE_VLAN_CREATE_NOT_ALLOWED;
    			}
    			else{
    				/* call npd_vlan_interface_active, to do somthing as BELOW:*/
    				ret = npd_vlan_activate(vlanId,vlanName);
    				if (VLAN_RETURN_CODE_ERR_NONE == ret) {
    					syslog_ax_vlan_err("npd_dbus101:: add vlan vid %d\n",vlanId);
    					npd_mstp_add_vlan_on_mst(vlanId,untagbmp,tagbmp);
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
}
/*enter vlan configure node*/
DBusMessage * npd_dbus_vlan_config_layer2
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned short	vlanId;
	unsigned int	ret = 0,op_ret = VLAN_RETURN_CODE_ERR_NONE;
	struct vlan_s * vlanNode = NULL;
	DBusError err;
	
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_UINT16,&vlanId,
			DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		
		if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
			ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN;
		}
		else {
			/* call npd_check_vlan_exist, return the vlan exist or NOT:*/
			ret = npd_check_vlan_exist(vlanId);
			if (VLAN_RETURN_CODE_BADPARAM == ret) {
				op_ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN; 
			}
			else if (VLAN_RETURN_CODE_VLAN_NOT_EXISTS == ret) {
				op_ret = VLAN_RETURN_CODE_ERR_GENERAL; 
			}
			
	        /* we must allow to config it ,otherwise the already exists configuration will be locked
			else if(advanced_routing_default_vid == vlanId){
				op_ret = VLAN_RETURN_CODE_CONFIG_NOT_ALLOWED;
			}
			*/
			else if(VLAN_RETURN_CODE_VLAN_EXISTS == ret) {  
				op_ret = VLAN_RETURN_CODE_ERR_NONE;      
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &op_ret);
	/*
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT16,
									 &vlanId);
	*/
	
	return reply;
	
}
/*enter vlan configure node*/
DBusMessage * npd_dbus_vlan_config_layer2_vname
(	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned short	vlanId = 0;
	char*	vlanName = NULL;
	struct vlan_s*	vlanNode = NULL;
	unsigned int	op_ret = VLAN_RETURN_CODE_ERR_NONE;
	DBusError err;
	
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_STRING,&vlanName,
			DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		 syslog_ax_vlan_dbg("get vlan name %s",vlanName);
		vlanNode = npd_find_vlan_by_name(vlanName);
		if(NULL == vlanNode) {
			op_ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
		}
		else {
			vlanId = vlanNode->vid;
			if(1 == vlanId) { 
				op_ret = VLAN_RETURN_CODE_BADPARAM;
			}/*default vlan can NOT config*/
			else if (4095 == vlanId) {
				op_ret = VLAN_RETURN_CODE_PORT_L3_INTF;
			}
			else if(vlanNode->isAutoCreated){
				op_ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
			}
			else 
				op_ret = VLAN_RETURN_CODE_ERR_NONE;      
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &op_ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT16,
									 &vlanId);
	
	return reply;
	
}


DBusMessage *npd_dbus_vlan_config_port_add_del
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	isAdd = 0, isTagged = 0;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index =0;
	unsigned short	vlanId = 0;	

	unsigned int	port_index = 0, eth_g_index = 0;
	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;
	struct eth_port_s	*ethPort = NULL;	
	enum product_id_e productId = PRODUCT_ID;
	/*enum module_id_e module_type;*/
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isAdd,
							DBUS_TYPE_BYTE,&slot_no,
							DBUS_TYPE_BYTE,&local_port_no,
							DBUS_TYPE_BYTE,&isTagged,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	 syslog_ax_vlan_dbg("%s port %d/%d %s %s vlan %d\n",(isAdd==1)?"add":"delete", 
			slot_no,local_port_no,(isTagged==1)?"tagged":"untagged",(isAdd==1)?"to":"from",vlanId);
	
	if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
			slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
			port_index = ETH_LOCAL_NO2INDEX(slot_index, local_port_no);
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,port_index);
			/*module_type = MODULE_TYPE_ON_SLOT_INDEX(slot_index);*/
			
			if((PRODUCT_ID_AX5K_I == productId) \
				&& (eth_g_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9))  \
				&& (eth_g_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) {
				syslog_ax_vlan_dbg("port(%d %d) index %d do not allow adding to vlan.\n",slot_no,local_port_no,eth_g_index);
				ret = ETHPORT_RETURN_CODE_UNSUPPORT;
			}
			
			/*check the port is trunk member ot NOT*/
			ethPort = npd_get_port_by_index(eth_g_index);
			if(NULL == ethPort){
				ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
			}
			else if(NULL != ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
				/*port is member of trunk*/
				ret = VLAN_RETURN_CODE_PORT_TRUNK_MBR;
			}
		
			if(0 != npd_count_static_arp_in_port_vlan(eth_g_index,vlanId)){
				/*port has static arp*/
				ret = VLAN_RETURN_CODE_ARP_STATIC_CONFLICT;
			}

		    if(NPD_TRUE == npd_pvlan_check_port_ispvlan(slot_no,local_port_no)) {
				/* by zhubo@autelan.com check pvlan port  2008.8.27*/
				ret = PVLAN_RETURN_CODE_THIS_PORT_HAVE_PVE;
			}
			if(VLAN_RETURN_CODE_ERR_NONE != ret){
				/*find out some invalid */
				reply = dbus_message_new_method_return(msg);
				
				dbus_message_iter_init_append (reply, &iter);
				
				dbus_message_iter_append_basic (&iter,
												 DBUS_TYPE_UINT32,
												 &ret);
				return reply;
			}
			else {
				/*zhangcl add for delete static fdb when del port from vlan*/
				if(isAdd == 0)
				{
					ret = npd_fdb_static_entry_del_with_port_vlan(slot_no,local_port_no,vlanId);
					if(ret != VLAN_RETURN_CODE_ERR_NONE)
					{
						syslog_ax_vlan_dbg("did not find static fdb by port %d-%d,vlan %d\n",slot_no,local_port_no,vlanId);
					}
					else
					{
						
						ret = nam_fdb_static_delete_by_port_vlan(vlanId,eth_g_index);
						if(ret != VLAN_RETURN_CODE_ERR_NONE)
						{
							syslog_ax_vlan_err("delete static fdb for port %d-%d,vlan %d failed !\n",slot_no,local_port_no,vlanId);
						}
					}

					if(isTagged)
					{
						if(local_port_no > 32)
						{
							if((g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp)&(1<<(local_port_no-33)))
							{
								ret = npd_vlan_qinq_endis(vlanId,eth_g_index,0);
								if(ret != VLAN_RETURN_CODE_ERR_NONE)
								{
									syslog_ax_vlan_err("npd vlan qinq disable eth_g_index %d fail !\n",eth_g_index);
								}
								g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
							}
						}
						else
						{
							if((g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp)&(1<<(local_port_no-1)))
							{
								ret = npd_vlan_qinq_endis(vlanId,eth_g_index,0);
								if(ret != VLAN_RETURN_CODE_ERR_NONE)
								{
									syslog_ax_vlan_err("npd vlan qinq disable eth_g_index %d fail !\n",eth_g_index);
								}
								g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
							}
						}

						ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
						if( ret!=0 )
					    {
					        syslog_ax_vlan_err("msync shm_vlan failed \n" );   
					    }
					}
				}
				ret = npd_vlan_add_or_del_port(vlanId, eth_g_index, isTagged, isAdd);
			}
		}
		else {
			 syslog_ax_vlan_dbg("Bad local_port_no!!\n");
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
	}
	else {
		 syslog_ax_vlan_dbg("Bad slot_no!!\n");
		ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
	}
	#if 0    /* not needed, for the addto &delform has update the g_vlanlist[] */
	/* add for update g_vlanlist[] on local board, is not SMU */
    if((ret == VLAN_RETURN_CODE_ERR_NONE)&&(SYSTEM_TYPE == IS_DISTRIBUTED))
    {
        if(isAdd==1)
        {
            if(local_port_no>32)
            {
        		if(isTagged ==1)
        		{
        			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].high_bmp |= (unsigned int)(1<<(local_port_no-33));
        		}
        		else
        		{
        			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].high_bmp |= (unsigned int)(1<<(local_port_no-33));
    				g_vlanlist[0].untagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));		/* remove from default vlan */		
        		}      					
            }
    		else
    		{
        		if(isTagged ==1)
        		{
        			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));
        		}
        		else
        		{
        			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));
    				g_vlanlist[0].untagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));		/* remove from default vlan */		
        		}      					
    			
    		}    
        }
    	else
    	{

            if(local_port_no>32)
            {
        		if(isTagged ==1)
        		{
        			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
        		}
        		else
        		{
        			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
        		}      					
            }
    		else
    		{
        		if(isTagged ==1)
        		{
        			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
        		}
        		else
        		{
        			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
    				g_vlanlist[0].untagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));		/* add to default vlan */		
        		}      					
    			
    		}
    	}
        
    	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
		if( ret!=0 )
        {
            syslog_ax_vlan_err("msync shm_vlan failed \n" );   
        }
    	syslog_ax_vlan_dbg("update g_vlanlist[] on local board OK !\n");
    	ret = VLAN_RETURN_CODE_ERR_NONE;		
    }
	#endif
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}


DBusMessage * npd_dbus_vlan_config_vlan_update_vid
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned short	vid_c = 0,vid_new = 0,vid_c_arry[1] = {0},vid_new_arry[1] = {0};	
	struct vlan_s*	vlanNode = NULL;
	unsigned int 	i,ret = NPD_DBUS_SUCCESS,untagbmp[2] = {0},tagbmp[2] = {0};
	struct eth_port_s *ethPort = NULL;
	unsigned int 	mbrCount = 0, eth_g_index[64] ={0};
	vlan_trunk_list_s* vlanTrunkList = NULL;
	unsigned int ifindex = 0;
	unsigned char tagMode = NPD_FALSE;
	DBusError err;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_UINT16,&vid_c,
							DBUS_TYPE_UINT16,&vid_new,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	vid_c_arry[1] = vid_c;
	vid_new_arry[1] = vid_new;
	ret = npd_check_vlan_exist(vid_new);

	if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == ret) {
		vlanNode = npd_find_vlan_by_vid(vid_c);
		if(NULL == vlanNode) {
			ret = VLAN_RETURN_CODE_BADPARAM;/* vid_c error,it'll happen?CMD-config vlan ID has already make sure*/
									/* the vlan vid_c pointing to is exist,vlanNode can not be NULL. */
		}
        else if(advanced_routing_default_vid == vid_c){
			ret = VLAN_RETURN_CODE_CONFIG_NOT_ALLOWED;
		}
		else if(npd_eth_port_check_subif_and_qinq_by_vid(vid_c)){
            ret = VLAN_RETURN_CODE_PORT_SUBINTF_EXISTS;
		}
        else if(npd_prot_vlan_vid_check_for_all_port(vid_c)){
            ret = VLAN_RETURN_CODE_HAVE_PROT_VLAN_CONFIG;
        }
        else
        {
			/*check vlan has trunk member*/
			vlanTrunkList = vlanNode->tagTrunkList;
			if(NULL != vlanTrunkList){
				if(0 == vlanTrunkList->count)
					ret = VLAN_RETURN_CODE_TRUNK_EXISTS;
			}
			ret = npd_vlan_interface_check(vid_c,&ifindex);
			if(NPD_TRUE == ret && ifindex != ~0UI){
				ret = VLAN_RETURN_CODE_L3_INTF;
			}
			else {
				/* syslog_ax_vlan_dbg("find vlan with ID vid_c.");*/
				vlanNode->vid = vid_new;
				npd_vlan_fillin_global(vid_new,vlanNode);
				npd_vlan_flushout_global(vid_c);
				/*update port attributes*/
                memset(eth_g_index,0,64);
                mbrCount = 0;
				npd_vlan_member_port_index_get(vid_new, NPD_TRUE, eth_g_index, &mbrCount);
				for(i = 0;i<mbrCount;i++) {
					ethPort = npd_get_port_by_index(eth_g_index[i]);
					if(NULL != ethPort ){
						npd_eth_port_dot1q_remove(eth_g_index[i],vid_c);
						npd_eth_port_dot1q_add(eth_g_index[i],vid_new);
					}
				}                    
                memset(eth_g_index,0,64);
                mbrCount = 0;
				npd_vlan_member_port_index_get(vid_new,NPD_FALSE, eth_g_index,&mbrCount);
				for(i = 0;i<mbrCount;i++) {
					ethPort = npd_get_port_by_index(eth_g_index[i]);
					if(NULL != ethPort ){
						npd_eth_port_set_pvid(eth_g_index[i],vid_new);
					}
				}
				nam_fdb_table_delete_entry_with_vlan(vid_c);
				ret = nam_vlan_update_vid(PRODUCT_ID, vid_c,vid_new);
				if(VLAN_CONFIG_FAIL == ret){
					ret = VLAN_RETURN_CODE_ERR_HW;
				}
				else {
					ret = nam_asic_vlan_get_port_members_bmp(vid_new, untagbmp, tagbmp);
					if(VLAN_CONFIG_SUCCESS == ret) {
						npd_mstp_del_vlan_on_mst(vid_c);
						npd_mstp_add_vlan_on_mst(vid_new,  untagbmp,  tagbmp);
					}
				}

				/*check vid_old is allowed in any trunk*/
				for(i=1;i<128;i++){
					if(NPD_TRUE == npd_vlan_check_contain_trunk(vid_c,i,&tagMode)){
						npd_trunk_refuse_vlan(i,1,vid_c_arry,tagMode);
						npd_trunk_allow_vlan(i,1,vid_new_arry,tagMode);
					}
				}
				ret = VLAN_RETURN_CODE_ERR_NONE;
			}
		}
	}
	else 
	{
		 syslog_ax_vlan_dbg("vlan new points to vlan has existed!\n");
		ret = VLAN_RETURN_CODE_VLAN_EXISTS;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}


/*original*/
DBusMessage * npd_dbus_vlan_show_vlanlist_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	static vlanList	*actvlanList[CHASSIS_VLAN_RANGE_MAX];   /* change to static , zhangdi 2011-10-13 */
	unsigned int	vlan_count = 0;
	unsigned int	i,ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned short	vlanId=0;
	char*			vlanName = NULL;
	struct vlan_s*	vlanNode = NULL;
	enum product_id_e productId = PRODUCT_ID;
	unsigned int untagMbrBmp = 0;
	unsigned int tagMbrBmp = 0;
	
	DBusError err;

	dbus_error_init(&err);
	
	for (vlanId=1;vlanId<=CHASSIS_VLAN_RANGE_MAX;vlanId++){
		ret = npd_check_vlan_exist(vlanId);
		if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS ==ret) {
			ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
			continue;
		}
		else if (VLAN_RETURN_CODE_VLAN_EXISTS == ret) {

			/*vlan_count++;*/
			vlanNode = npd_find_vlan_by_vid(vlanId);
			vlanName = vlanNode->vlanName;
			 syslog_ax_vlan_dbg("show vlan %d name %s\n",vlanId,vlanName);
			actvlanList[vlan_count] = (vlanList*)malloc(sizeof(vlanList));
			memset(actvlanList[vlan_count],0,sizeof(vlanList));
			actvlanList[vlan_count]->vlanId = vlanId;
			actvlanList[vlan_count]->vlanName = vlanNode->vlanName;
			actvlanList[vlan_count]->vlanStat = vlanNode->brgVlan.state;

			if(0 != vlanNode->tagPortList ||
					0 != vlanNode->untagPortList)
			{
				ret = npd_vlan_get_mbr_bmp_via_sw(vlanId,\
										&untagMbrBmp,\
										&tagMbrBmp);
				if(NPD_SUCCESS == ret) {
					 syslog_ax_vlan_dbg("\nactVlanList[%d].untagMbrBmp %x tagMbrBmp %x\n",vlan_count,\
																			untagMbrBmp,\
																			tagMbrBmp);
					 syslog_ax_vlan_dbg("loop iter vlanId=%d,vlan_count =%d\n",vlanId,vlan_count);
					ret = VLAN_RETURN_CODE_ERR_NONE;
				}
			}
			vlan_count++;
		}
	}
	if (vlan_count > 0) {
		ret = VLAN_RETURN_CODE_ERR_NONE;
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	/* Total active vlan count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &vlan_count);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);

	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*vlanId*/
											DBUS_TYPE_STRING_AS_STRING /*vlanname*/
										   	DBUS_TYPE_UINT32_AS_STRING /*untag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*tag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*vlan state*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < vlan_count; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		/*
		 syslog_ax_vlan_dbg("actvlanList[%d] vlanId=%d,untagBmp=%#0x ,tagBmp %#0x",i,	\
															actvlanList[i]->vlanId,\
															actvlanList[i]->untagMbrBmp,	\
															actvlanList[i]->tagMbrBmp));
		*/
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT16,
				  &(actvlanList[i]->vlanId));
		dbus_message_iter_append_basic 
				(&iter_struct,
				 DBUS_TYPE_STRING,
				 &actvlanList[i]->vlanName);
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &untagMbrBmp);
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &tagMbrBmp);	
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->vlanStat));
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(actvlanList[i]);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}

/*original*/
DBusMessage * npd_dbus_vlan_show_vlanlist_port_member_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	
    /* Here we change the location of actvlanList, because the error on XCAT */
	/* The array large than 1024, we should change it to static on XCAT */
	static vlanList* actvlanList[CHASSIS_VLAN_RANGE_MAX];
	memset( actvlanList, NULL, CHASSIS_VLAN_RANGE_MAX*(sizeof(vlanList*)) );
    /*	vlanList	*actvlanList[CHASSIS_VLAN_RANGE_MAX]; zhangdi 2011-03-22 */

	unsigned int	vlan_count = 0;
	unsigned int	i = 0, ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned short	vlanId=0;
	char*			vlanName = NULL;
	struct vlan_s*	vlanNode = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	unsigned int promisPortBmp[2] = {0};
	
	DBusError err;

	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
	    npd_vlan_get_promis_port_bmp(&promisPortBmp);
		for (vlanId=1;vlanId<=CHASSIS_VLAN_RANGE_MAX;vlanId++){
			ret = npd_check_vlan_exist(vlanId);
			if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS ==ret) {
				ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
				continue;
			}
			else if (VLAN_RETURN_CODE_VLAN_EXISTS == ret) {

				/*vlan_count++;*/
				vlanNode = npd_find_vlan_by_vid(vlanId);
				vlanName = vlanNode->vlanName;
				 syslog_ax_vlan_dbg("show vlan %d name %s\n",vlanId,vlanName);
				actvlanList[vlan_count] = (vlanList*)malloc(sizeof(vlanList));
				memset(actvlanList[vlan_count],0,sizeof(vlanList));
				actvlanList[vlan_count]->vlanId = vlanId;
				actvlanList[vlan_count]->vlanName = vlanNode->vlanName;
				actvlanList[vlan_count]->vlanStat = vlanNode->brgVlan.state;

				if(0 != vlanNode->tagPortList ||
						0 != vlanNode->untagPortList)
				{
					ret = npd_vlan_get_mbr_bmp_via_sw_v1(vlanId,\
											&(actvlanList[vlan_count]->untagMbrBmp),\
											&(actvlanList[vlan_count]->tagMbrBmp));
					if(NPD_SUCCESS == ret) {
						 syslog_ax_vlan_dbg("\nactVlanList[%d].untagMbrBmp[0] %x tagMbrBmp[0] %x\n",vlan_count,\
																				actvlanList[vlan_count]->untagMbrBmp.portMbr[0],\
																				actvlanList[vlan_count]->tagMbrBmp.portMbr[0]);
						 syslog_ax_vlan_dbg("loop iter vlanId=%d,vlan_count =%d\n",vlanId,vlan_count);
						ret = VLAN_RETURN_CODE_ERR_NONE;
					}
				}
				vlan_count++;
			}
		}
		if (vlan_count > 0) {
			ret = VLAN_RETURN_CODE_ERR_NONE;
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	/* Total active vlan count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &vlan_count);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &promisPortBmp[0]);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &promisPortBmp[1]);


	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*vlanId*/
											DBUS_TYPE_STRING_AS_STRING /*vlanname*/
										   	DBUS_TYPE_UINT32_AS_STRING /*untag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*untag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*tag	*/									   	
										   	DBUS_TYPE_UINT32_AS_STRING /*tag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*vlan state*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < vlan_count; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		/*
		 syslog_ax_vlan_dbg("actvlanList[%d] vlanId=%d,untagBmp=%#0x ,tagBmp %#0x",i,	\
															actvlanList[i]->vlanId,\
															actvlanList[i]->untagMbrBmp,	\
															actvlanList[i]->tagMbrBmp));
		*/
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT16,
				  &(actvlanList[i]->vlanId));
		dbus_message_iter_append_basic 
				(&iter_struct,
				 DBUS_TYPE_STRING,
				 &actvlanList[i]->vlanName);
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->untagMbrBmp.portMbr[0]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->untagMbrBmp.portMbr[1]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->tagMbrBmp.portMbr[0]));		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->tagMbrBmp.portMbr[1]));	
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->vlanStat));
		dbus_message_iter_close_container (&iter_array, &iter_struct);

	syslog_ax_vlan_dbg("\n vlan_count %d, vlanId %d,vlanName %s,actvlanList[%d]->vlanStat %d,\n"\
		"actvlanList[%d]->untagMbrBmp.portMbr[0] %x, actvlanList[%d]->untagMbrBmp.portMbr[1] %x,\n"\
		"actvlanList[%d]->tagMbrBmp.portMbr[0] %x, actvlanList[%d]->tagMbrBmp.portMbr[1] %x.\n",\
		vlan_count,actvlanList[i]->vlanId,actvlanList[i]->vlanName,i,actvlanList[i]->vlanStat,\
	    i,actvlanList[i]->untagMbrBmp.portMbr[0],i,actvlanList[i]->untagMbrBmp.portMbr[1],\
		i,actvlanList[i]->tagMbrBmp.portMbr[0],i,actvlanList[i]->tagMbrBmp.portMbr[1]);

		free(actvlanList[i]);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}

DBusMessage * npd_dbus_vlan_show_vlanlist_port_member_v2
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	static vlanList	*actvlanList[CHASSIS_VLAN_RANGE_MAX];   /* change to static , zhangdi 2011-10-13 */
	unsigned int	vlan_count = 0;
	unsigned int	i = 0, ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned short	vlanId=0;
	char*			vlanName = NULL;
	struct vlan_s*	vlanNode = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	unsigned int promisPortBmp[2] = {0};
    unsigned int ifIndex = ~0UI;
	
	DBusError err;

	dbus_error_init(&err);
	
    npd_vlan_get_promis_port_bmp(&promisPortBmp);
	for (vlanId=1;vlanId<=CHASSIS_VLAN_RANGE_MAX;vlanId++){
		ret = npd_check_vlan_exist(vlanId);
		if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS ==ret) {
			ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
			continue;
		}
		else if (VLAN_RETURN_CODE_VLAN_EXISTS == ret) {

			/*vlan_count++;*/
			vlanNode = npd_find_vlan_by_vid(vlanId);
            if(!vlanNode){
                continue;
            }
			vlanName = vlanNode->vlanName;
            if((TRUE != npd_vlan_interface_check(vlanId,&ifIndex))||\
                (ifIndex == ~0UI)){
                /*not simple vlan intf,maybe is advanced-routing*/
			    npd_intf_ifindex_get_by_ifname((unsigned char *)vlanName,&ifIndex);
            }
            if(!ifIndex){ /* 0 is invalid ifindex */
                ifIndex = ~0UI;
            }
			actvlanList[vlan_count] = (vlanList*)malloc(sizeof(vlanList));
			memset(actvlanList[vlan_count],0,sizeof(vlanList));
			actvlanList[vlan_count]->vlanId = vlanId;
			actvlanList[vlan_count]->vlanName = vlanNode->vlanName;            
            actvlanList[vlan_count]->ifIndex = ifIndex;
			actvlanList[vlan_count]->vlanStat = vlanNode->brgVlan.state;

			if(0 != vlanNode->tagPortList ||
					0 != vlanNode->untagPortList)
			{
				ret = npd_vlan_get_mbr_bmp_via_sw_v1(vlanId,\
										&(actvlanList[vlan_count]->untagMbrBmp),\
										&(actvlanList[vlan_count]->tagMbrBmp));
				if(NPD_SUCCESS == ret) {
					 syslog_ax_vlan_dbg("\nactVlanList[%d].untagMbrBmp[0] %x tagMbrBmp[0] %x\n",vlan_count,\
																			actvlanList[vlan_count]->untagMbrBmp.portMbr[0],\
																			actvlanList[vlan_count]->tagMbrBmp.portMbr[0]);
					 syslog_ax_vlan_dbg("loop iter vlanId=%d,vlan_count =%d\n",vlanId,vlan_count);
					ret = VLAN_RETURN_CODE_ERR_NONE;
				}
			}
			vlan_count++;
		}
	}
	if (vlan_count > 0) {
		ret = VLAN_RETURN_CODE_ERR_NONE;
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	/* Total active vlan count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &vlan_count);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &promisPortBmp[0]);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &promisPortBmp[1]);


	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*vlanId*/
											DBUS_TYPE_STRING_AS_STRING /*vlanname*/
											DBUS_TYPE_UINT32_AS_STRING /* ifIndex*/
										   	DBUS_TYPE_UINT32_AS_STRING /*untag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*untag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*tag	*/									   	
										   	DBUS_TYPE_UINT32_AS_STRING /*tag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*vlan state*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < vlan_count; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		/*
		 syslog_ax_vlan_dbg("actvlanList[%d] vlanId=%d,untagBmp=%#0x ,tagBmp %#0x",i,	\
															actvlanList[i]->vlanId,\
															actvlanList[i]->untagMbrBmp,	\
															actvlanList[i]->tagMbrBmp));
		*/
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT16,
				  &(actvlanList[i]->vlanId));
		dbus_message_iter_append_basic 
				(&iter_struct,
				 DBUS_TYPE_STRING,
				 &actvlanList[i]->vlanName);
        dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->ifIndex));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->untagMbrBmp.portMbr[0]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->untagMbrBmp.portMbr[1]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->tagMbrBmp.portMbr[0]));		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->tagMbrBmp.portMbr[1]));	
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->vlanStat));
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(actvlanList[i]);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}


DBusMessage * npd_dbus_vlan_show_vlanlist_trunk_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	DBusError err;
	#if 0
	vlanList	*actvlanList[CHASSIS_VLAN_RANGE_MAX];
	#endif
	static vlanList* actvlanList[CHASSIS_VLAN_RANGE_MAX];
	memset( actvlanList, NULL, CHASSIS_VLAN_RANGE_MAX*(sizeof(vlanList*)) );
	unsigned int	vlan_count = 0;
	unsigned int	i = 0, ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned short	vlanId = 0;
	char*			vlanName = NULL;
	struct vlan_s*	vlanNode = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	

	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		for(vlanId=CHASSIS_VLAN_RANGE_MIN; vlanId<=CHASSIS_VLAN_RANGE_MAX; vlanId++) {
			if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
			{
				ret = VLAN_RETURN_CODE_TRUNK_NOTEXISTS;
				break;
			}
			ret = npd_check_vlan_exist(vlanId);
			if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == ret) {
				ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
				continue;
			}
			else if (VLAN_RETURN_CODE_VLAN_EXISTS == ret) {
				/*vlan_count++;*/
				vlanNode = npd_find_vlan_by_vid(vlanId);
				vlanName = vlanNode->vlanName;
				 syslog_ax_vlan_dbg("show vlan %d name %s\n",vlanId,vlanName);
				actvlanList[vlan_count] = (vlanList*)malloc(sizeof(vlanList));
				if(NULL == actvlanList[vlan_count]){
					syslog_ax_vlan_dbg("Malloc error\n");
					continue;
				}
				memset(actvlanList[vlan_count],0,sizeof(vlanList));
				actvlanList[vlan_count]->vlanId = vlanId;
				actvlanList[vlan_count]->vlanName = vlanNode->vlanName;
				actvlanList[vlan_count]->vlanStat = vlanNode->brgVlan.state;
				if(NULL != vlanNode->tagTrunkList || NULL != vlanNode->unTagtrunkList) {
					syslog_ax_vlan_dbg("get vlan trunk member in vlan %d\n",vlanId);
					ret = npd_vlan_get_trunk_mbr_bmp_via_sw(vlanId,\
											&(actvlanList[vlan_count]->untagTrkBmp),\
											&(actvlanList[vlan_count]->tagTrkBmp));
					if(NPD_SUCCESS == ret) {
						ret = VLAN_RETURN_CODE_ERR_NONE;
					}
				}
				vlan_count++;
			}
		}
		if (vlan_count > 0) {
			ret = VLAN_RETURN_CODE_ERR_NONE;
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	/* Total active vlan count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &vlan_count);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);

	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*vlanId*/
											DBUS_TYPE_STRING_AS_STRING /*vlanname*/
											DBUS_TYPE_UINT32_AS_STRING /*untagBmp*/
											DBUS_TYPE_UINT32_AS_STRING /*untagBmp*/
											DBUS_TYPE_UINT32_AS_STRING /*untagBmp*/
											DBUS_TYPE_UINT32_AS_STRING /*untagBmp*/
											DBUS_TYPE_UINT32_AS_STRING /*tagBmp*/
											DBUS_TYPE_UINT32_AS_STRING /*tagBmp*/
											DBUS_TYPE_UINT32_AS_STRING /*tagBmp*/
										   	DBUS_TYPE_UINT32_AS_STRING /*tagBmp*/
										   	DBUS_TYPE_UINT32_AS_STRING /*vlan state*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < vlan_count; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		/*
		 syslog_ax_vlan_dbg("actvlanList[%d] vlanId=%d,untagBmp=%#0x ,tagBmp %#0x",i,	\
															actvlanList[i]->vlanId,\
															actvlanList[i]->untagMbrBmp,	\
															actvlanList[i]->tagMbrBmp));
		*/
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT16,
				  &(actvlanList[i]->vlanId));
		dbus_message_iter_append_basic 
				(&iter_struct,
				 DBUS_TYPE_STRING,
				 &actvlanList[i]->vlanName);		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->untagTrkBmp.trunkMbr[0]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->untagTrkBmp.trunkMbr[1]));	
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->untagTrkBmp.trunkMbr[2]));	
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->untagTrkBmp.trunkMbr[3]));	
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->tagTrkBmp.trunkMbr[0]));	
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->tagTrkBmp.trunkMbr[1]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->tagTrkBmp.trunkMbr[2]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->tagTrkBmp.trunkMbr[3]));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->vlanStat));
		
		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(actvlanList[i]);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}

/*show a special vlan port-member slot_port*/
DBusMessage * npd_dbus_vlan_show_vlan_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter	iter;
	DBusError err;
	unsigned short	vlanId = 0;
	char*			vlanName = NULL;
	struct vlan_s*	vlanNode = NULL;
	unsigned int	op_ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned int untagBmp = 0,tagBmp = 0;
	enum product_id_e productId = PRODUCT_ID; 
	unsigned int vlanStat = VLAN_STATE_DOWN_E; /* add by zhubo@autelan.com*/ 
	
	vlanName = (char*)malloc(ALIAS_NAME_SIZE+1);
	if(NULL == vlanName){
		return reply;
	}	
	memset(vlanName,0,ALIAS_NAME_SIZE+1);
	 syslog_ax_vlan_dbg("Entering show vlan port members!\n");

	/*Read from SW Record or vlan entry table in HW*/
	/*here choose the later method*/
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&vlanId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		free(vlanName);
		vlanName = NULL;
		return NULL;
	}

	if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
		op_ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN;
	}
	else if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == npd_check_vlan_exist(vlanId)) {
		op_ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
		 syslog_ax_vlan_dbg("vlanId =%d NOT exists.\n",vlanId);
		/*return op_ret;*/
	}
	else 
	{
		vlanNode = npd_find_vlan_by_vid(vlanId);
		vlanStat = vlanNode->brgVlan.state;  /*by zhubo@autelan.com*/
		memcpy(vlanName,vlanNode->vlanName,ALIAS_NAME_SIZE);
		if (0 != vlanNode->tagPortList ||
			0 != vlanNode->untagPortList) {
			op_ret = npd_vlan_get_mbr_bmp_via_sw(vlanId,&untagBmp,&tagBmp);
			if(op_ret != NPD_SUCCESS) {
				 syslog_ax_vlan_err("get vlan %d port bitmap error\n",vlanId);
			}
			else { 
				 syslog_ax_vlan_dbg("vlan %d :untagBmp %#0X,tagBmp %#0X\n",vlanId,untagBmp,tagBmp);
				op_ret = VLAN_RETURN_CODE_ERR_NONE;
			}
		}		
	}
	

	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&op_ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);

	/* syslog_ax_vlan_dbg("dbus_message_iter_append_basic:return op_ret %d",op_ret);*/
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_STRING,&vlanName);
	
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagBmp);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagBmp);

	/*by zhubo@autelan.com*/
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&vlanStat);
	free(vlanName);
	vlanName = NULL;
	return reply;
}


/*show a special vlan port-member slot_port_v1*/
DBusMessage * npd_dbus_vlan_show_vlan_port_member_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter	iter;
	DBusError err;
	unsigned short	vlanId = 0;
	char*			vlanName = NULL;
	struct vlan_s*	vlanNode = NULL;
	unsigned int	op_ret = VLAN_RETURN_CODE_ERR_NONE;
	PORT_MEMBER_BMP untagMbrBmp, tagMbrBmp;
	PORT_MEMBER_BMP tagMbrBmpwan;
	unsigned int wan = 0;
	unsigned int    untagPortWord[2] = {0}, tagPortWord[2] = {0};	
	enum product_id_e productId = PRODUCT_ID; 
	unsigned int vlanStat = VLAN_STATE_DOWN_E; /* add by zhubo@autelan.com */
	unsigned int promisPortBmp[2] = {0};
	int i = 0;
	memset(&untagMbrBmp ,0 ,sizeof(PORT_MEMBER_BMP));
	memset(&tagMbrBmp ,0 ,sizeof(PORT_MEMBER_BMP));	
	memset(&tagMbrBmpwan ,0 ,sizeof(PORT_MEMBER_BMP));
	
	vlanName = (char*)malloc(ALIAS_NAME_SIZE+1);
	if(NULL == vlanName){
		return reply;
	}	
	memset(vlanName,0,ALIAS_NAME_SIZE+1);
	 syslog_ax_vlan_dbg("Entering show vlan port members!\n");

	/*Read from SW Record or vlan entry table in HW*/
	/*here choose the later method*/
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&vlanId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		free(vlanName);	
		vlanName = NULL;
		return NULL;
	}

	if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
		op_ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN;
	}
	else if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == npd_check_vlan_exist(vlanId)) {
		op_ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
		 syslog_ax_vlan_dbg("vlanId =%d NOT exists.\n",vlanId);
		/*return op_ret;*/
	}
	else 
	{
	    npd_vlan_get_promis_port_bmp(&promisPortBmp);
		vlanNode = npd_find_vlan_by_vid(vlanId);
		vlanStat = vlanNode->brgVlan.state;  /*by zhubo@autelan.com*/
		memcpy(vlanName,vlanNode->vlanName,ALIAS_NAME_SIZE);
		if (0 != vlanNode->tagPortList ||
			0 != vlanNode->untagPortList) {
			op_ret = npd_vlan_get_mbr_bmp_via_sw_v1(vlanId,&untagMbrBmp,&tagMbrBmp);
			if(op_ret != NPD_SUCCESS) {
				 syslog_ax_vlan_err("get vlan %d port bitmap error\n",vlanId);
				 op_ret = VLAN_RETURN_CODE_ERR_GENERAL;
			}
			else { 
				 syslog_ax_vlan_dbg("vlan %d :untagBmp %#0X,tagBmp %#0X\n",vlanId,untagMbrBmp,tagMbrBmp);
				op_ret = VLAN_RETURN_CODE_ERR_NONE;
			}
		}
		for(i=0;i<2;i++){
			untagPortWord[i] = untagMbrBmp.portMbr[i];
			tagPortWord[i] = tagMbrBmp.portMbr[i];
		}		
	}

	op_ret = npd_vlan_get_mbr_bmp_wan(vlanId,&tagMbrBmpwan);
	if(op_ret != NPD_SUCCESS) {
		 syslog_ax_vlan_err("get master port vlan %d port bitmap error\n",vlanId);
		 op_ret = VLAN_RETURN_CODE_ERR_GENERAL;
	}
	else { 
		 syslog_ax_vlan_dbg("vlan %d :tagbmp[0] 0x%x,tagbmp[1] 0x%x\n",vlanId,tagMbrBmpwan.portMbr[0],tagMbrBmpwan.portMbr[1]);
		op_ret = VLAN_RETURN_CODE_ERR_NONE;
	}
	
	for(i=0;i<2;i++){
		tagPortWord[i] = tagPortWord[i] | tagMbrBmpwan.portMbr[i];
		syslog_ax_vlan_dbg("tagPortWord[0] %x,tagPortWord[1] %x",tagPortWord[0],tagPortWord[1]);
	}
	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&op_ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &promisPortBmp[0]);

	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &promisPortBmp[1]);

	/* syslog_ax_vlan_dbg("dbus_message_iter_append_basic:return op_ret %d",op_ret);*/
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_STRING,&vlanName);
	
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagPortWord[0]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagPortWord[1]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagPortWord[0]);	
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagPortWord[1]);

	/*by zhubo@autelan.com*/
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&vlanStat);
	free(vlanName);
	vlanName = NULL;
	return reply;
}

/*show a special vlan trunk-member slot_port*/
DBusMessage * npd_dbus_vlan_show_vlan_trunk_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply = NULL;
	DBusMessageIter		iter;
	DBusError err;
	unsigned int	i = 0,ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned short	vlanId=0;
	char*			vlanName = NULL;
	TRUNK_MEMBER_BMP untagTrkBmp, tagTrkBmp;
	unsigned int    untagTrkWord[4] = {0}, tagTrkWord[4] = {0};
	unsigned int vlanStat = 0;
	struct vlan_s*	vlanNode = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	
	memset(&untagTrkBmp ,0 ,sizeof(TRUNK_MEMBER_BMP));
	memset(&tagTrkBmp ,0 ,sizeof(TRUNK_MEMBER_BMP));

	vlanName = (char*)malloc(ALIAS_NAME_SIZE+1);
	if(NULL == vlanName){
		return reply;
	}
	memset(vlanName,0,ALIAS_NAME_SIZE+1);	
	
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_UINT16,&vlanId,
			DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			free(vlanName);
			vlanName = NULL;
			return NULL;
		}
		
		if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
			ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN;
		}
		else if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == npd_check_vlan_exist(vlanId)) {
			ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
			 syslog_ax_vlan_dbg("vlanId =%d NOT exists.\n",vlanId);
			/*return op_ret;*/
		}
		else 
		{
			vlanNode = npd_find_vlan_by_vid(vlanId);
			vlanStat = vlanNode->brgVlan.state;  /*by zhubo@autelan.com*/
			memcpy(vlanName,vlanNode->vlanName,ALIAS_NAME_SIZE);
			/*vlanName = vlanNode->vlanName;*/
			if (NULL != vlanNode->tagTrunkList ||
				NULL != vlanNode->unTagtrunkList) {
				ret = npd_vlan_get_trunk_mbr_bmp_via_sw(vlanId,\
										&untagTrkBmp,\
										&tagTrkBmp);
				syslog_ax_vlan_dbg("npd_vlan_get_trunk_mbr_bmp_via_sw vlan %d ret %d \n",vlanId,ret);
				if(NPD_SUCCESS == ret) {
					ret = VLAN_RETURN_CODE_ERR_NONE;
				}
			}
			for(i=0;i<4;i++){
				untagTrkWord[i] = untagTrkBmp.trunkMbr[i];
				tagTrkWord[i] = tagTrkBmp.trunkMbr[i];
			}
		}
	}
	
	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);

	/* syslog_ax_vlan_dbg("dbus_message_iter_append_basic:return op_ret %d",op_ret);*/
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_STRING,&vlanName);
	
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagTrkWord[0]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagTrkWord[1]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagTrkWord[2]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagTrkWord[3]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagTrkWord[0]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagTrkWord[1]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagTrkWord[2]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagTrkWord[3]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&vlanStat);
	free(vlanName);
	vlanName = NULL;
	return reply;
}

/*show a special vlan member slot_port*/
DBusMessage * npd_dbus_vlan_show_vlan_port_member_vname
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter	iter;
	unsigned short	vlanId = 0;
	char*			vlanName = NULL;
	struct vlan_s* 	vlanNode = NULL;
	unsigned int	op_ret = VLAN_RETURN_CODE_ERR_NONE;
	DBusError err;
	unsigned int untagBmp = 0,tagBmp = 0;
	enum product_id_e productId = PRODUCT_ID;
	unsigned int vlanStat = 0;

	 syslog_ax_vlan_dbg("Entering show vlan port members!\n");

	/*Read from SW Record or vlan entry table in HW*/
	/*here choose the later method*/
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_STRING,&vlanName,
								DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	/* syslog_ax_vlan_dbg("show vlan %s. \n",vlanName);*/
	vlanNode = npd_find_vlan_by_name(vlanName);
	if(NULL == vlanNode){
		op_ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
	}
	else {
		vlanId = vlanNode->vid;
		vlanStat = vlanNode->brgVlan.state;
		if(0 != vlanNode->tagPortList ||
		   0 != vlanNode->untagPortList)
		{
			op_ret = npd_vlan_get_mbr_bmp_via_sw(vlanId,&untagBmp,&tagBmp);
			if(op_ret != NPD_SUCCESS) {
				 syslog_ax_vlan_err("get vlan %d port bitmap error\n",vlanId);
			}
			else { 
				 syslog_ax_vlan_dbg("vlan %d :untagBmp %#0X,tagBmp %#0X\n",vlanId,untagBmp,tagBmp);
				op_ret = VLAN_RETURN_CODE_ERR_NONE;
			}
		}
	}
	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&op_ret);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);
	/* syslog_ax_vlan_dbg("dbus_message_iter_append_basic:return op_ret %d",op_ret);*/
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT16,&vlanId);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagBmp);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagBmp);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&vlanStat);
	return reply;
}

/*show a special vlan member slot_port*/
DBusMessage * npd_dbus_vlan_show_vlan_port_member_vname_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter	iter;
	unsigned short	vlanId= 0;
	char*			vlanName = NULL;
	struct vlan_s* 	vlanNode =NULL;
	unsigned int	op_ret = VLAN_RETURN_CODE_ERR_NONE;
	DBusError err;
	PORT_MEMBER_BMP untagMbrBmp,tagMbrBmp;
	unsigned int    untagPortWord[2] = {0},tagPortWord[2] = {0};		
	enum product_id_e productId = PRODUCT_ID;
	unsigned int vlanStat = 0;
	unsigned int promisPortBmp[2] = {0};
	int i = 0;

	memset(&untagMbrBmp ,0 ,sizeof(PORT_MEMBER_BMP));
	memset(&tagMbrBmp ,0 ,sizeof(PORT_MEMBER_BMP));		

	 syslog_ax_vlan_dbg("Entering show vlan port members!\n");

	/*Read from SW Record or vlan entry table in HW*/
	/*here choose the later method*/
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_STRING,&vlanName,
									DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}	
		npd_vlan_get_promis_port_bmp(&promisPortBmp);
		/* syslog_ax_vlan_dbg("show vlan %s. \n",vlanName);*/
		vlanNode = npd_find_vlan_by_name(vlanName);
		if(NULL == vlanNode){
			op_ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
		}
		else {
			vlanId = vlanNode->vid;
			vlanStat = vlanNode->brgVlan.state;
			if(0 != vlanNode->tagPortList ||
			   0 != vlanNode->untagPortList)
			{
				op_ret = npd_vlan_get_mbr_bmp_via_sw_v1(vlanId,&untagMbrBmp,&tagMbrBmp);
				if(op_ret != NPD_SUCCESS) {
					 syslog_ax_vlan_err("get vlan %d port bitmap error\n",vlanId);
					op_ret = VLAN_RETURN_CODE_ERR_GENERAL;
				}
				else { 
					 syslog_ax_vlan_dbg("vlan %d :untagBmp %#0X,tagBmp %#0X\n",vlanId,untagMbrBmp,tagMbrBmp);
					op_ret = VLAN_RETURN_CODE_ERR_NONE;
				}
			}		
			for(i=0;i<2;i++){
				untagPortWord[i] = untagMbrBmp.portMbr[i];
				tagPortWord[i] = tagMbrBmp.portMbr[i];
			}		
		}
	}
	
	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&op_ret);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &promisPortBmp[0]);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &promisPortBmp[1]);

	/* syslog_ax_vlan_dbg("dbus_message_iter_append_basic:return op_ret %d",op_ret);*/
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT16,&vlanId);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagPortWord[0]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagPortWord[1]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagPortWord[0]);	
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagPortWord[1]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&vlanStat);
	return reply;
}

/*show a special vlan trunk members*/
DBusMessage * npd_dbus_vlan_show_vlan_trunk_member_vname
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply = NULL;
	DBusMessageIter	iter;
	unsigned int	i = 0,op_ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned short	vlanId = 0;
	char*			vlanName = NULL;
	TRUNK_MEMBER_BMP untagTrkBmp, tagTrkBmp;
	unsigned int    untagTrkWord[4] = {0},tagTrkWord[4] = {0};
	unsigned int vlanStat = 0;
	struct vlan_s*	vlanNode = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	DBusError err;

	memset(&untagTrkBmp, 0, sizeof(TRUNK_MEMBER_BMP));
	memset(&tagTrkBmp, 0, sizeof(TRUNK_MEMBER_BMP));

	 syslog_ax_vlan_dbg("Entering show vlan trunk members!\n");

	/*Read from SW Record or vlan entry table in HW*/
	/*here choose the later method*/
	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		op_ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_STRING,&vlanName,
									DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		/* syslog_ax_vlan_dbg("show vlan %s. \n",vlanName);*/
		vlanNode = npd_find_vlan_by_name(vlanName);
		if(NULL == vlanNode){
			op_ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
		}
		else {
			vlanId = vlanNode->vid;
			vlanStat = vlanNode->brgVlan.state;
			if(0 != vlanNode->tagTrunkList ||
			   0 != vlanNode->unTagtrunkList){
				op_ret = npd_vlan_get_trunk_mbr_bmp_via_sw(vlanId,&untagTrkBmp,&tagTrkBmp);
				if(NPD_SUCCESS == op_ret) {
					op_ret = VLAN_RETURN_CODE_ERR_NONE;
				}
			}
			for(i=0;i<4;i++){
				untagTrkWord[i] = untagTrkBmp.trunkMbr[i];
				tagTrkWord[i] = tagTrkBmp.trunkMbr[i];
			}	
		}
	}
	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append (reply, &iter);
		
	dbus_message_iter_append_basic (&iter,
									DBUS_TYPE_UINT32,&op_ret);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);
	/* syslog_ax_vlan_dbg("dbus_message_iter_append_basic:return op_ret %d",op_ret);*/
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT16,&vlanId);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagTrkWord[0]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagTrkWord[1]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagTrkWord[2]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&untagTrkWord[3]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagTrkWord[0]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagTrkWord[1]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagTrkWord[2]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&tagTrkWord[3]);
	dbus_message_iter_append_basic(&iter,
								DBUS_TYPE_UINT32,&vlanStat);
	return reply;

	
}

#if 0
/*add | delete trunk to(from) vlan*/
DBusMessage * npd_dbus_vlan_config_trunk_add_del(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	isAdd;
	/*unsigned char	isTagged;*/
	unsigned short	trunkId;
	unsigned short	vlanId;	

	unsigned int 	ret;
	DBusError err;
	// syslog_ax_vlan_dbg("Enter :vlan_config_trunk_add_del..\n");	

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&isAdd,
		DBUS_TYPE_UINT16,&trunkId,
		/*DBUS_TYPE_BYTE,&isTagged,*/
		DBUS_TYPE_UINT16,&vlanId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	// syslog_ax_vlan_dbg("%s trunk %d vlan %d ...\n",isAdd?"Add":"Delete",trunkId,vlanId);

	if(isAdd){
		ret = npd_vlan_trunk_entity_add(vlanId,trunkId,NPD_FALSE);
	}
	else{
		ret = npd_vlan_trunk_entity_del(vlanId,trunkId,NPD_FALSE);
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}
#endif

/*add | delete trunk to(from) vlan*/
DBusMessage * npd_dbus_vlan_config_trunk_untag_tag_add_del
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	isAdd = 0;
	unsigned char	isTagged = 0;
	unsigned short	trunkId = 0;
	unsigned short	vlanId = 0;	

	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;
	DBusError err;	

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_BYTE,&isAdd,
		DBUS_TYPE_UINT16,&trunkId,
		DBUS_TYPE_BYTE,&isTagged,
		DBUS_TYPE_UINT16,&vlanId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	/* syslog_ax_vlan_dbg("%s trunk %d vlan %d ...\n",isAdd?"Add":"Delete",trunkId,vlanId);*/

	if(isAdd){
		ret = npd_vlan_trunk_entity_add(vlanId,trunkId,isTagged);
	}
	else{
		ret = npd_vlan_trunk_entity_del(vlanId,trunkId,isTagged);
	}
	/* update trunk info of g_vlanlist[] */
	if(ret == VLAN_RETURN_CODE_ERR_NONE)
	{
		/* add trunk untag to default vlan */
        ret = npd_vlanlist_add_del_trunk(vlanId,trunkId,isAdd,isTagged);
		if(0 != ret)
		{
    		syslog_ax_vlan_err("update trunk %d info to vlan %d error!!!\n",trunkId,vlanId);			
		    ret = VLAN_RETURN_CODE_ERR_HW;			
		}
		else
		{
		    ret = VLAN_RETURN_CODE_ERR_NONE;
		}
	}	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}

DBusMessage * npd_dbus_vlan_delete_vlan_entry_one(DBusConnection *conn, DBusMessage *msg, void *user_data) {
    if((SYSTEM_TYPE == IS_DISTRIBUTED) || (PRODUCT_ID == PRODUCT_ID_AX7K_I))
    {
    	DBusMessage* reply;
    	DBusMessageIter	 iter;
    	unsigned short	vlanId = 0;
    	struct vlan_s*	vlanNode = NULL;
    	unsigned int	ret = VLAN_RETURN_CODE_ERR_NONE;
        unsigned int ifIndex = ~0UI;
        enum VLAN_PORT_SUBIF_FLAG  flag = VLAN_PORT_SUBIF_NOTEXIST;
    	DBusError err;
    	unsigned int tmp,Igroupnum=0, Egroupnum=0, dret=0;
    	
    	dbus_error_init(&err);
    	
    	if (!(dbus_message_get_args ( msg, &err,
    		DBUS_TYPE_UINT16,&vlanId,
    		DBUS_TYPE_INVALID))) {
    		 syslog_ax_vlan_err("Unable to get input args ");
    		if (dbus_error_is_set(&err)) {
    			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
    			dbus_error_free(&err);
    		}
    		return NULL;
    	}
    	/* syslog_ax_vlan_dbg("delete vlan entry %d.\n",vlanId);*/
    	vlanNode = npd_find_vlan_by_vid(vlanId);
    	if(NULL == vlanNode) {
    		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
    		reply = dbus_message_new_method_return(msg);
    		
    		dbus_message_iter_init_append (reply, &iter);
    		
    		dbus_message_iter_append_basic (&iter,
    										 DBUS_TYPE_UINT32, 
    										 &ret);
    		return reply;
    	}
    	if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
    		 syslog_ax_vlan_err("illegal vlanId %d\n",vlanId);
    		ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN;
    	} 
    	else if(DEFAULT_VLAN_ID == vlanId) {
    		ret = VLAN_RETURN_CODE_BADPARAM;
    	}       
    	else if(vlanNode->isAutoCreated){
    		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
    	}
    	else if((advanced_routing_default_vid == vlanId)&&\
    		(NPD_ADV_ROUTING_DEFAULT_VID != vlanId)){
    		ret = VLAN_RETURN_CODE_ADVANCED_VLAN_CANNOT_DEL;
    	}
        else if((NPD_TRUE == npd_vlan_interface_check(vlanId,&ifIndex))&&\
            (ifIndex !=(~0UI))){
            		ret = VLAN_RETURN_CODE_L3_INTF;
        }
		/* 1 */
        else if((NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag))&&\
             (flag == VLAN_PORT_SUBIF_EXIST)) {
            ret = VLAN_RETURN_CODE_SUBINTF_EXISTS;
        }
		/* 2 should instead 1 */
		else if(VLAN_BONDED == npd_vlan_check_bond_info(vlanId))
		{
     		syslog_ax_vlan_err("vlan%d is bonded, Please unbond first !\n",vlanId);
    		ret = VLAN_RETURN_CODE_SUBINTF_EXISTS;			
		}		
        else if(npd_eth_port_check_subif_and_qinq_by_vid(vlanId)){
            ret = VLAN_RETURN_CODE_PORT_SUBINTF_EXISTS;
        }
        else if(npd_prot_vlan_vid_check_for_all_port(vlanId)){
            ret = VLAN_RETURN_CODE_HAVE_PROT_VLAN_CONFIG;
        }
        else
		{
			ret = npd_fdb_static_entry_del_by_vlan(vlanId);  /*slove bug AXSSZFI-176*/
			if( VLAN_RETURN_CODE_ERR_NONE != ret){
                npd_syslog_err("Delete fdb static by vlan in sw error: %d\n",ret);
                ret = VLAN_RETURN_CODE_ERR_GENERAL;
			}
			else{
				ret =  nam_fdb_static_table_delete_entry_with_vlan(vlanId);
				if (ret != VLAN_RETURN_CODE_ERR_NONE) {
					npd_syslog_err("nam_fdb_table_delete_entry_with_vlan:erro !\n");
                    ret = VLAN_RETURN_CODE_ERR_GENERAL;
				}
			}

			/*disable qinq before delete vlan */
			ret = npd_vlan_qinq_disable(vlanId);
			if(ret != VLAN_RETURN_CODE_ERR_NONE)
			{
				npd_syslog_err("npd vlan qinq disable FAIL !\n");
				ret = VLAN_RETURN_CODE_ERR_GENERAL;
			}
				
    	    tmp=npd_vlan_acl_bind_check(vlanId,&Igroupnum,ACL_DIRECTION_INGRESS);       
    		if(ACL_GROUP_VLAN_BINDED==tmp)/*api*/
    		{
    			dret=npd_vlan_unbind_group(vlanId,Igroupnum,ACL_DIRECTION_INGRESS);
    			if(dret!=NPD_DBUS_SUCCESS) {
    				syslog_ax_acl_dbg("delete acl group failure\n");
    				ret = NPD_DBUS_ERROR;
    			}			
    		}
    		tmp=npd_vlan_acl_bind_check(vlanId,&Egroupnum,ACL_DIRECTION_EGRESS);       
    		if(ACL_GROUP_VLAN_BINDED==tmp)/*api*/
    		{
    			dret=npd_vlan_unbind_group(vlanId,Egroupnum,ACL_DIRECTION_EGRESS);
    			if(dret!=NPD_DBUS_SUCCESS) {
    				syslog_ax_acl_dbg("delete acl group failure\n");
    				ret = NPD_DBUS_ERROR;
    			}			
    		}
    		syslog_ax_vlan_dbg("To destroy vlan %d entity.\n",vlanId);
    		ret = npd_vlan_interface_destroy_node(vlanId);
    		if (VLAN_RETURN_CODE_ERR_NONE != ret) { /*no use if struct*/
          		syslog_ax_vlan_dbg("To destroy vlan %d entity error !\n",vlanId);				
    		}
			else
			{
				#if 1
    			npd_mstp_del_vlan_on_mst(vlanId);
				#endif
                /* add for distributed OS, add cscd port to all vlan */
        		ret = npd_vlan_cscd_del(vlanId);
        		if (VLAN_RETURN_CODE_ERR_NONE != ret) {
        			syslog_ax_vlan_err("npd_vlan_cscd_del vlan %d error\n",vlanId);
        			ret = VLAN_RETURN_CODE_ERR_GENERAL;
        		}
				else
				{
        			/* we need update the g_vlanlist[] on local-board */
            		ret = npd_vlanlist_invalid(vlanId);
            		if (VLAN_RETURN_CODE_ERR_NONE != ret) {
            			syslog_ax_vlan_err("npd set npd_vlanlist_invalid error !\n");
            			ret = VLAN_RETURN_CODE_ERR_GENERAL;
            		}
        			/* update g_vlans[] on AX81-SMU, for L3-vlan */
        			#if 0  /* done in npd_vlan_interface_destroy_node() */
        			g_vlans[vlanId-1]->brgVlan.state = VLAN_STATE_DOWN_E;
        		    #endif
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
	else
    {
    	DBusMessage* reply;
    	DBusMessageIter	 iter;
    	unsigned short	vlanId = 0;
    	struct vlan_s*	vlanNode = NULL;
    	unsigned int	ret = VLAN_RETURN_CODE_ERR_NONE;
        unsigned int ifIndex = ~0UI;
        enum VLAN_PORT_SUBIF_FLAG  flag = VLAN_PORT_SUBIF_NOTEXIST;
    	DBusError err;
    	unsigned int tmp,Igroupnum=0, Egroupnum=0, dret=0;
    	
    	dbus_error_init(&err);
    	
    	if (!(dbus_message_get_args ( msg, &err,
    		DBUS_TYPE_UINT16,&vlanId,
    		DBUS_TYPE_INVALID))) {
    		 syslog_ax_vlan_err("Unable to get input args ");
    		if (dbus_error_is_set(&err)) {
    			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
    			dbus_error_free(&err);
    		}
    		return NULL;
    	}
    	/* syslog_ax_vlan_dbg("delete vlan entry %d.\n",vlanId);*/
    	vlanNode = npd_find_vlan_by_vid(vlanId);
    	if(NULL == vlanNode) {
    		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
    		reply = dbus_message_new_method_return(msg);
    		
    		dbus_message_iter_init_append (reply, &iter);
    		
    		dbus_message_iter_append_basic (&iter,
    										 DBUS_TYPE_UINT32, 
    										 &ret);
    		return reply;
    	}
    	if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
    		 syslog_ax_vlan_err("illegal vlanId %d\n",vlanId);
    		ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN;
    	} 
    	else if(DEFAULT_VLAN_ID == vlanId) {
    		ret = VLAN_RETURN_CODE_BADPARAM;
    	}       
    	else if(vlanNode->isAutoCreated){
    		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
    	}
    	else if((advanced_routing_default_vid == vlanId)&&\
    		(NPD_ADV_ROUTING_DEFAULT_VID != vlanId)){
    		ret = VLAN_RETURN_CODE_ADVANCED_VLAN_CANNOT_DEL;
    	}
        else if((NPD_TRUE == npd_vlan_interface_check(vlanId,&ifIndex))&&\
            (ifIndex !=(~0UI))){
            		ret = VLAN_RETURN_CODE_L3_INTF;
        }
        else if((NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag))&&\
             (flag == VLAN_PORT_SUBIF_EXIST)) {
            ret = VLAN_RETURN_CODE_SUBINTF_EXISTS;
        }
        else if(npd_eth_port_check_subif_and_qinq_by_vid(vlanId)){
            ret = VLAN_RETURN_CODE_PORT_SUBINTF_EXISTS;
        }
        else if(npd_prot_vlan_vid_check_for_all_port(vlanId)){
            ret = VLAN_RETURN_CODE_HAVE_PROT_VLAN_CONFIG;
        }
        else {
    	    tmp=npd_vlan_acl_bind_check(vlanId,&Igroupnum,ACL_DIRECTION_INGRESS);       
    		if(ACL_GROUP_VLAN_BINDED==tmp)/*api*/
    		{
    			dret=npd_vlan_unbind_group(vlanId,Igroupnum,ACL_DIRECTION_INGRESS);
    			if(dret!=NPD_DBUS_SUCCESS) {
    				syslog_ax_acl_dbg("delete acl group failure\n");
    				ret = NPD_DBUS_ERROR;
    			}			
    		}
    		tmp=npd_vlan_acl_bind_check(vlanId,&Egroupnum,ACL_DIRECTION_EGRESS);       
    		if(ACL_GROUP_VLAN_BINDED==tmp)/*api*/
    		{
    			dret=npd_vlan_unbind_group(vlanId,Egroupnum,ACL_DIRECTION_EGRESS);
    			if(dret!=NPD_DBUS_SUCCESS) {
    				syslog_ax_acl_dbg("delete acl group failure\n");
    				ret = NPD_DBUS_ERROR;
    			}			
    		}
    		 syslog_ax_vlan_dbg("To destroy vlan %d entity.\n",vlanId);
    		ret = npd_vlan_interface_destroy_node(vlanId);
    		if (VLAN_RETURN_CODE_ERR_NONE == ret) { /*no use if struct*/
    			npd_mstp_del_vlan_on_mst(vlanId);
    		}   		
    	}
    	reply = dbus_message_new_method_return(msg);
    	
    	dbus_message_iter_init_append (reply, &iter);
    	
    	dbus_message_iter_append_basic (&iter,
    									 DBUS_TYPE_UINT32, 
    									 &ret);
    	
    	return reply;
    }
}

DBusMessage * npd_dbus_vlan_delete_vlan_entry_vname(DBusConnection *conn, DBusMessage *msg, void *user_data) {
	
	DBusMessage* reply = NULL;
	DBusMessageIter	 iter;
	unsigned short	vlanId = 0;
	char*			vlanName = NULL;
	struct vlan_s*	vlanNode = NULL;
	unsigned int	ret = VLAN_RETURN_CODE_ERR_NONE, ifIndex = 0;    
    enum VLAN_PORT_SUBIF_FLAG  flag = VLAN_PORT_SUBIF_NOTEXIST;
	DBusError err;
	unsigned int tmp = 0, Igroupnum=0, Egroupnum=0, dret=0;

	
	/* syslog_ax_vlan_dbg("Entering delete vlan one!\n");*/
	dbus_error_init(&err);

	
	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_STRING,&vlanName,
			DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		/* syslog_ax_vlan_dbg("delete vlan name %s.\n",vlanName);*/
		vlanNode = npd_find_vlan_by_name(vlanName);
		if(NULL == vlanNode) {
			ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
			reply = dbus_message_new_method_return(msg);
			
			dbus_message_iter_init_append (reply, &iter);
			
			dbus_message_iter_append_basic (&iter,
											 DBUS_TYPE_UINT32, 
											 &ret);
			return reply;
		}
		else {
			vlanId = vlanNode->vid;
			/*check vlan is L3 interface or NOT*/
			ret = npd_vlan_interface_check(vlanId,&ifIndex);
			if(NPD_TRUE == ret && ifIndex != ~0UI){
				ret = VLAN_RETURN_CODE_L3_INTF;
				reply = dbus_message_new_method_return(msg);
				
				dbus_message_iter_init_append (reply, &iter);
				
				dbus_message_iter_append_basic (&iter,
												 DBUS_TYPE_UINT32, 
												 &ret);
				return reply;
			}
		}
		
		if (!CHASSIS_VLANID_ISLEGAL(vlanId)) {
			 syslog_ax_vlan_err("illegal vlanId %d\n",vlanId);
			ret = ETHPORT_RETURN_CODE_NO_SUCH_VLAN;
		} 
		else if(DEFAULT_VLAN_ID == vlanId) {
			ret = VLAN_RETURN_CODE_BADPARAM;
		}
		else if(NPD_PORT_L3INTF_VLAN_ID == vlanId) {
			ret = VLAN_RETURN_CODE_PORT_L3_INTF;
		}
	 	else if(vlanNode->isAutoCreated){
			ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
		}
		else if((advanced_routing_default_vid == vlanId)&&\
			(NPD_ADV_ROUTING_DEFAULT_VID != vlanId)){
			ret = VLAN_RETURN_CODE_ADVANCED_VLAN_CANNOT_DEL;
		}
	    else if((NPD_SUCCESS == npd_vlan_get_interface_ve_flag(vlanId, &flag))&&\
	         (flag == VLAN_PORT_SUBIF_EXIST)) {
	        ret = VLAN_RETURN_CODE_SUBINTF_EXISTS;
	    }
	    else if(npd_eth_port_check_subif_and_qinq_by_vid(vlanId)){
	        ret = VLAN_RETURN_CODE_PORT_SUBINTF_EXISTS;
	    }
	    else if(npd_prot_vlan_vid_check_for_all_port(vlanId)){
	        ret = VLAN_RETURN_CODE_HAVE_PROT_VLAN_CONFIG;
	    }
		else {
			tmp=npd_vlan_acl_bind_check(vlanId,&Igroupnum,ACL_DIRECTION_INGRESS);       
			if(ACL_GROUP_VLAN_BINDED==tmp)/*api*/
			{
				dret=npd_vlan_unbind_group(vlanId,Igroupnum,ACL_DIRECTION_INGRESS);
				if(dret!=NPD_DBUS_SUCCESS) {
					syslog_ax_acl_dbg("delete acl group failure\n");
					ret = NPD_DBUS_ERROR;
				}			
			}
			tmp=npd_vlan_acl_bind_check(vlanId,&Egroupnum,ACL_DIRECTION_EGRESS);       
			if(ACL_GROUP_VLAN_BINDED==tmp)/*api*/
			{
				dret=npd_vlan_unbind_group(vlanId,Egroupnum,ACL_DIRECTION_EGRESS);
				if(dret!=NPD_DBUS_SUCCESS) {
					syslog_ax_acl_dbg("delete acl group failure\n");
					ret = NPD_DBUS_ERROR;
				}			
			}
			/* syslog_ax_vlan_dbg("To destroy vlan %d entity.\n",vlanId);*/
			ret = npd_vlan_interface_destroy_node(vlanId);
			if (VLAN_RETURN_CODE_ERR_NONE == ret){ /*no use if struct*/
				npd_mstp_del_vlan_on_mst(vlanId);
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

/*npd_dbus_vlan_set_port_vlan_ingresflt*/

DBusMessage * npd_dbus_vlan_set_port_vlan_ingresflt
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0;  
	unsigned char	enDis = 0;	

	unsigned int	eth_g_index = 0;
	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;
	
	/* syslog_ax_vlan_dbg("Entering add|delete ethport to vlan...\n");*/

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else{
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_BYTE,&slot_no,
								DBUS_TYPE_BYTE,&local_port_no,
								DBUS_TYPE_BYTE,&enDis,
								DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}

		/* syslog_ax_vlan_dbg("slot %d port %d",slot_no,local_port_no);*/
		/* syslog_ax_vlan_dbg("set ingress fliter  %s\n",(enDis)?"enable":"Disable");*/
		
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				local_port_no = ETH_LOCAL_NO2INDEX(slot_index, local_port_no);
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,local_port_no);
				/* syslog_ax_vlan_dbg("ethport global index %#0x \n",eth_g_index);*/

				/* here set eth port vlan ingress fliter en/dis*/
				ret = npd_vlan_set_port_vlan_ingresfilter(eth_g_index,enDis);
				if(VLAN_RETURN_CODE_ERR_NONE == ret) {
					ret = VLAN_RETURN_CODE_ERR_NONE;
				}
			}
			else {
				 syslog_ax_vlan_err("illegal slot %d\n",slot_no);
				ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
			}
		}
		else {
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}

/*npd_dbus_vlan_set_one_port_pvid*/
DBusMessage * npd_dbus_vlan_set_one_port_pvid
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0;
	unsigned short	pvid = 0;	

	unsigned int	eth_g_index = 0;
	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;
	DBusError err;
	
	/* syslog_ax_vlan_dbg("Entering add|delete ethport to vlan...\n");*/

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_BYTE,&slot_no,
								DBUS_TYPE_BYTE,&local_port_no,
								DBUS_TYPE_UINT16,&pvid,
								DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}

		/* syslog_ax_vlan_dbg("slot %d port %d",slot_no,local_port_no);*/
		/* syslog_ax_vlan_dbg("set pvid %d\n",pvid);*/
		
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				local_port_no = ETH_LOCAL_NO2INDEX(slot_index, local_port_no);
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,local_port_no);

				/* here set eth port <slot/port> a pvid */

				ret = npd_vlan_set_port_pvid(eth_g_index,pvid);
				if(VLAN_RETURN_CODE_ERR_NONE == ret) {
					ret = VLAN_RETURN_CODE_ERR_NONE;}
			}
			else {
				ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
			}
		}
		else {
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}

/*npd_dbus_vlan_show_one_port_pvid*/

/*original*/
DBusMessage * npd_dbus_vlan_show_one_port_pvid(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	
	DBusMessage* reply;
	DBusMessageIter		iter;
	unsigned int	port_index = 0, ret = VLAN_RETURN_CODE_ERR_NONE;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index = 0;
	unsigned short pvid = 0;
	unsigned int	eth_g_index = 0;
	DBusError err;


	dbus_error_init(&err);
	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support VLAN!\n");
		ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){	
		npd_syslog_dbg("do not support VLAN!\n");
		ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else{
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_BYTE,&slot_no,
			DBUS_TYPE_BYTE,&local_port_no,
			DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		/* syslog_ax_vlan_dbg("Get params : slot %d, port %d",slot_no,local_port_no);*/
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
				slot_index = CHASSIS_SLOT_NO2INDEX(slot_no);
				/* here get eth port <slot/port> pvid */
				port_index = ETH_LOCAL_NO2INDEX(slot_index, local_port_no);
				eth_g_index =  ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,port_index);

				ret = npd_vlan_port_pvid_get(eth_g_index, &pvid);
				if(VLAN_RETURN_CODE_ERR_NONE == ret) {
					ret = VLAN_RETURN_CODE_ERR_NONE;	
				}
			}
			else {
				ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
			}
		}
		else {
			ret = ETHPORT_RETURN_CODE_NO_SUCH_PORT;
		}
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32, 
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT16, 
									 &pvid);
	
	return reply;
}


/*npd_dbus_vlan_show_ports_pvid*/

/*original*/
DBusMessage * npd_dbus_vlan_show_ports_pvid(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	
	DBusMessage* reply;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	unsigned int	i,j,port_index,count = 0,ret = VLAN_RETURN_CODE_ERR_NONE;
	portPvid	pvid[EXTENDED_SLOT_NUM_MAX*ETH_PORT_NUM_MAX];
	unsigned short pvidtmp;
	unsigned int	eth_g_index,slot_index = 0;
	DBusError err;

	/* syslog_ax_vlan_dbg("Entering show port list pvid!\n");*/

	dbus_error_init(&err);
	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support VLAN!\n");
		ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){	
		npd_syslog_dbg("do not support VLAN!\n");
		ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else{
		#if 1
		for (i = 1; i <= CHASSIS_SLOT_COUNT; i++ ) {
			if (CHASSIS_SLOTNO_ISLEGAL(i)) 
			{
				slot_index = CHASSIS_SLOT_NO2INDEX(i)	;
				for (j = 1; j <= ETH_LOCAL_PORT_COUNT(slot_index); j++)
				{
					if (ETH_LOCAL_PORTNO_ISLEGAL(i,j)) {
						port_index = ETH_LOCAL_NO2INDEX(slot_index, j);
						eth_g_index =  ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,port_index);
						/* 5612i main control port should not get pvid */
						if((PRODUCT_ID_AX5K_I == PRODUCT_ID) 	\
							&& (eth_g_index >= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,9)) \
							&& (eth_g_index <= ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(1,12))) {
							continue;
						}
						ret = npd_vlan_port_pvid_get(eth_g_index, &pvidtmp);
						pvid[count].pvid = pvidtmp;
						pvid[count].slotno = i;
						pvid[count].localportNo = j;
						count++;
					} /*continue;*/
				}
			} /*continue;*/
		}
		#endif
		if(VLAN_RETURN_CODE_ERR_NONE != ret) {
			syslog_ax_vlan_dbg("get pvid error ret value is %d\n",ret);
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &count);
	/* syslog_ax_vlan_dbg("valid port Count :%d; open outer container!",count);*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
								      		DBUS_TYPE_UINT32_AS_STRING	/*slotNo*/
								      		DBUS_TYPE_UINT32_AS_STRING  /*localportNo*/
											DBUS_TYPE_UINT16_AS_STRING /*pvid*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);

	#if 0
	count = 0;
	for(i = 1; i <= EXTENDED_SLOT_NUM_MAX;i++){
		for (j = 1; j <= ETH_PORT_NUM_MAX; j++ ) {
	#endif
	for(i = 0; i < count;i++){
			DBusMessageIter iter_struct;
			dbus_message_iter_open_container (&iter_array,
											   DBUS_TYPE_STRUCT,
											   NULL,
											   &iter_struct);
			/* syslog_ax_vlan_dbg("open inner container,%d times!",i);*/
			dbus_message_iter_append_basic(&iter_struct,
											  DBUS_TYPE_UINT32,
											  &(pvid[i].slotno));

			dbus_message_iter_append_basic(&iter_struct,
											  DBUS_TYPE_UINT32,
											  &(pvid[i].localportNo));

			dbus_message_iter_append_basic(&iter_struct,
											  DBUS_TYPE_UINT16,
											  &(pvid[i].pvid));
			
			/* syslog_ax_vlan_dbg("slot_no %d,port_no%d,count = %d,pvid = %d",i,j,count-1,pvid[count-1].pvid);*/
			dbus_message_iter_close_container (&iter_array, &iter_struct);
		}

	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}

DBusMessage * npd_dbus_vlan_show_egress_filter(DBusConnection *conn, DBusMessage *msg, void *user_data) 
{
	DBusMessage* 	reply = NULL;
	DBusMessageIter	iter = {0};
	DBusError 		err;
	unsigned int	Isable = 0;
	unsigned int 	ret = 0;
	syslog_ax_acl_dbg("show vlan egress filter\n");

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support VLAN!\n");
		ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){	
		npd_syslog_dbg("do not support VLAN!\n");
		ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else{
		dbus_error_init(&err);
		if(NPD_TRUE == g_vlan_egress_filter) {
		   	syslog_ax_acl_dbg("vlan egress filter is enabled");		   
		}
		else {
			syslog_ax_acl_dbg("vlan egress filter is disabled");	
		}
		Isable = g_vlan_egress_filter;
	}
	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append (reply, &iter);	
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&Isable);
	dbus_message_iter_append_basic (&iter,DBUS_TYPE_UINT32,&ret);
	
	return reply;	
}
		
/**********************************************************************************
 *  npd_igmp_snp_vlan_member_port_index_get
 *
 *	DESCRIPTION:
 * 		check out if a port is an untagged or tagged port member of specified vlan
 *
 *	INPUT:
 *		vlanId - vlan id
 *		eth_g_index - global ethernet port index
 *		isTagged - untagged or tagged port member
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NPD_TRUE  - if port is IGMP enable
 *		NPD_FALSE - if port is not IGMP enable
 *		
 *
 **********************************************************************************/
unsigned int npd_igmpsnp_vlan_member_port_index_get
(
	unsigned short	vlanId,
	unsigned char	isTagged,
	unsigned int	eth_g_index[],
	unsigned int	*mbr_count
)
{
	struct vlan_s *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	struct vlan_port_list_node_s *node = NULL;
	struct list_head  *list = NULL,*pos = NULL;
	unsigned int mbrCnt = 0;
	vlanNode = npd_find_vlan_by_vid(vlanId);
	if(NULL == vlanNode) {
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}
	/* tagged port list*/
	if(NPD_TRUE == isTagged) {
		portList = vlanNode->tagPortList;
	}
	else if(NPD_FALSE == isTagged) {
		portList = vlanNode->untagPortList;
	}

	if(NULL == portList) {
		return IGMPSNP_RETURN_CODE_NULL_PTR;
	}
	else if(0 != portList->count) {
		/* syslog_ax_vlan_dbg("npd vlan check port:: vlan %d %s port count %d",vlanId,isTagged ? "tagged":"untagged",portList->count);*/
		list = &(portList->list);
		__list_for_each(pos,list) {
			node = list_entry(pos,struct vlan_port_list_node_s,list);
			if(NULL != node && NPD_TRUE==node->igmpEnFlag){
				eth_g_index[mbrCnt] = node->eth_global_index;
				mbrCnt++;
			}
		}
	}
	*mbr_count = mbrCnt;
	return IGMPSNP_RETURN_CODE_OK;
}



/******************************************************
 *  show enable igmp snoop vlan port member .
 *
 *****************************************************/
unsigned int npd_vlan_get_igmpsnp_mbr_bmp_via_sw
(
	unsigned short vlanId,
	unsigned int *untagMbrBmp_sp,/**/
	unsigned int *tagMbrBmp_sp
)
{
	unsigned int i,ret = IGMPSNP_RETURN_CODE_OK;
	unsigned int unmbrCont = 0,tagmbrCont = 0;
	unsigned int unmbr_index[64] = {0},tagmbr_index[64] = {0};
	unsigned int MbrBmpTmpSP = 0;
	unsigned int slotNo = 0,localPortNo = 0,slot_index = 0;
	struct eth_port_s	*ethPort = NULL;
	/*struct DRV_VLAN_PORT_BMP_S *unMbrBmpDrv = NULL,*tagMbrBmpDrv = NULL;*/
	/*struct vlan_ports_bmp  untag_vlan_port_bmp,tag_vlan_port_bmp ;*/
	ret = npd_igmpsnp_vlan_member_port_index_get(vlanId, NPD_FALSE, unmbr_index, &unmbrCont);
	if(IGMPSNP_RETURN_CODE_OK == ret){
		for(i = 0;i<unmbrCont;i++){
			/*npd_get_devport_by_global_index(unmbr_index[i], &devNum, &portNum);*//*no use:0604*/
			ethPort = npd_get_port_by_index(unmbr_index[i]);
			if(NULL == ethPort){
				return IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;
			}
			if(NULL == ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(unmbr_index[i]);
				slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
				localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(unmbr_index[i]);
				localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
				MbrBmpTmpSP |= 1<<((slotNo-1)*8+localPortNo);
			 syslog_ax_vlan_dbg("vlan %d untag port index[%d] = %#x bitmap %#x",vlanId,i,unmbr_index[i],MbrBmpTmpSP);
			}
		}
		*untagMbrBmp_sp =MbrBmpTmpSP;
	}
	MbrBmpTmpSP = 0;
	ret = npd_igmpsnp_vlan_member_port_index_get(vlanId, NPD_TRUE, tagmbr_index, &tagmbrCont);
	if(IGMPSNP_RETURN_CODE_OK == ret)
	{
		for(i = 0;i<tagmbrCont;i++){
			/*npd_get_devport_by_global_index(unmbr_index[i], &devNum, &portNum);*//*no use:0604*/
			ethPort = npd_get_port_by_index(tagmbr_index[i]);
			if(NULL == ethPort){
				return IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;
			}
			if(NULL == ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(tagmbr_index[i]);
				slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
				localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(tagmbr_index[i]);
				localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
				MbrBmpTmpSP |= 1<<((slotNo-1)*8+localPortNo);
			 syslog_ax_vlan_dbg("vlan %d tagmbr port index[%d] = %#x bitmap %#x",vlanId,i,tagmbr_index[i],MbrBmpTmpSP);
			}
		}
		*tagMbrBmp_sp =MbrBmpTmpSP;
	}
	return IGMPSNP_RETURN_CODE_OK;
}


/******************************************************
 *  show enable igmp snoop vlan port member .
 *
 *****************************************************/
unsigned int npd_vlan_get_igmpsnp_mbr_bmp_via_sw_v1
(
	unsigned short vlanId,
	PORT_MEMBER_BMP * untagMbrBmp_sp,/**/
	PORT_MEMBER_BMP * tagMbrBmp_sp
)
{
	unsigned int i,ret = IGMPSNP_RETURN_CODE_OK;
	unsigned int unmbrCont = 0,tagmbrCont = 0;
	unsigned int untagmbr_index[MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT],
				 tagmbr_index[MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT];
	unsigned int slotNo = 0,localPortNo = 0,slot_index = 0;
	struct eth_port_s	*ethPort = NULL;

	memset(untagmbr_index, 0, sizeof(unsigned int)* MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT);
	memset(tagmbr_index, 0, sizeof(unsigned int)* MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT);
	/*struct DRV_VLAN_PORT_BMP_S *unMbrBmpDrv = NULL,*tagMbrBmpDrv = NULL;*/
	/*struct vlan_ports_bmp  untag_vlan_port_bmp,tag_vlan_port_bmp ;*/
	ret = npd_igmpsnp_vlan_member_port_index_get(vlanId, NPD_FALSE, untagmbr_index, &unmbrCont);
	if(IGMPSNP_RETURN_CODE_OK == ret){
		for(i = 0;i<unmbrCont;i++){
			/*npd_get_devport_by_global_index(unmbr_index[i], &devNum, &portNum);*//*no use:0604*/
			ethPort = npd_get_port_by_index(untagmbr_index[i]);
			if(NULL == ethPort){
				return IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;
			}
			if(NULL == ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(untagmbr_index[i]);
				slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
				localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(untagmbr_index[i]);
                /* we do not care about slot-no, houxx@autelan.com 2012-11-27 */
				#if 1				
				if (localPortNo > 31) {
				   untagMbrBmp_sp->portMbr[1] |= 1 << (localPortNo - 32);
				}else {
				   untagMbrBmp_sp->portMbr[0] |= 1 << (localPortNo);
				}
				
				#else
				localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);

				if (localPortNo > 31) {
				   untagMbrBmp_sp->portMbr[1] |= 1 << ((slotNo - 1) * 8 + (localPortNo - 32));
				}else {
				   untagMbrBmp_sp->portMbr[0] |= 1 << ((slotNo - 1) * 8 + localPortNo);
				}
				#endif
				syslog_ax_vlan_dbg("vlan %d untag port index[%d] = %#x bitmap[0] %#x bitmap[1] %#x\n",
				 					vlanId,i,untagmbr_index[i],
				 					untagMbrBmp_sp->portMbr[0],
				 					untagMbrBmp_sp->portMbr[1]);
			}
		}
	}
	ret = npd_igmpsnp_vlan_member_port_index_get(vlanId, NPD_TRUE, tagmbr_index, &tagmbrCont);
	if(IGMPSNP_RETURN_CODE_OK == ret)
	{
		for(i = 0;i<tagmbrCont;i++){
			/*npd_get_devport_by_global_index(unmbr_index[i], &devNum, &portNum);*//*no use:0604*/
			ethPort = npd_get_port_by_index(tagmbr_index[i]);
			if(NULL == ethPort){
				return IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;
			}
			if(NULL == ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
				slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(tagmbr_index[i]);
				slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
				localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(tagmbr_index[i]);
				#if 1 				
				if (localPortNo > 31) {
				   tagMbrBmp_sp->portMbr[1] |= 1 << (localPortNo - 32);
				}else {
				   tagMbrBmp_sp->portMbr[0] |= 1 << (localPortNo);
				}
				
				#else				
				localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
				if (localPortNo > 31) {
				   tagMbrBmp_sp->portMbr[1] |= 1 << ((slotNo - 1) * 8 + (localPortNo - 32));
				}else {
				   tagMbrBmp_sp->portMbr[0] |= 1 << ((slotNo - 1) * 8 + localPortNo);
				}
                #endif
			 	syslog_ax_vlan_dbg("vlan %d tagmbr port index[%d] = %#x bitmap[0] %#x bitmap[1] %#x\n",
			 					vlanId,i,tagmbr_index[i],
			 					tagMbrBmp_sp->portMbr[0],
			 					tagMbrBmp_sp->portMbr[1]);
			}
		}
	return IGMPSNP_RETURN_CODE_OK;
	}
	return ret;
}


#if 1			
/********************************************
 *
 *			IGMP Snooping
 *			=============
 *
 *********************************************/

/*npd_dbus_vlan_check_igmp_snoop*/
DBusMessage * npd_dbus_vlan_check_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int ret = IGMPSNP_RETURN_CODE_OK;
	unsigned char status = 0, devchk = 0;
	unsigned char ismld = 0;
	DBusError err;
	
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_BYTE,&ismld,
								DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if(PRODUCT_ID_AX5K_E == PRODUCT_ID){
		devchk = 1;
	}

	syslog_ax_vlan_dbg("before check vlan igmp or mld snooping status %d.\n",status);

	if(1 == ismld){
		ret = npd_check_mld_snp_status(&status);
	}
	else{
		ret = npd_check_igmp_snp_status(&status);
	}
	
	syslog_ax_vlan_dbg("check vlan igmp snooping status %d ret %d\n",status,ret);
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &status);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &devchk);
	return reply;
} 


/*npd_dbus_vlan_check_vlan_igmp_snoop*/
DBusMessage * npd_dbus_vlan_check_vlan_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int ret = IGMPSNP_RETURN_CODE_OK;
	unsigned short vlanId = 0;
	unsigned char status = 0,statusm = 0;	
	DBusError err;
	
	 syslog_ax_vlan_dbg("Enter: check igmp vlan status...\n");

	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
							 DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	npd_check_igmp_snp_status(&status);
	npd_check_mld_snp_status(&statusm);
	if((0 == status) && (0 == statusm)){
		ret = IGMPSNP_RETURN_CODE_ENABLE_GBL;
	}
	else {
		ret = npd_check_igmp_snp_vlan_status(vlanId,&status);
		syslog_ax_vlan_dbg("vlan endis IGMP :vlan %d,status = %d",vlanId,status);
	} 

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &status);
	return reply;
}

DBusMessage * npd_dbus_vlan_igmp_snoop_count
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int i,vCount = 0,ret = IGMPSNP_RETURN_CODE_OK;
	unsigned char status = 0,statusm = 0;
	DBusError err;
	
	 syslog_ax_vlan_dbg("Entering check igmp status...\n");

	
	if(!((productinfo.capbmp) & FUNCTION_IGMP_VLAUE)){
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else if(((productinfo.capbmp) & FUNCTION_IGMP_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_IGMP_VLAUE)){	
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else{
		dbus_error_init(&err);
		npd_check_igmp_snp_status(&status);
		npd_check_mld_snp_status(&statusm);
		if((0 == status) && (0 == statusm)){
			ret = IGMPSNP_RETURN_CODE_ENABLE_GBL;
		}
		else {
			for(i =0;i<NPD_VLAN_NUMBER_MAX;i++){
				if(VLAN_RETURN_CODE_VLAN_EXISTS == npd_check_vlan_exist(i)){
					ret = npd_check_igmp_snp_vlan_status(i, &status);
					if(IGMPSNP_RETURN_CODE_OK == ret && 1 == status) {
						vCount++;
						ret = IGMPSNP_RETURN_CODE_OK;
					}
				}
			}
			syslog_ax_vlan_dbg("endis IGMP :status = %d",status);
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &vCount);
	return reply;
}


DBusMessage * npd_dbus_vlan_portmbr_check_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int slotno = 0,portno = 0,eth_g_index = 0,ret = NPD_DBUS_SUCCESS;
	unsigned char status = 0;
	unsigned short vlanId = 0;
	DBusError err;
	
	 syslog_ax_vlan_dbg("Entering check igmp status...\n");

	dbus_error_init(&err);
	if (!(dbus_message_get_args ( msg, &err,
							 DBUS_TYPE_UINT16,&vlanId,
							 DBUS_TYPE_UINT32,&slotno,
							 DBUS_TYPE_UINT32,&portno,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slotno,portno);
	/*ret = npd_check_igmp_snp_port_status(eth_g_index, &status);*/
    ret = npd_check_igmp_snp_vlanMbr_status(vlanId,eth_g_index,&status);
	syslog_ax_vlan_dbg("vlan_portmbr_check_igmp_snoop::endis IGMP :status = %d",status);
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &status);
	return reply;
}

/*npd_dbus_vlan_config_igmp_snoop*/
DBusMessage * npd_dbus_vlan_config_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned short	vid;
	unsigned int isEnable = 0,ret = IGMPSNP_RETURN_CODE_OK;
	unsigned char	status = 0,statusm = 0;
	DBusError err;
	
	dbus_error_init(&err);

	
	if(!((productinfo.capbmp) & FUNCTION_IGMP_VLAUE)){
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else if(((productinfo.capbmp) & FUNCTION_IGMP_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_IGMP_VLAUE)){	
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else{
		if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT16,&vid,
								 DBUS_TYPE_UINT32,&isEnable,
								DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		ret = npd_check_igmp_snp_status(&status);
		ret = npd_check_mld_snp_status(&statusm);
		if((0 == status) && (0 == statusm) ){
			syslog_ax_vlan_err("IGMP or MLD Snooping NOT Enabled Global.");
			ret = IGMPSNP_RETURN_CODE_NOT_ENABLE_GBL;
		}
		else{
			ret = npd_check_igmp_snp_vlan_status(vid,&status);
			if(IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST == ret){
				 syslog_ax_vlan_err("vlan %d not created.",vid);
			}
			else if(0 == isEnable &&0 == status ){
				ret = IGMPSNP_RETURN_CODE_NOTENABLE_VLAN;
				 syslog_ax_vlan_err("vlan %d not enabled IGMP or MLD Snooping.",vid);
			}
			else if(1== isEnable && 1== status){
				ret = IGMPSNP_RETURN_CODE_HASENABLE_VLAN;
				 syslog_ax_vlan_err("vlan %d already enabled IGMP or MLD Snooping.",vid);
			}
			else if(IGMPSNP_RETURN_CODE_OK == ret){
				 syslog_ax_vlan_dbg("get vid = %d ,isEnable = %d",vid,isEnable);
				ret = npd_vlan_igmp_snp_endis_config(isEnable,vid);
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


/***********************************
 *
 * npd_dbus_vlan_igmp_snoop_info_get
 *
 *
 *
**************************************/

DBusMessage * npd_dbus_vlan_igmp_snoop_info_get
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	DBusError err;
	static vlanList	*actvlanList[CHASSIS_VLAN_RANGE_MAX];       /* change to static , zhangdi 2011-10-13 */
	unsigned int	vlan_count = 0;
	unsigned int	i,ret = IGMPSNP_RETURN_CODE_OK;
	unsigned short	vlanId=0;
	struct vlan_s*	vlanNode = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	unsigned char enableFlag = 0;
	unsigned int untagMbrBmp = 0;
	unsigned int tagMbrBmp = 0;
	

	dbus_error_init(&err);
	
	npd_check_igmp_snp_status(&enableFlag);
	if(IGMPSNP_RETURN_CODE_OK != enableFlag){
		ret = IGMPSNP_RETURN_CODE_NOT_ENABLE_GBL;
	}
	else {
		for (vlanId=1;vlanId<=CHASSIS_VLAN_RANGE_MAX;vlanId++){
			ret = npd_check_vlan_exist(vlanId);
			if(VLAN_RETURN_CODE_VLAN_EXISTS !=ret) {
				ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
				continue;
			}
			else {
				vlanNode = npd_find_vlan_by_vid(vlanId);
				if(NULL != vlanNode && NPD_TRUE == vlanNode->igmpSnpEnDis){
					/*vlan_count++;*/
					actvlanList[vlan_count] = (vlanList*)malloc(sizeof(vlanList));
					memset(actvlanList[vlan_count],0,sizeof(vlanList));
					actvlanList[vlan_count]->vlanId = vlanId;
					if(0 != vlanNode->tagPortList ||
						0 != vlanNode->untagPortList ||
						0 != vlanNode->tagTrunkList||
						0 != vlanNode->unTagtrunkList)	{
						ret = npd_vlan_get_igmpsnp_mbr_bmp_via_sw(vlanId,\
												&untagMbrBmp,\
												&tagMbrBmp);
						if(IGMPSNP_RETURN_CODE_OK == ret) {
						 syslog_ax_vlan_dbg("\nactVlanList[%d].untagMbrBmp %x tagMbrBmp %x",vlan_count,\
																				untagMbrBmp,\
																				tagMbrBmp);
						 syslog_ax_vlan_dbg("loop iter vlanId=%d,vlan_count =%d",vlanId,vlan_count);
						ret = IGMPSNP_RETURN_CODE_OK;
						}
						else if (IGMPSNP_RETURN_CODE_OK != ret) {
							 syslog_ax_vlan_dbg("loop iter vlanId=%d",vlanId);
							ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
						}
					}
					vlan_count++;
				}
				else {
					ret = IGMPSNP_RETURN_CODE_NOTENABLE_VLAN;
				}
			}
		}
		if (vlan_count > 0) {
			ret = IGMPSNP_RETURN_CODE_OK;
		}
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	/* Total active vlan count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &vlan_count);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);

	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*lanId*/
										   	DBUS_TYPE_UINT32_AS_STRING /*untag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*tag*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < vlan_count; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		/*
		 syslog_ax_vlan_dbg("actvlanList[%d] vlanId=%d,untagBmp=%#0x ,tagBmp %#0x",i,	\
															actvlanList[i]->vlanId,\
															actvlanList[i]->untagMbrBmp,	\
															actvlanList[i]->tagMbrBmp));
		*/
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT16,
				  &(actvlanList[i]->vlanId));
		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(untagMbrBmp));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(tagMbrBmp));

		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(actvlanList[i]);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}


/***********************************
 *
 * npd_dbus_vlan_igmp_snoop_info_get
 *
 *
 *
**************************************/

DBusMessage * npd_dbus_vlan_igmp_snoop_info_get_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{	
	DBusMessage* reply;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	DBusError err;
	static vlanList	*actvlanList[CHASSIS_VLAN_RANGE_MAX];    /* change to static for npd error, zhangdi 2011-10-13 */
	unsigned int	vlan_count = 0;
	unsigned int	i,ret = IGMPSNP_RETURN_CODE_OK;
	unsigned short	vlanId=0;
	struct vlan_s*	vlanNode = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	unsigned char enableFlag = 0,enableFlagm = 0;
	

	dbus_error_init(&err);

	
	if(!((productinfo.capbmp) & FUNCTION_IGMP_VLAUE)){
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else if(((productinfo.capbmp) & FUNCTION_IGMP_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_IGMP_VLAUE)){	
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else{
		npd_check_igmp_snp_status(&enableFlag);
		npd_check_mld_snp_status(&enableFlagm);
		if((0 == enableFlag) && (0 == enableFlagm)){
			ret = IGMPSNP_RETURN_CODE_NOT_ENABLE_GBL;
		}
		else {
			for (vlanId=1;vlanId<=CHASSIS_VLAN_RANGE_MAX;vlanId++){
				ret = npd_check_vlan_exist(vlanId);
				if(VLAN_RETURN_CODE_VLAN_EXISTS !=ret) {
					ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
					continue;
				}
				else {
					vlanNode = npd_find_vlan_by_vid(vlanId);
					if(NULL != vlanNode && NPD_TRUE == vlanNode->igmpSnpEnDis){
						/*vlan_count++;*/
						actvlanList[vlan_count] = (vlanList*)malloc(sizeof(vlanList));
						memset(actvlanList[vlan_count],0,sizeof(vlanList));
						actvlanList[vlan_count]->vlanId = vlanId;
						if(0 != vlanNode->tagPortList ||
							0 != vlanNode->untagPortList ||
							0 != vlanNode->tagTrunkList||
							0 != vlanNode->unTagtrunkList)	{
							ret = npd_vlan_get_igmpsnp_mbr_bmp_via_sw_v1(vlanId,\
													&(actvlanList[vlan_count]->untagMbrBmp),\
													&(actvlanList[vlan_count]->tagMbrBmp));
							if(IGMPSNP_RETURN_CODE_OK == ret) {
								syslog_ax_vlan_dbg("\nactVlanList[%d].untagMbrBmp[0] %x untagMbrBmp[1] %x tagMbrBmp[0] %x tagMbrBmp[1] %x\n",
													vlan_count,\
													actvlanList[vlan_count]->untagMbrBmp.portMbr[0],
													actvlanList[vlan_count]->untagMbrBmp.portMbr[1],
													actvlanList[vlan_count]->tagMbrBmp.portMbr[0],
													actvlanList[vlan_count]->tagMbrBmp.portMbr[1]);
							 syslog_ax_vlan_dbg("loop iter vlanId=%d,vlan_count =%d\n",vlanId,vlan_count);
							ret = IGMPSNP_RETURN_CODE_OK;
							}
							else if (IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST == ret) {
								 syslog_ax_vlan_dbg("loop iter vlanId=%d",vlanId);
								ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
							}
							else  {
								ret = IGMPSNP_RETURN_CODE_ERROR_HW;
							}
						}
						vlan_count++;
					}
					else {
						ret = IGMPSNP_RETURN_CODE_NOTENABLE_VLAN;
					}
				}
			}
			if (vlan_count > 0) {
				ret = IGMPSNP_RETURN_CODE_OK;
			}
		}
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	/* Total active vlan count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &vlan_count);

	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);

	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING  /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*vlanId*/
										   	DBUS_TYPE_UINT32_AS_STRING /*untag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*untag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*tag*/
										   	DBUS_TYPE_UINT32_AS_STRING /*tag	*/									   	
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < vlan_count; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		/*
		 syslog_ax_vlan_dbg("actvlanList[%d] vlanId=%d,untagBmp=%#0x ,tagBmp %#0x",i,	\
															actvlanList[i]->vlanId,\
															actvlanList[i]->untagMbrBmp,	\
															actvlanList[i]->tagMbrBmp));
		*/
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT16,
				  &(actvlanList[i]->vlanId));
		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->untagMbrBmp.portMbr[0]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->untagMbrBmp.portMbr[1]));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->tagMbrBmp.portMbr[0]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actvlanList[i]->tagMbrBmp.portMbr[1]));

		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(actvlanList[i]);
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}
/*npd_dbus_vlan_portmbr_config_igmp_snoop*/
DBusMessage * npd_dbus_vlan_portmbr_config_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	slot_no,local_port_no,status = 0,isEnable =0,tagMode = 0, statusm =0;
	unsigned short	vid =0;	
	unsigned int	eth_g_index=0;
	unsigned int 	ret = IGMPSNP_RETURN_CODE_OK;
	struct eth_port_s* ethPort = NULL;
	DBusError err;
	
	syslog_ax_vlan_dbg(("Entering config vlan port member enable/disable igmp or mld ...\n"));

	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_IGMP_VLAUE)){
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else if(((productinfo.capbmp) & FUNCTION_IGMP_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_IGMP_VLAUE)){	
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else{
		if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT16,&vid,
								 DBUS_TYPE_BYTE,&slot_no,
								 DBUS_TYPE_BYTE,&local_port_no,
								 DBUS_TYPE_BYTE,&isEnable,
								DBUS_TYPE_INVALID))) {
			syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			syslog_ax_vlan_err("return error caused dbus.");
			return NULL;
		}
		syslog_ax_vlan_dbg("get params: vid %d, slot_no %d,port_no %d, enDis %d.",\
												vid,slot_no,local_port_no,isEnable);

		ret = npd_check_igmp_snp_status(&status);
		ret = npd_check_mld_snp_status(&statusm);
		if((0 == status) && (0 == statusm) ){
			syslog_ax_vlan_err("IGMP or MLD Snooping NOT Enabled Global.");
			ret = IGMPSNP_RETURN_CODE_NOT_ENABLE_GBL;
		}
		else{
			syslog_ax_vlan_dbg("vlan ID %d,slot %d,port %d,isEnable %d",vid,slot_no,local_port_no,isEnable);
			if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
				if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {
					eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
					syslog_ax_vlan_dbg("slot %d,port %d convert eth_g_index %d",slot_no,local_port_no,eth_g_index);
					ethPort = npd_get_port_by_index(eth_g_index);
					if(NULL == ethPort){
						ret = IGMPSNP_RETURN_CODE_NO_SUCH_PORT;
					}
					else if(NULL != ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
						/*port is member of trunk*/
						ret = IGMPSNP_RETURN_CODE_PORT_TRUNK_MBR;
					}
					else if(NPD_TRUE != npd_vlan_check_contain_port(vid,eth_g_index, &tagMode)){
						/*check port membership*/
						ret = IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;	
					}
					else {
						syslog_ax_vlan_dbg("check port %d IGMP or MLD Snp status.",eth_g_index);
						/*ret = npd_check_igmp_snp_port_status(eth_g_index,&status);*/
						ret = npd_check_igmp_snp_vlanMbr_status(vid,eth_g_index,&status);
						if(IGMPSNP_RETURN_CODE_PORT_NOT_EXIST == ret){
							 syslog_ax_vlan_err("port %d not exists.",eth_g_index);
						}
						else if(0 == isEnable &&0 == status ){
							ret = IGMPSNP_RETURN_CODE_NOTENABLE_PORT;
							 syslog_ax_vlan_err(" port %d not enabled IGMP or MLD Snooping.",eth_g_index);
						}
						else if(1 == isEnable && 1 == status){
							ret = IGMPSNP_RETURN_CODE_HASENABLE_PORT;
							 syslog_ax_vlan_err("port %d already enabled IGMP or MLD Snooping.",eth_g_index);
						}
						else{
							/* here add or delete eth port <slot/port> to(from) vlan */
							syslog_ax_vlan_dbg("%s IGMP or MLD Snp on vlan %d, port %d.",(isEnable)?"En":"Dis",\
													vid,eth_g_index);
							ret = npd_vlan_port_igmp_snp_endis_config(isEnable,vid,eth_g_index,tagMode);
						}
					}
				}
				else {
					ret = IGMPSNP_RETURN_CODE_NO_SUCH_PORT;
				}
			}
			else {
					ret = IGMPSNP_RETURN_CODE_NO_SUCH_PORT;
			}
		}
		syslog_ax_vlan_dbg("port endis IGMP :::ret = %d",ret);
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}

/*npd_dbus_vlan_portmbr_config_igmp_snoop*/
DBusMessage * npd_dbus_vlan_mcrouterport_config_igmp_snoop
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	slot_no,local_port_no,isEnable;
	unsigned short	vid;	
	unsigned int	eth_g_index, ret = NPD_DBUS_SUCCESS;
	struct eth_port_s* ethPort = NULL;
	DBusError err;
	
	 syslog_ax_vlan_dbg("Entering add|delete mcrouter port IGMP or  MLD Snooping...\n");

	dbus_error_init(&err);

	if(!((productinfo.capbmp) & FUNCTION_IGMP_VLAUE)){
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else if(((productinfo.capbmp) & FUNCTION_IGMP_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_IGMP_VLAUE)){	
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else{
		if (!(dbus_message_get_args ( msg, &err,
								 DBUS_TYPE_UINT16,&vid,
								 DBUS_TYPE_BYTE,&slot_no,
								 DBUS_TYPE_BYTE,&local_port_no,
								 DBUS_TYPE_BYTE,&isEnable,
								DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}

		syslog_ax_vlan_dbg("vlan ID %d,slot %d,port %d,isEnable %d",vid,slot_no,local_port_no,isEnable);
		if (CHASSIS_SLOTNO_ISLEGAL(slot_no)) {
			if (ETH_LOCAL_PORTNO_ISLEGAL(slot_no,local_port_no)) {

				/*ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX*/
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_NO(slot_no,local_port_no);
				ethPort = npd_get_port_by_index(eth_g_index);
				if(NULL == ethPort){
					ret = IGMPSNP_RETURN_CODE_PORT_NOT_EXIST;
				}
				else if(NULL != ethPort->funcs.func_data[ETH_PORT_FUNC_LINK_AGGREGATION]){
					/*port is member of trunk*/
					ret = IGMPSNP_RETURN_CODE_PORT_TRUNK_MBR;
				}
				else {
					/* here add or delete eth port <slot/port> to(from) vlan */
					ret = npd_mcrouter_port_igmp_snp_endis_config(isEnable,vid,eth_g_index);
				}
			}
			else {ret = IGMPSNP_RETURN_CODE_NO_SUCH_PORT;}
		}
		else {ret = IGMPSNP_RETURN_CODE_NO_SUCH_PORT;}

		syslog_ax_vlan_dbg("port endis IGMP :::ret = %d",ret);
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}

/*original*/
DBusMessage * npd_dbus_vlan_show_one_group_port_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply;
	DBusMessageIter		iter;
	unsigned int	i,ret = NPD_DBUS_SUCCESS, npdret =0;
	unsigned char	devNum = 0;
	unsigned short	vlanId = 1;
	unsigned short	vidx =0;
	unsigned int	groupAddr = 0;
	PORT_MEMBER_BMP groupMbrBmp;
	unsigned short groupAddrv6[SIZE_OF_IPV6_ADDR];
	unsigned short *pnpdaddr = NULL;
	DBusError err;

    memset(&groupMbrBmp,0,sizeof(PORT_MEMBER_BMP));
	 syslog_ax_vlan_dbg("Enter :show one group port member.");
	/*Read from SW Record or vlan entry table in HW*/
	/*here choose the later method*/
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT16,&vlanId,
									DBUS_TYPE_UINT16,&vidx,
									DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	ret = npd_igmp_snp_l2mc_vlan_exist(vlanId);
	if(0 == ret)
	{
		ret = npd_igmp_snp_l2mc_group_exist(vlanId,vidx);
		if(0 == ret)
		{
			pnpdaddr = groupAddrv6;
			npd_igmp_get_groupip_by_vlan_vidx(vlanId,vidx,&groupAddr,&pnpdaddr, &npdret);
			if(0 != nam_asic_group_mbr_bmp(devNum,vidx,&groupMbrBmp))
				ret = NPD_VLAN_ERR_HW;
			/*break;*/ /*break out off this Fun,the process goes Kill.*/
		}
		else {
			ret = NPD_GROUP_NOTEXIST;
		}
	}
	else {
		ret = NPD_GROUP_NOTEXIST;
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);/*SUCCESS,NPD_VLAN_ERR_HW,NPD_GROUP_NOTEXIST*/
	/*
	dbus_message_iter_append_basic(&iter,
			  						DBUS_TYPE_UINT16,
			 						&vlanId);
	*/
	dbus_message_iter_append_basic(&iter,
			  						DBUS_TYPE_UINT32,
			 						&groupAddr);
	
	dbus_message_iter_append_basic(&iter,
			  						DBUS_TYPE_UINT32,
			 						&groupMbrBmp.portMbr[0]);
	dbus_message_iter_append_basic(&iter,
			  						DBUS_TYPE_UINT32,
			 						&groupMbrBmp.portMbr[1]);
									
	/* code optimize:Uninitialized scalar variable  houxx@autelan.com	2013-1-17 */
	 syslog_ax_vlan_dbg(" vlanId=%d,vidx=%d,groupIp 0x%x, groupMbrBmp[0] %#0x",vlanId,vidx,	groupAddr,groupMbrBmp.portMbr[0]);

	return reply;
}


/*show all group member in one vlan. one vlan can has one or more groups.*/
DBusMessage * npd_dbus_vlan_show_vlan_group_list_port_mbr
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	DBusError err;
	static groupList *actgroupList[CHASSIS_VLAN_RANGE_MAX];      /* change to static , zhangdi 2011-10-13 */
	unsigned char	devNum = 0;
	unsigned int	i,group_count = 0,ret = NPD_DBUS_SUCCESS, npdret = 0;
	unsigned short	vlanId =0,vidx =0;
	unsigned int	groupAddr = 0;
	enum product_id_e productId = PRODUCT_ID; 
	unsigned char enableFlag = 0, vlanEnFlag = 0;
	unsigned short groupAddrv6[SIZE_OF_IPV6_ADDR];
	unsigned short *pnpdaddr = NULL;

	/*Read from SW Record or vlan entry table in HW*/
	/*here choose the later method*/
	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
		DBUS_TYPE_UINT16,&vlanId,
		DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	npd_check_igmp_snp_status(&enableFlag);
	if(0 == enableFlag){
		ret = NPD_IGMP_SNP_NOTENABLE;
	}
	else {
		ret = npd_check_igmp_snp_vlan_status(vlanId,&vlanEnFlag);
		if(NPD_IGMP_SNP_SUCCESS == ret) {
			if(NPD_FALSE == vlanEnFlag) {
				ret = NPD_IGMP_SNP_NOTENABLE_VLAN;
			}
			syslog_ax_vlan_dbg("igmp snooping vlan %d %s",vlanId,vlanEnFlag ? "enabled":"disabled");
		}
	} 

	if(NPD_IGMP_SNP_SUCCESS == ret) {
		ret = npd_igmp_snp_l2mc_vlan_exist(vlanId);
		if(0 != ret ) {
			ret = NPD_GROUP_NOTEXIST;
		}
		else{
			for(vidx=0;vidx<=CHASSIS_VIDX_RANGE_MAX;vidx++){
				ret = npd_igmp_snp_l2mc_group_exist(vlanId,vidx);
				if(0 == ret ){
					actgroupList[group_count] = (groupList*)malloc(sizeof(groupList));
					memset(actgroupList[group_count],0,sizeof(groupList));
					actgroupList[group_count]->vidx = vidx;
					pnpdaddr = groupAddrv6;
					ret = npd_igmp_get_groupip_by_vlan_vidx(vlanId,vidx,&groupAddr,&pnpdaddr, &npdret);
					if(NPD_TRUE == ret && 0 !=groupAddr){
						actgroupList[group_count]->groupIp = groupAddr;
					}
					else {
						free(actgroupList[group_count]);
						continue;
					}
					ret = nam_asic_group_mbr_bmp(devNum,vidx,&(actgroupList[group_count]->groupMbrBmp));
					if(BRG_MC_SUCCESS == ret) {
						ret = NPD_DBUS_SUCCESS;
						 syslog_ax_vlan_dbg("\nactGroupList[%d] vidx = %d,groupIp 0x%x,groupMbrBmp = 0x%x",\
									group_count,\
									actgroupList[group_count]->vidx,\
									actgroupList[group_count]->groupIp,\
									actgroupList[group_count]->groupMbrBmp);
					}
					else if (BRG_MC_FAIL == ret) {
						free(actgroupList[group_count]);
						ret = NPD_VLAN_ERR_HW;
					}
					group_count++;
					syslog_ax_vlan_dbg("loop iter %d,group_count =%d",vidx,group_count);
				}
				else {
					ret = NPD_GROUP_NOTEXIST;
					continue;
				}
			}
		}
		if (group_count > 0) {
			ret = NPD_DBUS_SUCCESS;
		}
	}
		
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);/*SUCCESS,GROUP_NOTEXIST,NPD_VLAN_ERR_HW*/
	/* Total active group count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &group_count);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);

	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
								      DBUS_STRUCT_BEGIN_CHAR_AS_STRING /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*vidx*/
											DBUS_TYPE_UINT32_AS_STRING /*groupIp*/
										   	DBUS_TYPE_UINT32_AS_STRING /*mbrBmp*/
    								  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < group_count; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		 syslog_ax_vlan_dbg("actgroupList[%d] vlanId=%d,vidx=%d ,groupIp 0x%x, groupMbrBmp %#0x",\
													i,\
													vlanId,\
													actgroupList[i]->vidx,	\
													actgroupList[i]->groupIp,\
													actgroupList[i]->groupMbrBmp);
		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT16,
				  &(actgroupList[i]->vidx));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actgroupList[i]->groupIp));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actgroupList[i]->groupMbrBmp));

		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(actgroupList[i]);/*05/19*/
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}

/*show all group member in one vlan. one vlan can has one or more groups.*/
DBusMessage * npd_dbus_vlan_show_vlan_group_list_port_mbr_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply;
	DBusMessageIter		iter;
	DBusMessageIter		iter_array;
	DBusError err;
	static groupList *actgroupList[CHASSIS_VLAN_RANGE_MAX];     /* change to static for npd error, zhangdi 2011-10-13 */
	unsigned char	devNum = 0;
	unsigned int	i,j,group_count = 0,ret = IGMPSNP_RETURN_CODE_OK, npdret =0;
	unsigned short	vlanId =0,vidx =0;
	unsigned int	groupAddr = 0;
	enum product_id_e productId = PRODUCT_ID; 
	unsigned char enableFlag = 0, vlanEnFlag = 0, enableFlagm =0;
	unsigned int num_of_asic = 0;
	unsigned short groupAddrv6[SIZE_OF_IPV6_ADDR];
	unsigned short *paddr6 = NULL;

	/*Read from SW Record or vlan entry table in HW*/
	/*here choose the later method*/
	dbus_error_init(&err);

	
	if(!((productinfo.capbmp) & FUNCTION_IGMP_VLAUE)){
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else if(((productinfo.capbmp) & FUNCTION_IGMP_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_IGMP_VLAUE)){	
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else{
		if (!(dbus_message_get_args ( msg, &err,
			DBUS_TYPE_UINT16,&vlanId,
			DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		npd_check_igmp_snp_status(&enableFlag);
		npd_check_mld_snp_status(&enableFlagm);
		if((0 == enableFlag) && (0 == enableFlagm)){
			ret = IGMPSNP_RETURN_CODE_NOT_ENABLE_GBL;
		}
		else {
			ret = npd_check_igmp_snp_vlan_status(vlanId,&vlanEnFlag);
			if(IGMPSNP_RETURN_CODE_OK == ret) {
				if(NPD_FALSE == vlanEnFlag) {
					ret = IGMPSNP_RETURN_CODE_NOTENABLE_VLAN;
				}
				syslog_ax_vlan_dbg("igmp or mld snooping vlan %d %s",vlanId,vlanEnFlag ? "enabled":"disabled");
			}
		} 

		syslog_ax_vlan_dbg("igmp or mld snooping ensure the vlan %d",vlanId);
		
		if(IGMPSNP_RETURN_CODE_OK == ret) {
			ret = npd_igmp_snp_l2mc_vlan_exist(vlanId);
			if(IGMPSNP_RETURN_CODE_OK != ret ) {
				ret = IGMPSNP_RETURN_CODE_MC_VLAN_NOT_EXIST;
			}
			else{
				for(vidx=0;vidx<CHASSIS_VIDX_RANGE_MAX;vidx++){
					ret = npd_igmp_snp_l2mc_group_exist(vlanId,vidx);
					if(IGMPSNP_RETURN_CODE_OK == ret ){
						actgroupList[group_count] = (groupList*)malloc(sizeof(groupList));
						memset(actgroupList[group_count],0,sizeof(groupList));
						actgroupList[group_count]->vidx = vidx;
						paddr6 = groupAddrv6;
						ret = npd_igmp_get_groupip_by_vlan_vidx(vlanId,vidx,&groupAddr, &paddr6, &npdret);
						syslog_ax_vlan_dbg("mld snooping get the groupip vlanid %d vidx %d groupaddr 0x%x npdret %d \n"\
										"groupaddrv6 %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",vlanId,vidx,groupAddr,npdret,ADDRSHOWV6(groupAddrv6));
						if((IGMPSNP_RETURN_CODE_OK == ret) && (0 != groupAddr) && (NPD_FALSE == npdret)){
							actgroupList[group_count]->groupIp = groupAddr;
						}
						else if((IGMPSNP_RETURN_CODE_OK == ret) && (0 != groupAddr) && (NPD_TRUE == npdret)){
							paddr6 = actgroupList[group_count]->groupIpv6;
							npd_mld_copy_ipv6addr(groupAddrv6, &paddr6);
							syslog_ax_vlan_dbg("mld snooping get the groupip groupAddrv6 %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x.\n"\
											"actgroupList[group_count]->groupIpv6 %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x.\n", ADDRSHOWV6(groupAddrv6),ADDRSHOWV6(actgroupList[group_count]->groupIpv6));
						}
						else {
							free(actgroupList[group_count]);
							continue;
						}

						num_of_asic = nam_asic_get_instance_num();
						for (devNum = 0; devNum < num_of_asic; devNum++) {
							ret = nam_asic_group_mbr_bmp_v1(devNum,vidx,groupAddr,vlanId,&(actgroupList[group_count]->groupMbrBmp));
							if ((BRG_MC_SUCCESS == ret) || (-7 == ret)) { /* ret = -7, is notfound*/
								ret = IGMPSNP_RETURN_CODE_OK;
								 syslog_ax_vlan_dbg("\nactGroupList[%d] vidx = %d,groupIp 0x%x,groupIpv6 %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x groupMbrBmp[0] = 0x%x, groupMbrBmp[1]=0x%x\n",\
											group_count,\
											actgroupList[group_count]->vidx,\
											actgroupList[group_count]->groupIp,\
											ADDRSHOWV6(actgroupList[group_count]->groupIpv6),\
											actgroupList[group_count]->groupMbrBmp.portMbr[0],\
											actgroupList[group_count]->groupMbrBmp.portMbr[1]);
							}
							else {
								free(actgroupList[group_count]);
								ret = IGMPSNP_RETURN_CODE_ERROR_HW;
								break;
							}
						}
						group_count++;
						syslog_ax_vlan_dbg("loop iter %d,group_count =%d",vidx,group_count);
					}
					else {
						ret = IGMPSNP_RETURN_CODE_GROUP_NOTEXIST;
						continue;
					}
				}
			}
			if (group_count > 0) {
				ret = IGMPSNP_RETURN_CODE_OK;
			}
		}
	}
		
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);/*SUCCESS,GROUP_NOTEXIST,NPD_VLAN_ERR_HW*/
	/* Total active group count*/
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &group_count);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &productId);

	/*open outer container*/
	dbus_message_iter_open_container (&iter,
								   DBUS_TYPE_ARRAY,
									  DBUS_STRUCT_BEGIN_CHAR_AS_STRING /*begin*/
											DBUS_TYPE_UINT16_AS_STRING /*vidx*/
											DBUS_TYPE_UINT32_AS_STRING /*groupIp*/
											DBUS_TYPE_UINT16_AS_STRING /*groupIpv6[0]*/
											DBUS_TYPE_UINT16_AS_STRING /*groupIpv6[1]*/
											DBUS_TYPE_UINT16_AS_STRING /*groupIpv6[2]*/
											DBUS_TYPE_UINT16_AS_STRING /*groupIpv6[3]*/
											DBUS_TYPE_UINT16_AS_STRING /*groupIpv6[4]*/
											DBUS_TYPE_UINT16_AS_STRING /*groupIpv6[5]*/
											DBUS_TYPE_UINT16_AS_STRING /*groupIpv6[6]*/
											DBUS_TYPE_UINT16_AS_STRING /*groupIpv6[7]*/
											DBUS_TYPE_UINT32_AS_STRING /*mbrBmp[0]*/
											DBUS_TYPE_UINT32_AS_STRING /*mbrBmp[1]*/
									  DBUS_STRUCT_END_CHAR_AS_STRING, /*end*/
								   &iter_array);
	for (i = 0; i < group_count; i++ ) {
		DBusMessageIter iter_struct;
		dbus_message_iter_open_container (&iter_array,
										   DBUS_TYPE_STRUCT,
										   NULL,
										   &iter_struct);
		 syslog_ax_vlan_dbg("actgroupList[%d] vlanId=%d,vidx=%d ,groupIp 0x%x,groupIpv6 %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x,  groupMbrBmp[0] %#0x, groupMbrBmp[1] %#0x\n",\
													i,\
													vlanId,\
													actgroupList[i]->vidx,	\
													actgroupList[i]->groupIp,\
													ADDRSHOWV6(actgroupList[i]->groupIpv6),\
													actgroupList[i]->groupMbrBmp.portMbr[0],\
													actgroupList[i]->groupMbrBmp.portMbr[1]);
		
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT16,
				  &(actgroupList[i]->vidx));

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actgroupList[i]->groupIp));

		for(j=0; j<SIZE_OF_IPV6_ADDR; j++){
			dbus_message_iter_append_basic
					(&iter_struct,
					  DBUS_TYPE_UINT16,
					  &(actgroupList[i]->groupIpv6[j]));
		}

		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actgroupList[i]->groupMbrBmp.portMbr[0]));
		dbus_message_iter_append_basic
				(&iter_struct,
				  DBUS_TYPE_UINT32,
				  &(actgroupList[i]->groupMbrBmp.portMbr[1]));

		dbus_message_iter_close_container (&iter_array, &iter_struct);
		free(actgroupList[i]);/*05/19*/
	}
	dbus_message_iter_close_container (&iter, &iter_array);
	
	return reply;
}

/* show a special vlan route port-member slot_port */
DBusMessage *npd_dbus_vlan_show_vlan_routeport_member
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply = NULL;
	DBusMessageIter	iter;
	DBusError err;

	char			*vlanName = NULL;	
	unsigned short	vlanId = 0;
	unsigned int	ret = IGMPSNP_RETURN_CODE_OK;
	unsigned int	tagBmp = 0;	
	unsigned int	untagBmp = 0;
	unsigned int 	vlanStat = VLAN_STATE_DOWN_E;
	unsigned int	eth_g_index = 0;
	
	unsigned int	i = 0;
	unsigned int	slotNo = 0;
	unsigned int	slot_index = 0;
	unsigned int	localPortNo = 0;
	unsigned int	MbrBmpTmpSP = 0;

	struct vlan_s	 *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	struct vlan_port_list_node_s *node = NULL;
	struct list_head *list = NULL;
	struct list_head *pos = NULL;

	vlanName = (char*)malloc(ALIAS_NAME_SIZE + 1);
	if(NULL == vlanName) {
		return reply;
	}
	memset(vlanName, 0, ALIAS_NAME_SIZE + 1);

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT16, &vlanId,
								DBUS_TYPE_INVALID)))
	{
		syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_vlan_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		free(vlanName);
		vlanName = NULL;
		return NULL;
	}

	if (!CHASSIS_VLANID_ISLEGAL(vlanId))
	{
		ret = IGMPSNP_RETURN_CODE_NO_SUCH_VLAN;
		syslog_ax_vlan_dbg("no such vlan =%d.\n", vlanId);
	}
	else if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS == npd_check_vlan_exist(vlanId))
	{
		ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
		syslog_ax_vlan_dbg("vlanId =%d NOT exists.\n", vlanId);
	}
	else 
	{    
		/* get the vlanNode 			*/
		vlanNode = npd_find_vlan_by_vid(vlanId);
		vlanStat = vlanNode->brgVlan.state;
		memcpy(vlanName,vlanNode->vlanName,ALIAS_NAME_SIZE);

		/* find route port in tagPortList	*/
		portList = vlanNode->tagPortList;
		if (portList != NULL && portList->count != 0)
		{
			list = &(portList->list);
			__list_for_each(pos, list)
			{
				node = list_entry(pos, struct vlan_port_list_node_s, list);
				if (NULL != node && NPD_TRUE == node->igmp_rport_flag)
				{
					eth_g_index = node->eth_global_index;
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
					localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
					MbrBmpTmpSP |= 1<<((slotNo-1)*8+localPortNo);
					syslog_ax_vlan_dbg("vlan %d tag port index[%d] = %#x bitmap %#x\n",
										vlanId, i++, eth_g_index, MbrBmpTmpSP);
				}
			}
		}
		tagBmp = MbrBmpTmpSP;
		
		/* find route port in untagPortList */
		i = 0;
		eth_g_index = 0;
		MbrBmpTmpSP = 0;		
		portList = vlanNode->untagPortList;
		if (portList != NULL && portList->count != 0)
		{
			list = &(portList->list);
			__list_for_each(pos, list)
			{
				node = list_entry(pos, struct vlan_port_list_node_s, list);
				if (NULL != node && NPD_TRUE == node->igmp_rport_flag)
				{
					eth_g_index = node->eth_global_index;
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
					localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
					MbrBmpTmpSP |= 1<<((slotNo-1)*8+localPortNo);
					syslog_ax_vlan_dbg("vlan %d untag port index[%d] = %#x bitmap %#x\n",
										vlanId, i++, eth_g_index, MbrBmpTmpSP);
				}
			}
		}
		untagBmp = MbrBmpTmpSP;

		ret = IGMPSNP_RETURN_CODE_OK;
	}

	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append(reply, &iter);
		
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &ret);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &productId);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_STRING, &vlanName);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &untagBmp);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &tagBmp);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &vlanStat);
	free(vlanName);
	vlanName = NULL;	
	return reply;
}

/* show a special vlan route port-member slot_port */
DBusMessage *npd_dbus_vlan_show_vlan_routeport_member_v1
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply = NULL;
	DBusMessageIter	iter;
	DBusError err;

	char			*vlanName = NULL;	
	unsigned short	vlanId = 0;
	unsigned int	ret = IGMPSNP_RETURN_CODE_OK;
	unsigned int 	vlanStat = VLAN_STATE_DOWN_E;
	unsigned int	eth_g_index = 0;
	
	unsigned int	i = 0;
	unsigned int	slotNo = 0;
	unsigned int	slot_index = 0;
	unsigned int	localPortNo = 0;
	PORT_MEMBER_BMP	MbrBmpTmpSP,tagBmp,untagBmp;
	
	struct vlan_s	 *vlanNode = NULL;
	vlan_port_list_s *portList = NULL;
	enum product_id_e productId = PRODUCT_ID; 
	struct vlan_port_list_node_s *node = NULL;
	struct list_head *list = NULL;
	struct list_head *pos = NULL;
	unsigned int promisPortBmp[2] = {0};

    memset(&MbrBmpTmpSP,0,sizeof(PORT_MEMBER_BMP));
	memset(&tagBmp,0,sizeof(PORT_MEMBER_BMP));
	memset(&untagBmp,0,sizeof(PORT_MEMBER_BMP));
	vlanName = (char*)malloc(ALIAS_NAME_SIZE + 1);
	if(NULL == vlanName) {
		return reply;
	}	
	memset(vlanName, 0, ALIAS_NAME_SIZE + 1);

	dbus_error_init(&err);
	
	if(!((productinfo.capbmp) & FUNCTION_IGMP_VLAUE)){
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else if(((productinfo.capbmp) & FUNCTION_IGMP_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_IGMP_VLAUE)){	
			npd_syslog_dbg("do not support igmp!\n");
			ret  = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;	}
	else{
	if (!(dbus_message_get_args(msg, &err,
								DBUS_TYPE_UINT16, &vlanId,
								DBUS_TYPE_INVALID)))
	{
		syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			syslog_ax_vlan_err("%s raised: %s", err.name, err.message);
			dbus_error_free(&err);
		}
		free(vlanName);
		vlanName = NULL;		
		return NULL;
	}

	if (!CHASSIS_VLANID_ISLEGAL(vlanId))
	{
		ret = IGMPSNP_RETURN_CODE_NO_SUCH_VLAN;
		syslog_ax_vlan_dbg("no such vlan =%d.\n", vlanId);
	}
	else if(VLAN_RETURN_CODE_VLAN_EXISTS != npd_check_vlan_exist(vlanId))
	{
		ret = IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
		syslog_ax_vlan_dbg("vlanId =%d NOT exists.\n", vlanId);
	}
	else 
	{    
		npd_vlan_get_promis_port_bmp(&promisPortBmp);
		/* get the vlanNode 			*/
		vlanNode = npd_find_vlan_by_vid(vlanId);
		vlanStat = vlanNode->brgVlan.state;
		memcpy(vlanName,vlanNode->vlanName,ALIAS_NAME_SIZE);

		/* find route port in tagPortList	*/
		portList = vlanNode->tagPortList;
		if (portList != NULL && portList->count != 0)
		{
			list = &(portList->list);
			__list_for_each(pos, list)
			{
				node = list_entry(pos, struct vlan_port_list_node_s, list);
				if (NULL != node && NPD_TRUE == node->igmp_rport_flag)
				{
					eth_g_index = node->eth_global_index;
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
					localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					
					#if 1
					MbrBmpTmpSP.portMbr[localPortNo/32] |= 1<<(localPortNo%32);
                    #else
					localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
					if(localPortNo>31)
					   MbrBmpTmpSP.portMbr[1] |= 1<<((slotNo-1)*8+(localPortNo-32));
					else
					   MbrBmpTmpSP.portMbr[0] |= 1<<((slotNo-1)*8+localPortNo);
					
					#endif
					syslog_ax_vlan_dbg("vlan %d tag port index[%d] = %#x bitmap[0] %#x bitmap[1] %#x\n",
										vlanId, i++, eth_g_index, 
										MbrBmpTmpSP.portMbr[0],
										MbrBmpTmpSP.portMbr[1]);
				}
			}
		}
		tagBmp.portMbr[0]= MbrBmpTmpSP.portMbr[0];
		tagBmp.portMbr[1]= MbrBmpTmpSP.portMbr[1];
		/* find route port in untagPortList */
		i = 0;
		eth_g_index = 0;
		memset(&MbrBmpTmpSP,0,sizeof(PORT_MEMBER_BMP));		
		portList = vlanNode->untagPortList;
		if (portList != NULL && portList->count != 0)
		{
			list = &(portList->list);
			__list_for_each(pos, list)
			{
				node = list_entry(pos, struct vlan_port_list_node_s, list);
				if (NULL != node && NPD_TRUE == node->igmp_rport_flag)
				{
					eth_g_index = node->eth_global_index;
					slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
					slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
					localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);

					#if 1
					MbrBmpTmpSP.portMbr[localPortNo/32] |= 1<<(localPortNo%32);
                    #else
					
					localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
					MbrBmpTmpSP.portMbr[i/32] |= 1<<((slotNo-1)*8+(localPortNo%32));
					#endif
					syslog_ax_vlan_dbg("vlan %d untag port index[%d] = %#x bitmap[0] %#x bitmap[1] %#x\n",
										vlanId, i++, eth_g_index, 
										MbrBmpTmpSP.portMbr[0],
										MbrBmpTmpSP.portMbr[1]);
				}
			}
		}
		untagBmp.portMbr[0] = MbrBmpTmpSP.portMbr[0];
        untagBmp.portMbr[1] = MbrBmpTmpSP.portMbr[1];
		ret = IGMPSNP_RETURN_CODE_OK;
		}
	}

	reply = dbus_message_new_method_return(msg);
		
	dbus_message_iter_init_append(reply, &iter);
		
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &ret);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &productId);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &promisPortBmp[0]);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &promisPortBmp[1]);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_STRING, &vlanName);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &untagBmp.portMbr[0]);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &untagBmp.portMbr[1]);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &tagBmp.portMbr[0]);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &tagBmp.portMbr[1]);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &vlanStat);
	free(vlanName);
	vlanName = NULL;	
	return reply;
}


/* show a special vlan route port-member slot_port */
DBusMessage *npd_dbus_vlan_exchange_ifindex_igmp_to_npd
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	DBusMessage* reply;
	DBusMessageIter	iter;
	DBusMessageIter	iter_struct;
	DBusMessageIter iter_array;
	DBusError err;

	unsigned int ret = IGMPSNP_RETURN_CODE_OK;
	unsigned int i = 0;

	unsigned int eth_g_index = 0;
	unsigned int slotNo = 0;
	unsigned int slot_index = 0;
	unsigned int localPortNo = 0;
	unsigned int count = 0;

	PORT_MEMBER_BMP routeBmp = {0};
	long routeArray[MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT];

	for (i = 0; i < (MAX_ETHPORT_PER_BOARD * CHASSIS_SLOT_COUNT); i++) {
		routeArray[i] = -1;
	}

	dbus_message_iter_init(msg, &iter);
	/* get count of running route port*/
	dbus_message_iter_get_basic(&iter, &count);

	dbus_message_iter_next(&iter);

	/* get port-index of running route port*/
	if (count > 0) {
		dbus_message_iter_recurse(&iter,&iter_array);
		for (i = 0; i < count; i++) {
			dbus_message_iter_recurse(&iter_array, &iter_struct);
			dbus_message_iter_get_basic(&iter_struct, &routeArray[i]);
			dbus_message_iter_next(&iter_struct);
			dbus_message_iter_next(&iter_array);
		}
	}

	for (i = 0; i < count; i++)
	{
		if (routeArray[i] != -1)
		{
			syslog_ax_vlan_dbg("routeArray[%d] =[%d]\n", i, routeArray[i]);
			eth_g_index = routeArray[i];
			slot_index = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
			slotNo = CHASSIS_SLOT_INDEX2NO(slot_index);
			localPortNo = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
			localPortNo = ETH_LOCAL_INDEX2NO(slot_index, localPortNo);
			if (localPortNo > 31) {
			   routeBmp.portMbr[1] |= 1 << ((slotNo - 1) * 8 + (localPortNo - 32));
			}else {
			   routeBmp.portMbr[0] |= 1 << ((slotNo - 1) * 8 + localPortNo);
			}
		}
	}

	syslog_ax_vlan_dbg(" npd vlan routeBmp[0] =[%#x] routeBmp[1] =[%#x]\n",
						routeBmp.portMbr[0],
						routeBmp.portMbr[1]);
	ret = IGMPSNP_RETURN_CODE_OK;

	reply = dbus_message_new_method_return(msg);

	dbus_message_iter_init_append(reply, &iter);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &ret);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &routeBmp.portMbr[0]);
	dbus_message_iter_append_basic(&iter,
									DBUS_TYPE_UINT32, &routeBmp.portMbr[1]);

	return reply;
}


/*npd_dbus_vlan_portmbr_config_igmp_snoop*/
DBusMessage * npd_dbus_vlan_get_slotport_by_ethportindex
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned char	slotNo = 0,localPortNo = 0;
	unsigned int	slotIndex = 0,portIndex = 0,eth_g_index = 0;
	unsigned int 	ret = NPD_DBUS_SUCCESS;
	DBusError err;
	
	syslog_ax_vlan_dbg("Entering convert eth_g_index to slot/port...\n");

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_UINT32,&eth_g_index,
								DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	syslog_ax_vlan_dbg("get param: eth_port_index %d.",eth_g_index);
	slotIndex = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	slotNo = CHASSIS_SLOT_INDEX2NO(slotIndex);
	portIndex = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	localPortNo = ETH_LOCAL_INDEX2NO(slotIndex, portIndex);
	if (CHASSIS_SLOTNO_ISLEGAL(slotNo)) {
		if (ETH_LOCAL_PORTNO_ISLEGAL(slotNo,localPortNo)) {
			ret = NPD_DBUS_SUCCESS;
		}
		else {
			 syslog_ax_vlan_dbg("illegal slot no!");
			ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
		}
	}
	else {
		 syslog_ax_vlan_dbg("illegal port no!");
		ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
	}

	syslog_ax_vlan_dbg("retval = %d,slot = %d,port = %d.",ret);
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &slotNo);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &localPortNo);
	return reply;
}

#endif


DBusMessage * npd_dbus_vlan_config_vlan_mtu(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int mtu = 1522, routeMtu = 1508;/* route mtu with no layer 2 info */
	int ret  = 0;
	DBusError err;
	unsigned char dev = 0;
	unsigned int mtuIndex = 0; /*default  mtuindex ,all vlan bind the value*/

	NPD_DEBUG(("Entering config vlan mtu ...\n"));

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&mtu,
									DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		NPD_DEBUG(("config mtu value %d\n",mtu));
		ret = nam_config_vlan_mtu(dev,mtuIndex,mtu);
		if(0 == ret) {
			vlan_mtu = mtu;
			ret = VLAN_RETURN_CODE_ERR_NONE;
		}

		/* 
		  * IP route mtu contains only all ip packet length without layer 2 info 
		  * 12 for DMAC and SMAC length 
		  * 2 for ether type length
		  */
		routeMtu = mtu-12-2;
		nam_config_route_mtu(dev, mtuIndex, routeMtu);
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
	
}

DBusMessage * npd_dbus_vlan_config_vlan_egress_filter(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int isable;
	int ret  = 0;
	DBusError err;
	unsigned char dev = 0;

	NPD_DEBUG(("Entering config vlan egress filter ...\n"));

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT32,&isable,
									DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		NPD_DEBUG(("config egress filter value %d\n",isable));
		ret = nam_config_vlan_egress_filter(dev,isable);
		if(NPD_SUCCESS != ret) {
			 syslog_ax_vlan_err("nam_config_vlan_egress_filter() fail");
		}
		else {
			g_vlan_egress_filter = isable;
		}
	}

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
	
}

DBusMessage * npd_dbus_vlan_config_vlan_filter(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	unsigned int filterType = 0xff,enDis = NPD_FALSE;
	int ret  = 0;
	unsigned short vlanId = 0;
	DBusError err;

	NPD_DEBUG(("Entering config vlan filter ...\n"));

	if(!((productinfo.capbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else if(((productinfo.capbmp) & FUNCTION_VLAN_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_VLAN_VLAUE)){
		npd_syslog_dbg("do not support vlan!\n");
		ret = COMMON_PRODUCT_NOT_SUPPORT_FUCTION;
	}
	else {
		dbus_error_init(&err);
		
		if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_UINT16,&vlanId,
									DBUS_TYPE_UINT32,&filterType,
									DBUS_TYPE_UINT32,&enDis,
									DBUS_TYPE_INVALID))) {
			 syslog_ax_vlan_err("Unable to get input args ");
			if (dbus_error_is_set(&err)) {
				 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
				dbus_error_free(&err);
			}
			return NULL;
		}
		syslog_ax_vlan_dbg("set vlan filterType %d %s\n",filterType,(1 == enDis)?"enalbe":"disable");
		ret = nam_config_vlan_filter(vlanId,filterType,enDis);
		if(0 != ret) {
			ret = NPD_DBUS_ERROR;
		}
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
	
}

DBusMessage * npd_dbus_vlan_egress_filter_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter  iter;
	char *showStr = NULL, *cursor = NULL;
	int totalLen = 0;

	showStr = (char*)malloc(NPD_VLAN_RUNNING_CFG_MEM);
	if(NULL == showStr) {
		 syslog_ax_vlan_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_VLAN_RUNNING_CFG_MEM);
	
	cursor = showStr;

	 if(g_vlan_egress_filter == NPD_FALSE){
			totalLen += sprintf(cursor,"config vlan egress-filter disable\n");
			cursor = showStr + totalLen;			
	 }

	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr); 
	free(showStr);
	showStr = NULL;
	return reply;
}

DBusMessage * npd_dbus_vlan_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	char *showStr = NULL;
    int advVidStrLen = strlen("config advanced-routing default-vid empty\n  ");

	showStr = (char*)malloc(NPD_VLAN_RUNNING_CFG_MEM + advVidStrLen);
	if(NULL == showStr) {
		 syslog_ax_vlan_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_VLAN_RUNNING_CFG_MEM + advVidStrLen);
	/*save vlan cfg*/
	npd_save_vlan_cfg(showStr,NPD_VLAN_RUNNING_CFG_MEM + advVidStrLen);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_STRING,
									 &showStr);	
	free(showStr);
	showStr = NULL;
	return reply;
}

DBusMessage * npd_dbus_vlan_igmp_snp_show_running_config(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	char *showStr = NULL;
	unsigned char en_dis = 0,Ismld = 0;

	dbus_error_init(&err);

	if (!(dbus_message_get_args ( msg, &err,
									DBUS_TYPE_BYTE,&Ismld,
									DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	showStr = (char*)malloc(NPD_VLAN_IGMP_SNP_RUNNING_CFG_MEM);
	if(NULL == showStr) {
		 syslog_ax_vlan_err("memory malloc error\n");
		return NULL;
	}
	memset(showStr,0,NPD_VLAN_IGMP_SNP_RUNNING_CFG_MEM);
	/*save vlan cfg*/
	npd_save_vlan_igmp_snp_cfg(showStr,NPD_VLAN_IGMP_SNP_RUNNING_CFG_MEM,&en_dis,Ismld);
	
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
 * npd_dbus_vlan_igmp_get_product_function
 *		igmp get product function
 *
 *	INPUT:
 *		DBusConnection *conn, 
 *		DBusMessage *msg, 
 *		void *user_data
 *
 *	OUTPUT:
 *		NULL
 *
 *	RETURN:
 *		reply
 *	 	IsSupport:
 *			1				- function do not support
 *			0				- function support
 *
 **********************************************************************************/

DBusMessage * npd_dbus_vlan_igmp_get_product_function(DBusConnection *conn, DBusMessage *msg, void *user_data) {

	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char IsSupport = 0;

	syslog_ax_dldp_dbg("Entering get product igmp function!\n");
	
	if(!((productinfo.capbmp) & FUNCTION_IGMP_VLAUE)){
		npd_syslog_dbg("do not support igmp!\n");
		IsSupport = 1;
	}
	else if(((productinfo.capbmp) & FUNCTION_IGMP_VLAUE) \
			&& !((productinfo.runbmp) & FUNCTION_IGMP_VLAUE)){
		npd_syslog_dbg("do not support igmp!\n");
		IsSupport = 1;
	}
		
	dbus_error_init(&err);
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_BYTE,
									 &IsSupport);
	return reply;
	
}

#if 1
DBusMessage * npd_dbus_asic_bcast_rate_limiter
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned int enDis ;
	int ret  = 0;

	NPD_DEBUG(("Entering config asic broadcast rate limiter...\n"));

	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
								DBUS_TYPE_UINT32,&enDis,
								DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	NPD_DEBUG(("config flag %d\n",enDis));
	ret = nam_asic_bcast_rate_limiter_config(enDis);
	if(0 == ret) {
		bcast_ratelimit = enDis;
	}
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &bcast_ratelimit);
	return reply;
	
}


/*get promiscuous mode port bitmap*/
unsigned int npd_vlan_get_promis_port_bmp(unsigned int * promisPortBmp){
    npd_vlan_get_port_bmp_by_mode(promisPortBmp,ETH_PORT_FUNC_MODE_PROMISCUOUS);
	return 0;
}
/**************************************************************
 *npd_vlan_get_port_bmp_by_mode
 *
 *DESCRIPTION:
 *    get port bitmap by mode
 *INPUT:
 *    mode  -- the mode what by we want to get the bitmap 
 *OUTPUT:
 *    portBmp  --  the bitmap we got 
 *NOTE:
 *    we assume the port is not more than 64, and the portBmp should be a 
 * integer array which length is 2
 *
 **************************************************************/
unsigned int npd_vlan_get_port_bmp_by_mode(unsigned int * portBmp,unsigned int mode){
	unsigned int eth_g_index = 0;
	unsigned int slot_index = 0;
	unsigned int port_index = 0;
	unsigned int slotNo = 0;
	unsigned int localPortNo = 0;
	unsigned int i = 0;
	unsigned int ifIndex = ~0UI;
	portBmp[0] = 0;
	portBmp[1] = 0;
	for(i = 0; i < 64; i++){
		if((PRODUCT_ID_AX7K_I == PRODUCT_ID) || (PRODUCT_ID_AX7K == PRODUCT_ID)) {
			slotNo = i/8 + 1;
			localPortNo = i%8;
		}
		else if((PRODUCT_ID_AX5K == PRODUCT_ID) ||
				(PRODUCT_ID_AU4K == PRODUCT_ID) ||
				(PRODUCT_ID_AU3K == PRODUCT_ID) ||
				(PRODUCT_ID_AU3K_BCM == PRODUCT_ID) ||
				(PRODUCT_ID_AU3K_BCAT == PRODUCT_ID) ||
				(PRODUCT_ID_AU2K_TCAT == PRODUCT_ID)){
			slotNo = 1;
			localPortNo = i;
		}
		else if(PRODUCT_ID_AX5K_I == PRODUCT_ID) {
			/* ports 1/9 ~ 1/12 are out of control which 
 			 * 	 connect to CPU RGMII interface directly
 			 *	by qinhs@autelan.com 2009/7/27
			 */ 
			if( i >= 9 ) {
				continue;
			}
			else {
				slotNo = 1;
				localPortNo = i;
			}
		}
		if (ETH_LOCAL_PORTNO_ISLEGAL(slotNo,localPortNo)) {
			slot_index = CHASSIS_SLOT_NO2INDEX(slotNo);
			port_index = ETH_LOCAL_NO2INDEX(slot_index, localPortNo);
			eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,port_index);
		
			if(ETH_PORT_FUNC_BRIDGE == mode){
				if(NPD_TRUE == npd_check_port_switch_mode(eth_g_index)){
	                portBmp[i/32] |= (1<<(i%32));
				}
			}else if(ETH_PORT_FUNC_IPV4 == mode ){
				if((NPD_TRUE == npd_eth_port_interface_check(eth_g_index,&ifIndex))&&(ifIndex != ~0UI)){
					portBmp[i/32] |= (1<<(i%32));
		        }
			}
			else if(ETH_PORT_FUNC_MODE_PROMISCUOUS == mode){
		        if((TRUE != npd_eth_port_rgmii_type_check(slotNo,localPortNo))&&\
					(NPD_TRUE == npd_check_port_promi_mode(eth_g_index))){
					portBmp[i/32] |= (1<<(i%32));
		        }
			}
		}
    }
	return 0;
}

#endif

#ifndef AX_PLATFORM_DISTRIBUTED

/*
    For distributed vlan init
*/
int npd_init_distributed_vlan(void)
{
	int i = 0;
	int fd = -1;
	int length = 0;
	int ret = -1;
	int devnum=0, portnum=0;
	int vlanId = 0;
	
	char* vlan_1 = "Default";
	char* file_path = "/dbm/shm/vlan/shm_vlan";
    void * mmaped_base = NULL;
    syslog_ax_eth_port_dbg("\n============== init distributed vlan: ===============\n");

    /* 1. init struct for old npd, in npd_init_vlan() */
	struct vlan_s * vlanInfo = NULL;
	g_vlans = malloc(sizeof(struct vlan_s*)*(NPD_VLAN_NUMBER_MAX));
	
	if(NULL == g_vlans) 
	{
		 syslog_ax_vlan_err("memory alloc error for vlan init!!\n");
		return -1;
	}
	memset(g_vlans,0,sizeof(struct vlan_s *) * NPD_VLAN_NUMBER_MAX);
    syslog_ax_eth_port_dbg("malloc g_vlans = 0x%x\n", g_vlans);	

    /* 2. init g_vlanback[] for vlan backup */	
	g_vlanback = malloc(sizeof(vlan_list_t)*4096);	
	if(NULL == g_vlanback)
	{
	    syslog_ax_vlan_err("memory alloc error for g_vlanback[].\n");
		return -1;
	}
	memset(g_vlanback, 0, sizeof(vlan_list_t)*4096);			
    syslog_ax_eth_port_dbg("malloc g_vlanback = 0x%x\n", g_vlanback);	

	
    /* 3. mmap file /dbm/shm/vlan/shm_vlan to g_vlanlist */
    {
		system("mkdir -p /dbm/shm/vlan/");	
        syslog_ax_eth_port_dbg("file_path = %s\n", file_path);
		fd = open(file_path, O_CREAT | O_RDWR, 00755);
		
    	if(fd < 0)
        {
            syslog_ax_eth_port_err("Failed to open! %s\n", strerror(errno));
            return -1;
        }

		length = lseek(fd, sizeof(vlan_list_t)*4096, SEEK_SET);
        write(fd,"",1);

        g_vlanlist = (vlan_list_t *)mmap(NULL, sizeof(vlan_list_t)*4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0 );

        syslog_ax_eth_port_dbg("mmap addr for shm_vlan: g_vlanlist = 0x%x\n", g_vlanlist);
    
        if (MAP_FAILED == g_vlanlist)
        {
            syslog_ax_eth_port_dbg("Failed to mmap for g_vlanlist[]! %s\n", strerror(errno));
			close(fd);	/* code optimize: recourse leak houxx@autelan.com  2013-1-18 */
            return -1;
        }
		memset(g_vlanlist, 0, sizeof(vlan_list_t)*4096);			
	
		close(fd);
    }

    /* create vlan1 & l3 vlan */
	ret = create_default_vlan();
    if (0 != ret)
    {
        syslog_ax_vlan_err("create_default_vlan() error !\n",ret);
        return -1;
    }
    syslog_ax_vlan_dbg("init default vlan1 on local board OK.\n\n");
	
	ret = create_port_l3intf_vlan();
    if (0 != ret)
    {
        syslog_ax_vlan_err("create_port_l3intf_vlan() error !\n",ret);
        return -1;
    }
    syslog_ax_vlan_dbg("\ninit L3 vlan on local board OK.\n\n");

	/* valid default vlan */
	npd_vlanlist_valid(1,vlan_1);
	#if 0
	/* init default vlan g_vlanlist[0],but AX71_CRSMU have not asic_board, jump */
	if(BOARD_TYPE != BOARD_TYPE_AX71_CRSMU)
	{
    	for (i= asic_board->asic_port_start_no; i<=(asic_board->asic_port_cnt_visable + asic_board->asic_port_start_no -1); i++ )
    	{
			if(i<=32)
			{
    		    g_vlanlist[0].untagPortBmp[SLOT_ID-1].low_bmp |= (unsigned int)(1<<(i-1));
			}
			else
			{
    		    g_vlanlist[0].untagPortBmp[SLOT_ID-1].high_bmp |= (unsigned int)(1<<(i-33));				
			}
    	}
	}

    ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( ret!=0 )
    {
        syslog_ax_vlan_err("msync shm_vlan failed \n" );
		return VLAN_RETURN_CODE_ERR_GENERAL;
    }
    syslog_ax_vlan_dbg("init default vlan1 on local board OK.\n");
	#endif
	
	/* init vlan 4094 for obc0 & rpa on AX71_CRSMU */
    if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
    {
		/* 1. Create vlan 4094 untag for TIPC */
		#if 0   /* do not need anymore, for the xcat is disable before load-config  */
        /* Disable port25 of 275 */	
        ret = nam_set_ethport_enable(0, 25, 0);
        if (0 != ret)
        {
            syslog_ax_vlan_err("disable port 25 fail, return %d\n",ret);
            return -1;
        }
        syslog_ax_vlan_dbg("disable port 25 of 275 OK\n");
		#endif
		
	    /* force 804 port 26 to 10G mode */	
		ret = force_port_to_xg_mode(1,26);
	    if(0 != ret)
		{
    		syslog_ax_vlan_err("force 804 port 26 to 10G mode error\n");
    		return -1;
	    }	
		
		/* add vlan 4094 for 275 */
        ret = create_vlan(0,4094);		/* code optimize:Unchecked return value  houxx@autelan.com  2013-1-18 */
		if(NPD_SUCCESS != ret)
		{
			syslog_ax_vlan_err("create vlan error\n");
    		return -1;
		}
	   	ret = nam_asic_vlan_entry_ports_add(0, 4094, 12, 0);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add cscd port to vlan %d error\n",4094);
    		return -1;
	    }
	   	ret = nam_asic_vlan_entry_ports_add(0, 4094, 26, 1);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add cscd port to vlan %d error\n",4094);
    		return -1;
	    }
		/* add vlan 4094 for 804 */
	    create_vlan(1,4094);	
	   	ret = nam_asic_vlan_entry_ports_add(1, 4094, 24, 0);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add cscd port to vlan %d error\n",4094);
    		return -1;
	    }
	   	ret = nam_asic_vlan_entry_ports_add(1, 4094, 26, 0);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add cscd port to vlan %d error\n",4094);
    		return -1;
	    }		
	   	ret = nam_asic_vlan_entry_ports_add(1, 4094, 27, 1);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add cscd port to vlan %d error\n",4094);
    		return -1;
	    }
    	syslog_ax_vlan_dbg("npd init vlan 4094 untag for TIPC on AX71-CRSMU OK!\n");

		/* 2. Create vlan 4093 tag for RPA, need just on 98DX804 */
	    ret = create_vlan(1,4093);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu create vlan %d error\n",4093);
    		return -1;
	    }		
	   	ret = nam_asic_vlan_entry_ports_add(1, 4093, 24, 1);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add port (1,24) tag to vlan %d error\n",4093);
    		return -1;
	    }
	   	ret = nam_asic_vlan_entry_ports_add(1, 4093, 26, 1);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add port (1,26) tag to vlan %d error\n",4093);
    		return -1;
	    }	
    	syslog_ax_vlan_dbg("npd init vlan 4093 tag for RPA on AX71-CRSMU OK!\n");
		
    }
	else	/* init vlan 4094 for internal rpa conmunicate */
	{
		/* 1. add cscd port untag to 4094,tag to vlan1.
		 * vlan4094---for rpa
		 * vlan1 -----for distributed vlan1
		 */
		vlanId = 4094;      /* vlan4094 - untag */
		create_vlan(0,vlanId);	 /* have not vlan4094, create it! */
		
        if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)
        {
    		create_vlan(1,vlanId);	 /* create vlan on lion-1 */
        }
		
        for(i=0; i<asic_board->asic_cscd_port_cnt; i++)
        {
        	devnum = asic_board->asic_cscd_ports[i].dev_num;
	        portnum = asic_board->asic_cscd_ports[i].port_num;
	        vlanId = 4094;
        	/*for AX8603 add slot1 port 25 and slot2 port 26 to vlan 4094 as untag */
			/*cscd ports are in vlan 1 as untag */
			if(PRODUCT_TYPE== AUTELAN_PRODUCT_AX8603)
			{
				if((BOARD_TYPE == BOARD_TYPE_AX81_AC12C) || (BOARD_TYPE == BOARD_TYPE_AX81_AC8C))
				{
					if(SLOT_ID == 1)
					{
						ret = nam_asic_vlan_entry_ports_add(devnum, vlanId, 25, 0);
			            if(0 != ret)
			    		{
			        		syslog_ax_vlan_err("nam add  port (%d,25) untag to vlan %d error\n",devnum,vlanId);
			        		return VLAN_RETURN_CODE_ERR_GENERAL;
			    	    }
			       		syslog_ax_vlan_dbg("nam add cpu port (%d,25) untag to vlan %d OK !\n",devnum,vlanId);
					}
					else if(SLOT_ID == 2)
					{
						ret = nam_asic_vlan_entry_ports_add(devnum, vlanId, 26, 0);
			            if(0 != ret)
			    		{
			        		syslog_ax_vlan_err("nam add  port (%d,26) untag to vlan %d error\n",devnum,vlanId);
			        		return VLAN_RETURN_CODE_ERR_GENERAL;
			    	    }
			       		syslog_ax_vlan_dbg("nam add cpu port (%d,26) untag to vlan %d OK !\n",devnum,vlanId);
					}
				}
				else if(BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S)
				{
					if(SLOT_ID == 1)
					{
						ret = nam_asic_vlan_entry_ports_add(devnum, vlanId, 24, 0);
			            if(0 != ret)
			    		{
			        		syslog_ax_vlan_err("nam add  port (%d,24) untag to vlan %d error\n",devnum,vlanId);
			        		return VLAN_RETURN_CODE_ERR_GENERAL;
			    	    }
			       		syslog_ax_vlan_dbg("nam add cpu port (%d,24) untag to vlan %d OK !\n",devnum,vlanId);
					}
					else if(SLOT_ID == 2)
					{
						ret = nam_asic_vlan_entry_ports_add(devnum, vlanId, 25, 0);
			            if(0 != ret)
			    		{
			        		syslog_ax_vlan_err("nam add  port (%d,25) untag to vlan %d error\n",devnum,vlanId);
			        		return VLAN_RETURN_CODE_ERR_GENERAL;
			    	    }
			       		syslog_ax_vlan_dbg("nam add cpu port (%d,25) untag to vlan %d OK !\n",devnum,vlanId);
					}
				}
			}
			else
			{
	        	ret = nam_asic_vlan_entry_ports_add(devnum, vlanId, portnum, 0);
	            if(0 != ret)
				{
	        		syslog_ax_vlan_err("nam add cscd port (%d,%d) untag to vlan %d error\n",devnum, portnum,vlanId);
	        		return VLAN_RETURN_CODE_ERR_GENERAL;
	    	    }
	       		syslog_ax_vlan_dbg("nam add cscd port (%d,%d) untag to vlan %d OK !\n",devnum, portnum,vlanId);
				
	        	vlanId = 1;     /* vlan1 - tag */
	        	ret = nam_asic_vlan_entry_ports_add(devnum, vlanId, portnum, 1);
	            if(0 != ret)
				{
	        		syslog_ax_vlan_err("nam add cscd port (%d,%d) tag to vlan %d error\n",devnum, portnum,vlanId);
	        		return VLAN_RETURN_CODE_ERR_GENERAL;
	    	    }
	       		syslog_ax_vlan_dbg("nam add cscd port (%d,%d) tag to vlan %d OK !\n",devnum, portnum,vlanId);
	            ret = nam_asic_set_port_vlan_ingresflt(devnum, portnum, 1);
				if(0 != ret)
				{
	        		syslog_ax_vlan_err("nam set port(%d,%d) vlan Ingress Filtering enable error !\n",devnum, portnum);
	        		return VLAN_RETURN_CODE_ERR_GENERAL;
	    	    }
				syslog_ax_vlan_err("nam set port(%d,%d) vlan Ingress Filtering enable OK !\n",devnum, portnum);
			}

		}
		/* 2. add cpu port(AC8C,AC12C) untag to 4094 */
        if((BOARD_TYPE == BOARD_TYPE_AX81_AC4X)||(BOARD_TYPE == BOARD_TYPE_AX81_AC8C)||(BOARD_TYPE == BOARD_TYPE_AX81_AC12C)||(BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S))
    	{
            vlanId = 4094;			
            for(i=0;i<MAX_CPU_PORT_NUM;i++)
            {
            	devnum = asic_board->asic_to_cpu_ports[i].dev_num;
            	portnum = asic_board->asic_to_cpu_ports[i].port_num;	

				if((devnum<0)||(portnum<0))
				{
					continue;
				}

				if(BOARD_TYPE == BOARD_TYPE_AX81_AC12C)
				{
					if(portnum == 24)
						continue;
				}
				else if(BOARD_TYPE == BOARD_TYPE_AX81_AC4X)
				{
					if(portnum  != 48)
						continue;
				}
            	ret = nam_asic_vlan_entry_ports_add(devnum, vlanId, portnum, 0);
                if(0 != ret)
        		{
            		syslog_ax_vlan_err("nam add cpu port (%d,%d) untag to vlan %d error\n",devnum, portnum,vlanId);
            		return VLAN_RETURN_CODE_ERR_GENERAL;
        	    }
           		syslog_ax_vlan_dbg("nam add cpu port (%d,%d) untag to vlan %d OK !\n",devnum, portnum,vlanId);

                ret = nam_asic_set_port_vlan_ingresflt(devnum, portnum, 1);
    			if(0 != ret)
    			{
            		syslog_ax_vlan_err("nam set port(%d,%d) vlan Ingress Filtering enable error !\n",devnum, portnum);
            		return VLAN_RETURN_CODE_ERR_GENERAL;
        	    }
    			syslog_ax_vlan_err("nam set port(%d,%d) vlan Ingress Filtering enable OK !\n",devnum, portnum);							
            }
			
			/* For AX81-1X12G12S have two cpu port 26 & 27, here add 27 */
			if(BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S)
			{
            	devnum = 0;
            	portnum = 26;
            	ret = nam_asic_vlan_entry_ports_add(devnum, vlanId, portnum, 0);
                if(0 != ret)
        		{
            		syslog_ax_vlan_err("nam add cpu port (%d,%d) untag to vlan %d error\n",devnum, portnum,vlanId);
            		return VLAN_RETURN_CODE_ERR_GENERAL;
        	    }
           		syslog_ax_vlan_dbg("nam add cpu port (%d,%d) untag to vlan %d OK !\n",devnum, portnum,vlanId);

                ret = nam_asic_set_port_vlan_ingresflt(devnum, portnum, 1);
    			if(0 != ret)
    			{
            		syslog_ax_vlan_err("nam set port(%d,%d) vlan Ingress Filtering enable error !\n",devnum, portnum);
            		return VLAN_RETURN_CODE_ERR_GENERAL;
        	    }
    			syslog_ax_vlan_err("nam set port(%d,%d) vlan Ingress Filtering enable OK !\n",devnum, portnum);				
			}
		}
		syslog_ax_vlan_dbg("init internal vlan 4094 & tag vlan1 OK !\n");
	
	}
    			
	/* create the thread for vlan up/down */
	nam_thread_create("NpdVlanCheckState",(void *)npd_vlan_check_state,NULL,NPD_FALSE,NPD_FALSE);
	syslog_ax_vlan_dbg("Create thread: NpdVlanCheckState OK!\n");
    syslog_ax_eth_port_dbg("=========== init distributed vlan OK ! ==============\n\n");
	
	return 0;	
}

extern int npd_intf_set_intf_state(unsigned char * ifname,unsigned int state);
int npd_vlan_check_bond_info(int vlanId)
{
	int count_bond = 0;
	int k = 0;
	for(k = 0; k < CHASSIS_SLOT_COUNT; k++ )
	{   
		if(g_vlanlist[vlanId-1].bond_slot[k] != 0)
		{
			count_bond++;
		}
	}
	if(count_bond > 0)
	{
		return VLAN_BONDED;
	}
	else
	{
		return VLAN_NOBONDED;	
	}		
}

int check_ve_port_status(unsigned char old_vlan_state[])
{	
	FILE *fp;	
	char cmd[128] = "ls /proc/net/vlan/ | grep '^ve.*\\..*' | sort ";
	char vename[20] = {0};	
	int ve_length = 0;
	unsigned char slot_no = 0,cpu_no = 0,cpu_port_no = 0;
	unsigned int  vid1 = 0,vid2 = 0;
	int ret = VLAN_RETURN_CODE_ERR_NONE;

	if((fp=popen(cmd,"r"))==NULL)	
	{		
		perror("popen");		
		syslog_ax_vlan_err("open qinq results from pipe failed \n");
		return VLAN_RETURN_CODE_ERR_GENERAL;
	}
	while((fgets(vename,20,fp))!= NULL)	
	{		
		ve_length = strlen(vename);
		/*use '\0' replace '\n'*/
		vename[ve_length-1]='\0';
		ret = npd_parse_ve_params(vename,&slot_no,&cpu_no,&cpu_port_no,&vid1,&vid2);	
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_err("parse %s failed !\n",vename);
			continue;
		}
		if(((g_vlanlist[vid1-1].bond_slot[slot_no-1])&(unsigned int)(1<<(cpu_port_no-1))))
		{
			/*this will first UP/DOWN ve subintf and then up/down qinq intf*/
			if(old_vlan_state[vid1-1] != g_vlanlist[vid1-1].updown)
			{
				syslog_ax_vlan_dbg("vlan %d state changed to %s\n",vid1,g_vlanlist[vid1-1].updown?"UP":"DOWN");
				if(g_vlanlist[vid1-1].updown == VLAN_UP)			
				{		
					ret = npd_intf_set_intf_state(vename,1);    /* set ve sub intf up */
					if(ret != 0)				
					{					
						syslog_ax_vlan_err(" set %s UP error !\n",vename);	
						continue;
					}				
					else				
					{					
						syslog_ax_vlan_dbg("Active-MCB set %s UP OK !\n",vename);				
					}	
					usleep(50000);
				}			
				else			
				{		
					ret = npd_intf_set_intf_state(vename,0);    /* set ve sub intf down */
					if(ret != 0)				
					{					
						syslog_ax_vlan_err(" set %s DOWN error !\n",vename);	
						continue;
					}				
					else				
					{					
						syslog_ax_vlan_dbg("Active-MCB set %s DOWN OK !\n",vename);				
					} 	
					if(vid2 == 0)
					{
						ret = nam_fdb_table_delete_entry_with_vlan(vid1);
						if(ret != 0)
						{
							syslog_ax_vlan_err("delete vlan %d fdb fail !\n");
						}
					}
				}
			}
			else
			{
				/*if vlan state not changed*/
				continue;
			}
		}
		else
		{
			syslog_ax_vlan_dbg("%s did not bond to cpu port.\n",vename);
			/*if ve subintf or QinQ intf not bond*/
			continue;
		}
	}
	pclose(fp);
	return ret;
}
/* Check vlan state on distributed system */
void npd_vlan_check_state(void)
{
	int vid_index =0, vid=0;
    int i =0, j =0, m =0, n=0, ret =0;
	int count_up = 0;
	char vename[21] = {0};
	unsigned int ifIndex = ~0UI;
	int is_active_master = -1;
	unsigned char	old_state =0, new_state = 0;
	unsigned char old_vlan_state[4096] = {0};
	int vlan_state_changed = 0;

	/* tell my thread id */
	npd_init_tell_whoami("NpdVlanCheckState", 0);	
	
    while(1)
    {		
		for(vid_index =0; vid_index<=4095; vid_index++)
		{
			/* log the old state of vlan */
			vid = vid_index + 1;
			old_state = g_vlanlist[vid_index].updown;
			new_state = g_vlanlist[vid_index].updown;   
			/* Check if the vlan valid */
            if(VLAN_VALID == g_vlanlist[vid_index].vlanStat)
            {
				/*save old state for ve-subintf UP/DOWN*/
				old_vlan_state[vid_index] = g_vlanlist[vid_index].updown;
			
				/* check untag port in this vlan */
    			count_up = 0;
    			for(i = 0; i < CHASSIS_SLOT_COUNT; i++ )
    			{
        			for(j = 0; j < 64; j++ )
        			{				
    					if(j<32)
    					{
                            if((g_vlanlist[vid_index].untagPortBmp[i].low_bmp)&(1<<j))
                        	{	
								if(((start_fp[i][j].attr_bitmap >>1) & 0x1) == 0x1)  /* port is up */
								{								
									count_up++;
									break;  /* just one port is up, then OK  */
								}						
                        	}
        					else
        					{
        						continue;
        					}
    					}
    					else
    					{
                            if((g_vlanlist[vid_index].untagPortBmp[i].high_bmp)&(1<<(j-32)))
                        	{
								if(((start_fp[i][j].attr_bitmap >>1)& 0x1) == 0x1)  /* port is up */
								{
									count_up++; 
									break;  /* just one port is up, then OK  */
 
								}
                        	}
        					else
        					{
        						continue;
        					}								
    					}
        			}				
    				usleep(10);
    			}

				/* check tag port in this vlan */
    			for(i = 0; i < CHASSIS_SLOT_COUNT; i++ )
    			{
        			for(j = 0; j < 64; j++ )
        			{
    					if(j<32)
    					{
                            if((g_vlanlist[vid_index].tagPortBmp[i].low_bmp)&(1<<j))
                        	{
								if(((start_fp[i][j].attr_bitmap >>1) & 0x1) == 1)  /* port is up */
								{
									count_up++;
									break;  /* just one port is up, then OK  */
 
										
								}						
                        	}
        					else
        					{
        						continue;
        					}
    					}
    					else
    					{
                            if((g_vlanlist[vid_index].tagPortBmp[i].high_bmp)&(1<<(j-32)))
                        	{
								if(((start_fp[i][j].attr_bitmap >>1) & 0x1) == 1)  /* port is up */
								{									
									count_up++;
									break;  /* just one port is up, then OK  */
 
									
								}
                        	}
        					else
        					{
        						continue;
        					}								
    					}
        			}
					usleep(10);
    			}

				/*check trunk  state */
				for(i=0;i<127;i++)/*check trunk tag state*/
				{
					if(g_vlanlist[vid_index].tagTrkBmp[i])/*if vlan has tag trunk*/
					{
						if(g_dst_trunk[i].tLinkState)/*if trunk state is up*/
						{
							count_up++;
						}
					}
					else
					{
						continue;
					}
				}

				for(i=0;i<127;i++)/*check trunk untag state*/
				{
					if(g_vlanlist[vid_index].untagTrkBmp[i])/*if vlan has untag trunk*/
					{
						if(g_dst_trunk[i].tLinkState)/*if trunk state is up*/
						{
							count_up++;
						}
					}
					else
					{
						continue;
					}
				}
				
				if(count_up > 0)
				{
				    new_state = VLAN_UP;
				}
				else
				{
				    new_state = VLAN_DOWN;
				}
            }
			else
			{
				continue;
			}

			/* If state changed, do the follow action */
			if(new_state != old_state)
			{
                syslog_ax_vlan_dbg("==============================\n");				
                syslog_ax_vlan_dbg("vlan%d change state: %s \n",vid,new_state ? "UP":"DOWN");
				/*vlan state changed, if not changed ,do not entry in port UP/DOWN function*/
				vlan_state_changed = 1;
				/* 1. update the g_vlanlist[] for display */
				g_vlanlist[vid_index].updown = new_state;
                ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
            	if( ret != 0 )
                {
                    syslog_ax_vlan_err("msync shm_vlan error ! \n" );
					continue;
                }
				else
				{
                    syslog_ax_vlan_dbg("update g_vlanlist[] OK \n" );
				}
				
				/* 2. UP/DOWN L3 intf vlan */
                if((TRUE == npd_vlan_interface_check(vid, &ifIndex)) && (ifIndex != ~0UI))
			    {
    				if( new_state == VLAN_UP)
                    {
						ret = npd_intf_set_vlan_l3intf_status(vid, VLAN_STATE_UP_E);		
                        if(ret != 0)
                        {
                            syslog_ax_vlan_err(" Set vlan%d L3 state UP error !\n",vid);
							continue;
                        }
						else
						{
                            syslog_ax_vlan_dbg(" Set vlan%d L3 state UP OK \n",vid);
						}
                    }
    				else
    				{
						ret = npd_intf_set_vlan_l3intf_status(vid, VLAN_STATE_DOWN_E);		
                        if(ret != 0)
                        {
                            syslog_ax_vlan_err(" Set vlan%d L3 state DOWN error !\n",vid);
							continue;
                        }	
						else
						{
                            syslog_ax_vlan_dbg(" Set vlan%d L3 state DOWN OK \n",vid);
						}
    				}							
			    }
			}
			usleep(200);
 		}
		/* 3. UP/DOWN sub intf of ve, only on Active-MCB */
		if(vlan_state_changed == 1)
		{
			if(1 == IS_ACTIVE_MASTER_NPD)/* if local board is Active-MCB  && vlan state changed*/
			{
				ret = check_ve_port_status(old_vlan_state);
				if(ret != 0)
				{
					syslog_ax_vlan_err("set ve subintf or qinq intf failed\n");
				}
			}
			else
			{
				syslog_ax_vlan_dbg("I am not Active-MCB,do nothing \n");
			}
			vlan_state_changed = 0;
			syslog_ax_vlan_dbg("==============================\n");
		}
		/* Wait for a while */
		sleep(1);
    }
	
}

/* Init the g_vlanlist[] of other board on SMU, because it should init the default vlan of other board */
int npd_get_default_vlan_smu(void)
{
	int i = 0, slot =0,ret =0;
	int asic_port_start_no = 0;
	int asic_port_cnt = 0;
	int slot_cnt = 0;
	char path_start_no[64];
	char path_cnt[64];
	
	for (slot = 1; slot <= CHASSIS_SLOT_COUNT; slot++ )
	{
		sprintf(path_start_no, "/dbm/product/slot/slot%d/asic_start_no", slot);
		sprintf(path_cnt, "/dbm/product/slot/slot%d/asic_port_num", slot);

		asic_port_start_no = get_num_from_file(path_start_no);
		asic_port_cnt = get_num_from_file(path_cnt);
        if((asic_port_start_no<0)||(asic_port_start_no>64))	/* code optimize:Same on both sides houxx@autelan.com  2013-1-17 */
        {
			syslog_ax_board_dbg("get_num_from_file of slot %d return -1 !\n",slot);
			continue;
        }
		
    	for (i= asic_port_start_no; i <= (asic_port_cnt + asic_port_start_no -1); i++ )
    	{
			if(i<=32)
			{
    		    g_vlanlist[0].untagPortBmp[slot-1].low_bmp |= (unsigned int)(1<<(i-1));
			}
			else
			{
    		    g_vlanlist[0].untagPortBmp[slot-1].high_bmp |= (unsigned int)(1<<(i-33));				
			}
    	}		
	}
    ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( ret != 0 )
    {
        syslog_ax_board_err("msync shm_vlan failed \n" );
		return VLAN_RETURN_CODE_ERR_GENERAL;
    }	
    syslog_ax_board_dbg("init g_vlanlist[] OK on SMU !\n");	
    return 0;
}


unsigned int npd_vlan_cscd_add(unsigned short vlanId)
{
    unsigned int ret = 0;
	int i = 0;
	int isTagged =1;
	int eth_g_index= 0,devnum=0,portnum =0;
	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
	{
	   	ret = nam_asic_vlan_entry_ports_add(0, vlanId, 25, isTagged);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add cscd port to vlan %d error\n",vlanId);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
	    }

	   	ret = nam_asic_vlan_entry_ports_add(0, vlanId, 26, isTagged);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add cscd port to vlan %d error\n",vlanId);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
	    }
		#if 0  /* do this in 'bond vlan vid to slot N' */
	   	ret = nam_asic_vlan_entry_ports_add(1, vlanId, 26, isTagged);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add cscd port to vlan %d error\n",vlanId);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
	    }
		#endif
	   	ret = nam_asic_vlan_entry_ports_add(1, vlanId, 27, isTagged);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add cscd port to vlan %d error\n",vlanId);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
	    }
		
	}
	else
	{
        for(i=0; i<asic_board->asic_cscd_port_cnt; i++)
        {
        	devnum = asic_board->asic_cscd_ports[i].dev_num;
        	portnum = asic_board->asic_cscd_ports[i].port_num;
			
        	ret = nam_asic_vlan_entry_ports_add(devnum, vlanId, portnum, isTagged);
            if(0 != ret)
			{
        		syslog_ax_vlan_err("nam add cscd port (%d,%d) to vlan %d error\n",devnum, portnum,vlanId);
        		return VLAN_RETURN_CODE_ERR_GENERAL;
    	    }
       		syslog_ax_vlan_dbg("nam add cscd port (%d,%d) to vlan %d OK !\n",devnum, portnum,vlanId);

			/* when rx packet, npd_vlan_check_contain_port() need this */
			if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)    /* just for L3-vlan on AX81-SMU */
			{
        		/* update g_vlan[]->tag for L3-vlan */
        		/* TODO:add port to vlan tagged port list*/
                npd_get_global_index_by_devport(devnum, portnum, &eth_g_index);			
        		ret = npd_vlan_port_list_addto(vlanId,eth_g_index,isTagged);
        		if(NPD_TRUE != ret)
        		{
            		syslog_ax_vlan_err("npd add cscd port (%d,%d) to vlan %d error\n",devnum, portnum,vlanId);				
            		return VLAN_RETURN_CODE_ERR_GENERAL;
        		}
    	   		syslog_ax_vlan_dbg("npd add cscd port (%d,%d) to vlan %d OK !\n",devnum, portnum,vlanId);				
			}
		}		
	}
    
	return VLAN_RETURN_CODE_ERR_NONE;
}

unsigned int npd_vlan_qinq_tpid_entry_set(unsigned char devNum,unsigned short etherType)
{
	int ret = NPD_FAIL;
	int ingress =1,egress = 1;
	unsigned short get_etherType = 0;


	syslog_ax_vlan_dbg("etherType = 0x%x\n",etherType);
	if(etherType == 0x8100)
	{
		return VLAN_RETURN_CODE_ERR_NONE;
	}
	for(ingress;ingress <8;ingress++)
	{
		ret = nam_vlan_qinq_tpid_entry_get(devNum,0,ingress,&get_etherType);
		syslog_ax_vlan_dbg("get_etherType = 0x%x\n",get_etherType);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_err("nam_vlan_qinq_tpid_entry_get err \n");
			return ret;
		}

		if(get_etherType == 0x8100)
		{
			break;
		}
	}
	nam_syslog_dbg("ingress = %d\n",ingress);
	ret = nam_vlan_qinq_tpid_entry_set(devNum,0,ingress,etherType);

	if(ret != VLAN_RETURN_CODE_ERR_NONE)
	{
		syslog_ax_vlan_err("nam_vlan_qinq_tpid_entry_set err \n");
		return ret;
	}

	for(egress;egress <8;egress++)
	{
		ret = nam_vlan_qinq_tpid_entry_get(devNum,1,egress,&get_etherType);
		syslog_ax_vlan_dbg("get_etherType = 0x%x\n",get_etherType);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_err("nam_vlan_qinq_tpid_entry_get err \n");
			return ret;
		}

		if(get_etherType == 0x8100)
		{
			break;
		}
	}
	ret = nam_vlan_qinq_tpid_entry_set(devNum,1,egress,etherType);

	if(ret != VLAN_RETURN_CODE_ERR_NONE)
	{
		syslog_ax_vlan_err("nam_vlan_qinq_tpid_entry_set err \n");
		return ret;
	}

	return ret;
}

unsigned int npd_vlan_qinq_ingress_port_tpid_set(unsigned char devNum,unsigned char portNum,unsigned short outer_etherType,unsigned short inner_etherType)
{
	int ret = NPD_FAIL;
	unsigned long outer_tpidBmp = 0,inner_tpidBmp = 0;
	int ingress =1,egress = 1;
	unsigned short get_etherType = 0;
	if(outer_etherType == 0x8100)
	{
		outer_tpidBmp = 0x1;
	}
	for(ingress;ingress <8;ingress++)
	{
		ret = nam_vlan_qinq_tpid_entry_get(0,0,ingress,&get_etherType);
		syslog_ax_vlan_dbg("get_etherType = 0x%x\n",get_etherType);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_err("nam_vlan_qinq_tpid_entry_get err \n");
			return ret;
		}

		if(get_etherType == outer_etherType)
		{
			outer_tpidBmp = 0x1 << ingress;
			break;
		}
	}
	if(outer_tpidBmp == 0)
	{
		syslog_ax_vlan_err("can't find the outer ethertype please check out !");
		return NPD_FAIL;
	}
	if(inner_etherType == 0x8100)
	{
		inner_tpidBmp = 0x1;
	}
	for(egress;egress <8;egress++)
	{
		ret = nam_vlan_qinq_tpid_entry_get(0,0,egress,&get_etherType);
		syslog_ax_vlan_dbg("get_etherType = 0x%x\n",get_etherType);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_err("nam_vlan_qinq_tpid_entry_get err \n");
			return ret;
		}

		if(get_etherType == inner_etherType)
		{
			inner_tpidBmp = 0x1 << egress;
			break;
		}
	}
	if(inner_tpidBmp == 0)
	{
		syslog_ax_vlan_err("can't find the inner ethertype please check out !");
		return NPD_FAIL;
	}
	ret = nam_vlan_qinq_port_ingress_tpid_set(devNum,portNum,1,outer_tpidBmp);
	if(ret != COMMON_SUCCESS)
	{
		syslog_ax_vlan_err("set vlan qinq port ingress tpid err !\n");
		return ret;
	}
	ret = nam_vlan_qinq_port_ingress_tpid_set(devNum,portNum,2,inner_tpidBmp);
	if(ret != COMMON_SUCCESS)
	{
		syslog_ax_vlan_err("set vlan qinq port ingress tpid err !\n");
		return ret;
	}

	return ret;
}


unsigned int npd_vlan_qinq_egress_port_tpid_set(unsigned char devNum,unsigned char portNum,unsigned short outer_etherType,unsigned short inner_etherType)
{
	int ret = NPD_FAIL;
	unsigned long outer_tpidentryIndex = 0,inner_tpidentryIndex = 0;
	int ingress =1,egress = 1;
	unsigned short get_etherType = 0;
	if(outer_etherType == 0x8100)
	{
		outer_tpidentryIndex = 0;
	}
	for(ingress;ingress <8;ingress++)
	{
		ret = nam_vlan_qinq_tpid_entry_get(0,0,ingress,&get_etherType);
		syslog_ax_vlan_dbg("get_etherType = 0x%x\n",get_etherType);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_err("nam_vlan_qinq_tpid_entry_get err \n");
			return ret;
		}

		if(get_etherType == outer_etherType)
		{
			outer_tpidentryIndex = ingress;
			break;
		}
	}
	if(outer_tpidentryIndex == 0)
	{
		syslog_ax_vlan_err("can't find the outer ethertype please check out !");
		return NPD_FAIL;
	}
	if(inner_etherType == 0x8100)
	{
		inner_tpidentryIndex = 0;
	}
	for(egress;egress <8;egress++)
	{
		ret = nam_vlan_qinq_tpid_entry_get(0,0,egress,&get_etherType);
		syslog_ax_vlan_dbg("get_etherType = 0x%x\n",get_etherType);
		if(ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_err("nam_vlan_qinq_tpid_entry_get err \n");
			return ret;
		}

		if(get_etherType == inner_etherType)
		{
			inner_tpidentryIndex = egress;
			break;
		}
	}
	if(inner_tpidentryIndex == 0)
	{
		syslog_ax_vlan_err("can't find the inner ethertype please check out !");
		return NPD_FAIL;
	}
	ret = nam_vlan_qinq_port_egress_tpid_set(devNum,portNum,1,outer_tpidentryIndex);
	if(ret != COMMON_SUCCESS)
	{
		syslog_ax_vlan_err("set vlan qinq port ingress tpid err !\n");
		return ret;
	}
	ret = nam_vlan_qinq_port_egress_tpid_set(devNum,portNum,2,inner_tpidentryIndex);
	if(ret != COMMON_SUCCESS)
	{
		syslog_ax_vlan_err("set vlan qinq port ingress tpid err !\n");
		return ret;
	}

	return ret;
}


unsigned int npd_vlan_cscd_del(unsigned short vlanId)
{
    unsigned int ret = 0;
	int i = 0;
	int needJoin =0; /* need not add the port to vlan 1,it always in vlan1 */
	int eth_g_index= 0,devnum=0,portnum =0;

	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
	{

		/*delete this(these) port(s) from this vlan*/
		ret = nam_asic_vlan_entry_ports_del(vlanId,0,25,needJoin);
		if(0 != ret) {
			syslog_ax_vlan_err("delete vlan %d tag port %d error ret %d.\n",vlanId,25,ret);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
		}
		ret = nam_asic_vlan_entry_ports_del(vlanId,0,26,needJoin);
		if(0 != ret) {
			syslog_ax_vlan_err("delete vlan %d tag port %d error ret %d.\n",vlanId,26,ret);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
		}
		#if 0
		ret = nam_asic_vlan_entry_ports_del(vlanId,1,26,needJoin);
		if(0 != ret) {
			syslog_ax_vlan_err("delete vlan %d tag port %d error ret %d.\n",vlanId,26,ret);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
		}
		#endif
		ret = nam_asic_vlan_entry_ports_del(vlanId,1,27,needJoin);
		if(0 != ret) {
			syslog_ax_vlan_err("delete vlan %d tag port %d error ret %d.\n",vlanId,27,ret);
    		return VLAN_RETURN_CODE_ERR_GENERAL;
		}
		
	}
	else
	{
        for(i=0; i< asic_board->asic_cscd_port_cnt; i++)
        {

        	devnum = asic_board->asic_cscd_ports[i].dev_num;
        	portnum = asic_board->asic_cscd_ports[i].port_num;
					
        	ret = nam_asic_vlan_entry_ports_del(vlanId, devnum , portnum, needJoin);
            if(0 != ret)
			{
        		syslog_ax_vlan_err("asic_board delete cscd port from vlan %d error\n",vlanId);
        		return VLAN_RETURN_CODE_ERR_GENERAL;
    	    }
		    syslog_ax_vlan_dbg("nam asic vlan entry ports delete: vlanId %d, devNum %d,PortNum %d ret %d.\n",\
    						    vlanId,devnum,portnum,ret);

			#if 0
			/* here is not needed, for the g_vlan[] has been set to NULL, */
			/* when called npd_delete_vlan_by_vid(vlanId) in npd_vlan_interface_destroy_node() */
            /* update g_vlan for RX TX packet on AX81-SMU */
			if(BOARD_TYPE == BOARD_TYPE_AX81_SMU)    /* just for L3-vlan on AX81-SMU */
			{
    			/* TODO:delete port from vlan tagged port list*/
                npd_get_global_index_by_devport(devnum, portnum, &eth_g_index);						
    			ret = npd_vlan_port_list_delfrom(vlanId,eth_g_index,NPD_TRUE);
        		if(NPD_TRUE != ret)
        		{
            		syslog_ax_vlan_err("npd del cscd port (%d,%d) from vlan %d error\n",devnum, portnum,vlanId);				
            		return VLAN_RETURN_CODE_ERR_GENERAL;
        		}
    	   		syslog_ax_vlan_dbg("npd del cscd port (%d,%d) from vlan %d OK !\n",devnum, portnum,vlanId);				
				
			}
			#endif
		}
	}
    
	return VLAN_RETURN_CODE_ERR_NONE;
}

unsigned int npd_vlanlist_valid(unsigned short vlanId,char *vlanName)
{
    int ret =0;
	g_vlanlist[vlanId-1].vlanStat = 1;
	/* memcpy(g_vlanlist[vlanId-1].vlanName , vlanName ,NPD_VLAN_IFNAME_SIZE);  */  /* vlanName is 12 chars */
	strcpy(g_vlanlist[vlanId-1].vlanName ,vlanName);		/* code optimize: Out-of-bounds access  houxx@autelan.com  2013-1-18 */
	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( ret!=0 )
    {
        syslog_ax_vlan_err("msync shm_vlan failed \n" );
		return VLAN_RETURN_CODE_ERR_GENERAL;
    }	
	syslog_ax_vlan_dbg("set vlan %d valid ok\n",vlanId);	
	return ret;
}
unsigned int npd_vlanlist_invalid(unsigned short vlanId)
{
    int ret =0;
	int slot=0,i =0;
    /* update the default-vlan of g_vlanlist[] after delete vlan */
	for (slot= 0; slot < CHASSIS_SLOT_COUNT; slot++ )
	{
    	for (i= 0; i < 64; i++ )
    	{
    		if(i<=31)
    		{
    		    if(((g_vlanlist[vlanId-1].untagPortBmp[slot].low_bmp)>>i)&0x1)
    		    {
    				g_vlanlist[0].untagPortBmp[slot].low_bmp |= (unsigned int)(0x1<<i);		/* add to default vlan */		
                	syslog_ax_vlan_dbg("add vlan %d untag port %d-%d to default-vlan ok\n",vlanId,slot+1,i+1);	
				}
    		}
    		else
    		{
    		    if(((g_vlanlist[vlanId-1].untagPortBmp[slot].high_bmp)>>(i-32))&0x1)
    		    {
    				g_vlanlist[0].untagPortBmp[slot].high_bmp |= (unsigned int)(0x1<<(i-32));		/* add to default vlan */		
                	syslog_ax_vlan_dbg("add vlan %d untag port %d-%d to default-vlan ok\n",vlanId,slot+1,i+1);	
				}				
    		}
    	}
	}
	
	memset(&(g_vlanlist[vlanId-1]),0,sizeof(vlan_list_t));
	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( ret!=0 )
    {
        syslog_ax_vlan_err("msync shm_vlan failed \n" ); 
		return VLAN_RETURN_CODE_ERR_GENERAL;
    }		
	syslog_ax_vlan_dbg("set vlan %d invalid ok\n",vlanId);	
	return ret;
}

/* update g_vlanlist[] after add of delete trunk */	
unsigned int npd_vlanlist_add_del_trunk(unsigned short vlanId,unsigned short trunkId,unsigned char isAdd,unsigned char isTag)
{
    int ret =0,i=0;
	unsigned char slot_no =0,local_port_no = 0;
    if(1== isAdd)
    {
		if(1==isTag)
		{
            g_vlanlist[vlanId-1].tagTrkBmp[trunkId-1] = 1;		
		}
		else
		{
            g_vlanlist[vlanId-1].untagTrkBmp[trunkId-1] = 1;			
			if(1 != vlanId)
			{
                g_vlanlist[0].untagTrkBmp[trunkId-1] = 0;
			}
		}
    }
	else
	{
		if(1==isTag)
		{
            g_vlanlist[vlanId-1].tagTrkBmp[trunkId-1] = 0;		
		}
		else
		{
            g_vlanlist[vlanId-1].untagTrkBmp[trunkId-1] = 0;
			if(1 != vlanId)
			{
                g_vlanlist[0].untagTrkBmp[trunkId-1] = 1;
			}			
		}		
	}

	for(i=0;i<g_dst_trunk[trunkId-1].portLog;i++)
	{
		slot_no = g_dst_trunk[trunkId-1].portmap[i].slot;
		local_port_no = g_dst_trunk[trunkId-1].portmap[i].port;

		/* add for update the g_vlanlist[] of distributed */
	    if(isAdd==1)
	    {   
			
	        /* remove the port from vlanId */
			if(g_vlanlist[vlanId-1].vlanStat == 1)
    		{			
                if(local_port_no>32)
                {
        			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));		/* remove from all vlan */		
        			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));			
                }
        		else
        		{
        			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));		/* remove from all vlan */			
        			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
        		}
            }
	    }
		else
		{

	        if(local_port_no>32)
	        {
				g_vlanlist[0].untagPortBmp[slot_no-1].high_bmp |= (unsigned int)((1<<(local_port_no-33)));		/* add to default vlan  */		
	        }
			else
			{
		        g_vlanlist[0].untagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));		/* add to default vlan */	
			}

			if(isTag == 1)
			{
				/*update trunk qinq state after the vlan del trunk*/
				if(local_port_no > 32)
				{
					if((g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp)&(1<<(local_port_no-33)))
					{
						g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
					}
				}
				else
				{
					if((g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp)&(1<<(local_port_no-1)))
					{
						g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
					}
				}
			}
		}

	}
	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( ret!=0 )
    {
        syslog_ax_vlan_err("msync shm_vlan failed \n" );
		ret = TRUNK_RETURN_CODE_ERR_HW;
    }
	else
	{
		ret = TRUNK_RETURN_CODE_ERR_NONE;
	}
	syslog_ax_vlan_dbg("%s trunk %d %s vlan %d ok\n",isAdd ? "add":"delete",trunkId,isTag ? "tag":"untag",vlanId);	
	return ret;
}


/* update g_vlan_list[4096] when add/delete port on SMU */
DBusMessage *npd_dbus_vlan_config_vlanlist_port_add_del
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	isAdd = 0, isTagged = 0;
	unsigned char	slot_no = 0, local_port_no = 0, slot_index =0;
	unsigned short	vlanId = 0;	

	unsigned int	port_index = 0, eth_g_index = 0;
	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;
	struct eth_port_s	*ethPort = NULL;	
	enum product_id_e productId = PRODUCT_ID;
	/*enum module_id_e module_type;*/
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isAdd,
							DBUS_TYPE_BYTE,&slot_no,
							DBUS_TYPE_BYTE,&local_port_no,
							DBUS_TYPE_BYTE,&isTagged,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	
	/*zhangcl add for update g_static_fdb when delete port from the vlan*/
	ret = npd_fdb_static_entry_del_with_port_vlan(slot_no,local_port_no,vlanId);
	if(ret == NPD_FDB_ERR_NODE_NOT_EXIST)
	{
		syslog_ax_vlan_dbg("did not find static fdb item \n");
	}
	else if(ret != VLAN_RETURN_CODE_ERR_NONE)
	{
		syslog_ax_vlan_err("delete static fdb for port %d-%d,vlan %d Failed !\n",slot_no,local_port_no,vlanId);
	}
	 syslog_ax_vlan_dbg("g_vlanlist:%s port %d/%d %s %s vlan %d\n",(isAdd==1)?"add":"delete", 
			slot_no,local_port_no,(isTagged==1)?"tagged":"untagged",(isAdd==1)?"to":"from",vlanId);
    if(isAdd==1)
    {
        if(local_port_no>32)
        {
    		if(isTagged ==1)
    		{
    			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].high_bmp |= (unsigned int)(1<<(local_port_no-33));
    		}
    		else
    		{
    			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].high_bmp |= (unsigned int)(1<<(local_port_no-33));
				g_vlanlist[0].untagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));		/* remove from default vlan */		
    		}      					
        }
		else
		{
    		if(isTagged ==1)
    		{
    			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));
    		}
    		else
    		{
    			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));
				g_vlanlist[0].untagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));		/* remove from default vlan */		
    		}      					
			
		}    
    }
	else
	{

        if(local_port_no>32)
        {
    		if(isTagged ==1)
    		{
				if((g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp)&(1<<(local_port_no-33)))/*disable port qinq*/
				{
					g_vlanlist[vlanId-1].qinq[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
				}
    			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
    		}
    		else
    		{
    			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].high_bmp &= (unsigned int)(~(1<<(local_port_no-33)));
				g_vlanlist[0].untagPortBmp[slot_no-1].high_bmp |= (unsigned int)((1<<(local_port_no-33)));		/* add to default vlan  */		

    		}      					
        }
		else
		{
    		if(isTagged ==1)
    		{
				if((g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp)&(1<<(local_port_no-1)))/*disable port qinq*/
				{
					g_vlanlist[vlanId-1].qinq[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
				}
    			g_vlanlist[vlanId-1].tagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
    		}
    		else
    		{
    			g_vlanlist[vlanId-1].untagPortBmp[slot_no-1].low_bmp &= (unsigned int)(~(1<<(local_port_no-1)));
				g_vlanlist[0].untagPortBmp[slot_no-1].low_bmp |= (unsigned int)(1<<(local_port_no-1));		/* add to default vlan */		
    		}      					
			
		}
	}

	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( ret!=0 )
    {
        syslog_ax_vlan_err("msync shm_vlan failed \n" );   
    }
    /* we do not use this for display vlan on standby-MCB,zhangdi 20110803 */
	#if 0
    /* make the file on Active & Standby sync */
    ret = npd_asic_vlan_sync(0,0);
	if(0 != ret)	
    {
		 syslog_ax_vlan_err("npd send msg: sync vlan file error ");
		 ret = VLAN_RETURN_CODE_ERR_GENERAL; 
    }
	#endif
	else
	{
		ret = VLAN_RETURN_CODE_ERR_NONE;
	}
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}
/* read g_vlanlist[4096] to show vlan port-member list */
/* change to show with file "/dbm/shm/vlan/shm_vlan", so need not this */
DBusMessage * npd_dbus_vlan_show_vlanlist_port_member_distributed
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
) 
{
	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned int ret = VLAN_RETURN_CODE_ERR_NONE;
	
	void* mem;
    int shmid;
	struct shmid_ds shmds;   
	int mem_size = 4096*sizeof(vlan_list_t);

    syslog_ax_vlan_dbg( "share mem_size:%d",mem_size);
	
	dbus_error_init(&err);	

	/* create share mem */
    shmid=shmget(MY_SHM_VLANLIST_ID, mem_size, 0666|IPC_CREAT );  
    if( shmid<0 )
    {
        syslog_ax_vlan_dbg( "Create a shared memory error !\n" );
		return NULL;		
    }
	syslog_ax_vlan_dbg( "shmid:%d\n",shmid );
    ret = shmctl( shmid,IPC_STAT,&shmds );   
    if(ret == 0)   
    {   
        syslog_ax_vlan_dbg( "Size of memory segment is %d\n",shmds.shm_segsz );   
        syslog_ax_vlan_dbg( "Numbre of attaches %d\n",( int )shmds.shm_nattch );   
    }   
    else   
    {   
        syslog_ax_vlan_dbg( "shmctl(  ) call failed\n" );   
    }

	mem= shmat(shmid, 0, 0); 
    if( (int)mem != -1 )
    {
        syslog_ax_vlan_dbg( "Shared memory was attached in our address space at %p\n",mem ); 
    	memcpy(mem, g_vlanlist ,mem_size);
    }
	else
	{
		syslog_ax_vlan_dbg( "shmat() failed\n" ); 
	}
	
	
	ret = VLAN_RETURN_CODE_ERR_NONE;
	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
	
}

unsigned int npd_vlan_to_cpu_port_add(unsigned short vlanId,unsigned short cpu_no,unsigned short cpu_port_no)
{
    unsigned int ret = 0;
	int i = 0;
	int istag =1;
	int dev=0, port=0;
	
	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
	{
        /* check if the cpu port is exist, only cpu 0 port 0 on AX71-CRSMU */
    	if((cpu_no!=0)||(cpu_port_no!=0))
    	{
    		/* the cpu port is not exist */
    	    return 	VLAN_RETURN_CODE_ERR_HW;
    	}		
	   	ret = nam_asic_vlan_entry_ports_add(1, vlanId, 26, istag);
        if(0 != ret)
		{
    		syslog_ax_vlan_err("ax71-smu add cscd port to vlan %d error\n",vlanId);
    		return VLAN_RETURN_CODE_VLAN_BOND_ERR;
	    }	
	}
    else
    {
    	dev = asic_board->asic_to_cpu_ports[8*cpu_no+cpu_port_no].dev_num;
    	port = asic_board->asic_to_cpu_ports[8*cpu_no+cpu_port_no].port_num;				
		
        /* check if the cpu port is exist */
    	if((dev<0)||(port<0))
    	{
    		/* the cpu port is not exist */
    	    return 	VLAN_RETURN_CODE_ERR_HW;
    	}
        /* if is ax81-1x12g12s we must add the other trunk port (0,26) */
    	if(BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S)
    	{
    		ret = nam_asic_vlan_entry_ports_add(dev, vlanId, port, istag);
            if(0 != ret)
    		{
        		syslog_ax_vlan_err("add CPU port (%d,%d) istag % to vlan %d error\n",dev,port,istag,vlanId);
        		return VLAN_RETURN_CODE_VLAN_BOND_ERR;
    	    }
       		syslog_ax_vlan_dbg("add CPU port (%d,%d) istag % to vlan %d OK\n",dev,port,istag,vlanId);
    		ret = nam_asic_vlan_entry_ports_add(0, vlanId, 26, istag);
            if(0 != ret)
    		{
        		syslog_ax_vlan_err("add CPU port to vlan %d error\n",vlanId);
        		return VLAN_RETURN_CODE_VLAN_BOND_ERR;
    	    }
       		syslog_ax_vlan_dbg("add CPU port (%d,%d) istag % to vlan %d OK\n",dev,port,istag,vlanId);
    	}
    	else
    	{
        	ret = nam_asic_vlan_entry_ports_add(dev, vlanId, port, istag);
            if(0 != ret)
    		{
        		syslog_ax_vlan_err("add CPU port to vlan %d error\n",vlanId);
        		return VLAN_RETURN_CODE_VLAN_BOND_ERR;
    	    }
       		syslog_ax_vlan_dbg("add CPU port (%d,%d) istag % to vlan %d OK\n",dev,port,istag,vlanId);
        }		
    }   
	return VLAN_RETURN_CODE_ERR_NONE;
}

unsigned int npd_vlan_to_cpu_port_del(unsigned short vlanId,unsigned short cpu_no,unsigned short cpu_port_no)
{
    unsigned int ret = 0;
	int i = 0;
	int needJoin =0; /* need not add the port to vlan 1 */
	int dev=0,port=0;
	if(BOARD_TYPE == BOARD_TYPE_AX71_CRSMU)
	{
		/*delete this(these) port(s) from this vlan*/
		ret = nam_asic_vlan_entry_ports_del(vlanId,1,26,needJoin);
		if(0 != ret) {
			syslog_ax_vlan_err("delete vlan %d tag dev %d port %d error ret %d.\n",1,vlanId,26,ret);
    		return VLAN_RETURN_CODE_VLAN_UNBOND_ERR;
		}		
	}
	else
	{
    	dev = asic_board->asic_to_cpu_ports[8*cpu_no+cpu_port_no].dev_num;
    	port = asic_board->asic_to_cpu_ports[8*cpu_no+cpu_port_no].port_num;				
        /* check if the cpu port is exist */
    	if((dev<0)||(port<0))
    	{
    		/* the cpu port is not exist */
    	    return 	VLAN_RETURN_CODE_ERR_HW;
    	}
        /* if is ax81-1x12g12s we must add the other trunk port (0,26) */
    	if(BOARD_TYPE == BOARD_TYPE_AX81_1X12G12S)
    	{
    		ret = nam_asic_vlan_entry_ports_del(vlanId, dev, port, needJoin);
            if(0 != ret)
    		{
        		syslog_ax_vlan_err("nam delete port(%d,%d) from vlan %d error\n",dev,port,vlanId);
        		return VLAN_RETURN_CODE_VLAN_UNBOND_ERR;
    	    }
       		syslog_ax_vlan_dbg("nam delete port(%d,%d) from vlan %d error\n",dev,port,vlanId);
    		ret = nam_asic_vlan_entry_ports_del(vlanId, 0, 26, needJoin);
            if(0 != ret)
    		{
        		syslog_ax_vlan_err("nam delete port(%d,%d) from vlan %d error\n",0,26,vlanId);
        		return VLAN_RETURN_CODE_VLAN_UNBOND_ERR;
    	    }
       		syslog_ax_vlan_dbg("nam delete port(%d,%d) from vlan %d OK\n",dev,port,vlanId);
    	}
		else
		{
            ret = nam_asic_vlan_entry_ports_del(vlanId, dev, port, needJoin);
            if(0 != ret)
    		{
        		syslog_ax_vlan_err("nam delete port(%d,%d) from vlan %d error\n",dev,port,vlanId);
        		return VLAN_RETURN_CODE_VLAN_UNBOND_ERR;
    	    }
       		syslog_ax_vlan_dbg("nam delete port(%d,%d) from vlan %d OK\n",dev,port,vlanId);			
		}
	}    
	return VLAN_RETURN_CODE_ERR_NONE;
}

DBusMessage *npd_dbus_vlan_to_cpu_port_add_del
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned char	isbond = 0;
	unsigned short	vlanId = 0;	
	unsigned short	slotId = 0;	
	unsigned short	cpu_no = 0;	
	unsigned short	cpu_port_no = 0;	
	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;
	char vename[21] = {0};
	int is_active_master = 0;
	unsigned int ifIndex = ~0UI;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_BYTE,&isbond,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_UINT16,&slotId,
        					DBUS_TYPE_UINT16,&cpu_no,
        					DBUS_TYPE_UINT16,&cpu_port_no,							
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}

	if(slotId == SLOT_ID)  /* is on local board */
	{
        if(g_vlanlist[vlanId-1].vlanStat == 0)
        {
    	    syslog_ax_vlan_err("vlan %d is not exists ");	
    		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
        }
    	else
    	{
        	syslog_ax_vlan_dbg("Real %s vlan %d to slot %d cpu %d port %d \n",(isbond==1)?"bond":"unbond",vlanId,slotId,cpu_no,cpu_port_no);
        	if(1==isbond)       /* is bond */
        	{
				/* if not bonded */
				if(((g_vlanlist[vlanId-1].bond_slot[slotId-1])&(unsigned int)(1<<(8*cpu_no+cpu_port_no)))==0)
				{
           		    ret = npd_vlan_to_cpu_port_add(vlanId,cpu_no,cpu_port_no);
					if(ret==VLAN_RETURN_CODE_ERR_NONE)
					{
					    g_vlanlist[vlanId-1].bond_slot[slotId-1] |= (unsigned int)(1<<(8*cpu_no+cpu_port_no));
					}
				}
				else
				{
    				/* this vlan have already bonded, can not bond */
    				ret = VLAN_RETURN_CODE_VLAN_ALREADY_BOND;					
				}    				
        	}
        	else    /* is unbond */
        	{
				/* if not bonded */
				if(((g_vlanlist[vlanId-1].bond_slot[slotId-1])&(unsigned int)(1<<(8*cpu_no+cpu_port_no)))==0)
				{
    				/* this vlan have not bonded, can not unbond */
    				ret = VLAN_RETURN_CODE_VLAN_NOT_BONDED;				
				}
				else
				{
           		    ret = npd_vlan_to_cpu_port_del(vlanId,cpu_no,cpu_port_no);
					if(ret==VLAN_RETURN_CODE_ERR_NONE)
					{
					    g_vlanlist[vlanId-1].bond_slot[slotId-1] &= ((unsigned int)(~(1<<(8*cpu_no+cpu_port_no))));
					}
                    /* disable qinq state */
					if(((g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1])&(unsigned int)(1<<(8*cpu_no+cpu_port_no))) == 1)
					{
						/*local board if enable cpu port qinq, disabled it*/
						ret = npd_vlan_tocpuport_qinq_endis(vlanId,cpu_no,cpu_port_no,0);
						if(ret != VLAN_RETURN_CODE_ERR_NONE)
						{
							syslog_ax_vlan_err("disable slot %d,cpu_no %d,cpu_port_no %d fail !\n",slotId,cpu_no,cpu_port_no);
						}
						else
						{
							syslog_ax_vlan_err("disable slot %d,cpu_no %d,cpu_port_no %d successfully !\n",slotId,cpu_no,cpu_port_no);
							g_vlanlist[vlanId-1].cpu_port_qinq[slotId-1] &= ((unsigned int)(~(1<<(8*cpu_no+cpu_port_no))));
						}
					}
				}            		
        	}
    	}
	}
	else   /* not my board,only update the vlan_slot[] */
	{
        if(g_vlanlist[vlanId-1].vlanStat == 0)
        {
    	    syslog_ax_vlan_err("vlan %d is not exists on my slot:%d ",slotId);	
    		ret = VLAN_RETURN_CODE_VLAN_NOT_EXISTS;
        }
    	else
    	{
        	syslog_ax_vlan_dbg("update info: %s vlan %d to slot %d cpu %d port %d\n",(isbond==1)?"bond":"unbond",vlanId,slotId,cpu_no,cpu_port_no);
        	if(1==isbond)       /* is bond */
        	{
				g_vlanlist[vlanId-1].bond_slot[slotId-1] |= (unsigned int)(1<<(8*cpu_no+cpu_port_no));    				
        	}
        	else    /* is unbond */
        	{
				g_vlanlist[vlanId-1].bond_slot[slotId-1] &= ((unsigned int)(~(1<<(8*cpu_no+cpu_port_no))));
        	}
    	}
	}

	/* Update the state of ve.vid only on Active-MCB */
	if(ret == VLAN_RETURN_CODE_ERR_NONE)
	{
        if(1==IS_ACTIVE_MASTER_NPD)
        {
        	if(1==isbond)
        	{
            	/* Set the vlan down, then the thread will update the ve.vid state */
                g_vlanlist[vlanId-1].updown = 0;
        	}
        	else
        	{
    			/* set ve.vid down */
				sprintf(vename,"ve%02d%c%d.%d",slotId,((cpu_no+1)==1)?'f':'s',cpu_port_no+1,vlanId);
    			if(0 == npd_intf_ifindex_get_by_ifname(vename, &ifIndex))  /* get ifindex OK */
    		    {
                    if(npd_intf_set_intf_state(vename,0)!= 0)   /* set ve sub intf down */
                    {
                        syslog_ax_vlan_err(" set %s DOWN error !\n",vename);
                    }
    				else
    				{
						ret = VLAN_RETURN_CODE_ERR_NONE;
    				    syslog_ax_vlan_dbg(" set %s DOWN OK !\n",vename);
    				} 
                }
    			else
    			{
    				syslog_ax_vlan_dbg("Can not find interface: %s\n",vename);					
    			}
        	}	
        }
    	else
    	{
    		syslog_ax_vlan_dbg("I am not Active-MCB, do nothing.\n");			
    	}	
    	
    	/* sync mem to file */
    	if(msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC) != 0 )
        {
            syslog_ax_vlan_err("msync shm_vlan failed \n" );
    		ret = VLAN_RETURN_CODE_VLAN_SYNC_ERR;
        }		
	}

	
	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}	
DBusMessage *npd_dbus_vlan_to_cpu_exist_interface_under_vlan_on_slot
(
	DBusConnection *conn, 
	DBusMessage *msg, 
	void *user_data
)
{	
	DBusMessage* reply;
	DBusMessageIter	 iter;
	DBusError err;
	unsigned short	vlanId = 0;	
	unsigned short	slotId = 0;	
	unsigned short 	cpu_no = 0, cpu_port_no = 0;
	unsigned int 	ret = VLAN_RETURN_CODE_ERR_NONE;
	char vename[21] = {0};
	char fors_cpu = 0;
	unsigned int ifIndex = ~0UI;
	
	dbus_error_init(&err);
	
	if (!(dbus_message_get_args ( msg, &err,
							DBUS_TYPE_UINT16,&vlanId,
							DBUS_TYPE_UINT16,&slotId,
						    DBUS_TYPE_UINT16,&cpu_no,
							DBUS_TYPE_UINT16,&cpu_port_no,							
							DBUS_TYPE_INVALID))) {
		 syslog_ax_vlan_err("Unable to get input args ");
		if (dbus_error_is_set(&err)) {
			 syslog_ax_vlan_err("%s raised: %s",err.name,err.message);
			dbus_error_free(&err);
		}
		return NULL;
	}
	/* if not bonded */
	if(((g_vlanlist[vlanId-1].bond_slot[slotId-1])&(unsigned int)(1<<(8*cpu_no+cpu_port_no)))==0)
	{
		ret = VLAN_RETURN_CODE_VLAN_NOT_BONDED;				
	}
    else
    {
		fors_cpu = (cpu_no == 0)?'f':((cpu_no == 1)?'s':0);
    	sprintf(vename, "ve%02d%c%d.%d",slotId,fors_cpu,(cpu_port_no+1),vlanId);
		syslog_ax_vlan_dbg("to check %s interface exist or not.\n",vename);
    	ifIndex = if_nametoindex(vename);
    	if(ifIndex > 0)
    	{
    		ret = VLAN_RETURN_CODE_ERR_NONE;
    		syslog_ax_vlan_dbg("found interface :%s\n",vename);
    	}
    	else
    	{
    		ret = VLAN_RETURN_CODE_ERR_HW;
    		syslog_ax_vlan_dbg("sub-interface is not exist!\n");
    	}
    }

	reply = dbus_message_new_method_return(msg);
	
	dbus_message_iter_init_append (reply, &iter);
	
	dbus_message_iter_append_basic (&iter,
									 DBUS_TYPE_UINT32,
									 &ret);
	return reply;
}
unsigned int npd_vlan_qinq_init(unsigned char devNum)
{
	unsigned int ret = 0;
	
	ret = npd_vlan_qinq_tpid_entry_set(0,0x8100);
	if(ret != NPD_SUCCESS)
	{
		npd_syslog_err("npd_vlan_qinq_tpid_entry_set error !\n");
	}
	ret =npd_vlan_qinq_tpid_entry_set(0,0x88a8);
	if(ret != NPD_SUCCESS)
	{
		npd_syslog_err("npd_vlan_qinq_tpid_entry_set error !\n");
	}
	ret = npd_vlan_qinq_tpid_entry_set(0,0x9100);
	if(ret != NPD_SUCCESS)
	{
		npd_syslog_err("npd_vlan_qinq_tpid_entry_set error !\n");
	}
	return ret;
}

unsigned int npd_vlan_trunk_qinq_disable(unsigned short trunkId,unsigned short vlanId)
{
	int i=0;
	unsigned char slot =0,port = 0,slot_index =0;
	unsigned int  port_index =0,eth_g_index = 0,isEnable = 0;
	unsigned int ret = NPD_FAIL;

	if(g_vlanlist[vlanId-1].tagTrkBmp[trunkId-1] == 1)/*judge if the trunk is a tag member of vlan*/
	{

		for(i=0;i<g_dst_trunk[trunkId-1].portLog;i++)/*find trunk port in the vlan*/
		{
			slot = g_dst_trunk[trunkId-1].portmap[i].slot;
			port = g_dst_trunk[trunkId-1].portmap[i].port;
			if(SLOT_ID == slot)
			{
				slot_index = CHASSIS_SLOT_NO2INDEX(slot);
				port_index = ETH_LOCAL_NO2INDEX(slot_index, port);
				eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,port_index);
				npd_syslog_dbg("SLOT_ID = %d,slot = %d\n",SLOT_ID,slot);
				if((port <= 32)&&((g_vlanlist[vlanId-1].qinq[slot-1].low_bmp)&(1<<(port-1))))/*if trunk port is enable for qinq*/
				{
					/*disable the specific port on the specific slot*/
					ret = npd_vlan_qinq_endis(vlanId, eth_g_index, isEnable);
					if(ret != NPD_SUCCESS)
					{
						npd_syslog_err("set slot %d port %d qinq disable FAIL !",slot,port);
					}
					else
					{
						npd_syslog_dbg("set vlan %d port %d-%d qinq disable SUCCESS\n",vlanId,slot,port);
						g_vlanlist[vlanId-1].qinq[slot-1].low_bmp &= (unsigned int)(~(1<<(port-1)));
					}
					
				}
				else if((port>32) && ((g_vlanlist[vlanId-1].qinq[slot-1].high_bmp)&(1<<(port-33))))
				{
					ret = npd_vlan_qinq_endis(vlanId, eth_g_index, isEnable);
					if(ret != NPD_SUCCESS)
					{
						npd_syslog_err("set slot %d port %d qinq disable FAIL !",slot,port);
					}
					else
					{
						npd_syslog_dbg("set vlan %d port %d-%d qinq disable SUCCESS\n",vlanId,slot,port);
						g_vlanlist[vlanId-1].qinq[slot-1].high_bmp &= (unsigned int)(~(1<<(port-33)));
					}
				}
			}
			else
			{
				continue;
			}
		}

		ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
		if( ret != VLAN_RETURN_CODE_ERR_NONE)
		{
			syslog_ax_vlan_err("msync shm_vlan failed \n" );   
		}
		return ret;
	}

	return VLAN_RETURN_CODE_ERR_NONE;
}

unsigned int npd_vlan_qinq_disable(unsigned short vlanId)
{
	int j=0,k=0,m=0;
	unsigned char slot =0,port = 0,slot_index =0;
	unsigned int  port_index =0,eth_g_index = 0,isEnable = 0;
	unsigned int ret = NPD_FAIL;
	/*j is slot number*/
	for(j = 0; j < CHASSIS_SLOT_COUNT; j++ )
	{
		/*k is port num*/
		for(k = 0; k < 64; k++ )
		{
			if(k<32)
			{
                if((g_vlanlist[vlanId-1].qinq[j].low_bmp)&(1<<k))
            	{
					slot = j+1;
					port = k+1;
					if(SLOT_ID == slot)
					{
						slot_index = CHASSIS_SLOT_NO2INDEX(slot);
						port_index = ETH_LOCAL_NO2INDEX(slot_index, port);
						eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,port_index);
						npd_syslog_dbg("SLOT_ID = %d,slot = %d\n",SLOT_ID,slot);
						/*disable the specific port on the specific slot*/
						ret = npd_vlan_qinq_endis(vlanId, eth_g_index, isEnable);
						if(ret != NPD_SUCCESS)
						{
							npd_syslog_err("set slot %d port %d qinq disable FAIL !",slot,port);
							return -1;	/* code optimize: missing return statement houxx@autelan.com  2013-1-18 */
						}
						else
						{
							npd_syslog_dbg("set vlan %d port %d-%d qinq disable SUCCESS\n",vlanId,slot,port);
							g_vlanlist[vlanId-1].qinq[slot-1].low_bmp &= (unsigned int)(~(1<<k));
						}
					}
					
					
            	}
				else
				{
					continue;
				}
			}
			else
			{
                if((g_vlanlist[vlanId-1].qinq[j].high_bmp)&(1<<(k-32)))
            	{

					slot = j+1;
					port = k+1;
					if(SLOT_ID == slot)
					{
						slot_index = CHASSIS_SLOT_NO2INDEX(slot);
						port_index = ETH_LOCAL_NO2INDEX(slot_index, port);
						eth_g_index = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(slot_index,port_index);
						ret = npd_vlan_qinq_endis(vlanId, eth_g_index, isEnable);
						if(ret != NPD_SUCCESS)
						{
							npd_syslog_err("set slot %d port %d qinq disable FAIL !",slot,port);
							return NPD_FAIL;	/* code optimize: missing return statement houxx@autelan.com  2013-1-18 */
						}
						else
						{
							npd_syslog_dbg("set vlan %d port %d-%d qinq disable SUCCESS\n",vlanId,slot,port);
							g_vlanlist[vlanId-1].qinq[slot-1].high_bmp &= (unsigned int)(~(1<<(k-32)));
						}
					}
															
            	}
				else
				{
					continue;
				}								
			}
		}				
	}

	ret = msync(g_vlanlist, sizeof(vlan_list_t)*4096, MS_SYNC);
	if( ret!=0 )
	{
		syslog_ax_vlan_err("msync shm_vlan failed \n" );   
		return NPD_FAIL;	/* code optimize: missing return statement houxx@autelan.com  2013-1-18 */
	}
	return VLAN_RETURN_CODE_ERR_NONE;/* code optimize: missing return statement houxx@autelan.com  2013-1-18 */
}
#endif

#ifdef __cplusplus
}
#endif
