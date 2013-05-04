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
* npd_acl_drv.c
*
*
* CREATOR:
*		qinhs@autelan.com
*
* DESCRIPTION:
*		APIs used in NPD for ACL module driver process.
*
* DATE:
*		02/21/2008	
*
*  FILE REVISION NUMBER:
*  		$Revision: 1.58 $	
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "npd_log.h"
#include "util/npd_list.h"
#include "sysdef/npd_sysdef.h"

#include "npd/nam/npd_amapi.h"
#include "npd/nbm/npd_bmapi.h"
#include "npd_c_slot.h"
#include "npd_eth_port.h"
#include "npd_product.h"
#include "npd_acl_drv.h"
#include "npd_acl.h"
#include "npd_vlan.h"
#include "dbus/npd/npd_dbus_def.h"
#include "sysdef/returncode.h"

extern AclRuleList 				**g_acl_rule;
extern struct eth_port_s 		**g_eth_ports;
extern struct acl_group_list_s  *acl_group[MAX_GROUP_NUM];
extern struct acl_group_list_s  *egress_acl_group[MAX_GROUP_NUM];
extern struct vlan_s		    **g_vlans;
extern QOS_POLICER_STC		    **g_policer;
extern unsigned char g_acl_enable;

/*********************************************************
*  search_global_acl_rule
*
*  DESCRIPTION:
* 	  check if rule <number> has been set
*
*  INPUT:
* 	    number -enable
*  
*  OUTPUT:
* 	    NULL
*
*  RETURN: 	 
* 	    op_ret - enable acl service for all ports
*			
*
***********************************************************/
unsigned int npd_acl_service_enable(int enable, int init)
{
	unsigned int	i = 0, j = 0, portnum = 0, softenable = 1;
	unsigned char	   devNum=0,virPort=0;
	unsigned int	   ret = ACL_RETURN_CODE_SUCCESS, portInfo=NPD_DBUS_ERROR_NO_SUCH_PORT;

	for (i = 0; i < CHASSIS_SLOT_COUNT; i++ ) {
		syslog_ax_eth_port_dbg("init acl service for eth-port on slot %d module %s\n",	\
				i,module_id_str(MODULE_TYPE_ON_SLOT_INDEX(i)));
		for (j = 0; j < ETH_LOCAL_PORT_COUNT(i); j++) {
			portnum = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i, j);
			portInfo = npd_get_devport_by_global_index(portnum,&devNum,&virPort);
			if(portInfo != ACL_RETURN_CODE_SUCCESS) {
				/*ret = NPD_DBUS_ERROR_NO_SUCH_PORT;*/
				continue;
			}
			/*portPtr = npd_get_port_by_index(portnum);
			//if(NULL==portPtr) {
			//	syslog_ax_eth_port_dbg("wrong port infomation when search by eth_g_index %d!\n",portnum);
		        //	ret = 1;
		//	}*/
			if(NULL==(g_eth_ports[portnum]->funcs.func_data[ETH_PORT_FUNC_ACL_RULE])) {
				/*syslog_ax_acl_dbg("interface no group infomation!\n");*/
				softenable = 0;
			}
		      else{
		   	       softenable = npd_eth_port_acl_bind_check(portnum, ACL_DIRECTION_INGRESS);
				 if(NPD_TRUE != softenable){
				/*	syslog_ax_acl_dbg("please enable the port acl service!\n");*/
					softenable = 0;
				 }
		      	}
			/*softenable = (portPtr->funcs.funcs_cap_bitmap)&(1<< ETH_PORT_FUNC_ACL_RULE);*/
			if (init) {
				/*npd_syslog_dbg("acl for system init\n");*/
				ret = nam_bcm_acl_port_acl_enable(devNum, virPort, enable, init, g_acl_enable);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					npd_syslog_dbg("nam_acl_port_acl_enable fail ! \n");
					return ACL_RETURN_CODE_ERROR;
				}
			}
			else {
				/*npd_syslog_dbg("change port enable or disable by softenable %d\n", softenable);*/
				ret = nam_bcm_acl_port_acl_enable(devNum, virPort, enable, softenable, g_acl_enable);
				if (ACL_RETURN_CODE_SUCCESS != ret) {
					npd_syslog_dbg("nam_acl_port_acl_enable fail ! \n");
					return ACL_RETURN_CODE_ERROR;
				}
			}
		}
	}
	return ret;
}

/* add for xcat port metering enable*/
unsigned int npd_acl_portmeteringenableset(int enable)
{
	unsigned int	i = 0, j = 0, portnum = 0, softenable = 1;
	unsigned char	   devNum=0,virPort=0;
	unsigned int	   ret = ACL_RETURN_CODE_SUCCESS, portInfo=NPD_DBUS_ERROR_NO_SUCH_PORT;

	for (i = 0; i < CHASSIS_SLOT_COUNT; i++ ) {
		syslog_ax_eth_port_dbg("init acl service for eth-port on slot %d module %s\n",	\
				i,module_id_str(MODULE_TYPE_ON_SLOT_INDEX(i)));
		for (j = 0; j < ETH_LOCAL_PORT_COUNT(i); j++) {
			portnum = ETH_GLOBAL_INDEX_FROM_SLOT_PORT_LOCAL_INDEX(i, j);
			portInfo = npd_get_devport_by_global_index(portnum,&devNum,&virPort);
			if(portInfo != ACL_RETURN_CODE_SUCCESS) {
				/*ret = NPD_DBUS_ERROR_NO_SUCH_PORT;*/
				continue;
			}
	
			ret = nam_qos_portmeteringenableset(devNum, virPort, enable);
			if (ACL_RETURN_CODE_SUCCESS != ret) {
				npd_syslog_dbg("nam_acl_port_acl_enable fail ! \n");
				return ACL_RETURN_CODE_ERROR;
			}
		
		}
	}
	return ret;
}

/*********************************************************
*  npd_acl_len2mask
*
*  DESCRIPTION:
* 	  get ip mask by mask length
*
*  INPUT:
* 	    len - mask length
*  
*  OUTPUT:
* 	    mask -ip mask
*
*  RETURN: 	 
* 	   
*
*
***********************************************************/
void npd_acl_len2mask (int len,unsigned long *mask)
{
    int i = 0, tmp=1;
	if(len>1){
		for(i=1;i<len;i++)
			tmp=(tmp<<1)+1;
		tmp=tmp<<(32-len);
		*mask=tmp;
	}
	else
		*mask =0 ;
}

/*********************************************************
*  search_global_acl_rule
*
*  DESCRIPTION:
* 	  check if rule <number> has been set
*
*  INPUT:
* 	    number - rule index
*  
*  OUTPUT:
* 	    NULL
*
*  RETURN: 	 
* 	    op_ret - ACL_RETURN_CODE_SUCCESS   existed
*				 ACL_RETURN_CODE_ERROR     not existed
*
*
***********************************************************/
unsigned int search_global_acl_rule(unsigned long number)
{
	unsigned int ret = 0;
	
	if(g_acl_rule[number]==NULL) {
		ret= ACL_RETURN_CODE_ERROR; /*not existed rule*/
	}
	else {
		ret= ACL_RETURN_CODE_SUCCESS;	/*exist*/
	}

	return ret;
}

/*********************************************************
*  search_acl_group_rule_index_exist
*
*  DESCRIPTION:
* 	  check if acl list <index> has been added in acl group <group_num>
*
*  INPUT:
* 	    index - acl list index
*  
*  OUTPUT:
* 	    num - if has added in group , output the group number
*
*  RETURN: 	 
* 	    op_ret - ACL_RETURN_CODE_SUCCESS   existed
*				 ACL_RETURN_CODE_ERROR     not existed
*
*
***********************************************************/
unsigned int search_acl_group_rule_index_exist
(
	unsigned long index,
	unsigned int  *groupNum,
	unsigned int  *direction
)
{
	struct acl_rule_info_s  		*ruleInfo = NULL; 
	struct acl_group_list_s	 	*groupInfo = NULL;
	struct list_head         		*ptr = NULL;   
	unsigned  int                 ret=ACL_RETURN_CODE_SUCCESS,result=ACL_RETURN_CODE_SUCCESS;    
	unsigned  int					j=0,i=0;

	npd_syslog_dbg("searching global list,cheack if rule %ld existed\n",index);
  
      ret=search_global_acl_rule(index);
      if (ACL_RETURN_CODE_SUCCESS!=ret){
		npd_syslog_err("can not find the rule %ld\n",index);
		return ACL_RETURN_CODE_GLOBAL_NOT_EXISTED;		 
   	  }	  
	  else if(ACL_RETURN_CODE_SUCCESS==ret) {
		for(i=0;i<MAX_GROUP_NUM;i++) {
		 	groupInfo = acl_group[i];
			if(NULL != groupInfo){
				npd_syslog_err("searching acl group,check if rule %ld added!\n",index);
				
				__list_for_each(ptr,&(groupInfo->list1)) {
					
					ruleInfo=list_entry(ptr,struct acl_rule_info_s,list);
					syslog_ax_acl_dbg("ruleInfo %p\n",ruleInfo);
					if(NULL != ruleInfo){
						if(index == ruleInfo->ruleIndex) {
							npd_syslog_err("the %ld rule has been added in acl group %d\n",index,i);	
							result=ACL_RETURN_CODE_GROUP_RULE_EXISTED; 
							 *groupNum = i;
							 *direction = ACL_DIRECTION_INGRESS;
							break;
						}
					}
		  		} 				
	  		}
		}	
		for(j=0;j<MAX_GROUP_NUM;j++) {
		 	groupInfo = egress_acl_group[j];
			if(NULL != groupInfo) {
				npd_syslog_dbg("searching acl group,check if rule %ld added!\n",index);
				
				__list_for_each(ptr,&(groupInfo->list1)) {
					ruleInfo=list_entry(ptr,struct acl_rule_info_s,list);
					if(NULL != ruleInfo){
						if(index==ruleInfo->ruleIndex) {
							npd_syslog_dbg("the %ld rule has been added in acl group %d\n",index,j);	
							result=ACL_RETURN_CODE_EGRESS_GROUP_RULE_EXISTED; 
							 *groupNum = j;
							  *direction = ACL_DIRECTION_EGRESS;
							break;
						}
					}
		  		} 				
	  		}
		}	
      }
      else {
		result = ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
      }
	 
     return result;           
}      

/**********************************************************************************
 *  npd_acl_find_addr_by_ruleIndex
 *
 *	DESCRIPTION:
 * 		Check out if specified acl rule has been set or not
 * 		index used to find acl struction.
 *
 *	INPUT:
 *		index - rule index
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL  - if parameters given are wrong
 *		aclNode	- if acl rule has been set before 
 *		
 **********************************************************************************/
AclRuleList *npd_acl_find_addr_by_ruleIndex
(
	unsigned long index
)
{
	AclRuleList *aclNode = NULL;
	/*aclId should be in range of (0-1023)*/
	/*code optimize:  Unsigned compared against 0
	zhangcl@autelan.com 2013-1-17*/
	if(index > (MAX_ACL_RULE_NUMBER-1)) 
	{
		aclNode = NULL;
	}
	else 
	{
		aclNode = g_acl_rule[index];
	}
	return aclNode;
}

unsigned int npd_acl_group_add_equal_rule
(
	unsigned long index,
	unsigned int groupnum,
	unsigned int groupty
)
{
	struct ACCESS_LIST_STC *aclNode = NULL;
	struct acl_rule_info_s *ruleInfo = NULL ;
	struct acl_group_list_s	 	*groupInfo = NULL;
      struct list_head         		*ptr = NULL; 
	unsigned int ret = ACL_RETURN_CODE_SUCCESS;
	
	aclNode = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
	if(aclNode == NULL) 
	{
		/*code optimize:    Dereference after null check
		zhangcl@autelan.com 2013-1-17*/
		ret = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
		return ret;
	}
	memset(aclNode,0,sizeof(struct ACCESS_LIST_STC));
/*	
	ruleInfo = (struct ACCESS_LIST_STC *)malloc(sizeof(struct ACCESS_LIST_STC));
	if(ruleInfo == NULL) {
		ret = ACL_RETURN_CODE_NORMAL_MALLOC_ERROR_NONE;
	}
	memset(ruleInfo,0,sizeof(struct ACCESS_LIST_STC));
*/	
	memcpy(aclNode,  g_acl_rule[index], sizeof(struct ACCESS_LIST_STC));

	if (groupty == ACL_DIRECTION_INGRESS) 
	{
		groupInfo = acl_group[groupnum];
	}
	else if (groupty == ACL_DIRECTION_EGRESS) 
	{
		groupInfo = egress_acl_group[groupnum];
	}
	else 
	{
		/*code optimize:    Explicit null dereferenced
		zhangcl@autelan.com 2013-1-17*/
		free(aclNode);
		return ACL_RETURN_CODE_ERROR;
	}
			
	__list_for_each(ptr,&(groupInfo->list1)) 
	{
		ruleInfo = list_entry(ptr,struct acl_rule_info_s,list);
		memcpy(&(aclNode->ruleIndex), &(g_acl_rule[ruleInfo->ruleIndex]->ruleIndex), sizeof(unsigned long));
		if(memcmp(aclNode, g_acl_rule[ruleInfo->ruleIndex], sizeof(struct ACCESS_LIST_STC)) == 0 ) 
		{
			ret = ACL_RETURN_CODE_ADD_EQUAL_RULE;
			break;
		}
	}	
	syslog_ax_acl_dbg("npd_acl_group_add_equal_rule ret is %d\n", ret);
	free(aclNode);
/*	free(ruleInfo);*/
	return ret;
}

