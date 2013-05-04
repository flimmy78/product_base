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
* npd_mld_snp_com.c
*
*
*CREATOR:
*		yangxs@autelan.com
*
* DESCRIPTION:
*   	APIs used in NPD for mld snooping process.
*
* DATE:
*  		03/31/2010	
*
*  FILE REVISION NUMBER:
*       $Revision: 1.3 $
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*include header files begin */
/*kernel part */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/select.h>
#include "util/npd_list.h"
#include <arpa/inet.h>

/*user part */
#include "sysdef/npd_sysdef.h"
#include "sysdef/returncode.h"
#include "util/npd_list.h"
#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_product.h"
#include "npd_vlan.h"
#include "npd_log.h"
#include "npd_igmp_snp_com.h"
#include "sysdef/returncode.h"
#include "npd_mld_snp_com.h"

/*******************************************************************************
* mld_v6addr_equal_0
*
* DESCRIPTION:
*		
* INPUTS:
* 	lVid  - the vlan id
*
* OUTPUTS:
*    	
*
* RETURNS:
* 	IGMPSNP_RETURN_CODE_OK   	- the vlan exist
* 	IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST	- vlan not exist
*
* COMMENTS:
*    
**
********************************************************************************/
unsigned int npd_mld_v6addr_equal_0
(
	unsigned short * group
)
{
	if((0 == group[0])&&(0 == group[1])&&(0 == group[2])&&(0 == group[3])&& \
		(0 == group[4])&&(0 == group[5])&&(0 == group[6])&&(0 == group[7])){
		return MLD_RETURN_CODE_EQUAL;
	}
	else{
		return MLD_RETURN_CODE_NOT_EQUAL;
	}
}
/*******************************************************************************
* mld_ntohs
*
* DESCRIPTION:
*		
* INPUTS:
* 	lVid  - the vlan id
*
* OUTPUTS:
*    	
*
* RETURNS:
* 	IGMPSNP_RETURN_CODE_OK   	- the vlan exist
* 	IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST	- vlan not exist
*
* COMMENTS:
*    
**
********************************************************************************/
unsigned int npd_mld_ntohs
(
	unsigned short ** group_ip
)
{
	int i = 0;
	unsigned short tempgp = 0;
	
	for(i=0; i < SIZE_OF_IPV6_ADDR; i++){ 
		tempgp = ntohs((*group_ip)[i]);/*ntohs()---network to host ushort*/
		(*group_ip)[i] = tempgp;
	}
	/*change the position of 8 u_short symmetrically*/
	tempgp = 0;
	for(i=0; i<(SIZE_OF_IPV6_ADDR/2); i++){
		tempgp = (*group_ip)[i];
		(*group_ip)[i] = (*group_ip)[SIZE_OF_IPV6_ADDR-1-i];
		(*group_ip)[SIZE_OF_IPV6_ADDR-1-i] = tempgp;
	}
	return MLD_RETURN_CODE_OK;
}

/*******************************************************************************
* mld_htons
*
* DESCRIPTION:
*		
* INPUTS:
* 	lVid  - the vlan id
*
* OUTPUTS:
*    	
*
* RETURNS:
* 	IGMPSNP_RETURN_CODE_OK   	- the vlan exist
* 	IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST	- vlan not exist
*
* COMMENTS:
*    
**
********************************************************************************/
unsigned int npd_mld_htons
(
	unsigned short ** group_ip
)
{
	int i = 0;
	unsigned short tempgp = 0;
	
	for(i=0; i < SIZE_OF_IPV6_ADDR; i++){ 
		tempgp = ntohs((*group_ip)[i]);/*ntohs()---network to host ushort*/
		(*group_ip)[i] = tempgp;
	}
	/*change the position of 8 u_short symmetrically*/
	tempgp = 0;
	for(i=0; i<(SIZE_OF_IPV6_ADDR/2); i++){
		tempgp = (*group_ip)[i];
		(*group_ip)[i] = (*group_ip)[SIZE_OF_IPV6_ADDR-1-i];
		(*group_ip)[SIZE_OF_IPV6_ADDR-1-i] = tempgp;
	}
	return MLD_RETURN_CODE_OK;
}

/*******************************************************************************
 * mld_copy_ipv6addr
 *
 * DESCRIPTION:
 *		
 * INPUTS:
 * 	lVid  - the vlan id
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	IGMPSNP_RETURN_CODE_OK   	- the vlan exist
 * 	IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST	- vlan not exist
 *
 * COMMENTS:
 *    
 **
 ********************************************************************************/