/**********************************************************************************
 *  npd_acl_group_bind_check
 *
 *	DESCRIPTION:
 * 		check if group been binded by port
 *
 *	INPUT:
 *		*node - rule contact
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR  - unbind
 *		ACL_RETURN_CODE_SUCCESS- bind
 *		
 **********************************************************************************/
unsigned int npd_acl_group_bind_check(struct acl_group_list_s *nodeGroupPtr)
{
	unsigned int ret = 0;
	if(((nodeGroupPtr->eth_count)==0)&&((nodeGroupPtr->vlan_count)==0)) {
		ret=ACL_RETURN_CODE_ERROR;
	}
	else if(((nodeGroupPtr->eth_count)!=0)||((nodeGroupPtr->vlan_count)!=0)) {
		ret= ACL_RETURN_CODE_SUCCESS;
	}
	return ret;
}

/**********************************************************************************
 *  npd_port_bind_group_check
 *
 *	DESCRIPTION:
 * 		check if port bind group
 *
 *	INPUT:
 *		eth_g_index  port index
 *	
 *	OUTPUT:
 *		groupNo      group id
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR  		- port info error
 *		ACL_RETURN_CODE_GROUP_PORT_NOTFOUND - no group bind on port
 *		ACL_RETURN_CODE_GROUP_PORT_BINDED	- group bind on port
 *		
 **********************************************************************************/
unsigned int npd_port_bind_group_check(unsigned int eth_g_index,unsigned int *groupNo,unsigned int dir_info)
{
	struct eth_group_info  			*eth_group = NULL;
	struct eth_port_s				*portInfo = NULL;
	unsigned int 					ret=ACL_RETURN_CODE_SUCCESS;
	
	portInfo = npd_get_port_by_index(eth_g_index);
	if(NULL == portInfo) {
		ret = ACL_RETURN_CODE_ERROR;
	}
	else{
		eth_group=portInfo->funcs.func_data[ETH_PORT_FUNC_ACL_RULE];
		if(eth_group!=NULL){
			if(dir_info == ACL_DIRECTION_INGRESS){				
					if(eth_group->groupId!=0){
						ret = ACL_RETURN_CODE_GROUP_PORT_BINDED;
						npd_syslog_dbg("ingress group num :eth_group->groupId %d\n",eth_group->groupId);
						*groupNo = eth_group->groupId;				    
					}
					else
						ret = ACL_RETURN_CODE_GROUP_PORT_NOTFOUND;				
			}/*ingress*/
			else if(dir_info == ACL_DIRECTION_EGRESS){				
					if(eth_group->EgGroupId!=0){
						ret = ACL_RETURN_CODE_GROUP_PORT_BINDED;
						npd_syslog_dbg("ingress group num :eth_group->EgGroupId %d\n",eth_group->EgGroupId);
						*groupNo = eth_group->EgGroupId;					
					}
					else
						ret = ACL_RETURN_CODE_GROUP_PORT_NOTFOUND;
				
			}/*egress*/
		}
		else 
			ret = ACL_RETURN_CODE_GROUP_PORT_NOTFOUND; 
		
	}
	return ret;   
}

/**********************************************************************************
 *  npd_vlan_bind_group
 *
 *	DESCRIPTION:
 * 		port bind group ,store group id ,eth_index 
 *
 *	INPUT:
 *		vlanId 		- vlan index
 *		group_num	- group id
 *	OUTPUT:
 *		null
 *
 * 	RETURN:
 *		NPD_VLAN_NOTEXISTS  		- vlan not exists
 *		ACL_RETURN_CODE_GROUP_NOT_EXISTED   	- group [group_num] not exist
 *		ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL 	- malloc error
 *		ACL_RETURN_CODE_SUCCESS  			- sw bind success
 *		ACL_RETURN_CODE_ERROR				- sw  bind fail
 **********************************************************************************/

unsigned int npd_vlan_bind_group
(
	unsigned short vlanId,
	unsigned int   group_num,
	unsigned int   dir_info
)
{
	struct vlan_s					*node=NULL;
	struct group_vlan_info			*vlan_group=NULL;
	/*struct vlan_acl_info_s 			*vlanPtr=NULL;*/	
	struct acl_group_list_s 		*group = NULL;
	unsigned int					 op_ret=ACL_RETURN_CODE_ERROR;
	
	op_ret=npd_check_vlan_exist(vlanId);
	if(VLAN_RETURN_CODE_VLAN_NOT_EXISTS==op_ret) {
		npd_syslog_err("vlan not existed!\n");
	}
	else {
		node = npd_find_vlan_by_vid(vlanId);
		if(NULL == node) {
			npd_syslog_dbg("npd_lookup_ports_by_vlanid::can not find vlan %d  node \n",vlanId);
			return NPD_VLAN_NOTEXISTS;
		}
		else{
			if(dir_info==ACL_DIRECTION_INGRESS){
				group=acl_group[group_num];
				if(NULL==group) {
					npd_syslog_err("error! config acl-group firstly!\n");
					return ACL_RETURN_CODE_GROUP_NOT_EXISTED;														  
				}
				else{
				
					vlan_group =  (struct group_vlan_info *)malloc (sizeof(struct group_vlan_info));
					if(NULL==vlan_group){
						npd_syslog_err("malloc fail!\n");
						return ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
					}
					memset(vlan_group,0,sizeof(struct group_vlan_info));

					vlan_group->vlan_index=vlanId;
					npd_syslog_dbg("vlan_group->vlan_index %d\n",vlan_group->vlan_index);
									
					npd_syslog_dbg("node->aclList %p\n",node->aclList);
					/*vlan retain groupId info */
					node->aclList->groupId=group_num;
					npd_syslog_dbg("node->aclList.groupId %d\n",node->aclList->groupId);
					/*group retain vlan info*/
					list_add_tail(&(vlan_group->list),&(group->list3));
					group->vlan_count+=1;
					npd_syslog_dbg("group->vlan_count %d\n",group->vlan_count);
					op_ret = ACL_RETURN_CODE_SUCCESS;						
				}				
			}
			else if(dir_info==ACL_DIRECTION_EGRESS) {
				group=egress_acl_group[group_num];
				if(NULL==group) {
					npd_syslog_err("error! config acl-group firstly!\n");
					return ACL_RETURN_CODE_GROUP_NOT_EXISTED;														  
				}
				else {				
					vlan_group =  (struct group_vlan_info *)malloc (sizeof(struct group_vlan_info));
					if(NULL==vlan_group){
						npd_syslog_err("malloc fail!\n");
						return ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
					}
					memset(vlan_group,0,sizeof(struct group_vlan_info));

					vlan_group->vlan_index=vlanId;
					npd_syslog_err("vlan_group->vlan_index %d\n",vlan_group->vlan_index);
									
					npd_syslog_err("node->aclList %p\n",node->aclList);
					/*vlan retain groupId info */
					node->aclList->EgrGroupId=group_num;
					npd_syslog_err("node->aclList.groupId %d\n",node->aclList->EgrGroupId);
					/*group retain vlan info*/
					list_add_tail(&(vlan_group->list),&(group->list3));
					group->vlan_count+=1;
					npd_syslog_err("group->vlan_count %d\n",group->vlan_count);
					op_ret = ACL_RETURN_CODE_SUCCESS;						
				}
			}
		}	
	}  	 

	return op_ret;
}

/**********************************************************************************
 *  npd_port_bind_group
 *
 *	DESCRIPTION:
 * 		port bind group ,store group id ,eth_index 
 *
 *	INPUT:
 *		eth_g_index - port index
 *		group_num	- group id
 *	OUTPUT:
 *		null
 *
 * 	RETURN:
 *		NPD_DBUS_ERROR_NO_SUCH_PORT  		- port not found ,info error
 *		ACL_RETURN_CODE_GROUP_NOT_EXISTED   - group [group_num] not exist
 *		ACL_RETURN_CODE_GROUP_PORT_BINDED	- port bind group yet
 *		ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL - malloc error
 *		ACL_RETURN_CODE_SUCCESS  		- sw bind success
 *      ACL_RETURN_CODE_ERROR			-bind fail
 **********************************************************************************/
unsigned int npd_port_bind_group
(	
	unsigned int eth_g_index,
	unsigned int group_num,
	unsigned int dir_info
)
{	
	struct eth_port_s				*portInfo = NULL;
	struct acl_group_list_s 		*group = NULL;
	struct eth_group_info  			*tmpBind = NULL;
	struct group_eth_info	        *group_eth=NULL;
	unsigned int					op_ret=ACL_RETURN_CODE_ERROR,groupNo=0,rem = 0;

	
	/*check port bind group or not*/
	rem = npd_port_bind_group_check(eth_g_index,&groupNo,dir_info);
	
	if(ACL_RETURN_CODE_ERROR==rem){
		npd_syslog_err("eth_g_index error!\n");
		return NPD_DBUS_ERROR_NO_SUCH_PORT;
	}
	else if(ACL_RETURN_CODE_GROUP_PORT_BINDED==rem){
		  npd_syslog_dbg("this interface has binded acl group %d\n",groupNo);
		  return  ACL_RETURN_CODE_GROUP_PORT_BINDED;
	}
	else if(ACL_RETURN_CODE_GROUP_PORT_NOTFOUND==rem){

		if(dir_info == ACL_DIRECTION_INGRESS)
			group=acl_group[group_num];
		else if(dir_info == ACL_DIRECTION_EGRESS)
			group=egress_acl_group[group_num];
		
		if(NULL==group)
		{
			npd_syslog_err("error! config acl-group firstly!\n");
			return ACL_RETURN_CODE_GROUP_NOT_EXISTED;														  
		}
		else{
			#if 0
			eth_group = (struct eth_group_info *)malloc (sizeof(struct eth_group_info));
			if(NULL==eth_group){
					npd_syslog_err("malloc fail!\n");
					return ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
			}
			memset(eth_group,0,sizeof(struct eth_group_info));
			#endif
			
			group_eth =  (struct group_eth_info *)malloc (sizeof(struct group_eth_info));
			if(NULL==group_eth){
					npd_syslog_err("malloc fail!\n");
					return ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
			}
			memset(group_eth,0,sizeof(struct group_eth_info));

			/*group eth*/
			group_eth->eth_index = eth_g_index;

			/*eth group*/			
			/*eth_group->groupId = group_num; */
			
			npd_syslog_dbg("group_eth->eth_index is %d\n",group_eth->eth_index);

			portInfo = npd_get_port_by_index(eth_g_index);
			if(NULL==portInfo){
				npd_syslog_err("port info error!\n");
				free(group_eth);
				return NPD_DBUS_ERROR_NO_SUCH_PORT;
			}
			else {
				tmpBind = portInfo->funcs.func_data[ETH_PORT_FUNC_ACL_RULE];
				if(ACL_DIRECTION_INGRESS==dir_info){
					/*port retain groupId info*/

					tmpBind->groupId = group_num;
					npd_syslog_dbg("tmpBind->groupId %d\n",tmpBind->groupId);
					/*group retain port info*/
					list_add_tail(&(group_eth->list),&(group->list2));
					group->eth_count+=1;
					npd_syslog_dbg("group->eth_count %d\n",group->eth_count);
					op_ret = ACL_RETURN_CODE_SUCCESS;
					
				}
				else if(ACL_DIRECTION_EGRESS==dir_info){
					/*port retain groupId info*/

					tmpBind->EgGroupId= group_num;
					npd_syslog_dbg("tmpBind->groupId %d\n",tmpBind->EgGroupId);

					/*group retain port info*/
					list_add_tail(&(group_eth->list),&(group->list2));
					group->eth_count+=1;

					npd_syslog_dbg("group->eth_count %d\n",group->eth_count);
					op_ret = ACL_RETURN_CODE_SUCCESS;
				}
				
					#if 0
					if(NULL!=tmpBind){
					     npd_syslog_dbg("this interface has binded acl group!\n");
					      return  ACL_RETURN_CODE_GROUP_PORT_BINDED;
					}
					else{	
						/*port retain groupId info*/
						npd_syslog_dbg("eth_group %p\n",eth_group);
					    portInfo->funcs.func_data[ETH_PORT_FUNC_ACL_RULE] = eth_group;
					    npd_syslog_dbg("after bind ,eth_group \n");
				  
						/*group retain port info*/
						list_add_tail(&(group_eth->list),&(group->list2));
						group->eth_count+=1;
						npd_syslog_dbg("group->eth_count %d\n",group->eth_count);
						op_ret = ACL_RETURN_CODE_SUCCESS;			   
					}
					#endif
			}
		}  /*no group bind*/
	}
	return op_ret;
}


/**********************************************************************************
 *  npd_acl_rule_index_check
 *
 *	DESCRIPTION:
 * 		check if acl rule index legal
 *
 *	INPUT:
 *		index - acl rule index
 *		
 *	OUTPUT:
 *		null
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_GLOBAL_EXISTED  	- rule [index] exist
 *		ACL_RETURN_CODE_EXT_NO_SPACE   		-	because extended rule need twice spaces than std rule
 *								-   rule[index+512] has been taken up by extend rule
 **********************************************************************************/
unsigned int npd_acl_rule_index_check(unsigned long index,unsigned int type)
{
	unsigned int ret=ACL_RETURN_CODE_SUCCESS;
	/*
	if((index<512)&&(index>=0))
	{
	     if(NULL!=g_acl_rule[index])	{
			npd_syslog_err("This node has already existed!\n");
			ret= ACL_RETURN_CODE_GLOBAL_EXISTED;
	      }
		
	     else if(NULL!=g_acl_rule[index+512]){
		 	
			if(STANDARD_ACL_RULE==(g_acl_rule[index+512]->ruleType))
		    ret=ACL_RETURN_CODE_EXT_NO_SPACE;
		}
		 //when set ext rule,if [index] null,
		 //also consider [index+512] if has taken by std rule,
		 // if taken ,ext has no enough spaces to store
		 //when set std rule ,only consider [index]
		 
	}	
	else if((index>511)&&(index<1024)){
		if(NULL!=g_acl_rule[index])	{
			npd_syslog_err("This node has already existed!\n");
			ret= ACL_RETURN_CODE_GLOBAL_EXISTED;
	     }
		 else if(NULL!=g_acl_rule[index-512]){
			if(EXTENDED_ACL_RULE==(g_acl_rule[index-512]->ruleType))
		    ret=ACL_RETURN_CODE_EXT_NO_SPACE;
		}
		//if index >=512 ,no ext rules,only std rule,
		//you shoule consider if [index-512] has taken by ext rule,
		//if [index-512] has taken by std rule ,no effect
	}*/
	if(EXTENDED_ACL_RULE == type) 
	{
		/*code optimize:  Unsigned compared against 0
		zhangcl@autelan.com 2013-1-17*/
		if(index<512) 
		{
		     if(NULL != g_acl_rule[index]) 
			 {
				npd_syslog_err("This node has already existed!\n");
				ret= ACL_RETURN_CODE_GLOBAL_EXISTED;
		     }		
		     else if(NULL != g_acl_rule[index+512]) 
			 {
			 	if (STANDARD_ACL_RULE==(g_acl_rule[index+512]->ruleType)) 
				{
			      		ret=ACL_RETURN_CODE_EXT_NO_SPACE;
			 	}
			}		 
		}
	}
	else 
	{
		if(index<512) 
		{
		     if(NULL != g_acl_rule[index]) 
			 {
				npd_syslog_err("This node has already existed!\n");
				ret= ACL_RETURN_CODE_GLOBAL_EXISTED;
		     }
		}
		else if((index>511)&&(index<1024)) 
		{
			if(NULL != g_acl_rule[index]) 
			{
				npd_syslog_err("This node has already existed!\n");
				ret= ACL_RETURN_CODE_GLOBAL_EXISTED;
		    }
			else if(NULL != g_acl_rule[index-512]) 
			{
				if(EXTENDED_ACL_RULE==(g_acl_rule[index-512]->ruleType))
			      ret=ACL_RETURN_CODE_EXT_NO_SPACE;
			}
		}
	}	
	return ret;
}

/**********************************************************************************
 *  npd_acl_qos_add
 *
 *	DESCRIPTION:
 * 		store qos based on acl (up,dscp) to qos profile
 *
 *	INPUT:
 *		*node - rule contact
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR    - if parameters given are wrong
 *		ACL_RETURN_CODE_SUCCESS  - if acl rule successfully stored 
 *		ACL_RETURN_CODE_GLOBAL_EXISTED- rule has not set yet
 *      ACL_RETURN_CODE_EXT_NO_SPACE  - no enough spaces for store
 *		ACL_RETURN_CODE_SAME_FIELD    - set same rule again,error 
 **********************************************************************************/
unsigned int npd_acl_qos_add(struct ACCESS_LIST_STC *node)
{

	struct ACL_TCAM_STC 	*tcam=NULL;
	struct ACL_ACTION_STC	*action=NULL;
	ACL_RULE_STDIPV4L4_T	*nodeStdIpv4L4=NULL;
	unsigned int			index=0,ret=0,type=0;
		
	if(NULL == node){
		npd_syslog_err("npd_acl_add:node null pointer error!\n");
		return ACL_RETURN_CODE_ERROR;
	}
	else {
		tcam			= &(node->TcamPtr);
		action			= &(node->ActionPtr);	
		nodeStdIpv4L4	= &(node->TcamPtr.ruleInfo.stdIpv4L4);
	}
	
	index=node->ruleIndex;
	type=node->ruleType;
	if(index>MAX_ACL_RULE_NUMBER-1) {
		npd_syslog_err("ERROR! The ruleIndex is in 1-%d!\n",MAX_ACL_RULE_NUMBER);
		return ACL_RETURN_CODE_RULE_INDEX_ERROR;
	}
			
	ret = npd_acl_rule_index_check(index, type);
	if(ACL_RETURN_CODE_GLOBAL_EXISTED == ret) {
		npd_syslog_dbg("This node has already existed!\n");
		return ACL_RETURN_CODE_GLOBAL_EXISTED;
	}
	else if(ACL_RETURN_CODE_EXT_NO_SPACE == ret) {
		return ACL_RETURN_CODE_EXT_NO_SPACE;
	}
/*	
	for(i=0;i<MAX_ACL_RULE_NUMBER;i++) {
		s0 = g_acl_rule[i];
		if(NULL!=s0) {
			tcamTmp   =&(s0->TcamPtr);
			actionTmp =&(s0->ActionPtr);
			stdIpv4L4Tmp=&(s0->TcamPtr.ruleInfo.stdIpv4L4);

			if((type==s0->ruleType)
				&&
				(actionTmp->actionInfo.qos.qos.ingress.profileIndex==action->actionInfo.qos.qos.ingress.profileIndex)
				&&
				(stdIpv4L4Tmp->rule.UP==nodeStdIpv4L4->rule.UP)
				&&
				(stdIpv4L4Tmp->mask.maskUP==nodeStdIpv4L4->mask.maskUP)
				&&
				(stdIpv4L4Tmp->mask.maskDSCP==nodeStdIpv4L4->mask.maskDSCP)
				&&
				(stdIpv4L4Tmp->rule.DSCP==nodeStdIpv4L4->rule.DSCP)			
				)			
			{
				npd_syslog_err("warning!same packet data eara can not be duplicatable!\n");
					return ACL_RETURN_CODE_SAME_FIELD;
			}
		}	
	}	*/	
	g_acl_rule[node->ruleIndex]=node;
	return ACL_RETURN_CODE_SUCCESS;	
}

/**********************************************************************************
 *  npd_acl_qos_egrAdd
 *
 *	DESCRIPTION:
 * 		store qos based on acl (up,dscp) to new value
 *
 *	INPUT:
 *		*node - rule contact
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR    - if parameters given are wrong
 *		ACL_RETURN_CODE_SUCCESS  - if acl rule successfully stored 
 *		ACL_RETURN_CODE_GLOBAL_EXISTED- rule has not set yet
 *      ACL_RETURN_CODE_EXT_NO_SPACE  - no enough spaces for store
 *		ACL_SAME_FIELD    - set same rule again,error 
 **********************************************************************************/
unsigned int npd_acl_qos_egrAdd(struct ACCESS_LIST_STC *node)
{
	struct ACL_TCAM_STC 	*tcam=NULL;
	struct ACL_ACTION_STC	*action=NULL;
	ACL_RULE_STDIPV4L4_T	*nodeStdIpv4L4=NULL;
	unsigned int			index=0,ret=0,type=0;
	
	if(NULL == node){
		npd_syslog_err("npd_acl_add:node null pointer error!\n");
		return ACL_RETURN_CODE_ERROR;
	}
	else {
		tcam			= &(node->TcamPtr);
		action			= &(node->ActionPtr);	
		nodeStdIpv4L4	= &(node->TcamPtr.ruleInfo.stdIpv4L4);
	}
	index=node->ruleIndex;
	type=node->ruleType;
	if(index>MAX_ACL_RULE_NUMBER-1) {
		npd_syslog_err("ERROR! The ruleIndex is in 1-%d!\n",MAX_ACL_RULE_NUMBER);
		return ACL_RETURN_CODE_RULE_INDEX_ERROR;
	}	
	ret=npd_acl_rule_index_check(index,type);
	if(ACL_RETURN_CODE_GLOBAL_EXISTED==ret) {
		npd_syslog_err("This node has already existed!\n");
		return ACL_RETURN_CODE_GLOBAL_EXISTED;
	}
	else if(ACL_RETURN_CODE_EXT_NO_SPACE==ret){
		return ACL_RETURN_CODE_EXT_NO_SPACE;
	}
/*	
	for(i=0;i<MAX_ACL_RULE_NUMBER;i++) {
		s0 = g_acl_rule[i];
		if(NULL!=s0) {
			tcamTmp   =&(s0->TcamPtr);
			actionTmp =&(s0->ActionPtr);
			stdIpv4L4Tmp=&(s0->TcamPtr.ruleInfo.stdIpv4L4);

			if((type==s0->ruleType)
				&&
				(stdIpv4L4Tmp->rule.UP==nodeStdIpv4L4->rule.UP)
				&&
				(stdIpv4L4Tmp->rule.DSCP==nodeStdIpv4L4->rule.DSCP)			
				&&
				(stdIpv4L4Tmp->mask.maskUP==nodeStdIpv4L4->mask.maskUP)
				&&
				(stdIpv4L4Tmp->mask.maskDSCP==nodeStdIpv4L4->mask.maskDSCP)
				&&
				(actionTmp->actionType==action->actionType)
				)			
			{
				npd_syslog_err("warning!same packet data eara can not be duplicatable!\n");
					return ACL_RETURN_CODE_SAME_FIELD;
			}
		}	
	}		*/
	g_acl_rule[node->ruleIndex]=node;
	return ACL_RETURN_CODE_SUCCESS;	
}

/**********************************************************************************
 *  npd_acl_add
 *
 *	DESCRIPTION:
 * 		store the matched (ip,tcp,udp,icmp)rule into acl struction
 *
 *	INPUT:
 *		*node - rule contact
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR    - if parameters given are wrong
 *		ACL_RETURN_CODE_SUCCESS  - if acl rule successfully stored 
 *		ACL_RETURN_CODE_GLOBAL_EXISTED- rule has not set yet
 *      ACL_RETURN_CODE_EXT_NO_SPACE  - no enough spaces for store
 *		ACL_RETURN_CODE_SAME_FIELD    - set same rule again,error 
 **********************************************************************************/