unsigned int npd_mld_copy_ipv6addr
(
	unsigned short *ugroup,
	unsigned short **destugroup
)
{
	int i =0;
	for(i=0; i<SIZE_OF_IPV6_ADDR; i++){
		(*destugroup)[i] = ugroup[i];
	}
	return MLD_RETURN_CODE_OK;
}


/*******************************************************************************
 * mld_compare_ipv6addr
 *
 * DESCRIPTION:
 *		
 * INPUTS:
 * 	lVid  - the vlan id
 *
 * OUTPUTS:
 *    	
 *
 * RETURNS:
 * 	IGMPSNP_RETURN_CODE_OK   	- the vlan exist
 * 	IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST	- vlan not exist
 *
 * COMMENTS:
 *    
 **
 ********************************************************************************/
/*compare ipv6 address*/
unsigned int npd_mld_compare_ipv6addr
(
	unsigned short *addr1, 
	unsigned short *addr2
)
{
	unsigned short temp1 = 0;
	unsigned short temp2 = 0;
	int i =0;
	for(i =0; i<SIZE_OF_IPV6_ADDR; i++){
		temp1 = addr1[i];
		temp2 = addr2[i];
		if(temp1 != temp2) break;
	}

	if(i== SIZE_OF_IPV6_ADDR) 
		return MLD_RETURN_CODE_EQUAL;
	else
		return MLD_RETURN_CODE_NOT_EQUAL;		
}

/*******************************************************************************
 * npd_check_mld_snp_status
 *
 * DESCRIPTION:
 *   		check the global status is enable or disable
 *
 * INPUTS:
 *		null
 *
 * OUTPUTS:
 *    	status - output the status
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - get the status successfully
 *
 * COMMENTS:
 *   
 **
 ********************************************************************************/

int npd_check_mld_snp_status(unsigned char *status)
{
	syslog_ax_vlan_dbg("npd check mld snp status: %d.",mldSnpEnable);
	*status = mldSnpEnable;
	return IGMPSNP_RETURN_CODE_OK;
}


/**********************************************************************************
 *  npd_mld_fdb_entry_addto
 *
 * DESCRIPTION:
 * 		add a fdb entry to vlanNode list
 *
 * INPUT:
 *		vid - vlan id
 *		groupIp - group Ip address
 *		vidx - the vlan index about group
 *	
 * OUTPUT:
 *		NULL
 *
 *  RETURN:
 *		IGMPSNP_RETURN_CODE_OK - add success
 *		IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST - can not find vlannode by vid
 *		IGMPSNP_RETURN_CODE_ALLOC_MEM_NULL - melloc memery fail
 *
 **********************************************************************************/
unsigned int npd_mld_fdb_entry_addto
(
	unsigned short vid,
	unsigned short *groupIp,
	unsigned short vidx
)
{
	syslog_ax_vlan_dbg("npd mld fdb entry addto:vlanId %d,vidx %d,"\
						"groupIp %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x.\n",vid,vidx,ADDRSHOWV6(groupIp));
	struct vlan_s *vlanNode = NULL;
	igmp_snooping_list_s *fdbList = NULL;
	struct igmp_snooping_list_node_s *node = NULL;
	unsigned short *stdip6 =NULL;

	vlanNode = npd_find_vlan_by_vid(vid);
	if(NULL == vlanNode){
		return IGMPSNP_RETURN_CODE_VLAN_NOT_EXIST;
	}
	
	fdbList = vlanNode->igmpList;
	
	if(NULL == fdbList) { /* create port list */
		/* syslog_ax_vlan_dbg("npd vlan %d add first %s port %#0x",vlanId,isTagged ? "tag":"untag",eth_g_index);*/
		fdbList = (igmp_snooping_list_s *)malloc(sizeof(igmp_snooping_list_s));
		if(NULL == fdbList) {
			 syslog_ax_vlan_err("npd vlan %d add igmpfdb entry with vidx %d memory err",	
					vid,vidx);
			return IGMPSNP_RETURN_CODE_ALLOC_MEM_NULL;
		}
		else {
			memset(fdbList, 0, sizeof(igmp_snooping_list_s));
			INIT_LIST_HEAD(&(fdbList->list));
				vlanNode->igmpList = fdbList;
		}
	}

	node = (struct igmp_snooping_list_node_s *)malloc(sizeof(struct igmp_snooping_list_node_s));
	if(NULL == node) {
		 syslog_ax_vlan_err("npd vlan %d add igmp entry vidx %d "\
		 					"groupId %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x to list error:null memory alloc", \
							vid, vidx,ADDRSHOWV6(groupIp));
		return IGMPSNP_RETURN_CODE_ALLOC_MEM_NULL;
	}
	memset(node,0,sizeof(struct igmp_snooping_list_node_s));
	node->vidx = vidx;
	stdip6 = node->grouIpV6;
	npd_mld_copy_ipv6addr(groupIp, &(stdip6));
	list_add(&(node->list),&(fdbList->list));
	fdbList->count++;
	syslog_ax_vlan_dbg("######vlan %d current igmpList node count %d,\n"\
						"#####igmpList Node->vidx %d,\n"\
						"######node->groupIp %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x.\n",\
								vid,fdbList->count,node->vidx,ADDRSHOWV6(node->grouIpV6));

	return IGMPSNP_RETURN_CODE_OK;
}