unsigned int npd_acl_add(struct ACCESS_LIST_STC *node)
{
	struct ACL_TCAM_STC 	*tcam=NULL;
	struct ACL_ACTION_STC   *action=NULL;
	ACL_RULE_STDIPV4L4_T 	*nodeStdIpv4L4=NULL;
	unsigned int 		 ret=0;
	unsigned long         index=0,type=0;
	
	if(NULL == node){
		npd_syslog_err("npd_acl_add:node null pointer error!\n");
		return ACL_RETURN_CODE_ERROR;
	}
	else {
		tcam			= &(node->TcamPtr);
		action			= &(node->ActionPtr);	
		nodeStdIpv4L4	= &(node->TcamPtr.ruleInfo.stdIpv4L4);
	}
	
	index=node->ruleIndex;
	type=node->ruleType;
	if(index>MAX_ACL_RULE_NUMBER-1) {
		npd_syslog_err("ERROR! The ruleIndex is in 1-%d!\n",MAX_ACL_RULE_NUMBER);
		return ACL_RETURN_CODE_RULE_INDEX_ERROR;
	}
			
	ret=npd_acl_rule_index_check(index,type);
	if(ACL_RETURN_CODE_GLOBAL_EXISTED==ret) {
		npd_syslog_err("This node has already existed!\n");
		return ACL_RETURN_CODE_GLOBAL_EXISTED;
	}
	else if(ACL_RETURN_CODE_EXT_NO_SPACE==ret) {
		return ACL_RETURN_CODE_EXT_NO_SPACE;
	}
/*	
	for(i=0;i<MAX_ACL_RULE_NUMBER;i++) {
		s0 = g_acl_rule[i];
		if(NULL!=s0) {
			tcamTmp   =&(s0->TcamPtr);
			actionTmp =&(s0->ActionPtr);
			stdIpv4L4Tmp=&(s0->TcamPtr.ruleInfo.stdIpv4L4);
			#if 0
			if(ACL_ACTION_TYPE_MIRROR_TO_ANALYZER==(action->actionType)){
				if(actionTmp->actionInfo.redirect.portNum==action->actionInfo.redirect.portNum)
				portInfo=1;		  
			}

			if(ACL_ACTION_TYPE_REDIRECT==(action->actionType)){
			    if(action->actionInfo.mirror.portNum==action->actionInfo.mirror.portNum)
					portInfo=1;
			}
			(actionTmp->actionType==action->actionType)
				&&		
				(portInfo==1)
			#endif	
		    if((type==s0->ruleType)
				&&
				(actionTmp->actionType==action->actionType)
				&&
				(tcamTmp->packetType==tcam->packetType)
				&&
				(stdIpv4L4Tmp->rule.Dip==nodeStdIpv4L4->rule.Dip)
				&&
			    (stdIpv4L4Tmp->mask.maskDip==nodeStdIpv4L4->mask.maskDip)
			    &&
			    (stdIpv4L4Tmp->rule.Sip==nodeStdIpv4L4->rule.Sip)
			    &&
			    (stdIpv4L4Tmp->mask.maskSip==nodeStdIpv4L4->mask.maskSip)
			    &&
			    (stdIpv4L4Tmp->rule.SrcPort==nodeStdIpv4L4->rule.SrcPort)
				&&
			    (stdIpv4L4Tmp->rule.DstPort==nodeStdIpv4L4->rule.DstPort)
				&&
			    (stdIpv4L4Tmp->rule.Code==nodeStdIpv4L4->rule.Code)
				&&
			    (stdIpv4L4Tmp->rule.Type==nodeStdIpv4L4->rule.Type) 			
				)			
			{
				npd_syslog_err("warning!same packet data eara can not be duplicatable!\n");
		    		return ACL_RETURN_CODE_SAME_FIELD;
			}
		}
	
	}
*/		
	g_acl_rule[node->ruleIndex]=node;
	return ACL_RETURN_CODE_SUCCESS;	

}

/*
npd_acl_drv_add_rule2group

*/
unsigned int npd_acl_drv_add_rule2group
(
	struct acl_group_list_s *groupInfo,
	unsigned int			add_index,
	unsigned int 			group_num,
	unsigned int 			dir_info
)
{
	unsigned int   op_info=ACL_RETURN_CODE_ERROR,result = ACL_RETURN_CODE_SUCCESS;	 
	struct acl_rule_info_s 	*ruleInfo = NULL;
	ACL_TCAM_RULE_FORMAT_E		ruleFormat;   

	ruleFormat = g_acl_rule[add_index]->TcamPtr.ruleFormat;
	npd_syslog_dbg("ruleFormat %d\n",ruleFormat);

	result = nam_acl_drv_tcam_set(group_num,add_index,ruleFormat,dir_info);
	if ( ACL_RETURN_CODE_SUCCESS != result) {
		npd_syslog_err("add write the tcam failure\n");
	}
	else if (ACL_RETURN_CODE_SUCCESS==result){
		op_info=ACL_RETURN_CODE_SUCCESS; 
		groupInfo->count+=1; /*count++*/
		groupInfo->stdrule = g_acl_rule[add_index]->ruleType;
		npd_syslog_dbg("groupInfo->count %d\n",groupInfo->count);
		ruleInfo=(struct acl_rule_info_s*)malloc(sizeof(struct acl_rule_info_s));
		if(NULL==ruleInfo) {   
			npd_syslog_dbg("ruleInfo malloc failure!\n");
			op_info = ACL_RETURN_CODE_ERROR;
		}
		else {
			memset(ruleInfo,0,sizeof(struct acl_rule_info_s)); 

			ruleInfo->ruleIndex=add_index;   
			/*INIT_LIST_HEAD(&(ruleInfo->list));*/
			npd_syslog_dbg("the add rule index is %ld\n",ruleInfo->ruleIndex);

			list_add_tail(&(ruleInfo->list),&(groupInfo->list1));
			npd_syslog_dbg("rule %d been added!\n",add_index);
		}
	}

	return op_info;
}

/*
npd_acl_drv_rule_action_check

*/
unsigned int npd_acl_drv_rule_action_check(unsigned int ruleIndex)
{
	unsigned int  actionT=0,ret = 0;
	
	ret=search_global_acl_rule(ruleIndex);
	if(ret==ACL_RETURN_CODE_SUCCESS)
	{
		actionT = g_acl_rule[ruleIndex]->ActionPtr.actionType;
		if((ACL_ACTION_TYPE_PERMIT==actionT) \
			||(ACL_ACTION_TYPE_DENY==actionT) \
			||(ACL_ACTION_TYPE_EGRESS_QOS==actionT)
			) {
			return ACL_RETURN_CODE_SUCCESS;
		}
		else {
			return ACL_RETURN_CODE_ERROR;
		}
	}
	else {
		return ACL_RETURN_CODE_ERROR;
	}
}

unsigned int npd_acl_drv_rule_in_action_check(unsigned int ruleIndex)
{
	unsigned int  actionT=0;
	if(g_acl_rule[ruleIndex]==NULL){
		return ACL_RETURN_CODE_GROUP_RULE_NOTEXISTED;
	}
	else{
		actionT = g_acl_rule[ruleIndex]->ActionPtr.actionType;
		if(ACL_ACTION_TYPE_EGRESS_QOS!=actionT) { 	
			return ACL_RETURN_CODE_SUCCESS;
		}
		else {
			return ACL_RETURN_CODE_ERROR;
		}
	}
}

unsigned int npd_acl_drv_ingress_rule_check(unsigned int ruleIndex, unsigned int groupnum)
{
	if((g_acl_rule[ruleIndex]->ruleType != acl_group[groupnum]->stdrule)&&(acl_group[groupnum]->count != 0)){
		return ACL_RETURN_CODE_STD_RULE;
	}
	return ACL_RETURN_CODE_SUCCESS;
}

unsigned int npd_acl_drv_egress_rule_check(unsigned int ruleIndex, unsigned int groupnum)
{
	if((g_acl_rule[ruleIndex]->ruleType != egress_acl_group[groupnum]->stdrule)&&(egress_acl_group[groupnum]->count != 0)){
		return ACL_RETURN_CODE_STD_RULE;
	}		
	return ACL_RETURN_CODE_SUCCESS;
}

unsigned int npd_acl_drv_time_rule_in_action_check(unsigned int ruleIndex)
{
	unsigned int  actionT=0;

	actionT = g_acl_rule[ruleIndex]->ActionPtr.actionType;
	if((ACL_ACTION_TYPE_PERMIT==actionT)||(ACL_ACTION_TYPE_DENY==actionT)) {
		return ACL_RETURN_CODE_SUCCESS;
	}
	else {
		return ACL_RETURN_CODE_ERROR;
	}
}

/**********************************************************************************
 *  npd_acl_add_notIp
 *
 *	DESCRIPTION:
 * 		store the matched rule (ethernet,arp) into acl struction
 *
 *	INPUT:
 *		*node - rule contact
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR  - if parameters given are wrong
 *		ACL_RETURN_CODE_SUCCESS- if acl rule successfully stored 
 *		
 **********************************************************************************/
unsigned int npd_acl_add_notIp(struct ACCESS_LIST_STC *node)
{
	struct ACL_TCAM_STC 	*tcam = NULL;
	struct ACL_ACTION_STC   *action = NULL;
	ACL_RULE_STDNOTIP_T 	*nodeStdNotIp = NULL;
	unsigned int 			ret = 0, index = 0, type=0;
	

	if(NULL == node){
		npd_syslog_err("npd_acl_add:node null pointer error!\n");
		return ACL_RETURN_CODE_ERROR;
	}
	else {
		tcam			= &(node->TcamPtr);
		action			= &(node->ActionPtr);	
		nodeStdNotIp	= &(node->TcamPtr.ruleInfo.stdNotIp);
	}
	
	index=node->ruleIndex;
	type=node->ruleType;
	if(index>MAX_ACL_RULE_NUMBER) {
		npd_syslog_err("ERROR! The ruleIndex is in 1 ~ %d!\n",MAX_ACL_RULE_NUMBER);
		return ACL_RETURN_CODE_ERROR;
	}
			
	ret=npd_acl_rule_index_check(index,type);
	if(ACL_RETURN_CODE_GLOBAL_EXISTED==ret)	{
		npd_syslog_err("This node has already existed!\n");
		return ACL_RETURN_CODE_GLOBAL_EXISTED;
	}
	else if(ACL_RETURN_CODE_EXT_NO_SPACE==ret){
		return ACL_RETURN_CODE_EXT_NO_SPACE;
	}
/*	
	for(i=0;i<MAX_ACL_RULE_NUMBER;i++) {
		s0 = g_acl_rule[i];
		if(NULL!=s0) {
			tcamTmp   =&(s0->TcamPtr);
			actionTmp =&(s0->ActionPtr);
			stdNotIpTmp=&(s0->TcamPtr.ruleInfo.stdNotIp);
			
			for(j=0;j<6;j++){
			      if((stdNotIpTmp->rule.dMac[j]==nodeStdNotIp->rule.dMac[j])
				  	&&
			 	    (stdNotIpTmp->rule.sMac[j]==nodeStdNotIp->rule.sMac[j]))
			 	   		count=count+1;				  
				}
			
		    if((count ==6)
				&&
				(type==s0->ruleType)
				&&
				(actionTmp->actionType==action->actionType)
				&&
				(tcamTmp->packetType==tcam->packetType)
				&&
				(stdNotIpTmp->rule.vlanId==nodeStdNotIp->rule.vlanId)
				&&
				(stdNotIpTmp->rule.portno==nodeStdNotIp->rule.portno)			
				){
				npd_syslog_err("warning! same packet data eara can not be duplicatable!\n");
		    		return ACL_RETURN_CODE_SAME_FIELD;
			}
		}
		count=0;
	}
*/		
	g_acl_rule[node->ruleIndex]=node;
	return ACL_RETURN_CODE_SUCCESS;	

}

/**********************************************************************************
 *  npd_acl_add_extNotIpv6
 *
 *	DESCRIPTION:
 * 		store the matched rule (extended ) into acl struction
 *
 *	INPUT:
 *		*node - rule contact
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR  - if parameters given are wrong
 *		ACL_RETURN_CODE_SUCCESS- if acl rule successfully stored 
 *		
 **********************************************************************************/
unsigned int npd_acl_add_extNotIpv6(struct ACCESS_LIST_STC *node)
{
	struct ACL_TCAM_STC 	*tcam = NULL;
	struct ACL_ACTION_STC	*action = NULL;
	ACL_RULE_EXTNOTIPV6_T	*nodeExtNotIpv6 = NULL;
	unsigned int 			ret = 0, index = 0,type=0;
	
	if(NULL == node){
		npd_syslog_dbg("npd_acl_add:node null pointer error!\n");
		return ACL_RETURN_CODE_ERROR;
	}
	else {
		tcam			= &(node->TcamPtr);
		action			= &(node->ActionPtr);	
		nodeExtNotIpv6= &(node->TcamPtr.ruleInfo.extNotIpv6);
	}
	
	index=node->ruleIndex;
	type=node->ruleType;
	if(index>MAX_ACL_RULE_NUMBER) {
		npd_syslog_err("ERROR! The ruleIndex is in 1-%d!\n",MAX_ACL_RULE_NUMBER);
		return ACL_RETURN_CODE_ERROR;
	}
			
	ret=npd_acl_rule_index_check(index,type);
	if(ACL_RETURN_CODE_GLOBAL_EXISTED==ret)	{
		npd_syslog_err("This node has already existed!\n");
		return ACL_RETURN_CODE_GLOBAL_EXISTED;
	}
	else if(ACL_RETURN_CODE_EXT_NO_SPACE==ret){
		return ACL_RETURN_CODE_EXT_NO_SPACE;
	}
/*	
	for(i=0;i<MAX_ACL_RULE_NUMBER;i++) {
		s0 = g_acl_rule[i];
		if(NULL!=s0) {
			tcamTmp   =&(s0->TcamPtr);
			actionTmp =&(s0->ActionPtr);
			extNotIpv6Tmp=&(s0->TcamPtr.ruleInfo.extNotIpv6);

			for(j=0;j<6;j++){
			      if((extNotIpv6Tmp->rule.dMac[j]==nodeExtNotIpv6->rule.dMac[j])&&
			 	  (extNotIpv6Tmp->rule.sMac[j]==nodeExtNotIpv6->rule.sMac[j]))
			 	   		count=count+1;				  
				}
		  if( (type==s0->ruleType)
			&&
		  	(tcamTmp->packetType==tcam->packetType)
				&&
				(actionTmp->actionType==action->actionType)
				&&
				(extNotIpv6Tmp->rule.Dip==nodeExtNotIpv6->rule.Dip)
				&&
			    (extNotIpv6Tmp->mask.maskDip==nodeExtNotIpv6->mask.maskDip)
			    &&
			    (extNotIpv6Tmp->rule.Sip==nodeExtNotIpv6->rule.Sip)
			    &&
			    (extNotIpv6Tmp->mask.maskSip==nodeExtNotIpv6->mask.maskSip)
			    &&
			    (extNotIpv6Tmp->rule.SrcPort==nodeExtNotIpv6->rule.SrcPort)
				&&
			    (extNotIpv6Tmp->rule.DstPort==nodeExtNotIpv6->rule.DstPort)				
				&&
                (count==6)
                &&
                (extNotIpv6Tmp->rule.vlanId==nodeExtNotIpv6->rule.vlanId)
				&&
				(extNotIpv6Tmp->rule.portNo==nodeExtNotIpv6->rule.portNo)
             )			
			{
				npd_syslog_err("warning!same packet data eara can not be duplicatable!\n");
		    		return ACL_RETURN_CODE_SAME_FIELD;
			}		
			#if 0
			&&
                (extNotIpv6Tmp->rule.vlanId==nodeExtNotIpv6->rule.vlanId)
				&&
				(extNotIpv6Tmp->rule.portNo==nodeExtNotIpv6->rule.portNo)
			#endif
		}
		count=0;
	}
*/		
	g_acl_rule[node->ruleIndex]=node;
	return ACL_RETURN_CODE_SUCCESS;	

}


/**********************************************************************************
 *  npd_acl_nodesearch
 *
 *	DESCRIPTION:
 * 		check acl rule struction if null or not
 *
 *	INPUT:
 *		NULL
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR  - no any acl rule in  struction
 *		ACL_RETURN_CODE_SUCCESS- if has rule set in struction 
 *		
 **********************************************************************************/
unsigned int npd_acl_nodesearch()
{
    int i= 0,rs = ACL_RETURN_CODE_ERROR;
	for(i=0; i < MAX_ACL_RULE_NUMBER; i++) {
		if(g_acl_rule[i]!=NULL) {
			rs =ACL_RETURN_CODE_SUCCESS;                   
			return rs; 
        	}
	}
	return rs;
}

/**********************************************************************************
 *  npd_ip_long2str
 *
 *	DESCRIPTION:
 * 		convert such as <ABCD> to <A.B.C.D>
 *
 *	INPUT:
 *		ipAddress - <ABCD>
 *      
 *	
 *	OUTPUT:
 *		buff - <A.B.C.D>
 *
 * 	RETURN:
 *		cnt  - <A.B.C.D>
 *		
 **********************************************************************************/
unsigned int npd_ip_long2str(unsigned long ipAddress,unsigned char **buff)
{
	unsigned long	cnt= 0;
	unsigned char *tmpPtr = *buff;

	cnt = sprintf((char*)tmpPtr,"%ld.%ld.%ld.%ld",(ipAddress>>24) & 0xFF, \
			(ipAddress>>16) & 0xFF,(ipAddress>>8) & 0xFF,ipAddress & 0xFF);
	
	return cnt;
}

/**********************************************************************************
 *  npd_acl_extNotIpv6_display
 *
 *	DESCRIPTION:
 * 		display rule <extended > detail
 *
 *	INPUT:
 *		s0 - specifial acl rule struction
 *      
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS  - successfully display
 *		
 **********************************************************************************/