/*******************************************************************************
 * npd_mld_snp_recvmsg_proc
 *
 * DESCRIPTION:
 *		 config dev according to running result of igmp snoop protocol
 *
 * INPUTS:
 * 		igmp_notify - notify message
 *
 * OUTPUTS:
 *    	null
 *
 * RETURNS:
 *		IGMPSNP_RETURN_CODE_OK - config success
 *
 * COMMENTS:
 *  
 **
 ********************************************************************************/
unsigned int npd_mld_snp_recvmsg_proc(mld_notify_mod_npd * mld_notify)
{
	unsigned int ret = 0;
	unsigned short vlanId = 0;
	unsigned int eth_g_index = 0;
	unsigned int group_addr = 0x0;
	unsigned short group_ip[SIZE_OF_IPV6_ADDR] = {0};
	unsigned short vidx = 0;
	unsigned short *pstIp6 =NULL;
	unsigned int igmpSysSet =0;/*fdb_entry_exist = 0;*/

	vlanId = mld_notify->vlan_id;
	eth_g_index = (unsigned int)mld_notify->ifindex;
	pstIp6 = group_ip;
	npd_mld_copy_ipv6addr(mld_notify->groupadd, &(pstIp6));
	vidx = mld_notify->reserve;
	
	/*copy ipv6 address the last two shorts to a int*/
	group_addr = group_addr | group_ip[6];
	group_addr = group_addr<<16;
	group_addr = group_addr | group_ip[7];
	
	syslog_ax_igmp_dbg("npd receive message from mld_snp:get mldMsg:type %d, vlan_id %d, eth_g_index %d,\n"\
						"group_ip %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x, group_addr 0x%8x,%s %d.\n",\
						mld_notify->mod_type, vlanId, eth_g_index, ADDRSHOWV6(group_ip), group_addr, \
						(MLD_SYS_SET ==mld_notify->mod_type)?"reserve":"vidx", \
						mld_notify->reserve);

	switch(mld_notify->mod_type)
	{
		case MLD_ADDR_ADD: /*the main purpose is to add a group ,and at the same time,  add a member in the multicast group */
		{



			/*FDB Entry operation,if fdb already exists,do nothing on Sw.*/
			if(IGMPSNP_RETURN_CODE_OK != npd_igmp_check_contain_fdb(vlanId,vidx)){
				ret = npd_mld_fdb_entry_addto(vlanId,group_ip,vidx);
				if(IGMPSNP_RETURN_CODE_OK == ret ){
					/*the FDB entry is a New one,Here to config on HW*/
					syslog_ax_igmp_dbg("Mld: here to set FDB entry On HW,"\
										"group_ip %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x,vidx %d,vlan %d!\n",\
										ADDRSHOWV6(group_ip),vidx,vlanId);
					ret = nam_static_fdb_entry_indirect_set_for_mld(group_addr,vidx,vlanId);
					syslog_ax_igmp_dbg("npd mld Set FDB entry on HW,ret =%d.\n",ret);
				}
			}

			/*vidx = mld_notify->reserve;*/
			/*eth_g_index is a new member of vidx*/
			ret = npd_l2mc_entry_mbr_add(vlanId,vidx,eth_g_index, group_addr);
			
			syslog_ax_igmp_dbg("npd_l2mc_entry_mbr_add in mld ret %x!\n", ret);
			break;
		}
		case MLD_ADDR_DEL:  /*the main purpose is to delete a member in the multicast group,and at the same time,if the multicast group is empty,delete it.*/
		{
			/*vidx = igmp_notify->reserve;*/
			ret = npd_l2mc_entry_mbr_del(vlanId,vidx,eth_g_index,group_addr);
			if(IGMPSNP_RETURN_CODE_OK != npd_l2mc_entry_mbr_exist(vidx))
			{
				/*L2mc GROUP empty!!*/
				/*delete l2mc entry*/
				npd_l2mc_entry_delete(vlanId,vidx,group_addr);

				/*delete fdb entry*/
				if(IGMPSNP_RETURN_CODE_OK != npd_igmp_check_contain_fdb(vlanId,vidx)){
					ret = npd_igmp_fdb_entry_delfrom(vlanId,vidx);
					if(IGMPSNP_RETURN_CODE_OK == ret ){
						syslog_ax_igmp_dbg("Here to delete FDB entry On HW,",\
											"group_ip %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x,vidx %d,vlan %d!",\
											ADDRSHOWV6(group_ip),vidx,vlanId);
						nam_static_fdb_entry_indirect_delete_for_mld(group_addr,vlanId);
					}
				}
			}
			/*npd_show_vidx_member(vlanId,vidx);*/
			break;
		}
		case MLD_ADDR_RMV: /*the main purpose is just to delete group according vidx, no relationship with groupmembers*/
		{
			/*vidx = igmp_notify->reserve;*/
			if(IGMPSNP_RETURN_CODE_OK == npd_igmp_check_contain_fdb(vlanId,vidx)){
				ret = npd_igmp_fdb_entry_delfrom(vlanId,vidx);
				if(IGMPSNP_RETURN_CODE_OK == ret ){
					/*Here to delete FDB entry on HW*/
					syslog_ax_igmp_dbg("Here to delete FDB entry On HW,group_ip 0x%x,vidx %d,vlan %d!\n",group_addr,vidx,vlanId);
					nam_static_fdb_entry_indirect_delete_for_mld(group_addr,vlanId);
					ret = npd_l2mc_entry_delete(vlanId,vidx,group_addr);
				}
			}
			break;
		}
		case MLD_SYS_SET:
		{

			igmpSysSet = mld_notify->reserve;
			if( MLD_SYS_SET_INIT == igmpSysSet)
			{
				/*set igmp snooping Enable Global Flag */
				if(!mldSnpEnable){mldSnpEnable = 1;}
				syslog_ax_igmp_dbg("Set MLD Snooping Enable Global Flag: mldSnpEnable %d.\n",mldSnpEnable);
				/*transfer Product system MAC address to igmp snooping*/
				ret = npd_igmp_sysmac_notifer();
				if(IGMPSNP_RETURN_CODE_OK != ret){
					syslog_ax_igmp_dbg("npd sysmac notifier error!\n");
				}

				/*add DEFAULT vlan,and all port member of default vlan -- 08/09/23*/
				/*set igmp trap enable on Default vlan*/
				ret = npd_vlan_igmp_snp_endis_config(NPD_TRUE,1);

				/*add default vlan port member to igmp snooping*/
				npd_default_vlan_port_mbr_igmp_enable(1,NPD_TRUE);
			}
			else if(MLD_SYS_SET_STOP == igmpSysSet)
			{
				/*clear igmp snooping Enable Global Flag */
				mldSnpEnable = 0;
				syslog_ax_igmp_dbg("Clear mld Snooping Enable Global Flag: mldSnpEnable %d.\n",mldSnpEnable);

				/*igmp Disable:delete all FDB entries which interfaceType = Vidx,
				  Vidx membership entries,*/
				ret = npd_igmp_clear_fdb_list_new();
				syslog_ax_igmp_dbg("npd_igmp_clear_fdb_list_new completed");

				/*disable igmp snooping on DEFAULT vlan --08/09/23*/
				ret = npd_vlan_igmp_snp_endis_config(NPD_FALSE, 1);

				/*disable default vlan port member to igmp snooping */
				npd_default_vlan_port_mbr_igmp_enable(1,NPD_FALSE);
				/*ret = npd_l2mc_entry_delete_all();*/
			}
			break;	
		}
		default :
		{
			syslog_ax_igmp_dbg("npd can NOT proccess the running result of Protocol");
			break;
		}
	}
	return IGMPSNP_RETURN_CODE_OK;
}



#ifdef _cplusplus
}
#endif