unsigned int npd_acl_extNotIpv6_display(struct ACCESS_LIST_STC *s0)
{
	ACL_RULE_EXTNOTIPV6_T *extNotIpv6 = NULL;	
	int 			i = 0, rs = 0;
	char			actype[50];
	ETHERADDR		dmacAddr,smacAddr;

	extNotIpv6= &(s0->TcamPtr.ruleInfo.extNotIpv6);
	rs = s0->ActionPtr.actionType;

	switch(rs) {
		case 0:strcpy(actype,"Permit"); 			break;
		case 1:strcpy(actype,"Deny");				break;
		case 2:strcpy(actype,"TrapToCpu");			break;
		case 3:strcpy(actype,"MirrorToAnalyzer");	break;
		case 4:strcpy(actype,"Redirect");			break;
		default: 
				break;		
	}
	   
	for(i=0;i<6;i++){
		dmacAddr.arEther[i]= extNotIpv6->rule.dMac[i];
		smacAddr.arEther[i]= extNotIpv6->rule.sMac[i];
	}
	npd_syslog_dbg("**************************************************\n"); 
	npd_syslog_dbg("%-40s: %ld\n","access-list index",(s0->ruleIndex)+1);
	npd_syslog_dbg("%-40s: %s\n","action",actype);
	npd_syslog_dbg("%-40s: %0x:%0x:%0x:%0x:%0x:%0x\n","destination MAC address",dmacAddr.arEther[0],
												dmacAddr.arEther[1],
												dmacAddr.arEther[2],
												dmacAddr.arEther[3],
												dmacAddr.arEther[4],
												dmacAddr.arEther[5]);
	npd_syslog_dbg("%-40s: %0x:%0x:%0x:%0x:%0x:%0x\n","source MAC address",smacAddr.arEther[0],
												smacAddr.arEther[1],
												smacAddr.arEther[2],
												smacAddr.arEther[3],
												smacAddr.arEther[4],
												smacAddr.arEther[5]);
	npd_syslog_dbg("%-40s: %d\n","vlan Id",extNotIpv6->rule.vlanId);
	npd_syslog_dbg("%-40s: %d\n","source port",extNotIpv6->rule.portNo);
	npd_syslog_dbg("**************************************************\n");
	
	return ACL_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_acl_notIp_display
 *
 *	DESCRIPTION:
 * 		display rule <not ip > detail
 *
 *	INPUT:
 *		s0 - specifial acl rule struction
 *      
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS  - successfully display
 *		
 **********************************************************************************/
unsigned int npd_acl_notIp_display(struct ACCESS_LIST_STC *s0)
{
	ACL_RULE_STDNOTIP_T *stdNotIp=NULL;	
	int             i = 0, rs = 0;
	char 			actype[50];
	ETHERADDR       dmacAddr,smacAddr;

	stdNotIp= &(s0->TcamPtr.ruleInfo.stdNotIp);
	rs = s0->ActionPtr.actionType;
	
	switch(rs) {
		case 0:strcpy(actype,"Permit");	      		break;
		case 1:strcpy(actype,"Deny");	  			break;
		case 2:strcpy(actype,"TrapToCpu");			break;
		case 3:strcpy(actype,"MirrorToAnalyzer");	break;
		case 4:strcpy(actype,"Redirect");			break;
		default: 
			  	break;		
	}
   
	for(i=0;i<6;i++){
		dmacAddr.arEther[i]= stdNotIp->rule.dMac[i];
		smacAddr.arEther[i]= stdNotIp->rule.sMac[i];
	}
	
	npd_syslog_dbg("**************************************************\n");	
	npd_syslog_dbg("%-40s: %ld\n","access-list index",(s0->ruleIndex)+1);
	npd_syslog_dbg("%-40s: %s\n","action",actype);
	npd_syslog_dbg("%-40s: %d\n","vlan Id",stdNotIp->rule.vlanId);
	npd_syslog_dbg("%-40s: %d\n","source port",stdNotIp->rule.portno);
	npd_syslog_dbg("%-40s: %0x:%0x:%0x:%0x:%0x:%0x\n","destination MAC address",dmacAddr.arEther[0],
												dmacAddr.arEther[1],
												dmacAddr.arEther[2],
												dmacAddr.arEther[3],
												dmacAddr.arEther[4],
												dmacAddr.arEther[5]);
	npd_syslog_dbg("%-40s: %0x:%0x:%0x:%0x:%0x:%0x\n","source MAC address",smacAddr.arEther[0],
												smacAddr.arEther[1],
												smacAddr.arEther[2],
												smacAddr.arEther[3],
												smacAddr.arEther[4],
												smacAddr.arEther[5]);
	npd_syslog_dbg("**************************************************\n");
	
	return ACL_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_acl_isIp_display
 *
 *	DESCRIPTION:
 * 		display rule <ip,tcp,udp,icmp ..> detail
 *
 *	INPUT:
 *		s0 - specifial acl rule struction
 *      
 *	
 *	OUTPUT:
 *		
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_SUCCESS  - successfully display
 *		
 **********************************************************************************/
unsigned int npd_acl_isIp_display(int ts,struct ACCESS_LIST_STC *s0)
{
	ACL_RULE_STDIPV4L4_T *stdIpv4L4 = NULL;	
	int             rs = 0;
	char 			actype[50],protype[10];

	unsigned char sipBuf[MAX_IP_STRLEN] = {0};
	unsigned char dipBuf[MAX_IP_STRLEN] = {0};
	
	unsigned char *sipPtr = sipBuf,*dipPtr = dipBuf;	

	stdIpv4L4 = &(s0->TcamPtr.ruleInfo.stdIpv4L4);
	rs = s0->ActionPtr.actionType;
	
	switch(rs) {
		case 0:strcpy(actype,"Permit");	      		break;
		case 1:strcpy(actype,"Deny");	  			break;
		case 2:strcpy(actype,"TrapToCpu");			break;
		case 3:strcpy(actype,"MirrorToAnalyzer");	break;
		case 4:strcpy(actype,"Redirect");			break;
		default: 
			  	break;		
	}
	switch(ts) {
		case 0:strcpy(protype,"IP");	   	break;
		case 1:strcpy(protype,"UDP");	  	break;
		case 2:strcpy(protype,"TCP");		break;
		case 3:strcpy(protype,"ICMP");	    break;			
		default:
				  break;							  		
	}	
	npd_ip_long2str(stdIpv4L4->rule.Dip,&dipPtr);
	npd_ip_long2str(stdIpv4L4->rule.Sip,&sipPtr);
	npd_syslog_dbg("**************************************************\n");
	npd_syslog_dbg("%-40s: %ld\n","access-list index",(s0->ruleIndex)+1);
	npd_syslog_dbg("%-40s: %s\n","action",actype);
	npd_syslog_dbg("%-40s: %s\n","ip protocal",protype);
	npd_syslog_dbg("%-40s: %s/%ld\n","destination IP address",dipPtr,stdIpv4L4->mask.maskLenDip);
	npd_syslog_dbg("%-40s: %ld\n","destination port",stdIpv4L4->rule.DstPort);	
	npd_syslog_dbg("%-40s: %s/%ld\n","source IP address",sipPtr,stdIpv4L4->mask.maskLenSip);
	npd_syslog_dbg("%-40s: %ld\n","source port",stdIpv4L4->rule.SrcPort);
	npd_syslog_dbg("%-40s: %d\n","icmp code",stdIpv4L4->rule.Code);
	npd_syslog_dbg("%-40s: %d\n","icmp type",stdIpv4L4->rule.Type);	
	npd_syslog_dbg("**************************************************\n");
	
	return ACL_RETURN_CODE_SUCCESS;
}

/**********************************************************************************
 *  npd_acl_delete
 *
 *	DESCRIPTION:
 * 		delete specifial acl rule
 *
 *	INPUT:
 *		index - delete index
 *      
 *	
 *	OUTPUT:
 *		group_num - if rule <index> be added in the acl group ,return group num
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR               - para error
 *		ACL_RETURN_CODE_GROUP_RULE_NOTEXISTED    - rule <index> has not been set
 *		ACL_RETURN_CODE_GROUP_RULE_EXISTED       - rule <index> has been added in group
 *		
 **********************************************************************************/
unsigned int npd_acl_delete(unsigned long index,unsigned int *group_num)
{
	unsigned int 	num=0,ret = 0, exist_dir = 0;
	int rs=0;
	
	if(index>=MAX_ACL_RULE_NUMBER)
	{
		npd_syslog_err("ERROR!\n");
		return ACL_RETURN_CODE_ERROR;
	}
	else if(NULL==g_acl_rule[index])
	{
		npd_syslog_err("rule %ld not existed!\n",index);
		return ACL_RETURN_CODE_GLOBAL_NOT_EXISTED;
	}			
	else {
		 	ret=search_acl_group_rule_index_exist(index,&num,&exist_dir);

			if((ACL_RETURN_CODE_GROUP_RULE_EXISTED == ret)||(ACL_RETURN_CODE_EGRESS_GROUP_RULE_EXISTED == ret))
			{
				*group_num = num;
			}
	
			else if(ACL_RETURN_CODE_SUCCESS==ret)
			{
				rs=npd_mirror_src_acl_check(0,index);
				
				if(rs==NPD_TRUE)
				{
					npd_syslog_dbg("use in mirror!\n");
					return ACL_RETURN_CODEL_MIRROR_USE;
				}
				else{
					free(g_acl_rule[index]);
					g_acl_rule[index]=NULL;
					npd_syslog_dbg("ACL rule %ld is deleted!\n",(index+1));
				}
			}
	}
						
    return ret;

}

/*npd_vlan_unbind_group
NPD_VLAN_NOTEXISTS
ACL_RETURN_CODE_GROUP_NOT_EXISTED //no group
ACL_RETURN_CODE_SUCCESS	//delete success
ACL_RETURN_CODE_ERROR    //wrong gourp id
*/
unsigned int npd_vlan_unbind_group
(
	unsigned short vlanId,
	unsigned int group_num,
	unsigned int dir_info
)
{
	struct vlan_s		    *node=NULL;
	struct vlan_acl_info_s  *aclPtr=NULL;
	struct group_vlan_info	*gvidinfo=NULL;
	struct acl_group_list_s	*groupPtr=NULL;
	struct list_head		 *ptr=NULL;
	unsigned int 			ret=ACL_RETURN_CODE_ERROR;

	node=npd_find_vlan_by_vid(vlanId);
	if(NULL==node)
		ret= NPD_VLAN_NOTEXISTS;
	else{
		aclPtr=node->aclList;
		if(NULL!=aclPtr){

			if(dir_info==ACL_DIRECTION_INGRESS){
				  npd_syslog_dbg("aclPtr->IsEnable %d\n",aclPtr->IsEnable);
				  /*notice :if acl disable,but group remain*/
				 				  
					npd_syslog_dbg("aclPtr->groupId %d\n",aclPtr->groupId);
					if(group_num==(aclPtr->groupId)){
						aclPtr->groupId = 0;
						/*tong bu */
						groupPtr = acl_group[group_num];
						__list_for_each(ptr,&(groupPtr->list3)) {
							gvidinfo=list_entry(ptr, struct group_vlan_info, list); 
							if(NULL != gvidinfo){
								if(gvidinfo->vlan_index == vlanId) {
									__list_del((gvidinfo->list).prev, (gvidinfo->list).next);
									break;
								}
								syslog_ax_acl_dbg("group been unbind by vlan %d \n", vlanId);
							}				  
							else
								syslog_ax_acl_dbg(" port have not group %d\n", group_num);
						}
						acl_group[group_num]->vlan_count-=1;
						npd_syslog_dbg("delete sw groupId success\n");
						ret =ACL_RETURN_CODE_SUCCESS;
					}
					else{
						npd_syslog_dbg("wrong groupId\n");
						ret = ACL_RETURN_CODE_GROUP_WRONG_INDEX;
					}			
			}
			else if(dir_info==ACL_DIRECTION_EGRESS){
				   npd_syslog_dbg("aclPtr->IsEnable %d\n",aclPtr->IsEgrEnable);				 
				   npd_syslog_dbg("aclPtr->EgrGroupId %d\n",aclPtr->EgrGroupId);
					if(group_num==(aclPtr->EgrGroupId)){
						aclPtr->EgrGroupId = 0;
						/*tong bu*/
						groupPtr = egress_acl_group[group_num];
						__list_for_each(ptr,&(groupPtr->list3)) {
							gvidinfo=list_entry(ptr, struct group_vlan_info, list); 
							if(NULL != gvidinfo){
								if(gvidinfo->vlan_index == vlanId) {
									__list_del((gvidinfo->list).prev, (gvidinfo->list).next);
									break;
								}
								syslog_ax_acl_dbg("group been unbind by vlan %d \n", vlanId);
							}				  
							else
								syslog_ax_acl_dbg(" port have not group %d\n", group_num);
						}
						egress_acl_group[group_num]->vlan_count-=1;
						npd_syslog_dbg("delete sw groupId success\n");
						ret =ACL_RETURN_CODE_SUCCESS;
					}
					else{
						npd_syslog_err("wrong groupId\n");
						ret = ACL_RETURN_CODE_GROUP_WRONG_INDEX;
					}			
				  
			}		

		}
	}
	return ret;
}

/*
	npd_vlan_acl_enable_check
*/
unsigned int npd_vlan_acl_enable_check
(
	unsigned short vlanId,
	unsigned int dir_info
)
{
	struct vlan_s			*node=NULL;
	struct vlan_acl_info_s	*aclPtr=NULL;
	unsigned int			 ret=ACL_RETURN_CODE_SUCCESS;

	node=npd_find_vlan_by_vid(vlanId);
	if(NULL!=node){
		aclPtr=node->aclList;
		if(NULL!=aclPtr){
		 
		    if(dir_info==ACL_DIRECTION_INGRESS){								
			  if(ACL_TRUE!=aclPtr->IsEnable){		  
		 			ret = ACL_RETURN_CODE_ERROR;
					npd_syslog_err("in has been disabled\n");
			  }
		    }
		    else if(dir_info==ACL_DIRECTION_EGRESS){				
			  if(ACL_TRUE!=aclPtr->IsEgrEnable){			  	
		 		    ret = ACL_RETURN_CODE_ERROR;
					npd_syslog_err("out has been disabled\n");
				}		  	  
	        }
	     }
	    else if(NULL==aclPtr){
			ret = ACL_RETURN_CODE_ERROR;
			npd_syslog_err("acl never enable or not\n");
	     }
		
	}
	else if(NULL==node){
		ret = NPD_VLAN_NOTEXISTS;
		npd_syslog_err("vlan not exist\n");
	}

    return ret;
}

/*npd_vlan_acl_bind_check
ACL_RETURN_CODE_GROUP_VLAN_BINDED	//bind,ignore acl enable or not
ACL_RETURN_CODE_SUCCESS		//acl----enable ,no group binded
ACL_RETURN_CODE_ERROR			//acl----disable,no group binded
NPD_VLAN_NOTEXISTS		//vlan error
*/
unsigned int npd_vlan_acl_bind_check
(
	unsigned short vlanId,
	unsigned int   *groupNo,
	unsigned int   dir_info
)
{
	struct vlan_s		    *node=NULL;
	struct vlan_acl_info_s  *aclPtr=NULL;
	unsigned int 			 ret=ACL_RETURN_CODE_ERROR;

	node=npd_find_vlan_by_vid(vlanId);
	if(NULL!=node){
		aclPtr=node->aclList;
		if(NULL!=aclPtr){

		  npd_syslog_dbg("aclPtr->IsEnable %d\n",aclPtr->IsEnable);
		  npd_syslog_dbg("aclPtr->IsEgrEnable %d\n",aclPtr->IsEgrEnable);
		  if(NULL==aclPtr){
			ret = ACL_RETURN_CODE_ERROR;
			npd_syslog_err("acl never enable or not\n");
		  }
		  else if(((aclPtr->IsEnable == 0)&&(dir_info==ACL_DIRECTION_INGRESS))
		  		||((aclPtr->IsEgrEnable== 0)&&(dir_info==ACL_DIRECTION_EGRESS))) {
		  	ret = ACL_RETURN_CODE_ON_PORT_DISABLE;
			npd_syslog_err("acl vlan serice disabled \n");
		  }
		  else if(dir_info==ACL_DIRECTION_INGRESS){
				if(ACL_TRUE==aclPtr->IsEnable){
				/*ret = ACL_RETURN_CODE_SUCCESS;*/
				npd_syslog_dbg("has been enabled!\n");
				/*notice:if acl disabled .but group remain */
				if(0!=(node->aclList->groupId)){
				   ret = ACL_RETURN_CODE_GROUP_VLAN_BINDED;
				   *groupNo=node->aclList->groupId;
				   npd_syslog_dbg("*groupNo %d\n",*groupNo);
				}
				else
					ret = ACL_RETURN_CODE_SUCCESS;
			  }
				
			  else if(ACL_FALSE==aclPtr->IsEnable){
			  	if(0!=(node->aclList->groupId)){
				   ret = ACL_RETURN_CODE_GROUP_VLAN_BINDED;
				   *groupNo=node->aclList->groupId;
				   npd_syslog_dbg("*groupNo %d\n",*groupNo);
				}
				else{
		 			ret = ACL_RETURN_CODE_ERROR;
					npd_syslog_err("has been disabled\n");
				}
			  }

		  }
		  else if(dir_info==ACL_DIRECTION_EGRESS){
				if(ACL_TRUE==aclPtr->IsEgrEnable){
				/*ret = ACL_RETURN_CODE_SUCCESS;*/
				npd_syslog_dbg("egress has been enabled!\n");
				if(0!=(node->aclList->EgrGroupId)){
				   ret = ACL_RETURN_CODE_GROUP_VLAN_BINDED;
				   *groupNo=node->aclList->EgrGroupId;
				   npd_syslog_dbg("*groupNo %d\n",*groupNo);
				}
				else
					ret = ACL_RETURN_CODE_SUCCESS;
			  }
			  else if(ACL_FALSE==aclPtr->IsEgrEnable){
			  	if(0!=(node->aclList->EgrGroupId)){
				   ret = ACL_RETURN_CODE_GROUP_VLAN_BINDED;
				   *groupNo=node->aclList->EgrGroupId;
				   npd_syslog_dbg("*groupNo %d\n",*groupNo);
				}
				else{
			 		ret = ACL_RETURN_CODE_ERROR;
					npd_syslog_err("has been disabled\n");
				}
			  }
		  }		  
	    }
	}
	else if(NULL==node) {
		ret = NPD_VLAN_NOTEXISTS;
	}

    return ret;
}

/*
	npd_vlan_acl_enable
*/
unsigned int npd_vlan_acl_enable
(
	unsigned short  vlanId,
	unsigned int 	acl_enable,
	unsigned int 	dir_info
)
{
	struct vlan_acl_info_s  *aclPtr=NULL,*vaclptr=NULL;
	struct vlan_s			*node=NULL;
	unsigned int            ret=ACL_RETURN_CODE_ERROR;

	node=npd_find_vlan_by_vid(vlanId);

	if(NULL!=node) 
	{
		aclPtr=node->aclList;
		
		if(NULL==aclPtr) 
		{
			if(NULL==(vaclptr=(struct vlan_acl_info_s *)malloc(sizeof(struct vlan_acl_info_s)))) 
			{
				npd_syslog_err("malloc fail!\n");
				return ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
			}		
			else
			{
				memset(vaclptr,0,sizeof(struct vlan_acl_info_s));

				if(dir_info==ACL_DIRECTION_INGRESS) 
				{
					vaclptr->IsEnable=acl_enable;
					npd_syslog_dbg("vaclptr->IsEnable %d\n",vaclptr->IsEnable);
				}
				else if(dir_info==ACL_DIRECTION_EGRESS)
				{
					vaclptr->IsEgrEnable=acl_enable;
					npd_syslog_dbg("vaclptr->IsEgrEnable %d\n",vaclptr->IsEgrEnable);
				}
				else 
				{ 
					/*code optimize:  Resource leak
					zhangcl@autelan.com 2013-1-17*/
					free(vaclptr);
					return ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
				}

				node->aclList = vaclptr;

				npd_syslog_dbg("vid %d,node->aclList %p,aclPtr %p,vaclptr->IsEnable %d,vaclptr->IsEgrEnable %d\n",\
					vlanId,node->aclList,aclPtr,node->aclList->IsEnable,node->aclList->IsEgrEnable);

				ret =ACL_RETURN_CODE_SUCCESS;
			}	
		}
		else if(NULL!=aclPtr)
		{
			if(dir_info==ACL_DIRECTION_INGRESS)
			{
				    aclPtr->IsEnable=acl_enable;					
					npd_syslog_dbg("aclPtr->IsEnable %d\n",aclPtr->IsEnable);
					ret=ACL_RETURN_CODE_SUCCESS;				
			}
			else if(dir_info==ACL_DIRECTION_EGRESS)
			{
				    aclPtr->IsEgrEnable=acl_enable;					
					npd_syslog_dbg("aclPtr->IsEgrEnable%d\n",aclPtr->IsEgrEnable);	
					ret=ACL_RETURN_CODE_SUCCESS;				
			}			
		}
	}
	else if(NULL==node) 
	{
		return NPD_VLAN_NOTEXISTS;
	}		
	return ret;
}

/**
	npd_drv_vlan_acl_enable
*/
unsigned int npd_drv_vlan_acl_enable
(
	unsigned int eth_g_index,
	unsigned int acl_enable,
	unsigned short vlanId,
	unsigned int dir_info
)
{
	unsigned int port_info = 0,temp=ACL_RETURN_CODE_SUCCESS;
	unsigned char devNum = 0,virPortNo = 0;
	
	port_info = NPD_DBUS_ERROR_NO_SUCH_PORT;

	npd_syslog_dbg("eth_g_index : %d ,acl_enable :%d\n",eth_g_index,acl_enable);

	port_info = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNo);
	if(port_info != 0) {
		npd_syslog_err("Npd_dbus::npd_get_devport_by_global_index::get devport Error.\n");
		return port_info;
	}
	else if(port_info==0) {
		npd_syslog_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortNo);
		npd_syslog_dbg("before set ,para :acl_enable %d\n",acl_enable);
		
		npd_syslog_dbg("enable every port acl\n");
			/* --------need to repair ,becase input has no dir_info---------------*/
		if(dir_info==ACL_DIRECTION_INGRESS){
			temp = nam_acl_port_acl_enable((unsigned char)devNum,virPortNo,acl_enable,ACL_DIRECTION_INGRESS, g_acl_enable);
			if(temp!=ACL_RETURN_CODE_SUCCESS) {				
				npd_syslog_err("port ingress acl fail\n");
			}
		}
		else if(dir_info==ACL_DIRECTION_EGRESS){
			temp = nam_acl_port_acl_enable((unsigned char)devNum,virPortNo,acl_enable,ACL_DIRECTION_EGRESS, g_acl_enable);

			if(temp!=ACL_RETURN_CODE_SUCCESS) {				
				npd_syslog_err("port egress acl fail\n");
			}	
		}
	 }

	return temp;
}

/*
npd_drv_port_acl_enable

*/
unsigned int npd_drv_port_acl_enable
(
	unsigned int eth_g_index,
	unsigned int acl_enable,
	unsigned int dir_info
)
{
	unsigned int port_info = 0,portEnable,temp = ACL_RETURN_CODE_SUCCESS;
	unsigned char devNum = 0,virPortNo = 0;
	enum product_id_e productId = PRODUCT_ID; 
	int isBoard = NPD_FALSE;/* 1 -if main board port, 0 - slave board port*/ 	
	
	port_info = NPD_DBUS_ERROR_NO_SUCH_PORT;

	npd_syslog_dbg("eth_g_index : %d ,acl_enable :%d\n",eth_g_index,acl_enable);

	port_info = npd_get_devport_by_global_index(eth_g_index,&devNum,&virPortNo);
	if(port_info != 0) {
		if(((PRODUCT_ID_AX7K_I == productId)||(PRODUCT_ID_AX7K == productId))
			&& (eth_g_index < 4)) {
			isBoard = NPD_TRUE;
			return ACL_PORT_NOT_SUPPORT_BINDED;
		}
		else if((PRODUCT_ID_AX5K_I == productId) && ((eth_g_index >= 8) && (eth_g_index <= 11))) {
			isBoard = NPD_TRUE;
			return ACL_PORT_NOT_SUPPORT_BINDED;
		}
		else {
			npd_syslog_dbg("Npd_dbus::npd_get_devport_by_global_index::get devport Error.\n");
			return port_info;
		}
	}
	else if(port_info==0) {
		npd_syslog_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n",devNum,virPortNo);
		portEnable = eth_port_acl_enable(eth_g_index,acl_enable,dir_info);
		if(ACL_RETURN_CODE_SUCCESS==portEnable) {			
			temp = nam_acl_port_acl_enable((unsigned char)devNum,virPortNo,acl_enable,dir_info,g_acl_enable);
			if(temp!=ACL_RETURN_CODE_SUCCESS) {				
				 npd_syslog_err("port acl fail\n");	
			}
		}
		else {
			temp =ACL_RETURN_CODE_ERROR;
		}
	 }

	return temp;
}

/**********************************************************************************
 *  npd_drv_bind_acl_group
 *
 *	DESCRIPTION:
 * 		bind acl group on ethport port or vlan
 *
 *	INPUT:
 *		devNum      - device number
 *		portId      - bind on port <id>
 *		group_num   - bind acl group <num>
 *		eth_g_index - port global index
 *		access_type - access pcl configure table mode 
 *      			   - ACCESS_PORT_TYPE  port
 *					   - ACCESS_VID_TYPE   vlan
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *      ACL_GROUP_ERROR_NONE	     - cfg table set fail
 *      ACL_RETURN_CODE_SUCCESS			 - cfg table set success
 **********************************************************************************/		
unsigned int npd_drv_bind_acl_group
(
	unsigned char devNum,
	unsigned char portId,
	unsigned int  aclId,
	unsigned int  g_index,
	unsigned int  access_type,
	unsigned short vlanId,
	unsigned int dir_info,
	unsigned int stdrule
)
{ 
	unsigned int	op_ret = 0, cfg_tmp = ACL_RETURN_CODE_SUCCESS;
	struct group_eth_info	*gethinfo=NULL;
	struct list_head					*ptr = NULL;
	unsigned int 			eth_g_index = 0;
	struct acl_group_list_s	*groupPtr = NULL;
	unsigned char DEVNum = 0, virPortId = 0;
	unsigned int  pbmptmp[2] = {0, 0}; /* find a macro dev num to change 2*/
	int groupdev = 0;
	
	if(dir_info == ACL_DIRECTION_INGRESS) 
	{ 		
		groupPtr = acl_group[aclId];
		groupdev = groupPtr->devnum;
	}
	else if(dir_info == ACL_DIRECTION_EGRESS) 
	{
		groupPtr = egress_acl_group[aclId];
		groupdev = groupPtr->devnum;
	}
	else
	{
		/*code optimize:    Explicit null dereferenced
		zhangcl@autelan.com 2013-1-17*/
		return ACL_RETURN_CODE_ERROR;
	}

	npd_syslog_dbg("old group dev  %d \n", groupdev);

	__list_for_each(ptr,&(groupPtr->list2)) 
	{
		gethinfo=list_entry(ptr,struct group_eth_info,list);	
		if (NULL != gethinfo) 
		{
			eth_g_index = gethinfo->eth_index;
			op_ret = npd_get_devport_by_global_index(eth_g_index,&DEVNum,&virPortId);
			if (op_ret != 0) 
			{
				op_ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
				npd_syslog_dbg("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
			}
			else 
			{
				/*groupPtr->devnum &= ACL_DEFAULT_DEVID^(1<<DEVNum);*/
				/*if (portId != virPortId) {*/
				NPD_PBMP_PORT_ADD(pbmptmp[DEVNum], virPortId);
				npd_syslog_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n", DEVNum, virPortId);
						
			}
		}			  
	}

	if (0 != pbmptmp[0]) 
	{
		groupPtr->devnum &= ACL_DEFAULT_DEVID^(1<<0);
	}
	if (0 != pbmptmp[1]) 
	{
		groupPtr->devnum &= ACL_DEFAULT_DEVID^(1<<1);
	}
	
	npd_syslog_dbg("new group dev  %d \n", groupPtr->devnum);
	if (ACL_DEFAULT_DEVID != groupdev) 
	{
		cfg_tmp = nam_bcm_acl_drv_tcam_clear(aclId, groupdev);
		if(0!=cfg_tmp)
		{
			npd_syslog_err("set cfg table fail!\n");
			op_ret = ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
		}
	}
	npd_syslog_dbg("npd_drv_bind_acl_group pbmptmp[0] %d \n", pbmptmp[0]);
	npd_syslog_dbg("npd_drv_bind_acl_group pbmptmp[1] %d \n", pbmptmp[1]);
	
	if(access_type == ACCESS_PORT_TYPE) 
	{
		cfg_tmp = nam_acl_drv_cfg_table_set(devNum,portId,aclId,ACL_TRUE,ACCESS_PORT_TYPE,0,dir_info,stdrule);
		if(0 != cfg_tmp) 
		{
			npd_syslog_err("set cfg table fail!\n");
			op_ret = ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
		}		
		else if(ACL_RETURN_CODE_SUCCESS==cfg_tmp) 
		{			
			op_ret = ACL_RETURN_CODE_SUCCESS;
		}
		cfg_tmp = nam_bcm_acl_drv_tcam_set(aclId, groupPtr->devnum, pbmptmp);
	}					   
	else if (access_type == ACCESS_VID_TYPE) 
	{	 								
		cfg_tmp =nam_acl_drv_cfg_table_set(devNum,portId,aclId,ACL_TRUE,ACCESS_VID_TYPE,vlanId,dir_info,stdrule);
		if(0!=cfg_tmp) 
		{
			npd_syslog_err("set cfg table fail!\n");
			op_ret = ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
		}
		else if(ACL_RETURN_CODE_SUCCESS == cfg_tmp) 
		{			
			op_ret = ACL_RETURN_CODE_SUCCESS;
		}

	} /*ACCESS_VID_TYPE*/

	return op_ret;	  
} 

unsigned int npd_drv_unbind_acl_group
(
	unsigned char devNum,
	unsigned char portId,
	unsigned int  aclId,
	unsigned int  g_index,
	unsigned int  access_type,
	unsigned short vlanId,
	unsigned int dir_info,
	unsigned int stdrule
)
{ 
	unsigned int	op_ret = 0,cfg_tmp = ACL_RETURN_CODE_SUCCESS;
	struct group_eth_info	*gethinfo=NULL;
	struct list_head					*ptr = NULL;
	unsigned int 			groupdev = 0, eth_g_index = 0;
	struct acl_group_list_s	*groupPtr = NULL;
	unsigned char DEVNum = 0, virPortId = 0;
	unsigned int  pbmptmp[2] = {0, 0};

	if(dir_info == ACL_DIRECTION_INGRESS) 
	{ 		
		groupPtr = acl_group[aclId];
		groupdev = groupPtr->devnum;
	}
	else if(dir_info == ACL_DIRECTION_EGRESS) 
	{
		groupPtr = egress_acl_group[aclId];
		groupdev = groupPtr->devnum;
	}
	else
	{
		/*code optimize:    Explicit null dereferenced
		zhangcl@autelan.com 2013-1-17*/
		return ACL_RETURN_CODE_ERROR;
	}

	npd_syslog_dbg("old group dev  %d \n", groupdev);

	__list_for_each(ptr,&(groupPtr->list2)) 
	{
		gethinfo=list_entry(ptr,struct group_eth_info,list);	
		if(NULL != gethinfo) 
		{
			eth_g_index = gethinfo->eth_index;
			op_ret = npd_get_devport_by_global_index(eth_g_index,&DEVNum,&virPortId);
			if(op_ret != 0) 
			{
				op_ret = NPD_DBUS_ERROR_NO_SUCH_PORT;
				npd_syslog_dbg("Npd_acl::npd_get_devport_by_global_index::get devport Error.\n");
			}
			else 
			{
				if ((portId == virPortId) && (devNum == DEVNum))
				{
					continue;
				}
				/*groupPtr->devnum &= ACL_DEFAULT_DEVID^(1<<DEVNum);*/
				NPD_PBMP_PORT_ADD(pbmptmp[DEVNum], virPortId);
				npd_syslog_dbg("npd_get_devport_by_global_index::devNum %d,portNum %d\n", DEVNum, virPortId);		
			}
		}
		npd_syslog_dbg("group been unbind by %d \n",g_index);				  
	}

	npd_syslog_dbg("npd_drv_bind_acl_group pbmptmp[0] %d \n", pbmptmp[0]);
	npd_syslog_dbg("npd_drv_bind_acl_group pbmptmp[1] %d \n", pbmptmp[1]);

	cfg_tmp = nam_bcm_acl_drv_tcam_clear(aclId, groupdev);
	if(0 != cfg_tmp) 
	{
		npd_syslog_err("set cfg table fail!\n");
		op_ret = ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
	}
	if (0 == pbmptmp[0]) 
	{
		groupPtr->devnum |= (1<<0);
	}
	if (0 == pbmptmp[1]) 
	{
		groupPtr->devnum |= (1<<1);
	}

	if(access_type == ACCESS_PORT_TYPE) 
	{
		cfg_tmp = nam_acl_drv_cfg_table_set(devNum,portId,0,ACL_FALSE,ACCESS_PORT_TYPE,0,dir_info,stdrule);
		if(0 != cfg_tmp) 
		{
			npd_syslog_err("set cfg table fail!\n");
			op_ret = ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
		}
		else if(ACL_RETURN_CODE_SUCCESS==cfg_tmp) 
		{			
			op_ret = ACL_RETURN_CODE_SUCCESS;
		}
		if ((0 != pbmptmp[0]) || (0 != pbmptmp[1])) 
		{
			cfg_tmp = nam_bcm_acl_drv_tcam_set(aclId, groupPtr->devnum, pbmptmp);
		}
	}	
	else if (access_type == ACCESS_VID_TYPE) 
	{	 								
		cfg_tmp =nam_acl_drv_cfg_table_set(devNum,portId,aclId,ACL_TRUE,ACCESS_VID_TYPE,vlanId,dir_info,stdrule);
		if(0!=cfg_tmp) 
		{
			npd_syslog_err("set cfg table fail!\n");
			op_ret = ACL_RETURN_CODE_NPD_DBUS_ERR_GENERAL;
		}
		else if(ACL_RETURN_CODE_SUCCESS == cfg_tmp) 
		{			
			op_ret = ACL_RETURN_CODE_SUCCESS;
		}
	} /*ACCESS_VID_TYP*/

	return op_ret;	  
} 


/**********************************************************************************
 *  npd_drv_delete_group_acl
 *
 *	DESCRIPTION:
 * 		unbind acl group on ethport port 
 *
 *	INPUT:
 *		
 *		group_num   - unbind acl group <num>
 *		
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR   - unbind fail
 *      ACL_RETURN_CODE_SUCCESS - unbind success
 **********************************************************************************/

unsigned int npd_drv_delete_group_acl(unsigned int group_num,unsigned int dir_info)
{

	struct acl_rule_info_s			    *ruleInfo = NULL;
	struct acl_group_list_s  			*groupPtr=NULL;
	struct list_head					*Ptr=NULL;
	unsigned int 						delete_index = 0, op_ret=ACL_RETURN_CODE_ERROR;

	if(dir_info ==ACL_DIRECTION_INGRESS) 
	{
		groupPtr = acl_group[group_num];
	}
	else if(dir_info==ACL_DIRECTION_EGRESS) 
	{
		groupPtr = egress_acl_group[group_num];
	}
	else
	{
		/*code optimize:    Explicit null dereferenced
		zhangcl@autelan.com 2013-1-17*/
		return ACL_RETURN_CODE_ERROR;
	}

	__list_for_each(Ptr,&(groupPtr->list1))
	{
		ruleInfo=list_entry(Ptr,struct acl_rule_info_s,list);
		if(NULL!=ruleInfo)
		{			
			delete_index=ruleInfo->ruleIndex;
			op_ret = nam_acl_drv_tcam_clear(delete_index, 0);
			if(0!=op_ret)
				npd_syslog_err("TCAM %d fail\n",delete_index);	
			else
			{
				groupPtr->count-=1;
				npd_syslog_dbg("cout****%d\n",groupPtr->count);
			}
			/*if(0==(groupPtr->count))break;*/
		}

	}  /*list_for_each*/	
	
	if(0 == (groupPtr->count)) 
	{
		if(dir_info == ACL_DIRECTION_INGRESS) 
		{
			free(acl_group[group_num]);				
			acl_group[group_num] = NULL;
   		 	op_ret=ACL_RETURN_CODE_SUCCESS;
		}
		else if(dir_info == ACL_DIRECTION_EGRESS) 
		{
			free(egress_acl_group[group_num]);
			egress_acl_group[group_num] = NULL;
   		 	op_ret=ACL_RETURN_CODE_SUCCESS;
		}			
	}

	return op_ret;	
}
/**********************************************************************************
 *  npd_acl_group_search
 *
 *	DESCRIPTION:
 * 		Check out if acl group null
 * 	
 *
 *	INPUT:	
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR      - group null
 *		ACL_RETURN_CODE_SUCCESS	- group not null
 *		
 **********************************************************************************/

int npd_acl_group_search(unsigned int *group_count)
{
	unsigned int             i = 0,count=0;
	struct acl_group_list_s  *groupNodePtr=NULL;
		
	for(i=0; i < MAX_GROUP_NUM; i++) {
	   groupNodePtr = acl_group[i];
	   if(groupNodePtr==NULL) {
		   count++;
	   }
	} 
	*group_count = MAX_GROUP_NUM-count;
	
	if(count==MAX_GROUP_NUM) {
		return ACL_RETURN_CODE_ERROR;
	}
	else {		
		return ACL_RETURN_CODE_SUCCESS;
	}	
}
/**********************************************************************************
 *  npd_acl_group_find_addr_by_index
 *
 *	DESCRIPTION:
 * 		Check out if specified acl rule has been added in acl group
 * 	
 *
 *	INPUT:
 *		index -  group number
 *	
 *	OUTPUT:
 *		NULL
 *
 * 	RETURN:
 *		NULL  - if parameters given are wrong
 *		groupNode	- if acl rule has been added in group
 *		
 **********************************************************************************/


struct acl_group_list_s *npd_acl_group_find_addr_by_Index
(
	unsigned long index
)
{
	struct acl_group_list_s *groupNode = NULL;
	/*aclId should be in range of (0-1023)*/
	/*code optimize:  Unsigned compared against 0
	zhangcl@autelan.com 2013-1-17*/
	if(index > (MAX_GROUP_NUM-1)) 
	{
		groupNode = NULL;
	}
	else
	{
		/*code optimize: Out-of-bounds read
		zhangcl@autelan.com 2013-1-17*/
		groupNode = acl_group[index];
	}

	return groupNode;
}

/**********************************************************************************
 *  npd_show_acl_group
 *
 *	DESCRIPTION:
 * 		show acl group detail 
 *
 *	INPUT:		
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR   - show acl group fail
 *      ACL_RETURN_CODE_SUCCESS - show acl group success
 **********************************************************************************/

unsigned int npd_show_acl_group(unsigned int dir_info)
 {

	 struct acl_rule_info_s        *ruleInfo = NULL;
	 struct list_head			   *ptr = NULL;
	 struct acl_group_list_s	   *group = NULL;
	
	 unsigned int i,op_ret = ACL_RETURN_CODE_SUCCESS,count=0;

	 for(i=0;i<MAX_GROUP_NUM;i++) {
	 	if(dir_info==ACL_DIRECTION_INGRESS) {
   			group = acl_group[i];
	 	}
		else if (dir_info==ACL_DIRECTION_EGRESS) {
			group = egress_acl_group[i];
		}
	 	if(NULL!=group) {
			npd_syslog_dbg("=================================================\n");
			npd_syslog_dbg("%-40s: %d\n","ACL_GROUP",i);
			npd_syslog_dbg("%-40s: %d\n","BIND NUMBER",group->count);
			
			__list_for_each(ptr,&(group->list1)) {
				  ruleInfo=list_entry(ptr,struct acl_rule_info_s,list);	
				  if(NULL!=ruleInfo){
				     npd_syslog_dbg("%-40s: %ld\n","Access-list index",(ruleInfo->ruleIndex+1));
				   }
			} 
			npd_syslog_dbg("=================================================\n");
			op_ret=ACL_RETURN_CODE_SUCCESS;

		}
		else {
			count =count+1;
		}

	 }/*for*/

		/*no  acl group*/
	 if(count==MAX_GROUP_NUM) {
	   	  op_ret= ACL_RETURN_CODE_ERROR;
	 }
 
 	return op_ret;
 }
/**********************************************************************************
 *  npd_show_eth_rule
 *
 *	DESCRIPTION:
 * 		show acl rule binded on interface detail
 *
 *	INPUT:		
 *	
 *	OUTPUT:
 *		
 * 	RETURN:
 *		ACL_RETURN_CODE_ERROR   - interface has no any bind information
 *      ACL_RETURN_CODE_SUCCESS - show acl group success
 **********************************************************************************/


unsigned int npd_show_eth_rule(unsigned int eth_g_index)
{	
	struct acl_rule_info_s      *ruleInfo = NULL;
	struct list_head			   *ptr=NULL;
	struct eth_group_info       *ethInfo=NULL;
	struct acl_group_list_s 		*groupNodePtr=NULL;
	unsigned int                 slot_no=0,local_port_no=0,ret=ACL_RETURN_CODE_SUCCESS;
	unsigned int				group_num=0;

	npd_syslog_dbg("show binded rules based on %d\n",eth_g_index);

	slot_no		   = SLOT_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no   = ETH_LOCAL_INDEX_FROM_ETH_GLOBAL_INDEX(eth_g_index);
	local_port_no   = ETH_LOCAL_INDEX2NO(slot_no,local_port_no);

	if(NULL==(g_eth_ports[eth_g_index]->funcs.func_data[ETH_PORT_FUNC_ACL_RULE])) {
		npd_syslog_dbg("interface no group infomation!\n");
		ret=ACL_RETURN_CODE_ERROR;
	}
	else {
		ethInfo=g_eth_ports[eth_g_index]->funcs.func_data[ETH_PORT_FUNC_ACL_RULE];

		group_num = ethInfo->groupId;
		if(acl_group[group_num]!=NULL) {
			groupNodePtr=acl_group[group_num];
			npd_syslog_dbg("=================================================\n");
			npd_syslog_dbg("%-40s: %d/%d\n","interface port", slot_no,local_port_no);
			npd_syslog_dbg("%-40s: %d\n","bind number",groupNodePtr->count);
			__list_for_each(ptr,&(groupNodePtr->list1)) {
				ruleInfo=list_entry(ptr,struct acl_rule_info_s,list);
				if(NULL!=ruleInfo){
				 	npd_syslog_dbg("%-40s: %ld\n","Access-list index",(ruleInfo->ruleIndex+1));
				}
			}/*list*/
		}
		npd_syslog_dbg("=================================================\n");
		return ACL_RETURN_CODE_SUCCESS;
	}
	return ret;
}

unsigned int npd_qos_profile_in_acl_check(unsigned int profileIndex)
{
	unsigned int i=0,ret=0;

	for(i=0;i<MAX_ACL_RULE_NUMBER;i++){
		if(g_acl_rule[i]!=NULL) {
			if(profileIndex==(g_acl_rule[i]->ActionPtr.actionInfo.qos.qos.ingress.profileIndex)) {	
				ret=1;
				npd_syslog_dbg("****in acl!\n");
			}
		}
	}
	return ret;
}
	
unsigned int npd_acl_policer_id_check(unsigned int policerId)
{
	if(NULL==g_policer[policerId]) {
		return ACL_RETURN_CODE_ERROR;
	}
	else {
		return ACL_RETURN_CODE_SUCCESS;
	}
}

unsigned int npd_acl_policer_use_check(unsigned int policerId)
{
	unsigned int i=0,ret=ACL_RETURN_CODE_SUCCESS;
	AclRuleList *rule=NULL;

	for(i=0;i<MAX_ACL_RULE_NUMBER;i++){
		if(NULL!=g_acl_rule[i]){
			rule=g_acl_rule[i];
			if((1==(rule->ActionPtr.policerPtr.policerEnable))&& \
			  (policerId==rule->ActionPtr.policerPtr.policerId)){
				npd_syslog_err("policer in used\n");		
				ret= ACL_RETURN_CODE_ERROR;
			}
			else 
				ret= ACL_RETURN_CODE_SUCCESS;
		}
	}
	return ret;
}

#ifdef __cplusplus
}
#endif
